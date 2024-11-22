#include "StdAfx.h"
#include "DlgMenu.h"
#include "resource.h"
#include "IssCommon.h"

#define		WM_MENU_Button	WM_USER+100

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
			// launch the help
			LaunchHelp(_T("VideoPoker.htm"), m_hInst);
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

