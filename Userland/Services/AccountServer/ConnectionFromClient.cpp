/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

// FIXME: Similar to FileSystemAccessServer. LibIPC Decoder and Encoder are sensitive to include order here.
// clang-format off
#include <LibGUI/ConnectionToWindowServer.h>
// clang-format on
#include "PasswordChangeDialog.h"
#include <AK/HashMap.h>
#include <AccountServer/AccountClientEndpoint.h>
#include <AccountServer/ConnectionFromClient.h>
#include <LibCore/System.h>
#include <LibGUI/Application.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/Window.h>
#include <LibGUI/WindowMode.h>

namespace AccountServer {

static HashMap<int, NonnullRefPtr<ConnectionFromClient>> s_connections;

ConnectionFromClient::ConnectionFromClient(NonnullOwnPtr<Core::Stream::LocalSocket> socket)
    : IPC::ConnectionFromClient<AccountClientEndpoint, AccountServerEndpoint>(*this, move(socket), 1)
{
    s_connections.set(1, *this);
}

void ConnectionFromClient::die()
{
    s_connections.remove(client_id());
    GUI::Application::the()->quit();
}

RefPtr<GUI::Window> ConnectionFromClient::create_dummy_child_window(i32 window_server_client_id, i32 parent_window_id)
{
    auto window = GUI::Window::construct();
    window->set_opacity(0);
    window->set_frameless(true);
    window->set_window_mode(GUI::WindowMode::Passive);
    auto rect = GUI::ConnectionToWindowServer::the().get_window_rect_from_client(window_server_client_id, parent_window_id);
    window->set_rect(rect);
    window->show();
    GUI::ConnectionToWindowServer::the().set_window_parent_from_client(window_server_client_id, parent_window_id, window->window_id());

    return window;
}

ErrorOr<ConnectionFromClient::ExpandedUserPair> ConnectionFromClient::expand_user_pair(i32 target_user_id, i32 active_user_id)
{
    return ExpandedUserPair { TRY(Core::Account::from_uid(target_user_id, Core::Account::Read::All)), TRY(Core::Account::from_uid(active_user_id)) };
}

ErrorOr<EscalatorWindow::AuthorizationResult> ConnectionFromClient::authorize(GUI::Window* parent_window, ExpandedUserPair const& users, StringView prompt)
{
    auto escalator_window = TRY(EscalatorWindow::try_create(EscalatorWindow::Options { {}, prompt, users.current_user, parent_window }));

    escalator_window->set_window_mode(GUI::WindowMode::Blocking);
    escalator_window->center_on_screen();

    auto result = escalator_window->request_authorization();
    escalator_window->close();
    return result;
}

void ConnectionFromClient::fail(FailMethod method, String error_message, i32 promise_id, GUI::Window* main_window)
{
    GUI::MessageBox::show(main_window, error_message, "AccountServer"sv, GUI::MessageBox::Type::Error);

    switch (method) {
        case FailMethod::FailPasswordUpdate:
            return async_user_password_updated(promise_id, false);
        case FailMethod::FailDelete:
            return async_user_deleted(promise_id, false);
        default:
            VERIFY_NOT_REACHED();
    }
}

void ConnectionFromClient::request_user_password_update(i32 promise_id, i32 target_user_id, i32 active_user_id, i32 window_server_client_id, i32 window_id)
{
    auto main_window = create_dummy_child_window(window_server_client_id, window_id);

    auto users_result = expand_user_pair(target_user_id, active_user_id);
    if (users_result.is_error())
        return fail(FailMethod::FailPasswordUpdate, String::formatted("Failed to expand user pair: {}", users_result.error()), promise_id, main_window);

    ExpandedUserPair users = users_result.value();

    auto authorization_result = authorize(main_window, users, String::formatted("To update password for user \"{}\", please authenticate by entering password for user \"{}\".", users.target_user.username(), users.current_user.username()));
    if (authorization_result.is_error())
        return fail(FailMethod::FailPasswordUpdate, String::formatted("Failed to authorize via Escalator: {}", authorization_result.error()), promise_id, main_window);

    if (authorization_result.value() == EscalatorWindow::AuthorizationResult::Fail)
        return async_user_password_updated(promise_id, false);

    auto dialog_result = PasswordChangeDialog::try_create(users.target_user, main_window);
    if (dialog_result.is_error())
        return fail(FailMethod::FailPasswordUpdate, String::formatted("Failed to create Escalator window: {}", dialog_result.error()), promise_id, main_window);

    dialog_result.value()->set_window_mode(GUI::WindowMode::Blocking);
    dialog_result.value()->center_on_screen();

    auto result = dialog_result.value()->prompt();
    dbgln("result of PasswordChangeDialog is {}", to_underlying(result));
    async_user_password_updated(promise_id, true);
}

void ConnectionFromClient::request_user_deletion(i32 promise_id, i32 target_user_id, i32 active_user_id, i32 window_server_client_id, i32 window_id)
{
    auto main_window = create_dummy_child_window(window_server_client_id, window_id);

    auto users_result = expand_user_pair(target_user_id, active_user_id);
    if (users_result.is_error())
        return fail(FailMethod::FailDelete, String::formatted("Failed to expand user pair: {}", users_result.error()), promise_id, main_window);

    ExpandedUserPair users = users_result.value();

    auto authorization_result = authorize(main_window, users, String::formatted("To delete user \"{}\", please authenticate by entering password for user \"{}\".", users.target_user.username(), users.current_user.username()));
    if (authorization_result.is_error())
        return fail(FailMethod::FailDelete, String::formatted("Failed to authorize via Escalator: {}", authorization_result.error()), promise_id, main_window);

    if (authorization_result.value() == EscalatorWindow::AuthorizationResult::Fail)
        return async_user_deleted(promise_id, false);

    // User is authorised to delete a user - spawn "userdel" process.
    char const* userdel = "/bin/userdel";
    char const* argv[] = { userdel, users.target_user.username().characters(), nullptr };
    auto spawn_result = Core::System::posix_spawn(String(userdel), nullptr, nullptr, const_cast<char**>(argv), environ);
    if (spawn_result.is_error())
        return fail(FailMethod::FailDelete, String::formatted("Failed to delete user: {}", spawn_result.error()), promise_id, main_window);

    auto wait_result = Core::System::waitpid(spawn_result.value());
    if (wait_result.is_error())
        return fail(FailMethod::FailDelete, String::formatted("Failed to delete user: {}", wait_result.error()), promise_id, main_window);

    if (wait_result.value().status)
        return fail(FailMethod::FailDelete, String::formatted("userdel returned non-0 status: {}\nThe user may or may not be deleted.", wait_result.value().status), promise_id, main_window);

    GUI::MessageBox::show(main_window, "User successfully deleted."sv, "Success"sv, GUI::MessageBox::Type::Information);

    async_user_deleted(promise_id, true);
}

Messages::AccountServer::ExposeWindowServerClientIdResponse ConnectionFromClient::expose_window_server_client_id()
{
    return GUI::ConnectionToWindowServer::the().expose_client_id();
}

}
