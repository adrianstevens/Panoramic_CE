#pragma once
#include "IssBJGame.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "IssImgFont.h"
#include "IssString.h"
#define MESSAGE_Deal	WM_USER + 10
#define MESSAGE_Reset   WM_USER + 11
#define MESSAGE_25		WM_USER + 12
#define MESSAGE_50		WM_USER + 13
#define MESSAGE_100		WM_USER + 14
#define MESSAGE_500		WM_USER + 15

#define MESSAGE_Hit			WM_USER + 16
#define MESSAGE_Stand		WM_USER + 17
#define MESSAGE_Double		WM_USER + 18
#define MESSAGE_Surrender	WM_USER + 19
#define MESSAGE_Split		WM_USER + 20
#define MESSAGE_Insurance	WM_USER + 21


enum EnumMenuBettingAmounts
{   
    MENU_25,
    MENU_50,
    MENU_100,
    MENU_500,
};

enum EnumGameplayButtons
{
    GAMEBTN_Hit,
    GAMEBTN_Stand,
    GAMEBTN_Double,
    GAMEBTN_Split,
    GAMEBTN_Insurance,
    GAMEBTN_Surrender,
    GAMEBTN_Menu,
    GAMEBTN_Count,
};

enum EnumBetting
{
    BET_25,
    BET_50,
    BET_100,
    BET_500,
    BET_Bet,
    BET_Clear,
    BET_Deal,
    BET_Menu,
    BET_Count,
};

enum EnumMenuBetting
{
    MENU_Bet,
    MENU_Reset,
    MENU_Deal,
    MENU_Menu,
};

enum EnumMenuInPlay
{
    MENU_Hit,
    MENU_Stand,
    MENU_Double,
    MENU_Split,
};


class CCardMenu
{
public:
    CCardMenu(void);
    ~CCardMenu(void);

    BOOL            DrawMenu(CIssGDIEx& gdi, RECT& rc, CIssBJGame* oGame, HINSTANCE hInst);
    RECT            GetMenuRect(){return m_rcBottomText[3];};
    RECT            GetMenuItemRect(EnumMenuBettingAmounts eBet){return m_rcTopText[eBet];};

    BOOL            OnLButtonUp(HWND hWnd, POINT pt, CIssBJGame* oGame);

    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, CIssBJGame* oGame);

    BOOL            OnSize(HWND hWnd);

private:
    BOOL            DrawSelector(CIssGDIEx& gdi, RECT& rc, CIssBJGame* oGame);


private:
    CIssString*     m_oStr;
    CIssGDIEx       m_gdiMenu;

    EnumBetting     m_eSelBetting;
    EnumGameplayButtons m_eSelGamePlay;

    HFONT           m_hFont;
    HFONT           m_hFontSm;

    RECT            m_rcMenuAlpha;
    RECT            m_rcMenuAlphaText;
    RECT            m_rcMenu;
    RECT            m_rcMenuText;

    RECT            m_rcTopText[4];
    RECT            m_rcBottomText[4];
    RECT            m_rcBottomLrg[3];

    BOOL            m_bShowSelector;
};
