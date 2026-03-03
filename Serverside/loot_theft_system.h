#ifndef __HEADER_LOOT_THEFT_SYSTEM_H__
#define __HEADER_LOOT_THEFT_SYSTEM_H__

#include "char.h"
#include "item.h"

// Basit hırsızlık sistemi:
// 1) PvP sırasında yang çalma efsunu
// 2) Yere düşen itemi owner koruma süresi bitince çalma mekaniği
// 3) Hırsız karakter her başarılı hırsızlıkta daha "zalim" olur (alignment düşer)

class CLootTheftSystem
{
public:
    enum ETheftResult
    {
        THEFT_RESULT_OK = 0,
        THEFT_RESULT_INVALID_TARGET = 1,
        THEFT_RESULT_INSUFFICIENT_YANG = 2,
        THEFT_RESULT_PROTECTED_ITEM = 3,
        THEFT_RESULT_CHANCE_FAILED = 4,
        THEFT_RESULT_NOT_ALLOWED = 5,
    };

    static CLootTheftSystem& Instance();

    // Yang çalma efsunu (ör: saldırıda proc)
    ETheftResult TryStealYang(LPCHARACTER pkThief, LPCHARACTER pkVictim, BYTE bBaseChance, BYTE bStealPercent);

    // Yere düşen item çalma mekaniği
    ETheftResult TryStealGroundItem(LPCHARACTER pkThief, LPITEM pkItem, DWORD dwOwnerPID, time_t tDropTime, DWORD dwProtectionSec);

    // Zalimlik cezası (alignment negatife gider)
    void ApplyCrueltyPenalty(LPCHARACTER pkThief, long lPenaltyValue);

private:
    CLootTheftSystem() {}
    ~CLootTheftSystem() {}

    bool RollChance(BYTE bChance) const;
    bool IsValidPvPTheft(LPCHARACTER pkThief, LPCHARACTER pkVictim) const;
};

// Önerilen balans sabitleri
#define THEFT_MIN_LEVEL 35
#define THEFT_MAX_YANG_PERCENT 8
#define THEFT_DEFAULT_ITEM_PROTECTION 10
#define THEFT_ALIGNMENT_PENALTY_YANG 800
#define THEFT_ALIGNMENT_PENALTY_ITEM 1500

#endif
