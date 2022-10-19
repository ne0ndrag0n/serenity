/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "AddUserWizardDialog.h"
#include "PasswordChangeDialog.h"
#include <AK/HashMap.h>
#include <AccountServer/AccountClientEndpoint.h>
#include <AccountServer/ConnectionFromClient.h>
#include <LibAuth/Auth.h>
#include <LibCore/Group.h>
#include <LibCore/System.h>
#include <LibGUI/Application.h>
#include <LibGUI/ConnectionToWindowServer.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/Window.h>
#include <LibGUI/WindowMode.h>
#include <unistd.h>

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

ErrorOr<ConnectionFromClient::ExpandedUserPair> ConnectionFromClient::expand_user_pair(i32 target_user_id)
{
    return ExpandedUserPair { TRY(Core::Account::from_uid(target_user_id, Core::Account::Read::All)), TRY(Core::Account::from_uid(getuid())) };
}

ErrorOr<void> ConnectionFromClient::user_password_update(i32 promise_id, i32 target_user_id, i32 window_server_client_id, i32 window_id)
{
    auto main_window = create_dummy_child_window(window_server_client_id, window_id);
    auto users = TRY(expand_user_pair(target_user_id));

    String prompt = String::formatted("To update password for user \"{}\", please authenticate by entering password for user \"{}\".", users.target_user.username(), users.current_user.username());

    if (!TRY(Auth::authorize_escalation({}, prompt, main_window))) {
        async_user_password_updated(promise_id, false);
        return {};
    }

    auto dialog = TRY(PasswordChangeDialog::try_create(users.target_user, main_window));
    dialog->set_window_mode(GUI::WindowMode::Blocking);
    dialog->center_on_screen();

    auto result = dialog->prompt();
    dbgln("result of PasswordChangeDialog is {}", to_underlying(result));
    async_user_password_updated(promise_id, true);
    return {};
}

void ConnectionFromClient::request_user_password_update(i32 promise_id, i32 target_user_id, i32 window_server_client_id, i32 window_id)
{
    if (user_password_update(promise_id, target_user_id, window_server_client_id, window_id).is_error())
        async_user_password_updated(promise_id, false);
}

ErrorOr<void> ConnectionFromClient::user_settings_update(i32 promise_id, i32 target_user_id, i32 window_server_client_id,
    i32 window_id, String const& preferred_shell, String const& gecos,
    u32 primary_gid, Vector<u32> const& secondary_gids)
{
    auto main_window = create_dummy_child_window(window_server_client_id, window_id);
    auto users = TRY(expand_user_pair(target_user_id));

    String prompt = String::formatted("To update user \"{}\", please authenticate by entering password for user \"{}\".", users.target_user.username(), users.current_user.username());
    if (!TRY(Auth::authorize_escalation({}, prompt, main_window))) {
        async_user_settings_updated(promise_id, false);
        return {};
    }

    TRY(update_user_settings(users.target_user, preferred_shell, gecos, primary_gid, secondary_gids));
    async_user_settings_updated(promise_id, true);
    return {};
}

void ConnectionFromClient::request_user_settings_update(i32 promise_id, i32 target_user_id, i32 window_server_client_id,
    i32 window_id, String const& preferred_shell, String const& gecos,
    u32 primary_gid, Vector<u32> const& secondary_gids)
{
    if (user_settings_update(promise_id, target_user_id, window_server_client_id, window_id, preferred_shell, gecos, primary_gid, secondary_gids).is_error())
        async_user_settings_updated(promise_id, false);
}

