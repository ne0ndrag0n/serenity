/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "EscalatorWindow.h"
#include <AK/Assertions.h>
#include <Applications/Escalator/EscalatorGML.h>
#include <LibCore/SecretString.h>
#include <LibCore/System.h>
#include <LibGUI/FileIconProvider.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Label.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/Widget.h>
#include <unistd.h>

EscalatorWindow::EscalatorWindow(EscalatorWindow::Options const& options)
    : m_arguments(options.arguments)
    , m_executable(options.executable)
    , m_current_user(options.current_user)
    , m_preserve_env(options.preserve_env)
{
    auto app_icon = GUI::FileIconProvider::icon_for_executable(m_executable);

    set_title("Run as Root");
    set_icon(app_icon.bitmap_for_size(16));
    resize(345, 100);
    set_resizable(false);
    set_minimizable(false);

    auto& main_widget = set_main_widget<GUI::Widget>();
    main_widget.load_from_gml(escalator_gml);

    RefPtr<GUI::Label> app_label = *main_widget.find_descendant_of_type_named<GUI::Label>("description");

    String prompt;
    if (options.description.is_empty())
        prompt = String::formatted("{} requires root access. Please enter password for user \"{}\".", m_arguments[0], m_current_user.username());
    else
        prompt = options.description;

    app_label->set_text(prompt);

    m_icon_image_widget = *main_widget.find_descendant_of_type_named<GUI::ImageWidget>("icon");
    m_icon_image_widget->set_bitmap(app_icon.bitmap_for_size(32));

    m_ok_button = *main_widget.find_descendant_of_type_named<GUI::DialogButton>("ok_button");
    m_ok_button->on_click = [this](auto) {
        auto result = check_password();
        if (result.is_error()) {
            GUI::MessageBox::show_error(this, String::formatted("Failed to execute command: {}", result.error()));
            close();
        }
    };
    m_ok_button->set_default(true);

    m_cancel_button = *main_widget.find_descendant_of_type_named<GUI::DialogButton>("cancel_button");
    m_cancel_button->on_click = [this](auto) {
        close();
    };

    m_password_input = *main_widget.find_descendant_of_type_named<GUI::PasswordBox>("password");
}

ErrorOr<void> EscalatorWindow::check_password()
{
    String password = m_password_input->text();
    if (password.is_empty()) {
        GUI::MessageBox::show_error(this, "Please enter a password."sv);
        return {};
    }

    // FIXME: PasswordBox really should store its input directly as a SecretString.
    Core::SecretString password_secret = Core::SecretString::take_ownership(password.to_byte_buffer());
    if (!m_current_user.authenticate(password_secret)) {
        GUI::MessageBox::show_error(this, "Incorrect or disabled password."sv);
        m_password_input->select_all();
        return {};
    }

    // Caller will close Escalator if error is returned.
    TRY(execute_command());
    VERIFY_NOT_REACHED();
}

ErrorOr<void> EscalatorWindow::execute_command()
{
    // Translate environ to format for Core::System::exec.
    Vector<StringView> exec_environ;
    for (size_t i = 0; environ[i]; ++i) {
        StringView environ_setting { environ[i], strlen(environ[i]) };

        if (m_preserve_env || m_current_user.uid() == 0)
            exec_environ.append(environ_setting);
        else {
            // When *not* passing environ, pass only the TERM var as pls does.
            auto split = environ_setting.split_view('=');

            if (split.size() == 2 && split[0] == "TERM"sv && split[1].length()) {
                exec_environ.append(environ_setting);
                break;
            }
        }
    }

    // Escalate process privilege to root user.
    TRY(Core::System::seteuid(0));
    auto root_user = TRY(Core::Account::from_uid(0));
    TRY(root_user.login());

    TRY(Core::System::pledge("stdio sendfd rpath exec"));
    TRY(Core::System::exec(m_executable, m_arguments, Core::System::SearchInPath::No, exec_environ));
    VERIFY_NOT_REACHED();
}
