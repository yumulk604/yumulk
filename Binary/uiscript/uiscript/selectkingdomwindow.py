import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/"

window = {
	"name" : "SelectKingdomWindow",

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

			"text" : "Krallık Seçimi",
			"color" : 0xFFFFD700,
			"fontsize" : "LARGE",
		},

		## Main Board
		{
			"name" : "MainBoard",
			"type" : "board_with_titlebar",

			"x" : (SCREEN_WIDTH - 600) / 2,
			"y" : (SCREEN_HEIGHT - 500) / 2,

			"width" : 600,
			"height" : 500,

			"title" : "Mevcut Krallıklar",

			"children" :
			(
				## Kingdom List
				{
					"name" : "KingdomListBox",
					"type" : "listbox",

					"x" : 20,
					"y" : 50,

					"width" : 560,
					"height" : 300,
				},

				## Kingdom Info Panel
				{
					"name" : "InfoBoard",
					"type" : "board",

					"x" : 20,
					"y" : 360,

					"width" : 560,
					"height" : 80,

					"children" :
					(
						{
							"name" : "InfoText",
							"type" : "text",

							"x" : 10,
							"y" : 10,

							"text" : "Krallık bilgileri burada görünecek...",
						},
						{
							"name" : "MemberCountText",
							"type" : "text",

							"x" : 10,
							"y" : 30,

							"text" : "Üye Sayısı: -",
						},
						{
							"name" : "KingNameText",
							"type" : "text",

							"x" : 10,
							"y" : 50,

							"text" : "Kral: -",
						},
					),
				},

				## Buttons
				{
					"name" : "JoinButton",
					"type" : "button",

					"x" : 50,
					"y" : 450,

					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",

					"text" : "Krallığa Katıl",
				},
				{
					"name" : "CreateButton",
					"type" : "button",

					"x" : 200,
					"y" : 450,

					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",

					"text" : "Yeni Krallık",
				},
				{
					"name" : "RefreshButton",
					"type" : "button",

					"x" : 350,
					"y" : 450,

					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",

					"text" : "Yenile",
				},
				{
					"name" : "ExitButton",
					"type" : "button",

					"x" : 500,
					"y" : 450,

					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",

					"text" : "Çıkış",
				},
			),
		},
	),
}
