#include "StdAfx.h"
#include "DlgContacts.h"
#include "IssHardwareKeys.h"
#include "CommonDefines.h"
#include "IssCommon.h"
#include "Resource.h"
#include "DlgBase.h"
#include "DlgOptions.h"
#include "IssRegistry.h"

CDlgContacts::CDlgContacts(EnumCurrentGui eSelection)
:m_pGui(NULL)
, m_guiToolBar()
{
	switch(eSelection)
	{
	case GUI_Favorites:
		m_pGui = (CGuiBase*)&m_guiFavorites;
		break;
	case GUI_Contacts:
		m_pGui = (CGuiBase*)&m_guiContacts;
		break;
	case GUI_CallLog:
		m_pGui = (CGuiBase*)&m_guiCallLog;
		break;
	}
    SetRectEmpty(&m_rcClient);
}

CDlgContacts::~CDlgContacts(void)
{
}

BOOL CDlgContacts::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    // init poom with handle to this window so we can get notifications
    // from here on call CPoomContacts::Instance();
    CPoomContacts *pPoom = new CPoomContacts(m_hWnd);  

    LoadOptions();

    // init background
    m_guiBackground.Init(m_hWnd, m_hInst);
    m_guiBackground.PreloadImages();

    // init toolbar
	m_guiToolBar.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions, this);
    m_guiToolBar.SetGlowImage(&GetGlowImg());

    m_dlgContactDetails.PreloadImages(m_hWnd, m_hInst, &m_gdiMem, &m_wndMenu);

	//all 3 guis should be initialized here
	m_guiContacts.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions, &m_dlgContactDetails);
	m_guiContacts.SetMenu(&m_wndMenu);
	m_guiFavorites.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions, &m_dlgContactDetails);
	m_guiFavorites.SetMenu(&m_wndMenu);
    m_guiFavorites.SetGlowImage(&GetGlowImg());
	m_guiCallLog.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions, &m_dlgContactDetails);
	m_guiCallLog.SetMenu(&m_wndMenu);

    // init global menu
	m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
	m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
	m_wndMenu.SetBackground(IDR_PNG_Group);
	m_wndMenu.PreloadImages(hWnd, m_hInst);

    // init hardware keys
    CIssHardwareKeys* oKeys = CIssHardwareKeys::Instance();
    if(oKeys)
        oKeys->RegisterHotKeys(m_hWnd, m_hInst, KEY_Talk);

    ::SetCursor(hCursor);
	return TRUE;
}


EnumCurrentGui CDlgContacts::GetCurSelection()
{
    if(m_pGui == &m_guiFavorites)
        return GUI_Favorites;
    else if(m_pGui == (CGuiBase*)&m_guiContacts)
        return GUI_Contacts;
    else if(m_pGui == &m_guiCallLog)
        return GUI_CallLog;

    return (EnumCurrentGui)0;
}

void CDlgContacts::LoadOptions()
{
    DWORD dwSize = sizeof(TypeOptions);
    if(S_OK != GetKey(REG_Save, _T("ContactOptions"), (LPBYTE)&m_sOptions, dwSize))
    {
        // set some defaults here
        m_sOptions.bShowSliderBar       = TRUE;
        m_sOptions.eShowCall            = CALL_All;
        m_sOptions.eShowHistory         = HISTORY_All;
        m_sOptions.eShowSMS             = SMS_All;
        m_sOptions.eSortContacts        = SORT_LastName;
    }
    DWORD dwItem;
    if(S_OK == GetKey(REG_Save, _T("ColorScheme"), dwItem))
        g_eColorScheme = (EnumColorScheme)dwItem;
}

void CDlgContacts::SaveOptions()
{
    SetKey(REG_Save, _T("ContactOptions"), (LPBYTE)&m_sOptions, sizeof(TypeOptions));
    DWORD dwItem = (DWORD)g_eColorScheme;
    SetKey(REG_Save, _T("ColorScheme"), dwItem);
}

