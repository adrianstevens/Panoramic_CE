#include "StdAfx.h"
#include "resource.h"
#include "DlgOptions.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "ContactsGuiDefines.h"
#include "IssRect.h"
#include "IssImageSliced.h"
#include "IssCommon.h"
#include "DlgMsgBox.h"
#include "issdebug.h"
#include "IssLocalisation.h"

#include "ObjGui.h"
#include "IssWndTouchMenu.h"

extern CObjGui* g_gui;
extern CIssWndTouchMenu* g_wndMenu;
extern TypeOptions*			g_sOptions;

#define IDMENU_Buttons      5000
#define IDMENU_Selection    5001
#define BUTTON_Height       (GetSystemMetrics(SM_CXICON)*3/4)

CDlgOptions::CDlgOptions(void)
:m_bFadeIn(FALSE)
,m_bSetTodayMenu(TRUE)
,m_szBackgroundFile(NULL)
,m_bBackgroundChanged(FALSE)
{
    TCHAR szInstallDir[STRING_MAX] = _T("");
    GetExeDirectory(szInstallDir);
    m_objAssociations = new CObjAssociations(szInstallDir);

    ZeroMemory(&m_sOptions, sizeof(TypeOptions));
	m_hFontText		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*14/32, FW_BOLD, TRUE);
	m_hFontBtnText	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*14/32, FW_NORMAL, TRUE);
}

CDlgOptions::~CDlgOptions(void)
{
    CIssGDIEx::DeleteFont(m_hFontText);

	CIssGDIEx::DeleteFont(m_hFontBtnText);

	m_oStr->Delete(&m_szBackgroundFile);


    if(m_objAssociations)
        delete m_objAssociations;
}


BOOL CDlgOptions::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    m_oMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, g_gui->GetSkin().crSelected);
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList|OPTION_Bounce);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteMyItem);
    //m_oMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu.SetSelected(g_gui->GetSkin().uiSelector);
    m_oMenu.PreloadImages(hWnd, m_hInst);


    //m_bStartMenuOn      = IsStartMenuOn();
    m_bSetTodayMenu     = m_objAssociations->IsTodaySoftkeySetL();
    m_dwOrigLanguage    = g_cLocale.GetCurrentLanguage();

    // Add Our Options here
    PopulateOptions();

    m_bFadeIn = TRUE;
	return TRUE;
}

BOOL CDlgOptions::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    m_oMenu.OnDraw(gdi, rcClient, rcClip);

    // when the popup menu is up then darken out the rest of the screen a little
    if(g_wndMenu->IsWindowUp(FALSE))
        AlphaFillRect(gdi, m_rcArea, 0, 150);

/*	if(m_bFadeIn == TRUE)
    {
        FadeInScreen();
		return FALSE;
    }*/

	return TRUE;
}

void CDlgOptions::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, m_oStr->GetText(ID(IDS_MENU_Options), m_hInst), rc, DT_LEFT | DT_VCENTER, g_gui->GetFontTitle(), RGB(51,51,51));

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
    rc.top += INDENT/2;
    rc.bottom -= INDENT/2;

    Draw(gdi,
         rc.right - m_oMenu.GetImageArrayGDI().GetHeight(), rc.top + (HEIGHT(rc)/2-m_oMenu.GetImageArrayGDI().GetHeight())/2,
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
	case OPTION_StartButton:
		if(m_sOptions.bStartButton)
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_On)));
		else
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Off)));
		break;
    case OPTION_AlwaysShowText:
        if(m_sOptions.bAlwaysShowText)
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_On)));
        else
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Off)));
        break;
	case OPTION_Fullscreen:
		if(m_sOptions.bShowFullScreen)
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_On)));
		else
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Off)));
		break;
	case OPTION_DrawTiles:
		if(m_sOptions.bDrawTiles)
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_On)));
		else
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Off)));
		break;
    case OPTION_HideOnAppLaunch:
        if(m_sOptions.bHideOnAppLaunch)
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_On)));
        else
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Off)));
        break;
    case OPTION_TodayMenuItem:
        if(m_bSetTodayMenu)
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_On)));
        else
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Off)));
        break;
    case OPTION_StartMenu:
        if(m_bStartMenuOn)
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_On)));
        else
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Off)));
        break;
    case OPTION_SettingsHeaders:
        if(m_sOptions.bShowSettingsHeaders)
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_On)));
        else
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Off)));
        break;
    case OPTION_FavSize:
        switch(m_sOptions.eFavSize)
        {
        case FAVSIZE_Large:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Large)));
            break;
        case FAVSIZE_Small:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Small)));
            break;
        case FAVSIZE_Normal:
        default:        
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Normal)));
            break;
        }
        break;
    case OPTION_SlideLeft:
        GetSliderName(m_sOptions.eSlideLeft, szInfo);
        break;
    case OPTION_SlideRight:
        GetSliderName(m_sOptions.eSlideRight, szInfo);
        break;
    case OPTION_IconStyle:
        switch(m_eIconStyle)
        {
        case ICON_Windows:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Windows)));
            break;
        case ICON_Rocket:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Rocket)));
            break;
        case ICON_Default:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Default)));
            break;
        case ICON_Off:
        default:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Off)));
            break;
        }
        break;
	case OPTION_Background:
		switch(m_sOptions.eBackground)
		{
		case BK_Picture:
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_BK_SelectPicture)));
			break;
		case BK_Camera:
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_BK_TakePicture)));
			break;
		case BK_Off:
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Off)));
			break;
		case BK_Normal:
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Normal)));
			break;
		case BK_Blue:
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_COLOR_Blue)));
			break;
		case BK_Orange:
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_COLOR_Orange)));
			break;
		case BK_Red:
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_COLOR_Red)));
			break;
		case BK_Green:
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_COLOR_Green)));
			break;
		case BK_DefaultToday:
		default:
			m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_Default)));
			break;
		}
		break;
    case OPTION_ColorScheme:
        switch(m_sOptions.eColorScheme)
        {
        case COLOR_Blue:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_COLOR_Blue)));
            break;
        case COLOR_Green:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_COLOR_Green)));
            break;
        case COLOR_Orange:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_COLOR_Orange)));
            break;
        case COLOR_Pink:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_COLOR_Pink)));
            break;
        case COLOR_Purple:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_COLOR_Purple)));
            break;
        case COLOR_Red:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_COLOR_Red)));
            break;
        }
        break;
    }

    SIZE sz;
    HFONT hOld = (HFONT)SelectObject(gdi, m_hFontBtnText);
    GetTextExtentPoint(gdi, szInfo, m_oStr->GetLength(szInfo), &sz);
    SelectObject(gdi, hOld);

	// title text
	rcText			= rc;
	rcText.left		+= INDENT;
    rcText.right	= rc.right - m_oMenu.GetImageArrayGDI().GetHeight() - sz.cx;
	DrawTextShadow(gdi, szTitle, rcText, DT_LEFT | DT_WORDBREAK, g_gui->GetFontTitle(), RGB(255,255,255), RGB(0,0,0));

    // value text
	rcText.right	= rc.right - m_oMenu.GetImageArrayGDI().GetHeight();
    DrawText(gdi, szInfo, rcText, DT_RIGHT, m_hFontBtnText, RGB(167,167,167));

	return TRUE;
}

