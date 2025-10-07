#include "stdafx.h"
#include "kingdom.h"
#include "kingdom_packet.h"
#include "char.h"
#include "desc.h"
#include "buffer_manager.h"
#include "packet.h"

void CKingdomPacketHandler::HandleCreateKingdom(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketCGCreateKingdom* packet = (TPacketCGCreateKingdom*)data;
    
    // Validate packet
    if (packet->dwNameLength == 0 || packet->dwNameLength > 20)
    {
        SendKingdomJoinResult(ch, false, "Geçersiz krallık adı.");
        return;
    }
    
    std::string kingdomName(packet->szName, packet->dwNameLength);
    
    // Create kingdom
    bool success = CKingdomManager::instance().CreateKingdom(
        ch->GetPlayerID(),
        kingdomName,
        packet->bColorR,
        packet->bColorG,
        packet->bColorB,
        packet->bFlag
    );
    
    if (success)
    {
        SendKingdomJoinResult(ch, true, "Krallık başarıyla oluşturuldu.");
        ch->ChatPacket(CHAT_TYPE_INFO, "Krallığınız '%s' başarıyla oluşturuldu!", kingdomName.c_str());
    }
    else
    {
        SendKingdomJoinResult(ch, false, "Krallık oluşturulamadı.");
    }
}

void CKingdomPacketHandler::HandleJoinKingdom(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketCGJoinKingdom* packet = (TPacketCGJoinKingdom*)data;
    
    bool success = CKingdomManager::instance().JoinKingdom(ch->GetPlayerID(), packet->dwKingdomID);
    
    if (success)
    {
        SendKingdomJoinResult(ch, true, "Krallığa başarıyla katıldınız.");
        
        // Send kingdom info to player
        SendKingdomInfo(ch, packet->dwKingdomID);
    }
    else
    {
        SendKingdomJoinResult(ch, false, "Krallığa katılamadınız.");
    }
}

void CKingdomPacketHandler::HandleLeaveKingdom(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    bool success = CKingdomManager::instance().LeaveKingdom(ch->GetPlayerID());
    
    if (success)
    {
        SendKingdomLeaveResult(ch, true, "Krallıktan başarıyla ayrıldınız.");
    }
    else
    {
        SendKingdomLeaveResult(ch, false, "Krallıktan ayrılamadınız.");
    }
}

void CKingdomPacketHandler::HandleRequestKingdomList(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    std::vector<SKingdom*> kingdoms = CKingdomManager::instance().GetAllKingdoms();
    
    // Calculate packet size
    int packetSize = sizeof(TPacketGCKingdomList) + kingdoms.size() * sizeof(TKingdomListData);
    
    TEMP_BUFFER buf;
    TPacketGCKingdomList* packet = (TPacketGCKingdomList*)buf.write_peek();
    packet->bHeader = HEADER_GC_KINGDOM_LIST;
    packet->wCount = kingdoms.size();
    buf.write(sizeof(TPacketGCKingdomList));
    
    for (const auto& kingdom : kingdoms)
    {
        TKingdomListData* data = (TKingdomListData*)buf.write_peek();
        data->dwKingdomID = kingdom->dwKingdomID;
        strncpy(data->szName, kingdom->strName.c_str(), sizeof(data->szName));
        data->wMemberCount = kingdom->vecMembers.size();
        data->bColorR = kingdom->bColorR;
        data->bColorG = kingdom->bColorG;
        data->bColorB = kingdom->bColorB;
        data->bFlag = kingdom->bFlag;
        buf.write(sizeof(TKingdomListData));
    }
    
    ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CKingdomPacketHandler::HandleKingdomInvite(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketCGKingdomInvite* packet = (TPacketCGKingdomInvite*)data;
    
    DWORD kingdomID = CKingdomManager::instance().GetPlayerKingdomID(ch->GetPlayerID());
    if (kingdomID == 0)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Bir krallığa üye değilsiniz.");
        return;
    }
    
    std::string playerName(packet->szPlayerName);
    playerName = playerName.substr(0, playerName.find('\0')); // Remove null chars
    
    bool success = CKingdomManager::instance().InvitePlayer(kingdomID, ch->GetPlayerID(), playerName);
    
    if (success)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "%s oyuncusuna davet gönderildi.", playerName.c_str());
    }
    else
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Davet gönderilemedi.");
    }
}

void CKingdomPacketHandler::HandleKingdomKick(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketCGKingdomKick* packet = (TPacketCGKingdomKick*)data;
    
    DWORD kingdomID = CKingdomManager::instance().GetPlayerKingdomID(ch->GetPlayerID());
    if (kingdomID == 0)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Bir krallığa üye değilsiniz.");
        return;
    }
    
    std::string playerName(packet->szPlayerName);
    playerName = playerName.substr(0, playerName.find('\0'));
    
    bool success = CKingdomManager::instance().KickPlayer(kingdomID, ch->GetPlayerID(), playerName);
    
    if (success)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "%s oyuncusu krallıktan atıldı.", playerName.c_str());
    }
    else
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Oyuncu atılamadı.");
    }
}

