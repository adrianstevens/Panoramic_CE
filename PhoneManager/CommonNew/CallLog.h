// CallLog.h: interface for the CCallLog class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "phone.h"
#include "IssVector.h"

class CCallLog  
{
public:
	CCallLog();
	virtual ~CCallLog();

	BOOL Destroy();
	BOOL Initialize();

	int  GetMissedCallsCount();
	int	 GetIncomingCallsCount();
	int	 GetOutgoingCallsCount();
	int  GetAllCallsCount();
	BOOL GetMissedCallsEntry(int iIndex, CALLLOGENTRY& sCallEntry);
	BOOL GetIncomingCallsEntry(int iIndex, CALLLOGENTRY& sCallEntry);
	BOOL GetOutGoingCallsEntry(int iIndex, CALLLOGENTRY& sCallEntry);
	BOOL GetAllCallsEntry(int iIndex, CALLLOGENTRY& sCallEntry);
    static HRESULT DeleteCallEntry(FILETIME& ftTime);
    static HRESULT DeleteAllCallEntry();


private:
	BOOL DestroyMissedCalls();
	BOOL DestroyIncomingCalls();
	BOOL DestroyOutgoingCalls();
    static HRESULT OpenPhoneCallLogDB(CEGUID& ceGuidDB, HANDLE& hDB, int& iNumRecords);


private:	// variables
	HANDLE	m_hCallLog;				// Handle to the call log session
	CIssVector <int> m_arrMissed;	// Missed Calls array
	CIssVector <int> m_arrIncoming; // Incoming calls array
	CIssVector <int> m_arrOutgoing; // Outgoing calls array
};


