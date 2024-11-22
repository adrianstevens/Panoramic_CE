#include "StdAfx.h"
#include "DlgContacts.h"
#include "IssHardwareKeys.h"
#include "CommonDefines.h"
#include "IssCommon.h"
#include "Resource.h"
#include "DlgBase.h"
#include "DlgOptions.h"
#include "DlgPerContactBase.h"
#include "IssRegistry.h"
#include "DlgAbout.h"
#include "IssStateAndNotify.h"
#include "DlgChooseContact.h"

CDlgContacts* gDlgContacts = NULL;
CDlgContacts::CDlgContacts(EnumCurrentGui eSelection)
:m_pGui(NULL)
,m_guiToolBar()
,m_bPreventUsage(FALSE)
{
    gDlgContacts = this;
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


    CIssStateAndNotify::InitializeNotifications();

	m_hFontNormal	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
 	m_hFontBold		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
}

CDlgContacts::~CDlgContacts(void)
{
    CIssGDIEx::DeleteFont(m_hFontNormal);
	CIssGDIEx::DeleteFont(m_hFontBold);

	m_wndMenu.ResetContent();
    CIssStateAndNotify::DestroyNotifications();
	CIssKineticList::DeleteAllContent();
	CDlgPerContactBase::DeleteAllContent();
	
}
TypeOptions& CDlgContacts::GetOptions()
{
    return gDlgContacts->m_sOptions;
}
    
CIssGDIEx& CDlgContacts::GetCallIcons()
{
    return gDlgContacts->GetIconArray();
}

CIssGDIEx& CDlgContacts::GetCallIconsGray()
{
    return gDlgContacts->GetIconArrayGray();
}

HFONT CDlgContacts::GetFontNormal()
{
    if(gDlgContacts)
        return gDlgContacts->_GetFontNormal();
    else 
        return NULL;
}

HFONT CDlgContacts::GetFontBold()
{
    if(gDlgContacts)
        return gDlgContacts->_GetFontBold();
    else
        return NULL;
}

void CDlgContacts::SaveMyOptions()
{
    if(gDlgContacts)
        gDlgContacts->SaveOptions();
}

BOOL CDlgContacts::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    LoadOptions();

    // MUST init poom with handle to this window so we can get notifications FIRST
    // SUBSEQUENT calls use CPoomContacts::Instance();
    CPoomContacts *pPoom = new CPoomContacts(m_hWnd, m_sOptions.bShowSimContacts);  


    //pPoom->Destroy();
    //pPoom->CreateNew(m_hWnd);
    //pPoom->Initialize(m_hWnd, m_sOptions.bShowSimContacts);

    //////////////////////////////////////////////////////////////////////////
    // DEBUG code: for adding a large amount of contacts
/*#ifdef DEBUG
    TypeContact sContact;
    ZeroMemory(&sContact, sizeof(TypeContact));
    sContact.szFirstName	= m_oStr->CreateAndCopy(_T("iSS"));
    sContact.szLastName		= new TCHAR[40];
    sContact.szMobileNumber = new TCHAR[40];

    for(int i=0; i<1; i++)
    {
        m_oStr->Format(sContact.szLastName, _T("Test%.4d"), i);
        m_oStr->Format(sContact.szMobileNumber, _T("604-555-%.4d"), i);
       pPoom->SetContact(sContact);
    }
    m_oStr->Delete(&sContact.szFirstName);
    m_oStr->Delete(&sContact.szLastName);
    m_oStr->Delete(&sContact.szMobileNumber);
#endif*/
    // end DEBUG code
    //////////////////////////////////////////////////////////////////////////

    if(m_oStr)
        m_oStr->SetResourceInstance(m_hInst);

    

    //all 3 guis should be initialized here
    m_guiCallLog.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions, &m_dlgContactDetails);
    m_guiCallLog.SetMenu(&m_wndMenu);
    m_guiContacts.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions, &m_dlgContactDetails);
    m_guiContacts.SetMenu(&m_wndMenu);
    m_guiFavorites.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions, &m_dlgContactDetails);
    m_guiFavorites.SetMenu(&m_wndMenu);
    m_guiFavorites.SetGlowImage(&GetGlowImg());    
     

    // init background
    m_guiBackground.Init(m_hWnd, m_hInst);
    m_guiBackground.PreloadImages();    

    // init toolbar
	m_guiToolBar.Init(m_hWnd, m_hInst, &m_gdiMem, &m_gdiBg, &m_guiBackground, &m_sOptions, this);
    m_guiToolBar.SetGlowImage(&GetGlowImg());

    m_dlgContactDetails.PreloadImages(m_hWnd, m_hInst, &m_gdiMem, &m_wndMenu);
    CDlgPerContactBase::PreloadImages(m_hWnd, m_hInst, &m_gdiMem, &m_wndMenu);	

    // init global menu
	m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
	m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
	m_wndMenu.SetBackground(IDR_PNG_Group);
	m_wndMenu.PreloadImages(hWnd, m_hInst);

    // init hardware keys
    //CIssHardwareKeys* oKeys = CIssHardwareKeys::Instance();
    //if(oKeys)
    //    oKeys->RegisterHotKeys(m_hWnd, m_hInst, KEY_Talk, HANDLE_Message);

    // register for snapi notifications
    CIssStateAndNotify::DestroyNotifications();
    CIssStateAndNotify::RegisterWindowNotification(m_hWnd, WM_NOTIFY_TIME);
    CIssStateAndNotify::RegisterWindowNotification(m_hWnd, WM_NOTIFY_SMS);
    CIssStateAndNotify::RegisterWindowNotification(m_hWnd, WM_NOTIFY_MISSED);
    CIssStateAndNotify::RegisterWindowNotification(m_hWnd, WM_NOTIFY_TALKING);

    CheckDemo();

    ::SetCursor(NULL);
	return TRUE;
}

