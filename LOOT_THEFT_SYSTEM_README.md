# Loot Theft System (Yang + Ground Item Theft)

Bu ekleme oyuna kısa bir **hırsızlık mekaniği** kazandırır:

1. **Yang Çalma Efsunu**
   - PvP sırasında proc olduğunda hedefin yang'ının %1-%8 arası çalınır.
   - Başarılı çalmada hırsızın alignment değeri düşer (daha **zalim** olur).

2. **Yerdeki Itemi Çalma**
   - Item düşürüldükten sonra owner koruma süresi geçerse başka oyuncu itemi alabilir.
   - Çalan oyuncu yine alignment cezası alır.

## Dosyalar
- `Serverside/loot_theft_system.h`
- `Serverside/loot_theft_system.cpp`

## Entegrasyon (kısa)

### 1) Yang efsunu tetikleme örneği
PvP hit çözümünde (örn. damage apply sonrası):

```cpp
// örnek değerler: %20 proc, kurbanın yang'ının %4'ü
CLootTheftSystem::Instance().TryStealYang(attacker, victim, 20, 4);
```

### 2) Ground item alma noktasında kontrol
Yerden item alma fonksiyonunda:

```cpp
CLootTheftSystem::ETheftResult res =
    CLootTheftSystem::Instance().TryStealGroundItem(ch, item, itemOwnerPID, itemDropTime, 10);

if (res == CLootTheftSystem::THEFT_RESULT_PROTECTED_ITEM)
{
    ch->ChatPacket(CHAT_TYPE_INFO, "Bu item hâlâ sahibi tarafından korunuyor.");
    return;
}
```

## Balans sabitleri
Header içinde hızlı tuning için:
- `THEFT_MIN_LEVEL`
- `THEFT_MAX_YANG_PERCENT`
- `THEFT_DEFAULT_ITEM_PROTECTION`
- `THEFT_ALIGNMENT_PENALTY_YANG`
- `THEFT_ALIGNMENT_PENALTY_ITEM`

## Not
Bu sistem PvP ekonomisini etkiler; canlıya almadan önce oran ve cezaları test shard'ında dengelemeniz önerilir.
