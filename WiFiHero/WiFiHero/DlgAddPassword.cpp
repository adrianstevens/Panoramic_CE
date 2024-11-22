#include "stdafx.h"
#include "DlgAddPassword.h"
#include "Resource.h"

#define MIN_PASS_LENGTH 8
#define MAX_PASS_LENGTH 32

CDlgAddPassword::CDlgAddPassword(void)
:m_hStatic(NULL)
,m_hEditControl(NULL)
{
    m_hFont = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*4/9, FW_NORMAL, TRUE);
}

CDlgAddPassword::~CDlgAddPassword(void)
{
    CIssGDIEx::DeleteFont(m_hFont);
}

BOOL CDlgAddPassword::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    //Show as a Pocket PC full screen dialog.
    SHINITDLGINFO DlgInfo;
    DlgInfo.dwMask	= SHIDIM_FLAGS;
    DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
    DlgInfo.hDlg	= hWnd;
    SHInitDialog(&DlgInfo);

    SHMENUBARINFO mbi;
    memset(&mbi, 0, sizeof(mbi)); 
    mbi.cbSize		= sizeof(mbi);
    mbi.hwndParent	= m_hWnd;
    mbi.nToolBarId	= IDR_MENU_OkCancel; 
    mbi.hInstRes	= m_hInst;
    SHCreateMenuBar(&mbi);

    DWORD dwParams = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_WANTRETURN;

    //create the controls
    m_hEditControl = CreateWindowEx(0, TEXT("edit"), NULL,
        dwParams, 0, 0, 0, 0, hWnd, NULL, 
        m_hInst, NULL);

    return TRUE;
}

BOOL CDlgAddPassword::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    ::FillRect(hDC, rcClient, 0xFFFFFF);

    DrawText(hDC, m_szMessage, m_rcTopMessage, DT_TOP | DT_WORDBREAK, m_hFont, 0);

    return TRUE;
}

BOOL CDlgAddPassword::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case IDOK:
    case IDMENU_Ok:
        m_oStr->Empty(m_szKey);

        SendMessage(m_hEditControl, WM_GETTEXT, STRING_LARGE, (LPARAM)&m_szKey);

        if(m_oStr->GetLength(m_szKey) >= MIN_PASS_LENGTH && m_oStr->GetLength(m_szKey) <= MAX_PASS_LENGTH)
            SafeCloseWindow(IDOK);
        else
            MessageBox(m_hWnd, _T("Not a valid network key.\r\nPlease check the key and try again."), _T("WiFi Hero"), MB_ICONEXCLAMATION);
    	break;
    case IDMENU_Cancel:
        if(IDYES == MessageBox(hWnd, _T("WiFi Hero will not be able to restore the settings for this access point without the network key.  Are you sure you want to cancel?"), _T("WiFi Hero"), MB_YESNO))
            SafeCloseWindow(IDCANCEL);
        break;
    default:
        break;
    }

    return TRUE;
}

BOOL CDlgAddPassword::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    int iIndent = GetSystemMetrics(SM_CXICON)/2;

    SetRect(&m_rcTopMessage, rcClient.left + iIndent, rcClient.top + iIndent, rcClient.right - iIndent, rcClient.top + HEIGHT(rcClient)/3);

    RECT rcTemp;
    SetRect(&rcTemp, rcClient.left + iIndent, m_rcTopMessage.bottom + iIndent, rcClient.right - iIndent, m_rcTopMessage.bottom + iIndent/2 + GetSystemMetrics(SM_CXICON));

    if(m_hEditControl)
        MoveWindow(m_hEditControl, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), FALSE);

    m_gdiMem.Destroy();

    return TRUE;
}

BOOL CDlgAddPassword::SetName(TCHAR *szName, BOOL bAccessPoint)
{
    if(szName == NULL)
        return FALSE;

    TCHAR szType[STRING_LARGE];

    if(bAccessPoint)
        m_oStr->StringCopy(szType, _T("Access Point"));
    else
        m_oStr->StringCopy(szType, _T("AdHoc Network"));

    m_oStr->Format(m_szMessage, _T("The %s %s requires a network key.  Please enter the network key once so WiFi Hero can correctly restore the settings."), szType , szName);

    return TRUE;
}

BOOL CDlgAddPassword::GetKey(TCHAR* szKey)
{
    if(szKey == NULL)
        return FALSE;

    m_oStr->StringCopy(szKey, m_szKey);

    return TRUE;

}
