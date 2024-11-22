#include "StdAfx.h"
#include "DlgLauncher.h"
#include "IssHardwareKeys.h"
#include "CommonDefines.h"
#include "IssCommon.h"
#include "Resource.h"
#include "DlgBase.h"
#include "DlgOptions.h"
#include "IssRegistry.h"
#include "DlgAbout.h"
#include "IssStateAndNotify.h"
#include "IssLocalisation.h"

#define REG_Key _T("SOFTWARE\\Pano\\Launcher\\Launcher")
#define CLASS_Start	_T("ClassLauncherStart")

CDlgLauncher* gDlgContacts = NULL;
CDlgLauncher::CDlgLauncher(EnumCurrentGui eSelection)
:m_pGui(NULL)
,m_bPreventUsage(FALSE)
{
    gDlgContacts = this;
	switch(eSelection)
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
	}
    SetRectEmpty(&m_rcClient);

    CIssStateAndNotify::InitializeNotifications();

	m_hFontNormal	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
 	m_hFontBold		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
}

CDlgLauncher::~CDlgLauncher(void)
{
    CIssGDIEx::DeleteFont(m_hFontNormal);
	CIssGDIEx::DeleteFont(m_hFontBold);

    m_dlgStart.CloseAll();
    m_dlgStart.Destroy();

	m_wndMenu.ResetContent();
    CIssStateAndNotify::DestroyNotifications();
	CDlgChooseSettings::DestroyAllContent();
	CDlgChoosePrograms::DeleteAllContent();
	CIssKineticList::DeleteAllContent();
	
}
TypeOptions& CDlgLauncher::GetOptions()
{
    return gDlgContacts->m_sOptions;
}
    
CIssGDIEx& CDlgLauncher::GetCallIcons()
{
    return gDlgContacts->GetIconArray();
}
HFONT CDlgLauncher::GetFontNormal()
{
    return gDlgContacts->_GetFontNormal();
}

HFONT CDlgLauncher::GetFontBold()
{
    return gDlgContacts->_GetFontBold();
}

BOOL CDlgLauncher::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    /*
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

#endif*/



    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    // MUST init poom with handle to this window so we can get notifications FIRST
    // SUBSEQUENT calls use CPoomContacts::Instance();

    g_cLocale.Init(REG_Key, m_hInst, SUPPORT_English | SUPPORT_German | SUPPORT_French | SUPPORT_Spanish, TRUE);

    if(m_oStr)
        m_oStr->SetResourceInstance(m_hInst);

    LoadOptions();

    UpdateStartIcon();
    
    // init background
    m_guiBackground.Init(m_hWnd, m_hInst);
    m_guiBackground.PreloadImages(FALSE);

    // init toolbar
	m_guiToolBar.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions, this);
    m_guiToolBar.SetGlowImage(&GetGlowImg());

	//all 3 guis should be initialized here
	m_guiPrograms.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions);
	m_guiPrograms.SetMenu(&m_wndMenu);
	m_guiFavorites.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions);
	m_guiFavorites.SetMenu(&m_wndMenu);
    m_guiFavorites.SetGlowImage(&GetGlowImg());
	m_guiSettings.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions);
	m_guiSettings.SetMenu(&m_wndMenu);

    // init global menu
	m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
	m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
	m_wndMenu.SetBackground(IDR_PNG_Group);
	m_wndMenu.PreloadImages(hWnd, m_hInst);

    CIssStateAndNotify::DestroyNotifications();
    CIssStateAndNotify::RegisterWindowNotification(m_hWnd, WM_NOTIFY_TIME);
    
    CheckDemo();

    if(m_bPreventUsage)
        CDlgOptions::TurnStartMenuOff();

    ::SetCursor(hCursor);
	return TRUE;
}

BOOL CDlgLauncher::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CIssHardwareKeys* oKeys = CIssHardwareKeys::Instance();

    switch(lParam >> 16)//not sure what the shift is for but it works
    {
    case VK_TTALK:
        if(oKeys)
            oKeys->OnHotKey(hWnd, wParam, lParam);
        //and of course handle the press

        break;
    default:
        return UNHANDLED;
        break;
    }
    return TRUE;
}

EnumCurrentGui CDlgLauncher::GetCurSelection()
{
    if(m_pGui == &m_guiFavorites)
        return GUI_Favorites;
    else if(m_pGui == (CGuiBase*)&m_guiPrograms)
        return GUI_Programs;
    else if(m_pGui == &m_guiSettings)
        return GUI_Settings;

    return (EnumCurrentGui)0;
}

