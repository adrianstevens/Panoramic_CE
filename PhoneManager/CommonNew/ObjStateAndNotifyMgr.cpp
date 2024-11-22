#include "StdAfx.h"
#include "ObjStateAndNotifyMgr.h"
#include "CommonDefines.h"
#include "ObjPhoneProfile.h"
#include "GeneralOptions.h"

#define NOTIFY_CNT 20				
HREGNOTIFY  g_hNotify[NOTIFY_CNT];  // Handles to notifications 


CObjStateAndNotifyMgr::CObjStateAndNotifyMgr(void)
{
    for(int i=0; i<NOTIFY_CNT; i++)
        g_hNotify[i] = NULL;
}

CObjStateAndNotifyMgr::~CObjStateAndNotifyMgr(void)
{
    for(int i = 0; i < NOTIFY_CNT; i++)
    {
        if (g_hNotify[i] != NULL)
        {
            // Close off the notification.
            RegistryCloseNotification(g_hNotify[i]);
        }
        g_hNotify[i] = NULL;
    }
}

//http://msdn.microsoft.com/en-us/library/aa455750.aspx
void CObjStateAndNotifyMgr::RegisterWindowNotifications(HWND hWndTarget)
{
	HRESULT hr;
	NOTIFICATIONCONDITION nc;

	// first unregister
	for(int i = 0; i < NOTIFY_CNT; i++)
	{
		if (g_hNotify[i] != NULL)
		{
			// Close off the notification.
			RegistryCloseNotification(g_hNotify[i]);
		}
		g_hNotify[i] = 0;
	}

	// 1) incoming call
	nc.ctComparisonType = REG_CT_ANYCHANGE;
	nc.dwMask           = SN_PHONEINCOMINGCALL_BITMASK;
	nc.TargetValue.dw   = 0;

	// PhoneIncomingCall
	hr = RegistryNotifyWindow(
		SN_PHONEINCOMINGCALL_ROOT,
		SN_PHONEINCOMINGCALL_PATH,
		SN_PHONEINCOMINGCALL_VALUE,
		hWndTarget, 
		WM_NOTIFY_INCOMING,      
		0,
		&nc,
		&g_hNotify[0]
	);

	// 2) caller name
	nc.ctComparisonType = REG_CT_ANYCHANGE;
	nc.dwMask           = 0;//SN_PHONEINCOMINGCALLERNAME_BITMASK;
	nc.TargetValue.dw   = 0;
	TCHAR szEmpty[2] = _T("");
	nc.TargetValue.psz = szEmpty;

	hr = RegistryNotifyWindow(
		SN_PHONEINCOMINGCALLERNAME_ROOT,
		SN_PHONEINCOMINGCALLERNAME_PATH,
		SN_PHONEINCOMINGCALLERNAME_VALUE,
		hWndTarget, 
		WM_NOTIFY_CALLERNAME,      
		0,
		&nc,
		&g_hNotify[1]
	);

	// 3) blocklist change
	nc.ctComparisonType = REG_CT_ANYCHANGE; //REG_CT_EQUAL;
	nc.dwMask           = 0x0f;
	nc.TargetValue.dw   = 0;

	hr = RegistryNotifyWindow(
		HKEY_CURRENT_USER,
		REG_KEY_ISS_PATH,
		REG_BlockListChanged,
		hWndTarget, 
		WM_NOTIFY_BLOCKLISTCHANGE,      
		0,
		&nc,
		&g_hNotify[2]
	);

	// 4) ringtone rule change
	nc.ctComparisonType = REG_CT_ANYCHANGE; 
	nc.dwMask           = 0x0f;
	nc.TargetValue.dw   = 0;

	hr = RegistryNotifyWindow(
		HKEY_CURRENT_USER,
		REG_KEY_ISS_PATH,
		REG_RingRuleChanged,
		hWndTarget, 
		WM_NOTIFY_RINGRULECHANGE,      
		0,
		&nc,
		&g_hNotify[3]
	);

	// 5) ringtone rule change
	nc.ctComparisonType = REG_CT_ANYCHANGE; 
	nc.dwMask           = 0x0f;
	nc.TargetValue.dw   = 0;

	hr = RegistryNotifyWindow(
		HKEY_CURRENT_USER,
		REG_KEY_ISS_PATH,
		REG_CallOptionsChanged,
		hWndTarget, 
		WM_NOTIFY_CALLOPTIONCHANGE,      
		0,
		&nc,
		&g_hNotify[4]
	);

	// 6) outgoing call
	nc.ctComparisonType = REG_CT_NOT_EQUAL;
	nc.dwMask           = SN_PHONECALLCALLING_BITMASK;
	nc.TargetValue.dw   = 0;	// 1

	hr = RegistryNotifyWindow(
		SN_PHONECALLCALLING_ROOT,
		SN_PHONECALLCALLING_PATH,
		SN_PHONECALLCALLING_VALUE,
		hWndTarget, 
		WM_NOTIFY_OUTGOING,      
		0,
		&nc,
		&g_hNotify[5]
	);

    // 7) call connected
    nc.ctComparisonType = REG_CT_ANYCHANGE;
    nc.dwMask           = SN_PHONECALLTALKING_BITMASK;
    nc.TargetValue.dw   = 0;	// 1

    hr = RegistryNotifyWindow(
        SN_PHONECALLTALKING_ROOT,
        SN_PHONECALLTALKING_PATH,
        SN_PHONECALLTALKING_VALUE,
        hWndTarget, 
        WM_NOTIFY_CONNECTED,      
        0,
        &nc,
        &g_hNotify[6]
    );

    // 8) battery level
    nc.ctComparisonType = REG_CT_NOT_EQUAL; 
    nc.dwMask           = SN_POWERBATTERYSTRENGTH_BITMASK;
    nc.TargetValue.dw   = 0;	// 1

    hr = RegistryNotifyWindow(
        SN_POWERBATTERYSTRENGTH_ROOT,
        SN_POWERBATTERYSTRENGTH_PATH,
        SN_POWERBATTERYSTRENGTH_VALUE,
        hWndTarget, 
        WM_NOTIFY_BATTLEVEL,      
        0,
        &nc,
        &g_hNotify[7]
    );

    // 9) signal level
    nc.ctComparisonType = REG_CT_NOT_EQUAL;
    nc.dwMask           = 0;
    nc.TargetValue.dw   = 0;	// 1

    hr = RegistryNotifyWindow(
        SN_PHONESIGNALSTRENGTH_ROOT,
        SN_PHONESIGNALSTRENGTH_PATH,
        SN_PHONESIGNALSTRENGTH_VALUE,
        hWndTarget, 
        WM_NOTIFY_SIGNALLEVEL,      
        0,
        NULL,
        &g_hNotify[8]
    );

    // 10) carrier
    nc.ctComparisonType = REG_CT_NOT_EQUAL;
    nc.dwMask           = 0;
    nc.TargetValue.dw   = 0;	// 1

    hr = RegistryNotifyWindow(
        SN_PHONEOPERATORNAME_ROOT,
        SN_PHONEOPERATORNAME_PATH,
        SN_PHONEOPERATORNAME_VALUE,
        hWndTarget, 
        WM_NOTIFY_CARRIER,      
        0,
        NULL,
        &g_hNotify[9]
    );

    // 11) Ring volume
    nc.ctComparisonType = REG_CT_ANYCHANGE; //REG_CT_EQUAL;
    nc.dwMask           = 0;
    nc.TargetValue.dw   = 0;	// 1

    hr = RegistryNotifyWindow(
        HKEY_CURRENT_USER,
        REG_SoundCategories,
        REG_SoundInitVolume,
        hWndTarget, 
        WM_NOTIFY_PROFILE_RINGVOLUME,      
        0,
        NULL,
        &g_hNotify[10]
    );

    // 12) Ring Name
    nc.ctComparisonType = REG_CT_ANYCHANGE; //REG_CT_EQUAL;
    nc.dwMask           = 0;
    nc.TargetValue.dw   = 0;	// 1

    hr = RegistryNotifyWindow(
        HKEY_CURRENT_USER,
        REG_Ringtones,
        REG_Sound,
        hWndTarget, 
        WM_NOTIFY_PROFILE_RINGNAME,      
        0,
        NULL,
        &g_hNotify[11]
    );

    // 13) Time change
    nc.ctComparisonType = REG_CT_ANYCHANGE; //REG_CT_EQUAL;
    nc.dwMask           = 0;
    nc.TargetValue.dw   = 0;	// 1

    hr = RegistryNotifyWindow(
        SN_TIME_ROOT,
        SN_TIME_PATH,
        SN_TIME_VALUE,
        hWndTarget, 
        WM_NOTIFY_DATETIME,      
        0,
        NULL,
        &g_hNotify[12]
    );

    // 14) Incoming OID
    nc.ctComparisonType = REG_CT_NOT_EQUAL;
    nc.dwMask           = 0;
    nc.TargetValue.dw   = 0;	// 1

    hr = RegistryNotifyWindow(
        SN_PHONEINCOMINGCALLERCONTACT_ROOT,
        SN_PHONEINCOMINGCALLERCONTACT_PATH,
        SN_PHONEINCOMINGCALLERCONTACT_VALUE,
        hWndTarget, 
        WM_NOTIFY_INCOMINGOID,      
        0,
        NULL,
        &g_hNotify[13]
    );


    // 15) outgoing OID
    nc.ctComparisonType = REG_CT_NOT_EQUAL;
    nc.dwMask           = 0;
    nc.TargetValue.dw   = 0;	// 1

    hr = RegistryNotifyWindow(
        SN_PHONETALKINGCALLERCONTACT_ROOT,
        SN_PHONETALKINGCALLERCONTACT_PATH,
        SN_PHONETALKINGCALLERCONTACT_VALUE,
        hWndTarget, 
        WM_NOTIFY_OUTGOINGOID,   //fine for now   
        0,
        NULL,
        &g_hNotify[14]
    );  

    // 16) General Options
    nc.ctComparisonType = REG_CT_ANYCHANGE; //REG_CT_EQUAL;
    nc.dwMask           = 0;
    nc.TargetValue.dw   = 0;	// 1

    hr = RegistryNotifyWindow(
        HKEY_CURRENT_USER,
        REG_KEY_ISS_PATH,
        REG_GeneralOptions,
        hWndTarget, 
        WM_NOTIFY_GENERALOPTIONS,      
        0,
        NULL,
        &g_hNotify[15]
    );

    // 16) Incoming number
    nc.ctComparisonType = REG_CT_NOT_EQUAL;
    nc.dwMask           = 0;
    nc.TargetValue.dw   = 0;	// 1

    hr = RegistryNotifyWindow(
        SN_PHONEINCOMINGCALLERNUMBER_ROOT,
        SN_PHONEINCOMINGCALLERNUMBER_PATH,
        SN_PHONEINCOMINGCALLERNUMBER_VALUE,
        hWndTarget, 
        WM_NOTIFY_INCOMINGNUMBER,      
        0,
        NULL,
        &g_hNotify[16]
    );

	int a = 0;
}

