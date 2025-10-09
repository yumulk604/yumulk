# -*- coding: utf-8 -*-
# Offline Shop UI System - Krallık sistemi ile entegre çalışan offline shop arayüzü

import ui
import uiCommon
import chat
import net
import player
import item
import localeInfo
import constInfo
import ime
import app
import grp
import wndMgr
import mouseModule
import time

class OfflineShopCreateWindow(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        
        self.isLoaded = False
        self.shopNameEditLine = None
        self.shopDescEditLine = None
        self.createButton = None
        self.cancelButton = None
        
    def __del__(self):
        ui.ScriptWindow.__del__(self)
        
    def LoadWindow(self):
        if self.isLoaded:
            return
            
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/OfflineShopCreateWindow.py")
        except:
            import exception
            exception.Abort("OfflineShopCreateWindow.LoadWindow.LoadObject")
            
        try:
            self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
            self.shopNameEditLine = self.GetChild("ShopNameEditLine")
            self.shopDescEditLine = self.GetChild("ShopDescEditLine")
            self.createButton = self.GetChild("CreateButton")
            self.cancelButton = self.GetChild("CancelButton")
            
            self.createButton.SetEvent(ui.__mem_func__(self.OnCreateShop))
            self.cancelButton.SetEvent(ui.__mem_func__(self.Close))
            
        except:
            import exception
            exception.Abort("OfflineShopCreateWindow.LoadWindow.BindObject")
            
        self.isLoaded = True
        
    def Open(self):
        if not self.isLoaded:
            self.LoadWindow()
            
        self.SetCenterPosition()
        self.SetTop()
        self.Show()
        
        if self.shopNameEditLine:
            self.shopNameEditLine.SetFocus()
            
    def Close(self):
        if self.shopNameEditLine:
            self.shopNameEditLine.SetText("")
        if self.shopDescEditLine:
            self.shopDescEditLine.SetText("")
            
        self.Hide()
        
    def OnCreateShop(self):
        if not self.shopNameEditLine or not self.shopDescEditLine:
            return
            
        shopName = self.shopNameEditLine.GetText()
        shopDesc = self.shopDescEditLine.GetText()
        
        if len(shopName) < 3:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Shop adı en az 3 karakter olmalıdır.")
            return
            
        if len(shopName) > 32:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Shop adı en fazla 32 karakter olabilir.")
            return
            
        if len(shopDesc) > 128:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Shop açıklaması en fazla 128 karakter olabilir.")
            return
            
        # Krallık bilgisi al
        kingdomID = player.GetKingdomID()
        if kingdomID == 0:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Offline shop açmak için bir krallığa üye olmalısınız.")
            return
            
        # Pozisyon bilgisi al
        x, y, z = player.GetMainCharacterPosition()
        mapIndex = background.GetMapName()
        
        # Server'a shop oluşturma isteği gönder
        net.SendOfflineShopCreatePacket(shopName, shopDesc, int(x), int(y))
        
        self.Close()
        
class OfflineShopManagementWindow(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        
        self.isLoaded = False
        self.shopID = 0
        self.shopInfo = {}
        self.itemSlots = []
        self.inventorySlots = []
        
        self.shopNameText = None
        self.shopDescText = None
        self.shopStatusText = None
        self.totalEarnedText = None
        
        self.openShopButton = None
        self.closeShopButton = None
        self.withdrawButton = None
        
    def __del__(self):
        ui.ScriptWindow.__del__(self)
        
    def LoadWindow(self):
        if self.isLoaded:
            return
            
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/OfflineShopManagementWindow.py")
        except:
            import exception
            exception.Abort("OfflineShopManagementWindow.LoadWindow.LoadObject")
            
        try:
            self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
            
            # Shop bilgi alanları
            self.shopNameText = self.GetChild("ShopNameText")
            self.shopDescText = self.GetChild("ShopDescText")
            self.shopStatusText = self.GetChild("ShopStatusText")
            self.totalEarnedText = self.GetChild("TotalEarnedText")
            
            # Butonlar
            self.openShopButton = self.GetChild("OpenShopButton")
            self.closeShopButton = self.GetChild("CloseShopButton")
            self.withdrawButton = self.GetChild("WithdrawButton")
            
            self.openShopButton.SetEvent(ui.__mem_func__(self.OnOpenShop))
            self.closeShopButton.SetEvent(ui.__mem_func__(self.OnCloseShop))
            self.withdrawButton.SetEvent(ui.__mem_func__(self.OnWithdrawEarnings))
            
            # Item slotları
            self.itemSlots = []
            for i in range(40):  # 40 slot
                slot = self.GetChild("ItemSlot_%d" % i)
                if slot:
                    slot.SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectEmptySlot))
                    slot.SetSelectItemSlotEvent(ui.__mem_func__(self.OnSelectItemSlot))
                    slot.SetOverInItemEvent(ui.__mem_func__(self.OnOverInItem))
                    slot.SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))
                    self.itemSlots.append(slot)
                    
        except:
            import exception
            exception.Abort("OfflineShopManagementWindow.LoadWindow.BindObject")
            
        self.isLoaded = True
        
    def Open(self, shopID):
        if not self.isLoaded:
            self.LoadWindow()
            
        self.shopID = shopID
        self.RefreshShopInfo()
        
        self.SetCenterPosition()
        self.SetTop()
        self.Show()
        
    def Close(self):
        self.shopID = 0
        self.shopInfo = {}
        self.Hide()
        
    def RefreshShopInfo(self):
        if self.shopID == 0:
            return
            
        # Server'dan shop bilgilerini iste
        net.SendOfflineShopInfoRequestPacket(self.shopID)
        
    def SetShopInfo(self, shopInfo):
        self.shopInfo = shopInfo
        
        if self.shopNameText:
            self.shopNameText.SetText(shopInfo.get("name", ""))
        if self.shopDescText:
            self.shopDescText.SetText(shopInfo.get("desc", ""))
        if self.shopStatusText:
            status = "Açık" if shopInfo.get("isOpen", False) else "Kapalı"
            self.shopStatusText.SetText(status)
        if self.totalEarnedText:
            self.totalEarnedText.SetText(localeInfo.NumberToMoneyString(shopInfo.get("totalEarned", 0)))
            
        # Buton durumlarını güncelle
        if self.openShopButton and self.closeShopButton:
            if shopInfo.get("isOpen", False):
                self.openShopButton.Hide()
                self.closeShopButton.Show()
            else:
                self.openShopButton.Show()
                self.closeShopButton.Hide()
                
    def SetShopItems(self, itemList):
        # Önce tüm slotları temizle
        for slot in self.itemSlots:
            slot.ClearSlot(0)
            
        # Yeni itemları yerleştir
        for itemInfo in itemList:
            slotIndex = itemInfo.get("slot", 0)
            if slotIndex < len(self.itemSlots):
                slot = self.itemSlots[slotIndex]
                slot.SetItemSlot(0, itemInfo.get("vnum", 0), itemInfo.get("count", 1))
                
    def OnSelectEmptySlot(self, slotIndex):
        # Boş slota item ekleme - inventory'den item seç
        self.OpenInventoryItemSelect(slotIndex)
        
    def OnSelectItemSlot(self, slotIndex):
        # Var olan itemi kaldır veya fiyat değiştir
        self.ShowItemContextMenu(slotIndex)
        
    def OnOverInItem(self, slotIndex):
        # Tooltip göster
        pass
        
    def OnOverOutItem(self):
        # Tooltip gizle
        pass
        
    def OnOpenShop(self):
        if self.shopID > 0:
            net.SendOfflineShopOpenPacket(self.shopID)
            
    def OnCloseShop(self):
        if self.shopID > 0:
            net.SendOfflineShopClosePacket(self.shopID)
            
    def OnWithdrawEarnings(self):
        if self.shopID > 0:
            earnings = self.shopInfo.get("totalEarned", 0)
            if earnings > 0:
                net.SendOfflineShopWithdrawPacket(self.shopID, earnings)
                
