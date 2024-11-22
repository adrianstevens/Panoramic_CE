/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssIniEx.cpp
// Abstract: Class for reading, creating, and parsing INI formated files
//  
/***************************************************************************************************************/ 

#include "IssIniEx.h"
#include "IssDebug.h"

#ifndef UNDER_CE
#include "stdio.h"
#endif

#define MAX_FILE_SIZE           30000

TypeValue::TypeValue()
:szKeys(NULL)
,szValues(NULL)
{}

TypeValue::~TypeValue()
{
    CIssString* oStr = CIssString::Instance();
    oStr->Delete(&szKeys);
    oStr->Delete(&szValues);
}

TypeSection::TypeSection()
:szSection(NULL)
{}

TypeSection::~TypeSection()
{
    CIssString* oStr = CIssString::Instance();
    oStr->Delete(&szSection);
    for(int i=0; i<arrValues.GetSize(); i++)
    {
        TypeValue* sValue = arrValues[i];
        if(sValue)
            delete sValue;
    }
    arrValues.RemoveAll();
}

CIssIniEx::CIssIniEx()
:m_hFile(INVALID_HANDLE_VALUE)
,m_bFileOpened(FALSE)
,m_oStr(CIssString::Instance())
{
    ::InitializeCriticalSection(&m_cs);     
}

CIssIniEx::~CIssIniEx()
{
    Destroy();
    ::DeleteCriticalSection(&m_cs); 
}

/********************************************************************
Function        OpenFromResrouce

Arguments:      szResName       - Resource text file to open
                hInst           - HINSTANCE to retrieve

Returns:        BOOL - true if successful

Comments:       Opens a file from a resource and reads in the INI values
*********************************************************************/
HRESULT CIssIniEx::OpenFromResrouce(TCHAR* szResName, HINSTANCE hInst)
{
    char* szFile = NULL;

    HRESULT hr = S_OK;
    CBARG(szResName && hInst, _T("bad values passed into OpenFromResource"));

    // let's start over
    Destroy();
    ::EnterCriticalSection(&m_cs);  

    DWORD dwDisposition = OPEN_EXISTING;

    // Open the INI File for reading
    HRSRC hres = FindResource(hInst, szResName, _T("TXT"));
    CPHR(hres, _T("FindResource failed"));

    //If resource is found a handle to the resource is returned
    //now just load the resource
    HGLOBAL    hbytes = LoadResource(hInst, hres);
    CPHR(hbytes, _T("LoadResource failed"));

    // Lock the resource
    LPVOID pdata    = LockResource(hbytes);
    char*  strData  = (char*)pdata;
    CPHR(strData, _T("LockResource failed"));

    szFile = m_oStr->CreateAndCopyChar(strData);
    CPHR(szFile, _T("szFile not created"));

    //if we made it this far we're good
    m_bFileOpened = TRUE;

    ::LeaveCriticalSection(&m_cs);

    TCHAR szLine[STRING_MAX];
    TCHAR szLeft[STRING_MAX];
    TCHAR szRight[STRING_MAX];

    m_oStr->Empty(szLeft);
    m_oStr->Empty(szLine);
    m_oStr->Empty(szRight);

    char   szEndl[] = "\n";
    char*  szToken;
    int    iSubStringPos;

    // Read the First Line in the File
    szToken = m_oStr->StringToken(szFile, szEndl);

    //We'll use this to count the number of end line characters in a row
    //bug fix for PPC 2003
    //this should prevent us from exceeding our buffer
    int iNumOfBackNs=0;
    int j=0;

    while (szToken != NULL && iNumOfBackNs<2)
    {
        m_oStr->StringCopy(szLine, szToken);
        j++;

        // take off any spaces and/or return characters
        m_oStr->Trim(szLine);

        // this is used for the Values checking
        iSubStringPos = m_oStr->Find(szLine, _T("="));

        // if the Line's first Character is '[' and 
        // last is ']' then we got a new section
        if (_T('[') == m_oStr->GetFirst(szLine)  && _T(']') == m_oStr->GetLast(szLine))
        {
            TypeSection* sSection = new TypeSection;
            CPHR(sSection, _T("sSection failed to create"));

            // create a new string in the list
            sSection->szSection = new TCHAR[m_oStr->GetLength(szLine) - 1];
            CPHR(sSection->szSection, _T("sSection->szSection failed to create"));
            ZeroMemory(sSection->szSection, sizeof(TCHAR)*(m_oStr->GetLength(szLine) - 1));

            // get the new section name, remove the '[' and ']'
            m_oStr->Mid(sSection->szSection,
                szLine,
                1,
                m_oStr->GetLength(szLine) - 2);
            hr = m_arrSections.AddElement(sSection);
            CHR(hr, _T("m_arrSections.AddElement"));
        }
        // if this is a value
        else if (iSubStringPos > 0)
        {
            // Get the Left Side and the Right Side
            m_oStr->Left(szLeft, szLine, iSubStringPos);
            m_oStr->Mid(szRight, szLine, iSubStringPos + 1);
            m_oStr->Trim(szLeft);
            m_oStr->Trim(szRight);

            TypeSection* sSection = m_arrSections[m_arrSections.GetSize()-1];

            // add it to the structure for storage
            if(sSection)
            {
                TypeValue* sValue = new TypeValue;
                CPHR(sValue, _T("sValue failed to create"));

                sValue->szKeys          = m_oStr->CreateAndCopy(szLeft);
                sValue->szValues        = m_oStr->CreateAndCopy(szRight);
                CPHR(sValue->szKeys, _T("sValue->szKeys failed to create"));
                CPHR(sValue->szValues, _T("sValue->szValues failed to create"));

                hr = sSection->arrValues.AddElement(sValue);
                CHR(hr, _T("sSection->arrValues.AddElement"));
            }
        }

        // get the next line
        szToken = m_oStr->StringToken(NULL, szEndl);

        if(0==m_oStr->Compare(szToken, "\r"))
            iNumOfBackNs++;
        else 
            iNumOfBackNs=0;
    }

Error:

    // close the File
    if(m_hFile != NULL && m_hFile != INVALID_HANDLE_VALUE)
        ::CloseHandle(m_hFile);
    m_hFile = INVALID_HANDLE_VALUE;
    
    m_oStr->Delete(&szFile);

    return hr;
}

