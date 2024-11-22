#pragma once

#include "IssWnd.h"
#include "IssGDI.h"
#include "IssImgFont.h"
#include "IssHighScores.h"
#include "IssHardwareKeys.h"


class CDlgGameOver:public CIssWnd
{
public:
	CDlgGameOver(void);
	~CDlgGameOver(void);
	void	Init(CIssImgFont* imgFont, int iScore, CIssHighScores* oHiScore, CIssHardwareKeys* oKeys){m_imgFont = imgFont; m_iScore = iScore; m_oHiScore = oHiScore; m_oKeys = oKeys;};
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
	CIssHighScores*	m_oHiScore;
	CIssImgFont*	m_imgFont;
	CIssHardwareKeys* m_oKeys;
	CIssGDI			m_gdiMem;
	CIssGDI			m_gdiBackground;
	
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
	
	RECT			m_rcYes;
	RECT			m_rcNo;
	RECT			m_rcGameOver;
	RECT			m_rcPlayAgain;
	RECT			m_rcHighScore;

	int				m_iScore;
	
	BOOL			m_bFirstTime;
};
