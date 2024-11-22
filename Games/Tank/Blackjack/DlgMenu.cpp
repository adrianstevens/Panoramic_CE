#include "StdAfx.h"
#include "DlgMenu.h"
#include "resource.h"
#include "IssCommon.h"

#if !defined(WIN32_PLATFORM_WFSP) && !defined(WIN32_PLATFORM_PSPC)
#include "DlgHelp.h"
#endif

#define		WM_MENU_Button	WM_USER + 100

CDlgMenu::CDlgMenu()
{
}

CDlgMenu::~CDlgMenu(void)
{
}


BOOL CDlgMenu::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	int iReturn = 0;
	switch(uiMessage)
	{
		case (WM_MENU_Button + BUTTON_Resume):
			iReturn = BUTTON_Resume+1;
			break;
		case (WM_MENU_Button + BUTTON_NewGame):
			iReturn = BUTTON_NewGame+1;
			break;
		case (WM_MENU_Button + BUTTON_Options):
			iReturn = BUTTON_Options+1;
			break;
		case (WM_MENU_Button + BUTTON_Help):
		{
#if defined(WIN32_PLATFORM_WFSP) || defined(WIN32_PLATFORM_PSPC)
			// launch the help
			LaunchHelp(_T("Blackjack.htm"), m_hInst);
#else
			CDlgHelp dlgHelp;
			dlgHelp.Init(m_hInst);
			LRESULT lr = FxShowDialog( m_hWnd, NULL,/*m_hInst,*/ IDFX_HELP_VIEW, dlgHelp.ProcWnd, (LPARAM)&dlgHelp);
#endif
		}
			break;
		case (WM_MENU_Button + BUTTON_Quit):
			iReturn = BUTTON_Quit+1;
			break;
		default:
			return FALSE;
	}

	if(iReturn)
		EndModalDialog(hWnd, iReturn);
	
	return TRUE;
}

BOOL CDlgMenu::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(wParam == VK_ESCAPE)
	{
		EndModalDialog(hWnd, BUTTON_Quit+1);
		return 0;
	}
	else 
		return CIssWndMenu::OnKeyUp(hWnd, wParam, lParam);

}
