/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefPtr.h>
#include <AK/String.h>
#include <LibGUI/AbstractButton.h>
#include <LibGfx/Bitmap.h>

namespace GUI {

class CheckBox : public AbstractButton {
    C_OBJECT(CheckBox);

public:
    virtual ~CheckBox() override = default;

    virtual void click(unsigned modifiers = 0) override;

    void set_icon(Gfx::Bitmap const*);
    void set_icon_from_path(String const&);
    Gfx::Bitmap const* icon() const { return m_icon.ptr(); }
    Gfx::Bitmap* icon() { return m_icon.ptr(); }

    bool is_autosize() const { return m_autosize; }
    void set_autosize(bool);

    enum class CheckBoxPosition {
        Left,
        Right,
    };
    CheckBoxPosition checkbox_position() const { return m_checkbox_position; }
    void set_checkbox_position(CheckBoxPosition value) { m_checkbox_position = value; }

protected:
    explicit CheckBox(String = {});

private:
    void size_to_fit();

    // These don't make sense for a check box, so hide them.
    using AbstractButton::auto_repeat_interval;
    using AbstractButton::set_auto_repeat_interval;

    virtual void paint_event(PaintEvent&) override;

    bool m_autosize { false };
    CheckBoxPosition m_checkbox_position { CheckBoxPosition::Left };
    RefPtr<Gfx::Bitmap> m_icon;
};

}