BOOL CDlgContacts::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

CIssGDIEx& CDlgContacts::GetIconArray()
{
    if(!m_gdiImgIconArray.GetDC())
    {
        UINT uiRes = IsVGA() ? IDR_PNG_HistoryArrayVGA : IDR_PNG_HistoryArray;
        m_gdiImgIconArray.LoadImage(uiRes, m_hWnd, m_hInst, TRUE);
    }

    return m_gdiImgIconArray;
}

CIssGDIEx& CDlgContacts::GetIconArrayGray()
{
    if(!m_gdiImgIconArrayGray.GetDC())
    {
        UINT uiRes = IsVGA() ? IDR_PNG_HistoryArrayVGA : IDR_PNG_HistoryArray;
        m_gdiImgIconArrayGray.LoadImage(uiRes, m_hWnd, m_hInst, TRUE);
        m_gdiImgIconArrayGray.GrayScale();
    }

    return m_gdiImgIconArrayGray;
}

void CDlgContacts::LoadOptions()
{
    m_sOptions.bShowSliderBar       = TRUE;
    m_sOptions.eShowCall            = CALL_All;
    m_sOptions.eShowHistory         = HISTORY_All;
    m_sOptions.eShowSMS             = SMS_All;
    m_sOptions.eSortContacts        = SORT_LastName;
    m_sOptions.bAlwaysShowText      = FALSE;
    m_sOptions.eFavSize             = FAVSIZE_Normal;
    m_sOptions.bContactsVerboseMode = FALSE;
    m_sOptions.bShowAnimations      = TRUE;
    m_sOptions.eSlideLeft           = SLIDE_Personal;
    m_sOptions.eSlideRight          = SLIDE_Business;
    m_sOptions.bShowSimContacts     = TRUE;

    DWORD dwSize = sizeof(TypeOptions);
    if(S_OK != GetKey(REG_Save, _T("ContactOptions"), (LPBYTE)&m_sOptions, dwSize))
    {
        // set some defaults here
        
    }

    if((int)m_sOptions.eFavSize < 3 || (int)m_sOptions.eFavSize > 5)
        m_sOptions.eFavSize = FAVSIZE_Normal;//messed up the registry when testing ... better safe than sorry

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
            CDlgPerContactBase::ReloadColorSchemeItems(m_hWnd, m_hInst);

            // glow
            m_gdiGlow.Destroy();
            GetGlowImg();

            // favorites
            m_guiFavorites.ReloadColorSchemeItems(m_hWnd, m_hInst);

            // contacts
            m_guiContacts.ReloadColorSchemeItems(m_hWnd, m_hInst);

			// History
			m_guiCallLog.ReloadColorSchemeItems(m_hWnd, m_hInst);

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
            m_guiCallLog.RefreshList();
        }
        // changes to contacts sorting
        if(sOld.eSortContacts != sNew.eSortContacts || 
           sOld.bContactsVerboseMode != sNew.bContactsVerboseMode ||
           sOld.bShowSimContacts != sNew.bShowSimContacts)
        {
            CPoomContacts* pPoom = CPoomContacts::Instance();
            if(pPoom && sOld.bShowSimContacts != sNew.bShowSimContacts)
            {
                // have to re-initialize POOM to either show or not show contacts
                pPoom->Destroy();
                pPoom->Initialize(m_hWnd, m_sOptions.bShowSimContacts);
            }
            m_guiContacts.RefreshContacts();
        }

        if(eOldSize != m_sOptions.eFavSize)
            m_guiFavorites.SetIconSize(m_sOptions.eFavSize);

        if(sOld.eSlideLeft != sNew.eSlideLeft || sOld.eSlideRight != sNew.eSlideRight)
            m_guiFavorites.ResetSliderText();

        SaveOptions();
        ::SetCursor(hCursor);
    }
}

