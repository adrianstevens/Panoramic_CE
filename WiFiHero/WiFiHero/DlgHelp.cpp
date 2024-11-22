#include "DlgHelp.h"
#include "StdAfx.h"
#include "Resource.h"

#define TXT_HELP_MSG _T("WiFi Hero Help\r\n\r\n\
WiFi Hero will automatically load and save all WiFi settings on your device. \
If WiFi Hero finds that any WiFi access points have been deleted they will be displayed. \r\n\r\n\
You can restore or delete saved access points individually or press \"Update\" on the menu to restore all. \r\n\r\n\
WiFi Hero will automatically save new access points when its launched.\r\n\r\n\
It is recommended that you turn off your WiFi radio before attempting to restore WiFi profiles.\r\n\r\n\
IF you're still unable to restore your profile - it may mean your preshared key does not meet the minimum complexity requirements.  It is recommended to use a random key of letters and numbers with at least a length of 12.\r\n\r\n\
For additional information, please visit www.panoramicsoft.com")


CDlgHelp::CDlgHelp(void)
:m_hFont(NULL)
,m_hWndEdit(NULL)
{
    m_hFont = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
}

CDlgHelp::~CDlgHelp(void)
{
    CIssGDIEx::DeleteFont(m_hFont);
}

BOOL CDlgHelp::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
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
    mbi.nToolBarId	= IDR_MENU_Back; 
    mbi.hInstRes	= m_hInst;
    SHCreateMenuBar(&mbi);

    //create the edit control
    UINT uiParams = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_VSCROLL | ES_READONLY | ES_MULTILINE;

    m_hWndEdit = CreateWindowEx(0, TEXT("edit"), NULL,
        uiParams,
        0, 0, 0, 0, hWnd, NULL, 
        m_hInst, NULL);

    SendMessage(m_hWndEdit, EM_FMTLINES, (WPARAM)FALSE, NULL ); 

    //set the font
    SendMessage(m_hWndEdit, WM_SETFONT, (WPARAM)m_hFont, (LPARAM)TRUE);

    //set the edit control text
    SetWindowText(m_hWndEdit, TXT_HELP_MSG);

    BringWindowToTop(m_hWndEdit);
    SetFocus(m_hWndEdit);

    return TRUE;
}

BOOL CDlgHelp::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case IDOK:
    case IDMENU_Back:
        SafeCloseWindow();
    	break;
    default:
        return UNHANDLED;
        break;
    }

    return TRUE;
}

BOOL CDlgHelp::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    if(m_hWndEdit)
        MoveWindow(m_hWndEdit, rcClient.left, rcClient.top, WIDTH(rcClient), HEIGHT(rcClient), FALSE);

    PostMessage(m_hWndEdit, WM_LBUTTONDOWN, MK_LBUTTON, HIWORD(1)+LOWORD(1));
    
    SendMessage(m_hWndEdit, EM_LINEINDEX, 0, 0);
    return TRUE;
}
