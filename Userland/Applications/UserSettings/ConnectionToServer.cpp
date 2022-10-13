/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ConnectionToServer.h"
#include <AK/ScopeGuard.h>
#include <LibGUI/ConnectionToWindowServer.h>

namespace UserSettings {

Result ConnectionToServer::try_request_settings_change(uid_t user_id, Settings settings, GUI::Window* parent_window)
{
    auto const id = get_new_id();
    m_promises.set(id, PromiseAndWindow { Core::Promise<Result>::construct(), parent_window });

    auto parent_window_server_client_id = GUI::ConnectionToWindowServer::the().expose_client_id();
    auto child_window_server_client_id = expose_window_server_client_id();
    auto parent_window_id = parent_window->window_id();

    GUI::ConnectionToWindowServer::the().add_window_stealing_for_client(child_window_server_client_id, parent_window_id);
    ScopeGuard guard([parent_window_id, child_window_server_client_id] {
        GUI::ConnectionToWindowServer::the().remove_window_stealing_for_client(child_window_server_client_id, parent_window_id);
    });

    async_request_user_settings_update(
        id,
        static_cast<i32>(user_id),
        parent_window_server_client_id,
        parent_window_id,
        settings.shell,
        settings.gecos,
        settings.primary_group,
        settings.groups);

    auto result = handle_promise(id);
    return result;
}

Result ConnectionToServer::try_request_password_change(uid_t user_id, GUI::Window* parent_window)
{
    auto const id = get_new_id();
    m_promises.set(id, PromiseAndWindow { Core::Promise<Result>::construct(), parent_window });

    auto parent_window_server_client_id = GUI::ConnectionToWindowServer::the().expose_client_id();
    auto child_window_server_client_id = expose_window_server_client_id();
    auto parent_window_id = parent_window->window_id();

    GUI::ConnectionToWindowServer::the().add_window_stealing_for_client(child_window_server_client_id, parent_window_id);
    ScopeGuard guard([parent_window_id, child_window_server_client_id] {
        GUI::ConnectionToWindowServer::the().remove_window_stealing_for_client(child_window_server_client_id, parent_window_id);
    });

    async_request_user_password_update(id, static_cast<i32>(user_id), parent_window_server_client_id, parent_window_id);

    auto result = handle_promise(id);
    return result;
}

Result ConnectionToServer::try_request_user_deletion(uid_t user_id, GUI::Window* parent_window)
{
    auto const id = get_new_id();
    m_promises.set(id, PromiseAndWindow { Core::Promise<Result>::construct(), parent_window });

    auto parent_window_server_client_id = GUI::ConnectionToWindowServer::the().expose_client_id();
    auto child_window_server_client_id = expose_window_server_client_id();
    auto parent_window_id = parent_window->window_id();

    GUI::ConnectionToWindowServer::the().add_window_stealing_for_client(child_window_server_client_id, parent_window_id);
    ScopeGuard guard([parent_window_id, child_window_server_client_id] {
        GUI::ConnectionToWindowServer::the().remove_window_stealing_for_client(child_window_server_client_id, parent_window_id);
    });

    async_request_user_deletion(id, static_cast<i32>(user_id), parent_window_server_client_id, parent_window_id);

    auto result = handle_promise(id);
    return result;
}

Result ConnectionToServer::try_request_user_addition(GUI::Window* parent_window)
{
    auto const id = get_new_id();
    m_promises.set(id, PromiseAndWindow { Core::Promise<Result>::construct(), parent_window });

    auto parent_window_server_client_id = GUI::ConnectionToWindowServer::the().expose_client_id();
    auto child_window_server_client_id = expose_window_server_client_id();
    auto parent_window_id = parent_window->window_id();

    GUI::ConnectionToWindowServer::the().add_window_stealing_for_client(child_window_server_client_id, parent_window_id);
    ScopeGuard guard([parent_window_id, child_window_server_client_id] {
        GUI::ConnectionToWindowServer::the().remove_window_stealing_for_client(child_window_server_client_id, parent_window_id);
    });

    async_request_user_addition(id, parent_window_server_client_id, parent_window_id);

    auto result = handle_promise(id);
    return result;
}

int ConnectionToServer::get_new_id()
{
    auto const new_id = m_last_id++;
    // Note: This verify shouldn't fail, and we should provide a valid ID
    // But we probably have more issues if this test fails.
    VERIFY(!m_promises.contains(new_id));
    return new_id;
}

Result ConnectionToServer::handle_promise(int id)
{
    auto result = m_promises.get(id)->promise->await();
    m_promises.remove(id);

    if (on_response)
        on_response();

    return result;
}

void ConnectionToServer::user_password_updated(i32 promise_id, bool success)
{
    dbgln("user_password_updated: {}", promise_id);
    auto potential_data = m_promises.get(promise_id);
    VERIFY(potential_data.has_value());
    auto& request_data = potential_data.value();

    if (!success)
        request_data.promise->resolve(Error::from_string_literal("AccountServer operation failed"));
    else {
        if (on_user_password_updated)
            on_user_password_updated();

        request_data.promise->resolve({});
    }
}

void ConnectionToServer::user_settings_updated(i32 promise_id, bool success)
{
    dbgln("user_settings_updated: {}", promise_id);
    auto potential_data = m_promises.get(promise_id);
    VERIFY(potential_data.has_value());
    auto& request_data = potential_data.value();

    if (!success)
        request_data.promise->resolve(Error::from_string_literal("AccountServer operation failed"));
    else {
        if (on_user_settings_updated)
            on_user_settings_updated();

        request_data.promise->resolve({});
    }
}

void ConnectionToServer::user_deleted(i32 promise_id, bool success)
{
    dbgln("user_deleted: {}", promise_id);
    auto potential_data = m_promises.get(promise_id);
    VERIFY(potential_data.has_value());
    auto& request_data = potential_data.value();

    if (!success)
        request_data.promise->resolve(Error::from_string_literal("AccountServer operation failed"));
    else {
        if (on_user_deleted)
            on_user_deleted();

        request_data.promise->resolve({});
    }
}

void ConnectionToServer::user_added(i32 promise_id, i32 new_user_id)
{
    dbgln("user_added: {}", promise_id);
    auto potential_data = m_promises.get(promise_id);
    VERIFY(potential_data.has_value());
    auto& request_data = potential_data.value();

    if (new_user_id == -1)
        request_data.promise->resolve(Error::from_string_literal("AccountServer operation failed"));
    else {
        if (on_user_added)
            on_user_added(new_user_id);

        request_data.promise->resolve({});
    }
}

}
