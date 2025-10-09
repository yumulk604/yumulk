#ifndef __HEADER_SAGE_SKILL_SYSTEM_H__
#define __HEADER_SAGE_SKILL_SYSTEM_H__

#include "../../common/CommonDefines.h"
#include "char.h"
#include "skill.h"

// Sage Skill System - Perfect seviyesinden sonra gelen yeni skill sistemi
// Normal -> Master -> Grand Master -> Perfect -> SAGE

class CSageSkillSystem
{
public:
    enum ESageSkillGrade
    {
        SAGE_SKILL_GRADE_NONE = 0,
        SAGE_SKILL_GRADE_S1 = 1,   // Sage Level 1
        SAGE_SKILL_GRADE_S2 = 2,   // Sage Level 2
        SAGE_SKILL_GRADE_S3 = 3,   // Sage Level 3
        SAGE_SKILL_GRADE_S4 = 4,   // Sage Level 4
        SAGE_SKILL_GRADE_S5 = 5,   // Sage Level 5
        SAGE_SKILL_GRADE_S6 = 6,   // Sage Level 6
        SAGE_SKILL_GRADE_S7 = 7,   // Sage Level 7
        SAGE_SKILL_GRADE_S8 = 8,   // Sage Level 8
        SAGE_SKILL_GRADE_S9 = 9,   // Sage Level 9
        SAGE_SKILL_GRADE_S10 = 10, // Sage Level 10 (Max)
        SAGE_SKILL_GRADE_MAX = SAGE_SKILL_GRADE_S10
    };
    
    enum ESageSkillUpgradeType
    {
        SAGE_UPGRADE_NONE = 0,
        SAGE_UPGRADE_ANCIENT_SCROLL = 1,    // Antik Rulo ile yükseltme
        SAGE_UPGRADE_SAGE_STONE = 2,        // Bilge Taşı ile yükseltme  
        SAGE_UPGRADE_MEDITATION = 3,        // Meditasyon ile yükseltme
        SAGE_UPGRADE_QUEST_REWARD = 4       // Quest ödülü ile yükseltme
    };
    
    enum ESageSkillError
    {
        SAGE_ERROR_NONE = 0,
        SAGE_ERROR_NOT_PERFECT = 1,          // Skill Perfect değil
        SAGE_ERROR_ALREADY_SAGE = 2,         // Zaten Sage seviyesinde
        SAGE_ERROR_MAX_LEVEL = 3,            // Maksimum seviye
        SAGE_ERROR_INSUFFICIENT_LEVEL = 4,   // Yetersiz karakter seviyesi
        SAGE_ERROR_INSUFFICIENT_KINGDOM = 5,  // Krallık gereksinimi
        SAGE_ERROR_NO_ANCIENT_SCROLL = 6,    // Antik rulo yok
        SAGE_ERROR_NO_SAGE_STONE = 7,        // Bilge taşı yok
        SAGE_ERROR_COOLDOWN_ACTIVE = 8,      // Cooldown aktif
        SAGE_ERROR_MEDITATION_FAILED = 9,    // Meditasyon başarısız
        SAGE_ERROR_UNKNOWN = 10
    };
    
    struct SSageSkillData
    {
        DWORD dwSkillVnum;              // Skill vnum
        BYTE bSageGrade;                // Sage seviyesi (1-10)
        time_t tLastUpgradeTime;        // Son yükseltme zamanı
        DWORD dwUpgradeAttempts;        // Toplam yükseltme denemesi
        DWORD dwSuccessfulUpgrades;     // Başarılı yükseltme sayısı
        BYTE bUpgradeMethod;            // Son kullanılan yükseltme yöntemi
        
        SSageSkillData()
        {
            dwSkillVnum = 0;
            bSageGrade = 0;
            tLastUpgradeTime = 0;
            dwUpgradeAttempts = 0;
            dwSuccessfulUpgrades = 0;
            bUpgradeMethod = SAGE_UPGRADE_NONE;
        }
    };
    
    struct SSageSkillBonus
    {
        float fDamageMultiplier;        // Hasar çarpanı
        float fCooldownReduction;       // Cooldown azaltma
        float fManaReduction;           // Mana maliyeti azaltma
        DWORD dwSpecialEffect;          // Özel efekt ID
        bool bUnlockNewAbility;         // Yeni yetenek açılır mı
        
        SSageSkillBonus()
        {
            fDamageMultiplier = 1.0f;
            fCooldownReduction = 0.0f;
            fManaReduction = 0.0f;
            dwSpecialEffect = 0;
            bUnlockNewAbility = false;
        }
    };

private:
    std::map<DWORD, SSageSkillData> m_mapPlayerSageSkills; // PlayerID -> SageSkillData map
    std::map<DWORD, time_t> m_mapUpgradeCooldowns;        // PlayerID -> Cooldown map
    std::map<BYTE, SSageSkillBonus> m_mapSageSkillBonuses; // Grade -> Bonus map
    
public:
    CSageSkillSystem();
    ~CSageSkillSystem();
    
