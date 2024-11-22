// IssWnd.cpp: implementation of the CIssWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "IssWnd.h"

#define ISS_WND_Default		_T("IssWndDefault")
#define ISS_CLASS_Default	_T("IssClassDefault")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIssWnd::CIssWnd()
:m_hWnd(NULL)
,m_hInst(NULL)
,m_aClass(0)
,m_eWndType(WND_Dialog)
,m_szClassName(NULL)
,m_bHandlePaint(TRUE)
{
	m_oStr = CIssString::Instance();
}

CIssWnd::~CIssWnd()
{
	Destroy();
}

BOOL CIssWnd::Destroy()
{
	if(m_hWnd && IsWindow(m_hWnd))
	{
        HWND hWndTemp = m_hWnd;
        m_hWnd = NULL;
		if(m_eWndType == WND_Dialog)
			EndDialog(hWndTemp, IDOK);
		else
			DestroyWindow(hWndTemp);
	}

	if(m_aClass != 0 && !m_oStr->IsEmpty(m_szClassName))
	{
		UnregisterClass(m_szClassName, m_hInst);
		m_aClass = 0;
	}

	if(m_szClassName)
	{
		delete [] m_szClassName;
		m_szClassName = NULL;
	}

	return TRUE;
}

void CIssWnd::SafeCloseWindow(UINT uiReturn)
{
    if(!m_hWnd || !IsWindow(m_hWnd))
        return;

    MSG stMsg = { 0 };
    while( PeekMessage( &stMsg, m_hWnd, 0, 0, PM_REMOVE ))
    {
        TranslateMessage( &stMsg );
        DispatchMessage( &stMsg );
    }	

    if(m_eWndType == WND_Dialog)
        EndDialog(m_hWnd, uiReturn);
    else
        PostQuitMessage(uiReturn);
}


BOOL CIssWnd::DoModal(HWND hWndParent, HINSTANCE hInst,UINT	uiDlgID)
{
#ifdef UNDER_CE
	// see if the window is up already
	if(CheckWindow(NULL))
		return TRUE;

	// destroy any memory if it exists
	Destroy();

	// save the instance
	m_hInst		= hInst;
	m_eWndType	= WND_Dialog;

	// create the dialog
	return (BOOL)DialogBoxParam( m_hInst,
						   MAKEINTRESOURCE(uiDlgID),
						   hWndParent,
						   &ProcWnd,
						   (LPARAM)this);	
#else
    RECT rc;
    GetClientRect(hWndParent, &rc);


    if(Create(_T("Modal"), NULL, hInst, _T("Modal"),
        CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
        0,
        DS_MODALFRAME | WS_CAPTION | WS_SYSMENU))
    {
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
        ShowWindow(m_hWnd, SW_SHOW);

        MSG msg;
        while( GetMessage( &msg,NULL,0,0 ) ) 
        {
            TranslateMessage( &msg );
            DispatchMessage ( &msg );
        }
    }
    return TRUE;
#endif
}

BOOL CIssWnd::DoModalNoRes(HWND hWndParent, HINSTANCE hInst, DWORD dwStyle)
{
	// see if the window is up already
	if(CheckWindow(NULL))
		return TRUE;

	// destroy any memory if it exists
	Destroy();

	// save the instance
	m_hInst		= hInst;
	m_eWndType	= WND_Dialog;
	
	HGLOBAL hgbl;
	LPDLGTEMPLATE lpdt;
	LPWORD lpw;
	LPWSTR lpwsz;
	LRESULT ret;
	char nchar;
	hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
	if (!hgbl)
	   return -1;

	lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);

	// Define a dialog box.

	lpdt->style = dwStyle;
	lpdt->cdit = 0;  // number of controls
	lpdt->x  = 0;  lpdt->y  = 0;
	lpdt->cx = 100; lpdt->cy = 100;

	lpw = (LPWORD) (lpdt + 1);
	*lpw++ = 0;   // no menu
	*lpw++ = 0;   // predefined dialog box class (by default)

	lpwsz = (LPWSTR) lpw;
	nchar = 1+ MultiByteToWideChar (CP_ACP, 0, "My Dialog", -1,
								   lpwsz, 50);
	lpw   += nchar;

	GlobalUnlock(hgbl);
	ret = DialogBoxIndirectParam(hInst,
						   (LPDLGTEMPLATE) hgbl,
						   hWndParent,
						   (DLGPROC) &ProcWnd,
						   (LPARAM)this);
	GlobalFree(hgbl);

	return (BOOL)ret;

}

