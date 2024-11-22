#pragma once

#include "IssWnd.h"

class CDlgEula:public CIssWnd
{
public:
	CDlgEula(void);
	~CDlgEula(void);

	BOOL OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
	HWND	m_hWndEdit;
};
