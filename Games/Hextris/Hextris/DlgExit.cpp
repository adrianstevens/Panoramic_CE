#include "DlgExit.h"
#include "resource.h"
#include "IssGDIEffects.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssBeveledEdge.h"
#include "IssRect.h"


CDlgExit::CDlgExit(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
{
}

CDlgExit::~CDlgExit(void)
{
	CIssGDI::DeleteFont(m_hFontText);
	CIssGDI::DeleteFont(m_hFontSelected);
}

BOOL CDlgExit::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgExit::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
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

BOOL CDlgExit::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgExit::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
	int iBtnWidth		= GetSystemMetrics(SM_CXSCREEN)/3;
#ifdef WIN32_PLATFORM_WFSP
    int iBtnHeight	= GetSystemMetrics(SM_CXICON)*6/5;
#else
    int iBtnHeight	= GetSystemMetrics(SM_CXICON)*3/2;
#endif

	int iTextHeight		= GetSystemMetrics(SM_CXICON)*3/6;
	int iTotalHeight	= 0;

	iTotalHeight		= iBtnHeight /*for buttons */ + 3*iTextHeight /* 4 Text Height */;
	int iYpos			= (rcClient.bottom - iTotalHeight)/2;

	m_rcText			= rcClient;
	m_rcText.top		= iYpos;
	m_rcText.bottom		= iYpos + 3*iTextHeight;

	iYpos				= m_rcText.bottom;
	m_rcYes.left		= rcClient.right/2 - iBtnWidth + iShadowSpace;
	m_rcYes.right		= m_rcYes.left + iBtnWidth;
	m_rcYes.top			= iYpos;
	m_rcYes.bottom		= m_rcYes.top + iBtnHeight;

	m_rcNo				= m_rcYes;
	m_rcNo.left			= rcClient.right/2 - iShadowSpace;
	m_rcNo.right		= m_rcNo.left + iBtnWidth;

	return TRUE;
}

BOOL CDlgExit::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case VK_ESCAPE:
	case VK_TSOFT1:
		SafeCloseWindow(IDYES);
		break;
	case VK_TSOFT2:
		SafeCloseWindow(IDNO);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;

}

BOOL CDlgExit::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(PtInRect(&m_rcYes, pt))
		SafeCloseWindow(IDYES);
	else if(PtInRect(&m_rcNo, pt))
		SafeCloseWindow(IDNO);

	return TRUE;
}

BOOL CDlgExit::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
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



BOOL CDlgExit::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgExit::FadeInScreen(HDC hDC, RECT rcClient)
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

void CDlgExit::InitBackground()
{
	if(!m_gdiBackground.GetDC())
	{
		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		HDC dc = GetDC(m_hWnd);
		m_gdiBackground.Create(dc, rcClient, TRUE, TRUE);
		ReleaseDC(m_hWnd, dc);

		// darken it up a bit
		CIssGDI::AlphaFillRect(m_gdiBackground, rcClient, RGB(0,0,0), 192);

		// create the Yes button
		CIssBeveledEdge btnItem;
		btnItem.Initialize(m_hWnd, m_hInst, 
			IDR_MN_TL, IDR_MN_T, IDR_MN_TR, IDR_MN_R, IDR_MN_BR, IDR_MN_B, IDR_MN_BL, IDR_MN_L, IDR_MN_M);
		btnItem.InitializeAlpha(IDR_MNA_TL, IDR_MNA_T, IDR_MNA_TR, IDR_MNA_R, IDR_MNA_BR, IDR_MNA_B, IDR_MNA_BL, IDR_MNA_L, IDR_MNA_M);
		btnItem.SetLocation(m_rcYes);
		btnItem.Draw(m_gdiBackground, 255);
		TCHAR szText[STRING_MAX];
		m_oStr->StringCopy(szText, IDS_MENU_Yes, STRING_MAX, m_hInst);
		CIssRect rcZero;
		rcZero.Set(m_rcYes);
		rcZero.Translate(1,1);
		CIssGDI::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(100,100,100));
		rcZero.Translate(-1,-1);
		CIssGDI::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255));

		// the no button
		btnItem.SetLocation(m_rcNo);
		btnItem.Draw(m_gdiBackground, 255);
		m_oStr->StringCopy(szText, IDS_MENU_No, STRING_MAX, m_hInst);
		rcZero.Set(m_rcNo);
		rcZero.Translate(1,1);
		CIssGDI::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(100,100,100));
		rcZero.Translate(-1,-1);
		CIssGDI::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255));
	}
}

void CDlgExit::DrawBackground(RECT rcLocation)
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

void CDlgExit::DrawText()
{
	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, IDS_MSG_Exit, STRING_MAX, m_hInst);
	
	RECT rc = m_rcText;
	rc.left ++;
	rc.top ++;
	CIssGDI::DrawText(m_gdiMem.GetDC(), szText, rc, DT_CENTER|DT_WORDBREAK, m_hFontText, RGB(0,0,0));
	rc.left --;
	rc.top --;
	CIssGDI::DrawText(m_gdiMem.GetDC(), szText, rc, DT_CENTER|DT_WORDBREAK, m_hFontText, RGB(255,255,255));
	
}




