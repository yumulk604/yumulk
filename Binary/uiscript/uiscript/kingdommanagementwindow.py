import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/"

window = {
	"name" : "KingdomManagementWindow",

	"x" : 0,
	"y" : 0,

	"width" : 600,
	"height" : 500,

	"children" :
	(
		## Title Bar
		{
			"name" : "TitleBar",
			"type" : "titlebar",

			"style" : ("attach",),

			"x" : 8,
			"y" : 8,

			"width" : 584,
			"color" : "red",

			"children" :
			(
				{
					"name" : "TitleName",
					"type" : "text",

					"x" : 0,
					"y" : 3,
					"horizontal_align" : "center",

					"text" : "Krallık Yönetimi",
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

			"width" : 600,
			"height" : 500,

			"children" :
			(
				## Kingdom Info
				{
					"name" : "KingdomName",
					"type" : "text",

					"x" : 20,
					"y" : 40,

					"text" : "Krallık Adı",
					"fontsize" : "LARGE",
					"color" : 0xFFFFD700,
				},
				{
					"name" : "MemberCount",
					"type" : "text",

					"x" : 20,
					"y" : 65,

					"text" : "Üye Sayısı: 0",
				},

				## Member List
				{
					"name" : "MemberListBox",
					"type" : "listbox",

					"x" : 20,
					"y" : 100,

					"width" : 350,
					"height" : 280,
				},

				## Member Management Buttons
				{
					"name" : "InviteButton",
					"type" : "button",

					"x" : 390,
					"y" : 100,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",

					"text" : "Davet Et",
				},
				{
					"name" : "KickButton",
					"type" : "button",

					"x" : 390,
					"y" : 140,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",

					"text" : "Üyeyi At",
				},
				{
					"name" : "PromoteButton",
					"type" : "button",

					"x" : 390,
					"y" : 180,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",

					"text" : "Terfi Ettir",
				},
				{
					"name" : "DemoteButton",
					"type" : "button",

					"x" : 390,
					"y" : 220,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",

					"text" : "Rütbe Düşür",
				},

				## Kingdom Management
				{
					"name" : "SettingsButton",
					"type" : "button",

					"x" : 390,
					"y" : 280,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",

					"text" : "Ayarlar",
				},
				{
					"name" : "LeaveButton",
					"type" : "button",

					"x" : 390,
					"y" : 320,

					"default_image" : ROOT_PATH + "middle_button_01.sub",
					"over_image" : ROOT_PATH + "middle_button_02.sub",
					"down_image" : ROOT_PATH + "middle_button_03.sub",

					"text" : "Ayrıl",
				},

				## Close Button
				{
					"name" : "CloseButton",
					"type" : "button",

					"x" : 250,
					"y" : 450,

					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",

					"text" : "Kapat",
				},

				## Rank Legend
				{
					"name" : "RankLegend",
					"type" : "text",

					"x" : 20,
					"y" : 390,

					"text" : "Rütbeler: Üye < Subay < Komutan < Kral",
					"color" : 0xFFAAAA00,
				},
			),
		},
	),
}