void CDlgOptions::GetSliderName(EnumSlideName& eSlider, TCHAR* szName)
{
    CIssString* oStr = CIssString::Instance();
    switch(eSlider)
    {
    case SLIDE_Programs:
        oStr->StringCopy(szName, oStr->GetText(ID(IDS_SLIDER_Programs)));
        break;
    case SLIDE_Games:
        oStr->StringCopy(szName, oStr->GetText(ID(IDS_SLIDER_Games)));
        break;
    case SLIDE_Apps:
        oStr->StringCopy(szName, oStr->GetText(ID(IDS_SLIDER_Apps)));
        break;
    case SLIDE_Settings:
        oStr->StringCopy(szName, oStr->GetText(ID(IDS_SLIDER_Settings)));
        break;
    case SLIDE_Utilities:
        oStr->StringCopy(szName, oStr->GetText(ID(IDS_SLIDER_Utilities)));
        break;
    case SLIDE_Music:
        oStr->StringCopy(szName, oStr->GetText(ID(IDS_SLIDER_Music)));
        break;
    case SLIDE_Folders:
        oStr->StringCopy(szName, oStr->GetText(ID(IDS_SLIDER_Folders)));
        break;
    case SLIDE_Files:
        oStr->StringCopy(szName, oStr->GetText(ID(IDS_SLIDER_Files)));
        break;
    case SLIDE_Videos:
        oStr->StringCopy(szName, oStr->GetText(ID(IDS_SLIDER_Videos)));
        break;
    case SLIDE_Pictures:
        oStr->StringCopy(szName, oStr->GetText(ID(IDS_SLIDER_Pictures)));
        break;
    case SLIDE_Favorites:
    default:
        oStr->StringCopy(szName, oStr->GetText(ID(IDS_SLIDER_Favorites)));
        break;
    }   
}

void CDlgOptions::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{

    RECT rc = m_rcBottomBar;

    if(!g_wndMenu->IsWindowUp(FALSE))
    {
        rc.right = m_rcBottomBar.right/2;
        DrawTextShadow(gdi, m_oStr->GetText(ID(IDS_MENU_Ok)), rc, DT_CENTER|DT_VCENTER, g_gui->GetFontTitle(), RGB(255,255,255), RGB(0,0,0));
    }

    rc = m_rcBottomBar;
    rc.left = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, m_oStr->GetText(ID(IDS_MENU_Cancel)), rc, DT_CENTER|DT_VCENTER, g_gui->GetFontTitle(), RGB(255,255,255), RGB(0,0,0));
}

void CDlgOptions::OnMenuLeft()
{
    BOOL bChanged = FALSE;

    if(m_bSetTodayMenu)
        m_objAssociations->SetTodaySoftkeyL();
    else
        m_objAssociations->RestoreTodaySoftkeyL();

    /*if(IsStartMenuOn() != m_bStartMenuOn)
    {
        if(m_bStartMenuOn)
            TurnStartMenuOn();
        else
            TurnStartMenuOff();
    }*/

    SafeCloseWindow(IDOK);
}

