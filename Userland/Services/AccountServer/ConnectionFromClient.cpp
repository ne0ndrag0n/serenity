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
#include <LibCore/Account.h>
#include <LibEscalator/EscalatorWindow.h>
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

void ConnectionFromClient::request_user_password_update(i32 promise_id, i32 target_user_id, i32 active_user_id, i32 window_server_client_id, i32 window_id)
{
    auto main_window = create_dummy_child_window(window_server_client_id, window_id);
    auto current_user = Core::Account::from_uid(active_user_id);
    if (current_user.is_error()) {
        GUI::MessageBox::show(main_window, String::formatted("Failed to acquire current user: {}", current_user.error()), "AccountServer"sv, GUI::MessageBox::Type::Error);
        async_user_password_updated(promise_id, false);
        return;
    }

    auto target_user = Core::Account::from_uid(target_user_id, Core::Account::Read::All);
    if (target_user.is_error()) {
        GUI::MessageBox::show(main_window, String::formatted("Failed to acquire target user: {}", target_user.error()), "AccountServer"sv, GUI::MessageBox::Type::Error);
        async_user_password_updated(promise_id, false);
        return;
    }

    auto prompt = String::formatted("To update password for user \"{}\", please authenticate by entering password for user \"{}\".", target_user.value().username(), current_user.value().username());
    auto window_result = EscalatorWindow::try_create(EscalatorWindow::Options { {}, prompt, current_user.value(), main_window });
    if (window_result.is_error()) {
        GUI::MessageBox::show(main_window, String::formatted("Failed to create Escalator window: {}", window_result.error()), "AccountServer"sv, GUI::MessageBox::Type::Error);
        async_user_password_updated(promise_id, false);
        return;
    }

    auto window = window_result.value();

    window->set_window_mode(GUI::WindowMode::Blocking);
    window->center_on_screen();
    auto result = window->request_authorization();
    if (result == EscalatorWindow::AuthorizationResult::Success) {
        window->close();

        auto dialog_result = PasswordChangeDialog::try_create(target_user.value(), main_window);
        if (dialog_result.is_error()) {
            GUI::MessageBox::show(main_window, String::formatted("Failed to create Escalator window: {}", window_result.error()), "AccountServer"sv, GUI::MessageBox::Type::Error);
            async_user_password_updated(promise_id, false);
            return;
        }

        dialog_result.value()->center_on_screen();
        dialog_result.value()->set_window_mode(GUI::WindowMode::Blocking);
        auto result = dialog_result.value()->prompt();
        dbgln("result of PasswordChangeDialog is {}", to_underlying(result));
    } else
        dbgln("failed auth for {}", target_user_id);

    async_user_password_updated(promise_id, true);
}

Messages::AccountServer::ExposeWindowServerClientIdResponse ConnectionFromClient::expose_window_server_client_id()
{
    return GUI::ConnectionToWindowServer::the().expose_client_id();
}

}
