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
#include "IssLocalisation.h"
#include "DlgTrialOver.h"


#define CLASS_Name  _T("ClassCalcPro")

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{

    // check for previous instances, don't care what the window name is
    HWND hWnd = FindWindow(CLASS_Name, NULL);
    if(hWnd)
    {
        SetForegroundWindow((HWND)((ULONG) (hWnd) | 0x00000001));
        return TRUE;
    }


    CIssKey key;

    // check if we're running a legit copy
    key.Init(_T("SOFTWARE\\Panoramic\\PanoCalcPro"), _T("SOFTWARE\\Pano\\PCP2"), 0x0, 0x2B00B1E5, 0x6498ABC1, 4, TRUE);
    key.CheckSetKey();
    key.CheckDemo();


#ifndef NMA
    if(!key.m_bGood)
    {
        CDlgTrialOver dlgTrialOver;
        dlgTrialOver.Initialize(IDS_APP_TITLE,
            ID(IDS_MSG_DemoOverShort),
            ID(IDS_MSG_DemoOver),
            ID(IDS_MENU_Exit),
            IsVGA()?IDR_PNG_IconsVGA:IDR_PNG_Icons,
            IsVGA()?IDR_PNG_BuyButtonVGA:IDR_PNG_BuyButton,
            IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray);

        dlgTrialOver.AddItem(0, _T("Calc Pro"), _T("The power of 10 complete calculators. Includes: scientific, financial, statistics, graphing, time, constants, and currency & unit conversions."), _T("https://marketplace.windowsphone.com/details.aspx?appId=2fdd840f-f4c2-4040-b225-182a5fd97396"));
        dlgTrialOver.AddItem(1, _T("Scientific Calc"), _T("A complete scientific calculator including base conververions, graphing and over 1500 constants."), _T("https://marketplace.windowsphone.com/details.aspx?appId=77a0fd4d-8eb6-4b41-8c03-00fc179495f1"));
        dlgTrialOver.AddItem(2, _T("Financial Calc"), _T("A complete financial calculator including 10 financial and 2 statistical worksheets to quickly solve common calculations."), _T("https://marketplace.windowsphone.com/details.aspx?appId=93f5c110-c5e2-47c3-b1fc-6fc0894466c8"));
        dlgTrialOver.AddItem(3, _T("Converter Pro"), _T("Convert values in 17 categories and converts over 150 currencies (updated daily)."), _T("https://marketplace.windowsphone.com/details.aspx?appId=e66fd763-351f-4d75-bd8c-87c0a714f9b5"));
        dlgTrialOver.AddItem(4, _T("Calc Pro Lite"), _T("An easy to use calculator that includes commonly used functions and a european currency converter."), _T("https://marketplace.windowsphone.com/details.aspx?appId=http://marketplace.windowsphone.com/details.aspx?appId=3d54a121-fd0e-4b86-9829-f9765a9226fa"));

        dlgTrialOver.Launch(hInstance);

        CIssKineticList::DeleteAllContent();
        CIssString::DeleteInstance();
        return 0;
    }
#endif

	CDlgCalcPro* dlgCalcPro = new CDlgCalcPro(hInstance);
	if(!dlgCalcPro)
		return FALSE;
	if(dlgCalcPro->Create(_T("Calc Pro"), NULL, hInstance, CLASS_Name))
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

	return 0;
}

