#include "StdAfx.h"
#include "DlgLiteOptions.h"
#include "IssCommon.h"
#include "Resource.h"
#include "IssLocalisation.h"

#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Item          0xD0D0D0
#define TEXTCOLOR_Selected      RGB(149,223,49) //RGB(255,246,0) //RGB(119,175,57)
#define IDMENU_Select           5000
#define IDMENU_SelectMenu       5001
#define BUTTON_Height           (GetSystemMetrics(SM_CXICON)*3/4)
#define HEIGHT_Text             (GetSystemMetrics(SM_CXICON)*4/9)
#define INDENT                  (GetSystemMetrics(SM_CXSMICON)/4)

CDlgOptions::CDlgOptions(void)
:m_wndMenu(NULL)
,m_dwReturn(0)
,m_hWndParent(NULL)
{
    m_hFontLabel    = CIssGDIEx::CreateFont(HEIGHT_Text, FW_BOLD, TRUE);
    m_hFontBtnText	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_NORMAL, TRUE);
}

CDlgOptions::~CDlgOptions(void)
{
    CIssGDIEx::DeleteFont(m_hFontLabel);
    CIssGDIEx::DeleteFont(m_hFontBtnText);
}

BOOL CDlgOptions::Init(TypeOptions& sOptions, CIssWndTouchMenu* wndMenu, CCalcMenu *oMenu, HWND hWnd)
{
	m_sOptions      = sOptions;
    m_wndMenu       = wndMenu;
	m_oBottomMenu   = oMenu;
	m_hWndParent	= hWnd;

	return TRUE;
}


BOOL CDlgOptions::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//these are up here for a reason 
	//InitControls();
	//SetControls();

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
#ifdef WIN32_PLATFORM_WFSP
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
#else
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR|SHIDIF_SIPDOWN;
#endif
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);
#endif
    m_oMenu.SetColors(TEXTCOLOR_Normal, TEXTCOLOR_Normal, TEXTCOLOR_Selected);
    m_oMenu.Initialize(hWnd, hWnd, m_hInst, OPTION_CircularList|OPTION_Bounce);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteItem);
    m_oMenu.SetSelected(IDR_PNG_MenuSelect);
    m_oMenu.SetImageArray(IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray);
    m_oMenu.PreloadImages(hWnd, m_hInst);

	m_dwOrigLanguage    = g_cLocale.GetCurrentLanguage();

    PopulateOptions();

	return TRUE;
}

BOOL CDlgOptions::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
    {	//this should only exist when using a smartphone in landscape orientation (for now anyways)
#ifdef WIN32_PLATFORM_WFSP

        SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 

        SetForegroundWindow(m_hWnd);
        SetFocus(m_hWnd);
#endif
        return TRUE;
    }
    return UNHANDLED;
}

BOOL CDlgOptions::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef WIN32_PLATFORM_WFSP
    OnKeyDown(hWnd, wParam, lParam);
#endif

	switch(LOWORD(wParam))
	{
#ifdef UNDER_CE
	case VK_TSOFT1:
		m_dwReturn = IDOK;
		SafeCloseWindow(m_dwReturn);
		break;
	case VK_TSOFT2:
#endif
	case VK_ESCAPE:
		m_dwReturn = IDCANCEL;
		SafeCloseWindow(m_dwReturn);
		return TRUE;
		break;
	default:
		break;
	}

    return FALSE;
}

BOOL CDlgOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    RECT rc;
    GetClientRect(hWnd, &rc);

#ifdef UNDER_CE
	if(WIDTH(rc) != GetSystemMetrics(SM_CXSCREEN))
		return TRUE;
#endif

	m_oBottomMenu->Init(hWnd, m_hInst, m_oBottomMenu->GetCurrentSkin());

    m_oMenu.OnSize(rc.left,rc.top, WIDTH(rc), HEIGHT(rc) - m_oBottomMenu->GetMenuHeight());

    return TRUE;
}


BOOL CDlgOptions::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	return TRUE;
}

