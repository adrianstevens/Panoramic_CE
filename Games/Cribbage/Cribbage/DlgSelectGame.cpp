#include "StdAfx.h"
#include "DlgSelectGame.h"
#include "Resource.h"
#include "IssGDIEx.h"
#include "IssGDIFx.h"
#include "IssGDIDraw.h"
#include "IssLocalisation.h"
#include "sipapi.h"

#define TEXT_COLOR 0xDDDDDD

extern CObjGui* g_gui;

CDlgSelectGame::CDlgSelectGame(void):
m_oStr(CIssString::Instance()),
m_hWndEdit(NULL),
m_iGameNum(-1),
m_hFont(CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*14/32, FW_BOLD, TRUE))
{
}

CDlgSelectGame::~CDlgSelectGame(void)
{
	CIssGDIEx::DeleteFont(m_hFont);
}

BOOL CDlgSelectGame::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
    //Show as a Pocket PC full screen dialog.
    SHINITDLGINFO DlgInfo;

    DlgInfo.dwMask	= SHIDIM_FLAGS;
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
    DlgInfo.hDlg	= hWnd;
    SHInitDialog(&DlgInfo);

    SHMENUBARINFO mbi;
    memset(&mbi, 0, sizeof(mbi)); 
    mbi.cbSize		= sizeof(mbi);
    mbi.hwndParent	= hWnd;
    mbi.nToolBarId	= IDR_MENU_Options;
    mbi.hInstRes	= m_hInst;
    SHCreateMenuBar(&mbi);

/*
#ifdef WIN32_PLATFORM_WFSP
	::SendMessage(	mbi.hwndMB, 
					SHCMBM_OVERRIDEKEY, 
					VK_TBACK, 
					MAKELPARAM(SHMBOF_NODEFAULT | SHMBOF_NOTIFY, SHMBOF_NODEFAULT | SHMBOF_NOTIFY));
#endif*/
#endif
//  really shouldn't need it here
//    g_gui->Init(hWnd, m_hInst);

	//make the edit control
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_NOHIDESEL | ES_NUMBER;

	m_hWndEdit = CreateWindowEx(0, TEXT("edit"), NULL,
		dwStyle,
		0, 0, 0, 0, hWnd, NULL, 
		m_hInst, NULL);

	SHSipPreference(m_hWnd, SIP_UP);
	SetFocus(m_hWndEdit);

    return TRUE;
}

BOOL CDlgSelectGame::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef WIN32_PLATFORM_WFSP
	if(HIWORD(lParam)==VK_TBACK && (0 != (MOD_KEYUP & LOWORD(lParam))))
	{
		//SHSendBackToFocusWindow(WM_HOTKEY, wParam, lParam);
		SendMessage(m_hWndEdit, WM_CHAR, VK_BACK, 0);
	}
#endif
	return UNHANDLED;
}

BOOL CDlgSelectGame::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	//set the RECTs
#define TEXT_INDENT GetSystemMetrics(SM_CXICON)/2

	m_rcTitle.left		= TEXT_INDENT;
	m_rcTitle.right		= rc.right - TEXT_INDENT;
	m_rcTitle.top		= TEXT_INDENT;
	m_rcTitle.bottom	= GetSystemMetrics(SM_CXICON)*3/2;

	m_rcEditCtrl.left	= TEXT_INDENT;
	m_rcEditCtrl.right	= rc.right - TEXT_INDENT;
	m_rcEditCtrl.top	= m_rcTitle.bottom + TEXT_INDENT/2;
	m_rcEditCtrl.bottom = m_rcEditCtrl.top + GetSystemMetrics(SM_CXICON)/2;

	m_rcUnsolvable.left	= TEXT_INDENT;
	m_rcUnsolvable.right= rc.right - TEXT_INDENT;
	m_rcUnsolvable.top	= m_rcEditCtrl.bottom + TEXT_INDENT/2;
	m_rcUnsolvable.bottom= rc.bottom - TEXT_INDENT;

	MoveWindow(m_hWndEdit, m_rcEditCtrl.left, m_rcEditCtrl.top, m_rcEditCtrl.right - m_rcEditCtrl.left, m_rcEditCtrl.bottom - m_rcEditCtrl.top, FALSE);

	return TRUE;
}

BOOL CDlgSelectGame::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == IDMENU_Save || LOWORD(wParam) == IDMENU_Left)
	{
		SHSipPreference(m_hWnd, SIP_FORCEDOWN);

		//get the int out of the edit control
		m_iGameNum = 0;
		int iTextLength = (int)SendMessage(m_hWndEdit, WM_GETTEXTLENGTH, 0, 0L);
		if(iTextLength > 0)
		{
			TCHAR* szText = new TCHAR[iTextLength + 1];
			ZeroMemory(szText, sizeof(TCHAR)*(iTextLength+1));
			SendMessage(m_hWndEdit, WM_GETTEXT, iTextLength+1, (LPARAM)szText);
			m_iGameNum = m_oStr->StringToInt(szText);
			delete [] szText;
		}
        SafeCloseWindow(1);
		//EndDialog(m_hWnd, 1);
		return TRUE;
	}
	else if(LOWORD(wParam) == IDMENU_Cancel || LOWORD(wParam) == IDMENU_Right)
	{
		SHSipPreference(m_hWnd, SIP_FORCEDOWN);
        SafeCloseWindow(0);
		//EndDialog(m_hWnd, 0);
		return TRUE;
	}

	return UNHANDLED;
}

BOOL CDlgSelectGame::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    g_gui->InitGDI(rcClient, hDC);

    g_gui->DrawBackground(rcClient);
    DrawText(g_gui->gdiMem.GetDC());

    //TCHAR szText[STRING_NORMAL];
    //m_oStr->StringCopy(szText, ID(IDS_MENU_Cancel), STRING_NORMAL, m_hInst);
//    g_gui->DrawBottomBar(rcClient, m_oStr->GetText(ID(IDS_MENU_Save), m_hInst), szText);

    g_gui->DrawScreen(rcClient, hDC);
	return TRUE;
}

BOOL CDlgSelectGame::DrawText(HDC hDC)
{
	::DrawText(hDC, m_oStr->GetText(ID(IDS_OPT_SelectGame), m_hInst), m_rcTitle, DT_CENTER, m_hFont, TEXT_COLOR);

    ::DrawText(hDC, m_oStr->GetText(ID(IDS_OPT_Unsolvable), m_hInst), m_rcUnsolvable, DT_LEFT | DT_WORDBREAK, m_hFont, TEXT_COLOR);

	return TRUE;
}


BOOL CDlgSelectGame::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) 
    { 
        SHFullScreen(m_hWnd, SHFS_HIDETASKBAR);// | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);   
        //SipShowIM(SIPF_ON);
        return TRUE; 
    }
    return UNHANDLED;
}

BOOL CDlgSelectGame::OnLButtonDown(HWND hWnd, POINT& pt)
{
    return g_gui->OnLButtonDown(hWnd, pt);
}

BOOL CDlgSelectGame::OnLButtonUp(HWND hWnd, POINT& pt)
{
    return g_gui->OnLButtonUp(hWnd, pt);
}


BOOL CDlgSelectGame::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case VK_TSOFT1:
        EndDialog(m_hWnd, IDOK);
        break;
    case VK_ESCAPE:
    case VK_TSOFT2:
        EndDialog(m_hWnd, IDCANCEL);
        break;
    default:

        return UNHANDLED;
    }
    return TRUE;
}