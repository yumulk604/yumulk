#include "stdafx.h"
#include "kingdom.h"
#include "char.h"
#include "desc.h"
#include "db.h"
#include "packet.h"
#include "buffer_manager.h"

CKingdomManager::CKingdomManager()
{
    m_dwNextKingdomID = 1;
}

CKingdomManager::~CKingdomManager()
{
}

bool CKingdomManager::CreateKingdom(DWORD dwPlayerID, const std::string& strName, 
                                   BYTE bColorR, BYTE bColorG, BYTE bColorB, BYTE bFlag)
{
    // Check if player is already in a kingdom
    if (IsPlayerInKingdom(dwPlayerID))
        return false;
    
    // Validate kingdom name
    if (!ValidateKingdomName(strName))
        return false;
    
    // Create new kingdom
    SKingdom kingdom;
    kingdom.dwKingdomID = GenerateKingdomID();
    kingdom.strName = strName;
    kingdom.bColorR = bColorR;
    kingdom.bColorG = bColorG;
    kingdom.bColorB = bColorB;
    kingdom.bFlag = bFlag;
    kingdom.dwCreateTime = time(0);
    kingdom.dwKingID = dwPlayerID;
    
    // Add creator as king
    SKingdomMember kingMember;
    kingMember.dwPlayerID = dwPlayerID;
    kingMember.bRank = KINGDOM_RANK_KING;
    kingMember.dwJoinTime = time(0);
    kingMember.bOnline = true;
    
    LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPlayerID);
    if (ch)
        kingMember.strName = ch->GetName();
    
    kingdom.vecMembers.push_back(kingMember);
    
    // Save to memory and database
    m_mapKingdoms[kingdom.dwKingdomID] = kingdom;
    m_mapPlayerKingdom[dwPlayerID] = kingdom.dwKingdomID;
    
    SaveKingdomToDB(kingdom);
    SaveMemberToDB(kingdom.dwKingdomID, kingMember);
    
    return true;
}

bool CKingdomManager::DeleteKingdom(DWORD dwKingdomID)
{
    auto it = m_mapKingdoms.find(dwKingdomID);
    if (it == m_mapKingdoms.end())
        return false;
    
    SKingdom& kingdom = it->second;
    
    // Remove all members from player-kingdom mapping
    for (const auto& member : kingdom.vecMembers)
    {
        m_mapPlayerKingdom.erase(member.dwPlayerID);
        DeleteMemberFromDB(dwKingdomID, member.dwPlayerID);
    }
    
    // Remove kingdom from database
    std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
        "DELETE FROM kingdom WHERE id = %u", dwKingdomID));
    
    // Remove from memory
    m_mapKingdoms.erase(it);
    
    return true;
}

bool CKingdomManager::JoinKingdom(DWORD dwPlayerID, DWORD dwKingdomID)
{
    // Check if player is already in a kingdom
    if (IsPlayerInKingdom(dwPlayerID))
        return false;
    
    // Check if kingdom exists
    auto it = m_mapKingdoms.find(dwKingdomID);
    if (it == m_mapKingdoms.end())
        return false;
    
    SKingdom& kingdom = it->second;
    
    // Add player as member
    SKingdomMember member;
    member.dwPlayerID = dwPlayerID;
    member.bRank = KINGDOM_RANK_MEMBER;
    member.dwJoinTime = time(0);
    member.bOnline = true;
    
    LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPlayerID);
    if (ch)
        member.strName = ch->GetName();
    
    kingdom.vecMembers.push_back(member);
    m_mapPlayerKingdom[dwPlayerID] = dwKingdomID;
    
    SaveMemberToDB(dwKingdomID, member);
    
    // Notify other kingdom members
    NotifyKingdomMembers(dwKingdomID, member.strName + " krallığa katıldı.");
    
    return true;
}

