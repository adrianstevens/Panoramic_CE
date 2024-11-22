#include "DlgOptions.h"
#include "resource.h"
#include "IssGDIEffects.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssBeveledEdge.h"

#define WM_Music	WM_USER + 10
#define WM_Effects	WM_USER + 11

struct TypeGameMode
{
	UINT	uiGameMode;
	UINT	uiGameDesc;
	EnumGameType eGameType;
};


TypeGameMode g_sGameMode [] = 
{
	{IDS_LBL_GM1, IDS_LBL_Desc1, GT_Classic},
	{IDS_LBL_GM2, IDS_LBL_Desc2, GT_Challenge},
	{IDS_LBL_GM3, IDS_LBL_Desc3, GT_Ultra},
	//{IDS_LBL_GM4, IDS_LBL_Desc4, GT_40},
};


CDlgOptions::CDlgOptions(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
,m_hBkFPS(NULL)
,m_hBkPower(NULL)
{
#ifdef WIN32_PLATFORM_WFSP
    m_hFontText				= CIssGDI::CreateFont(GetSystemMetrics(SM_CXICON)*8/22, FW_BOLD, TRUE);
    m_hFontSelected			= CIssGDI::CreateFont(GetSystemMetrics(SM_CXICON)*2/5, FW_BOLD, TRUE);
#else
    m_hFontText				= CIssGDI::CreateFont(GetSystemMetrics(SM_CXICON)/2, FW_BOLD, TRUE);
    m_hFontSelected			= CIssGDI::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);
#endif
}

CDlgOptions::~CDlgOptions(void)
{
	CIssGDI::DeleteFont(m_hFontText);
	CIssGDI::DeleteFont(m_hFontSelected);

	if(m_hBkFPS)
		::DeleteObject(m_hBkFPS);

	if(m_hBkPower)
		::DeleteObject(m_hBkPower);
}

BOOL CDlgOptions::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);
#endif

#ifdef WIN32_PLATFORM_WFSP
    SHMENUBARINFO mbi;
    memset(&mbi, 0, sizeof(mbi)); 
    mbi.cbSize		= sizeof(mbi);
    mbi.hwndParent	= m_hWnd;
    mbi.nToolBarId	= IDR_MENU_Hextris; 
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
		m_gdiMem.Create(hDC, rcClient, FALSE, FALSE);

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
	if(lParam == (LPARAM)m_lstGameMode)
	{
		if(HIWORD(wParam) == CBN_SELCHANGE)
		{
			int iIndex = ListGetCurSel(m_lstGameMode);
			// if there is a change then save the new game type
			if(g_sGameMode[iIndex].eGameType != m_oGame->GetGameType())
			{
				m_oGame->SetGameType(g_sGameMode[iIndex].eGameType);
				m_oGame->Reset();	// force a new game to restart
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
	}
	else if(lParam == (LPARAM)m_lstStartLevel)
	{
		if(HIWORD(wParam) == CBN_SELCHANGE)
		{
			int iIndex = ListGetCurSel(m_lstStartLevel);
			// if there is a change then save the new game type
			if(m_oGame->GetStartLevel() != iIndex)
			{
				m_oGame->SetStartLevel(iIndex);
				m_oGame->Reset();	// force a new game to restart
			}
		}
	}
	else if(lParam == (LPARAM)m_chkFPS)
	{
		m_oGame->SetShowFPS((BST_CHECKED==SendMessage(m_chkFPS, BM_GETCHECK,0,0)?TRUE:FALSE));
	}
	else if(lParam == (LPARAM)m_chkPower)
	{
		m_oGame->SetPowerSaving((BST_CHECKED==SendMessage(m_chkPower, BM_GETCHECK,0,0)?TRUE:FALSE));
	}
#ifdef WIN32_PLATFORM_WFSP
    else if(LOWORD(wParam) == IDMENU_Back)
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

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	MoveControls(rcClient);

	return TRUE;
}

BOOL CDlgOptions::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case VK_ESCAPE:
	case VK_TSOFT1:
		SafeCloseWindow(0);
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
		m_oGame->SaveRegistry();
		SafeCloseWindow(IDOK);
	}
	return TRUE;
}

