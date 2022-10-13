/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullRefPtr.h>
#include <AK/String.h>
#include <AK/Vector.h>
#include <LibGUI/Model.h>
#include <LibGUI/ModelIndex.h>
#include <LibGUI/ModelRole.h>
#include <LibGUI/Variant.h>

class StringArrayModel final : public GUI::Model {

public:
    static NonnullRefPtr<StringArrayModel> create(Vector<String> const& string_array);

    virtual ~StringArrayModel() = default;

    virtual int row_count(GUI::ModelIndex const& = GUI::ModelIndex()) const override { return m_string_array.size(); }
    virtual int column_count(GUI::ModelIndex const& = GUI::ModelIndex()) const override { return 1; }

    virtual GUI::Variant data(GUI::ModelIndex const& index, GUI::ModelRole role) const override;

private:
    StringArrayModel(Vector<String> const& string_array);

    Vector<String> m_string_array;
};