BOOL CDlgOptions::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(m_gdiMem.GetDC() == NULL || m_gdiMem.GetWidth() != WIDTH(rcClient) || m_gdiMem.GetHeight() != HEIGHT(rcClient))
        m_gdiMem.Create(hDC, rcClient, FALSE, TRUE, FALSE);

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    // draw the background
    FillRect(m_gdiMem, rcClip, RGB(0,0,0));

    m_oMenu.OnDraw(m_gdiMem, rcClient, rcClip);

	if(m_oBottomMenu)
    {
        TCHAR szTemp[STRING_LARGE];
        m_oStr->StringCopy(szTemp, ID(IDS_MENU_Ok), STRING_LARGE, m_hInst);

		m_oBottomMenu->DrawMenu(m_gdiMem.GetDC(), rcClip, szTemp, m_oStr->GetText(ID(IDS_MENU_Cancel), m_hInst));
    }

    BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem.GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);
	
	return TRUE;
}

BOOL CDlgOptions::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_LEFT_SOFTKEY)
	{
		m_dwReturn = IDOK;
		SafeCloseWindow(IDOK);
		return TRUE;
	}
	else if(uiMessage == WM_RIGHT_SOFTKEY)
	{
		m_dwReturn = IDCANCEL;
		SafeCloseWindow(IDCANCEL);
		return TRUE;
	}
	return UNHANDLED;
}

BOOL CDlgOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
    {
	case IDMENU_Cancel:
	{
		// set the language back if it needs it
		DWORD dwCurrent = g_cLocale.GetCurrentLanguage();
		if(dwCurrent != m_dwOrigLanguage)
			g_cLocale.SetCurrentLanguage(m_dwOrigLanguage);

		m_dwReturn = IDCANCEL;
		SafeCloseWindow(IDCANCEL);
        break;
	}
    case IDMENU_Save:
		m_dwReturn = IDOK;
        SafeCloseWindow(IDOK);
        break;
    case IDMENU_Select:
    case IDOK:
        LaunchPopupMenu();
        break;
    case IDMENU_SelectMenu:
        SetMenuSelection();
        break;
	}

	return TRUE;
}

BOOL CDlgOptions::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnTimer(wParam, lParam);
}


BOOL CDlgOptions::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnKeyDown(wParam, lParam);
}

BOOL CDlgOptions::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(m_wndMenu->IsWindowUp(TRUE))
        return TRUE;
	if(m_oBottomMenu->OnLButtonDown(pt, hWnd) == TRUE)
	{

	}
	else if(m_oMenu.OnLButtonDown(pt))
	{
	}
	
    return TRUE;
}

BOOL CDlgOptions::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_wndMenu->IsWindowUp(TRUE))
        return TRUE;
	if(m_oBottomMenu->OnLButtonUp(pt, hWnd) == TRUE)
	{}
    else if(m_oMenu.OnLButtonUp(pt))
	{}
	
    return TRUE;
}

BOOL CDlgOptions::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_wndMenu->IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnMouseMove(pt))
    {}

    return TRUE;
}

void CDlgOptions::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgOptions* pThis = (CDlgOptions*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumOption* eInfo = (EnumOption*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eInfo);
}

