// CalcProPC.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CalcProPC.h"

#include "DlgCalcPro.h"
#include "IssCommon.h"
#include "resource.h"
#include "IssKey.h"
#include "IssString.h"
#include <windows.h>
#include "IssHardwareKeys.h"

#define CLASS_Name  _T("ClassCalcPro")


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
    // see if we have to launch the eula first
    if(LaunchEula(REG_KEY))
        return 0;
#endif

    // check for previous instances, don't care what the window name is
    HWND hWnd = FindWindow(CLASS_Name, NULL);
    
	if(hWnd)
    {
		ShowWindow(hWnd, SW_SHOW);
        SetForegroundWindow((HWND)((ULONG) (hWnd) | 0x00000001));
        return TRUE;
    }


	HMENU hMenu = NULL; // CreateMenu();
//	AppendMenu(hMenu, MF_STRING, IDMENU_Copy, _T("File"));


    CDlgCalcPro* dlgCalcPro = new CDlgCalcPro(hInstance);
    if(!dlgCalcPro)
        return FALSE;
	//if(dlgCalcPro->CreateChildWindow(hInstance, NULL, IDD_DLG_MODAL))
    if(dlgCalcPro->Create(_T("Calc Pro"), NULL, hInstance, CLASS_Name,
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		0x00000110,
		/*WS_VISIBLE | */ WS_CAPTION | DS_MODALFRAME |
		WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS |  
		0x0000c000,
		hMenu,
        IDI_CALCPRO))
		//WS_MAXIMIZEBOX DS_MODALFRAME | WS_THICKFRAME
    {
        SetWindowPos(dlgCalcPro->GetWnd(), NULL, 0, 0, 0 ,0, 0);
        ShowWindow(dlgCalcPro->GetWnd(), SW_SHOW);

		//if(_tcscmp(_T("-m"), lpCmdLine))
		{
		//	PostMessage(dlgCalcPro->GetWnd(), WM_SYSCOMMAND, SC_MINIMIZE, 0);
		//	dlgCalcPro->m_bStartMinimized = TRUE;//lazy but effective
		}

        MSG msg;
        while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
        {
            TranslateMessage( &msg );
            DispatchMessage ( &msg );
        }
    }

    delete dlgCalcPro;

    CIssKey key;

    // check if we're running a legit copy
  /*  key.Init(_T("SOFTWARE\\Panoramic\\PanoCalcPro"), _T("SOFTWARE\\Pano\\PCP2"), 0x0, 0x2B00B1E5, 0x6498ABC1, 4, TRUE, HKEY_CURRENT_USER);
    key.CheckSetKey();
    key.CheckDemo();

    if(!key.m_bGood)
    {
        TCHAR szText[STRING_MAX];
        CIssString* oStr = CIssString::Instance();
        oStr->StringCopy(szText, IDS_DEMO_Over, STRING_MAX, hInstance);
        MessageBox(GetDesktopWindow(),szText, _T("Panoramic Calc Pro"), MB_OK | MB_ICONASTERISK);
        return FALSE;
    }*/
    return 0;
}




