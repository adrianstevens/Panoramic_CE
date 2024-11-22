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

#pragma once

#define KEY_NAME    TEXT("Key")
#define KEY_TCHAR_SIZE  100

#include "IssDebug.h"

class CIssKey
{
public:
    CIssKey(void);
    ~CIssKey();

    virtual HRESULT   Init(TCHAR* szStartupKey,
                        TCHAR*  szKeyKey,
                        DWORD   dwDemoStartup,
                        DWORD   dwLegitStartup,
                        DWORD   dwLegitKey,
                        int     iDemoDays,
                        BOOL    bAgressiveFail = FALSE,
#ifdef UNDER_CE
                        HKEY    hkKey1 = HKEY_CURRENT_USER); 
#else
                        HKEY    hkKey1 = HKEY_CLASSES_ROOT); 
#endif                                                  
                                                    
            
    HRESULT    Destroy();

    virtual HRESULT CheckSetKey(void);
    virtual HRESULT CheckDemo(void);
    int     GetDaysLeft(){return m_iDaysLeft;};

private:
    void    GetYesterday(WORD wYear, 
                        WORD wMonth, 
                        WORD wDay,
                        WORD& rwNewYear, 
                        WORD& rwNewMonth, 
                        WORD& rwNewDay);

    DWORD   MakeTodayDemoKey(void);
    DWORD   MakeDemoKey(WORD wDay, 
                        WORD wMonth, 
                        WORD wYear);

    HRESULT GetDemoKey(DWORD& rdwKey);
    HRESULT SetDemoKey(BOOL fLegit);
    WORD    GetDaysInMonth(WORD wNewMonth, 
                            WORD wNewYear);

public:
            
    BOOL    m_bGood;    //TRUE if we've confirmed this is a legit app - i.e a demo within
                        //the period, or a legit (paid for) program....ALSO set in CheckDemo 
    BOOL    m_bDemo;    //TRUE if this is a demo, or FALSE if it's a legit program.....Set in CheckDemo

protected:
    HKEY    m_hKeyStartup; //The key we use to specify the startup parameters (i.e. demo or full version).
    HKEY    m_hKey; //This app registry key.

    TCHAR   m_szStartupKey[KEY_TCHAR_SIZE];
    TCHAR   m_szKeyKey[KEY_TCHAR_SIZE];
    DWORD   m_dwDemoStartup;
    DWORD   m_dwLegitStartup;
    DWORD   m_dwLegitKey;
    int             m_iNumberofDays;
    
    
    BOOL    m_bAgressiveFail;

    public:
        int             m_iDaysLeft;
};


