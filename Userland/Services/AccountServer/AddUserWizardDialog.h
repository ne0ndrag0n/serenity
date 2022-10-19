/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <LibCore/Object.h>
#include <LibCore/SecretString.h>
#include <LibGUI/CheckBox.h>
#include <LibGUI/TextBox.h>
#include <LibGUI/Window.h>
#include <LibGUI/Wizards/CoverWizardPage.h>
#include <LibGUI/Wizards/WizardDialog.h>
#include <LibGUI/Wizards/WizardPage.h>

namespace AccountServer {

class AddUserWizardDialog : public GUI::WizardDialog {
    C_OBJECT(AddUserWizardDialog);

    struct NewUserSettings {
        String username;
        Core::SecretString password;
        bool password_not_required { false };
        bool account_disabled { false };
        String gecos;
    };

    // Only valid after ExecResult::OK is returned from exec().
    NewUserSettings get_settings();

protected:
    void handle_next() override;

private:
    AddUserWizardDialog(GUI::Window* parent_window = nullptr);

    void on_password_option_check();

    ErrorOr<void> setup_pages();
    ErrorOr<void> validate_username();
    ErrorOr<void> validate_password();

    RefPtr<GUI::CoverWizardPage> m_front_page;
    RefPtr<GUI::WizardPage> m_page_1;
    RefPtr<GUI::WizardPage> m_page_2;
    RefPtr<GUI::WizardPage> m_page_3;
    RefPtr<GUI::CheckBox> m_password_not_required;
    RefPtr<GUI::CheckBox> m_account_disabled;
    RefPtr<GUI::CoverWizardPage> m_back_page;
};

}