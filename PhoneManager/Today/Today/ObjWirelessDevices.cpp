#include "ObjWirelessDevices.h"
#include "bthutil.h"
#include "pm.h"
#include "IssDebug.h"

//#pragma comment( lib,"bthutil.lib")

#define REG_PowerKey     _T("System\\CurrentControlSet\\Control\\Power\\State")

#define DEVICE_TYPE_WIFI 0
#define DEVICE_TYPE_PHONE 1
#define DEVICE_TYPE_BLUETOOTH 2


CObjWirelessDevices::CObjWirelessDevices(void)
:m_oStr(CIssString::Instance())
,m_DllOssvcs(NULL)
,m_pfnGetWirelessDevices(NULL)
,m_pfnChangeRadioState(NULL)
,m_pfnFreeDeviceList(NULL)
,m_szBluetoothName(NULL)
,m_szWifiName(NULL)
,m_devices(NULL)
{
    Init();
    FindDevices();
}

CObjWirelessDevices::~CObjWirelessDevices(void)
{
    if (m_DllOssvcs)
        ::FreeLibrary(m_DllOssvcs);
    m_DllOssvcs = NULL;
    

    if(m_devices)
        m_pfnFreeDeviceList(m_devices);
    m_devices = NULL;
}

void CObjWirelessDevices::Init()
{
    m_Devices[DEVICE_TYPE_WIFI] = NULL;
    m_Devices[DEVICE_TYPE_PHONE] = NULL;
    m_Devices[DEVICE_TYPE_BLUETOOTH] = NULL;
    m_devices = NULL;


    if (m_DllOssvcs)
        ::FreeLibrary(m_DllOssvcs);

    m_DllOssvcs = ::LoadLibrary(TEXT("ossvcs.dll"));

    if (NULL != m_DllOssvcs)
    {
        m_pfnGetWirelessDevices = (PFN_GetWirelessDevices)::GetProcAddress(m_DllOssvcs, MAKEINTRESOURCE(GetWirelessDevice_ORDINAL));
        m_pfnChangeRadioState = (PFN_ChangeRadioState)::GetProcAddress(m_DllOssvcs, MAKEINTRESOURCE(ChangeRadioState_ORDINAL));
        m_pfnFreeDeviceList = (PFN_FreeDeviceList)::GetProcAddress(m_DllOssvcs, MAKEINTRESOURCE(FreeDeviceList_ORDINAL));
    }
}

BOOL CObjWirelessDevices::IsBTOn(BOOL bQuickCheck)
{
    return GetDeviceState(DEVICE_TYPE_BLUETOOTH);

    /*DBG_OUT((_T("CObjWirelessDevices::IsBTOn")));
    if(bQuickCheck && m_szBluetoothName)
    {
        HKEY hKey = GetKey(HKEY_LOCAL_MACHINE, REG_PowerKey);
        if(!hKey)
            return FALSE;

        DWORD dwValue = 0;
        BOOL bReturn = GetValue(hKey, m_szBluetoothName, dwValue);
        
        RegCloseKey(hKey);
        DBG_OUT((_T("Registry check return: %d"), dwValue&bReturn));
        return dwValue&&bReturn;
    }

    DWORD dwPhone, dwWifi, dwBT;
    if(GetWDevState(&dwWifi, &dwPhone, &dwBT))
    {
        DBG_OUT((_T("GetWDevState return: %d"), dwBT!=0?TRUE:FALSE));
        return dwBT!=0?TRUE:FALSE;
    }
    DBG_OUT((_T("GetWDevState function failed")));
    return FALSE;*/
}

