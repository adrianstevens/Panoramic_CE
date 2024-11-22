#include "StdAfx.h"
#include "DlgConnect.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "resourceppc.h"

CDlgConnect::CDlgConnect(void)
:m_oCallMgr(NULL)
{
}

CDlgConnect::~CDlgConnect(void)
{
}

BOOL CDlgConnect::Init(CIssCallManager* CallManger)
{
	m_oCallMgr = CallManger;
	return TRUE;
}

BOOL CDlgConnect::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
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


	return TRUE;
}

BOOL CDlgConnect::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	::FillRect(hDC, rcClient, 0xFFDDDD);

	//lets display some info 
	RECT rcTemp;
	rcTemp.left		= GetSystemMetrics(SM_CXSMICON);
	rcTemp.top		= GetSystemMetrics(SM_CXSMICON);
	rcTemp.right	= rcClient.right;
	rcTemp.bottom	= rcClient.bottom;

	rcTemp.top		+= GetSystemMetrics(SM_CXICON);

	::DrawText(hDC, m_oCallMgr->GetCallerName(), rcTemp, DT_LEFT | DT_TOP);

	rcTemp.top		+= GetSystemMetrics(SM_CXICON);

	::DrawText(hDC, m_oCallMgr->GetCallerNumber(), rcTemp, DT_LEFT | DT_TOP);

	rcTemp.top		+= GetSystemMetrics(SM_CXICON);

	::DrawText(hDC, m_oCallMgr->GetCallerLocation(), rcTemp, DT_LEFT | DT_TOP);

	return TRUE;
}


BOOL CDlgConnect::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
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