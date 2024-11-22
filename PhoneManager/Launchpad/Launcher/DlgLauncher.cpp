#include "StdAfx.h"
#include "DlgLauncher.h"
#include "IssHardwareKeys.h"
#include "IssGDIFX.h"
#include "CommonDefines.h"
#include "IssCommon.h"
#include "Resource.h"
#include "DlgBase.h"
#

#include "IssRegistry.h"
#include "DlgAbout.h"
#include "IssStateAndNotify.h"
#include "IssLocalisation.h"
#include "ObjHardwareKeys.h"
#include "DlgChooseSettings.h"
#include "DlgChoosePrograms.h"
#include "DlgChooseContact.h"

#include "PoomContacts.h"

#include "DlgOptions.h"
#include "ObjGui.h"
#include "IssWndTouchMenu.h"
#include "GuiToolBar.h"

extern CObjGui*				g_gui;
extern CIssWndTouchMenu*	g_wndMenu;
extern TypeOptions*			g_sOptions;
extern CGuiToolBar*			g_guiToolbar;


#define CLASS_Start	_T("ClassLauncherStart")

HRESULT ToolbarAnimateFromTo(HDC hdcScreen,
							 CIssGDIEx&	gdiBackground,
							 CIssGDIEx&	gdiAniObject,
							 POINT		ptFrom,
							 POINT		ptTo,
							 int		iWidth,
							 int		iHeight,
							 int		iDrawFrames,
							 int       iTime   // in ms
							 );

CDlgLauncher::CDlgLauncher()
:m_pGui(NULL)
,m_bPreventUsage(FALSE)
{
	/*switch(eSelection)
	{
	case GUI_Favorites:
		m_pGui = (CGuiBase*)&m_guiFavorites;
		break;
	case GUI_Programs:
		m_pGui = (CGuiBase*)&m_guiPrograms;
		break;
	case GUI_Settings:
		m_pGui = (CGuiBase*)&m_guiSettings;
		break;
	}*/

	m_pGui = &m_guiFavorites;

    CIssStateAndNotify::InitializeNotifications();
}

CDlgLauncher::~CDlgLauncher(void)
{
    m_dlgStart.CloseAll();
    m_dlgStart.Destroy();

    CIssStateAndNotify::DestroyNotifications();
	CDlgChooseSettings::DestroyAllContent();
	CDlgChoosePrograms::DeleteAllContent();
#ifndef NOPHONE
	CDlgChooseContact::DeleteAllContent();
#endif
	CIssKineticList::DeleteAllContent();
}

BOOL CDlgLauncher::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

	g_gui->Init(m_hWnd, m_hInst);

    g_cLocale.Init(REG_Key, m_hInst, SUPPORT_English | SUPPORT_TradChinese | SUPPORT_German | SUPPORT_French | SUPPORT_Spanish | SUPPORT_Portuguese | SUPPORT_Dutch | SUPPORT_Romanian | SUPPORT_Russian | SUPPORT_Norwegian, TRUE);

	// MUST init poom with handle to this window so we can get notifications FIRST
	// SUBSEQUENT calls use CPoomContacts::Instance();
	CPoomContacts *pPoom = new CPoomContacts(m_hWnd, TRUE);

    if(m_oStr)
        m_oStr->SetResourceInstance(m_hInst);

	LoadOptions();

	if(g_sOptions->bStartButton)
	{
		CObjHardwareKeys* oKeys = CObjHardwareKeys::Instance();
		if(oKeys)
			oKeys->RegisterHotKeys(m_hWnd, m_hInst);
	}

	g_gui->SetSkin();

    UpdateStartIcon();

    // init toolbar
	g_guiToolbar->Init(m_hWnd, m_hInst);

	//all 3 guis should be initialized here
	m_guiPrograms.Init(m_hWnd, m_hInst);
	m_guiSettings.Init(m_hWnd, m_hInst);
	m_guiFavorites.Init(m_hWnd, m_hInst);

    // init global menu
	g_wndMenu->SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, g_gui->GetSkin().crSelected);
    g_wndMenu->SetImageArray(g_gui->GetSkin().uiImageArray);
	g_wndMenu->SetSelected(g_gui->GetSkin().uiSelector);
	g_wndMenu->SetBackground(IDR_PNG_Group);
	g_wndMenu->PreloadImages(hWnd, m_hInst);

    CIssStateAndNotify::DestroyNotifications();
    CIssStateAndNotify::RegisterWindowNotification(m_hWnd, WM_NOTIFY_TIME);
    
    CheckDemo();

    //if(m_bPreventUsage)
    //    CDlgOptions::TurnStartMenuOff();

    ::SetCursor(hCursor);
	return TRUE;
}