BOOL CObjWirelessDevices::SetBTMode(BOOL bPowerOn)
{
    return SetDeviceState(DEVICE_TYPE_BLUETOOTH, bPowerOn);

   /* DBG_OUT((_T("CObjWirelessDevices::SetBTMode - %d"), bPowerOn));
    //DWORD dwSetValue = bPowerOn?BTH_DISCOVERABLE:BTH_POWER_OFF;
    //if(!BthSetMode(dwSetValue))
    //    return FALSE;
    if(!SetWDevState(RADIODEVICES_BLUETOOTH, bPowerOn?BTH_DISCOVERABLE:BTH_POWER_OFF))
    {
        DBG_OUT((_T("SetWDevState failed")));
        return FALSE;
    }
    DBG_OUT((_T("SetWDevState succeeded")));
    return TRUE;*/
}

BOOL CObjWirelessDevices::IsWifiOn(BOOL bQuickCheck)
{
    return GetDeviceState(DEVICE_TYPE_WIFI);
    
/*    if(m_oStr->IsEmpty(m_szWifiName))
        return FALSE;

    DBG_OUT((_T("CObjWirelessDevices::IsWifiOn")));

    if(bQuickCheck)
    {
        HKEY hKey = GetKey(HKEY_LOCAL_MACHINE, REG_PowerKey);
        if(!hKey)
            return FALSE;

        DWORD dwValue = 0;
        BOOL bReturn = GetValue(hKey, m_szWifiName, dwValue);

        RegCloseKey(hKey);
        DBG_OUT((_T("Registry check return: %d"), dwValue&&bReturn));
        return dwValue&&bReturn;
    }

    CEDEVICE_POWER_STATE eState;
    if(ERROR_SUCCESS == GetDevicePower(m_szWifiName, POWER_NAME, &eState))
    {
        DBG_OUT((_T("GetDevicePower return: %d"), eState == D4?FALSE:TRUE));
        return eState == D4?FALSE:TRUE;
    }

    DBG_OUT((_T("GetDevicePower function failed")));
    return FALSE;*/
}

BOOL CObjWirelessDevices::SetWifiMode(BOOL bPowerOn)
{
    return SetDeviceState(DEVICE_TYPE_WIFI, bPowerOn);

   /* DBG_OUT((_T("CObjWirelessDevices::SetWifiMode - %d"), bPowerOn));
    if(m_oStr->IsEmpty(m_szWifiName))
        return FALSE;

    if(ERROR_SUCCESS == SetDevicePower(m_szWifiName, POWER_NAME, bPowerOn?D0:D4))
    {
        DBG_OUT((_T("SetDevicePower success")));
        return TRUE;
    }
    DBG_OUT((_T("SetDevicePower failed")));
    return FALSE;*/
}

HKEY CObjWirelessDevices::GetKey(HKEY hKeyRoot, TCHAR* szKey, BOOL bCreate)
{
    if(m_oStr->IsEmpty(szKey))
        return NULL;

    HKEY hKey = NULL;
    if(ERROR_SUCCESS != RegOpenKeyEx(hKeyRoot, szKey, NULL, NULL, &hKey))
    {
        if(bCreate)
        {
            DWORD dwDisposition;
            if(ERROR_SUCCESS != RegCreateKeyEx(hKeyRoot, szKey, NULL, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, &dwDisposition))
                return NULL;
        }
        else
            return NULL;
    }

    return hKey;
}

BOOL CObjWirelessDevices::SetValue(HKEY hKey, TCHAR* szName, TCHAR* szValue)
{
    if(!hKey)
        return FALSE;

    DWORD dwType	= REG_SZ;
    DWORD dwDataSize= (m_oStr->GetLength(szValue)+1)*sizeof(TCHAR);
    if(ERROR_SUCCESS != RegSetValueEx(hKey, szName, NULL, dwType, (LPBYTE)szValue, dwDataSize))
        return FALSE;

    RegFlushKey(hKey);
    return TRUE;
}

BOOL CObjWirelessDevices::GetValue(HKEY hKey, TCHAR* szName, TCHAR* szValue)
{
    if(!hKey || !szValue)
        return FALSE;

    DWORD dwType	= REG_SZ;
    DWORD dwDataSize= MAX_PATH*sizeof(TCHAR);
    TCHAR szTemp[MAX_PATH];
    ZeroMemory(szTemp, sizeof(TCHAR)*MAX_PATH);
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szTemp, &dwDataSize))
        return FALSE;

    // blank out the values and keep going
    m_oStr->StringCopy(szValue, szTemp);
    return TRUE;
}

