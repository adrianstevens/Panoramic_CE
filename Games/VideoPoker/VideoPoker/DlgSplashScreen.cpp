#include "DlgSplashScreen.h"
#include "resource.h"
#include "IssGDIFx.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"

#define IDT_Timeout		100

extern CObjGui* g_gui;

CDlgSplashScreen::CDlgSplashScreen(void)
:m_iPercent(0)
{
}

CDlgSplashScreen::~CDlgSplashScreen(void)
{
}

BOOL CDlgSplashScreen::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);
#endif
    g_gui->Init(hWnd, m_hInst);

	HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    m_imgLoad.Initialize(hWnd, m_hInst, IDB_PNG_Loader);
    m_imgLoadInside.Initialize(hWnd, m_hInst, IDB_PNG_Loader_Inside);

    m_rcBar.left = GetSystemMetrics(SM_CXICON);
    m_rcBar.top = GetSystemMetrics(SM_CYSCREEN)/2 - GetSystemMetrics(SM_CXICON);
    m_rcBar.right = GetSystemMetrics(SM_CXSCREEN) - GetSystemMetrics(SM_CXICON);
    m_rcBar.bottom = m_rcBar.top + 50;

    //that should do it
    m_rcProgress = m_rcBar;
    m_rcProgress.left += 12;
    m_rcProgress.top += 13;
    m_rcProgress.bottom = m_rcProgress.top + 26;
    m_rcProgress.right -= 12;


    m_imgLoad.SetSize(WIDTH(m_rcBar), HEIGHT(m_rcBar));

	PostMessage(m_hWnd, WM_LOAD_Sounds, 0,0);

	::SetCursor(hCursor);

	return TRUE;
}

BOOL CDlgSplashScreen::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(m_gdiMem.GetDC() == NULL)
        m_gdiMem.Create(hDC, rcClient, FALSE, TRUE);

	if(m_gdiImage.GetDC() == NULL)
		FadeInScreen(hDC, rcClient);

    BitBlt(m_gdiMem.GetDC(),
        0,0,
        WIDTH(rcClient), HEIGHT(rcClient),
        m_gdiImage.GetDC(),
        0,0,
        SRCCOPY);

    m_imgLoad.DrawImage(m_gdiMem, m_rcBar.left, m_rcBar.top);

	if(m_iPercent > 0 && m_iPercent <= 100)
    {
        RECT rcTemp = m_rcProgress;
        rcTemp.right = rcTemp.left + m_iPercent*(WIDTH(m_rcProgress))/100;
        m_imgLoadInside.SetSize(WIDTH(rcTemp), HEIGHT(rcTemp));
        m_imgLoadInside.DrawImage(m_gdiMem, rcTemp.left, rcTemp.top);
    }
	
    BitBlt(hDC,
        0,0,
        WIDTH(rcClient), HEIGHT(rcClient),
        m_gdiMem.GetDC(),
        0,0,
        SRCCOPY);


	return TRUE;
}

BOOL CDlgSplashScreen::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(wParam == IDT_Timeout)
		EndDialog(m_hWnd, 0);
	return TRUE;
}

BOOL CDlgSplashScreen::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgSplashScreen::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgSplashScreen::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//EndDialog(m_hWnd, 0);
	return UNHANDLED;
}

BOOL CDlgSplashScreen::OnLButtonDown(HWND hWnd, POINT& pt)
{
	//EndDialog(m_hWnd, 0);
	return UNHANDLED;
}

BOOL CDlgSplashScreen::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgSplashScreen::FadeInScreen(HDC hDC, RECT rcClient)
{

	int iWidth	= GetSystemMetrics(SM_CXSCREEN);
	int iHeight = GetSystemMetrics(SM_CYSCREEN);

	m_gdiImage.Create(hDC, iWidth, iHeight, TRUE, TRUE);

	return TRUE;
}

BOOL CDlgSplashScreen::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uiMessage)
	{
	case WM_GET_Percent:
		return (BOOL)m_iPercent;
		break;
	case WM_SET_Percent:
		m_iPercent = (int)wParam;
		if(m_iPercent < 0)
			m_iPercent = 0;
		else if(m_iPercent > 100)
			m_iPercent = 100;
		break;
	case WM_LOAD_Sounds:
		//m_oStr->StringCopy(m_szLoading, _T("Loading Sounds..."));
		PostMessage(hWnd, WM_LOAD_Fonts, 0,0);
		break;
	case WM_LOAD_Fonts:
		//m_oStr->StringCopy(m_szLoading, _T("Loading Fonts..."));
		PostMessage(hWnd, WM_LOAD_Background, 0,0);
		break;
	case WM_LOAD_Background:
		//m_oStr->StringCopy(m_szLoading, _T("Loading Background..."));
		PostMessage(hWnd, WM_lOAD_Images, 0,0);
		break;
	case WM_lOAD_Images:
		//m_oStr->StringCopy(m_szLoading, _T("Loading Images..."));
		break;
	default:
		return UNHANDLED;
	}
	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
	return TRUE;
}



