#include "GuiBackground.h"
#include "IssGDIDraw.h"
#include "issdebug.h"
#include "IssCommon.h"
#include "IssImageSliced.h"
#include "Resource.h"
#include "ContactsGuiDefines.h"

CGuiBackground::CGuiBackground(void)
:m_hWnd(NULL)
,m_hInst(NULL)
{
    ZeroMemory(&m_rcTopBar, sizeof(RECT));
    ZeroMemory(&m_rcTopSlice, sizeof(RECT));
    ZeroMemory(&m_rcBottomBar, sizeof(RECT));
}

CGuiBackground::~CGuiBackground(void)
{
    Destroy();
}

void CGuiBackground::Destroy()
{
    m_gdiBackground.Destroy();
    m_gdiTopSlice.Destroy();
    m_gdiTopBar.Destroy();
    m_gdiBottomBar.Destroy();
    ZeroMemory(&m_rcTopBar, sizeof(RECT));
    ZeroMemory(&m_rcTopSlice, sizeof(RECT));
    ZeroMemory(&m_rcBottomBar, sizeof(RECT));
}

HRESULT CGuiBackground::Init(HWND hWnd, HINSTANCE hInst)
{
    HRESULT hr = S_OK;
    CBARG(hWnd && hInst, _T(""));
    m_hWnd = hWnd;
    m_hInst= hInst;

Error:
    return hr;
}

HRESULT CGuiBackground::PreloadImages(BOOL bFullScreen /* = TRUE */)
{
    HRESULT hr = S_OK;
    CBARG(m_hWnd && m_hInst, _T(""));

    Destroy();

    HDC dc = GetDC(m_hWnd);
    InitBackground(dc);
    InitTopBar(dc);
    InitBottomBar(dc, bFullScreen);
    ReleaseDC(m_hWnd, dc);

Error:
    return hr;
}

void CGuiBackground::DrawBackground(CIssGDIEx& gdi, RECT& rcClip, RECT& rcClient, int iAdjustY)
{
    if(m_gdiBackground.GetDC() == NULL || m_gdiBackground.GetWidth() != WIDTH(rcClient))
        InitBackground(gdi, rcClient);


    BitBlt(gdi,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiBackground.GetDC(),
        rcClip.left,rcClip.top-iAdjustY,
        SRCCOPY);
}

void CGuiBackground::DrawTopSlice(CIssGDIEx& gdi, RECT& rcClip, RECT& rcClient, int iAdjustY)
{
    if(m_gdiTopSlice.GetDC() == NULL || m_gdiTopSlice.GetWidth() != WIDTH(rcClient))
        InitBackground(gdi);
    RECT rcTopSlice = m_rcTopSlice;
    rcTopSlice.top += iAdjustY;
    rcTopSlice.bottom += iAdjustY;
    if(!IsRectInRect(rcClip, rcTopSlice))
        return;

    //Draw(gdi, m_rcTopSlice, m_gdiTopSlice, 0,0);

    //RECT rcDraw;
    //IntersectRect(&rcDraw,&rcTopSlice,&rcClip);
    Draw(gdi, rcTopSlice, m_gdiTopSlice, 0,0);
}

void CGuiBackground::DrawTopBar(CIssGDIEx& gdi, RECT& rcClip, RECT& rcClient)
{
    if(m_gdiTopBar.GetDC() == NULL || m_gdiTopBar.GetWidth() != WIDTH(rcClient))
        InitTopBar(gdi);
    if(!IsRectInRect(rcClip, m_rcTopBar))
        return;

    BitBlt(gdi,
           m_rcTopBar.left, m_rcTopBar.top,
           WIDTH(m_rcTopBar), HEIGHT(m_rcTopBar),
           m_gdiTopBar,
           0,0,
           SRCCOPY);
}

void CGuiBackground::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClip, RECT& rcClient)
{
    if(m_gdiBottomBar.GetDC() == NULL || m_gdiBottomBar.GetWidth() != WIDTH(rcClient))
        InitBottomBar(gdi);
    if(!IsRectInRect(rcClip, m_rcBottomBar))
        return;

    BitBlt(gdi,
        m_rcBottomBar.left, m_rcBottomBar.top,
        WIDTH(m_rcBottomBar), HEIGHT(m_rcBottomBar),
        m_gdiBottomBar,
        0,0,
        SRCCOPY);
}

