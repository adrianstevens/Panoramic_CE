// Today.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Today.h"
#include <windows.h>
#include <commctrl.h>
#include "DlgToday.h"

int RunApp(HINSTANCE hInstance);

HINSTANCE g_hStringResource;

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPTSTR    lpCmdLine,
				   int       nCmdShow)
{
	int iReturn = RunApp(hInstance);

	CPoomContacts* pPoom = CPoomContacts::Instance();
	pPoom->DeleteInstance();
	CIssString* oStr = CIssString::Instance();
	oStr->DeleteInstance();

	return iReturn;
}

int RunApp(HINSTANCE hInstance)
{
	CDlgToday dlgToday;

	if(dlgToday.Create(_T("Today Tester"), NULL, hInstance, _T("ClassToday")))
	{
		ShowWindow(dlgToday.GetWnd(), SW_SHOW);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) )
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
		return ( msg.wParam );
	}

	return 0;
}