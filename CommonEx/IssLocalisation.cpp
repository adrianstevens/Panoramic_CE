#include "IssLocalisation.h"
#include "IssRegistry.h"

#define KEY_Name        _T("Locale")

CIssLocalisation g_cLocale;

CIssLocalisation::CIssLocalisation()
:m_oStr(CIssString::Instance())
,m_dwLangOffset(LANG_ENGLISH)
,m_dwSubLanguageOffset(0)
,m_bVerifyResources(TRUE)
,m_bInitialized(FALSE)
,m_szRegKey(NULL)
,m_hInst(NULL)
,m_dwSupportedLanguages(SUPPORT_English)
{
    m_oStr->IntToString(m_szLangID, LANG_ENGLISH);
}

CIssLocalisation::~CIssLocalisation()
{
    Destroy();
}

void CIssLocalisation::Destroy()
{
    m_oStr->Delete(&m_szRegKey);
    m_dwLangOffset = LANG_ENGLISH;
    m_dwSupportedLanguages = SUPPORT_English;
    m_bVerifyResources = TRUE;
    m_bInitialized = FALSE;
    m_hInst         = NULL;
}

BOOL CIssLocalisation::Init(TCHAR* szRegKey, HINSTANCE hInst, DWORD dwLanguagesFlags, BOOL bVerifyResources)
{
    Destroy();

    m_szRegKey          = m_oStr->CreateAndCopy(szRegKey);
    m_bVerifyResources  = bVerifyResources;
    m_hInst             = hInst;
    m_dwSupportedLanguages = dwLanguagesFlags;
    m_bInitialized      = TRUE;

	m_dwSubLanguageOffset	= GetSystemSubLanguage();

    if(S_OK != GetKey(szRegKey, KEY_Name, m_dwLangOffset))
        m_dwLangOffset = GetSystemLanguage();

    if(!IsLanguageSupported(m_dwLangOffset))
        m_dwLangOffset = LANG_ENGLISH;

/*#ifdef DEBUG
	m_dwLangOffset = LANG_PORTUGUESE;
	m_dwSubLanguageOffset = SUBLANG_PORTUGUESE_BRAZILIAN;

#endif*/

	m_oStr->IntToString(m_szLangID, m_dwLangOffset);

    return TRUE;
}

BOOL CIssLocalisation::GetRegGamesFolder(TCHAR* szRegKey, int iLen, TCHAR* szLinkName)
{
    CIssString* oStr = CIssString::Instance();

    if(!szRegKey || iLen == 0 || oStr->IsEmpty(szLinkName))
        return FALSE;

    ZeroMemory(szRegKey, iLen*sizeof(TCHAR));

    oStr->StringCopy(szRegKey, _T("Security\\Shell\\StartInfo\\Start\\"));

    TCHAR szLanguage[10] = _T("ENU");

    int iResult = GetLocaleInfo(LOCALE_USER_DEFAULT,  LOCALE_ILANGUAGE, szLanguage, 10);

    TCHAR* szEndPtr;
    long l = _tcstol(szLanguage, &szEndPtr, 16);

    // We have the language ID, but we need to extract the PRIMARY language ID
    DWORD dwLang = PRIMARYLANGID(l);
    switch(dwLang)
    {
    case LANG_SPANISH:
        oStr->Concatenate(szRegKey, _T("Juegos"));
        break;
    case LANG_FRENCH:
        oStr->Concatenate(szRegKey, _T("Jeux"));
        break;
    case LANG_RUSSIAN:
        oStr->Concatenate(szRegKey, _T("Игры"));
        break;
    case LANG_ITALIAN:
        oStr->Concatenate(szRegKey, _T("Giochi"));
        break;
    case LANG_JAPANESE:
        oStr->Concatenate(szRegKey, _T("ゲーム"));
        break;
    case LANG_PORTUGUESE:
        oStr->Concatenate(szRegKey, _T("Jogos"));
        break;
    case LANG_GERMAN:
        oStr->Concatenate(szRegKey, _T("Spiele"));
        break;
    case LANG_DUTCH:
        oStr->Concatenate(szRegKey, _T("Spelletjes"));
        break;
    case LANG_CHINESE:
        oStr->Concatenate(szRegKey, _T("遊戲"));
        break;
    case LANG_KOREAN:
        oStr->Concatenate(szRegKey, _T("게임"));
        break;
    case LANG_NORWEGIAN:
        oStr->Concatenate(szRegKey, _T("Spill"));
        break;
    case LANG_ROMANIAN:
        oStr->Concatenate(szRegKey, _T("Jocuri"));
        break;
    default:
        oStr->Concatenate(szRegKey, _T("Games"));
        break;
    }

    oStr->Concatenate(szRegKey, _T("\\"));
    oStr->Concatenate(szRegKey, szLinkName);
    oStr->Concatenate(szRegKey, _T(".lnk"));

    return TRUE;

    return TRUE;
}

