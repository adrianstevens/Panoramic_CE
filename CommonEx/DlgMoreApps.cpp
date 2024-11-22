#include "DlgMoreApps.h"
#include "resource.h"
#include "IssGDIDraw.h"
#include "IssCommon.h"
#include "issdebug.h"
#include "IssInternet.h"
#include <htmlctrl.h>
#include <aygshell.h>

#pragma comment( lib,"htmlview.lib")

#define WND_Name		_T("WndMoreApps")
#define WND_Class		_T("ClassMoreApps")
#define MENU_Height     (IsVGA()?46:23)
#define STATUS_Height   (IsVGA()?40:20)
#define TEXT_HEIGHT_Small (GetSystemMetrics(SM_CXICON)*13/32)
#define WM_FirtTime		WM_USER + 100
#define WM_Internet		WM_USER + 101
#define IDT_DelayClose  100

#define ONE_SHOT_MOD        0x2001
#define VK_TSOFT1_ID        0x09
#define VK_TSOFT2_ID        0x0a

#define URL_MoreApps	_T("http://www.panoramicsoft.com/mobile/wm/apps.php")
#define URL_MoreAppsSP	_T("http://www.panoramicsoft.com/mobile/wm/SPapps.php")

CDlgMoreApps::CDlgMoreApps(void)
:m_hWndHtml(NULL)
,m_eState(HTML_Done)
,m_bFirstTime(TRUE)
,m_bBtnDown(FALSE)
//,m_hKeyboardHook(NULL)
{
	//g_pdlgMoreApps = this;

	InitHTMLControl(m_hInst);

	if(IsSmartphone())
		m_oStr->StringCopy(m_szURL, URL_MoreAppsSP);
	else
		m_oStr->StringCopy(m_szURL, URL_MoreApps);

	m_hFont = CIssGDIEx::CreateFont(TEXT_HEIGHT_Small, FW_BOLD, TRUE);
}

CDlgMoreApps::~CDlgMoreApps(void)
{
	/*if (m_hKeyboardHook) 
	{
		UnhookWindowsHookEx(m_hKeyboardHook); 
		m_hKeyboardHook = NULL;
	}*/

	CIssGDIEx::DeleteFont(m_hFont);
}

BOOL CDlgMoreApps::Launch(HINSTANCE hInst, HWND hWndParent /*= NULL*/)
{
	if(hWndParent)
	{
		return CIssWnd::DoModal(hWndParent, hInst, IDD_DLG_Basic);
	}
	else
	{
		if(CIssWnd::Create(WND_Name, NULL, hInst, WND_Class, 0, 0, 0, 0, 0, WS_VISIBLE))
		{
			ShowWindow(m_hWnd, SW_SHOW);

			MSG msg;
			while( GetMessage( &msg,NULL,0,0 ) )
			{
				TranslateMessage( &msg );
				DispatchMessage ( &msg );
			}
			return TRUE;
		}
	}
	return TRUE;
}

BOOL CDlgMoreApps::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// create the HTML control
	m_hWndHtml = CreateWindow(WC_HTML, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 100, 100, m_hWnd, NULL, m_hInst, NULL);

	::RegisterHotKey(hWnd, VK_TSOFT1_ID, ONE_SHOT_MOD, VK_TSOFT1);
	::RegisterHotKey(hWnd, VK_TSOFT2_ID, ONE_SHOT_MOD, VK_TSOFT2);

	//g_pOldHTMLWnd  = (WNDPROC)SetWindowLong(m_hWndHtml, GWL_WNDPROC, (LONG)ProcHTMLWnd);

	m_bFirstTime = TRUE;	

	/*m_hKeyboardHook = SetWindowsHookExW( WH_KEYBOARD_LL, 
		(HOOKPROC)KBHook,	// address of hook procedure 
		m_hInst,			// handle to application instance 
		0 );*/

	return TRUE;
}

BOOL CDlgMoreApps::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if((HIWORD(lParam)==VK_TSOFT1 || HIWORD(lParam)==VK_TSOFT2) /*&& (0 != (MOD_KEYUP & LOWORD(lParam)))*/)
	{
		HWND hWndParent = GetParent(hWnd);
		CloseWindow();

		if(hWndParent)
			SetForegroundWindow((HWND)((ULONG) (hWndParent) | 0x00000001));
	}
	return TRUE;
}

