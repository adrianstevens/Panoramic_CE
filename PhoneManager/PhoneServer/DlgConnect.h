#pragma once
#include "isswnd.h"
#include "IssCallManager.h"

class CDlgConnect : public CIssWnd
{
public:
	CDlgConnect(void);
	~CDlgConnect(void);

	BOOL		Init(CIssCallManager* CallManger);
	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);


private:

private:
	CIssCallManager*	m_oCallMgr;


};