/********************************************************************
Function        Open

Arguments:      szFileName - INI File to read data from

Returns:        BOOL - true if OK

Comments:       This function will open an INI File and read all the data
into a structure which can be used for later use
*********************************************************************/
HRESULT CIssIniEx::Open(TCHAR *szFileName)
{
    HRESULT hr = S_OK;

    BOOL bUnicode = FALSE;//we'll read the header

    char* szFile = NULL;

    // Open the INI File for reading
    DWORD dwFileSize = GetFileSize(szFileName);

    CBARG(dwFileSize > 0, _T("szFileName invalid"));

    // let's start over
    Destroy();
    ::EnterCriticalSection(&m_cs);  

    DWORD dwDisposition = OPEN_EXISTING;

    m_hFile = ::CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
        dwDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

    CFHHR(m_hFile, _T("CreateFile failed"));
    m_bFileOpened = TRUE;

    DWORD   dwNumberOfBytesRead;

    TCHAR szLine[STRING_MAX];
    TCHAR szLeft[STRING_MAX];
    TCHAR szRight[STRING_MAX];

    m_oStr->Empty(szLeft);
    m_oStr->Empty(szLine);
    m_oStr->Empty(szRight);

    // Open the File for Reading
    szFile = new char[dwFileSize+1];
    ZeroMemory(szFile, sizeof(char)*(dwFileSize+1));
    
    CPHR(szFile, _T("szFile failed to create"));
    
    BOOL bRet = ::ReadFile(m_hFile,szFile, dwFileSize, &dwNumberOfBytesRead, NULL);
    CBHR(bRet, _T("ReadFile failed"));

    szFile[dwNumberOfBytesRead] = '\0'; 
    char  szEndl[] = "\n";
    TCHAR szUEndl[] = _T("\n");
    char* szToken;
    TCHAR* szUToken;
    int   iSubStringPos;

    //check for the unicode header
    //I'll do it properly later 
    if(szFile[0] == 'ÿ' && szFile[1] == 'þ')
    {
        bUnicode = TRUE;
        szUToken = m_oStr->StringToken((TCHAR*)szFile, szUEndl);
        szUToken++;//so I think this works because its 2 bytes which is 1 character? .. it works and if doesn't if this line is gone
        szToken = NULL;
    }
    else
    {
        // Read the First Line in the File
        szToken = m_oStr->StringToken(szFile, szEndl);
        szUToken = NULL;
    }
    




    //We'll use this to count the number of end line characters in a row
    //bug fix for PPC 2003
    //this should prevent us from exceeding our buffer
    int iNumOfBackNs=0;
    int j=0;  

    while ((szToken != NULL || szUToken) //little nasty but it's fine .. as long as one of them isn't null
        && iNumOfBackNs<2)
    {
        if(bUnicode)
            m_oStr->StringCopy(szLine, szUToken);
        else
            m_oStr->StringCopy(szLine, szToken);
        j++;

        // take off any spaces and/or return characters
        m_oStr->Trim(szLine);

        // this is used for the Values checking
        iSubStringPos = m_oStr->Find(szLine, _T("="));

        // if the Line's first Character is '[' and 
        // last is ']' then we got a new section
        if (_T('[') == m_oStr->GetFirst(szLine)  && _T(']') == m_oStr->GetLast(szLine))
        {

            TypeSection* sSection = new TypeSection;
            CPHR(sSection, _T("sSection failed to create"));

            // create a new string in the list
            sSection->szSection = new TCHAR[m_oStr->GetLength(szLine) - 1];
            CPHR(sSection->szSection, _T("sSection->szSection failed to create"));
            ZeroMemory(sSection->szSection, sizeof(TCHAR)*(m_oStr->GetLength(szLine) - 1));

            // get the new section name, remove the '[' and ']'
            m_oStr->Mid(sSection->szSection,
                szLine,
                1,
                m_oStr->GetLength(szLine) - 2);

            hr = m_arrSections.AddElement(sSection);
            CHR(hr, _T("m_arrSections.AddElement"));
        }
        // if this is a value
        else if (iSubStringPos > 0)
        {
            // Get the Left Side and the Right Side
            m_oStr->Left(szLeft, szLine, iSubStringPos);
            m_oStr->Mid(szRight, szLine, iSubStringPos + 1);
            m_oStr->Trim(szLeft);
            m_oStr->Trim(szRight);

            TypeSection* sSection = m_arrSections[m_arrSections.GetSize()-1];

            // add it to the structure for storage
            if(sSection)
            {
                TypeValue* sValue = new TypeValue;
                CPHR(sValue, _T("sValue failed to create"));

                sValue->szKeys          = m_oStr->CreateAndCopy(szLeft);
                sValue->szValues        = m_oStr->CreateAndCopy(szRight);
                CPHR(sValue->szKeys, _T("sValue->szKeys failed to create"));
                CPHR(sValue->szValues, _T("sValue->szValues failed to create"));

                hr = sSection->arrValues.AddElement(sValue);
                CHR(hr, _T("sSection->arrValues.AddElement"));
            }
        }

        // get the next line
        if(bUnicode)
        {
            szUToken = m_oStr->StringToken(NULL, szUEndl);

            if(0==m_oStr->Compare(szUToken, _T("\r")))
                iNumOfBackNs++;
            else 
                iNumOfBackNs=0;
        }
        else
        {
            szToken = m_oStr->StringToken(NULL, szEndl);

            if(0==m_oStr->Compare(szToken, "\r"))
                iNumOfBackNs++;
            else 
                iNumOfBackNs=0;
        }
    }

Error:
    m_bFileOpened = FALSE;
    ::LeaveCriticalSection(&m_cs);
    // close the File
    if(m_hFile != INVALID_HANDLE_VALUE)
        ::CloseHandle(m_hFile);
    m_hFile = INVALID_HANDLE_VALUE;

    m_oStr->Delete(&szFile);

    return hr;
}

