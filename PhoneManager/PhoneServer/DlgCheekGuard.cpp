#include "StdAfx.h"
#include "IssGDIDraw.h"
#include "DlgCheekGuard.h"
#include "resourceppc.h"
#include "IssCommon.h"
#include "IssGDIFx.h"
#include "IssRegistry.h"
#include "GeneralOptions.h"
#include "CommonDefines.h"

#define WM_FullScreen	WM_USER + 200

#define TEXT_COLOR		0xFFFFFF

#define IDT_TIMER		1
#define IDT_TIMER_RING	2
#define IDT_TIMER_INVAL	3

#define IDT_KILL_POPUP  4

CDlgCheekGuard* g_dlgCheekguard = NULL;

extern "C" 
{ 
	typedef LRESULT (CALLBACK* HOOKPROC)(int code, WPARAM wParam, LPARAM lParam); 

#define WH_KEYBOARD_LL   		20
#define HC_ACTION 0 


	typedef struct tagKBDLLHOOKSTRUCT 
	{ 
		DWORD vkCode;		// virtual key code 
		DWORD scanCode;		// scan code 
		DWORD flags;		// flags 
		DWORD time;			// time stamp for this message 
		DWORD dwExtraInfo;	// extra info from the driver or keybd_event 
	} KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT; 


	HHOOK WINAPI SetWindowsHookExW( int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId ); 
	BOOL WINAPI UnhookWindowsHookEx( HHOOK hhk ); 
	LRESULT WINAPI CallNextHookEx( HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam ); 
	LRESULT CALLBACK KeyboardProc( int code,  WPARAM wParam, LPARAM lParam ); 
#define SetWindowsHookEx  SetWindowsHookExW 


} 

HHOOK g_hKeyboardHook = 0; 

LRESULT CALLBACK KBHook(int code, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT pKeyStruct = (PKBDLLHOOKSTRUCT)lParam; 
	LRESULT    lResult = 0; 

	if ((code   == HC_ACTION) && (wParam == /*WM_KEYDOWN*/WM_KEYUP) ) 
	{ 
		switch(pKeyStruct->vkCode ) 
		{ 
		case VK_TTALK: 
			{
				// if phone is ringing, and if this window is visible...
				// user key down brings phone window to foreground...
				// we give another key down to answer
				static int s_iKeySimCount = 0;

				if(!s_iKeySimCount)
				{
					if(g_dlgCheekguard->GetCallState() == CSTATE_Ringing)
					{
						if(g_dlgCheekguard && ::IsWindowVisible(g_dlgCheekguard->GetWnd()))
						{
							s_iKeySimCount++;
							DebugOut(_T("simulating VK_TTALK key action"));
							// Simulate a key press
							keybd_event( VK_TTALK,
								0x45,
								KEYEVENTF_EXTENDEDKEY | 0,
								0 );

							// Simulate a key release
							keybd_event( VK_TTALK,
								0x45,
								KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
								0);
						}
					}
				}
				else
				{
					s_iKeySimCount = 0;
				}
			}
			break;
		//case VK_TEND: 
		//case VK_LEFT: 
		//case VK_RIGHT: 
		//case VK_UP:
		//case VK_DOWN:
		//case VK_RETURN:
		default:
			break; 
		} 
	} 

	if (0 == lResult) 
		lResult = CallNextHookEx(g_hKeyboardHook, code,  wParam, lParam); 

	// Return true if hook was handled, false to pass it on 
	return lResult; 
}


CDlgCheekGuard::CDlgCheekGuard(void)
:m_bFirstPic(FALSE)
,m_bMovingArrow(FALSE)
,m_hFontText(NULL)
,m_oCall(NULL)
,m_iCallDurration(0)
,m_iRingCount(0)
, m_bScreenLocked(FALSE)
//, m_gdiPicture(NULL)
//,m_eState(CSTATE_Ringing)
{
	g_dlgCheekguard = this;
	m_hFontText = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
}

