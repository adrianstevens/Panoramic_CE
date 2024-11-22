/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: iSSString.cpp 
// Abstract: Common string functions  
// 
/***************************************************************************************************************/ 
#include "IssString.h"
#include "IssDebug.h"

CIssString* CIssString::m_instance  = NULL;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIssString::CIssString()
:m_szText1(NULL)
{
    m_hInstance = NULL;
}

CIssString::~CIssString()
{
	Delete(&m_szText1);
}

/********************************************************************
    Function:    Instance

    Arguments:                

    Returns:    CIssString* - used to pass only one instance of the class 
                               so everyone will share the same instance

    Comments:    Function used to pass back one global instance of this class
*********************************************************************/
CIssString* CIssString::Instance()
{
    if (m_instance == NULL)
    {
        m_instance  = new CIssString();
    }
    return m_instance;
}

/********************************************************************
    Function:    DeleteInstance

    Arguments:    
    
    Returns:    void

    Comments:    Function used to delete the global instance of this class
*********************************************************************/
void CIssString::DeleteInstance()
{
    if (m_instance != NULL)
    {
        delete m_instance;
        m_instance = NULL;
    }
}

/********************************************************************
Function:    Capitalize

Arguments:    pszSource (TCHAR* buffer)

Returns:    void

Comments:    Converts all characters in a TCHAR array to upper case
*********************************************************************/
HRESULT CIssString::Capitalize(TCHAR* pszSource)
{
    if(IsEmpty(pszSource))
        return E_INVALIDARG;

    for(int i=0; i<GetLength(pszSource); i++)
    {
        // if it's in the lower case letter range we'll have to offset to the uppercase range
        if(pszSource[i] > 96 && pszSource[i] < 123)
            pszSource[i] -= 32; 
    }

    return S_OK;
}

HRESULT CIssString::Lowercase(TCHAR* pszSource)
{
    if(IsEmpty(pszSource))
        return E_INVALIDARG;

    for(int i=0; i<GetLength(pszSource); i++)
    {
        if(isalpha(pszSource[i]))
            pszSource[i] = _tolower(pszSource[i]); 
    }

    return S_OK;
}


/********************************************************************
    Function    TrimLeft

    Arguments:    pszSource - Source String to trim

    Returns:    void

    Comments:    Function to Trim leading White spaces and Return 
                Characters
*********************************************************************/
HRESULT CIssString::TrimLeft(TCHAR * pszSource)
{
    TCHAR * cp = pszSource;

    if (cp && *cp)
    {
        // find first non-whitespace character
        while (_istspace(*cp))
            cp++;

        if (cp != pszSource)
            memcpy(pszSource, cp, (_tcslen(cp)+1)*sizeof(TCHAR));
    }
    else
        return E_INVALIDARG;

    return S_OK;
}

/********************************************************************
    Function    GetStringInBuffer

    Comments:   Given a buffer with string data in it, start & end strings to search for,
                returns the string contained between the start & end strings.

    Arguments:  pszBuffer - Buffer of string data to search.
                pszStart  - Start string to search for.
                pszEnd    - End string to search for.
                pszString - [out]Stores the string here.
                dwLen     - Max size of pszString.

    Returns:    TCHAR* - Pointer to new location in pszBuffer (after the detected start data), or the
                        same value as pszBuffer if the string was not found. NULL if there was
                        an error (pszBuffer == NULL).
*********************************************************************/
TCHAR* CIssString::GetStringInBuffer(TCHAR* pszBuffer, 
                                     TCHAR* pszStart, 
                                     TCHAR* pszEnd, 
                                     TCHAR** pszString, 
                                     DWORD dwLen
                                     )
{
    Empty(*pszString);
    
    if(IsEmpty(pszBuffer) || IsEmpty(pszStart) || IsEmpty(pszEnd) || pszString == NULL || dwLen == 0)
    {
        return NULL;
    }

    TCHAR* szStart = NULL;
    int iStart = FindNoCase(pszBuffer, pszStart);
    
    if(iStart >= 0)
    {
        iStart += GetLength(pszStart);
        szStart = &pszBuffer[iStart];
    }
    else
        return NULL;
    
    // start looking for the end tag AFTER the beginning tag.
    int iEnd = FindNoCase(szStart, pszEnd);    
    if(iEnd < 0)
        return NULL;

    // iEnd right now is relative to the start position.  we want iEnd to be relative to the beginning of the buffer,
    // so lets find out where it should really be...
    iEnd = iStart + iEnd;

    if(dwLen == -1 && *pszString == NULL)
        *pszString = CreateAndCopy(pszBuffer, iStart, iEnd - iStart);
    else
    {
        if((DWORD)(iEnd - iStart) > dwLen)
        {
            return NULL;
        }
        StringCopy(*pszString, pszBuffer, iStart, iEnd - iStart);
    }
    
    int iLen    = GetLength(pszBuffer);
    int iPosEnd    = iEnd + GetLength(pszEnd);
    if(iLen > iPosEnd)
        return &pszBuffer[iEnd + GetLength(pszEnd)];
    else if(iLen == iPosEnd )
        return NULL;
    else
    {
        return NULL;
    }
}

/********************************************************************
    Function    TrimLeft

    Arguments:    pszSource - Source String to trim

    Returns:    void

    Comments:    Function to Trim trailing White spaces and Return 
                Characters
*********************************************************************/
HRESULT CIssString::TrimRight(TCHAR * pszSource)
{
    TCHAR * cp = pszSource;

    if (cp && *cp)
    {
        BOOL bNonSpaceSeen = FALSE;

        // check if string is blank
        while (*cp)
        {
            if (!_istspace(*cp))
                bNonSpaceSeen = TRUE;
            cp++;
        }

        if (bNonSpaceSeen)
        {
            cp--;

            // find last non-whitespace character
            while ((cp >= pszSource) && (_istspace(*cp)))
                *cp-- = _T('\0');
        }
        else
        {
            // string contains only whitespace characters
            *pszSource = _T('\0');
        }
    }
    else
        return E_INVALIDARG;

    return S_OK;
}

/********************************************************************
    Function    TrimLeft

    Arguments:    pszSource - Source String to trim

    Returns:    void

    Comments:    Function to Trim leading and trailing White spaces and Return 
                Characters
*********************************************************************/
HRESULT CIssString::Trim(TCHAR *pszSource)
{
    HRESULT hr = TrimRight(pszSource);
    if(hr == S_OK)
        hr = TrimLeft(pszSource);

    return hr;
}

/********************************************************************
    Function    StringCopy

    Arguments:    pszDestination - copy to
                pszSource - copy from            

    Returns:    BOOL - true if OK

    Comments:    Copy one string to the Other
*********************************************************************/
HRESULT CIssString::StringCopy(TCHAR *pszDestination, 
                            TCHAR *pszSource)
{
    HRESULT hr = S_OK;
    CBARG(pszDestination&&pszSource, _T(""));

    TCHAR* sz = _tcscpy(pszDestination, pszSource);
    CPHR(sz, _T(""));

Error:
    return hr;
}

/********************************************************************
    Function    StringCopy

    Arguments:    pszDestination - copy to
                pszSource - copy from            
                iDestSize - Size of the destination buffer - prevents buffer overruns

    Returns:    BOOL - true if OK

    Comments:    Copy one string to the Other
*********************************************************************/
HRESULT CIssString::StringCopy(TCHAR *pszDestination, 
                            TCHAR *pszSource, 
                            int iDestSize
                            )
{
    HRESULT hr = S_OK;
    CBARG(pszDestination&&pszSource, _T(""));


    CBARG(GetLength(pszSource)+1 <= iDestSize, _T(""));

    TCHAR* sz = _tcscpy(pszDestination, pszSource);
    CPHR(sz, _T(""));

Error:
    return hr;
}

/********************************************************************
    Function    StringCopy

    Arguments:    pszDestination - string to write to
                uiSource - string table index
                iMaxBuffer - Max String buffer size

    Comments:    copy to a string from the string table
*********************************************************************/
HRESULT CIssString::StringCopy(TCHAR* pszDestination, 
                            UINT uiSource, 
                            int iMaxBuffer
                            )
{
    if(NULL == pszDestination ||
       NULL == m_hInstance ||
       iMaxBuffer <= 0)
       return FALSE;

    return (HRESULT)LoadString(m_hInstance, uiSource, pszDestination, iMaxBuffer);
}

/********************************************************************
    Function    StringCopy

    Arguments:    pszDestination - string to write to
                uiSource - string table index
                iMaxBuffer - Max String buffer size

    Comments:    copy to a string from the string table
*********************************************************************/
HRESULT CIssString::StringCopy(TCHAR* pszDestination, 
                            UINT uiSource, 
                            int iMaxBuffer, 
                            HINSTANCE hInst
                            )
{
    if(NULL == pszDestination ||
        NULL == hInst ||
        iMaxBuffer <= 0)
       return FALSE;

    return LoadString(hInst, uiSource, pszDestination, iMaxBuffer);

}

