// CallLog.cpp: implementation of the CCallLog class.
//
//////////////////////////////////////////////////////////////////////


#define EDB
#include "stdafx.h"
#include <windbase.h>

#include "CallLog.h"
#include "IssDebug.h"



#define PIMVOL      _T("\\pim.vol") 
#define CLOG        _T("clog.db") 

#pragma comment( lib,"phone.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCallLog::CCallLog()
:m_hCallLog(NULL)
{

}

CCallLog::~CCallLog()
{
	Destroy();
}

BOOL CCallLog::Destroy()
{
	DestroyMissedCalls();
	DestroyIncomingCalls();
	DestroyOutgoingCalls();

	if(m_hCallLog)
	{
		PhoneCloseCallLog(m_hCallLog);
		m_hCallLog = NULL;
	}
	
	return TRUE;
}

BOOL CCallLog::DestroyMissedCalls()
{
	// kill off our vector array
	int* iIndex;
	for(int i = 0; i<m_arrMissed.GetSize(); i++)
	{
		iIndex = m_arrMissed[i];
		if(iIndex)
			delete iIndex;

	}

	m_arrMissed.RemoveAll();

	return TRUE;
}

BOOL CCallLog::DestroyIncomingCalls()
{
	// kill off our vector array
	int* iIndex;
	for(int i = 0; i<m_arrIncoming.GetSize(); i++)
	{
		iIndex = m_arrIncoming[i];
		if(iIndex)
			delete iIndex;

	}

	m_arrIncoming.RemoveAll();
	return TRUE;
}

BOOL CCallLog::DestroyOutgoingCalls()
{
	// kill off our vector array
	int* iIndex;
	for(int i = 0; i<m_arrOutgoing.GetSize(); i++)
	{
		iIndex = m_arrOutgoing[i];
		if(iIndex)
			delete iIndex;

	}

	m_arrOutgoing.RemoveAll();

	return TRUE;
}
	
BOOL CCallLog::Initialize()
{
	// clear off any arrays that were previously set
	Destroy();

	// get a handle to the call log
	if(S_OK != PhoneOpenCallLog(&m_hCallLog))
		return FALSE;

	CALLLOGENTRY sCallEntry;
	int iIndex = 0;
	int* iArrIndex;

	sCallEntry.cbSize = sizeof(CALLLOGENTRY);
	while(S_OK == PhoneGetCallLogEntry(m_hCallLog, &sCallEntry))
	{
		iArrIndex	= new int;
		*iArrIndex	= iIndex;

		// figure out which element we type of call it was
		switch(sCallEntry.iom)
		{
		case IOM_MISSED:
			m_arrMissed.AddElement(iArrIndex);
			break;
		case IOM_INCOMING:
			m_arrIncoming.AddElement(iArrIndex);
			break;
		case IOM_OUTGOING:
		default:
			m_arrOutgoing.AddElement(iArrIndex);
			break;
		}

		// prepare for the next entry
		iIndex++;
		sCallEntry.cbSize = sizeof(CALLLOGENTRY);
	}
	
	return TRUE;
}

int CCallLog::GetMissedCallsCount()
{
	if(NULL == m_hCallLog)
		return 0;

	return m_arrMissed.GetSize();
}

int	CCallLog::GetIncomingCallsCount()
{
	if(NULL == m_hCallLog)
		return 0;

	return m_arrIncoming.GetSize();
}

int	CCallLog::GetOutgoingCallsCount()
{
	if(NULL == m_hCallLog)
		return 0;

	return m_arrOutgoing.GetSize();
}

int CCallLog::GetAllCallsCount()
{
	if(NULL == m_hCallLog)
		return 0;

	DWORD dwCount = 0;
	PhoneSeekCallLog(m_hCallLog, CALLLOGSEEK_END, 0, &dwCount);
	//return dwCount;
	return dwCount + 1;	// DH: looks like dwCount is 0-based index of last entry
}

