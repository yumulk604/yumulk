import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/"

window = {
	"name" : "KingdomSettingsWindow",

	"x" : 0,
	"y" : 0,

	"width" : 450,
	"height" : 550,

	"children" :
	(
		## Title Bar
		{
			"name" : "TitleBar",
			"type" : "titlebar",

			"style" : ("attach",),

			"x" : 8,
			"y" : 8,

			"width" : 434,
			"color" : "red",

			"children" :
			(
				{
					"name" : "TitleName",
					"type" : "text",

					"x" : 0,
					"y" : 3,
					"horizontal_align" : "center",

					"text" : "Krallık Ayarları",
					"color" : 0xFFFFFFFF,
				},
			),
		},

		## Main Board
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 450,
			"height" : 550,

			"children" :
			(
				## Kingdom Name
				{
					"name" : "NameText",
					"type" : "text",

					"x" : 30,
					"y" : 50,

					"text" : "Krallık Adı:",
				},
				{
					"name" : "NameEditLine",
					"type" : "editline",

					"x" : 30,
					"y" : 75,

					"width" : 200,
					"height" : 18,

					"input_limit" : 20,
				},

				## Description
				{
					"name" : "DescText",
					"type" : "text",

					"x" : 30,
					"y" : 110,

					"text" : "Açıklama:",
				},
				{
					"name" : "DescEditLine",
					"type" : "editline",

					"x" : 30,
					"y" : 135,

					"width" : 350,
					"height" : 18,

					"input_limit" : 100,
				},

				## Colors
				{
					"name" : "ColorText",
					"type" : "text",

					"x" : 30,
					"y" : 170,

					"text" : "Krallık Renkleri:",
				},

				## Red Slider
				{
					"name" : "RedText",
					"type" : "text",

					"x" : 30,
					"y" : 200,

					"text" : "Kırmızı:",
					"color" : 0xFFFF0000,
				},
				{
					"name" : "RedSlider",
					"type" : "sliderbar",

					"x" : 100,
					"y" : 200,

					"width" : 200,
					"height" : 17,
				},
				{
					"name" : "RedValue",
					"type" : "text",

					"x" : 320,
					"y" : 200,

					"text" : "255",
				},

				## Green Slider
				{
					"name" : "GreenText",
					"type" : "text",

					"x" : 30,
					"y" : 230,

					"text" : "Yeşil:",
					"color" : 0xFF00FF00,
				},
				{
					"name" : "GreenSlider",
					"type" : "sliderbar",

					"x" : 100,
					"y" : 230,

					"width" : 200,
					"height" : 17,
				},
				{
					"name" : "GreenValue",
					"type" : "text",

					"x" : 320,
					"y" : 230,

					"text" : "255",
				},

				## Blue Slider
				{
					"name" : "BlueText",
					"type" : "text",

					"x" : 30,
					"y" : 260,

					"text" : "Mavi:",
					"color" : 0xFF0000FF,
				},
				{
					"name" : "BlueSlider",
					"type" : "sliderbar",

					"x" : 100,
					"y" : 260,

					"width" : 200,
					"height" : 17,
				},
				{
					"name" : "BlueValue",
					"type" : "text",

					"x" : 320,
					"y" : 260,

					"text" : "255",
				},

				## Color Preview
				{
					"name" : "ColorPreview",
					"type" : "bar",

					"x" : 350,
					"y" : 200,

					"width" : 80,
					"height" : 80,

					"color" : 0xFFFFFFFF,
				},

				## Flag Selection
				{
					"name" : "FlagText",
					"type" : "text",

					"x" : 30,
					"y" : 310,

					"text" : "Bayrak Seçimi:",
				},

				## Flag Buttons
				{
					"name" : "FlagButton0",
					"type" : "radio_button",

					"x" : 30,
					"y" : 340,

					"default_image" : ROOT_PATH + "small_button_01.sub",
					"over_image" : ROOT_PATH + "small_button_02.sub",
					"down_image" : ROOT_PATH + "small_button_03.sub",

					"text" : "Ejder",
				},
				{
					"name" : "FlagButton1",
					"type" : "radio_button",

					"x" : 110,
					"y" : 340,

					"default_image" : ROOT_PATH + "small_button_01.sub",
					"over_image" : ROOT_PATH + "small_button_02.sub",
					"down_image" : ROOT_PATH + "small_button_03.sub",

					"text" : "Kartal",
				},
				{
					"name" : "FlagButton2",
					"type" : "radio_button",

					"x" : 190,
					"y" : 340,

					"default_image" : ROOT_PATH + "small_button_01.sub",
					"over_image" : ROOT_PATH + "small_button_02.sub",
					"down_image" : ROOT_PATH + "small_button_03.sub",

					"text" : "Aslan",
				},
				{
					"name" : "FlagButton3",
					"type" : "radio_button",

					"x" : 270,
					"y" : 340,

					"default_image" : ROOT_PATH + "small_button_01.sub",
					"over_image" : ROOT_PATH + "small_button_02.sub",
					"down_image" : ROOT_PATH + "small_button_03.sub",

					"text" : "Kurt",
				},
				{
					"name" : "FlagButton4",
					"type" : "radio_button",

					"x" : 350,
					"y" : 340,

					"default_image" : ROOT_PATH + "small_button_01.sub",
					"over_image" : ROOT_PATH + "small_button_02.sub",
					"down_image" : ROOT_PATH + "small_button_03.sub",

					"text" : "Föniks",
				},

				## Preview
				{
					"name" : "PreviewBoard",
					"type" : "board",

					"x" : 30,
					"y" : 390,

					"width" : 390,
					"height" : 80,

					"children" :
					(
						{
							"name" : "PreviewText",
							"type" : "text",

							"x" : 10,
							"y" : 10,

							"text" : "Önizleme:",
						},
						{
							"name" : "PreviewName",
							"type" : "text",

							"x" : 10,
							"y" : 30,

							"text" : "Krallık Adı",
							"fontsize" : "LARGE",
						},
						{
							"name" : "PreviewDesc",
							"type" : "text",

							"x" : 10,
							"y" : 50,

							"text" : "Krallık açıklaması...",
						},
					),
				},

				## Buttons
				{
					"name" : "SaveButton",
					"type" : "button",

					"x" : 120,
					"y" : 490,

					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",

					"text" : "Kaydet",
				},
				{
					"name" : "CancelButton",
					"type" : "button",

					"x" : 240,
					"y" : 490,

					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",

					"text" : "İptal",
				},
			),
		},
	),
}
