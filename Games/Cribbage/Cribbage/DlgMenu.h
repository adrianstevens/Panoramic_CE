#pragma once

#include "IssWndMenu.h"
#include "IssDynBtn.h"

enum EnumButtons
{
	BUTTON_Resume,
	BUTTON_NewGame,
	BUTTON_Options,
	BUTTON_Help,
	BUTTON_Quit,
	BUTTON_Unknown,
};

class CDlgMenu : public CIssWndMenu
{
public:
	CDlgMenu();
	~CDlgMenu(void);

	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("iSSCribbage.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};


protected:
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

private:	// variables
	
};
