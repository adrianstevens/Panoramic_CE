#pragma once

#include "IssWnd.h"

class CDlgTodayScreen:public CIssWnd
{
public:
	CDlgTodayScreen(void);
	~CDlgTodayScreen(void);

	BOOL OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
	HWND	m_hWndEdit;
    HWND    m_hWndChk;
};
