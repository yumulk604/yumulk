# Kingdom Spawn System
# Bu dosya oyuncuların ilk giriş yaptığında krallık spawn noktasına ışınlanması için gerekli kodları içerir

import player
import net
import app
import chat

class KingdomSpawnManager:
    """Krallık spawn yönetim sistemi"""
    
    def __init__(self):
        self.playerKingdomInfo = {}
        self.isFirstLogin = True
    
    def OnPlayerLogin(self):
        """Oyuncu giriş yaptığında çağrılır"""
        # Request player's kingdom info
        net.SendRequestPlayerKingdomInfoPacket()
    
    def OnReceivePlayerKingdomInfo(self, kingdomInfo):
        """Server'dan oyuncunun krallık bilgileri geldiğinde"""
        if not kingdomInfo:
            # Player has no kingdom, show kingdom selection
            self.ShowKingdomSelection()
            return
        
        # Store kingdom info
        self.playerKingdomInfo = kingdomInfo
        
        # Check if this is first login or player wants to spawn at kingdom
        if self.ShouldSpawnAtKingdom():
            self.SpawnAtKingdom()
    
    def ShouldSpawnAtKingdom(self):
        """Oyuncunun krallık spawn noktasına ışınlanıp ışınlanmayacağını kontrol et"""
        # Check if player is logging in for first time
        if self.isFirstLogin:
            return True
        
        # Check if player died and wants to respawn at kingdom
        if player.GetStatus() == player.STATUS_DEAD:
            return True
        
        # Check player preference (could be saved in settings)
        return self.GetSpawnPreference()
    
    def SpawnAtKingdom(self):
        """Oyuncuyu krallık spawn noktasına ışınla"""
        if not self.playerKingdomInfo:
            return
        
        kingdomID = self.playerKingdomInfo.get('id', 0)
        if kingdomID > 0:
            net.SendTeleportToOwnKingdomPacket()
            chat.AppendChat(chat.CHAT_TYPE_INFO, "[Krallık] Krallığınıza ışınlanıyorsunuz...")
    
    def ShowKingdomSelection(self):
        """Krallık seçim penceresini göster"""
        # This will be called from game.py
        import game
        gameWindow = game.GetGameWindow()
        if gameWindow:
            gameWindow.OpenKingdomSelection()
    
    def GetSpawnPreference(self):
        """Oyuncunun spawn tercihi (ayarlardan)"""
        # This could be saved in player settings
        return True  # Default: always spawn at kingdom
    
    def SetSpawnPreference(self, preference):
        """Spawn tercihini ayarla"""
        # Save to player settings
        pass

# Global spawn manager instance
g_kingdomSpawnManager = KingdomSpawnManager()

# Functions to be added to game.py

def OnPlayerEnterGame(self):
    """Oyuncu oyuna girdiğinde çağrılır"""
    # ... existing code ...
    
    # Initialize kingdom spawn system
    g_kingdomSpawnManager.OnPlayerLogin()

def OnReceivePlayerKingdomInfo(self, data):
    """Oyuncunun krallık bilgileri paketi"""
    import struct
    
    if len(data) < 5:  # No kingdom
        g_kingdomSpawnManager.OnReceivePlayerKingdomInfo(None)
        return
    
    # Parse kingdom info
    header, kingdom_id, map_index, spawn_x, spawn_y = struct.unpack("!BIIII", data[:17])
    
    kingdomInfo = {
        'id': kingdom_id,
        'mapIndex': map_index,
        'spawnX': spawn_x,
        'spawnY': spawn_y
    }
    
    g_kingdomSpawnManager.OnReceivePlayerKingdomInfo(kingdomInfo)

def OpenKingdomSelection(self):
    """Krallık seçim penceresini aç"""
    if not hasattr(self, 'selectKingdomWindow') or not self.selectKingdomWindow:
        import introempire_new
        self.selectKingdomWindow = introempire_new.SelectKingdomWindow(self)
    
    self.selectKingdomWindow.Open()

# Network functions to add to networkmodule.py

def SendRequestPlayerKingdomInfoPacket():
    """Oyuncunun krallık bilgilerini iste"""
    net.SendPacket(211, "")  # HEADER_CG_REQUEST_PLAYER_KINGDOM_INFO

# Server tarafı packet handler (kingdom_packet.cpp'ye eklenecek)

server_code = '''
// Player kingdom info packet
struct TPacketCGRequestPlayerKingdomInfo
{
    BYTE bHeader;
};

struct TPacketGCPlayerKingdomInfo
{
    BYTE bHeader;
    DWORD dwKingdomID;
    DWORD dwMapIndex;
    long lSpawnX, lSpawnY;
};

void CKingdomPacketHandler::HandleRequestPlayerKingdomInfo(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    DWORD kingdomID = CKingdomManager::instance().GetPlayerKingdomID(ch->GetPlayerID());
    
    TPacketGCPlayerKingdomInfo packet;
    packet.bHeader = HEADER_GC_PLAYER_KINGDOM_INFO;
    
    if (kingdomID > 0)
    {
        SKingdom* kingdom = CKingdomManager::instance().GetKingdom(kingdomID);
        if (kingdom)
        {
            packet.dwKingdomID = kingdomID;
            packet.dwMapIndex = kingdom->landInfo.dwMapIndex;
            packet.lSpawnX = kingdom->landInfo.lSpawnX;
            packet.lSpawnY = kingdom->landInfo.lSpawnY;
        }
        else
        {
            packet.dwKingdomID = 0;
            packet.dwMapIndex = 1;
            packet.lSpawnX = 469300;
            packet.lSpawnY = 964200;
        }
    }
    else
    {
        // No kingdom, use default spawn
        packet.dwKingdomID = 0;
        packet.dwMapIndex = 1;
        packet.lSpawnX = 469300;
        packet.lSpawnY = 964200;
    }
    
    ch->GetDesc()->Packet(&packet, sizeof(packet));
}

// Add to packet headers
enum
{
    HEADER_CG_REQUEST_PLAYER_KINGDOM_INFO = 211,
    HEADER_GC_PLAYER_KINGDOM_INFO = 210
};
'''