void CDlgOptions::OnMenuRight()
{
    // set the language back if it needs it
    DWORD dwCurrent = g_cLocale.GetCurrentLanguage();
    if(dwCurrent != m_dwOrigLanguage)
        g_cLocale.SetCurrentLanguage(m_dwOrigLanguage);

	if(g_sOptions->eColorScheme != m_sOptions.eColorScheme)
	{
		g_wndMenu->SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, g_gui->GetSkin().crSelected);
		g_wndMenu->SetImageArray(g_gui->GetSkin().uiImageArray);
		g_wndMenu->SetSelected(g_gui->GetSkin().uiSelector);
		g_wndMenu->PreloadImages(m_hWnd, m_hInst);
	}

    SafeCloseWindow(IDCANCEL);
}

BOOL CDlgOptions::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnTimer(wParam, lParam);
}


BOOL CDlgOptions::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnKeyDown(wParam, lParam);
}

BOOL CDlgOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
    {
    case IDOK:
        m_oMenu.OnKeyDown(VK_RETURN, 0);
        break;
    case IDMENU_Buttons:
        LaunchPopupMenu();
        break;
    case IDMENU_Selection:
        SetMenuSelection();
        break;
    default:
        return UNHANDLED;
    }

	return TRUE;
}

void CDlgOptions::FadeInScreen()
{
	m_bFadeIn = FALSE;
	HDC hDC = GetDC(m_hWnd);

    RECT rc;
    SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    CIssGDIEx gdiTemp;
    gdiTemp.Create(hDC, rc, FALSE);


	if(FadeIn(hDC, gdiTemp) != S_OK)
    {
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
	ReleaseDC(m_hWnd, hDC);
}

BOOL CDlgOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    // so we only do this once
	//if(WIDTH(rc) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rc) > GetSystemMetrics(SM_CYSCREEN))
	//	return FALSE;

	g_gui->OnSize(rc);

    m_oMenu.OnSize(m_rcArea.left + INDENT,m_rcArea.top, WIDTH(m_rcArea) - 2*INDENT, HEIGHT(m_rcArea));
    return UNHANDLED;
}

void CDlgOptions::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    EnumOption* eOption = (EnumOption*)lpItem;
    delete eOption;
}

void CDlgOptions::PopulateOptions()
{
    
    m_oMenu.ResetContent();

    //////////////////////////////////////////////////////////////////////////
    // General options
    m_oMenu.AddCategory(ID(IDS_CAT_General), m_hInst);

    EnumOption* eOption = new EnumOption;
    *eOption = OPTION_ColorScheme;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

	eOption = new EnumOption;
	*eOption = OPTION_Background;
	m_oMenu.AddItem(eOption, IDMENU_Buttons);

    eOption = new EnumOption;
    *eOption = OPTION_IconStyle;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

    //eOption = new EnumOption;
    //*eOption = OPTION_StartMenu;
    //m_oMenu.AddItem(eOption, IDMENU_Buttons);

	eOption = new EnumOption;
	*eOption = OPTION_StartButton;
	m_oMenu.AddItem(eOption, IDMENU_Buttons);

    eOption = new EnumOption;
    *eOption = OPTION_TodayMenuItem;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

	eOption = new EnumOption;
	*eOption = OPTION_Fullscreen;
	m_oMenu.AddItem(eOption, IDMENU_Buttons);

    eOption = new EnumOption;
    *eOption = OPTION_HideOnAppLaunch;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

    eOption = new EnumOption;
    *eOption = OPTION_Language;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

    //////////////////////////////////////////////////////////////////////////
    // Favorites options
    m_oMenu.AddCategory(ID(IDS_TOOLBAR_FAV), m_hInst);
    eOption = new EnumOption;
    *eOption = OPTION_FavSize;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

    /*eOption = new EnumOption;
    *eOption = OPTION_SlideLeft;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

    eOption = new EnumOption;
    *eOption = OPTION_SlideRight;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);*/

	eOption = new EnumOption;
	*eOption = OPTION_DrawTiles;
	m_oMenu.AddItem(eOption, IDMENU_Buttons);

    eOption = new EnumOption;
    *eOption = OPTION_AlwaysShowText;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);
    //////////////////////////////////////////////////////////////////////////
    // Contacts options
//    m_oMenu.AddCategory(_T("Programs"));


    //////////////////////////////////////////////////////////////////////////
    // History options
    m_oMenu.AddCategory(ID(IDS_TOOLBAR_SETTINGS), m_hInst);
    eOption = new EnumOption;
    *eOption = OPTION_SettingsHeaders;
    m_oMenu.AddItem(eOption, IDMENU_Buttons);

    SIZE sz;
    HDC dc = GetDC(m_hWnd);
    HFONT hOld = (HFONT)SelectObject(dc, g_gui->GetFontTitle());
    GetTextExtentPoint(dc, _T("APp"), 3, &sz);
    SelectObject(dc, hOld);
    ReleaseDC(m_hWnd, dc);

    m_oMenu.SetItemHeights(2*sz.cy + INDENT, 2*sz.cy + INDENT);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}

BOOL CDlgOptions::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(g_wndMenu->IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonDown(pt))
    {}

	return TRUE;
}

BOOL CDlgOptions::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(g_wndMenu->IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonUp(pt))
        return TRUE;
    
    return CDlgBase::OnLButtonUp(hWnd, pt);
}

BOOL CDlgOptions::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(g_wndMenu->IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnMouseMove(pt))
    {}

    return TRUE;
}

