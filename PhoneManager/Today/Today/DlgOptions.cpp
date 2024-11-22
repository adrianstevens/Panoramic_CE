// DlgOptions.cpp: implementation of the CDlgOptions class.
//
//////////////////////////////////////////////////////////////////////

#include <aygshell.h>
#include "DlgOptions.h"
#include "todaycmn.h"
#include ".\resource.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "CommonDefines.h"
#include "CommonGuiDefines.h"
#include "ObjTodayItems.h"
#include "DlgAddTodayItem.h"
#include "IssRegistry.h"
#include "IssCommon.h"
#include "DlgChooseContact.h"
#include "DlgChoosePrograms.h"
#include "DlgChooseSettings.h"
#include "DlgSetVoicemail.h"
#include "ObjVoicemail.h"
 

#define WM_Add		WM_USER + 200
#define WM_Remove	WM_USER + 201

CDlgOptions::CDlgOptions()
: m_bHasInit(FALSE)
, m_hWndList(NULL)
, m_bIsLocked(FALSE)
{	
	m_hCaptionFont	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*13/32, FW_BOLD, TRUE);
	m_hBtnFont		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);

}

CDlgOptions::~CDlgOptions()
{
	if(m_hWndList)
	{
		ListView_DeleteAllItems(m_hWndList);
		m_hWndList = NULL;
	}
	CIssGDIEx::DeleteFont(m_hCaptionFont);
	CIssGDIEx::DeleteFont(m_hBtnFont);
    CDlgChooseContact::ResetListContent();
    CDlgChoosePrograms::ResetListContent();
    CDlgChooseSettings::DestroyAllContent();

	Destroy();
}


BOOL CDlgOptions::DoModal(HWND hWndParent, HINSTANCE hInst)
{
#ifdef DEBUG
	return CIssWnd::DoModal(hWndParent, hInst, IDD_DLG_Basic);
#else
	return CIssWnd::DoModal(hWndParent, hInst, IDD_TODAY_CUSTOM);
#endif
}

BOOL CDlgOptions::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//MessageBox(m_hWnd, _T("CDlgOptions::OnInitDialog()"), _T(""), MB_OK);

    m_guiBackground.Init(hWnd, m_hInst);

	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLGFULLSCREEN;
	DlgInfo.hDlg	= hWnd;
	
	SHInitDialog(&DlgInfo);

    SHMENUBARINFO mbi;
    memset(&mbi, 0, sizeof(mbi)); 
    mbi.cbSize		= sizeof(mbi);
    mbi.hwndParent	= hWnd;
#ifdef DEBUG
    mbi.nToolBarId	= IDR_MENU_Done;
#else
    mbi.nToolBarId	= IDR_MENU_NewDoneMenu;
#endif
    mbi.hInstRes	= m_hInst;
    SHCreateMenuBar(&mbi);

    DWORD dwValue;
    if(S_OK == GetKey(REG_KEY_ISS_PATH, REG_LockItems, dwValue))
        m_bIsLocked = (BOOL)dwValue;

	InitControls();
	CreateButtons();
	return TRUE;
}

BOOL CDlgOptions::InitControls()
{
	m_bHasInit = FALSE;
	// create window
	m_hWndList = CreateWindowEx(0,
		WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN |
		WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL /*| LVS_OWNERDRAWFIXED | LVS_OWNERDATA*/ | LVS_NOCOLUMNHEADER, 
		0, 0, 200, 200, m_hWnd,
		(HMENU) NULL, m_hInst, NULL);

    ListView_SetExtendedListViewStyleEx(m_hWndList, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	// add one column
	LV_COLUMN LVColumn = {0};
	LVColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;
	LVColumn.fmt		= LVCFMT_LEFT;
	LVColumn.cx			= 200;		
	LVColumn.iSubItem	= 0;
	ListView_InsertColumn(m_hWndList, 0, &LVColumn);

	RefreshList();

	//m_bHasInit = TRUE;
	return TRUE;
}
void CDlgOptions::CreateButtons()
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	m_btnAdd.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnAdd.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnAdd.Init(m_rcAddBtn, _T("Add Item"), m_hWnd, WM_Add, 0, RGB(0,0,0), BTN_Shine3, NULL);

	m_btnRemove.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnRemove.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnRemove.Init(m_rcRemoveBtn, _T("Remove"), m_hWnd, WM_Remove, 0, RGB(0,0,0), BTN_Shine3, NULL);
}



int CDlgOptions::CompareItems(void* pt1, void* pt2)
{
	DWORD* dw1 = (DWORD*)pt1;
	DWORD* dw2 = (DWORD*)pt2;

	if(dw1 && dw2)
		return dw1 - dw2;
	return -1;
}