void CDlgContacts::LaunchMenu()
{
	if(m_wndMenu.IsWindowUp(TRUE) || !m_pGui)
		return;

	m_wndMenu.ResetContent();
#ifndef DEBUG
    if(m_bPreventUsage)
    {}
    else
#endif
	m_pGui->AddMenuItems();

    m_wndMenu.AddSeparator();
    m_wndMenu.AddItem(IDS_MENU_Options, m_hInst, IDMENU_Options);
    m_wndMenu.AddItem(IDS_MENU_Help, m_hInst, IDMENU_Help);
    m_wndMenu.AddItem(IDS_MENU_About, m_hInst, IDMENU_About);
    m_wndMenu.AddItem(IDS_MENU_Quit, m_hInst, IDMENU_Quit);

    m_wndMenu.SetSelectedItemIndex(0, TRUE);

    int iWidth = GetSystemMetrics(SM_CXICON)*11/2;//this could be dynamic for larger text

    int iX = WIDTH(m_pGui->GetFrame())/6;

	m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
		iX,10,
        //WIDTH(m_pGui->GetFrame())*2/3, m_pGui->GetFrame().bottom - 10,
        iWidth, m_pGui->GetFrame().bottom - 10,
		m_pGui->GetFrame().left, m_pGui->GetFrame().bottom - 10, iX,10, ADJUST_Bottom);
}

BOOL CDlgContacts::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
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
            ::DrawText(m_gdiMem, _T("Thank you for trying Panoramic Phone Genius.  Your trial period is now over.  Please return to the location that provided your download to purchase"), m_pGui->GetFrame(), DT_TOP|DT_CENTER|DT_WORDBREAK, GetFontNormal(), RGB(255,255,255));
        }
    }
    else
#endif
    {
    if(m_pGui)
	    m_pGui->Draw(m_gdiMem, m_gdiMem.GetDC(), m_pGui->GetFrame(), rcClip);
    }
	    
	// draw the toolbar
    m_guiToolBar.Draw(m_gdiMem, m_gdiMem.GetDC(), rcClient, rcClip);

    // add the selector
    m_guiToolBar.DrawSelector(m_gdiMem, rcClient, rcClip);

    // special case for the History screen drop down
    if(m_pGui == &m_guiCallLog && m_guiCallLog.IsQuickSetDown())
    {
        // copy to background GDI for animate out
        BitBlt(m_gdiBg,
               0,0,
               WIDTH(rcClient), HEIGHT(rcClient),
               m_gdiMem,
               0,0,
               SRCCOPY);
        AlphaFillRect(m_gdiMem, rcClient, 0, 150);
        m_guiCallLog.DrawQuickSetButtons(m_gdiMem, rcClient);
    }

	// draw it all to the screen
	BitBlt(hDC,
		   rcClip.left,rcClip.top,
		   WIDTH(rcClip), HEIGHT(rcClip),
		   m_gdiMem.GetDC(),
		   rcClip.left,rcClip.top,
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

	BOOL bLandscape = (rcClient.right > rcClient.bottom);

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
	m_guiContacts.MoveGui(rcTemp);
	m_guiFavorites.MoveGui(rcTemp);
	m_guiCallLog.MoveGui(rcTemp);

    ::SetCursor(hCursor);

	return TRUE;
}

BOOL CDlgContacts::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) 
	{ 
		SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON); 
		RECT rc; 
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
	}
    
    return (m_pGui?m_pGui->OnActivate(hWnd, wParam, lParam):TRUE); 
}

BOOL CDlgContacts::OnLButtonDown(HWND hWnd, POINT& pt)
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

	if(m_pGui && !m_pGui->OnLButtonDown(pt))
		m_guiToolBar.OnLButtonDown(pt);
	else
		m_guiToolBar.LoseFocus();

	return TRUE;
}

BOOL CDlgContacts::OnLButtonUp(HWND hWnd, POINT& pt)
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

	if(m_pGui && !m_pGui->OnLButtonUp(pt))
		m_guiToolBar.OnLButtonUp(pt);

	return TRUE;
}

BOOL CDlgContacts::OnMouseMove(HWND hWnd, POINT& pt)
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

	if(m_pGui && !m_pGui->OnMouseMove(pt))
		m_guiToolBar.OnMouseMove(pt);

	return TRUE;
}

BOOL CDlgContacts::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        m_guiToolBar.OnKeyDown(hWnd, wParam, lParam);
        return TRUE;
    }
