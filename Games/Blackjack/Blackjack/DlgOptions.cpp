#include "DlgOptions.h"
#include "resource.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssImageSliced.h"
#include "IssRect.h"
#include "IssLocalisation.h"

#define COLOR_Grayed    RGB(150,150,150)

CDlgOptions::CDlgOptions(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
,m_hBkPlaySounds(NULL)
{
}

CDlgOptions::~CDlgOptions(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);

	if(m_hBkPlaySounds)
		::DeleteObject(m_hBkPlaySounds);

}

BOOL CDlgOptions::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
#ifdef WIN32_PLATFORM_WFSP
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
#else
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
#endif
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);
#endif

#ifdef WIN32_PLATFORM_WFSP
    SHMENUBARINFO mbi;
    memset(&mbi, 0, sizeof(mbi)); 
    mbi.cbSize		= sizeof(mbi);
    mbi.hwndParent	= m_hWnd;
    mbi.nToolBarId	= IDR_MENU_Options; 
    mbi.hInstRes	= m_hInst;
    SHCreateMenuBar(&mbi);

    ::SendMessage(	mbi.hwndMB, 
        SHCMBM_OVERRIDEKEY, 
        VK_TBACK, 
        MAKELPARAM(SHMBOF_NODEFAULT | SHMBOF_NOTIFY, SHMBOF_NODEFAULT | SHMBOF_NOTIFY));
#endif

	m_bFirstTime = TRUE;

	return TRUE;
}

BOOL CDlgOptions::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef WIN32_PLATFORM_WFSP
    if(HIWORD(lParam)==VK_TBACK && (0 != (MOD_KEYUP & LOWORD(lParam))))
    {
        SafeCloseWindow();
    }
#endif
    return UNHANDLED;
}

BOOL CDlgOptions::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, TRUE);

	if(m_bFirstTime)
	{
		FadeInScreen(hDC, rcClient);
		m_bFirstTime = FALSE;
	}

	DrawBackground(rcClient);
	DrawText();

	// draw all to the screen
	BitBlt(hDC,
		rcClient.left,rcClient.top,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiMem.GetDC(),
		rcClient.left,rcClient.top,
		SRCCOPY);


	return TRUE;
}

BOOL CDlgOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(lParam == (LPARAM)m_lstCardCountView)
	{
		if(HIWORD(wParam) == CBN_SELCHANGE)
		{
			int iIndex = ListGetCurSel(m_lstCardCountView);
			m_psOptions->eCardCountView = (EnumCardCountView)iIndex;
            UpdateCardCountingOptions();
		}
	}
	else if(lParam == (LPARAM)m_lstCardCounting)
	{
		if(HIWORD(wParam) == CBN_SELCHANGE)
		{
			int iIndex = ListGetCurSel(m_lstCardCounting);
			m_psOptions->eCardCount = (EnumCardCount)iIndex;
		}
	}
    else if(lParam == (LPARAM)m_lstGameDifficulty)
    {
        if(HIWORD(wParam) == CBN_SELCHANGE)
        {
            int iIndex = ListGetCurSel(m_lstGameDifficulty);
            m_psOptions->eGameDifficulty = (EnumGameDifficulty)iIndex;
            UpdateCardCountingOptions();
        }
    }
    else if(lParam == (LPARAM)m_lstLanguage)
    {
        if(HIWORD(wParam) == CBN_SELCHANGE)
        {
            int iIndex = ListGetCurSel(m_lstLanguage);

			if(iIndex == 1)
				g_cLocale.SetCurrentLanguage(LANG_GERMAN);
			else if(iIndex == 2)
				g_cLocale.SetCurrentLanguage(LANG_FRENCH);
			else if(iIndex == 3)
				g_cLocale.SetCurrentLanguage(LANG_DUTCH);
			else if(iIndex == 4)
				g_cLocale.SetCurrentLanguage(LANG_SPANISH);
            else if(iIndex == 5)
				g_cLocale.SetCurrentLanguage(LANG_PORTUGUESE);
            else if(iIndex == 6)
                g_cLocale.SetCurrentLanguage(LANG_JAPANESE);
            else
				g_cLocale.SetCurrentLanguage(LANG_ENGLISH);
        }


    }
	else if(lParam == (LPARAM)m_chkPlaySounds)
	{
		m_psOptions->bPlaySounds = (BST_CHECKED==SendMessage(m_chkPlaySounds, BM_GETCHECK,0,0)?TRUE:FALSE);
	}