BOOL CDlgLauncher::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return UNHANDLED;
}

void CDlgLauncher::LoadOptions()
{
    DWORD dwSize = sizeof(TypeOptions);
    if(S_OK != GetKey(REG_Key, _T("Options2"), (LPBYTE)g_sOptions, dwSize))
    {
        // set some defaults here
        g_sOptions->eFavSize         = FAVSIZE_Normal;
        g_sOptions->bAlwaysShowText  = FALSE;
        g_sOptions->eSlideLeft       = SLIDE_Programs;
        g_sOptions->eSlideRight      = SLIDE_Games;
        g_sOptions->bShowSettingsHeaders = FALSE;
        g_sOptions->bHideOnAppLaunch = TRUE;
		g_sOptions->eSkin			 = SKIN_Glass;
		g_sOptions->bShowFullScreen  = TRUE;
		g_sOptions->eBackground		 = BK_DefaultToday;
		g_sOptions->bDrawTiles		 = TRUE;
		g_sOptions->eColorScheme	 = COLOR_Green;
		g_sOptions->bStartButton	 = FALSE;
    }
    
    if((int)g_sOptions->eFavSize < 3 || (int)g_sOptions->eFavSize > 5)
        g_sOptions->eFavSize = FAVSIZE_Normal;//messed up the registry when testing ... better safe than sorry

	if(g_sOptions->eBackground > BK_Count)
		g_sOptions->eBackground = BK_DefaultToday;

    DWORD dwItem;
    if(S_OK == GetKey(REG_Key, _T("Icon2"), dwItem))
        m_eIconStyle = (EnumIconStyle)dwItem;
    else
        m_eIconStyle = ICON_Default;

#ifdef DEBUG
    //m_eIconStyle = ICON_Off;
#endif
}

void CDlgLauncher::SaveOptions()
{
    SetKey(REG_Key, _T("Options2"), (LPBYTE)g_sOptions, sizeof(TypeOptions));
    DWORD dwItem;
    dwItem = (DWORD)m_eIconStyle;
    SetKey(REG_Key, _T("Icon2"), dwItem);
}

