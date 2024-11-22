#pragma once
#include "wndinfoedit.h"

class CWndCPU : public CWndInfoEdit
{
public:
	CWndCPU(void);
	~CWndCPU(void);

	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void		Refresh();

private:
	void		ShowGetSystemInfo();
	void		ShowCPUInfo(WORD wProcessorArchitecture, 
                    		WORD wProcessorLevel,
		                    WORD wProcessorRevision);
    float       ProcSpeedCalc();
};
