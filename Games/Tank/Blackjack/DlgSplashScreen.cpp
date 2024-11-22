#include "DlgSplashScreen.h"
#include "resource.h"
#include "IssGDIFx.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"

#define IDT_Timeout		100

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

	HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

	/*LoadImage(m_gdiSplashAlpha, IDR_LOGO_Alpha, m_hWnd, m_hInst);
	LoadImage(m_gdiSplash, IDR_LOGO, m_hWnd, m_hInst);

	m_gdiSplash.ConvertToDIB();
	m_gdiSplashAlpha.ConvertToDIB();*/
    m_gdiSplash.LoadImage(IDB_PNG_Pano, hWnd, m_hInst, TRUE);

	//m_oStr->Empty(m_szLoading);

	PostMessage(m_hWnd, WM_LOAD_Sounds, 0,0);

	::SetCursor(hCursor);

	return TRUE;
}

BOOL CDlgSplashScreen::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiImage.GetDC() == NULL)
		FadeInScreen(hDC, rcClient);

	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, TRUE);
	
	BitBlt(m_gdiMem.GetDC(),
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiImage.GetDC(),
		0,0,
		SRCCOPY);

	if(m_iPercent >= 0 && m_iPercent <= 100)
	{
		RECT rc			= m_rcPercent;
		rc.right		= rc.left + WIDTH(rc)*m_iPercent/100;
        int iCurve		= GetSystemMetrics(SM_CXICON)/4;
//		GradientRoundRectangle(m_gdiMem, rc, RGB(0,0,0), iCurve, iCurve, RGB(194, 147, 206), RGB(51,26, 57));

        GradientRoundRectangle(m_gdiMem, rc, RGB(0,0,0), iCurve, iCurve, RGB(147, 194, 206), RGB(26,51, 57));

		/*TCHAR szPercent[STRING_SMALL];
		m_oStr->IntToString(szPercent, m_iPercent);
		m_oStr->Concatenate(szPercent, _T("%"));

		rc.left ++;
		rc.top ++;
		CIssGDI::DrawText(m_gdiMem.GetDC(), szPercent, m_rcPercent, DT_CENTER|DT_VCENTER, NULL, RGB(100,100,100));
		rc.left --;
		rc.top --;
		CIssGDI::DrawText(m_gdiMem.GetDC(), szPercent, m_rcPercent, DT_CENTER|DT_VCENTER, NULL, RGB(255,255,255));*/
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
	if(LOWORD(wParam) == WA_ACTIVE) 
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

	Draw(m_gdiImage, 
		  (iWidth - m_gdiSplash.GetWidth())/2, (iHeight - m_gdiSplash.GetHeight())/2,
		  m_gdiSplash.GetWidth(), m_gdiSplash.GetHeight(),
		  m_gdiSplash,
		  0,0);

	int iCurve		= GetSystemMetrics(SM_CXICON)/4;
	int iIndent		= m_gdiSplash.GetWidth()/10;
	int iBarHeight	= m_gdiSplash.GetHeight()/20;

	m_rcPercent.left	= (iWidth - m_gdiSplash.GetWidth())/2 + iIndent;
	m_rcPercent.right	= m_rcPercent.left + m_gdiSplash.GetWidth() - 2*iIndent;
	m_rcPercent.top		= iHeight/2 + m_gdiSplash.GetHeight()/4 - iBarHeight/2;
	m_rcPercent.bottom	= m_rcPercent.top + iBarHeight;
	GradientRoundRectangle(m_gdiImage, m_rcPercent, RGB(0,0,0), iCurve, iCurve, RGB(0,0,0), RGB(100,100,100));

	
	FadeIn(hDC, m_gdiImage);


	//SetTimer(m_hWnd, IDT_Timeout, 700, NULL);

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



