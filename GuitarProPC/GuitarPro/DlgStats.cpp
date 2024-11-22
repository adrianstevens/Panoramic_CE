#include "DlgStats.h"

#define TEXT_COLOR 0xDDDDDD

extern CObjGui* g_gui;

CDlgStats::CDlgStats(void)
m_oStr(CIssString::Instance()),
m_hFont(CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*14/32, FW_BOLD, TRUE))
{
}

CDlgStats::~CDlgStats(void)
{
	CIssGDIEx::DeleteFont(m_hFont);
}
BOOL CDlgStats::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
    //Show as a Pocket PC full screen dialog.
    SHINITDLGINFO DlgInfo;

    DlgInfo.dwMask	= SHIDIM_FLAGS;
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
    DlgInfo.hDlg	= hWnd;
    SHInitDialog(&DlgInfo);

#ifdef WIN32_PLATFORM_WFSP
	::SendMessage(	mbi.hwndMB, 
					SHCMBM_OVERRIDEKEY, 
					VK_TBACK, 
					MAKELPARAM(SHMBOF_NODEFAULT | SHMBOF_NOTIFY, SHMBOF_NODEFAULT | SHMBOF_NOTIFY));
#endif
#endif
//  really shouldn't need it here
//    g_gui->Init(hWnd, m_hInst);

	return TRUE;

}


BOOL CDlgStats::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == IDMENU_Save || LOWORD(wParam) == IDMENU_Left)
	{
		EndDialog(m_hWnd, 0);
		return TRUE;
	}
	else if(LOWORD(wParam) == IDMENU_Cancel || LOWORD(wParam) == IDMENU_Right)
	{
		EndDialog(m_hWnd, 0);
		return TRUE;
	}

	return UNHANDLED;
}

BOOL CDlgStats::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(g_gui->gdiMem.GetDC() == NULL)
    {
        FillRect(hDC, rcClient, 0xFFFFFF);
        g_gui->InitGDI(rcClient, hDC);
    }

    g_gui->DrawBackground(rcClient);
    DrawText(g_gui->gdiMem.GetDC());

    g_gui->DrawBottomBar(rcClient, _T("Save"), _T("Cancel"));

    g_gui->DrawScreen(rcClient, hDC);
	return TRUE;
}

BOOL CDlgStats::DrawText(HDC hDC)
{
	::DrawText(hDC, m_szStats, m_rcTitle, DT_CENTER, m_hFont, TEXT_COLOR);
	return TRUE;
}


BOOL CDlgStats::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE) 
    { 
        SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);   

        return TRUE; 
    }
    return UNHANDLED;
}

BOOL CDlgStats::OnLButtonDown(HWND hWnd, POINT& pt)
{
    return g_gui->OnLButtonDown(hWnd, pt);
}

BOOL CDlgStats::OnLButtonUp(HWND hWnd, POINT& pt)
{
    return g_gui->OnLButtonUp(hWnd, pt);
}


BOOL CDlgStats::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
