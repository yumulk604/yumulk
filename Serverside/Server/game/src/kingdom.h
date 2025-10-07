#ifndef __INC_KINGDOM_H__
#define __INC_KINGDOM_H__

#include <string>
#include <vector>
#include <map>

// Kingdom member ranks
enum EKingdomRank
{
    KINGDOM_RANK_MEMBER = 0,
    KINGDOM_RANK_OFFICER = 1,
    KINGDOM_RANK_COMMANDER = 2,
    KINGDOM_RANK_KING = 3
};

// Kingdom member structure
struct SKingdomMember
{
    DWORD dwPlayerID;
    std::string strName;
    BYTE bRank;
    DWORD dwJoinTime;
    bool bOnline;
    
    SKingdomMember() : dwPlayerID(0), bRank(KINGDOM_RANK_MEMBER), dwJoinTime(0), bOnline(false) {}
};

// Kingdom land/territory structure
struct SKingdomLand
{
    DWORD dwMapIndex;     // Map ID where kingdom is located
    long lCenterX, lCenterY;  // Center coordinates of kingdom land
    long lSpawnX, lSpawnY;    // Spawn point for kingdom members
    DWORD dwLandSize;     // Size of kingdom territory (radius)
    bool bIsPrivate;      // Is this a private kingdom land?
    
    SKingdomLand() : dwMapIndex(0), lCenterX(0), lCenterY(0), 
                     lSpawnX(0), lSpawnY(0), dwLandSize(1000), bIsPrivate(true) {}
};

// Kingdom structure
struct SKingdom
{
    DWORD dwKingdomID;
    std::string strName;
    std::string strDescription;
    BYTE bColorR, bColorG, bColorB;
    BYTE bFlag;
    DWORD dwCreateTime;
    DWORD dwKingID;  // Player ID of the king
    std::vector<SKingdomMember> vecMembers;
    
    // Kingdom land/territory info
    SKingdomLand landInfo;
    
    // Kingdom buildings and structures
    std::vector<DWORD> vecBuildings;  // Building IDs in this kingdom
    
    // Kingdom resources
    DWORD dwGold;         // Kingdom treasury
    DWORD dwWood;         // Wood resources
    DWORD dwStone;        // Stone resources
    DWORD dwIron;         // Iron resources
    
    SKingdom() : dwKingdomID(0), bColorR(255), bColorG(255), bColorB(255), 
                 bFlag(0), dwCreateTime(0), dwKingID(0), dwGold(0), 
                 dwWood(0), dwStone(0), dwIron(0) {}
};

// Kingdom management class
class CKingdomManager : public singleton<CKingdomManager>
{
public:
    CKingdomManager();
    virtual ~CKingdomManager();
    
    // Kingdom operations
    bool CreateKingdom(DWORD dwPlayerID, const std::string& strName, 
                      BYTE bColorR, BYTE bColorG, BYTE bColorB, BYTE bFlag);
    bool DeleteKingdom(DWORD dwKingdomID);
    bool JoinKingdom(DWORD dwPlayerID, DWORD dwKingdomID);
    bool LeaveKingdom(DWORD dwPlayerID);
    
    // Member management
    bool InvitePlayer(DWORD dwKingdomID, DWORD dwInviterID, const std::string& strPlayerName);
    bool KickPlayer(DWORD dwKingdomID, DWORD dwKickerID, const std::string& strPlayerName);
    bool ChangePlayerRank(DWORD dwKingdomID, DWORD dwChangerID, const std::string& strPlayerName, BYTE bNewRank);
    
    // Kingdom settings
    bool UpdateKingdomSettings(DWORD dwKingdomID, DWORD dwPlayerID, 
                              const std::string& strName, const std::string& strDescription,
                              BYTE bColorR, BYTE bColorG, BYTE bColorB, BYTE bFlag);
    
    // Information retrieval
    SKingdom* GetKingdom(DWORD dwKingdomID);
    SKingdom* GetPlayerKingdom(DWORD dwPlayerID);
    std::vector<SKingdom*> GetAllKingdoms();
    SKingdomMember* GetKingdomMember(DWORD dwKingdomID, DWORD dwPlayerID);
    
    // Utility functions
    bool IsPlayerInKingdom(DWORD dwPlayerID);
    bool CanPlayerManage(DWORD dwKingdomID, DWORD dwPlayerID, DWORD dwTargetPlayerID);
    DWORD GetPlayerKingdomID(DWORD dwPlayerID);
    
    // Land/Territory management
    bool AssignKingdomLand(DWORD dwKingdomID, DWORD dwMapIndex, long lX, long lY);
    bool TeleportToKingdom(DWORD dwPlayerID, DWORD dwKingdomID);
    bool TeleportToOwnKingdom(DWORD dwPlayerID);
    SKingdomLand* GetKingdomLand(DWORD dwKingdomID);
    bool IsInKingdomTerritory(DWORD dwKingdomID, long lX, long lY);
    std::vector<DWORD> GetAvailableLandSlots();
    
    // Resource management
    bool AddKingdomResource(DWORD dwKingdomID, DWORD dwGold, DWORD dwWood, DWORD dwStone, DWORD dwIron);
    bool SpendKingdomResource(DWORD dwKingdomID, DWORD dwGold, DWORD dwWood, DWORD dwStone, DWORD dwIron);
    bool HasEnoughResources(DWORD dwKingdomID, DWORD dwGold, DWORD dwWood, DWORD dwStone, DWORD dwIron);
    
