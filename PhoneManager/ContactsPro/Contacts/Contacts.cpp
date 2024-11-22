// Contacts.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Contacts.h"
#include <windows.h>
#include <commctrl.h>
#include "DlgContacts.h"


#define WND_ContactsApp		_T("Pano Programs")
#define WND_ContactsClass	_T("ClassPrograms")

HINSTANCE g_hStringResource;

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPTSTR    lpCmdLine,
				   int       nCmdShow)
{
	// ensure single instance....
    HWND hAppWnd = FindWindow(WND_ContactsClass, WND_ContactsApp);
	if (hAppWnd)
	{
		// our app is already running
        ShowWindow(hAppWnd, SW_SHOW);
        SetForegroundWindow(hAppWnd);

		// has command line - Send the command line in a WM_COPYDATA message.
        if (_tcslen(lpCmdLine) != 0) 
		{
			COPYDATASTRUCT cds;
			cds.cbData = _tcslen(lpCmdLine) * sizeof(TCHAR);
			cds.lpData = lpCmdLine;
			SendMessage(hAppWnd, WM_COPYDATA, 0, (LPARAM)&cds);
		}
        return FALSE;
	}

    // handle command line args
	EnumCurrentGui eSel = GUI_Favorites;
    if (_tcslen(lpCmdLine) != 0) 
    {
	    if(_tcscmp(lpCmdLine, _T("-c")) == 0)
            eSel = GUI_Contacts;
        else if(_tcscmp(lpCmdLine, _T("-f")) == 0)
            eSel = GUI_Favorites;
        else if(_tcscmp(lpCmdLine, _T("-h")) == 0)
            eSel = GUI_CallLog;
    }
    eSel = GUI_Contacts;
    
    UINT uiShow = SW_SHOW;

	CDlgContacts dlgContacts(eSel);

	if(dlgContacts.Create(WND_ContactsApp, NULL, hInstance, WND_ContactsClass, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, WS_VISIBLE))
	{
		ShowWindow(dlgContacts.GetWnd(), uiShow);

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