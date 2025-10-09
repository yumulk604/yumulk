#include "sage_skill_system.h"
#include "db.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "config.h"
#include "utils.h"
#include "log.h"
#include "desc.h"
#include "questmanager.h"
#include "affect.h"
#include "skill.h"
#include "kingdom.h"

// CSageSkillSystem Implementation
CSageSkillSystem::CSageSkillSystem()
{
    InitializeSageSkillBonuses();
}

CSageSkillSystem::~CSageSkillSystem()
{
    m_mapPlayerSageSkills.clear();
    m_mapUpgradeCooldowns.clear();
    m_mapSageSkillBonuses.clear();
}

CSageSkillSystem& CSageSkillSystem::Instance()
{
    static CSageSkillSystem instance;
    return instance;
}

void CSageSkillSystem::InitializeSageSkillBonuses()
{
    // Her Sage seviyesi için bonus değerlerini tanımla
    for (BYTE i = 1; i <= SAGE_SKILL_GRADE_MAX; ++i)
    {
        SSageSkillBonus bonus;
        
        // Hasar bonusu: Her seviyede %5 artış
        bonus.fDamageMultiplier = 1.0f + (i * 0.05f);
        
        // Cooldown azaltma: Her seviyede %2 azaltma
        bonus.fCooldownReduction = i * 0.02f;
        
        // Mana maliyet azaltma: Her seviyede %1.5 azaltma
        bonus.fManaReduction = i * 0.015f;
        
        // Özel yetenekler belirli seviyelerde açılır
        switch(i)
        {
            case 3:  bonus.bUnlockNewAbility = true; bonus.dwSpecialEffect = SAGE_ABILITY_CHAIN_LIGHTNING; break;
            case 5:  bonus.bUnlockNewAbility = true; bonus.dwSpecialEffect = SAGE_ABILITY_DOUBLE_CAST; break;
            case 7:  bonus.bUnlockNewAbility = true; bonus.dwSpecialEffect = SAGE_ABILITY_MANA_SHIELD; break;
            case 9:  bonus.bUnlockNewAbility = true; bonus.dwSpecialEffect = SAGE_ABILITY_TIME_MANIPULATION; break;
            case 10: bonus.bUnlockNewAbility = true; bonus.dwSpecialEffect = SAGE_ABILITY_REALITY_TEAR; break;
            default: bonus.bUnlockNewAbility = false; bonus.dwSpecialEffect = SAGE_ABILITY_NONE; break;
        }
        
        m_mapSageSkillBonuses[i] = bonus;
    }
}

bool CSageSkillSystem::CanUpgradeToSage(LPCHARACTER ch, DWORD dwSkillVnum)
{
    if (!ch)
        return false;
        
    // Karakter seviye kontrolü
    if (ch->GetLevel() < SAGE_MIN_CHARACTER_LEVEL)
        return false;
        
    // Skill Perfect seviyesinde mi kontrolü
    if (!IsSkillPerfect(ch, dwSkillVnum))
        return false;
        
    // Zaten Sage seviyesinde mi kontrolü
    if (IsSkillSage(ch, dwSkillVnum))
        return false;
        
    // Krallık gereksinimi kontrolü
#if SAGE_KINGDOM_REQUIREMENT_ENABLED
    if (!CheckKingdomRequirement(ch, SAGE_SKILL_GRADE_S1))
        return false;
#endif
        
    return true;
}

bool CSageSkillSystem::IsSkillPerfect(LPCHARACTER ch, DWORD dwSkillVnum)
{
    if (!ch)
        return false;
        
    // Mevcut skill sisteminden Perfect kontrolü
    BYTE bSkillGrade = ch->GetSkillGrade(dwSkillVnum);
    return (bSkillGrade == SKILL_PERFECT);
}

bool CSageSkillSystem::IsSkillSage(LPCHARACTER ch, DWORD dwSkillVnum)
{
    if (!ch)
        return false;
        
    DWORD dwPlayerID = ch->GetPlayerID();
    auto it = m_mapPlayerSageSkills.find(dwPlayerID);
    
    if (it != m_mapPlayerSageSkills.end())
    {
        return (it->second.dwSkillVnum == dwSkillVnum && it->second.bSageGrade > 0);
    }
    
    return false;
}