BOOL CIssWnd::DoModaless(HWND hWndParent, HINSTANCE	hInst, UINT	uiDlgID)
{
	// see if the window is up already
	if(CheckWindow(NULL))
		return TRUE;

	// destroy any memory if it exists
	Destroy();

	// save the instance
	m_hInst		= hInst;
	m_eWndType	= WND_Dialog;

	if(m_hWnd = CreateDialogParam(m_hInst,
							 MAKEINTRESOURCE(uiDlgID),
							 hWndParent,
							 ProcWnd,
							 (LPARAM)this))
		 ShowWindow(m_hWnd, SW_SHOW);
	
	return (m_hWnd?TRUE:FALSE);
}

BOOL CIssWnd::Create(TCHAR*		szWindowName,
					 HWND		hWndParent,
					 HINSTANCE	hInst,
					 TCHAR*		szClassName,
					 int		x,
					 int		y,
					 int		cx,
					 int		cy,
					 DWORD		dwExtStyle,
					 DWORD		dwStyle,
					 HMENU		hMenu /* NULL */,
                     UINT       uiIcon /* 0 */)
{
	// clean any memory if it exists
	Destroy();

	// save the instance
	m_hInst		= hInst;
	m_eWndType	= WND_Window;
	
	if(!m_oStr->IsEmpty(szClassName))
		m_szClassName = m_oStr->CreateAndCopy(szClassName);
	else
		m_szClassName = m_oStr->CreateAndCopy(ISS_CLASS_Default);

		// see if the window is up already
	if(CheckWindow(szWindowName))
		return FALSE;

	// register the window
	m_aClass = RegisterMyClass(uiIcon);


	// initiate the window
	m_hWnd = CreateWindowEx(dwExtStyle,
							(LPCTSTR)m_aClass, 
							//(m_oStr->IsEmpty(szWindowName)?ISS_WND_Default:szWindowName), 
							szWindowName,
							dwStyle,
							x,y, 
							cx,cy,
							hWndParent, 
							hMenu, 
							m_hInst, 
							(LPVOID)this);

	return (m_hWnd?TRUE:FALSE);
}

BOOL CIssWnd::CreateChildWindow(HINSTANCE hInstance,
								HWND hWndParent,
								UINT uiResource)
{
	Destroy();

	m_hInst = hInstance;
	m_hWnd = CreateDialogIndirectParam(m_hInst, 
									   //MAKEINTRESOURCE(uiResource), 
									   (LPCDLGTEMPLATEA)LoadResource(hInstance, FindResource(hInstance, MAKEINTRESOURCE(uiResource), RT_DIALOG)),
									   hWndParent, 
									   ProcWnd, 
									   (LPARAM)this);

	return (m_hWnd?TRUE:FALSE);
}

ATOM CIssWnd::RegisterMyClass(UINT uiIcon)
{
	WNDCLASS	wc;

    HICON hIcon = NULL;

    if(uiIcon != 0)
        hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(uiIcon));

#ifdef UNDER_CE
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
#else
	wc.style			= CS_HREDRAW | CS_VREDRAW;
#endif
    wc.lpfnWndProc		= (WNDPROC) ProcWnd;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= m_hInst;
    wc.hIcon			= hIcon;
    wc.hCursor			= 0;
    wc.hbrBackground	= NULL;
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= m_szClassName;

    //if you find desktop issues we may want to update to RegisterClassEx
    //http://msdn.microsoft.com/en-us/library/bb384843.aspx 
    //but so far we're fine
	return RegisterClass(&wc);
}

