#include "StdAfx.h"
#include "DlgBase.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"
#include "resource.h"
#include "IssCommon.h"
#include "IssDebug.h"
#include "ContactsGuiDefines.h"
#include "ObjGui.h"


extern CObjGui*	g_gui;

#define TOPMENU_Height (IsVGA()?64:34) 



CDlgBase::CDlgBase(void)
:m_bAnimate(FALSE)
{

}

CDlgBase::~CDlgBase(void)
{
}

BOOL CDlgBase::Launch(HWND hWndParent, HINSTANCE hInst, BOOL bAnimate)
{
	CIssGDIEx* gdi = g_gui->GetGDI();

    if(!hInst || !gdi)
        return FALSE;

    CIssGDIEx gdiTemp;

    if(bAnimate)
    {
        m_bAnimate = TRUE;

        // save the original image
        RECT rcClient;
        if(hWndParent == NULL)
            GetClientRect(GetDesktopWindow(), &rcClient);
        else
            GetClientRect(hWndParent, &rcClient);
        gdiTemp.Create(gdi->GetDC(), rcClient, TRUE, TRUE, FALSE);
    }

    BOOL bRet = DoModal(hWndParent, hInst, IDD_DLG_Basic);

    if(bAnimate && bRet == IDOK)
    {
        // now slide back out
        HDC hDC = GetDC(hWndParent);
        SlideRight(hDC, gdiTemp);
        ReleaseDC(hWndParent, hDC);
    }

    return bRet;
}

BOOL CDlgBase::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SetWindowText(m_hWnd, _T("Launchpad"));

	return TRUE;
}

BOOL CDlgBase::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    CIssGDIEx* gdi = g_gui->GetGDI();

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

	DrawBackground(*gdi, rcClient, rcClip);

	if(IsRectInRect(m_rcTitle, rcClip))
		DrawTopBar(*gdi, rcClient, rcClip);

	if(IsRectInRect(m_rcBottomBar, rcClip))
		DrawBottomBar(*gdi, rcClient, rcClip);

   if(IsRectInRect(rcClip, m_rcArea))
        if(!OnDraw(*gdi, rcClient, rcClip))    // we may not actually want to draw to screen just yet
            return TRUE;

    if(m_bAnimate)
    {
        m_bAnimate = FALSE;
        SlideLeft(hDC, *gdi);
        return TRUE;
    }

    g_gui->DrawScreen(rcClip, hDC);
	
	return TRUE;
}

BOOL CDlgBase::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    RECT rc;
    GetClientRect(hWnd, &rc);

	//if(WIDTH(m_rcArea) == GetSystemMetrics(SM_CXSCREEN))
	//	return TRUE;

    g_gui->OnSize(rc);

	m_gdiBackground.Destroy();

	m_rcTitle	= m_rcBottomBar = m_rcArea = rc;

	m_rcTitle.bottom	= m_rcTitle.top + BAR_TOP_Height;
	m_rcBottomBar.top	= m_rcBottomBar.bottom - BAR_BOTTOM_Height;

	m_rcArea.top		= m_rcTitle.bottom + 1;
	m_rcArea.bottom		= m_rcBottomBar.top - 1;

    return TRUE;
}

BOOL CDlgBase::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) 
	{
		g_gui->AdjustWindow(hWnd, FALSE);
	}
	return UNHANDLED;
}



BOOL CDlgBase::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case VK_TSOFT1:
        OnMenuLeft();
        break;
    case VK_TSOFT2:
        OnMenuRight();
        break;
    default:
        return UNHANDLED;
        break;
    }

    return TRUE;
}

BOOL CDlgBase::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(PtInRect(&m_rcBottomBar, pt))
    {
        if(pt.x < GetSystemMetrics(SM_CXSCREEN)/2)
            OnMenuLeft();
        else
            OnMenuRight();
        return TRUE;
    }
    return UNHANDLED;//don't change this
}

BOOL CDlgBase::DrawBackground(CIssGDIEx& gdiMem, RECT& rcClient, RECT& rcClip)
{
	if(m_gdiBackground.GetDC() == NULL)
	{
		int iWidth = WIDTH(rcClient);
		int iHeight = HEIGHT(rcClient);

		m_gdiBackground.Create(gdiMem, iWidth, iHeight, FALSE, TRUE);
		m_gdiBackground.InitAlpha(FALSE);

		// draw the background
		RECT rc = {0};
		rc.bottom   = iHeight;
		rc.right    = iWidth*7/10;
		GradientFillRect(m_gdiBackground, rc, RGB(102,102,102), RGB(0,0,0), FALSE);
		rc.left     = rc.right;
		rc.right    = iWidth;
		GradientFillRect(m_gdiBackground, rc, RGB(106,106,106), RGB(12,12,12), FALSE);

		// border around the background
		iHeight = HEIGHT(rcClient) - BAR_TOP_Height - SLICE_Height;
		CIssImageSliced imgBorder;
		imgBorder.Initialize(m_hWnd, m_hInst, IsVGA() ? IDR_DLG_Border_VGA : IDR_DLG_Border);
		imgBorder.SetSize(iWidth, iHeight);

		Draw(m_gdiBackground,
			0,BAR_TOP_Height - SLICE_Height,
			iWidth, iHeight,
			imgBorder.GetImage(),
			0, 0);

		// title bar
		iHeight= BAR_TOP_Height-SLICE_Height;
		rc = m_rcTitle;
		rc.bottom   = iHeight/3;
		GradientFillRect(m_gdiBackground, rc, RGB(237,245,248), RGB(212,218,220), FALSE);
		rc.top      = rc.bottom;
		rc.bottom   = iHeight;
		GradientFillRect(m_gdiBackground, rc, RGB(182,186,188), RGB(150,152,152), FALSE);

		// bottom bar
		rc = m_rcBottomBar;
		rc.bottom   = rc.top + BAR_BOTTOM_Height/3;
		GradientFillRect(m_gdiBackground, rc, COLOR_TOOLBAR_GRAD2_END, COLOR_TOOLBAR_GRAD2_ST, FALSE);
		rc.top      = rc.bottom;
		rc.bottom   = m_rcBottomBar.bottom;
		GradientFillRect(m_gdiBackground, rc, COLOR_TOOLBAR_GRAD1_END, COLOR_TOOLBAR_GRAD1_ST, FALSE);
		Line(m_gdiBackground, iWidth/2 - 1, m_rcBottomBar.top + BAR_BOTTOM_Height/8, iWidth/2 - 1, rc.bottom - BAR_BOTTOM_Height/8, RGB(65,65,65));
		Line(m_gdiBackground, iWidth/2, m_rcBottomBar.top + BAR_BOTTOM_Height/8, iWidth/2, rc.bottom - BAR_BOTTOM_Height/8, RGB(150,152,151)); 
	}

	BitBlt(gdiMem, rcClip.left, rcClip.top, WIDTH(rcClip), HEIGHT(rcClip), m_gdiBackground, rcClip.left, rcClip.top, SRCCOPY);

	return TRUE;
}