BOOL CDlgMoreApps::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC,  rcClient, FALSE, TRUE, FALSE);

	RECT rcClip;
	GetClientRect(hWnd, &rcClip);

	//FillRect(m_gdiMem, rcClip, RGB(255,255,255));
	FillRect(m_gdiMem, rcClip, 0);

	RECT rc = m_rcStatusBar;
	rc.right--;
	rc.bottom--;
	FillRect(m_gdiMem, m_rcStatusBar, RGB(125,125,125));
	FrameRect(m_gdiMem, rc, 0);
	rc.left	+= (GetSystemMetrics(SM_CXSMICON)/4);
	switch(m_eState)
	{
	case HTML_Connecting:
		DrawText(m_gdiMem, _T("Connecting"), rc, DT_LEFT|DT_VCENTER, m_hFont, 0);
		break;
	case HTML_ConnectionFailed:
		DrawText(m_gdiMem, _T("Connection failed"), rc, DT_LEFT|DT_VCENTER, m_hFont, 0);
		break;
	case HTML_Downloading:
		DrawText(m_gdiMem, _T("Downloading"), rc, DT_LEFT|DT_VCENTER, m_hFont, 0);
		break;
	case HTML_DownloadingBytes:	
		if(m_oInet.GetBytesDownloaded() < 1000)
			m_oStr->Format(m_szTemp, _T("Downloading - %d b"), m_oInet.GetBytesDownloaded());
		else
			m_oStr->Format(m_szTemp, _T("Downloading - %d kb"), m_oInet.GetBytesDownloaded()/1000);
		DrawText(m_gdiMem, m_szTemp, rc, DT_LEFT|DT_VCENTER, m_hFont, 0);
		break;
	case HTML_DownloadFailed:
		DrawText(m_gdiMem, _T("Download failed"), rc, DT_LEFT|DT_VCENTER, m_hFont, 0);
		break;
	case HTML_Installing:
		DrawText(m_gdiMem, _T("Installing"), rc, DT_LEFT|DT_VCENTER, m_hFont, 0);
		break;
	case HTML_InstallingFailed:
		DrawText(m_gdiMem, _T("Installing failed"), rc, DT_LEFT|DT_VCENTER, m_hFont, 0);
		break;
	case HTML_Loading:
		DrawText(m_gdiMem, _T("Loading"), rc, DT_LEFT|DT_VCENTER, m_hFont, 0);
		break;
	}

	rc = m_rcMenuBar;
	rc.right --;
	rc.bottom -- ;
	GradientFillRect(m_gdiMem, m_rcMenuBar, RGB(133,133,133),  RGB(52,54,51), FALSE);
	FrameRect(m_gdiMem, rc, RGB(30,30,30));

	rc = m_rcMenuBar;
	rc.right /= 2;

	if(m_bBtnDown)
	{
		FrameRect(m_gdiMem, rc, RGB(255,255,255), 2);
		rc.top ++;
		rc.bottom ++;
		rc.left ++;
		rc.right ++;
	}

	DrawText(m_gdiMem, _T("Close"), rc, DT_CENTER|DT_VCENTER, m_hFont, RGB(255,255,255));

	// draw the separator
	rc = m_rcMenuBar;
	rc.left = m_rcMenuBar.right/2;
	rc.right = rc.left + 1;
	rc.top += IsVGA()?2:1;
	rc.bottom -= IsVGA()?2:1;

	AlphaFillRect(m_gdiMem, rc, RGB(80,80,80), 150);
	OffsetRect(&rc,1,0);
	AlphaFillRect(m_gdiMem, rc, RGB(200,200,200), 150);

	BitBlt(hDC,
		   rcClip.left, rcClip.top,
		   WIDTH(rcClip), HEIGHT(rcClip),
		   m_gdiMem,
		   rcClip.left, rcClip.top,
		   SRCCOPY);

	if(m_bFirstTime)
	{
		m_bFirstTime = FALSE;
		PostMessage(m_hWnd, WM_FirtTime, 0,0);
	}
	return TRUE;
}

