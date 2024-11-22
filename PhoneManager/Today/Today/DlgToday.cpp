#include "StdAfx.h"
#include "DlgToday.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "resource.h"
#include "IssCommon.h"
#include "TodayCmn.h"
#ifdef DEBUG
#include "DlgOptions.h"
#endif
#include "DlgChoosePrograms.h"

#define OFFSET_Initial	10


CDlgToday::CDlgToday(void)
:m_iHeight(0)
, m_bHasFocus(FALSE)
, m_bDestroyGDI(FALSE)
{
}

CDlgToday::~CDlgToday(void)
{
}

BOOL CDlgToday::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;
	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= m_hWnd;
	mbi.nToolBarId	= IDR_MENU_Quit; 
	mbi.hInstRes	= m_hInst;
	SHCreateMenuBar(&mbi);

	TODAYLISTITEM tli;
	ZeroMemory(&tli, sizeof(TODAYLISTITEM));
	//szName, tlit, dwOrder, fEnabled, fOptions and grfFlags 
	m_oStr->StringCopy(tli.szName, _T(""));
	tli.dwOrder		= 0;
	tli.fEnabled	= TRUE;
	tli.grfFlags	= 0;

	m_wndToday.CreateHome(m_hWnd, m_hInst);

	SendMessage(m_wndToday.GetWnd(), WM_TODAYCUSTOM_CLEARCACHE, (WPARAM)&tli, 0);
	SendMessage(m_wndToday.GetWnd(), WM_TODAYCUSTOM_QUERYREFRESHCACHE, (WPARAM)&tli, 0);

	m_iHeight = tli.cyp;
	MoveWindow(m_wndToday.GetWnd(), 0, OFFSET_Initial, GetSystemMetrics(SM_CXSCREEN), m_iHeight, TRUE);
#ifndef DEBUG
	::ShowWindow(m_wndToday.GetWnd(), SW_SHOW);
#endif

	return TRUE;
}

BOOL CDlgToday::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_bDestroyGDI == TRUE || m_gdiBackground.GetDC() == NULL)
    {
        m_bDestroyGDI = FALSE;
        if(m_gdiBackground.GetWidth() != GetSystemMetrics(SM_CXSCREEN) ||
            m_gdiBackground.GetHeight() != GetSystemMetrics(SM_CYSCREEN))
        {
            m_gdiBackground.Destroy();
            m_gdiBackground.LoadImage(IDR_PNG_Background, m_hWnd, m_hInst, TRUE);
        }
	}

    FillRect(hDC, rcClient, 0);
	BitBlt(hDC,
		0,0,
		m_gdiBackground.GetWidth(), m_gdiBackground.GetHeight(),
		m_gdiBackground.GetDC(),
		0,0,
		SRCCOPY);

	RECT rc;
	rc.left		= -1;
	rc.right	= rcClient.right;
	rc.top		= OFFSET_Initial - 1;
	rc.bottom	= OFFSET_Initial + m_iHeight + 1;
	FrameRect(hDC, rc, RGB(255,255,255));

#ifdef DEBUG
	static BOOL s_bFirstPaint = FALSE;
	if(!s_bFirstPaint)
	{
		::ShowWindow(m_wndToday.GetWnd(), SW_SHOW);
		s_bFirstPaint = TRUE;
		return TRUE;
	}
#endif

	
	return TRUE;
}

BOOL CDlgToday::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_bDestroyGDI = TRUE;
	MoveWindow(m_wndToday.GetWnd(), 0, OFFSET_Initial, GetSystemMetrics(SM_CXSCREEN), m_iHeight, TRUE);
	m_wndToday.RefreshRegistry();
	return TRUE;
}

BOOL CDlgToday::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE) 
	{
		::SetWindowText(GetWnd(), _T("Today Tester"));
	}

	return UNHANDLED;
}

BOOL CDlgToday::OnLButtonDown(HWND hWnd, POINT& pt)
{
	SendMessage(m_wndToday.GetWnd(), WM_TODAYCUSTOM_LOSTSELECTION, 0, 0);
	m_bHasFocus = FALSE;

	return UNHANDLED;
}

BOOL CDlgToday::OnLButtonUp(HWND hWnd, POINT& pt)
{
	return UNHANDLED;
}

BOOL CDlgToday::OnMouseMove(HWND hWnd, POINT& pt)
{
	return UNHANDLED;
}

BOOL CDlgToday::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
	SendMessage(m_wndToday.GetWnd(), WM_TODAYCUSTOM_USERNAVIGATION, wParam, lParam);

#endif // DEBUG
	return UNHANDLED;
}

BOOL CDlgToday::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_Quit:
	case IDOK:
		PostQuitMessage(0);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgToday::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case _T('a'):
        {
            CDlgChoosePrograms dlgChoose;

        }
        break;
    case _T('d'):
        m_wndToday.SetDefaultLayout();
        // fall through
	case _T('s'):
		{
			TODAYLISTITEM tli;
			ZeroMemory(&tli, sizeof(TODAYLISTITEM));
			//szName, tlit, dwOrder, fEnabled, fOptions and grfFlags 
			m_oStr->StringCopy(tli.szName, _T(""));
			tli.dwOrder		= 0;
			tli.fEnabled	= TRUE;
			tli.grfFlags	= 0;
			SendMessage(m_wndToday.GetWnd(), WM_TODAYCUSTOM_QUERYREFRESHCACHE, (WPARAM)&tli, 0);

			m_iHeight = tli.cyp;
			MoveWindow(m_wndToday.GetWnd(), 0, OFFSET_Initial, GetSystemMetrics(SM_CXSCREEN), m_iHeight, TRUE);
		}
		break;
   
	}

	return UNHANDLED;
}

BOOL CDlgToday::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == TODAYM_GETCOLOR)
	{
		switch(wParam)
		{
		case TODAYCOLOR_TEXT:
			{
				if(m_bHasFocus)
					return 	(COLORREF)0x00ffff00;
				else
					return (COLORREF)0x00ffff;
			}
		case TODAYCOLOR_HIGHLIGHTEDTEXT:
			{
				if(m_bHasFocus)
					return (COLORREF)0x00a0ff00;
				else
					return (COLORREF)0x0000ffa0;
			}
		case TODAYCOLOR_HIGHLIGHT:
			{
				if(m_bHasFocus)
					return (COLORREF)0x000044aa;
				else
					return (COLORREF)0x00aaaa00;
			}
		}

	}	
	else if(uiMessage == TODAYM_TOOKSELECTION)
	{
		m_bHasFocus = TRUE;

		SendMessage(m_wndToday.GetWnd(), WM_TODAYCUSTOM_RECEIVEDSELECTION, 0, 0);
	}
    else if(uiMessage == TODAYM_DRAWWATERMARK)
    {
        TODAYDRAWWATERMARKINFO* lpDw = (TODAYDRAWWATERMARKINFO*)lParam;
        if(!lpDw || !lpDw->hdc || !lpDw->hwnd)
            return TRUE;

        RECT rc;
        POINT pt;
        GetWindowRect(lpDw->hwnd, &rc);
        pt.x        = lpDw->rc.left;
        pt.y        = rc.top + lpDw->rc.top;
        ScreenToClient(m_hWnd, &pt);

        BitBlt(lpDw->hdc,
               lpDw->rc.left, lpDw->rc.top,
               WIDTH(lpDw->rc), HEIGHT(lpDw->rc),
               m_gdiBackground,
               pt.x, pt.y,
               SRCCOPY);        

    }
	

	return UNHANDLED;
}

