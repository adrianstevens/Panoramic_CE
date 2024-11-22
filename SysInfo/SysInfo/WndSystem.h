#pragma once
#include "wndinfoedit.h"

class CWndSystem :public CWndInfoEdit
{
public:
	CWndSystem(void);
	~CWndSystem(void);

	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void		Refresh();

private:
	void		ShowGetSystemInfo();
};
