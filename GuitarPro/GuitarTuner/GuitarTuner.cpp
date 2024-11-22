// guitartuner.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "guitartuner.h"
#include "Dlgguitartuner.h"
#include "IssCommon.h"
#include "resource.h"
#include "IssKey.h"
#include <windows.h>
#include <commctrl.h>

HINSTANCE g_hStringResource;
int			g_iDaysLeft = 0;

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	g_hStringResource = hInstance;

#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
	// see if we have to launch the eula first
	/*if(LaunchEula(REG_KEY))
		return 0;

	CIssKey key;

	// check if we're running a legit copy
	key.Init(_T("SOFTWARE\\Panoramic\\Panoguitartuner"), _T("SOFTWARE\\Panoramic\\Panoguitartuner2"), 0x0, 0x7B2E8E6E, 0x3E3A1FBE, 1, TRUE);
	key.CheckSetKey();
	key.CheckDemo();

	if(!key.m_bGood)
	{
		TCHAR szText[STRING_MAX];
		CIssString* oStr = CIssString::Instance();
		oStr->StringCopy(szText, IDS_DEMO_Over, STRING_MAX, hInstance);
		MessageBox(GetDesktopWindow(),szText, _T(""), MB_OK);
		return FALSE;
	}
	// set the daysleft for the menu (we don't display www.implicitsoftware.com on the menu)
	g_iDaysLeft	= (key.m_bDemo&&key.m_bGood ? key.GetDaysLeft(): 0);*/
#endif

	CDlgGuitarTuner dlgGuitarTuner;

#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
	//dlgguitartuner.DoModal(GetDesktopWindow(), hInstance, IDD_DLG_Basic);
	if(dlgGuitarTuner.Create(_T("Panoramic GuitarTuner"), NULL, hInstance, _T("ClassGuitarTuner")))
	{
		ShowWindow(dlgGuitarTuner.GetWnd(), SW_SHOW);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
		return ( msg.wParam );
	}
#endif
	return 0;
}

