#include "IssCallManager.h"
#include "CommonDefines.h"
#include "ObjStateAndNotifyMgr.h"
#ifndef USE_TAPI_DEBUGOUT
#include "IssCommon.h"
#endif


CIssCallManager::CIssCallManager()
: m_hWndDlg(NULL)
, m_eCallStatus(CSTATE_Disconnected)
{
	ClearCallerIDInfo();

	//initialize poom
	CPoomContacts::Instance();	
}

CIssCallManager::~CIssCallManager()
{

}

void CIssCallManager::ClearCallerIDInfo()
{
	DebugOut(_T("ClearCallerIDInfo()"));
	m_oStr->Empty(m_sCallerID.szName);
	m_oStr->Empty(m_sCallerID.szNumber);

	if(m_sCallerID.gdiPicture)
	{
		m_sCallerID.gdiPicture->Destroy();
		delete m_sCallerID.gdiPicture;
		m_sCallerID.gdiPicture = NULL;
	}
	m_sCallerID.lOid = 0;
}

BOOL CIssCallManager::Initialize(HINSTANCE hInst, HWND hWndDlg, HWND hWndDebugWindow)
{
	m_hWndDlg = hWndDlg;
	m_hInst = hInst;

	//init our area code class
	m_oArea.Init(hInst);
	m_oArea.LoadAreaCodes();

	return CIssTapi::Initialize(hInst, hWndDebugWindow);

}


void CIssCallManager::HandleLineAppNewCall(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{

}

void CIssCallManager::ReceiveNotification(UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uiMessage)
	{
	case WM_NOTIFY_INCOMING:
		HandleIncoming(wParam & SN_PHONEINCOMINGCALL_BITMASK);
		break;
	case WM_NOTIFY_CALLERNAME:	
		// notified when caller name changes to empty string - could filter notification
		DebugOut(_T("ReceiveNotification() - WM_NOTIFY_CALLERNAME"));
		if(HandleCallerName())
		{
			GetCallerInfo();
			HandleAreaCode();
		}
		break;
	case WM_NOTIFY_BLOCKLISTCHANGE:
		break;
	case WM_NOTIFY_RINGRULECHANGE:
		break;
	case WM_NOTIFY_CALLOPTIONCHANGE:
		break;
	case WM_NOTIFY_OUTGOING:
		{
			DebugOut(_T("ReceiveNotification() - WM_NOTIFY_OUTGOING"));
			BOOL bDialing = (wParam & SN_PHONECALLCALLING_BITMASK);
			if(bDialing)
			{
				HandleOutgoing();
				HandleAreaCode();
			}
		}
		break;
	}
}