class OfflineShopViewWindow(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        
        self.isLoaded = False
        self.shopID = 0
        self.shopInfo = {}
        self.itemSlots = []
        
        self.shopNameText = None
        self.shopOwnerText = None
        self.shopDescText = None
        
    def __del__(self):
        ui.ScriptWindow.__del__(self)
        
    def LoadWindow(self):
        if self.isLoaded:
            return
            
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/OfflineShopViewWindow.py")
        except:
            import exception
            exception.Abort("OfflineShopViewWindow.LoadWindow.LoadObject")
            
        try:
            self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
            
            self.shopNameText = self.GetChild("ShopNameText")
            self.shopOwnerText = self.GetChild("ShopOwnerText")
            self.shopDescText = self.GetChild("ShopDescText")
            
            # Item slotları
            self.itemSlots = []
            for i in range(40):
                slot = self.GetChild("ItemSlot_%d" % i)
                if slot:
                    slot.SetSelectItemSlotEvent(ui.__mem_func__(self.OnSelectItemToBuy))
                    slot.SetOverInItemEvent(ui.__mem_func__(self.OnOverInItem))
                    slot.SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))
                    self.itemSlots.append(slot)
                    
        except:
            import exception
            exception.Abort("OfflineShopViewWindow.LoadWindow.BindObject")
            
        self.isLoaded = True
        
    def Open(self, shopID):
        if not self.isLoaded:
            self.LoadWindow()
            
        self.shopID = shopID
        net.SendOfflineShopViewRequestPacket(shopID)
        
        self.SetCenterPosition()
        self.SetTop()
        self.Show()
        
    def Close(self):
        self.shopID = 0
        self.shopInfo = {}
        self.Hide()
        
    def SetShopInfo(self, shopInfo):
        self.shopInfo = shopInfo
        
        if self.shopNameText:
            self.shopNameText.SetText(shopInfo.get("name", ""))
        if self.shopOwnerText:
            self.shopOwnerText.SetText("Sahip: " + shopInfo.get("ownerName", ""))
        if self.shopDescText:
            self.shopDescText.SetText(shopInfo.get("desc", ""))
            
    def SetShopItems(self, itemList):
        # Tüm slotları temizle
        for slot in self.itemSlots:
            slot.ClearSlot(0)
            
        # Itemları yerleştir
        for itemInfo in itemList:
            slotIndex = itemInfo.get("slot", 0)
            if slotIndex < len(self.itemSlots):
                slot = self.itemSlots[slotIndex]
                slot.SetItemSlot(0, itemInfo.get("vnum", 0), itemInfo.get("count", 1))
                
    def OnSelectItemToBuy(self, slotIndex):
        # Item satın alma onayı
        self.ShowBuyConfirmation(slotIndex)
        
    def ShowBuyConfirmation(self, slotIndex):
        # Satın alma onay penceresi göster
        pass
        
    def OnOverInItem(self, slotIndex):
        # Tooltip ve fiyat bilgisi göster
        pass
        
    def OnOverOutItem(self):
        # Tooltip gizle
        pass
        