/********************************************************************
Function        GetValue

Arguments:      pszDestination - copy value of the key
pszSection - Section of the INI
pszKey - Key to retrieve
pszDefaultValue - if nothing is found, use this default

Returns:        BOOL - true if Key is Found

Comments:       Function to retrieve a Key from the INI File.  Note: Open
function will need to be called first
*********************************************************************/
HRESULT CIssIniEx::GetValue(TCHAR *pszDestination, 
                         TCHAR *pszSection, 
                         TCHAR *pszKey, 
                         TCHAR *pszDefaultValue)
{
    HRESULT hr = S_OK;
    CBARG(pszDestination&&pszSection&&pszKey&&pszDefaultValue, _T("GetValue invalid arg"));

    if(!m_oStr->IsEmpty(pszDefaultValue))
        m_oStr->StringCopy(pszDestination, pszDefaultValue);

    // retrieve the section first
    int iSectionIndex = GetSectionIndex(pszSection);
    CBARG(-1 != iSectionIndex, _T("GetSectionIndex failed"));

    // retrieve the key index now
    int iValueIndex = GetKeyIndex(iSectionIndex, pszKey);
    CBARG(-1 != iValueIndex, _T("GetKeyIndex failed"));

    TypeSection* sSection = m_arrSections[iSectionIndex];
    CPHR(sSection, _T("sSection failed"));

    TypeValue* sValue = sSection->arrValues[iValueIndex];
    CBARG(sValue&&!m_oStr->IsEmpty(sValue->szValues), _T("sValue failed"));

    // We found something!!
    m_oStr->StringCopy(pszDestination, sValue->szValues);

Error:
//    if(hr == E_INVALIDARG)
//        ASSERT(0);
    return hr;
}

