#pragma once
#include "wndinfoedit.h"

class CWndTime : public CWndInfoEdit
{
public:
	CWndTime(void);
	~CWndTime(void);

	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void		Refresh();

private:
	void		ShowTime();

};