# Respawn system integration

class KingdomRespawnDialog(ui.ScriptWindow):
    """Krallık respawn dialog penceresi"""
    
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.isLoaded = 0
        
        # UI Elements
        self.titleBar = None
        self.board = None
        self.messageText = None
        self.kingdomButton = None
        self.villageButton = None
        self.closeButton = None

    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def LoadWindow(self):
        if self.isLoaded == 1:
            return

        self.isLoaded = 1
        self.CreateUI()

    def CreateUI(self):
        """UI'ı oluştur"""
        self.SetSize(400, 200)
        self.SetWindowName("KingdomRespawnDialog")
        
        # Title bar
        self.titleBar = ui.TitleBar()
        self.titleBar.SetParent(self)
        self.titleBar.MakeTitleBar(400, "red")
        self.titleBar.SetPosition(0, 0)
        self.titleBar.SetTitleName("Yeniden Doğum")
        self.titleBar.Show()
        
        # Main board
        self.board = ui.Board()
        self.board.SetParent(self)
        self.board.SetSize(400, 200)
        self.board.SetPosition(0, 0)
        self.board.Show()
        
        # Message text
        self.messageText = ui.TextLine()
        self.messageText.SetParent(self.board)
        self.messageText.SetPosition(20, 50)
        self.messageText.SetText("Nerede yeniden doğmak istiyorsunuz?")
        self.messageText.Show()
        
        # Kingdom button
        self.kingdomButton = ui.Button()
        self.kingdomButton.SetParent(self.board)
        self.kingdomButton.SetPosition(50, 100)
        self.kingdomButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
        self.kingdomButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
        self.kingdomButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
        self.kingdomButton.SetText("Krallığımda")
        self.kingdomButton.SetEvent(ui.__mem_func__(self.RespawnAtKingdom))
        self.kingdomButton.Show()
        
        # Village button
        self.villageButton = ui.Button()
        self.villageButton.SetParent(self.board)
        self.villageButton.SetPosition(200, 100)
        self.villageButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
        self.villageButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
        self.villageButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
        self.villageButton.SetText("Köyde")
        self.villageButton.SetEvent(ui.__mem_func__(self.RespawnAtVillage))
        self.villageButton.Show()

    def Open(self):
        if self.isLoaded == 0:
            self.LoadWindow()

        self.SetCenterPosition()
        self.SetTop()
        self.Show()

    def Close(self):
        self.Hide()

    def RespawnAtKingdom(self):
        """Krallıkta yeniden doğ"""
        net.SendTeleportToOwnKingdomPacket()
        self.Close()

    def RespawnAtVillage(self):
        """Köyde yeniden doğ"""
        # Use default respawn
        net.SendRespawnPacket()
        self.Close()

    def OnPressEscapeKey(self):
        self.RespawnAtVillage()  # Default to village
        return TRUE

# Integration with existing respawn system

def ShowKingdomRespawnDialog(self):
    """Krallık respawn dialog'unu göster"""
    if not hasattr(self, 'kingdomRespawnDialog') or not self.kingdomRespawnDialog:
        self.kingdomRespawnDialog = KingdomRespawnDialog()
    
    # Only show if player has a kingdom
    if g_kingdomSpawnManager.playerKingdomInfo:
        self.kingdomRespawnDialog.Open()
    else:
        # No kingdom, use default respawn
        net.SendRespawnPacket()

# Chat commands for spawn management

def ProcessSpawnCommands(self, command, args):
    """Spawn yönetimi chat komutları"""
    
    if command == "/spawn_ayar" or command == "/spawn_setting":
        preference = True if len(args) > 0 and args[0].lower() in ['1', 'true', 'evet'] else False
        g_kingdomSpawnManager.SetSpawnPreference(preference)
        
        import chat
        message = "Krallık spawn'ı açıldı." if preference else "Krallık spawn'ı kapatıldı."
        chat.AppendChat(chat.CHAT_TYPE_INFO, "[Spawn] " + message)
        return True
    
    elif command == "/respawn_krallık" or command == "/respawn_kingdom":
        if g_kingdomSpawnManager.playerKingdomInfo:
            net.SendTeleportToOwnKingdomPacket()
        else:
            import chat
            chat.AppendChat(chat.CHAT_TYPE_INFO, "[Spawn] Bir krallığa üye değilsiniz.")
        return True
    
    return False
