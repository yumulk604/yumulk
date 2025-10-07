#ifndef __INC_KINGDOM_PACKET_H__
#define __INC_KINGDOM_PACKET_H__

#include "kingdom.h"

class LPCHARACTER;

class CKingdomPacketHandler
{
public:
    // Packet handlers
    static void HandleCreateKingdom(LPCHARACTER ch, const char* data);
    static void HandleJoinKingdom(LPCHARACTER ch, const char* data);
    static void HandleLeaveKingdom(LPCHARACTER ch, const char* data);
    static void HandleRequestKingdomList(LPCHARACTER ch, const char* data);
    static void HandleKingdomInvite(LPCHARACTER ch, const char* data);
    static void HandleKingdomKick(LPCHARACTER ch, const char* data);
    static void HandleKingdomRank(LPCHARACTER ch, const char* data);
    static void HandleKingdomSettings(LPCHARACTER ch, const char* data);
    
    // Response senders
    static void SendKingdomList(LPCHARACTER ch);
    static void SendKingdomInfo(LPCHARACTER ch, DWORD dwKingdomID);
    static void SendKingdomJoinResult(LPCHARACTER ch, bool bSuccess, const std::string& strMessage);
    static void SendKingdomLeaveResult(LPCHARACTER ch, bool bSuccess, const std::string& strMessage);
};

// Function to register packet handlers (to be called in input_main.cpp or similar)
void RegisterKingdomPacketHandlers();

#endif // __INC_KINGDOM_PACKET_H__
