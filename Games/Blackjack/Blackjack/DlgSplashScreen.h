#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssImageSliced.h"

#define WM_GET_Percent		WM_USER + 98
#define WM_SET_Percent		WM_USER + 99
#define WM_LOAD_Sounds		WM_USER + 100
#define WM_LOAD_Fonts		WM_USER + 101
#define WM_LOAD_Background	WM_USER + 102
#define WM_lOAD_Images	    WM_USER + 103

class CDlgSplashScreen:public CIssWnd
{
public:
	CDlgSplashScreen(void);
	~CDlgSplashScreen(void);

	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

private:	// functions
	BOOL	ShouldAbort();
	BOOL	FadeInScreen(HDC hDC, RECT rcClient);

private:	// variables
	CIssGDIEx	m_gdiMem;
	CIssGDIEx	m_gdiImage;		// screen
	
    CIssImageSliced   m_imgLoad;
    CIssImageSliced   m_imgLoadInside;

	//TCHAR	m_szLoading[STRING_LARGE];
	int		m_iPercent;
	RECT	m_rcPercent;

    RECT    m_rcBar;
    RECT    m_rcProgress;
};