BOOL CObjWirelessDevices::SetValue(HKEY hKey, TCHAR* szName, DWORD dwValue)
{
    if(!hKey)
        return FALSE;

    DWORD dwType	= REG_DWORD;
    DWORD dwDataSize= sizeof(DWORD);
    if(ERROR_SUCCESS != RegSetValueEx(hKey, szName, NULL, dwType, (LPBYTE)&dwValue, dwDataSize))
        return FALSE;

    RegFlushKey(hKey);
    return TRUE;
}

BOOL CObjWirelessDevices::GetValue(HKEY hKey, TCHAR* szName, DWORD& dwValue)
{
    if(!hKey )
        return FALSE;

    DWORD dwType	= REG_DWORD;
    DWORD dwDataSize= sizeof(DWORD);
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)&dwValue, &dwDataSize))
        return FALSE;

    return TRUE;
}

BOOL CObjWirelessDevices::FindDevices()
{
    HRESULT hr = S_OK;

    m_oStr->Delete(&m_szWifiName);
    m_oStr->Delete(&m_szBluetoothName);

    //CBARG(m_pfnGetWirelessDevices&&m_pfnFreeDeviceList, _T(""));

    HKEY hKey = NULL;
    RDD * pDevice = NULL;
    BOOL bResult = TRUE;
    RDD * pTD;

    hr = m_pfnGetWirelessDevices(&pDevice, 4);
    CHR(hr, _T("m_pfnGetWirelessDevices failed"));
    CBARG(pDevice, _T(""));

    pTD = pDevice;

    // loop through the linked list of devices
    while (pTD)
    {
        switch (pTD->DeviceType)
        {
        case RADIODEVICES_MANAGED:
            if(!m_oStr->IsEmpty(pTD->pszDeviceName))
                m_szWifiName = m_oStr->CreateAndCopy(pTD->pszDeviceName);
            break;
        case RADIODEVICES_PHONE:
            break;
        case RADIODEVICES_BLUETOOTH:
            if(!m_oStr->IsEmpty(pTD->pszDeviceName))
                m_szBluetoothName = m_oStr->CreateAndCopy(pTD->pszDeviceName);
            break;
        default:
            break;
        }
        pTD = pTD->pNext; 
    }

    hKey = GetKey(HKEY_LOCAL_MACHINE, REG_PowerKey);
    CPHR(hKey, _T(""));

    DWORD dwValue;

    // make sure we can access the wifi and bluetooth keys if installed
    if(m_szWifiName)
    {
        bResult = GetValue(hKey, m_szWifiName, dwValue);
        if(!bResult)
            m_oStr->Delete(&m_szWifiName);
    }

    if(m_szBluetoothName)
    {
        bResult = GetValue(hKey, m_szBluetoothName, dwValue);
        if(!bResult)
            m_oStr->Delete(&m_szBluetoothName);
    }

Error:

    if(hKey)
        RegCloseKey(hKey);

    // Free the list of devices retrieved with    
    // GetWirelessDevices()
    if(pDevice)
        m_pfnFreeDeviceList(pDevice);

    return TRUE;
}

//set the status of the desired wireless device
BOOL CObjWirelessDevices::SetWDevState(DWORD dwDevice, DWORD dwState)
{
    RDD * pDevice = NULL;
    RDD * pTD;
    HRESULT hr;
    BOOL bReturn = FALSE;

    hr = m_pfnGetWirelessDevices(&pDevice, 0);
    if (hr != S_OK) 
        return FALSE;

    if (pDevice)
    {
        pTD = pDevice;

        // loop through the linked list of devices
        while (pTD)
        {
            if  (pTD->DeviceType == dwDevice)
            {
                hr = m_pfnChangeRadioState(pTD, dwState, RADIODEVICES_PRE_SAVE);
                bReturn = TRUE;
            }

            pTD = pTD->pNext;
        }

        // Free the list of devices retrieved with    
        // GetWirelessDevices()
        m_pfnFreeDeviceList(pDevice);
    }

    return bReturn;
}

