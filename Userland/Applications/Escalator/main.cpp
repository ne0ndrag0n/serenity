/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/String.h>
#include <LibCore/Account.h>
#include <LibCore/ArgsParser.h>
#include <LibCore/File.h>
#include <LibCore/System.h>
#include <LibEscalator/EscalatorWindow.h>
#include <LibGUI/Application.h>
#include <LibGUI/Desktop.h>
#include <LibGUI/FileIconProvider.h>
#include <LibGUI/MessageBox.h>
#include <LibMain/Main.h>
#include <unistd.h>

static ErrorOr<void> execute_command(StringView executable, Vector<StringView> arguments, bool preserve_env)
{
    // Translate environ to format for Core::System::exec.
    Vector<StringView> exec_environment;
    for (size_t i = 0; environ[i]; ++i) {
        StringView env_view { environ[i], strlen(environ[i]) };
        auto maybe_needle = env_view.find('=');

        if (!maybe_needle.has_value())
            continue;

        if (!preserve_env && env_view.substring_view(0, maybe_needle.value()) != "TERM"sv)
            continue;

        exec_environment.append(env_view);
    }

    // Escalate process privilege to root user.
    TRY(Core::System::seteuid(0));
    auto root_user = TRY(Core::Account::from_uid(0));
    TRY(root_user.login());

    TRY(Core::System::pledge("stdio sendfd rpath exec"));
    TRY(Core::System::exec(executable, arguments, Core::System::SearchInPath::No, exec_environment));
    VERIFY_NOT_REACHED();
}

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    Vector<StringView> command;
    Core::ArgsParser args_parser;
    String description;
    bool preserve_env = false;
    args_parser.set_general_help("Escalate privilege to root for a given command using a GUI prompt.");
    args_parser.set_stop_on_first_non_option(true);
    args_parser.add_option(description, "Custom prompt to use for dialog", "prompt", 'P', "prompt");
    args_parser.add_option(preserve_env, "Preserve user environment when running command", "preserve-env", 'E');
    args_parser.add_positional_argument(command, "Command to run at elevated privilege level", "command");
    args_parser.parse(arguments);

    TRY(Core::System::pledge("stdio recvfd sendfd thread cpath rpath wpath unix proc exec id"));

    auto app = TRY(GUI::Application::try_create(arguments));

    auto executable_path = Core::File::resolve_executable_from_environment(command[0]);
    if (!executable_path.has_value()) {
        GUI::MessageBox::show_error(nullptr, String::formatted("Could not execute command {}: Command not found.", command[0]));
        return 127;
    }

    auto executable = executable_path.value();
    auto current_user = TRY(Core::Account::self());
    if (description.is_empty())
        description = String::formatted("{} requires root access. Please enter password for user \"{}\".", command[0], current_user.username());
    auto app_icon = GUI::FileIconProvider::icon_for_executable(executable);

    auto window = TRY(EscalatorWindow::try_create(EscalatorWindow::Options { app_icon, description, current_user, nullptr }));
    window->center_on_screen();

    if (current_user.uid() != 0) {
        auto result = window->request_authorization();
        if (result == EscalatorWindow::AuthorizationResult::Success)
            TRY(execute_command(executable, command, preserve_env));

        return 1;
    } else {
        // Run directly as root if already root uid.
        TRY(execute_command(executable, command, preserve_env));

        return 0;
    }
}