void CDlgLauncher::LaunchOptions()
{
    CDlgOptions dlgOptions;

    DWORD dwCurrent = g_cLocale.GetCurrentLanguage();
    dlgOptions.SetOptions(*g_sOptions);
	dlgOptions.SetIconStyle(m_eIconStyle);
    if(IDOK == dlgOptions.Launch(m_hWnd, m_hInst, FALSE))
    {
        HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

        EnumFavSize eOldSize = g_sOptions->eFavSize;
        TypeOptions sNew = dlgOptions.GetOptions();
        TypeOptions sOld = *g_sOptions;
        *g_sOptions = sNew;

        if(m_eIconStyle != dlgOptions.GetIconStyle())
        {
            m_eIconStyle = dlgOptions.GetIconStyle();
            SaveOptions();

            UpdateStartIcon();
        }

		// changes to the background
		if(dlgOptions.IsBackgroundChanged())
		{
			g_gui->SetSkin();
			g_gui->DeleteBackground();

			if((sNew.eBackground == BK_Camera || sNew.eBackground == BK_Picture) && !m_oStr->IsEmpty(dlgOptions.GetBackgroundFileName()))
				SetKey(REG_Key, REGVAL_Background, dlgOptions.GetBackgroundFileName(), m_oStr->GetLength(dlgOptions.GetBackgroundFileName()));
		}

		if(sOld.bStartButton != sNew.bStartButton)
		{
			if(sNew.bStartButton)
			{
				CObjHardwareKeys* oKeys = CObjHardwareKeys::Instance();
				if(oKeys)
					oKeys->RegisterHotKeys(m_hWnd, m_hInst);
			}
			else
			{
				CObjHardwareKeys::DeleteInstance();
			}
		}

		if(sOld.bShowFullScreen != sNew.bShowFullScreen)
		{
			// adjust the window to full or non
			g_gui->AdjustWindow(m_hWnd, FALSE);

		}

		if(sOld.bDrawTiles != sNew.bDrawTiles)
		{
			m_guiFavorites.ClearImageLoader();
			m_guiFavorites.UpdateImageLoader();
		}

        // changes to color scheme
        if(sOld.eColorScheme != sNew.eColorScheme)
        {
			g_gui->SetSkin();

            // new menu colors
            g_wndMenu->SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, g_gui->GetSkin().crSelected);
            g_wndMenu->SetImageArray(g_gui->GetSkin().uiImageArray);
            g_wndMenu->SetSelected(g_gui->GetSkin().uiSelector);
            g_wndMenu->PreloadImages(m_hWnd, m_hInst);

            // favorites
            m_guiFavorites.ReloadColorSchemeItems(m_hWnd, m_hInst);

            // contacts
            m_guiPrograms.ReloadColorSchemeItems(m_hWnd, m_hInst);

			// History
			m_guiSettings.ReloadColorSchemeItems(m_hWnd, m_hInst);

            // toolbar
            g_guiToolbar->ReloadColorSchemeItems(m_hWnd, m_hInst);
        }
        else if(sOld.bShowSettingsHeaders != sNew.bShowSettingsHeaders)
        {
            m_guiSettings.RefreshCPL();
        }

        if(eOldSize != g_sOptions->eFavSize)
        {
            m_guiFavorites.SetIconSize();
        }

      
        if(dwCurrent != g_cLocale.GetCurrentLanguage())
        {
            //m_guiFavorites.ResetSliderText();
            //m_guiFavorites.SetAllDirty();

			g_gui->DeleteBackground();

            if(sNew.bShowSettingsHeaders)
                m_guiSettings.RefreshCPL();
        }
        SaveOptions();
        ::SetCursor(hCursor);
    }
}

void CDlgLauncher::LaunchMenu()
{
	if(g_wndMenu->IsWindowUp(TRUE) || !m_pGui)
		return;

	g_wndMenu->ResetContent();
#ifndef DEBUG
    if(m_bPreventUsage)
    {}
    else
#endif
		m_pGui->AddMenuItems();

    g_wndMenu->AddSeparator();
    g_wndMenu->AddItem(ID(IDS_MENU_Options), m_hInst, IDMENU_Options);
    g_wndMenu->AddItem(ID(IDS_MENU_Help), m_hInst, IDMENU_Help);
    g_wndMenu->AddItem(ID(IDS_MENU_About), m_hInst, IDMENU_About);
    g_wndMenu->AddItem(ID(IDS_TOOLBAR_EXIT), m_hInst, IDMENU_Quit);

    g_wndMenu->SetSelectedItemIndex(0, TRUE);

    RECT rcWindow;
    GetWindowRect(m_hWnd, &rcWindow);

    int iWidth = WIDTH(m_pGui->GetFrame())*5/6;

    int iY = HEIGHT(m_pGui->GetFrame()); //m_pGui->GetFrame().bottom - 10 + rcWindow.top;

	g_wndMenu->PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
		m_pGui->GetFrame().left + (WIDTH(m_pGui->GetFrame()) - iWidth)/2, rcWindow.top,
        iWidth, iY,
		0, 0, 0,0, ADJUST_Bottom);
}

BOOL CDlgLauncher::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	RECT rcClip;
	GetClipBox(hDC, &rcClip);

	CIssGDIEx* gdi = g_gui->GetGDI();

	g_gui->DrawBackground(rcClip);

