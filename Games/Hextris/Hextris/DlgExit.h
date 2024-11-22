#pragma once

#include "IssWnd.h"
#include "IssGDI.h"
#include "IssHardwareKeys.h"

class CDlgExit:public CIssWnd
{
public:
	CDlgExit(void);
	~CDlgExit(void);
	void	Init(CIssHardwareKeys* oKeys){m_oKeys = oKeys;};
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
	CIssHardwareKeys* m_oKeys;
	CIssGDI			m_gdiMem;
	CIssGDI			m_gdiBackground;
	
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
	
	RECT			m_rcYes;
	RECT			m_rcNo;
	RECT			m_rcText;
	
	BOOL			m_bFirstTime;
};
