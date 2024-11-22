#include "StdAfx.h"
#include "resourceppc.h"
#include "DlgPhoneServer.h"
#include "IssRegistry.h"
#include "PoomContacts.h"
#include "CommonDefines.h"
#include <regext.h>
#include <snapi.h>

#include "DlgCallScreen.h"

CDlgPhoneServer::CDlgPhoneServer(void)
:m_bPreventUsage(FALSE)
{
}

CDlgPhoneServer::~CDlgPhoneServer(void)
{
}

BOOL CDlgPhoneServer::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
//#define SHOW_OUTPUT
#ifdef SHOW_OUTPUT
#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= hWnd;
	mbi.nToolBarId	= IDR_MENU;
	mbi.hInstRes	= m_hInst;
	SHCreateMenuBar(&mbi);

#endif

	// create the edit window
	m_hWndOutput = CreateWindowEx(0, TEXT("edit"), NULL, 
								  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY | ES_MULTILINE,
								  5, 5, 5, 5, 
								  hWnd, NULL, 
								  m_hInst, NULL);


#endif	// SHOW_OUTPUT

	// register notifications
	CObjStateAndNotifyMgr::RegisterWindowNotifications(hWnd);

    m_dlgCall.Init(&m_gdiMem, &m_guiBack, FALSE, FALSE);
    m_dlgCall.LoadImages(hWnd, m_hInst);
    m_dlgCall.CreateWin(NULL);

    CheckDemo();

	return TRUE;
}

BOOL CDlgPhoneServer::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	FillRect(hDC, &rcClient, (HBRUSH)GetStockObject(WHITE_BRUSH));
	return TRUE;
}

BOOL CDlgPhoneServer::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case IDMENU_Quit:
		//m_oSound.StopRecording();
		PostQuitMessage(0);
		break;

	default:
		return FALSE;
	}
	return TRUE;
}


BOOL CDlgPhoneServer::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//redraw the screen
	InvalidateRect(hWnd, NULL, FALSE);
	return TRUE;
}

BOOL CDlgPhoneServer::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	// only change the window size if it has been updated properly
	if(rcClient.right != GetSystemMetrics(SM_CXSCREEN))
		return TRUE;

	int iOutputWidth	= 9*GetSystemMetrics(SM_CXSCREEN)/10;
	int iOutputHeight	= 9*rcClient.bottom/10;

	MoveWindow(m_hWndOutput, 
			   (GetSystemMetrics(SM_CXSCREEN)-iOutputWidth)/2,
			   (rcClient.bottom-iOutputHeight)/2,
			   iOutputWidth, iOutputHeight,
			   FALSE);

	return TRUE;
}

BOOL CDlgPhoneServer::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage >= WM_NOTIFY_BASE && uiMessage < WM_NOTIFY_END)
    {
#ifndef DEBUG
        if(uiMessage == WM_NOTIFY_DATETIME)
        {
            DBG_OUT((_T("WM_NOTIFY_DATETIME")));
            SYSTEMTIME sysTime;
            GetLocalTime(&sysTime);
            if(sysTime.wMinute % 10 == 0)
                CheckDemo();
        }

        if(m_bPreventUsage)
            return TRUE;
#endif
        SendMessage(m_dlgCall.GetWnd(), uiMessage, wParam, lParam);
    }

	return TRUE;
}

void CDlgPhoneServer::AddOutput(TCHAR* szText)
{
	if(NULL == szText || NULL == m_hWndOutput)
		return;

	int iLen1		= GetWindowTextLength(m_hWndOutput);
	int iLen2		= m_oStr->GetLength(szText);
	TCHAR* szTotal	= new TCHAR[iLen1+iLen2+5]; 

	m_oStr->Empty(szTotal);
	GetWindowText(m_hWndOutput, szTotal, iLen1+1);
	if(!m_oStr->IsEmpty(szTotal))
		m_oStr->Insert(szTotal, _T("\r\n"));
	m_oStr->Insert(szTotal, szText);
	SetWindowText(m_hWndOutput, szTotal);
	delete [] szTotal;
}

void CDlgPhoneServer::CheckDemo()
{
    CIssKey oKey;
    oKey.Init(_T("SOFTWARE\\Panoramic\\CallManager"), _T("SOFTWARE\\Pano\\PS2"), 0x0, 0x1234CDEF, 0x67ABCEF1, 5, TRUE);
    oKey.CheckSetKey();
    oKey.CheckDemo();

    m_bPreventUsage = !oKey.m_bGood;

    if(!oKey.m_bGood)
    {
        CObjRingtoneEngine objRing;
        objRing.SetRingTone0ToDefault();
    }
}

