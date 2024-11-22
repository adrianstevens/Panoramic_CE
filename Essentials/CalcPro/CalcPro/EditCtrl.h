#pragma once

#include "IssWnd.h"

#define WM_ShowEditMenu WM_USER + 100 

class CEditCtrl:public CIssWnd
{
public:
	CEditCtrl(void);
	~CEditCtrl(void);

	BOOL	Create(HWND hWndParent, HINSTANCE hInst, DWORD dwStyle);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	
private:
	BOOL	ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:	// variables
	WNDPROC m_lpfnEditCtrl;
	HWND	m_hWndParent;
};