#ifndef DEBUG
	if(m_bPreventUsage)
	{
		if(m_pGui)
		{
			FillRect(*gdi, m_pGui->GetFrame(), 0);
			::DrawTextShadow(*gdi, m_oStr->GetText(ID(IDS_TRIAL_Over), m_hInst), m_pGui->GetFrame(), DT_TOP|DT_CENTER|DT_WORDBREAK, g_gui->GetFontTitle(), RGB(255,255,255), 0);
		}
	}
	else
#endif
	{
		if(m_pGui)
			m_pGui->Draw(*gdi, rcClient, rcClip);
	}

	// draw the toolbar
	g_guiToolbar->Draw(*gdi, rcClient, rcClip);

	g_gui->DrawScreen(rcClip, hDC);

	return TRUE;
}
    
BOOL CDlgLauncher::OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    DBG_OUT((_T("CDlgLauncher::OnSettingChange wParam - %d, lParam - %d"), wParam, lParam));

    // probably a new program installed so reset everything
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
    CDlgChoosePrograms::ResetListContent();
    CDlgChooseSettings::ResetListContent();
#ifndef NOPHONE
	CDlgChooseContact::ResetListContent();
#endif
	g_gui->DeleteBackground();
    m_guiPrograms.RefreshLinks();
    m_guiSettings.RefreshCPL();
    ::SetCursor(hCursor);

    return UNHANDLED;
}

BOOL CDlgLauncher::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN))
		return TRUE;

	if(g_sOptions->bShowFullScreen && HEIGHT(rcClient) != GetSystemMetrics(SM_CYSCREEN))
		return TRUE;

	g_gui->OnSize(rcClient);

	g_guiToolbar->MoveGui(g_gui->RectToolbar());
	m_guiPrograms.MoveGui(g_gui->RectScreen());
	m_guiSettings.MoveGui(g_gui->RectScreen());
	m_guiFavorites.MoveGui(g_gui->RectScreen());

	return TRUE;
}

BOOL CDlgLauncher::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE)
	{ 
		g_gui->AdjustWindow(hWnd, FALSE);
	}
    
    return TRUE;
}

BOOL CDlgLauncher::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(g_wndMenu->IsWindowUp(TRUE))
        return TRUE;

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        g_guiToolbar->OnLButtonDown(pt);
        return TRUE;
    }
#endif

	if(g_guiToolbar->OnLButtonDown(pt))
	{}
	else if(m_pGui && m_pGui->OnLButtonDown(pt))
	{}

	return TRUE;
}

BOOL CDlgLauncher::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(g_wndMenu->IsWindowUp(TRUE))
        return TRUE;

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        g_guiToolbar->OnLButtonUp(pt);
        return TRUE;
    }
#endif

	if(g_guiToolbar->OnLButtonUp(pt))
	{}
	else if(m_pGui && m_pGui->OnLButtonUp(pt))
	{}

	return TRUE;
}

BOOL CDlgLauncher::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(g_wndMenu->IsWindowUp(TRUE))
        return TRUE;

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        g_guiToolbar->OnMouseMove(pt);
        return TRUE;
    }
#endif

	if(g_guiToolbar->OnMouseMove(pt))
	{}
	else if(m_pGui && m_pGui->OnMouseMove(pt))
	{}

	return TRUE;
}

BOOL CDlgLauncher::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        g_guiToolbar->OnKeyDown(hWnd, wParam, lParam);
        return TRUE;
    }
#endif

	if(m_pGui)
		return m_pGui->OnKeyDown(hWnd, wParam, lParam);

	return TRUE;
}

BOOL CDlgLauncher::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        g_guiToolbar->OnKeyUp(hWnd, wParam, lParam);
        return TRUE;
    }
#endif

    if(LOWORD(wParam) == VK_TSOFT1 || LOWORD(wParam) == VK_TSOFT2)
    {   //give the menu focus and pop the ... menu
        g_guiToolbar->SetFocus(TRUE);
        LaunchMenu();
    }
	else if(LOWORD(wParam) == VK_THOME)
	{
		//is it the foreground window? if yes HIDE
		if(m_hWnd == GetForegroundWindow())
			ShowWindow(m_hWnd, SW_MINIMIZE);
		//otherwise SHOW
		else
		{
			ShowWindow(m_hWnd, SW_SHOW);
			SetForegroundWindow(m_hWnd);
		}
	}
	else if(m_pGui)
		return m_pGui->OnKeyUp(hWnd, wParam, lParam);

    return TRUE;
}