BOOL CDlgMoreApps::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_FirtTime)
	{
		m_eState = HTML_Connecting;
		InvalidateRect(m_hWnd, &m_rcStatusBar, FALSE);
		UpdateWindow(m_hWnd);

		if(!GoOnline())
		{
			m_eState = HTML_ConnectionFailed;
			InvalidateRect(m_hWnd, &m_rcStatusBar, FALSE);
			UpdateWindow(m_hWnd);
			return TRUE;
		}

		// navigate to website
		PostMessage(m_hWndHtml, DTM_NAVIGATE, NULL, (LPARAM)m_szURL);

		m_eState = HTML_Loading;
		InvalidateRect(m_hWnd, &m_rcStatusBar, FALSE);
		UpdateWindow(m_hWnd);

		return TRUE;
	}
	else if(uiMessage == WM_Internet)
	{
		//DBG_OUT((_T("CDlgMoreApps::OnUser - WM_Internet")));
		switch(wParam)
		{
		case INET_CON_FAILED:
		case INET_CON_ERROR_OPEN:
		case INET_CON_ERROR_READ:
		case INET_CON_GET_FAILED:
		case INET_CON_ERROR_BUF_LEN_SMALL:
		case INET_DIALUP_CONNECTION_LOST:
		case INET_RETURN_BAD_REQUEST:
		case INET_RETURN_NOT_FOUND:
			m_eState = HTML_DownloadFailed;
			break;
		case INET_CON_WRITTEN_BYTES:
			m_eState = HTML_DownloadingBytes;
			break;
		case INET_CON_DOWNLOAD_COMPLETE:
		//case INET_CON_GET_SUCCESS:
			InstallFile();
			m_eState = HTML_Done;
			SetTimer(m_hWnd, IDT_DelayClose, 2000, NULL);
			break;
		default:
			return TRUE;
		}
		InvalidateRect(m_hWnd, &m_rcStatusBar, FALSE);
		UpdateWindow(m_hWnd);
	}

	return UNHANDLED;
}

BOOL CDlgMoreApps::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	m_rcMenuBar		= rcClient;
	m_rcMenuBar.top = m_rcMenuBar.bottom - MENU_Height;

	m_rcStatusBar			= rcClient;
	m_rcStatusBar.bottom	= m_rcMenuBar.top;
	m_rcStatusBar.top		= m_rcStatusBar.bottom - STATUS_Height;

	MoveWindow(m_hWndHtml,
			   0,0,
			   WIDTH(rcClient), m_rcStatusBar.top,
			   TRUE);

	m_gdiMem.Destroy();
	return TRUE;
}

BOOL CDlgMoreApps::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE /*|| LOWORD(wParam) == WA_CLICKACTIVE*/) 
	{
		SHFullScreen(hWnd, SHFS_HIDETASKBAR|SHFS_HIDESTARTICON|SHFS_HIDESIPBUTTON);
		MoveWindow(hWnd, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), TRUE); 
	}
	return TRUE;
}

/*BOOL CDlgMoreApps::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{   
	case VK_TSOFT1:
	case VK_TSOFT2:
		if(IsSmartphone())
			CloseWindow();
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgMoreApps::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{   
	case VK_TSOFT1:
	case VK_TSOFT2:
		CloseWindow();
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}*/

BOOL CDlgMoreApps::OnLButtonDown(HWND hWnd, POINT& pt)
{	
	if(PtInRect(&m_rcMenuBar, pt))
	{
		m_bBtnDown = TRUE;
		InvalidateRect(hWnd, &m_rcMenuBar, FALSE);
		UpdateWindow(hWnd);
	}
	return UNHANDLED;
}

BOOL CDlgMoreApps::OnLButtonUp(HWND hWnd, POINT& pt)
{
	if(m_bBtnDown)
	{
		m_bBtnDown = FALSE;
		InvalidateRect(hWnd, &m_rcMenuBar, FALSE);
		UpdateWindow(hWnd);
	}

	if(PtInRect(&m_rcMenuBar, pt))
	{
		HWND hWndParent = GetParent(hWnd);
		CloseWindow();

		if(hWndParent)
			SetForegroundWindow((HWND)((ULONG) (hWndParent) | 0x00000001));
	}
	return UNHANDLED;
}

BOOL CDlgMoreApps::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}



