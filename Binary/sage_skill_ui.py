# -*- coding: utf-8 -*-
# Sage Skill System UI - Perfect seviyesinden sonra gelen Sage skill sistemi arayüzü
# Normal -> Master -> Grand Master -> Perfect -> SAGE (S1-S10)

import ui
import uiCommon
import chat
import net
import player
import skill
import localeInfo
import constInfo
import ime
import app
import grp
import wndMgr
import mouseModule
import time
import uiToolTip

# Sage Skill Constants
SAGE_SKILL_GRADE_NONE = 0
SAGE_SKILL_GRADE_S1 = 1
SAGE_SKILL_GRADE_S2 = 2
SAGE_SKILL_GRADE_S3 = 3
SAGE_SKILL_GRADE_S4 = 4
SAGE_SKILL_GRADE_S5 = 5
SAGE_SKILL_GRADE_S6 = 6
SAGE_SKILL_GRADE_S7 = 7
SAGE_SKILL_GRADE_S8 = 8
SAGE_SKILL_GRADE_S9 = 9
SAGE_SKILL_GRADE_S10 = 10

SAGE_UPGRADE_ANCIENT_SCROLL = 1
SAGE_UPGRADE_SAGE_STONE = 2
SAGE_UPGRADE_MEDITATION = 3

# Sage Skill Items
SAGE_ITEM_ANCIENT_SCROLL_WARRIOR = 90001
SAGE_ITEM_ANCIENT_SCROLL_NINJA = 90002
SAGE_ITEM_ANCIENT_SCROLL_SURA = 90003
SAGE_ITEM_ANCIENT_SCROLL_SHAMAN = 90004
SAGE_ITEM_ANCIENT_SCROLL_WOLFMAN = 90005

SAGE_ITEM_SAGE_STONE_LOW = 90010
SAGE_ITEM_SAGE_STONE_MID = 90011
SAGE_ITEM_SAGE_STONE_HIGH = 90012
SAGE_ITEM_SAGE_STONE_SUPREME = 90013