CDlgCheekGuard::~CDlgCheekGuard(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);

	//we are done with the hook. now uninstall it.
	//DeactivateKBHook();
	if (0 != g_hKeyboardHook) 
		UnhookWindowsHookEx(g_hKeyboardHook); 

	g_dlgCheekguard = NULL;
}

void CDlgCheekGuard::Init(CIssCallManager* oCall)
{
	m_oCall = oCall;
}


BOOL CDlgCheekGuard::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;
	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR | SHIDIF_SIPDOWN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

	/*SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= hWnd;
	mbi.nToolBarId	= IDR_MENU_Unlock;
	mbi.hInstRes	= m_hInst;
	SHCreateMenuBar(&mbi);*/
#endif

	//HWND hwndIncoming = FindWindow(_T("Dialog"), _T("Phone - Incoming"));
	//DestroyWindow(hwndIncoming);
	
	//SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON);
	//RECT rc;
	//SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	//MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);

	LoadImages();
	//PostMessage(m_hWnd, WM_FullScreen, 0,0);


	//// try SetWindowPos
	//::SetWindowPos(m_hWnd ,       // handle to window
	//	HWND_TOPMOST,  // placement-order handle
	//	rc.left,     // horizontal position
	//	rc.top,      // vertical position
	//	WIDTH(rc),  // width
	//	HEIGHT(rc), // height
	//	SWP_SHOWWINDOW // window-positioning options
	//	);




	// trap all the keys here
	//ActivateKBHook(m_hInst, KBHook);
	g_hKeyboardHook = SetWindowsHookExW( WH_KEYBOARD_LL, 
		(HOOKPROC) KBHook,	// address of hook procedure 
		m_hInst,			// handle to application instance 
		0 );

	return TRUE;
}

BOOL CDlgCheekGuard::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, FALSE, FALSE);

	DrawBackground(m_gdiMem, rcClient);
	DrawButtons(m_gdiMem.GetDC(), rcClient);
	DrawText(m_gdiMem.GetDC(), rcClient);
	DrawImage(m_gdiMem.GetDC(), rcClient);
	BitBlt(hDC,
		   0,0,
		   WIDTH(rcClient), HEIGHT(rcClient),
		   m_gdiMem.GetDC(),
		   0,0,
		   SRCCOPY);

	return TRUE;
}

void CDlgCheekGuard::DrawImage(HDC hdc, RECT rcClient)
{
	CIssGDIEx* gdiPicture = m_oCall->GetCallerPicture();

	if(!gdiPicture)
		return;
	if(gdiPicture->GetDC() == NULL)
		return;

	if(GetCallState() != CSTATE_Ringing &&
		GetCallState() != CSTATE_Connected)
		return;

	CIssRect rcTemp(m_rcPicture);
	rcTemp.Stretch(1);

	FillRect(hdc, rcTemp.Get(), 0xFFFFFF);

	TransparentBlt(hdc, m_rcPicture.left, m_rcPicture.top, 
		m_rcPicture.right - m_rcPicture.left, m_rcPicture.bottom - m_rcPicture.top, 
		gdiPicture->GetDC(),
		0, 0, gdiPicture->GetWidth(), gdiPicture->GetHeight(), 0);
		
}

void CDlgCheekGuard::DrawText(HDC hdc, RECT rcClient)
{
	TCHAR* szTemp = NULL;

	EnumCallState eState = GetCallState();

	if(eState == CSTATE_Connected || eState == CSTATE_Ringing)
	{
        ::DrawText(hdc, m_oCall->GetCallerName(),	m_rcName, DT_LEFT, m_hFontText, TEXT_COLOR);
        ::DrawText(hdc, m_oCall->GetCallerNumber(),	m_rcLocation, DT_LEFT, m_hFontText, TEXT_COLOR);
        ::DrawText(hdc, m_oCall->GetCallerLocation(),m_rcPhoneNum, DT_LEFT, m_hFontText, TEXT_COLOR);
	}

	TCHAR szTime[STRING_NORMAL];
	int iMin = m_iCallDurration/60;
	int iSec = m_iCallDurration - iMin*60;
	m_oStr->Format(szTime, _T("%.2i:%.2i"), iMin, iSec);


	switch(eState)
	{
	case CSTATE_Connected:
		::DrawText(hdc, _T("Connected"), m_rcCallStatus, DT_LEFT, m_hFontText, TEXT_COLOR);
		::DrawText(hdc, szTime, m_rcDurration, DT_LEFT, m_hFontText, 0xFFFFFF);
		break;
	case CSTATE_Disconnected:
		::DrawText(hdc, _T("Disconnected"), m_rcCallStatus, DT_LEFT, m_hFontText, TEXT_COLOR);
		break;
	case CSTATE_Ringing:
		::DrawText(hdc, _T("Incoming Call"), m_rcCallStatus, DT_LEFT, m_hFontText, TEXT_COLOR);
	    break;
	default:
	    break;
	}

//	CIssGDIEx::DrawText(hdc, m_oCall->Get, m_rcTime, DT_LEFT, m_hFontText, 0xFFFFFF);
}

