#include "IssWndTouchMenu.h"
#include "IssImageSliced.h"
#include "IssGDIFX.h"
#include "IssDebug.h"
#include "stdafx.h"

#define MENU_INDENT     GetSystemMetrics(SM_CXICON)/4

CIssWndTouchMenu::CIssWndTouchMenu()
:m_bFirstTime(FALSE)
{
    SetRect(&m_rcStartLocation, 0,0,0,0);

}

CIssWndTouchMenu::~CIssWndTouchMenu()
{
    Destroy();  // kill all the window stuff
}

BOOL CIssWndTouchMenu::IsWindowUp(BOOL bCheckLastTimeUp)
{
    if(m_hWnd)
        return TRUE;

    // also check if it was up a second ago
    if(bCheckLastTimeUp && GetTickCount()-m_dwTickLastUsed < 1000)
        return TRUE;

    return FALSE;
}

void CIssWndTouchMenu::Reset()
{
    CIssKineticList::Reset();
    m_bFirstTime        = TRUE;
}

HRESULT CIssWndTouchMenu::SetBackground(UINT uiBackGround)
{
    m_uiBackground = uiBackGround;
    m_gdiBackground.Destroy();
    m_imgBackground.Destroy();
    return S_OK;
}

HRESULT CIssWndTouchMenu::PreloadImages(HWND hWnd, HINSTANCE hInstance)
{
    HRESULT hr = S_OK;
    if(m_uiBackground != 0)
    {
        hr = m_imgBackground.Initialize(hWnd, hInstance, m_uiBackground);
        CHR(hr, _T("m_imgBackground.Initialize"));
    }
    hr = CIssKineticList::PreloadImages(hWnd, hInstance);
    CHR(hr, _T("PreloadImages"));

Error:
    return hr;
}

HRESULT CIssWndTouchMenu::PopupMenu(HWND hWndParent, 
                              HINSTANCE hInst,
                              DWORD dwFlags,
                              int iX, int iY,
                              int iWidth, int iHeight,
                              int iXStart, int iYStart,
                              int iWidthStart, int iHeightStart,
                              EnumAdjustHeight eAdjust)
{
    HRESULT hr = S_OK;
    if(m_hWnd && IsWindow(m_hWnd))
    {
        CloseWindow();
        return hr;
    }

    if(GetTickCount()-m_dwTickLastUsed < 500)
    {
        return E_FAIL;
    }

    Destroy();
    m_gdiBackground.Destroy();
    Reset();

    // make sure to fit the window properly
    AdjustWindow(iX, iY, iWidth, iHeight, eAdjust);

    m_rcStartLocation.left   = iXStart;
    m_rcStartLocation.top    = iYStart;
    m_rcStartLocation.right  = iXStart + iWidthStart;
    m_rcStartLocation.bottom = iYStart + iHeightStart;

    // clear off any messages first
    MSG stMsg = { 0 };
    while( PeekMessage( &stMsg, hWndParent, 0, 0, PM_REMOVE ))
    {
        TranslateMessage( &stMsg );
        DispatchMessage( &stMsg );
    }

    //BOOL bRet = CIssWnd::DoModal(hWndParent, hInst, IDD_DLG_Child);
    //CBHR(bRet, _T("CIssWnd::DoModal Failed"));
    BOOL bRet = CIssWnd::Create(_T("Menu"),  
                    hWndParent, hInst, 
                    _T("ClassMenu"),  
                    iX, iY, iWidth, iHeight, 
                    NULL,
                    WS_VISIBLE|WS_POPUP|WS_TABSTOP);
      CBHR(bRet, _T("CIssWnd::Create Failed"));

      // save the static functions here so we can reset them after we can initialize
      // because for the popup menu we call initialize last instead of first
      //DRAWITEM      pDrawFunc       = m_pDrawFunc;          // our custom draw function
      //DELETEITEM    pDeleteItemFunc = m_pDeleteItemFunc;	// function to delete our LPVOID item
      //LPVOID        lpClass         = m_lpClass;            // used with the custom draw function

     hr = CIssKineticList::Initialize(GetParent(m_hWnd), m_hWnd, m_hInst, dwFlags, TRUE);
     CHR(hr, _T("CIssKineticList::Initialize"));

     //m_pDrawFunc        = pDrawFunc;
     //m_pDeleteItemFunc  = pDeleteItemFunc;
     //m_lpClass          = lpClass;

     CIssKineticList::OnSize(MENU_INDENT,MENU_INDENT,iWidth - 2*MENU_INDENT, iHeight - 2*MENU_INDENT);

    SetWindowPos(m_hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
    EnableWindow(m_hWnd, TRUE);
    SetForegroundWindow(m_hWnd);
    SetFocus(m_hWnd);
    SetCapture(m_hWnd);

Error:
    return hr;
}

void CIssWndTouchMenu::AdjustWindow(int& iX, int& iY,
                                     int& iWidth, int& iHeight,
                                     EnumAdjustHeight eAdjust)
{
    if(eAdjust == ADJUST_None)
        return;

    int iMenuHeight = GetEndOfListYPos() + 2*GetIndent();

    if(iHeight <= iMenuHeight)
        return;     // nothing to adjust we're good

    if(eAdjust == ADJUST_Top)
    {
        iHeight = iMenuHeight;
    }
    else if(eAdjust == ADJUST_Bottom)
    {
        iY = iY + iHeight - iMenuHeight;
        iHeight = iMenuHeight;
    }
}

BOOL CIssWndTouchMenu::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    // force a redraw so the menu gets updated
    InvalidateRect(GetParent(hWnd), NULL, FALSE);
    UpdateWindow(GetParent(hWnd));

    m_bFirstTime = TRUE;
    return TRUE;
}

