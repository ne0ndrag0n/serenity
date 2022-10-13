/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "GroupsModel.h"
#include "Settings.h"
#include <AK/Error.h>
#include <AK/RefPtr.h>
#include <LibCore/Account.h>
#include <LibCore/Object.h>
#include <LibGUI/Button.h>
#include <LibGUI/SettingsWindow.h>
#include <LibGUI/TableView.h>

namespace UserSettings {

class UserSettingsWidget;

class UserPropertiesGroupsTab final : public GUI::SettingsWindow::Tab {
    C_OBJECT(UserPropertiesGroupsTab)

public:
    virtual void apply_settings() override;

private:
    UserPropertiesGroupsTab(Core::Account const& account, UserSettingsWidget& settings);

    void on_membership_groups_change();
    void on_available_groups_change();

    void on_add_group_click();
    void on_set_as_primary_click();
    void on_remove_group_click();

    Core::Account m_account;
    UserSettingsWidget& m_settings;

    RefPtr<GroupsModel> m_membership_model;
    RefPtr<GroupsModel> m_available_model;
    RefPtr<GUI::TableView> m_membership_groups;
    RefPtr<GUI::TableView> m_available_groups;
    RefPtr<GUI::Button> m_add_group;
    RefPtr<GUI::Button> m_set_as_primary;
    RefPtr<GUI::Button> m_remove_group;
};

}