#pragma once

#include "IssWnd.h"
#include "IssGDI.h"
#include "IssMenuHandler.h"
#include "IssBeveledEdge.h"
#include "IssSoundFX.h"
#include "IssHardwareKeys.h"
#include "HexGame.h"

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
	void	Init(CIssSoundFX* oSoundFX, CHexGame* oGame, CIssHardwareKeys* oKeys){m_oSoundFX = oSoundFX;m_oGame = oGame; m_oKeys = oKeys;};
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
	BOOL	OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEnterMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam);
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
	void	AnimateIn(CIssGDI& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal);
	static void fnAnimateIn(CIssGDI& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal);
	void	AnimateOut(CIssGDI& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal);
	static void fnAnimateOut(CIssGDI& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal);
	void	DrawBackground(RECT rcLocation);
	void	DrawButton(int iBtnIndex, BOOL bDrawDown = FALSE);

private:	// variables
	CIssSoundFX*	m_oSoundFX;
	CHexGame*		m_oGame;
	CIssHardwareKeys* m_oKeys;
	BOOL			m_bFirstTime;
	CIssMenuHandler	m_oMenu;
	CIssBeveledEdge m_btnItem;
	CIssGDI			m_gdiMem;
	CIssGDI			m_gdiImage;					// Splash screen to load
	CIssGDI			m_gdiButtons[BTN_Count];	// our buttons
	//CIssGDI			m_gdiAlphaSelected;			// alpha values for selected buttons
	RECT			m_rcItems[BTN_Count];		// location of all the buttons
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
	int				m_iSelected;				// currently selected button
	int				m_iIncreaseWidth;			// diff between selected button size and normal button size

};
