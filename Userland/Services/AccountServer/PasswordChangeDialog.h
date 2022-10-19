/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/OwnPtr.h>
#include <LibCore/Account.h>
#include <LibCore/EventLoop.h>
#include <LibCore/Object.h>
#include <LibGUI/CheckBox.h>
#include <LibGUI/TextBox.h>
#include <LibGUI/Window.h>

namespace AccountServer {

class PasswordChangeDialog final : public GUI::Window {
    C_OBJECT(PasswordChangeDialog)

public:
    enum class PasswordChangeResult {
        Success,
        Fail
    };

    virtual ~PasswordChangeDialog() override = default;

    virtual void close() override;

    PasswordChangeResult prompt();

private:
    PasswordChangeDialog(Core::Account const& account, GUI::Window* parent_window = nullptr);

    void load_state_from_target();

    void save_and_close();
    void on_no_password_required_toggle();
    void on_login_disabled_toggle();

    OwnPtr<Core::EventLoop> m_event_loop;

    RefPtr<GUI::CheckBox> m_no_password_required;
    RefPtr<GUI::CheckBox> m_login_disabled;

    RefPtr<GUI::PasswordBox> m_new_password;
    RefPtr<GUI::PasswordBox> m_confirm_new_password;

    Core::Account m_target_account;
};

}