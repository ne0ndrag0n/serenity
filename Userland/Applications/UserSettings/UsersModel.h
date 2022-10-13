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
#include <LibCore/Account.h>
#include <LibGUI/Model.h>
#include <LibGUI/ModelIndex.h>
#include <LibGUI/ModelRole.h>
#include <LibGUI/Variant.h>
#include <sys/types.h>

namespace UserSettings {

class UsersModel final : public GUI::Model {

public:
    static NonnullRefPtr<UsersModel> create(Vector<Core::Account> const& accounts);

    enum Column {
        Uid,
        Username,
        FullName,
        Group,
        Shell,
        __Count
    };

    virtual ~UsersModel() override = default;

    virtual GUI::Variant data(const GUI::ModelIndex&, GUI::ModelRole) const override;

    void set_accounts(Vector<Core::Account> const& accounts);

private:
    UsersModel(Vector<Core::Account> const& accounts);

    virtual int row_count(const GUI::ModelIndex&) const override { return m_accounts.size(); }
    virtual String column_name(int) const override;
    virtual int column_count(const GUI::ModelIndex&) const override { return to_underlying(Column::__Count); }

    Vector<Core::Account> m_accounts;
};

}