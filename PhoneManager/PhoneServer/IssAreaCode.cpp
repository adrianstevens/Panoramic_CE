#include "StdAfx.h"
#include "IssAreaCode.h"

CIssAreaCode::CIssAreaCode(void)
:m_oStr(CIssString::Instance())
,m_iAreaCodeIndex(-1)
{
	::InitializeCriticalSection(&m_cs);
}

CIssAreaCode::~CIssAreaCode(void)
{
	DelAreaCodeVector();
	::DeleteCriticalSection(&m_cs);
}

void CIssAreaCode::DelAreaCodeVector()
{
	AreaCodeType* pAreaCode;

	for(int i = 0; i < m_arrAreaCodes.GetSize(); i++)
	{
		pAreaCode = m_arrAreaCodes[i];
		if(pAreaCode)
		{
			if(pAreaCode->szDescription)
				m_oStr->Delete(&pAreaCode->szDescription);
		}
		delete pAreaCode;
		pAreaCode = NULL;
	}

	m_arrAreaCodes.RemoveAll();
	m_iAreaCodeIndex = -1;//reset it
}

BOOL CIssAreaCode::LoadAreaCodes()
{
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
	LPVOID pdata	= LockResource(hbytes);
	char*  strData	= (char*)pdata;

	char* szFile = m_oStr->CreateAndCopyChar(strData);
	if(!szFile)
	{
		::LeaveCriticalSection(&m_cs);
		return FALSE;
	}

	char		szEndl[] = "\n";
	char		*szToken;

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
		// area code first
		iIndex = m_oStr->Find(szLine, _T(","), 0);
		if(iIndex == -1)
		{ delete areaCode; break; }//clean up
		m_oStr->StringCopy(szTemp, szLine, 0, iIndex);
		areaCode->iAreaCode = m_oStr->StringToInt(szTemp);
		
		// region
		iPrev = iIndex + 1;
		iIndex = m_oStr->Find(szLine, _T(","), iPrev);
		if(iIndex == -1)
		{ delete areaCode; break; }//clean up
		m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
		if(m_oStr->GetLength(szTemp) < 3)
			m_oStr->StringCopy(areaCode->szRegion, szTemp);
		else
			m_oStr->Empty(areaCode->szRegion);

		// timezone offset from central
		iPrev = iIndex + 1;
		iIndex = m_oStr->Find(szLine, _T(","), iPrev);
		if(iIndex == -1)
		{ delete areaCode; break; }//clean up
		m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
		areaCode->iTimeZoneOffset = m_oStr->StringToInt(szTemp);

		// description
		iPrev = iIndex + 1;
		iIndex = m_oStr->GetLength(szLine);
		if(iIndex == -1)
		{ delete areaCode; break; }//clean up
		m_oStr->StringCopy(szTemp, szLine, iPrev, iIndex - iPrev);
		if(m_oStr->GetLength(szTemp) == 0)
			areaCode->szDescription = NULL;
		else
			areaCode->szDescription = m_oStr->CreateAndCopy(szTemp);
		
		// and add that sucker to the vector yo
		m_arrAreaCodes.AddElement(areaCode);

		// get the next line
		szToken = m_oStr->StringToken(NULL, szEndl);
	}

	::LeaveCriticalSection(&m_cs);

	return TRUE;
}

//give the area code and find the index in the vector
void CIssAreaCode::SetAreaCode(int iAreaCode)
{
	m_iAreaCodeIndex = -1;

	for(int i = 0; i < m_arrAreaCodes.GetSize(); i++)
	{
		AreaCodeType* areaCode = m_arrAreaCodes[i];
		
		if(areaCode == NULL)
			break;//oops

		if(iAreaCode == areaCode->iAreaCode)
		{
			m_iAreaCodeIndex = i;
			break;//success!
		}
	}
}

SYSTEMTIME CIssAreaCode::GetLocalTime()
{
	SYSTEMTIME localtime;
	memset(&localtime, 0, sizeof(SYSTEMTIME));

	if(m_iAreaCodeIndex < 0)
		return localtime;

	GetSystemTime(&localtime);
	localtime.wHour += m_arrAreaCodes[m_iAreaCodeIndex]->iTimeZoneOffset;

	if(localtime.wHour < 0)
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

AreaCodeType* CIssAreaCode::GetAreaCodeInfo()
{
	if(m_iAreaCodeIndex == -1)
		return NULL; 
	if(m_iAreaCodeIndex >= m_arrAreaCodes.GetSize())
		return NULL;
	return m_arrAreaCodes[m_iAreaCodeIndex];
}