BYTE CSageSkillSystem::GetSageSkillGrade(LPCHARACTER ch, DWORD dwSkillVnum)
{
    if (!ch || !IsSkillSage(ch, dwSkillVnum))
        return SAGE_SKILL_GRADE_NONE;
        
    DWORD dwPlayerID = ch->GetPlayerID();
    auto it = m_mapPlayerSageSkills.find(dwPlayerID);
    
    if (it != m_mapPlayerSageSkills.end() && it->second.dwSkillVnum == dwSkillVnum)
    {
        return it->second.bSageGrade;
    }
    
    return SAGE_SKILL_GRADE_NONE;
}

CSageSkillSystem::ESageSkillError CSageSkillSystem::UpgradeSkillToSage(LPCHARACTER ch, DWORD dwSkillVnum, ESageSkillUpgradeType eType)
{
    if (!ch)
        return SAGE_ERROR_UNKNOWN;
        
    // Temel kontroller
    if (!IsSkillPerfect(ch, dwSkillVnum) && !IsSkillSage(ch, dwSkillVnum))
        return SAGE_ERROR_NOT_PERFECT;
        
    // Cooldown kontrolü
    if (IsUpgradeCooldownActive(ch))
        return SAGE_ERROR_COOLDOWN_ACTIVE;
        
    BYTE bCurrentGrade = GetSageSkillGrade(ch, dwSkillVnum);
    BYTE bTargetGrade = bCurrentGrade + 1;
    
    // Maksimum seviye kontrolü
    if (bCurrentGrade >= SAGE_SKILL_GRADE_MAX)
        return SAGE_ERROR_MAX_LEVEL;
        
    // Krallık gereksinimi kontrolü
    if (!CheckKingdomRequirement(ch, bTargetGrade))
        return SAGE_ERROR_INSUFFICIENT_KINGDOM;
        
    ESageSkillError eResult = SAGE_ERROR_UNKNOWN;
    
    // Yükseltme tipine göre işlem yap
    switch(eType)
    {
        case SAGE_UPGRADE_ANCIENT_SCROLL:
            eResult = UpgradeSkillWithAncientScroll(ch, dwSkillVnum);
            break;
            
        case SAGE_UPGRADE_SAGE_STONE:
            eResult = UpgradeSkillWithSageStone(ch, dwSkillVnum);
            break;
            
        case SAGE_UPGRADE_MEDITATION:
            eResult = UpgradeSkillWithMeditation(ch, dwSkillVnum);
            break;
            
        default:
            return SAGE_ERROR_UNKNOWN;
    }
    
    return eResult;
}

