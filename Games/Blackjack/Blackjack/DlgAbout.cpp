#include "DlgAbout.h"
#include "resource.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssImageSliced.h"
#include "IssRect.h"
#include "IssLocalisation.h"

CDlgAbout::CDlgAbout(void)
:m_hFontText(NULL)
{
}

CDlgAbout::~CDlgAbout(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
}

BOOL CDlgAbout::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);
#endif

	m_bFirstTime = TRUE;


	return TRUE;
}

BOOL CDlgAbout::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
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

BOOL CDlgAbout::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgAbout::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_gdiBackground.Destroy();
	m_gdiMem.Destroy();

	CIssGDIEx::DeleteFont(m_hFontText);

#ifdef WIN32_PLATFORM_WFSP
    m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_BOLD, TRUE);
#else
	m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/6, FW_BOLD, TRUE);
#endif


	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	int iShadowSpace	= 2*HEIGHT(rcClient)/100;
    int iIndent = GetSystemMetrics(SM_CXICON)/4;
	m_rcBack.left	= iIndent;
	m_rcBack.right	= m_rcBack.left + GetSystemMetrics(SM_CXSCREEN)/4;
	m_rcBack.top	= rcClient.bottom - GetSystemMetrics(SM_CYICON) - iIndent;
	m_rcBack.bottom	= rcClient.bottom - iIndent;

	return TRUE;
}

BOOL CDlgAbout::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_TSOFT1:
    case VK_ESCAPE:
        SafeCloseWindow(0);
        break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgAbout::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(PtInRect(&m_rcBack, pt))
		SafeCloseWindow(0);
	return TRUE;
}

BOOL CDlgAbout::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
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



BOOL CDlgAbout::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgAbout::FadeInScreen(HDC hDC, RECT rcClient)
{

	int iWidth	= GetSystemMetrics(SM_CXSCREEN);
	int iHeight = GetSystemMetrics(SM_CYSCREEN);

	DrawBackground(rcClient);
	DrawText();

	FadeIn(hDC, m_gdiMem);


	// force the buttons to load
	//m_btnItems.Draw(m_gdiMem, 255, &m_ptItems[0]);

	//CIssGDIEffects::FadeIn(hDC, m_gdiMem, 900, fnAnimateIn, this);


	return TRUE;
}

void CDlgAbout::InitBackground()
{
	if(!m_gdiBackground.GetDC())
	{
		CIssGDIEx gdiTile, gdiLogo;
        gdiTile.LoadImage(IDB_PNG_Felt, m_hWnd, m_hInst);
        gdiLogo.LoadImage(IDB_PNG_Logo, m_hWnd, m_hInst);

		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		m_gdiBackground.Create(m_gdiMem.GetDC(), rcClient, FALSE, TRUE);

		// copy the background over
		TileBackGround(m_gdiBackground,
			rcClient,
			gdiTile,
			FALSE);

		m_iLogoHeight = gdiLogo.GetHeight()*5/4;

		Draw(m_gdiBackground,
			  (WIDTH(rcClient)-gdiLogo.GetWidth())/2,0,
			  gdiLogo.GetWidth(), gdiLogo.GetHeight(),
			  gdiLogo,
			  0,0);	

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
	}
}

void CDlgAbout::DrawBackground(RECT rcLocation)
{
	InitBackground();

	// copy the background over
	BitBlt(m_gdiMem.GetDC(),
		   rcLocation.left, rcLocation.top,
		   WIDTH(rcLocation), HEIGHT(rcLocation),
		   m_gdiBackground.GetDC(),
		   rcLocation.left, rcLocation.top,
		   SRCCOPY);
}

void CDlgAbout::DrawText()
{
	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, ID(IDS_ABOUT_1), STRING_MAX, m_hInst);

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	rc.top	+= m_iLogoHeight;

	rc.left ++;
	rc.top ++;
    ::DrawText(m_gdiMem.GetDC(), szText, rc, DT_CENTER, m_hFontText, RGB(0,0,0));
	rc.left --;
	rc.top --;
    ::DrawText(m_gdiMem.GetDC(), szText, rc, DT_CENTER, m_hFontText, RGB(255,255,255));
}