BOOL CDlgLauncher::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
	case IDMENU_Menu:
		LaunchMenu();
		break;
    case IDMENU_Options:
        LaunchOptions();
        break;
    case IDMENU_Help:
        LaunchHelp();
        break;
    case IDMENU_About:
        LaunchAbout();
        break;
    case IDMENU_Quit:
        PostQuitMessage(0);
        break;
    default:
		if(m_pGui)
			m_pGui->OnCommand(wParam, lParam);
        return UNHANDLED;
    }

    return UNHANDLED;

}

BOOL CDlgLauncher::OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    /*COPYDATASTRUCT cds = *(COPYDATASTRUCT*)lParam;
    TCHAR szCommand[STRING_NORMAL] = _T("");
    if(cds.cbData != 0)
        m_oStr->StringCopy(szCommand, (TCHAR*)cds.lpData);


    WPARAM wpGui = GUI_Favorites;
    if(m_oStr->Compare(szCommand, _T("-c")) == 0)
        wpGui = GUI_Programs;
    else if(m_oStr->Compare(szCommand, _T("-f")) == 0)
        wpGui = GUI_Favorites;
    else if(m_oStr->Compare(szCommand, _T("-h")) == 0)
        wpGui = GUI_Settings;
    else if(m_oStr->Compare(szCommand, _T("-q")) == 0)
    {
        PostQuitMessage(0);
        return TRUE;
    }*/

    //PostMessage(m_hWnd, WM_CHANGE_Gui, wpGui, 0);
	m_pGui = (CGuiBase*)&m_guiFavorites;
	g_gui->SetCurrentGui(GUI_Favorites);
	g_guiToolbar->ResetSelector();

    return TRUE;
}
	
