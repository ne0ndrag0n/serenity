@GUI::Widget {
	fill_with_background_color: true
	layout: @GUI::VerticalBoxLayout {
		margins: [4]
		spacing: 6
	}

	@GUI::TabWidget {
		name: "tab_widget"
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

		@GUI::Button {
			fixed_width: 80
			fixed_height: 22
			name: "apply_button"
			text: "Apply"
			enabled: false
		}
	}
}