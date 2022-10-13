@GUI::Frame {
	fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [8]
    }

	@GUI::GroupBox {
		max_height: 200
		title: "Group Memberships"
		layout: @GUI::VerticalBoxLayout {
			margins: [6, 8, 6, 8]
			spacing: 2
		}

		@GUI::TableView {
			name: "membership_groups"
		}
	}

	@GUI::Widget {
		max_height: 42
		layout: @GUI::HorizontalBoxLayout {
			margins: [10, 0, 10, 3]
		}

		@GUI::Button {
			fixed_width: 120
			fixed_height: 22
			name: "add_group"
			text: "Join Group"
			enabled: false
		}

		@GUI::Button {
			fixed_width: 120
			fixed_height: 22
			name: "set_as_primary"
			text: "Set as Primary"
			enabled: false
		}

		@GUI::Button {
			fixed_width: 120
			fixed_height: 22
			name: "remove_group"
			text: "Leave Group"
			enabled: false
		}
	}

	@GUI::GroupBox {
		max_height: 200
		title: "Available Groups"
		layout: @GUI::VerticalBoxLayout {
			margins: [6, 8, 6, 8]
			spacing: 2
		}

		@GUI::TableView {
			name: "available_groups"
		}
	}
}