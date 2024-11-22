#include "DlgAbout.h"
#include "resource.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssImageSliced.h"
#include "IssRect.h"
#include "IssLocalisation.h"

extern CObjGui* g_gui;

CDlgAbout::CDlgAbout(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
{
    m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/6, FW_BOLD, TRUE);
    m_hFontSelected			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);
}

CDlgAbout::~CDlgAbout(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);
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
	if(m_bFirstTime)
	{
		FadeInScreen(hDC, rcClient);
		m_bFirstTime = FALSE;
	}

	//g_gui->DrawBackground(rcClient);
    DrawBackground(*g_gui->GetGDI(), rcClient);
	DrawText(g_gui->GetGDI()->GetDC());

    g_gui->DrawToScreen(rcClient, hDC);

	return TRUE;
}

BOOL CDlgAbout::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDMENU_Left)
    {
        SafeCloseWindow(0);
    }
    return UNHANDLED;
}

BOOL CDlgAbout::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    SetRect(&m_rcMenuLeft, rc.left, rc.bottom - MENU_HEIGHT, rc.left + WIDTH(rc)/2, rc.bottom);
	return TRUE;
}

BOOL CDlgAbout::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_TSOFT1:
    case VK_ESCAPE:
        DrawMenuHilite(TRUE);
        SafeCloseWindow(0);
        break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgAbout::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(PtInRect(&m_rcMenuLeft, pt))
        DrawMenuHilite(TRUE);

	return g_gui->OnLButtonDown(hWnd, pt);;
}

BOOL CDlgAbout::OnLButtonUp(HWND hWnd, POINT& pt)
{
    return g_gui->OnLButtonUp(hWnd, pt);;
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

	//g_gui->DrawBackground(rcClient);
    DrawBackground(*g_gui->GetGDI(), rcClient);
	DrawText(g_gui->GetGDI()->GetDC());

	FadeIn(hDC, *g_gui->GetGDI());

	return TRUE;
}

void CDlgAbout::DrawBackground(CIssGDIEx& gdi, RECT rcClip)
{   
    BitBlt(gdi.GetDC(), rcClip.left, rcClip.top, WIDTH(rcClip), HEIGHT(rcClip), g_gui->GetAltBack()->GetDC(), rcClip.left, rcClip.top, SRCCOPY);
}

void CDlgAbout::DrawText(HDC hDC)
{
	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, ID(IDS_MSG_About), STRING_MAX, m_hInst);

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	rc.top	+= GetSystemMetrics(SM_CXICON) * 2;

	rc.left ++;
	rc.top ++;
    ::DrawText(hDC, szText, rc, DT_CENTER, m_hFontText, RGB(0,0,0));
	rc.left --;
	rc.top --;
    ::DrawText(hDC, szText, rc, DT_CENTER, m_hFontText, RGB(255,255,255));

    //and the menu
    ::DrawText(hDC, m_oStr->GetText(ID(IDS_MENU_Back), m_hInst), m_rcMenuLeft, DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_MenuLarge), 0xFFFFFF);

    //and finally ... the divider
    int iSpacing = (WIDTH(rc))/2;
    int iIndent = (MENU_HEIGHT)/5;

    POINT ptS, ptE;

    ptS.x = iSpacing-1;
    ptS.y = rc.bottom - MENU_HEIGHT + iIndent;
    ptE.x = iSpacing-1; 
    ptE.y = rc.bottom - iIndent;

    Line(hDC, ptS.x, ptS.y, ptE.x, ptE.y, 0);
    ptE.x++;
    ptS.x++;
    Line(hDC, ptS.x, ptS.y, ptE.x, ptE.y, 0x666666);

}

BOOL CDlgAbout::DrawMenuHilite(BOOL bLeft)
{
    RECT rcTemp;
    if(bLeft)
        rcTemp = m_rcMenuLeft;
    else 
        return FALSE;

    POINT ptDraw;
    ptDraw.y = rcTemp.top + (HEIGHT(rcTemp) - g_gui->GetGlow()->GetHeight())/2;

    ptDraw.x = rcTemp.left + (GetSystemMetrics(SM_CXSCREEN)/2 - g_gui->GetGlow()->GetWidth())/2;

    ::Draw(*g_gui->GetGDI(), ptDraw.x, ptDraw.y, 
        g_gui->GetGlow()->GetWidth(), g_gui->GetGlow()->GetHeight(),
        *g_gui->GetGlow());

    HDC dc = GetDC(m_hWnd);
    BitBlt(dc, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp),
        g_gui->GetGDI()->GetDC(), rcTemp.left, rcTemp.top, SRCCOPY);
    ReleaseDC(m_hWnd, dc);

    return TRUE;
}


