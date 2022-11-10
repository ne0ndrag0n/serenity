/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "UserConfiguration.h"
#include <LibIPC/Decoder.h>
#include <LibIPC/Encoder.h>

namespace IPC {

bool encode(Encoder& encoder, UserConfiguration const& user_configuration)
{
	encoder << user_configuration.shell << user_configuration.full_name;
	return true;
}

ErrorOr<void> decode(Decoder& decoder, UserConfiguration& user_configuration)
{
	TRY(decoder.decode(user_configuration.shell));
	TRY(decoder.decode(user_configuration.full_name));

	return {};
}

}
