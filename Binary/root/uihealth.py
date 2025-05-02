#Krzywy
import ui
import constInfo

def GetInfoFrom(id):
	table = {
		1	:	constInfo.PLAYER_NAME,
		2	:	constInfo.PLAYER_HP,
		3	:	constInfo.PLAYER_MAX_HP,
		4	:	constInfo.PLAYER_SP,
		5	:	constInfo.PLAYER_MAX_SP}
		
	if table.has_key(id):
		return table[id]
		

class HealthBoard(ui.ThinBoard):

	def __init__(self):
		ui.ThinBoard.__init__(self)
		
		self.Config()

	def __del__(self):
		ui.ThinBoard.__del__(self)
		
	def Config(self):
		self.SetSize(200, 120)
		self.SetPosition(0, 20)
		
		self.hp_bar = ui.Gauge()
		self.hp_bar.SetParent(self)
		self.hp_bar.SetPosition(30, 30+20)
		self.hp_bar.MakeGauge(130, "red")
		self.hp_bar.Show()
		
		self.sp_bar = ui.Gauge()
		self.sp_bar.SetParent(self)
		self.sp_bar.SetPosition(30, 60+20)
		self.sp_bar.MakeGauge(130, "blue")
		self.sp_bar.Show()
		
		self.name = ui.TextLine()
		self.name.SetParent(self)
		self.name.SetDefaultFontName()
		self.name.SetPosition(45, 30)
		self.name.SetText("")
		self.name.Show()	
		
		self.hp_show = ui.TextLine()
		self.hp_show.SetParent(self)
		self.hp_show.SetDefaultFontName()
		self.hp_show.SetPosition(60-15, 57)
		self.hp_show.SetText("")
		self.hp_show.Show()	
		
		self.sp_show = ui.TextLine()
		self.sp_show.SetParent(self)
		self.sp_show.SetDefaultFontName()
		self.sp_show.SetPosition(60-15, 80+7)
		self.sp_show.SetText("")
		self.sp_show.Show()	
		
	def OnUpdate(self):
		if (GetInfoFrom(2)+GetInfoFrom(3)+GetInfoFrom(4)+GetInfoFrom(5)) == 0:
			self.Hide()
		self.hp_bar.SetPercentage(GetInfoFrom(2), GetInfoFrom(3))
		self.sp_bar.SetPercentage(GetInfoFrom(4), GetInfoFrom(5))
		self.name.SetText(GetInfoFrom(1))
		self.hp_show.SetText("PV: %s / %s" % (GetInfoFrom(2), GetInfoFrom(3)))
		self.sp_show.SetText("PM: %s / %s" % (GetInfoFrom(4), GetInfoFrom(5)))
		self.name.SetText("Adversar: %s" % (GetInfoFrom(1)))