#include "StdAfx.h"
#include "winbase.h"
#include "resource.h"
#include "DlgStart.h"
#include "IssCommon.h"
#include "IssRegistry.h"
#include "IssGDIDraw.h"


#define LINK_NAME   _T("LauncherStart")

#define IDT_START_REDRAW  30000  //30 seconds??

CDlgStart*	g_dlgStart = NULL;
WNDPROC		g_pOldHHTaskbar = NULL;


LRESULT ProcHHTaskBarHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_LBUTTONDOWN)
    {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);
        LRESULT lrResult = 1;

        RECT rcClient;
        GetClientRect(hWnd, &rcClient);

        if(g_dlgStart && pt.x < rcClient.right/3)
        {
            if(g_dlgStart->LaunchLauncher())
                return TRUE;
        }
    }

    return CallWindowProc(g_pOldHHTaskbar, hWnd, uMsg, wParam, lParam);
}

CDlgStart::CDlgStart(void)
{
}

CDlgStart::~CDlgStart(void)
{
    CloseAll();
}

void CDlgStart::CloseAll()
{
    if(m_hWnd)
        KillTimer(m_hWnd, IDT_START_REDRAW);
    ResetHHProc();
}

BOOL CDlgStart::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    // read in the icon settings
    DWORD dwItem;

    m_gdiIcon.Destroy();

    m_eIconStyle = ICON_Default;

    if(S_OK == GetKey(_T("SOFTWARE\\Pano\\Launcher\\Launcher"), _T("Icon"), dwItem))
        m_eIconStyle = (EnumIconStyle)dwItem;

    if(m_eIconStyle == ICON_Off || m_eIconStyle == ICON_Default)
    {
        MoveWindow(m_hWnd, 0, 0, 0, 0, TRUE);
        ShowWindow(hWnd, SW_HIDE);
    }
    else
    {
        // place the window properly
        RECT rcWindow;
	    GetWindowRect(GetParent(m_hWnd), &rcWindow);
        int iWidth;
        int iHeight = HEIGHT(rcWindow);
        switch(HEIGHT(rcWindow))
        {
        case 35:
            iWidth = 31;
            iHeight= 31;
            break;
        case 52:
            iWidth = 45;
            iHeight= 45;
            break;
        default:
            iWidth = 23;
            iHeight= 24;    // default
        }
        MoveWindow(m_hWnd, (IsVGA()?2:1), (IsVGA()?2:1), iWidth, iHeight, TRUE);
    }

    g_dlgStart = this;

    SetHHProc();

    SetTimer(m_hWnd, IDT_START_REDRAW, IDT_START_REDRAW, NULL);
    
	return TRUE;
}

BOOL CDlgStart::OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    ResetHHProc();
    return UNHANDLED;
}

BOOL CDlgStart::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	/*switch(LOWORD(wParam))
	{

	}*/
	return TRUE;
}


BOOL CDlgStart::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(m_eIconStyle == ICON_Off || m_eIconStyle == ICON_Default)
        return TRUE;


	if(m_gdiIcon.GetDC() == NULL)
    {
        RECT rcWindow;
        GetWindowRect(GetParent(m_hWnd), &rcWindow);
        UINT uiImage;

        if(m_eIconStyle == ICON_Rocket)
        {
            switch(HEIGHT(rcWindow))
            {
            case 35:
                uiImage = IDR_PNG_Start35;
                break;
            case 52:
                uiImage = IDR_PNG_StartVGA;
                break;
            default:
                uiImage = IDR_PNG_Start;
            }
        }
        else
        {
            switch(HEIGHT(rcWindow))
            {
            case 35:
                uiImage = IDR_PNG_Start352;
                break;
            case 52:
                uiImage = IDR_PNG_StartVGA2;
                break;
            default:
                uiImage = IDR_PNG_Start2;
            }
        }

        CIssGDIEx gdiIcon;
        gdiIcon.LoadImage(uiImage, hWnd, m_hInst, TRUE);

        m_gdiIcon.Create(hDC, gdiIcon.GetWidth(), gdiIcon.GetHeight(), TRUE, TRUE);
        Draw(m_gdiIcon, 0,0, gdiIcon.GetWidth(), gdiIcon.GetHeight(), gdiIcon, 0,0);
    }


	// copy to the screen
	BitBlt(hDC, 
		0,0,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		m_gdiIcon.GetDC(),
		0,0,
		SRCCOPY);


	return TRUE;
}