    // Singleton
    static CSageSkillSystem& Instance();
    
    // Sage sistem kontrolü
    bool CanUpgradeToSage(LPCHARACTER ch, DWORD dwSkillVnum);
    bool IsSkillPerfect(LPCHARACTER ch, DWORD dwSkillVnum);
    bool IsSkillSage(LPCHARACTER ch, DWORD dwSkillVnum);
    BYTE GetSageSkillGrade(LPCHARACTER ch, DWORD dwSkillVnum);
    
    // Sage yükseltme fonksiyonları
    ESageSkillError UpgradeSkillToSage(LPCHARACTER ch, DWORD dwSkillVnum, ESageSkillUpgradeType eType);
    ESageSkillError UpgradeSkillWithAncientScroll(LPCHARACTER ch, DWORD dwSkillVnum);
    ESageSkillError UpgradeSkillWithSageStone(LPCHARACTER ch, DWORD dwSkillVnum);
    ESageSkillError UpgradeSkillWithMeditation(LPCHARACTER ch, DWORD dwSkillVnum);
    
    // Sage bonus hesaplamaları
    SSageSkillBonus GetSageSkillBonus(BYTE bGrade);
    float CalculateSageDamageBonus(LPCHARACTER ch, DWORD dwSkillVnum);
    float CalculateSageCooldownReduction(LPCHARACTER ch, DWORD dwSkillVnum);
    float CalculateSageManaReduction(LPCHARACTER ch, DWORD dwSkillVnum);
    
    // Sage sistem yönetimi
    bool LoadPlayerSageSkills(LPCHARACTER ch);
    bool SavePlayerSageSkills(LPCHARACTER ch);
    void InitializeSageSkillBonuses();
    
    // Cooldown yönetimi
    bool IsUpgradeCooldownActive(LPCHARACTER ch);
    time_t GetUpgradeCooldownRemaining(LPCHARACTER ch);
    void SetUpgradeCooldown(LPCHARACTER ch, DWORD dwCooldownSeconds);
    void ClearUpgradeCooldown(LPCHARACTER ch);
    
    // Kingdom gereksinimleri
    bool CheckKingdomRequirement(LPCHARACTER ch, BYTE bTargetGrade);
    DWORD GetRequiredKingdomLevel(BYTE bTargetGrade);
    
    // Item gereksinimleri
    bool HasAncientScroll(LPCHARACTER ch, DWORD dwSkillVnum);
    bool HasSageStone(LPCHARACTER ch, BYTE bGrade);
    bool ConsumeUpgradeItems(LPCHARACTER ch, DWORD dwSkillVnum, ESageSkillUpgradeType eType, BYTE bGrade);
    
    // Başarı oranları
    float GetUpgradeSuccessRate(LPCHARACTER ch, DWORD dwSkillVnum, ESageSkillUpgradeType eType, BYTE bCurrentGrade);
    bool RollUpgradeSuccess(float fSuccessRate);
    
    // Özel yetenekler
    void UnlockSageSpecialAbility(LPCHARACTER ch, DWORD dwSkillVnum, BYTE bGrade);
    bool HasSageSpecialAbility(LPCHARACTER ch, DWORD dwSkillVnum, DWORD dwAbilityID);
    
    // Meditasyon sistemi
    bool StartMeditation(LPCHARACTER ch, DWORD dwSkillVnum);
    bool CompleteMeditation(LPCHARACTER ch, DWORD dwSkillVnum);
    void CancelMeditation(LPCHARACTER ch);
    bool IsMeditating(LPCHARACTER ch);
    
    // İstatistik ve bilgi
    void SendSageSkillInfo(LPCHARACTER ch, DWORD dwSkillVnum);
    DWORD GetTotalSageSkillCount(LPCHARACTER ch);
    DWORD GetHighestSageGrade(LPCHARACTER ch);
    
    // Log ve debug
    void LogSageSkillUpgrade(LPCHARACTER ch, DWORD dwSkillVnum, BYTE bOldGrade, BYTE bNewGrade, ESageSkillUpgradeType eType, bool bSuccess);
    void SendDebugInfo(LPCHARACTER ch);
};

// Sage Skill Item VNUMs
enum ESageSkillItems
{
    // Antik Rulolar (Skill grubuna göre)
    SAGE_ITEM_ANCIENT_SCROLL_WARRIOR = 90001,     // Savaşçı Antik Rulosu
    SAGE_ITEM_ANCIENT_SCROLL_NINJA = 90002,       // Ninja Antik Rulosu
    SAGE_ITEM_ANCIENT_SCROLL_SURA = 90003,        // Sura Antik Rulosu
    SAGE_ITEM_ANCIENT_SCROLL_SHAMAN = 90004,      // Şaman Antik Rulosu
    SAGE_ITEM_ANCIENT_SCROLL_WOLFMAN = 90005,     // Kurt Adam Antik Rulosu
    