BOOL CDlgLauncher::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_CHANGE_Gui)
	{
		switch((EnumCurrentGui)wParam)
		{
		case GUI_Favorites:
			m_pGui = (CGuiBase*)&m_guiFavorites;
			break;
		case GUI_Programs:
			m_pGui = (CGuiBase*)&m_guiPrograms;
			break;
		case GUI_Settings:
			m_pGui = (CGuiBase*)&m_guiSettings;
			break;
		case GUI_Quit:
            {
#ifndef DEBUG
                ShowWindow(m_hWnd, SW_MINIMIZE);

				// put it back to favorites
				m_pGui = (CGuiBase*)&m_guiFavorites;
				wParam = (WPARAM)GUI_Favorites;
#else
                PostQuitMessage(0);
#endif
            }
		default:
			break;
		}
		g_gui->SetCurrentGui((EnumCurrentGui)wParam);
		g_guiToolbar->ResetSelector();
		InvalidateRect(hWnd, NULL, FALSE);
	}
    else if((uiMessage >= PIM_ITEM_CREATED_LOCAL && uiMessage <= PIM_ITEM_CHANGED_LOCAL) ||
        (uiMessage >= PIM_ITEM_CREATED_REMOTE && uiMessage <= PIM_ITEM_CHANGED_REMOTE))
    {
        m_guiFavorites.HandlePoomMessage(uiMessage, wParam, lParam);
    }
    else if(uiMessage == WM_NOTIFY_TIME)
    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
        if(sysTime.wMinute % 10 == 0)
            CheckDemo();

		DebugOutMemory();

        /*if(m_bPreventUsage)
        {
            CDlgOptions::TurnStartMenuOff();
            InvalidateRect(m_hWnd, NULL, FALSE);
        }*/

		m_guiFavorites.OnTimerChange();
    }
	else if(uiMessage == WM_AniChange)
	{
		EnumCurrentGui eNew = (EnumCurrentGui)wParam;

		if(eNew != GUI_Quit)
		{
			RECT rcNew, rcCurrent;
			rcNew = g_guiToolbar->GetSelectorLocation(eNew);
			rcCurrent = g_guiToolbar->GetSelectorLocation(g_gui->GetCurrentGui());

			// animate to this new location
			CIssGDIEx gdiSelector;

			RECT rcSelector;
			SetRect(&rcSelector, 0,0,WIDTH(rcCurrent), HEIGHT(rcCurrent));
			gdiSelector.Create(g_guiToolbar->GetSelectorGDI(), rcSelector, FALSE, TRUE);
			gdiSelector.InitAlpha(FALSE);
			::Draw(gdiSelector, rcSelector, g_guiToolbar->GetSelectorGDI(), 0,0,ALPHA_Copy);
			g_gui->DrawToolbarItem(gdiSelector, eNew, rcSelector, FALSE);

			POINT ptFrom, ptTo;
			int iMoveDistance, iTotalDistance;

			if(g_gui->IsLandscape())
			{
				ptFrom.x	= ptTo.x = 0;
				ptFrom.y	= rcCurrent.top;
				ptFrom.y	= max(0, ptFrom.y);
				ptTo.y		= rcNew.top;
				iMoveDistance = abs(ptFrom.y - ptTo.y);
				iTotalDistance= HEIGHT(g_gui->RectToolbar()) - HEIGHT(rcCurrent);
			}
			else
			{
				ptFrom.y	= ptTo.y = rcCurrent.top;
				ptFrom.x	= rcCurrent.left;
				ptFrom.x	= max(0,ptFrom.x);
				ptTo.x		= rcNew.left;
				iMoveDistance = abs(ptFrom.x - ptTo.x);
				iTotalDistance= WIDTH(g_gui->RectToolbar()) - WIDTH(rcCurrent);
			}

			CIssGDIEx gdiBackground;
			CIssGDIEx* gdi = g_gui->GetBackground();

			RECT rcClient;
			GetClientRect(m_hWnd, &rcClient);

			gdiBackground.Create(gdi->GetDC(), rcClient, TRUE, TRUE);

			if(m_pGui)
				m_pGui->Draw(gdiBackground, rcClient, rcClient);

			HDC dc = GetDC(m_hWnd);
			ToolbarAnimateFromTo(dc, /**g_gui->GetBackground()*/gdiBackground, gdiSelector, ptFrom, ptTo, WIDTH(rcCurrent), HEIGHT(rcCurrent), iMoveDistance, TOOLBAR_ANIMATE_TIME*iMoveDistance/iTotalDistance);
			ReleaseDC(m_hWnd, dc);
		}

		g_gui->SetCurrentGui(eNew);

		SendMessage(m_hWnd, WM_CHANGE_Gui, eNew, 0);
		
	}
    else if(m_pGui)
        m_pGui->OnUser(hWnd, uiMessage, wParam, lParam);

	return TRUE;
}

BOOL CDlgLauncher::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//DBG_OUT((_T("CDlgLauncher::OnTimer %d"), wParam));
    if(m_pGui && m_pGui->OnTimer(wParam, lParam))
		return TRUE;
        
	KillTimer(hWnd, wParam);
    return TRUE;
}

BOOL CDlgLauncher::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)	
{
	if(m_pGui)
		m_pGui->OnChar(wParam, lParam);
	return TRUE;
}

    
void CDlgLauncher::LaunchAbout()
{
    CDlgAbout dlg;
    dlg.Launch(m_hWnd, m_hInst, FALSE);
}