ErrorOr<void> ConnectionFromClient::user_deletion(i32 promise_id, i32 target_user_id, i32 window_server_client_id, i32 window_id)
{
    auto main_window = create_dummy_child_window(window_server_client_id, window_id);
    auto users = TRY(expand_user_pair(target_user_id));

    String prompt = String::formatted("To delete user \"{}\", please authenticate by entering password for user \"{}\".", users.target_user.username(), users.current_user.username());
    if (!TRY(Auth::authorize_escalation({}, prompt, main_window))) {
        async_user_deleted(promise_id, false);
        return {};
    }

    // User is authorised to delete a user - spawn "userdel" process.
    char const* userdel = "/bin/userdel";
    char const* argv[] = { userdel, users.target_user.username().characters(), nullptr };
    auto spawn = TRY(Core::System::posix_spawn(String(userdel), nullptr, nullptr, const_cast<char**>(argv), environ));
    auto wait = TRY(Core::System::waitpid(spawn));
    if (wait.status) {
        async_user_deleted(promise_id, false);
        return {};
    }

    GUI::MessageBox::show(main_window, "User successfully deleted."sv, "Success"sv, GUI::MessageBox::Type::Information);
    async_user_deleted(promise_id, true);
    return {};
}

void ConnectionFromClient::request_user_deletion(i32 promise_id, i32 target_user_id, i32 window_server_client_id, i32 window_id)
{
    if (user_deletion(promise_id, target_user_id, window_server_client_id, window_id).is_error())
        async_user_deleted(promise_id, false);
}

ErrorOr<void> ConnectionFromClient::user_addition(i32 promise_id, i32 window_server_client_id, i32 window_id)
{
    auto main_window = create_dummy_child_window(window_server_client_id, window_id);
    auto current_user = TRY(Core::Account::from_uid(getuid()));

    String prompt = String::formatted("To add a new user, please authenticate by entering password for user \"{}\".", current_user.username());
    if (!TRY(Auth::authorize_escalation({}, prompt, main_window))) {
        async_user_added(promise_id, -1);
        return {};
    }

    auto wizard = TRY(AddUserWizardDialog::try_create(main_window));
    auto result = wizard->exec();
    if (result == GUI::Dialog::ExecResult::OK) {
        auto settings = wizard->get_settings();

        // Spawn useradd with just the username.
        char const* useradd = "/bin/useradd";
        char const* argv[] = { useradd, settings.username.characters(), nullptr };
        auto spawn = TRY(Core::System::posix_spawn(String(useradd), nullptr, nullptr, const_cast<char**>(argv), environ));
        auto wait = TRY(Core::System::waitpid(spawn));
        if (wait.status) {
            async_user_added(promise_id, -1);
            return {};
        }

        // Now we can query Account for the username and set the remainder of the items.
        auto newuser = TRY(Core::Account::from_name(settings.username));

        if (settings.password_not_required)
            newuser.delete_password();
        else if (settings.account_disabled)
            newuser.set_password_enabled(false);
        else
            newuser.set_password(settings.password);

        newuser.set_gecos(settings.gecos);
        newuser.set_shell("/bin/Shell"sv);

        TRY(newuser.sync());

        // Useradd was succesful.
        async_user_added(promise_id, newuser.uid());
        return {};
    }

    async_user_added(promise_id, -1);
    return {};
}

void ConnectionFromClient::request_user_addition(i32 promise_id, i32 window_server_client_id, i32 window_id)
{
    if (user_addition(promise_id, window_server_client_id, window_id).is_error())
        async_user_added(promise_id, -1);
}

ErrorOr<void> ConnectionFromClient::update_user_settings(Core::Account target_user, String shell, String gecos, gid_t primary_gid, Vector<gid_t> secondary_gids)
{
    target_user.set_shell(shell);
    target_user.set_gecos(gecos);
    target_user.set_gid(primary_gid);

    TRY(target_user.sync());

    secondary_gids.remove_all_matching([primary_gid](gid_t needle) { return needle == primary_gid; });

    // Use this as an impromptu set.
    HashMap<gid_t, bool> group_memberships;
    for (gid_t gid : secondary_gids)
        group_memberships.ensure(gid);

    Vector<Core::Group> all_groups = TRY(Core::Group::all());
    for (auto& group : all_groups) {
        Vector<String>& members = group.members();
        members.remove_all_matching([target_user](String const& needle) { return needle == target_user.username(); });

        if (group_memberships.contains(group.id()))
            members.append(target_user.username());

        TRY(group.sync());
    }

    return {};
}

Messages::AccountServer::ExposeWindowServerClientIdResponse ConnectionFromClient::expose_window_server_client_id()
{
    return GUI::ConnectionToWindowServer::the().expose_client_id();
}

}