/********************************************************************
Function        GetValue

Arguments:      piDestination - copy value of the key
pszSection - Section of the INI
pszKey - Key to retrieve
iDefaultValue - if nothing is found, use this default

Returns:        BOOL - true if Key is Found

Comments:       Function to retrieve a Key from the INI File.  It has to
be an int.  Note: Open function will need to be called first
*********************************************************************/
HRESULT CIssIniEx::GetValue(int    *piDestination, 
                         TCHAR  *pszSection, 
                         TCHAR  *pszKey, 
                         int    iDefaultValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey, _T("GetValue arguments"));

    TCHAR   szTempDestination[STRING_MAX];
    TCHAR   szTempDefault[STRING_MAX];

    m_oStr->Empty(szTempDefault);
    m_oStr->Format(szTempDefault, _T("%d"), iDefaultValue);

    hr = GetValue(szTempDestination, 
        pszSection,
        pszKey,
        szTempDefault);
    CHR(hr, _T("GetValue failed"));

    // convert it back to integer
    *piDestination = m_oStr->StringToInt(szTempDestination);

Error:
    return hr;
}

/********************************************************************
Function        GetValue

Arguments:      pptDestination - copy value of the key
pszSection - Section of the INI
pszKey - Key to retrieve
ptDefaultValue - if nothing is found, use this default

Returns:        BOOL - true if Key is Found

Comments:       Function to retrieve a Key from the INI File.  It has to
be an int.  Note: Open function will need to be called first
*********************************************************************/
HRESULT CIssIniEx::GetValue(POINT  *pptDestination, 
                         TCHAR  *pszSection, 
                         TCHAR  *pszKey, 
                         POINT  ptDefaultValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey&&pptDestination, _T("GetValue arguments"));

    TCHAR*  szToken;
    TCHAR   szTempDestination[STRING_MAX];
    TCHAR   szTempDefault[STRING_MAX];

    m_oStr->Empty(szTempDefault);

    m_oStr->Format(szTempDefault, _T("%d,%d"), ptDefaultValue.x, ptDefaultValue.y);
    *pptDestination = ptDefaultValue;

    hr = GetValue(szTempDestination, 
        pszSection,
        pszKey,
        szTempDefault);
    CHR(hr, _T("GetValue failed"));

    // parse the string and place in the POINT structure
    szToken = m_oStr->StringToken(szTempDestination, _T(","));
    pptDestination->x       = m_oStr->StringToInt(szToken);
    szToken = m_oStr->StringToken(NULL, _T(","));
    pptDestination->y       = m_oStr->StringToInt(szToken);

Error:
    return hr;
}

/********************************************************************
Function        GetValue

Arguments:      sDestination - copy value of the key
pszSection - Section of the INI
pszKey - Key to retrieve
sDefaultValue - if nothing is found, use this default

Returns:        BOOL - true if Key is Found

Comments:       Function to retrieve a Key from the INI File.  It has to
be an SIZE struct.  Note: Open function will need to be called first
*********************************************************************/
HRESULT CIssIniEx::GetValue(SIZE   *psDestination, 
                         TCHAR  *pszSection, 
                         TCHAR  *pszKey, 
                         SIZE   sDefaultValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey&&psDestination, _T("GetValue arguments"));

    TCHAR*  szToken;
    TCHAR   szTempDestination[STRING_MAX];
    TCHAR   szTempDefault[STRING_MAX];

    m_oStr->Empty(szTempDefault);

    m_oStr->Format(szTempDefault, _T("%d,%d"), sDefaultValue.cx, sDefaultValue.cy);
    *psDestination = sDefaultValue;

    hr = GetValue(szTempDestination, 
        pszSection,
        pszKey,
        szTempDefault);
    CHR(hr, _T("GetValue failed"));

    // parse the string and place in the POINT structure
    szToken = m_oStr->StringToken(szTempDestination, _T(","));
    psDestination->cx       = m_oStr->StringToInt(szToken);
    szToken = m_oStr->StringToken(NULL, _T(","));
    psDestination->cy       = m_oStr->StringToInt(szToken);

Error:
    return hr;
}

/********************************************************************
Function        GetValue

Arguments:      pcrDestination - copy value of the key
pszSection - Section of the INI
pszKey - Key to retrieve
crDefaultValue - if nothing is found, use this default

Returns:        BOOL - true if Key is Found

Comments:       Function to retrieve a Key from the INI File.  It has to
be an COLORREF struct.  Note: Open function will need to be called first
*********************************************************************/
HRESULT CIssIniEx::GetValue(COLORREF       *pcrDestination, 
                         TCHAR          *pszSection, 
                         TCHAR          *pszKey, 
                         COLORREF       crDefaultValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey&&pcrDestination, _T("GetValue arguments"));

    TCHAR*  szToken;
    TCHAR   szTempDestination[STRING_MAX];
    TCHAR   szTempDefault[STRING_MAX];

    m_oStr->Empty(szTempDefault);

    m_oStr->Format(szTempDefault, _T("%d,%d,%d"), 
        GetRValue(crDefaultValue), 
        GetGValue(crDefaultValue),
        GetBValue(crDefaultValue));
    *pcrDestination = crDefaultValue;

    hr = GetValue(szTempDestination, 
        pszSection,
        pszKey,
        szTempDefault);
    CHR(hr, _T("GetValue failed"));

    int iR, iG, iB;
    // parse the string and place in the POINT structure
    szToken = m_oStr->StringToken(szTempDestination, _T(","));
    iR      = m_oStr->StringToInt(szToken);
    szToken = m_oStr->StringToken(NULL, _T(","));
    iG      = m_oStr->StringToInt(szToken);
    szToken = m_oStr->StringToken(NULL, _T(","));
    iB      = m_oStr->StringToInt(szToken);

    *pcrDestination = RGB(iR, iG, iB);