BOOL CDlgOptions::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption)
{
    // draw the down arrow
    Draw(gdi,
        rc.right - m_oMenu.GetImageArrayGDI().GetHeight(), rc.top + (HEIGHT(rc)-m_oMenu.GetImageArrayGDI().GetHeight())/2,
        m_oMenu.GetImageArrayGDI().GetHeight(),m_oMenu.GetImageArrayGDI().GetHeight(),
        m_oMenu.GetImageArrayGDI(),
        (bIsHighlighted?2:3)*m_oMenu.GetImageArrayGDI().GetHeight(), 0);

    RECT rcText;

    TCHAR szTitle[STRING_MAX] = _T("");
    TCHAR szInfo[STRING_MAX]  = _T("");

    GetItemTitle(*eOption, szTitle);

    switch(*eOption)
    {
	case OPTION_Language:
		{
			DWORD dwLanguage = g_cLocale.GetCurrentLanguage();
			g_cLocale.GetLanguageText(szInfo, dwLanguage);
			break;
		}
    case OPTION_UseRegionalSettings:
        m_oStr->StringCopy(szInfo, m_sOptions.bUseRegionalSettings?m_oStr->GetText(ID(IDS_MENU_On), m_hInst):m_oStr->GetText(ID(IDS_MENU_Off), m_hInst));
        break;
    case OPTION_PlaySounds:
        //m_oStr->StringCopy(szInfo, m_sOptions.bPlaySounds?m_oStr->GetText(ID(IDS_MENU_On), m_hInst):m_oStr->GetText(ID(IDS_MENU_Off), m_hInst));
        GetSoundModeText(m_sOptions.bPlaySounds, szInfo);
        break;
    case OPTION_RecordTape:
        m_oStr->StringCopy(szInfo, m_sOptions.bRecordTape?m_oStr->GetText(ID(IDS_MENU_On), m_hInst):m_oStr->GetText(ID(IDS_MENU_Off), m_hInst));
        break;
    case OPTION_AdvancedMemory:
        m_oStr->StringCopy(szInfo, m_sOptions.bAdvancedMemory?m_oStr->GetText(ID(IDS_MENU_On), m_hInst):m_oStr->GetText(ID(IDS_MENU_Off), m_hInst));
        break;

    case OPTION_GraphAccuracy:
        m_oStr->StringCopy(szInfo, m_sOptions.bGraphFast?m_oStr->GetText(ID(IDS_MENU_On), m_hInst):m_oStr->GetText(ID(IDS_MENU_Off), m_hInst));
        break;
    case OPTION_GraphUsePoints:
        m_oStr->StringCopy(szInfo, m_sOptions.bGraphUsePoints?m_oStr->GetText(ID(IDS_MENU_On), m_hInst):m_oStr->GetText(ID(IDS_MENU_Off), m_hInst));
        break;
    case OPTION_CalculatorMode:
        GetCalcModeText(m_sOptions.eCalculatorMode, szInfo);
        break;
    case OPTION_Notation:
        GetNotationText(m_sOptions.eNotation, szInfo);
        break;
    case OPTION_ConversionBuffer:
        GetConversionBufferText(m_sOptions.iConversionBuffer, szInfo);
        break;
    case OPTION_RPNStackSize:
        GetRPNStackSizeText(m_sOptions.iRPNStackSize, szInfo);
        break;
    case OPTION_Skin:
        GetSkinsText(m_sOptions.eSkin, szInfo);
        break;
    case OPTION_FixedDigits:
        GetFixedDigitsText(m_sOptions.iFixedDigits, szInfo);
        break;

    }

    // title text
    rcText			= rc;
    rcText.left		+= INDENT;
    DrawTextShadow(gdi, szTitle, rcText, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    // value text
    rcText.right	= rc.right - m_oMenu.GetImageArrayGDI().GetHeight();
    DrawText(gdi, szInfo, rcText, DT_RIGHT | DT_VCENTER, m_hFontBtnText, (bIsHighlighted?TEXTCOLOR_Normal:TEXTCOLOR_Item));


    return TRUE;
}

void CDlgOptions::GetFixedDigitsText(int iFixedDigits, TCHAR* szInfo)
{
    if(iFixedDigits < 0 || iFixedDigits > 8)
        return;
    m_oStr->IntToString(szInfo, iFixedDigits);
}

void CDlgOptions::GetSoundModeText(BOOL bPlaySounds, TCHAR* szInfo)
{
    switch((int)bPlaySounds)
    {
    case 0:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_MENU_Off), m_hInst));
    	break;
    case 1:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_MENU_On), m_hInst));
    	break;
    case 2:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_MENU_Quiet), m_hInst));
        break;
    default:
        break;
    }
}

