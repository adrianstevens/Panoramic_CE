#include "ObjAssociations.h"
#include "stdafx.h"
#include "IssRegistry.h"
#include "IssCommon.h"

#define KEY_TodaySoftkeyL   _T("Software\\Microsoft\\Today\\Keys\\112")
#define KEY_TodaySoftkey    _T("Software\\Microsoft\\Today\\Keys\\113")
#define KEY_ContactOverride _T("Software\\Microsoft\\Shell\\Rai\\:MSCONTACTS")
#define LOC_Contacts        _T("\"%spanocontacts.exe\"")
#define LOC_Favorites        _T("\"%spanolauncher.exe\"")
#define BACKUP_Name         _T("PanoName")
#define BACKUP_Location     _T("PanoLocation")
#define HM_RELOAD_BUTTON    (WM_APP+67)

CObjAssociations::CObjAssociations(TCHAR* szInstallDir)
:m_oStr(CIssString::Instance())
{
    //TCHAR szInstallDir[STRING_MAX] = _T("");
    //GetExeDirectory(szInstallDir);
    m_oStr->Format(m_szContactLoc, LOC_Contacts, szInstallDir);
    m_oStr->Format(m_szLauncherLoc, LOC_Favorites, szInstallDir);
}

CObjAssociations::~CObjAssociations(void)
{

}

void CObjAssociations::SetTodaySoftkey()
{
    HKEY hKey = GetKey(HKEY_CURRENT_USER, KEY_TodaySoftkey, TRUE);
    if(!hKey)
        return;

    BackupValue(hKey, NULL, BACKUP_Name);
    BackupValue(hKey, _T("Open"), BACKUP_Location);

    SetValue(hKey, NULL, _T("Favorites"));

    TCHAR szLocation[STRING_MAX];
    m_oStr->StringCopy(szLocation, m_szContactLoc);
    //m_oStr->Concatenate(szLocation, _T(" -f"));    
    SetValue(hKey, _T("Open"), szLocation);

    RegCloseKey(hKey);

    HWND hwndHome = GetDesktopWindow(); 
    PostMessage(hwndHome, HM_RELOAD_BUTTON, VK_TSOFT2, 0); 
}

void CObjAssociations::SetTodaySoftkeyL()
{
    HKEY hKey = GetKey(HKEY_CURRENT_USER, KEY_TodaySoftkeyL, TRUE);
    if(!hKey)
        return;

    BackupValue(hKey, NULL, BACKUP_Name);
    BackupValue(hKey, _T("Open"), BACKUP_Location);

    SetValue(hKey, NULL, _T("Launchpad"));

    TCHAR szLocation[STRING_MAX];
    m_oStr->StringCopy(szLocation, m_szLauncherLoc);
    //m_oStr->Concatenate(szLocation, _T(" -f"));    
    SetValue(hKey, _T("Open"), szLocation);

    RegCloseKey(hKey);

    HWND hwndHome = GetDesktopWindow(); 
    PostMessage(hwndHome, HM_RELOAD_BUTTON, VK_TSOFT1, 0); 
}

void CObjAssociations::RestoreTodaySoftkey()
{
    // it must be set to restore it
    if(!IsTodaySoftkeySet())
        return;

    HKEY hKey = GetKey(HKEY_CURRENT_USER, KEY_TodaySoftkey);
    if(!hKey)
        return;

    TCHAR szTemp[STRING_MAX];
    if(!GetValue(hKey, BACKUP_Name, szTemp) || !GetValue(hKey, BACKUP_Location, szTemp))
    {
        // Just set it to the contacts one for now, which is default
        SetValue(hKey, NULL, _T("Contacts"));
        SetValue(hKey, _T("Open"), _T(":MSCONTACTS"));
        RegCloseKey(hKey);
    }
    else
    {
        RestoreValue(hKey, BACKUP_Name, NULL);
        RestoreValue(hKey, BACKUP_Location, _T("Open"));
        RegCloseKey(hKey);
    }    

    HWND hwndHome = GetDesktopWindow(); 
    PostMessage(hwndHome, HM_RELOAD_BUTTON, VK_TSOFT2, 0); 
}

void CObjAssociations::RestoreTodaySoftkeyL()
{
    // it must be set to restore it
    if(!IsTodaySoftkeySetL())
        return;

    HKEY hKey = GetKey(HKEY_CURRENT_USER, KEY_TodaySoftkeyL);
    if(!hKey)
        return;

    TCHAR szTemp[STRING_MAX];
    if(!GetValue(hKey, BACKUP_Name, szTemp) || !GetValue(hKey, BACKUP_Location, szTemp))
    {
        RegDeleteValue(hKey, NULL);
        RegDeleteValue(hKey, _T("Open"));
        // flush it out
        RegFlushKey(hKey);
        RegCloseKey(hKey);
    }
    else
    {
        RestoreValue(hKey, BACKUP_Name, NULL);
        RestoreValue(hKey, BACKUP_Location, _T("Open"));
        RegCloseKey(hKey);
    }    

    HWND hwndHome = GetDesktopWindow(); 
    PostMessage(hwndHome, HM_RELOAD_BUTTON, VK_TSOFT1, 0); 
}

