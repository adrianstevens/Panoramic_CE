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
#include "IssKey.h"

HINSTANCE	g_hInst;

#define REG_PanoContacts		_T("SOFTWARE\\Pano\\PhoneGenius")
#define WND_ContactsApp		    _T("Pano Contacts")
#define WND_ContactsClass	    _T("ClassContacts")

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
    HINSTANCE hInst = LoadLibraryEx(_T("\\windows\\phone.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE);
    if(hInst)
        FreeLibrary(hInst);
    else
    {
        MessageBox(hwndParent, _T("Panoramic Contacts Genius will only work on a Windows Mobile phone edition device.  Please try installing on a proper phone device."), _T("Error"), MB_OK);
        return codeINSTALL_INIT_CANCEL;
    }

    if(IsAppInstalled(_T("Panoramic Phone Genius")))
    {
        MessageBox(hwndParent, _T("Panoramic Phone Genius is currently installed on the device.  Please uninstall Phone Genius before installing Panoramic Contacts Genius."), _T("Error"), MB_OK);
        return codeINSTALL_INIT_CANCEL;
    }

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

    oKey.Init(_T("SOFTWARE\\Panoramic\\PanoContactsManager"), _T("SOFTWARE\\Pano\\CM2"), 0x0, 0x393EF71D, 0x67ABCEF1, 5, TRUE);
    oKey.CheckSetKey();
    oKey.CheckDemo();
    oKey.Destroy();

	// put up the EULA screen
	CDlgEula dlgEula;
	if(IDOK != dlgEula.DoModal(hwndParent, g_hInst, IDD_DLG_Basic))
		return codeINSTALL_EXIT_UNINSTALL;

    CIssString* oStr = CIssString::Instance();

    TCHAR szPath[MAX_PATH];
    oStr->StringCopy(szPath, (TCHAR*)pszInstallDir);
    oStr->Concatenate(szPath, _T("\\"));

    // restore to the originals if they were set
    CObjAssociations obj(szPath);
    CDlgOptions dlgOptions(&obj);
    dlgOptions.DoModal(hwndParent, g_hInst, IDD_DLG_Basic);

    // do some install stuff of the battery
    
    
    //SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);'
    oStr->StringCopy(szPath, (TCHAR*)pszInstallDir);
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

    // turn Pano Contacts app off
    TCHAR szPath[MAX_PATH];
    //SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);
    oStr->StringCopy(szPath, (TCHAR*)pszInstallDir);
    oStr->Concatenate(szPath, _T("\\panocontacts.exe"));
    CreateProcess(szPath, _T("-q"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
    Sleep(1000);


#ifdef WIN32_PLATFORM_PSPC
    // remove the startup link file
    RemoveAutoLaunch(_T("PanoContacts"));
#endif

    oStr->StringCopy(szPath, (TCHAR*)pszInstallDir);
    oStr->Concatenate(szPath, _T("\\"));

    // restore to the originals if they were set
    CObjAssociations obj(szPath);
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

