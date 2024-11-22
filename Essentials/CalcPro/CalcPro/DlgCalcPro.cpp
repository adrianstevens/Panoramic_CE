#include "StdAfx.h"
#include "DlgCalcPro.h"
#include "resource.h"
#include "IssRegistry.h"
#include "IssCommon.h"
#include "IssCalcInterface.h"
#include "IssHardwareKeys.h"
#include "IssFormatFractions.h"
#include "IssLocalisation.h"


#define REG_PS			_T("PlaySounds")
//for PC system tray
#define ID_TRAY_APP_ICON                5000
#define ID_TRAY_EXIT_CONTEXT_MENU_ITEM  3000
#define ID_TRAY_RESTORE_CONTEXT_MENU_ITEM  3001
#define WM_TRAYICON ( WM_USER + 1 )

UINT WM_TASKBARCREATED = 0 ;
//end system tray


CDlgCalcPro::CDlgCalcPro(HINSTANCE hInst)
:m_bPlaySounds(TRUE)
,m_oInterface(FALSE)
{
#ifndef UNDER_CE
	TCHAR className[] = TEXT( "tray icon class" );
	WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated"); 
#endif

	ZeroMemory(&m_sizeWindow, sizeof(SIZE));
	m_hInst = hInst;

	// read values from the registry
	DWORD dwVal	= 0;

	if(S_OK == GetKey(REG_KEY, REG_PS, dwVal))
		m_bPlaySounds = (BOOL)dwVal;

	m_oInterface	= new CIssCalcInterface(m_hInst);
	m_oCalc			= CIssCalculator::Instance();
	m_oFormat		= CIssFormatCalcDisplay::Instance();
}

CDlgCalcPro::~CDlgCalcPro(void)
{
	// write values to the registry
	DWORD dwVal;
	dwVal		= (DWORD)m_bPlaySounds;
	SetKey(REG_KEY, REG_PS, dwVal);

   Destroy();
}

BOOL CDlgCalcPro::Destroy()
{
#ifndef UNDER_CE
	//clean up the system tray
	Shell_NotifyIcon(NIM_DELETE, &m_notifyData);
#endif

	if(m_oInterface)
	{
		delete m_oInterface;
		m_oInterface = NULL;
	}

	if(m_oStr)
	{
		m_oStr->DeleteInstance();
		m_oStr = NULL;
	}

	if(m_oCalc)
	{
		m_oCalc->DeleteInstance();
		m_oCalc = NULL;
	}

	if(m_oFormat)
	{
		m_oFormat->DeleteInstance();
		m_oFormat = NULL;
	}

    CIssKineticList::DeleteAllContent();
	return TRUE;
}

BOOL CDlgCalcPro::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	// first check if we need to reload the images first
	if(m_sizeWindow.cx	== (rcClient.right - rcClient.left)&&
		m_sizeWindow.cy	== (rcClient.bottom - rcClient.top))
		return TRUE;

	// update our size structure
	m_sizeWindow.cx		= rcClient.right - rcClient.left;
	m_sizeWindow.cy		= rcClient.bottom - rcClient.top;

	m_gdiMem.Destroy();

	if(m_oInterface)
	{
		m_oInterface->OnSize(hWnd, wParam, lParam);

		CIssCalcInterface* oInt = (CIssCalcInterface*)m_oInterface;

		if(oInt->GetDeviceType() == DEVTYPE_SPLand)
		{
			if(m_oEdit.GetWnd() == NULL)
			{
				//now create the edit control
				DWORD dwStyle = WS_CHILD;//nice and simple
				m_oEdit.Create(m_hWnd, m_hInst, dwStyle);
			}
			SetFocus(m_oEdit.GetWnd());		
		}
		else
		{
			m_oEdit.Destroy();
			SetFocus(m_hWnd);
		}
	}
	return TRUE;
}

