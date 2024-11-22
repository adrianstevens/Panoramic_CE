#include "StdAfx.h"
#include "IssStateAndNotify.h"
//#include "CommonDefines.h"
#include "IssString.h"


#define NOTIFY_MAX_CNT 20				
HREGNOTIFY  g_hNotify[NOTIFY_MAX_CNT];  // Handles to notifications 


static int g_iNotifyCount = 0;

void CIssStateAndNotify::InitializeNotifications()
{
    for(int i = 0; i<NOTIFY_MAX_CNT; i++)
    {
        g_hNotify[i] = NULL;
    }
    g_iNotifyCount = 0;
}

void CIssStateAndNotify::DestroyNotifications()
{
    for(int i=0; i<NOTIFY_MAX_CNT; i++)
    {
        if(g_hNotify[i])
        {
            RegistryCloseNotification(g_hNotify[i]);
            g_hNotify[i] = NULL;
        }
    }
    g_iNotifyCount = 0;
}

BOOL CIssStateAndNotify::RegisterCallbackNotification(DWORD dwUserData, void* callback)
{
	if(g_iNotifyCount >= NOTIFY_MAX_CNT)
		return FALSE;

	// first unregister
	if (g_hNotify[g_iNotifyCount] != NULL)
	{
		// Close off the notification.
		RegistryCloseNotification(g_hNotify[g_iNotifyCount]);
	}
	g_hNotify[g_iNotifyCount] = 0;

	HRESULT hr;


	hr = RegistryNotifyCallback(SN_TIME_ROOT, 
		SN_TIME_PATH, 
		SN_TIME_VALUE, 
		(REGISTRYNOTIFYCALLBACK)callback,
		dwUserData, 
		NULL, 
		&g_hNotify[g_iNotifyCount]);

	g_iNotifyCount++;
	return TRUE;
}

BOOL CIssStateAndNotify::RegisterCustomWindowNotification(HWND hWndTarget, HKEY hKeyStart, TCHAR* szKey, TCHAR* szValue, UINT uiMessage)
{
    if(g_iNotifyCount >= NOTIFY_MAX_CNT)
        return FALSE;

    HRESULT hr;

    // first unregister
    if (g_hNotify[g_iNotifyCount] != NULL)
    {
        // Close off the notification.
        RegistryCloseNotification(g_hNotify[g_iNotifyCount]);
    }
    g_hNotify[g_iNotifyCount] = 0;

    hr = RegistryNotifyWindow(
        hKeyStart,
        szKey,
        szValue,
        hWndTarget, 
        uiMessage,      
        0,
        NULL,
        &g_hNotify[g_iNotifyCount]);

    g_iNotifyCount++;
    return TRUE;
}

BOOL CIssStateAndNotify::RegisterWindowNotification(HWND hWndTarget, WindowsMessage msg)
{
	switch(msg)
	{
    case WM_NOTIFY_TALKING:
        return RegisterCustomWindowNotification(hWndTarget,
                                                SN_PHONECALLTALKING_ROOT,
                                                SN_PHONECALLTALKING_PATH,
                                                SN_PHONECALLTALKING_VALUE,
                                                WM_NOTIFY_TALKING);

            break;
	case WM_NOTIFY_TIME:
            return RegisterCustomWindowNotification(hWndTarget,
				                                    SN_TIME_ROOT,
				                                    SN_TIME_PATH,
				                                    SN_TIME_VALUE,
				                                    WM_NOTIFY_TIME);

		    break;
	case WM_NOTIFY_DATE:
            return RegisterCustomWindowNotification(hWndTarget,
				SN_DATE_ROOT,
				SN_DATE_PATH,
				SN_DATE_VALUE,
				WM_NOTIFY_DATE);      
            break;
	case WM_NOTIFY_MISSED:
		    return RegisterCustomWindowNotification(hWndTarget,
				SN_PHONEMISSEDCALLS_ROOT,
				SN_PHONEMISSEDCALLS_PATH,
				SN_PHONEMISSEDCALLS_VALUE,
				WM_NOTIFY_MISSED);
		    break;
	case WM_NOTIFY_SMS:
		    return RegisterCustomWindowNotification(hWndTarget,
				SN_MESSAGINGSMSUNREAD_ROOT,
				SN_MESSAGINGSMSUNREAD_PATH,
				SN_MESSAGINGSMSUNREAD_VALUE,
				WM_NOTIFY_SMS);
		    break;
	case WM_NOTIFY_VOICEMAIL:
		    return RegisterCustomWindowNotification(hWndTarget,
				SN_MESSAGINGVOICEMAILTOTALUNREAD_ROOT,
				SN_MESSAGINGVOICEMAILTOTALUNREAD_PATH,
				SN_MESSAGINGVOICEMAILTOTALUNREAD_VALUE,
				WM_NOTIFY_VOICEMAIL);
            break;
	case WM_NOTIFY_EMAIL:
		    return RegisterCustomWindowNotification(hWndTarget,
				SN_MESSAGINGTOTALEMAILUNREAD_ROOT,
				SN_MESSAGINGTOTALEMAILUNREAD_PATH,
				SN_MESSAGINGTOTALEMAILUNREAD_VALUE,
				WM_NOTIFY_EMAIL);
		    break;
    case WM_NOTIFY_PROFILE:
            return RegisterCustomWindowNotification(hWndTarget,
                SN_PHONERADIOOFF_ROOT,
                SN_PHONERADIOOFF_PATH,
                SN_PHONERADIOOFF_VALUE,
                WM_NOTIFY_PROFILE);
 
            break;
	}

	return TRUE;

}