bool CKingdomManager::LeaveKingdom(DWORD dwPlayerID)
{
    auto playerIt = m_mapPlayerKingdom.find(dwPlayerID);
    if (playerIt == m_mapPlayerKingdom.end())
        return false;
    
    DWORD dwKingdomID = playerIt->second;
    auto kingdomIt = m_mapKingdoms.find(dwKingdomID);
    if (kingdomIt == m_mapKingdoms.end())
        return false;
    
    SKingdom& kingdom = kingdomIt->second;
    
    // Find and remove member
    std::string playerName;
    for (auto it = kingdom.vecMembers.begin(); it != kingdom.vecMembers.end(); ++it)
    {
        if (it->dwPlayerID == dwPlayerID)
        {
            playerName = it->strName;
            kingdom.vecMembers.erase(it);
            break;
        }
    }
    
    // If this was the king and there are other members, promote someone
    if (kingdom.dwKingID == dwPlayerID && !kingdom.vecMembers.empty())
    {
        // Find highest ranking member to promote
        SKingdomMember* newKing = nullptr;
        BYTE highestRank = 0;
        
        for (auto& member : kingdom.vecMembers)
        {
            if (member.bRank >= highestRank)
            {
                highestRank = member.bRank;
                newKing = &member;
            }
        }
        
        if (newKing)
        {
            newKing->bRank = KINGDOM_RANK_KING;
            kingdom.dwKingID = newKing->dwPlayerID;
            SaveMemberToDB(dwKingdomID, *newKing);
            
            NotifyKingdomMembers(dwKingdomID, newKing->strName + " yeni kral oldu.");
        }
    }
    
    // If kingdom is empty, delete it
    if (kingdom.vecMembers.empty())
    {
        DeleteKingdom(dwKingdomID);
    }
    else
    {
        NotifyKingdomMembers(dwKingdomID, playerName + " krallıktan ayrıldı.");
    }
    
    // Remove from mappings and database
    m_mapPlayerKingdom.erase(playerIt);
    DeleteMemberFromDB(dwKingdomID, dwPlayerID);
    
    return true;
}

bool CKingdomManager::InvitePlayer(DWORD dwKingdomID, DWORD dwInviterID, const std::string& strPlayerName)
{
    // Check if inviter has permission
    SKingdomMember* inviter = GetKingdomMember(dwKingdomID, dwInviterID);
    if (!inviter || inviter->bRank < KINGDOM_RANK_OFFICER)
        return false;
    
    // Find target player
    LPCHARACTER targetCh = CHARACTER_MANAGER::instance().FindPC(strPlayerName.c_str());
    if (!targetCh)
        return false;
    
    // Check if target is already in a kingdom
    if (IsPlayerInKingdom(targetCh->GetPlayerID()))
        return false;
    
    // Send invite packet to target player
    SKingdom* kingdom = GetKingdom(dwKingdomID);
    if (!kingdom)
        return false;
    
    TPacketGCKingdomInvite packet;
    packet.bHeader = HEADER_GC_KINGDOM_INVITE;
    strncpy(packet.szKingdomName, kingdom->strName.c_str(), sizeof(packet.szKingdomName));
    strncpy(packet.szInviterName, inviter->strName.c_str(), sizeof(packet.szInviterName));
    packet.dwKingdomID = dwKingdomID;
    
    targetCh->GetDesc()->Packet(&packet, sizeof(packet));
    
    return true;
}

bool CKingdomManager::KickPlayer(DWORD dwKingdomID, DWORD dwKickerID, const std::string& strPlayerName)
{
    SKingdom* kingdom = GetKingdom(dwKingdomID);
    if (!kingdom)
        return false;
    
    // Find kicker and target
    SKingdomMember* kicker = nullptr;
    SKingdomMember* target = nullptr;
    
    for (auto& member : kingdom->vecMembers)
    {
        if (member.dwPlayerID == dwKickerID)
            kicker = &member;
        if (member.strName == strPlayerName)
            target = &member;
    }
    
    if (!kicker || !target)
        return false;
    
    // Check permissions
    if (kicker->bRank <= target->bRank)
        return false;
    
    // Cannot kick the king
    if (target->bRank == KINGDOM_RANK_KING)
        return false;
    
    // Remove target from kingdom
    DWORD targetPlayerID = target->dwPlayerID;
    
    for (auto it = kingdom->vecMembers.begin(); it != kingdom->vecMembers.end(); ++it)
    {
        if (it->dwPlayerID == targetPlayerID)
        {
            kingdom->vecMembers.erase(it);
            break;
        }
    }
    
    m_mapPlayerKingdom.erase(targetPlayerID);
    DeleteMemberFromDB(dwKingdomID, targetPlayerID);
    
    // Notify members
    NotifyKingdomMembers(dwKingdomID, strPlayerName + " krallıktan atıldı.");
    
    // Notify kicked player
    LPCHARACTER targetCh = CHARACTER_MANAGER::instance().FindByPID(targetPlayerID);
    if (targetCh)
    {
        TPacketGCKingdomJoinResult packet;
        packet.bHeader = HEADER_GC_KINGDOM_LEAVE_RESULT;
        packet.bResult = 1;
        strncpy(packet.szMessage, "Krallıktan atıldınız.", sizeof(packet.szMessage));
        targetCh->GetDesc()->Packet(&packet, sizeof(packet));
    }
    
    return true;
}

