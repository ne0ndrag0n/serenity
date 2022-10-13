@GUI::Frame {
	fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [8]
    }

	@GUI::GroupBox {
		max_height: 90
		title: "User Password"
		layout: @GUI::VerticalBoxLayout {
			margins: [6, 8, 6, 8]
			spacing: 2
		}

		@GUI::Widget {
			layout: @GUI::HorizontalBoxLayout {
				spacing: 16
			}

			@GUI::Label {
				fixed_width: 32
				fixed_height: 32
				icon: "/res/icons/32x32/msgbox-information.png"
			}

			@GUI::Label {
				fixed_height: 40
				text: "Click the button below to authenticate and change, disable, or clear the password for this user."
				text_alignment: "CenterLeft"
			}
		}

		@GUI::Widget {
			layout: @GUI::HorizontalBoxLayout {
				spacing: 8
			}

			@GUI::Layout::Spacer {}

			@GUI::Button {
				fixed_width: 150
				name: "change_password"
				text: "Set User Password..."
			}
		}
	}
}