#ifndef __HEADER_ANTICORE_SYSTEM_H__
#define __HEADER_ANTICORE_SYSTEM_H__

#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cxxabi.h>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <memory>

// Anti-Core Protection System - Hiç core vermeyecek sistem
// Crash'leri önler, hataları yakalar ve otomatik çözer

class CAntiCoreSystem
{
public:
    enum ECrashType
    {
        CRASH_SEGMENTATION_FAULT = 1,
        CRASH_ABORT_SIGNAL = 2,
        CRASH_FLOATING_POINT = 3,
        CRASH_BUS_ERROR = 4,
        CRASH_STACK_OVERFLOW = 5,
        CRASH_HEAP_CORRUPTION = 6,
        CRASH_NULL_POINTER = 7,
        CRASH_MEMORY_LEAK = 8,
        CRASH_INFINITE_LOOP = 9,
        CRASH_DEADLOCK = 10
    };
    
    enum ERecoveryAction
    {
        RECOVERY_NONE = 0,
        RECOVERY_RESTART_THREAD = 1,
        RECOVERY_RELOAD_DATA = 2,
        RECOVERY_RESET_CONNECTION = 3,
        RECOVERY_CLEAR_MEMORY = 4,
        RECOVERY_GRACEFUL_RESTART = 5,
        RECOVERY_EMERGENCY_SAVE = 6
    };
    
    struct SCrashInfo
    {
        ECrashType eCrashType;
        int iSignalNumber;
        void* pCrashAddress;
        std::string strFunctionName;
        std::string strFileName;
        int iLineNumber;
        std::string strStackTrace;
        time_t tCrashTime;
        pid_t processID;
        
        SCrashInfo()
        {
            eCrashType = CRASH_SEGMENTATION_FAULT;
            iSignalNumber = 0;
            pCrashAddress = nullptr;
            strFunctionName = "";
            strFileName = "";
            iLineNumber = 0;
            strStackTrace = "";
            tCrashTime = 0;
            processID = 0;
        }
    };
    
    struct SSystemState
    {
        double dCPUUsage;
        size_t ulMemoryUsage;
        size_t ulFreeMemory;
        int iPlayerCount;
        int iConnectionCount;
        bool bDatabaseConnected;
        time_t tUptime;
        int iErrorCount;
        
        SSystemState()
        {
            dCPUUsage = 0.0;
            ulMemoryUsage = 0;
            ulFreeMemory = 0;
            iPlayerCount = 0;
            iConnectionCount = 0;
            bDatabaseConnected = true;
            tUptime = 0;
            iErrorCount = 0;
        }
    };
    
private:
    static CAntiCoreSystem* ms_pInstance;
    
    bool m_bInitialized;
    bool m_bCrashProtectionEnabled;
    bool m_bAutoRecoveryEnabled;
    bool m_bDebugModeEnabled;
    
    std::map<int, std::string> m_mapSignalNames;
    std::vector<SCrashInfo> m_vecCrashHistory;
    SSystemState m_systemState;
    
    // Function call tracking
    std::map<std::string, int> m_mapFunctionCallCount;
    std::vector<std::string> m_vecCallStack;
    
    // Error patterns
    std::map<std::string, int> m_mapErrorPatterns;
    std::map<std::string, ERecoveryAction> m_mapRecoveryActions;
    
    // Memory tracking
    std::map<void*, size_t> m_mapAllocatedMemory;
    size_t m_ulTotalAllocated;
    
    // Monitoring
    time_t m_tLastHealthCheck;
    int m_iCrashCount;
    bool m_bMonitoringActive;
    
public:
    CAntiCoreSystem();
    ~CAntiCoreSystem();
    
    // Singleton
    static CAntiCoreSystem& Instance();
    static void CreateInstance();
    static void DestroyInstance();
    
    // Initialization
    bool Initialize();\n    void Destroy();
    bool IsInitialized() const { return m_bInitialized; }
    
    // Core Protection
    void InstallSignalHandlers();
    void RemoveSignalHandlers();
    static void HandleSignal(int sig, siginfo_t* info, void* context);
    static void HandleCrash(int sig, siginfo_t* info, void* context);
    
    // Exception Handling
    void HandleException(const char* function, const char* file, int line, const char* description = "");
    void LogException(const std::string& function, const std::string& error);
    
    // Memory Protection  
    void* SafeMalloc(size_t size, const char* file, int line);
    void SafeFree(void* ptr, const char* file, int line);
    bool ValidatePointer(void* ptr);
    void CheckMemoryLeaks();
    
    // Function Call Tracking
    void EnterFunction(const char* funcName);
    void ExitFunction(const char* funcName);
    void LogFunctionCall(const char* funcName, const char* params = "");
    
