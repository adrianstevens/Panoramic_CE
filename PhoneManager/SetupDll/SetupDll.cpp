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
#include "objringtoneengine.h"
#include "ObjPhoneProfile.h"
#include "DlgTodayScreen.h"
#include "CommonDefines.h"
#include "ObjRingtonerulelist.h"
#include "IssKey.h"
#include "ServerCommunicate.h"
#include "regext.h"


#define NAME_PhoneServer    _T("PhoneServer")
#define NAME_PhoneContacts  _T("PhoneContacts")
#define REG_Today		    _T("SOFTWARE\\Microsoft\\Today\\Items")
#define REG_TodayPhoneGenius _T("Phone Genius")
#define REG_TodayFull   	_T("SOFTWARE\\Microsoft\\Today\\Items\\Phone Genius")
#define REG_Calendar        _T("\"Calendar\"")
#define REG_Tasks           _T("\"Tasks\"")
#define REG_KeyOrder        _T("Order")
#define REG_KeyEnabled      _T("Enabled")

#define REG_PanoPhoneGenius		_T("SOFTWARE\\Pano\\PhoneGenius")
#define WND_ContactsApp		    _T("Pano Contacts")
#define WND_ContactsClass	    _T("ClassContacts")

HINSTANCE	g_hInst;

void RemoveAutoLaunch(TCHAR* szName);
void SetAutoLaunch(TCHAR* szFileName, TCHAR* szName);
void SetTodayKeyItem(TCHAR* szKeyName, TCHAR* szValue, DWORD dwValue);
void SetTodayScreen();
BOOL IsAppInstalled(TCHAR* szInstallName);
BOOL EnumWindowsProc(HWND hWnd, DWORD lParam);
void SetDialerDll();
void ResetDialerDll();
HKEY GetKey(HKEY hKeyRoot, TCHAR* szKey, BOOL bCreate);
BOOL RestoreValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination);
BOOL BackupValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination);

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
		MessageBox(hwndParent, _T("Panoramic Phone Genius will only work on a Windows Mobile phone edition device.  Please try installing on a proper phone device."), _T("Error"), MB_OK);
		return codeINSTALL_INIT_CANCEL;
	}

    if(IsAppInstalled(_T("Panoramic Contacts Genius")))
    {
        MessageBox(hwndParent, _T("Panoramic Contacts Genius is currently installed on the device.  Please uninstall Contacts Genius before installing Panoramic Phone Genius."), _T("Error"), MB_OK);
        return codeINSTALL_INIT_CANCEL;
    }

    if(IsAppInstalled(_T("Panoramic Call Genius")))
    {
        MessageBox(hwndParent, _T("Panoramic Call Genius is currently installed on the device.  Please uninstall Call Genius before installing Panoramic Phone Genius."), _T("Error"), MB_OK);
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
    CIssString* oStr = CIssString::Instance();

    CIssKey oKey;
    oKey.Init(_T("SOFTWARE\\Panoramic\\CallManager"), _T("SOFTWARE\\Pano\\PS2"), 0x0, 0x1234CDEF, 0x67ABCEF1, 5, TRUE);
    oKey.CheckSetKey();
    oKey.CheckDemo();
    oKey.Destroy();

    oKey.Init(_T("SOFTWARE\\Panoramic\\PanoContactsManager"), _T("SOFTWARE\\Pano\\CM2"), 0x0, 0x393EF71D, 0x67ABCEF1, 5, TRUE);
    oKey.CheckSetKey();
    oKey.CheckDemo();
    oKey.Destroy();

    oKey.Init(_T("SOFTWARE\\Panoramic\\AreaCode"), _T("SOFTWARE\\Pano\\AC2"), 0x0, 0x9834CDEF, 0x23ABCEF1, 5, TRUE);
    oKey.CheckSetKey();
    oKey.CheckDemo();
    oKey.Destroy();


    // put up the EULA screen
    CDlgEula dlgEula;
    if(IDOK != dlgEula.DoModal(hwndParent, g_hInst, IDD_DLG_Basic))
        return codeINSTALL_EXIT_UNINSTALL;

    TCHAR szPath[MAX_PATH];
    oStr->StringCopy(szPath, (TCHAR*)pszInstallDir);
    oStr->Concatenate(szPath, _T("\\"));

    CObjAssociations obj(szPath);
    // some basic setup options
    CDlgOptions dlgOptions(&obj);
    dlgOptions.DoModal(hwndParent, g_hInst, IDD_DLG_Basic);

    CObjRingtoneRuleList objRingRule;
    objRingRule.ReloadList();
	objRingRule.FindAllContactRingtones();
    objRingRule.SaveList(FALSE);

    // should we put up the today screen?
    /*CDlgTodayScreen dlgToday;
    if(IDOK == dlgToday.DoModal(hwndParent, g_hInst, IDD_DLG_Basic))
    {
        SetTodayScreen();
        PostMessage(HWND_BROADCAST, WM_WININICHANGE, 0xF2, 0);
    }*/

    // Launching contacts app
    //SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);
    oStr->StringCopy(szPath, (TCHAR*)pszInstallDir);
    oStr->Concatenate(szPath, _T("\\panocontacts.exe"));
    CreateProcess(szPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
    // make sure to put it in the auto start directory
    SetAutoLaunch(szPath, NAME_PhoneContacts);

    // Set the PhoneProfile to normal
    CObjPhoneProfile objPhone;
    objPhone.SetPhoneProfile(PP_Normal);

    // make sure to back up the ringtone first, and then blank it out cause our phone server will be taking care of it all
    CObjRingtoneEngine objRing;
    objRing.SetRingTone0ToNone();

    // Launching Phone server
    //SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);
    oStr->StringCopy(szPath, (TCHAR*)pszInstallDir);
    oStr->Concatenate(szPath, _T("\\PanoPhoneServer.exe"));
    CreateProcess(szPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);

    // make sure to put it in the auto start directory
    SetAutoLaunch(szPath, NAME_PhoneServer);

    // make it so the dialer will load properly
    EnumWindows((WNDENUMPROC)EnumWindowsProc, 0);    // kill cprog
    SetDialerDll();

    // restart dialer hidden
    PROCESS_INFORMATION pi = {0};
    CreateProcess(_T("\\Windows\\cprog.exe"), _T("-n"), NULL, NULL, NULL, 0, NULL, NULL, NULL, &pi);

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

    // Set the PhoneProfile to normal
    CObjPhoneProfile objPhone;
    objPhone.SetPhoneProfile(PP_Normal);

    // make sure to set the ringtone back to the default before it was installed
    CObjRingtoneEngine objRing;
    objRing.SetRingTone0ToDefault();
    objRing.DestroyIssRingTone();

	// remove the startup link file
	RemoveAutoLaunch(NAME_PhoneServer);
    RemoveAutoLaunch(NAME_PhoneContacts);

    oStr->StringCopy(szPath, (TCHAR*)pszInstallDir);
    oStr->Concatenate(szPath, _T("\\"));

    // restore to the originals if they were set
    CObjAssociations obj(szPath);
    obj.RestoreContactOverride();
    obj.RestoreTodaySoftkey();

    // make sure the dialer is unloaded
    EnumWindows((WNDENUMPROC)EnumWindowsProc, 0);    // kill cprog
    ResetDialerDll();

    // restart dialer hidden
    PROCESS_INFORMATION pi = {0};
    CreateProcess(_T("\\Windows\\cprog.exe"), _T("-n"), NULL, NULL, NULL, 0, NULL, NULL, NULL, &pi);


    RegDeleteKeyNT(HKEY_LOCAL_MACHINE, REG_TodayFull);

    //Tell the plug-in to uninstall itself.
    ::SendMessage(HWND_BROADCAST, WM_WININICHANGE, 0xF2, 0);

    Sleep(2000);



    // we should delete the key directories
    TCHAR szMsg[STRING_MAX];
    oStr->StringCopy(szMsg, IDS_MSG_DeleteSettings, STRING_MAX, g_hInst);
    if(IDYES == MessageBox(hwndParent, szMsg, _T("Uninstall"), MB_YESNO))
    {
        RegDeleteKeyNT(HKEY_CURRENT_USER, REG_PanoPhoneGenius);
        DeleteFile(TXT_FileBlocklist);
        DeleteFile(TXT_FileCallOptionlist);
        DeleteFile(TXT_FileRingtonRulelist);
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

void SetTodayScreen()
{
    // get all items in registry
    HKEY hKey;

    CIssString* oStr = CIssString::Instance();

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,REG_Today,0,KEY_QUERY_VALUE,&hKey) != ERROR_SUCCESS) 
        return;

    DWORD dwIndex	= 0;
    DWORD dwStringSize = 0;
    TCHAR szItemName[STRING_MAX];

    while(TRUE)
    {
        dwStringSize = STRING_MAX*sizeof(TCHAR);
        if(ERROR_SUCCESS != RegEnumKeyEx(hKey, dwIndex++, szItemName, &dwStringSize, NULL, NULL, NULL, NULL))
            break;

        if(oStr->Compare(szItemName, REG_Calendar) == 0)
        {
            SetTodayKeyItem(szItemName, REG_KeyEnabled, 1);
            SetTodayKeyItem(szItemName, REG_KeyOrder, 1);
        }
        else if(oStr->Compare(szItemName, REG_Tasks) == 0)
        {
            SetTodayKeyItem(szItemName, REG_KeyEnabled, 1);
            SetTodayKeyItem(szItemName, REG_KeyOrder, 2);
        }
        else if(oStr->Compare(szItemName, REG_TodayPhoneGenius) == 0)
        {
            SetTodayKeyItem(szItemName, REG_KeyEnabled, 1);
            SetTodayKeyItem(szItemName, REG_KeyOrder, 0);
        }
        else
        {
            SetTodayKeyItem(szItemName, REG_KeyEnabled, 0);
            SetTodayKeyItem(szItemName, REG_KeyOrder, 3);
        }        
    }

    RegCloseKey(hKey);
}

void SetTodayKeyItem(TCHAR* szKeyName, TCHAR* szValue, DWORD dwValue)
{
    HKEY hKey;

    CIssString* oStr = CIssString::Instance();
    TCHAR szKey[STRING_MAX];
    oStr->StringCopy(szKey, REG_Today);
    oStr->Concatenate(szKey, _T("\\"));
    oStr->Concatenate(szKey, szKeyName);

    if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey ,0,0, &hKey))
    {
        RegSetValueEx(hKey, szValue, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
        RegFlushKey(hKey);
        RegCloseKey(hKey);
    }
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

// go through and find the CProg process to kill it
BOOL EnumWindowsProc(HWND hWnd, DWORD lParam)
{
    DWORD pid;
    BOOL bRet = TRUE;

    // get the processid for this window 
    if (!GetWindowThreadProcessId(hWnd, &pid))  
        return bRet;

    // let's get the full path of the process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if(hProcess)
    {
        CIssString* oStr = CIssString::Instance();
        TCHAR szFullName[MAX_PATH] = _T("");
        GetModuleFileName((HMODULE)hProcess, szFullName, MAX_PATH);

        // we're looking for the cprog process so we can kill it
        if(-1 != oStr->FindNoCase(szFullName, _T("cprog.exe")))
        {
            TerminateProcess(hProcess, 0);
            bRet = FALSE;
        }
        CloseHandle(hProcess);
    }   

    return bRet; // keep going
}

void SetDialerDll()
{
    HKEY hKey = GetKey(HKEY_LOCAL_MACHINE, KEY_Skin, TRUE);
    if(!hKey)
        return;

    BackupValue(hKey, _T("ext"), REG_BackupOEM);
  
    RegFlushKey(hKey);
    RegCloseKey(hKey);

    RegistrySetString(HKEY_LOCAL_MACHINE, KEY_Skin, _T("ext"), _T("panodialer.dll"));
    RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_Skin, _T("Enabled"), 1);
}

