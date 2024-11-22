/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssAreaCode.cpp
// Abstract: Determines area code from phone number - North American formatting only
//                       Area code information loaded in from CSV files
//  
/***************************************************************************************************************/ 

#include "StdAfx.h"
#include "IssAreaCode.h"

#define MAX_LOCAL_LEN 10
#define MIN_LOCAL_LEN 7

AreaCodeType::AreaCodeType()
:szExitCode(NULL)
,szLocalCode(NULL)
,szRegion(NULL)
,szCountry(NULL)
,szCapital(NULL)
,szCurrency(NULL)
,szLanguages(NULL)
,iAreaCode(0)
,iCountryCode(0)
,iTimeZoneOffset(0)
,iX(0)
,iY(0)
,iFrame(0)
,iPopulation(0)
,iDataYear(0)
,iImageIndex(0)
{}

AreaCodeType::~AreaCodeType()
{
	CIssString* oStr = CIssString::Instance();
	oStr->Delete(&szExitCode);
	oStr->Delete(&szLocalCode);
	oStr->Delete(&szRegion);
	oStr->Delete(&szCountry);
	oStr->Delete(&szCapital);
	oStr->Delete(&szCurrency);
	oStr->Delete(&szLanguages);
}

CIssAreaCode::CIssAreaCode(void)
:m_oStr(CIssString::Instance())
,m_iAreaCodeIndex(-1)
,m_eType(LT_CountryCodes)
{
	::InitializeCriticalSection(&m_cs);
	m_oStr->Empty(m_szUserNum);
}

CIssAreaCode::~CIssAreaCode(void)
{
	::DeleteCriticalSection(&m_cs);
	Destroy();
}

void CIssAreaCode::Destroy()
{
    DelCodeVector(m_arrAreaCodes);
    DelCodeVector(m_arrCountryCodes);
    DelCodeVector(m_arrOtherCountry);
}

void CIssAreaCode::DelCodeVector(CIssVector<AreaCodeType>& arrVector)
{
    AreaCodeType* pAreaCode;

    for(int i = 0; i < arrVector.GetSize(); i++)
    {
        pAreaCode = arrVector[i];
        if(pAreaCode)
            delete pAreaCode;
    }

    arrVector.RemoveAll();
    m_iAreaCodeIndex = -1;//reset it

}

