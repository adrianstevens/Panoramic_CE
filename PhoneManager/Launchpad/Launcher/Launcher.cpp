// Contacts.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Launcher.h"
#include <windows.h>
#include <commctrl.h>
#include "DlgLauncher.h"
#include "IssWndTouchMenu.h"
#include "IssString.h"
#include "ObjGui.h"
#include "DlgOptions.h"
#include "GuiToolBar.h"
#include "PoomContacts.h"
#include "ObjHardwareKeys.h"


#define WND_ContactsApp		_T("Launchpad")
#define WND_ContactsClass	_T("ClassLauncher")

int RunApp(UINT uiShow, HINSTANCE hInstance, EnumCurrentGui eSel);

CObjGui*			g_gui = NULL;
CIssWndTouchMenu*	g_wndMenu = NULL;
TypeOptions*		g_sOptions = NULL;		// options settings
CGuiToolBar*		g_guiToolbar;			// the toolbar

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
            ShowWindow(hAppWnd, SW_MINIMIZE);
            return FALSE;
        }

		// has command line - Send the command line in a WM_COPYDATA message.
		//if (_tcslen(lpCmdLine) != 0) 
		{
			COPYDATASTRUCT cds;
			cds.cbData = _tcslen(lpCmdLine) * sizeof(TCHAR);
			cds.lpData = lpCmdLine;
			SendMessage(hAppWnd, WM_COPYDATA, 0, (LPARAM)&cds);
		}

		// our app is already running
        ShowWindow(hAppWnd, SW_SHOW);
        SetForegroundWindow(hAppWnd);
		
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
    
	UINT uiShow = (_tcscmp(lpCmdLine,_T("-h"))==0 || GetTickCount() < 120000) ? SW_MINIMIZE :SW_SHOW; // should we start up as hidden



#ifdef DEBUG
	uiShow = SW_SHOW;
#endif

	g_sOptions	= new TypeOptions;
	g_wndMenu	= new CIssWndTouchMenu;
    g_gui		= new CObjGui;	
	g_guiToolbar= new CGuiToolBar;

	int iRet = RunApp(uiShow, hInstance, eSel);

	if(g_guiToolbar)
		delete g_guiToolbar;
    if(g_gui)
        delete g_gui;
	if(g_wndMenu)
		delete g_wndMenu;
	if(g_sOptions)
		delete g_sOptions;

	g_guiToolbar = NULL;
    g_gui = NULL;
	g_wndMenu = NULL;
	g_sOptions = NULL;

	CPoomContacts::DeleteInstance();
	CObjHardwareKeys::DeleteInstance();
	CIssString::DeleteInstance();


	return iRet;
}

int RunApp(UINT uiShow, HINSTANCE hInstance, EnumCurrentGui eSel)
{
	CDlgLauncher dlgLaunch/*(eSel)*/;

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