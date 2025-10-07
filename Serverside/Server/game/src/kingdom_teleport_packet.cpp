#include "stdafx.h"
#include "kingdom.h"
#include "kingdom_packet.h"
#include "char.h"
#include "desc.h"
#include "packet.h"

// Teleportation packet structures
#pragma pack(1)

struct TPacketCGTeleportToKingdom
{
    BYTE bHeader;
    DWORD dwKingdomID;
};

struct TPacketCGTeleportToOwnKingdom
{
    BYTE bHeader;
};

struct TPacketCGGetKingdomLandInfo
{
    BYTE bHeader;
    DWORD dwKingdomID;
};

struct TPacketGCTeleportResult
{
    BYTE bHeader;
    BYTE bResult;  // 0 = success, 1 = fail
    char szMessage[100];
};

struct TPacketGCKingdomLandInfo
{
    BYTE bHeader;
    DWORD dwKingdomID;
    DWORD dwMapIndex;
    long lCenterX, lCenterY;
    long lSpawnX, lSpawnY;
    DWORD dwLandSize;
    BYTE bIsPrivate;
};

#pragma pack()

// Packet headers for teleportation
enum
{
    HEADER_CG_TELEPORT_TO_KINGDOM = 208,
    HEADER_CG_TELEPORT_TO_OWN_KINGDOM = 209,
    HEADER_CG_GET_KINGDOM_LAND_INFO = 210,
    
    HEADER_GC_TELEPORT_RESULT = 208,
    HEADER_GC_KINGDOM_LAND_INFO = 209
};

void CKingdomPacketHandler::HandleTeleportToKingdom(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketCGTeleportToKingdom* packet = (TPacketCGTeleportToKingdom*)data;
    
    bool success = CKingdomManager::instance().TeleportToKingdom(ch->GetPlayerID(), packet->dwKingdomID);
    
    if (success)
    {
        SendTeleportResult(ch, true, "Krallığa başarıyla ışınlandınız.");
    }
    else
    {
        SendTeleportResult(ch, false, "Krallığa ışınlanamadınız. Yetkiniz olmayabilir.");
    }
}

void CKingdomPacketHandler::HandleTeleportToOwnKingdom(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    bool success = CKingdomManager::instance().TeleportToOwnKingdom(ch->GetPlayerID());
    
    if (success)
    {
        SendTeleportResult(ch, true, "Kendi krallığınıza ışınlandınız.");
    }
    else
    {
        SendTeleportResult(ch, false, "Bir krallığa üye değilsiniz.");
    }
}

void CKingdomPacketHandler::HandleGetKingdomLandInfo(LPCHARACTER ch, const char* data)
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketCGGetKingdomLandInfo* packet = (TPacketCGGetKingdomLandInfo*)data;
    
    SKingdom* kingdom = CKingdomManager::instance().GetKingdom(packet->dwKingdomID);
    if (!kingdom)
        return;
    
    // Send land info
    TPacketGCKingdomLandInfo landPacket;
    landPacket.bHeader = HEADER_GC_KINGDOM_LAND_INFO;
    landPacket.dwKingdomID = kingdom->dwKingdomID;
    landPacket.dwMapIndex = kingdom->landInfo.dwMapIndex;
    landPacket.lCenterX = kingdom->landInfo.lCenterX;
    landPacket.lCenterY = kingdom->landInfo.lCenterY;
    landPacket.lSpawnX = kingdom->landInfo.lSpawnX;
    landPacket.lSpawnY = kingdom->landInfo.lSpawnY;
    landPacket.dwLandSize = kingdom->landInfo.dwLandSize;
    landPacket.bIsPrivate = kingdom->landInfo.bIsPrivate ? 1 : 0;
    
    ch->GetDesc()->Packet(&landPacket, sizeof(landPacket));
}

void CKingdomPacketHandler::SendTeleportResult(LPCHARACTER ch, bool bSuccess, const std::string& strMessage)
{
    if (!ch || !ch->GetDesc())
        return;
    
    TPacketGCTeleportResult packet;
    packet.bHeader = HEADER_GC_TELEPORT_RESULT;
    packet.bResult = bSuccess ? 0 : 1;
    strncpy(packet.szMessage, strMessage.c_str(), sizeof(packet.szMessage));
    
    ch->GetDesc()->Packet(&packet, sizeof(packet));
}

// Enhanced teleportation functions with safety checks