/********************************************************************
Function:       LoadAreaCodes

Arguments:                              

Returns:        void

Comments:       Load area codes from the CSV resource
*********************************************************************/
BOOL CIssAreaCode::LoadAreaCodes()
{
    if(m_arrAreaCodes.GetSize() > 0)
        return TRUE;//why do it twice?

    ::EnterCriticalSection(&m_cs);  

    DWORD dwDisposition = OPEN_EXISTING;

    // Open the CSV File for reading
    HRSRC hres = FindResource(m_hInst, _T("IDR_CSV_AREACODES"), _T("TXT"));
    if(hres == 0)
            return FALSE;

    //If resource is found a handle to the resource is returned
    //now just load the resource
    HGLOBAL    hbytes = LoadResource(m_hInst, hres);

    // Lock the resource
    LPVOID pdata    = LockResource(hbytes);
    char*  strData  = (char*)pdata;

    char* szFile = m_oStr->CreateAndCopyChar(strData);
    if(!szFile)
    {
            ::LeaveCriticalSection(&m_cs);
            return FALSE;
    }

    char            szEndl[] = "\n";
    char            *szToken;

    TCHAR szLine[STRING_MAX];
    TCHAR szTemp[STRING_MAX];

    m_oStr->Empty(szLine);
    m_oStr->Empty(szTemp);

    // Read the First Line in the File
    szToken = m_oStr->StringToken(szFile, szEndl);

    int iLineIndex = 0;//because string tokens suck

    int iIndex;
    int iPrev;

    while (szToken != NULL && m_oStr->GetLength(szToken) > 0)
    {
        AreaCodeType* areaCode = new AreaCodeType;
        
        // copy the line
        m_oStr->StringCopy(szLine, szToken);

        // take off any spaces and/or return characters
        m_oStr->Trim(szLine);
            
	    // take the line apart and put it into the vector
        // country code first
        iIndex = m_oStr->Find(szLine, _T(","), 0);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, 0, iIndex);
        areaCode->iCountryCode = m_oStr->StringToInt(szTemp);

	    // area code
        iPrev = iIndex + 1;
	    iIndex = m_oStr->Find(szLine, _T(","), iPrev);
	    if(iIndex == -1)
	    { delete areaCode; break; }//clean up
	    m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
	    areaCode->iAreaCode = m_oStr->StringToInt(szTemp);

        // exit code
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->GetLength(szTemp) > 0)
            areaCode->szExitCode = m_oStr->CreateAndCopy(szTemp);
        else
            areaCode->szExitCode = NULL;

        // local code
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->GetLength(szTemp) > 0)
            areaCode->szLocalCode = m_oStr->CreateAndCopy(szTemp);
        else
            areaCode->szLocalCode = NULL;
		
        // country
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->GetLength(szTemp) > 0)
            areaCode->szCountry = m_oStr->CreateAndCopy(szTemp);
        else
            areaCode->szCountry = NULL;

        // region
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->GetLength(szTemp) == 0)
            areaCode->szRegion = NULL;
        else
            areaCode->szRegion = m_oStr->CreateAndCopy(szTemp);

	    // timezone offset from central
	    iPrev = iIndex + 1;
	    iIndex = m_oStr->Find(szLine, _T(","), iPrev);
	    if(iIndex == -1)
	    { delete areaCode; break; }//clean up
	    m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->IsEmpty(szTemp))
            areaCode->iTimeZoneOffset = -1;
        else
	        areaCode->iTimeZoneOffset = m_oStr->StringToInt(szTemp);

        //frame
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->IsEmpty(szTemp))
            areaCode->iFrame = 0;
        else
            areaCode->iFrame = m_oStr->StringToInt(szTemp);
      //  areaCode->iFrame--;

        //x
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->IsEmpty(szTemp))
            areaCode->iX = -1;
        else
            areaCode->iX = m_oStr->StringToInt(szTemp);
     
        //y
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->IsEmpty(szTemp))
            areaCode->iY = -1;
        else
            areaCode->iY = m_oStr->StringToInt(szTemp);

        //capital
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->GetLength(szTemp) == 0)
            areaCode->szCapital = NULL;
        else
            areaCode->szCapital = m_oStr->CreateAndCopy(szTemp);


        //currency
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->GetLength(szTemp) == 0)
            areaCode->szCurrency = NULL;
        else
            areaCode->szCurrency = m_oStr->CreateAndCopy(szTemp);

        //Languages
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->GetLength(szTemp) == 0)
            areaCode->szLanguages = NULL;
        else
            areaCode->szLanguages = m_oStr->CreateAndCopy(szTemp);

        //population
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        areaCode->iPopulation = m_oStr->StringToInt(szTemp);

        //year
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        areaCode->iDataYear = m_oStr->StringToInt(szTemp);

        //image index
        iPrev = iIndex + 1;
        iIndex = m_oStr->GetLength(szLine);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(iIndex - iPrev > 0)
            areaCode->iImageIndex = m_oStr->StringToInt(szTemp);

        // now here's where the fun comes in
        if(areaCode->iCountryCode != 1)//its a country code plain and simple
        {
            m_arrCountryCodes.AddElement(areaCode);
            areaCode = NULL;
        }
        else if(areaCode->iCountryCode == 1 && areaCode->iAreaCode == 0)//good ol North America
        {
            m_arrCountryCodes.AddElement(areaCode);
            areaCode = NULL;
        }
        else //we have NANP number ... we're going to make life easy for our users
        {//and add it to both lists
            //so first things first ... it goes into the area codes
            BOOL bNoRegion = FALSE;

            if(areaCode->iAreaCode)
            {
                //however ... if there's no region we're going to hack it for sorting purposes
                if(areaCode->szRegion == NULL)
                {
                    areaCode->szRegion = m_oStr->CreateAndCopy(areaCode->szCountry);
                    bNoRegion = TRUE;
                }
                
                m_arrAreaCodes.AddElement(areaCode);

            }

            if(bNoRegion)
            {
                //now make a new pointer for the country code since we're gonna mod it slightly
                AreaCodeType* countryCode = new AreaCodeType;
                countryCode->iAreaCode = 0;
                countryCode->iCountryCode = 1000 + areaCode->iAreaCode; //since really the two together are the country code 
                countryCode->iDataYear = areaCode->iDataYear;
                countryCode->iFrame = areaCode->iFrame;
                countryCode->iPopulation = areaCode->iPopulation;
                countryCode->iTimeZoneOffset = areaCode->iTimeZoneOffset;
                countryCode->iX = areaCode->iX;
                countryCode->iY = areaCode->iY;
                countryCode->iImageIndex = areaCode->iImageIndex;
                
                // no region for countries
                countryCode->szRegion = NULL;


                if(m_oStr->IsEmpty(areaCode->szCountry))
                    countryCode->szCountry = NULL;
                else
                    countryCode->szCountry = m_oStr->CreateAndCopy(areaCode->szCountry);

                if(m_oStr->IsEmpty(areaCode->szCapital))
                    countryCode->szCapital = NULL;
                else
                    countryCode->szCapital = m_oStr->CreateAndCopy(areaCode->szCapital);

                if(m_oStr->IsEmpty(areaCode->szCurrency))
                    countryCode->szCurrency = NULL;
                else
                    countryCode->szCurrency = m_oStr->CreateAndCopy(areaCode->szCurrency);

                if(m_oStr->IsEmpty(areaCode->szLanguages))
                    countryCode->szLanguages = NULL;
                else
                    countryCode->szLanguages = m_oStr->CreateAndCopy(areaCode->szLanguages);

                if(m_oStr->IsEmpty(areaCode->szLocalCode))
                    countryCode->szLocalCode = NULL;
                else
                    countryCode->szLocalCode = m_oStr->CreateAndCopy(areaCode->szLocalCode);

                if(m_oStr->IsEmpty(areaCode->szExitCode))
                    countryCode->szExitCode = NULL;
                else
                    countryCode->szExitCode = m_oStr->CreateAndCopy(areaCode->szExitCode);

                m_arrCountryCodes.AddElement(countryCode);
                countryCode = NULL;
            }

            //and nullify bitches
            areaCode = NULL;
            
        }

        // get the next line
        szToken = m_oStr->StringToken(NULL, szEndl);
    }

    ::LeaveCriticalSection(&m_cs);

    //sort em
    m_arrAreaCodes.Sort(CompareRegions);
    m_arrCountryCodes.Sort(CompareCountries);

	m_oStr->Delete(&szFile);


