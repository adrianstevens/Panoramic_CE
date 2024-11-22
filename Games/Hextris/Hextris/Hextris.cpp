// Hextris.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Hextris.h"
#include "IssCommon.h"
#include "resource.h"
#include "Key.h"
#include "IssString.h"
#include <windows.h>
#include "DlgHextris.h"

HINSTANCE	g_hStringResource;
int			g_iDaysLeft = 0;

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	g_hStringResource = hInstance;

#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
	// see if we have to launch the eula first
	if(LaunchEula(REG_KEY))
		return 0;

	CKey key;

	// check if we're running a legit copy
	key.Init(_T("SOFTWARE\\Panoramic\\PanoHexeGems"), _T("SOFTWARE\\Pano\\PanoHexeGems2"), 0x0, 0x7B2E8E6D, 0x3E3A1FBE, 2, TRUE);
	key.CheckSetKey();
	key.CheckDemo();

	if(!key.m_fGood)
	{
		TCHAR szText[STRING_MAX];
		CIssString* oStr = CIssString::Instance();
		oStr->StringCopy(szText, IDS_DEMO_Over, STRING_MAX, hInstance);
		MessageBox(GetDesktopWindow(),szText, _T(""), MB_OK);
		return FALSE;
	}
	// set the daysleft for the menu (we don't display www.implicitsoftware.com on the menu)
	g_iDaysLeft	= (key.m_fDemo&&key.m_fGood ? key.GetDaysLeft(): 0);
#endif

	// NOTE: this speeds up the loading time HUGE
	HMODULE hDll = LoadLibrary(_T("imaging.dll"));

	CDlgHextris dlgHextris;

#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
	if(dlgHextris.Create(_T("HexeGems"), NULL, hInstance, _T("ClassHextris")))
	{
		ShowWindow(dlgHextris.GetWnd(), SW_SHOW);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
		FreeLibrary(hDll);
		return ( msg.wParam );
	}
#else
	if (FxIsRunning(hInstance))
	{
		//True = Instance is already running 
		// process command line params for already running instance
		FxBringToForeground(L"Hextris", lpCmdLine);
		return 0;
	}

//	g_hStringResource = LoadStringResources(hInstance);
//	if(NULL == g_hStringResource)
//		return 0;

	dlgHextris.Init(hInstance);
	LRESULT lRes = FxShowDialog( NULL, NULL, /*hInstance,*/ IDFX_MAIN_VIEW, dlgHextris.ProcWnd, (LPARAM)&dlgHextris);

	//FxUninitialize();
#endif

	FreeLibrary(hDll);
	return 0;
}

