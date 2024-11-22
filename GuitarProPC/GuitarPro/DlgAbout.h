#pragma once

#include "IssWnd.h"
#include "ObjGui.h"

class CDlgAbout:public CIssWnd
{
public:
	CDlgAbout(void);
	~CDlgAbout(void);

protected:
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL    OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	OnMove(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:	// functions
	BOOL	FadeInScreen(HDC hDC, RECT rcClient);
	void	DrawText(HDC hDC);
    void    DrawBackground(CIssGDIEx& gdi, RECT rcClip);

    BOOL    DrawMenuHilite(BOOL bLeft);

public:
	HWND			m_hWndParent;

private:	// variables


    BOOL			m_bFirstTime;
	
	int				m_iLogoHeight;
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size

    RECT            m_rcMenuLeft;
};
