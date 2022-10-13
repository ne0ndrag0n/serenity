/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "UserSettingsWidget.h"
#include <AK/RefPtr.h>
#include <LibCore/System.h>
#include <LibGUI/Application.h>
#include <LibGUI/Icon.h>
#include <LibGUI/SettingsWindow.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    auto app = TRY(GUI::Application::try_create(arguments));

    auto app_icon = GUI::Icon::default_icon("key"sv);
    auto window = TRY(GUI::SettingsWindow::create("Users and Groups"sv));
    TRY(window->add_tab<UserSettings::UserSettingsWidget>("Users"sv, "users"sv, TRY(Core::Account::all(Core::Account::Read::PasswdOnly))));
    window->set_icon(app_icon.bitmap_for_size(16));
    window->set_active_tab("users"sv);

    window->show();
    return app->exec();
}