BOOL CDlgCalcPro::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL bRet = UNHANDLED;
#ifdef UNDER_CE
    HWND hWndSip = NULL;

    bRet =  m_oInterface->OnActivate(hWnd, wParam, lParam);
	if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
	{	//this should only exist when using a smartphone in landscape orientation (for now anyways)
#ifdef WIN32_PLATFORM_WFSP

        SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 

        SetForegroundWindow(m_hWnd);
        SetFocus(m_hWnd);

        if(m_oEdit.GetWnd() != NULL)
            m_oEdit.Destroy();
            
        DWORD dwStyle = WS_CHILD;//nice and simple
        m_oEdit.Create(m_hWnd, m_hInst, dwStyle);
        SetFocus(m_oEdit.GetWnd());
#else

        if(m_oInterface->GetFullscreen())
        {
            SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
            RECT rc; 
            SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
            MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
        }
        else
        {
            SHFullScreen(m_hWnd, SHFS_HIDESIPBUTTON);  
        }

        
        //get rid of the damn sip too ... just in case
        HWND hWndSip = FindWindow(_T("MS_SIPBUTTON"), NULL);
        if(hWndSip)
            ShowWindow(hWndSip, SW_HIDE);
        
        ShowWindow(m_hWnd, SW_SHOW);
        SetForegroundWindow(m_hWnd);
        SetFocus(m_hWnd);

#endif
    }
    else if(LOWORD(wParam) == WA_INACTIVE)
    {
//        HWND hWndSip = FindWindow(_T("MS_SIPBUTTON"), NULL);
  //      ShowWindow(hWndSip, SW_SHOW);
    }
#endif

    // check if we're running a legit copy
#ifdef UNDER_CE
    m_oKey.Init(_T("SOFTWARE\\Panoramic\\PanoCalcPro"), _T("SOFTWARE\\Pano\\PCP2"), 0x0, 0x2B00B1E5, 0x6498ABC1, 4, TRUE);
#else
     m_oKey.Init(_T("SOFTWARE\\Panoramic\\PanoCalcPro"), _T("SOFTWARE\\Pano\\PCP2"), 0x0, 0x2B00B1E5, 0x6498ABC1, 4, TRUE, HKEY_CURRENT_USER);
#endif
   
    m_oKey.CheckSetKey();
    m_oKey.CheckDemo();



    if(m_oInterface)
        m_oInterface->SetExpired(m_oKey.m_bGood == FALSE);

	return bRet;
}

BOOL CDlgCalcPro::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	::SetCursor(LoadCursor(NULL, IDC_WAIT));

    //let's get iSSLocalisation going
	m_oStr->SetResourceInstance(m_hInst);
    g_cLocale.Init(REG_KEY, m_hInst, SUPPORT_English|SUPPORT_Dutch|SUPPORT_German|SUPPORT_Spanish|SUPPORT_Portuguese|SUPPORT_French|SUPPORT_Japanese|SUPPORT_Italian, TRUE);

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
#ifdef WIN32_PLATFORM_WFSP
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
#else
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR|SHIDIF_SIPDOWN;
#endif
	DlgInfo.hDlg	= hWnd;
    
	
/*	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= hWnd;
	mbi.nToolBarId	= IDR_MENU_Calculator;
	mbi.hInstRes	= m_hInst;
	if(!SHCreateMenuBar(&mbi))
	{
		int i = 0;
	}*/

    SHInitDialog(&DlgInfo);
    
	//handle the back button
#ifdef WIN32_PLATFORM_WFSP
//	::SendMessage(	mbi.hwndMB, 
//		SHCMBM_OVERRIDEKEY, 
//		VK_TBACK, 
//		MAKELPARAM(SHMBOF_NODEFAULT | SHMBOF_NOTIFY, SHMBOF_NODEFAULT | SHMBOF_NOTIFY));

	//and change the keyboard mode
	//we won't be too aggressive about it
	//BOOL bRet = SendMessage(m_hWnd, EM_SETINPUTMODE, 0, EIM_TEXT | IMMF_SHIFT );
#endif

#endif

	if(m_oInterface)
		m_oInterface->OnInit(m_hWnd, m_hInst);

	//now create the edit control
//	DWORD dwStyle = WS_CHILD;//nice and simple
//	m_oEdit.Create(m_hWnd, m_hInst, dwStyle);
//	SetFocus(m_oEdit.GetWnd());

#ifdef WIN32_PLATFORM_WFSP
//	BOOL bRet = SendMessage(m_oEdit.GetWnd(), EM_SETINPUTMODE, 0, EIM_TEXT );

//	BOOL bRet = Edit_SetInputMode(m_oEdit.GetWnd(), IM_NUMBERS);
    SHFullScreen(m_hWnd, SHFS_HIDESIPBUTTON); 
    RECT rc; 
    GetClientRect(hWnd, &rc);
    SetRect(&rc, rc.top, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)-rc.top); 
    MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
#endif


#ifdef WIN32_PLATFORM_PSPC
    if(m_oInterface->GetFullscreen())
    {
        SHFullScreen(m_hWnd, SHFS_HIDESIPBUTTON); 
        RECT rc; 
        GetClientRect(hWnd, &rc);
        SetRect(&rc, rc.top, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)-rc.top); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
    }
