// CalcPro.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CalcPro.h"
#include "DlgCalcPro.h"
#include "IssCommon.h"
#include "resource.h"
#include "IssKey.h"
#include "IssString.h"
#include <windows.h>
#include "IssHardwareKeys.h"

#define CLASS_Name  _T("ClassCalcProLite")

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
    // see if we have to launch the eula first
    /*if(LaunchEula(REG_KEY))
        return 0;*/
#endif

    // check for previous instances, don't care what the window name is
    HWND hWnd = FindWindow(CLASS_Name, NULL);
    if(hWnd)
    {
        SetForegroundWindow((HWND)((ULONG) (hWnd) | 0x00000001));
        return TRUE;
    }
    
	CDlgCalcPro* dlgCalcPro = new CDlgCalcPro(hInstance);
	if(!dlgCalcPro)
		return FALSE;
	if(dlgCalcPro->Create(_T("Calc Pro Lite"), NULL, hInstance, CLASS_Name))
	{
		ShowWindow(dlgCalcPro->GetWnd(), SW_SHOW);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
	}

	delete dlgCalcPro;
	/*CIssHardwareKeys* oKeys = CIssHardwareKeys::Instance();
	oKeys->DeleteInstance();
	
	CIssCalculator* oCalc = CIssCalculator::Instance();
	oCalc->DeleteInstance();
	CIssFormatCalcDisplay* oDisplay = CIssFormatCalcDisplay::Instance();
	oDisplay->DeleteInstance();*/

    CIssKey key;

    // check if we're running a legit copy
    key.Init(_T("SOFTWARE\\Panoramic\\PanoCalcProLite"), _T("SOFTWARE\\Pano\\PCPL2"), 0x0, 0x2B00B1ED, 0x6498ABC1, 4, TRUE);
    key.CheckSetKey();
    key.CheckDemo();

    if(!key.m_bGood)
    {
        TCHAR szText[STRING_MAX];
        CIssString* oStr = CIssString::Instance();
        oStr->StringCopy(szText, IDS_DEMO_Over, STRING_MAX, hInstance);
        MessageBox(GetDesktopWindow(),szText, _T("Panoramic Calc Pro"), MB_OK | MB_ICONASTERISK);
        return FALSE;
    }
	return 0;
}