void CDlgOptions::SetMenuSelection()
{
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    TypeItems* sMenuItem = g_wndMenu->GetSelectedItem();
    if(!sMenuItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;

    switch(*eOption)
    {
    case OPTION_Language:
        g_cLocale.SetCurrentLanguage((DWORD)sMenuItem->lParam);
        PopulateOptions();
        break;
	case OPTION_StartButton:
		m_sOptions.bStartButton = (BOOL)sMenuItem->lParam;
		break;
    case OPTION_AlwaysShowText:
        m_sOptions.bAlwaysShowText = (BOOL)sMenuItem->lParam;
        break;
	case OPTION_Fullscreen:
		m_sOptions.bShowFullScreen = (BOOL)sMenuItem->lParam;
		break;
	case OPTION_DrawTiles:
		m_sOptions.bDrawTiles = (BOOL)sMenuItem->lParam;
		break;
    case OPTION_HideOnAppLaunch:
        m_sOptions.bHideOnAppLaunch = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_TodayMenuItem:
        m_bSetTodayMenu = (BOOL)sMenuItem->lParam;
        if(m_bSetTodayMenu == TRUE)
        {
            CDlgMsgBox msgBox;
            msgBox.PopupMessage(ID(IDS_SoftReset), ID(IDS_MENU_Options), m_hWnd, m_hInst, MB_OK);
        }
        break;
    case OPTION_StartMenu:
        m_bStartMenuOn = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_SettingsHeaders:
        m_sOptions.bShowSettingsHeaders = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_FavSize:
        m_sOptions.eFavSize = (EnumFavSize)(sMenuItem->lParam);
        break;
    case OPTION_IconStyle:
        m_eIconStyle = (EnumIconStyle)(sMenuItem->lParam);
        break;
    case OPTION_SlideLeft:
        m_sOptions.eSlideLeft = (EnumSlideName)(sMenuItem->lParam);
        break;
    case OPTION_SlideRight:
        m_sOptions.eSlideRight = (EnumSlideName)(sMenuItem->lParam);
        break;
    case OPTION_ColorScheme:
        {
            m_sOptions.eColorScheme = (EnumColorScheme)sMenuItem->lParam;		

            EnumColorScheme eOld = g_sOptions->eColorScheme;
			g_sOptions->eColorScheme = m_sOptions.eColorScheme;
			g_gui->SetSkin();

            // show the new color scheme
            m_oMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, g_gui->GetSkin().crSelected);
            m_oMenu.SetImageArray(g_gui->GetSkin().uiImageArray);
			m_oMenu.SetSelected(g_gui->GetSkin().uiSelector);
            m_oMenu.PreloadImages(m_hWnd, m_hInst);

            g_wndMenu->SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, g_gui->GetSkin().crSelected);
			g_wndMenu->SetImageArray(g_gui->GetSkin().uiImageArray);
            g_wndMenu->SetSelected(g_gui->GetSkin().uiSelector);
            g_wndMenu->PreloadImages(m_hWnd, m_hInst);

            g_sOptions->eColorScheme	= eOld;
			g_gui->SetSkin();

			// set the skin back

            InvalidateRect(m_hWnd, NULL, FALSE);
        }
        break;
	case OPTION_Background:
		SetBackgroundOption((EnumBackground)sMenuItem->lParam);
		break;
    }

}

void CDlgOptions::SetBackgroundOption(EnumBackground eBackground)
{
	m_bBackgroundChanged = FALSE;
	m_oStr->Delete(&m_szBackgroundFile);

	// set background stuff
	if(eBackground == BK_Camera)
	{
		SHCAMERACAPTURE shcc;
		ZeroMemory(&shcc, sizeof(shcc));
		shcc.cbSize             = sizeof(shcc);
		shcc.hwndOwner          = m_hWnd;
		shcc.pszInitialDir      = NULL;
		shcc.pszDefaultFileName = NULL;
		shcc.pszTitle           = m_oStr->GetText(ID(IDS_BK_TakePicture));
		shcc.StillQuality       = CAMERACAPTURE_STILLQUALITY_HIGH;
		shcc.VideoTypes         = CAMERACAPTURE_VIDEOTYPE_STANDARD ;
		shcc.nResolutionWidth   = 0;
		shcc.nResolutionHeight  = 0;
		shcc.nVideoTimeLimit    = 0;
		shcc.Mode               = CAMERACAPTURE_MODE_STILL;

		// Call SHCameraCapture() function
		SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
		HRESULT hReturn = SHCameraCapture(&shcc);
		if(hReturn == S_OK && !m_oStr->IsEmpty(shcc.szFile))
		{
			m_bBackgroundChanged = TRUE;
			m_sOptions.eBackground = BK_Camera;
			m_szBackgroundFile = m_oStr->CreateAndCopy(shcc.szFile);
		}

	}
	else if(eBackground == BK_Picture)
	{
		OPENFILENAMEEX  ofnex   = {0};
		TCHAR           szPictureFile[MAX_PATH] = _T("");
		BOOL            bResult = FALSE;

		// Setup the Picture picker structure
		ofnex.lStructSize   = sizeof(ofnex);
		ofnex.ExFlags       = OFN_EXFLAG_THUMBNAILVIEW;    
		ofnex.lpstrFile     = szPictureFile;
		ofnex.nMaxFile      = MAX_PATH;

		SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
		bResult = GetOpenFileNameEx(&ofnex);
		if(bResult)
		{
			m_bBackgroundChanged = TRUE;
			m_sOptions.eBackground = BK_Picture;
			m_szBackgroundFile = m_oStr->CreateAndCopy(szPictureFile);
		}
	}
	else if(eBackground != m_sOptions.eBackground)
	{
		m_bBackgroundChanged = TRUE;
		m_sOptions.eBackground = eBackground;
	}
}

