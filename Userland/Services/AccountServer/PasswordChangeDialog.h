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
#include <LibGUI/Window.h>

class PasswordChangeDialog final : public GUI::Window {
    C_OBJECT(PasswordChangeDialog)

public:
    virtual ~PasswordChangeDialog() override = default;

    virtual void close() override;

    void prompt();

private:
    PasswordChangeDialog(Core::Account const& account, GUI::Window* parent_window = nullptr);

    OwnPtr<Core::EventLoop> m_event_loop;

    Core::Account m_target_account;
};
