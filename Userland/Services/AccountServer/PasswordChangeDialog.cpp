/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "PasswordChangeDialog.h"
#include <AK/String.h>
#include <AccountServer/PasswordChangeDialogGML.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Label.h>
#include <LibGUI/Widget.h>

PasswordChangeDialog::PasswordChangeDialog(Core::Account const& account, GUI::Window* parent_window)
    : GUI::Window(parent_window)
    , m_target_account(account)
{
    auto app_icon = GUI::Icon::default_icon("key"sv);

    set_icon(app_icon.bitmap_for_size(16));
    set_title("Update Password"sv);
    resize(345, 125);
    set_resizable(false);
    set_minimizable(false);

    auto& main_widget = set_main_widget<GUI::Widget>();
    main_widget.load_from_gml(password_change_dialog_gml);

    RefPtr<GUI::Label> description = *main_widget.find_descendant_of_type_named<GUI::Label>("description");
    description->set_text(String::formatted("Updating password for user \"{}\"", m_target_account.username()));
}

void PasswordChangeDialog::close()
{
    GUI::Window::close();
    if (m_event_loop)
        m_event_loop->quit(0);
}

void PasswordChangeDialog::prompt()
{
    VERIFY(!m_event_loop);
    m_event_loop = make<Core::EventLoop>();

    show();
    m_event_loop->exec();
    remove_from_parent();
}