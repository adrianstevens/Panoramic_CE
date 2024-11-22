// SetupDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include "SetupDll.h"
#include "resource.h"
#include "IssString.h"
#include "IssRegistry.h"
#include "DlgEula.h"
#include "DlgOptions.h"
#include "ObjAssociations.h"

HINSTANCE	g_hInst;

#define REG_PanoContacts		_T("SOFTWARE\\Pano\\ContactsManager")

void RemoveAutoLaunch(TCHAR* szName);
void SetAutoLaunch(TCHAR* szFileName, TCHAR* szName);

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
	// put up the EULA screen
	CDlgEula dlgEula;
	if(IDOK != dlgEula.DoModal(hwndParent, g_hInst, IDD_DLG_Basic))
		return codeINSTALL_EXIT_UNINSTALL;

    CDlgOptions dlgOptions;
    dlgOptions.DoModal(hwndParent, g_hInst, IDD_DLG_Basic);

    // do some install stuff of the battery
    CIssString* oStr = CIssString::Instance();
    TCHAR szPath[MAX_PATH];
    SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);
    oStr->Concatenate(szPath, _T("\\panocontacts.exe"));

    // launch the battery app
    CreateProcess(szPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);

    // make sure to put it in the auto start directory
    SetAutoLaunch(szPath, _T("PanoContacts"));

	Sleep(2500);
	SetActiveWindow(hwndParent);
	SetFocus(hwndParent);
	SetForegroundWindow(hwndParent);

	return codeINSTALL_EXIT_DONE;
}

SETUP_API codeUNINSTALL_INIT Uninstall_Init(HWND        hwndParent,
								  LPCTSTR     pszInstallDir)
{
	CIssString* oStr = CIssString::Instance();
#ifdef WIN32_PLATFORM_PSPC
    // remove the startup link file
    RemoveAutoLaunch(_T("PanoContacts"));
#endif

    // restore to the originals if they were set
    CObjAssociations obj;
    obj.RestoreContactOverride();
    obj.RestoreTodaySoftkey();


	// we should delete the key directories
	TCHAR szMsg[STRING_MAX];
	oStr->StringCopy(szMsg, IDS_MSG_DeleteSettings, STRING_MAX, g_hInst);
	if(IDYES == MessageBox(hwndParent, szMsg, _T("Uninstall"), MB_YESNO))
	{
		RegDeleteKeyNT(HKEY_CURRENT_USER, REG_PanoContacts);
	}

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