BOOL CDlgCheekGuard::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case _T('1'):
		FadeIn();
		break;
	case _T('2'):
		SweepRight();
		break;
	case _T('3'):
		SweepLeft();
		break;
	case _T('4'):
		SlideRight();
		break;
	case _T('5'):
		SlideLeft();
		break;
	case _T('6'):
		Trapazoid();
		break;
	case _T('7'):
		CubeHorzUp();
		break;
	case VK_RIGHT:
	case VK_LEFT:
	case VK_UP:
	case VK_DOWN:
	case VK_RETURN:
		PostQuitMessage(0);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgCheekGuard::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int	iHeight	= GetSystemMetrics(SM_CYICON);
	int iIndent = iHeight/3;

	m_gdiMem.Destroy();
	m_gdiArrowBackground.Destroy();
	m_gdiArrow.Destroy();
	m_gdiRed.Destroy();
	m_gdiGreen.Destroy();

	m_rcArrowBackground.left	= iIndent;
	m_rcArrowBackground.right	= rcClient.right - iIndent;
	m_rcArrowBackground.bottom	= rcClient.bottom - iIndent;
	m_rcArrowBackground.top		= rcClient.bottom - iIndent - iHeight;

	m_rcAnswer.left			= iIndent;
	m_rcAnswer.right		= rcClient.right/2 - iIndent;
	m_rcAnswer.bottom		= rcClient.bottom - iIndent;
	m_rcAnswer.top			= rcClient.bottom - iIndent - iHeight;

	m_rcEnd.left			= m_rcAnswer.right + 2*iIndent;
	m_rcEnd.right			= rcClient.right - iIndent;
	m_rcEnd.bottom			= rcClient.bottom  - iIndent;
	m_rcEnd.top				= rcClient.bottom - iIndent - iHeight;

	m_rcArrow					= m_rcArrowBackground;
	m_rcArrow.right				= m_rcArrow.left + iHeight*3/2;

	// picture
	m_rcPicture.left	= GetSystemMetrics(SM_CXSCREEN) - iIndent - 2*GetSystemMetrics(SM_CXICON);
	m_rcPicture.right   = GetSystemMetrics(SM_CXSCREEN) - iIndent;
	m_rcPicture.top		= iIndent + GetSystemMetrics(SM_CXICON);
	m_rcPicture.bottom  = m_rcPicture.top + 2*GetSystemMetrics(SM_CXICON);

	// name
	m_rcName.left		= iIndent;
	m_rcName.top		= iIndent + GetSystemMetrics(SM_CXICON);
	m_rcName.right		= GetSystemMetrics(SM_CYSCREEN) - 2*iIndent - 3*GetSystemMetrics(SM_CXICON);
	m_rcName.bottom		= m_rcName.top + GetSystemMetrics(SM_CXSMICON);

	m_rcLocation.left	= m_rcName.left;
	m_rcLocation.top	= m_rcName.bottom;
	m_rcLocation.right	= m_rcName.right;
	m_rcLocation.bottom	= m_rcLocation.top + GetSystemMetrics(SM_CXSMICON);

	m_rcPhoneNum.left	= m_rcName.left;
	m_rcPhoneNum.top	= m_rcLocation.bottom;
	m_rcPhoneNum.right	= m_rcName.right;
	m_rcPhoneNum.bottom	= m_rcPhoneNum.top + GetSystemMetrics(SM_CXSMICON);

	m_rcTime.left		= m_rcName.left;
	m_rcTime.top		= m_rcPhoneNum.bottom + GetSystemMetrics(SM_CXSMICON);
	m_rcTime.right		= GetSystemMetrics(SM_CXSCREEN) - iIndent;
	m_rcTime.bottom		= m_rcTime.top + GetSystemMetrics(SM_CXSMICON);

	m_rcCallStatus.left	= m_rcName.left;
	m_rcCallStatus.top	= m_rcTime.bottom;
	m_rcCallStatus.right= m_rcTime.right;
	m_rcCallStatus.bottom= m_rcCallStatus.top + GetSystemMetrics(SM_CXSMICON);

	m_rcDurration.left	= m_rcName.left;
	m_rcDurration.top	= m_rcCallStatus.bottom;
	m_rcDurration.right= m_rcTime.right;
	m_rcDurration.bottom= m_rcDurration.top + GetSystemMetrics(SM_CXSMICON);

	return TRUE;
}

