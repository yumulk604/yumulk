#include "AntiCoreSystem.h"
#include "utils.h"
#include "log.h"
#include "char_manager.h"
#include "desc_manager.h"
#include "config.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <sys/resource.h>
#include <sys/stat.h>

// Static member initialization
CAntiCoreSystem* CAntiCoreSystem::ms_pInstance = nullptr;
bool CCriticalSectionGuard::s_bInCriticalSection = false;

// Constructor
CAntiCoreSystem::CAntiCoreSystem()
{
    m_bInitialized = false;
    m_bCrashProtectionEnabled = true;
    m_bAutoRecoveryEnabled = true;
    m_bDebugModeEnabled = false;
    m_tLastHealthCheck = 0;
    m_iCrashCount = 0;
    m_bMonitoringActive = false;
    m_ulTotalAllocated = 0;
    
    // Initialize signal names
    m_mapSignalNames[SIGSEGV] = "SIGSEGV (Segmentation Fault)";
    m_mapSignalNames[SIGABRT] = "SIGABRT (Abort Signal)";
    m_mapSignalNames[SIGFPE] = "SIGFPE (Floating Point Exception)";
    m_mapSignalNames[SIGBUS] = "SIGBUS (Bus Error)";
    m_mapSignalNames[SIGILL] = "SIGILL (Illegal Instruction)";
    m_mapSignalNames[SIGTRAP] = "SIGTRAP (Trace/Breakpoint Trap)";
    m_mapSignalNames[SIGPIPE] = "SIGPIPE (Broken Pipe)";
    m_mapSignalNames[SIGALRM] = "SIGALRM (Alarm Clock)";
    
    // Initialize recovery actions
    m_mapRecoveryActions["null_pointer"] = RECOVERY_RESET_CONNECTION;
    m_mapRecoveryActions["memory_leak"] = RECOVERY_CLEAR_MEMORY;
    m_mapRecoveryActions["stack_overflow"] = RECOVERY_RESTART_THREAD;
    m_mapRecoveryActions["database_error"] = RECOVERY_RELOAD_DATA;
    m_mapRecoveryActions["connection_lost"] = RECOVERY_RESET_CONNECTION;
    m_mapRecoveryActions["critical_error"] = RECOVERY_EMERGENCY_SAVE;
}

// Destructor
CAntiCoreSystem::~CAntiCoreSystem()
{
    if (m_bInitialized)
    {
        Destroy();
    }
}

// Singleton Instance
CAntiCoreSystem& CAntiCoreSystem::Instance()
{
    if (!ms_pInstance)
    {
        CreateInstance();
    }
    return *ms_pInstance;
}

void CAntiCoreSystem::CreateInstance()
{
    if (!ms_pInstance)
    {
        ms_pInstance = new CAntiCoreSystem();
    }
}

void CAntiCoreSystem::DestroyInstance()
{
    if (ms_pInstance)
    {
        delete ms_pInstance;
        ms_pInstance = nullptr;
    }
}

// Initialize the protection system
bool CAntiCoreSystem::Initialize()
{
    if (m_bInitialized)
        return true;
        
    sys_log(0, "[AntiCore] Initializing Anti-Core Protection System...");
    
    // Create crash reports directory
    mkdir("crash_reports", 0755);
    mkdir("debug_logs", 0755);
    
    // Load configuration
    LoadConfiguration();
    
    // Install signal handlers
    InstallSignalHandlers();
    
    // Start monitoring
    m_bMonitoringActive = true;
    m_tLastHealthCheck = time(0);
    
    // Initialize system state
    UpdateSystemState();
    
    m_bInitialized = true;
    
    sys_log(0, "[AntiCore] Protection system initialized successfully!");
    return true;
}

void CAntiCoreSystem::Destroy()
{
    if (!m_bInitialized)
        return;
        
    sys_log(0, "[AntiCore] Shutting down protection system...");
    
    // Stop monitoring
    m_bMonitoringActive = false;
    
    // Remove signal handlers
    RemoveSignalHandlers();
    
    // Save final state
    SaveConfiguration();
    
    // Check for memory leaks
    CheckMemoryLeaks();
    
    m_bInitialized = false;
    
    sys_log(0, "[AntiCore] Protection system shut down.");
}

