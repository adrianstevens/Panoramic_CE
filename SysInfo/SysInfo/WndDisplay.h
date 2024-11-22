#pragma once
#include "wndinfoedit.h"

class CWndDisplay : public CWndInfoEdit
{
public:
	CWndDisplay(void);
	~CWndDisplay(void);

	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void		Refresh();

private:
	void		ShowDisplayInfo();
	void		ShowRawFrameBufferInfo();
};

