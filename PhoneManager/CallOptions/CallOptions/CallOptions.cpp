// CallOptions.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CallOptions.h"
#include <windows.h>
#include <commctrl.h>
#include "DlgCallOptions.h"
#include "IssKey.h"


HINSTANCE g_hStringResource;

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPTSTR    lpCmdLine,
				   int       nCmdShow)
{
    CIssKey key;
#ifndef DEBUG
    // check if we're running a legit copy
    key.Init(_T("SOFTWARE\\Panoramic\\CallManager"), _T("SOFTWARE\\Pano\\PS2"), 0x0, 0x1234CDEF, 0x67ABCEF1, 5, TRUE);
    key.CheckSetKey();
    key.CheckDemo();

    if(!key.m_bGood)
    {
        MessageBox(GetDesktopWindow(), _T("Thank you for trying Call Genius.  Your trial period is now over.  Please return to the location that provided your download to purchase."), _T("Panoramic Call Genius"), MB_OK);
        return FALSE;
    }
#endif




	CDlgCallOptions dlgCallOptions;

    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    dlgCallOptions.PreloadImages(GetDesktopWindow(), hInstance);

    ::SetCursor(hCursor);

    dlgCallOptions.Create(_T("Call Options"), _T("CallClass"), hInstance);

	return 0;
}