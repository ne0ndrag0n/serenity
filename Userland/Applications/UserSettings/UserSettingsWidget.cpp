/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ConnectionToServer.h"
#include "UserSettingsWidget.h"
#include "UserProperties.h"
#include "UsersModel.h"
#include <Applications/UserSettings/UserSettingsWidgetGML.h>
#include <LibGUI/Icon.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/ModelRole.h>
#include <unistd.h>

namespace UserSettings {

UserSettingsWidget::UserSettingsWidget(Vector<Core::Account> const& accounts)
    : m_accounts(accounts)
{
    auto app_icon = GUI::Icon::default_icon("key"sv);

    load_from_gml(user_settings_widget_gml);

    m_users_icon = *find_descendant_of_type_named<GUI::ImageWidget>("users_icon");
    m_users_icon->set_bitmap(app_icon.bitmap_for_size(32));

    m_login_required = *find_descendant_of_type_named<GUI::CheckBox>("login_required");
    m_login_required->set_icon_from_path("/res/icons/16x16/app-escalator.png");

    m_change_root_password = *find_descendant_of_type_named<GUI::Button>("change_root_password");
    m_change_root_password->set_icon_from_path("/res/icons/16x16/app-escalator.png");
    m_change_root_password->on_click = [this](auto) { on_change_root_password(); };

    m_users_list = *find_descendant_of_type_named<GUI::TableView>("users_list");
    m_users_list->set_model(UsersModel::create(accounts));

    m_remove_button = *find_descendant_of_type_named<GUI::Button>("remove_button");
    m_properties_button = *find_descendant_of_type_named<GUI::Button>("properties_button");

    m_users_list->on_selection_change = [this]() {
        auto model = m_users_list->model();
        VERIFY(model);

        auto& selection = m_users_list->selection();
        bool remove_setting = false;
        bool properties_setting = false;
        bool selected = !selection.is_empty();
        remove_setting = properties_setting = selected;

        // Both root and the currently logged in user are not removable.
        if (remove_setting) {
            auto index = selection.first();
            uid_t uid = model->data(index.sibling(index.row(), 0), GUI::ModelRole::Display).to_i32();
            if (uid == 0 || uid == getuid()) {
                remove_setting = false;
            }
        }

        m_remove_button->set_enabled(remove_setting);
        m_properties_button->set_enabled(properties_setting);

        if (selected) {
            m_remove_button->set_tooltip(remove_setting ? "" : "The root and currently logged in users are not removable.");
            m_properties_button->set_tooltip("");
        } else {
            char const* warning = "Please select a user from the list.";
            m_remove_button->set_tooltip(warning);
            m_properties_button->set_tooltip(warning);
        }
    };

    m_properties_button->on_click = [this](auto) {
        auto model = m_users_list->model();
        VERIFY(model);

        auto& selection = m_users_list->selection();
        VERIFY(!selection.is_empty());

        auto index = selection.first();
        uid_t uid = model->data(index.sibling(index.row(), 0), GUI::ModelRole::Display).to_i32();
        dbgln("selected uid {}", uid);

        for (auto const& account : m_accounts) {
            if (account.uid() == uid) {
                RefPtr<UserProperties> properties = settings_window().add<UserProperties>(account);
                properties->on_close = [properties = properties.ptr()] {
                    properties->remove_from_parent();
                };

                properties->show();
                return;
            }
        }

        // If we get here, a fatal error occurs, because there shouldn't be a mismatch
        // between what's in the GUI model and what's in m_accounts.
        VERIFY_NOT_REACHED();
    };
}

ErrorOr<void> UserSettingsWidget::open_ipc_connection()
{
    m_settings_server = TRY(UserSettings::ConnectionToServer::try_create());
    m_settings_server->on_response = [this]() { on_ipc_response(); };
    return {};
}

void UserSettingsWidget::on_change_root_password()
{
    if (open_ipc_connection().is_error()) {
        GUI::MessageBox::show_error(&settings_window(), "Unable to open connection to AccountServer!"sv);
        return;
    }

    if (m_settings_server->try_request_password_change(0, &settings_window()).is_error())
        GUI::MessageBox::show_error(&settings_window(), "Failed to contact AccountServer!"sv);
}

void UserSettingsWidget::on_ipc_response()
{
    dbgln("call complete");
    m_settings_server = nullptr;
}

void UserSettingsWidget::apply_settings()
{
}

}