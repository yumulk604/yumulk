#include "stdafx.h"
#include "offline_shop.h"
#include "char.h"

COfflineShop::COfflineShop() : m_ownerPID(0)
{
}

bool COfflineShop::Create(LPCHARACTER owner, const std::vector<TOfflineShopItem>& items)
{
    if (!owner)
        return false;
    m_ownerPID = owner->GetPlayerID();
    m_items = items;
    return true;
}

bool COfflineShop::Buy(LPCHARACTER ch, BYTE pos)
{
    if (pos >= m_items.size())
        return false;
    TOfflineShopItem& rItem = m_items[pos];
    // TODO: item creation and gold transfer
    return true;
}

COfflineShopManager::COfflineShopManager()
{
}

COfflineShop* COfflineShopManager::Get(DWORD ownerPID)
{
    auto it = m_map_pkShop.find(ownerPID);
    if (it == m_map_pkShop.end())
        return nullptr;
    return it->second;
}

void COfflineShopManager::AddShop(DWORD ownerPID, COfflineShop* pkShop)
{
    m_map_pkShop[ownerPID] = pkShop;
}

void COfflineShopManager::RemoveShop(DWORD ownerPID)
{
    m_map_pkShop.erase(ownerPID);
}
