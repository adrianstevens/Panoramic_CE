// TodayRls.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "TodayRls.h"
#include <windows.h>
#include <commctrl.h>
#include "WndToday.h"
#include "DlgOptions.h"
#include <Todaycmn.h>

#define WND_TodayName		_T("TodayWindowChild")
#define WND_TodayClass		_T("ClassToday")

HINSTANCE	g_hInst;
CWndToday*	g_wndToday = 0;		// 0ur today window
CDlgOptions* g_dlgOptions = 0;

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CoInitializeEx(NULL, 0);
		g_hInst = (HINSTANCE)hModule;
		UnregisterClass(WND_TodayClass, g_hInst);//from http://www.developer.com/ws/pc/article.php/3484036

		// try creating windows here...
		//g_wndToday = new CWndToday;
		g_dlgOptions = new CDlgOptions;

        g_dlgOptions->SetInstance(g_hInst);

		break;
		//case DLL_THREAD_ATTACH:
		//	break;
		//case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		UnregisterClass(WND_TodayClass, g_hInst);
        if(g_wndToday)
        {
		    g_wndToday->DestroyHome();
            delete g_wndToday;
            g_wndToday = 0;
        }
		g_dlgOptions->Destroy();		
		delete g_dlgOptions;
		g_dlgOptions = 0;

		CoUninitialize();
		break;
	}
	return TRUE;
}

HWND APIENTRY InitializeCustomItem(TODAYLISTITEM *ptli, HWND hwndParent)
{
	//MessageBox(0, _T("InitializeCustomItem"), _T("APIENTRY"), MB_OK);

	UnregisterClass(WND_TodayClass, g_hInst);

    if(g_wndToday)
    {
        g_wndToday->DestroyHome();
        delete g_wndToday;
        g_wndToday = 0;
    }
    g_wndToday = new CWndToday;
	if(!g_wndToday->CreateHome(hwndParent, g_hInst))
		return NULL;

	return g_wndToday->GetWnd();
}		

BOOL WINAPI CustomItemOptionsDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam) 
//LRESULT APIENTRY CustomItemOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	//MessageBox(0, _T("CustomItemOptionsDlgProc"), _T("WINAPI"), MB_OK);

	if(message == WM_INITDIALOG)
    {
		lParam = (long)g_dlgOptions;
    }

	return g_dlgOptions->ProcWnd(hDlg, message, wParam, lParam);

	return 0;
}

TODAYRLS_API void LaunchOptions(HWND hWndParent)
{
    if(g_dlgOptions)
    {
        g_dlgOptions->DoModal(hWndParent, g_hInst);
        //Tell the plug-in to uninstall itself.
        ::SendMessage(HWND_BROADCAST, WM_WININICHANGE, 0xF2, 0);
    }
}
