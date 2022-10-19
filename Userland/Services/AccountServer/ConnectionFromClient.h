/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/NonnullOwnPtr.h>
#include <AK/RefPtr.h>
#include <AccountServer/AccountClientEndpoint.h>
#include <AccountServer/AccountServerEndpoint.h>
#include <LibCore/Stream.h>
#include <LibGUI/Window.h>
#include <LibIPC/ConnectionFromClient.h>

namespace AccountServer {

class ConnectionFromClient final
    : public IPC::ConnectionFromClient<AccountClientEndpoint, AccountServerEndpoint> {
    C_OBJECT(ConnectionFromClient);

public:
    ~ConnectionFromClient() override = default;
    virtual void die() override;

private:
    explicit ConnectionFromClient(NonnullOwnPtr<Core::Stream::LocalSocket>);

    RefPtr<GUI::Window> create_dummy_child_window(i32, i32);

    virtual void request_user_password_update(i32 promise_id, i32 target_user_id, i32 active_user_id, i32 window_server_client_id, i32 window_id) override;

    virtual Messages::AccountServer::ExposeWindowServerClientIdResponse expose_window_server_client_id() override;
};

}
