@GUI::Widget {
	layout: @GUI::VerticalBoxLayout {
        margins: [20]
    }

	@GUI::Label {
        text: "Select a username for the new user account. This will be the identifier used by this user to login to this computer."
        text_alignment: "TopLeft"
        fixed_height: 64
    }

    @GUI::Widget {
        fixed_height: 25
        layout: @GUI::VerticalBoxLayout {
            margins: [0,20,0,80]
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 8
            }

            @GUI::Label {
                text: "Account Username:"
                text_alignment: "CenterLeft"
                fixed_width: 100
            }

            @GUI::TextBox {
                fixed_width: 200
                name: "username"
            }
        }
    }
}