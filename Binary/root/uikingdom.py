import ui
import uiScriptLocale
import localeInfo
import net
import player
import chat

class KingdomManagementWindow(ui.ScriptWindow):
	"""Krallık yönetim penceresi"""
	
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		
		# Kingdom data
		self.kingdomInfo = {}
		self.memberList = []
		self.selectedMember = None
		
		# UI Elements
		self.titleBar = None
		self.board = None
		self.kingdomNameText = None
		self.memberCountText = None
		self.memberListBox = None
		self.inviteButton = None
		self.kickButton = None
		self.promoteButton = None
		self.demoteButton = None
		self.leaveButton = None
		self.settingsButton = None
		self.closeButton = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/KingdomManagementWindow.py")
		except:
			import exception
			exception.Abort("KingdomManagementWindow.LoadWindow.LoadObject")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.board = self.GetChild("board")
			self.kingdomNameText = self.GetChild("KingdomName")
			self.memberCountText = self.GetChild("MemberCount")
			self.memberListBox = self.GetChild("MemberListBox")
			self.inviteButton = self.GetChild("InviteButton")
			self.kickButton = self.GetChild("KickButton")
			self.promoteButton = self.GetChild("PromoteButton")
			self.demoteButton = self.GetChild("DemoteButton")
			self.leaveButton = self.GetChild("LeaveButton")
			self.settingsButton = self.GetChild("SettingsButton")
			self.closeButton = self.GetChild("CloseButton")
		except:
			import exception
			exception.Abort("KingdomManagementWindow.LoadWindow.BindObject")

		# Set button events
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.inviteButton.SetEvent(ui.__mem_func__(self.OnInvite))
		self.kickButton.SetEvent(ui.__mem_func__(self.OnKick))
		self.promoteButton.SetEvent(ui.__mem_func__(self.OnPromote))
		self.demoteButton.SetEvent(ui.__mem_func__(self.OnDemote))
		self.leaveButton.SetEvent(ui.__mem_func__(self.OnLeave))
		self.settingsButton.SetEvent(ui.__mem_func__(self.OnSettings))
		self.closeButton.SetEvent(ui.__mem_func__(self.Close))

		self.memberListBox.SetSelectEvent(ui.__mem_func__(self.OnSelectMember))

	def Destroy(self):
		self.ClearDictionary()
		
		self.titleBar = None
		self.board = None
		self.kingdomNameText = None
		self.memberCountText = None
		self.memberListBox = None
		self.inviteButton = None
		self.kickButton = None
		self.promoteButton = None
		self.demoteButton = None
		self.leaveButton = None
		self.settingsButton = None
		self.closeButton = None

	def Open(self):
		if self.isLoaded == 0:
			self.LoadWindow()

		self.SetCenterPosition()
		self.SetTop()
		self.Show()

		# Request kingdom info from server
		net.SendRequestKingdomInfoPacket()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def UpdateKingdomInfo(self, kingdomInfo):
		"""Server'dan gelen krallık bilgilerini güncelle"""
		self.kingdomInfo = kingdomInfo
		
		if self.kingdomNameText:
			self.kingdomNameText.SetText(kingdomInfo.get('name', 'Bilinmeyen Krallık'))
		
		if self.memberCountText:
			memberCount = len(kingdomInfo.get('members', []))
			self.memberCountText.SetText("Üye Sayısı: %d" % memberCount)

	def UpdateMemberList(self, memberList):
		"""Üye listesini güncelle"""
		self.memberList = memberList
		
		if self.memberListBox:
			self.memberListBox.RemoveAllItems()
			
			for member in memberList:
				item = ui.ListBoxEx.Item()
				rankText = self.GetRankText(member.get('rank', 0))
				item.SetText("%s (%s)" % (member.get('name', ''), rankText))
				self.memberListBox.AppendItem(item)

	def GetRankText(self, rank):
		"""Rütbe numarasını metne çevir"""
		rankTexts = {
			0: "Üye",
			1: "Subay", 
			2: "Komutan",
			3: "Kral"
		}
		return rankTexts.get(rank, "Bilinmeyen")

	def OnSelectMember(self):
		"""Üye seçildiğinde"""
		selectedIndex = self.memberListBox.GetSelectedItem()
		if selectedIndex >= 0 and selectedIndex < len(self.memberList):
			self.selectedMember = self.memberList[selectedIndex]
			
			# Butonları güncelle
			self.UpdateButtonStates()

	def UpdateButtonStates(self):
		"""Seçilen üyeye göre buton durumlarını güncelle"""
		if not self.selectedMember:
			return
			
		playerRank = self.GetPlayerRank()
		selectedMemberRank = self.selectedMember.get('rank', 0)
		
		# Sadece daha yüksek rütbeli oyuncular işlem yapabilir
		canManage = playerRank > selectedMemberRank
		
		if self.kickButton:
			self.kickButton.Enable() if canManage else self.kickButton.Disable()
		if self.promoteButton:
			self.promoteButton.Enable() if canManage and selectedMemberRank < 3 else self.promoteButton.Disable()
		if self.demoteButton:
			self.demoteButton.Enable() if canManage and selectedMemberRank > 0 else self.demoteButton.Disable()

	def GetPlayerRank(self):
		"""Oyuncunun krallıktaki rütbesini al"""
		playerName = player.GetName()
		for member in self.memberList:
			if member.get('name') == playerName:
				return member.get('rank', 0)
		return 0

	def OnInvite(self):
		"""Oyuncu davet et"""
		inputDialog = uiCommon.InputDialog()
		inputDialog.SetTitle("Oyuncu Davet Et")
		inputDialog.SetAcceptEvent(ui.__mem_func__(self.OnInviteAccept))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.OnInviteCancel))
		inputDialog.Open()
		self.inputDialog = inputDialog

	def OnInviteAccept(self):
		playerName = self.inputDialog.GetText()
		if len(playerName) > 0:
			net.SendKingdomInvitePacket(playerName)
			chat.AppendChat(chat.CHAT_TYPE_INFO, "%s oyuncusuna davet gönderildi." % playerName)
		self.inputDialog.Close()
		self.inputDialog = None

	def OnInviteCancel(self):
		self.inputDialog.Close()
		self.inputDialog = None

	def OnKick(self):
		"""Seçili üyeyi at"""
		if not self.selectedMember:
			return
			
		memberName = self.selectedMember.get('name', '')
		net.SendKingdomKickPacket(memberName)
		chat.AppendChat(chat.CHAT_TYPE_INFO, "%s krallıktan atıldı." % memberName)

	def OnPromote(self):
		"""Seçili üyeyi terfi ettir"""
		if not self.selectedMember:
			return
			
		memberName = self.selectedMember.get('name', '')
		currentRank = self.selectedMember.get('rank', 0)
		newRank = min(currentRank + 1, 3)
		
		net.SendKingdomRankPacket(memberName, newRank)
		chat.AppendChat(chat.CHAT_TYPE_INFO, "%s terfi ettirildi." % memberName)

	def OnDemote(self):
		"""Seçili üyeyi rütbe düşür"""
		if not self.selectedMember:
			return
			
		memberName = self.selectedMember.get('name', '')
		currentRank = self.selectedMember.get('rank', 0)
		newRank = max(currentRank - 1, 0)
		
		net.SendKingdomRankPacket(memberName, newRank)
		chat.AppendChat(chat.CHAT_TYPE_INFO, "%s rütbesi düşürüldü." % memberName)

	def OnLeave(self):
		"""Krallıktan ayrıl"""
		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText("Krallıktan ayrılmak istediğinizden emin misiniz?")
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnLeaveAccept))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.OnLeaveCancel))
		questionDialog.Open()
		self.questionDialog = questionDialog

	def OnLeaveAccept(self):
		net.SendLeaveKingdomPacket()
		self.questionDialog.Close()
		self.questionDialog = None
		self.Close()

	def OnLeaveCancel(self):
		self.questionDialog.Close()
		self.questionDialog = None

	def OnSettings(self):
		"""Krallık ayarları"""
		settingsWindow = KingdomSettingsWindow()
		settingsWindow.SetKingdomInfo(self.kingdomInfo)
		settingsWindow.Open()