bool CKingdomManager::TeleportToKingdom(DWORD dwPlayerID, DWORD dwKingdomID)
{
    LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPlayerID);
    if (!ch)
        return false;
    
    SKingdom* kingdom = GetKingdom(dwKingdomID);
    if (!kingdom)
        return false;
    
    // Check if player has permission to enter this kingdom
    if (kingdom->landInfo.bIsPrivate)
    {
        // Only kingdom members can enter private kingdoms
        if (!GetKingdomMember(dwKingdomID, dwPlayerID))
        {
            ch->ChatPacket(CHAT_TYPE_INFO, "Bu krallık özeldir. Sadece üyeler girebilir.");
            return false;
        }
    }
    
    // Check if player is in combat
    if (ch->IsPosition(POS_FIGHTING))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Savaş sırasında ışınlanamazsınız.");
        return false;
    }
    
    // Check if player is in a dungeon
    if (ch->GetMapIndex() >= 10000)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Dungeon içindeyken ışınlanamazsınız.");
        return false;
    }
    
    // Check teleportation cooldown (5 minutes)
    static std::map<DWORD, DWORD> s_mapTeleportCooldown;
    DWORD currentTime = get_global_time();
    
    auto it = s_mapTeleportCooldown.find(dwPlayerID);
    if (it != s_mapTeleportCooldown.end())
    {
        if (currentTime - it->second < 300) // 5 minutes cooldown
        {
            DWORD remainingTime = 300 - (currentTime - it->second);
            ch->ChatPacket(CHAT_TYPE_INFO, "Teleportasyon için %d saniye beklemelisiniz.", remainingTime);
            return false;
        }
    }
    
    // Teleport player to kingdom spawn point
    ch->WarpSet(kingdom->landInfo.lSpawnX, kingdom->landInfo.lSpawnY, kingdom->landInfo.dwMapIndex);
    
    // Set cooldown
    s_mapTeleportCooldown[dwPlayerID] = currentTime;
    
    // Notify player and kingdom members
    ch->ChatPacket(CHAT_TYPE_INFO, "[Krallık] %s krallığına ışınlandınız.", kingdom->strName.c_str());
    
    // Notify kingdom members if it's not their own kingdom
    DWORD playerKingdomID = GetPlayerKingdomID(dwPlayerID);
    if (playerKingdomID != dwKingdomID)
    {
        NotifyKingdomMembers(dwKingdomID, ch->GetName() + std::string(" krallığınızı ziyaret ediyor."));
    }
    
    return true;
}

bool CKingdomManager::TeleportToOwnKingdom(DWORD dwPlayerID)
{
    DWORD kingdomID = GetPlayerKingdomID(dwPlayerID);
    if (kingdomID == 0)
    {
        LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPlayerID);
        if (ch)
            ch->ChatPacket(CHAT_TYPE_INFO, "Bir krallığa üye değilsiniz.");
        return false;
    }
    
    return TeleportToKingdom(dwPlayerID, kingdomID);
}

// Kingdom land management with enhanced features

bool CKingdomManager::SetKingdomLandPrivacy(DWORD dwKingdomID, DWORD dwPlayerID, bool bIsPrivate)
{
    SKingdom* kingdom = GetKingdom(dwKingdomID);
    if (!kingdom)
        return false;
    
    // Check if player is king
    if (kingdom->dwKingID != dwPlayerID)
        return false;
    
    kingdom->landInfo.bIsPrivate = bIsPrivate;
    SaveKingdomToDB(*kingdom);
    
    std::string message = bIsPrivate ? "Krallık artık özel." : "Krallık artık herkese açık.";
    NotifyKingdomMembers(dwKingdomID, message);
    
    return true;
}

bool CKingdomManager::ExpandKingdomLand(DWORD dwKingdomID, DWORD dwPlayerID, DWORD dwNewSize)
{
    SKingdom* kingdom = GetKingdom(dwKingdomID);
    if (!kingdom)
        return false;
    
    // Check if player has permission (king or commander)
    SKingdomMember* member = GetKingdomMember(dwKingdomID, dwPlayerID);
    if (!member || member->bRank < KINGDOM_RANK_COMMANDER)
        return false;
    
    // Check resource cost for expansion
    DWORD goldCost = (dwNewSize - kingdom->landInfo.dwLandSize) * 10;
    DWORD stoneCost = (dwNewSize - kingdom->landInfo.dwLandSize) * 5;
    
    if (!HasEnoughResources(dwKingdomID, goldCost, 0, stoneCost, 0))
        return false;
    
    // Spend resources
    if (!SpendKingdomResource(dwKingdomID, goldCost, 0, stoneCost, 0))
        return false;
    
    // Expand land
    kingdom->landInfo.dwLandSize = dwNewSize;
    SaveKingdomToDB(*kingdom);
    
    NotifyKingdomMembers(dwKingdomID, "Krallık toprakları genişletildi!");
    
    return true;
}

std::vector<LPCHARACTER> CKingdomManager::GetPlayersInKingdomTerritory(DWORD dwKingdomID)
{
    std::vector<LPCHARACTER> players;
    
    SKingdom* kingdom = GetKingdom(dwKingdomID);
    if (!kingdom)
        return players;
    
    // Find all players in kingdom territory
    const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
    
    for (auto it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
    {
        LPDESC d = *it;
        if (!d->GetCharacter())
            continue;
        
        LPCHARACTER ch = d->GetCharacter();
        if (ch->GetMapIndex() == kingdom->landInfo.dwMapIndex)
        {
            if (IsInKingdomTerritory(dwKingdomID, ch->GetX(), ch->GetY()))
            {
                players.push_back(ch);
            }
        }
    }
    
    return players;
}

void CKingdomManager::BroadcastToKingdomTerritory(DWORD dwKingdomID, const std::string& strMessage)
{
    std::vector<LPCHARACTER> players = GetPlayersInKingdomTerritory(dwKingdomID);
    
    for (LPCHARACTER ch : players)
    {
        ch->ChatPacket(CHAT_TYPE_NOTICE, "[Krallık] %s", strMessage.c_str());
    }
}
