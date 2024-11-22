#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssImageSliced.h"

#include "DlgOptions.h"


enum EnumMenuState
{
    MSTATE_SelectGame,
    MSTATE_NewOrCustom,
    MSTATE_Size,
    MSTATE_Difficulty,
    MSTATE_Count,
};


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
    void    Init(CSEngine* oGame,
                 TypeOptions &sOptions);
	static  void LaunchHelp(HWND hWnd, HINSTANCE hInst);
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

    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void     DeleteListItem(LPVOID lpItem);

    void    PopulateList();

    BOOL    HandleMenu(WPARAM wParam);

    void    OnResume();
    void    OnNewGame();
    void    OnBack();

    void    OnLeftMenu();
    void    OnRightMenu();

    BOOL    IsNewGame();



private:	// variables
    static CIssKineticList* m_oMenu;

    TypeOptions*    m_sOptions;

    CSEngine*       m_oGame;

    EnumMenuState   m_eState;

    RECT            m_rcText;

    HFONT           m_hFontMenu;

    static DWORD    m_dwFullListCount;

    BOOL            m_bIsGameInPlay;
    BOOL			m_bFirstTime;
    BOOL            m_bDrawSelector;

    static BOOL     m_bPreventDeleteList;
};