void CGuiBackground::InitBackground(HDC hDC, RECT rcClient)
{
    int iWidth = WIDTH(rcClient);
    int iHeight = HEIGHT(rcClient);

    m_gdiBackground.Create(hDC, iWidth, iHeight, FALSE, TRUE);
    m_gdiBackground.InitAlpha(FALSE);

    // draw the background
    RECT rc = {0};
    rc.bottom   = iHeight;
    rc.right    = iWidth*7/10;
    GradientFillRect(m_gdiBackground, rc, RGB(102,102,102), RGB(0,0,0), FALSE);
    rc.left     = rc.right;
    rc.right    = iWidth;
    GradientFillRect(m_gdiBackground, rc, RGB(106,106,106), RGB(12,12,12), FALSE);

    CIssImageSliced imgBorder;
    imgBorder.Initialize(m_hWnd, m_hInst, IsVGA() ? IDR_PNG_ContactsBorderVGA : IDR_PNG_ContactsBorder);
    imgBorder.SetSize(iWidth, iHeight);

    // draw the border minus the top slice
    /*Draw(m_gdiBackground,
    0,SLICE_Height,
    iWidth, iHeight-SLICE_Height,
    imgBorder.GetImage(),
    0, SLICE_Height);*/
    Draw(m_gdiBackground,
        0,0,
        iWidth, iHeight,
        imgBorder.GetImage(),
        0, 0);

    m_gdiTopSlice.Create(hDC, iWidth, SLICE_Height, FALSE, TRUE);
    m_gdiTopSlice.InitAlpha(TRUE);

    // draw the top slice
    Draw(m_gdiTopSlice,
        0,0,
        iWidth, SLICE_Height,
        imgBorder.GetImage(),
        0,0,
        ALPHA_Copy);  
    SetRect(&m_rcTopSlice, 0, 0, iWidth, SLICE_Height);
}

void CGuiBackground::InitBackground(HDC hDC)
{
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = GetSystemMetrics(SM_CXSCREEN);
    rc.bottom = GetSystemMetrics(SM_CYSCREEN);

    InitBackground(hDC, rc);
}

void CGuiBackground::InitTopBar(HDC hDC)
{
    int iWidth = GetSystemMetrics(SM_CXSCREEN);
    int iHeight= BAR_TOP_Height-SLICE_Height;
    
    m_gdiTopBar.Create(hDC, iWidth, iHeight, FALSE, TRUE);
        
    RECT rc = {0};
    rc.right    = iWidth;
    rc.bottom   = iHeight/3;
    GradientFillRect(m_gdiTopBar, rc, RGB(237,245,248), RGB(212,218,220), FALSE);
    rc.top      = rc.bottom;
    rc.bottom   = iHeight;
    GradientFillRect(m_gdiTopBar, rc, RGB(182,186,188), RGB(150,152,152), FALSE);
    //Line(m_gdiTopBar, 0, rc.bottom, iWidth, rc.bottom, RGB(88,88,88));
    //Line(m_gdiTopBar, 0, rc.bottom+1, iWidth, rc.bottom+1, RGB(141,141,141)); 
    SetRect(&m_rcTopBar, 0, 0, iWidth, iHeight /*+ 1*/);
}

void CGuiBackground::InitBottomBar(HDC hDC, BOOL bFullScreen /* = TRUE */)
{
    int iWidth = GetSystemMetrics(SM_CXSCREEN);
    int iHeight= GetSystemMetrics(SM_CYSCREEN);

    if(bFullScreen == FALSE)
    {
        HWND hWndTaskbar= FindWindow(_T("HHTaskBar"), NULL);

        RECT rcTaskBar;

        int iMenu = 0;
        if(hWndTaskbar)
        {
            GetWindowRect(hWndTaskbar,&rcTaskBar);
            iMenu = HEIGHT(rcTaskBar);
        }

        if(iMenu == 0)
        {
            switch(GetSystemMetrics(SM_CXICON))
            {
            case 32:
                iMenu = 26;
                break;
            case 44:
                iMenu = 35;
                break;
            case 64:
                iMenu = 52;
                break;
            default:
                iMenu = GetSystemMetrics(SM_CXICON)*26/32;
                break;
            }
        }
        iHeight -= iMenu;
    }

    m_gdiBottomBar.Create(hDC, iWidth, BAR_BOTTOM_Height, FALSE, TRUE);

    RECT rc = {0};
    rc.right    = iWidth;
    rc.bottom   = rc.top + BAR_BOTTOM_Height/3;
    GradientFillRect(m_gdiBottomBar, rc, COLOR_TOOLBAR_GRAD2_END, COLOR_TOOLBAR_GRAD2_ST, FALSE);
    rc.top      = rc.bottom;
    rc.bottom   = BAR_BOTTOM_Height;
    GradientFillRect(m_gdiBottomBar, rc, COLOR_TOOLBAR_GRAD1_END, COLOR_TOOLBAR_GRAD1_ST, FALSE);
    Line(m_gdiBottomBar, iWidth/2 - 1, BAR_BOTTOM_Height/8, iWidth/2 - 1, rc.bottom - BAR_BOTTOM_Height/8, RGB(65,65,65));
    Line(m_gdiBottomBar, iWidth/2, BAR_BOTTOM_Height/8, iWidth/2, rc.bottom - BAR_BOTTOM_Height/8, RGB(150,152,151)); 
    SetRect(&m_rcBottomBar, 0,  iHeight - BAR_BOTTOM_Height, iWidth, iHeight);    
}

int CGuiBackground::GetSideSliceWidth()
{
   return IsVGA()?10:5;
}