/********************************************************************
    Function    StringCopy

    Arguments:    pszDestination - copy to
                pszSource - copy from
                iStart - Starting position
                iCount - How many chars to copy over

    Returns:    BOOL - true if OK

    Comments:    Copy one string to the other but only a certain set of characters
*********************************************************************/
HRESULT CIssString::StringCopy(CHAR* pszDestination, 
                            CHAR* pszSource, 
                            int iStart, 
                            int iCount
                            )
{
    HRESULT hr = S_OK;

    if(NULL == pszDestination ||
       NULL == pszSource ||
       iStart < 0 ||
       iCount < 0 ||
       GetLength(pszSource) < 1)
    {
        hr = E_INVALIDARG;
        goto Error;
    }

    // validity checking
    if(iStart + iCount > GetLength(pszSource))
    {
        hr = E_INVALIDARG;
        goto Error;
    }

    // clear out the destination string first
    Empty(pszDestination);

    char* sz = strncpy(pszDestination, pszSource + iStart, iCount);
    CPHR(sz, _T("strncpy"));

    // set the last value with \0
    pszDestination[iCount] = '\0';

Error:
    return hr;
}

/********************************************************************
    Function    StringCopy

    Arguments:    pszDestination - copy to
                pszSource - copy from    
                iDestSize - Size of destination buffer - prevents over runs.

    Returns:    BOOL - true if OK

    Comments:    Copy one string to the Other.  This will also convert
                the multi byte char to the wide char at the same time
*********************************************************************/
HRESULT CIssString::StringCopy(TCHAR *pszDestination, 
                            char *pszSource, 
                            int iDestSize
                            )
{
    if(NULL == pszDestination ||
       NULL == pszSource)
       return FALSE;

    if(GetLength(pszSource) + 1 > iDestSize)
    {
        return FALSE;
    }

    // initialize the destination string
    wcscpy(pszDestination, _T(""));

    // now convert the char to TCHAR
    mbstowcs(pszDestination, pszSource, GetLength(pszSource));

    // make sure we have a leading \0 at the end
    pszDestination[GetLength(pszSource)] = _T('\0');
    
    return TRUE;
}

/********************************************************************
    Function    StringCopy

    Arguments:    pszDestination - copy to
                pszSource - copy from                

    Returns:    BOOL - true if OK

    Comments:    Copy one string to the Other.  This will also convert
                the multi byte char to the wide char at the same time
*********************************************************************/
HRESULT CIssString::StringCopy(TCHAR *pszDestination, 
                            char *pszSource
                            )
{
    if(NULL == pszDestination ||
       NULL == pszSource)
       return E_INVALIDARG;

    // initialize the destination string
    wcscpy(pszDestination, _T(""));

    // now convert the char to TCHAR
    mbstowcs(pszDestination, pszSource, GetLength(pszSource));

    // make sure we have a leading \0 at the end
    pszDestination[GetLength(pszSource)] = _T('\0');
    
    return S_OK;
}

/********************************************************************
Function    StringCopy

Arguments:    pszDestination - copy to
pszSource - copy from                

Returns:    BOOL - true if OK

Comments:    Copy one string to the other (CHAR to CHAR)
*********************************************************************/
HRESULT CIssString::StringCopy(CHAR* pszDestination, 
                            CHAR* pszSource, 
                            int iDestSize
                            )
{
    if(pszDestination && pszSource)
    {
        if(GetLength(pszSource)+1 <= iDestSize)
        {
            return StringCopy(pszDestination, pszSource);
        }
    }
    return E_INVALIDARG;
}

/********************************************************************
Function    StringCopy

Arguments:    pszDestination - copy to
pszSource - copy from                

Returns:    BOOL - true if OK

Comments:    Copy one string to the other (CHAR to CHAR)
*********************************************************************/
HRESULT CIssString::StringCopy(char* pszDestination, 
                            char* pszSource
                            )
{
    HRESULT hr = S_OK;
    CBARG(pszDestination && pszSource, _T(""));

    char* sz = strcpy(pszDestination, pszSource);
    CPHR(sz, _T(""));

Error:
    return hr;
}

/********************************************************************
Function    Concatenate

Arguments:    szSource: Source string buffer to work from (TCHAR)
            szAddStrin: String buffer to add to end of szSource (CHAR)

Returns:    BOOL - TRUE if succeeded

Comments:    Add a string to the end of buffer (make sure you have a large enough szSource first)
*********************************************************************/
BOOL CIssString::Concatenate(TCHAR* szSource, 
                             char* szAddString
                             )
{
    TCHAR* szTemp = NULL;
    HRESULT hr = S_OK;

    if( NULL == szSource ||
        NULL == szAddString)
    {
        return E_INVALIDARG;
        goto Error;
    }

    szTemp = new TCHAR[GetLength(szAddString)+1];
    CPHR(szTemp, _T("Concatenate Error"));

    hr = StringCopy(szTemp, szAddString);
    CHR(hr, _T("Concat Error"));

    Concatenate(szSource,szTemp);

Error:
    SAFE_DELETE_ARRAY(szTemp);
    if(hr == S_OK)
        return TRUE;
    return FALSE;
}

/********************************************************************
Function    Concatenate

Arguments:    szSource: Source string buffer to work from (char)
szAddStrin: String buffer to add to end of szSource (TCHAR)

Returns:    BOOL - TRUE if succeeded

Comments:    Add a string to the end of buffer (make sure you have a large enough szSource first)
*********************************************************************/
BOOL CIssString::Concatenate(char* szSource, 
                             TCHAR* szAddString
                             )
{
    HRESULT hr = S_OK;
    CHAR* szTemp = NULL;
    BOOL bRet = TRUE;

    if( NULL == szSource ||
        NULL == szAddString)
    {
        hr =  E_INVALIDARG;
        goto Error;
    }
    
    szTemp = new CHAR[GetLength(szAddString)+1];
    CPHR(szTemp, _T("Concatenate Error"));

    hr = StringCopy(szTemp, szAddString);
    CHR(hr, _T("Concatenate Error"));

    bRet = Concatenate(szSource,szTemp);

Error:
    SAFE_DELETE_ARRAY(szTemp)
    if(hr == S_OK && bRet == TRUE)
        return TRUE;
    return FALSE;
}

/********************************************************************
    Function    StringCopy

    Arguments:    pszDestination - copy to
                pszSource - copy from        
                iDestSize - Size of destination buffer - prevents over runs

    Returns:    BOOL - true if OK

    Comments:    Copy one string to the Other.  This will also convert
                the multi byte char to the wide char at the same time
*********************************************************************/
HRESULT CIssString::StringCopy(char *pszDestination, 
                            TCHAR *pszSource, 
                            int iDestSize
                            )
{
    if(NULL == pszDestination ||
       NULL == pszSource)
       return E_INVALIDARG;

    if(GetLength(pszSource) + 1 > iDestSize)
    {
        return E_INVALIDARG;
    }

    // initialize the destination string
    Empty(pszDestination);

    // now convert the char to TCHAR
    wcstombs(pszDestination, pszSource, GetLength(pszSource));

    // make sure we have a leading \0 at the end
    pszDestination[GetLength(pszSource)] = '\0';
    
    return S_OK;
}


/********************************************************************
    Function    StringCopy

    Arguments:    pszDestination - copy to
                pszSource - copy from                

    Returns:    BOOL - true if OK

    Comments:    Copy one string to the Other.  This will also convert
                the multi byte char to the wide char at the same time
*********************************************************************/
HRESULT CIssString::StringCopy(char *pszDestination, 
                            TCHAR *pszSource
                            )
{
    HRESULT hr = S_OK;

    if(NULL == pszDestination ||
       NULL == pszSource)
    {
        hr = E_INVALIDARG;
        goto Error;
    }

    // initialize the destination string
    Empty(pszDestination);

    // now convert the char to TCHAR
    wcstombs(pszDestination, pszSource, GetLength(pszSource));

    // make sure we have a leading \0 at the end
    pszDestination[GetLength(pszSource)] = '\0';
    
Error:
    return hr;
}

/********************************************************************
    Function    GetLength

    Arguments:    pszSource - string
                
    Returns:    int - 0 if it can't read the string properly

    Comments:    This function will return the length of the provided string
*********************************************************************/
int CIssString::GetLength(char *pszSource)
{
    if (NULL == pszSource )
        return 0;

    return (int)strlen(pszSource);
}

