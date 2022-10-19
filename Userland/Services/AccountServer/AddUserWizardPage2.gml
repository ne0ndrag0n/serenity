@GUI::Widget {
	layout: @GUI::VerticalBoxLayout {
        margins: [20]
    }

	@GUI::Label {
        text: "Enter a password for this account. You may also choose to create the user with no password required to login, or with login disabled."
        text_alignment: "TopLeft"
        fixed_height: 64
    }

    @GUI::Widget {
        fixed_height: 100
        layout: @GUI::VerticalBoxLayout {
            margins: [0,20,0,80]
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 8
            }

            @GUI::Label {
                text: "Password:"
                text_alignment: "CenterLeft"
                fixed_width: 100
            }

            @GUI::PasswordBox {
                fixed_width: 200
                name: "password"
            }
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 8
            }

            @GUI::Label {
                text: "Confirm Password:"
                text_alignment: "CenterLeft"
                fixed_width: 100
            }

            @GUI::PasswordBox {
                fixed_width: 200
                name: "confirm_password"
            }
        }

        @GUI::CheckBox {
            name: "password_not_required"
            text: "Password is not required for login"
        }

        @GUI::CheckBox {
            name: "account_disabled"
            text: "Login is disabled"
        }
    }
}