#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"

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
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

private:	// functions
	BOOL	FadeInScreen(HDC hDC, RECT rcClient);
	void	InitBackground();
	void	DrawBackground(RECT rcLocation);
	void	DrawText();

private:	// variables
	BOOL			m_bFirstTime;
	CIssGDIEx		m_gdiMem;
	CIssGDIEx		m_gdiBackground;
	int				m_iLogoHeight;
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
	RECT			m_rcBack;
	
};
