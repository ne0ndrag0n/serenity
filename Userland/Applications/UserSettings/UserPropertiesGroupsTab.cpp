/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "UserPropertiesGroupsTab.h"
#include "UserSettingsWidget.h"
#include <AK/String.h>
#include <Applications/UserSettings/UserPropertiesGroupsTabGML.h>
#include <LibCore/Group.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/ModelRole.h>
#include <LibGUI/SortingProxyModel.h>
#include <unistd.h>

namespace UserSettings {

UserPropertiesGroupsTab::UserPropertiesGroupsTab(Core::Account const& account, UserSettingsWidget& settings)
    : m_account(account)
    , m_settings(settings)
{
    load_from_gml(user_properties_groups_tab_gml);

    m_add_group = *find_descendant_of_type_named<GUI::Button>("add_group");
    m_add_group->on_click = [this](auto) { on_add_group_click(); };
    m_set_as_primary = *find_descendant_of_type_named<GUI::Button>("set_as_primary");
    m_set_as_primary->on_click = [this](auto) { on_set_as_primary_click(); };
    m_remove_group = *find_descendant_of_type_named<GUI::Button>("remove_group");
    m_remove_group->on_click = [this](auto) { on_remove_group_click(); };

    m_membership_groups = *find_descendant_of_type_named<GUI::TableView>("membership_groups");
    m_available_groups = *find_descendant_of_type_named<GUI::TableView>("available_groups");

    auto gids = m_account.extra_gids();
    gids.prepend(m_account.gid());

    // Attempt to build user's groups.
    auto membership_model = GroupsModel::create(gids, account.gid());
    m_membership_model = membership_model;
    auto sort_proxy_result_gids = GUI::SortingProxyModel::create(membership_model);
    if (sort_proxy_result_gids.is_error()) {
        GUI::MessageBox::show_error(&settings_window(), String::formatted("Failed to list available groups: {}", sort_proxy_result_gids.error()));
        settings_window().close();
        return;
    }
    m_membership_groups->set_model(sort_proxy_result_gids.value());

    // Attempt to build all groups.
    auto all_groups_result = Core::Group::all();
    if (all_groups_result.is_error()) {
        GUI::MessageBox::show_error(&settings_window(), String::formatted("Failed to retrieve system groups: {}", all_groups_result.error()));
        settings_window().close();
        return;
    }

    Vector<gid_t> system_gids;
    for (auto const& group : all_groups_result.value())
        system_gids.append(group.id());

    system_gids.remove_all_matching([&gids](auto& gid) { return gids.contains_slow(gid); });

    auto available_model = GroupsModel::create(system_gids, {});
    m_available_model = available_model;
    auto sort_proxy_result_available_gids = GUI::SortingProxyModel::create(available_model);
    if (sort_proxy_result_available_gids.is_error()) {
        GUI::MessageBox::show_error(&settings_window(), String::formatted("Failed to list available groups: {}", sort_proxy_result_available_gids.error()));
        settings_window().close();
        return;
    }
    m_available_groups->set_model(sort_proxy_result_available_gids.value());

    m_membership_groups->on_selection_change = [this]() { on_membership_groups_change(); };
    m_available_groups->on_selection_change = [this]() { on_available_groups_change(); };
}

void UserPropertiesGroupsTab::on_membership_groups_change()
{
    bool primary_selected = false;
    if (!m_membership_groups->selection().is_empty()) {
        auto index = m_membership_groups->selection().first();
        gid_t selected_gid = m_membership_model->data(index.sibling(index.row(), 0), GUI::ModelRole::Display).to_i32();
        VERIFY(m_membership_model->primary_gid().has_value());
        primary_selected = selected_gid == m_membership_model->primary_gid().value();
    }

    m_available_groups->selection().clear();

    bool selection_is_empty = m_membership_groups->selection().is_empty();
    m_add_group->set_enabled(false);
    m_set_as_primary->set_enabled(primary_selected ? false : !selection_is_empty);
    m_remove_group->set_enabled(primary_selected ? false : !selection_is_empty);
}

void UserPropertiesGroupsTab::on_available_groups_change()
{
    m_membership_groups->selection().clear();

    bool selection_is_empty = m_available_groups->selection().is_empty();
    m_add_group->set_enabled(!selection_is_empty);
    m_set_as_primary->set_enabled(false);
    m_remove_group->set_enabled(false);
}

void UserPropertiesGroupsTab::on_add_group_click()
{
    auto& selection = m_available_groups->selection();
    VERIFY(!selection.is_empty());

    auto index = selection.first();
    gid_t selected_gid = m_available_model->data(index.sibling(index.row(), 0), GUI::ModelRole::Display).to_i32();

    m_available_model->remove_gid(selected_gid);
    m_membership_model->add_gid(selected_gid);

    settings_window().set_modified(true);
}

void UserPropertiesGroupsTab::on_set_as_primary_click()
{
    VERIFY(m_membership_model->primary_gid().has_value());

    auto& selection = m_membership_groups->selection();
    VERIFY(!selection.is_empty());

    auto index = selection.first();
    gid_t selected_gid = m_membership_model->data(index.sibling(index.row(), 0), GUI::ModelRole::Display).to_i32();

    // This should always happen because the button is disabled when you select the primary group.
    VERIFY(selected_gid != m_membership_model->primary_gid().value());

    m_membership_model->set_primary_gid(selected_gid);
}

void UserPropertiesGroupsTab::on_remove_group_click()
{
    auto& selection = m_membership_groups->selection();
    VERIFY(!selection.is_empty());

    auto index = selection.first();
    gid_t selected_gid = m_membership_model->data(index.sibling(index.row(), 0), GUI::ModelRole::Display).to_i32();

    m_available_model->add_gid(selected_gid);
    m_membership_model->remove_gid(selected_gid);

    settings_window().set_modified(true);
}

void UserPropertiesGroupsTab::apply_settings()
{
    if (settings_window().is_modified())
        m_settings.get_settings(m_account.username(), [this](Settings& settings) {
            settings.primary_group = m_membership_model->primary_gid().value();
            settings.groups = m_membership_model->gids();
        });
}

}
