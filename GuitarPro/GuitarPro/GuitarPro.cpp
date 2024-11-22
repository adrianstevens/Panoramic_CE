// guitarpro.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "GuitarPro.h"
#include "DlgGuitarPro.h"
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
CObjOptions*            g_options = NULL;

void RunApp(HINSTANCE hInstance);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
    DEVMODE devmode = {0};
    devmode.dmSize = sizeof(DEVMODE);

    devmode.dmFields = DM_DISPLAYORIENTATION;
    ChangeDisplaySettingsEx(NULL, &devmode, 0, CDS_TEST, NULL);

    if(devmode.dmDisplayOrientation != DMDO_DEFAULT && 
        GetSystemMetrics(SM_CXSCREEN) != GetSystemMetrics(SM_CYSCREEN) )
    {
        memset(&devmode, 0, sizeof(DEVMODE));
        devmode.dmSize = sizeof(DEVMODE);
        devmode.dmDisplayOrientation = DMDO_DEFAULT; 
        devmode.dmFields = DM_DISPLAYORIENTATION;
        ChangeDisplaySettingsEx(NULL, &devmode, NULL, 0, NULL);
    }

    //set the hinst for iSS String
    CIssString* oStr = CIssString::Instance();
    oStr->SetResourceInstance(hInstance);

    if(!g_gui)
        g_gui = new CObjGui;

    if(!g_options)
        g_options = new CObjOptions;

	RunApp(hInstance);

	if(g_gui)
		delete g_gui;
	g_gui = NULL;

    if(g_options)
        delete g_options;
    g_options = NULL;
	
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
    key.Init(_T("SOFTWARE\\Panoramic\\PanoGuitarSuite"), _T("SOFTWARE\\Pano\\PanoGuitarSuite2"), 0x0, 0x393EF73E, 0x94EE6A15, 1, TRUE);
	key.CheckSetKey();
	key.CheckDemo();

	if(!key.m_bGood)
	{
		TCHAR szText[STRING_MAX];
		CIssString* oStr = CIssString::Instance();
		oStr->StringCopy(szText, ID(IDS_MSG_DemoOver), STRING_MAX, hInstance);
		MessageBox(GetDesktopWindow(),szText, _T("Panoramic Guitar Suite"), MB_OK);
		return;
	}

	CDlgGuitarPro dlgGP;

	if(dlgGP.Create(_T("Panoramic Guitar Suite"), NULL, hInstance, _T("ClassGuitarSuite")))
	{
		ShowWindow(dlgGP.GetWnd(), SW_SHOW);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
		return;
	}

}