#ifdef DEBUG
 /*   TCHAR szPhoneNum[STRING_LARGE];
    BOOL bIsCountry;
    BOOL bRet;
    int iCode;

    m_oStr->StringCopy(szPhoneNum, _T("531160822"));//cuba??
    MakeStrictNum(szPhoneNum);
    bRet = ExtractCode(szPhoneNum, iCode, bIsCountry);

    m_oStr->StringCopy(szPhoneNum, _T("542615244781"));//argentina
    MakeStrictNum(szPhoneNum);
    bRet = ExtractCode(szPhoneNum, iCode, bIsCountry);

    m_oStr->StringCopy(szPhoneNum, _T("+43-720-880603"));//austria
    MakeStrictNum(szPhoneNum);
    bRet = ExtractCode(szPhoneNum, iCode, bIsCountry);

    m_oStr->StringCopy(szPhoneNum, _T("+32-50580107"));//belguim
    MakeStrictNum(szPhoneNum);
    bRet = ExtractCode(szPhoneNum, iCode, bIsCountry);

    m_oStr->StringCopy(szPhoneNum, _T("+353-91443995"));//Ireland
    MakeStrictNum(szPhoneNum);
    bRet = ExtractCode(szPhoneNum, iCode, bIsCountry);

    m_oStr->StringCopy(szPhoneNum, _T("+1 (784) 535-9144"));//St Vincent
    MakeStrictNum(szPhoneNum);
    bRet = ExtractCode(szPhoneNum, iCode, bIsCountry);*/

#endif 

    return TRUE;
}

/********************************************************************
Function:       SetAreaCode

Arguments:                              

Returns:        void

Comments:       Find a given area code in the vector
*********************************************************************/
//give the area code and find the index in the vector
BOOL CIssAreaCode::SetAreaCode(int iCode, EnumLookupType eType)
{
    m_iAreaCodeIndex = -1;

    int iEditedCode = iCode;

    if(eType == LT_AreaCodes)
    {
        if(iEditedCode > 1000)
            iEditedCode -= 1000;

        for(int i = 0; i < m_arrAreaCodes.GetSize(); i++)
        {
            AreaCodeType* areaCode = m_arrAreaCodes[i];

            if(areaCode == NULL)
                break;//oops

            if(iEditedCode == areaCode->iAreaCode)
            {
                m_iAreaCodeIndex = i;
                m_eType = eType;
                return TRUE;
                break;//success!
            }
        }
    }
    else if(eType == LT_CountryCodes)
    {
        for(int i = 0; i < m_arrCountryCodes.GetSize(); i++)
        {
            AreaCodeType* areaCode = m_arrCountryCodes[i];

            if(areaCode == NULL)
                break;//oops

            if(iCode == 1 && areaCode->iImageIndex > 0)
                continue; // north america hack

            if(iCode == areaCode->iCountryCode)
            {
                m_iAreaCodeIndex = i;
                m_eType = eType;
                return TRUE;
                break;//success!
            }
        }
    }
    else //might need to load in some values
    {
        if(m_eType != eType || m_arrOtherCountry.GetSize() == 0)
        {   //get to work
            if(LoadCountryCSV(eType) == FALSE)
            {
                m_iAreaCodeIndex = -1;
                return FALSE;
            }
        }
        //and search
        for(int i = 0; i < m_arrOtherCountry.GetSize(); i++)
        {
            AreaCodeType* areaCode = m_arrOtherCountry[i];

            if(areaCode == NULL)
                break;//oops

            if(iCode == areaCode->iAreaCode)
            {
                m_iAreaCodeIndex = i;
                m_eType = eType;
                return TRUE;
                break;//success!
            }
        }
    }
    

    m_iAreaCodeIndex = -1;
    return FALSE;
}

