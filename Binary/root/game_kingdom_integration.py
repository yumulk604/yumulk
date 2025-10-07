# Bu kod game.py dosyasına eklenecek krallık sistemi entegrasyonu

# GameWindow sınıfına eklenecek kod:

def __init__(self):
    # ... mevcut init kodu ...
    
    # Kingdom system
    self.kingdomManagementWindow = None
    self.kingdomSettingsWindow = None

def __del__(self):
    # ... mevcut del kodu ...
    
    # Kingdom system cleanup
    if self.kingdomManagementWindow:
        self.kingdomManagementWindow.Destroy()
        self.kingdomManagementWindow = None
    
    if self.kingdomSettingsWindow:
        self.kingdomSettingsWindow.Destroy()
        self.kingdomSettingsWindow = None

def OpenKingdomManagement(self):
    """Krallık yönetim penceresini aç"""
    if not self.kingdomManagementWindow:
        import uikingdom
        self.kingdomManagementWindow = uikingdom.KingdomManagementWindow()
    
    self.kingdomManagementWindow.Open()

def CloseKingdomManagement(self):
    """Krallık yönetim penceresini kapat"""
    if self.kingdomManagementWindow:
        self.kingdomManagementWindow.Close()

def OpenKingdomSettings(self):
    """Krallık ayarları penceresini aç"""
    if not self.kingdomSettingsWindow:
        import uikingdom
        self.kingdomSettingsWindow = uikingdom.KingdomSettingsWindow()
    
    self.kingdomSettingsWindow.Open()

def CloseKingdomSettings(self):
    """Krallık ayarları penceresini kapat"""
    if self.kingdomSettingsWindow:
        self.kingdomSettingsWindow.Close()

# Packet handler fonksiyonları (RecvPacket fonksiyonuna eklenecek)

def RecvKingdomList(self, data):
    """Krallık listesi paketi aldığında"""
    import struct
    
    # Parse packet
    header, count = struct.unpack("!BH", data[:3])
    offset = 3
    
    kingdoms = []
    for i in range(count):
        kingdom_data = struct.unpack("!I20sHBBBB", data[offset:offset+30])
        kingdom = {
            'id': kingdom_data[0],
            'name': kingdom_data[1].decode('utf-8').rstrip('\0'),
            'memberCount': kingdom_data[2],
            'color': [kingdom_data[3], kingdom_data[4], kingdom_data[5]],
            'flag': kingdom_data[6]
        }
        kingdoms.append(kingdom)
        offset += 30
    
    # Update kingdom selection window if open
    if hasattr(self, 'selectKingdomWindow') and self.selectKingdomWindow:
        self.selectKingdomWindow.UpdateKingdomList(kingdoms)

def RecvKingdomInfo(self, data):
    """Krallık bilgileri paketi aldığında"""
    import struct
    
    # Parse kingdom info
    header_data = struct.unpack("!BI20s100sBBBBIH", data[:139])
    
    kingdom_info = {
        'id': header_data[1],
        'name': header_data[2].decode('utf-8').rstrip('\0'),
        'description': header_data[3].decode('utf-8').rstrip('\0'),
        'color': [header_data[4], header_data[5], header_data[6]],
        'flag': header_data[7],
        'createTime': header_data[8],
        'memberCount': header_data[9]
    }
    
    # Parse members
    offset = 139
    members = []
    for i in range(kingdom_info['memberCount']):
        member_data = struct.unpack("!I20sBI?", data[offset:offset+30])
        member = {
            'id': member_data[0],
            'name': member_data[1].decode('utf-8').rstrip('\0'),
            'rank': member_data[2],
            'joinTime': member_data[3],
            'online': member_data[4]
        }
        members.append(member)
        offset += 30
    
    kingdom_info['members'] = members
    
    # Update kingdom management window
    if self.kingdomManagementWindow:
        self.kingdomManagementWindow.UpdateKingdomInfo(kingdom_info)
        self.kingdomManagementWindow.UpdateMemberList(members)

def RecvKingdomInvite(self, data):
    """Krallık daveti paketi aldığında"""
    import struct
    
    header, kingdom_name, inviter_name, kingdom_id = struct.unpack("!B20s20sI", data)
    
    kingdom_name = kingdom_name.decode('utf-8').rstrip('\0')
    inviter_name = inviter_name.decode('utf-8').rstrip('\0')
    
    # Show invite dialog
    import uiCommon
    questionDialog = uiCommon.QuestionDialog()
    questionDialog.SetText("%s sizi '%s' krallığına davet ediyor. Kabul ediyor musunuz?" % (inviter_name, kingdom_name))
    questionDialog.SetAcceptEvent(lambda: self.AcceptKingdomInvite(kingdom_id))
    questionDialog.SetCancelEvent(lambda: self.DeclineKingdomInvite(kingdom_id))
    questionDialog.Open()

def AcceptKingdomInvite(self, kingdom_id):
    """Krallık davetini kabul et"""
    import net
    net.SendJoinKingdomPacket(kingdom_id)

def DeclineKingdomInvite(self, kingdom_id):
    """Krallık davetini reddet"""
    # Just close the dialog, no packet needed
    pass

