// WifiHero.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WifiHero.h"
#include <windows.h>
#include <commctrl.h>
#include "DlgWifiHero.h"
#include "IssKey.h"

#define REG_KEY		        _T("SOFTWARE\\Pano\\WiFiHero")

HINSTANCE g_hStringResource;

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPTSTR    lpCmdLine,
				   int       nCmdShow)
{
    /*if(LaunchEula(REG_KEY))
        return 0;*/

	CDlgWifiHero dlgWifiHero;

	if(dlgWifiHero.Create(_T("Wifi Hero"), NULL, hInstance, _T("ClassWifiHero")))
	{
		ShowWindow(dlgWifiHero.GetWnd(), SW_SHOW);

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