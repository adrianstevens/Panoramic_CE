#include "DlgEula.h"
#include "stdafx.h"
#include "resource.h"

CDlgEula::CDlgEula(void)
:m_hWndEdit(NULL)
{
}

CDlgEula::~CDlgEula(void)
{
}

BOOL CDlgEula::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
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
	mbi.nToolBarId	= IDR_MENU; 
	mbi.hInstRes	= m_hInst;
	SHCreateMenuBar(&mbi);

	// fill the content
	// try and load up the text from the resource
	HRSRC hres = FindResource(m_hInst, _T("EULA"), _T("TXT"));
	if (hres == 0)
	{
		MessageBox(m_hWnd, _T("FindResource Failed"), _T(""), MB_OK);
		return FALSE;
	}

	//If resource is found a handle to the resource is returned
	//now just load the resource
	HGLOBAL    hbytes = LoadResource(m_hInst, hres);

	// Lock the resource
	LPVOID pdata	= LockResource(hbytes);
	char*  strData	= (char*)pdata;

	// find out how long the string is
	TCHAR* szTemp = m_oStr->CreateAndCopy(strData);
	if(!szTemp)
		return FALSE;

	// send it to the screen
	SetWindowText(m_hWndEdit, szTemp);

	// cleanup
	delete [] szTemp;

	SetFocus(m_hWndEdit);

	// Reset selection to 0
	PostMessage(m_hWndEdit, EM_SETSEL, 0, 0);
	PostMessage(m_hWndEdit, EM_SCROLLCARET, 0, 0);
	PostMessage(m_hWndEdit, WM_KEYDOWN, VK_UP, 0);
	PostMessage(m_hWndEdit, WM_KEYUP, VK_UP, 0);

	return TRUE;
}

BOOL CDlgEula::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
#ifndef WIN32_PLATFORM_WFSP
	case IDOK:
#endif
	case IDMENU_Accept:
		EndDialog(hWnd, IDOK);
		break;
	case IDMENU_Deny:
		EndDialog(hWnd, IDCANCEL);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgEula::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int	iIndent		= GetSystemMetrics(SM_CXICON)/8;
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	rcClient.left	+= iIndent;
	rcClient.right	-= iIndent;
	rcClient.top	+= iIndent;
	rcClient.bottom -= iIndent;

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