/********************************************************************
    Function    IsEmpty

    Arguments:    pszSource - string to check

    Comments:    Check if this string is empty
*********************************************************************/
BOOL CIssString::IsEmpty(char* pszSource)
{
    if(pszSource == NULL)
        return TRUE;

    if(GetLength(pszSource) > 0)
        return FALSE;
    else
        return TRUE;
}

BOOL CIssString::Concatenate(TCHAR* pszSource, char cAddValue)
{
    char szVal[2];
    szVal[1] = '\0';
    szVal[0] = cAddValue;
    return Concatenate(pszSource, szVal);
}

BOOL CIssString::Concatenate(TCHAR* pszSource, TCHAR cAddValue)
{
    TCHAR szVal[2];
    szVal[1] = _T('\0');
    szVal[0] = cAddValue;
    return Concatenate(pszSource, szVal);
}

BOOL CIssString::Concatenate(char* pszSource, char cAddValue)
{
    char szVal[2];
    szVal[1] = '\0';
    szVal[0] = cAddValue;
    return Concatenate(pszSource, szVal);
}

BOOL CIssString::Concatenate(char* pszSource, TCHAR cAddValue)
{
    TCHAR szVal[2];
    szVal[1] = _T('\0');
    szVal[0] = cAddValue;
    return Concatenate(pszSource, szVal);
}

/********************************************************************
    Function    Concatenate

    Arguments:    pszSource    - source string
                pszAddString - string to concatenate with

    Returns:    BOOL        - TRUE if ok

    Comments:    add one string to another
*********************************************************************/
BOOL CIssString::Concatenate(char* pszSource,
                             char* pszAddString
                             )
{
    if(NULL == pszSource ||
       NULL == pszAddString)
       return FALSE;

    if(strcat(pszSource, pszAddString) > 0)
        return TRUE;
    else 
        return FALSE;//probably means your buffers are too small
}

/********************************************************************
    Function    Empty

    Arguments:    pszSource    - source string

    Returns:    BOOL        - TRUE if ok

    Comments:    Blank out the string
*********************************************************************/
HRESULT CIssString::Empty(char *pszSource)
{
    if(NULL == pszSource)
        return E_INVALIDARG;

    return StringCopy(pszSource, "");
}

/********************************************************************
    Function    StringToken

    Arguments:    pszSource    - source string to parse through
                pszToken    - token to search for

    Returns:    char*    - passing back the substring

    Comments:    parse through a string looking for tokens and giving back
                the substring as a pointer
*********************************************************************/
char* CIssString::StringToken(char *pszSource, 
                              char *pszToken
                              )
{
    // Note: pszSource can be NULL
    if(NULL == pszToken)
        return NULL;
    
    return strtok(pszSource, pszToken);
}

/********************************************************************
    Function    Find

    Arguments:    pszSource - source string
                pszStrToFind - sub string to find
                nStart - Starting position in source string to find substring                

    Returns:    int - -1 if it can't find anything

    Comments:    Find a subString in the String
*********************************************************************/
int CIssString::Find(char *pszStrToSearch, 
                     TCHAR* pszStrToFind
                     )
{
    if(NULL == pszStrToSearch ||
        NULL == pszStrToFind)
        return -1;
    
    
    char* szStrToFindChar = new char[GetLength(pszStrToFind)+1];
    if(szStrToFindChar == NULL)
    {
        return -1;
    }
    StringCopy(szStrToFindChar, pszStrToFind);

    int iReturn = Find(pszStrToSearch, szStrToFindChar);

    Delete(&szStrToFindChar);

    return iReturn;
}

/********************************************************************
    Function    StringToDouble

    Arguments:    pszSource - string to convert to double

    Returns:    double - converted double. -1 if unsuccessful

    Comments:    Convert a string to an double value
*********************************************************************/
double CIssString::StringToDouble(char *pszSource)
{
    if(NULL == pszSource)
        return -1;

    char *stopstring; 
    return strtod(pszSource,&stopstring);
}


/********************************************************************
    Function    Compare

    Arguments:    pszStr1 - string 1
                pszStr2 - string 2                

    Returns:    int -    -1 if unable to compare string
                        0 if they are equal
                        < 0 if pstr2 is larger then pstr1
                        > 0 if pstr1 is larger then pstr2

    Comments:    This function will compare the two strings provided.            
*********************************************************************/
int CIssString::Compare(char *pszStr1, 
                        char *pszStr2
                        )
{
    if (NULL == pszStr1 ||
        NULL == pszStr2)
        return -1;

    return strcmp(pszStr1, pszStr2);
}

/********************************************************************
    Function    Compare

    Arguments:    pszStr1 - string 1
                pszStr2 - string 2                

    Returns:    int -    -1 if unable to compare string
                        0 if they are equal
                        < 0 if pstr2 is larger then pstr1
                        > 0 if pstr1 is larger then pstr2

    Comments:    This function will compare the two strings provided.            
*********************************************************************/
int CIssString::Compare(TCHAR *pszStr1, 
                        char *pszStr2
                        )
{
    if (NULL == pszStr1 ||
        NULL == pszStr2)
        return -1;

    CHAR* szStr1 = new CHAR[GetLength(pszStr1) + 1];
    if(szStr1 == NULL)
    {
        return -1;
    }
    CHAR* szStr2 = new CHAR[GetLength(pszStr2) + 1];
    if(szStr2 == NULL)
    {
        delete [] szStr1;
        return -1;
    }

    StringCopy(szStr1, pszStr1);
    StringCopy(szStr2, pszStr2);

    int iResult = Compare(szStr1,szStr2);
    
    delete [] szStr1;
    szStr1 = NULL;

    delete [] szStr2;
    szStr2 = NULL;

    return iResult;
}


/********************************************************************
    Function    FindLastOccurance

    Arguments:    pszStrToSearch - source string to search
                pszFindString - sub string to find

    Returns:    int - -1 if it can't find anything

    Comments:    Find the last occurrence of a string in a string
*********************************************************************/
int CIssString::FindLastOccurance(TCHAR* pszStrToSearch, 
                                  char* pszStrToFind
                                  )
{
    if(!pszStrToSearch || !pszStrToFind)
    {
        return -1;
    }

    // convert to char and use char version of this function
    char* pszStrToSearchChar = new char[GetLength(pszStrToSearch) + 1];
    if(pszStrToSearchChar == NULL)
    {
        return -1;
    }

    StringCopy(pszStrToSearchChar, pszStrToSearch);

    int iReturn = FindLastOccurance(pszStrToSearchChar, pszStrToFind);

    Delete(&pszStrToSearchChar);

    return iReturn;
}

/********************************************************************
    Function    FindLastOccurance

    Arguments:    pszStrToSearch - source string to search
                pszFindString - sub string to find

    Returns:    int - -1 if it can't find anything

    Comments:    Find the last occurrence of a string in a string
*********************************************************************/
int CIssString::FindLastOccurance(char* pszStrToSearch, 
                                  TCHAR* pszStrToFind
                                  )
{
    if(!pszStrToSearch || !pszStrToFind)
    {
        return -1;
    }
    
    // convert to char and use char version of this function
    char* pszStrToFindChar = new char[GetLength(pszStrToFind) + 1];
    if(pszStrToFindChar == NULL)
    {
        return -1;
    }
    
    StringCopy(pszStrToFindChar, pszStrToFind);
    
    int iReturn = FindLastOccurance(pszStrToSearch, pszStrToFindChar);
    
    Delete(&pszStrToFindChar);
    
    return iReturn;
}


/********************************************************************
    Function    FindLastOccurance

    Arguments:    pszStrToSearch - source string to search
                pszFindString - sub string to find

    Returns:    int - -1 if it can't find anything

    Comments:    Find the last occurrence of a string in a string
*********************************************************************/
int    CIssString::FindLastOccurance(TCHAR* pszStrToSearch, 
                                     TCHAR* pszStrToFind
                                     )
{
    if(!pszStrToSearch || !pszStrToFind)
    {
        return -1;
    }

    int iCharToFind = pszStrToFind[0];

    TCHAR* pszReturn = _tcsrchr(pszStrToSearch, iCharToFind);

    if(!pszReturn)
    {
        return -1;
    }

    int iReturn = int(pszReturn - pszStrToSearch);

    // At this point we have found the start of the string we are searching for (may 
    // not be the whole string...just the first char)

    // Now we have to see if the string we are looking at actually matches the string we are 
    // trying to find.
    int iFindStrPos = Find(pszReturn, pszStrToFind);

    if(iFindStrPos != -1)
    {
        return iReturn;
    }
    else
    {
        return -1;
    }

    /*if(!pszStrToSearch || !pszStrToFind)
    {
        return -1;
    }

    //convert to char, then use the char version of this function.
    char* pszStrToSearchChar    = new char[GetLength(pszStrToSearch) + 1];
    if(pszStrToSearchChar == NULL)
    {
        return -1;
    }
    char* pszStrToFindChar        = new char[GetLength(pszStrToFind) + 1];
    if(pszStrToFindChar == NULL)
    {
        Delete(&pszStrToSearchChar);
        return -1;
    }

    StringCopy(pszStrToSearchChar, pszStrToSearch);
    StringCopy(pszStrToFindChar, pszStrToFind);

    int iReturn = FindLastOccurance(pszStrToSearchChar, pszStrToFindChar);

    Delete(&pszStrToFindChar);
    Delete(&pszStrToSearchChar);

    return iReturn;*/
}


