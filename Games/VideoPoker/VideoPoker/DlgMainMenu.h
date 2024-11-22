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
    void    Init(BOOL bIsGameInPlay, 
                BOOL bPlaysounds, 
                BOOL bAutoHold,
                BOOL bQuickPlay,
                EnumBackgroundColor eBG, 
                EnumBetAmounts eBets, 
                EnumMultiPlay eMulti);
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

	void	ShowMenu();
	void	ShowAboutScreen();
	void	ShowOptionsScreen();
	void	AnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal);
	static void fnAnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal);
	void	AnimateOut(CIssGDIEx& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal);
	static void fnAnimateOut(CIssGDIEx& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal);

    void    LoadImages();

public: //cause I'm lazy
    EnumBackgroundColor m_eBGColor;
    EnumBetAmounts  m_eBetAmount;
    EnumMultiPlay   m_eMultiplay;
    BOOL            m_bPlaySounds;
    BOOL            m_bAutoHold;
    BOOL            m_bQuickPlay;

private:	// variables

    CIssGDIEx       m_gdiTitle;

    BOOL            m_bIsGameInPlay;
    BOOL			m_bFirstTime;
};