Error:
    return hr;
}

/********************************************************************
Function        GetValue

Arguments:      pdbDestination - copy value of the key
pszSection - Section of the INI
pszKey - Key to retrieve
dbDefaultValue - if nothing is found, use this default

Returns:        BOOL - true if Key is Found

Comments:       Function to retrieve a Key from the INI File.  It has to
be an double.  Note: Open function will need to be called first
*********************************************************************/
HRESULT CIssIniEx::GetValue(double*        pdbDestination,
                         TCHAR*         pszSection,
                         TCHAR*         pszKey,
                         double         dbDefaultValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey&&pdbDestination, _T("GetValue arguments"));

    TCHAR   szTempDestination[STRING_MAX];
    TCHAR   szTempDefault[STRING_MAX];

    m_oStr->Empty(szTempDefault);
    m_oStr->DoubleToString(szTempDefault, dbDefaultValue);
    *pdbDestination = dbDefaultValue;

    hr = GetValue(szTempDestination, 
        pszSection,
        pszKey,
        szTempDefault);
    CHR(hr, _T("GetValue failed"));

    *pdbDestination = m_oStr->StringToDouble(szTempDestination);

Error:
    return hr;
}

/********************************************************************
Function        GetKeyIndex

Arguments:      iSectionIndex - the Section Index in the Structure
pszKey - key to find



Returns:        int - returns index of in the structure of the key.
-1 if not found

Comments:       search through the structure and try and find the key in
the section
*********************************************************************/
int CIssIniEx::GetKeyIndex(int iSectionIndex, TCHAR* pszKey)
{
    // Validity Checking
    if (m_oStr->IsEmpty(pszKey))
        return -1;

    TypeSection* sSection = m_arrSections[iSectionIndex];
    if(!sSection)
        return -1;

    // try and find the key in the section list
    for(int i=0; i<sSection->arrValues.GetSize(); i++)
    {
        TypeValue* sValue = sSection->arrValues[i];
        if(sValue && 0 == m_oStr->CompareNoCase(sValue->szKeys, pszKey))
            return i;
    }

    return -1;
}

/********************************************************************
Function        GetSectionIndex

Arguments:      pszSection - Section to retrieve

Returns:        int - return Index of the section found.
-1 if not found.

Comments:       Search through the structure and try and find the 
matching section.  return the index if found
*********************************************************************/
int CIssIniEx::GetSectionIndex(TCHAR* pszSection)
{
    if (m_oStr->IsEmpty(pszSection))
        return -1;

    // try and find the section
    for (int i=0; i<m_arrSections.GetSize(); i++)
    {
        TypeSection* sSection = m_arrSections[i];
        if(sSection && 0 == m_oStr->CompareNoCase(sSection->szSection, pszSection))
            return i;
    }

    return -1;
}

