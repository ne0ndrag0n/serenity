/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Error.h>
#include <AccountServer/ConnectionFromClient.h>
#include <LibCore/EventLoop.h>
#include <LibGUI/Application.h>
#include <LibIPC/SingleServer.h>
#include <LibMain/Main.h>

ErrorOr<int> serenity_main(Main::Arguments)
{
    auto app = TRY(GUI::Application::try_create(0, nullptr));
    app->set_quit_when_last_window_deleted(false);
    dbgln("I am AccountServer!");
    auto client = TRY(IPC::take_over_accepted_client_from_system_server<AccountServer::ConnectionFromClient>());
    return app->exec();
}