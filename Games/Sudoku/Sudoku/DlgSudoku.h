#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "ObjGui.h"
#include "SEngine.h"
#include "DlgMainMenu.h"

enum EnumDraw
{
    DRAW_Guess,
    DRAW_Given,
    DRAW_PencilMarks,
    DRAW_Kakuro,
    DRAW_Blank,
    DRAW_None,
};

enum EnumMenuButtons
{
    MENU_Undo,
    MENU_Hint,
    MENU_Pencil,
    MENU_Menu,
};


class CDlgSudoku:public CIssWnd
{
public:
	CDlgSudoku(void);
	~CDlgSudoku(void);

	BOOL	Init(HINSTANCE hInstance){m_hInst = hInstance; return TRUE;};

protected:
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

	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("PanoSudoku.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};

private:	// Functions
    void    DrawBackground(CIssGDIEx& gdi, RECT& rcClip);
    BOOL    DrawCell(CIssGDIEx& gdi, int iX, int iY, BOOL bNoText = FALSE);
    BOOL    DrawCellUpdate(CIssGDIEx& gdi, int iX, int iY);
    BOOL    DrawCellBackground(CIssGDIEx& gdi, int iX, int iY, EnumSquares eSquares);
    BOOL    DrawCellFont(CIssGDIEx& gdi, int iX, int iY, int iValue, EnumSquares eSquares);
    BOOL    DrawPencilMarks(CIssGDIEx& gdi, int iX, int iY, EnumSquares eSquare);
    BOOL    DrawOutlineItem(CIssGDIEx& gdi, int x, int y);
    BOOL    DrawTotal(CIssGDIEx& gdi, int iX, int iY, EnumSquares eSquare);
    BOOL    DrawGreater(CIssGDIEx& gdi, int iX, int iY, EnumSquares eSquare);
	BOOL	DrawText(CIssGDIEx& gdi, RECT& rcClip);
	BOOL	DrawSelector(CIssGDIEx& gdi, RECT& rcClip);
    BOOL    DrawButtons(CIssGDIEx& gdi, RECT& rcClip);
    BOOL    DrawCells(CIssGDIEx& gdi, RECT& rcClip, BOOL bNoText = FALSE);
    BOOL    UpdateCells(CIssGDIEx& gdi, RECT& rcClip);
    void	PlaySounds(TCHAR* szWave);

	void	OnNewGame();
    BOOL	ShouldAbort();
    BOOL    OnNumber(int iNum);

	void	LoadImages(HWND hWndSplash, int iMaxPercent);
    
	//helper
	void	UpdateSelector(int iOldSel);

    BOOL    OnMenuMenu();
    BOOL    OnMenuSolve();
    BOOL    OnMenuHint();
    BOOL    OnMenuUndo();
    BOOL    OnMenuOptions();
    BOOL    OnMenuStats();
    BOOL    OnMenuPencil();

    BOOL    ShowMainMenu();
    BOOL    ShowSplashScreen();

    void    SaveRegistry();

    void    CalculateLayout();

    BOOL    DrawHighlight(int iX, int iY);

    BOOL    IsAltColor(int iX, int iY);
   
    BOOL    FadeTransitionCell(int iX, int iY, int iTime = CORRECT_CELL_ANIMATION_TIME);
    BOOL    FadeOutCell(int iX, int iY, int iTime = CORRECT_CELL_ANIMATION_TIME);

    BOOL    IsPreDrawnCell(int x, int y);

    BOOL    AnimateEndGame();

    BOOL    AnimateEndGameSelector();//specifics
    BOOL    AnimateEndGameFade();
    BOOL    AnimateEndGameSolitaire();
    BOOL    AnimateEndGameScale();
    BOOL    AnimateEndGameBounce();
    BOOL    AnimateEndGameFireworks();
    BOOL    AnimateEndGameFireworks2();
    BOOL    AnimateSolve();
    BOOL    FadeToBlank();

    void    ResetSelector();

    void    ReloadSkin();

    COLORREF    GetRandomFireColor();
    COLORREF    BlendColors(COLORREF cr1, COLORREF cr2, int iWeight); //0 is cr1 - 100 cr2

    void        FormatTime(int iTime, TCHAR* szTime);

   
private:	// Variables
    CDlgMainMenu	m_dlgMenu;
    CSEngine        m_oGame;

    CIssImageSliced m_imgBar;
    CIssImageSliced m_imgBtn;
    CIssImageSliced m_imgBtnBack;
    CIssImageSliced m_imgTop;
    CIssImageSliced m_imgMenu;

    CIssGDIEx       m_gdiIcons;
    CIssGDIEx       m_gdiIconGlow;
    CIssGDIEx       m_gdiBtnFontGrey;
    CIssGDIEx       m_gdiBtnFontWhite;

    CIssGDIEx       m_gdiBackground; //yeah its in the gui layer but we're going
                                     //to reduce some draw calls

    TypeOptions     m_sOptions;//just easier to keep it in here 

    TCHAR       m_szTime[STRING_NORMAL];

    HFONT       m_hFontText;
	HFONT		m_hFontTitle;
    HFONT       m_hFontPencil;
    HFONT       m_hFontTotals;
    
    SIZE		m_sizeWindow;			// size of the Window, used to prevent reloading of images
	SIZE		m_sizeCards;

    RECT        m_rcTime;
    RECT        m_rcRemaining;

    RECT        m_rcGrid[MAX_GRID_SIZE][MAX_GRID_SIZE];
    RECT        m_rcValues[9];
    RECT        m_rcGameBoard;  //we'll use this for redraws ... save a few pixels
    RECT        m_rcNumButtons;
    
    RECT        m_rcUndo;
    RECT        m_rcHint;
    RECT        m_rcPencil;
    RECT        m_rcMenu;

    RECT        m_rcBottomBar;
    
  

    BOOL		m_bShowSplashScreen;	// BOOL to show the Menu screen
	BOOL		m_bMessageBoxUp;        // probably not needed anymore but I'm too lazy to take it out

    BOOL        m_bSelectValues;
    BOOL        m_bPenMode;

    
    //for debugging
    BOOL        m_bShowSolution;


    POINT       m_ptSelector;
    POINT       m_ptGlowOffset;

    int         m_iTimer;

    int         m_iButtonUp;

    int         m_iMenuHighlight;
};
