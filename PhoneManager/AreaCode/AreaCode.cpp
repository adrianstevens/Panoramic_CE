// DlgAreaCode.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DlgAreaCode.h"
#include "IssGDIEx.h"
#include "IssKey.h"
#include "DlgTrialOver.h"
#include "Resource.h"

#include <windows.h>
#include <commctrl.h>


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	
    CIssKey key;
#ifndef DEBUG
    // check if we're running a legit copy
    key.Init(_T("SOFTWARE\\Panoramic\\AreaCode"), _T("SOFTWARE\\Pano\\AC2"), 0x0, 0x9834CDEF, 0x23ABCEF1, 5, TRUE);
    key.CheckSetKey();
    key.CheckDemo();

    if(!key.m_bGood)
    {
        //MessageBox(GetDesktopWindow(), _T("Thank you for trying Area Code Genius.  Your trial period is now over.  Please return to the location that provided your download to purchase."), _T("Panoramic Area Code Genius"), MB_OK);
		
		CDlgTrialOver dlgTrialOver;
		dlgTrialOver.Initialize(IDS_APP_TITLE,
			IDS_MSG_TrialOverShort,
			IDS_MSG_DemoOver,
			IDS_Quit,
			IsVGA()?IDR_PNG_DemoIconsVGA:IDR_PNG_DemoIcons,
			IsVGA()?IDR_PNG_BuyVGA:IDR_PNG_Buy,
			IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray);

		dlgTrialOver.AddItem(0, _T("Area Code Genius"), _T("Purchase the full version"), _T("https://marketplace.windowsphone.com/details.aspx?appId=04288aaf-e168-46d5-ad11-393d92a0f1c9"));
		dlgTrialOver.AddItem(1, _T("Calc Pro"), _T("All-in-one Calculator"), _T("https://marketplace.windowsphone.com/details.aspx?appId=2fdd840f-f4c2-4040-b225-182a5fd97396"));
		dlgTrialOver.AddItem(2, _T("moTweets"), _T("Premiere Twitter client"), _T("https://marketplace.windowsphone.com/details.aspx?appId=82b80004-16b7-4e57-a4cd-bd9af2a66d38"));
		dlgTrialOver.AddItem(3, _T("Ultimate Sudoku"), _T("Sudoku, Kakuro, Ken-Ken, Killer Sudoku"), _T("https://marketplace.windowsphone.com/details.aspx?appId=02cef10b-6fff-44c0-8d60-eb2aa51fd70a"));

		dlgTrialOver.Launch(hInstance);
        return FALSE;
    }
#endif

    // save the current cursor
    HCURSOR hCurs = GetCursor();
    SetCursor(LoadCursor(NULL, IDC_WAIT));
	
    CDlgAreaCode dlgArea;
    CGuiBackground* guiBack = new CGuiBackground;
    CIssGDIEx* gdiMem = new CIssGDIEx;
        
    dlgArea.Init(gdiMem, guiBack, FALSE, TRUE);
    dlgArea.LoadImages(GetDesktopWindow(), hInstance);

    // restore cursor
    SetCursor(hCurs);

    if(dlgArea.Create(_T("Area Code"), NULL, hInstance, _T("ClassAreaCode"), 0, 0, 0, 0, 0, WS_VISIBLE))
    {
        ShowWindow(dlgArea.GetWnd(), SW_SHOW);

        MSG msg;
        while( GetMessage( &msg,NULL,0,0 ) )
        {
            TranslateMessage( &msg );
            DispatchMessage ( &msg );
        }
		delete gdiMem;
		delete guiBack;
        return ( msg.wParam );
    }

    delete gdiMem;
    delete guiBack;

	return 0;
}