////////////////////////////////////////////////////////////////
////////////////////////// incoming ////////////////////////////
BOOL CIssStateAndNotify::GetLastCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize)
{
	HRESULT hr = RegistryGetString( SN_PHONELASTINCOMINGCALLERNUMBER_ROOT,
			SN_PHONELASTINCOMINGCALLERNUMBER_PATH,
			SN_PHONELASTINCOMINGCALLERNUMBER_VALUE,
			szPhoneNumber,
			dwSize);

	return (hr == ERROR_SUCCESS);
}

BOOL CIssStateAndNotify::GetIncomingCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize)
{
	HRESULT hr = RegistryGetString( SN_PHONEINCOMINGCALLERNUMBER_ROOT,
		SN_PHONEINCOMINGCALLERNUMBER_PATH,
		SN_PHONEINCOMINGCALLERNUMBER_VALUE,
		szPhoneNumber,
		dwSize);

	return (hr == ERROR_SUCCESS);
}

BOOL CIssStateAndNotify::GetIncomingCallerName(TCHAR* szName, DWORD dwSize)
{
	HRESULT hr = RegistryGetString( SN_PHONEINCOMINGCALLERNAME_ROOT,
		SN_PHONEINCOMINGCALLERNAME_PATH,
		SN_PHONEINCOMINGCALLERNAME_VALUE,
		szName,
		dwSize);

	return (hr == ERROR_SUCCESS);
}

BOOL CIssStateAndNotify::GetIncomingCallerContactOID(long* pOid)
{
	DWORD dwResult;
	HRESULT hr = RegistryGetDWORD( SN_PHONEINCOMINGCALLERCONTACT_ROOT,
		SN_PHONEINCOMINGCALLERCONTACT_PATH,
		SN_PHONEINCOMINGCALLERCONTACT_VALUE,
		&dwResult);

	*pOid = dwResult;

	return (hr == ERROR_SUCCESS);
}
//////////////////////////////////////////////////////////////////////////
///////////////////////// outgoing ///////////////////////////////////////
BOOL CIssStateAndNotify::GetOutgoingCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize)
{
	HRESULT hr = RegistryGetString( SN_PHONETALKINGCALLERNUMBER_ROOT,
		SN_PHONETALKINGCALLERNUMBER_PATH,
		SN_PHONETALKINGCALLERNUMBER_VALUE,
		szPhoneNumber,
		dwSize);

	return (hr == ERROR_SUCCESS);
}

BOOL CIssStateAndNotify::GetOutgoingCallerName(TCHAR* szName, DWORD dwSize)
{
	HRESULT hr = RegistryGetString( SN_PHONETALKINGCALLERNAME_ROOT,
		SN_PHONETALKINGCALLERNAME_PATH,
		SN_PHONETALKINGCALLERNAME_VALUE,
		szName,
		dwSize);

	return (hr == ERROR_SUCCESS);
}

BOOL CIssStateAndNotify::GetOutgoingCallerContactOID(long* pOid)
{
	DWORD dwResult;
	HRESULT hr = RegistryGetDWORD( SN_PHONETALKINGCALLERCONTACT_ROOT,
		SN_PHONETALKINGCALLERCONTACT_PATH,
		SN_PHONETALKINGCALLERCONTACT_VALUE,
		&dwResult);

	*pOid = dwResult;

	return (hr == ERROR_SUCCESS);

}

