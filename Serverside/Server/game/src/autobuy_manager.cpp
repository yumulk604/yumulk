#include "stdafx.h"
#include "autobuy_manager.h"
#include "db.h"
#include "log.h"
#include "char.h"
#include "shop_manager.h"
#include "config.h"

EVENTINFO(autobuy_tick_info)
{
	DWORD dummy;
};

EVENTFUNC(autobuy_tick)
{
	CAutoBuyManager::instance().OnTick();
	int sec = CAutoBuyManager::instance().GetIntervalSec();
	if (sec <= 0) sec = 60;
	return PASSES_PER_SEC(sec);
}

CAutoBuyManager::CAutoBuyManager()
	: m_pkEvent(NULL), m_bRunning(false), m_iIntervalSec(60)
{
}

CAutoBuyManager::~CAutoBuyManager()
{
	Stop();
}

void CAutoBuyManager::Start()
{
	if (m_bRunning)
		return;

	if (!LoadConfigsFromDB())
	{
		// still start with defaults, it will retry on ticks
	}
	RecomputeInterval();
	autobuy_tick_info* info = AllocEventInfo<autobuy_tick_info>();
	m_pkEvent = event_create(autobuy_tick, info, PASSES_PER_SEC(m_iIntervalSec));
	m_bRunning = true;
	sys_log(0, "AUTOBUY: started (interval=%d)", m_iIntervalSec);
}

void CAutoBuyManager::Stop()
{
	if (!m_bRunning)
		return;
	if (m_pkEvent)
	{
		event_cancel(&m_pkEvent);
		m_pkEvent = NULL;
	}
	m_bRunning = false;
	sys_log(0, "AUTOBUY: stopped");
}

void CAutoBuyManager::Reload()
{
	LoadConfigsFromDB();
	RecomputeInterval();
	sys_log(0, "AUTOBUY: reloaded (interval=%d)", m_iIntervalSec);
}

void CAutoBuyManager::Schedule()
{
	if (!m_bRunning)
		return;
	if (m_pkEvent)
	{
		event_cancel(&m_pkEvent);
		m_pkEvent = NULL;
	}
	autobuy_tick_info* info = AllocEventInfo<autobuy_tick_info>();
	m_pkEvent = event_create(autobuy_tick, info, PASSES_PER_SEC(m_iIntervalSec));
}

void CAutoBuyManager::OnTick()
{
	// Lazy load/refresh in case table appeared
	LoadConfigsFromDB();
	RecomputeInterval();

	// Iterate PC shops and perform purchases per config
	const CShopManager::TShopMap& pcShops = CShopManager::instance().GetPCShops();
	for (size_t c = 0; c < m_configs.size(); ++c)
	{
		const TAutoBuyConfig& cfg = m_configs[c];
		int remaining = cfg.buyCount > 0 ? cfg.buyCount : 0;
		if (remaining == 0)
			continue;
		for (CShopManager::TShopMap::const_iterator it = pcShops.begin(); it != pcShops.end() && remaining > 0; ++it)
		{
			CShop* shop = it->second;
			if (!shop)
				continue;
			long paid = shop->SystemAutoBuy(cfg.itemVnum, 0 /* any count */, (long)cfg.maxUnitPrice, cfg.ownerPID);
			if (paid > 0)
			{
				--remaining;
			}
		}
	}
}

bool CAutoBuyManager::LoadConfigsFromDB()
{
	// Try to read configs; if table does not exist, ignore errors.
	std::auto_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(
		"SELECT item_vnum, max_unit_price, buy_count, interval_sec, owner_pid, marketplaces FROM autobuy_config WHERE enabled=1"));
	SQLResult* res = pmsg.get() ? pmsg->Get() : NULL;
	if (!res || res->uiNumRows == 0)
	{
		m_configs.clear();
		return false;
	}
	m_configs.clear();
	m_configs.reserve(res->uiNumRows);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res->pSQLResult)))
	{
		TAutoBuyConfig cfg;
		str_to_number(cfg.itemVnum, row[0]);
		cfg.maxUnitPrice = atoll(row[1] ? row[1] : "0");
		str_to_number(cfg.buyCount, row[2]);
		str_to_number(cfg.intervalSec, row[3]);
		str_to_number(cfg.ownerPID, row[4]);
		cfg.marketplaces = row[5] ? row[5] : "OFFLINE";
		m_configs.push_back(cfg);
	}
	return true;
}

void CAutoBuyManager::RecomputeInterval()
{
	int best = 60;
	for (size_t i = 0; i < m_configs.size(); ++i)
	{
		if (m_configs[i].intervalSec > 0)
			best = std::min(best, m_configs[i].intervalSec);
	}
	if (best <= 0)
		best = 60;
	m_iIntervalSec = best;
	Schedule();
}

void CAutoBuyManager::Status(LPCHARACTER ch) const
{
	if (!ch)
		return;
	ch->ChatPacket(CHAT_TYPE_INFO, "AutoBuy %s, interval %d sec, %zu config(s)", m_bRunning ? "RUNNING" : "STOPPED", m_iIntervalSec, m_configs.size());
}

