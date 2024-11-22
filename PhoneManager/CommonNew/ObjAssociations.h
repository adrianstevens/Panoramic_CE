#pragma once

#include "IssString.h"
#include "windows.h"

class CObjAssociations
{
public:
    CObjAssociations(TCHAR* szInstallDir);
    ~CObjAssociations(void);

    void    SetTodaySoftkey();
    void    SetTodaySoftkeyL();
    void    RestoreTodaySoftkey();
    void    RestoreTodaySoftkeyL();
    void    SetContactOverride();
    void    RestoreContactOverride();
    BOOL    IsTodaySoftkeySet();
    BOOL    IsTodaySoftkeySetL();
    BOOL    IsContactOverrideSet();

private:    // functions
    HKEY    GetKey(HKEY hKeyRoot, TCHAR* szKey, BOOL bCreate = FALSE);
    BOOL    RestoreValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination);
    BOOL    BackupValue(HKEY hKey, TCHAR* szSource, TCHAR* szDestination);
    BOOL    SetValue(HKEY hKey, TCHAR* szName, TCHAR* szValue);
    BOOL    GetValue(HKEY hKey, TCHAR* szName, TCHAR* szValue);


private:    // variables
    CIssString* m_oStr;
    TCHAR       m_szContactLoc[STRING_MAX];
    TCHAR       m_szLauncherLoc[STRING_MAX];
};