BOOL CIssWndTouchMenu::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    if((m_gdiMem.GetWidth() == WIDTH(rcClient) && m_gdiMem.GetHeight() == HEIGHT(rcClient)) ||
        WIDTH(rcClient) == 0 || HEIGHT(rcClient) == 0)
        return FALSE;

#ifdef UNDER_CE
    if(WIDTH(rcClient) != m_gdiMem.GetWidth() || HEIGHT(rcClient) != m_gdiMem.GetHeight())
    {
        // if we are changing orientation just close the window
        if(m_gdiMem.GetWidth() != 0 && m_gdiMem.GetHeight() != 0)
        {
            CloseWindow();
            return TRUE;
        }
    }
#endif
    m_gdiMem.Destroy();
    m_gdiBackground.Destroy();
    CIssKineticList::OnSize(MENU_INDENT,MENU_INDENT,WIDTH(rcClient) - 2*MENU_INDENT, HEIGHT(rcClient) - 2*MENU_INDENT);

    return TRUE;
}

BOOL CIssWndTouchMenu::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(m_gdiMem.GetDC() == NULL || WIDTH(rcClient) != m_gdiMem.GetWidth() || HEIGHT(rcClient) != m_gdiMem.GetHeight())
        m_gdiMem.Create(hDC, WIDTH(rcClient), HEIGHT(rcClient), FALSE, TRUE);

    RECT rcClip;
    GetClipBox(hDC, &rcClip); 

    DrawBackground(m_gdiMem, rcClient, rcClip);
    DrawEntries(m_gdiMem, rcClip);
    DrawScrollArrows(m_gdiMem, rcClip);
    DrawScrollBar(m_gdiMem, rcClip);  

    if(m_bFirstTime)
    {
        m_bFirstTime = FALSE;
        ShowSelectedItem();
    }

    BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem,
        rcClip.left,rcClip.top,
        SRCCOPY);

    return TRUE;
}

BOOL CIssWndTouchMenu::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return CIssKineticList::OnKeyDown(wParam, lParam);
}

BOOL CIssWndTouchMenu::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef UNDER_CE
    switch(wParam)
    {
    case VK_TTALK: 
    case VK_TEND:
    case VK_TSOFT1:
    case VK_TSOFT2:
        CloseWindow();
        return TRUE;
        break;
    }
#endif
    return TRUE;
}

BOOL CIssWndTouchMenu::OnLButtonDown(HWND hWnd, POINT& pt)
{
    return CIssKineticList::OnLButtonDown(pt);
}

