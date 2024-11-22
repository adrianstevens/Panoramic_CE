// SetupDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include "SetupDll.h"
#include "resource.h"
#include "IssString.h"
#include "IssKey.h"
#include "IssRegistry.h"
#include "issdebug.h"
#include "IssLocalisation.h"
#include <regext.h>

HINSTANCE	g_hInst;

#define REG_KEY         _T("SOFTWARE\\Pano\\FreeCell") 


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hInst = (HINSTANCE)hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

SETUP_API codeINSTALL_INIT Install_Init(HWND hwndParent, 
							  BOOL fFirstCall,
							  BOOL fPreviouslyInstalled, 
							  LPCTSTR pszInstallDir)
{
    return codeINSTALL_INIT_CONTINUE;
}

SETUP_API codeINSTALL_EXIT Install_Exit(HWND    hwndParent,
							  LPCTSTR pszInstallDir,      // final install directory
							  WORD    cFailedDirs,
							  WORD    cFailedFiles,
							  WORD    cFailedRegKeys,
							  WORD    cFailedRegVals,
							  WORD    cFailedShortcuts)
{

    CIssString* oStr = CIssString::Instance();

    CIssKey key;

    // check if we're running a legit copy
    key.Init(_T("SOFTWARE\\Panoramic\\PanoFreecell"), _T("SOFTWARE\\Pano\\PanoFreeCell2"), 0x0, 0x393EF71D, 0x94EE6A15, 1, TRUE);
    key.CheckSetKey();
    key.CheckDemo();

    BOOL bIsSmartphone = FALSE;

    TCHAR szPlatform[STRING_NORMAL];
    ZeroMemory(szPlatform, sizeof(TCHAR)*STRING_NORMAL);
    if(SystemParametersInfo(SPI_GETPLATFORMTYPE, STRING_NORMAL, szPlatform, 0))
    {
        if(0 == oStr->Compare(szPlatform, _T("SmartPhone")))
        {
            bIsSmartphone = TRUE;
        }
    }

    if(!bIsSmartphone)
    {
        // set the WM 6.5 registry key
        HKEY hKey = NULL;

        TCHAR szRegKey[2*STRING_MAX];
        
        CIssLocalisation::GetRegGamesFolder(szRegKey, 2*STRING_MAX, _T("Panoramic FreeCell"));

        if(ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, szRegKey, 0, NULL, 0, 0, NULL, &hKey, NULL))
        {
            TCHAR szValue[STRING_MAX];
            oStr->StringCopy(szValue, (TCHAR*)pszInstallDir);
            oStr->Concatenate(szValue, _T("\\freecell90x90_192.png"));
            DWORD len = oStr->GetLength(szValue);
            RegSetValueEx(hKey, _T("Icon"), 0, REG_SZ, (LPBYTE)szValue, (len+1)*sizeof(TCHAR));
            RegFlushKey(hKey);
            RegCloseKey(hKey);
        }
    }


   	return codeINSTALL_EXIT_DONE;
}

SETUP_API codeUNINSTALL_INIT Uninstall_Init(HWND        hwndParent,
								  LPCTSTR     pszInstallDir)
{
	CIssString* oStr = CIssString::Instance();

    // have to close mo first
    HWND hWndMo = FindWindow(_T("ClassFreeCell"), _T("Panoramic FreeCell"));
    if(hWndMo)
    {
        PostMessage(hWndMo, WM_CLOSE, 0,0);
        // 10 seconds to close Twitter
        int iCount = 0;
        while(iCount < 20 && IsWindow(hWndMo))
        {
            iCount++;
            Sleep(500);
        }
    }

	// we should delete the key directories
	TCHAR szMsg[STRING_MAX];
	oStr->StringCopy(szMsg, IDS_MSG_DeleteSettings, STRING_MAX, g_hInst);
	if(IDYES == MessageBox(hwndParent, szMsg, _T("Uninstall"), MB_YESNO))
	{
        RegDeleteKeyNT(HKEY_CURRENT_USER, REG_KEY);
	}

	return codeUNINSTALL_INIT_CONTINUE;
}

SETUP_API codeUNINSTALL_EXIT Uninstall_Exit(HWND hwndParent)
{
	return codeUNINSTALL_EXIT_DONE;
}