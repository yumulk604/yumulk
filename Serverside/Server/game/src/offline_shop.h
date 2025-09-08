#ifndef __INC_METIN_II_GAME_OFFLINE_SHOP_H__
#define __INC_METIN_II_GAME_OFFLINE_SHOP_H__

#include "typedef.h"
#include "shop.h"
#include <vector>

struct TOfflineShopItem
{
	DWORD	vnum;
	DWORD	price;
	BYTE	count;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];

	TOfflineShopItem()
	{
		vnum = 0;
		price = 0;
		count = 0;
		memset(alSockets, 0, sizeof(alSockets));
		memset(aAttr, 0, sizeof(aAttr));
	}
};

class COfflineShop : public CShop
{
public:
	COfflineShop();
	virtual ~COfflineShop();

	bool	Create(DWORD dwOwnerPID, const char* c_pszSign);
	bool	AddItem(const TOfflineShopItem& item);
	void	ClearItems();

	virtual bool	AddGuest(LPCHARACTER ch, DWORD owner_vid, bool bOtherEmpire);
	virtual int		Buy(LPCHARACTER ch, BYTE pos);
	virtual bool	IsPCShop() { return false; }
	virtual bool	IsSellingItem(DWORD itemID) { return false; }

	DWORD		GetOwnerPID() const { return m_dwOwnerPID; }
	const std::string& GetSign() const { return m_stSign; }
	DWORD		GetVID() const { return m_dwVID; }
	void		SetVID(DWORD vid) { m_dwVID = vid; }

private:
	void		BuildPacketItems(TPacketGCShopStart& out) const;

private:
	DWORD		m_dwOwnerPID;
	std::string	m_stSign;
	DWORD		m_dwVID; // virtual entity id if any
	std::vector<TOfflineShopItem> m_items; // fixed-size SHOP_HOST_ITEM_MAX_NUM optional
};

typedef COfflineShop* LPOFFLINESHOP;

#endif

