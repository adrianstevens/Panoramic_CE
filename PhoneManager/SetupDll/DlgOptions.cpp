#include "DlgOptions.h"
#include "resource.h"
#include "stdafx.h"
#include "ObjAssociations.h"
#include "GeneralOptions.h"
#include "CommonDefines.h"
#include "IssRegistry.h"

CDlgOptions::CDlgOptions(CObjAssociations* objAssociations)
//:m_hChkTodaySoftkey(NULL)
:m_hChkReplaceDefault(NULL)
,m_hStaticMsg(NULL)
,m_objAssociations(objAssociations)
{
}

CDlgOptions::~CDlgOptions(void)
{
}

BOOL CDlgOptions::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// create the controls
	m_hStaticMsg= CreateWindowEx(0, _T("Static"), NULL, WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
	//m_hChkTodaySoftkey	= CreateWindowEx(0, _T("Button"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
	m_hChkReplaceDefault	= CreateWindowEx(0, _T("Button"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_hChkShowIncoming	= CreateWindowEx(0, _T("Button"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_hChkShowCheekGuard	= CreateWindowEx(0, _T("Button"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
    m_hChkShowPost	= CreateWindowEx(0, _T("Button"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
	
	TCHAR szText[STRING_MAX];
	m_oStr->StringCopy(szText, IDS_MSG_Contacts, STRING_MAX, m_hInst);
	SetWindowText(m_hStaticMsg, szText);

	// set the text of the controls
	//SetWindowText(m_hChkTodaySoftkey, _T("Set today screen menu"));
	SetWindowText(m_hChkReplaceDefault, _T("Override default contacts app"));
    SetWindowText(m_hChkShowIncoming, _T("Show incoming call screen"));
    SetWindowText(m_hChkShowCheekGuard, _T("Show cheek guard during call"));
    SetWindowText(m_hChkShowPost, _T("Show post call screen"));

	// set all the check boxes to true
	//SendMessage(m_hChkTodaySoftkey, BM_SETCHECK, (WPARAM)FALSE, 0);
	SendMessage(m_hChkReplaceDefault, BM_SETCHECK, (WPARAM)FALSE, 0);
    SendMessage(m_hChkShowIncoming, BM_SETCHECK, (WPARAM)TRUE, 0);
    SendMessage(m_hChkShowCheekGuard, BM_SETCHECK, (WPARAM)TRUE, 0);
    SendMessage(m_hChkShowPost, BM_SETCHECK, (WPARAM)TRUE, 0);

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
	mbi.nToolBarId	= IDR_MENU_OkCancel; 
	mbi.hInstRes	= m_hInst;
	SHCreateMenuBar(&mbi);



	return TRUE;
}

BOOL CDlgOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
#ifndef WIN32_PLATFORM_WFSP
	case IDOK:
#endif
	case IDMENU_Ok:
		OnMenuOk();
		break;
	case IDMENU_Cancel:
		EndDialog(hWnd, IDCANCEL);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

void CDlgOptions::OnMenuOk()
{
    CIssString* oStr = CIssString::Instance();

    /*TCHAR szPath[MAX_PATH];
    oStr->StringCopy(szPath, (TCHAR*)m_szDirectory);
    oStr->Concatenate(szPath, _T("\\"));

    CObjAssociations obj(szPath);*/

    DWORD dwValue = 0;

    // read in old values if they were there
    if(S_OK == GetKey(REG_KEY_ISS_PATH, REG_GeneralOptions, dwValue))
    {
        // turn off IncomingCall, Cheekguard, and post call
        if(dwValue & FLAG_SHOW_HandleRingtone)
        {
            dwValue = FLAG_SHOW_HandleRingtone;
        }
        else
            dwValue = 0;
    }
    else
        dwValue = FLAG_SHOW_HandleRingtone; // on by default

    /*if(BST_CHECKED==SendMessage(m_hChkTodaySoftkey, BM_GETCHECK,0,0))
        obj.SetTodaySoftkey();*/
    if(BST_CHECKED==SendMessage(m_hChkReplaceDefault, BM_GETCHECK,0,0))
        m_objAssociations->SetContactOverride();
    if(BST_CHECKED==SendMessage(m_hChkShowIncoming, BM_GETCHECK,0,0))
        dwValue |= FLAG_SHOW_IncomingCall;
    if(BST_CHECKED==SendMessage(m_hChkShowCheekGuard, BM_GETCHECK,0,0))
        dwValue |= FLAG_SHOW_Cheekguard;
    if(BST_CHECKED==SendMessage(m_hChkShowPost, BM_GETCHECK,0,0))
        dwValue |= FLAG_SHOW_PostCall;


    SetKey(REG_KEY_ISS_PATH, REG_GeneralOptions, dwValue);

	EndDialog(m_hWnd, IDOK);
}

BOOL CDlgOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int	iIndent		= GetSystemMetrics(SM_CXICON)/8;
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

#ifdef WIN32_PLATFORM_WFSP
	int iIconSize	= GetSystemMetrics(SM_CXICON)*2/3; // GetSystemMetrics(SM_CXICON)*2/3;//good height for smartphone
#else
	int iIconSize	= GetSystemMetrics(SM_CYVSCROLL);
#endif

	int iY = iIndent;

	MoveWindow(m_hStaticMsg, iIndent, iY, rcClient.right - 2*iIndent, iIconSize, FALSE);
	iY += iIconSize;
	/*MoveWindow(m_hChkTodaySoftkey, iIndent, iY, rcClient.right - 2*iIndent, iIconSize, FALSE);
	iY += iIconSize;*/
	MoveWindow(m_hChkReplaceDefault, iIndent, iY, rcClient.right - 2*iIndent, iIconSize, FALSE);
    iY += iIconSize;
    MoveWindow(m_hChkShowIncoming, iIndent, iY, rcClient.right - 2*iIndent, iIconSize, FALSE);
    iY += iIconSize;
    MoveWindow(m_hChkShowCheekGuard, iIndent, iY, rcClient.right - 2*iIndent, iIconSize, FALSE);
    iY += iIconSize;
    MoveWindow(m_hChkShowPost, iIndent, iY, rcClient.right - 2*iIndent, iIconSize, FALSE);
	
	return TRUE;
}