void CDlgLauncher::LaunchHelp()
{
    //SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_SHOWSIPBUTTON);
    ShowWindow(m_hWnd, SW_MINIMIZE);

    switch(g_cLocale.GetCurrentLanguage())
    {
    case LANG_FRENCH:
        CreateProcess(_T("peghelp"), _T("launchpadhelp_fr.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
    	break;
    case LANG_GERMAN:
        CreateProcess(_T("peghelp"), _T("launchpadhelp_de.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
        break;
	case LANG_DUTCH:
		CreateProcess(_T("peghelp"), _T("launchpadhelp_du.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
		break;
    case LANG_SPANISH:
        CreateProcess(_T("peghelp"), _T("launchpadhelp_es.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
        break;
	case LANG_ROMANIAN:
		CreateProcess(_T("peghelp"), _T("launchpadhelp_ro.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
		break;
	case LANG_NORWEGIAN:
		CreateProcess(_T("peghelp"), _T("launchpadhelp_no.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
		break;
	case LANG_RUSSIAN:
		CreateProcess(_T("peghelp"), _T("launchpadhelp_ru.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
		break;
	case LANG_PORTUGUESE:
		CreateProcess(_T("peghelp"), _T("launchpadhelp_pt.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
		break;
	case LANG_CHINESE:
		CreateProcess(_T("peghelp"), _T("launchpadhelp_zh.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
		break;
    default:
        CreateProcess(_T("peghelp"), _T("launchpadhelp.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
        break;
    }

    
}

void CDlgLauncher::CheckDemo()
{
    CIssKey oKey;
    oKey.Init(_T("SOFTWARE\\Panoramic\\PanoLaunchpad"), _T("SOFTWARE\\Pano\\LP2"), 0x0, 0x383EF71D, 0x67ABCEF1, 2, TRUE);
    oKey.CheckSetKey();
    oKey.CheckDemo();

    m_bPreventUsage = !oKey.m_bGood;
}

void CDlgLauncher::UpdateStartIcon()
{
#ifdef DEBUG
    return;
#endif

    if(IsWindow(m_dlgStart.GetWnd()))
    {
        m_dlgStart.CloseAll();
        m_dlgStart.Destroy();
    }

    if(m_eIconStyle == ICON_Off)
        return;

    HWND hWndParent = NULL;
    hWndParent = FindWindow(_T("HHTaskBar"), _T(""));
    if(!hWndParent)
        return;

    m_dlgStart.Create(NULL, hWndParent, m_hInst, CLASS_Start, 0,0,0,0, WS_EX_NOACTIVATE, WS_POPUP);
    ShowWindow(m_dlgStart.GetWnd(), SW_SHOW);

	// have to make sure our window still displays on top
	if(g_sOptions->bShowFullScreen)
	{
		SetForegroundWindow(m_hWnd);
		ShowWindow(m_hWnd, SW_SHOW);
	}

    return;
}

//////////////////////////////////////////////////////////////////////////////
// custom toolbar animation - from IssGDIFX
//
#ifndef PI
#define PI 3.1415926538
#endif

HRESULT ToolbarAnimateFromTo(HDC    hdcScreen,
							 CIssGDIEx&	gdiBackground,
							 CIssGDIEx&	gdiAniObject,
							 POINT	ptFrom,
							 POINT	ptTo,
							 int		iWidth,
							 int		iHeight,
							 int		iDrawFrames,
							 int      iTime   // in ms
							 )
{
	HRESULT hr = S_OK;
	CIssGDIEx*	gdiTemp;
	CBARG(hdcScreen, _T("hdcScreen"));
	CBARG(gdiBackground.GetDC(), _T("gdiBackground.GetDC()"));
	CBARG(gdiAniObject.GetDC(), _T("gdiAniObject.GetDC()"));
	CBARG(iWidth != 0, _T("iWidth != 0"));
	CBARG(iHeight != 0, _T("iHeight != 0"));
	CBARG(iDrawFrames != 0, _T("iDrawFrames != 0"));
	CBARG(gdiAniObject.GetHeight() >= iHeight, _T("gdiAniObject.GetHeight() >= iHeight"));
	CBARG(gdiAniObject.GetWidth() >= iWidth, _T("gdiAniObject.GetWidth()	>= iWidth"));

	float fXStep = (float)(ptTo.x - ptFrom.x)/(float)iDrawFrames;
	float fYStep = (float)(ptTo.y - ptFrom.y)/(float)iDrawFrames;

	int iXDraw		= ptFrom.x;
	int iYDraw		= ptFrom.y;
	int	iXOldDraw	= ptFrom.x;
	int iYOldDraw	= ptFrom.y;

	RECT rcDraw;
	rcDraw.left		= min(ptFrom.x, ptTo.x);
	rcDraw.top		= min(ptFrom.y, ptTo.y);
	rcDraw.right	= max(ptFrom.x, ptTo.x) + iWidth;
	rcDraw.bottom	= max(ptFrom.y, ptTo.y) + iHeight;

	gdiTemp = g_gui->GetGDI();

	// draw the full background to the temp GDI
	BitBlt(gdiTemp->GetDC(),
		0,0,
		rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top,
		gdiBackground.GetDC(),
		rcDraw.left, rcDraw.top,
		SRCCOPY);

	DWORD dwCurTime = GetTickCount();
	int i = GetCurrentFrame(dwCurTime, iDrawFrames, (double)iTime);

	const float fAngleStep = (float)PI/iDrawFrames;
	RECT rcScreen;
	while(i < iDrawFrames)
	{
		rcScreen.left	= min(iXOldDraw, iXDraw);
		rcScreen.top	= min(iYOldDraw, iYDraw);
		rcScreen.right	= max(iXOldDraw, iXDraw) + iWidth;
		rcScreen.bottom	= max(iYOldDraw, iYDraw) + iHeight;

		// Draw the old Background
		BitBlt(gdiTemp->GetDC(),
			iXOldDraw - rcDraw.left, iYOldDraw - rcDraw.top,
			iWidth, iHeight,
			gdiBackground.GetDC(),
			iXOldDraw, iYOldDraw,
			SRCCOPY);
		// draw moving image
		hr = Draw(*gdiTemp, iXDraw - rcDraw.left, iYDraw - rcDraw.top, iWidth, iHeight, gdiAniObject);

		// draw it all to the screen
		BitBlt( hdcScreen,
			rcScreen.left,rcScreen.top,
			WIDTH(rcScreen), HEIGHT(rcScreen),
			gdiTemp->GetDC(),
			rcScreen.left - rcDraw.left, rcScreen.top - rcDraw.top,
			SRCCOPY);

		// update the values for next draw
		i = GetCurrentFrame(dwCurTime, iDrawFrames, (double)iTime);

		// give the movement some shape (accelerate-decelerate)
		int iCenterFrame = iDrawFrames / 2;
		float fAngle = (float)PI - (i * fAngleStep); 
		float fScaled = iCenterFrame + (iCenterFrame * (float)cos(fAngle));

		iXOldDraw	= iXDraw;
		iYOldDraw	= iYDraw;
		iXDraw		= (int)(/*i*/fScaled*fXStep) + ptFrom.x;
		iYDraw		= (int)(/*i*/fScaled*fYStep) + ptFrom.y;
		//DebugOut(_T("i = %d factored: %.2f"), i, fScaled);
	}

	if(iXDraw != ptTo.x || iYDraw != ptTo.y)
	{	//draw the final frame
		// update the values and draw again
		iXDraw		= ptTo.x;
		iYDraw		= ptTo.y;

		rcScreen.left	= min(iXOldDraw, iXDraw);
		rcScreen.top	= min(iYOldDraw, iYDraw);
		rcScreen.right	= max(iXOldDraw, iXDraw) + iWidth;
		rcScreen.bottom	= max(iYOldDraw, iYDraw) + iHeight;

		// Draw the old Background
		BitBlt(gdiTemp->GetDC(),
			iXOldDraw - rcDraw.left, iYOldDraw - rcDraw.top,
			iWidth, iHeight,
			gdiBackground.GetDC(),
			iXOldDraw, iYOldDraw,
			SRCCOPY);

		// draw moving image
		hr = Draw(*gdiTemp, iXDraw - rcDraw.left, iYDraw - rcDraw.top, iWidth, iHeight, gdiAniObject);

		// draw it all to the screen
		BitBlt( hdcScreen,
			rcScreen.left,rcScreen.top,
			rcScreen.right - rcScreen.left, rcScreen.bottom - rcScreen.top,
			gdiTemp->GetDC(),
			rcScreen.left - rcDraw.left, rcScreen.top - rcDraw.top,
			SRCCOPY);

	}

Error:
	return hr;
}