/********************************************************************
Function:       GetLocalTime

Arguments:                              

Returns:        void

Comments:       Gets the current local time
*********************************************************************/
SYSTEMTIME CIssAreaCode::GetLocalTime()
{
    SYSTEMTIME localtime;
	memset(&localtime, 0, sizeof(SYSTEMTIME));

    TIME_ZONE_INFORMATION timeZone;
    DWORD dwDS = 0;

    dwDS = GetTimeZoneInformation(&timeZone);


	GetSystemTime(&localtime);
    

    if(m_iAreaCodeIndex < 0  || 
        (m_eType == LT_CountryCodes && m_arrCountryCodes[m_iAreaCodeIndex]->iTimeZoneOffset == -1) ||
        (m_eType == LT_AreaCodes && m_arrAreaCodes[m_iAreaCodeIndex]->iTimeZoneOffset == -1))
    {
        ::GetLocalTime(&localtime);
        return localtime;
    }

	if(m_eType != LT_AreaCodes && m_eType != LT_CountryCodes && m_arrOtherCountry[m_iAreaCodeIndex]->iTimeZoneOffset == -1)
	{//should never happen ...
		::GetLocalTime(&localtime);
		return localtime;
	}

    int iTemp = 0;
    
    if(m_eType == LT_CountryCodes)
        iTemp = m_arrCountryCodes[m_iAreaCodeIndex]->iTimeZoneOffset;
    else if(m_eType == LT_AreaCodes)
        iTemp = m_arrAreaCodes[m_iAreaCodeIndex]->iTimeZoneOffset;
	else
		iTemp = m_arrOtherCountry[m_iAreaCodeIndex]->iTimeZoneOffset;

	localtime.wHour += iTemp;

    //now correct for DST
    if(dwDS == TIME_ZONE_ID_DAYLIGHT)
        localtime.wHour ++;

	if(localtime.wHour > 48)
	{
		localtime.wHour += 24;
	}
	if(localtime.wHour > 24)
	{
		localtime.wHour -= 24;
	}

	//the date and days of the week will be wrong but I don't care
	return localtime;
}

AreaCodeType* CIssAreaCode::GetCodeInfo()
{
	if(m_iAreaCodeIndex < 0)
		return NULL; 

    int iSize;

    if(m_eType == LT_CountryCodes)
    {
        iSize = m_arrCountryCodes.GetSize();
        if(m_iAreaCodeIndex < m_arrCountryCodes.GetSize())
            return m_arrCountryCodes[m_iAreaCodeIndex];
    }
    else if(m_eType == LT_AreaCodes)
    {
        iSize = m_arrAreaCodes.GetSize();
        if(m_iAreaCodeIndex < m_arrAreaCodes.GetSize())
            return m_arrAreaCodes[m_iAreaCodeIndex];
    }
    else
    {
        iSize = m_arrOtherCountry.GetSize();
        if(m_iAreaCodeIndex < m_arrOtherCountry.GetSize())
            return m_arrOtherCountry[m_iAreaCodeIndex];

    }

    return NULL;
}

AreaCodeType* CIssAreaCode::GetCodeInfo(EnumLookupType eType, int iIndex)
{
    switch(eType)
    {
    default:
        if(eType != m_eType)
            LoadCountryCSV(eType);
        break;
    case LT_CountryCodes:
        return GetCountryCodeInfo(iIndex);
        break;
    case LT_AreaCodes:
        return GetAreaCodeInfo(iIndex);
        break;
    }

    return NULL;
}

