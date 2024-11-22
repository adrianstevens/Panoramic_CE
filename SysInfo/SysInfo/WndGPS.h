#pragma once
#include "wndinfoedit.h"
#include "IssGPS.h"

class CWndGPS : public CWndInfoEdit
{
public:
    CWndGPS(void);
    ~CWndGPS(void);

    BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void		Refresh();

private:

private:
    CIssGPS     m_oGPS;

};
