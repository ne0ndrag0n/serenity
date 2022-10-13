@GUI::Frame {
	fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [8]
    }

	@GUI::Widget {
		max_height: 48
		layout: @GUI::HorizontalBoxLayout {
			spacing: 16
		}

		@GUI::ImageWidget {
			name: "users_icon"
		}

		@GUI::Label {
			name: "info"
			text: "Listed below are users and groups available on this computer. Add, edit, and remove users using this tab."
			text_alignment: "CenterLeft"
		}
	}

	@GUI::Widget {
		max_height: 205
        layout: @GUI::VerticalBoxLayout {
			spacing: 8
		}

		@GUI::CheckBox {
			name: "login_required"
			text: "Require users to enter their credentials to use this computer"
			tooltip: "If unselected, you will be prompted for a default user to log in at startup."
		}

		@GUI::TableView {
			name: "users_list"
			fixed_height: 170
		}
	}

	@GUI::Widget {
		max_height: 22
		layout: @GUI::HorizontalBoxLayout {}

		@GUI::Layout::Spacer {}

		@GUI::DialogButton {
			name: "add_button"
			text: "Add..."
		}

		@GUI::DialogButton {
			name: "remove_button"
			text: "Remove"
			enabled: false
			tooltip: "Please select a user from the list."
		}

		@GUI::DialogButton {
			name: "properties_button"
			text: "Properties"
			enabled: false
			tooltip: "Please select a user from the list."
		}
	}

	@GUI::GroupBox {
		title: "Root Password"
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
				icon: "/res/icons/32x32/settings-root.png"
			}

			@GUI::Label {
				fixed_height: 40
				text: "The root user is used for actions requiring the highest level of privilege on this computer. Use these settings carefully to configure access to the root user."
				text_alignment: "CenterLeft"
			}
		}

		@GUI::Widget {
			layout: @GUI::VerticalBoxLayout {

			}

			@GUI::Layout::Spacer {}
		}

		@GUI::Widget {
			layout: @GUI::HorizontalBoxLayout {
				spacing: 8
			}

			@GUI::Layout::Spacer {}

			@GUI::Button {
				fixed_width: 150
				name: "change_root_password"
				text: "Set Root Password..."
			}
		}
	}
}