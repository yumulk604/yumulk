#include "offlineshop_packets.h"
#include "offlineshop.h"
#include "char.h"
#include "desc.h"
#include "buffer_manager.h"
#include "packet.h"
#include "config.h"
#include "utils.h"
#include "log.h"
#include "db.h"

// Packet handler fonksiyonları
void COfflineShopPacketHandler::HandleCreateShop(LPCHARACTER ch, const char* data)
{
    if (!ch || !data)
        return;
        
    const TPacketCGOfflineShopCreate* packet = reinterpret_cast<const TPacketCGOfflineShopCreate*>(data);
    
    // Güvenlik kontrolleri
    if (!ch->GetDesc())
        return;
        
    DWORD dwPlayerPID = ch->GetPlayerID();
    DWORD dwKingdomID = ch->GetKingdomID();
    
    // Kingdom kontrolü
    if (dwKingdomID == 0)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_NO_KINGDOM);
        return;
    }
    
    // Konum kontrolü - Kingdom alanında mı?
    long lMapIndex = ch->GetMapIndex();
    long lX = ch->GetX();
    long lY = ch->GetY();
    
    if (!IsValidShopLocation(ch, lMapIndex, lX, lY))
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_INVALID_LOCATION);
        return;
    }
    
    // Shop sayısı kontrolü
    if (!COfflineShopManager::Instance().CanCreateShop(dwPlayerPID, dwKingdomID, lMapIndex))
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_SHOP_LIMIT);
        return;
    }
    
    // Shop oluştur
    COfflineShop* pShop = COfflineShopManager::Instance().CreateShop(
        dwPlayerPID, dwKingdomID, packet->szShopName, packet->szShopDesc, 
        lMapIndex, lX, lY);
        
    if (!pShop)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_CREATE_FAILED);
        return;
    }
    
    // Başarı paketi gönder
    TPacketGCOfflineShopCreateResult resultPacket;
    resultPacket.bHeader = HEADER_GC_OFFLINESHOP_CREATE_RESULT;
    resultPacket.dwShopID = pShop->GetShopID();
    resultPacket.bResult = OFFLINESHOP_CREATE_SUCCESS;
    
    ch->GetDesc()->Packet(&resultPacket, sizeof(resultPacket));
    
    // Log
    LogManager::instance().CharLog(ch, 0, "OFFLINESHOP_CREATE", pShop->GetShopName());
    
    chat.AppendChat(chat.CHAT_TYPE_INFO, "Offline shop başarıyla oluşturuldu!");
}

void COfflineShopPacketHandler::HandleOpenShop(LPCHARACTER ch, const char* data)
{
    if (!ch || !data)
        return;
        
    const TPacketCGOfflineShopOpen* packet = reinterpret_cast<const TPacketCGOfflineShopOpen*>(data);
    
    COfflineShop* pShop = COfflineShopManager::Instance().GetShop(packet->dwShopID);
    if (!pShop)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_SHOP_NOT_FOUND);
        return;
    }
    
    // Sahiplik kontrolü
    if (pShop->GetOwnerPID() != ch->GetPlayerID())
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_NOT_OWNER);
        return;
    }
    
    // Shop zaten açık mı?
    if (pShop->IsOpen())
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_ALREADY_OPEN);
        return;
    }
    
    // En az bir item var mı?
    if (pShop->GetItemCount() == 0)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_NO_ITEMS);
        return;
    }
    
    // Shop'ı aç
    if (pShop->OpenShop())
    {
        TPacketGCOfflineShopResult resultPacket;
        resultPacket.bHeader = HEADER_GC_OFFLINESHOP_RESULT;
        resultPacket.bResult = OFFLINESHOP_RESULT_SUCCESS;
        resultPacket.dwShopID = packet->dwShopID;
        
        ch->GetDesc()->Packet(&resultPacket, sizeof(resultPacket));
        
        ch->ChatPacket(CHAT_TYPE_INFO, "Shop açıldı!");
        
        // Log
        LogManager::instance().CharLog(ch, packet->dwShopID, "OFFLINESHOP_OPEN", "");
    }
    else
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_OPEN_FAILED);
    }
}

