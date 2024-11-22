#pragma once
#include "wndinfoedit.h"

class CWndPower :
	public CWndInfoEdit
{
public:
	CWndPower(void);
	~CWndPower(void);

    BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void		Refresh();
};
