import ui
import net
import wndMgr
import dbg
import app
import event
import _weakref
import localeInfo
import uiScriptLocale
import player

class CreateKingdomWindow(ui.ScriptWindow):

	def __init__(self, stream):
		print "NEW CREATE KINGDOM WINDOW  ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, self)

		self.stream = stream
		self.kingdomName = ""
		self.kingdomColor = [255, 255, 255]  # Default white
		self.kingdomFlag = 0  # Default flag design
		
		# UI Elements
		self.nameEditLine = None
		self.colorSliders = {}
		self.flagButtons = []
		self.previewImage = None
		self.createButton = None
		self.cancelButton = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, 0)
		print "---------------------------------------------------------------------------- DELETE CREATE KINGDOM WINDOW"

	def Close(self):
		print "---------------------------------------------------------------------------- CLOSE CREATE KINGDOM WINDOW"		

		self.ClearDictionary()
		self.nameEditLine = None
		self.colorSliders = None
		self.flagButtons = None
		self.previewImage = None
		self.createButton = None
		self.cancelButton = None

		self.KillFocus()
		self.Hide()

		app.HideCursor()
		event.Destroy()

	def Open(self):
		print "OPEN CREATE KINGDOM WINDOW ----------------------------------------------------------------------------"

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("CreateKingdomWindow")
		self.Show()	

		if not self.__LoadScript(uiScriptLocale.LOCALE_UISCRIPT_PATH + "CreateKingdomWindow.py"):
			dbg.TraceError("CreateKingdomWindow.Open - __LoadScript Error")
			return

		self.__CreateUI()
		app.ShowCursor()

	def __CreateUI(self):
		# Create kingdom name input
		self.nameEditLine = ui.EditLine()
		self.nameEditLine.SetParent(self)
		self.nameEditLine.SetPosition(400, 200)
		self.nameEditLine.SetSize(200, 20)
		self.nameEditLine.SetMax(20)
		self.nameEditLine.Show()
		
		# Create color sliders for RGB
		colors = ['Red', 'Green', 'Blue']
		for i, color in enumerate(colors):
			slider = ui.SliderBar()
			slider.SetParent(self)
			slider.SetPosition(400, 250 + i * 40)
			slider.SetSize(200, 20)
			slider.SetSliderPos(1.0)  # Default to maximum
			slider.Show()
			self.colorSliders[color.lower()] = slider
		
		# Create flag selection buttons
		for i in range(5):  # 5 different flag designs
			button = ui.Button()
			button.SetParent(self)
			button.SetPosition(400 + i * 60, 400)
			button.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
			button.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
			button.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
			button.SetEvent(ui.__mem_func__(self.SelectFlag), i)
			button.Show()
			self.flagButtons.append(button)
		
		# Create kingdom button
		self.createButton = ui.Button()
		self.createButton.SetParent(self)
		self.createButton.SetPosition(350, 500)
		self.createButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.createButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.createButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.createButton.SetText("Krallık Oluştur")
		self.createButton.SetEvent(ui.__mem_func__(self.CreateKingdom))
		self.createButton.Show()
		
		# Cancel button
		self.cancelButton = ui.Button()
		self.cancelButton.SetParent(self)
		self.cancelButton.SetPosition(500, 500)
		self.cancelButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.cancelButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.cancelButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.cancelButton.SetText("İptal")
		self.cancelButton.SetEvent(ui.__mem_func__(self.Cancel))
		self.cancelButton.Show()

	def __LoadScript(self, fileName):
		# For now, we'll create UI programmatically
		return 1

	def SelectFlag(self, flagIndex):
		self.kingdomFlag = flagIndex
		# Update preview here

	def CreateKingdom(self):
		self.kingdomName = self.nameEditLine.GetText()
		
		if len(self.kingdomName) < 3:
			# Show error message
			return
		
		# Get color values from sliders
		self.kingdomColor[0] = int(self.colorSliders['red'].GetSliderPos() * 255)
		self.kingdomColor[1] = int(self.colorSliders['green'].GetSliderPos() * 255)
		self.kingdomColor[2] = int(self.colorSliders['blue'].GetSliderPos() * 255)
		
		# Send kingdom creation packet to server
		net.SendCreateKingdomPacket(self.kingdomName, self.kingdomColor, self.kingdomFlag)
		self.stream.SetSelectCharacterPhase()

	def Cancel(self):
		self.stream.SetLoginPhase()

	def OnPressEscapeKey(self):
		self.Cancel()
		return TRUE