#ifdef WIN32_PLATFORM_WFSP
    else if(LOWORD(wParam) == IDMENU_Save || LOWORD(wParam) == IDMENU_Cancel)
    {
        SafeCloseWindow();
    }
#endif

	return UNHANDLED;
}

BOOL CDlgOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_gdiBackground.Destroy();
	m_gdiMem.Destroy();

	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);
	m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/6, FW_BOLD, TRUE);
	m_hFontSelected			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	MoveControls(rcClient);

	return TRUE;
}

BOOL CDlgOptions::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_TSOFT1:
    case VK_ESCAPE:
		SafeCloseWindow();
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgOptions::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(PtInRect(&m_rcBack, pt))
	{
		SafeCloseWindow();
	}
	return TRUE;
}

BOOL CDlgOptions::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) 
	{ 
		SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
		RECT rc; 
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);

		return TRUE; 
	}
	return UNHANDLED;
}



BOOL CDlgOptions::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	/*switch(uiMessage)
	{
	default:
		return UNHANDLED;
	}*/
	return TRUE;
}

BOOL CDlgOptions::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	/*LPNMHDR lpnmh = (LPNMHDR)lParam;
	ASSERT(lpnmh);

	switch(lpnmh->code)
	{
	default:
		return UNHANDLED;
	}*/
	return TRUE;
}

BOOL CDlgOptions::FadeInScreen(HDC hDC, RECT rcClient)
{

	int iWidth	= GetSystemMetrics(SM_CXSCREEN);
	int iHeight = GetSystemMetrics(SM_CYSCREEN);

	SetRect(&rcClient, 0, 0, iWidth, iHeight);
	DrawBackground(rcClient);
	DrawText();

	FadeIn(hDC, m_gdiMem);

	CreateControls();
	MoveControls(rcClient);

	return TRUE;
}

void CDlgOptions::InitBackground()
{
	if(!m_gdiBackground.GetDC())
	{
        CIssGDIEx gdiTile;
        gdiTile.LoadImage(IDB_PNG_Felt, m_hWnd, m_hInst);

        RECT rcClient;
        GetClientRect(m_hWnd, &rcClient);
        m_gdiBackground.Create(m_gdiMem.GetDC(), rcClient, FALSE, TRUE);

        // copy the background over
        TileBackGround(m_gdiBackground,
            rcClient,
            gdiTile,
            FALSE);

#ifndef WIN32_PLATFORM_WFSP
        // create the back button
        CIssImageSliced btnItem;
        btnItem.Initialize(m_hWnd, m_hInst, (IsVGA()?IDR_PNG_MenuBtnVGA:IDR_PNG_MenuBtn));
        btnItem.SetSize(WIDTH(m_rcBack), HEIGHT(m_rcBack));
        btnItem.DrawImage(m_gdiBackground, m_rcBack.left, m_rcBack.top);
        TCHAR szText[STRING_MAX];
        m_oStr->StringCopy(szText, ID(IDS_MENU_Back), STRING_MAX, m_hInst);
        CIssRect rcZero;
        rcZero.Set(m_rcBack);
        rcZero.Translate(1,1);
        ::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(100,100,100));
        rcZero.Translate(-1,-1);
        ::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255));
#endif
	}
}

