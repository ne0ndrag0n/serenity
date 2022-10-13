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
#include <sys/types.h>

namespace UserSettings {

class GroupsModel final : public GUI::Model {

public:
    static NonnullRefPtr<GroupsModel> create(Vector<gid_t> const& gids);

    enum Column {
        Gid,
        GroupName,
        __Count
    };

    virtual ~GroupsModel() override = default;

    virtual GUI::Variant data(const GUI::ModelIndex&, GUI::ModelRole) const override;

    void add_gid(gid_t gid);
    void remove_gid(gid_t gid);

private:
    GroupsModel(Vector<gid_t> const& gids);

    virtual int row_count(const GUI::ModelIndex&) const override { return m_gids.size(); }
    virtual String column_name(int) const override;
    virtual int column_count(const GUI::ModelIndex&) const override { return to_underlying(Column::__Count); }

    Vector<gid_t> m_gids;
};

}