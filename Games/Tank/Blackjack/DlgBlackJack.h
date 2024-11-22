#pragma once

#include "isswnd.h"
#include "ISSGDIEx.h"
#include "IssGDIDraw.h"
#include "IssString.h"
#include "IssSprite.h"
#include "IssImgFont.h"
#include "IssMenuHandler.h"
#include "IssHighScores.h"
#include "Tank.h"



#define REG_KEY		        _T("SOFTWARE\\Pano\\Blackjack")

enum EnumCardHands
{
	HAND_Dealer,
	HAND_Player,
	HAND_Split
};

class CDlgBackJack :
	public CIssWnd
{
public:
	CDlgBackJack(void);
public:
	~CDlgBackJack(void);
	void	Init(HINSTANCE hInst);

private:
    void    GameLoop();

	void	SaveRegSettings();
	void	LoadRegSettings();

	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnMouseMove(HWND hWnd, POINT& pt);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnHandleNavBtn(HWND hWnd, WPARAM wParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
    BOOL    OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	OnEnterMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL	DrawBackground(CIssGDIEx& gdi, RECT& rc);
    BOOL    DrawTableText(CIssGDIEx& gdiDest, RECT& rc);

	void	PlaySounds(TCHAR* szWave);

	void	LoadImages(HWND hWndSplash, int iMaxPercent);
    void	LoadFonts(HWND hWndSplash, int iMaxPercent);

	BOOL	IsLowRes();

    BOOL    ShowSplashScreen();
    BOOL    ShowMainMenu();
    BOOL	ShowMenu();

    void    AniDealerFlip();
    void    AniDollar(POINT ptStart, POINT ptEnd);


    void    NewGame(BOOL bCheckHiScore = TRUE);

    BOOL    ForceScreenUpdate();

private:

    CIssMenuHandler		m_oMenuHandler;
    CIssHighScores      m_oHighScores;

    CTank               m_oTank;



	CIssString*			m_oStr;
	CIssGDIEx    		m_gdiMem;
	CIssGDIEx			m_gdiBackground;

    CIssGDIEx           m_gdiDollar;                //cool little animation yo

    CIssGDIEx           m_gdiMenu;

    CIssSprite          m_aniCardFlip;              //card flip animation

    RECT                m_rcTopText;

  

	HFONT				m_hfButtonText;
	HFONT				m_hfScreenText;
    HFONT               m_hFontCount;               //big numbers

	BOOL				m_bPlaySounds;
    BOOL                m_bShowCount;               //temp variable
    BOOL                m_bAniCardFlip;
    BOOL                m_bShowSplashScreen;


	SIZE				m_sizeWindow;				// size of the Window, used to prevent reloading of images

	int					m_iYBtnStart;				// Y position of buttons 

    //for floating text
    BOOL                m_bDrawFloat;
    DWORD               m_dwStartFloatMsg;
    DWORD               m_dwTickCount;




};