void ResetDialerDll()
{
    HKEY hKey = GetKey(HKEY_LOCAL_MACHINE, KEY_Skin, FALSE);
    if(!hKey)
        return;

    RestoreValue(hKey, REG_BackupOEM, _T("ext"));

    RegFlushKey(hKey);
    RegCloseKey(hKey);
}

HKEY GetKey(HKEY hKeyRoot, TCHAR* szKey, BOOL bCreate)
{
    CIssString* oStr = CIssString::Instance();
    if(oStr->IsEmpty(szKey))
        return NULL;

    HKEY hKey = NULL;
    if(ERROR_SUCCESS != RegOpenKeyEx(hKeyRoot, szKey, NULL, NULL, &hKey))
    {
        if(bCreate)
        {
            DWORD dwDisposition;
            if(ERROR_SUCCESS != RegCreateKeyEx(hKeyRoot, szKey, NULL, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, &dwDisposition))
                return NULL;
        }
        else
            return NULL;
    }

    return hKey;
}

BOOL BackupValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination)
{
    CIssString* oStr = CIssString::Instance();
    if(!hKey || oStr->IsEmpty(szDestination))
        return FALSE;

    // first we check if there is already a backup because we don't want to do it twice
    DWORD dwType	= REG_SZ;
    DWORD dwDataSize= MAX_PATH*sizeof(TCHAR);
    TCHAR szTemp[MAX_PATH];
    ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
    if(ERROR_SUCCESS == RegQueryValueEx(hKey, szDestination, NULL, &dwType, (LPBYTE)szTemp, &dwDataSize))
        return TRUE;

    // read in the data
    dwType	= REG_SZ;
    dwDataSize= MAX_PATH*sizeof(TCHAR);
    ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, szSource, NULL, &dwType, (LPBYTE)szTemp, &dwDataSize) || dwDataSize == 0)
        return FALSE;

    // set the backup data
    RegSetValueEx(hKey, szDestination, NULL, dwType, (LPBYTE)szTemp, dwDataSize);
    RegFlushKey(hKey);

    return TRUE;
}

BOOL RestoreValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination)
{
    CIssString* oStr = CIssString::Instance();
    // read in the backed up value
    DWORD dwType	= REG_SZ;
    DWORD dwDataSize= MAX_PATH*sizeof(TCHAR);
    TCHAR szTemp[MAX_PATH];
    ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, szSource, NULL, &dwType, (LPBYTE)szTemp, &dwDataSize))
    {
        RegDeleteValue(hKey, szDestination);
        RegFlushKey(hKey);
        return TRUE;
    }

    // restore to original location
    RegSetValueEx(hKey, szDestination, NULL, dwType, (LPBYTE)szTemp, dwDataSize);

    // delete the backup copy
    RegDeleteValue(hKey, szSource);

    // flush it out
    RegFlushKey(hKey);

    return TRUE;
}
