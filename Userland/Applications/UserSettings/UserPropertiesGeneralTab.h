/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Settings.h"
#include <AK/Error.h>
#include <AK/NonnullRefPtr.h>
#include <AK/String.h>
#include <AK/Vector.h>
#include <LibCore/Account.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/ImageWidget.h>
#include <LibGUI/SettingsWindow.h>
#include <LibGUI/TextBox.h>

namespace UserSettings {

class UserSettingsWidget;

class UserPropertiesGeneralTab final : public GUI::SettingsWindow::Tab {
    C_OBJECT(UserPropertiesGeneralTab)

public:
    virtual void apply_settings() override;

private:
    UserPropertiesGeneralTab(Core::Account const& account, UserSettingsWidget& settings);

    ErrorOr<Vector<String>> get_shells();

    Core::Account m_account;
    UserSettingsWidget& m_settings;

    RefPtr<GUI::ImageWidget> m_users_icon;
    RefPtr<GUI::ComboBox> m_default_shell;
    RefPtr<GUI::TextBox> m_full_name;
    RefPtr<GUI::TextBox> m_contact;
    RefPtr<GUI::TextBox> m_office_tel;
    RefPtr<GUI::TextBox> m_home_tel;
    RefPtr<GUI::TextBox> m_other_info;
};

}