/********************************************************************
    Function    FindLastOccurance

    Arguments:    pszStrToSearch - source string to search
                pszFindString - sub string to find

    Returns:    int - -1 if it can't find anything

    Comments:    Find the last occurrence of a string in a string
*********************************************************************/
int CIssString::FindLastOccurance(CHAR* pszStrToSearch, 
                                  CHAR* pszStrToFind
                                  )
{
    if(!pszStrToSearch || !pszStrToFind)
    {
        return -1;
    }

    int iCharToFind = pszStrToFind[0];

    char* pszReturn = strrchr(pszStrToSearch, iCharToFind);

    if(!pszReturn)
    {
        return -1;
    }

    int iReturn = int(pszReturn - pszStrToSearch);

    // At this point we have found the start of the string we are searching for (may 
    // not be the whole string...just the first char)

    // Now we have to see if the string we are looking at actually matches the string we are 
    // trying to find.
    int iFindStrPos = Find(pszReturn, pszStrToFind);

    if(iFindStrPos != -1)
    {
        return iReturn;
    }
    else
    {
        return -1;
    }
}

/********************************************************************
    Function    Compare

    Arguments:    pszStr1 - string 1
                pszStr2 - string 2                

    Returns:    int -    -1 if unable to compare string
                        0 if they are equal
                        < 0 if pstr2 is larger then pstr1
                        > 0 if pstr1 is larger then pstr2

    Comments:    This function will compare the two strings provided.            
*********************************************************************/
int CIssString::Compare(CHAR *pszStr1, 
                        TCHAR* pszStr2
                        )
{
    if (NULL == pszStr1 ||
        NULL == pszStr2)
        return -1;
    
    CHAR* szStr1 = new CHAR[GetLength(pszStr1) + 1];
    if(szStr1 == NULL)
    {
        return -1;
    }
    CHAR* szStr2 = new CHAR[GetLength(pszStr2) + 1];
    if(szStr2 == NULL)
    {
        delete [] szStr1;
        return -1;
    }
    
    StringCopy(szStr1, pszStr1);
    StringCopy(szStr2, pszStr2);
    
    int iResult = Compare(szStr1,szStr2);
    
    delete [] szStr1;
    szStr1 = NULL;
    
    delete [] szStr2;
    szStr2 = NULL;
    
    return iResult;
}

/********************************************************************
    Function    Find

    Arguments:    pszSource - source string
                pszFindString - sub string to find
                nStart - Starting position in source string to find substring                

    Returns:    int - -1 if it can't find anything

    Comments:    Find a subString in the String
*********************************************************************/
int CIssString::Find(char *pszSource, 
                     char *pszFindString, 
                     int nStart
                     )
{
    if(NULL == pszSource ||
       NULL == pszFindString ||
       nStart < 0)
        return -1;

    if (nStart > GetLength(pszSource))
        return -1;

    // find first matching substring
    char *lpsz = strstr(pszSource + nStart, pszFindString);

    // return -1 for not found, distance from beginning otherwise
    return (lpsz == NULL) ? -1 : (int)(lpsz - pszSource);
}

/********************************************************************
    Function    Find

    Arguments:    pszSource - source string
                pszFindString - sub string to find
                nStart - Starting position in source string to find substring                

    Returns:    int - -1 if it can't find anything

    Comments:    Find a subString in the String
*********************************************************************/
int CIssString::FindNoCase(CHAR* pszSource, 
                           TCHAR* pszFindString, 
                           int iStart
                           )
{
    TCHAR* szSource = CreateAndCopy(pszSource);
    int iReturn = FindNoCase(szSource, pszFindString, iStart);

    Delete(&szSource);

    return iReturn;
}

/********************************************************************
    Function    Find

    Arguments:    pszSource - source string
                pszFindString - sub string to find
                nStart - Starting position in source string to find substring                

    Returns:    int - -1 if it can't find anything

    Comments:    Find a subString in the String
*********************************************************************/
int CIssString::FindNoCase(CHAR* pszSource, 
                           CHAR* pszFindString, 
                           int iStart
                           )
{
    if(pszSource == NULL || pszFindString == NULL)
        return -1;

    if(IsEmpty(pszSource) || IsEmpty(pszFindString))
        return -1;
    
    int iLenSrc = GetLength(pszSource)+1;
    char* szCharSrc = new CHAR[iLenSrc];
    if(szCharSrc == NULL)
    {
        return -1;
    }
    StringCopy(szCharSrc, pszSource);
    
    int i = 0;
    while( i < iLenSrc && szCharSrc[i])
    {
        szCharSrc[i] = toupper(szCharSrc[i]);
        
        // slide along 1 char.
        i++;
    }
    
    // no capitalize the string we are finding.
    int iFindLen = GetLength(pszFindString)+1;

    char* szCharFind = new CHAR[iFindLen];
    if(szCharFind == NULL)
    {
        delete [] szCharSrc;
        return -1;
    }
    StringCopy(szCharFind, pszFindString);
    
    
    i = 0;
    while(iFindLen > i && szCharFind[i])
    {
        szCharFind[i] = toupper(szCharFind[i]);
        
        // slide along 1 char.
        i++;
    }
    
    int iReturn = Find(szCharSrc, szCharFind, iStart);
    
    delete [] szCharFind;
    szCharFind = NULL;
    delete [] szCharSrc;
    szCharSrc = NULL;
    
    return iReturn;
    
}

/********************************************************************
    Function    Find

    Arguments:    pszSource - source string
                pszFindString - sub string to find
                nStart - Starting position in source string to find substring                

    Returns:    int - -1 if it can't find anything

    Comments:    Find a subString in the String
*********************************************************************/
int CIssString::FindNoCase(TCHAR* pszSource, 
                           TCHAR* pszFindString, 
                           int iStart
                           )
{
    if(pszSource == NULL || pszFindString == NULL)
        return -1;

    if(IsEmpty(pszSource) || IsEmpty(pszFindString))
        return -1;

    int iSrcLen = GetLength(pszSource)+1;
    

    CHAR* szCharSrc = new CHAR[iSrcLen];
    if(szCharSrc == NULL)
    {
        return -1;
    }
    StringCopy(szCharSrc, pszSource);

    int i = 0;
    while(i < iSrcLen && szCharSrc[i])
    {
        szCharSrc[i] = toupper(szCharSrc[i]);

        // slide along 1 char.
        i++;
    }

    // now capitalize the string we are finding.
    int iFindLen = GetLength(pszFindString)+1;
    char* szCharFind = new CHAR[iFindLen];
    if(szCharFind == NULL)
    {
        delete [] szCharSrc;
        return -1;
    }
    StringCopy(szCharFind, pszFindString);

    i = 0;
    while(szCharFind[i] && iFindLen > i)
    {
        szCharFind[i] = toupper(szCharFind[i]);
        
        // slide along 1 char.
        i++;
    }

    int iReturn = Find(szCharSrc, szCharFind, iStart);

    delete [] szCharFind;
    szCharFind = NULL;
    delete [] szCharSrc;
    szCharSrc = NULL;

    return iReturn;

}
/********************************************************************
    Function    Find

    Arguments:    pszSource - source string
                pszFindString - sub string to find

    Returns:    int - -1 if it can't find anything

    Comments:    Find a subString in the String
*********************************************************************/
int CIssString::Find(char *pszSource, 
                     char *pszFindString
                     )
{
    return Find(pszSource, pszFindString, 0);
}

/********************************************************************
    Function    StringCopy

    Arguments:    pszDestination - copy to
                pszSource - copy from
                iStart - Starting position
                iCount - How many chars to copy over

    Returns:    BOOL - true if OK

    Comments:    Copy one string to the other but only a certain set of characters
*********************************************************************/
HRESULT CIssString::StringCopy(TCHAR* pszDestination, 
                            CHAR* pszSource, 
                            int iStart, 
                            int iCount
                            )
{
    if(!pszSource || !pszDestination || iStart < 0 || iCount < 0)
    {    
        return FALSE;
    }

    TCHAR* szSource = CreateAndCopy(pszSource);

    HRESULT hr = StringCopy(pszDestination, szSource, iStart, iCount);

    Delete(&szSource);

    return hr;
}

