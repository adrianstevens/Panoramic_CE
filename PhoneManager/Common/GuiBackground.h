#pragma once

#include "IssGDIEx.h"
#include "IssCommon.h"

#define INDENT                          (GetSystemMetrics(SM_CXSMICON)/4)

#define SLICE_Height                    (IsVGA()?10:5)
#define BAR_TOP_Height                  (GetSystemMetrics(SM_CXICON)/2+SLICE_Height)
#define BAR_BOTTOM_Height               (GetSystemMetrics(SM_CXICON)*3/4)


class CGuiBackground
{
public:
    CGuiBackground(void);
    ~CGuiBackground(void);

    HRESULT     Init(HWND hWnd, HINSTANCE hInst);
    HRESULT     PreloadImages(BOOL bFullScreen = TRUE);
    void        Destroy();
    void        DrawBackground(CIssGDIEx& gdi, RECT& rcClip, RECT& rcClient, int iAdjustY = 0);
    void        DrawTopSlice(CIssGDIEx& gdi, RECT& rcClip, RECT& rcClient, int iAdjustY = 0);
    void        DrawTopBar(CIssGDIEx& gdi, RECT& rcClip, RECT& rcClient);
    void        DrawBottomBar(CIssGDIEx& gdi, RECT& rcClip, RECT& rcClient);
    CIssGDIEx&  GetBackground(){return m_gdiBackground;};
    CIssGDIEx&  GetTopSlice(){return m_gdiTopSlice;};
    CIssGDIEx&  GetTopBar(){return m_gdiTopBar;};
    CIssGDIEx&  GetBottomBar(){return m_gdiBottomBar;};
    RECT&       GetTopBarRect(){return m_rcTopBar;};
    RECT&       GetTopSliceRect(){return m_rcTopSlice;};
    RECT&       GetBottomBarRect(){return m_rcBottomBar;};
    int         GetSideSliceWidth();
    int         GetSliceHeight(){return SLICE_Height;};

private:    // functions
    void        InitBackground(HDC hDC, RECT rcClient);
    void        InitBackground(HDC hDC);
    void        InitTopBar(HDC hDC);
    void        InitBottomBar(HDC hDC, BOOL bFullScreen = TRUE);

private:    // variables
    HWND        m_hWnd;
    HINSTANCE   m_hInst;
    CIssGDIEx   m_gdiBackground;
    CIssGDIEx   m_gdiTopSlice;
    CIssGDIEx   m_gdiTopBar;
    CIssGDIEx   m_gdiBottomBar;
    RECT        m_rcTopBar;
    RECT        m_rcTopSlice;
    RECT        m_rcBottomBar;
   
};
