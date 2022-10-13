/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "UserPropertiesPasswordTab.h"
#include <Applications/UserSettings/UserPropertiesPasswordTabGML.h>
#include <LibGUI/MessageBox.h>

namespace UserSettings {

UserPropertiesPasswordTab::UserPropertiesPasswordTab(Core::Account const& account)
    : m_account(account)
{
    load_from_gml(user_properties_password_tab_gml);

    m_change_password = *find_descendant_of_type_named<GUI::Button>("change_password");
    m_change_password->set_icon_from_path("/res/icons/16x16/app-escalator.png");
    m_change_password->on_click = [this](auto) { on_setup_password(); };
}

ErrorOr<void> UserPropertiesPasswordTab::open_ipc_connection()
{
    m_settings_server = TRY(ConnectionToServer::try_create());
    m_settings_server->on_response = [this]() { on_ipc_response(); };
    return {};
}

void UserPropertiesPasswordTab::on_setup_password()
{
    if (open_ipc_connection().is_error()) {
        GUI::MessageBox::show_error(&settings_window(), "Unable to open connection to AccountServer!"sv);
        return;
    }

    if (m_settings_server->try_request_password_change(m_account.uid(), &settings_window()).is_error())
        GUI::MessageBox::show_error(&settings_window(), "AccountServer encountered error or user canceled authorization."sv);
}

void UserPropertiesPasswordTab::on_ipc_response()
{
    dbgln("ipc response in UserPropertiesPasswordTab");
    m_settings_server = nullptr;
}

void UserPropertiesPasswordTab::apply_settings()
{
    dbgln("password tab saving settings");
}

}