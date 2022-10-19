/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "AddUserWizardDialog.h"
#include <AccountServer/AddUserWizardPage1GML.h>
#include <AccountServer/AddUserWizardPage2GML.h>
#include <AccountServer/AddUserWizardPage3GML.h>
#include <LibGUI/Button.h>
#include <LibGUI/Icon.h>
#include <LibGUI/MessageBox.h>
#include <string.h>

namespace AccountServer {

AddUserWizardDialog::AddUserWizardDialog(GUI::Window* parent_window)
    : GUI::WizardDialog(parent_window)
{
    auto app_icon = GUI::Icon::default_icon("key"sv);
    set_title("Add New User Wizard");
    set_icon(app_icon.bitmap_for_size(16));

    if (setup_pages().is_error())
        GUI::MessageBox::show_error(parent_window, "Failed to set up wizard pages!"sv);
}

ErrorOr<void> AddUserWizardDialog::setup_pages()
{
    m_front_page = TRY(GUI::CoverWizardPage::try_create());
    m_front_page->set_header_text("Welcome to the Add New User Wizard");
    m_front_page->set_body_text("This wizard helps you add a new user to this computer.\n\nTo continue, click Next.");
    m_front_page->on_next_page = [this]() {
        return m_page_1;
    };

    m_page_1 = TRY(GUI::WizardPage::try_create("Account Username", "Select a username for this user account."));
    m_page_1->body_widget().load_from_gml(add_user_wizard_page_1_gml);
    m_page_1->on_next_page = [this]() {
        return m_page_2;
    };

    m_page_2 = TRY(GUI::WizardPage::try_create("Account Authorization", "Select and configure an authorization method for this user account."));
    m_page_2->body_widget().load_from_gml(add_user_wizard_page_2_gml);
    m_page_2->on_next_page = [this]() {
        return m_page_3;
    };

    m_password_not_required = m_page_2->body_widget().find_descendant_of_type_named<GUI::CheckBox>("password_not_required");
    m_password_not_required->on_checked = [this](auto) {
        bool selected = m_password_not_required->is_checked();

        m_page_2->body_widget().find_descendant_of_type_named<GUI::PasswordBox>("password")->set_enabled(!selected);
        m_page_2->body_widget().find_descendant_of_type_named<GUI::PasswordBox>("confirm_password")->set_enabled(!selected);
        m_account_disabled->set_enabled(!selected);
    };
    m_account_disabled = m_page_2->body_widget().find_descendant_of_type_named<GUI::CheckBox>("account_disabled");
    m_account_disabled->on_checked = [this](auto) {
        bool selected = m_account_disabled->is_checked();

        m_page_2->body_widget().find_descendant_of_type_named<GUI::PasswordBox>("password")->set_enabled(!selected);
        m_page_2->body_widget().find_descendant_of_type_named<GUI::PasswordBox>("confirm_password")->set_enabled(!selected);
        m_password_not_required->set_enabled(!selected);
    };

    m_page_3 = TRY(GUI::WizardPage::try_create("Account Information", "Enter additional details for this user account."));
    m_page_3->body_widget().load_from_gml(add_user_wizard_page_3_gml);
    m_page_3->on_next_page = [this]() {
        return m_back_page;
    };

    m_back_page = TRY(GUI::CoverWizardPage::try_create());
    m_back_page->set_header_text("Wizard complete.");
    m_back_page->set_body_text("Select Finish to create the new user.");
    m_back_page->set_is_final_page(true);

    push_page(*m_front_page);

    return {};
}

void AddUserWizardDialog::handle_next()
{
    ErrorOr<void> validation_result;

    if (&current_page() == m_page_1)
        validation_result = validate_username();
    else if (&current_page() == m_page_2)
        validation_result = validate_password();

    if (validation_result.is_error())
        GUI::MessageBox::show_error(this, String::formatted("{}", validation_result.error()));
    else
        GUI::WizardDialog::handle_next();
}

ErrorOr<void> AddUserWizardDialog::validate_username()
{
    auto username = m_page_1->body_widget().find_descendant_of_type_named<GUI::TextBox>("username");

    if (username->text().is_empty()) {
        username->set_focus(true);
        return Error::from_string_literal("Please enter a username.");
    }

    if (strpbrk(username->text().characters(), "\\/!@#$%^&*()~+=`:\n ")) {
        username->set_focus(true);
        return Error::from_string_literal("Username must not contain a space or any of the following characters:\n\\, /, !, @, #, $, %, ^, &, *, (, ), ~, +, =, `, :");
    }

    if (username->text()[0] == '_' || username->text()[0] == '-' || !isalpha(username->text()[0])) {
        username->set_focus(true);
        return Error::from_string_literal("Username must not start with _, -, or a non-alphabetical character.");
    }

    return {};
}

ErrorOr<void> AddUserWizardDialog::validate_password()
{
    auto password = m_page_2->body_widget().find_descendant_of_type_named<GUI::PasswordBox>("password");
    auto confirm_password = m_page_2->body_widget().find_descendant_of_type_named<GUI::PasswordBox>("confirm_password");

    bool not_required = m_password_not_required->is_checked();
    bool disabled = m_account_disabled->is_checked();

    if (!not_required && !disabled) {
        if (password->text().is_empty()) {
            password->set_focus(true);
            return Error::from_string_literal("Please enter a password.");
        }

        if (password->text() != confirm_password->text()) {
            password->set_focus(true);
            password->select_all();
            return Error::from_string_literal("Password and Confirm Password must match.");
        }
    }

    return {};
}

AddUserWizardDialog::NewUserSettings AddUserWizardDialog::get_settings()
{
    auto username = m_page_1->body_widget().find_descendant_of_type_named<GUI::TextBox>("username");
    auto password = m_page_2->body_widget().find_descendant_of_type_named<GUI::PasswordBox>("password");
    auto full_name = m_page_3->body_widget().find_descendant_of_type_named<GUI::TextBox>("full_name");
    auto room_number = m_page_3->body_widget().find_descendant_of_type_named<GUI::TextBox>("room_number");
    auto office_telephone = m_page_3->body_widget().find_descendant_of_type_named<GUI::TextBox>("office_telephone");
    auto home_telephone = m_page_3->body_widget().find_descendant_of_type_named<GUI::TextBox>("home_telephone");
    auto other_information = m_page_3->body_widget().find_descendant_of_type_named<GUI::TextBox>("other_information");

    String password_string = password->text();
    String gecos = String::formatted("{},{},{},{},{}", full_name->text(), room_number->text(), office_telephone->text(), home_telephone->text(), other_information->text());
    return NewUserSettings { username->text(), Core::SecretString::take_ownership(password_string.to_byte_buffer()), m_password_not_required->is_checked(), m_account_disabled->is_checked(), gecos };
}

}