BOOL CIssWndTouchMenu::OnLButtonUp(HWND hWnd, POINT& pt)
{
#ifdef UNDER_CE 
    return CIssKineticList::OnLButtonUp(pt);
#else
    if(CIssKineticList::OnLButtonUp(pt) == FALSE)
    {   //lets hide it
        HWND hWndParent = GetParent(m_hWnd);
        if(m_hWnd && IsWindow(m_hWnd))
        {
            Destroy();
        }
        m_lpClass       = NULL;
        m_pDrawFunc     = NULL;
        m_dwTickLastUsed= GetTickCount();

        InvalidateRect(hWndParent, NULL, FALSE);
    }
    

    return TRUE;
#endif
}

BOOL CIssWndTouchMenu::OnMouseMove(HWND hWnd, POINT& pt)
{
    //Set Capture Sucks ..
    if(pt.y > GetSystemMetrics(SM_CYSCREEN) || pt.y < 0)
        return UNHANDLED;

    return CIssKineticList::OnMouseMove(pt);
}

BOOL CIssWndTouchMenu::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return CIssKineticList::OnTimer(wParam, lParam);
}


void CIssWndTouchMenu::DrawBackground(CIssGDIEx& gdiMem, RECT rcClient, RECT rcClip)
{
    if(m_gdiBackground.GetDC() == NULL ||
       WIDTH(rcClient) != m_gdiBackground.GetWidth() ||
       HEIGHT(rcClient) != m_gdiBackground.GetHeight())
    {
        m_gdiBackground.Create(gdiMem.GetDC(), rcClient, FALSE, TRUE, FALSE);

        RECT rcWindow;
        GetWindowRect(m_hWnd, &rcWindow);

        // copy the background over
        HDC dc = GetDC(NULL);
        BitBlt(m_gdiBackground.GetDC(),
               0, 0,
               WIDTH(rcWindow), HEIGHT(rcWindow),
               dc,
               rcWindow.left, rcWindow.top,
               SRCCOPY);
        ReleaseDC(NULL, dc);

        if(!m_imgBackground.IsLoaded())
            m_imgBackground.Initialize(m_hWnd, m_hInst, m_uiBackground);
        if(m_imgBackground.GetWidth() != WIDTH(rcClient) || m_imgBackground.GetHeight() != HEIGHT(rcClient))
            m_imgBackground.SetSize(WIDTH(rcClient), HEIGHT(rcClient));
        m_imgBackground.DrawImage(m_gdiBackground, 0, 0);

        RECT rcBar;
        rcBar.left      = m_rcScrollUp.left;
        rcBar.right     = m_rcScrollUp.right;
        rcBar.top       = m_rcScrollUp.bottom;
        rcBar.bottom    = m_rcScrollDown.top;

        // if we draw the scroll bar we do it here because it will save on draw time
        /*if(m_dwFlags & OPTION_DrawScrollBar)
        {
            RECT rcBar;
            rcBar.left      = m_rcScrollUp.left;
            rcBar.right     = m_rcScrollUp.right;
            rcBar.top       = m_rcScrollUp.bottom;
            rcBar.bottom    = m_rcScrollDown.top;

            int iHeight = rcBar.top;
            while(iHeight < rcBar.bottom - m_gdiImageArray.GetHeight())
            {
                Draw(m_gdiBackground,
                     rcBar.left, iHeight,
                     m_gdiImageArray.GetHeight(), m_gdiImageArray.GetHeight(),
                     m_gdiImageArray,
                     IA_ScrollbarBackground*m_gdiImageArray.GetHeight(), 0);

                iHeight += m_gdiImageArray.GetHeight();
            }
            if(iHeight < rcBar.bottom)
            {
                Draw(m_gdiBackground,
                    rcBar.left, iHeight,
                    m_gdiImageArray.GetHeight(), rcBar.bottom - iHeight,
                    m_gdiImageArray,
                    IA_ScrollbarBackground*m_gdiImageArray.GetHeight(), 0);
            }
            
        }*/
    }

    BitBlt(gdiMem.GetDC(),
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiBackground.GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);
}

