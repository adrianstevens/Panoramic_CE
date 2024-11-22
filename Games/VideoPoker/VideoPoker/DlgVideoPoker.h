#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "IssVPGame.h"
#include "ObjGui.h"
#include "DlgGameType.h"

#define IDT_Time_Timer		101
#define	IDT_NewGame_Timer	102
#define IDT_InvalidMove_Timer 104

#define DISPLAY_MESSAGE_TIME	1000 // 1 second

enum EnumGameButtons
{
    GBTN_BetOne,
    GBTN_BetMax,
    GBTN_Draw,
    GBTN_Menu,
    GBTN_Count,

};

class CDlgVideoPoker:public CIssWnd
{
public:
	CDlgVideoPoker(void);
	~CDlgVideoPoker(void);

	BOOL	Init(HINSTANCE hInstance){m_hInst = hInstance; return TRUE;};

protected:
    CIssVPGame* GetGame(){return &m_oGame;};

	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL    OnMouseMove(HWND hWnd, POINT& pt);
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("PanoVideoPoker.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};

private:	// Functions
	BOOL	DrawCards(CIssGDIEx& gdi, RECT& rcClip);
    BOOL    DrawCardsMulti(CIssGDIEx& gdi, RECT& rcClip);//keeps it clean
	BOOL	DrawText(CIssGDIEx& gdi, RECT& rcClient);
	BOOL	DrawSelected(CIssGDIEx& gdi, RECT& rcClip);
    BOOL	DrawSelectedMulti(CIssGDIEx& gdi, RECT& rcClip);
	BOOL	DrawSelector(CIssGDIEx& gdi, RECT& rcClient);
    BOOL    DrawWinHighlight(CIssGDIEx& gdi, RECT& rcClient);
    BOOL    DrawWinHighlightMulti(CIssGDIEx& gdi, RECT& rcClient);
    BOOL    DrawButtons(CIssGDIEx& gdi, RECT& rcClip);

    BOOL    DrawScore(CIssGDIEx& gdi, POINT pt, int iScore = 0);
    BOOL    DrawPayoutTable(CIssGDIEx& gdi, RECT& rcClip);
    
	void	PlaySounds(TCHAR* szWave);

	void	OnNewGame();
    void    OnSelectGame();
	BOOL	ShouldAbort();

	void	LoadImages(HWND hWndSplash, int iMaxPercent);
    
	//helper
	void	UpdateSelector(int iOldSel);

    BOOL    OnMenuMenu();

    BOOL    ShowMainMenu();
    BOOL    ShowSplashScreen();

    BOOL	EvaluateState();
    BOOL	AnimateBankIncrease(int iOldBank, int iNewBank);

    BOOL    GetResultsLabel(TCHAR* szResult, EnumPokerResult eResult);

    void    SaveRegistry();

    void    CalcCardPositions();
    void    OnDeal();//start timers and redraw screen 

private:	// Variables
    CDlgGameType m_dlgGameType;
	CIssVPGame	m_oGame;
    CIssPokerScoreCalc* m_oCalc;

	CIssGDIEx	m_gdiCards[4];			// spades card deck
    CIssGDIEx   m_gdiCardsSm[4];       // for our multihand
    CIssGDIEx   m_gdiX2;                // only for stud .. replaces max ... keep it separate
	CIssGDIEx	m_gdiCardBack;			// image of card back
    CIssGDIEx   m_gdiCardBackSm;
    CIssGDIEx   m_gdiDigitalFont;       // 11 entries
    CIssGDIEx   m_gdiHold;              
    
    CIssGDIEx   m_gdiBtn[GBTN_Count];
    BOOL        m_bBtnDown[GBTN_Count];
    RECT        m_rcBtn[GBTN_Count];
    

    CIssImageSliced m_imgScore;         // might want more than one?
    CIssImageSliced m_imgSelector;
    CIssImageSliced m_imgWinSelector;
    CIssImageSliced m_imgWinSelectorSm;
    CIssImageSliced m_imgWinGlow;
    CIssImageSliced m_img33;
    CIssImageSliced m_img50;
    CIssImageSliced m_imgMenu;

    HFONT       m_hFontText;
    HFONT       m_hFontPayout;
	HFONT		m_hFontTitle;

    SIZE		m_sizeWindow;			// size of the Window, used to prevent reloading of images
	SIZE		m_sizeCards;

    RECT        m_rcCards[5];
    //we'll calculate 5 for now ...
    //update to 7 for tall devices later
    RECT        m_rcCardsM[4][5];
    RECT        m_rcCardArea;
        
	RECT		m_rcDisplay;
    RECT        m_rcBetText;
    RECT        m_rcBetAmount;
    RECT        m_rcCreditsText;
    RECT        m_rcCreditsAmount;
    

    RECT        m_rcResults[12];

    RECT        m_rcMenu;
    RECT        m_rcLowerText;

    EnumBackgroundColor m_eBGColor;
    BOOL        m_bPlaySounds;
    BOOL        m_bQuickPlay;

    
    BOOL		m_bShowSplashScreen;	// BOOL to show the Menu screen
	BOOL		m_bDrawSelector;
	BOOL		m_bMessageBoxUp;        // probably not needed anymore but I'm too lazy to take it out
    	
	int			m_iSelector;
	int			m_iHorizontalOffset;    //to center on landscape ... might still use this ....

    //For the onscreen selector
    int			m_iCurrentSelectedBtn;	
    BOOL		m_bSelHold;	
};