CIssGDIEx& CDlgLauncher::GetIconArray()
{
    if(!m_gdiImgIconArray.GetDC())
    {
        UINT uiRes = IsVGA() ? IDR_PNG_HistoryArrayVGA : IDR_PNG_HistoryArray;
        m_gdiImgIconArray.LoadImage(uiRes, m_hWnd, m_hInst, TRUE);
    }

    return m_gdiImgIconArray;
}

void CDlgLauncher::LoadOptions()
{
    DWORD dwSize = sizeof(TypeOptions);
    if(S_OK != GetKey(REG_Key, _T("Options"), (LPBYTE)&m_sOptions, dwSize))
    {
        // set some defaults here
        m_sOptions.eFavSize         = FAVSIZE_Normal;
        m_sOptions.bAlwaysShowText  = FALSE;
        m_sOptions.eSlideLeft       = SLIDE_Programs;
        m_sOptions.eSlideRight      = SLIDE_Games;
        m_sOptions.bShowSettingsHeaders = FALSE;
        m_sOptions.bHideOnAppLaunch = FALSE;
    }
    
    if((int)m_sOptions.eFavSize < 3 || (int)m_sOptions.eFavSize > 5)
        m_sOptions.eFavSize = FAVSIZE_Normal;//messed up the registry when testing ... better safe than sorry

    DWORD dwItem;
    if(S_OK == GetKey(REG_Save, _T("ColorScheme"), dwItem))
        g_eColorScheme = (EnumColorScheme)dwItem;

    if(S_OK == GetKey(REG_Key, _T("Icon"), dwItem))
        m_eIconStyle = (EnumIconStyle)dwItem;
    else
        m_eIconStyle = ICON_Default;

#ifdef DEBUG
    m_eIconStyle = ICON_Off;
#endif
}

void CDlgLauncher::SaveOptions()
{
    SetKey(REG_Key, _T("Options"), (LPBYTE)&m_sOptions, sizeof(TypeOptions));
    DWORD dwItem = (DWORD)g_eColorScheme;
    SetKey(REG_Save, _T("ColorScheme"), dwItem);
    dwItem = (DWORD)m_eIconStyle;
    SetKey(REG_Key, _T("Icon"), dwItem);
}

void CDlgLauncher::LaunchOptions()
{
    CDlgOptions dlgOptions;
    dlgOptions.SetFullScreen(FALSE);

    DWORD dwCurrent = g_cLocale.GetCurrentLanguage();
    dlgOptions.SetOptions(m_sOptions);
    dlgOptions.SetIconStyle(m_eIconStyle);
    dlgOptions.Init(&m_gdiMem, &m_guiBackground);
    if(IDOK == dlgOptions.Launch(m_hWnd, m_hInst, FALSE))
    {
        HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

        EnumColorScheme eNew = dlgOptions.GetColorScheme();
        EnumColorScheme eOld = g_eColorScheme;
        EnumFavSize eOldSize = m_sOptions.eFavSize;
        TypeOptions sNew = dlgOptions.GetOptions();
        TypeOptions sOld = m_sOptions;
        m_sOptions = sNew;
        g_eColorScheme = eNew;

        if(m_eIconStyle != dlgOptions.GetIconStyle())
        {
            m_eIconStyle = dlgOptions.GetIconStyle();
            SaveOptions();
            //restart the start menu dude
            //dlgOptions.RestartStartMenu();
            UpdateStartIcon();
        }

        // changes to color scheme
        if(eOld != eNew)
        {
            // new menu colors
            m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
            m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
            m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
            m_wndMenu.PreloadImages(m_hWnd, m_hInst);

            // glow
            m_gdiGlow.Destroy();
            GetGlowImg();

            // favorites
            m_guiFavorites.ReloadColorSchemeItems(m_hWnd, m_hInst);

            // contacts
            m_guiPrograms.ReloadColorSchemeItems(m_hWnd, m_hInst);

			// History
			m_guiSettings.ReloadColorSchemeItems(m_hWnd, m_hInst);

            // toolbar
            m_guiToolBar.ReloadColorSchemeItems(m_hWnd, m_hInst);
        }
        else if(sOld.bShowSettingsHeaders != sNew.bShowSettingsHeaders)
        {
            m_guiSettings.RefreshCPL();
        }

        if(eOldSize != m_sOptions.eFavSize)
            m_guiFavorites.SetIconSize(m_sOptions.eFavSize);

        if(sOld.eSlideLeft != sNew.eSlideLeft || sOld.eSlideRight != sNew.eSlideRight)
            m_guiFavorites.ResetSliderText();
       
        if(dwCurrent != g_cLocale.GetCurrentLanguage())
        {
            m_guiToolBar.ReloadTextItems();
            m_guiToolBar.SetAllDirty();
            m_guiFavorites.ResetSliderText();
            m_guiFavorites.SetAllDirty();

            if(sNew.bShowSettingsHeaders)
                m_guiSettings.RefreshCPL();
        }
        SaveOptions();
        ::SetCursor(hCursor);
    }
}

