// HexDesk.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "HexDesk.h"
#include "IssString.h"
#include "DlgHextris.h"

#define _CRT_SECURE_NO_WARNINGS 1

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CDlgHextris dlgHextris;

	if(dlgHextris.Create(_T("Hexaminoes"), NULL, hInstance, _T("ClassHextris"), CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, WS_DLGFRAME | WS_THICKFRAME | WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | WS_SYSMENU))
{
		ShowWindow(dlgHextris.GetWnd(), SW_SHOW);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
		return (int)( msg.wParam );
	}

	return 0;
}






