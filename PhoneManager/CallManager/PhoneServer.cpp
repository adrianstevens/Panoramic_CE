// PhoneServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PhoneServer.h"
#include "DlgPhoneServer.h"
#include "IssString.h"
#include "PoomContacts.h"


#include <windows.h>
#include <commctrl.h>

int RunApp(HINSTANCE hInstance);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	// see if there is another window running already
    HWND hWndPrevious = FindWindow(_T("ClassPhoneServer"), NULL);
    if(hWndPrevious)
    {
        if(IDYES == MessageBox(NULL, _T("Would you like to close Panoramic Call Genius?"), _T("Call Genius"), MB_YESNO))
        {
            PostMessage(hWndPrevious, WM_CLOSE, 0,0);
            PostMessage(hWndPrevious, WM_QUIT, 0,0);
        }
        return 0;
    }

	int iRet = RunApp(hInstance);

	CIssString::DeleteInstance();
	CPoomContacts::DeleteInstance();

	return iRet;	
}

int RunApp(HINSTANCE hInstance)
{
	CDlgPhoneServer dlgPhoneServer;

	//HWND hWndParent = FindWindow(_T("HHTaskBar"), _T(""));

	//if(dlgPhoneServer.Create(NULL, hWndParent, hInstance, _T("ClassPhoneServer"), 0,0,0,0, WS_EX_NOACTIVATE, WS_POPUP))
   // if(dlgPhoneServer.Create(_T("PhoneServer"), NULL, hInstance, _T("ClassPhoneServer")))
    if(dlgPhoneServer.Create(_T("PhoneServer"), NULL, hInstance, _T("ClassPhoneServer"), 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, WS_VISIBLE))
	{
		ShowWindow(dlgPhoneServer.GetWnd(), SW_HIDE);

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

