/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <LibIPC/Forward.h>

struct UserConfiguration {
	String shell;
	String full_name;
};

namespace IPC {

bool encode(Encoder&, UserConfiguration const&);
ErrorOr<void> decode(Decoder&, UserConfiguration&);

}