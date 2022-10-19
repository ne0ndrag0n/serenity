@GUI::Widget {
	fill_with_background_color: true
	layout: @GUI::VerticalBoxLayout {
		margins: [8]
		spacing: 6
	}

	@GUI::Label {
		fixed_height: 22
		name: "description"
		text: "Updating password for user."
		text_alignment: "Center"
	}

	@GUI::HorizontalSeparator {}

	@GUI::Widget {
		layout: @GUI::HorizontalBoxLayout {
			spacing: 8
		}

		@GUI::Label {
			fixed_width: 125
			text: "New Password:"
			text_alignment: "CenterLeft"
		}

		@GUI::PasswordBox {
			name: "new_password"
		}
	}

	@GUI::Widget {
		layout: @GUI::HorizontalBoxLayout {
			spacing: 8
		}

		@GUI::Label {
			fixed_width: 125
			text: "Repeat New Password:"
			text_alignment: "CenterLeft"
		}

		@GUI::PasswordBox {
			name: "confirm_new_password"
		}
	}

	@GUI::HorizontalSeparator {}

	@GUI::Widget {
		fixed_height: 22
		layout: @GUI::HorizontalBoxLayout {}

		@GUI::CheckBox {
			name: "no_password_required"
			text: "Login does not require password"
		}
	}

	@GUI::Widget {
		fixed_height: 22
		layout: @GUI::HorizontalBoxLayout {}

		@GUI::CheckBox {
			name: "login_disabled"
			text: "Login disabled"
		}
	}


	@GUI::Widget {
		fixed_height: 22
		layout: @GUI::HorizontalBoxLayout {
			spacing: 5
		}

		@GUI::Layout::Spacer {}

		@GUI::Button {
			fixed_width: 80
			fixed_height: 22
			name: "ok_button"
			text: "OK"
		}

		@GUI::Button {
			fixed_width: 80
			fixed_height: 22
			name: "cancel_button"
			text: "Cancel"
		}
	}
}