    // Bilge Taşları (Grade'e göre)
    SAGE_ITEM_SAGE_STONE_LOW = 90010,             // Düşük Seviye Bilge Taşı (S1-S3)
    SAGE_ITEM_SAGE_STONE_MID = 90011,             // Orta Seviye Bilge Taşı (S4-S6)
    SAGE_ITEM_SAGE_STONE_HIGH = 90012,            // Yüksek Seviye Bilge Taşı (S7-S9)
    SAGE_ITEM_SAGE_STONE_SUPREME = 90013,         // Üstün Bilge Taşı (S10)
    
    // Meditasyon İtemleri
    SAGE_ITEM_MEDITATION_BOOK = 90020,             // Meditasyon Kitabı
    SAGE_ITEM_INCENSE_STICK = 90021,              // Tütsü Çubuğu
    SAGE_ITEM_MEDITATION_MAT = 90022,              // Meditasyon Matı
    
    // Özel Sage İtemleri
    SAGE_ITEM_SAGE_CRYSTAL = 90030,               // Bilge Kristali
    SAGE_ITEM_ANCIENT_WISDOM = 90031,             // Antik Bilgelik
    SAGE_ITEM_SAGE_BLESSING = 90032               // Bilge Kutsaması
};

// Sage Skill Cooldowns (saniye)
enum ESageCooldowns
{
    SAGE_COOLDOWN_ANCIENT_SCROLL = 86400,         // 24 saat
    SAGE_COOLDOWN_SAGE_STONE = 43200,             // 12 saat
    SAGE_COOLDOWN_MEDITATION = 21600,             // 6 saat
    SAGE_COOLDOWN_QUEST_REWARD = 604800           // 7 gün
};

// Sage Special Abilities
enum ESageSpecialAbilities
{
    SAGE_ABILITY_NONE = 0,
    SAGE_ABILITY_CHAIN_LIGHTNING = 1,             // S3: Zincir şimşek
    SAGE_ABILITY_DOUBLE_CAST = 2,                 // S5: Çift büyü
    SAGE_ABILITY_MANA_SHIELD = 3,                 // S7: Mana kalkanı
    SAGE_ABILITY_TIME_MANIPULATION = 4,           // S9: Zaman manipülasyonu
    SAGE_ABILITY_REALITY_TEAR = 5                 // S10: Gerçeklik yırtığı
};

// Network Packets
enum ESagePacketHeaders
{
    HEADER_CG_SAGE_SKILL_UPGRADE = 220,
    HEADER_CG_SAGE_SKILL_INFO = 221,
    HEADER_CG_SAGE_MEDITATION_START = 222,
    HEADER_CG_SAGE_MEDITATION_COMPLETE = 223,
    
    HEADER_GC_SAGE_SKILL_RESULT = 220,
    HEADER_GC_SAGE_SKILL_INFO = 221,
    HEADER_GC_SAGE_SKILL_BONUS = 222,
    HEADER_GC_SAGE_MEDITATION_STATUS = 223
};

// Packet Structures
typedef struct SPacketCGSageSkillUpgrade
{
    BYTE bHeader;
    DWORD dwSkillVnum;
    BYTE bUpgradeType;      // ESageSkillUpgradeType
} TPacketCGSageSkillUpgrade;

typedef struct SPacketGCSageSkillResult
{
    BYTE bHeader;
    DWORD dwSkillVnum;
    BYTE bResult;           // ESageSkillError
    BYTE bNewGrade;
    BYTE bUpgradeType;
} TPacketGCSageSkillResult;

typedef struct SPacketGCSageSkillInfo
{
    BYTE bHeader;
    DWORD dwSkillVnum;
    BYTE bSageGrade;
    float fDamageBonus;
    float fCooldownReduction;
    float fManaReduction;
    DWORD dwSpecialAbilities;
    time_t tLastUpgrade;
    DWORD dwTotalAttempts;
    DWORD dwSuccessfulUpgrades;
} TPacketGCSageSkillInfo;

// Global Functions
void InitializeSageSkillSystem();
void DestroySageSkillSystem();
bool IsSageSkillSystemEnabled();

// Character Extension Functions
namespace SageSkillExt
{
    bool HasAnySageSkill(LPCHARACTER ch);
    DWORD GetSageSkillCount(LPCHARACTER ch);
    float GetTotalSageDamageBonus(LPCHARACTER ch);
    void ApplySageSkillBonuses(LPCHARACTER ch);
    void RemoveSageSkillBonuses(LPCHARACTER ch);
}

// Config Definitions
#define SAGE_SYSTEM_ENABLED 1
#define SAGE_MIN_CHARACTER_LEVEL 90
#define SAGE_KINGDOM_REQUIREMENT_ENABLED 1
#define SAGE_MIN_KINGDOM_LEVEL 5
#define SAGE_MAX_DAILY_ATTEMPTS 3
#define SAGE_MEDITATION_TIME 1800  // 30 dakika

#endif // __HEADER_SAGE_SKILL_SYSTEM_H__