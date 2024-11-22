#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"

class CDlgMsgBox:public CIssWnd
{
public:
	CDlgMsgBox(void);
	~CDlgMsgBox(void);

    int     PopupMessage(UINT uiMessage, HWND hWndParent, HINSTANCE hInst, UINT uiFlag = MB_OK);
    int     PopupMessage(TCHAR* szMessage, HWND hWndParent, HINSTANCE hInst, UINT uiFlag = MB_OK);
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
	CIssGDIEx		m_gdiMem;
	CIssGDIEx		m_gdiBackground;
	
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
	
	RECT			m_rcYes;
	RECT			m_rcNo;
	RECT			m_rcText;
	
    TCHAR*          m_szText;
	BOOL			m_bFirstTime;
    BOOL            m_bYesNo;                   // are we doing yes/no or just OK
};
