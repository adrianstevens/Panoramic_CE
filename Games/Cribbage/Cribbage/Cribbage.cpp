// Cribbage.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Cribbage.h"
#include "DlgCribbage.h"
#include "IssCommon.h"
#include "resource.h"
#include "IssKey.h"
#include "IssString.h"
#include <windows.h>
#include "ObjGui.h"
#include "IssLocalisation.h"


HINSTANCE	g_hStringResource;
int			g_iDaysLeft;

CObjGui*                g_gui = NULL;

void RunApp(HINSTANCE hInstance);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
    if(!g_gui)
        g_gui = new CObjGui;

	RunApp(hInstance);

	if(g_gui)
		delete g_gui;
	g_gui = NULL;
	
	CIssKineticList::DeleteAllContent();
	CIssString::DeleteInstance();

	return 0;
}

void RunApp(HINSTANCE hInstance)
{
	// see if we have to launch the eula first
	g_hStringResource = hInstance;

	CIssKey key;

	// check if we're running a legit copy
	key.Init(_T("SOFTWARE\\Panoramic\\PanoCribbage"), _T("SOFTWARE\\Pano\\PanoCribbage2"), 0x0, 0x393EF71D, 0x94EE6A15, 1, TRUE);
	key.CheckSetKey();
	key.CheckDemo();

	if(!key.m_bGood)
	{
		TCHAR szText[STRING_MAX];
		CIssString* oStr = CIssString::Instance();
		oStr->StringCopy(szText, ID(IDS_MSG_DemoOver), STRING_MAX, hInstance);
		MessageBox(GetDesktopWindow(),szText, _T("Panoramic Cribbage"), MB_OK);
		return;
	}

	CDlgCribbage dlgCribbage;

	if(dlgCribbage.Create(_T("Panoramic Cribbage"), NULL, hInstance, _T("ClassCribbage")))
	{
		ShowWindow(dlgCribbage.GetWnd(), SW_SHOW);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
		return;
	}

}