BOOL CDlgCheekGuard::OnLButtonUp(HWND hWnd, POINT& pt)
{
	if(m_bMovingArrow)
	{
		m_bMovingArrow = FALSE;

		ReleaseCapture();

		// animate back to start
		// draw the button at the new location			
		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		HDC dc = GetDC(m_hWnd);

		POINT ptStart, ptEnd;
		ptStart.x	= m_rcArrow.left;
		ptEnd.x		= m_rcArrowBackground.left;
		ptStart.y	= ptEnd.y	= m_rcArrow.top;

		AnimateFromTo(dc, m_gdiTemp, m_gdiArrow, ptStart, ptEnd, 0,0,m_gdiArrow.GetWidth(), m_gdiArrow.GetHeight(), 50);

		m_rcArrow.left		= m_rcArrowBackground.left;
		m_rcArrow.right		= m_rcArrow.right + m_gdiArrow.GetWidth();

		m_gdiTemp.Destroy();

		ReleaseDC(m_hWnd, dc);
	}
	else if(m_bScreenLocked && (pt.x - m_ptOffset.x + m_gdiArrow.GetWidth()) > m_rcArrowBackground.right)
	{
		UnlockScreen();
	}
	else if(ShowSlider() == FALSE)
	{	//handle phone buttons
		if(PtInRect(&m_rcAnswer, pt))
		{
			OnTalkButton();	
		}
		if(PtInRect(&m_rcEnd, pt))
		{
			OnEndButton();
		}
	}
	return TRUE;
}

BOOL CDlgCheekGuard::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE) 
	{
		DebugOut(_T("CDlgCheekGuard::OnActivate msg = WA_ACTIVE"));

		BOOL bResult = SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON);

		RECT rc;
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		bResult = MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);

		SetTimer(m_hWnd, IDT_TIMER_INVAL, 500, NULL);

	}
	else if(LOWORD(wParam) == WA_INACTIVE)
	{
		DebugOut(_T("CDlgCheekGuard::OnActivate msg = WA_INACTIVE"));
		KillTimer(GetWnd(), IDT_TIMER_INVAL);
	}
	else if(LOWORD(wParam) == WA_CLICKACTIVE)
	{
		DebugOut(_T("CDlgCheekGuard::OnActivate msg = WA_CLICKACTIVE"));
	}

	return UNHANDLED;
}


BOOL CDlgCheekGuard::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(ShowSlider() == FALSE)
		return FALSE;
	

	if(PtInRect(&m_rcArrow, pt))
	{
		m_bMovingArrow = TRUE;

		m_ptOffset.x = pt.x - m_rcArrow.left;
		m_ptOffset.y = pt.y - m_rcArrow.top;

		SetCapture(m_hWnd);

		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		m_gdiTemp.Create(m_gdiMem.GetDC(), rcClient, FALSE, FALSE, FALSE);

		DrawBackground(m_gdiTemp, rcClient);
		DrawButtons(m_gdiTemp.GetDC(), rcClient, FALSE);
	}
	return TRUE;
}

