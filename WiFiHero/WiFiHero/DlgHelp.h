#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"


class CDlgHelp :
    public CIssWnd
{
public:
    CDlgHelp(void);
    ~CDlgHelp(void);

    BOOL        OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:

private:
    HFONT       m_hFont;

    HWND        m_hWndEdit;

};
