/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "StringArrayModel.h"
#include <LibGfx/TextAlignment.h>

NonnullRefPtr<StringArrayModel> StringArrayModel::create(Vector<String> const& string_array)
{
    return adopt_ref(*new StringArrayModel(string_array));
}

StringArrayModel::StringArrayModel(Vector<String> const& string_array)
    : m_string_array(string_array)
{
}

GUI::Variant StringArrayModel::data(GUI::ModelIndex const& index, GUI::ModelRole role) const
{
    if (role == GUI::ModelRole::TextAlignment)
        return Gfx::TextAlignment::CenterLeft;
    if (role == GUI::ModelRole::Display)
        return m_string_array[index.row()];

    return {};
}