void CDlgOptions::GetSkinsText(EnumCalcProSkins eSkin, TCHAR* szInfo)
{
    switch(eSkin)
    {
    case SKIN_Blue:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_MetallicBlue), m_hInst));
        break;
    case SKIN_Black:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_MidnightBlack), m_hInst));
        break;
    case SKIN_Green:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_WirelessGreen), m_hInst));
        break;
    case SKIN_Pink:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_GlamPink), m_hInst));
        break;
    case SKIN_Silver:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_BlueSteel), m_hInst));
        break;
    case SKIN_Brown:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Brown), m_hInst));
        break;
    }
}

void CDlgOptions::GetRPNStackSizeText(int iRPNStackSize, TCHAR* szInfo)
{
    switch(iRPNStackSize)//Adrian said to leave these here. March 11. 2010
    {
    case 2:
        m_oStr->StringCopy(szInfo, _T("2(X,Y)"));
        break;
    case 4:
        m_oStr->StringCopy(szInfo, _T("4(X,Y,Z,T)"));
        break;
    case 5:
        m_oStr->StringCopy(szInfo, _T("5"));
        break;
    case 10:
        m_oStr->StringCopy(szInfo, _T("10"));
        break;
    case 50:
        m_oStr->StringCopy(szInfo, _T("50"));
        break;
    }
}

void CDlgOptions::GetConversionBufferText(int iConversionBuffer, TCHAR* szInfo)
{
    switch(iConversionBuffer)//Adrian said to leave these here. March 11. 2010
    {
    case 8:
        m_oStr->StringCopy(szInfo, _T("Byte (8 bit)"));
        break;
    case 16:
        m_oStr->StringCopy(szInfo, _T("Word (16 bit)"));
        break;
    case 32:
        m_oStr->StringCopy(szInfo, _T("DWord (32 bit)"));
        break;
    case 48:
        m_oStr->StringCopy(szInfo, _T("Large (48 bit)"));
        break;
    case 64:
        m_oStr->StringCopy(szInfo, _T("QWord (64 bit)"));
        break;

    }
}

void CDlgOptions::GetNotationText(DisplayType eNotation, TCHAR* szInfo)
{
    switch(eNotation)
    {
    default:
    case DISPLAY_Float:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Normal), m_hInst));
        break;
    case DISPLAY_Fixed:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Fixed), m_hInst));
        break;
    case DISPLAY_Scientific:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Scientific), m_hInst));
        break;
    case DISPLAY_Engineering:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Engineering), m_hInst));
        break;
    case DISPLAY_Fractions:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Fractions), m_hInst));
        break;
    case DISPLAY_DMS:
        m_oStr->StringCopy(szInfo, _T("D°M\x2032S\x2033"));//Adrian said to leave this here. March 11. 2010
        break;
    case DISPLAY_BCD:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_BinaryDec), m_hInst));
        break;
    }
}

void CDlgOptions::GetCalcModeText(CalcOperationType eCalculatorMode, TCHAR* szInfo)
{
    switch(eCalculatorMode)
    {
    case CALC_Chain:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Normal), m_hInst));
        break;
    case CALC_OrderOfOpps:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Algebraic), m_hInst));
        break;
    case CALC_DAL:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_DirectAlgebraic), m_hInst));
        break;
    case CALC_RPN:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_RPN), m_hInst));
        break;
    case CALC_String:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Expression), m_hInst));
        break;
    }
}

