#ifndef __INC_OFFLINE_SHOP_H__
#define __INC_OFFLINE_SHOP_H__

#include "shop_manager.h"

struct TOfflineShopItem
{
    DWORD vnum;
    long price;
    BYTE count;
    DWORD seller_id;
};

class COfflineShop
{
public:
    COfflineShop();
    bool    Create(LPCHARACTER owner, const std::vector<TOfflineShopItem>& items);
    bool    Buy(LPCHARACTER ch, BYTE pos);

private:
    std::vector<TOfflineShopItem> m_items;
    DWORD m_ownerPID;
};

class COfflineShopManager : public singleton<COfflineShopManager>
{
public:
    COfflineShopManager();
    COfflineShop* Get(DWORD ownerPID);
    void    AddShop(DWORD ownerPID, COfflineShop* pkShop);
    void    RemoveShop(DWORD ownerPID);

private:
    std::map<DWORD, COfflineShop*> m_map_pkShop;
};

#endif
