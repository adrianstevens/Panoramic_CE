#include "StdAfx.h"
#include "WirelessDevices.h"

CWirelessDevices::CWirelessDevices(void)
{
	m_DllOssvcs = NULL;
	
	Init();
}

CWirelessDevices::~CWirelessDevices(void)
{
	if (m_DllOssvcs)
		::FreeLibrary(m_DllOssvcs);
}

void CWirelessDevices::Init()
{
	if (m_DllOssvcs)
		::FreeLibrary(m_DllOssvcs);

	m_DllOssvcs = ::LoadLibrary(TEXT("ossvcs.dll"));

	if (NULL != m_DllOssvcs)
	{
		//m_pfnGetWirelessDevices = (PFN_GetWirelessDevices)::GetProcAddress(m_DllOssvcs, TEXT("GetWirelessDevices"));
		//m_pfnChangeRadioState = (PFN_ChangeRadioState)::GetProcAddress(m_DllOssvcs, TEXT("ChangeRadioState"));
		//m_pfnFreeDeviceList = (PFN_FreeDeviceList)::GetProcAddress(m_DllOssvcs, TEXT("FreeDeviceList"));

		m_pfnGetWirelessDevices = (PFN_GetWirelessDevices)::GetProcAddress(m_DllOssvcs, MAKEINTRESOURCE(GetWirelessDevice_ORDINAL));
		m_pfnChangeRadioState = (PFN_ChangeRadioState)::GetProcAddress(m_DllOssvcs, MAKEINTRESOURCE(ChangeRadioState_ORDINAL));
		m_pfnFreeDeviceList = (PFN_FreeDeviceList)::GetProcAddress(m_DllOssvcs, MAKEINTRESOURCE(FreeDeviceList_ORDINAL));

		if ( (NULL == m_pfnGetWirelessDevices) ||
			 (NULL == m_pfnChangeRadioState) ||
			 (NULL == m_pfnFreeDeviceList) )
		{
			::FreeLibrary(m_DllOssvcs);
		}
	}
}

BOOL CWirelessDevices::IsWifiPowerOn(void)
{
	DWORD dwWifi, dwPhone, dwBT;

	GetWDevState(&dwWifi, &dwPhone, &dwBT);
	// For dwWifi == 1 The WiFi Radio is ON

	return (BOOL)(dwWifi != 0);
}

void CWirelessDevices::TurnWifiPowerOn()
{
	SetWDevState( RADIODEVICES_MANAGED, _WLAN_SWITCH_ON);
}

void CWirelessDevices::TurnWifiPowerOff()
{
	SetWDevState( RADIODEVICES_MANAGED, _WLAN_SWITCH_OFF);
}

//set the status of the desired wireless device
DWORD CWirelessDevices::SetWDevState(DWORD dwDevice, DWORD dwState)
{
	RDD * pDevice = NULL;
    RDD * pTD;
    HRESULT hr;
	DWORD retval = 0;

    hr = m_pfnGetWirelessDevices(&pDevice, 0);
	if (hr != S_OK) return -1;
    
    if (pDevice)
    {
        pTD = pDevice;

        // loop through the linked list of devices
        while (pTD)
        {
			if  (pTD->DeviceType == dwDevice)
			{
				hr = m_pfnChangeRadioState(pTD, dwState, RADIODEVICES_PRE_SAVE);
				retval = 0;
			}
          
			pTD = pTD->pNext;
        }

        // Free the list of devices retrieved with    
        // GetWirelessDevices()
		m_pfnFreeDeviceList(pDevice);
    }

	if (hr == S_OK) return retval;
	
	return -2;
}

//get status of all wireless devices at once
DWORD CWirelessDevices::GetWDevState(DWORD* bWifi, DWORD* bPhone, DWORD* bBT)
{
	RDD * pDevice = NULL;
    RDD * pTD;

    HRESULT hr;
	DWORD retval = 0;
	
    hr = m_pfnGetWirelessDevices(&pDevice, 0);

	if (hr != S_OK) return -1;
	
    if (pDevice)
    {
	    pTD = pDevice;

        // loop through the linked list of devices
		while (pTD)
		{
			switch (pTD->DeviceType)
			{
				case RADIODEVICES_MANAGED:
					*bWifi = pTD->dwState;
					break;
				case RADIODEVICES_PHONE:
					*bPhone = pTD->dwState;
					break;
				case RADIODEVICES_BLUETOOTH:
					*bBT = pTD->dwState;
					break;
				default:
					break;
			}

			pTD = pTD->pNext; 
	    }

        // Free the list of devices retrieved with    
        // GetWirelessDevices()
        m_pfnFreeDeviceList(pDevice);
    }

	if (hr == S_OK) return retval;
	
	return -2;
}