/********************************************************************
Function        Destroy

Arguments:      

Returns:        void

Comments:       Destroys all used memory.  NOTE: can be called at any time
*********************************************************************/
void CIssIniEx::Destroy()
{
    // close the file
    if (INVALID_HANDLE_VALUE != m_hFile)
    {
        ::CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
    m_bFileOpened = FALSE;


    for(int i=0; i<m_arrSections.GetSize(); i++)
    {
        TypeSection* sSection = m_arrSections[i];
        if(sSection)
            delete sSection;
    }
    m_arrSections.RemoveAll();
}


HRESULT CIssIniEx::SaveUnicode(TCHAR* pszFileName)
{
    HRESULT hr = S_OK;

    CBARG(pszFileName, _T("Save arguments"));

    FILE*   fp          = NULL;

    //open the file
    if((fp = _tfopen(pszFileName, _T("wb"))) == NULL)
    {
        //the file was not able to be open
        return E_INVALIDARG;
    }

    //unicode header first
    char szBOM[2];
    szBOM[0]=(char)0xFF;
    szBOM[1]=(char)0xFE;
    //insert header
    fwrite(szBOM, sizeof(char), 2, fp);



    TCHAR    szLine[STRING_MAX];
    TCHAR    szTemp1[STRING_MAX];
    TCHAR    szTemp2[STRING_MAX];
    int     i,j;

    // now write all the contents to the file
    for(i=0; i<m_arrSections.GetSize(); i++)
    {
        TypeSection* sSection = m_arrSections[i];
        if(!sSection)
            continue;

        // add the section
        m_oStr->StringCopy(szTemp1, sSection->szSection);
        m_oStr->Format(szLine, _T("[%s]\r\n"), szTemp1);

        fwrite(szLine, sizeof(TCHAR), m_oStr->GetLength(szLine), fp);
        //fwprintf(fp, szLine);

        for(j=0; j<sSection->arrValues.GetSize(); j++)
        {
            TypeValue* sValue = sSection->arrValues[j];
            if(!sValue)
                continue;

            // add the attributes
            m_oStr->StringCopy(szTemp1, sValue->szKeys);
            m_oStr->StringCopy(szTemp2, sValue->szValues);
            m_oStr->Format(szLine, _T("%s=%s\r\n"), szTemp1, szTemp2);
  
            fwrite(szLine, sizeof(TCHAR), m_oStr->GetLength(szLine), fp);
        }

        // put a line in between sections
        fwrite(_T("\r\n"), sizeof(TCHAR), 1, fp);
    }

Error:
    // close the file
    if(fp)
        fclose(fp);

    return hr;
}



HRESULT CIssIniEx::SaveUTF8(TCHAR* pszFileName)
{
	HRESULT hr = S_OK;

	CBARG(pszFileName, _T("Save arguments"));

	FILE*   fp          = NULL;

	//open the file
	if((fp = _tfopen(pszFileName, _T("wb"))) == NULL)
	{
		//the file was not able to be open
		return E_INVALIDARG;
	}

	//UTF8 header first
	char szBOM[3];
	szBOM[0]=(char)0xEF;
	szBOM[1]=(char)0xBB;
	szBOM[2]=(char)0xBF;
	//insert header
	fwrite(szBOM, sizeof(char), 3, fp);


	char	cszTemp[STRING_MAX*2];

	TCHAR    szLine[STRING_MAX];
	TCHAR    szTemp1[STRING_MAX];
	TCHAR    szTemp2[STRING_MAX];
	int     i,j;

	int		iUTFLen = 0;

	// now write all the contents to the file
	for(i=0; i<m_arrSections.GetSize(); i++)
	{
		TypeSection* sSection = m_arrSections[i];
		if(!sSection)
			continue;

		// add the section
		m_oStr->StringCopy(szTemp1, sSection->szSection);
		m_oStr->Format(szLine, _T("[%s]\r\n"), szTemp1);


		iUTFLen = WideCharToMultiByte( CP_UTF8,       // code page
			0,               // performance and mapping flags
			szLine,// wide char buffer
			m_oStr->GetLength(szLine),          // chars in wide char buffer
			cszTemp,          // resultant ascii string
			STRING_MAX*2,          // size of ascii string buffer
			NULL,            // char to sub. for unmapped chars
			NULL);  // flag to set if default char used	

		fwrite(cszTemp, sizeof(char), iUTFLen, fp);


		for(j=0; j<sSection->arrValues.GetSize(); j++)
		{
			TypeValue* sValue = sSection->arrValues[j];
			if(!sValue)
				continue;

			// add the attributes
			m_oStr->StringCopy(szTemp1, sValue->szKeys);
			m_oStr->StringCopy(szTemp2, sValue->szValues);
			m_oStr->Format(szLine, _T("%s=%s\r\n"), szTemp1, szTemp2);

			iUTFLen = WideCharToMultiByte( CP_UTF8,       // code page
				0,               // performance and mapping flags
				szLine,// wide char buffer
				m_oStr->GetLength(szLine),          // chars in wide char buffer
				cszTemp,          // resultant ascii string
				STRING_MAX*2,          // size of ascii string buffer
				NULL,            // char to sub. for unmapped chars
				NULL);  // flag to set if default char used	

			fwrite(cszTemp, sizeof(char), iUTFLen, fp);
		}

		// put a line in between sections
		m_oStr->StringCopy(szLine, _T("\r\n"));

		iUTFLen = WideCharToMultiByte( CP_UTF8,       // code page
			0,               // performance and mapping flags
			szLine,// wide char buffer
			m_oStr->GetLength(szLine),          // chars in wide char buffer
			cszTemp,          // resultant ascii string
			STRING_MAX*2,          // size of ascii string buffer
			NULL,            // char to sub. for unmapped chars
			NULL);  // flag to set if default char used	

		fwrite(cszTemp, sizeof(char), iUTFLen, fp);
	}

Error:
	// close the file
	if(fp)
		fclose(fp);

	return hr;
}


/********************************************************************
Function        Save

Arguments:      pszFileName

Returns:        HRESULT - S_OK if OK

Comments:       Function to Save the Settings to a file
*********************************************************************/
HRESULT CIssIniEx::Save(TCHAR *pszFileName)
{
    HRESULT hr = S_OK;

    CBARG(pszFileName, _T("Save arguments"));

    FILE* fp = NULL;

	DeleteFile(pszFileName);
    fp = _tfopen(pszFileName, _T("w"));
	CPHR(fp, _T("fp = _tfopen(szTemp, _T(\"w\"));"));

    //char    szWriteBuffer[10000];
    TCHAR    szLine[STRING_MAX];
    TCHAR    szTemp1[STRING_MAX];
    TCHAR    szTemp2[STRING_MAX];
    int     i,j;

    // now write all the contents to the file
    for(i=0; i<m_arrSections.GetSize(); i++)
    {
        TypeSection* sSection = m_arrSections[i];
        if(!sSection)
            continue;

        // add the section
        m_oStr->StringCopy(szTemp1, sSection->szSection);
        m_oStr->Format(szLine, _T("[%s]\r\n"), szTemp1);
       // m_oStr->Concatenate(szWriteBuffer, szLine);

		//fwrite(szLine, sizeof(TCHAR), m_oStr->GetLength(szLine), fp);

	    fwprintf(fp, szLine);

        for(j=0; j<sSection->arrValues.GetSize(); j++)
        {
            TypeValue* sValue = sSection->arrValues[j];
            if(!sValue)
                continue;

            // add the attributes
            m_oStr->StringCopy(szTemp1, sValue->szKeys);
            m_oStr->StringCopy(szTemp2, sValue->szValues);
            m_oStr->Format(szLine, _T("%s=%s\r\n"), szTemp1, szTemp2);
            //m_oStr->Concatenate(szWriteBuffer, szLine);

			//fwrite(szLine, sizeof(TCHAR), m_oStr->GetLength(szLine), fp);
    	    fwprintf(fp, szLine);
        }

        // put a line in between sections
        //m_oStr->Concatenate(szWriteBuffer, "\r\n");

        fwprintf(fp, _T("\r\n"));
    }

Error:
    // close the file
    if(fp)
		fclose(fp);

    return hr;
}

/********************************************************************
Function        AddSection

Arguments:      pszSection - Section to Add

Returns:        BOOL - TRUE if OK

Comments:       will add a section to the list, will check for duplicates
*********************************************************************/
HRESULT CIssIniEx::AddSection(TCHAR* pszSection)
{
    HRESULT hr = S_OK;
    CBARG(pszSection, _T("AddSection arguments"));
    CBARG(-1 == GetSectionIndex(pszSection), _T("Section already found"));

    // create the new section
    TypeSection* sSection = new TypeSection;
    CPHR(sSection, _T("sSection not created"));
    sSection->szSection     = m_oStr->CreateAndCopy(pszSection);
    CPHR(sSection->szSection, _T("sSection->szSection not created"));

    hr = m_arrSections.AddElement(sSection);
    CHR(hr, _T("m_arrSections.AddElement"));
Error:
    return hr;
}

/********************************************************************
Function        AddAttribute

Arguments:      pszSection - Section
pszKey     - Key
pszValue   - Value

Returns:        BOOL - TRUE if OK

Comments:       Will Add an Attribute to the section
*********************************************************************/
HRESULT CIssIniEx::AddAttribute(TCHAR*     pszSection,
                             TCHAR* pszKey,
                             TCHAR* pszValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey&&pszValue, _T("AddAttribute arguments"));

    int iSection    = GetSectionIndex(pszSection);
    CBARG(-1 != iSection, _T("GetSectionIndex failed"));

    // check if the key also exists
    CBARG(-1 == GetKeyIndex(iSection, pszKey), _T("Key already exists"));

    TypeSection* sSection = m_arrSections[iSection];
    CPHR(sSection, _T("sSection failed"));

    // insert the new Attribute
    TypeValue* sValue = new TypeValue;
    CPHR(sValue, _T("sValue failed"));
    sValue->szKeys  = m_oStr->CreateAndCopy(pszKey);
    CPHR(sValue->szKeys, _T("sValue->szKeys failed"));
    sValue->szValues= m_oStr->CreateAndCopy(pszValue);
    CPHR(sValue->szValues, _T("sValue->szValues failed"));
    hr = sSection->arrValues.AddElement(sValue);
Error:
    return hr;
}