void CDlgOptions::GetItemTitle(EnumOption eOption, TCHAR* szTitle)
{
    switch(eOption)
    {
    case OPTION_UseRegionalSettings:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_RegionalSettings), m_hInst));
        break;
    case OPTION_PlaySounds:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_PlaySounds), m_hInst));
        break;
    case OPTION_RecordTape:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_RecordTape), m_hInst));
        break;
    case OPTION_AdvancedMemory:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_AdvancedMemory), m_hInst));
        break;
    case OPTION_CalculatorMode:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_CalculatorMode), m_hInst));
        break;
    case OPTION_Notation:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_Notation), m_hInst));
        break;
    case OPTION_ConversionBuffer:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_ConversionBuffer), m_hInst));
        break;
    case OPTION_RPNStackSize:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_RPNStackSize), m_hInst));
        break;
    case OPTION_Skin:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_Skin), m_hInst));
        break;
    case OPTION_FixedDigits:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_FixedDigits), m_hInst));
        break;
    case OPTION_GraphAccuracy:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_FastGraphing), m_hInst));
        break;
    case OPTION_GraphUsePoints:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_ShowPointsOnly), m_hInst));
        break;
	case OPTION_Language:
		g_cLocale.GetLanguageOptionText(szTitle, g_cLocale.GetCurrentLanguage());
		break;
    }
}

void CDlgOptions::PopulateOptions()
{
    m_oMenu.ResetContent();

    m_oMenu.AddCategory(m_oStr->GetText(ID(IDS_MENU_General), m_hInst));

    EnumOption* eOption = new EnumOption;
    *eOption = OPTION_Skin;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_PlaySounds;
    m_oMenu.AddItem(eOption, IDMENU_Select);

	eOption = new EnumOption;
	*eOption = OPTION_Language;
	m_oMenu.AddItem(eOption, IDMENU_Select);
    eOption = new EnumOption;
    *eOption = OPTION_UseRegionalSettings;
    m_oMenu.AddItem(eOption, IDMENU_Select);   

    m_oMenu.AddCategory(m_oStr->GetText(ID(IDS_MENU_Calculator), m_hInst));

    eOption = new EnumOption;
    *eOption = OPTION_CalculatorMode;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_Notation;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_FixedDigits;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    

    m_oMenu.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}