DWORD CIssLocalisation::GetSystemLanguage()
{
    TCHAR szLanguage[10] = _T("ENU");

    int iResult = GetLocaleInfo(LOCALE_USER_DEFAULT,  LOCALE_ILANGUAGE, szLanguage, 10);

    TCHAR* szEndPtr;
    long l = _tcstol(szLanguage, &szEndPtr, 16);

    // We have the language ID, but we need to extract the PRIMARY language ID
    DWORD dwLang = PRIMARYLANGID(l);

    // check if the language is supported
    if(IsLanguageSupported(dwLang))
        return dwLang;
    else
        return LANG_ENGLISH;    // default
}

DWORD CIssLocalisation::GetSystemSubLanguage()
{
	TCHAR szLanguage[10] = _T("ENU");

	int iResult = GetLocaleInfo(LOCALE_USER_DEFAULT,  LOCALE_ILANGUAGE, szLanguage, 10);

	TCHAR* szEndPtr;
	long l = _tcstol(szLanguage, &szEndPtr, 16);

	// We have the language ID, but we need to extract the PRIMARY language ID
	return SUBLANGID(l);
}

BOOL CIssLocalisation::IsLanguageSupported(DWORD& dwLanguage)
{
    BOOL bRet;
    switch(dwLanguage)
    {
    case LANG_ENGLISH:
        bRet    = (m_dwSupportedLanguages&SUPPORT_English);
        break;
    case LANG_SPANISH:
        bRet    = (m_dwSupportedLanguages&SUPPORT_Spanish);
        break;
    case LANG_FRENCH:
        bRet    = (m_dwSupportedLanguages&SUPPORT_French);
        break;
    case LANG_RUSSIAN:
        bRet    = (m_dwSupportedLanguages&SUPPORT_Russian);
        break;
    case LANG_ITALIAN:
        bRet    = (m_dwSupportedLanguages&SUPPORT_Italian);
        break;
    case LANG_JAPANESE:
        bRet    = (m_dwSupportedLanguages&SUPPORT_Japanese);
        break;
    case LANG_PORTUGUESE:
        bRet    = (m_dwSupportedLanguages&SUPPORT_Portuguese);
        break;
    case LANG_GERMAN:
        bRet    = (m_dwSupportedLanguages&SUPPORT_German);
        break;
    case LANG_DUTCH:
        bRet    = (m_dwSupportedLanguages&SUPPORT_Dutch);
        break;
    case LANG_CHINESE:
        bRet    = (m_dwSupportedLanguages&SUPPORT_TradChinese);
        break;
    case LANG_KOREAN:
        bRet    = (m_dwSupportedLanguages&SUPPORT_Korean);
        break;
	case LANG_NORWEGIAN:
		bRet    = (m_dwSupportedLanguages&SUPPORT_Norwegian);
		break;
	case LANG_ROMANIAN:
		bRet    = (m_dwSupportedLanguages&SUPPORT_Romanian);
		break;
    default:
        bRet = FALSE;
    }

    return bRet;
}