BOOL CDlgCheekGuard::OnMouseMove(HWND hWnd, POINT& pt)
{
	if(m_bMovingArrow)
	{
		if((pt.x - m_ptOffset.x) < m_rcArrowBackground.left)
		{
			// draw the arrow at the left most position
			m_rcArrow.left		= m_rcArrowBackground.left;
			m_rcArrow.right		= m_rcArrow.left + m_gdiArrow.GetWidth();

			RECT rcClient;
			GetClientRect(m_hWnd, &rcClient);
			HDC dc = GetDC(m_hWnd);
			DrawButtons(m_gdiMem.GetDC(), rcClient);
			BitBlt(dc,
				m_rcArrowBackground.left,m_rcArrowBackground.top,
				WIDTH(m_rcArrowBackground), HEIGHT(m_rcArrowBackground),
				m_gdiMem.GetDC(),
				m_rcArrowBackground.left,m_rcArrowBackground.top,
				SRCCOPY);
			ReleaseDC(m_hWnd, dc);

			m_bMovingArrow = FALSE;
		}
		else if((pt.x - m_ptOffset.x + m_gdiArrow.GetWidth()) > m_rcArrowBackground.right)
		{
			m_bMovingArrow = FALSE;
			//UnlockScreen();
			//PostQuitMessage(0);	// we're outta here
		}
		else
		{
			// copy the old background over
			BitBlt(m_gdiMem.GetDC(),
				   m_rcArrow.left, m_rcArrow.top,
				   WIDTH(m_rcArrow), HEIGHT(m_rcArrow),
				   m_gdiTemp.GetDC(),
				   m_rcArrow.left, m_rcArrow.top,
				   SRCCOPY);

			CIssRect rcFull(m_rcArrow);

			// draw the button at the new location			
			m_rcArrow.left		= pt.x - m_ptOffset.x;
			m_rcArrow.right		= m_rcArrow.left + m_gdiArrow.GetWidth();

			rcFull.Concatenate(m_rcArrow);

			TransparentBlt(m_gdiMem.GetDC(),
							m_rcArrow.left, m_rcArrow.top,
							WIDTH(m_rcArrow), HEIGHT(m_rcArrow),
							m_gdiArrow.GetDC(),
							0,0,
							WIDTH(m_rcArrow), HEIGHT(m_rcArrow),
							TRANSPARENT_COLOR);


			HDC dc = GetDC(m_hWnd);

			// draw to the screen			
			BitBlt(dc,
					rcFull.left,rcFull.top,
					rcFull.GetWidth(), rcFull.GetHeight(),
					m_gdiMem.GetDC(),
					rcFull.left,rcFull.top,
					SRCCOPY);
			ReleaseDC(m_hWnd, dc);
		}
	}
	
	return TRUE;
}

BOOL CDlgCheekGuard::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgCheekGuard::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_FullScreen)
	{
		DebugOut(_T("CDlgCheekGuard setting fullscreen..."));
		BOOL bResult = FALSE;
		
/*		SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON);
		bResult = SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON);

		RECT rc;
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		bResult = MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);

		FadeIn();*/
		return OnIncomingCall();
	}
	return UNHANDLED;
}

BOOL CDlgCheekGuard::OnIncomingCall()
{
    SetTimer(m_hWnd, IDT_KILL_POPUP, 25, NULL);
      
    SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON);

    RECT rc;
    SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);

    FadeIn();

        return TRUE;
}

void CDlgCheekGuard::UnlockScreen()
{
	// hide this dlg
	m_bScreenLocked = FALSE;
	Hide();
}

BOOL CDlgCheekGuard::UseCheekGuard()
{
	UINT uFlags = 0;
	DWORD dwSize = sizeof(UINT);
	GetKey(/*REG_KEY*/REG_KEY_ISS_PATH, REG_GeneralOptions, (LPBYTE)&uFlags, dwSize);

	return uFlags & FLAG_USE_CHEEKGUARD;
}