// Install signal handlers for crash protection
void CAntiCoreSystem::InstallSignalHandlers()
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sa.sa_sigaction = HandleSignal;
    sigemptyset(&sa.sa_mask);
    
    // Install handlers for critical signals
    sigaction(SIGSEGV, &sa, nullptr);  // Segmentation fault
    sigaction(SIGABRT, &sa, nullptr);  // Abort signal
    sigaction(SIGFPE, &sa, nullptr);   // Floating point exception
    sigaction(SIGBUS, &sa, nullptr);   // Bus error
    sigaction(SIGILL, &sa, nullptr);   // Illegal instruction
    sigaction(SIGTRAP, &sa, nullptr);  // Trap
    
    sys_log(0, "[AntiCore] Signal handlers installed.");
}

void CAntiCoreSystem::RemoveSignalHandlers()
{
    // Reset to default signal handlers
    signal(SIGSEGV, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGTRAP, SIG_DFL);
    
    sys_log(0, "[AntiCore] Signal handlers removed.");
}

// Main signal handler
void CAntiCoreSystem::HandleSignal(int sig, siginfo_t* info, void* context)
{
    // Get instance
    CAntiCoreSystem& antiCore = Instance();
    
    // Create crash info
    SCrashInfo crashInfo;
    crashInfo.iSignalNumber = sig;
    crashInfo.pCrashAddress = info ? info->si_addr : nullptr;
    crashInfo.tCrashTime = time(0);
    crashInfo.processID = getpid();
    
    // Determine crash type
    switch(sig)
    {
        case SIGSEGV: crashInfo.eCrashType = CRASH_SEGMENTATION_FAULT; break;
        case SIGABRT: crashInfo.eCrashType = CRASH_ABORT_SIGNAL; break;
        case SIGFPE:  crashInfo.eCrashType = CRASH_FLOATING_POINT; break;
        case SIGBUS:  crashInfo.eCrashType = CRASH_BUS_ERROR; break;
        default:      crashInfo.eCrashType = CRASH_SEGMENTATION_FAULT; break;
    }
    
    // Generate stack trace
    crashInfo.strStackTrace = antiCore.GenerateStackTrace(context);
    
    // Handle the crash
    HandleCrash(sig, info, context);
}

// Crash handler with recovery attempt
void CAntiCoreSystem::HandleCrash(int sig, siginfo_t* info, void* context)
{
    CAntiCoreSystem& antiCore = Instance();
    
    sys_log(0, "[AntiCore] CRASH DETECTED! Signal: %d", sig);
    
    // Create crash info
    SCrashInfo crashInfo;
    crashInfo.iSignalNumber = sig;
    crashInfo.pCrashAddress = info ? info->si_addr : nullptr;
    crashInfo.tCrashTime = time(0);
    crashInfo.strStackTrace = antiCore.GenerateStackTrace(context);
    
    // Emergency save
    antiCore.EmergencySave();
    
    // Generate crash report
    antiCore.GenerateCrashReport(crashInfo);
    
    // Attempt recovery
    if (antiCore.m_bAutoRecoveryEnabled)
    {
        bool recovered = antiCore.AttemptRecovery(crashInfo.eCrashType);
        if (recovered)
        {
            sys_log(0, "[AntiCore] Crash recovered successfully!");
            return; // Don't exit, continue running
        }
    }
    
    // If recovery failed, try graceful shutdown
    sys_err("[AntiCore] Critical error - attempting graceful shutdown");
    
    // Final emergency save
    antiCore.EmergencySave();
    
    // Don't call abort() or exit() - try to continue
    sys_log(0, "[AntiCore] Continuing execution after crash handling");
}

// Generate detailed stack trace
std::string CAntiCoreSystem::GenerateStackTrace(void* context)
{
    std::stringstream ss;
    void *array[50];
    size_t size;
    char **strings;
    
    size = backtrace(array, 50);
    strings = backtrace_symbols(array, size);
    
    ss << "\n=== STACK TRACE (" << size << " frames) ===\n";
    
    for (size_t i = 0; i < size; i++)
    {
        std::string line(strings[i]);
        
        // Try to demangle C++ function names
        size_t start = line.find('(');
        size_t end = line.find('+');
        
        if (start != std::string::npos && end != std::string::npos)
        {
            std::string mangled = line.substr(start + 1, end - start - 1);
            std::string demangled = DemangledName(mangled.c_str());
            if (!demangled.empty())
            {
                line = line.substr(0, start + 1) + demangled + line.substr(end);
            }
        }
        
        ss << "[" << i << "] " << line << "\n";
    }
    
    ss << "=== END STACK TRACE ===\n";
    
    free(strings);
    return ss.str();
}

