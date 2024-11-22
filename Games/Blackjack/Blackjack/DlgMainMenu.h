#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssMenuHandler.h"
#include "IssImageSliced.h"
#include "IssHighScores.h"
#include "IssBjGame.h"
#include "DlgOptions.h"

enum EnumBtns
{
	//BTN_Resume,
	BTN_NewGame,
	//BTN_Quit,
	BTN_Menu,
	BTN_Count,
};

class CDlgMainMenu:public CIssWnd
{
public:
	CDlgMainMenu(void);
	~CDlgMainMenu(void);
	void	Init(CIssHighScores* oHighScores, BOOL bIsGameInPlay, TypeOptions* psOptions){m_oHighScores = oHighScores; m_bIsGameInPlay = bIsGameInPlay; m_psOptions = psOptions;};
	static void LaunchHelp(HWND hWnd, HINSTANCE hInst);
protected:
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
/*
// 	BOOL	OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
// 	BOOL	OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
// 	BOOL	OnEnterMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam);
*/
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

private:	// functions
	BOOL	FadeInScreen(HDC hDC, RECT rcClient);
	BOOL	AnimateOutScreen();
	void	InitButtons();
	void	InitBackground();
	void	ShowMenu();
	void	ShowAboutScreen();
	void	ShowOptionsScreen();
	void	ShowHighScoreScreen();
	void	AnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal);
	static void fnAnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal);
	void	AnimateOut(CIssGDIEx& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal);
	static void fnAnimateOut(CIssGDIEx& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal);
	void	DrawBackground(RECT rcLocation);
	void	DrawButton(int iBtnIndex, BOOL bDrawDown = FALSE);
    void    DrawBtnText(int iBtnIndex, RECT& rc);

private:	// variables
	TypeOptions*    m_psOptions;
	CIssHighScores* m_oHighScores;
    BOOL            m_bIsGameInPlay;
	BOOL			m_bFirstTime;
	//CIssMenuHandler	m_oMenu;
	CIssImageSliced m_btnItem;
	CIssGDIEx		m_gdiMem;
	CIssGDIEx		m_gdiImage;					// Splash screen to load
	//CIssGDIEx		m_gdiButtons[BTN_Count];	// our buttons
	//CIssGDI			m_gdiAlphaSelected;			// alpha values for selected buttons
	RECT			m_rcItems[BTN_Count];		// location of all the buttons
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
	int				m_iSelected;				// currently selected button
	int				m_iIncreaseWidth;			// diff between selected button size and normal button size

};
