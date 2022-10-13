/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "ConnectionToServer.h"
#include "Settings.h"
#include <AK/Error.h>
#include <AK/Function.h>
#include <AK/HashMap.h>
#include <AK/NonnullRefPtr.h>
#include <AK/Optional.h>
#include <AK/RefPtr.h>
#include <AK/String.h>
#include <AK/StringView.h>
#include <AK/Vector.h>
#include <LibCore/Account.h>
#include <LibGUI/Button.h>
#include <LibGUI/CheckBox.h>
#include <LibGUI/ImageWidget.h>
#include <LibGUI/SettingsWindow.h>
#include <LibGUI/TableView.h>
#include <LibGUI/Window.h>
#include <unistd.h>

namespace UserSettings {

class UserSettingsWidget final : public GUI::SettingsWindow::Tab {
    C_OBJECT(UserSettingsWidget)

public:
    virtual void apply_settings() override;

    // The lifetime of an object reference obtained from m_staged_settings may be potentially invalidated, so this function
    // allows Properties window tabs to acquire a fresh reference to stage changes in apply_settings.
    void get_settings(String const& username, Function<void(Settings&)> operation);

private:
    UserSettingsWidget(Vector<Core::Account> const& accounts);

    bool flush_staged_settings(Core::Account user, GUI::Window* properties);

    Optional<uid_t> get_selected_uid() const;
    Optional<String> get_selected_username() const;

    void on_add_click();
    void on_remove_click();
    void on_properties_click();

    ErrorOr<NonnullRefPtr<GUI::SettingsWindow>> create_properties_window(Core::Account const& account);
    ErrorOr<void> open_ipc_connection();

    void on_change_root_password();
    void on_change_user_selection();
    void on_ipc_response();

    HashMap<String, Settings> m_staged_settings;

    Vector<Core::Account> m_accounts;
    RefPtr<GUI::ImageWidget> m_users_icon;
    RefPtr<GUI::TableView> m_users_list;
    RefPtr<GUI::CheckBox> m_login_required;
    RefPtr<GUI::Button> m_change_root_password;

    RefPtr<GUI::Button> m_add_button;
    RefPtr<GUI::Button> m_remove_button;
    RefPtr<GUI::Button> m_properties_button;

    RefPtr<ConnectionToServer> m_settings_server;
};

}