#endif

    if(m_pGui && m_pGui->OnKeyDown(hWnd, wParam, lParam))
    {
        m_guiToolBar.LoseFocus();
    }
    else
    {
        m_guiToolBar.OnKeyDown(hWnd, wParam, lParam);
    }
	return TRUE;
}

BOOL CDlgContacts::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
    else if(m_pGui && m_pGui->OnKeyUp(hWnd, wParam, lParam))
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
        LaunchHelp();
        break;
    case IDMENU_About:
        LaunchAbout();
        break;
    case IDMENU_Quit:
        PostQuitMessage(0);
        break;
    default:
        return (m_pGui?m_pGui->OnCommand(wParam, lParam):TRUE);
    }

    return UNHANDLED;

}

BOOL CDlgContacts::OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    COPYDATASTRUCT cds = *(COPYDATASTRUCT*)lParam;
    TCHAR szCommand[STRING_NORMAL] = _T("");
    if(cds.cbData != 0)
        m_oStr->StringCopy(szCommand, (TCHAR*)cds.lpData);

    TCHAR szMsg[STRING_LARGE];
    m_oStr->Format(szMsg, _T("received msg: %s"), szCommand);


    WPARAM wpGui = GUI_Favorites;
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
#ifndef DEBUG
                ShowWindow(m_hWnd, SW_HIDE);
#else
                PostQuitMessage(0);
#endif
            }
		default:
			break;
		}
        if(m_pGui)
        {
            m_pGui->SetAllDirty();
            m_guiToolBar.SetAllDirty();
            m_pGui->OnFocus();
        }
	}
    else if((uiMessage >= PIM_ITEM_CREATED_LOCAL && uiMessage <= PIM_ITEM_CHANGED_LOCAL) ||
        (uiMessage >= PIM_ITEM_CREATED_REMOTE && uiMessage <= PIM_ITEM_CHANGED_REMOTE))
    {
        
        m_guiContacts.HandlePoomMessage(uiMessage, wParam, lParam);
        m_guiFavorites.HandlePoomMessage(uiMessage, wParam, lParam);

        // close the details window if it's up
        //if(IsWindow(m_dlgContactDetails.GetWnd()))
        //    m_dlgContactDetails.CloseWindow(IDCANCEL);
    }
    else if(uiMessage == WM_NOTIFY_MISSED || uiMessage == WM_NOTIFY_TIME || uiMessage == WM_NOTIFY_SMS || uiMessage == WM_NOTIFY_TALKING)
    {
        if(uiMessage == WM_NOTIFY_TIME)
        {
            DWORD dwListTickCount = CDlgChooseContact::GetListTickCount();
            // greater then ten minutes, then delete choose contacts list
            if(CDlgChooseContact::GetListCount() > 0 && (GetTickCount() - dwListTickCount) > 600000)
                CDlgChooseContact::ResetListContent();

#ifdef DEBUG
            DebugOutMemory();

#endif

            SYSTEMTIME sysTime;
            GetLocalTime(&sysTime);
            if(sysTime.wMinute % 10 == 0)
                CheckDemo();

            if(m_bPreventUsage)
            {
                RECT rcClient;
                GetClientRect(m_hWnd, &rcClient);
                FillRect(m_gdiMem, rcClient, 0);
                SetDirty(rcClient);
                InvalidateRect(m_hWnd, NULL, FALSE);
            }
        }
        m_guiCallLog.OnUser(hWnd, uiMessage, wParam, lParam);
    }
    else if(m_pGui)
        m_pGui->OnUser(hWnd, uiMessage, wParam, lParam);

	InvalidateRect(m_hWnd, NULL, FALSE);

	return TRUE;
}

BOOL CDlgContacts::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return (m_pGui?m_pGui->OnTimer(wParam, lParam):TRUE);
}

BOOL CDlgContacts::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)	
{
    return (m_pGui?m_pGui->OnChar(wParam, lParam):TRUE);
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

void CDlgContacts::LaunchAbout()
{
    CDlgAbout dlg;
    dlg.Init(&m_gdiMem, &m_guiBackground);
    dlg.Launch(m_hWnd, m_hInst, FALSE);
}

void CDlgContacts::LaunchHelp()
{
    //SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_SHOWSIPBUTTON);
    ShowWindow(m_hWnd, SW_HIDE);
    CreateProcess(_T("peghelp"), _T("contactshelp.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
}



void CDlgContacts::CheckDemo()
{
    CIssKey oKey;
    oKey.Init(_T("SOFTWARE\\Panoramic\\PanoContactsManager"), _T("SOFTWARE\\Pano\\CM2"), 0x0, 0x393EF71D, 0x67ABCEF1, 5, TRUE);
    oKey.CheckSetKey();
    oKey.CheckDemo();

    m_bPreventUsage = !oKey.m_bGood;
}