BOOL CIssWndTouchMenu::OnLostFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CloseWindow();
    return TRUE;
}
	
BOOL CIssWndTouchMenu::OnCaptureChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HWND hWndParent = NULL;
#ifdef UNDER_CE
    if(m_hWnd && IsWindow(m_hWnd))
    {
        
        hWndParent = GetParent(m_hWnd);
        //ReleaseCapture(); // we are not to do this - already done
        Destroy();
    }
    m_lpClass       = NULL;
    m_pDrawFunc     = NULL;
    m_dwTickLastUsed= GetTickCount();
#endif

    // force a full screen redraw of the parent window
    if(hWndParent)
        InvalidateRect(hWndParent, NULL, FALSE);

    return UNHANDLED;
}

BOOL CIssWndTouchMenu::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
    {
        SetCapture(m_hWnd);
        return TRUE;
    }
    else if(LOWORD(wParam) == WA_INACTIVE)
    {
#ifdef UNDER_CE
        CloseWindow();
#endif
    }
    return TRUE;
}

void CIssWndTouchMenu::CloseWindow()
{
    HWND hWndParent = NULL;
    if(m_hWnd && IsWindow(m_hWnd))
    {
        hWndParent = GetParent(m_hWnd);
        ReleaseCapture();

     /*   MSG stMsg = { 0 };
        while( PeekMessage( &stMsg, m_hWnd, 0, 0, PM_REMOVE ))
        {
            TranslateMessage( &stMsg );
            DispatchMessage( &stMsg );
        }	*/
        Destroy();
    }
    m_lpClass       = NULL;
    m_pDrawFunc     = NULL;
    m_dwTickLastUsed= GetTickCount();

    // force a full screen redraw of the parent window
    if(hWndParent)
    {
    //    EnableWindow(hWndParent, TRUE);
    //    SetForegroundWindow(hWndParent);
    //    SetFocus(hWndParent);
   //     SetActiveWindow(hWndParent);     
        InvalidateRect(hWndParent, NULL, FALSE);
    }

    
}