/********************************************************************
    Function    StringCopy

    Arguments:    pszDestination - copy to
                pszSource - copy from
                iStart - Starting position
                iCount - How many chars to copy over

    Returns:    BOOL - true if OK

    Comments:    Copy one string to the other but only a certain set of characters
*********************************************************************/
HRESULT CIssString::StringCopy(TCHAR* pszDestination, 
                            TCHAR* pszSource, 
                            int iStart, 
                            int iCount
                            )
{
    //gotta declare this first 
    TCHAR* szTempSource = NULL;

    HRESULT hr = S_OK;
    BOOL bEval = TRUE;
    if(pszDestination == NULL ||
        pszSource == NULL ||
        iStart < 0 ||
        iCount < 0)
        bEval = FALSE;
    CBARG(bEval, _T(""));
 //   CBARG(pszDestination && pszSource && iStart >= 0 && iCount > 0, _T(""));

    szTempSource = new TCHAR[GetLength(pszSource)+1];
    CPHR(szTempSource, _T(""));

    _tcscpy(szTempSource, pszSource);
    
    // validity checking
    CBARG(iStart + iCount <= GetLength(pszSource), _T(""));

    // clear out the destination string first
    _tcscpy(pszDestination, _T(""));

    TCHAR* sz = _tcsncpy(pszDestination, szTempSource + iStart, iCount);
    CPHR(sz, _T("_tcsncpy"));

    // set the last value with \0
    pszDestination[iCount] = _T('\0');

Error:
    if(szTempSource)
        delete [] szTempSource;

#ifdef DEBUG
    if(hr != S_OK)
    {
        int i=0;
    }
#endif
    return hr;
}

/********************************************************************
    Function    GetLength

    Arguments:    pszSource - string
                
    Returns:    int - 0 if it can't read the string properly

    Comments:    This function will return the length of the provided string
*********************************************************************/
int CIssString::GetLength(TCHAR *pszSource)
{
    if (NULL == pszSource  )
        return 0;

    return (int)_tcslen(pszSource);
}

/********************************************************************
    Function    CompareNoCase

    Arguments:    pszStr1 - string 1
                pszStr2 - string 2                

    Returns:    int -    -1 if unable to compare string
                        0 if they are equal
                        < 0 if pstr2 is larger then pstr1
                        > 0 if pstr1 is larger then pstr2

    Comments:    This function will compare the two strings provided. It
                doesn't matter if it is capitalized or not though
*********************************************************************/
int CIssString::CompareNoCase(TCHAR *pszStr1, 
                              TCHAR *pszStr2
                              )
{
    if (NULL == pszStr1 ||
        NULL == pszStr2)
        return -1;

    return _wcsicmp(pszStr1, pszStr2);
//    return _stricmp(pszStr1, pszStr2);//desktop?
}

/********************************************************************
    Function    GetFirst

    Arguments:    pszSource - string
                
    Returns:    TCHAR - First Character in the String

    Comments:    This function will return the first character in the string
*********************************************************************/
TCHAR CIssString::GetFirst(TCHAR *pszSource)
{
    if(NULL == pszSource)
        return 0;

    return pszSource[0];
}

/********************************************************************
    Function    GetLast

    Arguments:    pszSource - string
                
    Returns:    TCHAR - Last Character in the String

    Comments:    This function will return the last character in the string
*********************************************************************/
TCHAR CIssString::GetLast(TCHAR *pszSource)
{
    if(NULL == pszSource)
        return 0;
    

    return pszSource[GetLength(pszSource) - 1];
}

/********************************************************************
    Function    Find

    Arguments:    pszSource - source string
                pszFindString - sub string to find

    Returns:    int - -1 if it can't find anything

    Comments:    Find a subString in the String
*********************************************************************/
int CIssString::Find(TCHAR *pszSource, 
                     TCHAR *pszFindString
                     )
{
    return Find(pszSource, pszFindString, 0);
}

/********************************************************************
Function    FindOccurances

Arguments:    pszSource - source string (CHAR)
pszFindString - sub string to find (CHAR)

Returns:    int - -1 if it can't find anything

Comments:    counts returns the number of occurances of a string in a buffer
*********************************************************************/
int    CIssString::FindOccurances(CHAR* pszSource, 
                                  CHAR* pszFindString
                                  )
{
    if(NULL == pszSource ||
        NULL == pszFindString)
    {
        return -1;
    }

    int iOccurances = 0;
    char* pch = NULL;

    pch=strstr(pszSource,pszFindString);

    while (pch!=NULL)
    {
        pch=strstr(pch+1,pszFindString);
        iOccurances++;
    }

    if(iOccurances == 0)
        iOccurances--;

    return iOccurances;
}

/********************************************************************
    Function    Find

    Arguments:    pszSource - source string
                pszFindString - sub string to find
                nStart - Starting position in source string to find substring                

    Returns:    int - -1 if it can't find anything

    Comments:    Find a subString in the String
*********************************************************************/
int CIssString::Find(TCHAR *pszSource, 
                     TCHAR *pszFindString, 
                     int nStart
                     )
{
    if(NULL == pszSource ||
       NULL == pszFindString ||
       nStart < 0)
        return -1;
    
    if (nStart > GetLength(pszSource))
        return -1;

    // find first matching substring
    TCHAR *lpsz = _tcsstr(pszSource + nStart, pszFindString);

    // return -1 for not found, distance from beginning otherwise
    return (lpsz == NULL) ? -1 : (int)(lpsz - pszSource);
}

/********************************************************************
    Function    Mid

    Arguments:    pszDestination - copy to
                pszSource - copy from
                nFirst - start position to copy from

    Returns:    BOOL - true if OK

    Comments:    copy a substring within the source string
*********************************************************************/
BOOL CIssString::Mid(TCHAR *pszDestination,
                     TCHAR *pszSource, 
                     int nFirst
                     )
{
    if(nFirst < 0)
    {
        return FALSE;
    }

    return Mid(pszDestination, pszSource, nFirst, GetLength(pszSource) - nFirst);
}

/********************************************************************
    Function    Mid

    Arguments:    pszDestination - copy to
                pszSource - copy from
                nFirst - start position to copy from
                nCount - how many chars to copy

    Returns:    BOOL - true if OK

    Comments:    copy a substring within the source string
*********************************************************************/
BOOL CIssString::Mid(TCHAR *pszDestination, 
                     TCHAR *pszSource, 
                     int nFirst, 
                     int nCount
                     )
{
    // validity checking
    if(NULL == pszDestination ||
       NULL == pszSource)
       return FALSE;

    int iStrlen = GetLength(pszSource);

    // out-of-bounds requests return sensible things
    if (nFirst < 0)
        nFirst = 0;
    if (nCount < 0)
        nCount = 0;

    if (nFirst + nCount > iStrlen)
        nCount = iStrlen - nFirst;
    if (nFirst > iStrlen)
        nCount = 0;

#ifdef UNDER_CE
    ASSERT(nFirst >= 0);
    ASSERT(nFirst + nCount <= iStrlen);
#endif

    // optimize case of returning entire string
    if (nFirst == 0 && nFirst + nCount == iStrlen)
        return StringCopy(pszDestination, pszSource);
    else
        return StringCopy(pszDestination, pszSource, nFirst, nCount);
}

/********************************************************************
    Function    Right

    Arguments:    pszDestination - copy to
                pszSource - copy from
                nCount - how many chars starting from the right                

    Returns:    BOOL - true if OK

    Comments:    Copy a substring from the source string of nCount Characters
                starting from the right
*********************************************************************/
BOOL CIssString::Right(TCHAR *pszDestination, 
                       TCHAR *pszSource, 
                       int nCount
                       )
{
    if (NULL == pszDestination ||
        NULL == pszSource)
        return FALSE;

    if (nCount < 0)
        nCount = 0;

    int iStrLen = GetLength(pszSource);

    HRESULT hr = S_OK;
    if (nCount >= iStrLen)
        hr = StringCopy(pszDestination, pszSource);
    else
        hr = StringCopy(pszDestination, pszSource, iStrLen - nCount, nCount);

    if(SUCCEEDED(hr))
        return TRUE;
    else
        return FALSE;

}

