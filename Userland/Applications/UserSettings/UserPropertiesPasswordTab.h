/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "ConnectionToServer.h"
#include <AK/Error.h>
#include <AK/RefPtr.h>
#include <AK/String.h>
#include <AK/Vector.h>
#include <LibCore/Account.h>
#include <LibGUI/Button.h>
#include <LibGUI/SettingsWindow.h>

namespace UserSettings {

class UserPropertiesPasswordTab final : public GUI::SettingsWindow::Tab {
    C_OBJECT(UserPropertiesPasswordTab)

public:
    virtual void apply_settings() override;

private:
    UserPropertiesPasswordTab(Core::Account const& account);

    ErrorOr<void> open_ipc_connection();
    void on_setup_password();
    void on_ipc_response();

    RefPtr<GUI::Button> m_change_password;

    Core::Account m_account;

    RefPtr<ConnectionToServer> m_settings_server;
};

}
