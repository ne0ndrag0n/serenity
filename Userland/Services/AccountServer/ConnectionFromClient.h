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
#include <AK/StringView.h>
#include <AccountServer/AccountClientEndpoint.h>
#include <AccountServer/AccountServerEndpoint.h>
#include <LibCore/Account.h>
#include <LibCore/Stream.h>
#include <LibEscalator/EscalatorWindow.h>
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
    struct ExpandedUserPair {
        Core::Account target_user;
        Core::Account current_user;
    };

    enum class FailMethod {
        FailPasswordUpdate,
        FailDelete
    };

    explicit ConnectionFromClient(NonnullOwnPtr<Core::Stream::LocalSocket>);

    void fail(FailMethod method, String error_message, i32 promise_id, GUI::Window* main_window);

    ErrorOr<ExpandedUserPair> expand_user_pair(i32 target_user_id, i32 active_user_id);
    ErrorOr<EscalatorWindow::AuthorizationResult> authorize(GUI::Window* parent_window, ExpandedUserPair const& users, StringView prompt);

    RefPtr<GUI::Window> create_dummy_child_window(i32, i32);

    virtual void request_user_password_update(i32 promise_id, i32 target_user_id, i32 active_user_id, i32 window_server_client_id, i32 window_id) override;
    virtual void request_user_deletion(i32 promise_id, i32 target_user_id, i32 active_user_id, i32 window_server_client_id, i32 window_id) override;

    virtual Messages::AccountServer::ExposeWindowServerClientIdResponse expose_window_server_client_id() override;
};

}
