# Kingdom Teleportation System
# Bu dosya game.py'ye entegre edilecek teleportasyon fonksiyonlarını içerir

import ui
import net
import player
import chat
import app

class KingdomTeleportWindow(ui.ScriptWindow):
    """Krallık teleportasyon penceresi"""
    
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.isLoaded = 0
        
        # Kingdom list
        self.kingdomList = []
        
        # UI Elements
        self.titleBar = None
        self.board = None
        self.kingdomListBox = None
        self.teleportButton = None
        self.homeButton = None
        self.closeButton = None
        self.infoText = None

    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def LoadWindow(self):
        if self.isLoaded == 1:
            return

        self.isLoaded = 1

        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/KingdomTeleportWindow.py")
        except:
            # Create UI programmatically if script doesn't exist
            self.CreateUI()

    def CreateUI(self):
        """UI'ı programatik olarak oluştur"""
        self.SetSize(400, 500)
        self.SetWindowName("KingdomTeleportWindow")
        
        # Title bar
        self.titleBar = ui.TitleBar()
        self.titleBar.SetParent(self)
        self.titleBar.MakeTitleBar(400, "red")
        self.titleBar.SetPosition(0, 0)
        self.titleBar.SetTitleName("Krallık Teleportasyonu")
        self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
        self.titleBar.Show()
        
        # Main board
        self.board = ui.Board()
        self.board.SetParent(self)
        self.board.SetSize(400, 500)
        self.board.SetPosition(0, 0)
        self.board.Show()
        
        # Info text
        self.infoText = ui.TextLine()
        self.infoText.SetParent(self.board)
        self.infoText.SetPosition(20, 40)
        self.infoText.SetText("Teleport olmak istediğiniz krallığı seçin:")
        self.infoText.Show()
        
        # Kingdom list
        self.kingdomListBox = ui.ListBoxEx()
        self.kingdomListBox.SetParent(self.board)
        self.kingdomListBox.SetPosition(20, 70)
        self.kingdomListBox.SetSize(360, 300)
        self.kingdomListBox.Show()
        
        # Home button (teleport to own kingdom)
        self.homeButton = ui.Button()
        self.homeButton.SetParent(self.board)
        self.homeButton.SetPosition(20, 390)
        self.homeButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
        self.homeButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
        self.homeButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
        self.homeButton.SetText("Kendi Krallığım")
        self.homeButton.SetEvent(ui.__mem_func__(self.TeleportToHome))
        self.homeButton.Show()
        
        # Teleport button
        self.teleportButton = ui.Button()
        self.teleportButton.SetParent(self.board)
        self.teleportButton.SetPosition(150, 390)
        self.teleportButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
        self.teleportButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
        self.teleportButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
        self.teleportButton.SetText("Teleport")
        self.teleportButton.SetEvent(ui.__mem_func__(self.TeleportToSelected))
        self.teleportButton.Show()
        
        # Close button
        self.closeButton = ui.Button()
        self.closeButton.SetParent(self.board)
        self.closeButton.SetPosition(280, 390)
        self.closeButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
        self.closeButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
        self.closeButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
        self.closeButton.SetText("Kapat")
        self.closeButton.SetEvent(ui.__mem_func__(self.Close))
        self.closeButton.Show()

    def Open(self):
        if self.isLoaded == 0:
            self.LoadWindow()

        self.SetCenterPosition()
        self.SetTop()
        self.Show()
        
        # Request kingdom list for teleportation
        net.SendRequestKingdomListPacket()

    def Close(self):
        self.Hide()

    def UpdateKingdomList(self, kingdomList):
        """Krallık listesini güncelle"""
        self.kingdomList = kingdomList
        
        if self.kingdomListBox:
            self.kingdomListBox.RemoveAllItems()
            
            for kingdom in kingdomList:
                # Only show public kingdoms or kingdoms player is member of
                playerKingdomID = self.GetPlayerKingdomID()
                if not kingdom.get('isPrivate', True) or kingdom['id'] == playerKingdomID:
                    item = ui.ListBoxEx.Item()
                    item.SetText("%s (%d üye) - %s" % (
                        kingdom.get('name', ''),
                        kingdom.get('memberCount', 0),
                        self.GetMapName(kingdom.get('mapIndex', 1))
                    ))
                    self.kingdomListBox.AppendItem(item)

    def GetPlayerKingdomID(self):
        """Oyuncunun krallık ID'sini al (bu fonksiyon player modülünden gelecek)"""
        # Bu fonksiyon player.py'de implement edilmeli
        return getattr(player, 'kingdomID', 0)

    def GetMapName(self, mapIndex):
        """Map index'ine göre map adını döndür"""
        mapNames = {
            1: "Joan",
            3: "Yongbi Çölü", 
            21: "Sohan Dağı",
            41: "Gautama Uçurumu"
        }
        return mapNames.get(mapIndex, "Bilinmeyen Bölge")

    def TeleportToHome(self):
        """Kendi krallığına teleport ol"""
        net.SendTeleportToOwnKingdomPacket()
        self.Close()

    def TeleportToSelected(self):
        """Seçili krallığa teleport ol"""
        selectedIndex = self.kingdomListBox.GetSelectedItem()
        if selectedIndex >= 0 and selectedIndex < len(self.kingdomList):
            kingdom = self.kingdomList[selectedIndex]
            net.SendTeleportToKingdomPacket(kingdom['id'])
            self.Close()
        else:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Lütfen bir krallık seçin.")

    def OnPressEscapeKey(self):
        self.Close()
        return TRUE