BOOL CDlgStart::OnLButtonDown(HWND hWnd, POINT& pt)
{
    LaunchLauncher();

	return TRUE;
}



BOOL CDlgStart::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDT_START_REDRAW)
    {
        KillTimer(m_hWnd, IDT_START_REDRAW);
        m_gdiIcon.Destroy();
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
	return TRUE;
}

BOOL CDlgStart::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage == WM_USER + 6123)
    {
        ResetHHProc();
        PostQuitMessage(0);
    }

	return TRUE;		
}

BOOL CDlgStart::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgStart::OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(wParam == SETTINGCHANGE_RESET)
	{
        InvalidateRect(GetParent(m_hWnd), NULL, FALSE);
        SetTimer(m_hWnd, IDT_START_REDRAW, 500, NULL);//half a second should do it
	}
	return TRUE;
}

BOOL CDlgStart::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//DebugOut(_T("OnActivate - %d"), wParam);
	if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
	{	
		// give the focus back 
		EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)this); 
	}
	return TRUE;
}

BOOL CDlgStart::EnumWindowsProc(HWND hWnd, DWORD lParam)
{
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	if(rcClient.right-rcClient.left != GetSystemMetrics(SM_CXSCREEN))
		return TRUE;	// keep going till we find a valid window

	if((dwStyle & WS_VISIBLE) == 0 || (dwStyle & WS_OVERLAPPED) == 0)
		//Not a visible window. Don't include it.
		return TRUE;

	DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

	TCHAR szClassName[STRING_MAX];
	GetClassName(hWnd, szClassName, STRING_MAX);

	CDlgStart* pThis = (CDlgStart*)lParam;
	if(pThis->m_oStr->Compare(szClassName, _T("HHTaskBar"))==0)
		return TRUE;

	SetForegroundWindow((HWND)(((DWORD)hWnd) | 0x1));
	SetActiveWindow(hWnd);
	SetFocus(hWnd);
	return FALSE;
	/*TCHAR szWindowName[STRING_MAX];
	GetWindowText(hWnd, szWindowName, STRING_MAX);
	DebugOut(_T("EnumWindowProc - %s, %s, %d, %d, %d"), szWindowName, szClassName, hWndParent, dwStyle, dwExStyle);*/
	return TRUE;
}

BOOL CDlgStart::ResetWindowPosition()
{
    if(m_eIconStyle == ICON_Off || m_eIconStyle == ICON_Default)
        return TRUE;

	int iHeight = 2;

	RECT rcParent;
	GetClientRect(GetParent(m_hWnd), &rcParent);

	int iXStart = 0;
	int iYStart = 0;
	int	iWidth  = GetSystemMetrics(SM_CXSCREEN);
	

	// we don't really want to cover up the start menu
	MoveWindow(m_hWnd,
				iXStart,iYStart,
			   iWidth, iHeight,
			   TRUE);
	return TRUE;
}



BOOL CDlgStart::IsAutoLaunch()
{
	TCHAR szStartupFolder[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szStartupFolder, CSIDL_STARTUP, FALSE);

	m_oStr->Concatenate(szStartupFolder, _T("\\"));
	m_oStr->Concatenate(szStartupFolder, LINK_NAME);
	m_oStr->Concatenate(szStartupFolder, _T(".lnk"));

	WIN32_FIND_DATA data;

	HANDLE hFile = FindFirstFile(szStartupFolder, &data);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFile);
		return TRUE;
	}

	return FALSE;
}

