/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssKey.h
// Abstract: Unit Conversion Class ... fully localizable but is dependant on resources
//  
/***************************************************************************************************************/

#include "windows.h"
#include "IssKey.h"

//Globals
#define START_UP         _T("StartUp")

CIssKey::CIssKey(void) :
 m_bGood(FALSE)
,m_bDemo(FALSE)
,m_iNumberofDays(0)
,m_iDaysLeft(0)
,m_dwDemoStartup(0)
,m_dwLegitStartup(0)
,m_dwLegitKey(0)
,m_hKeyStartup(NULL)
,m_hKey(NULL)
,m_bAgressiveFail(FALSE)
{
}

CIssKey::~CIssKey()
{
    Destroy();
}

/********************************************************************
Function:       Destroy

Arguments:                              

Returns:        TRUE is successful

Comments:       Closes down open Keys so they don't stay locked
*********************************************************************/
HRESULT CIssKey::Destroy()
{
    if(m_hKey)
    {
        RegFlushKey(m_hKey);
        RegCloseKey(m_hKey);
        m_hKey = NULL;
    }

    if(m_hKeyStartup)
    {
        RegFlushKey(m_hKeyStartup);
        RegCloseKey(m_hKeyStartup);
        m_hKeyStartup = NULL;
    }
    return S_OK;
}


/********************************************************************
Function:       CheckSetKey

Arguments:                              

Returns:        E_ABORT:     Error creating key.
                S_FALSE:    Didn't set a key.
                S_OK:       Set a demo or legit key. First time calling this function.

Comments:       Check to see if we should set the key to demo or legitimate. 
*********************************************************************/
HRESULT CIssKey::CheckSetKey(void)
{
    //Check for the StartUp key. If we find it, register the relative key.

    if(!m_hKeyStartup)
    {
        //No key available.
        return E_ABORT;
    }

    //Get the startup key, if it exists.
    DWORD   dwStartUp   = 0xFFFFFFFF;
    DWORD   dwSize      = sizeof(DWORD);
    HRESULT hr          = S_OK;
    LRESULT lResult     = 0;

    lResult = RegQueryValueEx(m_hKeyStartup, START_UP, 0L, NULL, (LPBYTE)&dwStartUp, &dwSize);

    DWORD   dwKey   = 0L;

    //If the demo key exists AND the startup key is DEMO, don't create a new key!
    if(GetDemoKey(dwKey) == S_OK && dwStartUp == m_dwDemoStartup)
    {
        hr = S_FALSE;
    }

    //Did we read valid data?
    if(lResult != ERROR_SUCCESS || dwSize != sizeof(DWORD))
    {
                hr = S_FALSE;
    }

    //Make sure we have a start up key AND we DON'T have a demo key. If we have a demo key
    //the user is just reinstalling.
    if(hr == S_OK)
    {
        //Set the demo key to either demo or legit.
        if(dwStartUp == m_dwDemoStartup)
            SetDemoKey(FALSE);
        if(dwStartUp == m_dwLegitStartup)
            SetDemoKey(TRUE);
    }

    //Delete the StartUp key so we don't perform this code again.
        if(m_hKeyStartup != NULL)
		{
                RegDeleteValue(m_hKeyStartup, START_UP);
				RegFlushKey(m_hKeyStartup);
		}

    return hr;
}

/********************************************************************
Function:       Destroy

Arguments:      wYear, wMonth, wDay - today
                        rw ... will be set to yesterday given the above values

Returns:        

Comments:       This sets the rw parameters to yesterday given the w params
*********************************************************************/
void CIssKey::GetYesterday(WORD wYear,
                            WORD wMonth,
                            WORD wDay,
                            WORD& rwNewYear,
                            WORD& rwNewMonth,
                            WORD& rwNewDay)
{
    rwNewYear = wYear;
    rwNewMonth = wMonth;

    if(wDay == 1)
    {
        //Go back to previous month.
        if(wMonth == 1)
        {
            rwNewYear = wYear - 1; //Go back to previous year.
            rwNewMonth = 12;
        }
        else
        {
            rwNewMonth--;
        }
    }
    else
    {
        rwNewDay = wDay - 1;
    }
}