void CDlgOptions::DrawBackground(RECT rcLocation)
{
	if(m_gdiBackground.GetDC() == NULL)
		InitBackground();

	// copy the background over
	BitBlt(m_gdiMem.GetDC(),
		   rcLocation.left, rcLocation.top,
		   WIDTH(rcLocation), HEIGHT(rcLocation),
		   m_gdiBackground.GetDC(),
		   rcLocation.left, rcLocation.top,
		   SRCCOPY);
}

void CDlgOptions::DrawText()
{

    MyDrawText(ID(IDS_LBL_PlaySounds), m_rcLabelPlaySounds);
	MyDrawText(ID(IDS_LBL_CardCountView), m_rcLabelCardCountView);
	MyDrawText(ID(IDS_LBL_CardCounting), m_rcLabelCardCounting);
	MyDrawText(ID(IDS_LBL_GameDifficulty), m_rcLabelDifficulty);
    MyDrawText(ID(IDS_LBL_Language), m_rcLabelLanguage);


}

void CDlgOptions::MoveControls(RECT rcClient)
{
	int iIndent			= GetSystemMetrics(SM_CXICON)/2;
	int iItemHeight		= GetSystemMetrics(SM_CXICON)*3/5;
	int iTextHeight		= GetSystemMetrics(SM_CXICON)/2;
	int iShadowSpace	= 2*HEIGHT(rcClient)/100;
	RECT rc;

#ifdef WIN32_PLATFORM_WFSP
	int iIconSize	= GetSystemMetrics(SM_CXICON)*2/3; // GetSystemMetrics(SM_CXICON)*2/3;//good height for smartphone
#else
	int iIconSize	= GetSystemMetrics(SM_CYVSCROLL);
#endif

#ifdef WIN32_PLATFORM_WFSP
	int iDropWindowSize = iIconSize;
#else
	int iDropWindowSize = GetSystemMetrics(SM_CYSCREEN);
#endif

    int iIndent2 = GetSystemMetrics(SM_CXICON)/4;
    m_rcBack.left	= iIndent2;
    m_rcBack.right	= m_rcBack.left + GetSystemMetrics(SM_CXSCREEN)/4;
    m_rcBack.top	= rcClient.bottom - GetSystemMetrics(SM_CYICON) - iIndent2;
    m_rcBack.bottom	= rcClient.bottom - iIndent2;
    RECT rcSpin = {0};

	rc.left		= rcClient.left + iIndent;
	rc.right	= rcClient.right - iIndent;

    //////////////////////////////////////////////////////////////////////////
    //Play Sounds
    rc.top			= 0;
    rc.bottom		= rc.top + iItemHeight;
    rc.right		= rc.left + iIndent*3/2;
    MoveWindow(m_chkPlaySounds, rc.left, rc.top, WIDTH(rc), HEIGHT(rc), FALSE);

    m_rcLabelPlaySounds	= rc;
    m_rcLabelPlaySounds.left	= rc.right;
    m_rcLabelPlaySounds.right	= rcClient.right - iIndent;

    rc.left		= rcClient.left + iIndent;
    rc.right	= rcClient.right - iIndent;

    //////////////////////////////////////////////////////////////////////////
    // Game Difficulty
    m_rcLabelDifficulty		= rc;
    m_rcLabelDifficulty.top	= rc.bottom;
    m_rcLabelDifficulty.bottom= m_rcLabelDifficulty.top + iTextHeight;

	rc.top			= m_rcLabelDifficulty.bottom;
	rc.bottom		= rc.top + iItemHeight;
	MoveWindow(m_lstGameDifficulty, rc.left, rc.top, WIDTH(rc), iDropWindowSize, FALSE);

#ifdef WIN32_PLATFORM_WFSP
     GetClientRect(m_hSpin1, &rcSpin);
    SetWindowPos(m_hSpin1, 
        HWND_TOPMOST,
        rc.right - WIDTH(rcSpin),
        rc.top,
        WIDTH(rcSpin),
        iDropWindowSize,
        SWP_SHOWWINDOW);
#endif

    //////////////////////////////////////////////////////////////////////////
    // Card Count View
	m_rcLabelCardCountView		= rc;
	m_rcLabelCardCountView.top	= rc.bottom;
	m_rcLabelCardCountView.bottom= m_rcLabelCardCountView.top + iTextHeight;
	
	rc.top			= m_rcLabelCardCountView.bottom;
	rc.bottom		= rc.top + iItemHeight;
	MoveWindow(m_lstCardCountView, rc.left, rc.top, WIDTH(rc), iDropWindowSize, FALSE);

#ifdef WIN32_PLATFORM_WFSP
    GetClientRect(m_hSpin2, &rcSpin);
    SetWindowPos(m_hSpin2, 
        HWND_TOPMOST,
        rc.right - WIDTH(rcSpin),
        rc.top,
        WIDTH(rcSpin),
        iDropWindowSize,
        SWP_SHOWWINDOW);
#endif

    //////////////////////////////////////////////////////////////////////////
	//Card Counting
    m_rcLabelCardCounting		= rc;
    m_rcLabelCardCounting.top	= rc.bottom;
    m_rcLabelCardCounting.bottom= m_rcLabelCardCounting.top + iTextHeight;

	rc.top			= m_rcLabelCardCounting.bottom;
	rc.bottom		= rc.top + iItemHeight;
	MoveWindow(m_lstCardCounting, rc.left, rc.top, WIDTH(rc), iDropWindowSize, FALSE);

#ifdef WIN32_PLATFORM_WFSP
    GetClientRect(m_hSpin3, &rcSpin);
    SetWindowPos(m_hSpin3, 
        HWND_TOPMOST,
        rc.right - WIDTH(rcSpin),
        rc.top,
        WIDTH(rcSpin),
        iDropWindowSize,
        SWP_SHOWWINDOW);
#endif


    //////////////////////////////////////////////////////////////////////////
    //Language
    m_rcLabelLanguage = rc;
    m_rcLabelLanguage.top = rc.bottom;
    m_rcLabelLanguage.bottom = m_rcLabelLanguage.top + iTextHeight;

    rc.top = m_rcLabelLanguage.bottom;
    rc.bottom = rc.top + iItemHeight;
    MoveWindow(m_lstLanguage, rc.left, rc.top, WIDTH(rc), iDropWindowSize, FALSE);

#ifdef WIN32_PLATFORM_WFSP
    GetClientRect(m_hSpin4, &rcSpin);
    SetWindowPos(m_hSpin4, 
        HWND_TOPMOST,
        rc.right - WIDTH(rcSpin),
        rc.top,
        WIDTH(rcSpin),
        iDropWindowSize,
        SWP_SHOWWINDOW);
#endif


    //////////////////////////////////////////////////////////////////////////
    // Play Sounds brush
	if(m_hBkPlaySounds)
	{
		DeleteObject(m_hBkPlaySounds);
		m_hBkPlaySounds = NULL;
	}
	m_hBkPlaySounds = CreatePlaySoundsBrush();

}