void CDlgLauncher::LaunchMenu()
{
	if(m_wndMenu.IsWindowUp(TRUE))
		return;

	m_wndMenu.ResetContent();
#ifndef DEBUG
    if(m_bPreventUsage)
    {}
    else
#endif
	m_pGui->AddMenuItems();

    m_wndMenu.AddSeparator();
    m_wndMenu.AddItem(ID(IDS_MENU_Options), m_hInst, IDMENU_Options);
    m_wndMenu.AddItem(ID(IDS_MENU_Help), m_hInst, IDMENU_Help);
    m_wndMenu.AddItem(ID(IDS_MENU_About), m_hInst, IDMENU_About);
    m_wndMenu.AddItem(ID(IDS_TOOLBAR_EXIT), m_hInst, IDMENU_Quit);

    m_wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcWindow;
    GetWindowRect(m_hWnd, &rcWindow);

    int iWidth = WIDTH(m_pGui->GetFrame())*5/6;

    int iY = HEIGHT(m_pGui->GetFrame()); //m_pGui->GetFrame().bottom - 10 + rcWindow.top;

	m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
		m_pGui->GetFrame().left + (WIDTH(m_pGui->GetFrame()) - iWidth)/2, rcWindow.top,
        iWidth, iY,
		0, 0, 0,0, ADJUST_Bottom);
}

BOOL CDlgLauncher::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL || WIDTH(rcClient) != m_gdiMem.GetWidth() || HEIGHT(rcClient) != m_gdiMem.GetHeight())
    {
        m_gdiMem.Create(hDC, rcClient, FALSE, TRUE, FALSE);
        FillRect(m_gdiMem, rcClient, 0);
    }

    if(m_guiToolBar.IsDragging())
        return FALSE;

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        if(m_pGui)
        {
            FillRect(m_gdiMem, m_pGui->GetFrame(), 0);
            ::DrawText(m_gdiMem, m_oStr->GetText(ID(IDS_TRIAL_Over), m_hInst), m_pGui->GetFrame(), DT_TOP|DT_CENTER|DT_WORDBREAK, GetFontNormal(), RGB(255,255,255));
        }
    }
    else
#endif
    {
    if(m_pGui)
	    m_pGui->Draw(m_gdiMem.GetDC(), m_pGui->GetFrame(), rcClip);
    }
	    
	// draw the toolbar
    m_guiToolBar.Draw(m_gdiMem.GetDC(), rcClient, rcClip);

    // add the selector
    m_guiToolBar.DrawSelector(m_gdiMem, rcClient, rcClip);

	// draw it all to the screen
	BitBlt(hDC,
		   0,0,
		   WIDTH(rcClient), HEIGHT(rcClient),
		   m_gdiMem.GetDC(),
		   0,0,
		   SRCCOPY);


	return TRUE;
}
    
void CDlgLauncher::SetDirty(RECT& rc)
{
    if(m_pGui)
        m_pGui->SetDirty(rc);
}

BOOL CDlgLauncher::OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    DBG_OUT((_T("CDlgLauncher::OnSettingChange wParam - %d, lParam - %d"), wParam, lParam));

    // probably a new program installed so reset everything
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
    CDlgChoosePrograms::ResetListContent();
    CDlgChooseSettings::ResetListContent();
    m_guiPrograms.RefreshLinks();
    m_guiSettings.RefreshCPL();
    ::SetCursor(hCursor);

    return UNHANDLED;
}