# Network functions to add to networkmodule.py

def SendTeleportToKingdomPacket(kingdomID):
    """Belirtilen krallığa teleport paketi gönder"""
    import struct
    
    packet = struct.pack("!I", kingdomID)
    net.SendPacket(208, packet)  # HEADER_CG_TELEPORT_TO_KINGDOM

def SendTeleportToOwnKingdomPacket():
    """Kendi krallığına teleport paketi gönder"""
    net.SendPacket(209, "")  # HEADER_CG_TELEPORT_TO_OWN_KINGDOM

def SendGetKingdomLandInfoPacket(kingdomID):
    """Krallık land bilgilerini iste"""
    import struct
    
    packet = struct.pack("!I", kingdomID)
    net.SendPacket(210, packet)  # HEADER_CG_GET_KINGDOM_LAND_INFO


# Game.py'ye eklenecek fonksiyonlar

def OpenKingdomTeleport(self):
    """Krallık teleportasyon penceresini aç"""
    if not hasattr(self, 'kingdomTeleportWindow') or not self.kingdomTeleportWindow:
        import kingdom_teleport
        self.kingdomTeleportWindow = kingdom_teleport.KingdomTeleportWindow()
    
    self.kingdomTeleportWindow.Open()

def CloseKingdomTeleport(self):
    """Krallık teleportasyon penceresini kapat"""
    if hasattr(self, 'kingdomTeleportWindow') and self.kingdomTeleportWindow:
        self.kingdomTeleportWindow.Close()

def RecvTeleportResult(self, data):
    """Teleportasyon sonucu paketi"""
    import struct
    
    header, result, message = struct.unpack("!BB100s", data)
    message = message.decode('utf-8').rstrip('\0')
    
    import chat
    if result == 0:  # Success
        chat.AppendChat(chat.CHAT_TYPE_INFO, "[Teleport] " + message)
    else:  # Failure
        chat.AppendChat(chat.CHAT_TYPE_ERROR, "[Teleport] " + message)