void COfflineShopPacketHandler::HandleCloseShop(LPCHARACTER ch, const char* data)
{
    if (!ch || !data)
        return;
        
    const TPacketCGOfflineShopClose* packet = reinterpret_cast<const TPacketCGOfflineShopClose*>(data);
    
    COfflineShop* pShop = COfflineShopManager::Instance().GetShop(packet->dwShopID);
    if (!pShop)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_SHOP_NOT_FOUND);
        return;
    }
    
    // Sahiplik kontrolü
    if (pShop->GetOwnerPID() != ch->GetPlayerID())
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_NOT_OWNER);
        return;
    }
    
    // Shop'ı kapat
    if (pShop->CloseShop())
    {
        TPacketGCOfflineShopResult resultPacket;
        resultPacket.bHeader = HEADER_GC_OFFLINESHOP_RESULT;
        resultPacket.bResult = OFFLINESHOP_RESULT_SUCCESS;
        resultPacket.dwShopID = packet->dwShopID;
        
        ch->GetDesc()->Packet(&resultPacket, sizeof(resultPacket));
        
        ch->ChatPacket(CHAT_TYPE_INFO, "Shop kapatıldı!");
        
        // Log
        LogManager::instance().CharLog(ch, packet->dwShopID, "OFFLINESHOP_CLOSE", "");
    }
    else
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_CLOSE_FAILED);
    }
}

void COfflineShopPacketHandler::HandleAddItem(LPCHARACTER ch, const char* data)
{
    if (!ch || !data)
        return;
        
    const TPacketCGOfflineShopAddItem* packet = reinterpret_cast<const TPacketCGOfflineShopAddItem*>(data);
    
    COfflineShop* pShop = COfflineShopManager::Instance().GetShop(packet->dwShopID);
    if (!pShop)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_SHOP_NOT_FOUND);
        return;
    }
    
    // Sahiplik kontrolü
    if (pShop->GetOwnerPID() != ch->GetPlayerID())
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_NOT_OWNER);
        return;
    }
    
    // Shop açık iken item eklenemez
    if (pShop->IsOpen())
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_SHOP_OPEN);
        return;
    }
    
    // Slot kontrolü
    if (!pShop->IsValidSlot(packet->bySlot) || !pShop->IsSlotEmpty(packet->bySlot))
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_INVALID_SLOT);
        return;
    }
    
    // Item kontrolü
    LPITEM item = ch->GetInventoryItem(packet->byItemSlot);
    if (!item)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_ITEM_NOT_FOUND);
        return;
    }
    
    // Fiyat kontrolü
    if (packet->dwPrice == 0 || packet->dwPrice > 2000000000) // 2 milyar gold limit
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_INVALID_PRICE);
        return;
    }
    
    // Item ekle
    if (pShop->AddItem(packet->bySlot, item, packet->dwPrice))
    {
        TPacketGCOfflineShopResult resultPacket;
        resultPacket.bHeader = HEADER_GC_OFFLINESHOP_RESULT;
        resultPacket.bResult = OFFLINESHOP_RESULT_SUCCESS;
        resultPacket.dwShopID = packet->dwShopID;
        
        ch->GetDesc()->Packet(&resultPacket, sizeof(resultPacket));
        
        ch->ChatPacket(CHAT_TYPE_INFO, "Item shop'a eklendi!");
        
        // Log
        LogManager::instance().ItemLog(ch, item, "OFFLINESHOP_ADD", packet->dwPrice);
    }
    else
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_ADD_ITEM_FAILED);
    }
}

void COfflineShopPacketHandler::HandleRemoveItem(LPCHARACTER ch, const char* data)
{
    if (!ch || !data)
        return;
        
    const TPacketCGOfflineShopRemoveItem* packet = reinterpret_cast<const TPacketCGOfflineShopRemoveItem*>(data);
    
    COfflineShop* pShop = COfflineShopManager::Instance().GetShop(packet->dwShopID);
    if (!pShop)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_SHOP_NOT_FOUND);
        return;
    }
    
    // Sahiplik kontrolü
    if (pShop->GetOwnerPID() != ch->GetPlayerID())
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_NOT_OWNER);
        return;
    }
    
    // Shop açık iken item kaldırılamaz
    if (pShop->IsOpen())
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_SHOP_OPEN);
        return;
    }
    
    // Item var mı?
    COfflineShop::SOfflineShopItem* pItem = pShop->GetItem(packet->bySlot);
    if (!pItem)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_ITEM_NOT_FOUND);
        return;
    }
    
    // Inventory'de yer var mı?
    if (!ch->GetInventory().IsEmptySpace(1)) // Basit kontrol, gerçekte item boyutuna göre olmalı
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_INVENTORY_FULL);
        return;
    }
    
    // Item'i geri ver
    LPITEM returnItem = ITEM_MANAGER::instance().CreateItem(pItem->dwVnum, pItem->dwCount);
    if (returnItem)
    {
        memcpy(returnItem->GetSockets(), pItem->lSockets, sizeof(pItem->lSockets));
        memcpy(returnItem->GetAttributes(), pItem->aAttr, sizeof(pItem->aAttr));
        
        ch->AutoGiveItem(returnItem);
        
        // Shop'tan kaldır
        pShop->RemoveItem(packet->bySlot);
        
        TPacketGCOfflineShopResult resultPacket;
        resultPacket.bHeader = HEADER_GC_OFFLINESHOP_RESULT;
        resultPacket.bResult = OFFLINESHOP_RESULT_SUCCESS;
        resultPacket.dwShopID = packet->dwShopID;
        
        ch->GetDesc()->Packet(&resultPacket, sizeof(resultPacket));
        
        ch->ChatPacket(CHAT_TYPE_INFO, "Item shop'tan kaldırıldı!");
        
        // Log
        LogManager::instance().ItemLog(ch, returnItem, "OFFLINESHOP_REMOVE", 0);
    }
    else
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_REMOVE_ITEM_FAILED);
    }
}