void CDlgCheekGuard::SweepRight()
{
	//m_ddDraw.SweepRight(m_bFirstPic?m_dd2:m_dd1);
	HDC dc = GetDC(m_hWnd);
    ::SweepRight(dc, m_bFirstPic?m_gdi1:m_gdi2);
	ReleaseDC(m_hWnd,dc);
	m_bFirstPic = !m_bFirstPic;
}

void CDlgCheekGuard::SweepLeft()
{
	//m_ddDraw.SweepLeft(m_bFirstPic?m_dd2:m_dd1);
	HDC dc = GetDC(m_hWnd);
    ::SweepLeft(dc, m_bFirstPic?m_gdi1:m_gdi2);
	ReleaseDC(m_hWnd,dc);
	m_bFirstPic = !m_bFirstPic;
}

void CDlgCheekGuard::SlideRight()
{
	//m_ddDraw.SlideRight(m_bFirstPic?m_dd2:m_dd1);
	HDC dc = GetDC(m_hWnd);
    ::SlideRight(dc, m_bFirstPic?m_gdi1:m_gdi2);
	ReleaseDC(m_hWnd,dc);
	m_bFirstPic = !m_bFirstPic;
}

void CDlgCheekGuard::SlideLeft()
{
	//m_ddDraw.SlideLeft(m_bFirstPic?m_dd2:m_dd1);
	HDC dc = GetDC(m_hWnd);
    ::SlideLeft(dc, m_bFirstPic?m_gdi1:m_gdi2);
	ReleaseDC(m_hWnd,dc);
	m_bFirstPic = !m_bFirstPic;
}

void CDlgCheekGuard::FadeIn()
{
	//m_ddDraw.FadeIn(m_bFirstPic?m_dd2:m_dd1);
	HDC dc = GetDC(m_hWnd);
    ::FadeIn(dc, m_bFirstPic?m_gdi1:m_gdi2);
	ReleaseDC(m_hWnd,dc);
	m_bFirstPic = !m_bFirstPic;
}

void CDlgCheekGuard::Trapazoid()
{
	POINT ptTL = {40,10};
	POINT ptBL = {5,200};
	POINT ptTR = {200,10};
	POINT ptBR = {235,200};
	HDC dc = GetDC(m_hWnd);
    ::Trapazoid(dc, m_bFirstPic?m_gdi1:m_gdi2, ptTL, ptTR, ptBL, ptBR);
	ReleaseDC(m_hWnd,dc);
	m_bFirstPic = !m_bFirstPic;
}