//get status of all wireless devices at once
BOOL CObjWirelessDevices::GetWDevState(DWORD* dwWifi, DWORD* dwPhone, DWORD* dwBT)
{
    RDD * pDevice = NULL;
    RDD * pTD;

    HRESULT hr;

    hr = m_pfnGetWirelessDevices(&pDevice, 0);
    if (hr != S_OK) 
        return FALSE;

    if (pDevice)
    {
        pTD = pDevice;

        // loop through the linked list of devices
        while (pTD)
        {
            switch (pTD->DeviceType)
            {
            case RADIODEVICES_MANAGED:
                *dwWifi = pTD->dwState;
                break;
            case RADIODEVICES_PHONE:
                *dwPhone = pTD->dwState;
                break;
            case RADIODEVICES_BLUETOOTH:
                *dwBT = pTD->dwState;
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

    return TRUE;
}

void CObjWirelessDevices::RefreshDevices()
{
    if(m_devices) 
        m_pfnFreeDeviceList(m_devices);
    
    RDD *rdd = NULL;
    if(m_pfnGetWirelessDevices(&m_devices, 0) == S_OK)
    {
        rdd = m_devices;
        while(rdd)
        {
            // add the device to our list
            if(rdd->DeviceType == 1) // wifi
                m_Devices[0] = rdd;
            else if(rdd->DeviceType == 2) // phone
                m_Devices[1] = rdd;
            else if(rdd->DeviceType == 3) // bluetooth
                m_Devices[2] = rdd;
            rdd = rdd->pNext;
        }
    }
}

BOOL CObjWirelessDevices::GetDeviceState(int iType)
{
    if(iType < 0 || iType > 2)
        return FALSE; //safety first

    RefreshDevices();

    if(m_Devices[iType] == NULL)
        return FALSE;
    
    return (m_Devices[iType]->dwState == 1);     
}

BOOL CObjWirelessDevices::SetDeviceState(int iType, BOOL bState)
{
    if(iType < 0 || iType > 2)
        return FALSE; //safety first

    RefreshDevices();

    if(m_Devices[iType] == NULL)
        return FALSE;

    if(!bState)
    {
        if(m_Devices[iType]->dwState == 0)
            return TRUE;


        // disable
        if(iType == DEVICE_TYPE_WIFI)
        {
            m_pfnChangeRadioState(m_Devices[iType], 0, RADIODEVICES_PRE_SAVE);
            DevicePowerNotify(m_Devices[iType]->pszDeviceName, D4, 1);
        }
        else
        {
            if(m_pfnChangeRadioState(m_Devices[iType], 0, RADIODEVICES_PRE_SAVE) == S_OK)
                return TRUE;
            else
                return FALSE;
        }
    }
    else
    {
        if(m_Devices[iType]->dwState == 1)
        {
            return TRUE;
        }
        // enable
        if(iType == DEVICE_TYPE_WIFI)
        {
            DevicePowerNotify(m_Devices[iType]->pszDeviceName, D0, 1);
            m_pfnChangeRadioState(m_Devices[iType], 1, RADIODEVICES_PRE_SAVE);
        }
        else
        {
            if(m_pfnChangeRadioState(m_Devices[iType], 1, RADIODEVICES_PRE_SAVE) == S_OK)
                return TRUE;
            else
                return FALSE;
        }
    }
    return FALSE;

}

LRESULT CObjWirelessDevices::GetWirelessDevices(RDD **pDevices, DWORD dwFlags)
{

    return S_OK;
}

LRESULT CObjWirelessDevices::ChangeRadioState(RDD* pDev, DWORD dwState, SAVEACTION sa)
{
    return S_OK;
}

LRESULT CObjWirelessDevices::FreeDeviceList(RDD *pRoot)
{
    return S_OK;
}