#endif

    //we'll load in the menubar image here
    m_gdiMenu.LoadImage(IDR_PNG_MenuBack, hWnd, m_hInst, FALSE);

#ifndef UNDER_CE
	memset(&m_notifyData, 0, sizeof( NOTIFYICONDATA )) ;
	m_notifyData.cbSize = sizeof(NOTIFYICONDATA);
	m_notifyData.hWnd = hWnd;
	m_notifyData.uID = ID_TRAY_APP_ICON;
	m_notifyData.uFlags = NIF_ICON | NIF_MESSAGE | 	NIF_TIP;
	m_notifyData.uCallbackMessage = WM_TRAYICON; //this message must be handled in hwnd's window procedure. more info below.
	//m_notifyData.hIcon = (HICON)LoadImage( NULL, _T("calcpro.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE  ) ;
	m_notifyData.hIcon = (HICON)LoadImage( m_hInst, MAKEINTRESOURCE(IDI_CALCPRO), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR  ) ;
	m_oStr->StringCopy(m_notifyData.szTip, _T("Panoramic Calc Pro\r\nwww.panoramicsoft.com"));

    //HCURSOR hCursor = ::SetCursor(LoadCursor(NULL, IDC_ARROW));
#ifndef UNDER_CE
	POINT pt;
	DWORD dwTemp;

	if(S_OK == GetKey(REG_KEY, _T("XPos"), dwTemp))
		pt.x = dwTemp;
	else
		pt.x = 20;
	if(S_OK == GetKey(REG_KEY, _T("YPos"), dwTemp))
		pt.y = dwTemp;
	else 
		pt.y = 20;

	SetWindowPos(hWnd, NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE);
#endif
    //MoveWindow(hWnd, 200, 200, WINDOW_WIDTH, WINDOW_HEIGHT, FALSE);
#endif

	InvalidateRect(hWnd, NULL, FALSE);

	SetCursor(NULL);
	return TRUE;
}

//code not used since we never registered the hot keys ... doesn't really hurt to leave it 

BOOL CDlgCalcPro::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef WIN32_PLATFORM_WFSP
	//if(HIWORD(lParam)==VK_TBACK && (0 != (MOD_KEYUP & LOWORD(lParam))))
	if(HIWORD(lParam) == VK_ESCAPE)
	{
		if(m_oInterface)
			m_oInterface->OnBackKey();
	//	DebugOut(_T("WParam: %i  LParam: %i"), wParam, lParam);
		return TRUE;
	}
#endif
    if(HIWORD(lParam) == VK_TSOFT1 || HIWORD(lParam) == VK_TSOFT2)
    {
        if(m_oInterface)
        {   
            m_oInterface->OnKeyDown(hWnd, wParam, lParam);
            return TRUE;
        }
    }
    
	return UNHANDLED;
}



BOOL CDlgCalcPro::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	RECT rcClip;
	GetClipBox(hDC, &rcClip);

	// if our memory buffer has not been created yet
	if(!m_gdiMem.GetDC())
	{
	//	FillRect(hDC, rcClient, RGB(255,255,255));
		RECT rcTemp;
        GetClientRect(hWnd, &rcTemp);

		m_gdiMem.Create(hDC, rcTemp, FALSE, TRUE);
	}
	if(m_oInterface)
		m_oInterface->Draw(m_gdiMem, rcClip);

	// draw all to the screen
	BitBlt( hDC,
			rcClip.left, rcClip.top, 
			WIDTH(rcClip),
			HEIGHT(rcClip),
			m_gdiMem.GetDC(),
			rcClip.left,
			rcClip.top,
			SRCCOPY);

	return TRUE;
}

BOOL CDlgCalcPro::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(m_oKey.m_bGood == FALSE)
        return UNHANDLED;

	if(m_oInterface)
		return m_oInterface->OnChar(hWnd, wParam, lParam);
	else
		return UNHANDLED;
}