/********************************************************************
Function        SetValue

Arguments:      pszSection - Section
pszKey     - Key
pszValue   - Value

Returns:        BOOL - TRUE if OK

Comments:       Will set the string value to the ini class
*********************************************************************/
HRESULT CIssIniEx::SetValue(TCHAR* pszSection,
                         TCHAR* pszKey,
                         TCHAR* pszValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey&&pszValue, _T("SetValue arguments"));

    int iSection    = GetSectionIndex(pszSection);
    CBARG(-1 != iSection, _T("GetSectionIndex failes"));

    // check if the key also exists
    int iKeyIndex = GetKeyIndex(iSection, pszKey);
    CBARG(-1 != iKeyIndex, _T("GetKeyIndex failed"));

    TypeSection* sSection = m_arrSections[iSection];
    CPHR(sSection, _T("sSection faile to create"));

    TypeValue* sValue = sSection->arrValues[iKeyIndex];
    CPHR(sValue, _T("sValue"));

    // insert the new Attribute
    m_oStr->StringCopy(sValue->szKeys, pszKey);
    m_oStr->StringCopy(sValue->szValues, pszValue);

Error:
    return hr;
}

/********************************************************************
Function        SetValue

Arguments:      pszSection - Section
pszKey     - Key
iValue   - Value

Returns:        BOOL - TRUE if OK

Comments:       Will set the integer to the ini class
*********************************************************************/
HRESULT CIssIniEx::SetValue(TCHAR* pszSection,
                         TCHAR* pszKey,
                         int    iValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey, _T("SetValue arguments"));

    TCHAR szValue[STRING_LARGE];

    m_oStr->IntToString(szValue, iValue);

    hr = SetValue(pszSection, pszKey, szValue);