BOOL CDlgLauncher::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

    // don't allow full screen
    if(EqualRect(&rcClient, &m_rcClient) || WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rcClient) == GetSystemMetrics(SM_CYSCREEN))
        return TRUE;

    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    m_rcClient = rcClient;

    // create the shared gdi's now
    // screens will preload images based on these
    HDC hDC = ::GetDC(m_hWnd);
	if(m_gdiMem.GetDC() == NULL || WIDTH(rcClient) != m_gdiMem.GetWidth() || HEIGHT(rcClient) != m_gdiMem.GetHeight())
    {
		m_gdiMem.Create(hDC, rcClient, FALSE, TRUE, FALSE);
        FillRect(m_gdiMem, rcClient, 0);
    }
	if(m_gdiBg.GetDC() == NULL || WIDTH(rcClient) != m_gdiBg.GetWidth() || HEIGHT(rcClient) != m_gdiBg.GetHeight())
    {
        m_gdiBg.Create(hDC, rcClient, FALSE, TRUE, FALSE);
        FillRect(m_gdiBg, rcClient, 0);
    }
	ReleaseDC(m_hWnd, hDC);

	BOOL bLandscape = (GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN));

	// move the toolbar
	RECT rcTemp = rcClient;

    if(bLandscape)	
    {
        int iMenuSize = 52;
        if(IsVGA())
            iMenuSize *= 2;
        rcTemp.right = iMenuSize;
    }
	else	// portrait, square				
    {
        int iMenuSize = 40;
        if(IsVGA())
            iMenuSize *= 2;
        rcTemp.top	= rcClient.bottom - iMenuSize;
    }

	m_guiToolBar.MoveGui(rcTemp);

	if(bLandscape)	
	{
		rcTemp.left = rcTemp.right;
		rcTemp.right = rcClient.right;
	}
	else	// portrait, square					
	{
		rcTemp.bottom = rcTemp.top;
		rcTemp.top = rcClient.top;
	}

	//move the gui ... all the guis should be moved here
	m_guiPrograms.MoveGui(rcTemp);
	m_guiFavorites.MoveGui(rcTemp);
	m_guiSettings.MoveGui(rcTemp);

    ::SetCursor(hCursor);

	return TRUE;
}

BOOL CDlgLauncher::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
	{ 


//#ifdef DEBUG
		SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR | SHFS_HIDESIPBUTTON); 
        OnSize(hWnd, 0, 0);

        //get rid of the damn sip too ... just in case 
        HWND hWndSip = FindWindow(_T("MS_SIPBUTTON"), NULL); 
        if(hWndSip) 
            ShowWindow(hWndSip, SW_HIDE);

		RECT rc; 
        int iMenu = 0;
        switch(GetSystemMetrics(SM_CXICON))
        {
        case 32:
            iMenu = 26;
        	break;
        case 44:
            iMenu = 35;
        	break;
        case 64:
            iMenu = 52;
            break;
        default:
            iMenu = GetSystemMetrics(SM_CXICON)*26/32;
            break;
        }
        
		SetRect(&rc, 0, iMenu, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
/*#else
        SHFullScreen(m_hWnd, SHFS_HIDETASKBAR  | SHFS_HIDESIPBUTTON); 
        OnSize(hWnd, 0, 0);
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 

#endif*/
	}
    
    return m_pGui->OnActivate(hWnd, wParam, lParam); 
}

BOOL CDlgLauncher::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        m_guiToolBar.OnLButtonDown(pt);
        return TRUE;
    }
#endif

	if(!m_pGui->OnLButtonDown(pt))
		m_guiToolBar.OnLButtonDown(pt);
	else
		m_guiToolBar.LoseFocus();

	return TRUE;
}

BOOL CDlgLauncher::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        m_guiToolBar.OnLButtonUp(pt);
        return TRUE;
    }
#endif

	if(!m_pGui->OnLButtonUp(pt))
		m_guiToolBar.OnLButtonUp(pt);

	return TRUE;
}

BOOL CDlgLauncher::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        m_guiToolBar.OnMouseMove(pt);
        return TRUE;
    }
#endif

	if(!m_pGui->OnMouseMove(pt))
		m_guiToolBar.OnMouseMove(pt);

	return TRUE;
}

BOOL CDlgLauncher::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        m_guiToolBar.OnKeyDown(hWnd, wParam, lParam);
        return TRUE;
    }
#endif

    if(m_pGui->OnKeyDown(hWnd, wParam, lParam))
    {
        m_guiToolBar.LoseFocus();
    }
    else
    {
        m_guiToolBar.OnKeyDown(hWnd, wParam, lParam);
    }
	return TRUE;
}

BOOL CDlgLauncher::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        m_guiToolBar.OnKeyUp(hWnd, wParam, lParam);
        return TRUE;
    }