void CIssLocalisation::GetLanguageCode(TCHAR* szDest)
{
	if(!szDest)
		return;
	m_oStr->Empty(szDest);

	switch(m_dwLangOffset)
	{
	case LANG_ARABIC:
		m_oStr->StringCopy(szDest, _T("ar"));
		break;
	case LANG_CHINESE:
		m_oStr->StringCopy(szDest, _T("zh"));
		break;
	case LANG_CZECH:
		m_oStr->StringCopy(szDest, _T("cs"));
		break;
	case LANG_DANISH:
		m_oStr->StringCopy(szDest, _T("da"));
		break;
	case LANG_DUTCH:
		m_oStr->StringCopy(szDest, _T("du"));
		break;
	case LANG_FRENCH:
		m_oStr->StringCopy(szDest, _T("fr"));
		break;
	case LANG_GERMAN:
		m_oStr->StringCopy(szDest, _T("de"));
		break;
	case LANG_GREEK:
		m_oStr->StringCopy(szDest, _T("el"));
		break;
	case LANG_HINDI:
		m_oStr->StringCopy(szDest, _T("hi"));
		break;
	case LANG_ITALIAN:
		m_oStr->StringCopy(szDest, _T("it"));
		break;
	case LANG_JAPANESE:
		m_oStr->StringCopy(szDest, _T("ja"));
		break;
	case LANG_KOREAN:
		m_oStr->StringCopy(szDest, _T("ko"));
		break;
	case LANG_RUSSIAN:
		m_oStr->StringCopy(szDest, _T("ru"));
		break;
	case LANG_SPANISH:
		m_oStr->StringCopy(szDest, _T("es"));
		break;
	case LANG_ROMANIAN:
		m_oStr->StringCopy(szDest, _T("ro"));
		break;
	case LANG_PORTUGUESE:
		m_oStr->StringCopy(szDest, _T("pt"));
		break;
	case LANG_NORWEGIAN:
		m_oStr->StringCopy(szDest, _T("no"));
		break;
	case LANG_ENGLISH:
	default:
		m_oStr->StringCopy(szDest, _T("en"));
		break;
	}
}

void CIssLocalisation::GetLanguageText(TCHAR* szDest, DWORD dwLanguage)
{
    if(!szDest)
        return;

    switch(dwLanguage)
    {    
    case LANG_SPANISH:
        m_oStr->StringCopy(szDest, _T("Español"));
        break;
    case LANG_FRENCH:
        m_oStr->StringCopy(szDest, _T("Français"));
        break;
    case LANG_HINDI:
        m_oStr->StringCopy(szDest, _T("हिन्दी"));
        break;
    case LANG_ITALIAN:
        m_oStr->StringCopy(szDest, _T("Italiano"));
        break;
    case LANG_JAPANESE:
        m_oStr->StringCopy(szDest, _T("日本語"));
        break;
    case LANG_PORTUGUESE:
        m_oStr->StringCopy(szDest, _T("Português"));
        break;
    case LANG_GERMAN:
        m_oStr->StringCopy(szDest, _T("Deutsch"));
        break;
    case LANG_DUTCH:
        m_oStr->StringCopy(szDest, _T("Nederlands"));
        break;
    case LANG_CHINESE:
        m_oStr->StringCopy(szDest, _T("中文"));
        break;
    case LANG_KOREAN:
        m_oStr->StringCopy(szDest, _T("한국어"));
        break;
	case LANG_NORWEGIAN:
		m_oStr->StringCopy(szDest, _T("Norsk"));
		break;
	case LANG_RUSSIAN:
		m_oStr->StringCopy(szDest, _T("Русский"));
		break;
	case LANG_ROMANIAN:
		m_oStr->StringCopy(szDest, _T("Român"));
		break;
    case LANG_ENGLISH:
    default:
        m_oStr->StringCopy(szDest, _T("English"));
        break;
    }
}

