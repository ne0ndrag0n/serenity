/*
 * Copyright (c) 2022, Ashley N. <dev-serenity@ne0ndrag0n.com>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "UsersModel.h"
#include <LibCore/System.h>

namespace UserSettings {

NonnullRefPtr<UsersModel> UsersModel::create(Vector<Core::Account> const& accounts)
{
    return adopt_ref(*new UsersModel(accounts));
}

UsersModel::UsersModel(Vector<Core::Account> const& accounts)
    : m_accounts(accounts)
{
}

String UsersModel::column_name(int column) const
{
    switch (column) {
    case Column::Uid:
        return "UID";
    case Column::Username:
        return "User";
    case Column::FullName:
        return "Full Name";
    case Column::Group:
        return "Primary Group";
    case Column::Shell:
        return "Terminal Shell";
    default:
        VERIFY_NOT_REACHED();
    }
}

GUI::Variant UsersModel::data(const GUI::ModelIndex& index, GUI::ModelRole role) const
{
    if (role != GUI::ModelRole::Display)
        return {};

    auto const& account = m_accounts[index.row()];

    switch (index.column()) {
    case Column::Uid:
        return account.uid();
    case Column::Username:
        return account.username();
    case Column::FullName: {
        auto split = account.gecos().split(',');
        return split.is_empty() ? "" : split[0];
    }
    case Column::Group: {
        auto group_query = Core::System::getgrgid(account.gid());
        if (group_query.is_error())
            return "<unknown>";

        if (!group_query.value().has_value())
            return String::formatted("{}", account.gid());

        return String::formatted("{} ({})", group_query.value().value().gr_name, account.gid());
    }
    case Column::Shell:
        return account.shell();
    default:
        VERIFY_NOT_REACHED();
    }
}

void UsersModel::set_accounts(Vector<Core::Account> const& accounts)
{
    m_accounts = accounts;
    invalidate();
}

}
