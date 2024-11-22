// Start.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Start.h"
#include "IssString.h"
#include "DlgStart.h"
#include "IssCommon.h"
#include <windows.h>
#include <commctrl.h>

HINSTANCE g_hStringResource;

#define CLASS_Start	_T("ClassLauncherStart")

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	g_hStringResource = hInstance;

	CDlgStart dlgStart;

	HWND hWndParent = NULL;
#if defined WIN32_PLATFORM_WFSP
	hWndParent = FindWindow(_T("Start"), _T(""));
#else if defined WIN32_PLATFORM_PSPC
	hWndParent = FindWindow(_T("HHTaskBar"), _T(""));
	if(!hWndParent)
		return 0;

	// see if there is another window running already
	HWND hWndPrevious = FindWindow(CLASS_Start, NULL);
	if(hWndPrevious)
	{
		return 0;
	}
#endif

	if(dlgStart.Create(NULL, hWndParent, hInstance, CLASS_Start, 0,0,0,0, WS_EX_NOACTIVATE, WS_POPUP))
	{
		ShowWindow(dlgStart.GetWnd(), SW_SHOW);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
		return ( msg.wParam );
	}

    dlgStart.ResetHHProc();
	return 1;
}