void CIssCallManager::HandleLineCallState(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	switch(dwParam3)
	{
	case 0:
		break;
	case LINECALLPRIVILEGE_MONITOR:
		DebugOut(_T("HandleLineCallState - LINECALLPRIVILEDGE_MONITOR"));
		// if there is an existing call
		if(m_psLine->hCallCurr)
		{
			if(m_psLine->hCallCurr != (HCALL)hDevice)
				DeallocateCall((HCALL)hDevice);
			else
			{
				// new call
				LPLINECALLINFO lpLineCallInfo = GetCallInfo((HCALL)hDevice);
				if(lpLineCallInfo)
				{
					if(lpLineCallInfo->dwOrigin & LINECALLORIGIN_OUTBOUND)
						// Outgoing Call
						DeallocateCall((HCALL)hDevice);
					else
					{
						// Incoming Call
						m_psLine->hCallCurr	= (HCALL)hDevice;
						LONG lReturn = lineSetCallPrivilege(m_psLine->hCallCurr, LINECALLPRIVILEGE_OWNER);
						
						switch(lReturn)
						{
						case LINEERR_INVALCALLHANDLE:
							{int i = 0;}
							break;
						case LINEERR_INVALCALLSTATE:
							{int i = 0;}
							break;
						case LINEERR_INVALCALLPRIVILEGE:
						    {int i = 0;}
							break;
						case LINEERR_OPERATIONFAILED:
						    {int i = 0;}
							break;
						case LINEERR_RESOURCEUNAVAIL:
							{int i = 0;}
							break;
						case LINEERR_UNINITIALIZED:
							{int i = 0;}
							break;
						default:
						    break;
						}
					}

					free(lpLineCallInfo);
				}
			}

		}
		break;
	case LINECALLPRIVILEGE_OWNER:
		DebugOut(_T("HandleLineCallState - LINECALLPRIVILEDGE_OWNER"));
		if(m_psLine->hCallCurr == (HCALL)NULL)
			// New Call
			m_psLine->hCallCurr = (HCALL)hDevice;
		else
		{
			if(m_psLine->hCallCurr != (HCALL)hDevice)
			{
				// another incoming call
				if(dwParam1 != LINECALLSTATE_IDLE)
					DropCall((HCALL)hDevice);
				else
					DeallocateCall((HCALL)hDevice);
			}

		}
		break;
	}


	switch(dwParam1)
	{
	case LINECALLSTATE_OFFERING:
		DebugOut(_T("HLCS - LINECALLSTATE_OFFERING"));
		
		if(m_psLine->hCallCurr == NULL)//not sure if this is good form but we'll give it a shot
			m_psLine->hCallCurr = (HCALL)hDevice;

		if(m_psLine->hCallCurr && 
			(m_psLine->hCallCurr == (HCALL)hDevice))
		{
			LPLINEADDRESSCAPS lpLineAddressCaps;

			lpLineAddressCaps = GetLineAddressCaps();
			if (lpLineAddressCaps!=NULL) 
			{
				if (lpLineAddressCaps->dwAddrCapFlags & LINEADDRCAPFLAGS_ACCEPTTOALERT ) 
					AcceptLine();
			}
		}
		break;
	case LINECALLSTATE_ACCEPTED:
		DebugOut(_T("HLCS - LINECALLSTATE_ACCEPTED"));
		break;
	case LINECALLSTATE_CONNECTED:
		DebugOut(_T("HLCS - LINECALLSTATE_CONNECTED"));
		
		// outgoing call - we need to fill in some members
		if(m_psLine->hCallCurr == (HCALL)NULL)
			// New Call
			m_psLine->hCallCurr = (HCALL)hDevice;

		// now enable cheekguard and display connection info...
		//m_eCallStatus = CSTATE_Connected;
//		m_dlgIncoming->Connected();
        PostMessage(m_hWndDlg, WM_CALL_CONNECTED, 0, 0);

		break;
	case LINECALLSTATE_BUSY:
	case LINECALLSTATE_DISCONNECTED:
		DebugOut(_T("HLCS - LINECALLSTATE_DISCONNECTED"));
		//m_eCallStatus = CSTATE_Disconnected;
	//	m_dlgIncoming->Disconnected();
        PostMessage(m_hWndDlg, WM_CALL_ENDED, 0, 0);
		DropCall((HCALL)hDevice);
		break;
	case LINECALLSTATE_IDLE:
		DebugOut(_T("HLCS - LINECALLSTATE_IDLE"));
		if(!m_psLine->hCallCurr || m_psLine->hCallCurr != (HCALL)hDevice)
			DeallocateCall((HCALL)hDevice);
		else
		{
			DeallocateCall(m_psLine->hCallCurr);
			m_psLine->hCallCurr = NULL;

			CloseLine();

			// reinitialize TAPI
			//CIssTapi::Initialize(m_hInstance, m_hWndDebug); // appears to be unsafe to do this here
		}
		break;
	}
}

void CIssCallManager::HandleLineDevState(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	switch(dwParam1)
	{
	case LINEDEVSTATE_RINGING:
		DebugOut(_T("HLDS - RINGING"));
		if(m_psLine->hCurrLine && m_psLine->hCurrLine == (HLINE)hDevice)
		{
			//m_eCallStatus = CSTATE_Ringing;
			//PostMessage(m_hWndDlg, WM_PHONE_RINGING, dwParam2, dwParam3);
		}
		break;
	case LINEDEVSTATE_OUTOFSERVICE:
	case LINEDEVSTATE_DISCONNECTED:
	case LINEDEVSTATE_MAINTENANCE:
	case LINEDEVSTATE_REINIT: 
		DebugOut(_T("HLDS - REINIT etc"));
		if(m_psLine->hCurrLine && m_psLine->hCurrLine == (HLINE)hDevice)
		{
			// close TAPI
			DropCall(m_psLine->hCallCurr);
			CloseLine();
			Destroy();
		}
		break;
	case LINEDEVSTATE_OPEN:
		DebugOut(_T("HLDS - LINEDEVSTATE_OPEN"));
		break;
	case LINEDEVSTATE_CLOSE:
		DebugOut(_T("HLDS - LINEDEVSTATE_CLOSE"));
		break;
	case LINEDEVSTATE_NUMCALLS:
		DebugOut(_T("HLDS - LINEDEVSTATE_NUMCALLS"));
		break;

	default:
		{
			DebugOut(_T("unhandled HandleLineDevState: 0x%08x"), dwParam1);
		}
	}
}