#endif

    if(LOWORD(wParam) == VK_TSOFT1 || LOWORD(wParam) == VK_TSOFT2)
    {   //give the menu focus and pop the ... menu
        m_guiToolBar.SetFocus(TRUE);
        LaunchMenu();
    }
    else if(m_pGui->OnKeyUp(hWnd, wParam, lParam))
		m_guiToolBar.LoseFocus();
	else
		m_guiToolBar.OnKeyUp(hWnd, wParam, lParam);

    return TRUE;
}

BOOL CDlgLauncher::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
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
        return m_pGui->OnCommand(wParam, lParam);
    }

    return UNHANDLED;

}

BOOL CDlgLauncher::OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    COPYDATASTRUCT cds = *(COPYDATASTRUCT*)lParam;
    TCHAR szCommand[STRING_NORMAL] = _T("");
    if(cds.cbData != 0)
        m_oStr->StringCopy(szCommand, (TCHAR*)cds.lpData);

    TCHAR szMsg[STRING_LARGE];
    m_oStr->Format(szMsg, _T("received msg: %s"), szCommand);


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
        return UNHANDLED;
    }

    PostMessage(m_hWnd, WM_CHANGE_Gui, wpGui, 0);
    return UNHANDLED;
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
                ShowWindow(m_hWnd, SW_HIDE);
#else
                PostQuitMessage(0);
#endif
            }
		default:
			break;
		}
        m_pGui->SetAllDirty();
	}
    else if((uiMessage >= PIM_ITEM_CREATED_LOCAL && uiMessage <= PIM_ITEM_CHANGED_LOCAL) ||
        (uiMessage >= PIM_ITEM_CREATED_REMOTE && uiMessage <= PIM_ITEM_CHANGED_REMOTE))
    {
        
        m_guiPrograms.HandlePoomMessage(uiMessage, wParam, lParam);
        m_guiFavorites.HandlePoomMessage(uiMessage, wParam, lParam);

        // close the details window if it's up
        //if(IsWindow(m_dlgContactDetails.GetWnd()))
        //    m_dlgContactDetails.CloseWindow(IDCANCEL);
    }
    else if(uiMessage == WM_NOTIFY_TIME)
    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
        if(sysTime.wMinute % 10 == 0)
            CheckDemo();

        if(m_bPreventUsage)
        {
            CDlgOptions::TurnStartMenuOff();
            RECT rcClient;
            GetClientRect(m_hWnd, &rcClient);
            FillRect(m_gdiMem, rcClient, 0);
            SetDirty(rcClient);
            InvalidateRect(m_hWnd, NULL, FALSE);
        }
    }
    else
        m_pGui->OnUser(hWnd, uiMessage, wParam, lParam);

	InvalidateRect(m_hWnd, NULL, FALSE);

	return TRUE;
}

BOOL CDlgLauncher::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = m_pGui->OnTimer(wParam, lParam);
    if(bRet == FALSE || bRet == UNHANDLED)
        KillTimer(hWnd, wParam);
    return bRet;
}

BOOL CDlgLauncher::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)	
{
	return m_pGui->OnChar(wParam, lParam);
}

    
CIssGDIEx& CDlgLauncher::GetGlowImg()
{
    // load once only... doesn't change size
    if(!m_gdiGlow.GetDC())
    {
        m_gdiGlow.LoadImage(SKIN(IDR_PNG_Glow), m_hWnd, m_hInst);
    }

    return m_gdiGlow;
}
    
void CDlgLauncher::NotifyToolbarFocus(BOOL bHasFocus)
{
    if(bHasFocus)
    {
        if(m_pGui)
            m_pGui->LoseFocus();
    }
}

void CDlgLauncher::LaunchAbout()
{
    CDlgAbout dlg;
    dlg.SetFullScreen(FALSE);
    dlg.Init(&m_gdiMem, &m_guiBackground);
    dlg.Launch(m_hWnd, m_hInst, FALSE);
}

void CDlgLauncher::LaunchHelp()
{
    //SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_SHOWSIPBUTTON);
    ShowWindow(m_hWnd, SW_HIDE);

    switch(g_cLocale.GetCurrentLanguage())
    {
    case LANG_FRENCH:
        CreateProcess(_T("peghelp"), _T("launchpadhelp_fr.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
    	break;
    case LANG_GERMAN:
        CreateProcess(_T("peghelp"), _T("launchpadhelp_de.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
        break;
    case LANG_SPANISH:
        CreateProcess(_T("peghelp"), _T("launchpadhelp_es.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
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
    return;
}
