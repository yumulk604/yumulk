#include "RefineSystemTXT.h"
#include "AntiCoreSystem.h"
#include "utils.h"
#include "log.h"
#include "char.h"
#include "item_manager.h"
#include <iostream>
#include <sstream>
#include <sys/stat.h>

// Static member initialization
CRefineSystemTXT* CRefineSystemTXT::ms_pInstance = nullptr;

CRefineSystemTXT::CRefineSystemTXT()
{
    m_bDataLoaded = false;
    m_strDataFile = "";
    m_tLastLoadTime = 0;
    m_iLoadErrors = 0;
    
    m_mapRefineData.clear();
    m_mapItemRefines.clear();
    m_vecErrorLog.clear();
}

CRefineSystemTXT::~CRefineSystemTXT()
{
    m_mapRefineData.clear();
    m_mapItemRefines.clear();
    m_vecErrorLog.clear();
}

// Singleton implementation
CRefineSystemTXT& CRefineSystemTXT::Instance()
{
    if (!ms_pInstance)
    {
        CreateInstance();
    }
    return *ms_pInstance;
}

void CRefineSystemTXT::CreateInstance()
{
    if (!ms_pInstance)
    {
        ms_pInstance = new CRefineSystemTXT();
    }
}

void CRefineSystemTXT::DestroyInstance()
{
    if (ms_pInstance)
    {
        delete ms_pInstance;
        ms_pInstance = nullptr;
    }
}

