#pragma once

#include "windows.h"
#include "IssString.h"

#define SUPPORT_English         0x001
#define SUPPORT_Spanish         0x001 << 1
#define SUPPORT_French          0x001 << 2
#define SUPPORT_Russian         0x001 << 3
#define SUPPORT_Italian         0x001 << 4
#define SUPPORT_Japanese        0x001 << 5
#define SUPPORT_Portuguese      0x001 << 6
#define SUPPORT_German          0x001 << 7
#define SUPPORT_Dutch           0x001 << 8
#define SUPPORT_TradChinese     0x001 << 9
#define SUPPORT_Korean          0x001 << 10
#define SUPPORT_Romanian        0x001 << 11
#define SUPPORT_Norwegian       0x001 << 12

#define SUPPORT_All             0xffffff

class CIssLocalisation;

extern CIssLocalisation g_cLocale;

#define ID(res) (g_cLocale.GetString(res))

class CIssLocalisation
{
public:
    CIssLocalisation(void);
    ~CIssLocalisation(void);

    BOOL        Init(TCHAR* szRegKey, HINSTANCE hInst, DWORD dwLanguagesFlags = SUPPORT_English, BOOL bVerifyResources = FALSE);
    BOOL        SetCurrentLanguage(DWORD dwLanguage);
    DWORD       GetCurrentLanguage(){return m_dwLangOffset;};
    void        Destroy();
    DWORD       GetSystemLanguage();
	DWORD		GetSystemSubLanguage();
    static BOOL        GetRegGamesFolder(TCHAR* szRegKey, int iLen, TCHAR* szLinkName);
    UINT        GetString(UINT uiEnglish);
    void        GetLanguageText(TCHAR* szDest, DWORD dwLanguage);
    void        GetLanguageOptionText(TCHAR* szDest, DWORD dwLanguage);
	void		GetLanguageCode(TCHAR* szDest);

    HINSTANCE   GetHInst(){return m_hInst;};
    void        SetHInst(HINSTANCE hInst){m_hInst = hInst;};


private:    // functions
    BOOL        IsLanguageSupported(DWORD& dwLanguage);
    BOOL        IsStringExists(UINT uiString);


private:    // variables
    CIssString* m_oStr;
    DWORD       m_dwLangOffset;
    DWORD       m_dwSupportedLanguages;
	DWORD		m_dwSubLanguageOffset;
    BOOL        m_bInitialized;
    BOOL        m_bVerifyResources;
    TCHAR*      m_szRegKey;
    TCHAR       m_szLangID[STRING_SMALL]; // thought I'd keep it as a member variable so we don't have to create it every time

    HINSTANCE   m_hInst;
};