void CKingdomPacketHandler::HandleKingdomRank(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketCGKingdomRank* packet = (TPacketCGKingdomRank*)data;
    
    DWORD kingdomID = CKingdomManager::instance().GetPlayerKingdomID(ch->GetPlayerID());
    if (kingdomID == 0)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Bir krallığa üye değilsiniz.");
        return;
    }
    
    std::string playerName(packet->szPlayerName);
    playerName = playerName.substr(0, playerName.find('\0'));
    
    bool success = CKingdomManager::instance().ChangePlayerRank(kingdomID, ch->GetPlayerID(), playerName, packet->bRank);
    
    if (success)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "%s oyuncusunun rütbesi değiştirildi.", playerName.c_str());
    }
    else
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Rütbe değiştirilemedi.");
    }
}

void CKingdomPacketHandler::HandleKingdomSettings(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketCGKingdomSettings* packet = (TPacketCGKingdomSettings*)data;
    
    DWORD kingdomID = CKingdomManager::instance().GetPlayerKingdomID(ch->GetPlayerID());
    if (kingdomID == 0)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Bir krallığa üye değilsiniz.");
        return;
    }
    
    std::string name(packet->szName);
    name = name.substr(0, name.find('\0'));
    
    std::string description(packet->szDescription);
    description = description.substr(0, description.find('\0'));
    
    bool success = CKingdomManager::instance().UpdateKingdomSettings(
        kingdomID,
        ch->GetPlayerID(),
        name,
        description,
        packet->bColorR,
        packet->bColorG,
        packet->bColorB,
        packet->bFlag
    );
    
    if (success)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Krallık ayarları güncellendi.");
    }
    else
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Ayarlar güncellenemedi.");
    }
}

void CKingdomPacketHandler::SendKingdomList(LPCHARACTER ch)
{
    if (!ch || !ch->GetDesc())
        return;
    
    std::vector<SKingdom*> kingdoms = CKingdomManager::instance().GetAllKingdoms();
    
    TEMP_BUFFER buf;
    TPacketGCKingdomList* packet = (TPacketGCKingdomList*)buf.write_peek();
    packet->bHeader = HEADER_GC_KINGDOM_LIST;
    packet->wCount = kingdoms.size();
    buf.write(sizeof(TPacketGCKingdomList));
    
    for (const auto& kingdom : kingdoms)
    {
        TKingdomListData* data = (TKingdomListData*)buf.write_peek();
        data->dwKingdomID = kingdom->dwKingdomID;
        strncpy(data->szName, kingdom->strName.c_str(), sizeof(data->szName));
        data->wMemberCount = kingdom->vecMembers.size();
        data->bColorR = kingdom->bColorR;
        data->bColorG = kingdom->bColorG;
        data->bColorB = kingdom->bColorB;
        data->bFlag = kingdom->bFlag;
        buf.write(sizeof(TKingdomListData));
    }
    
    ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CKingdomPacketHandler::SendKingdomInfo(LPCHARACTER ch, DWORD dwKingdomID)
{
    if (!ch || !ch->GetDesc())
        return;
    
    SKingdom* kingdom = CKingdomManager::instance().GetKingdom(dwKingdomID);
    if (!kingdom)
        return;
    
    TEMP_BUFFER buf;
    TPacketGCKingdomInfo* packet = (TPacketGCKingdomInfo*)buf.write_peek();
    packet->bHeader = HEADER_GC_KINGDOM_INFO;
    packet->dwKingdomID = kingdom->dwKingdomID;
    strncpy(packet->szName, kingdom->strName.c_str(), sizeof(packet->szName));
    strncpy(packet->szDescription, kingdom->strDescription.c_str(), sizeof(packet->szDescription));
    packet->bColorR = kingdom->bColorR;
    packet->bColorG = kingdom->bColorG;
    packet->bColorB = kingdom->bColorB;
    packet->bFlag = kingdom->bFlag;
    packet->dwCreateTime = kingdom->dwCreateTime;
    packet->wMemberCount = kingdom->vecMembers.size();
    buf.write(sizeof(TPacketGCKingdomInfo));
    
    // Add member data
    for (const auto& member : kingdom->vecMembers)
    {
        TKingdomMemberData* memberData = (TKingdomMemberData*)buf.write_peek();
        memberData->dwPlayerID = member.dwPlayerID;
        strncpy(memberData->szName, member.strName.c_str(), sizeof(memberData->szName));
        memberData->bRank = member.bRank;
        memberData->dwJoinTime = member.dwJoinTime;
        memberData->bOnline = member.bOnline;
        buf.write(sizeof(TKingdomMemberData));
    }
    
    ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CKingdomPacketHandler::SendKingdomJoinResult(LPCHARACTER ch, bool bSuccess, const std::string& strMessage)
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketGCKingdomJoinResult packet;
    packet.bHeader = HEADER_GC_KINGDOM_JOIN_RESULT;
    packet.bResult = bSuccess ? 0 : 1;
    strncpy(packet.szMessage, strMessage.c_str(), sizeof(packet.szMessage));
    
    ch->GetDesc()->Packet(&packet, sizeof(packet));
}

void CKingdomPacketHandler::SendKingdomLeaveResult(LPCHARACTER ch, bool bSuccess, const std::string& strMessage)
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketGCKingdomJoinResult packet;
    packet.bHeader = HEADER_GC_KINGDOM_LEAVE_RESULT;
    packet.bResult = bSuccess ? 0 : 1;
    strncpy(packet.szMessage, strMessage.c_str(), sizeof(packet.szMessage));
    
    ch->GetDesc()->Packet(&packet, sizeof(packet));
}
