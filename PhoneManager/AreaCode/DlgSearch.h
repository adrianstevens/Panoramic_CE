#pragma once
#include "dlgbase.h"
#include "IssGDIEx.h"
#include "IssWndTouchMenu.h"
#include "IssKineticList.h"

class CDlgSearch : public CDlgBase
{
public:
    CDlgSearch(void);
    ~CDlgSearch(void);

    BOOL            OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
//    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
//    BOOL            OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
//    BOOL            OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);

  //  BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
 //   BOOL		    OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
 //   BOOL		    OnLButtonUp(HWND hWnd, POINT& pt);
 //   BOOL		    OnLButtonDown(HWND hWnd, POINT& pt);
//   BOOL		    OnMouseMove(HWND hWnd, POINT& pt);
    BOOL		    OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
    BOOL            InitMenu();
    BOOL            PreloadImages(HWND hWnd, HINSTANCE hInst);

    void            InitMenuEntries();

private:
    CIssGDIEx       m_gdiMem;
    CIssWndTouchMenu m_wndMenu;
};
