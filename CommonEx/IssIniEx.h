/***************************************************************************************************************/  
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved  
//  
// This source code is licensed under Implicit Software Solutions Inc.    
//   
// Any use of this software is limited to those who have agreed to the License.  
//  
// FileName: IssIniEx.h
// Abstract: Class for reading, creating, and parsing INI formated files
//  
/***************************************************************************************************************/ 
#pragma once

#include "IssString.h"
#include "IssVector.h"

struct TypeValue
{
    TypeValue();
    ~TypeValue();
    TCHAR   *szKeys;
    TCHAR   *szValues;
};

struct TypeSection
{
    TypeSection();
    ~TypeSection();
    TCHAR*                  szSection;
    CIssVector<TypeValue>   arrValues;
};

class CIssIniEx  
{
public:
    CIssIniEx();
    virtual ~CIssIniEx();

    void Destroy();
    HRESULT GetValue(TCHAR*    pszDestination, 
                  TCHAR*    pszSection, 
                  TCHAR*    pszKey, 
                  TCHAR*    pszDefaultValue
                  );
    HRESULT GetValue(int*  piDestination, 
                      TCHAR*    pszSection, 
                      TCHAR*    pszKey, 
                      int       iDefaultValue
                      );
    HRESULT GetValue(double*   pdbDestination,
                      TCHAR*    pszSection,
                      TCHAR*    pszKey,
                      double    dbDefaultValue
                      );
    HRESULT GetValue(POINT* pptDestination, 
                      TCHAR* pszSection, 
                      TCHAR* pszKey, 
                      POINT ptDefaultValue
                      );
    HRESULT GetValue(SIZE* psDestination, 
                      TCHAR* pszSection, 
                      TCHAR* pszKey, 
                      SIZE  sDefaultValue
                      );
    HRESULT GetValue(COLORREF* pcrDestination, 
                      TCHAR* pszSection, 
                      TCHAR* pszKey, 
                      COLORREF crDefaultValue
                      );
    HRESULT Open(TCHAR* pszFileName);
    HRESULT OpenUnicode(TCHAR* pszFileName);//Open will call this if it finds the unicode header so you shouldn't need to call this separately

    HRESULT OpenFromResrouce(TCHAR* szResName, HINSTANCE hInst);
    HRESULT Save(TCHAR* pszFileName);
    HRESULT SaveUnicode(TCHAR* pszFileName);
	HRESULT SaveUTF8(TCHAR* pszFileName);
    HRESULT AddSection(TCHAR* pszSection);
    HRESULT AddAttribute(TCHAR* pszSection,
                      TCHAR*  pszKey,
                      TCHAR*  pszValue
                      );
    HRESULT AddAttribute(TCHAR* pszSection,
                          TCHAR*  pszKey,
                          double  dbValue
                          );
    HRESULT SetValue(TCHAR*  pszSection,
                      TCHAR* pszKey,
                      int    iValue
                      );
    HRESULT SetValue(TCHAR* pszSection,
                      TCHAR* pszKey,
                      TCHAR* pszValue
                      );
    HRESULT SetValue(TCHAR* pszSection,
                  TCHAR*  pszKey,
                  COLORREF  crValue
                  );
    HRESULT SetValue(TCHAR*    pszSection,
                  TCHAR*  pszKey,
                  POINT   ptValue
                  );
    HRESULT SetValue(TCHAR*    pszSection,
                      TCHAR*   pszKey,
                      SIZE     sValue
                      );
    HRESULT SetValue(TCHAR*  pszSection,
                      TCHAR* pszKey,
                      double dbValue
                      );
private: // Functions   
    int     GetKeyIndex (int iSectionIndex,TCHAR *pszKey);
    int     GetSectionIndex (TCHAR *pszSection);

private: // Variables
    HANDLE                  m_hFile;
    BOOL                    m_bFileOpened;
    CRITICAL_SECTION        m_cs;
    CIssVector<TypeSection> m_arrSections;
    CIssString*             m_oStr;
};

