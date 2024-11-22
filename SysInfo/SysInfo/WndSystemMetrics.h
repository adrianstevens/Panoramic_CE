#pragma once
#include "wndinfoedit.h"

class CWndSystemMetrics :
	public CWndInfoEdit
{
public:
	CWndSystemMetrics(void);
	~CWndSystemMetrics(void);

	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void		Refresh();

private:
	void		ShowGetSystemMetrics();
};
