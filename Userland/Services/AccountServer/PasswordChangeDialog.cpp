/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "PasswordChangeDialog.h"
#include <AK/String.h>
#include <AccountServer/PasswordChangeDialogGML.h>
#include <LibCore/SecretString.h>
#include <LibGUI/Button.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Label.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/Widget.h>

PasswordChangeDialog::PasswordChangeDialog(Core::Account const& account, GUI::Window* parent_window)
    : GUI::Window(parent_window)
    , m_target_account(account)
{
    auto app_icon = GUI::Icon::default_icon("key"sv);

    set_icon(app_icon.bitmap_for_size(16));
    set_title("Update Password"sv);
    resize(345, 130);
    set_resizable(false);
    set_minimizable(false);

    auto& main_widget = set_main_widget<GUI::Widget>();
    main_widget.load_from_gml(password_change_dialog_gml);

    RefPtr<GUI::Label> description = *main_widget.find_descendant_of_type_named<GUI::Label>("description");
    description->set_text(String::formatted("Updating password for user \"{}\".", m_target_account.username()));

    m_new_password = *main_widget.find_descendant_of_type_named<GUI::PasswordBox>("new_password");
    m_confirm_new_password = *main_widget.find_descendant_of_type_named<GUI::PasswordBox>("confirm_new_password");

    RefPtr<GUI::Button> ok_button = *main_widget.find_descendant_of_type_named<GUI::Button>("ok_button");
    ok_button->on_click = [this](auto) { save_and_close(); };

    RefPtr<GUI::Button> cancel_button = *main_widget.find_descendant_of_type_named<GUI::Button>("cancel_button");
    cancel_button->on_click = [this](auto) { m_event_loop->quit(to_underlying(PasswordChangeDialog::PasswordChangeResult::Fail)); };

    m_no_password_required = *main_widget.find_descendant_of_type_named<GUI::CheckBox>("no_password_required");
    m_no_password_required->on_checked = [this](auto) { on_no_password_required_toggle(); };
    m_login_disabled = *main_widget.find_descendant_of_type_named<GUI::CheckBox>("login_disabled");
    m_login_disabled->on_checked = [this](auto) { on_login_disabled_toggle(); };

    load_state_from_target();
}

void PasswordChangeDialog::load_state_from_target()
{
    auto const& hash = m_target_account.password_hash();

    // Check for blank but non-null password (no password required for login).
    if (hash.is_empty() && !hash.is_null())
        m_no_password_required->set_checked(true);

    // Check for "!" in front of password, or null password (login is disabled).
    if ((hash.length() > 0 && hash[0] == '!') || hash.is_null())
        m_login_disabled->set_checked(true);
}

void PasswordChangeDialog::save_and_close()
{
    auto password = m_new_password->text();
    auto login_disabled = m_login_disabled->is_checked();
    auto no_password_required = m_no_password_required->is_checked();

    if (!(login_disabled || no_password_required)) {
        if (password.is_empty()) {
            // Password shall be present.
            GUI::MessageBox::show_error(this, "Please enter a password, or select a login option."sv);
            m_new_password->set_focus(true);
            return;
        }

        if (password != m_confirm_new_password->text()) {
            // Passwords shall match.
            GUI::MessageBox::show_error(this, "Passwords do not match."sv);
            m_new_password->set_focus(true);
            m_new_password->select_all();
            return;
        }
    }

    if (login_disabled) {
        // If "Login is disabled" is selected, warn about logged-in users.
        GUI::MessageBox::show(this, "User account lock will take effect upon next login attempt."sv, "Information"sv, GUI::MessageBox::Type::Information);

        // Set password as disabled.
        m_target_account.set_password_enabled(false);
    } else if (no_password_required) {
        // If "Password is not required for login" is selected, warn about security implications.
        auto result = GUI::MessageBox::show(this, "This account will be accessible to all users on and off this computer. Are you sure you want to continue?"sv, "Warning"sv, GUI::MessageBox::Type::Warning, GUI::MessageBox::InputType::YesNo);
        if (result == GUI::Dialog::ExecResult::No)
            return;

        m_target_account.delete_password();
    } else {
        // Actually set the password.
        Core::SecretString pass_secret = Core::SecretString::take_ownership(password.to_byte_buffer());
        m_target_account.set_password(pass_secret);
    }

    if (m_target_account.sync().is_error())
        GUI::MessageBox::show_error(this, String::formatted("Failed to update user password!"sv));
    else
        GUI::MessageBox::show(this, "User password updated successfully."sv, "Information"sv, GUI::MessageBox::Type::Information);

    m_event_loop->quit(to_underlying(PasswordChangeDialog::PasswordChangeResult::Success));
}

void PasswordChangeDialog::on_no_password_required_toggle()
{
    bool selected = m_no_password_required->is_checked();

    m_new_password->set_enabled(!selected);
    m_confirm_new_password->set_enabled(!selected);
    m_login_disabled->set_enabled(!selected);
}

void PasswordChangeDialog::on_login_disabled_toggle()
{
    bool selected = m_login_disabled->is_checked();

    m_new_password->set_enabled(!selected);
    m_confirm_new_password->set_enabled(!selected);
    m_no_password_required->set_enabled(!selected);
}

void PasswordChangeDialog::close()
{
    GUI::Window::close();
    if (m_event_loop)
        m_event_loop->quit(to_underlying(PasswordChangeDialog::PasswordChangeResult::Fail));
}

PasswordChangeDialog::PasswordChangeResult PasswordChangeDialog::prompt()
{
    VERIFY(!m_event_loop);
    m_event_loop = make<Core::EventLoop>();

    show();
    auto result = m_event_loop->exec();
    remove_from_parent();
    return static_cast<PasswordChangeDialog::PasswordChangeResult>(result);
}