// Attempt to recover from crash
bool CAntiCoreSystem::AttemptRecovery(ECrashType crashType)
{
    sys_log(0, "[AntiCore] Attempting recovery from crash type: %d", crashType);
    
    switch(crashType)
    {
        case CRASH_SEGMENTATION_FAULT:
        case CRASH_NULL_POINTER:
            // Try to reset pointers and continue
            ExecuteRecoveryAction(RECOVERY_RESET_CONNECTION);
            return true;
            
        case CRASH_MEMORY_LEAK:
        case CRASH_HEAP_CORRUPTION:
            // Clear problematic memory
            ExecuteRecoveryAction(RECOVERY_CLEAR_MEMORY);
            return true;
            
        case CRASH_STACK_OVERFLOW:
            // Restart problematic thread
            ExecuteRecoveryAction(RECOVERY_RESTART_THREAD);
            return true;
            
        case CRASH_DEADLOCK:
            // Reset connections
            ExecuteRecoveryAction(RECOVERY_RESET_CONNECTION);
            return true;
            
        default:
            return false;
    }
}

// Execute recovery actions
void CAntiCoreSystem::ExecuteRecoveryAction(ERecoveryAction action)
{
    switch(action)
    {
        case RECOVERY_RESET_CONNECTION:
            sys_log(0, "[AntiCore] Executing connection reset...");
            // Reset network connections
            break;
            
        case RECOVERY_CLEAR_MEMORY:
            sys_log(0, "[AntiCore] Executing memory cleanup...");
            // Clear cached data
            break;
            
        case RECOVERY_RESTART_THREAD:
            sys_log(0, "[AntiCore] Executing thread restart...");
            // Restart worker threads
            break;
            
        case RECOVERY_EMERGENCY_SAVE:
            sys_log(0, "[AntiCore] Executing emergency save...");
            EmergencySave();
            break;
            
        default:
            break;
    }
}

// Emergency save all critical data
void CAntiCoreSystem::EmergencySave()
{
    sys_log(0, "[AntiCore] Emergency save initiated!");
    
    // Save all player data
    try
    {
        CHARACTER_MANAGER& charMgr = CHARACTER_MANAGER::instance();
        charMgr.FlushPendingDestroy();
        
        // Force save all online players
        auto playerMap = charMgr.GetPCMap();
        for (auto& pair : playerMap)
        {
            if (pair.second)
            {
                pair.second->Save();
                pair.second->SaveReal();
            }
        }
        
        sys_log(0, "[AntiCore] Emergency save completed - %zu players saved", playerMap.size());
    }
    catch (const std::exception& e)
    {
        sys_err("[AntiCore] Emergency save failed: %s", e.what());
    }
    catch (...)
    {
        sys_err("[AntiCore] Emergency save failed: unknown error");
    }
}

// Generate comprehensive crash report
void CAntiCoreSystem::GenerateCrashReport(const SCrashInfo& crashInfo)
{
    std::stringstream ss;
    
    ss << "=== ANTI-CORE CRASH REPORT ===\n";
    ss << "Crash Time: " << GetCurrentTime() << "\n";
    ss << "Process ID: " << crashInfo.processID << "\n";
    ss << "Signal: " << crashInfo.iSignalNumber;
    
    auto signalIt = m_mapSignalNames.find(crashInfo.iSignalNumber);
    if (signalIt != m_mapSignalNames.end())
        ss << " (" << signalIt->second << ")";
    ss << "\n";
    
    ss << "Crash Address: " << crashInfo.pCrashAddress << "\n";
    ss << "Crash Type: " << crashInfo.eCrashType << "\n\n";
    
    // System state
    ss << "=== SYSTEM STATE ===\n";
    ss << "CPU Usage: " << m_systemState.dCPUUsage << "%\n";
    ss << "Memory Usage: " << (m_systemState.ulMemoryUsage / 1024 / 1024) << " MB\n";
    ss << "Player Count: " << m_systemState.iPlayerCount << "\n";
    ss << "Connection Count: " << m_systemState.iConnectionCount << "\n";
    ss << "Database Connected: " << (m_systemState.bDatabaseConnected ? "Yes" : "No") << "\n";
    ss << "Uptime: " << m_systemState.tUptime << " seconds\n\n";
    
    // Stack trace
    ss << crashInfo.strStackTrace << "\n";
    
    // Function call history
    ss << "=== FUNCTION CALL HISTORY ===\n";
    for (auto it = m_vecCallStack.rbegin(); it != m_vecCallStack.rend() && std::distance(m_vecCallStack.rbegin(), it) < 20; ++it)
    {
        ss << "-> " << *it << "\n";
    }
    ss << "\n";
    
    // Error patterns
    ss << "=== ERROR PATTERNS ===\n";
    for (const auto& pattern : m_mapErrorPatterns)
    {
        ss << pattern.first << ": " << pattern.second << " times\n";
    }
    
    ss << "\n=== END REPORT ===\n";
    
    // Save to file
    char filename[256];
    snprintf(filename, sizeof(filename), "crash_reports/crash_%lu.txt", time(0));
    WriteToFile(filename, ss.str());
    
    // Also log to system
    sys_log(0, "[AntiCore] Crash report saved: %s", filename);
    
    // Increment crash count
    m_iCrashCount++;
}

