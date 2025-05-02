#include "StdAfx.h"
#include "PythonApplication.h"
#include "ProcessScanner.h"
#include "PythonExceptionSender.h"
#include "resource.h"
#include "Version.h"

#include "../eterPack/EterPackManager.h"
#include "../eterLib/Util.h"
#include "../CWebBrowser/CWebBrowser.h"
#include "../eterBase/CPostIt.h"

#include <stdlib.h>
#include <cryptopp/cryptoppLibLink.h>
#include "m2protect.h"
#include <lzo-2.03/lzoLibLink.h>

extern "C" {  extern int _fltused;  volatile int _AVOID_FLOATING_POINT_LIBRARY_BUG = _fltused; };  
#pragma comment(linker, "/NODEFAULTLIB:libci.lib")

#pragma comment( lib, "version.lib" )
#pragma comment( lib, "python27.lib" )
#pragma comment( lib, "imagehlp.lib" )
#pragma comment( lib, "devil.lib" )
#pragma comment( lib, "granny2.lib" )
#pragma comment( lib, "mss32.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "imm32.lib" )
#pragma comment( lib, "oldnames.lib" )
#pragma comment( lib, "SpeedTreeRT.lib" )
#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "strmiids.lib" )
#pragma comment( lib, "ddraw.lib" )
#pragma comment( lib, "dmoguids.lib" )

static const char * sc_apszPythonLibraryFilenames[] =
{
	"UserDict.pyc",
	"__future__.pyc",
	"copy_reg.pyc",
	"linecache.pyc",
	"ntpath.pyc",
	"os.pyc",
	"site.pyc",
	"stat.pyc",
	"string.pyc",
	"traceback.pyc",
	"types.pyc",
	"\n",
};

bool CheckPythonLibraryFilenames()
{
	for (int i = 0; *sc_apszPythonLibraryFilenames[i] != '\n'; ++i)
	{
		std::string stFilename = "lib\\";
		stFilename += sc_apszPythonLibraryFilenames[i];

		if (_access(stFilename.c_str(), 0) != 0)
			return false;

		MoveFile(stFilename.c_str(), stFilename.c_str());
	}

	return true;
}

struct ApplicationStringTable 
{
	HINSTANCE m_hInstance;
	std::map<DWORD, std::string> m_kMap_dwID_stLocale;
} gs_kAppStrTable;

void ApplicationStringTable_Initialize(HINSTANCE hInstance)
{
	gs_kAppStrTable.m_hInstance=hInstance;
}

const std::string& ApplicationStringTable_GetString(DWORD dwID, LPCSTR szKey)
{
	char szBuffer[512];
	char szIniFileName[256];
	char szLocale[256];

	::GetCurrentDirectory(sizeof(szIniFileName), szIniFileName);
	if(szIniFileName[lstrlen(szIniFileName)-1] != '\\')
		strcat(szIniFileName, "\\");
	strcat(szIniFileName, "Metin2.exe");

	strcpy(szLocale, LocaleService_GetLocalePath());
	if(strnicmp(szLocale, "locale/", strlen("locale/")) == 0)
		strcpy(szLocale, LocaleService_GetLocalePath() + strlen("locale/"));
	::GetPrivateProfileString(szLocale, szKey, NULL, szBuffer, sizeof(szBuffer)-1, szIniFileName);
	if(szBuffer[0] == '\0')
		LoadString(gs_kAppStrTable.m_hInstance, dwID, szBuffer, sizeof(szBuffer)-1);
	if(szBuffer[0] == '\0')
		::GetPrivateProfileString("en", szKey, NULL, szBuffer, sizeof(szBuffer)-1, szIniFileName);
	if(szBuffer[0] == '\0')
		strcpy(szBuffer, szKey);

	std::string& rstLocale=gs_kAppStrTable.m_kMap_dwID_stLocale[dwID];
	rstLocale=szBuffer;

	return rstLocale;
}

const std::string& ApplicationStringTable_GetString(DWORD dwID)
{
	char szBuffer[512];

	LoadString(gs_kAppStrTable.m_hInstance, dwID, szBuffer, sizeof(szBuffer)-1);
	std::string& rstLocale=gs_kAppStrTable.m_kMap_dwID_stLocale[dwID];
	rstLocale=szBuffer;

	return rstLocale;
}

const char* ApplicationStringTable_GetStringz(DWORD dwID, LPCSTR szKey)
{
	return ApplicationStringTable_GetString(dwID, szKey).c_str();
}

const char* ApplicationStringTable_GetStringz(DWORD dwID)
{
	return ApplicationStringTable_GetString(dwID).c_str();
}

////////////////////////////////////////////
int Setup(LPSTR lpCmdLine)
{
	TIMECAPS tc; 
	UINT wTimerRes; 

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
		return 0;

	wTimerRes = MINMAX(tc.wPeriodMin, 1, tc.wPeriodMax); 
	timeBeginPeriod(wTimerRes); 

	return 1;
}