def RecvKingdomJoinResult(self, data):
    """Krallığa katılma sonucu paketi"""
    import struct
    
    header, result, message = struct.unpack("!BB100s", data)
    message = message.decode('utf-8').rstrip('\0')
    
    import chat
    if result == 0:  # Success
        chat.AppendChat(chat.CHAT_TYPE_INFO, "[Krallık] " + message)
    else:  # Failure
        chat.AppendChat(chat.CHAT_TYPE_ERROR, "[Krallık] " + message)

def RecvKingdomLeaveResult(self, data):
    """Krallıktan ayrılma sonucu paketi"""
    import struct
    
    header, result, message = struct.unpack("!BB100s", data)
    message = message.decode('utf-8').rstrip('\0')
    
    import chat
    if result == 0:  # Success
        chat.AppendChat(chat.CHAT_TYPE_INFO, "[Krallık] " + message)
        # Close kingdom windows
        self.CloseKingdomManagement()
        self.CloseKingdomSettings()
    else:  # Failure
        chat.AppendChat(chat.CHAT_TYPE_ERROR, "[Krallık] " + message)

# Klavye kısayolları (OnKeyDown fonksiyonuna eklenecek)

def OnKeyDown(self, key):
    # ... mevcut kod ...
    
    # Kingdom management hotkey (K key)
    if key == app.DIK_K:
        if app.IsPressed(app.DIK_LCONTROL):  # Ctrl+K
            self.OpenKingdomManagement()
            return TRUE
    
    return FALSE

# Chat komutları (OnRecvWhisper veya chat handler'a eklenecek)

def ProcessChatCommand(self, command, args):
    """Chat komutlarını işle"""
    
    if command == "/krallık" or command == "/kingdom":
        self.OpenKingdomManagement()
        return True
    
    elif command == "/krallık_ayar" or command == "/kingdom_settings":
        self.OpenKingdomSettings()
        return True
    
    elif command == "/krallık_davet" or command == "/kingdom_invite":
        if len(args) > 0:
            import net
            net.SendKingdomInvitePacket(args[0])
            import chat
            chat.AppendChat(chat.CHAT_TYPE_INFO, "[Krallık] %s oyuncusuna davet gönderildi." % args[0])
        else:
            import chat
            chat.AppendChat(chat.CHAT_TYPE_INFO, "[Krallık] Kullanım: /krallık_davet <oyuncu_adı>")
        return True
    
    elif command == "/krallık_ayrıl" or command == "/kingdom_leave":
        import uiCommon
        questionDialog = uiCommon.QuestionDialog()
        questionDialog.SetText("Krallıktan ayrılmak istediğinizden emin misiniz?")
        questionDialog.SetAcceptEvent(lambda: self.LeaveKingdom())
        questionDialog.SetCancelEvent(lambda: None)
        questionDialog.Open()
        return True
    
    return False

def LeaveKingdom(self):
    """Krallıktan ayrıl"""
    import net
    net.SendLeaveKingdomPacket()

# UI entegrasyonu için menü ekleme (interface modülüne eklenecek)

def AddKingdomMenuItems(self):
    """Ana menüye krallık seçeneklerini ekle"""
    
    # System menüsüne krallık yönetimi ekle
    if hasattr(self, 'systemDialog'):
        # Krallık yönetimi butonu ekle
        kingdomButton = ui.Button()
        kingdomButton.SetParent(self.systemDialog)
        kingdomButton.SetPosition(10, 200)  # Uygun pozisyon
        kingdomButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
        kingdomButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
        kingdomButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
        kingdomButton.SetText("Krallık Yönetimi")
        kingdomButton.SetEvent(ui.__mem_func__(self.OpenKingdomManagement))
        kingdomButton.Show()

# Packet handler registration (input_main.cpp'ye eklenecek kod için referans)
"""
C++ tarafında input_main.cpp dosyasına eklenecek kod:

// Kingdom packet handlers
case HEADER_CG_CREATE_KINGDOM:
    CKingdomPacketHandler::HandleCreateKingdom(ch, c_pData);
    break;

case HEADER_CG_JOIN_KINGDOM:
    CKingdomPacketHandler::HandleJoinKingdom(ch, c_pData);
    break;

case HEADER_CG_LEAVE_KINGDOM:
    CKingdomPacketHandler::HandleLeaveKingdom(ch, c_pData);
    break;

case HEADER_CG_REQUEST_KINGDOM_LIST:
    CKingdomPacketHandler::HandleRequestKingdomList(ch, c_pData);
    break;

case HEADER_CG_KINGDOM_INVITE:
    CKingdomPacketHandler::HandleKingdomInvite(ch, c_pData);
    break;

case HEADER_CG_KINGDOM_KICK:
    CKingdomPacketHandler::HandleKingdomKick(ch, c_pData);
    break;

case HEADER_CG_KINGDOM_RANK:
    CKingdomPacketHandler::HandleKingdomRank(ch, c_pData);
    break;

case HEADER_CG_KINGDOM_SETTINGS:
    CKingdomPacketHandler::HandleKingdomSettings(ch, c_pData);
    break;
"""
