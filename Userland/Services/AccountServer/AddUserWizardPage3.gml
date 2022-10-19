@GUI::Widget {
	layout: @GUI::VerticalBoxLayout {
        margins: [20]
    }

	@GUI::Label {
        text: "Enter any additional relevant information for this user."
        text_alignment: "TopLeft"
        fixed_height: 64
    }

    @GUI::Widget {
        fixed_height: 132
        layout: @GUI::VerticalBoxLayout {
            margins: [0,20,0,80]
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 8
            }

            @GUI::Label {
                text: "Full Name:"
                text_alignment: "CenterLeft"
                fixed_width: 100
            }

            @GUI::TextBox {
                fixed_width: 200
                name: "full_name"
            }
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 8
            }

            @GUI::Label {
                text: "Room Number:"
                text_alignment: "CenterLeft"
                fixed_width: 100
            }

            @GUI::TextBox {
                fixed_width: 200
                name: "room_number"
            }
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 8
            }

            @GUI::Label {
                text: "Office Telephone:"
                text_alignment: "CenterLeft"
                fixed_width: 100
            }

            @GUI::TextBox {
                fixed_width: 200
                name: "office_telephone"
            }
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 8
            }

            @GUI::Label {
                text: "Home Telephone:"
                text_alignment: "CenterLeft"
                fixed_width: 100
            }

            @GUI::TextBox {
                fixed_width: 200
                name: "home_telephone"
            }
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 8
            }

            @GUI::Label {
                text: "Other Information:"
                text_alignment: "CenterLeft"
                fixed_width: 100
            }

            @GUI::TextBox {
                fixed_width: 200
                name: "other_information"
            }
        }
    }
}