// Load refine data from TXT file
bool CRefineSystemTXT::LoadFromFile(const char* filename)
{
    SAFE_FUNCTION_ENTER();
    
    if (!filename)
    {
        REPORT_ERROR("Null filename provided");
        SAFE_FUNCTION_EXIT();
        return false;
    }
    
    m_strDataFile = filename;
    ClearErrorLog();
    
    std::ifstream file(filename);
    if (!file.is_open())
    {
        LogError("Could not open refine data file: " + std::string(filename));
        SAFE_FUNCTION_EXIT();
        return false;
    }
    
    sys_log(0, "[RefineSystemTXT] Loading refine data from: %s", filename);
    
    m_mapRefineData.clear();
    m_mapItemRefines.clear();
    
    std::string line;
    int lineNumber = 0;
    int loadedCount = 0;
    
    while (std::getline(file, line))
    {
        lineNumber++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#')
            continue;
            
        SRefineData refineData;
        if (ParseRefineDataLine(line, refineData))
        {
            if (ValidateRefineData(refineData))
            {
                m_mapRefineData[refineData.dwRefineID] = refineData;
                loadedCount++;
            }
            else
            {
                LogError("Invalid refine data at line " + std::to_string(lineNumber));
            }
        }
        else
        {
            LogError("Failed to parse line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    file.close();
    
    // Index data for fast lookup
    IndexRefineData();
    
    m_bDataLoaded = (loadedCount > 0);
    m_tLastLoadTime = time(0);
    
    if (m_bDataLoaded)
    {
        sys_log(0, "[RefineSystemTXT] Successfully loaded %d refine entries from %s", loadedCount, filename);
        sys_log(0, "[RefineSystemTXT] Total errors: %d", m_iLoadErrors);
    }
    else
    {
        sys_err("[RefineSystemTXT] Failed to load any refine data from %s", filename);
    }
    
    SAFE_FUNCTION_EXIT();
    return m_bDataLoaded;
}

// Parse a single line of refine data
bool CRefineSystemTXT::ParseRefineDataLine(const std::string& line, SRefineData& refineData)
{
    SAFE_CALL_WITH_RECOVERY(
    {
        std::vector<std::string> tokens = SplitString(line, '|');
        
        // Minimum required: ID|cost|prob|vnum0|count0|vnum1|count1|...|vnum9|count9|src_vnum|result_vnum
        // Total: 1 + 1 + 1 + (10*2) + 1 + 1 = 25 fields
        if (tokens.size() < 25)
        {
            LogError("Insufficient fields in line: " + line);
            return false;
        }
        
        int tokenIndex = 0;
        
        // Parse basic fields
        refineData.dwRefineID = strtoul(tokens[tokenIndex++].c_str(), nullptr, 10);
        refineData.dwCost = strtoul(tokens[tokenIndex++].c_str(), nullptr, 10);
        refineData.wSuccessRate = (WORD)strtoul(tokens[tokenIndex++].c_str(), nullptr, 10);
        
        // Parse materials (vnum0, count0, vnum1, count1, ..., vnum9, count9)
        for (int i = 0; i < 10; ++i)
        {
            DWORD vnum = strtoul(tokens[tokenIndex++].c_str(), nullptr, 10);
            WORD count = (WORD)strtoul(tokens[tokenIndex++].c_str(), nullptr, 10);
            
            refineData.materials[i] = SRefineMaterial(vnum, count);
        }
        
        // Parse source and result vnums
        refineData.dwSourceVnum = strtoul(tokens[tokenIndex++].c_str(), nullptr, 10);
        refineData.dwResultVnum = strtoul(tokens[tokenIndex++].c_str(), nullptr, 10);
        
        return true;
    });
    
    return false;
}

// Validate refine data integrity
bool CRefineSystemTXT::ValidateRefineData(const SRefineData& data) const
{
    SAFE_CALL_WITH_RECOVERY(
    {
        // Basic validation
        if (data.dwRefineID == 0)
        {
            return false;
        }
        
        if (data.wSuccessRate > 100)
        {
            return false;
        }
        
        if (data.dwSourceVnum == 0 || data.dwResultVnum == 0)
        {
            return false;
        }
        
        // Validate that at least one material is specified
        bool hasMaterial = false;
        for (int i = 0; i < 10; ++i)
        {
            if (data.materials[i].dwVnum > 0 && data.materials[i].wCount > 0)
            {
                hasMaterial = true;
                
                // Validate individual material
                if (!IsValidVnum(data.materials[i].dwVnum))
                {
                    return false;
                }
                
                if (data.materials[i].wCount > 1000) // Reasonable limit
                {
                    return false;
                }
            }
        }
        
        if (!hasMaterial)
        {
            return false;
        }
        
        // Validate source and result vnums
        if (!IsValidVnum(data.dwSourceVnum) || !IsValidVnum(data.dwResultVnum))
        {
            return false;
        }
        
        return true;
    });
    
    return false;
}

// Get refine data by ID
const CRefineSystemTXT::SRefineData* CRefineSystemTXT::GetRefineData(DWORD dwRefineID) const
{
    SAFE_CALL_WITH_RECOVERY(
    {
        auto it = m_mapRefineData.find(dwRefineID);
        if (it != m_mapRefineData.end())
        {
            return &(it->second);
        }
    });
    
    return nullptr;
}

// Get all refine options for an item
std::vector<DWORD> CRefineSystemTXT::GetRefineListForItem(DWORD dwItemVnum) const
{
    SAFE_CALL_WITH_RECOVERY(
    {
        auto it = m_mapItemRefines.find(dwItemVnum);
        if (it != m_mapItemRefines.end())
        {
            return it->second;
        }
    });
    
    return std::vector<DWORD>();
}

// Check if player has required materials
bool CRefineSystemTXT::HasRequiredMaterials(LPCHARACTER ch, DWORD dwRefineID) const
{
    SAFE_POINTER_CHECK_RETURN(ch, false);
    
    const SRefineData* pData = GetRefineData(dwRefineID);
    SAFE_POINTER_CHECK_RETURN(pData, false);
    
    SAFE_CALL_WITH_RECOVERY(
    {
        for (int i = 0; i < 10; ++i)
        {
            const SRefineMaterial& material = pData->materials[i];
            if (material.dwVnum > 0 && material.wCount > 0)
            {
                if (ch->CountSpecifyItem(material.dwVnum) < material.wCount)
                {
                    return false;
                }
            }
        }
        
        return true;
    });
    
    return false;
}

// Index refine data for fast lookup
void CRefineSystemTXT::IndexRefineData()
{
    SAFE_CALL_WITH_RECOVERY(
    {
        m_mapItemRefines.clear();
        
        for (const auto& pair : m_mapRefineData)
        {
            const SRefineData& data = pair.second;
            m_mapItemRefines[data.dwSourceVnum].push_back(data.dwRefineID);
        }
        
        sys_log(0, "[RefineSystemTXT] Indexed %zu item types for refining", m_mapItemRefines.size());
    });
}

// Split string by delimiter
std::vector<std::string> CRefineSystemTXT::SplitString(const std::string& str, char delimiter) const
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }
    
    return tokens;
}

// Validate item vnum
bool CRefineSystemTXT::IsValidVnum(DWORD vnum) const
{
    // Basic vnum validation
    return (vnum > 0 && vnum < 999999);
}

// Log error messages
void CRefineSystemTXT::LogError(const std::string& error)
{
    m_vecErrorLog.push_back(error);
    m_iLoadErrors++;
    sys_err("[RefineSystemTXT] Error: %s", error.c_str());
}

void CRefineSystemTXT::ClearErrorLog()
{
    m_vecErrorLog.clear();
    m_iLoadErrors = 0;
}

// Reload data from file
bool CRefineSystemTXT::ReloadData()
{
    if (m_strDataFile.empty())
    {
        REPORT_ERROR("No data file specified for reload");
        return false;
    }
    
    sys_log(0, "[RefineSystemTXT] Reloading data from %s", m_strDataFile.c_str());
    return LoadFromFile(m_strDataFile.c_str());
}

// Check if refine exists
bool CRefineSystemTXT::HasRefineData(DWORD dwRefineID) const
{
    return m_mapRefineData.find(dwRefineID) != m_mapRefineData.end();
}

// Check if item can be refined
bool CRefineSystemTXT::CanRefineItem(DWORD dwItemVnum) const
{
    return m_mapItemRefines.find(dwItemVnum) != m_mapItemRefines.end();
}

// Dump all refine data for debugging
void CRefineSystemTXT::DumpRefineData() const
{
    sys_log(0, "[RefineSystemTXT] === REFINE DATA DUMP ===");
    sys_log(0, "[RefineSystemTXT] Total entries: %zu", m_mapRefineData.size());
    sys_log(0, "[RefineSystemTXT] Load errors: %d", m_iLoadErrors);
    
    for (const auto& pair : m_mapRefineData)
    {
        const SRefineData& data = pair.second;
        sys_log(0, "[RefineSystemTXT] Refine ID: %u, Cost: %u, Rate: %u%%, Src: %u -> Dst: %u",
               data.dwRefineID, data.dwCost, data.wSuccessRate, data.dwSourceVnum, data.dwResultVnum);
        
        for (int i = 0; i < 10; ++i)
        {
            if (data.materials[i].dwVnum > 0)
            {
                sys_log(0, "[RefineSystemTXT]   Material %d: Vnum %u, Count %u", 
                       i, data.materials[i].dwVnum, data.materials[i].wCount);
            }
        }
    }
}

// Print specific refine info
void CRefineSystemTXT::PrintRefineInfo(DWORD dwRefineID) const
{
    const SRefineData* pData = GetRefineData(dwRefineID);
    if (!pData)
    {
        sys_log(0, "[RefineSystemTXT] Refine ID %u not found", dwRefineID);
        return;
    }
    
    sys_log(0, "[RefineSystemTXT] === REFINE INFO: ID %u ===", dwRefineID);
    sys_log(0, "[RefineSystemTXT] Cost: %u yang", pData->dwCost);
    sys_log(0, "[RefineSystemTXT] Success Rate: %u%%", pData->wSuccessRate);
    sys_log(0, "[RefineSystemTXT] Source Item: %u", pData->dwSourceVnum);
    sys_log(0, "[RefineSystemTXT] Result Item: %u", pData->dwResultVnum);
    sys_log(0, "[RefineSystemTXT] Required Materials:");
    
    for (int i = 0; i < 10; ++i)
    {
        if (pData->materials[i].dwVnum > 0)
        {
            sys_log(0, "[RefineSystemTXT]   - Item %u x%u", 
                   pData->materials[i].dwVnum, pData->materials[i].wCount);
        }
    }
}

// Get system information
std::string CRefineSystemTXT::GetSystemInfo() const
{
    std::stringstream ss;
    
    ss << "=== REFINE SYSTEM TXT INFO ===\n";
    ss << "Data File: " << m_strDataFile << "\n";
    ss << "Data Loaded: " << (m_bDataLoaded ? "Yes" : "No") << "\n";
    ss << "Last Load Time: " << m_tLastLoadTime << "\n";
    ss << "Total Refines: " << m_mapRefineData.size() << "\n";
    ss << "Total Items: " << m_mapItemRefines.size() << "\n";
    ss << "Load Errors: " << m_iLoadErrors << "\n";
    
    if (!m_vecErrorLog.empty())
    {
        ss << "\nRecent Errors:\n";
        int count = 0;
        for (auto it = m_vecErrorLog.rbegin(); it != m_vecErrorLog.rend() && count < 10; ++it, ++count)
        {
            ss << "- " << *it << "\n";
        }
    }
    
    ss << "\n=== END INFO ===\n";
    
    return ss.str();
}

// Backup data file
bool CRefineSystemTXT::BackupDataFile() const
{
    if (m_strDataFile.empty())
        return false;
        
    std::string backupFile = m_strDataFile + ".backup." + std::to_string(time(0));
    
    std::ifstream src(m_strDataFile, std::ios::binary);
    std::ofstream dst(backupFile, std::ios::binary);
    
    if (!src.is_open() || !dst.is_open())
        return false;
        
    dst << src.rdbuf();
    
    src.close();
    dst.close();
    
    sys_log(0, "[RefineSystemTXT] Data file backed up to: %s", backupFile.c_str());
    return true;
}

// Check if file was modified
bool CRefineSystemTXT::IsFileModified() const
{
    if (m_strDataFile.empty())
        return false;
        
    struct stat fileStat;
    if (stat(m_strDataFile.c_str(), &fileStat) != 0)
        return false;
        
    return fileStat.st_mtime > m_tLastLoadTime;
}

// Watch for file changes and auto-reload
void CRefineSystemTXT::WatchFileChanges()
{
    if (IsFileModified())
    {
        sys_log(0, "[RefineSystemTXT] Data file modified, auto-reloading...");
        BackupDataFile();
        ReloadData();
    }
}

// Global functions
void InitializeRefineSystemTXT()
{
    SAFE_CALL_WITH_RECOVERY(
    {
        CRefineSystemTXT::CreateInstance();
        
        // Load default data file
        if (!CRefineSystemTXT::Instance().LoadFromFile("data/refine_proto.txt"))
        {
            sys_err("[RefineSystemTXT] Failed to load refine data!");
        }
        
        sys_log(0, "[RefineSystemTXT] Refine System TXT initialized.");
    });
}

void DestroyRefineSystemTXT()
{
    SAFE_CALL_WITH_RECOVERY(
    {
        CRefineSystemTXT::DestroyInstance();
        sys_log(0, "[RefineSystemTXT] Refine System TXT destroyed.");
    });
}

bool IsRefineSystemTXTLoaded()
{
    return CRefineSystemTXT::Instance().IsDataLoaded();
}

void ReloadRefineSystemTXT()
{
    SAFE_CALL_WITH_RECOVERY(
    {
        CRefineSystemTXT::Instance().ReloadData();
        sys_log(0, "[RefineSystemTXT] Refine data reloaded.");
    });
}

// Integration helper functions
const CRefineSystemTXT::SRefineData* GetRefineDataTXT(DWORD dwRefineID)
{
    return CRefineSystemTXT::Instance().GetRefineData(dwRefineID);
}

bool CanRefineItemTXT(DWORD dwItemVnum)
{
    return CRefineSystemTXT::Instance().CanRefineItem(dwItemVnum);
}

std::vector<DWORD> GetRefineListTXT(DWORD dwItemVnum)
{
    return CRefineSystemTXT::Instance().GetRefineListForItem(dwItemVnum);
}