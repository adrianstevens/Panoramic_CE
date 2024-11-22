#define EDB
#include "stdafx.h"
#include <windbase.h>
#include "ObjVoicemail.h"
#include "IssDebug.h"
#include "IssRegistry.h"
#include "CommonGuiDefines.h"
#include "simmgr.h"
#include "snapi.h"

#define PIMVOL              _T("\\pim.vol") 
#define SPEEDIAL            _T("speed.db") 
#define CONTACTSDATABASE    _T("Contacts Database")
#define REG_Vmail           _T("Vmail")
#define EF_MBDN_2G          0x6F17
#define EF_MBDN_3G          0x6FC7

CObjVoicemail::CObjVoicemail(void)
:m_oStr(CIssString::Instance())
{
}

CObjVoicemail::~CObjVoicemail(void)
{
}

HRESULT CObjVoicemail::GetVmailNumber(TCHAR* szNumber)
{
    HRESULT hr = S_OK;
    CPHR(szNumber, _T("szNumber"));

    m_oStr->Empty(szNumber);

    // first we always check the SIM to see if we can get the latest from there
    hr = GetSIMVmailNumber(szNumber);
    if(hr == S_OK && m_oStr->GetLength(szNumber) > 0)
        goto Error;

    // check if we have a saved value
    hr = GetKey(REG_KEY_ISS_PATH, REG_Vmail, szNumber, STRING_MAX);
    if(hr == S_OK && m_oStr->GetLength(szNumber) > 0)
        goto Error;

    hr = GetKey(_T("SOFTWARE\\Microsoft\\Vmail"), _T("PhoneNumber1"), szNumber, STRING_MAX);
    if(hr == S_OK && m_oStr->GetLength(szNumber) > 0)
        goto Error;

    hr = GetKey(_T("SOFTWARE\\Microsoft\\Vmail"), _T("PhoneNumber2"), szNumber, STRING_MAX);
    if(hr == S_OK && m_oStr->GetLength(szNumber) > 0)
        goto Error;

    hr = GetKey(_T("SOFTWARE\\Microsoft\\Vmail"), _T("UserProvidedNumber1"), szNumber, STRING_MAX);
    if(hr == S_OK && m_oStr->GetLength(szNumber) > 0)
        goto Error;

    hr = GetKey(_T("SOFTWARE\\Microsoft\\Vmail"), _T("CarrierProvidedNumber1"), szNumber, STRING_MAX);
    if(hr == S_OK && m_oStr->GetLength(szNumber) > 0)
        goto Error;

    hr = GetKey(_T("System\\State\\Messages\\vmail"), _T("VMailNumber"), szNumber, STRING_MAX);
    if(hr == S_OK && m_oStr->GetLength(szNumber) > 0)
        goto Error;

    hr = GetKey(_T("Palm\\State\\Messages\\vmail"), _T("VMailNumber"), szNumber, STRING_MAX);
    if(hr == S_OK && m_oStr->GetLength(szNumber) > 0)
        goto Error;

    // resort back to the owner phone number (works for all CDMA phones and perhaps some GSM phones)
    hr = GetKey(SN_OWNERPHONENUMBER_PATH, SN_OWNERPHONENUMBER_VALUE, szNumber, STRING_MAX);
    if(hr == S_OK && m_oStr->GetLength(szNumber) > 0)
        goto Error;

Error:
    // if we found something save it to the registry so we don't have to look again
    if(hr == S_OK && m_oStr->GetLength(szNumber) > 0)
        SetKey(REG_KEY_ISS_PATH, REG_Vmail, szNumber, m_oStr->GetLength(szNumber));

    return hr;
}

void CObjVoicemail::SetVmailNumber(TCHAR* szNumber)
{
    if(m_oStr->GetLength(szNumber) > 0)
        SetKey(REG_KEY_ISS_PATH, REG_Vmail, szNumber, m_oStr->GetLength(szNumber));
}