void CObjAssociations::SetContactOverride()
{
    HKEY hKey = GetKey(HKEY_LOCAL_MACHINE, KEY_ContactOverride);
    if(!hKey)
        return;

    BackupValue(hKey, _T("1"), BACKUP_Location);

    TCHAR szLocation[STRING_MAX];
    m_oStr->StringCopy(szLocation, m_szContactLoc);
    m_oStr->Concatenate(szLocation, _T(" -c"));    
    SetValue(hKey, _T("1"), szLocation);

    RegCloseKey(hKey);
}

void CObjAssociations::RestoreContactOverride()
{
    // it must be set to restore it
    if(!IsContactOverrideSet())
        return;

    HKEY hKey = GetKey(HKEY_LOCAL_MACHINE, KEY_ContactOverride);
    if(!hKey)
        return;

    RestoreValue(hKey, BACKUP_Location, _T("1"));

    RegCloseKey(hKey);
}

BOOL CObjAssociations::IsTodaySoftkeySet()
{
    BOOL bRet = FALSE;

    HKEY hKey = GetKey(HKEY_CURRENT_USER, KEY_TodaySoftkey);
    if(!hKey)
        return FALSE;

    DWORD dwType	= REG_SZ;
    DWORD dwDataSize= MAX_PATH*sizeof(TCHAR);
    TCHAR szTemp[MAX_PATH];
    ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, _T("Open"), NULL, &dwType, (LPBYTE)szTemp, &dwDataSize))
    {
        bRet = FALSE;
        goto End;
    }

    // see if we find the pano contacts link
    if(-1 != m_oStr->Find(szTemp, _T("panocontacts.exe")))
        bRet = TRUE;

End:
    if(hKey)
        RegCloseKey(hKey);
    return bRet;
}

BOOL CObjAssociations::IsTodaySoftkeySetL()
{
    BOOL bRet = FALSE;

    HKEY hKey = GetKey(HKEY_CURRENT_USER, KEY_TodaySoftkeyL);
    if(!hKey)
        return FALSE;

    DWORD dwType	= REG_SZ;
    DWORD dwDataSize= MAX_PATH*sizeof(TCHAR);
    TCHAR szTemp[MAX_PATH];
    ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, _T("Open"), NULL, &dwType, (LPBYTE)szTemp, &dwDataSize))
    {
        bRet = FALSE;
        goto End;
    }

    // see if we find the pano contacts link
    if(-1 != m_oStr->Find(szTemp, _T("panolauncher.exe")))
        bRet = TRUE;

End:
    if(hKey)
        RegCloseKey(hKey);
    return bRet;
}

BOOL CObjAssociations::IsContactOverrideSet()
{
    BOOL bRet = FALSE;

    HKEY hKey = GetKey(HKEY_LOCAL_MACHINE, KEY_ContactOverride);
    if(!hKey)
        return FALSE;

    DWORD dwType	= REG_SZ;
    DWORD dwDataSize= MAX_PATH*sizeof(TCHAR);
    TCHAR szTemp[MAX_PATH];
    ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, _T("1"), NULL, &dwType, (LPBYTE)szTemp, &dwDataSize))
    {
        bRet = FALSE;
        goto End;
    }

    // see if we find the pano contacts link
    if(-1 != m_oStr->Find(szTemp, _T("panocontacts.exe")))
        bRet = TRUE;

End:
    if(hKey)
        RegCloseKey(hKey);
    return bRet;
}

HKEY CObjAssociations::GetKey(HKEY hKeyRoot, TCHAR* szKey, BOOL bCreate)
{
    if(m_oStr->IsEmpty(szKey))
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

BOOL CObjAssociations::BackupValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination)
{
    if(!hKey || m_oStr->IsEmpty(szDestination))
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

BOOL CObjAssociations::RestoreValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination)
{
    // read in the backed up value
    DWORD dwType	= REG_SZ;
    DWORD dwDataSize= MAX_PATH*sizeof(TCHAR);
    TCHAR szTemp[MAX_PATH];
    ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, szSource, NULL, &dwType, (LPBYTE)szTemp, &dwDataSize))
    {
        // blank out the values and keep going
        m_oStr->Empty(szTemp);
    }

    // restore to original location
    RegSetValueEx(hKey, szDestination, NULL, dwType, (LPBYTE)szTemp, dwDataSize);

    // delete the backup copy
    RegDeleteValue(hKey, szSource);

    // flush it out
    RegFlushKey(hKey);

    return TRUE;
}

BOOL CObjAssociations::SetValue(HKEY hKey, TCHAR* szName, TCHAR* szValue)
{
    if(!hKey)
        return FALSE;

    DWORD dwType	= REG_SZ;
    DWORD dwDataSize= (m_oStr->GetLength(szValue)+1)*sizeof(TCHAR);
    if(ERROR_SUCCESS != RegSetValueEx(hKey, szName, NULL, dwType, (LPBYTE)szValue, dwDataSize))
        return FALSE;
    
    RegFlushKey(hKey);
    return TRUE;
}

BOOL CObjAssociations::GetValue(HKEY hKey, TCHAR* szName, TCHAR* szValue)
{
    if(!hKey || !szValue)
        return FALSE;

    DWORD dwType	= REG_SZ;
    DWORD dwDataSize= MAX_PATH*sizeof(TCHAR);
    TCHAR szTemp[MAX_PATH];
    ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szTemp, &dwDataSize))
        return FALSE;

    // blank out the values and keep going
    m_oStr->StringCopy(szValue, szTemp);
    return TRUE;
}