BOOL CDlgOptions::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL ||
		m_gdiMem.GetWidth() != (rcClient.right - rcClient.left) || 
		m_gdiMem.GetHeight() != (rcClient.bottom - rcClient.top))
	{
		m_gdiMem.Destroy();
		m_gdiMem.Create(hDC, rcClient, FALSE, FALSE, FALSE);
	}

	FillRect(m_gdiMem.GetDC(), rcClient, 0x00ffffff);

	// draw the caption
	DrawText(m_gdiMem.GetDC(), _T("Today: Options"), 
		m_rcCaption, DT_SINGLELINE|DT_VCENTER|DT_LEFT, m_hCaptionFont, ::GetSysColor(COLOR_HIGHLIGHT));

	// Paint lines
	RECT rcLine = rcClient;
	rcLine.top = m_rcCaption.bottom;
	rcLine.bottom = rcLine.top + 1;
	FillRect(m_gdiMem.GetDC(), rcLine, 0);

	// draw buttons
	m_btnAdd.Draw(m_hBtnFont, m_gdiMem.GetDC());
	m_btnRemove.Draw(m_hBtnFont, m_gdiMem.GetDC());

	BitBlt(hDC,
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiMem.GetDC(),
		0,0,
		SRCCOPY);

	return TRUE;
}

BOOL CDlgOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	BOOL bChanged = FALSE;
	switch(LOWORD(wParam))
    {
        case IDOK:
	    case IDCANCEL:
        case IDMENU_Done:
            m_gdiMem.Destroy();
            m_guiBackground.Destroy();
            m_btnAdd.Destroy();
            m_btnRemove.Destroy();
    		EndDialog(hWnd, bChanged);
    		break;
        case IDMENU_Menu:
            OnMenu();
            break;
        case IDMENU_Help:
            CreateProcess(_T("peghelp"), _T("incominghelp.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
            break;
        case IDMENU_LockItems:
            m_bIsLocked = !m_bIsLocked;
            SetKey(REG_KEY_ISS_PATH, REG_LockItems, (DWORD)m_bIsLocked);
            break;
        case IDMENU_SetVoicemail:
            SetVoicemailNumber();
            break;
        case IDMENU_RestoreDefaults:
            CObjTodayItems::ResetLayout();
            RefreshList();
            break;
    }
	return TRUE;
}

BOOL CDlgOptions::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_Add)
		AddItem();
	else if(uiMessage == WM_Remove)
		DeleteItem();

	return UNHANDLED;
}

void CDlgOptions::SetVoicemailNumber()
{
    CObjVoicemail objVmail;
       
    // put up the manual voicemail dialog
    CDlgSetVoicemail dlgVoice;
    if(dlgVoice.DoModal(m_hWnd, m_hInst, IDD_DLG_Modal) == IDOK)
    {
        if(m_oStr->GetLength(dlgVoice.GetVmailNumber()))
        {
            // save the new voicemail number in the registry
            objVmail.SetVmailNumber(dlgVoice.GetVmailNumber());
        }
    }
}

void CDlgOptions::AddItem()
{
	// launch the edit rules window
	CDlgAddTodayItem dlg(&m_gdiMem, &m_guiBackground);
	if(dlg.DoModal(m_hWnd, m_hInst, IDD_DLG_Modal) == IDOK)
	{
		RefreshList();
	}
}

void CDlgOptions::OnMenu()
{
    HMENU hMenuContext		= CreatePopupMenu();
    if(!hMenuContext)
        return;

    AppendMenu(hMenuContext, MF_STRING|(m_bIsLocked?MF_CHECKED:NULL), IDMENU_LockItems,  _T("Lock Items"));
    AppendMenu(hMenuContext, MF_STRING, IDMENU_RestoreDefaults,  _T("Restore Defaults"));
    AppendMenu(hMenuContext, MF_STRING, IDMENU_SetVoicemail,  _T("Set Voicemail number"));
    AppendMenu(hMenuContext, MF_STRING, IDMENU_Help,  _T("Help"));

    POINT pt;
    RECT rc;
    GetWindowRect(m_hWnd, &rc);
    pt.x	= rc.right - GetSystemMetrics(SM_CXVSCROLL);
    pt.y	= GetSystemMetrics(SM_CYSCREEN) - rc.top;

    //Display it.
    TrackPopupMenu(	hMenuContext, 
        TPM_RIGHTALIGN|TPM_BOTTOMALIGN, 
        pt.x,pt.y, 
        0, 
        m_hWnd, NULL);

    DestroyMenu(hMenuContext);
}