void CDlgContacts::LaunchOptions()
{
    CDlgOptions dlgOptions;

    dlgOptions.SetOptions(m_sOptions);
    dlgOptions.Init(&m_gdiMem, &m_guiBackground);
    if(IDOK == dlgOptions.Launch(m_hWnd, m_hInst))
    {
        HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

        EnumColorScheme eNew = dlgOptions.GetColorScheme();
        EnumColorScheme eOld = g_eColorScheme;
        TypeOptions sNew = dlgOptions.GetOptions();
        TypeOptions sOld = m_sOptions;
        m_sOptions = sNew;
        g_eColorScheme = eNew;

        // changes to color scheme
        if(eOld != eNew)
        {
            // new menu colors
            m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
            m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
            m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
            m_wndMenu.PreloadImages(m_hWnd, m_hInst);
            // details screen changes
            m_dlgContactDetails.ReloadColorSchemeItems(m_hWnd, m_hInst);

            // glow
            m_gdiGlow.Destroy();
            GetGlowImg();

            // favorites
            m_guiFavorites.ReloadColorSchemeItems(m_hWnd, m_hInst);

            // contacts
            m_guiContacts.ReloadColorSchemeItems(m_hWnd, m_hInst);

            // toolbar
            m_guiToolBar.ReloadColorSchemeItems(m_hWnd, m_hInst);
        }
        // changes to favorites (should we show the slider bar or not)
        if(sOld.bShowSliderBar != sNew.bShowSliderBar)
        {

        }
        // changes to history display
        if(sOld.eShowCall != sNew.eShowCall || sOld.eShowSMS != sNew.eShowSMS || sOld.eShowHistory != sNew.eShowHistory)
        {
            m_guiCallLog.RefreshCPL();
        }
        // changes to contacts sorting
        if(sOld.eSortContacts != sNew.eSortContacts)
        {
            m_guiContacts.RefreshLinks();
        }
        SaveOptions();
        ::SetCursor(hCursor);
    }
}

void CDlgContacts::LaunchMenu()
{
	m_wndMenu.ResetContent();

	m_pGui->AddMenuItems();

    m_wndMenu.AddSeparator();
    m_wndMenu.AddItem(IDS_MENU_Options, m_hInst, IDMENU_Options);
    m_wndMenu.AddItem(IDS_MENU_Help, m_hInst, IDMENU_Help);
    m_wndMenu.AddItem(IDS_MENU_About, m_hInst, IDMENU_About);

    m_wndMenu.SetSelectedItemIndex(0, TRUE);

	m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
		10,10,WIDTH(m_pGui->GetFrame())*2/3,m_pGui->GetFrame().bottom - 10,
		m_pGui->GetFrame().left, m_pGui->GetFrame().bottom - 10, 10,10, ADJUST_Bottom);

}

BOOL CDlgContacts::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL || WIDTH(rcClient) != m_gdiMem.GetWidth() || HEIGHT(rcClient) != m_gdiMem.GetHeight())
		m_gdiMem.Create(hDC, rcClient, FALSE, TRUE, TRUE);

    if(m_guiToolBar.IsDragging())
        return FALSE;

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    if(m_pGui)
	    m_pGui->Draw(m_gdiMem.GetDC(), m_pGui->GetFrame(), rcClip);
	    
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
    
void CDlgContacts::SetDirty(RECT& rc)
{
    if(m_pGui)
        m_pGui->SetDirty(rc);
}

BOOL CDlgContacts::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

    if(EqualRect(&rcClient, &m_rcClient))
        return TRUE;

    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    m_rcClient = rcClient;

    // create the shared gdi's now
    // screens will preload images based on these
    HDC hDC = ::GetDC(m_hWnd);
	if(m_gdiMem.GetDC() == NULL || WIDTH(rcClient) != m_gdiMem.GetWidth() || HEIGHT(rcClient) != m_gdiMem.GetHeight())
		m_gdiMem.Create(hDC, rcClient, FALSE, TRUE, TRUE);
	if(m_gdiBg.GetDC() == NULL || WIDTH(rcClient) != m_gdiBg.GetWidth() || HEIGHT(rcClient) != m_gdiBg.GetHeight())
		m_gdiBg.Create(hDC, rcClient, FALSE, TRUE, TRUE);

	BOOL bLandscape = (rcClient.right > rcClient.bottom);

	// move the toolbar
	RECT rcTemp = rcClient;

    

	if(bLandscape)	
    {
        int iMenuSize = 26;
        if(IsVGA())
            iMenuSize *= 2;
        rcTemp.right	= min(iMenuSize, rcClient.bottom/4);
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
	m_guiContacts.MoveGui(rcTemp);
	m_guiFavorites.MoveGui(rcTemp);
	m_guiCallLog.MoveGui(rcTemp);

    ::SetCursor(hCursor);

	return TRUE;
}

BOOL CDlgContacts::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE) 
	{ 
		SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON); 
		RECT rc; 
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
	}
    
    return m_pGui->OnActivate(hWnd, wParam, lParam); 
}

BOOL CDlgContacts::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

	if(!m_pGui->OnLButtonDown(pt))
		m_guiToolBar.OnLButtonDown(pt);
	else
		m_guiToolBar.LoseFocus();

	return TRUE;
}

BOOL CDlgContacts::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

	if(!m_pGui->OnLButtonUp(pt))
		m_guiToolBar.OnLButtonUp(pt);

	return TRUE;
}