CSageSkillSystem::ESageSkillError CSageSkillSystem::UpgradeSkillWithAncientScroll(LPCHARACTER ch, DWORD dwSkillVnum)
{
    if (!HasAncientScroll(ch, dwSkillVnum))
        return SAGE_ERROR_NO_ANCIENT_SCROLL;
        
    BYTE bCurrentGrade = GetSageSkillGrade(ch, dwSkillVnum);
    BYTE bTargetGrade = bCurrentGrade + 1;
    
    // Başarı oranını hesapla
    float fSuccessRate = GetUpgradeSuccessRate(ch, dwSkillVnum, SAGE_UPGRADE_ANCIENT_SCROLL, bCurrentGrade);
    
    // Item tüket
    if (!ConsumeUpgradeItems(ch, dwSkillVnum, SAGE_UPGRADE_ANCIENT_SCROLL, bCurrentGrade))
        return SAGE_ERROR_NO_ANCIENT_SCROLL;
        
    // Cooldown uygula
    SetUpgradeCooldown(ch, SAGE_COOLDOWN_ANCIENT_SCROLL);
    
    // Başarı kontrolü
    bool bSuccess = RollUpgradeSuccess(fSuccessRate);
    
    DWORD dwPlayerID = ch->GetPlayerID();
    
    // Player sage skill data güncelle veya oluştur
    if (m_mapPlayerSageSkills.find(dwPlayerID) == m_mapPlayerSageSkills.end())
    {
        SSageSkillData newData;
        newData.dwSkillVnum = dwSkillVnum;
        newData.bSageGrade = bSuccess ? SAGE_SKILL_GRADE_S1 : SAGE_SKILL_GRADE_NONE;
        newData.tLastUpgradeTime = time(0);
        newData.dwUpgradeAttempts = 1;
        newData.dwSuccessfulUpgrades = bSuccess ? 1 : 0;
        newData.bUpgradeMethod = SAGE_UPGRADE_ANCIENT_SCROLL;
        
        m_mapPlayerSageSkills[dwPlayerID] = newData;
    }
    else
    {
        SSageSkillData& data = m_mapPlayerSageSkills[dwPlayerID];
        if (bSuccess)
            data.bSageGrade = bTargetGrade;
        data.tLastUpgradeTime = time(0);
        data.dwUpgradeAttempts++;
        if (bSuccess)
            data.dwSuccessfulUpgrades++;
        data.bUpgradeMethod = SAGE_UPGRADE_ANCIENT_SCROLL;
    }
    
    // Veritabanına kaydet
    SavePlayerSageSkills(ch);
    
    // Özel yetenek aç
    if (bSuccess && m_mapSageSkillBonuses[bTargetGrade].bUnlockNewAbility)
    {
        UnlockSageSpecialAbility(ch, dwSkillVnum, bTargetGrade);
    }
    
    // Log
    LogSageSkillUpgrade(ch, dwSkillVnum, bCurrentGrade, bSuccess ? bTargetGrade : bCurrentGrade, SAGE_UPGRADE_ANCIENT_SCROLL, bSuccess);
    
    // Başarı mesajı
    if (bSuccess)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Skill başarıyla Sage seviyesine yükseltildi! (S%d)", bTargetGrade);
        return SAGE_ERROR_NONE;
    }
    else
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Sage skill yükseltme başarısız oldu.");
        return SAGE_ERROR_UNKNOWN;
    }
}

CSageSkillSystem::ESageSkillError CSageSkillSystem::UpgradeSkillWithSageStone(LPCHARACTER ch, DWORD dwSkillVnum)
{
    BYTE bCurrentGrade = GetSageSkillGrade(ch, dwSkillVnum);
    
    if (!HasSageStone(ch, bCurrentGrade))
        return SAGE_ERROR_NO_SAGE_STONE;
        
    BYTE bTargetGrade = bCurrentGrade + 1;
    
    // Başarı oranını hesapla (Sage Stone ile daha yüksek oran)
    float fSuccessRate = GetUpgradeSuccessRate(ch, dwSkillVnum, SAGE_UPGRADE_SAGE_STONE, bCurrentGrade);
    
    // Item tüket
    if (!ConsumeUpgradeItems(ch, dwSkillVnum, SAGE_UPGRADE_SAGE_STONE, bCurrentGrade))
        return SAGE_ERROR_NO_SAGE_STONE;
        
    // Cooldown uygula
    SetUpgradeCooldown(ch, SAGE_COOLDOWN_SAGE_STONE);
    
    // Başarı kontrolü
    bool bSuccess = RollUpgradeSuccess(fSuccessRate);
    
    // Sage skill data güncelle
    DWORD dwPlayerID = ch->GetPlayerID();
    if (m_mapPlayerSageSkills.find(dwPlayerID) != m_mapPlayerSageSkills.end())
    {
        SSageSkillData& data = m_mapPlayerSageSkills[dwPlayerID];
        if (bSuccess)
            data.bSageGrade = bTargetGrade;
        data.tLastUpgradeTime = time(0);
        data.dwUpgradeAttempts++;
        if (bSuccess)
            data.dwSuccessfulUpgrades++;
        data.bUpgradeMethod = SAGE_UPGRADE_SAGE_STONE;
    }
    
    SavePlayerSageSkills(ch);
    
    if (bSuccess && m_mapSageSkillBonuses[bTargetGrade].bUnlockNewAbility)
    {
        UnlockSageSpecialAbility(ch, dwSkillVnum, bTargetGrade);
    }
    
    LogSageSkillUpgrade(ch, dwSkillVnum, bCurrentGrade, bSuccess ? bTargetGrade : bCurrentGrade, SAGE_UPGRADE_SAGE_STONE, bSuccess);
    
    if (bSuccess)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Bilge Taşı ile skill başarıyla yükseltildi! (S%d)", bTargetGrade);
        return SAGE_ERROR_NONE;
    }
    else
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Bilge Taşı ile yükseltme başarısız oldu.");
        return SAGE_ERROR_UNKNOWN;
    }
}

