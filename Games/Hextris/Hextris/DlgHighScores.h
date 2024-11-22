#pragma once

#include "IssWnd.h"
#include "IssGDI.h"
#include "HexGame.h"
#include "IssHardwareKeys.h"

class CDlgHighScores:public CIssWnd
{
public:
	CDlgHighScores(void);
	~CDlgHighScores(void);
	void	Init(CHexGame* oGame, CIssHardwareKeys* oKeys){m_oGame = oGame; m_oKeys = oKeys;};

protected:
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:	// functions
	BOOL	FadeInScreen(HDC hDC, RECT rcClient);
	void	InitBackground();
	void	DrawBackground(RECT rcLocation);
	void	DrawText();
	void	MyDrawText(UINT uiText, RECT rcLocation, HFONT hFont, UINT uiFormat = DT_VCENTER|DT_CENTER, COLORREF crText = RGB(255,255,255), COLORREF crShadow = RGB(0,0,0));
	void	MyDrawText(TCHAR* szText, RECT rcLocation, HFONT hFont, UINT uiFormat = DT_VCENTER|DT_CENTER, COLORREF crText = RGB(255,255,255), COLORREF crShadow = RGB(0,0,0));

private:	// variables
	CIssHardwareKeys* m_oKeys;
	CHexGame*		m_oGame;
	BOOL			m_bFirstTime;
	CIssGDI			m_gdiMem;
	CIssGDI			m_gdiBackground;
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
	RECT			m_rcBack;					// location of the back button
	
};
