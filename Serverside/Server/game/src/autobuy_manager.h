#ifndef __INC_METIN_II_GAME_AUTOBUY_MANAGER_H__
#define __INC_METIN_II_GAME_AUTOBUY_MANAGER_H__

#include "stdafx.h"
#include "singleton.h"
#include "event.h"
#include <vector>
#include <string>

struct TAutoBuyConfig
{
	DWORD	itemVnum;
	long long	maxUnitPrice;
	int		buyCount;
	int		intervalSec;
	DWORD	ownerPID;
	std::string marketplaces;

	TAutoBuyConfig()
		: itemVnum(0), maxUnitPrice(0), buyCount(0), intervalSec(60), ownerPID(0), marketplaces("OFFLINE")
	{
	}
};

class CAutoBuyManager : public singleton<CAutoBuyManager>
{
public:
	CAutoBuyManager();
	~CAutoBuyManager();

	void	Start();
	void	Stop();
	void	Reload();
	bool	IsRunning() const { return m_bRunning; }
	int		GetIntervalSec() const { return m_iIntervalSec; }
	void	Status(LPCHARACTER ch) const;

private:
	void	Schedule();
	void	OnTick();
	bool	LoadConfigsFromDB();
	void	RecomputeInterval();

private:
	LPEVENT			m_pkEvent;
	bool			m_bRunning;
	int				m_iIntervalSec;
	std::vector<TAutoBuyConfig>	m_configs;
};

#endif