void CDlgOptions::LaunchPopupMenu()
{
    g_wndMenu->ResetContent();

    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;
    TCHAR szTitle[STRING_MAX];
    GetItemTitle(*eOption, szTitle);
    g_wndMenu->AddCategory(szTitle);

    switch(*eOption)
    {
    case OPTION_Language:
    {
        TCHAR szText[STRING_MAX];
        DWORD dwCurrent = g_cLocale.GetCurrentLanguage();
        g_cLocale.GetLanguageText(szText, LANG_ENGLISH);
        g_wndMenu->AddItem(szText, IDMENU_Selection, (dwCurrent == LANG_ENGLISH?FLAG_Radio:NULL), LANG_ENGLISH);
        g_cLocale.GetLanguageText(szText, LANG_SPANISH);
        g_wndMenu->AddItem(szText, IDMENU_Selection, (dwCurrent == LANG_SPANISH?FLAG_Radio:NULL), LANG_SPANISH);
        g_cLocale.GetLanguageText(szText, LANG_FRENCH);
        g_wndMenu->AddItem(szText,  IDMENU_Selection, (dwCurrent == LANG_FRENCH?FLAG_Radio:NULL), LANG_FRENCH);
		g_cLocale.GetLanguageText(szText, LANG_RUSSIAN);
		g_wndMenu->AddItem(szText,  IDMENU_Selection, (dwCurrent == LANG_RUSSIAN?FLAG_Radio:NULL), LANG_RUSSIAN);
		g_cLocale.GetLanguageText(szText, LANG_NORWEGIAN);
		g_wndMenu->AddItem(szText,  IDMENU_Selection, (dwCurrent == LANG_NORWEGIAN?FLAG_Radio:NULL), LANG_NORWEGIAN);
		g_cLocale.GetLanguageText(szText, LANG_ROMANIAN);
		g_wndMenu->AddItem(szText,  IDMENU_Selection, (dwCurrent == LANG_ROMANIAN?FLAG_Radio:NULL), LANG_ROMANIAN);
        //g_cLocale.GetLanguageText(szText, LANG_HINDI);
        //g_wndMenu->AddItem(szText, IDMENU_Selection, (dwCurrent == LANG_HINDI?FLAG_Radio:NULL), LANG_HINDI);
        //g_cLocale.GetLanguageText(szText, LANG_ITALIAN);
        //g_wndMenu->AddItem(szText, IDMENU_Selection, (dwCurrent == LANG_ITALIAN?FLAG_Radio:NULL), LANG_ITALIAN);
        //g_cLocale.GetLanguageText(szText, LANG_JAPANESE);
        //g_wndMenu->AddItem(szText, IDMENU_Selection, (dwCurrent == LANG_JAPANESE?FLAG_Radio:NULL), LANG_JAPANESE);
        g_cLocale.GetLanguageText(szText, LANG_PORTUGUESE);
        g_wndMenu->AddItem(szText, IDMENU_Selection, (dwCurrent == LANG_PORTUGUESE?FLAG_Radio:NULL), LANG_PORTUGUESE);
        g_cLocale.GetLanguageText(szText, LANG_GERMAN);
        g_wndMenu->AddItem(szText, IDMENU_Selection, (dwCurrent == LANG_GERMAN?FLAG_Radio:NULL), LANG_GERMAN);
        g_cLocale.GetLanguageText(szText, LANG_DUTCH);
        g_wndMenu->AddItem(szText, IDMENU_Selection, (dwCurrent == LANG_DUTCH?FLAG_Radio:NULL), LANG_DUTCH);
        g_cLocale.GetLanguageText(szText, LANG_CHINESE);
        g_wndMenu->AddItem(szText, IDMENU_Selection, (dwCurrent == LANG_CHINESE?FLAG_Radio:NULL), LANG_CHINESE);
        break;
     }
	 case OPTION_StartButton:
		g_wndMenu->AddItem(ID(IDS_On), m_hInst, IDMENU_Selection, (m_sOptions.bStartButton?FLAG_Radio:NULL), TRUE);
		g_wndMenu->AddItem(ID(IDS_Off), m_hInst, IDMENU_Selection, (!m_sOptions.bStartButton?FLAG_Radio:NULL), FALSE);
		break;
     case OPTION_AlwaysShowText:
        g_wndMenu->AddItem(ID(IDS_On), m_hInst, IDMENU_Selection, (m_sOptions.bAlwaysShowText?FLAG_Radio:NULL), TRUE);
        g_wndMenu->AddItem(ID(IDS_Off), m_hInst, IDMENU_Selection, (!m_sOptions.bAlwaysShowText?FLAG_Radio:NULL), FALSE);
        break;
	 case OPTION_Fullscreen:
		 g_wndMenu->AddItem(ID(IDS_On), m_hInst, IDMENU_Selection, (m_sOptions.bShowFullScreen?FLAG_Radio:NULL), TRUE);
		 g_wndMenu->AddItem(ID(IDS_Off), m_hInst, IDMENU_Selection, (!m_sOptions.bShowFullScreen?FLAG_Radio:NULL), FALSE);
		 break;
	 case OPTION_DrawTiles:
		 g_wndMenu->AddItem(ID(IDS_On), m_hInst, IDMENU_Selection, (m_sOptions.bDrawTiles?FLAG_Radio:NULL), TRUE);
		 g_wndMenu->AddItem(ID(IDS_Off), m_hInst, IDMENU_Selection, (!m_sOptions.bDrawTiles?FLAG_Radio:NULL), FALSE);
		 break;
    case OPTION_HideOnAppLaunch:
        g_wndMenu->AddItem(ID(IDS_On), m_hInst, IDMENU_Selection, (m_sOptions.bHideOnAppLaunch?FLAG_Radio:NULL), TRUE);
        g_wndMenu->AddItem(ID(IDS_Off), m_hInst, IDMENU_Selection, (!m_sOptions.bHideOnAppLaunch?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_TodayMenuItem:
        g_wndMenu->AddItem(ID(IDS_On), m_hInst, IDMENU_Selection, (m_bSetTodayMenu?FLAG_Radio:NULL), TRUE);
        g_wndMenu->AddItem(ID(IDS_Off), m_hInst, IDMENU_Selection, (!m_bSetTodayMenu?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_StartMenu:
        g_wndMenu->AddItem(ID(IDS_On), m_hInst, IDMENU_Selection, (m_bStartMenuOn?FLAG_Radio:NULL), TRUE);
        g_wndMenu->AddItem(ID(IDS_Off), m_hInst, IDMENU_Selection, (!m_bStartMenuOn?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_SettingsHeaders:
        g_wndMenu->AddItem(ID(IDS_On), m_hInst, IDMENU_Selection, (m_sOptions.bShowSettingsHeaders?FLAG_Radio:NULL), TRUE);
        g_wndMenu->AddItem(ID(IDS_Off), m_hInst, IDMENU_Selection, (!m_sOptions.bShowSettingsHeaders?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_ColorScheme:
        g_wndMenu->AddItem(ID(IDS_COLOR_Blue), m_hInst, IDMENU_Selection, (m_sOptions.eColorScheme==COLOR_Blue?FLAG_Radio:NULL), COLOR_Blue);
        g_wndMenu->AddItem(ID(IDS_COLOR_Green), m_hInst, IDMENU_Selection, (m_sOptions.eColorScheme==COLOR_Green?FLAG_Radio:NULL), COLOR_Green);
        g_wndMenu->AddItem(ID(IDS_COLOR_Orange), m_hInst, IDMENU_Selection, (m_sOptions.eColorScheme==COLOR_Orange?FLAG_Radio:NULL), COLOR_Orange);
        g_wndMenu->AddItem(ID(IDS_COLOR_Pink), m_hInst, IDMENU_Selection, (m_sOptions.eColorScheme==COLOR_Pink?FLAG_Radio:NULL), COLOR_Pink);
        g_wndMenu->AddItem(ID(IDS_COLOR_Purple), m_hInst, IDMENU_Selection, (m_sOptions.eColorScheme==COLOR_Purple?FLAG_Radio:NULL), COLOR_Purple);
        g_wndMenu->AddItem(ID(IDS_COLOR_Red), m_hInst, IDMENU_Selection, (m_sOptions.eColorScheme==COLOR_Red?FLAG_Radio:NULL), COLOR_Red);
        break;
    case OPTION_FavSize:
        g_wndMenu->AddItem(ID(IDS_Large), m_hInst, IDMENU_Selection, (m_sOptions.eFavSize==FAVSIZE_Large?FLAG_Radio:NULL), FAVSIZE_Large);
        g_wndMenu->AddItem(ID(IDS_Normal), m_hInst, IDMENU_Selection, (m_sOptions.eFavSize==FAVSIZE_Normal?FLAG_Radio:NULL), FAVSIZE_Normal);
        g_wndMenu->AddItem(ID(IDS_Small), m_hInst, IDMENU_Selection, (m_sOptions.eFavSize==FAVSIZE_Small?FLAG_Radio:NULL), FAVSIZE_Small);
        break;
    case OPTION_IconStyle:
        g_wndMenu->AddItem(ID(IDS_Windows), m_hInst, IDMENU_Selection, (m_eIconStyle==ICON_Windows?FLAG_Radio:NULL), ICON_Windows);
        g_wndMenu->AddItem(ID(IDS_Rocket), m_hInst, IDMENU_Selection, (m_eIconStyle==ICON_Rocket?FLAG_Radio:NULL), ICON_Rocket);
        g_wndMenu->AddItem(ID(IDS_Default), m_hInst, IDMENU_Selection, (m_eIconStyle==ICON_Default?FLAG_Radio:NULL), ICON_Default);
        g_wndMenu->AddItem(ID(IDS_Off), m_hInst, IDMENU_Selection, (m_eIconStyle==ICON_Off?FLAG_Radio:NULL), ICON_Off);
        break;
    case OPTION_SlideLeft:
        g_wndMenu->AddItem(ID(IDS_SLIDER_Programs), m_hInst, IDMENU_Selection, (m_sOptions.eSlideLeft==SLIDE_Programs?FLAG_Radio:NULL), SLIDE_Programs);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Games), m_hInst, IDMENU_Selection, (m_sOptions.eSlideLeft==SLIDE_Games?FLAG_Radio:NULL), SLIDE_Games);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Apps), m_hInst, IDMENU_Selection, (m_sOptions.eSlideLeft==SLIDE_Apps?FLAG_Radio:NULL), SLIDE_Apps);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Settings), m_hInst, IDMENU_Selection, (m_sOptions.eSlideLeft==SLIDE_Settings?FLAG_Radio:NULL), SLIDE_Settings);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Favorites), m_hInst, IDMENU_Selection, (m_sOptions.eSlideLeft==SLIDE_Favorites?FLAG_Radio:NULL), SLIDE_Favorites);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Utilities), m_hInst, IDMENU_Selection, (m_sOptions.eSlideLeft==SLIDE_Utilities?FLAG_Radio:NULL), SLIDE_Utilities);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Music), m_hInst, IDMENU_Selection, (m_sOptions.eSlideLeft==SLIDE_Music?FLAG_Radio:NULL), SLIDE_Music);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Folders), m_hInst, IDMENU_Selection, (m_sOptions.eSlideLeft==SLIDE_Folders?FLAG_Radio:NULL), SLIDE_Folders);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Files), m_hInst, IDMENU_Selection, (m_sOptions.eSlideLeft==SLIDE_Files?FLAG_Radio:NULL), SLIDE_Files);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Videos), m_hInst, IDMENU_Selection, (m_sOptions.eSlideLeft==SLIDE_Videos?FLAG_Radio:NULL), SLIDE_Videos);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Pictures), m_hInst, IDMENU_Selection, (m_sOptions.eSlideLeft==SLIDE_Pictures?FLAG_Radio:NULL), SLIDE_Pictures);
        break;
    case OPTION_SlideRight:
        g_wndMenu->AddItem(ID(IDS_SLIDER_Programs), m_hInst, IDMENU_Selection, (m_sOptions.eSlideRight==SLIDE_Programs?FLAG_Radio:NULL), SLIDE_Programs);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Games), m_hInst, IDMENU_Selection, (m_sOptions.eSlideRight==SLIDE_Games?FLAG_Radio:NULL), SLIDE_Games);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Apps), m_hInst, IDMENU_Selection, (m_sOptions.eSlideRight==SLIDE_Apps?FLAG_Radio:NULL), SLIDE_Apps);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Settings), m_hInst, IDMENU_Selection, (m_sOptions.eSlideRight==SLIDE_Settings?FLAG_Radio:NULL), SLIDE_Settings);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Favorites), m_hInst, IDMENU_Selection, (m_sOptions.eSlideRight==SLIDE_Favorites?FLAG_Radio:NULL), SLIDE_Favorites);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Utilities), m_hInst, IDMENU_Selection, (m_sOptions.eSlideRight==SLIDE_Utilities?FLAG_Radio:NULL), SLIDE_Utilities);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Music), m_hInst, IDMENU_Selection, (m_sOptions.eSlideRight==SLIDE_Music?FLAG_Radio:NULL), SLIDE_Music);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Folders), m_hInst, IDMENU_Selection, (m_sOptions.eSlideRight==SLIDE_Folders?FLAG_Radio:NULL), SLIDE_Folders);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Files), m_hInst, IDMENU_Selection, (m_sOptions.eSlideRight==SLIDE_Files?FLAG_Radio:NULL), SLIDE_Files);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Videos), m_hInst, IDMENU_Selection, (m_sOptions.eSlideRight==SLIDE_Videos?FLAG_Radio:NULL), SLIDE_Videos);
        g_wndMenu->AddItem(ID(IDS_SLIDER_Pictures), m_hInst, IDMENU_Selection, (m_sOptions.eSlideRight==SLIDE_Pictures?FLAG_Radio:NULL), SLIDE_Pictures);
        break;
	case OPTION_Background:
	{
		TCHAR szText[STRING_MAX];
		g_wndMenu->AddItem(ID(IDS_Default), m_hInst, IDMENU_Selection, (m_sOptions.eBackground==BK_DefaultToday?FLAG_Radio:NULL), BK_DefaultToday);
		m_oStr->StringCopy(szText, ID(IDS_BK_SelectPicture), STRING_MAX, m_hInst);
		m_oStr->Concatenate(szText, _T("..."));
		g_wndMenu->AddItem(szText, IDMENU_Selection, (m_sOptions.eBackground==BK_Picture?FLAG_Radio:NULL), BK_Picture);
		m_oStr->StringCopy(szText, ID(IDS_BK_TakePicture), STRING_MAX, m_hInst);
		m_oStr->Concatenate(szText, _T("..."));
		g_wndMenu->AddItem(szText, IDMENU_Selection, (m_sOptions.eBackground==BK_Camera?FLAG_Radio:NULL), BK_Camera);
		g_wndMenu->AddItem(ID(IDS_Off), m_hInst, IDMENU_Selection, (m_sOptions.eBackground==BK_Off?FLAG_Radio:NULL), BK_Off);
		g_wndMenu->AddItem(ID(IDS_Normal), m_hInst, IDMENU_Selection, (m_sOptions.eBackground==BK_Normal?FLAG_Radio:NULL), BK_Normal);
		g_wndMenu->AddItem(ID(IDS_COLOR_Blue), m_hInst, IDMENU_Selection, (m_sOptions.eBackground==BK_Blue?FLAG_Radio:NULL), BK_Blue);
		g_wndMenu->AddItem(ID(IDS_COLOR_Orange), m_hInst, IDMENU_Selection, (m_sOptions.eBackground==BK_Orange?FLAG_Radio:NULL), BK_Orange);
		g_wndMenu->AddItem(ID(IDS_COLOR_Red), m_hInst, IDMENU_Selection, (m_sOptions.eBackground==BK_Red?FLAG_Radio:NULL), BK_Red);
		g_wndMenu->AddItem(ID(IDS_COLOR_Green), m_hInst, IDMENU_Selection, (m_sOptions.eBackground==BK_Green?FLAG_Radio:NULL), BK_Green);
		break;
	}
    }

    g_wndMenu->SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    g_wndMenu->PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.right/4,m_rcTitle.bottom,
        rcClient.right*3/4, m_rcBottomBar.top - m_rcTitle.bottom,
        0,0,0,0, ADJUST_Bottom);
}

