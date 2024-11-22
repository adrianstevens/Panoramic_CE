#pragma once

#include "windows.h"
#include "IssString.h"
#include "wrlspwr.h"

#define REG_PowerKey     _T("System\\CurrentControlSet\\Control\\Power\\State")

#define GetWirelessDevice_ORDINAL				276
#define ChangeRadioState_ORDINAL				273
#define FreeDeviceList_ORDINAL					280

//imports from ossvcs.dll
typedef LRESULT (CALLBACK* PFN_GetWirelessDevices)(RDD **pDevices, DWORD dwFlags);
typedef LRESULT (CALLBACK* PFN_ChangeRadioState)(RDD* pDev, DWORD dwState, SAVEACTION sa);
typedef LRESULT (CALLBACK* PFN_FreeDeviceList)(RDD *pRoot);

class CObjWirelessDevices
{
public:
    CObjWirelessDevices(void);
    ~CObjWirelessDevices(void);

    BOOL    IsBTInstalled(){return m_szBluetoothName?TRUE:FALSE;};
    BOOL    IsBTOn(BOOL bQuickCheck);
    BOOL    SetBTMode(BOOL bPowerOn);   
    BOOL    IsWifiInstalled(){return m_szWifiName?TRUE:FALSE;};
    BOOL    IsWifiOn(BOOL bQuickCheck);
    BOOL    SetWifiMode(BOOL bPowerOn);

    TCHAR*  GetWifiKey(){return m_szWifiName;};
    TCHAR*  GetBTKey(){return m_szBluetoothName;};

private:    // functions
    HKEY    GetKey(HKEY hKeyRoot, TCHAR* szKey, BOOL bCreate = FALSE);
    BOOL    SetValue(HKEY hKey, TCHAR* szName, TCHAR* szValue);
    BOOL    GetValue(HKEY hKey, TCHAR* szName, TCHAR* szValue);
    BOOL    SetValue(HKEY hKey, TCHAR* szName, DWORD dwValue);
    BOOL    GetValue(HKEY hKey, TCHAR* szName, DWORD& dwValue);
    BOOL    GetWDevState(DWORD* dwWifi, DWORD* dwPhone, DWORD* dwBT);
    BOOL    SetWDevState(DWORD dwDevice, DWORD dwState);
    void    Init();
    BOOL    FindDevices();

    void    RefreshDevices();

    BOOL    GetDeviceState(int iType);//should probably be an enum ....
    BOOL    SetDeviceState(int iType, BOOL bState);

    LRESULT GetWirelessDevices(RDD **pDevices, DWORD dwFlags);
    LRESULT ChangeRadioState(RDD* pDev, DWORD dwState, SAVEACTION sa);
    LRESULT FreeDeviceList(RDD *pRoot);

   

private:    // variables
    CIssString*     m_oStr;

    HINSTANCE				m_DllOssvcs;

    PFN_GetWirelessDevices	m_pfnGetWirelessDevices;
    PFN_ChangeRadioState	m_pfnChangeRadioState;
    PFN_FreeDeviceList		m_pfnFreeDeviceList;

    RDD *                   m_Devices[3];//phone,bt,wifi
    RDD *                   m_devices; //not sure why we need this yet ...

    TCHAR*                  m_szBluetoothName;
    TCHAR*                  m_szWifiName;
};
