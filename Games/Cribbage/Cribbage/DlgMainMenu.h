#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssImageSliced.h"

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
	void	Init(BOOL bIsGameInPlay, CIssCribbage* oGame){m_bIsGameInPlay = bIsGameInPlay; m_oGame = oGame;};
	static void	LaunchHelp(HWND hWnd, HINSTANCE hInst);
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
    BOOL    OnMouseMove(HWND hWnd, POINT& pt);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

private:	// functions
	BOOL	FadeInScreen(HDC hDC, RECT rcClient);
	BOOL	AnimateOutScreen();
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

private:	// variables

    BOOL            m_bIsGameInPlay;
	BOOL			m_bFirstTime;

    CIssCribbage*   m_oGame;                    // just gonna pass it into the options screen

	CIssGDIEx		m_gdiMem;
	CIssGDIEx		m_gdiImage;					// Splash screen to load

};