CSageSkillSystem::ESageSkillError CSageSkillSystem::UpgradeSkillWithMeditation(LPCHARACTER ch, DWORD dwSkillVnum)
{
    // Meditasyon özel bir yöntem - daha uzun sürer ama garantilidir
    if (IsMeditating(ch))
    {
        // Meditasyon tamamlandıysa yükselt
        return CompleteMeditation(ch, dwSkillVnum) ? SAGE_ERROR_NONE : SAGE_ERROR_MEDITATION_FAILED;
    }
    else
    {
        // Meditasyon başlat
        return StartMeditation(ch, dwSkillVnum) ? SAGE_ERROR_NONE : SAGE_ERROR_MEDITATION_FAILED;
    }
}

float CSageSkillSystem::GetUpgradeSuccessRate(LPCHARACTER ch, DWORD dwSkillVnum, ESageSkillUpgradeType eType, BYTE bCurrentGrade)
{
    float fBaseRate = 0.0f;
    
    // Yönteme göre temel oran
    switch(eType)
    {
        case SAGE_UPGRADE_ANCIENT_SCROLL:
            fBaseRate = 0.30f - (bCurrentGrade * 0.02f); // %30'dan başlayıp her seviyede %2 azalır
            break;
            
        case SAGE_UPGRADE_SAGE_STONE:
            fBaseRate = 0.50f - (bCurrentGrade * 0.03f); // %50'den başlayıp her seviyede %3 azalır
            break;
            
        case SAGE_UPGRADE_MEDITATION:
            fBaseRate = 1.0f; // Meditasyon garantili ama uzun sürer
            break;
            
        default:
            fBaseRate = 0.10f;
            break;
    }
    
    // Karakter seviyesi bonusu
    float fLevelBonus = (ch->GetLevel() - SAGE_MIN_CHARACTER_LEVEL) * 0.001f;
    
    // Krallık seviyesi bonusu
    float fKingdomBonus = 0.0f;
    if (ch->GetKingdomID() > 0)
    {
        // Kingdom seviyesi ne kadar yüksekse o kadar bonus
        fKingdomBonus = GetRequiredKingdomLevel(bCurrentGrade + 1) * 0.01f;
    }
    
    float fFinalRate = fBaseRate + fLevelBonus + fKingdomBonus;
    
    // Minimum %5, maksimum %95 sınırı
    return std::max(0.05f, std::min(0.95f, fFinalRate));
}

bool CSageSkillSystem::RollUpgradeSuccess(float fSuccessRate)
{
    return (number(1, 10000) <= (int)(fSuccessRate * 10000));
}

bool CSageSkillSystem::HasAncientScroll(LPCHARACTER ch, DWORD dwSkillVnum)
{
    if (!ch)
        return false;
        
    // Karakter classına göre doğru antik rulo tipini kontrol et
    DWORD dwScrollVnum = 0;
    
    switch(ch->GetJob())
    {
        case JOB_WARRIOR:
            dwScrollVnum = SAGE_ITEM_ANCIENT_SCROLL_WARRIOR;
            break;
        case JOB_ASSASSIN:
            dwScrollVnum = SAGE_ITEM_ANCIENT_SCROLL_NINJA;
            break;
        case JOB_SURA:
            dwScrollVnum = SAGE_ITEM_ANCIENT_SCROLL_SURA;
            break;
        case JOB_SHAMAN:
            dwScrollVnum = SAGE_ITEM_ANCIENT_SCROLL_SHAMAN;
            break;
        case JOB_WOLFMAN:
            dwScrollVnum = SAGE_ITEM_ANCIENT_SCROLL_WOLFMAN;
            break;
        default:
            return false;
    }
    
    return ch->CountSpecifyItem(dwScrollVnum) > 0;
}

bool CSageSkillSystem::HasSageStone(LPCHARACTER ch, BYTE bGrade)
{
    if (!ch)
        return false;
        
    DWORD dwStoneVnum = 0;
    
    // Grade'e göre gerekli taş tipini belirle
    if (bGrade <= 3)
        dwStoneVnum = SAGE_ITEM_SAGE_STONE_LOW;
    else if (bGrade <= 6)
        dwStoneVnum = SAGE_ITEM_SAGE_STONE_MID;
    else if (bGrade <= 9)
        dwStoneVnum = SAGE_ITEM_SAGE_STONE_HIGH;
    else
        dwStoneVnum = SAGE_ITEM_SAGE_STONE_SUPREME;
        
    return ch->CountSpecifyItem(dwStoneVnum) > 0;
}