    // Crash Analysis
    void AnalyzeCrash(const SCrashInfo& crashInfo);
    std::string GenerateStackTrace(void* context = nullptr);
    void SaveCrashDump(const SCrashInfo& crashInfo);
    void GenerateCrashReport(const SCrashInfo& crashInfo);
    
    // Auto Recovery
    bool AttemptRecovery(ECrashType crashType);
    void ExecuteRecoveryAction(ERecoveryAction action);
    bool RecoverFromCrash(const SCrashInfo& crashInfo);
    void EmergencySave();
    
    // System Monitoring
    void UpdateSystemState();
    void MonitorResources();
    void CheckSystemHealth();
    bool IsSystemHealthy();
    
    // Debug Helpers
    void EnableDebugMode() { m_bDebugModeEnabled = true; }
    void DisableDebugMode() { m_bDebugModeEnabled = false; }
    void DumpSystemInfo();
    void DumpMemoryInfo();
    void DumpCallStack();
    
    // Configuration
    void LoadConfiguration();
    void SaveConfiguration();
    void SetCrashProtection(bool enabled) { m_bCrashProtectionEnabled = enabled; }
    void SetAutoRecovery(bool enabled) { m_bAutoRecoveryEnabled = enabled; }
    
    // Statistics
    int GetCrashCount() const { return m_iCrashCount; }
    time_t GetUptime() const;
    const SSystemState& GetSystemState() const { return m_systemState; }
    
    // Utility Functions
    std::string GetCurrentTime();
    std::string DemangledName(const char* name);
    void WriteToFile(const std::string& filename, const std::string& content);
    
    // Public API for manual error reporting
    void ReportError(const char* function, const char* description);
    void ReportWarning(const char* function, const char* description);
    void ReportInfo(const char* function, const char* description);
};

// Macro definitions for easy integration
#define SAFE_FUNCTION_ENTER() \
    CAntiCoreSystem::Instance().EnterFunction(__FUNCTION__)

#define SAFE_FUNCTION_EXIT() \
    CAntiCoreSystem::Instance().ExitFunction(__FUNCTION__)

#define SAFE_CALL_WITH_RECOVERY(func) \
    try { \
        func; \
    } catch (const std::exception& e) { \
        CAntiCoreSystem::Instance().HandleException(__FUNCTION__, __FILE__, __LINE__, e.what()); \
    } catch (...) { \
        CAntiCoreSystem::Instance().HandleException(__FUNCTION__, __FILE__, __LINE__, "Unknown exception"); \
    }

#define SAFE_POINTER_CHECK(ptr) \
    if (!CAntiCoreSystem::Instance().ValidatePointer(ptr)) { \
        CAntiCoreSystem::Instance().ReportError(__FUNCTION__, "Invalid pointer detected"); \
        return; \
    }

#define SAFE_POINTER_CHECK_RETURN(ptr, ret) \
    if (!CAntiCoreSystem::Instance().ValidatePointer(ptr)) { \
        CAntiCoreSystem::Instance().ReportError(__FUNCTION__, "Invalid pointer detected"); \
        return ret; \
    }

#define SAFE_MALLOC(size) \
    CAntiCoreSystem::Instance().SafeMalloc(size, __FILE__, __LINE__)

#define SAFE_FREE(ptr) \
    CAntiCoreSystem::Instance().SafeFree(ptr, __FILE__, __LINE__)

// Global error reporting macros
#define REPORT_ERROR(desc) \
    CAntiCoreSystem::Instance().ReportError(__FUNCTION__, desc)

#define REPORT_WARNING(desc) \
    CAntiCoreSystem::Instance().ReportWarning(__FUNCTION__, desc)

#define REPORT_INFO(desc) \
    CAntiCoreSystem::Instance().ReportInfo(__FUNCTION__, desc)

// Memory debugging in debug mode
#ifdef _DEBUG
    #define DEBUG_MALLOC(size) SAFE_MALLOC(size)
    #define DEBUG_FREE(ptr) SAFE_FREE(ptr)
#else
    #define DEBUG_MALLOC(size) malloc(size)
    #define DEBUG_FREE(ptr) free(ptr)
#endif

// Function debugging wrapper
#define DEBUG_FUNCTION() \
    SAFE_FUNCTION_ENTER(); \
    struct FunctionGuard { \
        ~FunctionGuard() { CAntiCoreSystem::Instance().ExitFunction(); } \
    } guard;

// Critical section protection
class CCriticalSectionGuard
{
    static bool s_bInCriticalSection;
public:
    CCriticalSectionGuard();
    ~CCriticalSectionGuard();
    
    static bool IsInCriticalSection() { return s_bInCriticalSection; }
};

#define CRITICAL_SECTION() CCriticalSectionGuard guard;

// Global initialization function
void InitializeAntiCoreSystem();
void DestroyAntiCoreSystem();

#endif // __HEADER_ANTICORE_SYSTEM_H__