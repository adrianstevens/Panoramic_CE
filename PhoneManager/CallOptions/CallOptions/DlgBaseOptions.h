#pragma once

#include "DlgBase.h"
#include "IssGDIDraw.h"
#include "IssGDIEx.h"
#include "GuiBackground.h"
#include "IssWndTouchMenu.h"
#include "IssKineticList.h"
#include "IssImageSliced.h"

#define COLOR_TEXT_NORMAL 0xFFFFFF 

class CDlgBaseOptions:public CDlgBase
{
public:
    CDlgBaseOptions(void);
    ~CDlgBaseOptions(void);

    BOOL    PreloadImages(HWND hWndParent, HINSTANCE hInst);
    BOOL    Create(TCHAR* szWindowName, TCHAR* szClassName, HINSTANCE hInst);
    BOOL    Launch(HWND hWndParent, HINSTANCE hInst, BOOL bAnimate);
    void    OnColorSchemeChange(HWND hWndParent, HINSTANCE hInst);

protected:      // functions
    void    DrawButton(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc);
    BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL	OnMouseMove(HWND hWnd, POINT& pt);
    BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

protected:      // variables
    static CIssGDIEx       m_gdi;
    static CIssGDIEx       m_gdiBackArrow;
    static CIssGDIEx       m_gdiForwardArrow;
    static CGuiBackground  m_guiBG;
    static CIssKineticList m_oMenu;
    static CIssWndTouchMenu m_wndMenu;
    static CIssImageSliced  m_imgBtn;
    static CIssImageSliced  m_imgBtnSelected;
    BOOL                    m_bAnimate;
};
