#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"

class CDlgAddPassword : public CIssWnd
{
public:
    CDlgAddPassword(void);
    ~CDlgAddPassword(void);

    BOOL        OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);

    BOOL        SetName(TCHAR* szName, BOOL bAccessPoint);
    BOOL        GetKey(TCHAR* szKey);

private:

private:
    CIssGDIEx   m_gdiMem;

    TCHAR       m_szMessage[STRING_MAX];
    TCHAR       m_szKey[STRING_LARGE];

    HWND        m_hEditControl;
    HWND        m_hStatic;

    HFONT       m_hFont;

    RECT        m_rcTopMessage;
    RECT        m_rcEditLabel;
};
