/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "ConnectionToServer.h"
#include "GroupsModel.h"
#include <AK/Error.h>
#include <AK/RefPtr.h>
#include <AK/Vector.h>
#include <LibCore/Account.h>
#include <LibCore/Object.h>
#include <LibGUI/Button.h>
#include <LibGUI/CheckBox.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/Icon.h>
#include <LibGUI/ImageWidget.h>
#include <LibGUI/Label.h>
#include <LibGUI/TabWidget.h>
#include <LibGUI/TableView.h>
#include <LibGUI/TextBox.h>
#include <LibGUI/Window.h>

namespace UserSettings {

class UserProperties final : public GUI::Window {
    C_OBJECT(UserProperties)

private:
    UserProperties(Core::Account const& account, GUI::Window* parent_window = nullptr);

    void setup_general(GUI::Icon const& app_icon);
    void setup_password();
    void setup_groups();

    ErrorOr<Vector<String>> get_shells();
    ErrorOr<void> open_ipc_connection();

    void save();

    void on_membership_groups_change();
    void on_available_groups_change();

    void on_add_group_click();
    void on_remove_group_click();
    void on_setup_password();
    void on_ipc_response();

    RefPtr<GUI::TabWidget> m_tab_widget;

    // General Tab.
    RefPtr<GUI::ImageWidget> m_users_icon;
    RefPtr<GUI::ComboBox> m_default_shell;
    RefPtr<GUI::TextBox> m_full_name;
    RefPtr<GUI::TextBox> m_contact;
    RefPtr<GUI::TextBox> m_office_tel;
    RefPtr<GUI::TextBox> m_home_tel;
    RefPtr<GUI::TextBox> m_other_info;

    // Passwords Tab.
    RefPtr<GUI::Button> m_change_password;

    // Groups Tab.
    RefPtr<GroupsModel> m_membership_model;
    RefPtr<GroupsModel> m_available_model;
    RefPtr<GUI::TableView> m_membership_groups;
    RefPtr<GUI::TableView> m_available_groups;
    RefPtr<GUI::Button> m_add_group;
    RefPtr<GUI::Button> m_set_as_primary;
    RefPtr<GUI::Button> m_remove_group;

    Core::Account m_account;

    RefPtr<ConnectionToServer> m_settings_server;
};

}