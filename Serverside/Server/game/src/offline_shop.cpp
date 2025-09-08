#include "stdafx.h"
#include "offline_shop.h"
#include "char.h"
#include "desc.h"
#include "desc_manager.h"
#include "item_manager.h"
#include "log.h"

COfflineShop::COfflineShop()
	: m_dwOwnerPID(0), m_dwVID(0)
{
	m_items.reserve(SHOP_HOST_ITEM_MAX_NUM);
}

COfflineShop::~COfflineShop()
{
}

bool COfflineShop::Create(DWORD dwOwnerPID, const char* c_pszSign)
{
	m_dwOwnerPID = dwOwnerPID;
	m_stSign = c_pszSign ? c_pszSign : "";
	m_itemVector.clear();
	m_itemVector.resize(SHOP_HOST_ITEM_MAX_NUM);
	memset(&m_itemVector[0], 0, sizeof(SHOP_ITEM) * m_itemVector.size());
	return true;
}

bool COfflineShop::AddItem(const TOfflineShopItem& item)
{
	if (m_items.size() >= SHOP_HOST_ITEM_MAX_NUM)
		return false;
	m_items.push_back(item);
	// mirror into base vector for packet compatibility
	SHOP_ITEM baseItem;
	baseItem.vnum = item.vnum;
	baseItem.price = item.price;
	baseItem.count = item.count;
	baseItem.pkItem = NULL;
	baseItem.itemid = 0;
	m_itemVector[m_items.size() - 1] = baseItem;
	return true;
}

void COfflineShop::ClearItems()
{
	m_items.clear();
	for (size_t i = 0; i < m_itemVector.size(); ++i)
	{
		m_itemVector[i].vnum = 0;
		m_itemVector[i].price = 0;
		m_itemVector[i].count = 0;
		m_itemVector[i].pkItem = NULL;
		m_itemVector[i].itemid = 0;
	}
}

void COfflineShop::BuildPacketItems(TPacketGCShopStart& pack) const
{
	for (DWORD i = 0; i < m_itemVector.size() && i < SHOP_HOST_ITEM_MAX_NUM; ++i)
	{
		pack.items[i].vnum = m_itemVector[i].vnum;
		pack.items[i].price = m_itemVector[i].price;
		pack.items[i].count = m_itemVector[i].count;
		memset(pack.items[i].alSockets, 0, sizeof(pack.items[i].alSockets));
		memset(pack.items[i].aAttr, 0, sizeof(pack.items[i].aAttr));
	}
}

bool COfflineShop::AddGuest(LPCHARACTER ch, DWORD owner_vid, bool bOtherEmpire)
{
	if (!ch)
		return false;
	if (ch->GetExchange())
		return false;
	if (ch->GetShop())
		return false;

	ch->SetShop(this);
	m_map_guest.insert(GuestMapType::value_type(ch, bOtherEmpire));

	TPacketGCShop pack;
	pack.header = HEADER_GC_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_START;

	TPacketGCShopStart pack2;
	memset(&pack2, 0, sizeof(pack2));
	pack2.owner_vid = owner_vid; // could be 0 for offline shops
	BuildPacketItems(pack2);

	pack.size = sizeof(pack) + sizeof(pack2);

	ch->GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
	ch->GetDesc()->Packet(&pack2, sizeof(TPacketGCShopStart));
	return true;
}

int COfflineShop::Buy(LPCHARACTER ch, BYTE pos)
{
	if (pos >= m_itemVector.size())
		return SHOP_SUBHEADER_GC_INVALID_POS;

	SHOP_ITEM& r_item = m_itemVector[pos];
	if (r_item.vnum == 0 || r_item.count == 0)
		return SHOP_SUBHEADER_GC_SOLD_OUT;

	DWORD dwPrice = r_item.price;
	if (ch->GetGold() < (int)dwPrice)
		return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;

	LPITEM item = ITEM_MANAGER::instance().CreateItem(r_item.vnum, r_item.count);
	if (!item)
		return SHOP_SUBHEADER_GC_SOLD_OUT;

	int iEmptyPos = ch->GetEmptyInventory(item->GetSize());
	if (iEmptyPos < 0)
	{
		M2_DESTROY_ITEM(item);
		return SHOP_SUBHEADER_GC_INVENTORY_FULL;
	}

	ch->PointChange(POINT_GOLD, -static_cast<int>(dwPrice), false);
	item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
	ITEM_MANAGER::instance().FlushDelayedSave(item);
	LogManager::instance().ItemLog(ch, item, "OFFLINE_SHOP_BUY", item->GetName());

	r_item.vnum = 0;
	r_item.count = 0;
	r_item.price = 0;
	BroadcastUpdateItem(pos);

	return SHOP_SUBHEADER_GC_OK;
}

