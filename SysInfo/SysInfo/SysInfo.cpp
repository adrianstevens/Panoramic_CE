// SysInfo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SysInfo.h"
#include "DlgSysInfo.h"
#include "IssCommon.h"
#include "resource.h"
#include "IssString.h"
#include <windows.h>

HINSTANCE g_hStringResource;

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
	// see if we have to launch the eula first
	g_hStringResource = hInstance;
	
/*	if(LaunchEula(REG_KEY))
		return 0;

	CKey key;

	// check if we're running a legit copy
	key.Init(_T("SOFTWARE\\iSSTurboSolitaire"), REG_KEY, 0x0, 0x393EF71D, 0x94EE6A15, 4);
	key.CheckSetKey();
	key.CheckDemo();

	if(!key.m_fGood)
	{
		TCHAR szText[STRING_MAX];
		CIssString* oStr = CIssString::Instance();
		oStr->StringCopy(szText, IDS_DEMO_Over, STRING_MAX, hInstance);
		MessageBox(GetDesktopWindow(),szText, _T(""), MB_OK);
		return FALSE;
	}*/
#endif

	CDlgSysInfo dlg;

#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
	if(dlg.Create(_T("Mobile System Info"), NULL, hInstance, _T("MobileSystemInfo")))
	{
		ShowWindow(dlg.GetWnd(), SW_SHOW);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
		return ( msg.wParam );
	}
#else
	//FxInitialize(hInstance);

	if(!FxSingleInstanceApp())
		return 0;

	dlgTurboSolitaire.Init(hInstance);
	LRESULT lRes = FxShowDialog( NULL, NULL, /*hInstance,*/ IDFX_MAIN_VIEW, dlgTurboSolitaire.ProcWnd, (LPARAM)&dlgTurboSolitaire);

	//FxUninitialize();
#endif

	return 0;
}