class OfflineShopSearchWindow(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        
        self.isLoaded = False
        self.searchEditLine = None
        self.searchButton = None
        self.resultListBox = None
        
    def __del__(self):
        ui.ScriptWindow.__del__(self)
        
    def LoadWindow(self):
        if self.isLoaded:
            return
            
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/OfflineShopSearchWindow.py")
        except:
            import exception
            exception.Abort("OfflineShopSearchWindow.LoadWindow.LoadObject")
            
        try:
            self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
            
            self.searchEditLine = self.GetChild("SearchEditLine")
            self.searchButton = self.GetChild("SearchButton")
            self.resultListBox = self.GetChild("ResultListBox")
            
            self.searchButton.SetEvent(ui.__mem_func__(self.OnSearch))
            
        except:
            import exception
            exception.Abort("OfflineShopSearchWindow.LoadWindow.BindObject")
            
        self.isLoaded = True
        
    def Open(self):
        if not self.isLoaded:
            self.LoadWindow()
            
        self.SetCenterPosition()
        self.SetTop()
        self.Show()
        
        if self.searchEditLine:
            self.searchEditLine.SetFocus()
            
    def Close(self):
        if self.searchEditLine:
            self.searchEditLine.SetText("")
        if self.resultListBox:
            self.resultListBox.RemoveAllItems()
            
        self.Hide()
        
    def OnSearch(self):
        if not self.searchEditLine:
            return
            
        keyword = self.searchEditLine.GetText()
        if len(keyword) < 2:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Arama için en az 2 karakter giriniz.")
            return
            
        # Server'a arama isteği gönder
        net.SendOfflineShopSearchPacket(keyword)
        
    def SetSearchResults(self, results):
        if not self.resultListBox:
            return
            
        self.resultListBox.RemoveAllItems()
        
        for shop in results:
            item = ui.ListBoxEx.Item()
            item.SetText(shop.get("name", ""))
            item.SetValue(shop.get("shopID", 0))
            self.resultListBox.AppendItem(item)
            
# Network fonksiyonları
def SendOfflineShopCreatePacket(name, desc, x, y):
    """Offline shop oluşturma paketi gönder"""
    pass
    
def SendOfflineShopOpenPacket(shopID):
    """Shop açma paketi gönder"""
    pass
    
def SendOfflineShopClosePacket(shopID):
    """Shop kapatma paketi gönder"""
    pass
    
def SendOfflineShopViewRequestPacket(shopID):
    """Shop görüntüleme isteği paketi gönder"""
    pass
    
def SendOfflineShopSearchPacket(keyword):
    """Shop arama paketi gönder"""
    pass
    
# Global değişkenler
g_offlineShopCreateWindow = None
g_offlineShopManagementWindow = None
g_offlineShopViewWindow = None
g_offlineShopSearchWindow = None

def GetOfflineShopCreateWindow():
    global g_offlineShopCreateWindow
    if not g_offlineShopCreateWindow:
        g_offlineShopCreateWindow = OfflineShopCreateWindow()
    return g_offlineShopCreateWindow
    
def GetOfflineShopManagementWindow():
    global g_offlineShopManagementWindow
    if not g_offlineShopManagementWindow:
        g_offlineShopManagementWindow = OfflineShopManagementWindow()
    return g_offlineShopManagementWindow
    
def GetOfflineShopViewWindow():
    global g_offlineShopViewWindow
    if not g_offlineShopViewWindow:
        g_offlineShopViewWindow = OfflineShopViewWindow()
    return g_offlineShopViewWindow
    
def GetOfflineShopSearchWindow():
    global g_offlineShopSearchWindow
    if not g_offlineShopSearchWindow:
        g_offlineShopSearchWindow = OfflineShopSearchWindow()
    return g_offlineShopSearchWindow