bool CKingdomManager::ChangePlayerRank(DWORD dwKingdomID, DWORD dwChangerID, const std::string& strPlayerName, BYTE bNewRank)
{
    SKingdom* kingdom = GetKingdom(dwKingdomID);
    if (!kingdom)
        return false;
    
    // Find changer and target
    SKingdomMember* changer = nullptr;
    SKingdomMember* target = nullptr;
    
    for (auto& member : kingdom->vecMembers)
    {
        if (member.dwPlayerID == dwChangerID)
            changer = &member;
        if (member.strName == strPlayerName)
            target = &member;
    }
    
    if (!changer || !target)
        return false;
    
    // Check permissions
    if (changer->bRank <= target->bRank || changer->bRank <= bNewRank)
        return false;
    
    // Cannot change king rank
    if (target->bRank == KINGDOM_RANK_KING || bNewRank == KINGDOM_RANK_KING)
        return false;
    
    // Update rank
    target->bRank = bNewRank;
    SaveMemberToDB(dwKingdomID, *target);
    
    // Notify members
    std::string rankName;
    switch (bNewRank)
    {
        case KINGDOM_RANK_MEMBER: rankName = "Üye"; break;
        case KINGDOM_RANK_OFFICER: rankName = "Subay"; break;
        case KINGDOM_RANK_COMMANDER: rankName = "Komutan"; break;
    }
    
    NotifyKingdomMembers(dwKingdomID, strPlayerName + " " + rankName + " rütbesine terfi etti.");
    
    return true;
}

bool CKingdomManager::UpdateKingdomSettings(DWORD dwKingdomID, DWORD dwPlayerID, 
                                           const std::string& strName, const std::string& strDescription,
                                           BYTE bColorR, BYTE bColorG, BYTE bColorB, BYTE bFlag)
{
    SKingdom* kingdom = GetKingdom(dwKingdomID);
    if (!kingdom)
        return false;
    
    // Check if player is king
    if (kingdom->dwKingID != dwPlayerID)
        return false;
    
    // Validate name
    if (!ValidateKingdomName(strName))
        return false;
    
    // Update settings
    kingdom->strName = strName;
    kingdom->strDescription = strDescription;
    kingdom->bColorR = bColorR;
    kingdom->bColorG = bColorG;
    kingdom->bColorB = bColorB;
    kingdom->bFlag = bFlag;
    
    SaveKingdomToDB(*kingdom);
    
    NotifyKingdomMembers(dwKingdomID, "Krallık ayarları güncellendi.");
    
    return true;
}

SKingdom* CKingdomManager::GetKingdom(DWORD dwKingdomID)
{
    auto it = m_mapKingdoms.find(dwKingdomID);
    return (it != m_mapKingdoms.end()) ? &it->second : nullptr;
}

SKingdom* CKingdomManager::GetPlayerKingdom(DWORD dwPlayerID)
{
    auto it = m_mapPlayerKingdom.find(dwPlayerID);
    if (it == m_mapPlayerKingdom.end())
        return nullptr;
    
    return GetKingdom(it->second);
}

std::vector<SKingdom*> CKingdomManager::GetAllKingdoms()
{
    std::vector<SKingdom*> kingdoms;
    for (auto& pair : m_mapKingdoms)
    {
        kingdoms.push_back(&pair.second);
    }
    return kingdoms;
}

SKingdomMember* CKingdomManager::GetKingdomMember(DWORD dwKingdomID, DWORD dwPlayerID)
{
    SKingdom* kingdom = GetKingdom(dwKingdomID);
    if (!kingdom)
        return nullptr;
    
    for (auto& member : kingdom->vecMembers)
    {
        if (member.dwPlayerID == dwPlayerID)
            return &member;
    }
    
    return nullptr;
}

bool CKingdomManager::IsPlayerInKingdom(DWORD dwPlayerID)
{
    return m_mapPlayerKingdom.find(dwPlayerID) != m_mapPlayerKingdom.end();
}

bool CKingdomManager::CanPlayerManage(DWORD dwKingdomID, DWORD dwPlayerID, DWORD dwTargetPlayerID)
{
    SKingdomMember* player = GetKingdomMember(dwKingdomID, dwPlayerID);
    SKingdomMember* target = GetKingdomMember(dwKingdomID, dwTargetPlayerID);
    
    if (!player || !target)
        return false;
    
    return player->bRank > target->bRank;
}

DWORD CKingdomManager::GetPlayerKingdomID(DWORD dwPlayerID)
{
    auto it = m_mapPlayerKingdom.find(dwPlayerID);
    return (it != m_mapPlayerKingdom.end()) ? it->second : 0;
}

