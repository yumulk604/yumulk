#include "questflag.h"

CQuestFlagManager& CQuestFlagManager::instance()
{
    static CQuestFlagManager instance;
    return instance;
}

CQuestFlagManager::CQuestFlagManager() : m_nextFlagID(1)
{
    // ID 0 is reserved for invalid/not-found flags
}

CQuestFlagManager::~CQuestFlagManager()
{
}

int CQuestFlagManager::GetFlagID(const std::string& name)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_flagMap.find(name);
    if (it != m_flagMap.end())
    {
        return it->second;
    }

    int id = m_nextFlagID++;
    m_flagMap[name] = id;
    m_flagNames.push_back(name);
    return id;
}

const std::string& CQuestFlagManager::GetFlagName(int id) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (id <= 0 || id >= m_nextFlagID)
    {
        static const std::string emptyString = "";
        return emptyString;
    }
    return m_flagNames[id - 1];
}
