/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "UserPropertiesGeneralTab.h"
#include "StringArrayModel.h"
#include "UserSettingsWidget.h"
#include <Applications/UserSettings/UserPropertiesGeneralTabGML.h>
#include <LibCore/EventLoop.h>
#include <LibCore/File.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Label.h>
#include <LibGUI/MessageBox.h>

namespace UserSettings {

ErrorOr<Vector<String>> UserPropertiesGeneralTab::get_shells()
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

UserPropertiesGeneralTab::UserPropertiesGeneralTab(Core::Account const& account, UserSettingsWidget& settings)
    : m_account(account)
    , m_settings(settings)
{
    auto app_icon = GUI::Icon::default_icon("key"sv);

    load_from_gml(user_properties_general_tab_gml);

    m_full_name = *find_descendant_of_type_named<GUI::TextBox>("full_name");
    m_contact = *find_descendant_of_type_named<GUI::TextBox>("contact");
    m_office_tel = *find_descendant_of_type_named<GUI::TextBox>("office_tel");
    m_home_tel = *find_descendant_of_type_named<GUI::TextBox>("home_tel");
    m_other_info = *find_descendant_of_type_named<GUI::TextBox>("other_info");

    Vector<RefPtr<GUI::TextBox>> gecos_fields = { m_full_name, m_contact, m_office_tel, m_home_tel, m_other_info };

    Vector<String> fields = m_account.gecos().split(',');
    for (size_t i = 0; i < fields.size(); i++)
        if (i < gecos_fields.size())
            gecos_fields[i]->set_text(fields[i]);

    m_users_icon = *find_descendant_of_type_named<GUI::ImageWidget>("users_icon");
    m_users_icon->set_bitmap(app_icon.bitmap_for_size(32));

    RefPtr<GUI::Label> user_name = *find_descendant_of_type_named<GUI::Label>("user_name");
    user_name->set_text(m_account.username());

    RefPtr<GUI::Label> label_uid = *find_descendant_of_type_named<GUI::Label>("label_uid");
    label_uid->set_text(String::formatted("{}", m_account.uid()));

    RefPtr<GUI::Label> label_gid = *find_descendant_of_type_named<GUI::Label>("label_gid");
    label_gid->set_text(String::formatted("{}", m_account.gid()));

    RefPtr<GUI::Label> label_home_path = *find_descendant_of_type_named<GUI::Label>("label_home_path");
    label_home_path->set_text(String::formatted("{}", m_account.home_directory()));

    auto shells_result = get_shells();
    if (shells_result.is_error()) {
        GUI::MessageBox::show_error(nullptr, String::formatted("Failed to retrieve system shells: {}", shells_result.error()));
        settings_window().close();
        return;
    }

    m_default_shell = *find_descendant_of_type_named<GUI::ComboBox>("default_shell");
    m_default_shell->set_model(StringArrayModel::create(shells_result.value()));
    m_default_shell->set_selected_index(0);
    m_default_shell->set_only_allow_values_from_model(true);

    // Constructor needs to be complete before these on_change handlers are assigned.
    Core::EventLoop::current().deferred_invoke([this] {
        m_full_name->on_change = [this]() { settings_window().set_modified(true); };
        m_contact->on_change = [this]() { settings_window().set_modified(true); };
        m_office_tel->on_change = [this]() { settings_window().set_modified(true); };
        m_home_tel->on_change = [this]() { settings_window().set_modified(true); };
        m_other_info->on_change = [this]() { settings_window().set_modified(true); };
        m_default_shell->on_change = [this](auto, auto) { settings_window().set_modified(true); };
    });
}

void UserPropertiesGeneralTab::apply_settings()
{
    if (settings_window().is_modified())
        m_settings.get_settings(m_account.username(), [this](Settings& settings) {
            settings.shell = m_default_shell->text();
            settings.gecos = String::formatted("{},{},{},{},{}", m_full_name->text(), m_contact->text(), m_office_tel->text(), m_home_tel->text(), m_other_info->text());
        });
}

}