BOOL CDlgMoreApps::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnmh = (LPNMHDR)lParam;

	switch(lpnmh->code)
	{
	case NM_BEFORENAVIGATE:
		{
			NM_HTMLVIEW * pnmHTMLView = (NM_HTMLVIEW *)lParam;
			/*m_bLoading = TRUE;
			InvalidateRect(m_hWnd, &g_gui->rcTopBar, FALSE);
			UpdateWindow(m_hWnd);*/
			char* sz = (char*)pnmHTMLView->szTarget;
			m_oStr->StringCopy(m_szTemp, sz);
			DBG_OUT((_T("NM_BEFORENAVIGATE - %s"), m_szTemp));

			if(-1 != m_oStr->Find(m_szTemp, _T("http://www.wirelessappstore.com/apps")) ||
			   -1 != m_oStr->Find(m_szTemp, _T(".cab")))
			{
				PostMessage(m_hWndHtml, DTM_STOP, 0, 0);

				::UnregisterHotKey(m_hWnd, VK_TSOFT1_ID);
				::UnregisterHotKey(m_hWnd, VK_TSOFT2_ID);

				if(IDYES == MessageBox(m_hWnd, _T("Download now?"), _T("Install demo"), MB_YESNO|MB_ICONQUESTION))
					DownloadCabFile(m_szTemp);

				::RegisterHotKey(m_hWnd, VK_TSOFT1_ID, ONE_SHOT_MOD, VK_TSOFT1);
				::RegisterHotKey(m_hWnd, VK_TSOFT2_ID, ONE_SHOT_MOD, VK_TSOFT2);

				return TRUE;
			}
			else if(-1 != m_oStr->Find(m_szTemp, _T("details.aspx?appId=")))
			{
				PostMessage(m_hWndHtml, DTM_STOP, 0, 0);
				LaunchMarketplace(m_szTemp, m_hWnd);
				// close the window
				SetTimer(m_hWnd, IDT_DelayClose, 2000, NULL);
			}

			break;
		}
	case NM_NAVIGATECOMPLETE:
		//case NM_DOCUMENTCOMPLETE:
		{
			NM_HTMLVIEW * pnmHTMLView = (NM_HTMLVIEW *)lParam;
			m_eState = HTML_Done;
			InvalidateRect(m_hWnd, &m_rcStatusBar, FALSE);
			UpdateWindow(m_hWnd);
			char* sz = (char*)pnmHTMLView->szTarget;            
			m_oStr->StringCopy(m_szTemp, sz);
			DBG_OUT((_T("NM_NAVIGATECOMPLETE - %s"), m_szTemp));

			break;
		}
	case NM_TITLE:
		{
			NM_HTMLVIEW * pnmHTMLView = (NM_HTMLVIEW *)lParam;
			char* sz = (char*)pnmHTMLView->szTarget;

			/*m_oStr->Delete(&m_szTitle);
			m_szTitle = m_oStr->CreateAndCopy(sz);

			DBG_OUT((_T("NM_TITLE - %s"), m_szTitle));

			m_bLoading = FALSE;
			InvalidateRect(m_hWnd, &g_gui->rcTopBar, FALSE);
			UpdateWindow(m_hWnd);*/

			break;
		}
	default:
		return UNHANDLED;
	}

	return TRUE;
}

void CDlgMoreApps::LaunchMarketplace(TCHAR* szURL, HWND hWnd)
{
	CIssString* oStr = CIssString::Instance();
	TCHAR szTemp[STRING_MAX]  =_T("");

	int iIndex = oStr->Find(szURL, _T("appId="));
	if(iIndex == -1)
		return;

	int iLen = oStr->GetLength(szURL);
	oStr->StringCopy(szTemp, szURL, iIndex+6, iLen - iIndex - 6);
	oStr->Insert(szTemp, _T("/appid:"));

	// Marketplace app has to be installed first
	// \\Windows\\WMMarketplaceFullClient.exe
	if(!IsFileExists(_T("\\Windows\\WMMarketplaceFullClient.exe")))
	{
		if(IsFileExists(_T("\\Windows\\WMMarketplace.exe")))
		{
			PROCESS_INFORMATION pi;  
			ZeroMemory(&pi, sizeof(pi)); 

			CreateProcess(L"\\Windows\\WMMarketplace.exe",
				szTemp, NULL,  
				NULL, FALSE, 0, NULL, NULL,NULL, &pi);
		}
		else
		{
			// http://mobile.microsoft.com/windowsmobile/en-us/downloads/get-marketplace.mspx
			TCHAR szPocketIE[MAX_PATH];
			ZeroMemory(szPocketIE, sizeof(TCHAR)*MAX_PATH);

			SHGetSpecialFolderPath(NULL, szPocketIE, CSIDL_WINDOWS, FALSE);
			oStr->Concatenate(szPocketIE, _T("\\iexplore.exe"));

			// launch the browser with the Marketplace web url
			SHELLEXECUTEINFO sei = {0};
			sei.cbSize	    = sizeof(sei);
			sei.hwnd	    = hWnd;
			sei.nShow	    = SW_SHOWNORMAL;
			sei.lpFile      = szPocketIE;
			//sei.lpParameters= _T("http://mobile.microsoft.com/windowsmobile/en-us/downloads/get-marketplace.mspx");
			sei.lpParameters= szURL;
			ShellExecuteEx(&sei);
		}
	}
	else
	{
		PROCESS_INFORMATION pi;  
		ZeroMemory(&pi, sizeof(pi)); 

		CreateProcess(L"\\Windows\\WMMarketplaceFullClient.exe",
			szTemp, NULL,  
			NULL, FALSE, 0, NULL, NULL,NULL, &pi);
	}
}