void CDlgOptions::CreateControls()
{

    m_chkPlaySounds		= CreateWindowEx(0, _T("Button"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);

#ifdef WIN32_PLATFORM_PSPC
	m_lstGameDifficulty	= CreateWindowEx(0, _T("ComboBox"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL| WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
	m_lstCardCountView	= CreateWindowEx(0, _T("ComboBox"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL| WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_lstCardCounting	= CreateWindowEx(0, _T("ComboBox"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL| WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_lstLanguage   	= CreateWindowEx(0, _T("ComboBox"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL| WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
#else
	m_lstGameDifficulty	= CreateWindowEx(0, _T("LISTBOX"), NULL, WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_hSpin1			= CreateWindowEx(0, _T("msctls_updown32"), NULL, WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_lstCardCountView	= CreateWindowEx(0, _T("LISTBOX"), NULL, WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_hSpin2			= CreateWindowEx(0, _T("msctls_updown32"), NULL, WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_lstCardCounting	= CreateWindowEx(0, _T("LISTBOX"), NULL, WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_hSpin3			= CreateWindowEx(0, _T("msctls_updown32"), NULL, WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_lstLanguage	    = CreateWindowEx(0, _T("LISTBOX"), NULL, WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_hSpin4			= CreateWindowEx(0, _T("msctls_updown32"), NULL, WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
	
#endif
	
    // set the play sounds
	SendMessage(m_chkPlaySounds, BM_SETCHECK, (WPARAM)m_psOptions->bPlaySounds, 0);

	TCHAR szText[STRING_MAX];

    // set the game difficulty items
    m_oStr->StringCopy(szText, ID(IDS_CMB_GD_Easy), STRING_MAX, m_hInst);
    ListAddString(m_lstGameDifficulty, szText);
    m_oStr->StringCopy(szText, ID(IDS_CMB_GD_Normal), STRING_MAX, m_hInst);
    ListAddString(m_lstGameDifficulty, szText);
    m_oStr->StringCopy(szText, ID(IDS_CMB_GD_Difficult), STRING_MAX, m_hInst);
    ListAddString(m_lstGameDifficulty, szText);
    m_oStr->StringCopy(szText, ID(IDS_CMB_GD_Learn), STRING_MAX, m_hInst);
    ListAddString(m_lstGameDifficulty, szText);
    ListSetCurSel(m_lstGameDifficulty, (int)m_psOptions->eGameDifficulty);

    // set the card count view items
    m_oStr->StringCopy(szText, ID(IDS_CMB_CCV_On), STRING_MAX, m_hInst);
    ListAddString(m_lstCardCountView, szText);
    m_oStr->StringCopy(szText, ID(IDS_CMB_CCV_Off), STRING_MAX, m_hInst);
    ListAddString(m_lstCardCountView, szText);
#ifdef WIN32_PLATFORM_PSPC
    m_oStr->StringCopy(szText, ID(IDS_CMB_CCV_Semi), STRING_MAX, m_hInst);
    ListAddString(m_lstCardCountView, szText);
#endif
    ListSetCurSel(m_lstCardCountView, (int)m_psOptions->eCardCountView);

    // set the game difficulty items
    m_oStr->StringCopy(szText, ID(IDS_CMB_CC_Wizard), STRING_MAX, m_hInst);
    ListAddString(m_lstCardCounting, szText);
    m_oStr->StringCopy(szText, ID(IDS_CMB_CC_KO), STRING_MAX, m_hInst);
    ListAddString(m_lstCardCounting, szText);
    m_oStr->StringCopy(szText, ID(IDS_CMB_CC_HiLow), STRING_MAX, m_hInst);
    ListAddString(m_lstCardCounting, szText);
    m_oStr->StringCopy(szText, ID(IDS_CMB_CC_HiOpt1), STRING_MAX, m_hInst);
    ListAddString(m_lstCardCounting, szText);
    m_oStr->StringCopy(szText, ID(IDS_CMB_CC_HiOpt2), STRING_MAX, m_hInst);
    ListAddString(m_lstCardCounting, szText);
    m_oStr->StringCopy(szText, ID(IDS_CMB_CC_Zen), STRING_MAX, m_hInst);
    ListAddString(m_lstCardCounting, szText);
    m_oStr->StringCopy(szText, ID(IDS_CMB_CC_OmegaII), STRING_MAX, m_hInst);
    ListAddString(m_lstCardCounting, szText);
    ListSetCurSel(m_lstCardCounting, (int)m_psOptions->eCardCount);

    //and finally the language
    g_cLocale.GetLanguageText(szText, LANG_ENGLISH);
    ListAddString(m_lstLanguage, szText);
	g_cLocale.GetLanguageText(szText, LANG_GERMAN);
	ListAddString(m_lstLanguage, szText);
	g_cLocale.GetLanguageText(szText, LANG_FRENCH);
	ListAddString(m_lstLanguage, szText);
	g_cLocale.GetLanguageText(szText, LANG_DUTCH);
	ListAddString(m_lstLanguage, szText);
	g_cLocale.GetLanguageText(szText, LANG_SPANISH);
	ListAddString(m_lstLanguage, szText);
    g_cLocale.GetLanguageText(szText, LANG_PORTUGUESE);
    ListAddString(m_lstLanguage, szText);
    g_cLocale.GetLanguageText(szText, LANG_JAPANESE);
    ListAddString(m_lstLanguage, szText);
    DWORD dwCurrent = g_cLocale.GetCurrentLanguage();
	if(dwCurrent == LANG_GERMAN)
		ListSetCurSel(m_lstLanguage, 1);
	else if(dwCurrent == LANG_FRENCH)
		ListSetCurSel(m_lstLanguage, 2);
	else if(dwCurrent == LANG_DUTCH)
		ListSetCurSel(m_lstLanguage, 3);
	else if(dwCurrent == LANG_SPANISH)
		ListSetCurSel(m_lstLanguage, 4);
	else if(dwCurrent == LANG_PORTUGUESE)
        ListSetCurSel(m_lstLanguage, 5);
    else if(dwCurrent == LANG_JAPANESE)
        ListSetCurSel(m_lstLanguage, 6);
    else
        ListSetCurSel(m_lstLanguage, 0);

    UpdateCardCountingOptions();

    //and set focus to one of the controls
    SetFocus(m_chkPlaySounds);
}

void CDlgOptions::MyDrawText(UINT uiText, RECT rcLocation, UINT uiFormat, COLORREF crText, COLORREF crShadow)
{
	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, uiText, STRING_MAX, m_hInst);

	rcLocation.left ++;
	rcLocation.right ++;
	
    ::DrawText(m_gdiMem.GetDC(), szText, rcLocation, uiFormat, m_hFontText, crShadow);
	rcLocation.left --;
	rcLocation.top --;
	rcLocation.right --;
	rcLocation.bottom --;
    ::DrawText(m_gdiMem.GetDC(), szText, rcLocation, uiFormat, m_hFontText, crText);
}

BOOL CDlgOptions::OnCtlColorStatic(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hDCStatic = (HDC) wParam;
	HWND hWndCtrl = (HWND) lParam;

	if(m_gdiBackground.GetDC() == NULL)
		InitBackground();

	if(hWndCtrl == m_chkPlaySounds)
		return (BOOL)m_hBkPlaySounds;

	return UNHANDLED;
}

HBRUSH CDlgOptions::CreatePlaySoundsBrush()
{
	if(!m_chkPlaySounds)
		return NULL;

	RECT rc;
	GetWindowRect(m_chkPlaySounds, &rc);

	CIssGDIEx gdiBack;
	gdiBack.Create(m_gdiBackground, rc, FALSE, FALSE, FALSE);

	BitBlt(gdiBack.GetDC(),
		0,0,
		WIDTH(rc), HEIGHT(rc),
		m_gdiBackground.GetDC(),
		rc.left, rc.top,
		SRCCOPY);

	return ::CreatePatternBrush(gdiBack.GetHBitmap());
}

void CDlgOptions::UpdateCardCountingOptions()
{
    int iIndex = ListGetCurSel(m_lstGameDifficulty);
    
    // if we selected "learn to play"
    if(iIndex == 3)
    {
    //    EnableWindow(m_lstCardCounting, FALSE);
        EnableWindow(m_lstCardCountView, FALSE);
    }
    else
    {
        EnableWindow(m_lstCardCountView, TRUE);

     /*   iIndex = ListGetCurSel(m_lstCardCountView);

        // if our card count view is off
        if(iIndex == 1)
            EnableWindow(m_lstCardCounting, FALSE);
        else
            EnableWindow(m_lstCardCounting, TRUE);*/
    }
}




