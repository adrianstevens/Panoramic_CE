#pragma once
#include "wndinfoedit.h"

#define MARRAY_SIZE 30

class CWndTest :
    public CWndInfoEdit //we don't need the list but since this works we'll keep it
{
public:
    CWndTest(void);
    ~CWndTest(void);

    BOOL    OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL    OnMouseMove(HWND hWnd, POINT& pt);
    BOOL    OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
    BOOL    DrawMouseTrail(HDC hDC);
//    BOOL    DrawText(HDC hDC);

private:
    POINT   m_ptMouseArray[MARRAY_SIZE];//should be big enough
    int     m_iMouseIndex;
    int     m_iMouseCount;

    DWORD   m_dwCharWParam;
    DWORD   m_dwCharLParam;

    DWORD   m_dwKeyDownWParam;
    DWORD   m_dwKeyDownLParam;

    DWORD   m_dwKeyUpWParam;
    DWORD   m_dwKeyUpLParam;

    HFONT   m_hFont;
};
