import uiScriptLocale

LOCALE_PATH = uiScriptLocale.LOGIN_PATH
window = {
	"name" : "LoginWindow",
	"sytle" : ("movable",),

	"x" : 0,
	"y" : 0,

	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,

	"children" :
	(
		{
			"name" : "bg1", "type" : "expanded_image", "x" : 0, "y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1024.0, "y_scale" : float(SCREEN_HEIGHT) / 768.0,
			"image" : "locale/ro/ui/login.sub",
		},

		{ 
			"name" : "id_titlename", 
			"type" : "text", 
			"x" : -75, 
			"y" : -90,
			"all_align" : "center",
			"fontname" : "Verdana:14",
			"outline" : 1,
			"color" : 0xffAB5E00,
			"text" : uiScriptLocale.ID_TITLENAME,
		},
		{
			"name" : "id_slotbar",
			"type" : "image",
			"x" : 0, "y" : -60,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"image" : LOCALE_PATH + "bar.tga",
			"children" : 
			(
				{
					"name" : "ID_EditLine",
					"type" : "editline",
					"x" : 12, "y" : 10,
					"width" : 200, "height" : 16,
					"color" : 0xffc8aa80,
					"input_limit": 16,
				},
			),
		},

		{ 
			"name" : "pwd_titlename", 
			"type" : "text",
			"x" : -112, 
			"y" : -25,
			"all_align" : "center",
			"fontname" : "Verdana:14",
			"outline" : 1,
			"color" : 0xffAB5E00,
			"text" : uiScriptLocale.PASSWORD_TITLENAME,
		},
		{
			"name" : "pwd_slotbar",
			"type" : "image",
			"x" : 0, "y" : 5,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"image" : LOCALE_PATH + "bar.tga",
			"children" : 
			(
				{
					"name" : "Password_EditLine",
					"type" : "editline",
					"x" : 12, "y" : 10,
					"width" : 200, "height" : 16,
					"color" : 0xffc8aa80,
					"input_limit": 16,
					"secret_flag": 1,
				},
			),
		},
		{
			"name" : "LoginButton",
			"type" : "button",
			"x" : 0, 
			"y" : 65,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"default_image" : LOCALE_PATH + "login_0.tga", 
			"over_image" : LOCALE_PATH + "login_1.tga",
			"down_image" : LOCALE_PATH + "login_2.tga",
		},
		{
			"name" : "LoginExitButton",
			"type" : "button",
			"x" : 0,
			"y" : 120,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"default_image" : LOCALE_PATH + "exit_0.tga",
			"over_image" :  LOCALE_PATH + "exit_1.tga",
			"down_image" : LOCALE_PATH + "exit_2.tga",
		},
		{
			"name" : "ch1",
			"type" : "radio_button",
			"x" : 180, "y" : - 55,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"default_image" : LOCALE_PATH + "ch1_0.tga",
			"over_image" : LOCALE_PATH + "ch1_1.tga",
			"down_image" : LOCALE_PATH + "ch1_2.tga",
		},
		{
			"name" : "ch2",
			"type" : "radio_button",
			"x" : 180, "y" : 0,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"default_image" : LOCALE_PATH + "ch2_0.tga",
			"over_image" : LOCALE_PATH + "ch2_1.tga",
			"down_image" : LOCALE_PATH + "ch2_2.tga",
		},
	),
}