void CDlgOptions::DeleteItem()
{
    LVITEM lvi = {0};
    lvi.mask		= LVIF_PARAM;
    lvi.iItem = ListView_GetNextItem(m_hWndList, -1, LVNI_SELECTED);
    if(-1 != lvi.iItem)
    {
        if (!ListView_GetItem(m_hWndList, &lvi))
        {
            MessageBeep(MB_ICONHAND);
            return;
        }
    }
    else
        return;

    TCHAR* szName = (TCHAR*)lvi.lParam;

	DeleteValue(REG_TodayItems_Path, szName);

	RefreshList();
}
void CDlgOptions::RefreshList()
{
	ListView_DeleteAllItems(m_hWndList);

	// loop through all fixed registry items
	HKEY hKey;

	if(RegOpenKeyEx(HKEY_CURRENT_USER,REG_TodayItems_Path,0,KEY_QUERY_VALUE,&hKey) != ERROR_SUCCESS) 
		return;

	DWORD dwType	= 0;
	DWORD dwIndex	= 0;
	DWORD dwBufSize = sizeof(RegTodayItem);
	DWORD dwStringSize = 0;
	BYTE  btBuffer[2000];
	TCHAR szItemName[STRING_MAX];

	while(TRUE)
	{
		dwStringSize = STRING_MAX;
		ZeroMemory(&btBuffer, 2000);
        dwBufSize = 2000;

		if (RegEnumValue(hKey, dwIndex++,
			szItemName, &dwStringSize, 
			NULL, &dwType, 
			(LPBYTE)&btBuffer,
			&dwBufSize) != ERROR_SUCCESS)
			break;

		if(m_oStr->Compare(szItemName, _T("Default")) == 0)
			continue;

        LVITEM lvItem	= {0};
        lvItem.mask		= LVIF_TEXT | LVIF_PARAM;
        if(dwBufSize == sizeof(RegTodayItem))
        {
		    lvItem.lParam	= (LPARAM)m_oStr->CreateAndCopy(szItemName);
		    lvItem.pszText = szItemName;
        }
        else if(dwBufSize == sizeof(RegTodayLaunchItem))
        {
            RegTodayLaunchItem* sTodayLaunch = (RegTodayLaunchItem*)&btBuffer;
            lvItem.lParam	= (LPARAM)m_oStr->CreateAndCopy(szItemName);
            lvItem.pszText = sTodayLaunch->szFriendlyName;
        }
        else
            continue;

		ListView_InsertItem(m_hWndList, &lvItem);	
	}

    if(ListView_GetItemCount(m_hWndList) > 0)
    {
	    ListView_SortItems(m_hWndList, CompareItems, this);

	    // select the first item
	    LVITEM lvItem	= {0};
	    lvItem.mask		= LVIF_STATE;
	    lvItem.iItem	= 0;
	    lvItem.state	= LVIS_FOCUSED|LVIS_SELECTED;
	    lvItem.stateMask= LVIS_FOCUSED|LVIS_SELECTED;
	    ListView_SetItem(m_hWndList, &lvItem);
    }

    RegCloseKey(hKey);

}

BOOL CDlgOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// set size the listbox and anything else
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	const int xIndent = GetSystemMetrics(SM_CXICON)/4;
	const int yIndent = xIndent;

	m_rcCaption = rcClient;
	m_rcCaption.bottom = m_rcCaption.top + GetSystemMetrics(SM_CYCAPTION);
	m_rcCaption.left += xIndent;
	m_rcCaption.right -= xIndent;

	// buttons
	m_rcAddBtn = m_rcRemoveBtn = rcClient;
	m_rcAddBtn.top = m_rcRemoveBtn.top = rcClient.bottom - GetSystemMetrics(SM_CYICON);
	m_rcAddBtn.right	= m_rcRemoveBtn.left = WIDTH(rcClient)/2;

	RECT rcList = {0, m_rcCaption.bottom + yIndent, rcClient.right, m_rcAddBtn.top};
	if(m_hWndList)
		MoveWindow(m_hWndList, 
		rcList.left,rcList.top,WIDTH(rcList), HEIGHT(rcList),
		TRUE);

    //if(GetWindowLong(m_hWndList, GWL_STYLE) & WS_VSCROLL)
	    ListView_SetColumnWidth(m_hWndList, 0, WIDTH(rcClient) - GetSystemMetrics(SM_CXVSCROLL));
    //else
    //    ListView_SetColumnWidth(m_hWndList, 0, WIDTH(rcClient));

	return UNHANDLED;
}

BOOL CDlgOptions::OnLButtonDown(HWND hWnd, POINT& pt)
{
	m_btnAdd.OnLButtonDown(pt);
	m_btnRemove.OnLButtonDown(pt);

	return UNHANDLED;
}

BOOL CDlgOptions::OnLButtonUp(HWND hWnd, POINT& pt)
{
	m_btnAdd.OnLButtonUp(pt);
	m_btnRemove.OnLButtonUp(pt);

	return UNHANDLED;
}

BOOL CDlgOptions::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR lpnmh = (LPNMHDR)lParam;
    switch(lpnmh->code)
    {
    case LVN_DELETEITEM:
    {
        NMLISTVIEW* lplv = (NMLISTVIEW*)lpnmh;
        TCHAR* szItem = (TCHAR*)lplv->lParam;
        if(szItem)
            m_oStr->Delete(&szItem);
        break;
    }
    case NM_RECOGNIZEGESTURE:
        return TRUE; // we don't accept gestures
        break;
    case GN_CONTEXTMENU:
        break;
    case NM_DBLCLK:
    case NM_CLICK:
        break;
    default:
        return UNHANDLED;
        break;
    }
	return TRUE;
}
