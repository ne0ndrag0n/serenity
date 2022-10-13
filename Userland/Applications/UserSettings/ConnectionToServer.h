/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Settings.h"
#include <AK/Error.h>
#include <AK/Function.h>
#include <AK/HashMap.h>
#include <AK/RefPtr.h>
#include <AccountServer/AccountClientEndpoint.h>
#include <AccountServer/AccountServerEndpoint.h>
#include <LibCore/Promise.h>
#include <LibCore/Stream.h>
#include <LibGUI/Window.h>
#include <LibIPC/ConnectionToServer.h>
#include <unistd.h>

namespace UserSettings {

using Result = ErrorOr<void>;

class ConnectionToServer
    : public IPC::ConnectionToServer<AccountClientEndpoint, AccountServerEndpoint>
    , public AccountClientEndpoint {
    IPC_CLIENT_CONNECTION(ConnectionToServer, "/tmp/portal/accounts"sv)
    virtual ~ConnectionToServer() = default;

    Result try_request_settings_change(uid_t user_id, Settings settings, GUI::Window* parent_window);
    Result try_request_password_change(uid_t user_id, GUI::Window* parent_window);
    Result try_request_user_deletion(uid_t user_id, GUI::Window* parent_window);
    Result try_request_user_addition(GUI::Window* parent_window);

    Function<void()> on_response;
    Function<void()> on_user_deleted;
    Function<void()> on_user_password_updated;
    Function<void()> on_user_settings_updated;
    Function<void(i32)> on_user_added;

private:
    ConnectionToServer(NonnullOwnPtr<Core::Stream::LocalSocket> socket)
        : IPC::ConnectionToServer<AccountClientEndpoint, AccountServerEndpoint>(*this, move(socket))
    {
    }

    int get_new_id();
    Result handle_promise(int);

    virtual void user_password_updated(i32 promise_id, bool success) override;
    virtual void user_settings_updated(i32 promise_id, bool success) override;
    virtual void user_deleted(i32 promise_id, bool success) override;
    virtual void user_added(i32 promise_id, i32 new_user_id) override;

    struct PromiseAndWindow {
        RefPtr<Core::Promise<Result>> promise {};
        GUI::Window* parent_window { nullptr };
    };

    HashMap<int, PromiseAndWindow> m_promises {};
    int m_last_id { 0 };
};

}