void CIssWndTouchMenu::FadeInScreen(HDC hDC, RECT rcClient)
{
    #define NUM_ANI_FRAMES			1000	//aribitrary as the animation control code will dictate actual frames
    #define TOTAL_ANIMATION_TIME	300

    CIssGDIEx gdiNew, gdiBack;
    gdiNew.Create(m_gdiMem, m_rcLoc, TRUE, TRUE);
    gdiBack.Create(hDC, m_rcLoc, TRUE, TRUE);

    if(!m_imgBackground.IsLoaded())
        m_imgBackground.Initialize(m_hWnd, m_hInst, m_uiBackground);

    RECT rcEnd = m_rcLoc;
    RECT rcStart, rcCurrent;
    POINT ptTL, ptTR, ptBL, ptBR;

    rcStart = m_rcStartLocation;
    if(WIDTH(m_rcStartLocation) == 0 || HEIGHT(m_rcStartLocation) == 0)
    {
        rcStart.left  = rcEnd.left + WIDTH(rcEnd)/2;
        rcStart.top   = rcEnd.top + HEIGHT(rcEnd)/2;
        rcStart.right = rcStart.left + 1;
        rcStart.bottom= rcStart.top + 1;
    }

    DWORD dwCurTime = GetTickCount();
    //we'll do the work here ... of course we'll render the fades first as they go below the focus window
    int i = GetCurrentFrame(dwCurTime, NUM_ANI_FRAMES, TOTAL_ANIMATION_TIME);

    int iHalf = NUM_ANI_FRAMES/2;

    while(i < NUM_ANI_FRAMES)
    {
        rcCurrent.left  = rcStart.left - (rcStart.left-rcEnd.left)*i/NUM_ANI_FRAMES;
        rcCurrent.top   = rcStart.top - (rcStart.top-rcEnd.top)*i/NUM_ANI_FRAMES;
        rcCurrent.right = rcStart.right + (rcEnd.right - rcStart.right)*i/NUM_ANI_FRAMES;
        rcCurrent.bottom= rcStart.bottom + (rcEnd.bottom - rcStart.bottom)*i/NUM_ANI_FRAMES;

        //FillRect(m_gdiMem, rcCurrent, TRANSPARENT_COLOR);
        BitBlt(m_gdiMem.GetDC(),
               rcCurrent.left, rcCurrent.top,
               WIDTH(rcCurrent), HEIGHT(rcCurrent),
               gdiBack.GetDC(),
               rcCurrent.left, rcCurrent.top,
               SRCCOPY);

        /*if(m_dwFlags & OPTION_AnimateGenieDown)
        {
            ptBL.x      = rcCurrent.left;
            ptBL.y      = rcCurrent.bottom;
            ptBR.x      = rcCurrent.right;
            ptBR.y      = rcCurrent.bottom;
            if(i<iHalf)
            {
                ptTL.x      = rcStart.left;
                ptTL.y      = rcStart.top;
                ptTR.x      = rcStart.right;
                ptTR.y      = rcStart.top;
            }
            else
            {
                ptTL.x      = rcStart.left - (rcStart.left-rcEnd.left)*(i-iHalf)/iHalf;
                ptTL.y      = rcStart.top + (rcEnd.top - rcStart.top)*(i-iHalf)/iHalf;
                ptTR.x      = rcStart.right + (rcEnd.right - rcStart.right)*(i-iHalf)/iHalf;
                ptTR.y      = ptTL.y;
            }
        }
        else if(m_dwFlags & OPTION_AnimateGenieUp)
        {
            ptTL.x      = rcCurrent.left;
            ptTL.y      = rcCurrent.top;
            ptTR.x      = rcCurrent.right;
            ptTR.y      = rcCurrent.top;
            if(i<iHalf)
            {
                ptBL.x      = rcStart.left;
                ptBL.y      = rcStart.bottom;
                ptBR.x      = rcStart.right;
                ptBR.y      = rcStart.bottom;
            }
            else
            {
                ptBL.x      = rcStart.left - (rcStart.left-rcEnd.left)*(i-iHalf)/iHalf;
                ptBL.y      = rcStart.bottom + (rcEnd.bottom - rcStart.bottom)*(i-iHalf)/iHalf;
                ptBR.x      = rcStart.right + (rcEnd.right - rcStart.right)*(i-iHalf)/iHalf;
                ptBR.y      = ptBL.y;
            }
        }
        else*/
        {
            ptTL.x      = rcCurrent.left;
            ptTL.y      = rcCurrent.top;
            ptTR.x      = rcCurrent.right;
            ptTR.y      = rcCurrent.top;
            ptBL.x      = rcCurrent.left;
            ptBL.y      = rcCurrent.bottom;
            ptBR.x      = rcCurrent.right;
            ptBR.y      = rcCurrent.bottom;
        }      

     
        Trapazoid(m_gdiMem, gdiNew, ptTL, ptTR, ptBL, ptBR);

        /*TransparentBlt(hDC,
            rcCurrent.left, rcCurrent.top,
            WIDTH(rcCurrent), HEIGHT(rcCurrent),
            m_gdiMem,
            rcCurrent.left, rcCurrent.top,
            WIDTH(rcCurrent), HEIGHT(rcCurrent),
            TRANSPARENT_COLOR);*/
        BitBlt(hDC,
            rcCurrent.left, rcCurrent.top,
            WIDTH(rcCurrent), HEIGHT(rcCurrent),
            m_gdiMem.GetDC(),
            rcCurrent.left, rcCurrent.top,
            SRCCOPY);

        i = GetCurrentFrame(dwCurTime, NUM_ANI_FRAMES, TOTAL_ANIMATION_TIME);
        //i+=100;
    }

    /*BitBlt(m_gdiMem,
        0, 0,
        WIDTH(rcClient), HEIGHT(rcClient),
        gdiBack,
        0, 0,
        SRCCOPY);*/
}

BOOL CIssWndTouchMenu::SelecteItem(int iIndex)
{
    if(!CIssKineticList::SelecteItem(iIndex))
        return FALSE;

    // make a graceful close of this window
    CloseWindow();

    return TRUE;
}