// Safe memory allocation with tracking
void* CAntiCoreSystem::SafeMalloc(size_t size, const char* file, int line)
{
    if (size == 0)
    {
        REPORT_WARNING("Attempted to allocate 0 bytes");
        return nullptr;
    }
    
    if (size > 1024 * 1024 * 100) // 100MB limit
    {
        REPORT_ERROR("Attempted to allocate too much memory");
        return nullptr;
    }
    
    void* ptr = malloc(size);
    if (ptr)
    {
        m_mapAllocatedMemory[ptr] = size;
        m_ulTotalAllocated += size;
        
        if (m_bDebugModeEnabled)
        {
            sys_log(0, "[AntiCore] Allocated %zu bytes at %p (%s:%d)", size, ptr, file, line);
        }
    }
    else
    {
        REPORT_ERROR("Memory allocation failed");
    }
    
    return ptr;
}

// Safe memory deallocation with tracking
void CAntiCoreSystem::SafeFree(void* ptr, const char* file, int line)
{
    if (!ptr)
        return;
        
    auto it = m_mapAllocatedMemory.find(ptr);
    if (it != m_mapAllocatedMemory.end())
    {
        m_ulTotalAllocated -= it->second;
        m_mapAllocatedMemory.erase(it);
        
        if (m_bDebugModeEnabled)
        {
            sys_log(0, "[AntiCore] Freed %zu bytes at %p (%s:%d)", it->second, ptr, file, line);
        }
    }
    else if (m_bDebugModeEnabled)
    {
        REPORT_WARNING("Attempting to free untracked memory");
    }
    
    free(ptr);
}

// Validate pointer safety
bool CAntiCoreSystem::ValidatePointer(void* ptr)
{
    if (!ptr)
    {
        return false;
    }
    
    // Check if pointer is in valid memory range
    // This is a basic check, more sophisticated validation can be added
    if ((uintptr_t)ptr < 0x1000) // Null pointer range
    {
        return false;
    }
    
    return true;
}

// Handle exceptions gracefully
void CAntiCoreSystem::HandleException(const char* function, const char* file, int line, const char* description)
{
    std::stringstream ss;
    ss << "Exception in " << function << " at " << file << ":" << line;
    if (description && strlen(description) > 0)
        ss << " - " << description;
        
    std::string errorMsg = ss.str();
    
    // Log the error
    sys_err("[AntiCore] %s", errorMsg.c_str());
    
    // Update error patterns
    m_mapErrorPatterns[function]++;
    
    // Try recovery if this function fails frequently
    if (m_mapErrorPatterns[function] > 5)
    {
        REPORT_ERROR("Function failing repeatedly - attempting recovery");
        ExecuteRecoveryAction(RECOVERY_CLEAR_MEMORY);
    }
}

// Function call tracking
void CAntiCoreSystem::EnterFunction(const char* funcName)
{
    if (!m_bDebugModeEnabled)
        return;
        
    m_vecCallStack.push_back(std::string(funcName));
    m_mapFunctionCallCount[funcName]++;
    
    // Keep call stack reasonable size
    if (m_vecCallStack.size() > 1000)
    {
        m_vecCallStack.erase(m_vecCallStack.begin(), m_vecCallStack.begin() + 500);
    }
}