bool CSageSkillSystem::ConsumeUpgradeItems(LPCHARACTER ch, DWORD dwSkillVnum, ESageSkillUpgradeType eType, BYTE bGrade)
{
    if (!ch)
        return false;
        
    switch(eType)
    {
        case SAGE_UPGRADE_ANCIENT_SCROLL:
        {
            DWORD dwScrollVnum = 0;
            switch(ch->GetJob())
            {
                case JOB_WARRIOR:  dwScrollVnum = SAGE_ITEM_ANCIENT_SCROLL_WARRIOR; break;
                case JOB_ASSASSIN: dwScrollVnum = SAGE_ITEM_ANCIENT_SCROLL_NINJA; break;
                case JOB_SURA:     dwScrollVnum = SAGE_ITEM_ANCIENT_SCROLL_SURA; break;
                case JOB_SHAMAN:   dwScrollVnum = SAGE_ITEM_ANCIENT_SCROLL_SHAMAN; break;
                case JOB_WOLFMAN:  dwScrollVnum = SAGE_ITEM_ANCIENT_SCROLL_WOLFMAN; break;
                default: return false;
            }
            return ch->RemoveSpecifyItem(dwScrollVnum, 1);
        }
        
        case SAGE_UPGRADE_SAGE_STONE:
        {
            DWORD dwStoneVnum = 0;
            if (bGrade <= 3)
                dwStoneVnum = SAGE_ITEM_SAGE_STONE_LOW;
            else if (bGrade <= 6)
                dwStoneVnum = SAGE_ITEM_SAGE_STONE_MID;
            else if (bGrade <= 9)
                dwStoneVnum = SAGE_ITEM_SAGE_STONE_HIGH;
            else
                dwStoneVnum = SAGE_ITEM_SAGE_STONE_SUPREME;
                
            return ch->RemoveSpecifyItem(dwStoneVnum, 1);
        }
        
        case SAGE_UPGRADE_MEDITATION:
            // Meditasyon item gerektirmez ama zaman gerektirir
            return true;
            
        default:
            return false;
    }
}

bool CSageSkillSystem::CheckKingdomRequirement(LPCHARACTER ch, BYTE bTargetGrade)
{
#if !SAGE_KINGDOM_REQUIREMENT_ENABLED
    return true;
#endif
    
    if (!ch || ch->GetKingdomID() == 0)
        return false;
        
    DWORD dwRequiredLevel = GetRequiredKingdomLevel(bTargetGrade);
    
    // Kingdom seviyesi kontrolü - kingdom.cpp'den alınacak
    // Geçici olarak true dönüyoruz
    return true;
}

DWORD CSageSkillSystem::GetRequiredKingdomLevel(BYTE bTargetGrade)
{
    // Her 2 Sage seviyesinde 1 Kingdom seviyesi gereksinimi
    return SAGE_MIN_KINGDOM_LEVEL + (bTargetGrade / 2);
}

void CSageSkillSystem::UnlockSageSpecialAbility(LPCHARACTER ch, DWORD dwSkillVnum, BYTE bGrade)
{
    if (!ch)
        return;
        
    DWORD dwAbilityID = m_mapSageSkillBonuses[bGrade].dwSpecialEffect;
    if (dwAbilityID == SAGE_ABILITY_NONE)
        return;
        
    switch(dwAbilityID)
    {
        case SAGE_ABILITY_CHAIN_LIGHTNING:
            ch->ChatPacket(CHAT_TYPE_INFO, "Özel Yetenek Açıldı: Zincir Şimşek!");
            break;
            
        case SAGE_ABILITY_DOUBLE_CAST:
            ch->ChatPacket(CHAT_TYPE_INFO, "Özel Yetenek Açıldı: Çift Büyü!");
            break;
            
        case SAGE_ABILITY_MANA_SHIELD:
            ch->ChatPacket(CHAT_TYPE_INFO, "Özel Yetenek Açıldı: Mana Kalkanı!");
            break;
            
        case SAGE_ABILITY_TIME_MANIPULATION:
            ch->ChatPacket(CHAT_TYPE_INFO, "Özel Yetenek Açıldı: Zaman Manipülasyonu!");
            break;
            
        case SAGE_ABILITY_REALITY_TEAR:
            ch->ChatPacket(CHAT_TYPE_INFO, "Özel Yetenek Açıldı: Gerçeklik Yırtığı!");
            break;
    }
    
    // Quest tetikle
    quest::CQuestManager::instance().Letter(ch->GetPlayerID(), "sage_ability_unlocked", dwAbilityID);
}