int CIssAreaCode::GetCodeCount()
{
    switch(m_eType)
    {
    case LT_CountryCodes:
        return m_arrCountryCodes.GetSize();
        break;
    case LT_AreaCodes:
        return m_arrAreaCodes.GetSize();
        break;
    default:
        return m_arrOtherCountry.GetSize();
        break;
    }

    return 0;
}

AreaCodeType* CIssAreaCode::GetOtherInfo(int iIndex)
{
    if(iIndex < 0 || iIndex >= m_arrOtherCountry.GetSize())
        return NULL;

    return m_arrOtherCountry[iIndex];
}


AreaCodeType* CIssAreaCode::GetAreaCodeInfo(int iIndex)
{
    if(iIndex < 0 || iIndex >= m_arrAreaCodes.GetSize())
        return NULL;

    return m_arrAreaCodes[iIndex];
}

AreaCodeType* CIssAreaCode::GetCountryCodeInfo(int iIndex)
{
    if(iIndex < 0 || iIndex >= m_arrCountryCodes.GetSize())
        return NULL;

    return m_arrCountryCodes[iIndex];
}

AreaCodeType* CIssAreaCode::GetCountryByCode(int iCode)
{
    AreaCodeType* sArea = NULL;

    for(int i = 0; i < m_arrCountryCodes.GetSize(); i++)
    {
        sArea = m_arrCountryCodes[i];

        if(sArea == NULL)
            continue;

        if(sArea->iCountryCode == iCode)
            return sArea;
    }
    return NULL;

}

int CIssAreaCode::CompareCountries(const void* lp1, const void* lp2)
{
    // sort text alphabetically
    AreaCodeType& area1 = **(AreaCodeType **)lp1;
    AreaCodeType& area2 = **(AreaCodeType **)lp2;

    CIssString* oStr = CIssString::Instance();

    if(area1.szCountry == NULL || area2.szCountry == NULL)
        return -1;

    return oStr->Compare(area1.szCountry, area2.szCountry);
}

int CIssAreaCode::CompareRegions(const void* lp1, const void* lp2)
{
    // sort text alphabetically
    AreaCodeType& area1 = **(AreaCodeType **)lp1;
    AreaCodeType& area2 = **(AreaCodeType **)lp2;

    CIssString* oStr = CIssString::Instance();

    if(area1.szRegion == NULL || area2.szRegion == NULL)
        return -1;

    return oStr->Compare(area1.szRegion, area2.szRegion);
}