    // Database operations
    void LoadKingdomsFromDB();
    void SaveKingdomToDB(const SKingdom& kingdom);
    void SaveMemberToDB(DWORD dwKingdomID, const SKingdomMember& member);
    void DeleteMemberFromDB(DWORD dwKingdomID, DWORD dwPlayerID);
    
private:
    std::map<DWORD, SKingdom> m_mapKingdoms;  // Kingdom ID -> Kingdom data
    std::map<DWORD, DWORD> m_mapPlayerKingdom;  // Player ID -> Kingdom ID
    std::map<DWORD, bool> m_mapLandSlots;  // Land slot ID -> is occupied
    DWORD m_dwNextKingdomID;
    
    // Land configuration
    struct SLandSlot {
        DWORD dwMapIndex;
        long lCenterX, lCenterY;
        long lSpawnX, lSpawnY;
        bool bOccupied;
        
        SLandSlot() : dwMapIndex(0), lCenterX(0), lCenterY(0), 
                      lSpawnX(0), lSpawnY(0), bOccupied(false) {}
        SLandSlot(DWORD map, long cx, long cy, long sx, long sy) 
            : dwMapIndex(map), lCenterX(cx), lCenterY(cy), 
              lSpawnX(sx), lSpawnY(sy), bOccupied(false) {}
    };
    
    std::vector<SLandSlot> m_vecLandSlots;  // Available land slots for kingdoms
    
    // Helper functions
    DWORD GenerateKingdomID();
    bool ValidateKingdomName(const std::string& strName);
    void NotifyKingdomMembers(DWORD dwKingdomID, const std::string& strMessage);
    void InitializeLandSlots();  // Initialize available land positions
    DWORD FindAvailableLandSlot();  // Find next available land slot
};

// Packet structures for kingdom system
#pragma pack(1)

// Client to Server packets
struct TPacketCGCreateKingdom
{
    BYTE bHeader;
    DWORD dwNameLength;
    char szName[20];
    BYTE bColorR, bColorG, bColorB;
    BYTE bFlag;
};

struct TPacketCGJoinKingdom
{
    BYTE bHeader;
    DWORD dwKingdomID;
};

struct TPacketCGLeaveKingdom
{
    BYTE bHeader;
};

struct TPacketCGRequestKingdomList
{
    BYTE bHeader;
};

struct TPacketCGKingdomInvite
{
    BYTE bHeader;
    char szPlayerName[20];
};

struct TPacketCGKingdomKick
{
    BYTE bHeader;
    char szPlayerName[20];
};

struct TPacketCGKingdomRank
{
    BYTE bHeader;
    char szPlayerName[20];
    BYTE bRank;
};

struct TPacketCGKingdomSettings
{
    BYTE bHeader;
    char szName[20];
    BYTE bColorR, bColorG, bColorB;
    BYTE bFlag;
    char szDescription[100];
};

// Server to Client packets
struct TPacketGCKingdomList
{
    BYTE bHeader;
    WORD wCount;
    // Followed by kingdom data
};

struct TKingdomListData
{
    DWORD dwKingdomID;
    char szName[20];
    WORD wMemberCount;
    BYTE bColorR, bColorG, bColorB;
    BYTE bFlag;
};

struct TPacketGCKingdomInfo
{
    BYTE bHeader;
    DWORD dwKingdomID;
    char szName[20];
    char szDescription[100];
    BYTE bColorR, bColorG, bColorB;
    BYTE bFlag;
    DWORD dwCreateTime;
    WORD wMemberCount;
    // Followed by member data
};

struct TKingdomMemberData
{
    DWORD dwPlayerID;
    char szName[20];
    BYTE bRank;
    DWORD dwJoinTime;
    bool bOnline;
};

struct TPacketGCKingdomInvite
{
    BYTE bHeader;
    char szKingdomName[20];
    char szInviterName[20];
    DWORD dwKingdomID;
};

struct TPacketGCKingdomJoinResult
{
    BYTE bHeader;
    BYTE bResult;  // 0 = success, 1 = fail
    char szMessage[100];
};

#pragma pack()

// Packet headers
enum
{
    HEADER_CG_CREATE_KINGDOM = 200,
    HEADER_CG_JOIN_KINGDOM = 201,
    HEADER_CG_LEAVE_KINGDOM = 202,
    HEADER_CG_REQUEST_KINGDOM_LIST = 203,
    HEADER_CG_KINGDOM_INVITE = 204,
    HEADER_CG_KINGDOM_KICK = 205,
    HEADER_CG_KINGDOM_RANK = 206,
    HEADER_CG_KINGDOM_SETTINGS = 207,
    
    HEADER_GC_KINGDOM_LIST = 200,
    HEADER_GC_KINGDOM_INFO = 201,
    HEADER_GC_KINGDOM_MEMBER_LIST = 202,
    HEADER_GC_KINGDOM_INVITE = 203,
    HEADER_GC_KINGDOM_JOIN_RESULT = 204,
    HEADER_GC_KINGDOM_LEAVE_RESULT = 205
};

#endif // __INC_KINGDOM_H__
