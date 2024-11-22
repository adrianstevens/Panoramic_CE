#include "DlgTodayScreen.h"
#include "stdafx.h"
#include "resource.h"

CDlgTodayScreen::CDlgTodayScreen(void)
:m_hWndEdit(NULL)
{
}

CDlgTodayScreen::~CDlgTodayScreen(void)
{
}

BOOL CDlgTodayScreen::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    m_hWndChk  = CreateWindowEx(0, _T("Button"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
	m_hWndEdit = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_READONLY | WS_VSCROLL | ES_MULTILINE, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);

	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN|SHIDIF_DONEBUTTON;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= m_hWnd;
	mbi.nToolBarId	= IDR_MENU_OkCancel; 
	mbi.hInstRes	= m_hInst;
	SHCreateMenuBar(&mbi);

    SetWindowText(m_hWndChk, _T("Enable Phone Genius today plugin"));
    SendMessage(m_hWndChk, BM_SETCHECK, (WPARAM)FALSE, 0);

    TCHAR szInstallToday[] = _T("WARNING: By enabling the today plugin here, all other today plugins will be turned off (except for the Calendar and Tasks plugins).  You can go to Start->Settings->Today->Items and configure your today screen plugins to find the best setup for you.");
	
	// send it to the screen
	SetWindowText(m_hWndEdit, szInstallToday);

	SetFocus(m_hWndEdit);

	// Reset selection to 0
	PostMessage(m_hWndEdit, EM_SETSEL, 0, 0);
	PostMessage(m_hWndEdit, EM_SCROLLCARET, 0, 0);
	PostMessage(m_hWndEdit, WM_KEYDOWN, VK_UP, 0);
	PostMessage(m_hWndEdit, WM_KEYUP, VK_UP, 0);

	return TRUE;
}

BOOL CDlgTodayScreen::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
#ifndef WIN32_PLATFORM_WFSP
	case IDOK:
#endif
	case IDMENU_Yes:
    case IDMENU_Ok:
        if(BST_CHECKED==SendMessage(m_hWndChk, BM_GETCHECK,0,0))
		    EndDialog(hWnd, IDOK);
        else
            EndDialog(hWnd, IDCANCEL);
		break;
	case IDMENU_No:
    case IDMENU_Cancel:
		EndDialog(hWnd, IDCANCEL);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgTodayScreen::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int	iIndent		= GetSystemMetrics(SM_CXICON)/8;
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

#ifdef WIN32_PLATFORM_WFSP
    int iIconSize	= GetSystemMetrics(SM_CXICON)*2/3; // GetSystemMetrics(SM_CXICON)*2/3;//good height for smartphone
#else
    int iIconSize	= GetSystemMetrics(SM_CYVSCROLL);
#endif

    int iY = iIndent;

    MoveWindow(m_hWndChk, iIndent, iY, rcClient.right - 2*iIndent, iIconSize, FALSE);
    iY += iIconSize;

    rcClient.top = iY;

	if(m_hWndEdit != NULL)//play safe
	{
		MoveWindow(m_hWndEdit, rcClient.left, rcClient.top, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, FALSE);

		// Reset selection to 0
		PostMessage(m_hWndEdit, EM_SETSEL, 0, 0);
		PostMessage(m_hWndEdit, EM_SCROLLCARET, 0, 0);

		SetFocus(m_hWndEdit);
	}
	return TRUE;
}