BOOL CCallLog::GetMissedCallsEntry(int iIndex, CALLLOGENTRY& sCallEntry)
{
	if(NULL == m_hCallLog)
		return FALSE;

	// go through our array to find the proper index in the full call log list
	int* iNewIndex = m_arrMissed[iIndex];
	if(NULL == iNewIndex)
		return FALSE;
	iIndex	= *iNewIndex;

	// try and find the record we are searching for
	DWORD dwRecord = 0;
	if(S_OK != PhoneSeekCallLog(m_hCallLog,CALLLOGSEEK_BEGINNING,  iIndex, &dwRecord))
		return FALSE;

	// get the values out of this entry
	sCallEntry.cbSize = sizeof(CALLLOGENTRY);

	if(S_OK != PhoneGetCallLogEntry(m_hCallLog, &sCallEntry))
		return FALSE;
	else
		return TRUE;
}

BOOL CCallLog::GetIncomingCallsEntry(int iIndex, CALLLOGENTRY& sCallEntry)
{
	if(NULL == m_hCallLog)
		return FALSE;

	// go through our array to find the proper index in the full call log list
	int* iNewIndex = m_arrIncoming[iIndex];
	if(NULL == iNewIndex)
		return FALSE;
	iIndex	= *iNewIndex;

	// try and find the record we are searching for
	DWORD dwRecord = 0;
	if(S_OK != PhoneSeekCallLog(m_hCallLog,CALLLOGSEEK_BEGINNING,  iIndex, &dwRecord))
		return FALSE;

	// get the values out of this entry
	sCallEntry.cbSize = sizeof(CALLLOGENTRY);
	if(S_OK != PhoneGetCallLogEntry(m_hCallLog, &sCallEntry))
		return FALSE;
	else
		return TRUE;
}

BOOL CCallLog::GetOutGoingCallsEntry(int iIndex, CALLLOGENTRY& sCallEntry)
{
	if(NULL == m_hCallLog)
		return FALSE;

	// go through our array to find the proper index in the full call log list
	int* iNewIndex = m_arrOutgoing[iIndex];
	if(NULL == iNewIndex)
		return FALSE;
	iIndex	= *iNewIndex;

	// try and find the record we are searching for
	DWORD dwRecord = 0;
	if(S_OK != PhoneSeekCallLog(m_hCallLog,CALLLOGSEEK_BEGINNING,  iIndex, &dwRecord))
		return FALSE;

	// get the values out of this entry
	sCallEntry.cbSize = sizeof(CALLLOGENTRY);
	if(S_OK != PhoneGetCallLogEntry(m_hCallLog, &sCallEntry))
		return FALSE;
	else
		return TRUE;
}

BOOL CCallLog::GetAllCallsEntry(int iIndex, CALLLOGENTRY& sCallEntry)
{
	if(NULL == m_hCallLog)
		return FALSE;

	// try and find the record we are searching for
	DWORD dwRecord = 0;
	if(S_OK != PhoneSeekCallLog(m_hCallLog,CALLLOGSEEK_BEGINNING,  iIndex, &dwRecord))
		return FALSE;

	// get the values out of this entry
	sCallEntry.cbSize = sizeof(CALLLOGENTRY);
	if(S_OK != PhoneGetCallLogEntry(m_hCallLog, &sCallEntry))
		return FALSE;
	else
		return TRUE;
}