void COfflineShopPacketHandler::HandleBuyItem(LPCHARACTER ch, const char* data)
{
    if (!ch || !data)
        return;
        
    const TPacketCGOfflineShopBuyItem* packet = reinterpret_cast<const TPacketCGOfflineShopBuyItem*>(data);
    
    COfflineShop* pShop = COfflineShopManager::Instance().GetShop(packet->dwShopID);
    if (!pShop)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_SHOP_NOT_FOUND);
        return;
    }
    
    // Shop açık mı?
    if (!pShop->IsOpen())
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_SHOP_CLOSED);
        return;
    }
    
    // Kendi shop'ından alamaz
    if (pShop->GetOwnerPID() == ch->GetPlayerID())
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_CANNOT_BUY_OWN);
        return;
    }
    
    // Item satın al
    if (pShop->BuyItem(ch, packet->bySlot))
    {
        TPacketGCOfflineShopBuyResult resultPacket;
        resultPacket.bHeader = HEADER_GC_OFFLINESHOP_BUY_RESULT;
        resultPacket.bResult = OFFLINESHOP_BUY_SUCCESS;
        resultPacket.dwShopID = packet->dwShopID;
        resultPacket.bySlot = packet->bySlot;
        
        ch->GetDesc()->Packet(&resultPacket, sizeof(resultPacket));
        
        // Shop sahibine bildirim gönder (online ise)
        LPCHARACTER owner = CHARACTER_MANAGER::instance().FindByPID(pShop->GetOwnerPID());
        if (owner && owner->GetDesc())
        {
            owner->ChatPacket(CHAT_TYPE_INFO, "%s shop'unuzdan bir item satın aldı!", ch->GetName());
        }
    }
    else
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_BUY_FAILED);
    }
}

void COfflineShopPacketHandler::HandleViewShop(LPCHARACTER ch, const char* data)
{
    if (!ch || !data)
        return;
        
    const TPacketCGOfflineShopView* packet = reinterpret_cast<const TPacketCGOfflineShopView*>(data);
    
    COfflineShop* pShop = COfflineShopManager::Instance().GetShop(packet->dwShopID);
    if (!pShop)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_SHOP_NOT_FOUND);
        return;
    }
    
    // Shop bilgilerini gönder
    SendShopInfo(ch, pShop);
    SendShopItems(ch, pShop);
    
    // Ziyaretci olarak kaydet (sahip değilse)
    if (pShop->GetOwnerPID() != ch->GetPlayerID())
    {
        pShop->AddVisitor(ch->GetPlayerID());
    }
}

void COfflineShopPacketHandler::HandleSearchShops(LPCHARACTER ch, const char* data)
{
    if (!ch || !data)
        return;
        
    const TPacketCGOfflineShopSearch* packet = reinterpret_cast<const TPacketCGOfflineShopSearch*>(data);
    
    std::string keyword(packet->szKeyword);
    if (keyword.length() < 2)
    {
        SendErrorPacket(ch, OFFLINESHOP_ERROR_INVALID_SEARCH);
        return;
    }
    
    std::vector<COfflineShop*> results = COfflineShopManager::Instance().SearchShops(keyword.c_str());
    
    // Sonuçları gönder
    SendSearchResults(ch, results);
}

