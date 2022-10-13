/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

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

    Result try_request_password_change(uid_t user_id, GUI::Window* parent_window);

    Function<void()> on_response;

private:
    ConnectionToServer(NonnullOwnPtr<Core::Stream::LocalSocket> socket)
        : IPC::ConnectionToServer<AccountClientEndpoint, AccountServerEndpoint>(*this, move(socket))
    {
    }

    int get_new_id();
    Result handle_promise(int);

    virtual void user_password_updated(i32 promise_id, bool success) override;

    struct PromiseAndWindow {
        RefPtr<Core::Promise<Result>> promise {};
        GUI::Window* parent_window { nullptr };
    };

    HashMap<int, PromiseAndWindow> m_promises {};
    int m_last_id { 0 };
};

}