/********************************************************************
    Function    Left

    Arguments:    pszDestination - copy to
                pszSource - copy from
                nCount - how many chars starting from the left            

    Returns:    BOOL - true if OK

    Comments:    Copy a substring from the source string of nCount Characters
                starting from the left
*********************************************************************/
BOOL CIssString::Left(TCHAR *pszDestination, 
                      TCHAR *pszSource, 
                      int nCount
                      )
{
    if (NULL == pszDestination ||
        NULL == pszSource)
        return FALSE;

    if (nCount < 0)
        nCount = 0;

    int iStrLen = GetLength(pszSource);

    HRESULT hr = S_OK;
    if (nCount >= iStrLen)
        hr =  StringCopy(pszDestination, pszSource);
    else
        hr =  StringCopy(pszDestination, pszSource, 0, nCount);

    if(SUCCEEDED(hr))
        return TRUE;
    else
        return FALSE;

}

/********************************************************************
    Function    Compare

    Arguments:    pszStr1 - string 1
                pszStr2 - string 2                

    Returns:    int -    -1 if unable to compare string
                        0 if they are equal
                        < 0 if pstr2 is larger then pstr1
                        > 0 if pstr1 is larger then pstr2

    Comments:    This function will compare the two strings provided.            
*********************************************************************/
int CIssString::Compare(TCHAR *pszStr1, 
                        TCHAR *pszStr2
                        )
{
    if (NULL == pszStr1 ||
        NULL == pszStr2)
        return -1;

    return _tcscmp(pszStr1, pszStr2);
}

/********************************************************************
    Function    Format

    Arguments:    pszDestination - copy to
                pszFormat - format of the String (eg. _T("%d") )
                ... - argument list                

    Returns:    BOOL - true if OK

    Comments:    create a formated string
*********************************************************************/
HRESULT CIssString::Format(CHAR *pszDestination, 
                        CHAR *pszFormat, ...)
{
    if(NULL == pszDestination ||
       NULL == pszFormat)
        return E_INVALIDARG;

    // Note: Might need to do more for this....
    va_list argList;
    va_start(argList, pszFormat);
#ifdef UNDER_CE
    int iRet = vsprintf(pszDestination, pszFormat, argList);
    //int iRet = _vtprintf(pszDestination, pszFormat, argList);
#else
    int iRet = 0;
//    int iRet = vsprintf(pszDestination, pszFormat, argList);
    //int iRet = _vtprintf(pszDestination, pszFormat, argList);
    //int iRet = swprintf(pszDestination, pszFormat, argList);
#endif
    va_end(argList);

    if(-1 == iRet)
        return E_INVALIDARG;
    else
        return S_OK;
}

/********************************************************************
    Function    Format

    Arguments:    pszDestination - copy to
                pszFormat - format of the String (eg. _T("%d") )
                ... - argument list                

    Returns:    BOOL - true if OK

    Comments:    create a formated string
*********************************************************************/
HRESULT CIssString::Format(TCHAR *pszDestination,
                        TCHAR *pszFormat, ...)
{
    if(NULL == pszDestination ||
       NULL == pszFormat)
        return E_INVALIDARG;

    Empty(pszDestination);

    // Note: Might need to do more for this....
    va_list argList;
    va_start(argList, pszFormat);
//#ifdef UNDER_CE
    int iRet = _vstprintf(pszDestination, pszFormat, argList);
    //int iRet = _vtprintf(pszDestination, pszFormat, argList);
//#else
//    int iRet = 0;
    //int iRet = swprintf(pszDestination, pszFormat, argList);
//#endif
    va_end(argList);

    if(-1 == iRet)
        return S_OK;
    else
        return E_INVALIDARG;
}

/********************************************************************
    Function    StringToInt

    Arguments:    pszSource - string to convert to int

    Returns:    int - converted int. -1 if unsuccessful

    Comments:    Convert a string to an integer
*********************************************************************/
int CIssString::StringToInt(TCHAR *pszSource)
{
    if(NULL == pszSource)
        return -1;

    return _ttoi(pszSource);
}

/********************************************************************
    Function    StringToDouble

    Arguments:    pszSource - string to convert to double

    Returns:    double - converted double. -1 if unsuccessful

    Comments:    Convert a string to an double value
*********************************************************************/
double CIssString::StringToDouble(TCHAR *pszSource)
{
    if(NULL == pszSource)
        return -1;

    TCHAR *stopstring; 
    return _tcstod(pszSource,&stopstring);
}


/********************************************************************
    Function    DoubleToString

    Arguments:    pszDestination - copy the double into this string
                dbSource       - double value to convert to string
                iSigFigs       - Significant figures desired

    Returns:    BOOL    - true if OK

    Comments:    Convert a double value to string....only use exponents
                if absolutely necessary
*********************************************************************/
HRESULT CIssString::DoubleToString(TCHAR *pszDestination, 
                                double dbSource, 
                                int iSigFigs
                                )
{
    HRESULT hr = S_OK;

    if(NULL == pszDestination ||
        iSigFigs < 0)
    {
        hr = E_INVALIDARG;
        goto Error;
    }

    // convert the double to a char first
    char     szMyChar[STRING_MAX];
    TCHAR    szTExp[255]; //TCHAR for the exponent
    TCHAR    szTemp[STRING_MAX];

    BOOL bNeg = FALSE;
    BOOL bRet = TRUE;

    if(dbSource < 0)
    {
        bNeg=TRUE;
        dbSource*=-1;
    }

    _gcvt(dbSource, iSigFigs, szMyChar);

    // now convert it to the TCHAR
    StringCopy(pszDestination, szMyChar);

    if(dbSource<0.1 &&
        dbSource!=0 &&
        Find(pszDestination, _T("e"))!=-1)
    //correct the _gvct conversion...there WILL be a negative exponent
    {
        int iExp;
        int iLength = GetLength(pszDestination)-1;
        
        //Copy exponent into its own TCHAR
        hr = StringCopy(szTExp, pszDestination,iLength-2, 3);
        CHR(hr, _T("Error - double to string"));

        //Convert to int
        iExp = StringToInt(szTExp);
        //check to see if we need to edit the szTChar
        if(iExp+iLength<iSigFigs+2)
        {
            //Remove period
            Delete(1,1,pszDestination);
            //remove exponent
            hr = StringCopy(szTemp, pszDestination, 0, iLength-5);
            CHR(hr, _T("Error - double to string"));
            hr = StringCopy(pszDestination, _T("0."));
            CHR(hr, _T("Error - double to string"));
            for (int i=0; i< iExp-1; i++)
            {
                _tcscat(pszDestination, _T("0"));
            }
            _tcscat(pszDestination, szTemp);
            
        }
    }
    else
    {
        //lets get rid of any trailing periods
        int iLength = GetLength(pszDestination);
        int iIndex  = Find(pszDestination, _T("."), iLength-1);
        if(iIndex==iLength-1)
        {
            bRet = Left(pszDestination, pszDestination, iLength-1);
            CBHR(bRet, _T("Error - double to string"));
        }
    }
    if(bNeg)
    {
        hr = Insert(pszDestination, _T("-"), 0);
        CHR(hr, _T("Error - double to string"));
    }

Error:
    return hr;
}



/********************************************************************
    Function    Delete

    Arguments:    int iIndex...where to start deleting
                int nCount...how many characters to delete
                TCHAR szString....TCHAR to edit

    Comments:    Deletes nCount number of characters from szString starting at iIndex
*********************************************************************/
void CIssString::Delete(int iIndex, 
                        int iCount, 
                        CHAR *szString
                        )
{
    if(!szString || iCount <= 0)
    {
        return;
    }

    TCHAR* szStringCpy = new TCHAR[GetLength(szString)+1];
    if(szStringCpy == NULL)
    {
        return;
    }
    StringCopy(szStringCpy, szString);

    Delete(iIndex, iCount, szStringCpy);
    StringCopy(szString, szStringCpy);
    Delete(&szStringCpy);
    
}

/********************************************************************
    Function    Delete

    Arguments:    int iIndex...where to start deleting
                int nCount...how many characters to delete
                TCHAR szString....TCHAR to edit

    Comments:    Deletes nCount number of characters from szString starting at iIndex
*********************************************************************/
void CIssString::Delete(int iIndex, 
                        int iCount, 
                        TCHAR *szString
                        )
{
    if(!szString || iCount <= 0)
        return;

    int iLength=GetLength(szString);

    TCHAR* szTempStart = new TCHAR[iLength + 1];
    if(szTempStart == NULL)
    {
        return;
    }
    TCHAR* szTempEnd = new TCHAR[iLength + 1];
    if(szTempEnd == NULL)
    {
        delete [] szTempStart;
        return;
    }

    if (iIndex < 0)
        iIndex = 0;

    if(iCount>iLength-iIndex)
        iCount=iLength-iIndex;

    //Get the parts
    Left(szTempStart, szString, iIndex);
    Right(szTempEnd, szString, iLength-iIndex-iCount);

    //put em back together
    _tcscpy(szString, szTempStart);
    _tcscat(szString, szTempEnd);

    delete [] szTempEnd;
    szTempEnd = NULL;

    delete [] szTempStart;
    szTempStart = NULL;

}

