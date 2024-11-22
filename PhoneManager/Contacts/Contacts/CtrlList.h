#pragma once

#include "stdafx.h"
#include "IssWnd.h"

#define WM_ListKeyRight	WM_USER + 500
#define WM_ListKeyLeft	WM_USER + 501


class CCtrlList:public CIssWnd
{
public:
	CCtrlList(void);
	~CCtrlList(void);

	BOOL Create(HWND hWndParent, HINSTANCE hInst);
	BOOL OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	WNDPROC					m_lpfnCtrl;
};
