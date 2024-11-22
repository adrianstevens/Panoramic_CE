#include "StdAfx.h"
#include "DlgHiddenWnd.h"
#include "resourceppc.h"
#include "IssGDIEx.h"
#include "DlgCheekGuard.h"

#define IDT_Demo_Timer 2

CDlgHiddenWnd::CDlgHiddenWnd(void)
:m_oStr(CIssString::Instance())
{
}

CDlgHiddenWnd::~CDlgHiddenWnd(void)
{
}

BOOL CDlgHiddenWnd::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// "Hide" the window
	MoveWindow(m_hWnd,
		0,0,
		0, 0,
		FALSE);

	ShowWindow(m_hWnd, SW_HIDE);

	// initialize tapi
	m_oCallManager.Initialize(m_hInst, m_hWnd);

	SetTimer(m_hWnd, IDT_Demo_Timer, 1500, NULL);

	return TRUE;
}

BOOL CDlgHiddenWnd::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgHiddenWnd::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgHiddenWnd::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(wParam == IDT_Demo_Timer)
	{
		KillTimer(m_hWnd, IDT_Demo_Timer);
		DisplayIncomingCall();
	}
	return UNHANDLED;
}

BOOL CDlgHiddenWnd::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgHiddenWnd::OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgHiddenWnd::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uiMessage)
	{
	case WM_PHONE_RINGING:
		DisplayIncomingCall();
		break;
	case WM_CALL_ENDED:
		MessageBox(m_hWnd, _T("Call Ended"), NULL, MB_OK);
		break;
	default:
	    break;
	}

	return UNHANDLED;
}

BOOL CDlgHiddenWnd::DisplayIncomingCall()
{
	CDlgCheekGuard dlgCheekGuard;

	dlgCheekGuard.Init(&m_oCallManager);

	if(dlgCheekGuard.Create(_T("Cheek Guard"), NULL, m_hInst, _T("ClassCheekGuard"), 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, WS_VISIBLE))
	{
		ShowWindow(dlgCheekGuard.GetWnd(), SW_SHOW);

		SetForegroundWindow(dlgCheekGuard.GetWnd());

		MSG msg;
		while( GetMessage( &msg,NULL,0,0 ) ) /* jump into message pump */
		{
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		}
		return ( msg.wParam );
	}
	
	return TRUE;
}