HRESULT CObjVoicemail::GetSIMVmailNumber(TCHAR* szNumber)
{
    HSIM hSim = NULL;
    LPBYTE buf = NULL;
    HRESULT hr = S_OK;
    SIMRECORDINFO simRecordInfo = {0};    
    simRecordInfo.cbSize = sizeof(SIMRECORDINFO);

    CBARG(szNumber, _T(""));

    m_oStr->Empty(szNumber);
    
    // initialize
    hr = SimInitialize (0, 0, 0, (LPHSIM)&hSim);
    CHR(hr, _T("SimInitialize"));

    DWORD address = EF_MBDN_2G;            
    
    // check the 2G address for the file    
    hr = SimGetRecordInfo(hSim, address, &simRecordInfo);    
    // The file may hold more than one number for devices with    
    // multiple lines; however we will assume only one line.    
    // A valid file needs to be "linear" type and at least 14 bytes.    
    if ( S_OK != hr || SIM_RECORDTYPE_LINEAR != simRecordInfo.dwRecordType || simRecordInfo.dwItemCount == 0 || simRecordInfo.dwSize < 14 )    
    {        
        // no valid 2G file, check the 3G address
        address = EF_MBDN_3G;        
        memset(&simRecordInfo, 0, sizeof(simRecordInfo));        
        simRecordInfo.cbSize = sizeof(SIMRECORDINFO);        
        hr = SimGetRecordInfo(hSim, address, &simRecordInfo);    
    }
    CHR(hr, _T("SimGetRecordInfo"));
    CBARG(SIM_RECORDTYPE_LINEAR == simRecordInfo.dwRecordType && simRecordInfo.dwItemCount > 0 && simRecordInfo.dwSize > 14, _T(""));

    buf = (LPBYTE)LocalAlloc(LPTR, simRecordInfo.dwSize); 
    CPHR(buf, _T("LocalAlloc"));
    DWORD bytesRead = 0;                
    
    // read the SIM file        
    hr = SimReadRecord(hSim, address, SIM_RECORDTYPE_LINEAR, 1, buf, simRecordInfo.dwSize, &bytesRead);
    CBARG(hr == S_OK && bytesRead >= 14, _T(""));

    // use the last 14 bytes        
    char data[14];        
    memcpy(&data, &buf[bytesRead-14], 14);        
    
    // byte 0 (X+1) is the length of dialing number        
    int length = data[0];       
    
    // TON and NPI determines the type of number        
    // see 3GPP specifications 31.102 clause 4.4.2.3        
    if (0x90 == (0xF0 & data[1]))        
    {            
        // add the + for international numbers            
        m_oStr->Concatenate(szNumber, '+');        
    }        
    
    // convert the BCD bytes to Ascii chars        
    for (int idx=2; idx < length+1; idx++)        
    {            
        // break the byte into 2 nibbles            
        BYTE nibble[2];            
        nibble[0] = data[idx] & 0xF;            
        nibble[1] = (data[idx] >> 4) & 0xF;            
        for (int i=0; i<2; i++)            
        {                
            if (nibble[i] == 0xA)                
            {                    
                // 0xA represents a *                    
                m_oStr->Concatenate(szNumber, '*');                
            }                
            else if (nibble[i] == 0xB)                
            {                    
                // 0xB represents a #                    
                m_oStr->Concatenate(szNumber, '#');                
            }                
            else if (nibble[i] < 0xA)                
            {                    
                // add the number                    
                m_oStr->Concatenate(szNumber, (char) ('0' + nibble[i]));                
            }            
        }        
    }

Error:
    if(hSim)
        SimDeinitialize(hSim);

    if(buf)
        LocalFree(buf);

    return hr;
}

