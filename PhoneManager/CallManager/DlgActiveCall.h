#pragma once
#include "isswnd.h"
#include "IssGDI.h"

class CDlgActiveCall :
	public CIssWnd
{
public:
	CDlgActiveCall(void);
	~CDlgActiveCall(void);

	BOOL			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);
};
