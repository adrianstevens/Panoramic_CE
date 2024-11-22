#include "StdAfx.h"
#include "resourceppc.h"
#include "DlgPhoneServer.h"
#include "IssRegistry.h"
#include "PoomContacts.h"
#include "CommonDefines.h"
#include <regext.h>
#include <snapi.h>

//#define NOTIFY_CNT 1				// start with 1
//HREGNOTIFY  g_hNotify[NOTIFY_CNT];  // Handles to notifications 


//void IncomingStateCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData)
//{
//	int a = 0;
//}

CDlgPhoneServer::CDlgPhoneServer(void)
:m_oStr(CIssString::Instance())
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


	/*TCHAR szText[STRING_MAX];

	m_oStr->Format(szText, _T("Wave In Devices: %d"), m_oSound.GetNumWaveInDevices());
	AddOutput(szText);
	for(int i=0; i<m_oSound.GetNumWaveInDevices(); i++)
	{
		m_oSound.GetWaveInDeviceName(i, szText);
		AddOutput(szText);
	}

	m_oStr->Format(szText, _T("Wave Out Devices: %d"), m_oSound.GetNumWaveOutDevices());
	AddOutput(szText);
	for(int i=0; i<m_oSound.GetNumWaveOutDevices(); i++)
	{
		m_oSound.GetWaveOutDeviceName(i, szText);
		AddOutput(szText);
	}*/

	// load our sound file
	//m_oSound.StartRecording(0,hWnd, "\\My Documents\\Patrick.wav");
	

	// initialize tapi
	m_oCallManager.Initialize(m_hInst, m_hWnd, m_hWnd);

	// register notifications
	CObjStateAndNotifyMgr::RegisterWindowNotifications(GetWnd());
	
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

BOOL CDlgPhoneServer::OnLButtonDown(HWND hWnd, POINT& pt)
{
	return TRUE;
}

BOOL CDlgPhoneServer::OnLButtonUp(HWND hWnd, POINT& pt)
{
	return TRUE;
}

BOOL CDlgPhoneServer::OnMouseMove(HWND hWnd, POINT& pt)
{
	return TRUE;
}

BOOL CDlgPhoneServer::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	return TRUE;
}

BOOL CDlgPhoneServer::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgPhoneServer::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
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
		m_oCallManager.ReceiveNotification(uiMessage, wParam, lParam);
	}
	switch(uiMessage)
	{
	case WM_TAPI_Output:
		AddOutput((TCHAR*)lParam);
		break;
	//case WM_NOTIFY_INCOMING:
	//	m_oCallManager.ReceiveNotification_Incoming();
	//	break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgPhoneServer::ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == MM_WIM_DATA || message == MM_WOM_DONE)
	{
		//AddOutput(_T("Audio Callback"));
		m_oSound.SoundCallback(message, wParam, lParam);
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

