#include "StdAfx.h"
#include "DlgSearch.h"
#include "Resource.h"

#define MENU_Height             46
#define COLOR_TEXT_NORMAL        RGB(255,255,255)
//#define COLOR_TEXT_SELECTED      RGB(149,223,49) //RGB(255,246,0) //RGB(119,175,57)

CDlgSearch::CDlgSearch(void)
{
}

CDlgSearch::~CDlgSearch(void)
{
}

BOOL CDlgSearch::PreloadImages(HWND hWnd, HINSTANCE hInst)
{
    m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_wndMenu.PreloadImages(hWnd, hInst);

    return TRUE;
}

BOOL CDlgSearch::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    RECT rc;
    GetClientRect(hWnd, &rc);

    return TRUE;
}

BOOL CDlgSearch::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(m_gdiMem.GetDC() == NULL || m_gdiMem.GetWidth() != WIDTH(rcClient) || m_gdiMem.GetHeight() != HEIGHT(rcClient))
        m_gdiMem.Create(hDC, rcClient, FALSE, TRUE, FALSE);

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    // draw the background
    FillRect(m_gdiMem, rcClip, RGB(0,0,0));

    m_wndMenu.OnDraw(m_gdiMem, rcClient, rcClip);

    BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem.GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);

    return TRUE;
}

BOOL CDlgSearch::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return UNHANDLED;
}

void CDlgSearch::InitMenuEntries()
{



}
