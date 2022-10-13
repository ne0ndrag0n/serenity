/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "GroupsModel.h"
#include <grp.h>
#include <unistd.h>

namespace UserSettings {

NonnullRefPtr<GroupsModel> GroupsModel::create(Vector<gid_t> const& gids)
{
    return adopt_ref(*new GroupsModel(gids));
}

GroupsModel::GroupsModel(Vector<gid_t> const& gids)
    : m_gids(gids)
{
}

String GroupsModel::column_name(int column) const
{
    switch (column) {
    case Column::Gid:
        return "GID";
    case Column::GroupName:
        return "Group Name";
    default:
        VERIFY_NOT_REACHED();
    }
}

GUI::Variant GroupsModel::data(const GUI::ModelIndex& index, GUI::ModelRole role) const
{
    if (!(role == GUI::ModelRole::Display || role == GUI::ModelRole::Sort))
        return {};

    auto gid = m_gids[index.row()];

    switch (index.column()) {
    case Column::Gid:
        return gid;
    case Column::GroupName: {
        auto* gr = getgrgid(gid);
        if (!gr) {
            return String::formatted("<unknown> ({})", gid);
        }

        return String::formatted("{}", gr->gr_name);
    }
    default:
        VERIFY_NOT_REACHED();
    }
}

void GroupsModel::add_gid(gid_t gid)
{
    if (!m_gids.contains_slow(gid))
        m_gids.append(gid);

    invalidate();
}

void GroupsModel::remove_gid(gid_t gid)
{
    m_gids.remove_all_matching([gid](gid_t needle) { return needle == gid; });
    invalidate();
}

}