/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "UserSettingsWidget.h"
#include "ConnectionToServer.h"
#include "UserPropertiesGeneralTab.h"
#include "UserPropertiesGroupsTab.h"
#include "UserPropertiesPasswordTab.h"
#include "UsersModel.h"
#include <Applications/UserSettings/UserSettingsWidgetGML.h>
#include <LibGUI/Icon.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/ModelRole.h>
#include <LibGUI/SettingsWindow.h>

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

    m_add_button = *find_descendant_of_type_named<GUI::Button>("add_button");
    m_add_button->set_icon_from_path("/res/icons/16x16/app-escalator.png");
    m_add_button->on_click = [this](auto) { on_add_click(); };

    m_remove_button = *find_descendant_of_type_named<GUI::Button>("remove_button");
    m_remove_button->set_icon_from_path("/res/icons/16x16/app-escalator.png");
    m_remove_button->on_click = [this](auto) { on_remove_click(); };
    m_properties_button = *find_descendant_of_type_named<GUI::Button>("properties_button");

    m_users_list->on_selection_change = [this]() { on_change_user_selection(); };
    m_properties_button->on_click = [this](auto) { on_properties_click(); };
}

bool UserSettingsWidget::flush_staged_settings(Core::Account user, GUI::Window* properties)
{
    auto query = m_staged_settings.get(user.username());
    if (query.has_value()) {
        auto settings = query.value();

        if (open_ipc_connection().is_error()) {
            GUI::MessageBox::show_error(properties, "Unable to open connection to AccountServer!"sv);
            return false;
        }

        m_settings_server->on_user_settings_updated = [this, user, properties] {
            m_staged_settings.remove(user.username());
            UsersModel* model = static_cast<UsersModel*>(m_users_list->model());

            // Reload the user, then resubmit the list of users to the GUI model.
            for (size_t i = 0; i < m_accounts.size(); i++)
                if (m_accounts[i].uid() == user.uid()) {
                    auto result = Core::Account::from_uid(user.uid(), Core::Account::Read::PasswdOnly);
                    if (result.is_error()) {
                        GUI::MessageBox::show_error(properties, String::formatted("Failed to retrieve edited user: {}", result.error()));
                        return;
                    }

                    m_accounts[i] = result.value();
                    model->set_accounts(m_accounts);
                    return;
                }
        };

        if (m_settings_server->try_request_settings_change(user.uid(), settings, properties).is_error()) {
            GUI::MessageBox::show_error(properties, "AccountServer encountered error or user canceled authorization."sv);
            return false;
        }

        return true;
    }

    return false;
}

void UserSettingsWidget::get_settings(String const& username, Function<void(Settings&)> operation)
{
    if (operation)
        operation(m_staged_settings.ensure(username));
}

Optional<uid_t> UserSettingsWidget::get_selected_uid() const
{
    auto model = m_users_list->model();
    VERIFY(model);

    auto& selection = m_users_list->selection();
    if (selection.is_empty())
        return {};

    auto index = selection.first();
    return model->data(index.sibling(index.row(), 0), GUI::ModelRole::Display).to_i32();
}

Optional<String> UserSettingsWidget::get_selected_username() const
{
    auto model = m_users_list->model();
    VERIFY(model);

    auto& selection = m_users_list->selection();
    if (selection.is_empty())
        return {};

    auto index = selection.first();
    return model->data(index.sibling(index.row(), 1), GUI::ModelRole::Display).to_string();
}

void UserSettingsWidget::on_add_click()
{
    if (open_ipc_connection().is_error()) {
        GUI::MessageBox::show_error(&settings_window(), "Unable to open connection to AccountServer!"sv);
        return;
    }

    m_settings_server->on_user_added = [this](i32 user_id) {
        auto new_account = Core::Account::from_uid(user_id, Core::Account::Read::PasswdOnly);
        if (new_account.is_error()) {
            GUI::MessageBox::show_error(&settings_window(), String::formatted("Failed to acquire recently-created account: {}", new_account.error()));
            return;
        }

        m_accounts.append(new_account.value());

        UsersModel* model = static_cast<UsersModel*>(m_users_list->model());
        VERIFY(model);
        model->set_accounts(m_accounts);
    };

    if (m_settings_server->try_request_user_addition(&settings_window()).is_error())
        GUI::MessageBox::show_error(&settings_window(), "AccountServer encountered error or user canceled authorization."sv);
}