class KingdomSettingsWindow(ui.ScriptWindow):
	"""Krallık ayarları penceresi"""
	
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.kingdomInfo = {}
		
		# UI Elements
		self.titleBar = None
		self.nameEditLine = None
		self.descriptionEditLine = None
		self.colorSliders = {}
		self.flagButtons = []
		self.saveButton = None
		self.cancelButton = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/KingdomSettingsWindow.py")
		except:
			# Create UI programmatically if script file doesn't exist
			self.CreateUI()

	def CreateUI(self):
		"""UI'ı programatik olarak oluştur"""
		self.SetSize(400, 500)
		self.SetWindowName("KingdomSettingsWindow")
		
		# Title bar
		self.titleBar = ui.TitleBar()
		self.titleBar.SetParent(self)
		self.titleBar.MakeTitleBar(400, "red")
		self.titleBar.SetPosition(0, 0)
		self.titleBar.SetTitleName("Krallık Ayarları")
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.titleBar.Show()
		
		# Name edit
		nameText = ui.TextLine()
		nameText.SetParent(self)
		nameText.SetPosition(20, 50)
		nameText.SetText("Krallık Adı:")
		nameText.Show()
		
		self.nameEditLine = ui.EditLine()
		self.nameEditLine.SetParent(self)
		self.nameEditLine.SetPosition(20, 70)
		self.nameEditLine.SetSize(200, 20)
		self.nameEditLine.SetMax(20)
		self.nameEditLine.Show()
		
		# Description edit
		descText = ui.TextLine()
		descText.SetParent(self)
		descText.SetPosition(20, 100)
		descText.SetText("Açıklama:")
		descText.Show()
		
		self.descriptionEditLine = ui.EditLine()
		self.descriptionEditLine.SetParent(self)
		self.descriptionEditLine.SetPosition(20, 120)
		self.descriptionEditLine.SetSize(350, 20)
		self.descriptionEditLine.SetMax(100)
		self.descriptionEditLine.Show()
		
		# Color sliders
		colors = ['Red', 'Green', 'Blue']
		for i, color in enumerate(colors):
			colorText = ui.TextLine()
			colorText.SetParent(self)
			colorText.SetPosition(20, 160 + i * 40)
			colorText.SetText(color + ":")
			colorText.Show()
			
			slider = ui.SliderBar()
			slider.SetParent(self)
			slider.SetPosition(80, 160 + i * 40)
			slider.SetSize(200, 20)
			slider.Show()
			self.colorSliders[color.lower()] = slider
		
		# Save button
		self.saveButton = ui.Button()
		self.saveButton.SetParent(self)
		self.saveButton.SetPosition(100, 400)
		self.saveButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.saveButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.saveButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.saveButton.SetText("Kaydet")
		self.saveButton.SetEvent(ui.__mem_func__(self.OnSave))
		self.saveButton.Show()
		
		# Cancel button
		self.cancelButton = ui.Button()
		self.cancelButton.SetParent(self)
		self.cancelButton.SetPosition(200, 400)
		self.cancelButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.cancelButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.cancelButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.cancelButton.SetText("İptal")
		self.cancelButton.SetEvent(ui.__mem_func__(self.Close))
		self.cancelButton.Show()

	def SetKingdomInfo(self, kingdomInfo):
		"""Krallık bilgilerini ayarla"""
		self.kingdomInfo = kingdomInfo
		
		if self.nameEditLine:
			self.nameEditLine.SetText(kingdomInfo.get('name', ''))
		if self.descriptionEditLine:
			self.descriptionEditLine.SetText(kingdomInfo.get('description', ''))
		
		# Set color sliders
		color = kingdomInfo.get('color', [255, 255, 255])
		if 'red' in self.colorSliders:
			self.colorSliders['red'].SetSliderPos(color[0] / 255.0)
		if 'green' in self.colorSliders:
			self.colorSliders['green'].SetSliderPos(color[1] / 255.0)
		if 'blue' in self.colorSliders:
			self.colorSliders['blue'].SetSliderPos(color[2] / 255.0)

	def Open(self):
		if self.isLoaded == 0:
			self.LoadWindow()

		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def OnSave(self):
		"""Ayarları kaydet"""
		settings = {
			'name': self.nameEditLine.GetText(),
			'description': self.descriptionEditLine.GetText(),
			'color': [
				int(self.colorSliders['red'].GetSliderPos() * 255),
				int(self.colorSliders['green'].GetSliderPos() * 255),
				int(self.colorSliders['blue'].GetSliderPos() * 255)
			],
			'flag': self.kingdomInfo.get('flag', 0)
		}
		
		net.SendKingdomSettingsPacket(settings)
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Krallık ayarları güncellendi.")
		self.Close()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE
