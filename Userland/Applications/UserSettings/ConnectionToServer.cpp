/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

// FIXME: LibIPC Decoder and Encoder are sensitive to include order here
// clang-format off
#include <LibGUI/ConnectionToWindowServer.h>
// clang-format on
#include "ConnectionToServer.h"
#include <AK/ScopeGuard.h>

namespace UserSettings {

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

    async_request_user_password_update(id, static_cast<i32>(user_id), static_cast<i32>(getuid()), parent_window_server_client_id, parent_window_id);

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
    return result;
}

void ConnectionToServer::user_password_updated(i32 promise_id, bool success)
{
    dbgln("you made it here: {}", promise_id);
    auto potential_data = m_promises.get(promise_id);
    VERIFY(potential_data.has_value());
    auto& request_data = potential_data.value();

    if (!success)
        request_data.promise->resolve(Error::from_string_literal("AccountServer operation failed"));
    else
        request_data.promise->resolve({});
}

}