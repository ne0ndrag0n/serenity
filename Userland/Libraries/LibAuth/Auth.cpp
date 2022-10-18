/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibAuth/Auth.h>
#include <LibAuth/EscalatorWindow.h>
#include <LibCore/Account.h>

namespace Auth {

ErrorOr<bool> authorize_escalation(Optional<GUI::Icon> icon, String description, GUI::Window* parent)
{
    auto current_user = TRY(Core::Account::self());

    // Root already has privilege.
    if (current_user.uid() == 0)
        return true;

    auto window = TRY(EscalatorWindow::try_create(EscalatorWindow::Options { icon, description, current_user, parent }));
    window->set_window_mode(GUI::WindowMode::Blocking);
    window->center_on_screen();

    auto result = window->request_authorization();
    window->close();

    return result;
}

}
