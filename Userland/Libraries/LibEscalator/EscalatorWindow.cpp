/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "EscalatorWindow.h"
#include <AK/Assertions.h>
#include <LibCore/SecretString.h>
#include <LibEscalator/EscalatorGML.h>
#include <LibEscalator/EscalatorWindow.h>
#include <LibGUI/Label.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/Widget.h>

EscalatorWindow::EscalatorWindow(EscalatorWindow::Options const& options)
    : GUI::Window(options.parent_window)
    , m_current_user(options.current_user)
{
    auto app_icon = options.icon.has_value() ? options.icon.value() : GUI::Icon::default_icon("app-escalator"sv);

    set_title("Run as Root");
    set_icon(app_icon.bitmap_for_size(16));
    resize(345, 100);
    set_resizable(false);
    set_minimizable(false);

    auto& main_widget = set_main_widget<GUI::Widget>();
    main_widget.load_from_gml(escalator_gml);

    RefPtr<GUI::Label> app_label = *main_widget.find_descendant_of_type_named<GUI::Label>("description");

    app_label->set_text(options.description);

    m_icon_image_widget = *main_widget.find_descendant_of_type_named<GUI::ImageWidget>("icon");
    m_icon_image_widget->set_bitmap(app_icon.bitmap_for_size(32));

    m_ok_button = *main_widget.find_descendant_of_type_named<GUI::DialogButton>("ok_button");
    m_ok_button->on_click = [this](auto) {
        auto result = check_password();
        if (result.is_error()) {
            GUI::MessageBox::show_error(this, String::formatted("Failed to execute command: {}", result.error()));
            close();
        }
    };
    m_ok_button->set_default(true);

    m_cancel_button = *main_widget.find_descendant_of_type_named<GUI::DialogButton>("cancel_button");
    m_cancel_button->on_click = [this](auto) {
        if (m_event_loop)
            m_event_loop->quit(to_underlying(EscalatorWindow::AuthorizationResult::Fail));
        else
            close();
    };

    m_password_input = *main_widget.find_descendant_of_type_named<GUI::PasswordBox>("password");
}

void EscalatorWindow::close()
{
    GUI::Window::close();
    if (m_event_loop)
        m_event_loop->quit(to_underlying(EscalatorWindow::AuthorizationResult::Fail));
}

EscalatorWindow::AuthorizationResult EscalatorWindow::request_authorization()
{
    VERIFY(!m_event_loop);
    m_event_loop = make<Core::EventLoop>();

    show();
    auto result = m_event_loop->exec();
    remove_from_parent();
    return static_cast<EscalatorWindow::AuthorizationResult>(result);
}

ErrorOr<void> EscalatorWindow::check_password()
{
    String password = m_password_input->text();
    if (password.is_empty()) {
        GUI::MessageBox::show_error(this, "Please enter a password."sv);
        return {};
    }

    // FIXME: PasswordBox really should store its input directly as a SecretString.
    Core::SecretString password_secret = Core::SecretString::take_ownership(password.to_byte_buffer());
    if (!m_current_user.authenticate(password_secret)) {
        GUI::MessageBox::show_error(this, "Incorrect or disabled password."sv);
        m_password_input->select_all();
        return {};
    }

    if (m_event_loop) {
        m_event_loop->quit(to_underlying(EscalatorWindow::AuthorizationResult::Success));
    }

    return {};
}