void CObjVoicemail::PopupVoiceMailNotification()
{
    SHNOTIFICATIONDATA shnd;
    CLSID clsid;
    LRESULT result;
    DWORD dwID = 0;

    if (0 == CLSIDFromString(_T("{A877D661-239C-47a7-9304-0D347F580408}"), &clsid)) 
    {
        memset(&shnd, 0, sizeof(shnd));
        shnd.cbStruct = sizeof(SHNOTIFICATIONDATA);

        do 
        {
            result = SHNotificationGetData(&clsid,dwID,&shnd);
            if (ERROR_SUCCESS == result) 
            {
                SHNOTIFICATIONDATA sn2 = {0};

                //Put the data from an existing notification into a second SHNOTIFICATIONDATA struct
                sn2.cbStruct = sizeof(sn2);
                result = SHNotificationGetData(&clsid, 1, &sn2);
                SHNotificationUpdate(SHNUM_TITLE | SHNUM_HTML | SHNUM_ICON | SHNUM_SOFTKEYCMDS | SHNUM_SOFTKEYS, &sn2);
                if (shnd.pszHTML) 
                    free((void *) shnd.pszHTML);
                shnd.pszHTML = NULL;

                if (shnd.pszTitle) 
                    free((void *) shnd.pszTitle);
                shnd.pszTitle = NULL;
            } 
            else 
                dwID++;
        } while ((ERROR_SUCCESS != result) && (dwID < 2000));

    }
}

HRESULT CObjVoicemail::ReadSpeedDialDB(TCHAR* szVoiceMail)
{
    HRESULT hr = S_OK;
    BOOL bRet;

    CPHR(szVoiceMail, _T("szVoiceMail"));
    m_oStr->Empty(szVoiceMail);

    CEGUID          ceGuidDB;
    CEOID           ClogSeekID;
    WORD            wPropID = 6;
    LPBYTE          lpData = NULL;
    DWORD           dwDataSize = 0;
    HANDLE          hDB = INVALID_HANDLE_VALUE;

    CREATE_INVALIDGUID(&ceGuidDB);

    int iNumRecords = 0;

    // open the DB and find out how many records there are
    hr = OpenSpeedDialDB(ceGuidDB, hDB, iNumRecords);
    CHR(hr, _T("OpenPhoneCallLogDB"));

    CBARG(iNumRecords != 0, _T(""));

    ClogSeekID=CeSeekDatabaseEx(hDB,CEDB_SEEK_BEGINNING,0,0,0);

    // go through and try and find the entry
    bRet = FALSE;
    bRet = CeReadRecordPropsEx(hDB, CEDB_ALLOWREALLOC, &wPropID,NULL,&lpData,&dwDataSize,NULL);   
    CBHR(bRet, _T("CeReadRecordPropsEx"));

    PCEPROPVAL ppv = (PCEPROPVAL)lpData;
    int iKeyVal, icLoc;
    TCHAR* szVal;
    TCHAR* szName;

    // Record properties:
    // ID: 0x00040003 - information location
    //     1 = Contacts Database, 4 = blob data, 5 = Voicemail?
    // ID: 0x00060041 - blob data
    //     if 0x00040003 = 1
    //      contacts OID in (val.blob.lpb)[3]
    //      attribute in (val.blob.lpb)[4]
    //     if 0x00040003 = 4
    //      number stored in val.blob.lpb
    // ID: 0x0007001f - contact name stored in val.lpwstr

    for(int i=0; i<wPropID; i++)
    {
        switch (ppv[ i ].propid)
        {
        case 0x00040003:
            icLoc = ppv[ i ].val.lVal;
            break;
        case 0x00060041:
            iKeyVal = i;
            break;
        case 0x0007001f:
            szName = ppv[ i ].val.lpwstr;
            break;
        }
    }

    switch (icLoc)
    {
    case 1:
        // Read the value from the Contacts Database.
        /*ContactsDatabase(guid,
            ((DWORD *)ppv[iKeyVal].val.blob.lpb)[3],
            ((DWORD *)ppv[iKeyVal].val.blob.lpb)[4],
            value);*/
        break;

    case 4:
        // Assume the first WORD is the starting location of the value.
        szVal = (WCHAR *)(((char *)ppv[iKeyVal].val.blob.lpb) + ((DWORD *)ppv[iKeyVal].val.blob.lpb)[0]);
        //wcscpy(*value = new WCHAR[wcslen(cvalue) + 1], cvalue);
        break;

    case 5:
        // Assume it is the Voicemail entry and read it from the following registry
        // \HKCU\Software\Microsoft\Vmail\PhoneNumber1
        /*if (key == 1)
        {
            HKEY hkey;

            if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Vmail"), 0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS)
            {
                DWORD buflen = 64;
                WCHAR buf[64];

                if (RegQueryValueEx(hkey, TEXT("PhoneNumber1"), NULL, NULL, (LPBYTE)buf, &buflen) == ERROR_SUCCESS)
                {
                    wcscpy(*value = new WCHAR[wcslen(buf) + 1], buf);
                }
                RegCloseKey(hkey);
            }
        }*/
        break;
    }






Error:
    if(lpData)
        LocalFree((HLOCAL)lpData);

    if(hDB != INVALID_HANDLE_VALUE)
        CloseHandle(hDB);

    if(!CHECK_INVALIDGUID(&ceGuidDB))
    {
        CeUnmountDBVol(&ceGuidDB);
    }

    return hr;
}