void UserSettingsWidget::on_remove_click()
{
    auto selected_uid = get_selected_uid();
    auto selected_username = get_selected_username();
    VERIFY(selected_uid.has_value());
    VERIFY(selected_username.has_value());

    uid_t uid = selected_uid.value();

    auto result = GUI::MessageBox::show(&settings_window(), String::formatted("Are you sure you want to delete user \"{}\"? This action is irreversible.", selected_username.value()), "Warning"sv, GUI::MessageBox::Type::Warning, GUI::MessageBox::InputType::YesNo);
    if (result == GUI::Dialog::ExecResult::No)
        return;

    if (open_ipc_connection().is_error()) {
        GUI::MessageBox::show_error(&settings_window(), "Unable to open connection to AccountServer!"sv);
        return;
    }

    m_settings_server->on_user_deleted = [this, uid]() {
        m_accounts.remove_first_matching([uid](auto const& user) { return user.uid() == uid; });

        UsersModel* model = static_cast<UsersModel*>(m_users_list->model());
        VERIFY(model);
        model->set_accounts(m_accounts);
    };

    if (m_settings_server->try_request_user_deletion(uid, &settings_window()).is_error())
        GUI::MessageBox::show_error(&settings_window(), "AccountServer encountered error or user canceled authorization."sv);
}

void UserSettingsWidget::on_properties_click()
{
    auto selected_uid = get_selected_uid();
    VERIFY(selected_uid.has_value());

    uid_t uid = selected_uid.value();
    dbgln("selected uid {}", uid);

    for (auto const& account : m_accounts) {
        if (account.uid() == uid) {
            auto properties_result = create_properties_window(account);
            if (properties_result.is_error()) {
                GUI::MessageBox::show_error(&settings_window(), "Unable to create Properties window!"sv);
                return;
            }

            auto properties = properties_result.value();
            properties->set_icon(settings_window().icon());
            properties->set_active_tab("general"sv);
            properties->set_quit_on_close(false);
            properties->on_close = [this, username = account.username(), properties = properties.ptr()] {
                properties->remove_from_parent();
                m_staged_settings.remove(username);
            };

            properties->on_settings_applied = [this, account, properties = properties.ptr()] {
                if (properties->is_modified())
                    return flush_staged_settings(account, properties);

                return true;
            };

            settings_window().add_child(*properties);
            properties->show();
            return;
        }
    }

    // If we get here, a fatal error occurs, because there shouldn't be a mismatch
    // between what's in the GUI model and what's in m_accounts.
    VERIFY_NOT_REACHED();
}

void UserSettingsWidget::on_change_user_selection()
{
    auto model = m_users_list->model();
    VERIFY(model);

    auto selection = get_selected_uid();
    bool remove_setting = false;
    bool properties_setting = false;
    bool selected = selection.has_value();
    remove_setting = properties_setting = selected;

    // Both root and the currently logged in user are not removable.
    if (remove_setting) {
        uid_t uid = selection.value();
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
}

ErrorOr<NonnullRefPtr<GUI::SettingsWindow>> UserSettingsWidget::create_properties_window(Core::Account const& account)
{
    auto properties = TRY(GUI::SettingsWindow::create(String::formatted("{} Properties", account.username())));
    TRY(properties->add_tab<UserPropertiesGeneralTab>("General"sv, "general"sv, account, *this));
    TRY(properties->add_tab<UserPropertiesPasswordTab>("Password"sv, "password"sv, account));
    TRY(properties->add_tab<UserPropertiesGroupsTab>("Groups"sv, "groups"sv, account, *this));

    return properties;
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
        GUI::MessageBox::show_error(&settings_window(), "AccountServer encountered error or user canceled authorization."sv);
}

void UserSettingsWidget::on_ipc_response()
{
    dbgln("ipc response in UserSettingsWidget");
    m_settings_server = nullptr;
}

void UserSettingsWidget::apply_settings()
{
}

}
