import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/"

window = {
	"name" : "CreateKingdomWindow",

	"x" : 0,
	"y" : 0,

	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,

	"children" :
	(
		## Background
		{
			"name" : "BackGround",
			"type" : "expanded_image",

			"x" : 0,
			"y" : 0,

			"image" : "d:/ymir work/ui/intro/pattern/background_pattern.tga",

			"rect" : (0.0, 0.0, float(SCREEN_WIDTH - 128) / 128.0, float(SCREEN_HEIGHT - 128) / 128.0),
		},

		## Title
		{
			"name" : "Title",
			"type" : "text",

			"x" : 0,
			"y" : 50,
			"horizontal_align" : "center",

			"text" : "Krallık Oluştur",
			"color" : 0xFFFFD700,
			"fontsize" : "LARGE",
		},

		## Main Board
		{
			"name" : "MainBoard",
			"type" : "board_with_titlebar",

			"x" : (SCREEN_WIDTH - 500) / 2,
			"y" : (SCREEN_HEIGHT - 600) / 2,

			"width" : 500,
			"height" : 600,

			"title" : "Yeni Krallık",

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

				## Kingdom Colors
				{
					"name" : "ColorText",
					"type" : "text",

					"x" : 30,
					"y" : 120,

					"text" : "Krallık Renkleri:",
				},

				## Red Slider
				{
					"name" : "RedText",
					"type" : "text",

					"x" : 30,
					"y" : 150,

					"text" : "Kırmızı:",
					"color" : 0xFFFF0000,
				},
				{
					"name" : "RedSlider",
					"type" : "sliderbar",

					"x" : 100,
					"y" : 150,

					"width" : 200,
					"height" : 17,
				},
				{
					"name" : "RedValue",
					"type" : "text",

					"x" : 320,
					"y" : 150,

					"text" : "255",
				},

				## Green Slider
				{
					"name" : "GreenText",
					"type" : "text",

					"x" : 30,
					"y" : 180,

					"text" : "Yeşil:",
					"color" : 0xFF00FF00,
				},
				{
					"name" : "GreenSlider",
					"type" : "sliderbar",

					"x" : 100,
					"y" : 180,

					"width" : 200,
					"height" : 17,
				},
				{
					"name" : "GreenValue",
					"type" : "text",

					"x" : 320,
					"y" : 180,

					"text" : "255",
				},

				## Blue Slider
				{
					"name" : "BlueText",
					"type" : "text",

					"x" : 30,
					"y" : 210,

					"text" : "Mavi:",
					"color" : 0xFF0000FF,
				},
				{
					"name" : "BlueSlider",
					"type" : "sliderbar",

					"x" : 100,
					"y" : 210,

					"width" : 200,
					"height" : 17,
				},
				{
					"name" : "BlueValue",
					"type" : "text",

					"x" : 320,
					"y" : 210,

					"text" : "255",
				},

				## Color Preview
				{
					"name" : "ColorPreview",
					"type" : "bar",

					"x" : 350,
					"y" : 150,

					"width" : 100,
					"height" : 80,

					"color" : 0xFFFFFFFF,
				},

				## Flag Selection
				{
					"name" : "FlagText",
					"type" : "text",

					"x" : 30,
					"y" : 260,

					"text" : "Bayrak Seçimi:",
				},

				## Flag Buttons
				{
					"name" : "FlagButton0",
					"type" : "radio_button",

					"x" : 30,
					"y" : 290,

					"default_image" : ROOT_PATH + "small_button_01.sub",
					"over_image" : ROOT_PATH + "small_button_02.sub",
					"down_image" : ROOT_PATH + "small_button_03.sub",

					"text" : "Ejder",
				},
				{
					"name" : "FlagButton1",
					"type" : "radio_button",

					"x" : 120,
					"y" : 290,

					"default_image" : ROOT_PATH + "small_button_01.sub",
					"over_image" : ROOT_PATH + "small_button_02.sub",
					"down_image" : ROOT_PATH + "small_button_03.sub",

					"text" : "Kartal",
				},
				{
					"name" : "FlagButton2",
					"type" : "radio_button",

					"x" : 210,
					"y" : 290,

					"default_image" : ROOT_PATH + "small_button_01.sub",
					"over_image" : ROOT_PATH + "small_button_02.sub",
					"down_image" : ROOT_PATH + "small_button_03.sub",

					"text" : "Aslan",
				},
				{
					"name" : "FlagButton3",
					"type" : "radio_button",

					"x" : 300,
					"y" : 290,

					"default_image" : ROOT_PATH + "small_button_01.sub",
					"over_image" : ROOT_PATH + "small_button_02.sub",
					"down_image" : ROOT_PATH + "small_button_03.sub",

					"text" : "Kurt",
				},
				{
					"name" : "FlagButton4",
					"type" : "radio_button",

					"x" : 390,
					"y" : 290,

					"default_image" : ROOT_PATH + "small_button_01.sub",
					"over_image" : ROOT_PATH + "small_button_02.sub",
					"down_image" : ROOT_PATH + "small_button_03.sub",

					"text" : "Föniks",
				},

				## Description
				{
					"name" : "DescText",
					"type" : "text",

					"x" : 30,
					"y" : 340,

					"text" : "Açıklama:",
				},
				{
					"name" : "DescEditLine",
					"type" : "editline",

					"x" : 30,
					"y" : 365,

					"width" : 400,
					"height" : 18,

					"input_limit" : 100,
				},

				## Preview Area
				{
					"name" : "PreviewBoard",
					"type" : "board",

					"x" : 30,
					"y" : 410,

					"width" : 440,
					"height" : 100,

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
							"y" : 35,

							"text" : "Krallık Adı",
							"fontsize" : "LARGE",
						},
						{
							"name" : "PreviewDesc",
							"type" : "text",

							"x" : 10,
							"y" : 60,

							"text" : "Krallık açıklaması...",
						},
					),
				},

				## Buttons
				{
					"name" : "CreateButton",
					"type" : "button",

					"x" : 150,
					"y" : 530,

					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",

					"text" : "Krallık Oluştur",
				},
				{
					"name" : "CancelButton",
					"type" : "button",

					"x" : 270,
					"y" : 530,

					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",

					"text" : "İptal",
				},
			),
		},
	),
}
