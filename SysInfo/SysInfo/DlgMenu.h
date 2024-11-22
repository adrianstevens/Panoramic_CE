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


protected:
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

private:	// variables
	
};
