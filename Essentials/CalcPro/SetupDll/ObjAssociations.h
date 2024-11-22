#pragma once

#include "IssString.h"
#include "windows.h"

class CObjAssociations
{
public:
    CObjAssociations(void);
    ~CObjAssociations(void);

    void    SetCalcOverride(TCHAR* szInstallDir);
    void    RestoreCalcOverride();
    BOOL    IsCalcOverrideSet();

private:    // functions
    HKEY    GetKey(HKEY hKeyRoot, TCHAR* szKey, BOOL bCreate = FALSE);
    BOOL    RestoreValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination);
    BOOL    BackupValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination);
    BOOL    SetValue(HKEY hKey, TCHAR* szName, TCHAR* szValue);
    BOOL    GetValue(HKEY hKey, TCHAR* szName, TCHAR* szValue);


private:    // variables
    CIssString* m_oStr;
};
