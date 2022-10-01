/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "EscalatorWindow.h"
#include <AK/Debug.h>
#include <Applications/Escalator/EscalatorGML.h>
#include <LibCore/MappedFile.h>
#include <LibCore/SecretString.h>
#include <LibCore/System.h>
#include <LibGUI/FileIconProvider.h>
#include <LibGUI/Label.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/Widget.h>
#include <stdlib.h>
#include <unistd.h>

EscalatorWindow::EscalatorWindow(StringView description, Vector<StringView> command, Core::Account current_user)
    : m_command(command)
    , m_current_user(current_user)
{
    auto app_icon = find_icon();

    set_title("Run as Root");
    set_icon(app_icon.bitmap_for_size(16));
    resize(345, 100);
    set_resizable(false);
    set_minimizable(false);

    auto& main_widget = set_main_widget<GUI::Widget>();
    main_widget.load_from_gml(escalator_gml);

    RefPtr<GUI::Label> app_label = *main_widget.find_descendant_of_type_named<GUI::Label>("description");

    String prompt;
    if (description.is_empty())
        prompt = String::formatted("{} requires root access. Please enter password for user \"{}\".", command[0], m_current_user.username());
    else
        prompt = description;

    app_label->set_text(prompt);

    m_icon_image_widget = *main_widget.find_descendant_of_type_named<GUI::ImageWidget>("icon");
    m_icon_image_widget->set_bitmap(app_icon.bitmap_for_size(32));

    m_ok_button = *main_widget.find_descendant_of_type_named<GUI::DialogButton>("ok_button");
    m_ok_button->on_click = [this](auto) {
        auto result = check_password();
        if (result.is_error()) {
            GUI::MessageBox::show_error(this, result.error().string_literal());
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

GUI::Icon EscalatorWindow::find_icon()
{
    String icon_path = m_command[0];

    // Path to executable directly provided.
    if (icon_path.find('/').has_value())
        return GUI::FileIconProvider::icon_for_executable(icon_path);

    // Find the application executable on the user's PATH.
    // If PATH is absent from the environ for whatever reason, fallback on /bin.
    String path = "/bin";
    for (size_t i = 0; environ[i]; ++i) {
        StringView environ_setting { environ[i], strlen(environ[i]) };
        auto split = environ_setting.split_view('=');

        if (split.size() == 2 && split[0] == "PATH" && split[1].length()) {
            path = split[1];
            break;
        }
    }

    Vector<StringView> paths = path.split_view(':');
    for (auto const& path_entry : paths) {
        String full_path = String::formatted("{}/{}", path_entry, m_command[0]);
        auto file_or_error = Core::MappedFile::map(full_path);
        if (!file_or_error.is_error())
            return GUI::FileIconProvider::icon_for_executable(full_path);
    }

    return GUI::Icon::default_icon("app-escalator"sv);
}

ErrorOr<void> EscalatorWindow::check_password()
{
    hide();

    String password = m_password_input->text();
    if (password.is_empty()) {
        GUI::MessageBox::show_error(this, "Please enter a password."sv);
        show();
        return {};
    }

    // FIXME: PasswordBox really should store its input directly as a SecretString.
    Core::SecretString password_secret = Core::SecretString::take_ownership(password.to_byte_buffer());
    if (!m_current_user.authenticate(password_secret)) {
        GUI::MessageBox::show_error(this, "Incorrect or disabled password."sv);
        show();
        return {};
    }

    // Caller will close Escalator if error is returned.
    TRY(execute_command());
    close();
    return {};
}

ErrorOr<int> EscalatorWindow::execute_command()
{
    String shell = m_current_user.shell();

    // Join all variable args from trailing command.
    StringBuilder builder;
    builder.join(' ', m_command);
    String command = builder.to_string();

    char const* shell_executable = shell.characters();
    char const* argv[] = { shell_executable, "-c", command.characters(), nullptr };

    // Escalate process privilege to root user.
    TRY(Core::System::seteuid(0));
    auto root_user = TRY(Core::Account::from_uid(0));
    TRY(root_user.login());

    TRY(Core::System::pledge("stdio recvfd sendfd thread cpath rpath wpath unix proc exec"));
    pid_t child_pid = TRY(Core::System::posix_spawn(shell, nullptr, nullptr, const_cast<char**>(argv), environ));
    auto waitpid_result = TRY(Core::System::waitpid(child_pid));
    if (waitpid_result.status < 0)
        return Error::from_string_literal("Shell failed to execute command.");

    int child_error = WEXITSTATUS(waitpid_result.status);
    dbgln("Child shell exited with code {}", child_error);

    if (child_error == 126)
        return Error::from_string_literal("Command not found.");

    if (child_error == 127)
        return Error::from_string_literal("Shell failed to execute command.");

    dbgln("Escalated task ran successfully.");
    return child_error;
}
