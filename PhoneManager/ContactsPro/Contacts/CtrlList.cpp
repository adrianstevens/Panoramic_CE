
#include "CtrlList.h"

CCtrlList::CCtrlList(void)
{
}

CCtrlList::~CCtrlList(void)
{
}

BOOL CCtrlList::Create(HWND hWndParent, HINSTANCE hInst)
{
	m_hInst = hInst;
	m_bHandlePaint	= FALSE;
	m_eWndType		= WND_Window;

	if ((m_hWnd = CreateWindowEx(0,
		WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN |
		WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_OWNERDRAWFIXED /*| LVS_OWNERDATA*/ | LVS_NOCOLUMNHEADER, 
		0, 0, 0, 0, hWndParent,
		(HMENU) NULL, hInst, NULL)) == NULL)
		return FALSE;

	//and now override the procs
	SetWindowLong( m_hWnd, GWL_USERDATA, reinterpret_cast<long>( this ) ); 
	m_lpfnCtrl = (WNDPROC)::GetWindowLong(m_hWnd, GWL_WNDPROC);
	SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)ProcWnd);

	//
	//  We only need to create one column
	//
	LV_COLUMN LVColumn = {0};

	LVColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;

	LVColumn.fmt		= LVCFMT_LEFT;
	LVColumn.cx			= 35;		// we'll change this later
	LVColumn.iSubItem	= 0;
	ListView_InsertColumn(m_hWnd, 0, &LVColumn);

	return TRUE;
}

BOOL CCtrlList::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_RIGHT:
		PostMessage(GetParent(GetParent(m_hWnd)), WM_ListKeyRight, 0,0);
		break;
	case VK_LEFT:
		PostMessage(GetParent(GetParent(m_hWnd)), WM_ListKeyLeft, 0,0);
		break;
	}
	return UNHANDLED;
}

BOOL CCtrlList::ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_GETDLGCODE)
		return (BOOL) (DLGC_WANTCHARS | DLGC_WANTARROWS);

	return CallWindowProc(m_lpfnCtrl, hWnd, message, wParam, lParam);
}