def RecvKingdomLandInfo(self, data):
    """Krallık land bilgileri paketi"""
    import struct
    
    # Parse land info packet
    header, kingdom_id, map_index, center_x, center_y, spawn_x, spawn_y, land_size, is_private = struct.unpack("!BIIBBIIII", data)
    
    # Store land info for use
    if not hasattr(self, 'kingdomLandInfo'):
        self.kingdomLandInfo = {}
    
    self.kingdomLandInfo[kingdom_id] = {
        'mapIndex': map_index,
        'centerX': center_x,
        'centerY': center_y,
        'spawnX': spawn_x,
        'spawnY': spawn_y,
        'landSize': land_size,
        'isPrivate': is_private == 1
    }


# Chat commands to add to game.py

def ProcessKingdomTeleportCommands(self, command, args):
    """Krallık teleportasyon chat komutları"""
    
    if command == "/krallık_tp" or command == "/kingdom_tp":
        self.OpenKingdomTeleport()
        return True
    
    elif command == "/ev" or command == "/home":
        net.SendTeleportToOwnKingdomPacket()
        return True
    
    elif command == "/krallık_git" or command == "/kingdom_go":
        if len(args) > 0:
            try:
                kingdomID = int(args[0])
                net.SendTeleportToKingdomPacket(kingdomID)
            except ValueError:
                import chat
                chat.AppendChat(chat.CHAT_TYPE_INFO, "[Teleport] Geçersiz krallık ID'si.")
        else:
            import chat
            chat.AppendChat(chat.CHAT_TYPE_INFO, "[Teleport] Kullanım: /krallık_git <krallık_id>")
        return True
    
    return False


# UI Script for KingdomTeleportWindow.py (to be created in uiscript folder)

kingdom_teleport_ui_script = '''
import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/"

window = {
    "name" : "KingdomTeleportWindow",

    "x" : 0,
    "y" : 0,

    "width" : 400,
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

            "width" : 384,
            "color" : "red",

            "children" :
            (
                {
                    "name" : "TitleName",
                    "type" : "text",

                    "x" : 0,
                    "y" : 3,
                    "horizontal_align" : "center",

                    "text" : "Krallık Teleportasyonu",
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

            "width" : 400,
            "height" : 500,

            "children" :
            (
                ## Info Text
                {
                    "name" : "InfoText",
                    "type" : "text",

                    "x" : 20,
                    "y" : 40,

                    "text" : "Teleport olmak istediğiniz krallığı seçin:",
                },

                ## Kingdom List
                {
                    "name" : "KingdomListBox",
                    "type" : "listbox",

                    "x" : 20,
                    "y" : 70,

                    "width" : 360,
                    "height" : 300,
                },

                ## Home Button
                {
                    "name" : "HomeButton",
                    "type" : "button",

                    "x" : 20,
                    "y" : 390,

                    "default_image" : ROOT_PATH + "large_button_01.sub",
                    "over_image" : ROOT_PATH + "large_button_02.sub",
                    "down_image" : ROOT_PATH + "large_button_03.sub",

                    "text" : "Kendi Krallığım",
                },

                ## Teleport Button
                {
                    "name" : "TeleportButton",
                    "type" : "button",

                    "x" : 150,
                    "y" : 390,

                    "default_image" : ROOT_PATH + "large_button_01.sub",
                    "over_image" : ROOT_PATH + "large_button_02.sub",
                    "down_image" : ROOT_PATH + "large_button_03.sub",

                    "text" : "Teleport",
                },

                ## Close Button
                {
                    "name" : "CloseButton",
                    "type" : "button",

                    "x" : 280,
                    "y" : 390,

                    "default_image" : ROOT_PATH + "large_button_01.sub",
                    "over_image" : ROOT_PATH + "large_button_02.sub",
                    "down_image" : ROOT_PATH + "large_button_03.sub",

                    "text" : "Kapat",
                },
            ),
        ),
    ),
}
'''

# Save the UI script to file
def CreateKingdomTeleportUIScript():
    """Krallık teleportasyon UI script dosyasını oluştur"""
    with open("/path/to/client/Binary/uiscript/uiscript/kingdomteleportwindow.py", "w") as f:
        f.write(kingdom_teleport_ui_script)