BOOL CDlgContacts::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

	if(!m_pGui->OnMouseMove(pt))
		m_guiToolBar.OnMouseMove(pt);

	return TRUE;
}

BOOL CDlgContacts::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(m_pGui->OnKeyDown(hWnd, wParam, lParam))
		m_guiToolBar.LoseFocus();
	else
		m_guiToolBar.OnKeyDown(hWnd, wParam, lParam);

	return TRUE;
}

BOOL CDlgContacts::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	if(m_pGui->OnKeyUp(hWnd, wParam, lParam))
		m_guiToolBar.LoseFocus();
	else
		m_guiToolBar.OnKeyUp(hWnd, wParam, lParam);

    return TRUE;
}

BOOL CDlgContacts::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case IDMENU_Options:
        LaunchOptions();
        break;
    case IDMENU_Help:
        // should launch the help here
        break;
    case IDMENU_About:
        // should launch the help here
        break;
    default:
        return m_pGui->OnCommand(wParam, lParam);
    }

    return UNHANDLED;

}

BOOL CDlgContacts::OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    COPYDATASTRUCT cds = *(COPYDATASTRUCT*)lParam;
    TCHAR szCommand[STRING_NORMAL];
    m_oStr->StringCopy(szCommand, (TCHAR*)cds.lpData);

    TCHAR szMsg[STRING_LARGE];
    m_oStr->Format(szMsg, _T("received msg: %s"), szCommand);


    WPARAM wpGui = 0;
    if(m_oStr->Compare(szCommand, _T("-c")) == 0)
        wpGui = GUI_Contacts;
    else if(m_oStr->Compare(szCommand, _T("-f")) == 0)
        wpGui = GUI_Favorites;
    else if(m_oStr->Compare(szCommand, _T("-h")) == 0)
        wpGui = GUI_CallLog;
    else if(m_oStr->Compare(szCommand, _T("-q")) == 0)
    {
        PostQuitMessage(0);
        return UNHANDLED;
    }

    PostMessage(m_hWnd, WM_CHANGE_Gui, wpGui, 0);
    return UNHANDLED;
}
	
BOOL CDlgContacts::OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam)		
{
    // try to keep the app running
    /*
    if(m_eWndType == WND_Dialog)
		EndDialog(m_hWnd, 0);
	else 
		PostQuitMessage(0); 
        */
	return TRUE;
}

BOOL CDlgContacts::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_CHANGE_Gui)
	{
		switch((EnumCurrentGui)wParam)
		{
		case GUI_Favorites:
			m_pGui = (CGuiBase*)&m_guiFavorites;
			break;
		case GUI_Contacts:
			m_pGui = (CGuiBase*)&m_guiContacts;
			break;
		case GUI_CallLog:
			m_pGui = (CGuiBase*)&m_guiCallLog;
			break;
		case GUI_Quit:
            {
/*#ifndef DEBUG
                ShowWindow(m_hWnd, SW_HIDE);
#else*/
                PostQuitMessage(0);
//#endif
            }
		default:
			break;
		}
        m_pGui->SetAllDirty();
	}
    else if((uiMessage >= PIM_ITEM_CREATED_LOCAL && uiMessage <= PIM_ITEM_CHANGED_LOCAL) ||
        (uiMessage >= PIM_ITEM_CREATED_REMOTE && uiMessage <= PIM_ITEM_CHANGED_REMOTE))
    {
        
        m_guiContacts.HandlePoomMessage(uiMessage, wParam, lParam);
        m_guiFavorites.HandlePoomMessage(uiMessage, wParam, lParam);

        // close the details window if it's up
        if(IsWindow(m_dlgContactDetails.GetWnd()))
            m_dlgContactDetails.CloseWindow(IDCANCEL);
    }
    else
        m_pGui->OnUser(hWnd, uiMessage, wParam, lParam);

	InvalidateRect(m_hWnd, NULL, FALSE);

	return TRUE;
}

BOOL CDlgContacts::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_pGui->OnTimer(wParam, lParam);
}

BOOL CDlgContacts::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)	
{
	return m_pGui->OnChar(wParam, lParam);
}

    
CIssGDIEx& CDlgContacts::GetGlowImg()
{
    // load once only... doesn't change size
    if(!m_gdiGlow.GetDC())
    {
        m_gdiGlow.LoadImage(SKIN(IDR_PNG_Glow), m_hWnd, m_hInst);
    }

    return m_gdiGlow;
}
    
void CDlgContacts::NotifyToolbarFocus(BOOL bHasFocus)
{
    if(bHasFocus)
    {
        if(m_pGui)
            m_pGui->LoseFocus();
    }
}