HRESULT CCallLog::DeleteCallEntry(FILETIME& ftTime)
{
    HRESULT hr = S_OK;
    BOOL bRet;

    CEGUID          ceGuidDB;
    CEOID           ClogSeekID;
    WORD            wPropID = 6;
    LPBYTE          lpData = NULL;
    DWORD           dwDataSize = 0;
    HANDLE          hDB = INVALID_HANDLE_VALUE;
    CEPROPVAL*      ClogVal;
    
    CREATE_INVALIDGUID(&ceGuidDB);

    int iNumRecords = 0;

    // open the DB and find out how many records there are
    hr = OpenPhoneCallLogDB(ceGuidDB, hDB, iNumRecords);
    CHR(hr, _T("OpenPhoneCallLogDB"));

    ClogSeekID=CeSeekDatabaseEx(hDB,CEDB_SEEK_BEGINNING,0,0,0);

    // go through and try and find the entry
    bRet = FALSE;
    for(int i=0; i<iNumRecords; i++)
    {
        bRet = CeReadRecordPropsEx(hDB, CEDB_ALLOWREALLOC, &wPropID,NULL,&lpData,&dwDataSize,NULL);   
        if(!bRet)
            break;

        ClogVal = (PCEPROPVAL)lpData + 1;   // this gets us the filetime

        // check for matching file times
        if(ClogVal->val.filetime.dwHighDateTime == ftTime.dwHighDateTime &&
           ClogVal->val.filetime.dwLowDateTime == ftTime.dwLowDateTime)
        {
			LocalFree((HLOCAL)lpData);
			lpData = NULL;
			dwDataSize = 0;

            bRet = CeDeleteRecord(hDB,ClogSeekID);			
            break;
        }

		LocalFree((HLOCAL)lpData);
		lpData = NULL;
		dwDataSize = 0;

        // get the next one
        ClogSeekID=CeSeekDatabaseEx(hDB,CEDB_SEEK_CURRENT,0,0,0); 
    }

    CBHR(bRet, _T("Failed to find Call record"));

Error:
    if(hDB != INVALID_HANDLE_VALUE)
        CloseHandle(hDB);

    if(!CHECK_INVALIDGUID(&ceGuidDB))
    {
        CeFlushDBVol(&ceGuidDB);
        CeUnmountDBVol(&ceGuidDB);
    }

    return hr;
}

HRESULT CCallLog::DeleteAllCallEntry()
{
    HRESULT hr = S_OK;
    BOOL bRet;

    CEGUID          ceGuidDB;
    CEOID           ClogSeekID;
    HANDLE          hDB = INVALID_HANDLE_VALUE;

    CREATE_INVALIDGUID(&ceGuidDB);

    int iNumRecords = 0;

    // open the DB and find out how many records there are
    hr = OpenPhoneCallLogDB(ceGuidDB, hDB, iNumRecords);
    CHR(hr, _T("OpenPhoneCallLogDB"));

    ClogSeekID=CeSeekDatabaseEx(hDB,CEDB_SEEK_BEGINNING,0,0,0);

    // go through and try and find the entry
    for(int i=0; i<iNumRecords && ClogSeekID != NULL; i++)
    {
        bRet = CeDeleteRecord(hDB,ClogSeekID);

        // get the next one
        ClogSeekID=CeSeekDatabaseEx(hDB,CEDB_SEEK_CURRENT,0,0,0); 
    }

    CBHR(bRet, _T("Failed to delete all records"));

Error:
    if(hDB != INVALID_HANDLE_VALUE)
        CloseHandle(hDB);

    if(!CHECK_INVALIDGUID(&ceGuidDB))
    {
        CeFlushDBVol(&ceGuidDB);
        CeUnmountDBVol(&ceGuidDB);
    }

    return hr;
}

HRESULT CCallLog::OpenPhoneCallLogDB(CEGUID& ceGuidDB, HANDLE& hDB, int& iNumRecords)
{
    HRESULT hr = S_OK;
    BOOL    bRet;
    CEOID   oidCalllog = 0;
    CEOIDINFOEX     ceObject;

    bRet = CeMountDBVolEx(&ceGuidDB, PIMVOL, 0, OPEN_EXISTING );
    CBHR(bRet, _T("CeMountDBVolEx"));

    hDB = CeOpenDatabaseInSession(NULL, &ceGuidDB, &oidCalllog, CLOG, NULL, CEDB_AUTOINCREMENT, NULL); 
    CBARG(hDB != INVALID_HANDLE_VALUE, _T("CeOpenDatabaseInSession"));

    ceObject.wVersion = CEOIDINFOEX_VERSION ; 
    bRet = CeOidGetInfoEx2(&ceGuidDB,oidCalllog, &ceObject);
    CBHR(bRet, _T("CeOidGetInfoEx2"));

    iNumRecords = ceObject.infDatabase.dwNumRecords;

Error:
    return hr;
}