/********************************************************************
    Function    StringToken

    Arguments:    pszSource    - source string to parse through
                pszToken    - token to search for

    Returns:    TCHAR*    - passing back the substring

    Comments:    parse through a string looking for tokens and giving back
                the substring as a pointer
*********************************************************************/
TCHAR* CIssString::StringToken(TCHAR *pszSource, 
                               TCHAR *pszToken
                               )
{
    // Note: pszSource can be NULL
    if(NULL == pszToken)
       return NULL;

    return _tcstok(pszSource, pszToken);
}

/********************************************************************
    Function    Empty

    Arguments:    pszSource    - source string

    Returns:    BOOL        - TRUE if ok

    Comments:    Blank out the string
*********************************************************************/
HRESULT CIssString::Empty(TCHAR *pszSource)
{
    if(NULL == pszSource)
        return E_INVALIDARG;

    _tcscpy(pszSource, _T(""));

    return S_OK;
}

/********************************************************************
    Function    Concatenate

    Arguments:    pszSource    - source string
                pszAddString - string to concatenate with

    Returns:    BOOL        - TRUE if ok

    Comments:    add one string to another
*********************************************************************/
BOOL CIssString::Concatenate(TCHAR* pszSource,
                             TCHAR* pszAddString)
{
    if(NULL == pszSource ||
       NULL == pszAddString)
       return FALSE;

    if(_tcscat(pszSource, pszAddString))
        return  TRUE;
    else 
        return FALSE;
}

/********************************************************************
Function    Insert

Arguments:    pszSource        - source string
            pszAddString    - string to insert
            iIndex            - where to insert into the string

Comments:    Insert a string into another
*********************************************************************/
HRESULT CIssString::Insert(CHAR *pszSource, 
                           CHAR *pszAddString, 
                           int iIndex
                          )
{
    HRESULT hr = S_OK;

    if(pszSource == NULL || pszAddString == NULL || iIndex < 0)
    {
        hr = E_INVALIDARG;
        CHR(hr, _T("Insert arguments are invalid"));
    }

    int iLen = GetLength(pszSource)+ GetLength(pszAddString) + 1;
    CBARG(iLen > 1, _T(""));

    // src is ultimately pszSource length plus the string to add...so make sure we have enough room.
    TCHAR* pszTcharSource = new TCHAR[iLen];
    CPHR(pszTcharSource, _T("Insert - out of memory"));
    ZeroMemory(pszTcharSource, iLen*sizeof(TCHAR));

    hr = StringCopy(pszTcharSource, pszSource);
    CHR(hr, _T("Insert Error"));

    iLen = GetLength(pszAddString)+1;
    TCHAR* pszTcharAddString= new TCHAR[iLen];
    CPHR(pszTcharAddString, _T("Insert - out of memory"));
    ZeroMemory(pszTcharAddString, iLen*sizeof(TCHAR));

    hr = StringCopy(pszTcharAddString, pszAddString);
    CHR(hr, _T("Insert Error"));

    hr = Insert(pszTcharSource, pszTcharAddString, iIndex);
    CHR(hr, _T("Insert Error"));
    
    hr = StringCopy(pszSource, pszTcharSource);
    CHR(hr, _T("Insert Error"));

    //hr = StringCopy(pszAddString, pszTcharAddString);
    //CHR(hr, _T("Insert Error"));

Error:
    Delete(&pszTcharSource);
    Delete(&pszTcharAddString);

    return hr;

}

/********************************************************************
    Function    Insert

    Arguments:    pszSource        - source string
                pszAddString    - string to insert
                iIndex            - where to insert into the string

    Comments:    Insert a string into another
*********************************************************************/
HRESULT CIssString::Insert(TCHAR *pszSource, 
                        TCHAR *pszAddString, 
                        int iIndex
                        )
{
    HRESULT hr = S_OK;
    BOOL bRet = TRUE;
    TCHAR *szLeftSide = NULL;
    TCHAR *szRightSide = NULL;
    TCHAR* sztemp = NULL;

    if(pszSource == NULL || pszAddString == NULL || iIndex < 0)
    {
        hr = E_INVALIDARG;
        CHR(hr, _T("Insert arguments are invalid"));
    }    

    int iAddStrLen = GetLength(pszAddString);    
    int iSrcLen = GetLength(pszSource);
    
    if(iIndex==0)
    {
        sztemp = new TCHAR[iAddStrLen + iSrcLen +1];
        CPHR(sztemp, _T("Insert - out of memory"));

        hr = StringCopy(sztemp,_T(""));
        CHR(hr, _T("Insert Error"));

        bRet = Concatenate(sztemp, pszAddString);
        CBHR(bRet, _T("Insert Error"));

        bRet = Concatenate(sztemp, pszSource);
        CBHR(bRet, _T("Insert Error"));

        hr = StringCopy(pszSource, sztemp);
        CHR(hr, _T("Insert Error"));

        
        //and done
        goto Error;
    }

    
    if(iIndex>iSrcLen)
        iIndex=iSrcLen;

    if(iIndex==iSrcLen)
    {
        Concatenate(pszSource, pszAddString);
        goto Error;
    }

    //actual work
    szLeftSide = new TCHAR[iSrcLen + iAddStrLen + 1];
    CPHR(szLeftSide, _T("Insert - out of memory"));

    szRightSide = new TCHAR[iSrcLen + 1];
    CPHR(szRightSide, _T("Insert - out of memory"));

    bRet = Left(szLeftSide, pszSource, iIndex);
    CBHR(bRet, _T("Insert Error"));
    bRet = Right(szRightSide, pszSource, iSrcLen-iIndex);
    CBHR(bRet, _T("Insert Error"));

    bRet = Concatenate(szLeftSide, pszAddString);
    CBHR(bRet, _T("Insert Error"));
    bRet = Concatenate(szLeftSide, szRightSide);
    CBHR(bRet, _T("Insert Error"));

    hr = StringCopy(pszSource, szLeftSide);
    CHR(hr, _T("Insert Error"));
    
Error:
    SAFE_DELETE_ARRAY(sztemp);
    SAFE_DELETE_ARRAY(szLeftSide);
    SAFE_DELETE_ARRAY(szRightSide);

    return hr;
}


/********************************************************************
    Function    IntToString

    Arguments:    pszSource        - source string
                iInt            - number we're putting in the string

    Comments:    convert an int to a TCHAR
*********************************************************************/
void CIssString::IntToString(TCHAR *pszDestination, 
                             int iSource
                             )
{
    if(pszDestination == NULL)
    {
        return;
    }

    wsprintf(pszDestination, _T("%i"), iSource);
}

/********************************************************************
    Function    SetResourceInstance

    Arguments:    hInstance - resource instance to set to

    Comments:    Set the HINSTANCE and store it within this class
*********************************************************************/
HRESULT CIssString::SetResourceInstance(HINSTANCE hInstance)
{
    if(NULL == hInstance)
        return E_INVALIDARG;

    m_hInstance = hInstance;
    
    return S_OK;
}

/********************************************************************
    Function    CreateAndCopy

    Arguments:    pszDestination    - String to create and copy contents to
                uiSource        - string table entry to get

    Comments:    Creates memory for pszDestination and then sets uiSource
                into the string.  NOTE:  pszDestination should be a NULL pointer
                and will have to be deleted when the caller is done with it
*********************************************************************/
TCHAR* CIssString::CreateAndCopy(UINT uiSource)
{
    return CreateAndCopy(uiSource, m_hInstance);
}

/********************************************************************
    Function    CreateAndCopy

    Arguments:    pszDestination    - String to create and copy contents to
                uiSource        - string table entry to get

    Comments:    Creates memory for pszDestination and then sets uiSource
                into the string.  NOTE:  pszDestination should be a NULL pointer
                and will have to be deleted when the caller is done with it
*********************************************************************/
TCHAR* CIssString::CreateAndCopy(UINT uiSource, 
                                 HINSTANCE hInst
                                 )
{
    // has the Instance been set properly yet?
    if(!hInst)
        return NULL;

    TCHAR szTemp[STRING_MAX];
    
    ZeroMemory(&szTemp, STRING_MAX * sizeof(TCHAR));
    // Get a Pointer to the string first
    LoadString(hInst, uiSource, szTemp, STRING_MAX);

    return CreateAndCopy(szTemp);
}