void CDlgOptions::LaunchPopupMenu()
{
    m_wndMenu->ResetContent();

    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;
    TCHAR szTitle[STRING_MAX];
    TCHAR szInfo[STRING_MAX];
    GetItemTitle(*eOption, szTitle);
    m_wndMenu->AddCategory(szTitle);
    int i;

    switch(*eOption)
    {
	case OPTION_Language:
		{
			TCHAR szText[STRING_MAX];
			DWORD dwCurrent = g_cLocale.GetCurrentLanguage();
			g_cLocale.GetLanguageText(szText, LANG_ENGLISH);
			m_wndMenu->AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_ENGLISH?FLAG_Radio:NULL), LANG_ENGLISH);
			g_cLocale.GetLanguageText(szText, LANG_SPANISH);
			m_wndMenu->AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_SPANISH?FLAG_Radio:NULL), LANG_SPANISH);
			g_cLocale.GetLanguageText(szText, LANG_FRENCH);
			m_wndMenu->AddItem(szText,  IDMENU_SelectMenu, (dwCurrent == LANG_FRENCH?FLAG_Radio:NULL), LANG_FRENCH);
			g_cLocale.GetLanguageText(szText, LANG_GERMAN);
			m_wndMenu->AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_GERMAN?FLAG_Radio:NULL), LANG_GERMAN);
			//g_cLocale.GetLanguageText(szText, LANG_DUTCH);
			//m_wndMenu->AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_DUTCH?FLAG_Radio:NULL), LANG_DUTCH);
			//g_cLocale.GetLanguageText(szText, LANG_RUSSIAN);
			//m_wndMenu->AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_RUSSIAN?FLAG_Radio:NULL), LANG_RUSSIAN);
			//g_cLocale.GetLanguageText(szText, LANG_ITALIAN);
			//m_wndMenu->AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_ITALIAN?FLAG_Radio:NULL), LANG_ITALIAN);
			g_cLocale.GetLanguageText(szText, LANG_PORTUGUESE);
			m_wndMenu->AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_PORTUGUESE?FLAG_Radio:NULL), LANG_PORTUGUESE); 
			//g_cLocale.GetLanguageText(szText, LANG_JAPANESE);
			//m_wndMenu->AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_JAPANESE?FLAG_Radio:NULL), LANG_JAPANESE);
			//g_cLocale.GetLanguageText(szText, LANG_KOREAN);
			//m_wndMenu->AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_KOREAN?FLAG_Radio:NULL), LANG_KOREAN);   
			//g_cLocale.GetLanguageText(szText, LANG_CHINESE);
			//m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_CHINESE?FLAG_Radio:NULL), LANG_CHINESE);
			break;
		}
    case OPTION_UseRegionalSettings:
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_On), m_hInst), IDMENU_SelectMenu, (m_sOptions.bUseRegionalSettings?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_Off), m_hInst), IDMENU_SelectMenu, (!m_sOptions.bUseRegionalSettings?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_PlaySounds:
     //   m_wndMenu->AddItem(_T("On"), IDMENU_SelectMenu, (m_sOptions.bPlaySounds?FLAG_Radio:NULL), TRUE);
     //   m_wndMenu->AddItem(_T("Off"), IDMENU_SelectMenu, (!m_sOptions.bPlaySounds?FLAG_Radio:NULL), FALSE);
        GetSoundModeText(0, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.bPlaySounds==0?FLAG_Radio:NULL), 0);
        GetSoundModeText(1, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.bPlaySounds==1?FLAG_Radio:NULL), 1);
        GetSoundModeText(2, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.bPlaySounds==2?FLAG_Radio:NULL), 2);
        break;
    case OPTION_RecordTape:
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_On), m_hInst), IDMENU_SelectMenu, (m_sOptions.bRecordTape?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_Off), m_hInst), IDMENU_SelectMenu, (!m_sOptions.bRecordTape?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_AdvancedMemory:
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_On), m_hInst), IDMENU_SelectMenu, (m_sOptions.bAdvancedMemory?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_Off), m_hInst), IDMENU_SelectMenu, (!m_sOptions.bAdvancedMemory?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_GraphAccuracy:
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_On), m_hInst), IDMENU_SelectMenu, (m_sOptions.bGraphFast?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_Off), m_hInst), IDMENU_SelectMenu, (!m_sOptions.bGraphFast?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_GraphUsePoints:
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_On), m_hInst), IDMENU_SelectMenu, (m_sOptions.bGraphUsePoints?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_Off), m_hInst), IDMENU_SelectMenu, (!m_sOptions.bGraphUsePoints?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_EndPeriodPayments:
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_On), m_hInst), IDMENU_SelectMenu, (m_sOptions.bTVMEndPeriod?FLAG_Radio:NULL), TRUE);
        m_wndMenu->AddItem(m_oStr->GetText(ID(IDS_MENU_Off), m_hInst), IDMENU_SelectMenu, (!m_sOptions.bTVMEndPeriod?FLAG_Radio:NULL), FALSE);
        break;

    case OPTION_CalculatorMode:        
        for(i=CALC_String; i<=CALC_RPN; i++)
        {
            CalcOperationType eCalcMode = (CalcOperationType)i;
            GetCalcModeText(eCalcMode, szInfo);
            m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.eCalculatorMode == eCalcMode?FLAG_Radio:NULL), eCalcMode);
        }
        break;
    case OPTION_Notation:
#ifdef DEBUG
        for(i=DISPLAY_Float; i<=DISPLAY_DMS; i++)
#else
        for(i=DISPLAY_Float; i<DISPLAY_DMS; i++)
#endif
        {
            DisplayType eDisplay = (DisplayType)i;
            GetNotationText(eDisplay, szInfo);
            m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.eNotation == eDisplay?FLAG_Radio:NULL), eDisplay);
        }
        break;
    case OPTION_ConversionBuffer:
        i = 8;
        GetConversionBufferText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iConversionBuffer == i?FLAG_Radio:NULL), i);
        i = 16;
        GetConversionBufferText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iConversionBuffer == i?FLAG_Radio:NULL), i);
        i = 32;
        GetConversionBufferText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iConversionBuffer == i?FLAG_Radio:NULL), i);
        i = 48;
        GetConversionBufferText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iConversionBuffer == i?FLAG_Radio:NULL), i);        