BOOL CIssAreaCode::ExtractAreaCode(TCHAR* szPhoneNum, int& iCode, EnumLookupType& eType)
{
	TCHAR szCode[STRING_SMALL];
	TCHAR szTemp[STRING_LARGE];
	CIssString* oStr = CIssString::Instance();

	int iLen = oStr->GetLength(szPhoneNum);

	m_oStr->StringCopy(szTemp, szPhoneNum);

	if(iLen != 10 && iLen != 11)
		return FALSE;

	int iOffset = 0;
	if(iLen == 11)
	{
		if(szPhoneNum[0] != _T('1'))
			return FALSE;
		iOffset = 1;
	}

	//now loop through and find our code 
	AreaCodeType* sArea = NULL;
	iLen = 0;
	int iTemp;

	oStr->StringCopy(szCode, szTemp, iOffset, 3);
	iTemp = oStr->StringToInt(szCode);

	for(int i = 0; i < m_arrAreaCodes.GetSize(); i++)
	{
		sArea = m_arrAreaCodes[i];

		if(sArea == NULL)
			continue;

		if(iTemp == sArea->iAreaCode)
		{
			eType = LT_AreaCodes;
			iCode = iTemp;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CIssAreaCode::ExtractCountryCode(TCHAR* szPhoneNum, int& iCode, EnumLookupType& eType)
{
	TCHAR szCode[STRING_SMALL];
	TCHAR szTemp[STRING_LARGE];
	CIssString* oStr = CIssString::Instance();

	int iLen = oStr->GetLength(szPhoneNum);

	m_oStr->StringCopy(szTemp, szPhoneNum);

	//the easy one ... since local area codes failed (or we're not checking)
	if(szPhoneNum[0] == _T('7'))
	{
		iCode = 7;
		return TRUE;
	}

	AreaCodeType* sArea = NULL;
	iLen = 0;
	int iTemp2, iTemp3, iTemp4;

	oStr->StringCopy(szCode, szTemp, 0, 2);
	iTemp2 = oStr->StringToInt(szCode);

	oStr->StringCopy(szCode, szTemp, 0, 3);
	iTemp3 = oStr->StringToInt(szCode);

	oStr->StringCopy(szCode, szTemp, 0, 4);
	iTemp4 = oStr->StringToInt(szCode);

	//now everything else is2 to 5 digits ... brute force time!
	for(int i = 0; i < m_arrCountryCodes.GetSize(); i++)
	{
		sArea = m_arrCountryCodes[i];
		if(sArea == NULL)
			continue;

		if(iTemp2 == sArea->iCountryCode)
		{
			iCode = iTemp2;
			return TRUE;
		}

		if(iTemp3 == sArea->iCountryCode)
		{
			iCode = iTemp3;
			return TRUE;
		}

		if(iTemp4 == sArea->iCountryCode)
		{
			iCode = iTemp4;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CIssAreaCode::ExtractOtherCode(TCHAR* szPhoneNum, int& iCode, EnumLookupType& eType)
{
	TCHAR szCode[STRING_SMALL];
	TCHAR szTemp[STRING_LARGE];
	CIssString* oStr = CIssString::Instance();

	m_oStr->StringCopy(szTemp, szPhoneNum);

	int iLen = oStr->GetLength(szPhoneNum);
	int iOffSet = 0;

	AreaCodeType* sArea = NULL;
	
	//is the country code attached??
	TCHAR szCountryCode[STRING_SMALL];
	
	//is there a leading zero
	if(szTemp[0] == _T('0'))
		iOffSet++;

	//is the country code attached?
	else if(m_arrOtherCountry[0] != NULL)
	{	
		m_oStr->IntToString(szCountryCode, m_arrOtherCountry[0]->iCountryCode);
		if(m_oStr->Find(szTemp, szCountryCode) == 0)
			iOffSet = m_oStr->GetLength(szCountryCode);
	}

	int iTemp2, iTemp3, iTemp4, iTemp5, iTemp6, iTemp7;

	oStr->StringCopy(szCode, szTemp, iOffSet, 2);
	iTemp2 = oStr->StringToInt(szCode);

	oStr->StringCopy(szCode, szTemp, iOffSet, 3);
	iTemp3 = oStr->StringToInt(szCode);

	oStr->StringCopy(szCode, szTemp, iOffSet, 4);
	iTemp4 = oStr->StringToInt(szCode);

	oStr->StringCopy(szCode, szTemp, iOffSet, 5);
	iTemp5 = oStr->StringToInt(szCode);

	oStr->StringCopy(szCode, szTemp, iOffSet, 6);
	iTemp6 = oStr->StringToInt(szCode);

	oStr->StringCopy(szCode, szTemp, iOffSet, 7);
	iTemp7 = oStr->StringToInt(szCode);

	//now everything else is2 to 5 digits ... brute force time!
	for(int i = 0; i < m_arrOtherCountry.GetSize(); i++)
	{
		sArea = m_arrOtherCountry[i];
		if(sArea == NULL)
			break;

		if(iTemp2 == sArea->iAreaCode)
		{
			iCode = iTemp2;
			return TRUE;
		}

		if(iTemp3 == sArea->iAreaCode)
		{
			iCode = iTemp3;
			return TRUE;
		}

		if(iTemp4 == sArea->iAreaCode)
		{
			iCode = iTemp4;
			return TRUE;
		}
		if(iTemp5 == sArea->iAreaCode)
		{
			iCode = iTemp5;
			return TRUE;
		}

		if(iTemp6 == sArea->iAreaCode)
		{
			iCode = iTemp6;
			return TRUE;
		}

		if(iTemp7 == sArea->iAreaCode)
		{
			iCode = iTemp7;
			return TRUE;
		}
	}
	return FALSE;
}

//assume a strict phone number ... don't be lazy
//this includes country code and area code
BOOL CIssAreaCode::ExtractCode(TCHAR* szPhoneNum, int& iCode, EnumLookupType& eType)
{
//    TCHAR szCode[STRING_SMALL];
//    TCHAR szTemp[STRING_LARGE];
    CIssString* oStr = CIssString::Instance();

    if(oStr->IsEmpty(szPhoneNum))
        return FALSE;

	switch(eType)
	{
	case LT_AreaCodes:
		if(ExtractAreaCode(szPhoneNum, iCode, eType))
		{
			eType = LT_AreaCodes;
			return TRUE;
		}
		else if(ExtractCountryCode(szPhoneNum, iCode, eType))
		{
			eType = LT_CountryCodes;
			return TRUE;
		}
		break;
	case LT_CountryCodes:
		if(ExtractCountryCode(szPhoneNum, iCode, eType))
		{
			eType = LT_CountryCodes;
			return TRUE;
		}
		else if(ExtractAreaCode(szPhoneNum, iCode, eType))
		{
			eType = LT_AreaCodes;
			return TRUE;
		} 
		break;
	default:
		if(ExtractOtherCode(szPhoneNum, iCode, eType))
		{
			return TRUE;
		}
		else if(ExtractCountryCode(szPhoneNum, iCode, eType))
		{
			eType = LT_CountryCodes;
			return TRUE;
		}
		break;
	}

    eType = LT_Off;
    iCode = 0;
    return FALSE;
}

BOOL CIssAreaCode::MakeStrictNum(TCHAR* szPhoneNum)
{
    CIssString* oStr = CIssString::Instance();

    if(oStr->IsEmpty(szPhoneNum))
        return FALSE;

    TCHAR* szTemp = oStr->CreateAndCopy(szPhoneNum);

    int iIndex = 0;

    for(int i = 0; i < oStr->GetLength(szTemp); i++)
    {
        if(szTemp[i] >= _T('0') && szTemp[i] <= _T('9'))
        {
            szPhoneNum[iIndex] = szTemp[i];
            iIndex++;
        }
    }

    oStr->Delete(&szTemp);

    szPhoneNum[iIndex] = _T('\0');
    return TRUE;
}

BOOL CIssAreaCode::SetUserNumber(TCHAR* szPhoneNum)
{   
	if(m_oStr->GetLength(m_szUserNum) > 0)
		return FALSE;//why do it again??

    m_oStr->Empty(m_szUserNum);
    m_iUserCountryCode = 1;
    m_eType = LT_CountryCodes;

    if(szPhoneNum == NULL)
        return FALSE;

    m_oStr->StringCopy(m_szUserNum, szPhoneNum);

    ExtractCode(m_szUserNum, m_iUserCountryCode, m_eType);
    SetAreaCode(m_iUserCountryCode, m_eType);

    //now lets set the exit code
    m_oStr->Empty(m_szExitCode);

    AreaCodeType* sArea = GetCodeInfo();

    if(sArea && sArea->szExitCode)
        m_oStr->StringCopy(m_szExitCode, sArea->szExitCode);
    else if(m_iUserCountryCode == 391 && m_eType == LT_AreaCodes)
        m_oStr->StringCopy(m_szExitCode, _T("011"));

    return TRUE;
}

BOOL CIssAreaCode::LoadCountryCSV(EnumLookupType eType)
{
    if(m_eType == eType)
        return TRUE;//should be good

    TCHAR szTemp[STRING_LARGE];



    switch(eType)
    {
    case LT_Australia:
        m_oStr->StringCopy(szTemp, _T("IDR_CSV_AUSTRALIA"));
    	break;
    case LT_NewZealand:
        m_oStr->StringCopy(szTemp, _T("IDR_CSV_NEWZEALAND"));
        break;
    case LT_UnitedKingdom:
        m_oStr->StringCopy(szTemp, _T("IDR_CSV_UNITEDKINGDOM"));
        break;
    default:
        return FALSE;
        break;
    }

    if(LoadCountryCSV(szTemp) == FALSE)
        return FALSE;

    m_eType = eType;
    return TRUE;
}


BOOL CIssAreaCode::LoadCountryCSV(TCHAR* szResName)
{
    if(m_arrCountryCodes.GetSize() < 1)
        return FALSE;

    HRSRC hres = FindResource(m_hInst, szResName, _T("TXT"));

    if(hres == 0)
        return FALSE;

    DelCodeVector(m_arrOtherCountry);//reset this sucker

    AreaCodeType* pCountry = NULL;

    //If resource is found a handle to the resource is returned
    //now just load the resource
    HGLOBAL    hbytes = LoadResource(m_hInst, hres);   

    // Lock the resource
    LPVOID pdata    = LockResource(hbytes);
    char*  strData  = (char*)pdata;

    char* szFile = m_oStr->CreateAndCopyChar(strData);
    if(!szFile)
    {
        ::LeaveCriticalSection(&m_cs);
        return FALSE;
    }

    char            szEndl[] = "\n";
    char            *szToken;

    TCHAR szLine[STRING_MAX];
    TCHAR szTemp[STRING_MAX];

    m_oStr->Empty(szLine);
    m_oStr->Empty(szTemp);

    // Read the First Line in the File
    szToken = m_oStr->StringToken(szFile, szEndl);

    int iLineIndex = 0;//because string tokens suck

    int iIndex;
    int iPrev;

    while (szToken != NULL && m_oStr->GetLength(szToken) > 0)
    {
        AreaCodeType* areaCode = new AreaCodeType;

        // copy the line
        m_oStr->StringCopy(szLine, szToken);

        // take off any spaces and/or return characters
        m_oStr->Trim(szLine);

        // take the line apart and put it into the vector
        // country code first
        iIndex = m_oStr->Find(szLine, _T(","), 0);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, 0, iIndex);
        areaCode->iCountryCode = m_oStr->StringToInt(szTemp);

        // area code
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        areaCode->iAreaCode = m_oStr->StringToInt(szTemp);

        // region
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->GetLength(szTemp) == 0)
            areaCode->szRegion = NULL;
        else
            areaCode->szRegion = m_oStr->CreateAndCopy(szTemp);

        // timezone offset from central
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->IsEmpty(szTemp))
            areaCode->iTimeZoneOffset = -1;
        else
            areaCode->iTimeZoneOffset = m_oStr->StringToInt(szTemp);

        //frame
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->IsEmpty(szTemp))
            areaCode->iFrame = 0;
        else
            areaCode->iFrame = m_oStr->StringToInt(szTemp);
        //  areaCode->iFrame--;

        //x
        iPrev = iIndex + 1;
        iIndex = m_oStr->Find(szLine, _T(","), iPrev);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->IsEmpty(szTemp))
            areaCode->iX = -1;
        else
            areaCode->iX = m_oStr->StringToInt(szTemp);

        //y
        iPrev = iIndex + 1;
        iIndex = m_oStr->GetLength(szLine);
        if(iIndex == -1)
        { delete areaCode; break; }//clean up
        m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
        if(m_oStr->IsEmpty(szTemp))
            areaCode->iY = -1;
        else
            areaCode->iY = m_oStr->StringToInt(szTemp);

        if(iLineIndex == 0)
        {
            pCountry = GetCountryByCode(areaCode->iCountryCode);
            if(pCountry == NULL)
            {
                m_arrOtherCountry.AddElement(areaCode);
                goto Error;
            }
        }

        //time to fill in the missing info
        areaCode->iDataYear = pCountry->iDataYear;
        areaCode->iPopulation = pCountry->iPopulation;
        areaCode->szCapital = m_oStr->CreateAndCopy(pCountry->szCapital);
        areaCode->szCurrency = m_oStr->CreateAndCopy(pCountry->szCurrency);
        areaCode->szExitCode = m_oStr->CreateAndCopy(pCountry->szExitCode);
        areaCode->szLanguages = m_oStr->CreateAndCopy(pCountry->szLanguages);
        areaCode->szLocalCode = m_oStr->CreateAndCopy(pCountry->szLocalCode);
        areaCode->szCountry = m_oStr->CreateAndCopy(pCountry->szCountry);

        if(areaCode->iX < 1 || areaCode->iY < 1)
        {
            areaCode->iFrame = pCountry->iFrame;
            areaCode->iX = pCountry->iX;
            areaCode->iY = pCountry->iY;
        }

        m_arrOtherCountry.AddElement(areaCode);

        // get the next line
        szToken = m_oStr->StringToken(NULL, szEndl);
    }


Error:
    ::LeaveCriticalSection(&m_cs);

    //sort em
    m_arrOtherCountry.Sort(CompareRegions);


    m_oStr->Delete(&szFile);
    return TRUE;
}

BOOL CIssAreaCode::SetLookupType(EnumLookupType eType)
{
    LoadCountryCSV(eType);
    m_eType = eType;

    return TRUE;
}

void CIssAreaCode::SetCanada()
{
    for(int i = 0; i < m_arrCountryCodes.GetSize(); i++)
    {
        AreaCodeType* areaCode = m_arrCountryCodes[i];

        if(areaCode == NULL)
            break;//oops

        if(areaCode->iCountryCode == 1 && areaCode->iImageIndex == 1)
        {
            m_iAreaCodeIndex = i;
            m_eType = LT_CountryCodes;
            return;
            break;//success!
        }
    }
}

void CIssAreaCode::SetUSA()
{
    for(int i = 0; i < m_arrCountryCodes.GetSize(); i++)
    {
        AreaCodeType* areaCode = m_arrCountryCodes[i];

        if(areaCode == NULL)
            break;//oops

        if(areaCode->iCountryCode == 1 && areaCode->iImageIndex == 2)
        {
            m_iAreaCodeIndex = i;
            m_eType = LT_CountryCodes;
            return;
            break;//success!
        }
    }
}