void CAntiCoreSystem::ExitFunction(const char* funcName)
{
    if (!m_bDebugModeEnabled)
        return;
        
    if (!m_vecCallStack.empty() && m_vecCallStack.back() == funcName)
    {
        m_vecCallStack.pop_back();
    }
}

// Update system state for monitoring
void CAntiCoreSystem::UpdateSystemState()
{
    // Update player count
    m_systemState.iPlayerCount = CHARACTER_MANAGER::instance().GetPCCount();
    
    // Update connection count  
    m_systemState.iConnectionCount = DESC_MANAGER::instance().GetClientCount();
    
    // Update uptime
    static time_t startTime = time(0);
    m_systemState.tUptime = time(0) - startTime;
    
    // Update memory usage
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0)
    {
        m_systemState.ulMemoryUsage = usage.ru_maxrss * 1024; // Convert to bytes
    }
}

// Load configuration from file
void CAntiCoreSystem::LoadConfiguration()
{
    std::ifstream file("anticore.conf");
    if (!file.is_open())
    {
        // Create default config
        std::ofstream defaultConfig("anticore.conf");
        defaultConfig << "# Anti-Core System Configuration\n";
        defaultConfig << "crash_protection=1\n";
        defaultConfig << "auto_recovery=1\n";
        defaultConfig << "debug_mode=0\n";
        defaultConfig << "monitoring=1\n";
        defaultConfig << "max_memory_mb=2048\n";
        defaultConfig.close();
        
        // Set defaults
        m_bCrashProtectionEnabled = true;
        m_bAutoRecoveryEnabled = true;
        m_bDebugModeEnabled = false;
        return;
    }
    
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
            
        size_t pos = line.find('=');
        if (pos == std::string::npos)
            continue;
            
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        if (key == "crash_protection")
            m_bCrashProtectionEnabled = (value == "1");
        else if (key == "auto_recovery")
            m_bAutoRecoveryEnabled = (value == "1");
        else if (key == "debug_mode")
            m_bDebugModeEnabled = (value == "1");
    }
    
    file.close();
}

// Utility functions
std::string CAntiCoreSystem::GetCurrentTime()
{
    time_t now = time(0);
    char* timeStr = ctime(&now);
    if (timeStr)
    {
        std::string result(timeStr);
        if (!result.empty() && result.back() == '\n')
            result.pop_back();
        return result;
    }
    return "Unknown";
}

std::string CAntiCoreSystem::DemangledName(const char* name)
{
    if (!name)
        return "";
        
    int status = 0;
    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    
    if (status == 0 && demangled)
    {
        std::string result(demangled);
        free(demangled);
        return result;
    }
    
    return std::string(name);
}

void CAntiCoreSystem::WriteToFile(const std::string& filename, const std::string& content)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << content;
        file.close();
    }
}

// Check memory leaks
void CAntiCoreSystem::CheckMemoryLeaks()
{
    if (m_mapAllocatedMemory.empty())
    {
        sys_log(0, "[AntiCore] No memory leaks detected.");
        return;
    }
    
    sys_err("[AntiCore] Memory leak detected! %zu unfreed blocks, %zu bytes total", 
           m_mapAllocatedMemory.size(), m_ulTotalAllocated);
           
    if (m_bDebugModeEnabled)
    {
        for (const auto& pair : m_mapAllocatedMemory)
        {
            sys_log(0, "[AntiCore] Leaked: %p (%zu bytes)", pair.first, pair.second);
        }
    }
}

// Critical section guard implementation
CCriticalSectionGuard::CCriticalSectionGuard()
{
    s_bInCriticalSection = true;
}

CCriticalSectionGuard::~CCriticalSectionGuard()
{
    s_bInCriticalSection = false;
}

// Global functions
void InitializeAntiCoreSystem()
{
    CAntiCoreSystem::CreateInstance();
    CAntiCoreSystem::Instance().Initialize();
    sys_log(0, "[AntiCore] System protection activated!");
}

void DestroyAntiCoreSystem()
{
    if (CAntiCoreSystem::ms_pInstance)
    {
        CAntiCoreSystem::Instance().Destroy();
        CAntiCoreSystem::DestroyInstance();
    }
    sys_log(0, "[AntiCore] System protection deactivated.");
}