Error:
    return hr;
}

/********************************************************************
Function        SetValue

Arguments:      pszSection - Section
pszKey     - Key
crValue   - Value

Returns:        BOOL - TRUE if OK

Comments:       Will set the integer to the ini class
*********************************************************************/
HRESULT CIssIniEx::SetValue(TCHAR*         pszSection,
                         TCHAR*         pszKey,
                         COLORREF       crValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey, _T("SetValue arguments"));

    TCHAR szValue[STRING_LARGE];

    m_oStr->Format(szValue, _T("%d,%d,%d"), 
        GetRValue(crValue), 
        GetGValue(crValue),
        GetBValue(crValue));


    hr = SetValue(pszSection, pszKey, szValue);
Error:
    return hr;
}

/********************************************************************
Function        SetValue

Arguments:      pszSection - Section
pszKey     - Key
crValue   - Value

Returns:        BOOL - TRUE if OK

Comments:       Will set the integer to the ini class
*********************************************************************/
HRESULT CIssIniEx::SetValue(TCHAR*         pszSection,
                         TCHAR*         pszKey,
                         SIZE           sValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey, _T("SetValue arguments"));

    TCHAR szValue[STRING_LARGE];

    m_oStr->Format(szValue, _T("%d,%d"), 
        sValue.cx, sValue.cy);

    hr = SetValue(pszSection, pszKey, szValue);
Error:
    return hr;
}

/********************************************************************
Function        SetValue

Arguments:      pszSection - Section
pszKey     - Key
crValue   - Value

Returns:        BOOL - TRUE if OK

Comments:       Will set the integer to the ini class
*********************************************************************/
HRESULT CIssIniEx::SetValue(TCHAR*         pszSection,
                         TCHAR*         pszKey,
                         POINT          ptValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey, _T("SetValue arguments"));

    TCHAR szValue[STRING_LARGE];

    m_oStr->Format(szValue, _T("%d,%d"), 
        ptValue.x, ptValue.y);


    hr = SetValue(pszSection, pszKey, szValue);

Error:
    return hr;
}

/********************************************************************
Function        SetValue

Arguments:      pszSection - Section
pszKey     - Key
crValue   - Value

Returns:        BOOL - TRUE if OK

Comments:       Will set the integer to the ini class
*********************************************************************/
HRESULT CIssIniEx::SetValue(TCHAR*         pszSection,
                         TCHAR*         pszKey,
                         double         dbValue)
{
    HRESULT hr = S_OK;
    CBARG(pszSection&&pszKey, _T("SetValue arguments"));

    TCHAR szValue[STRING_LARGE];

    m_oStr->DoubleToString(szValue, dbValue);       

    hr = SetValue(pszSection, pszKey, szValue);
Error:
    return hr;
}


/********************************************************************
Function        AddAttribute

Arguments:      pszSection - Section
pszKey     - Key
pszValue   - Value

Returns:        BOOL - TRUE if OK

Comments:       Will Add an Attribute to the section
*********************************************************************/
HRESULT CIssIniEx::AddAttribute(TCHAR*     pszSection,
                             TCHAR* pszKey,
                             double dbValue)
{
    TCHAR szValue[STRING_LARGE];
    m_oStr->DoubleToString(szValue, dbValue);

    return AddAttribute(pszSection, pszKey, szValue);
    
}