////////////////////////////////////////////////////////////////
////////////////////////// incoming ////////////////////////////
BOOL CObjStateAndNotifyMgr::GetLastCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize)
{
	HRESULT hr = RegistryGetString( SN_PHONELASTINCOMINGCALLERNUMBER_ROOT,
			SN_PHONELASTINCOMINGCALLERNUMBER_PATH,
			SN_PHONELASTINCOMINGCALLERNUMBER_VALUE,
			szPhoneNumber,
			dwSize);

	return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetLastCallerName(TCHAR* szName, DWORD dwSize)
{
    HRESULT hr = RegistryGetString( SN_PHONELASTINCOMINGCALLERNAME_ROOT,
        SN_PHONELASTINCOMINGCALLERNAME_PATH,
        SN_PHONELASTINCOMINGCALLERNAME_VALUE,
        szName,
        dwSize);

    return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetLastCallerType(TCHAR* szType, DWORD dwSize)
{
    HRESULT hr = RegistryGetString( SN_PHONEINCOMINGCALLERCONTACTPROPERTYNAME_ROOT,
        SN_PHONEINCOMINGCALLERCONTACTPROPERTYNAME_PATH,
        SN_PHONEINCOMINGCALLERCONTACTPROPERTYNAME_VALUE,
        szType,
        dwSize);

    return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetLastCallerOID(long* pOid)
{
    DWORD dwResult;
    HRESULT hr = RegistryGetDWORD( SN_PHONELASTINCOMINGCALLERCONTACT_ROOT,
        SN_PHONELASTINCOMINGCALLERCONTACT_PATH,
        SN_PHONELASTINCOMINGCALLERCONTACT_VALUE,
        &dwResult);

    if(hr == S_OK)
        *pOid = dwResult;

    return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetIncomingCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize)
{
	HRESULT hr = RegistryGetString( SN_PHONEINCOMINGCALLERNUMBER_ROOT,
		SN_PHONEINCOMINGCALLERNUMBER_PATH,
		SN_PHONEINCOMINGCALLERNUMBER_VALUE,
		szPhoneNumber,
		dwSize);

	return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetIncomingCallerName(TCHAR* szName, DWORD dwSize)
{
	HRESULT hr = RegistryGetString( SN_PHONEINCOMINGCALLERNAME_ROOT,
		SN_PHONEINCOMINGCALLERNAME_PATH,
		SN_PHONEINCOMINGCALLERNAME_VALUE,
		szName,
		dwSize);

	return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetIncomingCallerContactOID(long* pOid)
{
	DWORD dwResult;
	HRESULT hr = RegistryGetDWORD( SN_PHONEINCOMINGCALLERCONTACT_ROOT,
		SN_PHONEINCOMINGCALLERCONTACT_PATH,
		SN_PHONEINCOMINGCALLERCONTACT_VALUE,
		&dwResult);

    if(hr == S_OK)
	    *pOid = dwResult;

	return (hr == ERROR_SUCCESS);
}
//////////////////////////////////////////////////////////////////////////
///////////////////////// outgoing ///////////////////////////////////////
BOOL CObjStateAndNotifyMgr::GetOutgoingCallerNumber(TCHAR* szPhoneNumber, DWORD dwSize)
{
	HRESULT hr = RegistryGetString( SN_PHONETALKINGCALLERNUMBER_ROOT,
		SN_PHONETALKINGCALLERNUMBER_PATH,
		SN_PHONETALKINGCALLERNUMBER_VALUE,
		szPhoneNumber,
		dwSize);

	return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetOutgoingCallerName(TCHAR* szName, DWORD dwSize)
{
	HRESULT hr = RegistryGetString( SN_PHONETALKINGCALLERNAME_ROOT,
		SN_PHONETALKINGCALLERNAME_PATH,
		SN_PHONETALKINGCALLERNAME_VALUE,
		szName,
		dwSize);

	return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetOutgoingCallerContactOID(long* pOid)
{
	DWORD dwResult;
	HRESULT hr = RegistryGetDWORD( SN_PHONETALKINGCALLERCONTACT_ROOT,
		SN_PHONETALKINGCALLERCONTACT_PATH,
		SN_PHONETALKINGCALLERCONTACT_VALUE,
		&dwResult);

    if(hr == S_OK)
	    *pOid = dwResult;

	return (hr == ERROR_SUCCESS);

}

BOOL CObjStateAndNotifyMgr::GetBusyStatus()
{
    DWORD dwResult = 0;
    HRESULT hr = RegistryGetDWORD( SN_CALENDARAPPOINTMENTBUSYSTATUS_ROOT,
        SN_CALENDARAPPOINTMENTBUSYSTATUS_PATH,
        SN_CALENDARAPPOINTMENTBUSYSTATUS_VALUE,
        &dwResult);

    if(hr != ERROR_SUCCESS)
        return FALSE;

    return (dwResult != 0);
}

BOOL CObjStateAndNotifyMgr::GetBatteryLevel(int* iPcnt)
{
    DWORD dwResult;
    HRESULT hr = RegistryGetDWORD( SN_POWERBATTERYSTRENGTH_ROOT,
        SN_POWERBATTERYSTRENGTH_PATH,
        SN_POWERBATTERYSTRENGTH_VALUE,
        &dwResult);

    *iPcnt = (dwResult & SN_POWERBATTERYSTRENGTH_BITMASK);

    return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetSignalStrength(int* iPcnt)
{
    DWORD dwResult;
    HRESULT hr = RegistryGetDWORD( SN_PHONESIGNALSTRENGTH_ROOT,
        SN_PHONESIGNALSTRENGTH_PATH,
        SN_PHONESIGNALSTRENGTH_VALUE,
        &dwResult);

    *iPcnt = dwResult;

    return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetPhoneOperator(TCHAR* szCarrier, DWORD dwSize)
{
    HRESULT hr = RegistryGetString( SN_PHONEOPERATORNAME_ROOT,
        SN_PHONEOPERATORNAME_PATH,
        SN_PHONEOPERATORNAME_VALUE,
        szCarrier,
        dwSize);

    return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetPhoneType(TCHAR* szPhoneType, DWORD dwSize)
{
    HRESULT hr = RegistryGetString( SN_PHONELASTINCOMINGCALLERCONTACTPROPERTYNAME_ROOT,
        SN_PHONELASTINCOMINGCALLERCONTACTPROPERTYNAME_PATH,
        SN_PHONELASTINCOMINGCALLERCONTACTPROPERTYNAME_VALUE,
        szPhoneType,
        dwSize);

    return (hr == ERROR_SUCCESS);

}

//number of active calls
BOOL CObjStateAndNotifyMgr::GetActiveCallCount(int* iCount)
{
    DWORD dwResult;
    HRESULT hr = RegistryGetDWORD( SN_PHONEACTIVECALLCOUNT_ROOT,
        SN_PHONEACTIVECALLCOUNT_PATH,
        SN_PHONEACTIVECALLCOUNT_VALUE,
        &dwResult);

    *iCount = dwResult;

    return (hr == ERROR_SUCCESS);
}

BOOL CObjStateAndNotifyMgr::GetIsRoaming()
{
    DWORD dwResult = 0;
    HRESULT hr = RegistryGetDWORD( SN_PHONEROAMING_ROOT,
        SN_PHONEROAMING_PATH,
        SN_PHONEROAMING_VALUE,
        &dwResult);

    if(hr != ERROR_SUCCESS)
        return FALSE;

    return (dwResult & SN_PHONEROAMING_BITMASK);
}