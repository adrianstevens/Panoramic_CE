#pragma once

#include "isswnd.h"
#include "ISSGDIEx.h"
#include "IssGDIDraw.h"
#include "IssString.h"
#include "IssSprite.h"
#include "CardMenu.h"
#include "IssImgFont.h"
#include "IssMenuHandler.h"
#include "IssBJGame.h"
#include "IssHighScores.h"
#include "BJMetrics.h"


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
	CDlgBackJack(BOOL bDemo);
public:
	~CDlgBackJack(void);
	void	Init(HINSTANCE hInst);

private:
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
/*
//     BOOL	OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
//     BOOL	OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
//     BOOL	OnEnterMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam);
*/

	BOOL	DrawBackground(CIssGDIEx& gdi, RECT& rc);
	BOOL	DrawCards(CIssGDIEx& gdi, RECT& rc);
    BOOL    DrawChips(CIssGDIEx& gdi, RECT& rc);
    BOOL    DrawScores(CIssGDIEx& gdi, RECT& rc);
    BOOL    DrawTableText(CIssGDIEx& gdiDest, RECT& rc);

	void	PlaySounds(TCHAR* szWave);

	void	LoadImages(HWND hWndSplash, int iMaxPercent);
    void	LoadFonts(HWND hWndSplash, int iMaxPercent);
	BOOL	DrawCard(CIssGDIEx& gdi, int iX, int iY, EnumCardSuit eSuit, EnumCardRank eRank, RECT& rc, BOOL bShowFace = TRUE, BOOL bRotate = FALSE);
	BOOL	DrawHand(EnumCardHands eHand, CIssGDIEx& gdi, RECT& rc);

	void	FormatCurrency(TCHAR* szCurrency);
	BOOL	IsLowRes();
	void	DealNextCard();
	BOOL	RecalcCardLocations();
    BOOL    ShowSplashScreen();
    BOOL    ShowMainMenu();
    BOOL	ShowMenu();

    void    StartAnimation();
    void    EndAnimation();

    void    AniDealerFlip();
    void    AniDollar(POINT ptStart, POINT ptEnd, BOOL bPlaySound = TRUE);
    void    UpdateOptions(TypeOptions& sOptions);

    void    NewGame(BOOL bCheckHiScore = TRUE);

    BOOL    AnimateBankIncrease(int iOldBank, int iNewBank);

    BOOL    ForceScreenUpdate();

private:
    CBJMetrics          m_oMet;
    //CIssMenuHandler		m_oMenuHandler;
    CIssHighScores      m_oHighScores;
	CIssBJGame			m_oGame;
    CCardMenu           m_oMenu;
	BOOL				m_bDemo;

	CIssString*			m_oStr;
	CIssGDIEx    		m_gdiMem;
	CIssGDIEx			m_gdiBackground;
	CIssGDIEx			m_gdiCards[4];
	CIssGDIEx			m_gdiCardBack;				//single for now...array in future
    CIssGDIEx           m_gdiCount;
    CIssGDIEx           m_gdiDollar;                //cool little animation yo

    CIssGDIEx           m_gdiMenu;
    CIssGDIEx           m_gdiChips;

    CIssSprite          m_aniCardFlip;              //card flip animation

    RECT                m_rcChips[3];
    RECT                m_rcTopText;

    RECT                m_rcDealerScore;
    RECT                m_rcPlayerScore;
    RECT                m_rcSplitScore;

	POINT				m_ptDealer;					// location of the Dealer hand
	POINT				m_ptPlayer;					// location of the player hand
    POINT               m_ptPlayerwSplit;
    POINT               m_ptSplit;


    POINT               m_ptCountBtn;
    POINT               m_ptChips;                  

	HFONT				m_hfButtonText;
    HFONT				m_hfScreenText;
    HFONT               m_hFontCount;               //big numbers

	BOOL				m_bPlaySounds;
    BOOL                m_bShowCount;               //temp variable
    BOOL                m_bAniCardFlip;
    BOOL                m_bShowSplashScreen;

	EnumBetting			m_eSelBetting;				// betting Selector
	EnumGameplayButtons m_eSelGamePlay;				// Game Play Selector
	SIZE				m_sizeWindow;				// size of the Window, used to prevent reloading of images

	int					m_iYBtnStart;				// Y position of buttons 

    //for floating text
    BOOL                m_bDrawFloat;
    DWORD               m_dwStartFloatMsg;
    DWORD               m_dwTickCount;




};