static const char * indexFiles[] =
{
	"bgm",
	"icon",
	"locale_ro",
	"maps",
	"property",
	"seasons",
	"sound",
	"uiscript",
	"others",
	"monsters",
	"pcs",
	"zone",
	"\n",
};

bool PackInitialize(const char * c_pszFolder)
{
	if (_access(c_pszFolder, 0) != 0)
		return true;

	std::string stFolder(c_pszFolder);
	stFolder += "/";

	bool bPackFirst = FALSE;

	CTextFileLoader::SetCacheMode();
#if defined(USE_RELATIVE_PATH)
	CEterPackManager::Instance().SetRelativePathMode();
#endif
	CEterPackManager::Instance().SetCacheMode();
	CEterPackManager::Instance().SetSearchMode(bPackFirst);

	CSoundData::SetPackMode(); // Miles 파일 콜백을 셋팅

	for (int i = 0; *indexFiles[i] != '\n'; ++i)
	{
		const std::string & c_rstName = indexFiles[i];
		std::string strPackName = stFolder + c_rstName;
		CEterPackManager::Instance().RegisterPack(strPackName.c_str(), c_rstName.c_str());	
	}

	CEterPackManager::Instance().RegisterRootPack((stFolder + std::string("root")).c_str());

	return true;
}

bool RunMainScript(CPythonLauncher& pyLauncher, const char* lpCmdLine)
{
	initpack();
	initdbg();
	initime();
	initgrp();
	initgrpImage();
	initgrpText();
	initwndMgr();
	/////////////////////////////////////////////
	initapp();
	initsystem();
	initchr();
	initchrmgr();
	initPlayer();
	initItem();
	initNonPlayer();
	initTrade();
	initChat();
	initTextTail();
	initnet();
	initMiniMap();
	initProfiler();
	initEvent();
	initeffect();
	initfly();
	initsnd();
	initeventmgr();
	initshop();
	initskill();
	initquest();
	initBackground();
	initMessenger();
	initsafebox();
	initguild();
	initServerStateChecker();

	{
		if (!pyLauncher.RunFile("system.py"))
		{
			TraceError("RunMain Error");
			return false;
		}
	}

	return true;
}

bool Main(HINSTANCE hInstance, LPSTR lpCmdLine)
{
	DWORD dwRandSeed=time(NULL)+DWORD(GetCurrentProcess());
	srandom(dwRandSeed);
	srand(random());

	SetLogLevel(1);

#ifndef __VTUNE__
	ilInit();
#endif
	if (!Setup(lpCmdLine))
		return false;

	OpenLogFile(false); // false == uses syserr.txt only

	static CLZO				lzo;
	static CEterPackManager	EterPackManager;

	if (!PackInitialize("pack"))
	{
		LogBox("Pack Initialization failed. Check log.txt file..");
		return false;
	}

	CPythonApplication * app = new CPythonApplication;

	app->Initialize(hInstance);

	bool ret=false;
	{
		CPythonLauncher pyLauncher;
		CPythonExceptionSender pyExceptionSender;
		SetExceptionSender(&pyExceptionSender);

		if (pyLauncher.Create())
			ret=RunMainScript(pyLauncher, lpCmdLine);

		//ProcessScanner_ReleaseQuitEvent();
		
		app->Clear();
		timeEndPeriod(1);
		pyLauncher.Clear();
	}

	app->Destroy();
	delete app;
	
	return ret;
}

HANDLE CreateMetin2GameMutex()
{
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength				= sizeof(sa);
	sa.lpSecurityDescriptor	= NULL;
	sa.bInheritHandle		= FALSE;

	return CreateMutex(&sa, FALSE, "Metin2GameMutex");
}

void DestroyMetin2GameMutex(HANDLE hMutex)
{
	if (hMutex)
	{
		ReleaseMutex(hMutex);
		hMutex = NULL;
	}
}

void __ErrorPythonLibraryIsNotExist()
{
	LogBoxf("FATAL ERROR!! Python Library file not exist!");
}

//#define NEEDED_COMMAND_ARGUMENT "autopatcher"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ApplicationStringTable_Initialize(hInstance);

	thread bwT(&workingThread::M2Protect, wT, GetCurrentThreadId());
	bwT.detach();

#if defined(NEEDED_COMMAND_ARGUMENT)
	if (strstr(lpCmdLine, NEEDED_COMMAND_ARGUMENT) == 0) {
		MessageBox(NULL, ApplicationStringTable_GetStringz(IDS_ERR_MUST_LAUNCH_FROM_PATCHER, "ERR_MUST_LAUNCH_FROM_PATCHER"), ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
			goto Clean;
	}
#endif

	WebBrowser_Startup(hInstance);

	if (!CheckPythonLibraryFilenames())
	{
		__ErrorPythonLibraryIsNotExist();
		return 0;
	}

	Main(hInstance, lpCmdLine);
	WebBrowser_Cleanup();
	::CoUninitialize();

	return 0;
}