class SageSkillUpgradeWindow(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        
        self.isLoaded = False
        self.skillVnum = 0
        self.currentGrade = 0
        self.upgradeType = SAGE_UPGRADE_ANCIENT_SCROLL
        
        # UI Elements
        self.skillNameText = None
        self.currentGradeText = None
        self.targetGradeText = None
        self.successRateText = None
        self.bonusInfoText = None
        
        self.upgradeTypeButtons = []
        self.upgradeButton = None
        self.cancelButton = None
        
        self.itemSlots = []
        self.requiredItems = {}
        
    def __del__(self):
        ui.ScriptWindow.__del__(self)
        
    def LoadWindow(self):
        if self.isLoaded:
            return
            
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/SageSkillUpgradeWindow.py")
        except:
            import exception
            exception.Abort("SageSkillUpgradeWindow.LoadWindow.LoadObject")
            
        try:
            self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
            
            # Skill bilgi alanları
            self.skillNameText = self.GetChild("SkillNameText")
            self.currentGradeText = self.GetChild("CurrentGradeText")
            self.targetGradeText = self.GetChild("TargetGradeText")
            self.successRateText = self.GetChild("SuccessRateText")
            self.bonusInfoText = self.GetChild("BonusInfoText")
            
            # Yükseltme tipi butonları
            self.upgradeTypeButtons.append(self.GetChild("AncientScrollButton"))
            self.upgradeTypeButtons.append(self.GetChild("SageStoneButton"))
            self.upgradeTypeButtons.append(self.GetChild("MeditationButton"))
            
            for i, button in enumerate(self.upgradeTypeButtons):
                button.SetEvent(ui.__mem_func__(self.OnSelectUpgradeType), i + 1)
                
            # Ana butonlar
            self.upgradeButton = self.GetChild("UpgradeButton")
            self.cancelButton = self.GetChild("CancelButton")
            
            self.upgradeButton.SetEvent(ui.__mem_func__(self.OnUpgradeSkill))
            self.cancelButton.SetEvent(ui.__mem_func__(self.Close))
            
            # Item slotları
            for i in range(5):
                slot = self.GetChild("RequiredItem_%d" % i)
                if slot:
                    self.itemSlots.append(slot)
                    
        except:
            import exception
            exception.Abort("SageSkillUpgradeWindow.LoadWindow.BindObject")
            
        self.isLoaded = True
        
    def Open(self, skillVnum):
        if not self.isLoaded:
            self.LoadWindow()
            
        self.skillVnum = skillVnum
        self.RefreshSkillInfo()
        
        self.SetCenterPosition()
        self.SetTop()
        self.Show()
        
    def Close(self):
        self.skillVnum = 0
        self.currentGrade = 0
        self.upgradeType = SAGE_UPGRADE_ANCIENT_SCROLL
        self.Hide()
        
    def RefreshSkillInfo(self):
        if self.skillVnum == 0:
            return
            
        # Server'dan skill bilgilerini iste
        net.SendSageSkillInfoRequestPacket(self.skillVnum)
        
    def SetSkillInfo(self, skillInfo):
        if not skillInfo:
            return
            
        self.currentGrade = skillInfo.get("sageGrade", 0)
        
        if self.skillNameText:
            skillName = skill.GetSkillName(self.skillVnum)
            self.skillNameText.SetText(skillName)
            
        if self.currentGradeText:
            if self.currentGrade == 0:
                self.currentGradeText.SetText("Perfect")
            else:
                self.currentGradeText.SetText("S%d" % self.currentGrade)
                
        if self.targetGradeText:
            targetGrade = min(self.currentGrade + 1, SAGE_SKILL_GRADE_S10)
            if self.currentGrade == 0:
                self.targetGradeText.SetText("S1")
            else:
                self.targetGradeText.SetText("S%d" % targetGrade)
                
        self.UpdateSuccessRate()
        self.UpdateBonusInfo()
        self.UpdateRequiredItems()
        
    def OnSelectUpgradeType(self, upgradeType):
        self.upgradeType = upgradeType
        
        # Buton görsellerini güncelle
        for i, button in enumerate(self.upgradeTypeButtons):
            if i + 1 == upgradeType:
                button.SetUp()
                button.Down()
            else:
                button.SetUp()
                
        self.UpdateSuccessRate()
        self.UpdateRequiredItems()
        
    def UpdateSuccessRate(self):
        if not self.successRateText:
            return
            
        # Başarı oranını hesapla (basitçe)
        baseRate = 0.0
        
        if self.upgradeType == SAGE_UPGRADE_ANCIENT_SCROLL:
            baseRate = 0.30 - (self.currentGrade * 0.02)
        elif self.upgradeType == SAGE_UPGRADE_SAGE_STONE:
            baseRate = 0.50 - (self.currentGrade * 0.03)
        elif self.upgradeType == SAGE_UPGRADE_MEDITATION:
            baseRate = 1.0
            
        # Level bonusu ekle
        levelBonus = (player.GetStatus(player.LEVEL) - 90) * 0.001
        finalRate = max(0.05, min(0.95, baseRate + levelBonus))
        
        if self.upgradeType == SAGE_UPGRADE_MEDITATION:
            self.successRateText.SetText("Garantili (30 dakika meditasyon)")
        else:
            self.successRateText.SetText("Başarı Oranı: %.1f%%" % (finalRate * 100))
            
    def UpdateBonusInfo(self):
        if not self.bonusInfoText:
            return
            
        targetGrade = min(self.currentGrade + 1, SAGE_SKILL_GRADE_S10)
        
        # Hedef seviyedeki bonusları göster
        damageBonus = targetGrade * 5  # %5 per level
        cooldownReduction = targetGrade * 2  # %2 per level
        manaReduction = int(targetGrade * 1.5)  # %1.5 per level
        
        bonusText = "S%d Bonusları:\n" % targetGrade
        bonusText += "Hasar: +%d%%\n" % damageBonus
        bonusText += "Cooldown: -%d%%\n" % cooldownReduction
        bonusText += "Mana: -%d%%" % manaReduction
        
        # Özel yetenek kontrolü
        specialAbility = self.GetSpecialAbilityForGrade(targetGrade)
        if specialAbility:
            bonusText += "\n\nÖzel Yetenek:\n%s" % specialAbility
            
        self.bonusInfoText.SetText(bonusText)
        
    def GetSpecialAbilityForGrade(self, grade):
        abilities = {
            3: "Zincir Şimşek",
            5: "Çift Büyü",
            7: "Mana Kalkanı",
            9: "Zaman Manipülasyonu",
            10: "Gerçeklik Yırtığı"
        }
        return abilities.get(grade, None)
        
    def UpdateRequiredItems(self):
        # Gerekli itemleri temizle
        for slot in self.itemSlots:
            slot.ClearSlot(0)
            
        self.requiredItems.clear()
        
        if self.upgradeType == SAGE_UPGRADE_ANCIENT_SCROLL:
            # Class'a göre antik rulo
            job = player.GetJob()
            scrollVnum = 0
            
            if job == 0:    # Savaşçı
                scrollVnum = SAGE_ITEM_ANCIENT_SCROLL_WARRIOR
            elif job == 1:  # Ninja
                scrollVnum = SAGE_ITEM_ANCIENT_SCROLL_NINJA
            elif job == 2:  # Sura
                scrollVnum = SAGE_ITEM_ANCIENT_SCROLL_SURA
            elif job == 3:  # Şaman
                scrollVnum = SAGE_ITEM_ANCIENT_SCROLL_SHAMAN
            elif job == 4:  # Kurt Adam
                scrollVnum = SAGE_ITEM_ANCIENT_SCROLL_WOLFMAN
                
            if scrollVnum > 0:
                self.requiredItems[scrollVnum] = 1
                self.itemSlots[0].SetItemSlot(0, scrollVnum, 1)
                
        elif self.upgradeType == SAGE_UPGRADE_SAGE_STONE:
            # Grade'e göre bilge taşı
            stoneVnum = 0
            
            if self.currentGrade <= 3:
                stoneVnum = SAGE_ITEM_SAGE_STONE_LOW
            elif self.currentGrade <= 6:
                stoneVnum = SAGE_ITEM_SAGE_STONE_MID
            elif self.currentGrade <= 9:
                stoneVnum = SAGE_ITEM_SAGE_STONE_HIGH
            else:
                stoneVnum = SAGE_ITEM_SAGE_STONE_SUPREME
                
            if stoneVnum > 0:
                self.requiredItems[stoneVnum] = 1
                self.itemSlots[0].SetItemSlot(0, stoneVnum, 1)
                
        elif self.upgradeType == SAGE_UPGRADE_MEDITATION:
            # Meditasyon item gerektirmez
            pass
            
    def OnUpgradeSkill(self):
        if self.skillVnum == 0:
            return
            
        # Gerekli itemlerin kontrolü
        if not self.CheckRequiredItems():
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Gerekli itemler eksik!")
            return
            
        # Meditasyon özel durumu
        if self.upgradeType == SAGE_UPGRADE_MEDITATION:
            self.StartMeditation()
            return
            
        # Normal yükseltme isteği gönder
        net.SendSageSkillUpgradePacket(self.skillVnum, self.upgradeType)
        self.Close()
        
    def CheckRequiredItems(self):
        if self.upgradeType == SAGE_UPGRADE_MEDITATION:
            return True
            
        for itemVnum, count in self.requiredItems.items():
            if player.GetItemCountByVnum(itemVnum) < count:
                return False
                
        return True
        
    def StartMeditation(self):
        # Meditasyon başlatma penceresi aç
        meditationWindow = GetSageMeditationWindow()
        if meditationWindow:
            meditationWindow.Open(self.skillVnum)
        self.Close()
        
class SageMeditationWindow(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        
        self.isLoaded = False
        self.skillVnum = 0
        self.startTime = 0
        self.duration = 1800  # 30 dakika
        self.isActive = False
        
        # UI Elements
        self.skillNameText = None
        self.progressBar = None
        self.timeRemainingText = None
        self.instructionText = None
        self.startButton = None
        self.cancelButton = None
        
    def __del__(self):
        ui.ScriptWindow.__del__(self)
        
    def LoadWindow(self):
        if self.isLoaded:
            return
            
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/SageMeditationWindow.py")
        except:
            import exception
            exception.Abort("SageMeditationWindow.LoadWindow.LoadObject")
            
        try:
            self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
            
            self.skillNameText = self.GetChild("SkillNameText")
            self.progressBar = self.GetChild("ProgressBar")
            self.timeRemainingText = self.GetChild("TimeRemainingText")
            self.instructionText = self.GetChild("InstructionText")
            self.startButton = self.GetChild("StartButton")
            self.cancelButton = self.GetChild("CancelButton")
            
            self.startButton.SetEvent(ui.__mem_func__(self.OnStartMeditation))
            self.cancelButton.SetEvent(ui.__mem_func__(self.OnCancelMeditation))
            
        except:
            import exception
            exception.Abort("SageMeditationWindow.LoadWindow.BindObject")
            
        self.isLoaded = True
        
    def Open(self, skillVnum):
        if not self.isLoaded:
            self.LoadWindow()
            
        self.skillVnum = skillVnum
        self.RefreshWindow()
        
        self.SetCenterPosition()
        self.SetTop()
        self.Show()
        
    def Close(self):
        if self.isActive:
            self.OnCancelMeditation()
        self.Hide()
        
    def RefreshWindow(self):
        if self.skillNameText:
            skillName = skill.GetSkillName(self.skillVnum)
            self.skillNameText.SetText("%s Meditasyonu" % skillName)
            
        if self.instructionText:
            self.instructionText.SetText(
                "Meditasyon 30 dakika sürer ve garantili yükseltme sağlar.\n"
                "Meditasyon sırasında hareket edemez, saldırı yapamazsınız.\n"
                "Saldırıya uğrarsanz meditasyon kesilir.")
                
        if self.progressBar:
            self.progressBar.SetPercentage(0, 100)
            
        if self.timeRemainingText:
            self.timeRemainingText.SetText("30:00")
            
    def OnStartMeditation(self):
        if self.isActive:
            return
            
        # Server'a meditasyon başlatma isteği gönder
        net.SendSageMeditationStartPacket(self.skillVnum)
        
        self.isActive = True
        self.startTime = app.GetTime()
        
        if self.startButton:
            self.startButton.Hide()
        if self.cancelButton:
            self.cancelButton.SetText("İptal Et")
            
        chat.AppendChat(chat.CHAT_TYPE_INFO, "Meditasyon başlatıldı. 30 dakika boyunca hareket etmeyin!")
        
    def OnCancelMeditation(self):
        if not self.isActive:
            self.Hide()
            return
            
        # Server'a meditasyon iptal isteği gönder
        net.SendSageMeditationCancelPacket(self.skillVnum)
        
        self.isActive = False
        self.startTime = 0
        
        if self.startButton:
            self.startButton.Show()
        if self.cancelButton:
            self.cancelButton.SetText("Kapat")
            
        chat.AppendChat(chat.CHAT_TYPE_INFO, "Meditasyon iptal edildi.")
        self.Hide()
        
    def OnUpdate(self):
        if not self.isActive:
            return
            
        currentTime = app.GetTime()
        elapsedTime = currentTime - self.startTime
        
        if elapsedTime >= self.duration:
            # Meditasyon tamamlandı
            self.OnMeditationComplete()
            return
            
        # Progress bar güncelle
        progress = (elapsedTime * 100) / self.duration
        if self.progressBar:
            self.progressBar.SetPercentage(int(progress), 100)
            
        # Kalan zamanı güncelle
        remainingTime = self.duration - elapsedTime
        minutes = int(remainingTime / 60)
        seconds = int(remainingTime % 60)
        
        if self.timeRemainingText:
            self.timeRemainingText.SetText("%02d:%02d" % (minutes, seconds))
            
    def OnMeditationComplete(self):
        # Server'a meditasyon tamamlanma isteği gönder
        net.SendSageMeditationCompletePacket(self.skillVnum)
        
        self.isActive = False
        self.startTime = 0
        
        chat.AppendChat(chat.CHAT_TYPE_INFO, "Meditasyon tamamlandı! Skill başarıyla yükseltildi.")
        self.Hide()
        
class SageSkillInfoWindow(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        
        self.isLoaded = False
        self.skillVnum = 0
        
        # UI Elements
        self.skillNameText = None
        self.gradeText = None
        self.bonusListBox = None
        self.specialAbilitiesListBox = None
        self.statisticsText = None
        
    def __del__(self):
        ui.ScriptWindow.__del__(self)
        
    def LoadWindow(self):
        if self.isLoaded:
            return
            
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/SageSkillInfoWindow.py")
        except:
            import exception
            exception.Abort("SageSkillInfoWindow.LoadWindow.LoadObject")
            
        try:
            self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
            
            self.skillNameText = self.GetChild("SkillNameText")
            self.gradeText = self.GetChild("GradeText")
            self.bonusListBox = self.GetChild("BonusListBox")
            self.specialAbilitiesListBox = self.GetChild("SpecialAbilitiesListBox")
            self.statisticsText = self.GetChild("StatisticsText")
            
        except:
            import exception
            exception.Abort("SageSkillInfoWindow.LoadWindow.BindObject")
            
        self.isLoaded = True
        
    def Open(self, skillVnum):
        if not self.isLoaded:
            self.LoadWindow()
            
        self.skillVnum = skillVnum
        net.SendSageSkillInfoRequestPacket(skillVnum)
        
        self.SetCenterPosition()
        self.SetTop()
        self.Show()
        
    def Close(self):
        self.skillVnum = 0
        self.Hide()
        
    def SetSkillInfo(self, skillInfo):
        if not skillInfo:
            return
            
        sageGrade = skillInfo.get("sageGrade", 0)
        
        if self.skillNameText:
            skillName = skill.GetSkillName(self.skillVnum)
            self.skillNameText.SetText(skillName)
            
        if self.gradeText:
            if sageGrade == 0:
                self.gradeText.SetText("Perfect")
            else:
                self.gradeText.SetText("Sage S%d" % sageGrade)
                
        # Bonus bilgilerini güncelle
        if self.bonusListBox and sageGrade > 0:
            self.bonusListBox.RemoveAllItems()
            
            damageBonus = skillInfo.get("damageBonus", 0)
            cooldownReduction = skillInfo.get("cooldownReduction", 0)
            manaReduction = skillInfo.get("manaReduction", 0)
            
            self.bonusListBox.AppendTextLine("Hasar Bonusu: +%.1f%%" % (damageBonus * 100))
            self.bonusListBox.AppendTextLine("Cooldown Azaltma: -%.1f%%" % (cooldownReduction * 100))
            self.bonusListBox.AppendTextLine("Mana Azaltma: -%.1f%%" % (manaReduction * 100))
            
        # İstatistik bilgilerini güncelle
        if self.statisticsText:
            attempts = skillInfo.get("totalAttempts", 0)
            successes = skillInfo.get("successfulUpgrades", 0)
            successRate = (successes * 100.0 / max(attempts, 1)) if attempts > 0 else 0
            
            statText = "Toplam Deneme: %d\n" % attempts
            statText += "Başarılı Yükseltme: %d\n" % successes
            statText += "Başarı Oranı: %.1f%%" % successRate
            
            self.statisticsText.SetText(statText)
            
# Network fonksiyonları
def SendSageSkillUpgradePacket(skillVnum, upgradeType):
    """Sage skill yükseltme paketi gönder"""
    pass
    
def SendSageSkillInfoRequestPacket(skillVnum):
    """Sage skill bilgi isteği paketi gönder"""
    pass
    
def SendSageMeditationStartPacket(skillVnum):
    """Meditasyon başlatma paketi gönder"""
    pass
    
def SendSageMeditationCompletePacket(skillVnum):
    """Meditasyon tamamlama paketi gönder"""
    pass
    
def SendSageMeditationCancelPacket(skillVnum):
    """Meditasyon iptal paketi gönder"""
    pass

# Global değişkenler
g_sageSkillUpgradeWindow = None
g_sageMeditationWindow = None
g_sageSkillInfoWindow = None

def GetSageSkillUpgradeWindow():
    global g_sageSkillUpgradeWindow
    if not g_sageSkillUpgradeWindow:
        g_sageSkillUpgradeWindow = SageSkillUpgradeWindow()
    return g_sageSkillUpgradeWindow
    
def GetSageMeditationWindow():
    global g_sageMeditationWindow
    if not g_sageMeditationWindow:
        g_sageMeditationWindow = SageMeditationWindow()
    return g_sageMeditationWindow
    
def GetSageSkillInfoWindow():
    global g_sageSkillInfoWindow
    if not g_sageSkillInfoWindow:
        g_sageSkillInfoWindow = SageSkillInfoWindow()
    return g_sageSkillInfoWindow

# Skill tooltip extension
def GetSageSkillTooltipText(skillVnum):
    """Sage skill için tooltip metni oluştur"""
    # Server'dan sage skill bilgilerini al ve tooltip metnini döndür
    # Bu fonksiyon skill tooltip sistemine entegre edilecek
    return ""

# Skill grade display extension
def GetSageSkillGradeText(skillVnum):
    """Sage skill grade metnini döndür"""
    # Bu fonksiyon skill grade gösteriminde kullanılacak
    return "S1"  # Örnek

# Chat command integration
def ProcessSageSkillCommand(command, args):
    """Sage skill chat komutlarını işle"""
    if command == "sage_info":
        try:
            skillVnum = int(args[0])
            window = GetSageSkillInfoWindow()
            window.Open(skillVnum)
        except:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Kullanım: /sage_info [skill_vnum]")
    elif command == "sage_upgrade":
        try:
            skillVnum = int(args[0])
            window = GetSageSkillUpgradeWindow()
            window.Open(skillVnum)
        except:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Kullanım: /sage_upgrade [skill_vnum]")