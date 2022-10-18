/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/Optional.h>
#include <AK/String.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Window.h>

namespace Auth {

ErrorOr<bool> authorize_escalation(Optional<GUI::Icon> icon, String description, GUI::Window* parent);

}
