#pragma once

#include "stdafx.h"
#include "wrlspwr.h"
#include "Wininet.h"
#include <service.h>

// WLAN Switch
#define _WLAN_SWITCH_OFF                       0
#define _WLAN_SWITCH_ON                        1

#define GetWirelessDevice_ORDINAL				276
#define ChangeRadioState_ORDINAL				273
#define FreeDeviceList_ORDINAL					280

//imports from ossvcs.dll
typedef LRESULT (CALLBACK* PFN_GetWirelessDevices)(RDD **pDevices, DWORD dwFlags);
typedef LRESULT (CALLBACK* PFN_ChangeRadioState)(RDD* pDev, DWORD dwState, SAVEACTION sa);
typedef LRESULT (CALLBACK* PFN_FreeDeviceList)(RDD *pRoot);



class CWirelessDevices
{
public:
	CWirelessDevices(void);
	~CWirelessDevices(void);

	// throws a getprocaddress_exception on failure
	void Init(void);
	BOOL IsLoaded(void) { return (m_DllOssvcs != NULL); }

	BOOL IsWifiPowerOn(void);

	void TurnWifiPowerOn();
	void TurnWifiPowerOff();

private:
	DWORD SetWDevState(DWORD dwDevice, DWORD dwState);
	DWORD GetWDevState(DWORD* bWifi, DWORD* bPhone, DWORD* bBT);

	HINSTANCE				m_DllOssvcs;

	PFN_GetWirelessDevices	m_pfnGetWirelessDevices;
	PFN_ChangeRadioState	m_pfnChangeRadioState;
	PFN_FreeDeviceList		m_pfnFreeDeviceList;
};