void CDlgCheekGuard::CubeHorzUp()
{
	RECT rcDest, rcSrc;
	SetRect(&rcDest, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	SetRect(&rcSrc, 100, 200, 140, 260);
	HDC dc = GetDC(m_hWnd);
    ::CubeHorzUp(dc, m_bFirstPic?m_gdi1:m_gdi2, rcSrc, rcDest);
	ReleaseDC(m_hWnd,dc);
	m_bFirstPic = !m_bFirstPic;
}

void CDlgCheekGuard::LoadImages()
{
	//if(m_gdiPicture.GetDC() == NULL)
	//	LoadImage(m_gdiPicture, IDR_Sample, m_hWnd, m_hInst);

	if(m_gdiBackground.GetDC() == NULL)
        m_gdiBackground.LoadImage(IDR_Background, m_hWnd, m_hInst);
}

void CDlgCheekGuard::DrawBackground(CIssGDIEx& gdi, RECT rcClient)
{
	TileBackGround(gdi, rcClient, m_gdiBackground, FALSE);	

}

void CDlgCheekGuard::DrawButtons(HDC hdc, RECT rcClient, BOOL bDrawButton /*= TRUE*/)
{
	if(m_gdiArrowBackground.GetDC() == NULL)
	{
		CIssRect rc(m_rcArrowBackground);
		rc.ZeroBase();
		int iCurve		= GetSystemMetrics(SM_CXICON)/2;
		m_gdiArrowBackground.Create(hdc, rc.Get(), FALSE, TRUE, TRUE);
		GradientRoundRectangle(m_gdiArrowBackground, rc.Get(), RGB(100,100,100), iCurve, iCurve, RGB(0,0,0), RGB(50,50,50));
	}

	if(m_gdiArrow.GetDC() == NULL)
	{
		CIssRect rc(m_rcArrow);
		rc.ZeroBase();
		int iCurve		= GetSystemMetrics(SM_CXICON)/2;
		m_gdiArrow.Create(hdc, rc.Get(), FALSE, TRUE, TRUE);
		rc.Stretch(-1);
		ShinyRoundRectangle(m_gdiArrow, rc.Get(), RGB(120,120,120), iCurve, iCurve, RGB(255,255,255), RGB(180,180,180), RGB(120, 120, 120), RGB(180,180,180));
	}

	if(m_gdiGreen.GetDC() == NULL)
	{
		CIssRect rc(m_rcAnswer);
		rc.ZeroBase();
		int iCurve		= GetSystemMetrics(SM_CXICON)/3;
		m_gdiGreen.Create(hdc, rc.Get(), FALSE, TRUE, TRUE);
		rc.Stretch(-1);
		ShinyRoundRectangle(m_gdiGreen, rc.Get(), 0xBDD384, iCurve, iCurve, 0xBDD384, 0x639E31, 0x317D08, 0x42DF5A);
	}

	if(m_gdiRed.GetDC() == NULL)
	{
		CIssRect rc(m_rcEnd);
		rc.ZeroBase();
		int iCurve		= GetSystemMetrics(SM_CXICON)/3;
		m_gdiRed.Create(hdc, rc.Get(), FALSE, TRUE, TRUE);
		rc.Stretch(-1);
		//CIssGDIEx::DrawShinyRoundRectangle(m_gdiRed, rc.Get(), 0x9C96D6, iCurve, iCurve, 0x9C96D6, 0x5251A5, 0x313C8C, 0x4A5DD6);
		ShinyRoundRectangle(m_gdiRed, rc.Get(), 0x9C96D6, iCurve, iCurve, 0x9C96D6, 0x5251A5, 0x082C7D, 0x3729D3);
	}

	if(ShowSlider())
	{
		TransparentBlt(hdc,
			m_rcArrowBackground.left, m_rcArrowBackground.top,
			WIDTH(m_rcArrowBackground), HEIGHT(m_rcArrowBackground),
			m_gdiArrowBackground.GetDC(),
			0,0,
			WIDTH(m_rcArrowBackground), HEIGHT(m_rcArrowBackground),
			TRANSPARENT_COLOR);

		if(bDrawButton)
		{
			TransparentBlt(hdc,
				m_rcArrow.left, m_rcArrow.top,
				WIDTH(m_rcArrow), HEIGHT(m_rcArrow),
				m_gdiArrow.GetDC(),
				0,0,
				WIDTH(m_rcArrow), HEIGHT(m_rcArrow),
				TRANSPARENT_COLOR);
		}
	}
	else
	{
		TransparentBlt(hdc,
			m_rcAnswer.left, m_rcAnswer.top,
			WIDTH(m_rcAnswer), HEIGHT(m_rcAnswer),
			m_gdiGreen.GetDC(),
			0,0,
			WIDTH(m_rcAnswer), HEIGHT(m_rcAnswer),
			TRANSPARENT_COLOR);

		::DrawText(hdc, _T("Talk"), m_rcAnswer, DT_CENTER | DT_VCENTER, m_hFontText, 0xFFFFFF);

		TransparentBlt(hdc,
			m_rcEnd.left, m_rcEnd.top,
			WIDTH(m_rcEnd), HEIGHT(m_rcEnd),
			m_gdiRed.GetDC(),
			0,0,
			WIDTH(m_rcEnd), HEIGHT(m_rcEnd),
			TRANSPARENT_COLOR);

		::DrawText(hdc, _T("End"), m_rcEnd, DT_CENTER | DT_VCENTER, m_hFontText, 0xFFFFFF);
	}
}

BOOL CDlgCheekGuard::Connected()
{
	StartCallTimer();	

	// lock screen if needed
	if(UseCheekGuard())
		m_bScreenLocked = TRUE;

	return Show();
}

BOOL CDlgCheekGuard::Disconnected()
{
	m_bScreenLocked = FALSE;
	return Hide();

}

BOOL CDlgCheekGuard::Show()
{
	// foreground
	BOOL bResult = FALSE;
	ShowWindow(GetWnd(), SW_SHOW);

	bResult = SetForegroundWindow(GetWnd());
	if(!bResult)
	{
		DebugOut(_T("SetForegroundWindow() failed"));
	}

	return bResult;
}
BOOL CDlgCheekGuard::Hide()
{
	// hide this
	return ShowWindow(m_hWnd, SW_HIDE);
}

BOOL CDlgCheekGuard::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(IDT_TIMER == IDT_KILL_POPUP)
    {
        HWND hwndIncoming = NULL;

        FindWindow(_T("Dialog"), _T("Phone - Incoming"));
        if(hwndIncoming == NULL)
        {
            ShowWindow(hwndIncoming, SW_HIDE);   
            KillTimer(hwndIncoming, IDT_KILL_POPUP);//BUGBUG ... should default kill this somewhere
        }
        

    }

	//we'll need this to update the connected time
	if(wParam == IDT_TIMER)
	{
		m_iCallDurration++;
		InvalidateRect(m_hWnd, &m_rcDurration, FALSE);
	}
	else if(wParam == IDT_TIMER_INVAL)
	{
		EnumCallState eState = GetCallState();
		if(eState != m_eLastState)
		{
			DebugOut(_T("call state changed..."));
			InvalidateRect(m_hWnd, NULL, FALSE);
			m_eLastState = eState;
		}
	}

	return FALSE;
}

