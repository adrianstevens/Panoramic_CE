// Blackjack.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Blackjack.h"
#include "DlgBlackjack.h"
#include "IssCommon.h"
#include "resource.h"
#include "IssKey.h"
#include <windows.h>
#include <commctrl.h>
#include "IssLocalisation.h"
#include "DlgTrialOver.h"

HINSTANCE g_hStringResource;
int			g_iDaysLeft = 0;

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	g_hStringResource = hInstance;

	CIssKey key;

	// check if we're running a legit copy
	key.Init(_T("SOFTWARE\\Panoramic\\PanoBlackjack"), _T("SOFTWARE\\Pano\\PanoBlackjack2"), 0x0, 0x7B2E8E6E, 0x3E3A1FBE, 1, TRUE);
	key.CheckSetKey();
	key.CheckDemo();

	if(!key.m_bGood)
	{
		/*TCHAR szText[STRING_MAX];
		CIssString* oStr = CIssString::Instance();
		oStr->StringCopy(szText, ID(IDS_MSG_DemoOver), STRING_MAX, hInstance);
		MessageBox(GetDesktopWindow(),szText, _T(""), MB_OK);*/
		CIssString* oStr = CIssString::Instance();
		oStr->SetResourceInstance(hInstance);
		g_cLocale.Init(REG_KEY, hInstance, SUPPORT_English|SUPPORT_Portuguese|SUPPORT_German|SUPPORT_French|SUPPORT_Dutch|SUPPORT_Spanish, TRUE);

		CDlgTrialOver dlgTrialOver;
		dlgTrialOver.Initialize(IDS_APP_TITLE,
			ID(IDS_MSG_TrialOverShort),
			ID(IDS_MSG_DemoOver),
			ID(IDS_MENU_Quit),
			IsVGA()?IDR_PNG_DemoIconsVGA:IDR_PNG_DemoIcons,
			IsVGA()?IDR_PNG_BuyVGA:IDR_PNG_Buy,
			IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray);

		if(IsSmartphone())
		{
			dlgTrialOver.AddItem(0, _T("Blackjack"), NULL, _T("https://marketplace.windowsphone.com/details.aspx?appId=c1852e39-3390-4a79-bf39-57483fff8a57"));
			dlgTrialOver.AddItem(3, _T("FreeCell"), NULL, _T("https://marketplace.windowsphone.com/details.aspx?appId=dae15e1b-15ed-43ae-b3c1-a812b0cb40b4"));
		}
		else
		{
			dlgTrialOver.AddItem(0, _T("Blackjack"), NULL, _T("https://marketplace.windowsphone.com/details.aspx?appId=c1852e39-3390-4a79-bf39-57483fff8a57"));
			dlgTrialOver.AddItem(1, _T("Ultimate Sudoku"), NULL, _T("https://marketplace.windowsphone.com/details.aspx?appId=02cef10b-6fff-44c0-8d60-eb2aa51fd70a"));
			dlgTrialOver.AddItem(2, _T("Video Poker"), NULL, _T("https://marketplace.windowsphone.com/details.aspx?appId=bc929ed8-1841-4eda-82b3-df6339921002"));
			dlgTrialOver.AddItem(3, _T("FreeCell"), NULL, _T("https://marketplace.windowsphone.com/details.aspx?appId=dae15e1b-15ed-43ae-b3c1-a812b0cb40b4"));
		}

		dlgTrialOver.Launch(hInstance);

		return FALSE;
	}
	// set the daysleft for the menu (we don't display www.implicitsoftware.com on the menu)
	g_iDaysLeft	= (key.m_bDemo&&key.m_bGood ? key.GetDaysLeft(): 0);

	CDlgBackJack dlgBlackjack(key.m_bDemo);

	if(dlgBlackjack.Create(_T("Panoramic Blackjack"), NULL, hInstance, _T("ClassBlackjack")))
	{
		ShowWindow(dlgBlackjack.GetWnd(), SW_SHOW);

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
		return ( msg.wParam );
	}

	return 0;
}

