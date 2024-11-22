#pragma once

#include "IssWnd.h"

struct TypeStatic
{
	HWND hWndFrom;
	HDC	 hDC;
	RECT rcClient;
};

#define WM_DRAWSTATIC WM_USER + 123

class CWndStatic:public CIssWnd
{
public:
	CWndStatic(void);
	~CWndStatic(void);

	BOOL	Create(HWND hWndParent, HINSTANCE hInst, UINT uiFlags = 0);
	BOOL	MoveWindow(RECT& rc);
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnGetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLostFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	int		GetWidth(){return right-left;};
	int		GetHeight(){return bottom-top;};
	BOOL	SetText(TCHAR* szText);
	BOOL	SetText(UINT uiText);
	BOOL	SetFont(HFONT hFont);
	static void	DrawStaticText(TypeStatic* sStatic, TCHAR* szText, UINT uiText /*= 0*/, HINSTANCE hInst /*= NULL*/, HFONT hFont /*= NULL*/);

	BOOL	IsTextToLong(TCHAR* szText, HFONT hFont, HDC hDC);

public:	// variables
	int	left;
	int right;
	int top;
	int bottom;

private:
	WNDPROC					m_lpfnStatic;
	TCHAR*					m_szText;
	HFONT					m_hFont;
};
