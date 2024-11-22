#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "WndToday.h"

class CDlgToday:public CIssWnd
{
public:
	CDlgToday(void);
	~CDlgToday(void);

	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL 			OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL 			OnMouseMove(HWND hWnd, POINT& pt);
	BOOL 			OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);	
	BOOL			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);


private:
	CIssGDIEx		m_gdiBackground;
	CWndToday		m_wndToday;			// our actual today screen plugin
	int				m_iHeight;			// Height of the plugin
	BOOL			m_bHasFocus;		// plugin has focus

    BOOL            m_bDestroyGDI;
};