void CIssLocalisation::GetLanguageOptionText(TCHAR* szDest, DWORD dwLanguage)
{
    if(!szDest)
        return;

    switch(dwLanguage)
    {
    case LANG_SPANISH:
        m_oStr->StringCopy(szDest, _T("Idioma:"));
        break;
    case LANG_FRENCH:
        m_oStr->StringCopy(szDest, _T("Langue:"));
        break;
    case LANG_HINDI:
        m_oStr->StringCopy(szDest, _T("भाषा:"));
        break;
    case LANG_ITALIAN:
        m_oStr->StringCopy(szDest, _T("Lingua:"));
        break;
    case LANG_JAPANESE:
        m_oStr->StringCopy(szDest, _T("言語:"));
        break;
    case LANG_PORTUGUESE:
        m_oStr->StringCopy(szDest, _T("Linguagem:"));
        break;
    case LANG_GERMAN:
        m_oStr->StringCopy(szDest, _T("Sprache:"));
        break;
    case LANG_DUTCH:
        m_oStr->StringCopy(szDest, _T("Taal:"));
        break;
    case LANG_CHINESE:
        m_oStr->StringCopy(szDest, _T("語言:"));
        break;
	case LANG_KOREAN:
		m_oStr->StringCopy(szDest, _T("언어:"));
		break;
	case LANG_NORWEGIAN:
		m_oStr->StringCopy(szDest, _T("Språk:"));
		break;
	case LANG_RUSSIAN:
		m_oStr->StringCopy(szDest, _T("Язык:"));
		break;
	case LANG_ROMANIAN:
		m_oStr->StringCopy(szDest, _T("Limba:"));
		break;
    case LANG_ENGLISH:
    default:
        m_oStr->StringCopy(szDest, _T("Language:"));
        break;
    }
}

BOOL CIssLocalisation::SetCurrentLanguage(DWORD dwLanguage)
{
    if(!IsLanguageSupported(dwLanguage) || !m_bInitialized)
        return FALSE;

    m_dwLangOffset  = dwLanguage;
    SetKey(m_szRegKey, KEY_Name, m_dwLangOffset);

    m_oStr->IntToString(m_szLangID, dwLanguage);

    return TRUE;
}

BOOL CIssLocalisation::IsStringExists(UINT uiString)
{
    HRSRC hres = FindResource(m_hInst, (LPCWSTR)uiString, RT_STRING);
    return (hres==NULL?FALSE:TRUE);
}

UINT CIssLocalisation::GetString(UINT uiEnglish)
{
    if(!m_bInitialized || m_dwLangOffset == LANG_ENGLISH || uiEnglish > 1000)
        return uiEnglish;

    UINT uiNewRes = (m_dwLangOffset * 1000) + uiEnglish;

	// special case for Portugues brazil
	if(m_dwLangOffset == LANG_PORTUGUESE && m_dwSubLanguageOffset == SUBLANG_PORTUGUESE_BRAZILIAN)
	{
		// we're going to use the Turkish offset
		uiNewRes = (LANG_TURKISH * 1000) + uiEnglish;
	}

    //based off Katrina's setup ... so we'll compare the string to given language ID 
    if(m_dwLangOffset != LANG_ENGLISH && m_bVerifyResources == TRUE)
    {
        static TCHAR szTemp[STRING_SMALL];
        m_oStr->StringCopy(szTemp, uiNewRes, 4, m_hInst);//nice and small since we're only checking a number
        if(m_oStr->Compare(m_szLangID, szTemp) == 0)
            uiNewRes = uiEnglish;
    }

    // BUGBUG doesn't seem to work
    /*if(m_bVerifyResources)
    {
        if(!IsStringExists(uiNewRes))
            return uiEnglish;
    }*/

    return uiNewRes;
}

