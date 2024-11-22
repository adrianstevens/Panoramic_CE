#include "StdAfx.h"
#include "Resource.h"
#include "DlgAddContact.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"


CDlgAddContact::CDlgAddContact(void)
:m_iSelectedItem(-1)
,m_bMoved(FALSE)
,m_hWndCmb(0)
{
	m_hFontTitle = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON)*5/6, FW_BOLD, TRUE);
}

CDlgAddContact::~CDlgAddContact(void)
{
	CIssGDIEx::DeleteFont(m_hFontTitle);
}

BOOL CDlgAddContact::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// DH: try this here because OnSize() wil be called after SHInitDialog()
	m_hWndCmb = CreateWindowEx(0, _T("ComboBox"), NULL,	WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	//DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN|SHIDIF_DONEBUTTON|SHIDIF_SIZEDLG;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN|SHIDIF_DONEBUTTON;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= m_hWnd;
	mbi.nToolBarId	= IDR_MENU_OkCancel; 
	mbi.hInstRes	= m_hInst;
	SHCreateMenuBar(&mbi);

#endif

	//m_hWndCmb = CreateWindowEx(0, _T("ComboBox"), NULL,	WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);

	TCHAR szName[STRING_MAX] = _T("");
	for(int i=0; i<m_poomContacts->GetNumContacts(); i++)
	{
		// is this contact already on our list??
		BOOL bAddItem = TRUE;
		for(int j = 0; j < m_arrItems->GetSize(); j++)
		{

			CObjItemContact* objItem = (CObjItemContact*)m_arrItems->GetElement(j);
			if(m_poomContacts->GetUserOID(i) == objItem->GetOid())
			{
				bAddItem = FALSE;
				break;
			}
			
		}
		if(!bAddItem)
			continue;

		m_oStr->Empty(szName);
		TCHAR* szFirstName = m_poomContacts->GetUserFirstName(i);
		TCHAR* szLastName = m_poomContacts->GetUserLastName(i);
		if(!m_oStr->IsEmpty(szLastName))
		{
			m_oStr->Concatenate(szName, szLastName);
			if(!m_oStr->IsEmpty(szFirstName))
				m_oStr->Concatenate(szName, _T(", "));
		}
		if(!m_oStr->IsEmpty(szFirstName))
			m_oStr->Concatenate(szName, szFirstName);

		m_oStr->Delete(&szFirstName);
		m_oStr->Delete(&szLastName);

		int iIndex = SendMessage(m_hWndCmb, CB_ADDSTRING, 0, (LPARAM)szName);
		SendMessage(m_hWndCmb, CB_SETITEMDATA, iIndex, i);

	}

	SendMessage(m_hWndCmb, CB_SETCURSEL, 0, 0);

	RECT rc;
	RECT rcWindow;
	GetClientRect(mbi.hwndMB, &rc);
	GetClientRect(hWnd, &rcWindow);
	rcWindow.bottom += HEIGHT(rc);
	MoveWindow(hWnd, 0, 0, WIDTH(rcWindow), HEIGHT(rcWindow), FALSE);

	return TRUE;
}

BOOL CDlgAddContact::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//DebugOut(_T("CDlgAddContact::OnSize()"));

	int iIconSize	= GetSystemMetrics(SM_CXICON)*2/3;//good height for smartphone
	int	iIndent		= GetSystemMetrics(SM_CXICON)/8;

#ifdef WIN32_PLATFORM_WFSP
	int iDropWindowSize = iIconSize;
#else
	int iDropWindowSize = 180;
	if (GetSystemMetrics(SM_CYSCREEN) > 320)
		iDropWindowSize = 450;
#endif

	if(m_hWndCmb && !m_bMoved)
	{
		m_bMoved = TRUE;
		MoveWindow(	m_hWndCmb,
					iIndent, iIndent,
					GetSystemMetrics(SM_CXSCREEN) - iIndent, iDropWindowSize, //iIconSize,
					FALSE);
	}

	m_rcTitle.left = iIndent;
	m_rcTitle.top  = iIndent;
	m_rcTitle.right= GetSystemMetrics(SM_CXSCREEN) - iIndent;
	m_rcTitle.bottom = m_rcTitle.top + GetSystemMetrics(SM_CXSMICON);

	m_rcCombo		= m_rcTitle;
	m_rcCombo.top	= m_rcTitle.bottom;
	m_rcCombo.bottom= m_rcCombo.top + GetSystemMetrics(SM_CXSMICON);

	if(m_hWndCmb)
		MoveWindow(m_hWndCmb, m_rcCombo.left, m_rcCombo.top, WIDTH(m_rcCombo), HEIGHT(m_rcCombo), FALSE);

	return TRUE;
}

BOOL CDlgAddContact::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case IDOK:
	case IDMENU_Ok:
	{
		int iIndex = SendMessage(m_hWndCmb, CB_GETCURSEL, 0,0);
		if(iIndex != -1)
		{
			m_iSelectedItem = SendMessage(m_hWndCmb, CB_GETITEMDATA, iIndex, 0);
			EndDialog(m_hWnd, IDOK);
		}
		break;
	}
	case IDMENU_Cancel:
		EndDialog(m_hWnd, IDCANCEL);
		break;
	}
	return TRUE;
}

BOOL CDlgAddContact::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	DrawText(hDC, _T("Select Contact to Add:"), m_rcTitle, DT_CENTER, m_hFontTitle);
	return TRUE;


}