void CDlgOptions::GetItemTitle(EnumOption eOption, TCHAR* szTitle)
{
    switch(eOption)
    {
	case OPTION_StartButton:
		m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_StartMenu)));
		break;
    case OPTION_AlwaysShowText:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_ShowName)));
        break;
    case OPTION_HideOnAppLaunch:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_HideOnAppLaunch)));
        break;
    case OPTION_TodayMenuItem:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_SetTodayMenu)));
        break;
    case OPTION_IconStyle:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_SetStartMenu)));
        break;
    case OPTION_ColorScheme:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_ColorScheme)));
        break;
    case OPTION_FavSize:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_IconSize)));
        break;
    case OPTION_SlideLeft:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_LeftSliderTitle)));
        break;
    case OPTION_SlideRight:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_RightSliderTitle)));
        break;
    case OPTION_StartMenu:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_StartMenu)));
        break;
    case OPTION_SettingsHeaders:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_SeparateBySection)));
        break;
    case OPTION_Language:
        g_cLocale.GetLanguageOptionText(szTitle, g_cLocale.GetCurrentLanguage());
        break;
	case OPTION_Background:
		m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_Background)));
		break;
	case OPTION_Fullscreen:
		m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_UseFullscreen)));
		break;
	case OPTION_DrawTiles:
		m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPTION_DrawTiles)));
		break;
    }
}

