#ifndef __QUEST_FLAG_H__
#define __QUEST_FLAG_H__

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

class CQuestFlagManager
{
public:
    static CQuestFlagManager& instance();

    int GetFlagID(const std::string& name);
    const std::string& GetFlagName(int id) const;

private:
    CQuestFlagManager();
    ~CQuestFlagManager();

    std::unordered_map<std::string, int> m_flagMap;
    std::vector<std::string> m_flagNames;
    int m_nextFlagID;
    mutable std::mutex m_mutex;
};

#endif // __QUEST_FLAG_H__