BOOL CIssWnd::CheckWindow(TCHAR* szWindowName)
{
	// if the window exists already
	HWND hWndOther = NULL;
	if(!m_oStr->IsEmpty(szWindowName))
		hWndOther = FindWindow(m_szClassName, szWindowName);

	if(m_hWnd || hWndOther)
	{
		// set focus to foremost child window
		// The "| 0x01" is used to bring any owned windows to the foreground and
		// activate them.
		SetForegroundWindow((HWND)((ULONG) (m_hWnd?m_hWnd:hWndOther) | 0x00000001));
		

		return TRUE;
	}
	else
		return FALSE;
}

BOOL CIssWnd::ProcWnd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CIssWnd*	dlgBase = NULL;
	POINT		pt;
	BOOL		bReturn = UNHANDLED;

#ifndef SOLEUS
	if(message == WM_CREATE)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>( lParam ); 
		dlgBase = reinterpret_cast<CIssWnd*>( lpcs->lpCreateParams ); 
#ifdef UNDER_CE
		ASSERT( dlgBase && hWnd ); 
#endif
		SetWindowLong( hWnd, GWL_USERDATA, reinterpret_cast<long>( dlgBase ) ); 
		//The Windows window knows about this object.  Now make sure this object knows about the Windows window handle 
        dlgBase->m_hWnd = hWnd;
		return dlgBase->OnInitDialog(hWnd, wParam, lParam);
	}
	else
