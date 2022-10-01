/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <AK/StringView.h>
#include <AK/Vector.h>
#include <LibCore/Account.h>
#include <LibCore/Object.h>
#include <LibGUI/Button.h>
#include <LibGUI/Icon.h>
#include <LibGUI/ImageWidget.h>
#include <LibGUI/TextBox.h>
#include <LibGUI/Window.h>

class EscalatorWindow final : public GUI::Window {
    C_OBJECT(EscalatorWindow)
public:
    virtual ~EscalatorWindow() override = default;

    ErrorOr<int> execute_command();

private:
    EscalatorWindow(StringView description, Vector<StringView> command, Core::Account current_user);

    GUI::Icon find_icon();
    ErrorOr<void> check_password();

    Vector<StringView> m_command;
    Core::Account m_current_user;

    RefPtr<GUI::ImageWidget> m_icon_image_widget;
    RefPtr<GUI::Button> m_ok_button;
    RefPtr<GUI::Button> m_cancel_button;
    RefPtr<GUI::PasswordBox> m_password_input;
};
