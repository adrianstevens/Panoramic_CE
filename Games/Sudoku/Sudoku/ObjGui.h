#pragma once

#include "IssGDIEx.h"
#include "IssString.h"
#include "IssImageSliced.h"
#include "IssWndTouchMenu.h"
#include "SEngine.h"

#define SIZE_CELLARRAY 9
#define SIZE_CELLARRAY2 6

enum EnumSquares
{
    SQ_Board1,
    SQ_Board2,
    SQ_Given,
    SQ_Guess1,
    SQ_Guess2,
    SQ_Locked,
    SQ_Highlighted,
    SQ_Selector,
    SQ_Kakuro,
    SQ_Killer1,//seperate image
    SQ_Killer2,
    SQ_KillerSel,
    SQ_Killer3,
    SQ_Killer4,
    SQ_KillerSel2,
    SQ_None = 999,
};

enum EnumMenuSelect
{
    MENU_Left,
    MENU_Right,
    MENU_None,
};

enum EnumSudokuSkins
{
    SSKIN_Glass,
    SSKIN_Wood,
    SSKIN_Notepad,
    SSKIN_Grey,
    SSKIN_Custom0,
    SSKIN_Custom1,
    SSKIN_Custom2,
    SSKIN_Custom3,
    SSKIN_Custom4,
    SSKIN_Custom5,
    SSKIN_Count,
};


enum EnumPicSelection
{
    PIC_Camera,
    PIC_Picture,
    PIC_Given,
    PIC_None,
};

struct TypeSudokuSkin
{
    COLORREF    crPencil1;
    COLORREF    crPencil2;
    COLORREF    crPencilKakuro;  //for kakuro    
    COLORREF    crTopText;
    COLORREF    crTotal1;        //for killer sudoku
    COLORREF    crTotal2;        //for killer sudoku
    COLORREF    crOutline1;      //for killer sudoku
    COLORREF    crOutline2;      //for killer sudoku
    COLORREF    crOutline3;      //for killer sudoku
    COLORREF    crOutline4;      //for killer sudoku
    COLORREF    crOutline5;      //for killer sudoku
    int         iNumOutlineColors;
    COLORREF    crKakuroTotals;  //for Kakuro
    COLORREF    crGrad1;
    COLORREF    crGrad2;
    COLORREF    crSelector;
    int         iSelAlpha;
    UINT        uiFont1;
    UINT        uiFont2;
    UINT        uiCellArray;
    UINT        uiCellArray2;
    UINT        uiBackground;
    CIssGDIEx   gdiSQs;
    CIssGDIEx   gdiSQs2;    //at the moment there's only one so this is fine
    CIssGDIEx   gdiFont1;
    CIssGDIEx   gdiFont2;

    EnumSudokuSkins eSkin;
    EnumBackground eBackground;
};


#define IDMENU_Left         10000
#define IDMENU_Right        10001

class CObjGui
{
public:
    CObjGui(void);
    ~CObjGui(void);

    void    Init(HWND hWndParent, HINSTANCE hInst);
    void    InitGDI(RECT& rcClient, HDC dc);

    void    DrawScreen(RECT& rcClip, HDC dc);
    void    DrawBottomBar(CIssGDIEx& gdi, RECT& rcClip, TCHAR* szLeftMenu, TCHAR* szRightMenu, BOOL bLeftMenuGrayed = FALSE, BOOL bRightMenuGrayed = FALSE);
    void    DrawBottomBar(RECT& rcClip, TCHAR* szLeftMenu, TCHAR* szRightMenu, BOOL bLeftMenuGrayed = FALSE, BOOL bRightMenuGrayed = FALSE);
    void    DrawBackground(RECT& rcClip, EnumSudokuType eSudoku);
    void    DrawTitle(RECT& rcClip);

    BOOL    OnLButtonDown(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow = FALSE);
    BOOL    OnLButtonUp(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow = FALSE);
    BOOL    OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL    OnSize(HWND hWnd, RECT rcClient);
    void    SetCellSize(RECT& rcSize, int iGridSize, EnumSudokuType eType);//grid size is the number of blocks in the grid .. ie 9 across

    BOOL    LoadSkin(EnumSudokuSkins eSkin, EnumBackground eBackground = BACKGROUND_Default, BOOL bForceReload = FALSE);

    BOOL    LoadBackground(EnumSudokuType eType);
    BOOL    LoadTitle();
    BOOL    LoadBackground(UINT uiRes);
    BOOL    LoadBackground(COLORREF crGrad1, COLORREF crGrad);
    BOOL    LoadBackground(TCHAR* szFileName);

    void    NextSkin(EnumSudokuType eType);
    void    NextBackground();

    BOOL    UseFont1(EnumSquares eSquare);

    COLORREF GetTotalColor(EnumSquares eSquares);
    COLORREF GetPencilMark(EnumSquares eSquare, EnumSudokuType eType);
    CIssGDIEx* GetFontGDI(EnumSquares eSquare);


    void    SetSkin(){LoadSkin(sSkin.eSkin, sSkin.eBackground);};
    void    SetSkin(EnumBackground eBack, EnumSudokuSkins eSkin){LoadSkin(eSkin, eBack);};
    void    SetSkin(EnumBackground eBack){LoadSkin(sSkin.eSkin, eBack);}
    void    SetSkin(EnumSudokuSkins eSkin){LoadSkin(eSkin, sSkin.eBackground);};


    EnumSquares GetSquareEnum(EnumSquares eSquare, EnumSudokuType eType, BOOL bSameRowAsSelector);
    COLORREF    GetOutlineColor(int iSection, BOOL bHighlight);

    COLORREF GetBackgroundColor(BOOL bColor1, EnumBackground eBack);
    COLORREF GetBackgroundColor(BOOL bColor1);
    
    BOOL        SetImageBackground(TCHAR* szPath);
    BOOL        GetImagePath(TCHAR* szPath);

    BOOL        IsBackgroundLoaded(){if(gdiBackground.GetDC())return TRUE; return FALSE;};

private:
    void    Destroy();
    void    InitBottomBar(RECT& rcClient);

    BOOL    UseNotePadBG(EnumSudokuType eType);
    
    
public:
    TypeSudokuSkin  sSkin;
    CIssGDIEx       gdiMem;
    CIssGDIEx       gdiTitle;
    CIssGDIEx       gdiBackground;
    CIssGDIEx       gdiMenuArray;
    CIssGDIEx       gdiBottomBar;

    SIZE            szWindow;

    CIssImageSliced imgSelector;
    CIssImageSliced imgGlow;

    CIssWndTouchMenu wndMenu;

    RECT            rcBottomBar;
    RECT            rcBackground;

    int             iXCellOff;
    int             iYCellOff;
    int             iFontWidth;//save some math

private:
     EnumMenuSelect  eMenuSelect;
     EnumSudokuType  eGameType;

     HFONT           hFontLarge;

     RECT            rcCell;
     int             iGridSize;

     CIssString*     m_oStr;     

     HWND            m_hWndParent; 
    
     HINSTANCE       m_hInst;

     TCHAR           m_szImagePath[MAX_PATH];
};
