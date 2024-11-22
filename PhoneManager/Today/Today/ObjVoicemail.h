#pragma once

#include "IssString.h"

class CObjVoicemail
{
public:
    CObjVoicemail(void);
    ~CObjVoicemail(void);

    HRESULT GetVmailNumber(TCHAR* szNumber);  
    void    SetVmailNumber(TCHAR* szNumber);

private:    // functions
    HRESULT OpenSpeedDialDB(CEGUID& ceGuidDB, HANDLE& hDB, int& iNumRecords);
    void    ContactsDatabase(CEGUID guid, DWORD coid, DWORD catt, LPWSTR *value);
    HRESULT GetSIMVmailNumber(TCHAR* szNumber);
    HRESULT ReadSpeedDialDB(TCHAR* szVoiceMail);
    void    PopupVoiceMailNotification();


private:    // variables
    CIssString* m_oStr;
};