/********************************************************************
    Function    CreateAndCopy

    Arguments:    pszSource        - String to copy contents from

    Comments:    Creates memory for pszDestination and then sets pszSource
                into the string.  NOTE:  pszDestination should be a NULL pointer
                and will have to be deleted when the caller is done with it
*********************************************************************/
TCHAR* CIssString::CreateAndCopy(TCHAR* pszSource)
{
    if(!pszSource)
        return NULL;

    int iLen = GetLength(pszSource) + 1;

    TCHAR* pszDestination = new TCHAR[iLen];
    if(pszDestination == NULL)
    {
        return NULL;
    }
    ZeroMemory(pszDestination, iLen * sizeof(TCHAR));
    StringCopy(pszDestination, pszSource);

    return pszDestination;
}

/********************************************************************
    Function    CreateAndCopy

    Arguments:    pszSource        - String to copy contents from

    Comments:    Creates memory for pszDestination and then sets pszSource
                into the string.  NOTE:  pszDestination should be a NULL pointer
                and will have to be deleted when the caller is done with it
*********************************************************************/
CHAR* CIssString::CreateAndCopyChar(TCHAR* pszSource)
{
    if(!pszSource)
        return NULL;

    
    int iLen = GetLength(pszSource) + 1;
    
    CHAR* pszDestination = new CHAR[iLen];
    if(pszDestination == NULL)
    {
        return NULL;
    }
    ZeroMemory(pszDestination, iLen * sizeof(CHAR));
    
    StringCopy(pszDestination, pszSource);

    return pszDestination;
}

/********************************************************************
    Function    CreateAndCopy

    Arguments:    pszSource        - String to copy contents from

    Comments:    Creates memory for pszDestination and then sets pszSource
                into the string.  NOTE:  pszDestination should be a NULL pointer
                and will have to be deleted when the caller is done with it
*********************************************************************/
TCHAR* CIssString::CreateAndCopy(TCHAR* pszSource, 
                                 int iStart, 
                                 int iCount
                                 )
{
    if(!pszSource || iStart < 0 || iCount < 0)
    {
        return NULL;
    }

    if(GetLength(pszSource) < (iStart + iCount))
    {
        return NULL;
    }

    TCHAR* pszDestination = new TCHAR[iCount+1];
    if(pszDestination == NULL)
    {
        return NULL;
    }
    ZeroMemory(pszDestination, sizeof(TCHAR)*(iCount+1));

    if(FAILED(StringCopy(pszDestination, pszSource, iStart, iCount)))
    {
        Delete(&pszDestination);
        return NULL;
    }

    pszDestination[iCount] = '\0';

    return pszDestination;
}

/********************************************************************
    Function    CreateAndCopy

    Arguments:    pszSource        - String to copy contents from

    Comments:    Creates memory for pszDestination and then sets pszSource
                into the string.  NOTE:  pszDestination should be a NULL pointer
                and will have to be deleted when the caller is done with it
*********************************************************************/
CHAR* CIssString::CreateAndCopyChar(CHAR* pszSource)
{
    if(!pszSource)
        return NULL;

    int iLen = GetLength(pszSource) + 1;
    CHAR* pszDestination = new CHAR[iLen];
    if(pszDestination == NULL)
    {
        return NULL;
    }
    ZeroMemory(pszDestination, iLen * sizeof(CHAR));
    
    StringCopy(pszDestination, pszSource);

    return pszDestination;
}

/********************************************************************
    Function    CreateAndCopy

    Arguments:    pszSource        - String to copy contents from

    Comments:    Creates memory for pszDestination and then sets pszSource
                into the string.  NOTE:  pszDestination should be a NULL pointer
                and will have to be deleted when the caller is done with it
*********************************************************************/
CHAR* CIssString::CreateAndCopyChar(CHAR* pszSource, 
                                    int iStart, 
                                    int iCount
                                    )
{
    if(!pszSource || iStart < 0 || iCount < 0)
    {
        return NULL;
    }

    if(GetLength(pszSource) < (iStart + iCount))
    {
        return NULL;
    }
    
    CHAR* pszDestination = new CHAR[iCount+1];
    if(pszDestination == NULL)
    {
        return NULL;
    }
    ZeroMemory(pszDestination, iCount+1);

    if(FAILED(StringCopy(pszDestination, pszSource, iStart, iCount)))
    {
        Delete(&pszDestination);
        return NULL;
    }

    pszDestination[iCount] = '\0';

    return pszDestination;
}

/********************************************************************
    Function    CreateAndCopy

    Arguments:    pszSource        - String to copy contents from

    Comments:    Creates memory for pszDestination and then sets pszSource
                into the string.  NOTE:  pszDestination should be a NULL pointer
                and will have to be deleted when the caller is done with it
*********************************************************************/
TCHAR* CIssString::CreateAndCopy(CHAR* pszSource, 
                                 int iStart, 
                                 int iCount
                                 )
{
    if(!pszSource || iStart < 0 || iCount < 0)
    {
        return NULL;
    }

    if(GetLength(pszSource) < (iStart + iCount))
    {
        return NULL;
    }

    TCHAR* pszNewSource = CreateAndCopy(pszSource);


    TCHAR* pszDestination = new TCHAR[iCount + 1];
    if(pszDestination == NULL)
    {
        return NULL;
    }

    ZeroMemory(pszDestination, (iCount+1) * sizeof(TCHAR));

    if(FAILED(StringCopy(pszDestination, pszNewSource, iStart, iCount)))
    {
        Delete(&pszNewSource);
        Delete(&pszDestination);
        return NULL;
    }

    pszDestination[iCount] = _T('\0');

    Delete(&pszNewSource);

    return pszDestination;
}

TCHAR*  CIssString::GetText(UINT uiText)
{
	if(!m_szText1)
	{
		m_szText1 = new TCHAR[4*STRING_MAX];
		if(!m_szText1)
			return NULL;
	}
    Empty(m_szText1);
    if(m_hInstance)
        StringCopy(m_szText1, uiText, 4*STRING_MAX, m_hInstance);
    return m_szText1;
}

TCHAR*  CIssString::GetText(UINT uiText, HINSTANCE hInst)
{
	if(!m_szText1)
	{
		m_szText1 = new TCHAR[4*STRING_MAX];
		if(!m_szText1)
			return NULL;
	}

    Empty(m_szText1);
    StringCopy(m_szText1, uiText, 8*STRING_MAX, hInst);
    return m_szText1;
}


/********************************************************************
    Function    CreateAndCopy

    Arguments:    pszSource        - String to copy contents from

    Comments:    Creates memory for pszDestination and then sets pszSource
                into the string.  NOTE:  pszDestination should be a NULL pointer
                and will have to be deleted when the caller is done with it
*********************************************************************/
TCHAR* CIssString::CreateAndCopy(CHAR* pszSource)
{
    if(!pszSource)
        return NULL;

    int iLen = GetLength(pszSource) + 1;

    TCHAR* pszDestination = new TCHAR[iLen];
    if(pszDestination == NULL)
    {
        return NULL;
    }

    ZeroMemory(pszDestination, iLen * sizeof(TCHAR));

    StringCopy(pszDestination, pszSource);

    return pszDestination;
}


/********************************************************************
    Function    IsEmpty

    Arguments:    pszSource - string to check

    Comments:    Check if this string is empty
*********************************************************************/
BOOL CIssString::IsEmpty(TCHAR* pszSource)
{
    if(pszSource == NULL)
        return TRUE;

    if(GetLength(pszSource) > 0)
        return FALSE;
    else
        return TRUE;
}

/********************************************************************
Function    Delete

Arguments:    szIn - TCHAR buffer (double pointer)

Comments:    Will delete the TCHAR buffer and set the pointer to NULL
*********************************************************************/
BOOL CIssString::Delete(TCHAR** szIn)
{
    if(szIn == NULL)
        return FALSE;
    if(*szIn == NULL)
        return FALSE;
    delete [] *szIn;
    *szIn = NULL;
    return TRUE;
}

/********************************************************************
Function    Delete

Arguments:    szIn - char buffer (double pointer)

Comments:    Will delete the char buffer and set the pointer to NULL
*********************************************************************/
BOOL CIssString::Delete(char** szIn)
{
    if(szIn == NULL)
        return FALSE;
    if(*szIn == NULL)
        return FALSE;
    delete [] *szIn;
    *szIn = NULL;
    return TRUE;
}

BOOL CIssString::ReplaceText(TCHAR* szText, TCHAR* szSearch, TCHAR* szReplace)
{
    int iIndex = 0;

    if(!szText || !szSearch)
        return FALSE;

    iIndex = Find(szText, szSearch, 0);
    while(iIndex != -1)
    {
        // delete the text
        Delete(iIndex, GetLength(szSearch), szText);

        if(szReplace)
            Insert(szText, szReplace, iIndex);

        iIndex = Find(szText, szSearch, iIndex);
    }
    return TRUE;
}