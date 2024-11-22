// Contacts.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Launcher.h"
#include <windows.h>
#include <commctrl.h>
#include "DlgLauncher.h"
#include "IssHardwareKeys.h"
#include "IssString.h"
#include "ObjGui.h"


#define WND_ContactsApp		_T("Launchpad")
#define WND_ContactsClass	_T("ClassLauncher")

int RunApp(UINT uiShow, HINSTANCE hInstance, EnumCurrentGui eSel);

HINSTANCE g_hStringResource;
CObjGui*    g_gui = NULL;

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPTSTR    lpCmdLine,
				   int       nCmdShow)
{
	// ensure single instance....
    HWND hAppWnd = FindWindow(WND_ContactsClass, WND_ContactsApp);
	if (hAppWnd)
	{
        if(hAppWnd == GetForegroundWindow())
        {
            ShowWindow(hAppWnd, SW_HIDE);
            return FALSE;
        }

		// our app is already running
        ShowWindow(hAppWnd, SW_SHOW);
        SetForegroundWindow(hAppWnd);

		// has command line - Send the command line in a WM_COPYDATA message.
        //if (_tcslen(lpCmdLine) != 0) 
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
            eSel = GUI_Programs;
        else if(_tcscmp(lpCmdLine, _T("-f")) == 0)
            eSel = GUI_Favorites;
        else if(_tcscmp(lpCmdLine, _T("-h")) == 0)
            eSel = GUI_Settings;
    }
    
    UINT uiShow = (_tcslen(lpCmdLine) != 0) ? SW_SHOW : SW_HIDE;

#ifdef DEBUG
	uiShow = SW_SHOW;
#endif

    g_gui   = new CObjGui;

	int iRet = RunApp(uiShow, hInstance, eSel);

    if(g_gui)
        delete g_gui;
    g_gui = NULL;

	CIssHardwareKeys::DeleteInstance();
	CIssString::DeleteInstance();


	return iRet;
}

int RunApp(UINT uiShow, HINSTANCE hInstance, EnumCurrentGui eSel)
{
	CDlgLauncher dlgLaunch(eSel);

	if(dlgLaunch.Create(WND_ContactsApp, NULL, hInstance, WND_ContactsClass, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, WS_VISIBLE))
	{
		ShowWindow(dlgLaunch.GetWnd(), uiShow);

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