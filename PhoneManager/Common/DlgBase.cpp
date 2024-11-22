#include "StdAfx.h"
#include "DlgBase.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"
#include "resource.h"
#include "IssCommon.h"
#include "IssDebug.h"
#include "ContactsGuiDefines.h"


#define HEIGHT_Text                     (GetSystemMetrics(SM_CXICON)*4/9)

CDlgBase::CDlgBase(void)
:m_gdiMem(NULL)
,m_guiBackground(NULL)
,m_bDrawBottomBar(TRUE)
,m_bDrawTopBar(TRUE)
,m_bAnimate(FALSE)
,m_bFullScreen(TRUE)
{
    m_hFontLabel = CIssGDIEx::CreateFont(HEIGHT_Text, FW_BOLD, TRUE);
}

CDlgBase::~CDlgBase(void)
{
    CIssGDIEx::DeleteFont(m_hFontLabel);
}

BOOL CDlgBase::Init( CIssGDIEx* gdiMem, 
                     CGuiBackground* guiBackground,
                     BOOL bDrawTopBar,
                     BOOL bDrawBottomBar)
{
    if(!gdiMem || !guiBackground)
        return FALSE;

    m_gdiMem            = gdiMem;
    m_guiBackground     = guiBackground;
    m_bDrawBottomBar    = bDrawBottomBar;
    m_bDrawTopBar       = bDrawTopBar;
    return TRUE;
}

BOOL CDlgBase::Launch(HWND hWndParent, HINSTANCE hInst, BOOL bAnimate)
{
    if(!m_gdiMem || !m_guiBackground || !hInst)
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
        gdiTemp.Create(m_gdiMem->GetDC(), rcClient, TRUE, TRUE, FALSE);
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
	return TRUE;
}

BOOL CDlgBase::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(!m_gdiMem || !m_guiBackground)
        return FALSE;

    if(m_gdiMem->GetDC() == NULL || m_gdiMem->GetWidth() != WIDTH(rcClient) || m_gdiMem->GetHeight() != HEIGHT(rcClient))
        m_gdiMem->Create(hDC, rcClient, FALSE, TRUE, FALSE);

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    m_guiBackground->DrawBackground(*m_gdiMem, rcClip, rcClient, m_bDrawTopBar?HEIGHT(m_rcTitle)-m_guiBackground->GetSliceHeight():0);

    if(m_bDrawTopBar)
    {
        if(IsRectInRect(rcClip, m_rcTitle))
        {
            m_guiBackground->DrawTopBar(*m_gdiMem, rcClip, rcClient);
            DrawTopBar(*m_gdiMem, rcClient, rcClip);
        }
    }
    else if(IsRectInRect(rcClip, m_guiBackground->GetTopSliceRect()))
    {
        m_guiBackground->DrawTopSlice(*m_gdiMem, rcClip, rcClient);
        DrawTopBar(*m_gdiMem, rcClient, rcClip);
    }

    if(m_bDrawBottomBar && IsRectInRect(rcClip, m_rcBottomBar))
    {
        m_guiBackground->DrawBottomBar(*m_gdiMem, rcClip, rcClient);
        DrawBottomBar(*m_gdiMem, rcClient, rcClip);
    }

    if(IsRectInRect(rcClip, m_rcArea))
        if(!OnDraw(*m_gdiMem, rcClient, rcClip))    // we may not actually want to draw to screen just yet
            return TRUE;

    if(m_bAnimate)
    {
        m_bAnimate = FALSE;
        SlideLeft(hDC, *m_gdiMem);
        return TRUE;
    }

    BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem->GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);
	
	return TRUE;
}

BOOL CDlgBase::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    RECT rc;
    GetClientRect(hWnd, &rc);

    if(m_gdiMem && GetSystemMetrics(SM_CXSCREEN) != m_gdiMem->GetWidth())
        m_gdiMem->Destroy();

    if(!m_guiBackground)
        return TRUE;

    SetRect(&m_rcTitle, 0, 0, rc.right, BAR_TOP_Height);
    SetRect(&m_rcBottomBar, 0, rc.bottom - BAR_BOTTOM_Height, rc.right, rc.bottom);

    SetRect(&m_rcArea, 
            m_guiBackground->GetSideSliceWidth(), m_rcTitle.bottom+1, rc.right - m_guiBackground->GetSideSliceWidth(), m_rcBottomBar.top-1);

    return TRUE;
}

BOOL CDlgBase::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) 
	{
        // go full screen
//        SetForegroundWindow(hWnd);

        DWORD dwState;
        if(m_bFullScreen)
            dwState = SHFS_HIDETASKBAR|SHFS_HIDESTARTICON|SHFS_HIDESIPBUTTON;
        else
            dwState = SHFS_SHOWTASKBAR|SHFS_SHOWSTARTICON|SHFS_HIDESIPBUTTON;

		ShowWindow(hWnd, SW_SHOW);
		SHFullScreen(hWnd, dwState); 

        RECT rc, rcTaskBar; 

        HWND hWndTaskbar = NULL;

        hWndTaskbar= FindWindow(_T("HHTaskBar"), NULL);

        int iMenu = 0;
        if(hWndTaskbar)
        {
            GetWindowRect(hWndTaskbar,&rcTaskBar);
            iMenu = HEIGHT(rcTaskBar);
        }

        if(iMenu == 0)
        {
            switch(GetSystemMetrics(SM_CXICON))
            {
            case 32:
                iMenu = 26;
                break;
            case 44:
                iMenu = 35;
                break;
            case 64:
                iMenu = 52;
                break;
            default:
                iMenu = GetSystemMetrics(SM_CXICON)*26/32;
                break;
            }
        }

        int iYBottom = GetSystemMetrics(SM_CYSCREEN);

        if(m_bFullScreen)
            SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), iYBottom); 
        else
            SetRect(&rc, 0, iMenu, GetSystemMetrics(SM_CXSCREEN), iYBottom); 
        MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
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
    if(m_bDrawBottomBar && PtInRect(&m_rcBottomBar, pt))
    {
        if(pt.x < GetSystemMetrics(SM_CXSCREEN)/2)
            OnMenuLeft();
        else
            OnMenuRight();
        return TRUE;
    }
    return UNHANDLED;//don't change this
}



