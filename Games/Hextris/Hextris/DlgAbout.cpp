#include "DlgAbout.h"
#include "resource.h"
#include "IssGDIEffects.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssBeveledEdge.h"
#include "IssRect.h"


CDlgAbout::CDlgAbout(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
{
}

CDlgAbout::~CDlgAbout(void)
{
	CIssGDI::DeleteFont(m_hFontText);
	CIssGDI::DeleteFont(m_hFontSelected);
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

BOOL CDlgAbout::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgAbout::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_gdiBackground.Destroy();
	m_gdiMem.Destroy();

	CIssGDI::DeleteFont(m_hFontText);
	CIssGDI::DeleteFont(m_hFontSelected);
	m_hFontText				= CIssGDI::CreateFont(GetSystemMetrics(SM_CXICON)*3/6, FW_BOLD, TRUE);
	m_hFontSelected			= CIssGDI::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	int iShadowSpace	= 2*HEIGHT(rcClient)/100;

#ifdef WIN32_PLATFORM_WFSP
    int iBtnHeight	= GetSystemMetrics(SM_CXICON)*6/5;
#else
    int iBtnHeight	= GetSystemMetrics(SM_CXICON)*3/2;
#endif

	m_rcBack.left	= 0-iShadowSpace;
	m_rcBack.right	= m_rcBack.left + GetSystemMetrics(SM_CXSCREEN)/2;
	m_rcBack.top	= rcClient.bottom - iBtnHeight + iShadowSpace;
	m_rcBack.bottom	= m_rcBack.top + iBtnHeight;

	return TRUE;
}

BOOL CDlgAbout::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgAbout::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(PtInRect(&m_rcBack, pt))
		SafeCloseWindow(0);
	return TRUE;
}

BOOL CDlgAbout::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

	CIssGDIEffects::FadeIn(hDC, m_gdiMem);


	// force the buttons to load
	//m_btnItems.Draw(m_gdiMem, 255, &m_ptItems[0]);

	//CIssGDIEffects::FadeIn(hDC, m_gdiMem, 900, fnAnimateIn, this);


	return TRUE;
}

void CDlgAbout::InitBackground()
{
	if(!m_gdiBackground.GetDC())
	{
		CIssGDI gdiTile, gdiLogo, gdiLogoAlpha;
		LoadImage(gdiTile, IDR_TILE_BG, m_hWnd, m_hInst);
		LoadImage(gdiLogo, IDR_SLOGO, m_hWnd, m_hInst);
		LoadImage(gdiLogoAlpha, IDR_SLOGO_Alpha, m_hWnd, m_hInst);
		gdiLogo.ConvertToDIB();
		gdiLogoAlpha.ConvertToDIB();

		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		m_gdiBackground.Create(m_gdiMem.GetDC(), rcClient, FALSE, TRUE);

		// copy the background over
		CIssGDI::TileBackGround(m_gdiBackground.GetDC(),
			rcClient,
			gdiTile,
			FALSE);

		m_iLogoHeight = gdiLogo.GetHeight()*5/4;

		CIssGDI::MyAlphaImage(m_gdiBackground,
							  (WIDTH(rcClient)-gdiLogo.GetWidth())/2,0,
							  gdiLogo.GetWidth(), gdiLogo.GetHeight(),
							  gdiLogo,
							  gdiLogoAlpha, 
							  0,0);	

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

	m_oStr->StringCopy(szText, IDS_ABOUT_1, STRING_MAX, m_hInst);

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	rc.top	+= m_iLogoHeight;

	rc.left ++;
	rc.top ++;
	CIssGDI::DrawText(m_gdiMem.GetDC(), szText, rc, DT_CENTER, m_hFontText, RGB(0,0,0));
	rc.left --;
	rc.top --;
	CIssGDI::DrawText(m_gdiMem.GetDC(), szText, rc, DT_CENTER, m_hFontText, RGB(255,255,255));
}




