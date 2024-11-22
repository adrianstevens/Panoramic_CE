#pragma once

#include "IssGDIEx.h"
#include "IssString.h"
#include "IssImageSliced.h"
#include "IssWndTouchMenu.h"
#include "IssCribbage.h"

enum EnumMenuSelect
{
    MENU_Left,
    MENU_Right,
    MENU_None,
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
    void    DrawBackground(RECT& rcClip);

    BOOL    OnLButtonDown(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow = FALSE);
    BOOL    OnLButtonUp(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow = FALSE);
    BOOL    OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL    OnSize(HWND hWnd, RECT rcClient);

    BOOL    GetIsSmartPhone(){return m_bIsSmartphone;};

private:
    void    Destroy();

    void    InitBottomBar(RECT& rcClient);
    void    InitBackground(RECT& rcClient);

public:
    CIssGDIEx       gdiMem;
    CIssGDIEx       gdiBackground;
    CIssGDIEx       gdiMenuArray;
    CIssGDIEx       gdiBottomBar;

    CIssImageSliced imgSelector;
    CIssImageSliced imgGlow;

    CIssWndTouchMenu wndMenu;

    EnumMenuSelect  eMenuSelect;
    EnumBackgroundColor m_eBackground;
    
    HFONT           hFontLarge;

    RECT            rcBottomBar;
    RECT            rcBackground;

private:
    CIssString*     m_oStr;    

    HWND            m_hWndParent;
    
    HINSTANCE       m_hInst;

    BOOL            m_bIsSmartphone;


};