bool CSageSkillSystem::LoadPlayerSageSkills(LPCHARACTER ch)
{
    if (!ch)
        return false;
        
    char szQuery[512];
    snprintf(szQuery, sizeof(szQuery),
        "SELECT skill_vnum, sage_grade, last_upgrade_time, upgrade_attempts, successful_upgrades, upgrade_method "
        "FROM sage_skills WHERE player_id = %u", ch->GetPlayerID());
        
    std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
    if (!pMsg->Get()->uiNumRows)
        return false;
        
    MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
    if (!row)
        return false;
        
    SSageSkillData data;
    data.dwSkillVnum = strtoul(row[0], NULL, 10);
    data.bSageGrade = strtoul(row[1], NULL, 10);
    data.tLastUpgradeTime = strtoul(row[2], NULL, 10);
    data.dwUpgradeAttempts = strtoul(row[3], NULL, 10);
    data.dwSuccessfulUpgrades = strtoul(row[4], NULL, 10);
    data.bUpgradeMethod = strtoul(row[5], NULL, 10);
    
    m_mapPlayerSageSkills[ch->GetPlayerID()] = data;
    return true;
}

bool CSageSkillSystem::SavePlayerSageSkills(LPCHARACTER ch)
{
    if (!ch)
        return false;
        
    DWORD dwPlayerID = ch->GetPlayerID();
    auto it = m_mapPlayerSageSkills.find(dwPlayerID);
    
    if (it == m_mapPlayerSageSkills.end())
        return false;
        
    const SSageSkillData& data = it->second;
    
    char szQuery[512];
    snprintf(szQuery, sizeof(szQuery),
        "INSERT INTO sage_skills (player_id, skill_vnum, sage_grade, last_upgrade_time, upgrade_attempts, successful_upgrades, upgrade_method) "
        "VALUES (%u, %u, %u, %lu, %u, %u, %u) "
        "ON DUPLICATE KEY UPDATE "
        "sage_grade = VALUES(sage_grade), last_upgrade_time = VALUES(last_upgrade_time), "
        "upgrade_attempts = VALUES(upgrade_attempts), successful_upgrades = VALUES(successful_upgrades), "
        "upgrade_method = VALUES(upgrade_method)",
        dwPlayerID, data.dwSkillVnum, data.bSageGrade, data.tLastUpgradeTime,
        data.dwUpgradeAttempts, data.dwSuccessfulUpgrades, data.bUpgradeMethod);
        
    DBManager::instance().DirectQuery(szQuery);
    return true;
}

void CSageSkillSystem::LogSageSkillUpgrade(LPCHARACTER ch, DWORD dwSkillVnum, BYTE bOldGrade, BYTE bNewGrade, ESageSkillUpgradeType eType, bool bSuccess)
{
    if (!ch)
        return;
        
    char szBuffer[256];
    snprintf(szBuffer, sizeof(szBuffer), "SAGE_UPGRADE: Skill=%u OldGrade=S%u NewGrade=S%u Method=%d Success=%s",
        dwSkillVnum, bOldGrade, bNewGrade, eType, bSuccess ? "TRUE" : "FALSE");
        
    LogManager::instance().CharLog(ch, 0, "SAGE_SKILL", szBuffer);
}

// Global Functions
void InitializeSageSkillSystem()
{
    CSageSkillSystem::Instance();
    sys_log(0, "Sage Skill System initialized.");
}

void DestroySageSkillSystem()
{
    sys_log(0, "Sage Skill System destroyed.");
}

bool IsSageSkillSystemEnabled()
{
    return SAGE_SYSTEM_ENABLED;
}