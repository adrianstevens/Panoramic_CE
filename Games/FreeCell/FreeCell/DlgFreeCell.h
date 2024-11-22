#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "IssFreeCell.h"
#include "ObjGui.h"

#define IDT_Time_Timer		101
#define	IDT_NewGame_Timer	102
#define IDT_InvalidMove_Timer 104

#define DISPLAY_MESSAGE_TIME	1000 // 1 second

class CDlgFreeCell:public CIssWnd
{
public:
	CDlgFreeCell(void);
	~CDlgFreeCell(void);

	BOOL	Init(HINSTANCE hInstance){m_hInst = hInstance; return TRUE;};

protected:
    CIssFreeCell* GetGame(){return &m_oGame;};

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

	//BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("PanoFreecell.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};

private:	// Functions
	BOOL	DrawCards(CIssGDIEx& gdi, RECT& rcClip);
	BOOL	DrawText(HDC hDC, RECT& rcClient);
	BOOL	DrawSelected(CIssGDIEx& gdi, RECT& rcClip);
	BOOL	DrawSelector(CIssGDIEx& gdi, RECT& rcClient);
    BOOL    DrawTopBar(CIssGDIEx& gdi, RECT& rcClip);
    
	void	PlaySounds(TCHAR* szWave);
	void	LaunchMenu(BOOL bFirstTime = FALSE);

	void	OnNewGame();
	void	AnimateSolitaire(HDC hDC);
    void    DragCard(int* iCard, int iLocation, POINT pt);
	void	AnimateCardtoAceRack(int iCard, int iAceRack, BOOL bFromColumn); //from column or from freecell
	BOOL	ShouldAbort();

	void	LoadImages(HWND hWndSplash, int iMaxPercent);
    
	//helper
	void	DrawCard(CIssGDIEx& gdi, int iCard, POINT ptLocation);//set sCard to null for cardback
    void	DrawCard(HDC hDC, int iCard, POINT ptLocation);
	void	UpdateSelector(int iOldSel);


	BOOL	OnGameMenu();
    BOOL    OnMenuMenu();

	BOOL	CheckAndDrawPreview(POINT pt);//we'll check the RECTs and paint to the screen if nesessary

    BOOL    ShowMainMenu();
    BOOL    ShowSplashScreen();

    BOOL    CheckCardDrag(POINT pt);

    EnumCardLocations GetLocation(POINT pt);//returns -1 if not set

    BOOL    OnLBDColumns(POINT pt, int iColumm);

    int     GetCardOffset(int iColumn = -1); //default just return normal

    void    ResizeImages();

private:	// Variables
	CIssFreeCell	m_oGame;

	CIssGDIEx	m_gdiCards[4];			// spades card deck
	CIssGDIEx	m_gdiCardBack;			// image of card back
	CIssGDIEx   m_gdiMoveBackground;	// We'll use this as the background when we're dragging
    CIssGDIEx   m_gdiArrow;
	CIssGDIEx	m_gdiSeparator;

    CIssImageSliced m_imgSelector;
    CIssImageSliced m_imgTopBar;
    CIssImageSliced m_imgShadow;

	RECT		m_rcCards[CARD_COLUMNS];
	RECT		m_rcFreeCells[4];
	RECT		m_rcAceRack[4];
	
    RECT        m_rcTopBar;
//    RECT        m_rcGameNum;
//    RECT        m_rcCardsLeft;
    RECT        m_rcUndo;
    
       
	RECT		m_rcMsgText;

    RECT		m_rcOldSelectedCard;

    HFONT       m_hFontText;
	
    BOOL		m_bShowSplashScreen;	// BOOL to show the Menu screen
	SIZE		m_sizeWindow;			// size of the Window, used to prevent reloading of images
	SIZE		m_sizeCards;

	BOOL		m_bDrawSelector;
	BOOL		m_bShowInvalidMove;
	BOOL		m_bDrawRedAces;			// toggle to draw red overlay to show where the aces are at the start of the game
	BOOL		m_bMessageBoxUp;        // probably not needed anymore but I'm too lazy to take it out
    BOOL        m_bCardSelected;
    BOOL        m_bCardMoved;

    //dragging action ...
    POINT       m_ptDragStart;
    POINT       m_ptDragOffset;
    int         m_iSelectedCard[12];    //that's our max yo
    int         m_iSelectedLocation;    //ie where was it grabbed
    int         m_iSelectedCount;       //how far down the stack

	
	int			m_iSelector;
	int			m_iHorizontalOffset; //to center on landscape
    int         m_iMaxCardsPerColumn;

	COLORREF	m_rcTransparentColor;
};
