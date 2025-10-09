#ifndef __HEADER_REFINE_SYSTEM_TXT_H__
#define __HEADER_REFINE_SYSTEM_TXT_H__

#include "../../common/CommonDefines.h"
#include <map>
#include <string>
#include <vector>
#include <fstream>

// Refine System - TXT Tabanlı Veri Yükleme Sistemi
// Veritabanı yerine TXT dosyasından refine verilerini yükler
// 10 materyale kadar desteği vardır (REFINE_MATERIAL_MAX_NUM = 10)

class CRefineSystemTXT
{
public:
    struct SRefineMaterial
    {
        DWORD dwVnum;
        WORD wCount;
        
        SRefineMaterial()
        {
            dwVnum = 0;
            wCount = 0;
        }
        
        SRefineMaterial(DWORD vnum, WORD count)
        {
            dwVnum = vnum;
            wCount = count;
        }
    };
    
    struct SRefineData
    {
        DWORD dwRefineID;
        DWORD dwCost;                                    // Yang maliyeti
        WORD wSuccessRate;                               // Başarı oranı (0-100)
        SRefineMaterial materials[10];                   // 10 materyal desteği
        DWORD dwSourceVnum;                              // Kaynak item vnum
        DWORD dwResultVnum;                              // Sonuç item vnum
        
        SRefineData()
        {
            dwRefineID = 0;
            dwCost = 0;
            wSuccessRate = 100;
            dwSourceVnum = 0;
            dwResultVnum = 0;
            
            for (int i = 0; i < 10; ++i)
            {
                materials[i] = SRefineMaterial();
            }
        }
    };
    
private:
    static CRefineSystemTXT* ms_pInstance;
    
    std::map<DWORD, SRefineData> m_mapRefineData;        // RefineID -> RefineData
    std::map<DWORD, std::vector<DWORD>> m_mapItemRefines; // ItemVnum -> RefineIDs
    
    bool m_bDataLoaded;
    std::string m_strDataFile;
    time_t m_tLastLoadTime;
    
    // Error tracking
    int m_iLoadErrors;
    std::vector<std::string> m_vecErrorLog;
    
public:
    CRefineSystemTXT();
    ~CRefineSystemTXT();
    
    // Singleton
    static CRefineSystemTXT& Instance();
    static void CreateInstance();
    static void DestroyInstance();
    
    // Data Loading
    bool LoadFromFile(const char* filename);
    bool ReloadData();
    bool IsDataLoaded() const { return m_bDataLoaded; }
    time_t GetLastLoadTime() const { return m_tLastLoadTime; }
    
    // Data Access
    const SRefineData* GetRefineData(DWORD dwRefineID) const;
    std::vector<DWORD> GetRefineListForItem(DWORD dwItemVnum) const;
    bool HasRefineData(DWORD dwRefineID) const;
    
    // Validation
    bool ValidateRefineData(const SRefineData& data) const;
    bool CanRefineItem(DWORD dwItemVnum) const;
    bool HasRequiredMaterials(LPCHARACTER ch, DWORD dwRefineID) const;
    
    // Statistics
    DWORD GetTotalRefineCount() const { return m_mapRefineData.size(); }
    DWORD GetLoadErrorCount() const { return m_iLoadErrors; }
    const std::vector<std::string>& GetErrorLog() const { return m_vecErrorLog; }
    
    // Debugging
    void DumpRefineData() const;
    void PrintRefineInfo(DWORD dwRefineID) const;
    std::string GetSystemInfo() const;
    
    // File Management
    bool BackupDataFile() const;
    bool IsFileModified() const;
    void WatchFileChanges();
    
private:
    // Internal functions
    bool ParseRefineDataLine(const std::string& line, SRefineData& refineData);
    void LogError(const std::string& error);
    void ClearErrorLog();
    std::vector<std::string> SplitString(const std::string& str, char delimiter) const;
    bool IsValidVnum(DWORD vnum) const;
    void IndexRefineData();
};

// Helper macros for TXT-based refine system
#define REFINE_TXT_SYSTEM() CRefineSystemTXT::Instance()
#define GET_REFINE_DATA(id) CRefineSystemTXT::Instance().GetRefineData(id)
#define CAN_REFINE_ITEM(vnum) CRefineSystemTXT::Instance().CanRefineItem(vnum)
#define HAS_REFINE_MATERIALS(ch, id) CRefineSystemTXT::Instance().HasRequiredMaterials(ch, id)

// Global functions for easy integration
void InitializeRefineSystemTXT();
void DestroyRefineSystemTXT();
bool IsRefineSystemTXTLoaded();
void ReloadRefineSystemTXT();

// Integration functions
const CRefineSystemTXT::SRefineData* GetRefineDataTXT(DWORD dwRefineID);
bool CanRefineItemTXT(DWORD dwItemVnum);
std::vector<DWORD> GetRefineListTXT(DWORD dwItemVnum);

#endif // __HEADER_REFINE_SYSTEM_TXT_H__