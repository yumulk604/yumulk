#pragma once

#include "stdafx.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <time.h>

// Packet header definitions
#include "offlineshop_packets.h"

// Forward declarations
class COfflineShop;
class COfflineShopManager;

// Offline Shop System - Main Header File
class COfflineShop
{
public:
    struct SOfflineShopItem
    {
        BYTE bySlot;
        DWORD dwVnum;
        DWORD dwCount;
        DWORD dwPrice;
        long long lSockets[6];  
        TPlayerItemAttribute aAttr[7];
        
        SOfflineShopItem() : bySlot(0), dwVnum(0), dwCount(0), dwPrice(0)
        {
            memset(lSockets, 0, sizeof(lSockets));
            memset(aAttr, 0, sizeof(aAttr));
        }
    };

    // Constructor / Destructor
    COfflineShop(DWORD shopID, DWORD ownerPID, DWORD kingdomID, 
                 const char* shopName, const char* shopDesc,
                 long mapIndex, long x, long y);
    ~COfflineShop();

    // Basic getters
    DWORD GetShopID() const { return m_dwShopID; }
    DWORD GetOwnerPID() const { return m_dwOwnerPID; }
    DWORD GetKingdomID() const { return m_dwKingdomID; }
    const char* GetShopName() const { return m_strShopName.c_str(); }
    const char* GetShopDesc() const { return m_strShopDesc.c_str(); }
    long GetMapIndex() const { return m_lMapIndex; }
    long GetX() const { return m_lX; }
    long GetY() const { return m_lY; }
    
    // Status management
    bool IsOpen() const { return m_bIsOpen; }
    bool IsExpired() const;
    time_t GetExpireTime() const { return m_tExpireTime; }
    
    // Shop operations
    bool OpenShop();
    bool CloseShop();
    void ExtendTime(time_t additionalTime = 7 * 24 * 60 * 60); // 7 days default
    
    // Item management
    bool AddItem(BYTE slot, LPITEM pkItem, DWORD price);
    bool RemoveItem(BYTE slot);
    bool UpdateItem(BYTE slot, DWORD newPrice);
    const SOfflineShopItem* GetItem(BYTE slot) const;
    SOfflineShopItem* GetItem(BYTE slot);
    bool IsSlotEmpty(BYTE slot) const;
    bool IsValidSlot(BYTE slot) const { return slot < 40; }
    size_t GetItemCount() const { return m_Items.size(); }
    
    // Transaction management
    bool BuyItem(LPCHARACTER buyer, BYTE slot);
    void AddVisitor(DWORD playerID);
    void AddSaleTransaction(DWORD buyerPID, DWORD itemVnum, DWORD count, DWORD price);
    
    // Financial management
    DWORD GetTotalEarned() const { return m_dwTotalEarned; }
    DWORD GetTotalSales() const { return m_dwTotalSales; }
    bool WithdrawEarnings(LPCHARACTER owner, DWORD amount);
    
    // Database operations
    bool LoadFromDB();
    bool SaveToDB();
    bool UpdateDB();
    
    // Search and query support
    bool MatchesSearch(const char* keyword) const;
    bool IsInLocation(long mapIndex, long centerX, long centerY, long distance) const;

private:
    DWORD m_dwShopID;
    DWORD m_dwOwnerPID;
    DWORD m_dwKingdomID;
    std::string m_strShopName;
    std::string m_strShopDesc;
    long m_lMapIndex;
    long m_lX;
    long m_lY;
    bool m_bIsOpen;
    time_t m_tExpireTime;
    
    // Items
    std::map<BYTE, SOfflineShopItem> m_Items;
    
    // Statistics
    DWORD m_dwTotalEarned;
    DWORD m_dwTotalSales;
    
    // Timestamps
    time_t m_tCreatedTime;
    time_t m_tLastAccess;
    
    // Thread safety
    mutable std::mutex m_mtx;
};

class COfflineShopManager : public singleton_ex<COfflineShopManager>
{
public:
    COfflineShopManager();
    ~COfflineShopManager();
    
    // Initialization
    bool Initialize();
    void Destroy();
    
    // Shop creation and management
    COfflineShop* CreateShop(DWORD ownerPID, DWORD kingdomID, 
                            const char* shopName, const char* shopDesc,
                            long mapIndex, long x, long y);
    
    bool DeleteShop(DWORD shopID);
    COfflineShop* GetShop(DWORD shopID);
    
    // Permissions and constraints
    bool CanCreateShop(DWORD playerPID, DWORD kingdomID, long mapIndex) const;
    size_t GetPlayerShopCount(DWORD playerPID) const;
    size_t GetKingdomShopCount(DWORD kingdomID, long mapIndex) const;
    bool IsPlayerShopLimitReached(DWORD playerPID) const;
    bool IsKingdomShopLimitReached(DWORD kingdomID, long mapIndex) const;
    
    // Search and filters
    std::vector<COfflineShop*> GetPlayerShops(DWORD playerPID) const;
    std::vector<COfflineShop*> GetKingdomShops(DWORD kingdomID) const;
    std::vector<COfflineShop*> GetNearbyShops(long mapIndex, long x, long y, long distance) const;
    std::vector<COfflineShop*> SearchShops(const char* keyword) const;
    
    // Global queries
    std::vector<COfflineShop*> GetAllActiveShops() const;
    std::vector<COfflineShop*> GetExpiredShops() const;
    
    // Maintenance
    void CleanExpiredShops();
    void UpdateAllShops();
    
    // Statistics
    size_t GetTotalShopCount() const { return m_Shops.size(); }
    size_t GetActiveShopCount() const;
    
    // Kingdom coordinate validations
    bool IsValidShopLocation(DWORD kingdomID, long mapIndex, long x, long y) const;
    int GetRemainingShopsForKingdom(DWORD kingdomID, long mapIndex) const;
    int GetRemainingShopsForPlayer(DWORD playerPID) const;

private:
    std::map<DWORD, COfflineShop*> m_Shops;
    DWORD m_dwNextShopID;
    mutable std::mutex m_mtx;
    
    // Configuration
    static const size_t MAX_ITEMS_PER_SHOP = 40;
    static const size_t MAX_PLAYER_SHOPS = 3;
    static const size_t MAX_KINGDOM_SHOPS = 10;
    static const size_t MAX_SEARCH_DISTANCE = 5000;
    
    // Load from DB on startup
    bool LoadAllShops();
    bool SaveShopsToDB();
    
    // Kingdom integration
    bool SetupKingdomIntegration();
    
    // Internal helpers
    COfflineShop* FindShopByID(DWORD shopID) const;
};

// Integration functions
COfflineShop* GetOfflineShopByID(DWORD shopID);
bool HasOfflineShopPermissions(LPCCHARACTER ch);
bool IsOfflineShopEnabled();
DWORD GetPlayerKingdomID(DWORD playerPID);
bool IsLocationInKingdom(DWORD kingdomID, long mapIndex, long x, long y);