BOOL CDlgCalcPro::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(m_oKey.m_bGood == FALSE)
    {
        if(uiMessage != WM_LEFT_SOFTKEY)
            return UNHANDLED;
    }

	switch(uiMessage)
	{
	case WM_TRAYICON:
#ifndef UNDER_CE
		switch(wParam)
		{
		case ID_TRAY_APP_ICON:
			break;
		}

		if(lParam == WM_LBUTTONUP)
			TrayRestore();
		else if(lParam == WM_RBUTTONDOWN)
		{
			//context menu ... restore or close in my opinion
			// Get current mouse position.
			POINT curPoint ;

			HMENU hMenu = CreatePopupMenu();
			AppendMenu(hMenu, MF_STRING, ID_TRAY_RESTORE_CONTEXT_MENU_ITEM,  TEXT( "Restore" ) );
			AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT_CONTEXT_MENU_ITEM,  TEXT( "Exit" ) );

			GetCursorPos( &curPoint ) ;
			UINT clicked = TrackPopupMenu(
				hMenu,
				TPM_RETURNCMD | TPM_NONOTIFY, // don't send me WM_COMMAND messages about this window, instead return the identifier of the clicked menu item
				curPoint.x,
				curPoint.y,
				0,
				m_hWnd,
				NULL);

			if (clicked == ID_TRAY_EXIT_CONTEXT_MENU_ITEM)
			{
				// quit the application.
				PostQuitMessage( 0 ) ;
			}
			else if(clicked == ID_TRAY_RESTORE_CONTEXT_MENU_ITEM)
			{
				TrayRestore();
			}
			DestroyMenu(hMenu);
		}
		break;
	
#endif
	default:
		if(m_oInterface)
			return m_oInterface->OnUser(hWnd, uiMessage, wParam, lParam);
	    break;
	}
	return FALSE;
}

BOOL CDlgCalcPro::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(m_oInterface)
		return m_oInterface->OnLButtonDown(hWnd, pt);
	else
		return UNHANDLED;
}

BOOL CDlgCalcPro::OnLButtonUp(HWND hWnd, POINT& pt)
{	
	if(m_oInterface)
		return m_oInterface->OnLButtonUp(hWnd, pt);
	else
		return UNHANDLED;
}

BOOL CDlgCalcPro::OnMouseMove(HWND hWnd, POINT& pt)
{
	if(m_oInterface)
		return m_oInterface->OnMouseMove(hWnd, pt);
	else
		return UNHANDLED;
}
BOOL CDlgCalcPro::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(m_oInterface)
		return m_oInterface->OnKeyDown(hWnd, wParam, lParam);
	else
		return UNHANDLED;
}

BOOL CDlgCalcPro::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(m_oInterface)
		return m_oInterface->OnKeyUp(hWnd, wParam, lParam);
	else
		return UNHANDLED;
}

BOOL CDlgCalcPro::OnHandleNavBtn(WPARAM wParam)
{
	switch(wParam)
	{
	case VK_LEFT:
		break;
	case VK_RIGHT:
		break;
	case VK_UP:
	    break;
	case VK_DOWN:
	    break;
	}
	InvalidateRect(m_hWnd, NULL, FALSE);
	return TRUE;
} 

BOOL CDlgCalcPro::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(m_oInterface != NULL)
		return m_oInterface->OnTimer(hWnd, wParam, lParam);
	return FALSE;
}
BOOL CDlgCalcPro::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return CIssWnd::OnDestroy(hWnd, wParam, lParam);
}


BOOL CDlgCalcPro::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(m_oInterface != NULL)
		return m_oInterface->OnCommand(hWnd, wParam, lParam);
	return FALSE;
}

void CDlgCalcPro::PlaySounds(TCHAR* szWave)
{
	if(m_bPlaySounds)
		PlaySound(szWave, m_hInst, SND_RESOURCE | SND_ASYNC | SND_NOWAIT);
}

#ifndef UNDER_CE
BOOL CDlgCalcPro::OnSysCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch( wParam & 0xfff0 )  // (filter out reserved lower 4 bits:  see msdn remarks http://msdn.microsoft.com/en-us/library/ms646360(VS.85).aspx)
	{
	case SC_MINIMIZE:
	//case SC_CLOSE: 
		TrayMinimize();
		return TRUE;
		break;
	default:
		return UNHANDLED;
		break;
	}
}


void CDlgCalcPro::TrayMinimize()
{
	// add the icon to the system tray
	Shell_NotifyIcon(NIM_ADD, &m_notifyData);

	// ..and hide the main window
	ShowWindow(m_hWnd, SW_HIDE);
}

// Basically bring back the window (SHOW IT again)
// and remove the little icon in the system tray.
void CDlgCalcPro::TrayRestore()
{
	// Remove the icon from the system tray
	Shell_NotifyIcon(NIM_DELETE, &m_notifyData);

	// ..and show the window
	ShowWindow(m_hWnd, SW_SHOW);
}

#endif