BOOL CDlgOptions::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE) 
	{ 
		SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
		RECT rc; 
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);

		m_oKeys->RegisterHotKeys(m_hWnd, m_hInst, KEY_Soft1|KEY_Soft2);

		return TRUE; 
	}
	return UNHANDLED;
}



BOOL CDlgOptions::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uiMessage)
	{
	case WM_Effects:
		m_oGame->SetSFXLevel(ConvertTo64((int)wParam));
		m_oSoundFX->SetVolumeSFX(ConvertTo64((int)wParam));
		m_oSoundFX->PlaySFX(2);
		break;
	case WM_Music:
	{
		m_oGame->SetMusicLevel(ConvertTo64((int)wParam));
		m_oSoundFX->SetVolumeMOD(ConvertTo64((int)wParam));
		
		// play a sound effect to show what the new sound level will be
		int iSFXLevel = m_oSoundFX->GetVolumeSFX();
		m_oSoundFX->SetVolumeSFX(ConvertTo64((int)wParam));
		m_oSoundFX->PlaySFX(3);
        //InvalidateRect(m_sldMusic.GetWnd(), NULL, FALSE);
        //UpdateWindow(m_sldMusic.GetWnd());
		//Sleep(500); // let it play the sound before we change the volume back
		m_oSoundFX->SetVolumeSFX(iSFXLevel);
		break;
	}
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgOptions::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnmh = (LPNMHDR)lParam;
	ASSERT(lpnmh);

	switch(lpnmh->code)
	{
	/*case LVN_ITEMCHANGED:
	case CBN_SELCHANGE:
		if(lpnmh->hwndFrom == m_lstGameMode)
			InvalidateRect(hWnd, NULL, FALSE);
		break;*/
	case NM_PAINTBACKGROUND:
		{
			LPNMPAINTBACKGROUND lpBack = (LPNMPAINTBACKGROUND)lParam;

			if(m_gdiMem.GetDC() == NULL)
			{
				// no background yet so let's create it
				RECT rcClient;
				GetClientRect(hWnd, &rcClient);
				HDC hDC = GetDC(hWnd);
				m_gdiMem.Create(hDC, rcClient, FALSE, FALSE);
				ReleaseDC(hWnd, hDC);
				DrawBackground(rcClient);
			}

			POINT ptClient;
			ptClient.x	= lpBack->rcDraw.left;
			ptClient.y	= lpBack->rcDraw.top;
			ScreenToClient(m_hWnd, &ptClient);
			BitBlt(lpBack->hDC,
				   0,0,
				   WIDTH(lpBack->rcDraw), HEIGHT(lpBack->rcDraw),
				   m_gdiMem.GetDC(),
				   ptClient.x, ptClient.y,
				   SRCCOPY);

			break;
		}
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgOptions::FadeInScreen(HDC hDC, RECT rcClient)
{

	int iWidth	= GetSystemMetrics(SM_CXSCREEN);
	int iHeight = GetSystemMetrics(SM_CYSCREEN);

	SetRect(&rcClient, 0, 0, iWidth, iHeight);
	DrawBackground(rcClient);
	DrawText();

	CIssGDIEffects::FadeIn(hDC, m_gdiMem);

	CreateControls();
	MoveControls(rcClient);

	return TRUE;
}

void CDlgOptions::InitBackground()
{
	if(!m_gdiBackground.GetDC())
	{
		CIssGDI gdiTile;
		LoadImage(gdiTile, IDR_TILE_BG, m_hWnd, m_hInst);		

		RECT rcClient;
		::SetRect(&rcClient, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		m_gdiBackground.Create(m_gdiMem.GetDC(), rcClient, FALSE, TRUE);

		// copy the background over
		CIssGDI::TileBackGround(m_gdiBackground.GetDC(),
			rcClient,
			gdiTile,
			FALSE);

#ifndef WIN32_PLATFORM_WFSP
		// create the back button
		CIssBeveledEdge btnItem;
		btnItem.Initialize(m_hWnd, m_hInst, 
			IDR_MN_TL, IDR_MN_T, IDR_MN_TR, IDR_MN_R, IDR_MN_BR, IDR_MN_B, IDR_MN_BL, IDR_MN_L, IDR_MN_M);
		btnItem.InitializeAlpha(IDR_MNA_TL, IDR_MNA_T, IDR_MNA_TR, IDR_MNA_R, IDR_MNA_BR, IDR_MNA_B, IDR_MNA_BL, IDR_MNA_L, IDR_MNA_M);
		btnItem.SetLocation(m_rcBack);
		btnItem.Draw(m_gdiBackground, 255);
		TCHAR szText[STRING_MAX];
		m_oStr->StringCopy(szText, IDS_MENU_Back, STRING_MAX, m_hInst);
		CIssRect rcZero;
		rcZero.Set(m_rcBack);
		rcZero.Translate(1,1);
		CIssGDI::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(100,100,100));
		rcZero.Translate(-1,-1);
		CIssGDI::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255));
#endif

		// force the controls to redraw
		InvalidateRect(m_sldEffects.GetWnd(), NULL, FALSE);
		InvalidateRect(m_sldMusic.GetWnd(), NULL, FALSE);
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

	MyDrawText(IDS_LBL_Music, m_rcLabelMusic);
	MyDrawText(IDS_LBL_Effects, m_rcLabelEffects);
	MyDrawText(IDS_LBL_FPS, m_rcLabelFPS);
	MyDrawText(IDS_LBL_PowerSaving, m_rcLabelPowerSaving);
//	MyDrawText(IDS_LBL_GameMode, m_rcLabelGameMode);

	//int iIndex = ListGetCurSel(m_lstGameMode);
	//MyDrawText(g_sGameMode[iIndex].uiGameDesc, m_rcLabelDesc, DT_WORDBREAK);
	
}

void CDlgOptions::MoveControls(RECT rcClient)
{
	int iIndent			= GetSystemMetrics(SM_CXICON)/2;
	int iItemHeight		= GetSystemMetrics(SM_CXICON)*3/5;
	int iTextHeight		= GetSystemMetrics(SM_CXICON)/2;
	int iShadowSpace	= 2*HEIGHT(rcClient)/100;
	RECT rc;
    RECT rcSpin = {0};

#ifdef WIN32_PLATFORM_WFSP
	int iIconSize	= GetSystemMetrics(SM_CXICON)*2/3; // GetSystemMetrics(SM_CXICON)*2/3;//good height for smartphone
#else
	int iIconSize	= GetSystemMetrics(SM_CYVSCROLL);
#endif

#ifdef WIN32_PLATFORM_WFSP
	int iDropWindowSize = iIconSize;
#else
	int iDropWindowSize = 100;
	if (GetSystemMetrics(SM_CYSCREEN) > 320)
		iDropWindowSize = 300;
#endif

    //////////////////////////////////////////////////////////////////////////
    // Back button
    int iIndent2 = GetSystemMetrics(SM_CXICON)/4;
    m_rcBack.left	= iIndent2;
    m_rcBack.right	= m_rcBack.left + GetSystemMetrics(SM_CXSCREEN)/4;
    m_rcBack.top	= rcClient.bottom - GetSystemMetrics(SM_CYICON) - iIndent2;
    m_rcBack.bottom	= rcClient.bottom - iIndent2;

	rc.left		= rcClient.left + iIndent;
	rc.right	= rcClient.right - iIndent;

//	m_rcLabelGameMode	= rc;
//	m_rcLabelGameMode.top	= 0;
//	m_rcLabelGameMode.bottom= m_rcLabelGameMode.top + iTextHeight;

    //////////////////////////////////////////////////////////////////////////
    // Game Mode
	rc.top			= 0;
	rc.bottom		= rc.top + iItemHeight;
	MoveWindow(m_lstGameMode, rc.left, rc.top, WIDTH(rc), iDropWindowSize, FALSE);

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
    // Game Mode Label
	m_rcLabelDesc		= rc;
	/*m_rcLabelDesc.top	= rc.bottom;
	m_rcLabelDesc.bottom= m_rcLabelDesc.top + 3*iTextHeight;*/
	
    //////////////////////////////////////////////////////////////////////////
    // Start level
	rc.top			= m_rcLabelDesc.bottom;
	rc.bottom		= rc.top + iItemHeight;
	MoveWindow(m_lstStartLevel, rc.left, rc.top, WIDTH(rc), iDropWindowSize, FALSE);

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
	//FPS
	rc.top			= rc.bottom;
	rc.bottom		= rc.top + iItemHeight;
	rc.right		= rc.left + iIndent*3/2;
	MoveWindow(m_chkFPS, rc.left, rc.top, WIDTH(rc), HEIGHT(rc), FALSE);

	m_rcLabelFPS	= rc;
	m_rcLabelFPS.left	= rc.right;
	m_rcLabelFPS.right	= rcClient.right - iIndent;

    //////////////////////////////////////////////////////////////////////////
	//Power Savings
	rc.left		= rcClient.left + iIndent;
	rc.right	= rcClient.right - iIndent;

	rc.top			= rc.bottom;
	rc.bottom		= rc.top + iItemHeight;
	rc.right		= rc.left + iIndent*3/2;
	MoveWindow(m_chkPower, rc.left, rc.top, WIDTH(rc), HEIGHT(rc), FALSE);

	m_rcLabelPowerSaving		= rc;
	m_rcLabelPowerSaving.left	= rc.right;
	m_rcLabelPowerSaving.right	= rcClient.right - iIndent;

	rc.left		= rcClient.left + iIndent;
	rc.right	= rcClient.right - iIndent;

	m_rcLabelMusic			= rc;
	m_rcLabelMusic.top		= m_rcLabelPowerSaving.bottom;
	m_rcLabelMusic.bottom	= m_rcLabelMusic.top + iTextHeight;

    //////////////////////////////////////////////////////////////////////////
    // Music slider
	rc.top		= m_rcLabelMusic.bottom;
	rc.bottom	= rc.top + iItemHeight;	
	if(m_sldMusic.GetWnd())
		MoveWindow(m_sldMusic.GetWnd(), rc.left, rc.top, WIDTH(rc), HEIGHT(rc), FALSE);

	m_rcLabelEffects = rc;
	m_rcLabelEffects.top	= rc.bottom;
	m_rcLabelEffects.bottom	= m_rcLabelEffects.top + iTextHeight;	

    //////////////////////////////////////////////////////////////////////////
    // Effects slider
	rc.top		= m_rcLabelEffects.bottom;
	rc.bottom	= rc.top + iItemHeight;
	if(m_sldEffects.GetWnd())
		MoveWindow(m_sldEffects.GetWnd(), rc.left, rc.top, WIDTH(rc), HEIGHT(rc), FALSE);	

	if(m_hBkFPS)
	{
		DeleteObject(m_hBkFPS);
		m_hBkFPS = NULL;
	}
	m_hBkFPS = CreateFPSBrush();

	if(m_hBkPower)
	{
		DeleteObject(m_hBkPower);
		m_hBkPower = NULL;
	}
	m_hBkPower = CreatePowerBrush();
}

void CDlgOptions::CreateControls()
{

#ifdef WIN32_PLATFORM_PSPC
	m_lstGameMode		= CreateWindowEx(0, _T("ComboBox"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL| WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
	m_lstStartLevel		= CreateWindowEx(0, _T("ComboBox"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL| WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
#else
	m_lstGameMode		= CreateWindowEx(0, _T("LISTBOX"), NULL, WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
	m_hSpin1			= CreateWindowEx(0, _T("msctls_updown32"), NULL, WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_lstStartLevel		= CreateWindowEx(0, _T("LISTBOX"), NULL, WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_hSpin2			= CreateWindowEx(0, _T("msctls_updown32"), NULL, WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS | UDS_HORZ |0x200, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
#endif
	m_chkFPS			= CreateWindowEx(0, _T("Button"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
	m_chkPower			= CreateWindowEx(0, _T("Button"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);

    SetFocus(m_lstGameMode);

	SendMessage(m_chkFPS, BM_SETCHECK, (WPARAM)m_oGame->GetShowFPS(), 0);

	SendMessage(m_chkPower, BM_SETCHECK, (WPARAM)m_oGame->GetPowerSaving(), 0);

	int iCurrentGame = 0;
	TCHAR szText[STRING_MAX];
	for(int i=0; i<sizeof(g_sGameMode)/sizeof(TypeGameMode); i++)
	{
		if(m_oGame->GetGameType() == g_sGameMode[i].eGameType)
			iCurrentGame = i;

		m_oStr->StringCopy(szText, g_sGameMode[i].uiGameMode, STRING_MAX, m_hInst);
		ListAddString(m_lstGameMode, szText);
	}
	ListSetCurSel(m_lstGameMode, iCurrentGame);

	TCHAR szFormat[STRING_MAX];
	iCurrentGame = 0;
	m_oStr->StringCopy(szFormat, IDS_LBL_StartLevel, STRING_MAX, m_hInst);
	for(int i=0; i<9; i++)
	{
		if(m_oGame->GetStartLevel() == i)
			iCurrentGame = i;

		m_oStr->Format(szText, szFormat, i + 1);
		ListAddString(m_lstStartLevel, szText);
	}
	ListSetCurSel(m_lstStartLevel, iCurrentGame);

	m_sldEffects.Init(WM_Effects);
	m_sldMusic.Init(WM_Music);
	
	m_sldEffects.SetRange(0, 10);
	m_sldMusic.SetRange(0, 10);

	m_sldEffects.SetStep(1);
	m_sldMusic.SetStep(1);

	m_sldEffects.SetPos(ConvertTo10(m_oSoundFX->GetVolumeSFX()));
	m_sldMusic.SetPos(ConvertTo10(m_oSoundFX->GetVolumeMOD()));

	m_sldMusic.Create(m_hWnd, m_hInst);
	m_sldEffects.Create(m_hWnd, m_hInst);
	
	SetFocus(m_lstGameMode);

}

int CDlgOptions::ConvertTo10(int iValue)
{
	int iNewVal = iValue*10/64;
	return iNewVal;
}

int CDlgOptions::ConvertTo64(int iValue)
{
	int iNewVal = iValue*64/10;
	return iNewVal;
}

void CDlgOptions::MyDrawText(UINT uiText, RECT rcLocation, UINT uiFormat, COLORREF crText, COLORREF crShadow)
{
	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, uiText, STRING_MAX, m_hInst);

	rcLocation.left ++;
	rcLocation.right ++;
	
	CIssGDI::DrawText(m_gdiMem.GetDC(), szText, rcLocation, uiFormat, m_hFontText, crShadow);
	rcLocation.left --;
	rcLocation.top --;
	rcLocation.right --;
	rcLocation.bottom --;
	CIssGDI::DrawText(m_gdiMem.GetDC(), szText, rcLocation, uiFormat, m_hFontText, crText);
}

BOOL CDlgOptions::OnCtlColorStatic(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hDCStatic = (HDC) wParam;
	HWND hWndCtrl = (HWND) lParam;

	if(m_gdiBackground.GetDC() == NULL)
		InitBackground();

	if(hWndCtrl == m_chkFPS)
		return (BOOL)m_hBkFPS;
	else if(hWndCtrl == m_chkPower)
		return (BOOL)m_hBkPower;


	return UNHANDLED;
}

HBRUSH CDlgOptions::CreateFPSBrush()
{
	if(!m_chkFPS || m_gdiBackground.GetDC() == NULL)
		return NULL;

	RECT rc;
	GetWindowRect(m_chkFPS, &rc);

	CIssGDI gdiBack;
	gdiBack.Create(m_gdiBackground, rc, FALSE, FALSE, FALSE);

	BitBlt(gdiBack.GetDC(),
		0,0,
		WIDTH(rc), HEIGHT(rc),
		m_gdiBackground.GetDC(),
		rc.left, rc.top,
		SRCCOPY);

	return ::CreatePatternBrush(gdiBack.GetHBitmap());
}

HBRUSH CDlgOptions::CreatePowerBrush()
{
	if(!m_chkPower || m_gdiBackground.GetDC() == NULL)
		return NULL;

	RECT rc;
	GetWindowRect(m_chkPower, &rc);

	CIssGDI gdiBack;
	gdiBack.Create(m_gdiBackground, rc, FALSE, FALSE, FALSE);

	BitBlt(gdiBack.GetDC(),
		0,0,
		WIDTH(rc), HEIGHT(rc),
		m_gdiBackground.GetDC(),
		rc.left, rc.top,
		SRCCOPY);

	return ::CreatePatternBrush(gdiBack.GetHBitmap());
}



