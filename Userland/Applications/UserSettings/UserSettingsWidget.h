/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "ConnectionToServer.h"
#include <AK/RefPtr.h>
#include <AK/Vector.h>
#include <LibCore/Account.h>
#include <LibGUI/Button.h>
#include <LibGUI/CheckBox.h>
#include <LibGUI/ImageWidget.h>
#include <LibGUI/SettingsWindow.h>
#include <LibGUI/TableView.h>
#include <LibGUI/Window.h>

namespace UserSettings {

class UserSettingsWidget final : public GUI::SettingsWindow::Tab {
    C_OBJECT(UserSettingsWidget)

public:
    virtual void apply_settings() override;

private:
    UserSettingsWidget(Vector<Core::Account> const& accounts, RefPtr<ConnectionToServer> server);

    Vector<Core::Account> m_accounts;
    RefPtr<GUI::ImageWidget> m_users_icon;
    RefPtr<GUI::TableView> m_users_list;
    RefPtr<GUI::CheckBox> m_login_required;
    RefPtr<GUI::Button> m_change_root_password;

    RefPtr<GUI::Button> m_remove_button;
    RefPtr<GUI::Button> m_properties_button;

    RefPtr<ConnectionToServer> m_settings_server;
};

}