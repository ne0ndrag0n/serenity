@GUI::Widget {
	fill_with_background_color: true
	layout: @GUI::VerticalBoxLayout {
		margins: [8]
		spacing: 8
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
			name: "user_name"
			text_alignment: "CenterLeft"
		}
	}

	@GUI::HorizontalSeparator {}

	@GUI::Widget {
		fixed_height: 65
		layout: @GUI::VerticalBoxLayout {
			margins: [4, 4]
			spacing: 10
		}

		@GUI::Widget {
			fixed_height: 14
			layout: @GUI::HorizontalBoxLayout {
				spacing: 12
			}

			@GUI::Label {
				text: "UID:"
				text_alignment: "CenterLeft"
				fixed_width: 100
			}

			@GUI::Label {
				name: "label_uid"
				text: "0"
				text_alignment: "CenterLeft"
			}
		}

		@GUI::Widget {
			fixed_height: 14
			layout: @GUI::HorizontalBoxLayout {
				spacing: 12
			}

			@GUI::Label {
				text: "GID (Primary):"
				text_alignment: "CenterLeft"
				fixed_width: 100
			}

			@GUI::Label {
				name: "label_gid"
				text: "0"
				text_alignment: "CenterLeft"
			}
		}

		@GUI::Widget {
			fixed_height: 14
			layout: @GUI::HorizontalBoxLayout {
				spacing: 12
			}

			@GUI::Label {
				text: "Home Directory:"
				text_alignment: "CenterLeft"
				fixed_width: 100
			}

			@GUI::Label {
				name: "label_home_path"
				text: "/root"
				text_alignment: "CenterLeft"
			}
		}
	}

	@GUI::HorizontalSeparator {}

	@GUI::Widget {
		max_height: 30
		layout: @GUI::HorizontalBoxLayout {
			spacing: 8
		}

		@GUI::Label {
			fixed_width: 100
			text: "Default Shell:"
			text_alignment: "CenterLeft"
		}

		@GUI::ComboBox {
			name: "default_shell"
		}
	}

	@GUI::HorizontalSeparator {}

	@GUI::Widget {
		max_height: 140
		layout: @GUI::VerticalBoxLayout {
			spacing: 4
		}

		@GUI::Widget {
			layout: @GUI::HorizontalBoxLayout {
				spacing: 8
			}

			@GUI::Label {
				fixed_width: 100
				text: "Full Name:"
				text_alignment: "CenterLeft"
			}

			@GUI::TextBox {
				name: "full_name"
			}
		}

		@GUI::Widget {
			layout: @GUI::HorizontalBoxLayout {
				spacing: 8
			}

			@GUI::Label {
				fixed_width: 100
				text: "Room Number:"
				text_alignment: "CenterLeft"
			}

			@GUI::TextBox {
				name: "contact"
			}
		}

		@GUI::Widget {
			layout: @GUI::HorizontalBoxLayout {
				spacing: 8
			}

			@GUI::Label {
				fixed_width: 100
				text: "Office Telephone:"
				text_alignment: "CenterLeft"
			}

			@GUI::TextBox {
				name: "office_tel"
			}
		}

		@GUI::Widget {
			layout: @GUI::HorizontalBoxLayout {
				spacing: 8
			}

			@GUI::Label {
				fixed_width: 100
				text: "Home Telephone:"
				text_alignment: "CenterLeft"
			}

			@GUI::TextBox {
				name: "home_tel"
			}
		}

		@GUI::Widget {
			layout: @GUI::HorizontalBoxLayout {
				spacing: 8
			}

			@GUI::Label {
				fixed_width: 100
				text: "Other Information:"
				text_alignment: "CenterLeft"
			}

			@GUI::TextBox {
				name: "other_info"
			}
		}
	}
}