BOOL CDlgOptions::IsStartMenuOn()
{
    HWND hWndPrevious = FindWindow(_T("ClassLauncherStart"), NULL);
    return hWndPrevious?TRUE:FALSE;
}

void CDlgOptions::TurnStartMenuOn()
{
    /*DBG_OUT((_T("CDlgOptions::TurnStartMenuOn")));
    TCHAR szPath[MAX_PATH];
    SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);
    m_oStr->Concatenate(szPath, _T("\\panostart.exe"));
    CreateProcess(szPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);

    DBG_OUT((_T("CDlgOptions::TurnStartMenuOn - %s"), szPath));

    SetAutoLaunch(szPath, _T("PanoStart"));*/
}

void CDlgOptions::TurnStartMenuOff()
{

    DBG_OUT((_T("CDlgOptions::TurnStartMenuOff")));

    HWND hWndPrevious = FindWindow(_T("ClassLauncherStart"), NULL);
    if(hWndPrevious)
        PostMessage(hWndPrevious, WM_USER + 6123, 0,0);

    RemoveAutoLaunch(_T("PanoStart"));
}

void CDlgOptions::RestartStartMenu()
{
    /*HWND hWndPrevious = NULL;
    hWndPrevious = FindWindow(_T("ClassLauncherStart"), NULL);

    if(hWndPrevious != NULL)
        TurnStartMenuOff();
 
    Sleep(500);

    TurnStartMenuOn();*/
}