BOOL CDlgCheekGuard::ShowSlider()
{
	if(GetCallState() == CSTATE_Connected && m_bScreenLocked)
		return TRUE; 
	
	return FALSE;
}

EnumCallState CDlgCheekGuard::GetCallState()
{
	return m_oCall->GetCallState();
	//return m_eState;
}

void CDlgCheekGuard::OnTalkButton()
{
	//m_oCall->AnswerPhone();
	//http://msdn2.microsoft.com/en-us/library/ms927178.aspx
	//http://msdn2.microsoft.com/en-us/library/ms646304(VS.85).aspx

	// Simulate a key press
	keybd_event( VK_TTALK,
		0x45,
		KEYEVENTF_EXTENDEDKEY | 0,
		0 );

	// Simulate a key release
	keybd_event( VK_TTALK,
		0x45,
		KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
		0);

	// first keypress activates the windows phone screen
	// try a second keypress to answer call
	HWND hWndForground = NULL;
	TCHAR szWindowText[STRING_LARGE];
	for(int i = 0; i < 10; i++)
	{
		Sleep(50);
		//DebugOut(_T("%i"), i);
		hWndForground = ::GetForegroundWindow();
		::GetWindowText(hWndForground, szWindowText, STRING_LARGE);
		m_oStr->Trim(szWindowText);
		if(m_oStr->Compare(szWindowText, _T("Phone")) == 0)
			break;
	}
	// Simulate a key press
	keybd_event( VK_TTALK,
		0x45,
		KEYEVENTF_EXTENDEDKEY | 0,
		0 );

	// Simulate a key release
	keybd_event( VK_TTALK,
		0x45,
		KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
		0);

}

void CDlgCheekGuard::OnEndButton()
{
	// Simulate a key press
	keybd_event( VK_TEND,
		0x45,
		KEYEVENTF_EXTENDEDKEY | 0,
		0 );

	// Simulate a key release
	keybd_event( VK_TEND,
		0x45,
		KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
		0);
}

void CDlgCheekGuard::EndCall()
{
	m_oCall->EndCall();
}

void CDlgCheekGuard::StartCallTimer()
{
	m_iCallDurration = 0;
	SetTimer(m_hWnd, IDT_TIMER, 1000, NULL);
}

void CDlgCheekGuard::EndCallTimer()
{
	KillTimer(m_hWnd, IDT_TIMER);
	m_iCallDurration = 0;
}

