/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "UserProperties.h"
#include "StringArrayModel.h"
#include <AK/String.h>
#include <Applications/UserSettings/UserPropertiesGML.h>
#include <Applications/UserSettings/UserPropertiesGeneralTabGML.h>
#include <Applications/UserSettings/UserPropertiesGroupsTabGML.h>
#include <Applications/UserSettings/UserPropertiesPasswordTabGML.h>
#include <LibCore/File.h>
#include <LibCore/Group.h>
#include <LibCore/System.h>
#include <LibGUI/Icon.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/SortingProxyModel.h>
#include <LibGUI/Widget.h>
#include <LibGfx/Bitmap.h>
#include <errno.h>

namespace UserSettings {

ErrorOr<Vector<String>> UserProperties::get_shells()
{
    auto file = TRY(Core::File::open("/etc/shells"sv, Core::OpenMode::ReadOnly));
    errno = 0;
    auto buffer = file->read_all();
    if (errno)
        return Error::from_errno(errno);

    StringView shells_view = buffer;
    Vector<String> result = { m_account.shell() };
    for (auto const& view : shells_view.split_view('\n'))
        if (view != m_account.shell())
            result.append(view);

    file->close();
    return result;
}

ErrorOr<void> UserProperties::open_ipc_connection()
{
    m_settings_server = TRY(ConnectionToServer::try_create());
    m_settings_server->on_response = [this]() { on_ipc_response(); };
    return {};
}

void UserProperties::setup_general(GUI::Icon const& app_icon)
{
    auto& general_tab = m_tab_widget->add_tab<GUI::Widget>("General");
    general_tab.load_from_gml(user_properties_general_tab_gml);

    m_full_name = general_tab.find_descendant_of_type_named<GUI::TextBox>("full_name");
    m_contact = general_tab.find_descendant_of_type_named<GUI::TextBox>("contact");
    m_office_tel = general_tab.find_descendant_of_type_named<GUI::TextBox>("office_tel");
    m_home_tel = general_tab.find_descendant_of_type_named<GUI::TextBox>("home_tel");
    m_other_info = general_tab.find_descendant_of_type_named<GUI::TextBox>("other_info");

    Vector<RefPtr<GUI::TextBox>> gecos_fields = { m_full_name, m_contact, m_office_tel, m_home_tel, m_other_info };

    Vector<String> fields = m_account.gecos().split(',');
    for (size_t i = 0; i < fields.size(); i++)
        if (i < gecos_fields.size())
            gecos_fields[i]->set_text(fields[i]);

    m_users_icon = general_tab.find_descendant_of_type_named<GUI::ImageWidget>("users_icon");
    m_users_icon->set_bitmap(app_icon.bitmap_for_size(32));

    auto user_name = general_tab.find_descendant_of_type_named<GUI::Label>("user_name");
    user_name->set_text(m_account.username());

    auto label_uid = general_tab.find_descendant_of_type_named<GUI::Label>("label_uid");
    label_uid->set_text(String::formatted("{}", m_account.uid()));

    auto label_gid = general_tab.find_descendant_of_type_named<GUI::Label>("label_gid");
    label_gid->set_text(String::formatted("{}", m_account.gid()));

    auto label_home_path = general_tab.find_descendant_of_type_named<GUI::Label>("label_home_path");
    label_home_path->set_text(String::formatted("{}", m_account.home_directory()));

    auto shells_result = get_shells();
    if (shells_result.is_error()) {
        GUI::MessageBox::show_error(nullptr, String::formatted("Failed to retrieve system shells: {}", shells_result.error()));
        close();
        return;
    }

    m_default_shell = general_tab.find_descendant_of_type_named<GUI::ComboBox>("default_shell");
    m_default_shell->set_model(StringArrayModel::create(shells_result.value()));
    m_default_shell->set_selected_index(0);
    m_default_shell->set_only_allow_values_from_model(true);
}

void UserProperties::setup_password()
{
    auto& password_tab = m_tab_widget->add_tab<GUI::Widget>("Password");
    password_tab.load_from_gml(user_properties_password_tab_gml);

    m_change_password = password_tab.find_descendant_of_type_named<GUI::Button>("change_password");
    m_change_password->set_icon_from_path("/res/icons/16x16/app-escalator.png");
    m_change_password->on_click = [this](auto) { on_setup_password(); };
}

void UserProperties::setup_groups()
{
    auto& groups_tab = m_tab_widget->add_tab<GUI::Widget>("Groups");
    groups_tab.load_from_gml(user_properties_groups_tab_gml);

    m_add_group = groups_tab.find_descendant_of_type_named<GUI::Button>("add_group");
    m_add_group->on_click = [this](auto) { on_add_group_click(); };
    m_set_as_primary = groups_tab.find_descendant_of_type_named<GUI::Button>("set_as_primary");
    m_remove_group = groups_tab.find_descendant_of_type_named<GUI::Button>("remove_group");
    m_remove_group->on_click = [this](auto) { on_remove_group_click(); };

    m_membership_groups = groups_tab.find_descendant_of_type_named<GUI::TableView>("membership_groups");
    m_available_groups = groups_tab.find_descendant_of_type_named<GUI::TableView>("available_groups");

    auto gids = m_account.extra_gids();
    gids.prepend(m_account.gid());

    // Attempt to build user's groups.
    auto membership_model = GroupsModel::create(gids);
    m_membership_model = membership_model;
    auto sort_proxy_result_gids = GUI::SortingProxyModel::create(membership_model);
    if (sort_proxy_result_gids.is_error()) {
        GUI::MessageBox::show_error(nullptr, String::formatted("Failed to list available groups: {}", sort_proxy_result_gids.error()));
        close();
        return;
    }
    m_membership_groups->set_model(sort_proxy_result_gids.value());

    // Attempt to build all groups.
    auto all_groups_result = Core::Group::all();
    if (all_groups_result.is_error()) {
        GUI::MessageBox::show_error(nullptr, String::formatted("Failed to retrieve system groups: {}", all_groups_result.error()));
        close();
        return;
    }

    Vector<gid_t> system_gids;
    for (auto const& group : all_groups_result.value())
        system_gids.append(group.id());

    system_gids.remove_all_matching([&gids](auto& gid) { return gids.contains_slow(gid); });

    auto available_model = GroupsModel::create(system_gids);
    m_available_model = available_model;
    auto sort_proxy_result_available_gids = GUI::SortingProxyModel::create(available_model);
    if (sort_proxy_result_available_gids.is_error()) {
        GUI::MessageBox::show_error(nullptr, String::formatted("Failed to list available groups: {}", sort_proxy_result_available_gids.error()));
        close();
        return;
    }
    m_available_groups->set_model(sort_proxy_result_available_gids.value());

    m_membership_groups->on_selection_change = [this]() { on_membership_groups_change(); };
    m_available_groups->on_selection_change = [this]() { on_available_groups_change(); };
}

UserProperties::UserProperties(Core::Account const& account, GUI::Window* parent_window)
    : GUI::Window(parent_window)
    , m_account(account)
{
    auto app_icon = GUI::Icon::default_icon("key"sv);

    auto& main_widget = set_main_widget<GUI::Widget>();
    main_widget.load_from_gml(user_properties_gml);

    RefPtr<GUI::Button> ok_button = *main_widget.find_descendant_of_type_named<GUI::Button>("ok_button");
    ok_button->on_click = [this](auto) {
        save();
        close();
    };

    RefPtr<GUI::Button> cancel_button = *main_widget.find_descendant_of_type_named<GUI::Button>("cancel_button");
    cancel_button->on_click = [this](auto) { close(); };

    RefPtr<GUI::Button> apply_button = *main_widget.find_descendant_of_type_named<GUI::Button>("apply_button");
    apply_button->on_click = [this](auto) { save(); };

    resize(400, 480);
    set_title(String::formatted("{} Properties"sv, account.username()));
    set_resizable(false);
    set_minimizable(false);
    set_icon(app_icon.bitmap_for_size(16));

    m_tab_widget = *main_widget.find_descendant_of_type_named<GUI::TabWidget>("tab_widget");

    setup_general(app_icon);
    setup_password();
    setup_groups();
}

void UserProperties::save()
{
    // todo
}

void UserProperties::on_membership_groups_change()
{
    m_available_groups->selection().clear();

    bool selection_is_empty = m_membership_groups->selection().is_empty();
    m_add_group->set_enabled(false);
    m_set_as_primary->set_enabled(!selection_is_empty);
    m_remove_group->set_enabled(!selection_is_empty);
}

void UserProperties::on_available_groups_change()
{
    m_membership_groups->selection().clear();

    bool selection_is_empty = m_available_groups->selection().is_empty();
    m_add_group->set_enabled(!selection_is_empty);
    m_set_as_primary->set_enabled(false);
    m_remove_group->set_enabled(false);
}

void UserProperties::on_add_group_click()
{
    VERIFY(m_membership_model);
    VERIFY(m_available_model);

    auto& selection = m_available_groups->selection();
    VERIFY(!selection.is_empty());

    auto index = selection.first();
    gid_t selected_gid = m_available_model->data(index.sibling(index.row(), 0), GUI::ModelRole::Display).to_i32();

    m_available_model->remove_gid(selected_gid);
    m_membership_model->add_gid(selected_gid);
}

void UserProperties::on_remove_group_click()
{
    VERIFY(m_membership_model);
    VERIFY(m_available_model);

    auto& selection = m_membership_groups->selection();
    VERIFY(!selection.is_empty());

    auto index = selection.first();
    gid_t selected_gid = m_membership_model->data(index.sibling(index.row(), 0), GUI::ModelRole::Display).to_i32();

    m_available_model->add_gid(selected_gid);
    m_membership_model->remove_gid(selected_gid);
}

void UserProperties::on_setup_password()
{
    if (open_ipc_connection().is_error()) {
        GUI::MessageBox::show_error(this, "Unable to open connection to AccountServer!"sv);
        return;
    }

    if (m_settings_server->try_request_password_change(m_account.uid(), this).is_error())
        GUI::MessageBox::show_error(this, "Failed to contact AccountServer!"sv);
}

void UserProperties::on_ipc_response()
{
    dbgln("ipc response in UserProperties");
    m_settings_server = nullptr;
}

}