void CIssCallManager::HandleLineReply(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{

}

void CIssCallManager::HandleLineClose(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{

}
void CIssCallManager::HandleAreaCode()
{
	if(!m_sCallerID.szNumber)
		return;

	if(!m_oStr->GetLength(m_sCallerID.szNumber))
		return;

	TCHAR szTemp[STRING_NORMAL];

	MakeCanonicalNum(m_sCallerID.szNumber, szTemp, TRUE);

	m_oArea.SetAreaCode(m_oStr->StringToInt(szTemp));
	DebugOut(_T("Areacode:"));
	DebugOut(szTemp);
	if(m_oArea.GetAreaCodeInfo())
	{
		DebugOut(m_oArea.GetAreaCodeInfo()->szRegion);
		DebugOut(m_oArea.GetAreaCodeInfo()->szCountry);
	}
}

void CIssCallManager::HandleLineCallInfo(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	switch(dwParam1)
	{
	case LINECALLINFOSTATE_CALLERID:
		{
			// here we will decide whether to put the call through or not
			// as now we should have the number and contact info
			DWORD dwCallPermissions;
			if(!CallAllowed(dwCallPermissions))
			{
				DebugOut(_T("ending call"));
				EndCall();
			}
			else
			{
				// handle ring according to permissions

				// display incoming call
				DisplayIncomingCall();
			}
		}
		break;
	case LINECALLINFOSTATE_ORIGIN:
		DebugOut(_T("HLCI -  LINECALLINFOSTATE_ORIGIN"));
		break;
	case LINECALLINFOSTATE_MEDIAMODE:
		DebugOut(_T("HLCI -  LINECALLINFOSTATE_MEDIAMODE"));
		break;
	case LINECALLINFOSTATE_CONNECTEDID:
		DebugOut(_T("HLCI -  LINECALLINFOSTATE_CONNECTEDID"));
		break;
	case LINECALLINFOSTATE_NUMMONITORS:
		DebugOut(_T("HLCI -  LINECALLINFOSTATE_NUMMONITORS"));
		break;
	case LINECALLINFOSTATE_NUMOWNERDECR:
		DebugOut(_T("HLCI -  LINECALLINFOSTATE_NUMOWNERDECR"));
		break;
		
	default:
		{
			DebugOut(_T("unhandled HLCI: 0x%08x device: %d"), dwParam1, hDevice);
		}
		break;
	}
}

void CIssCallManager::HandleIncoming(BOOL bBegin)
{
	if(bBegin)
	{
		DebugOut(_T("HandleIncoming - begin"));

		// get phone number (may not be available yet)
	}
	else
	{

	}

}

void CIssCallManager::HandleOutgoing()
{
	CObjStateAndNotifyMgr::GetOutgoingCallerNumber(m_sCallerID.szNumber, sizeof(m_sCallerID.szNumber));

	// get the caller name...
	CObjStateAndNotifyMgr::GetOutgoingCallerName(m_sCallerID.szName, sizeof(m_sCallerID.szName));

	// get the caller oid
	long lOid;
	if(!CObjStateAndNotifyMgr::GetOutgoingCallerContactOID(&lOid))
	{
		DebugOut(_T("GetIncomingCallerContactOID() failed"));
	}
	else
		m_sCallerID.lOid = lOid;

	GetCallerPicture(lOid);
}
BOOL CIssCallManager::HandleCallerName()
{
	// get the caller name...
	CObjStateAndNotifyMgr::GetIncomingCallerName(m_sCallerID.szName, sizeof(m_sCallerID.szName));
	if(!m_oStr->GetLength(m_sCallerID.szName))
		return FALSE;

	DebugOut(_T("received caller name: %s"), m_sCallerID.szName);

	return TRUE;
}
void CIssCallManager::GetCallerInfo()
{
	// first check to see if we got the caller number
	if(m_sCallerID.szNumber && !m_oStr->GetLength(m_sCallerID.szNumber))
	{
		CObjStateAndNotifyMgr::GetIncomingCallerNumber(m_sCallerID.szNumber, sizeof(m_sCallerID.szNumber));
	}

	// get the caller oid
	long lOid;
	if(!CObjStateAndNotifyMgr::GetIncomingCallerContactOID(&lOid))
	{
		DebugOut(_T("GetIncomingCallerContactOID() failed"));
	}
	else
		m_sCallerID.lOid = lOid;

	GetCallerPicture(lOid);

}

void CIssCallManager::GetCallerPicture(long lOid)
{
	if(lOid == 0 || lOid == 1)	// 0 = NULL; 1 = UNKNOWN, I believe
		return;

	// get the caller picture
	// try to reduce repeated picture creating/destroying
	static long lLastOid = 0;
	if(lOid != lLastOid || !m_sCallerID.gdiPicture)
	{
		lLastOid = lOid;
		CPoomContacts* pPoom = CPoomContacts::Instance();
		if(pPoom)
		{
			if(m_sCallerID.gdiPicture && m_sCallerID.gdiPicture->GetDC())
			{
				DebugOut(_T("destroying contact picture"));
				m_sCallerID.gdiPicture->Destroy();
				delete m_sCallerID.gdiPicture;
				m_sCallerID.gdiPicture = NULL;
			}
			CIssGDIEx* gdiImage = pPoom->GetUserPicture(lOid, 60, 80, RGB(0,0,0));
			if(gdiImage)
			{
				DebugOut(_T("got picture for %s %d"), m_sCallerID.szName, lOid);
			}
			else
			{
				DebugOut(_T("could not get picture for %s %d"), m_sCallerID.szName, lOid);
			}
			m_sCallerID.gdiPicture = gdiImage;
		}
	}

}

BOOL CIssCallManager::CallAllowed(DWORD& dwCallPermissions)
{
	return TRUE;
}


void CIssCallManager::HandleLineMonitorDigits(DWORD hDevice, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{

}

BOOL CIssCallManager::EndCall()
{
#define VK_TEND VK_F4

	// Simulate a key press
	keybd_event( VK_TEND,
		0x45,
		KEYEVENTF_EXTENDEDKEY | 0,
		0 );

	// Simulate a key release
	keybd_event( VK_TEND,
		0x45,
		KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
		0);

	return TRUE;
}

void CIssCallManager::DropCall(HCALL hCall)
{
	CIssTapi::DropCall(hCall);
	ClearCallerIDInfo();

	//m_eCallStatus = CSTATE_Disconnected;
}

void CIssCallManager::AcceptLine()
{
	if (m_psLine == NULL)
		return;

	if (m_psLine->hCallCurr != (HCALL) NULL) 
	{
		LONG lReturn = lineAccept(m_psLine->hCallCurr, NULL, 0);
		//m_eCallStatus = CSTATE_Connected;
	}
}

void CIssCallManager::AnswerLine()
{
	if (m_psLine == NULL)
		return;

	if (m_psLine->hCallCurr != (HCALL) NULL) 
	{
		LONG lReturn = lineAnswer(m_psLine->hCallCurr, NULL, 0);
		//m_eCallStatus = CSTATE_Connected;
	}
}

TCHAR* CIssCallManager::GetCallerLocation()
{
	AreaCodeType* sArea = m_oArea.GetAreaCodeInfo();

	if(sArea == NULL)
		return NULL;
	
	return sArea->szCountry;

}

EnumCallState CIssCallManager::GetCallState()
{
	if(m_psLine == NULL)
		return CSTATE_Disconnected;

	if(m_psLine->hCallCurr == NULL)
		return CSTATE_Disconnected;

	LPLINECALLSTATUS lpStat = GetCallStatus(m_psLine->hCallCurr);

	if(lpStat == NULL)
		return CSTATE_Disconnected;

	// http://msdn2.microsoft.com/en-us/library/ms735543(VS.85).aspx
	switch(lpStat->dwCallState)
	{
	case LINECALLSTATE_ACCEPTED://after offering
	case LINECALLSTATE_CONFERENCED:
	case LINECALLSTATE_CONNECTED:
		return CSTATE_Connected;
		break;
	case LINECALLSTATE_ONHOLD:
	case LINECALLSTATE_ONHOLDPENDCONF:
	case LINECALLSTATE_ONHOLDPENDTRANSFER:
		return CSTATE_OnHold;
		break;
	case LINECALLSTATE_OFFERING: //offering but not necessarily ringing
		return CSTATE_Ringing;	// DH: i need something to return 'ringing' 
								// and this is the call state when phone is ringing (incoming)
		break;
	case LINECALLSTATE_DIALTONE:
	case LINECALLSTATE_DIALING:
	case LINECALLSTATE_PROCEEDING: //proceeding AFTER dialing
	case LINECALLSTATE_RINGBACK:
		return CSTATE_Dialing;
		break;
	case LINECALLSTATE_BUSY:
	case LINECALLSTATE_DISCONNECTED:
	case LINECALLSTATE_IDLE: //apparently this is a bad thing
		return CSTATE_Disconnected;
		break;
	case LINECALLSTATE_SPECIALINFO:
	case LINECALLSTATE_UNKNOWN:
	default:
		return CSTATE_Unknown;
		break;
	}

}

BOOL CIssCallManager::DisplayIncomingCall()
{


    PostMessage(m_hWndDlg, WM_PHONE_RINGING, 0, 0);

	return TRUE;
}