HRESULT CObjVoicemail::OpenSpeedDialDB(CEGUID& ceGuidDB, HANDLE& hDB, int& iNumRecords)
{
    HRESULT hr = S_OK;
    BOOL    bRet;
    CEOID   oidCalllog = 0;
    CEOIDINFOEX     ceObject;

    bRet = CeMountDBVolEx(&ceGuidDB, PIMVOL, 0, OPEN_EXISTING );
    CBHR(bRet, _T("CeMountDBVolEx"));

    hDB = CeOpenDatabaseInSession(NULL, &ceGuidDB, &oidCalllog, SPEEDIAL, NULL, CEDB_AUTOINCREMENT, NULL); 
    CBARG(hDB != INVALID_HANDLE_VALUE, _T("CeOpenDatabaseInSession"));

    ceObject.wVersion = CEOIDINFOEX_VERSION ; 
    bRet = CeOidGetInfoEx2(&ceGuidDB,oidCalllog, &ceObject);
    CBHR(bRet, _T("CeOidGetInfoEx2"));

    iNumRecords = ceObject.infDatabase.dwNumRecords;

Error:
    return hr;
}

void CObjVoicemail::ContactsDatabase(CEGUID guid, DWORD coid, DWORD catt, LPWSTR *value)
{
    CEOID oid = 0;
    HANDLE db = CeOpenDatabaseInSession(NULL, &guid, &oid, CONTACTSDATABASE, NULL, 0, NULL);

    if (db != INVALID_HANDLE_VALUE)
    {
        // Locate the reocrd with the given OID. The return value will be non-zero if successfully
        // located.
        if ((oid = CeSeekDatabaseEx(db, CEDB_SEEK_CEOID, (DWORD)coid, 1, NULL)) != 0)
        {
            // ID: 0x0080001f - contact name stored in val.lpwstr.
            // ID: attribute  - information stored in val.lpwstr.
            WORD numprop;
            LPBYTE buffer = NULL;
            DWORD blen;

            if ((oid = CeReadRecordPropsEx(db, CEDB_ALLOWREALLOC, &numprop, NULL, &buffer, &blen, NULL)) != 0)
            {
                PCEPROPVAL ppv = (PCEPROPVAL)buffer;

                for (int i = 0; i < numprop; i++)
                {
                    if (ppv[ i ].propid == catt)
                    {
                        wcscpy(*value = new WCHAR[wcslen(ppv[ i ].val.lpwstr) + 1], ppv[ i ].val.lpwstr);
                        break;
                    }
                }
            }
            LocalFree(buffer);
        }
        CloseHandle(db);
    }
}


