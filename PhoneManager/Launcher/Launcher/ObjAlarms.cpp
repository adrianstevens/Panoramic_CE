#include "ObjAlarms.h"
#include "IssRegistry.h"
#include "IssDebug.h"


#define REG_CLOCK _T("SOFTWARE\\Microsoft\\Clock\\")

CObjAlarms::CObjAlarms(void)
{
    m_oStr = CIssString::Instance();
    GetAlarmInfo();
}

CObjAlarms::~CObjAlarms(void)
{

}

HRESULT CObjAlarms::GetAlarmInfo()
{
    HRESULT hr = S_OK;

    TCHAR szTemp[STRING_MAX];

    DWORD dwSize;
    WORD wBuffer;

    for(int i = 0; i < NUMBER_OF_ALARMS; i++)
    {
        ZeroMemory(&m_sAlarms[i], sizeof(AlarmType));

        m_oStr->Format(szTemp, _T("%s%i\\"), REG_CLOCK, i);

        ZeroMemory(&wBuffer, sizeof(WORD));
        dwSize = 2;
        hr = GetKey(szTemp, _T("AlarmDays"), (LPBYTE)&wBuffer, dwSize);
        m_sAlarms[i].dwDays     = wBuffer;

        if(wBuffer & 1)
            m_sAlarms[i].bMon = true;
        if(wBuffer & 2)
            m_sAlarms[i].bTues = true;
        if(wBuffer & 4)
            m_sAlarms[i].bWed = true;
        if(wBuffer & 8)
            m_sAlarms[i].bThurs = true;
        if(wBuffer & 16)
            m_sAlarms[i].bFri = true;
        if(wBuffer & 32)
            m_sAlarms[i].bSat = true;
        if(wBuffer & 64)
            m_sAlarms[i].bSun = true;

        ZeroMemory(&wBuffer, sizeof(WORD));
        dwSize = 2;
        hr = GetKey(szTemp, _T("AlarmFlags"), (LPBYTE)&wBuffer, dwSize);
        //ok this one is easy
        m_sAlarms[i].bActive = (BOOL)wBuffer;

        ZeroMemory(&wBuffer, sizeof(WORD));
        dwSize = 4;
        hr = GetKey(szTemp, _T("AlarmTime"), (LPBYTE)&wBuffer, dwSize);
        //also easy
        m_sAlarms[i].dwHours    = wBuffer/60;
        m_sAlarms[i].dwMinutes  = wBuffer%60;
    }
    
//Error:
    return S_OK;

}


/********************************************************************

Function:    GetKey

Inputs:        szSubKey - Registry Directory where the stuff is stored
iLength - Maximum Length of the String
lpData - The Key Value to get

Outputs:    szText - Text to Get out                

Returns:    BOOL - TRUE if successful

Comments:    This function will retrieve a string from a registry key

*********************************************************************/
HRESULT CObjAlarms::GetKey(TCHAR* szSubKey, 
               TCHAR* szValue, 
               LPBYTE lpData, 
               DWORD& dwSize)  
{  
    HKEY hKey = NULL;  
    DWORD rc;  
    HRESULT hr = S_OK;

    //open the Key  
    rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, 0, &hKey);  
    CBARG(rc == ERROR_SUCCESS, _T("RegOpenKeyEx"));

    //get the string from the key  
    rc = RegQueryValueEx(hKey, szValue, NULL, NULL, lpData, &dwSize);  
    CBARG(rc == ERROR_SUCCESS, _T("RegQueryValueEx failed"));

Error:
    if(hKey)
        RegCloseKey(hKey);
    return hr; 
}  

BOOL CObjAlarms::IsAlarmActiveIn24(int iAlarm)
{
    if(iAlarm < 0 || iAlarm >= NUMBER_OF_ALARMS)
        return FALSE;

    if(m_sAlarms[iAlarm].bActive == FALSE)
        return FALSE;

    //otherwise check the alarm time and the day of the week
    SYSTEMTIME sTime;
    GetLocalTime(&sTime);

    BOOL bBefore = FALSE;

    if(sTime.wHour < m_sAlarms[iAlarm].dwHours)
        bBefore = TRUE;
    else if(sTime.wHour == m_sAlarms[iAlarm].dwHours && sTime.wMinute < m_sAlarms[iAlarm].dwMinutes)
        bBefore = TRUE;


    //if we're before the alarm time check today
    DWORD dwCompare = sTime.wDayOfWeek;

    if(bBefore == FALSE)
        dwCompare++;
    if(dwCompare > 6)
        dwCompare = 0;

    switch(dwCompare)
    {
    case 0:
        if(m_sAlarms[iAlarm].bSun)
            return TRUE;
        break;
    case 1:
        if(m_sAlarms[iAlarm].bMon)
            return TRUE;
        break;
    case 2:
        if(m_sAlarms[iAlarm].bTues)
            return TRUE;
        break;
    case 3:
        if(m_sAlarms[iAlarm].bWed)
            return TRUE;
        break;
    case 4:
        if(m_sAlarms[iAlarm].bThurs)
            return TRUE;
        break;
    case 5:
        if(m_sAlarms[iAlarm].bFri)
            return TRUE;
        break;
    case 6:
        if(m_sAlarms[iAlarm].bSat)
            return TRUE;
        break;

    }
    return FALSE;
}

AlarmType* CObjAlarms::GetAlarm(int iAlarm)
{
    if(iAlarm < 0 || iAlarm >= NUMBER_OF_ALARMS)
        return NULL;

    return &m_sAlarms[iAlarm];
}

BOOL CObjAlarms::IsAlarmActive()
{
    for(int i = 0; i < NUMBER_OF_ALARMS; i++)
    {
        if(m_sAlarms[i].bActive)
            return TRUE;
    }
    return FALSE;
}