#include "loot_theft_system.h"

#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "log.h"
#include "utils.h"

CLootTheftSystem& CLootTheftSystem::Instance()
{
    static CLootTheftSystem s_theftSystem;
    return s_theftSystem;
}

bool CLootTheftSystem::RollChance(BYTE bChance) const
{
    if (bChance == 0)
        return false;

    if (bChance >= 100)
        return true;

    return (number(1, 100) <= bChance);
}

bool CLootTheftSystem::IsValidPvPTheft(LPCHARACTER pkThief, LPCHARACTER pkVictim) const
{
    if (!pkThief || !pkVictim)
        return false;

    if (pkThief == pkVictim)
        return false;

    if (pkThief->GetLevel() < THEFT_MIN_LEVEL)
        return false;

    if (pkVictim->IsNPC() || pkVictim->IsDead())
        return false;

    return true;
}

CLootTheftSystem::ETheftResult CLootTheftSystem::TryStealYang(LPCHARACTER pkThief, LPCHARACTER pkVictim, BYTE bBaseChance, BYTE bStealPercent)
{
    if (!IsValidPvPTheft(pkThief, pkVictim))
        return THEFT_RESULT_INVALID_TARGET;

    if (!RollChance(bBaseChance))
        return THEFT_RESULT_CHANCE_FAILED;

    if (bStealPercent == 0)
        bStealPercent = 1;

    if (bStealPercent > THEFT_MAX_YANG_PERCENT)
        bStealPercent = THEFT_MAX_YANG_PERCENT;

    const long lVictimYang = pkVictim->GetGold();
    if (lVictimYang <= 0)
        return THEFT_RESULT_INSUFFICIENT_YANG;

    long lStealAmount = (lVictimYang * bStealPercent) / 100;
    if (lStealAmount <= 0)
        lStealAmount = 1;

    pkVictim->PointChange(POINT_GOLD, -lStealAmount, true);
    pkThief->PointChange(POINT_GOLD, lStealAmount, true);

    ApplyCrueltyPenalty(pkThief, THEFT_ALIGNMENT_PENALTY_YANG);

    pkThief->ChatPacket(CHAT_TYPE_INFO, "Efsun aktif: %ld Yang çaldın. Zalimliğin arttı.", lStealAmount);
    pkVictim->ChatPacket(CHAT_TYPE_INFO, "%s senden %ld Yang çaldı!", pkThief->GetName(), lStealAmount);

    LogManager::instance().CharLog(pkThief, pkVictim->GetPlayerID(), "YANG_THEFT", "amount %ld", lStealAmount);
    return THEFT_RESULT_OK;
}

CLootTheftSystem::ETheftResult CLootTheftSystem::TryStealGroundItem(LPCHARACTER pkThief, LPITEM pkItem, DWORD dwOwnerPID, time_t tDropTime, DWORD dwProtectionSec)
{
    if (!pkThief || !pkItem)
        return THEFT_RESULT_NOT_ALLOWED;

    if (pkThief->GetLevel() < THEFT_MIN_LEVEL)
        return THEFT_RESULT_NOT_ALLOWED;

    if (dwProtectionSec == 0)
        dwProtectionSec = THEFT_DEFAULT_ITEM_PROTECTION;

    const time_t tNow = time(0);
    if (dwOwnerPID != 0 && pkThief->GetPlayerID() != dwOwnerPID)
    {
        if ((tNow - tDropTime) < static_cast<time_t>(dwProtectionSec))
            return THEFT_RESULT_PROTECTED_ITEM;
    }

    if (!pkThief->CanHandleItem())
        return THEFT_RESULT_NOT_ALLOWED;

    if (!pkThief->AutoGiveItem(pkItem))
        return THEFT_RESULT_NOT_ALLOWED;

    ApplyCrueltyPenalty(pkThief, THEFT_ALIGNMENT_PENALTY_ITEM);

    pkThief->ChatPacket(CHAT_TYPE_INFO, "Yerdeki itemi çaldın: %s. Zalimlik puanın arttı.", pkItem->GetName());
    LogManager::instance().ItemLog(pkThief, pkItem, "GROUND_ITEM_THEFT", pkItem->GetName());

    return THEFT_RESULT_OK;
}

void CLootTheftSystem::ApplyCrueltyPenalty(LPCHARACTER pkThief, long lPenaltyValue)
{
    if (!pkThief || lPenaltyValue <= 0)
        return;

    // Alignment düştükçe karakter daha zalim olur.
    pkThief->PointChange(POINT_ALIGNMENT, -lPenaltyValue);

    // İsteğe bağlı: sabit bir alt sınır koymak isteyenler burada kısıtlayabilir.
}
