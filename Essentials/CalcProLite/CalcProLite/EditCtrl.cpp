#include "stdafx.h"
#include "EditCtrl.h"

CEditCtrl::CEditCtrl(void)
:m_lpfnEditCtrl(NULL)
,m_hWndParent(NULL)
{
}

CEditCtrl::~CEditCtrl(void)
{
}

BOOL CEditCtrl::Create(HWND hWndParent, HINSTANCE hInst, DWORD dwStyle)
{
	// let's start anew
	Destroy();

	m_hWndParent = hWndParent;

	m_bHandlePaint = FALSE;

	m_hWnd = CreateWindowEx(0, TEXT("edit"), NULL,
							dwStyle,
							0, 0, 0, 0, hWndParent, NULL, 
							hInst, NULL);

	//and now override the procs
	SetWindowLong( m_hWnd, GWL_USERDATA, reinterpret_cast<long>( this ) ); 
	m_lpfnEditCtrl = (WNDPROC)::GetWindowLong(m_hWnd, GWL_WNDPROC);
	SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)ProcWnd);

	return TRUE;
}



BOOL CEditCtrl::ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CHAR:
	case WM_KEYDOWN:
	case WM_KEYUP:
		SendMessage(m_hWndParent, message, wParam, lParam);
		return TRUE;
		break;
	
	}
	return (BOOL)CallWindowProc(m_lpfnEditCtrl, hWnd, message, wParam, lParam);
}

BOOL CEditCtrl::OnLButtonDown(HWND hWnd, POINT& pt)
{
#ifdef SHELL_AYGSHELL
	SHRGINFO    shrg;

	shrg.cbSize = sizeof(shrg);
	shrg.hwndClient = hWnd;
	shrg.ptDown.x = pt.x;
	shrg.ptDown.y = pt.y;
	shrg.dwFlags = SHRG_RETURNCMD|SHRG_LONGDELAY;

	if (SHRecognizeGesture(&shrg) == GN_CONTEXTMENU) 
	{
		//	g_dlgNotepad->OnMenuContext(pt);
		PostMessage(GetParent(hWnd), WM_ShowEditMenu, pt.x, pt.y);
		return TRUE;
	}
#endif
	return UNHANDLED;
}