void CKingdomManager::LoadKingdomsFromDB()
{
    // Load kingdoms
    std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
        "SELECT id, name, description, color_r, color_g, color_b, flag, create_time, king_id FROM kingdom"));
    
    if (pMsg->Get()->uiNumRows > 0)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != nullptr)
        {
            SKingdom kingdom;
            kingdom.dwKingdomID = strtoul(row[0], nullptr, 10);
            kingdom.strName = row[1];
            kingdom.strDescription = row[2] ? row[2] : "";
            kingdom.bColorR = strtoul(row[3], nullptr, 10);
            kingdom.bColorG = strtoul(row[4], nullptr, 10);
            kingdom.bColorB = strtoul(row[5], nullptr, 10);
            kingdom.bFlag = strtoul(row[6], nullptr, 10);
            kingdom.dwCreateTime = strtoul(row[7], nullptr, 10);
            kingdom.dwKingID = strtoul(row[8], nullptr, 10);
            
            m_mapKingdoms[kingdom.dwKingdomID] = kingdom;
            
            if (kingdom.dwKingdomID >= m_dwNextKingdomID)
                m_dwNextKingdomID = kingdom.dwKingdomID + 1;
        }
    }
    
    // Load members
    std::unique_ptr<SQLMsg> pMemberMsg(DBManager::instance().DirectQuery(
        "SELECT kingdom_id, player_id, player_name, rank, join_time FROM kingdom_member"));
    
    if (pMemberMsg->Get()->uiNumRows > 0)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(pMemberMsg->Get()->pSQLResult)) != nullptr)
        {
            DWORD dwKingdomID = strtoul(row[0], nullptr, 10);
            auto it = m_mapKingdoms.find(dwKingdomID);
            if (it != m_mapKingdoms.end())
            {
                SKingdomMember member;
                member.dwPlayerID = strtoul(row[1], nullptr, 10);
                member.strName = row[2];
                member.bRank = strtoul(row[3], nullptr, 10);
                member.dwJoinTime = strtoul(row[4], nullptr, 10);
                member.bOnline = false;  // Will be updated when player logs in
                
                it->second.vecMembers.push_back(member);
                m_mapPlayerKingdom[member.dwPlayerID] = dwKingdomID;
            }
        }
    }
}

void CKingdomManager::SaveKingdomToDB(const SKingdom& kingdom)
{
    DBManager::instance().DirectQuery(
        "REPLACE INTO kingdom (id, name, description, color_r, color_g, color_b, flag, create_time, king_id) "
        "VALUES (%u, '%s', '%s', %u, %u, %u, %u, %u, %u)",
        kingdom.dwKingdomID,
        kingdom.strName.c_str(),
        kingdom.strDescription.c_str(),
        kingdom.bColorR,
        kingdom.bColorG,
        kingdom.bColorB,
        kingdom.bFlag,
        kingdom.dwCreateTime,
        kingdom.dwKingID
    );
}

void CKingdomManager::SaveMemberToDB(DWORD dwKingdomID, const SKingdomMember& member)
{
    DBManager::instance().DirectQuery(
        "REPLACE INTO kingdom_member (kingdom_id, player_id, player_name, rank, join_time) "
        "VALUES (%u, %u, '%s', %u, %u)",
        dwKingdomID,
        member.dwPlayerID,
        member.strName.c_str(),
        member.bRank,
        member.dwJoinTime
    );
}

void CKingdomManager::DeleteMemberFromDB(DWORD dwKingdomID, DWORD dwPlayerID)
{
    DBManager::instance().DirectQuery(
        "DELETE FROM kingdom_member WHERE kingdom_id = %u AND player_id = %u",
        dwKingdomID, dwPlayerID
    );
}

DWORD CKingdomManager::GenerateKingdomID()
{
    return m_dwNextKingdomID++;
}

bool CKingdomManager::ValidateKingdomName(const std::string& strName)
{
    if (strName.length() < 3 || strName.length() > 20)
        return false;
    
    // Check for existing kingdom with same name
    for (const auto& pair : m_mapKingdoms)
    {
        if (pair.second.strName == strName)
            return false;
    }
    
    return true;
}

void CKingdomManager::NotifyKingdomMembers(DWORD dwKingdomID, const std::string& strMessage)
{
    SKingdom* kingdom = GetKingdom(dwKingdomID);
    if (!kingdom)
        return;
    
    for (const auto& member : kingdom->vecMembers)
    {
        LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(member.dwPlayerID);
        if (ch)
        {
            ch->ChatPacket(CHAT_TYPE_NOTICE, "[Krallık] %s", strMessage.c_str());
        }
    }
}