void CDlgMoreApps::DownloadCabFile(TCHAR* szURL)
{
	m_oInet.Init(m_hWnd, WM_Internet, TRUE, FALSE, FALSE, TRUE);

	TCHAR szFileName[STRING_MAX];

	GetExeDirectory(szFileName);
	m_oStr->Concatenate(szFileName, _T("Installer.cab"));

	DeleteFile(szFileName);

	m_oStr->StringCopy(m_szURL, szFileName);

	m_oInet.Contact(szURL, szFileName, NULL, NULL, 0, FALSE, NULL, 0);
}

void CDlgMoreApps::InstallFile()
{
	SHELLEXECUTEINFO sei = {0};
	sei.cbSize	= sizeof(sei);
	sei.hwnd	= m_hWnd;
	sei.nShow	= SW_SHOWNORMAL;
	sei.lpFile	= m_szURL;
	ShellExecuteEx(&sei);

	/*m_eState = HTML_Installing;
	InvalidateRect(m_hWnd, &m_rcStatusBar, FALSE);
	UpdateWindow(m_hWnd);

	m_oStr->Format(m_szTemp, _T("\"%s\" /silent"), m_szURL);

	PROCESS_INFORMATION pi;  
	ZeroMemory(&pi, sizeof(pi)); 

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));

	si.cb = sizeof(si);

	BOOL bReturn = CreateProcess(L"\\Windows\\wceload.exe",
								  m_szTemp, NULL,  
								  NULL, FALSE, 0, NULL, NULL,&si, &pi);

	if(!bReturn)
	{
		m_eState = HTML_InstallingFailed;
		InvalidateRect(m_hWnd, &m_rcStatusBar, FALSE);
		UpdateWindow(m_hWnd);
		return;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD dwExitCode;
	GetExitCodeProcess(pi.hProcess, &dwExitCode);

	if(dwExitCode != 0)
	{
		m_eState = HTML_InstallingFailed;
		InvalidateRect(m_hWnd, &m_rcStatusBar, FALSE);
		UpdateWindow(m_hWnd);
		return;
	}

	m_eState = HTML_Done;
	InvalidateRect(m_hWnd, &m_rcStatusBar, FALSE);
	UpdateWindow(m_hWnd);

	MessageBeep(MB_ICONHAND);
	SetForegroundWindow((HWND)((ULONG) (m_hWnd) | 0x00000001));
	SHFullScreen(m_hWnd, SHFS_HIDETASKBAR|SHFS_HIDESTARTICON|SHFS_HIDESIPBUTTON);
	MoveWindow(m_hWnd, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), TRUE); */
}

void CDlgMoreApps::CloseWindow()
{
	::SetCursor(LoadCursor(NULL, IDC_WAIT));
	DestroyWindow(m_hWndHtml);
	m_oInet.Destroy();

	/*if (m_hKeyboardHook) 
	{
		UnhookWindowsHookEx(m_hKeyboardHook); 
		m_hKeyboardHook = NULL;
	}*/
	
	if(GetParent(m_hWnd) == NULL)
	{
		PostQuitMessage(0);
	}
	else
	{
		SafeCloseWindow(IDOK);
	}
	::SetCursor(NULL);
}

BOOL CDlgMoreApps::IsMoreAppsLink(TCHAR* szURL)
{
	CIssString* oStr = CIssString::Instance();

	if(0 == oStr->Compare(szURL, URL_MoreAppsSP) || 0 == oStr->Compare(szURL, URL_MoreApps))
		return TRUE;
	else
		return FALSE;
}

BOOL CDlgMoreApps::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(wParam == IDT_DelayClose)
	{
		KillTimer(m_hWnd, IDT_DelayClose);
		CloseWindow();
	}
	return TRUE;
}