class SelectKingdomWindow(ui.ScriptWindow):
	"""Window for selecting existing kingdoms or creating a new one"""
	
	def __init__(self, stream):
		print "NEW SELECT KINGDOM WINDOW  ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, self)

		self.stream = stream
		self.kingdomList = []
		self.selectedKingdom = None
		
		# UI Elements
		self.kingdomListBox = None
		self.joinButton = None
		self.createButton = None
		self.exitButton = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, 0)
		print "---------------------------------------------------------------------------- DELETE SELECT KINGDOM WINDOW"

	def Close(self):
		print "---------------------------------------------------------------------------- CLOSE SELECT KINGDOM WINDOW"		

		self.ClearDictionary()
		self.kingdomListBox = None
		self.joinButton = None
		self.createButton = None
		self.exitButton = None

		self.KillFocus()
		self.Hide()

		app.HideCursor()
		event.Destroy()

	def Open(self):
		print "OPEN SELECT KINGDOM WINDOW ----------------------------------------------------------------------------"

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("SelectKingdomWindow")
		self.Show()	

		self.__CreateUI()
		self.__LoadKingdomList()
		app.ShowCursor()

	def __CreateUI(self):
		# Create kingdom list
		self.kingdomListBox = ui.ListBoxEx()
		self.kingdomListBox.SetParent(self)
		self.kingdomListBox.SetPosition(300, 200)
		self.kingdomListBox.SetSize(400, 300)
		self.kingdomListBox.Show()
		
		# Join kingdom button
		self.joinButton = ui.Button()
		self.joinButton.SetParent(self)
		self.joinButton.SetPosition(300, 520)
		self.joinButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.joinButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.joinButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.joinButton.SetText("Krallığa Katıl")
		self.joinButton.SetEvent(ui.__mem_func__(self.JoinKingdom))
		self.joinButton.Show()
		
		# Create new kingdom button
		self.createButton = ui.Button()
		self.createButton.SetParent(self)
		self.createButton.SetPosition(450, 520)
		self.createButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.createButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.createButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.createButton.SetText("Yeni Krallık")
		self.createButton.SetEvent(ui.__mem_func__(self.CreateNewKingdom))
		self.createButton.Show()
		
		# Exit button
		self.exitButton = ui.Button()
		self.exitButton.SetParent(self)
		self.exitButton.SetPosition(600, 520)
		self.exitButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.exitButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.exitButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.exitButton.SetText("Çıkış")
		self.exitButton.SetEvent(ui.__mem_func__(self.Exit))
		self.exitButton.Show()

	def __LoadKingdomList(self):
		# Request kingdom list from server
		net.SendRequestKingdomListPacket()

	def UpdateKingdomList(self, kingdomList):
		"""Called when kingdom list is received from server"""
		self.kingdomList = kingdomList
		self.kingdomListBox.RemoveAllItems()
		
		for kingdom in kingdomList:
			item = ui.ListBoxEx.Item()
			item.SetText(kingdom['name'] + " (" + str(kingdom['memberCount']) + " üye)")
			self.kingdomListBox.AppendItem(item)

	def JoinKingdom(self):
		selectedIndex = self.kingdomListBox.GetSelectedItem()
		if selectedIndex >= 0 and selectedIndex < len(self.kingdomList):
			kingdom = self.kingdomList[selectedIndex]
			net.SendJoinKingdomPacket(kingdom['id'])
			self.stream.SetSelectCharacterPhase()

	def CreateNewKingdom(self):
		# Switch to create kingdom window
		createWindow = CreateKingdomWindow(self.stream)
		createWindow.Open()
		self.Close()

	def Exit(self):
		self.stream.SetLoginPhase()

	def OnPressEscapeKey(self):
		self.Exit()
		return TRUE
