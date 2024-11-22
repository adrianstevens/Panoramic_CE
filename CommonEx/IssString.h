/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: iSSString.h 
// Abstract: Common string functions  
// 
/***************************************************************************************************************/ 

#pragma once

#define STRING_MAX     256
#define STRING_LARGE    64
#define STRING_NORMAL   32
#define STRING_SMALL     8

#ifdef _AFXDLL
#include "stdafx.h"
#else
#include "windows.h"
#endif

#ifndef UNDER_CE
#include "tchar.h"
#endif

class CIssString  
{
public:
    CIssString();
    virtual ~CIssString();
    static CIssString* Instance ();
    static void DeleteInstance  ();

    HRESULT    SetResourceInstance(HINSTANCE hInstance);

    // String Routines
    HRESULT    TrimLeft(TCHAR *pszSource);
    HRESULT    TrimRight(TCHAR *pszSource);
    HRESULT    Trim(TCHAR *pszSource);
    HRESULT    Capitalize(TCHAR* pszSource);
    HRESULT    Lowercase(TCHAR* pszSource);

    HRESULT    Insert(TCHAR *pszSource, TCHAR *pszAddString, int iIndex=0);
    HRESULT    Insert(CHAR *pszSource, CHAR *pszAddString, int iIndex=0);
    BOOL       ReplaceText(TCHAR* szText, TCHAR* szSearch, TCHAR* szReplace);

    //breaks the calc engine as HRESULTS ... leave as is for backwards compatibility
    BOOL       Concatenate(char* szSource, TCHAR* szAddString);
    BOOL       Concatenate(TCHAR* szSource, char* szAddString);
    BOOL       Concatenate(char* pszSource, char* pszAddString);
    BOOL       Concatenate(TCHAR* pszSource, TCHAR* pszAddString);
    BOOL        Concatenate(TCHAR* pszSource, char cAddValue);
    BOOL        Concatenate(TCHAR* pszSource, TCHAR cAddValue);
    BOOL        Concatenate(char* pszSource, char cAddValue);
    BOOL        Concatenate(char* pszSource, TCHAR cAddValue);

    HRESULT    StringCopy(TCHAR *pszDestination, TCHAR *pszSource);
    HRESULT    StringCopy(char* pszDestination, char* pszSource);
    HRESULT    StringCopy(char  *pszDestination, TCHAR *pszSource);
    HRESULT    StringCopy(TCHAR *pszDestination, char *pszSource);
    HRESULT    StringCopy(char  *pszDestination, TCHAR *pszSource, int iDestSize);
    HRESULT    StringCopy(TCHAR *pszDestination, CHAR *pszSource, int iDestSize);
    HRESULT    StringCopy(char  *pszDestination, char *pszSource, int iDestSize);
    HRESULT    StringCopy(TCHAR *pszDestination, TCHAR *pszSource, int iDestSize);
    HRESULT    StringCopy(CHAR* pszDestination, CHAR* pszSource, int iStart, int iCount);
    HRESULT    StringCopy(TCHAR* pszDestination, CHAR* pszSource, int iStart, int iCount);
    HRESULT    StringCopy(TCHAR* pszDestination, TCHAR* pszSource, int iStart, int iCount);
    HRESULT    StringCopy(TCHAR* pszDestination, UINT uiSource, int iMaxBuffer);
    HRESULT    StringCopy(TCHAR* pszDestination, UINT uiSource, int iMaxBuffer, HINSTANCE hInst);

    int     Compare(char *pszStr1, char *pszStr2);
    int     Compare(TCHAR* pszStr1, char* pszStr2);
    int     Compare(char* pszStr1, TCHAR* pszStr2);

    char*   StringToken(char *pszSource, char *pszToken);
    int     GetLength(char *pszSource);
    
