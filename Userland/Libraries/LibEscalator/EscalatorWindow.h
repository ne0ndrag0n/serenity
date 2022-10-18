/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Optional.h>
#include <AK/OwnPtr.h>
#include <AK/String.h>
#include <LibCore/Account.h>
#include <LibCore/EventLoop.h>
#include <LibCore/Object.h>
#include <LibGUI/Button.h>
#include <LibGUI/Icon.h>
#include <LibGUI/ImageWidget.h>
#include <LibGUI/TextBox.h>
#include <LibGUI/Window.h>

class EscalatorWindow final : public GUI::Window {
    C_OBJECT(EscalatorWindow)
public:
    enum class AuthorizationResult {
        Success,
        Fail
    };

    struct Options {
        Optional<GUI::Icon> icon;
        String description;
        Core::Account current_user;
        GUI::Window* parent_window { nullptr };
    };

    virtual ~EscalatorWindow() override = default;

    virtual void close() override;

    AuthorizationResult request_authorization();

private:
    EscalatorWindow(Options const& options);

    ErrorOr<void> check_password();

    Core::Account m_current_user;

    RefPtr<GUI::ImageWidget> m_icon_image_widget;
    RefPtr<GUI::Button> m_ok_button;
    RefPtr<GUI::Button> m_cancel_button;
    RefPtr<GUI::PasswordBox> m_password_input;

    // For request_authorization mode.
    OwnPtr<Core::EventLoop> m_event_loop;
};