/********************************************************************
Function:       MakeTodayDemoKey

Arguments:      

Returns:        The unique key

Comments:       Makes a unique key based on the current day for demo versions
*********************************************************************/
DWORD CIssKey::MakeTodayDemoKey(void)
{
    SYSTEMTIME Time;

    GetLocalTime(&Time);

    return MakeDemoKey(Time.wDay, Time.wMonth, Time.wYear);
}

/********************************************************************
Function:       MakeDemoKey

Arguments:      

Returns:        The unique key

Comments:       Makes a unique key based on the given day for demo versions
*********************************************************************/
DWORD CIssKey::MakeDemoKey(WORD wDay,
    WORD wMonth,
    WORD wYear)
{
    DWORD dwMixer = (wDay * 160821) % 0x1554;

    DWORD dwKey = ((wYear + wMonth + dwMixer + wDay) * 0x19011066) % 156034;

    if(dwKey == m_dwLegitKey)
    {
        dwKey++;
    }

    return dwKey;
}

/********************************************************************
Function:       GetDemoKey

Arguments:      rdwKey - key to be written to

Returns:        S_OK if a key is found
                        S_FALSE if no key found

Comments:       Sets rdwKey if there is already a key saved in the regsitry
*********************************************************************/
HRESULT CIssKey::GetDemoKey(DWORD& rdwKey)
{
    if(!m_hKey)
    {
        return S_FALSE; //No key available.
    }

    //Get the data key.
    DWORD dwSize        = sizeof(DWORD);
    HRESULT hr          = S_OK;

    if(RegQueryValueEx(m_hKey, KEY_NAME, 0L, NULL, (LPBYTE)&rdwKey, &dwSize)
        != ERROR_SUCCESS)
    {
        hr = S_FALSE;
    }

    return hr;
}

/********************************************************************
Function:       SetDemoKey

Arguments:      bLegit - if this is TRUE we write the full version key, otherwise demo

Returns:        S_OK all good
                        E_UNEXPECTED failed to set the key for some reason

Comments:       
*********************************************************************/
HRESULT CIssKey::SetDemoKey(BOOL bLegit)
{
    HRESULT hr = S_OK;

    CHHR(m_hKey, _T("hkey not set"));

    DWORD   dwKey   = 0L;
    
    if(bLegit)
    {
        //Store the legitimate key - this program has been paid for.
        dwKey = m_dwLegitKey;
    }
    else
    {
        //Make a key only good for today.
        dwKey = MakeTodayDemoKey();
    }

    if(RegSetValueEx(m_hKey, KEY_NAME, 0L, REG_DWORD, (LPBYTE)&dwKey,
        sizeof(DWORD)) != ERROR_SUCCESS)
    {
        hr = E_UNEXPECTED;
    }
    RegFlushKey(m_hKey);

Error:
    return hr;
}


/*

    PURPOSE:    Checks the 'demo' key to find out if we're a demo version which in the period, a
                demo version outside the demo period, or a legit version.

    PARAMETERS: void

    RESULT:     S_OK:           Legit.
                S_FALSE:        Demo, but within demo period.
                E_ABORT         Demo, outside demo period.
                E_UNEXPECTED:   Unexpected error.

*/