    int     Find(char *pszSource, char *pszFindString);
    int     Find(char *pszStrToSearch, TCHAR* pszStrToFind);
    int     Find(char *pszSource, char *pszFindString, int nStart);
    int     FindNoCase(TCHAR* pszSource, TCHAR* pszFindString, int iStart = 0);
    int     FindNoCase(CHAR* pszSource, TCHAR* pszFindString, int iStart = 0);
    int     FindNoCase(CHAR* pszSource, CHAR* pszFindString, int iStart = 0);
    int     FindLastOccurance(char* pszStrToSearch, char* pszStrToFind);
    int     FindLastOccurance(TCHAR* pszStrToSearch, char* pszStrToFind);
    int     FindLastOccurance(char* pszStrToSearch, TCHAR* pszStrToFind);
    int     FindLastOccurance(TCHAR* pszStrToSearch, TCHAR* pszStrToFind);

    double  StringToDouble(char *pszSource);

    HRESULT     Empty(char *pszSource);
    HRESULT     Empty(TCHAR *pszSource);
    BOOL        IsEmpty(char* pszSource);
    BOOL        IsEmpty(TCHAR* pszSource);

    int     FindOccurances(char* pszSource, char* pszFindString);

    TCHAR*  GetText(UINT uiText, HINSTANCE hInst);
    TCHAR*  GetText(UINT uiText);

    TCHAR*  CreateAndCopy(UINT uiSource);
    TCHAR*  CreateAndCopy(UINT uiSource, HINSTANCE hInst);
    TCHAR*  CreateAndCopy(TCHAR* pszSource);
    TCHAR*  CreateAndCopy(CHAR* pszSource);
    TCHAR*  CreateAndCopy(TCHAR* pszSource, int iStart, int iCount);
    TCHAR*  CreateAndCopy(CHAR* pszSource, int iStart, int iCount);

    CHAR*   CreateAndCopyChar(CHAR* pszSource, int iStart, int iCount);
    CHAR*   CreateAndCopyChar(TCHAR* szSource);
    CHAR*   CreateAndCopyChar(CHAR* szSource);

    TCHAR*  GetStringInBuffer(TCHAR* pszBuffer, TCHAR* pszStart, TCHAR* pszEnd, TCHAR** pszString, DWORD dwLen);
    
    BOOL    Delete(TCHAR** szIn); //returns FALSE if the TCHAR is already NULL
    BOOL    Delete(char** szIn); //returns FALSE if the char is already NULL

    int     GetLength(TCHAR *pszSource);
    TCHAR   GetLast(TCHAR *pszSource);
    TCHAR   GetFirst(TCHAR *pszSource);

    int     Find(TCHAR *pszSource, TCHAR *pszFindString);
    int     Find(TCHAR *pszSource, TCHAR *pszFindString, int nStart);
    
    int     Compare(TCHAR *pszStr1, TCHAR *pszStr2);
    
    TCHAR*  StringToken(TCHAR *pszSource, TCHAR *pszToken);

    HRESULT    Format(TCHAR *pszDestination, TCHAR *pszFormat, ...);
    HRESULT    Format(CHAR *pszDestination, CHAR *pszFormat, ...);

    int     StringToInt(TCHAR *pszSource);
    double  StringToDouble(TCHAR *pszSource);
    int     CompareNoCase(TCHAR *pszStr1, TCHAR *pszStr2);
    
    HRESULT    DoubleToString(TCHAR *pszDestination, double dbSource, int iSigFigs = 12);
    void    IntToString(TCHAR *pszDestination, int iSource);

    BOOL    Mid(TCHAR *pszDestination, TCHAR *pszSource, int nFirst);
    BOOL    Mid(TCHAR *pszDestination, TCHAR *pszSource, int nFirst, int nCount);
    BOOL    Right(TCHAR *pszDestination, TCHAR *pszSource, int nCount);
    BOOL    Left(TCHAR *pszDestination, TCHAR *pszSource, int nCount);

    void    Delete(int iIndex, int iCount, TCHAR *szString);
    void    Delete(int iIndex, int iCount, CHAR* szString);

    
private:
    static CIssString*   m_instance;
    HINSTANCE            m_hInstance;
	TCHAR*				 m_szText1;
};