void CDlgStart::SetAutoLaunch()
{
	TCHAR szStartupFolder[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szStartupFolder, CSIDL_STARTUP, FALSE);
	
	m_oStr->Concatenate(szStartupFolder, _T("\\"));
	m_oStr->Concatenate(szStartupFolder, LINK_NAME);
	m_oStr->Concatenate(szStartupFolder, _T(".lnk"));

	TCHAR szFullFileName[STRING_MAX];
	m_oStr->Format(szFullFileName, _T("\"%s\""), _T("\\windows\\PanoStart.exe"));

	SHCreateShortcut(szStartupFolder, szFullFileName);

	return;
}

void CDlgStart::RemoveAutoLaunch()
{
	TCHAR szStartupFolder[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szStartupFolder, CSIDL_STARTUP, FALSE);

	m_oStr->Concatenate(szStartupFolder, _T("\\"));
	m_oStr->Concatenate(szStartupFolder, LINK_NAME);
	m_oStr->Concatenate(szStartupFolder, _T(".lnk"));

	DeleteFile(szStartupFolder);
}



BOOL CDlgStart::OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	EnumWindows((WNDENUMPROC)EnumHelpProc, (LPARAM)this); 
	return TRUE;
}

BOOL CDlgStart::EnumHelpProc(HWND hWnd, DWORD lParam)
{
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	if(rcClient.right-rcClient.left != GetSystemMetrics(SM_CXSCREEN))
		return TRUE;	// keep going till we find a valid window

	if((dwStyle & WS_VISIBLE) == 0 || (dwStyle & WS_OVERLAPPED) == 0)
		//Not a visible window. Don't include it.
		return TRUE;

	DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

	TCHAR szClassName[STRING_MAX];
	GetClassName(hWnd, szClassName, STRING_MAX);

	CDlgStart* pThis = (CDlgStart*)lParam;
	if(pThis->m_oStr->Compare(szClassName, _T("HHTaskBar"))==0)
		return TRUE;

	PostMessage(hWnd, WM_HELP, 0, 0);
	return FALSE;

}

BOOL CDlgStart::LaunchLauncher()
{
    //first check if its running
#define WND_ContactsApp		_T("Launchpad")
#define WND_ContactsClass	_T("ClassLauncher")

    if(m_eIconStyle == ICON_Off)
        return TRUE;

    HWND hAppWnd = FindWindow(WND_ContactsClass, WND_ContactsApp);

    if(hAppWnd)
    {
        //is it the foreground window? if yes HIDE
        if(hAppWnd == GetForegroundWindow())
            ShowWindow(hAppWnd, SW_HIDE);
        //otherwise SHOW
        else
        {
            ShowWindow(hAppWnd, SW_SHOW);
            SetForegroundWindow(hAppWnd);
        }
        return TRUE;
    }
    else //otherwise ... try to load it up
    {
        TCHAR szPath[MAX_PATH];
        SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);
        m_oStr->Concatenate(szPath, _T("\\panoLauncher.exe"));
        if(IsFileExists(szPath))
        {
            CreateProcess(szPath, _T("-f"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
            return TRUE;
        }
        /*else
        {
        LPARAM lParam = MAKELONG(IsVGA()?6:3, IsVGA()?6:3);

        // pass the message off to the parent so it will drop down the normal start menu
        SendMessage(GetParent(m_hWnd), WM_LBUTTONDOWN, MK_LBUTTON, lParam);
        }*/
    }
    return FALSE;
    
}

void CDlgStart::ResetHHProc()
{
    if(g_pOldHHTaskbar)
    {
        HWND hWndTaskbar = FindWindow(_T("HHTaskBar"), NULL);
        SetWindowLong(hWndTaskbar, GWL_WNDPROC, (LONG)g_pOldHHTaskbar);
        g_pOldHHTaskbar = NULL;
    }
}

void CDlgStart::SetHHProc()
{
    ResetHHProc();

    HWND hWndTaskbar = FindWindow(_T("HHTaskBar"), NULL);
    g_pOldHHTaskbar  = (WNDPROC)SetWindowLong(hWndTaskbar, GWL_WNDPROC, (LONG)ProcHHTaskBarHook);
}