#ifdef DEBUG    
        i = 64;
        GetConversionBufferText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iConversionBuffer == i?FLAG_Radio:NULL), i);   
#endif
        break;
    case OPTION_RPNStackSize:
        i = 2;
        GetRPNStackSizeText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iRPNStackSize == i?FLAG_Radio:NULL), i);
        i = 4;
        GetRPNStackSizeText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iRPNStackSize == i?FLAG_Radio:NULL), i);
        i = 5;
        GetRPNStackSizeText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iRPNStackSize == i?FLAG_Radio:NULL), i);
        i = 10;
        GetRPNStackSizeText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iRPNStackSize == i?FLAG_Radio:NULL), i);
        i = 50;
        GetRPNStackSizeText(i, szInfo);
        m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iRPNStackSize == i?FLAG_Radio:NULL), i);
        break;
    case OPTION_Skin:
        for(i=SKIN_Blue; i<=SKIN_Silver; i++)
        {
            EnumCalcProSkins eSkins = (EnumCalcProSkins)i;
            GetSkinsText(eSkins, szInfo);
            m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.eSkin == eSkins?FLAG_Radio:NULL), eSkins);
        }
        break;
    case OPTION_FixedDigits:   
        for(i=0; i<=8; i++)
        {
            GetFixedDigitsText(i, szInfo);
            m_wndMenu->AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.iFixedDigits == i?FLAG_Radio:NULL), i);
        }
        break;
 
    }

    m_wndMenu->SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    m_wndMenu->PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        WIDTH(rcClient)/8+rcClient.left,rcClient.top,
        WIDTH(rcClient)*3/4, HEIGHT(rcClient) - m_oBottomMenu->GetMenuHeight(),
        0,0,0,0, ADJUST_Bottom);
}

void CDlgOptions::SetMenuSelection()
{
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    TypeItems* sMenuItem = m_wndMenu->GetSelectedItem();
    if(!sMenuItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;

    switch(*eOption)
    {
	case OPTION_Language:
		g_cLocale.SetCurrentLanguage((DWORD)sMenuItem->lParam);
		PopulateOptions();
		break;
    case OPTION_UseRegionalSettings:
        m_sOptions.bUseRegionalSettings = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_PlaySounds:
        m_sOptions.bPlaySounds = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_RecordTape:
        m_sOptions.bRecordTape = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_AdvancedMemory:
        m_sOptions.bAdvancedMemory = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_CalculatorMode:
        m_sOptions.eCalculatorMode = (CalcOperationType)sMenuItem->lParam;
        break;
    case OPTION_Notation:
        m_sOptions.eNotation = (DisplayType)sMenuItem->lParam;
        break;
    case OPTION_ConversionBuffer:
        m_sOptions.iConversionBuffer = (int)sMenuItem->lParam;
        break;
    case OPTION_RPNStackSize:
        m_sOptions.iRPNStackSize = (int)sMenuItem->lParam;
        break;
    case OPTION_Skin:
        m_sOptions.eSkin = (EnumCalcProSkins)sMenuItem->lParam;
        break;
    case OPTION_FixedDigits:
        m_sOptions.iFixedDigits = (int)sMenuItem->lParam;
        break;
    case OPTION_GraphAccuracy:
        m_sOptions.bGraphFast = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_GraphUsePoints:
        m_sOptions.bGraphUsePoints = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_EndPeriodPayments:
        m_sOptions.bTVMEndPeriod = (BOOL)sMenuItem->lParam;
        break;
    }
}

void CDlgOptions::DeleteItem(LPVOID lpItem)
{
    if(lpItem == NULL)
        return;

    EnumOption* eOption = (EnumOption*)lpItem;
    if(eOption)
        delete eOption;
}

BOOL CDlgOptions::OnMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifndef UNDER_CE
	RECT rcWindow;
	GetWindowRect(hWnd, &rcWindow);

	SetWindowPos(m_hWndParent, NULL, rcWindow.left, rcWindow.top, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
#endif
	return TRUE;
}
