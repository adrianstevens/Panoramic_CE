// SetupDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include "SetupDll.h"
#include "resource.h"
#include "IssString.h"
#include "IssRegistry.h"
#include "DlgEula.h"
#include "IssKey.h"
#include "ObjAssociations.h"


HINSTANCE	g_hInst;

#define REG_PanoLaunchpad		_T("SOFTWARE\\Pano\\Launcher")

void RemoveAutoLaunch(TCHAR* szName);
void SetAutoLaunch(TCHAR* szFileName, TCHAR* szName);
BOOL IsAppInstalled(TCHAR* szInstallName);

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

    CIssKey oKey;

    oKey.Init(_T("SOFTWARE\\Panoramic\\PanoLaunchpad"), _T("SOFTWARE\\Pano\\LP2"), 0x0, 0x383EF71D, 0x67ABCEF1, 2, TRUE);
    oKey.CheckSetKey();
    oKey.CheckDemo();
    oKey.Destroy();


	// put up the EULA screen
	/*CDlgEula dlgEula;
	if(IDOK != dlgEula.DoModal(hwndParent, g_hInst, IDD_DLG_Basic))
		return codeINSTALL_EXIT_UNINSTALL;*/


    // Start both apps
    CIssString* oStr = CIssString::Instance();
    TCHAR szPath[MAX_PATH];
    //SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);
    oStr->StringCopy(szPath, (TCHAR*)pszInstallDir);
    oStr->Concatenate(szPath, _T("\\panolauncher.exe"));
    CreateProcess(szPath, _T("-h"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
    SetAutoLaunch(szPath, _T("Panolauncher"));

    /*SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);
    oStr->Concatenate(szPath, _T("\\panostart.exe"));
    CreateProcess(szPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);  
    SetAutoLaunch(szPath, _T("Panostart"));*/

	/*Sleep(2500);
	SetActiveWindow(hwndParent);
	SetFocus(hwndParent);
	SetForegroundWindow(hwndParent);*/

	return codeINSTALL_EXIT_DONE;
}

SETUP_API codeUNINSTALL_INIT Uninstall_Init(HWND        hwndParent,
								  LPCTSTR     pszInstallDir)
{
	CIssString* oStr = CIssString::Instance();

    // turn Pano launcher app off
    TCHAR szPath[MAX_PATH];
    //SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);
    oStr->StringCopy(szPath, (TCHAR*)pszInstallDir);
    oStr->Concatenate(szPath, _T("\\panolauncher.exe"));
    CreateProcess(szPath, _T("-q"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
    Sleep(1000);

    // turn menu launcher off
    HWND hWndPrevious = FindWindow(_T("ClassLauncherStart"), NULL);
    if(hWndPrevious)
    {
        PostMessage(hWndPrevious, WM_USER + 6123, 0,0);
        Sleep(500);
    }


#ifdef WIN32_PLATFORM_PSPC
    // remove the startup link file
    RemoveAutoLaunch(_T("PanoLauncher"));
    RemoveAutoLaunch(_T("PanoStart"));
#endif

    oStr->StringCopy(szPath, (TCHAR*)pszInstallDir);
    oStr->Concatenate(szPath, _T("\\"));

    CObjAssociations obj(szPath);

    BOOL bSoftResetMessage = obj.IsTodaySoftkeySetL();
    obj.RestoreTodaySoftkeyL();

	// we should delete the key directories
	TCHAR szMsg[STRING_MAX];
	oStr->StringCopy(szMsg, IDS_MSG_DeleteSettings, STRING_MAX, g_hInst);
	if(IDYES == MessageBox(hwndParent, szMsg, _T("Uninstall"), MB_YESNO))
	{
        RegDeleteKeyNT(HKEY_CURRENT_USER, REG_PanoLaunchpad);
	}

    if(bSoftResetMessage)
        MessageBox(hwndParent, _T("You may need to soft reset your device in order to set your settings back to default."), _T("Uninstall"), MB_OK);

	return codeUNINSTALL_INIT_CONTINUE;
}

SETUP_API codeUNINSTALL_EXIT Uninstall_Exit(HWND hwndParent)
{
	return codeUNINSTALL_EXIT_DONE;
}

void RemoveAutoLaunch(TCHAR* szName)
{
	TCHAR szStartupFolder[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szStartupFolder, CSIDL_STARTUP, FALSE);

	CIssString* oStr = CIssString::Instance();
	oStr->Concatenate(szStartupFolder, _T("\\"));
	oStr->Concatenate(szStartupFolder, szName);
	oStr->Concatenate(szStartupFolder, _T(".lnk"));

	DeleteFile(szStartupFolder);
}

void SetAutoLaunch(TCHAR* szFileName, TCHAR* szName)
{
    TCHAR szStartupFolder[MAX_PATH];
    SHGetSpecialFolderPath(NULL, szStartupFolder, CSIDL_STARTUP, FALSE);

    CIssString* oStr = CIssString::Instance();
    oStr->Concatenate(szStartupFolder, _T("\\"));
    oStr->Concatenate(szStartupFolder, szName);
    oStr->Concatenate(szStartupFolder, _T(".lnk"));

    TCHAR szFullFileName[STRING_MAX];
    oStr->Format(szFullFileName, _T("\"%s\""), szFileName);

    SHCreateShortcut(szStartupFolder, szFullFileName);

    return;
}

BOOL IsAppInstalled(TCHAR* szInstallName)
{
    TCHAR szKey[STRING_MAX];

    CIssString* oStr = CIssString::Instance();

    oStr->StringCopy(szKey, _T("Software\\Apps\\"));
    oStr->Concatenate(szKey, szInstallName);

    HKEY hKey = NULL;
    if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0L, 0, &hKey))
    {
        RegCloseKey(hKey);
        return TRUE;
    }

    return FALSE;
}

