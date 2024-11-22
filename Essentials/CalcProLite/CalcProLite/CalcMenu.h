#pragma once

#include "IssGDIEx.h"
#include "CalcBtnManager.h"

#define WM_LEFT_SOFTKEY     WM_USER + 200
#define WM_RIGHT_SOFTKEY    WM_USER + 201
#define WM_ICON_SOFTKEY     WM_USER + 202

enum EnumMenuPressed
{
    MENU_None = -1,
    MENU_Left = 1,
    MENU_Right,
    MENU_Icon,
    
};

class CCalcMenu
{
public:
    CCalcMenu(void);
    ~CCalcMenu(void);

    HRESULT             Init(HWND hWnd, HINSTANCE hInst, EnumCalcProSkins eSkin);//so we can load the image
    BOOL                DrawMenu(HDC hdc, RECT& rcClip, TCHAR* szLeft, TCHAR* szRight);

    BOOL                OnLButtonDown(POINT& pt, HWND hWnd);
    EnumMenuPressed     OnLButtonUp(POINT& pt, HWND hWnd);

    BOOL                OnLeftMenu(HWND hWnd);
    BOOL                OnRightMenu(HWND hWnd);
    BOOL                OnIconMenu(HWND hWnd);

    int                 GetMenuHeight(){return HEIGHT(m_rcMenu)+1;};

private:

private:
    CIssGDIEx           m_gdiMem;

    EnumMenuPressed     m_eMenuPressed;

    RECT                m_rcMenu;
    RECT                m_rcLeft;
    RECT                m_rcRight;
    RECT                m_rcIcon;

    HFONT               m_hFontMenu;
};