void CDlgOptions::RemoveAutoLaunch(TCHAR* szName)
{
    TCHAR szStartupFolder[MAX_PATH];
    SHGetSpecialFolderPath(NULL, szStartupFolder, CSIDL_STARTUP, FALSE);

    CIssString* oStr = CIssString::Instance();
    oStr->Concatenate(szStartupFolder, _T("\\"));
    oStr->Concatenate(szStartupFolder, szName);
    oStr->Concatenate(szStartupFolder, _T(".lnk"));

    DeleteFile(szStartupFolder);
}

void CDlgOptions::SetAutoLaunch(TCHAR* szFileName, TCHAR* szName)
{
    TCHAR szStartupFolder[MAX_PATH];
    SHGetSpecialFolderPath(NULL, szStartupFolder, CSIDL_STARTUP, FALSE);

    CIssString* oStr = CIssString::Instance();
    oStr->Concatenate(szStartupFolder, _T("\\"));
    oStr->Concatenate(szStartupFolder, szName);
    oStr->Concatenate(szStartupFolder, _T(".lnk"));

    TCHAR szFullFileName[STRING_MAX];
    oStr->Format(szFullFileName, _T("\"%s\""), szFileName);

    SHCreateShortcut(szStartupFolder, szFullFileName);

    return;
}