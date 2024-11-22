#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssHighScores.h"

#ifdef WIN32_PLATFORM_WFSP
#define NUM_HIGHSCORES      6
#else
#define NUM_HIGHSCORES      10
#endif

struct TypeHighScore
{
    int			iBank;
};

class CDlgHighScores:public CIssWnd
{
public:
	CDlgHighScores(void);
	~CDlgHighScores(void);
	void	Init(CIssHighScores* oHighScores){m_oHighScores = oHighScores;};

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
	CIssHighScores* m_oHighScores;
	BOOL			m_bFirstTime;
	CIssGDIEx		m_gdiMem;
	CIssGDIEx		m_gdiBackground;
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
	RECT			m_rcBack;					// location of the back button
	
};
