#pragma once
#include "wndinfoedit.h"

class CWndMemory :
	public CWndInfoEdit
{
public:
	CWndMemory(void);
	~CWndMemory(void);

    BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void		Refresh();
};
