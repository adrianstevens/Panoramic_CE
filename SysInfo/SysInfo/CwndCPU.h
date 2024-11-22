#pragma once
#include "wndinfoedit.h"

class CwndCPU :	public CWndInfoEdit
{
public:
	CwndCPU(void);
	~CwndCPU(void);

	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void		Refresh();

private:
	void		ShowGetSystemInfo();
	void		ShowCPUInfo(WORD wProcessorArchitecture, 
							WORD wProcessorLevel,
							WORD wProcessorRevision);
};
