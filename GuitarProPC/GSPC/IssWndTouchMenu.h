#pragma once

#include "IssWnd.h"
#include "IssKineticList.h"

enum EnumAdjustHeight
{
    ADJUST_Top,
    ADJUST_Bottom,
    ADJUST_None,
};

class CIssWndTouchMenu:public CIssWnd, public CIssKineticList
{
public:
    CIssWndTouchMenu();
    ~CIssWndTouchMenu();

    HRESULT         SetBackground(UINT uiBackGround);
    HRESULT	        PopupMenu(  HWND hWndParent, 
                                HINSTANCE hInst,
                                DWORD dwFlags,
                                int iX, int iY,
                                int iWidth, int iHeight,
                                int iXStart = 0, int iYStart = 0,
                                int iWidthStart = 0, int iHeightStart = 0,
                                EnumAdjustHeight eAdjust = ADJUST_None);
    BOOL            IsWindowUp(BOOL bCheckLastTimeUp = FALSE);
    HRESULT         PreloadImages(HWND hWnd, HINSTANCE hInstance);
    void            CloseWindow(); 
    CIssImageSliced& GetBackgroundSlice(){return m_imgBackground;};

protected:
    BOOL            OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	        OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	        OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL            OnLostFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnLButtonDown(HWND hWnd, POINT& pt); 
    BOOL            OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL            OnMouseMove(HWND hWnd, POINT& pt);
    BOOL            OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	        OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	        OnCaptureChanged(HWND hWnd, WPARAM wParam, LPARAM lParam);

    void	        DrawBackground(CIssGDIEx& gdiMem, RECT rcClient, RECT rcClip);
    BOOL            SelecteItem(int iIndex);
    void            Reset();

private:    // functions
    void            FadeInScreen(HDC hDC, RECT rcClient);
    void            AdjustWindow(int& iX, int& iY,
                                int& iWidth, int& iHeight,
                                EnumAdjustHeight eAdjust);

private:    // variables
    RECT                    m_rcStartLocation;  // start the initial animation from this location
    BOOL                    m_bFirstTime;       // first time loaded
    CIssGDIEx				m_gdiMem;
    CIssGDIEx				m_gdiBackground;
    CIssImageSliced         m_imgBackground;    // our stretched background image
      
};