void COfflineShopPacketHandler::HandleListShops(LPCHARACTER ch, const char* data)
{
    if (!ch || !data)
        return;
        
    const TPacketCGOfflineShopList* packet = reinterpret_cast<const TPacketCGOfflineShopList*>(data);
    
    std::vector<COfflineShop*> shops;
    
    switch(packet->bListType)
    {
        case OFFLINESHOP_LIST_MY_SHOPS:
            shops = COfflineShopManager::Instance().GetPlayerShops(ch->GetPlayerID());
            break;
            
        case OFFLINESHOP_LIST_KINGDOM_SHOPS:
            shops = COfflineShopManager::Instance().GetKingdomShops(ch->GetKingdomID());
            break;
            
        case OFFLINESHOP_LIST_NEARBY_SHOPS:
            shops = COfflineShopManager::Instance().GetNearbyShops(
                ch->GetMapIndex(), ch->GetX(), ch->GetY(), 5000);
            break;
            
        default:
            SendErrorPacket(ch, OFFLINESHOP_ERROR_INVALID_REQUEST);
            return;
    }
    
    // Liste gönder
    SendShopList(ch, shops, packet->bListType);
}

// Yardımcı fonksiyonlar
void COfflineShopPacketHandler::SendErrorPacket(LPCHARACTER ch, BYTE bErrorCode)
{
    if (!ch || !ch->GetDesc())
        return;
        
    TPacketGCOfflineShopError errorPacket;
    errorPacket.bHeader = HEADER_GC_OFFLINESHOP_ERROR;
    errorPacket.bErrorCode = bErrorCode;
    
    ch->GetDesc()->Packet(&errorPacket, sizeof(errorPacket));
}

void COfflineShopPacketHandler::SendShopInfo(LPCHARACTER ch, COfflineShop* pShop)
{
    if (!ch || !ch->GetDesc() || !pShop)
        return;
        
    TPacketGCOfflineShopInfo infoPacket;
    infoPacket.bHeader = HEADER_GC_OFFLINESHOP_SHOP_INFO;
    infoPacket.dwShopID = pShop->GetShopID();
    infoPacket.dwOwnerPID = pShop->GetOwnerPID();
    
    // Sahip adını bul
    LPCHARACTER owner = CHARACTER_MANAGER::instance().FindByPID(pShop->GetOwnerPID());
    if (owner)
    {
        strlcpy(infoPacket.szOwnerName, owner->GetName(), sizeof(infoPacket.szOwnerName));
    }
    else
    {
        // DB'den al
        char szQuery[256];
        snprintf(szQuery, sizeof(szQuery), "SELECT name FROM player WHERE id = %u", pShop->GetOwnerPID());
        std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
        if (pMsg->Get()->uiNumRows > 0)
        {
            MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
            strlcpy(infoPacket.szOwnerName, row[0], sizeof(infoPacket.szOwnerName));
        }
        else
        {
            strlcpy(infoPacket.szOwnerName, "Unknown", sizeof(infoPacket.szOwnerName));
        }
    }
    
    strlcpy(infoPacket.szShopName, pShop->GetShopName(), sizeof(infoPacket.szShopName));
    strlcpy(infoPacket.szShopDesc, pShop->GetShopDesc(), sizeof(infoPacket.szShopDesc));
    infoPacket.dwItemCount = pShop->GetItemCount();
    infoPacket.tExpireTime = pShop->GetExpireTime();
    infoPacket.bIsOwner = (pShop->GetOwnerPID() == ch->GetPlayerID()) ? true : false;
    
    ch->GetDesc()->Packet(&infoPacket, sizeof(infoPacket));
}

void COfflineShopPacketHandler::SendShopItems(LPCHARACTER ch, COfflineShop* pShop)
{
    if (!ch || !ch->GetDesc() || !pShop)
        return;
        
    const std::map<BYTE, COfflineShop::SOfflineShopItem>& items = pShop->GetItems();
    
    for (const auto& pair : items)
    {
        const COfflineShop::SOfflineShopItem& item = pair.second;
        
        TPacketGCOfflineShopItem itemPacket;
        itemPacket.bySlot = item.bySlot;
        itemPacket.dwVnum = item.dwVnum;
        itemPacket.dwCount = item.dwCount;
        itemPacket.dwPrice = item.dwPrice;
        memcpy(itemPacket.lSockets, item.lSockets, sizeof(itemPacket.lSockets));
        memcpy(itemPacket.aAttr, item.aAttr, sizeof(itemPacket.aAttr));
        
        ch->GetDesc()->Packet(&itemPacket, sizeof(itemPacket));
    }
}

bool COfflineShopPacketHandler::IsValidShopLocation(LPCHARACTER ch, long lMapIndex, long lX, long lY)
{
    // Kingdom alanı kontrolü
    DWORD dwKingdomID = ch->GetKingdomID();
    if (dwKingdomID == 0)
        return false;
        
    // Kingdom coordinate kontrolü (kingdom.cpp'den alınabilir)
    // Bu kısım kingdom sistemindeki coordinate kontrol fonksiyonu ile entegre edilmeli
    
    return true; // Şimdilik true, gerçek implementasyonda kingdom koordinat kontrolü yapılacak
}