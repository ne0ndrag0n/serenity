/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <AK/Vector.h>
#include <sys/types.h>

namespace UserSettings {

struct Settings {
    String shell;
    String gecos;
    Vector<gid_t> groups;
    gid_t primary_group { 0 };
};

}
