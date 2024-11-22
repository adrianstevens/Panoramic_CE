// PhoneServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PhoneServer.h"
#include "DlgPhoneServer.h"
#include "DlgHiddenWnd.h"
#include <windows.h>
#include <commctrl.h>


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	HWND hWndParent = FindWindow(_T("HHTaskBar"), _T(""));
	//CDlgHiddenWnd dlgPhoneServer;
	CDlgPhoneServer dlgPhoneServer;

	if(dlgPhoneServer.Create(NULL, hWndParent, hInstance, _T("ClassPhoneServer"), 0,0,0,0, WS_EX_NOACTIVATE, WS_POPUP))
	//if(dlgPhoneServer.Create(_T("iSS PhoneServer"), NULL, hInstance, _T("ClassPhoneServer")))
	{
		ShowWindow(dlgPhoneServer.GetWnd(), SW_SHOW);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
		return ( msg.wParam );
	}
	return 0;
}