BOOL CIssStateAndNotify::GetPhoneRadioOff()
{
    DWORD dwResult;
    HRESULT hr = RegistryGetDWORD( SN_PHONERADIOOFF_ROOT,
        SN_PHONERADIOOFF_PATH,
        SN_PHONERADIOOFF_VALUE,
        &dwResult);

    if(hr != ERROR_SUCCESS)
        return FALSE;

    return (BOOL)(dwResult & SN_PHONERADIOOFF_BITMASK);
}

/////////////////////////////////////////////////////////////////////////////////
BOOL CIssStateAndNotify::GetBusyStatus()
{
	DWORD dwResult;
	HRESULT hr = RegistryGetDWORD( SN_CALENDARAPPOINTMENTBUSYSTATUS_ROOT,
		SN_CALENDARAPPOINTMENTBUSYSTATUS_PATH,
		SN_CALENDARAPPOINTMENTBUSYSTATUS_VALUE,
		&dwResult);

	if(hr != ERROR_SUCCESS)
		return FALSE;

	return (dwResult != 0);
}

int CIssStateAndNotify::GetUnreadEmailCount()
{
	DWORD dwResult;
	HRESULT hr = RegistryGetDWORD( SN_MESSAGINGTOTALEMAILUNREAD_ROOT,
		SN_MESSAGINGTOTALEMAILUNREAD_PATH,
		SN_MESSAGINGTOTALEMAILUNREAD_VALUE,
		&dwResult);

	if(hr == ERROR_SUCCESS)
		return (int)dwResult;

	return 0;
}
int CIssStateAndNotify::GetMissedCallsCount()
{
	DWORD dwResult;
	HRESULT hr = RegistryGetDWORD( SN_PHONEMISSEDCALLS_ROOT,
		SN_PHONEMISSEDCALLS_PATH,
		SN_PHONEMISSEDCALLS_VALUE,
		&dwResult);

	if(hr == ERROR_SUCCESS)
		return (int)dwResult;

	return 0;
}

BOOL CIssStateAndNotify::ResetMissedCallsCount()
{
	HRESULT hr = RegistrySetDWORD( SN_PHONEMISSEDCALLS_ROOT,
		SN_PHONEMISSEDCALLS_PATH,
		SN_PHONEMISSEDCALLS_VALUE,
		0);

	return(hr == ERROR_SUCCESS);
}

int CIssStateAndNotify::GetUnreadSMSCount()
{
	DWORD dwResult;
	HRESULT hr = RegistryGetDWORD( SN_MESSAGINGSMSUNREAD_ROOT,
		SN_MESSAGINGSMSUNREAD_PATH,
		SN_MESSAGINGSMSUNREAD_VALUE,
		&dwResult);

	if(hr == ERROR_SUCCESS)
		return (int)dwResult;

	return 0;
}

int CIssStateAndNotify::GetUnReadVoicemailCount()
{
	DWORD dwResult;
	HRESULT hr = RegistryGetDWORD( SN_MESSAGINGVOICEMAILTOTALUNREAD_ROOT,
		SN_MESSAGINGVOICEMAILTOTALUNREAD_PATH,
		SN_MESSAGINGVOICEMAILTOTALUNREAD_VALUE,
		&dwResult);

	if(hr == ERROR_SUCCESS)
		return (int)dwResult;

	return 0;
}

BOOL CIssStateAndNotify::GetServiceProviderName(TCHAR* szName, DWORD dwSize)
{
	HRESULT hr = RegistryGetString(SN_PHONEOPERATORNAME_ROOT, SN_PHONEOPERATORNAME_PATH, 
		SN_PHONEOPERATORNAME_VALUE, szName, dwSize);

    CIssString* oStr = CIssString::Instance();
    oStr->Trim(szName);

	return (hr == ERROR_SUCCESS);
}

BOOL CIssStateAndNotify::GetVoiceMailNumber(TCHAR* szName, DWORD dwSize)
{
	/*HRESULT hr = RegistryGetString(SN_VOICEMAILPHONENUMBER_ROOT, SN_VOICEMAILPHONENUMBER_PATH, 
		SN_VOICEMAILPHONENUMBER_VALUE, szName, dwSize);

	return (hr == ERROR_SUCCESS);*/

    
    return TRUE;


}