/********************************************************************
Function:       CheckDemo

Returns:        S_OK:           Legit 
                        S_FALSE:        Demo, but within demo period.
                        E_ABORT:        Demo, outside demo period.
                        E_UNEXPECTED:   Unexpected error.

Comments:       Get the current state of the application.  ie full, demo, expired
*********************************************************************/
HRESULT CIssKey::CheckDemo(void)
{
    HRESULT hr = S_OK;

#ifdef DEBUG
    m_bGood = TRUE;
    m_bDemo = FALSE;
    goto Error;

#endif

    //Now check that the registry key is present. If it is, check it's today's demo key or
    //a legitimate permanent key. If no key or the key is wrong, pop up a message box saying
    //the demo period is over. Please buy me!
    DWORD dwKey     = 0L;

    //No key present. User might have deleted it maliciously. Exit.
    if(GetDemoKey(dwKey) == S_FALSE)
    {
        hr =  E_UNEXPECTED;
        goto Error;
    }

    if(dwKey == m_dwLegitKey)
    {
        //Legit. Just continue.
        m_bGood = TRUE;
        m_bDemo = FALSE;
        goto Error; //we're good
    }

    //Are we within NUM_DAYS_FOR_DEMO days of starting using the demo version?
    SYSTEMTIME Time;

    GetLocalTime(&Time);

    WORD wDay;
    WORD wMonth;
    WORD wYear;

    m_iDaysLeft = m_iNumberofDays;

    for(int i=0; i<m_iNumberofDays; i++)
    {
        DWORD dwTodaysKey = MakeDemoKey(Time.wDay, Time.wMonth, Time.wYear);

        if(dwKey == dwTodaysKey)
        {
            if(m_bAgressiveFail)
            {
                DWORD dwSize  = sizeof(DWORD);
                DWORD rdwKey;
                if(RegQueryValueEx(m_hKey, _T("Expired"), 0L, NULL, (LPBYTE)&rdwKey, &dwSize)
                         == ERROR_SUCCESS)
                {
                    m_bGood = FALSE;
                    m_bDemo = FALSE;
                    hr = S_FALSE;
                    goto Error;//this is by design
                }

            }
            //Demo, but within period.
            m_bGood = TRUE;
            m_bDemo = TRUE;
            
            hr = S_FALSE;
            goto Error;//this is by design
        }

        m_iDaysLeft--;

        //Go to yesterday.
        GetYesterday(Time.wYear, Time.wMonth, Time.wDay, wYear, wMonth, wDay);

        Time.wYear  = wYear;
        Time.wMonth = wMonth;
        Time.wDay   = wDay;
    }

    //We've reached our games quota or we're outside the demo period (day).
    //Display message box telling user to buy the legit version.
    m_bGood = FALSE;

    DWORD dwNumber = 1;
    
    //add to the registry that we've failed once
    RegSetValueEx(m_hKey, _T("Expired"), 0,  REG_DWORD, (LPBYTE)&dwNumber, sizeof(dwNumber));
    RegFlushKey(m_hKey);

    hr = S_FALSE;

Error:

    return hr;
//    return E_ABORT;
}

//get the number of days in the given month
WORD CIssKey::GetDaysInMonth(WORD wNewMonth, WORD wNewYear)
{
    switch(wNewMonth)
    {
    // these are the 30 day months
    case 10:
    case 3:
    case 5:
    case 8:
            return 30;
            break;
    // if it's February
    case 1:
            // check for leap year
            if((wNewYear % 4) == 0)
                    return 28;
            else
                    return 29;
            break;
    // the rest are 31
    default:
            return 31;
            break;
    }
}

//initialize
HRESULT CIssKey::Init(TCHAR* szStartupKey, TCHAR* szKeyKey, DWORD  dwDemoStartup,  DWORD   dwLegitStartup,
                                        DWORD   dwLegitKey, int iDemoDays, BOOL bAgressiveFail, HKEY    hkKey1)
{
    HRESULT hr = S_OK;

    CBARG(szStartupKey&&szKeyKey, _T(""));

    _tcscpy(m_szStartupKey, szStartupKey);
    _tcscpy(m_szKeyKey, szKeyKey);
    
    Destroy();

    m_bAgressiveFail        = bAgressiveFail;
    m_dwDemoStartup         = dwDemoStartup;
    m_dwLegitStartup        = dwLegitStartup;
    m_dwLegitKey            = dwLegitKey;
    m_iNumberofDays         = iDemoDays;

    DWORD   dwDisposition   = 0L;

    //Try to create the keys.
   LRESULT lResult = RegCreateKeyEx(hkKey1, 
                m_szStartupKey, 0L, NULL, 0L,
                KEY_ALL_ACCESS, NULL, &m_hKeyStartup, &dwDisposition);

    LRESULT lResult2 = RegCreateKeyEx(HKEY_CURRENT_USER, m_szKeyKey, 0L, NULL, 0L,
                KEY_ALL_ACCESS, NULL, &m_hKey, &dwDisposition);

    CHHRE(m_hKey, _T("invalid hkey"), E_INVALIDARG);
    CHHRE(m_hKeyStartup, _T("ivalid startup hkey"), E_INVALIDARG);
    CBHRE(lResult == ERROR_SUCCESS, _T("Reg create failed"), E_ABORT);
    CBHRE(lResult2 == ERROR_SUCCESS, _T("Reg create failed"), E_ABORT);

        
Error:
    return hr;
}