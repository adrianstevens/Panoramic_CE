/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssAreaCode.h
// Abstract: Determines area code from phone number - North American formatting only
//                       Area code information loaded in from CSV files
//  
/***************************************************************************************************************/ 

#pragma once

#include "IssVector.h"
#include "IssString.h"

#define NO_TIME_ZONE 99

enum EnumLookupType
{
    LT_CountryCodes,
    LT_AreaCodes,
    LT_Australia,
    LT_NewZealand,
    LT_UnitedKingdom,
    LT_Off,
    LT_Count,//always useful
};

struct AreaCodeType
{
	TCHAR*  szExitCode;
    TCHAR*  szLocalCode;
	TCHAR*	szRegion;
    TCHAR*  szCountry;
    TCHAR*  szCapital;
    TCHAR*  szCurrency;
    TCHAR*  szLanguages;
	int		iAreaCode;
	int     iCountryCode;
	int		iTimeZoneOffset; //relative to central time
	int     iX;
	int     iY;
	int     iFrame;
    int     iPopulation;
    int     iDataYear;
    int     iImageIndex;
	AreaCodeType();
	~AreaCodeType();
};

class CIssAreaCode
{
public:
        CIssAreaCode(void);
        ~CIssAreaCode(void);

		void					Destroy();
        void                    Init(HINSTANCE hInst){m_hInst = hInst;};
        BOOL                    LoadAreaCodes();//loads north America and world codes
        EnumLookupType          GetLookupType(){return m_eType;};
        BOOL                    SetLookupType(EnumLookupType eType);//mostly for other countries
        BOOL                    SetAreaCode(int iCode, EnumLookupType eType);//input the area code as an int - ie 604
        SYSTEMTIME              GetLocalTime();//get the local time of the incoming call
        AreaCodeType*	        GetCodeInfo();
        AreaCodeType*           GetCodeInfo(EnumLookupType eType, int iIndex);
        AreaCodeType*	        GetAreaCodeInfo(int iIndex);
        AreaCodeType*	        GetCountryCodeInfo(int iIndex);
        AreaCodeType*           GetOtherInfo(int iIndex);
        AreaCodeType*           GetCountryByCode(int iCode);

        int                     GetCodeCount();
        int                     GetAreaCodeCount(){return m_arrAreaCodes.GetSize();};
        int                     GetCountryCodeCount(){return m_arrCountryCodes.GetSize();};
        int                     GetOtherCount(){return m_arrOtherCountry.GetSize();};

//        BOOL                    IsCountryCode(){return m_bIsCountryCode;};

        BOOL                    ExtractCode(TCHAR* szPhoneNum, int& iCode, EnumLookupType& eType);
        static BOOL             MakeStrictNum(TCHAR* szPhoneNum);

        BOOL                    SetUserNumber(TCHAR* szPhoneNum);

        void                    SetCanada();
        void                    SetUSA();

private:
		BOOL					ExtractAreaCode(TCHAR* szPhoneNum, int& iCode, EnumLookupType& eType);
		BOOL					ExtractCountryCode(TCHAR* szPhoneNum, int& iCode, EnumLookupType& eType);
		BOOL					ExtractOtherCode(TCHAR* szPhoneNum, int& iCode, EnumLookupType& eType);

        void                    DelCodeVector(CIssVector<AreaCodeType>& arrVector);

        static int              CompareCountries(const void* lp1, const void* lp2);
        static int              CompareRegions(const void* lp1, const void* lp2);

        BOOL                    LoadCountryCSV(EnumLookupType eType);
        BOOL                    LoadCountryCSV(TCHAR* szResName);
        
private:
        CIssString*                     m_oStr;
        
        CIssVector<AreaCodeType>        m_arrAreaCodes;
        CIssVector<AreaCodeType>        m_arrCountryCodes;
        CIssVector<AreaCodeType>        m_arrOtherCountry;//we'll create and destroy as needed

        TCHAR                           m_szUserNum[STRING_LARGE];
        TCHAR                           m_szExitCode[STRING_SMALL];
        int                             m_iUserCountryCode;

        EnumLookupType                  m_eType;


        CRITICAL_SECTION                m_cs;

        HINSTANCE                       m_hInst;

        int                             m_iAreaCodeIndex;//index in the vector for the area code
};