#endif
	if(message == WM_INITDIALOG)
	{
		dlgBase = reinterpret_cast<CIssWnd*>( lParam ); 
#ifdef UNDER_CE
		ASSERT( dlgBase && hWnd );
#endif
		SetWindowLong( hWnd, GWL_USERDATA, reinterpret_cast<long>( dlgBase ) ); 
		//The Windows window knows about this object.  Now make sure this object knows about the Windows window handle 
        dlgBase->m_hWnd = hWnd;
		return dlgBase->OnInitDialog(hWnd, wParam, lParam);
	}

	//Call the real WndProc 
	dlgBase = reinterpret_cast<CIssWnd*>( GetWindowLong( hWnd, GWL_USERDATA ) ); 
	
	if( !dlgBase || hWnd != dlgBase->m_hWnd ) 
		//better safe than sorry
		return DefWindowProc( hWnd, message, wParam, lParam ) ;
	else if(message >= WM_USER)
	{
		// custom message
		bReturn = dlgBase->OnUser(hWnd, message, wParam, lParam);
		if(UNHANDLED == bReturn)
			return dlgBase->ProcDefault(hWnd, message, wParam, lParam);
		else
			return bReturn;
	}

	// check all the messages here
	switch(message)
	{
	case WM_HSCROLL:	bReturn = dlgBase->OnHorizontalScroll(hWnd, wParam, lParam);		break;
	case WM_VSCROLL:	bReturn  = dlgBase->OnVerticalScroll(hWnd, wParam, lParam);			break;
	case WM_PAINT:		bReturn  = dlgBase->OnPaint(hWnd, wParam, lParam);					break;
	case WM_ERASEBKGND:	bReturn  = dlgBase->OnEraseBkgnd(hWnd, wParam, lParam);				break;
	case WM_COMMAND:	bReturn  = dlgBase->OnCommand(hWnd, wParam, lParam);				break;
	case WM_SYSCOMMAND: bReturn  = dlgBase->OnSysCommand(hWnd, wParam, lParam);				break;
	case WM_HOTKEY:		bReturn  = dlgBase->OnHotKey(hWnd, wParam, lParam);					break;
	case WM_ACTIVATE:	bReturn  = dlgBase->OnActivate(hWnd, wParam, lParam);				break;
	case WM_CREATE:		bReturn  = dlgBase->OnCreate(hWnd, wParam, lParam);					break;
	case WM_TIMER:		bReturn  = dlgBase->OnTimer(hWnd, wParam, lParam);					break;
	case WM_SETTINGCHANGE:bReturn  = dlgBase->OnSettingChange(hWnd, wParam, lParam);		break;
	case WM_SIZE:		bReturn  = dlgBase->OnSize(hWnd, wParam, lParam);					break;
	case WM_NOTIFY:		bReturn  = dlgBase->OnNotify(hWnd, wParam, lParam);					break;	
	case WM_MEASUREITEM:bReturn  = dlgBase->OnMeasureItem(hWnd, wParam, lParam);			break;
	case WM_DRAWITEM:	bReturn  = dlgBase->OnDrawItem(hWnd, wParam, lParam);				break;
	case WM_DESTROY:	bReturn  = dlgBase->OnDestroy(hWnd, wParam, lParam);	dlgBase->m_hWnd = NULL; break;
	case WM_CHAR:		bReturn  = dlgBase->OnChar(hWnd, wParam, lParam);					break;
	case WM_KEYDOWN:	bReturn  = dlgBase->OnKeyDown(hWnd, wParam, lParam);				break;
	case WM_KEYUP:		bReturn  = dlgBase->OnKeyUp(hWnd, wParam, lParam);					break;
	case WM_KILLFOCUS:	bReturn  = dlgBase->OnLostFocus(hWnd, wParam, lParam);				break;
	case WM_SETFOCUS:	bReturn  = dlgBase->OnGetFocus(hWnd, wParam, lParam);				break;
#ifdef UNDER_CE
	case WM_HIBERNATE:	bReturn  = dlgBase->OnHibernate(hWnd, wParam, lParam);				break;
#else
	case MM_MCINOTIFY:  bReturn = dlgBase->OnMCNotify(hWnd, wParam, lParam);				break;
#endif
	case WM_MOVE: bReturn = dlgBase->OnMove(hWnd, wParam, lParam);						break;
	case WM_CLOSE: bReturn = dlgBase->OnClose(hWnd, wParam, lParam);						break;
	case WM_QUIT: bReturn = dlgBase->OnClose(hWnd, wParam, lParam);							break;
	case WM_SYSCOLORCHANGE:	bReturn  = dlgBase->OnSysColorChange(hWnd, wParam, lParam);		break;
	case WM_CTLCOLORSTATIC: bReturn = dlgBase->OnCtlColorStatic(hWnd, wParam, lParam);		break;
	case WM_CTLCOLORBTN: bReturn	= dlgBase->OnCtlColorBtn(hWnd, wParam, lParam);			break;
	case WM_COPYDATA:	bReturn = dlgBase->OnCopyData(hWnd, wParam, lParam);				break;
	case WM_HELP: bReturn = dlgBase->OnHelp(hWnd, wParam, lParam);							break;
	case WM_ENTERMENULOOP: bReturn = dlgBase->OnEnterMenuLoop(hWnd, wParam, lParam);		break;
	case WM_EXITMENULOOP: bReturn = dlgBase->OnExitMenuLoop(hWnd, wParam, lParam);			break;
	case WM_CAPTURECHANGED: bReturn = dlgBase->OnCaptureChanged(hWnd, wParam, lParam);		break;
	case WM_LBUTTONDOWN:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		bReturn  = dlgBase->OnLButtonDown(hWnd, pt);
		break;
	case WM_LBUTTONUP:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		bReturn  = dlgBase->OnLButtonUp(hWnd, pt);
		break;
	case WM_MOUSEMOVE:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		bReturn  = dlgBase->OnMouseMove(hWnd, pt);
		break;
	default:	
		bReturn  = UNHANDLED;
		break;
	}

	// if we didn't handle it properly
	if(bReturn == UNHANDLED )
		return dlgBase->ProcDefault(hWnd, message, wParam, lParam);
	
	return bReturn;
}

BOOL CIssWnd::OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(!m_bHandlePaint)
		return UNHANDLED;

	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hWnd, &ps);
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	// call the useful function
	BOOL bReturn = OnPaint(hWnd, hDC, rcClient);

	EndPaint(hWnd, &ps);
	return bReturn;
}







