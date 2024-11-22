#include "ObjAssociations.h"
#include "stdafx.h"
#include "IssRegistry.h"

#define KEY_CalcOverride _T("Software\\Microsoft\\Shell\\Rai\\:MSCALC")
#define KEY_CalcOverride2 _T("Software\\Microsoft\\Shell\\Rai\\:MSCALC_TPC")
#define LOC_CalcPro      _T("Calcpro.exe")
#define BACKUP_Name         _T("PanoName")
#define BACKUP_Location     _T("PanoLocation")

CObjAssociations::CObjAssociations(void)
:m_oStr(CIssString::Instance())
{

}

CObjAssociations::~CObjAssociations(void)
{

}

void CObjAssociations::SetCalcOverride(TCHAR* szInstallDir)
{
    if(m_oStr->IsEmpty(szInstallDir))
        return;

    HKEY hKey = GetKey(HKEY_LOCAL_MACHINE, KEY_CalcOverride);
    if(!hKey)
        return;

    BackupValue(hKey, _T("1"), BACKUP_Location);

    TCHAR szLocation[STRING_MAX];
    m_oStr->StringCopy(szLocation, szInstallDir);
    if(szLocation[m_oStr->GetLength(szLocation)-1] != _T('\\'))
        m_oStr->Concatenate(szLocation, _T("\\"));
    m_oStr->Concatenate(szLocation, LOC_CalcPro);

    // put quotes around the path
    m_oStr->Insert(szLocation, _T("\""));
    m_oStr->Concatenate(szLocation, _T("\"")); 

    SetValue(hKey, _T("1"), szLocation);

    RegCloseKey(hKey);

    hKey = GetKey(HKEY_LOCAL_MACHINE, KEY_CalcOverride2);
    if(!hKey)
        return;

    BackupValue(hKey, _T("1"), BACKUP_Location);

    SetValue(hKey, _T("1"), szLocation);

    RegCloseKey(hKey);
}

void CObjAssociations::RestoreCalcOverride()
{
    // it must be set to restore it
    if(!IsCalcOverrideSet())
        return;

    HKEY hKey = GetKey(HKEY_LOCAL_MACHINE, KEY_CalcOverride);
    if(!hKey)
        return;

    RestoreValue(hKey, BACKUP_Location, _T("1"));

    RegCloseKey(hKey);

    hKey = GetKey(HKEY_LOCAL_MACHINE, KEY_CalcOverride2);
    if(!hKey)
        return;

    RestoreValue(hKey, BACKUP_Location, _T("1"));

    RegCloseKey(hKey);
}

BOOL CObjAssociations::IsCalcOverrideSet()
{
    BOOL bRet = FALSE;

    HKEY hKey = GetKey(HKEY_LOCAL_MACHINE, KEY_CalcOverride);
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
    if(-1 != m_oStr->Find(szTemp, LOC_CalcPro))
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