#include "StdAfx.h"
#include "DlgAddTodayItem.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "IssRegistry.h"
#include "CommonDefines.h"
#include "CommonGuiDefines.h"
#include "ObjTodayItems.h"
#include "ObjTodayComponent.h"
#include "DlgChooseContact.h"
#include "resource.h"
#include "PoomContacts.h"
#include "GuiBackground.h"
#include "IssCommon.h"
#include "DlgChoosePrograms.h"
#include "DlgChooseSettings.h"

#define WM_Add		WM_USER + 200
#define WM_Cancel	WM_USER + 201



struct TodayItemDefaults
{
	ComponentType	eType;
	RECT			rcLocation;
};

static TodayItemDefaults s_TodayDefaults[] = {
	//{	CompType_Info_Provider,		{	-COMP_WIDTH_Provider,	-COMP_HEIGHT_Provider,	0,	0}	},
	{	CompType_Info_TimeLarge,	{	-COMP_WIDTH_Time, -COMP_HEIGHT_TimeLarge,	0,	0}	},
	{	CompType_Info_TimeSmall,	{	-COMP_WIDTH_Time, -COMP_HEIGHT_TimeSmall,	0,	0}	},
	{	CompType_Sys_Profile,		{	-COMP_WIDTH_Message,	-COMP_HEIGHT_Message,	0,	0}	},
	{	CompType_Message_Email,		{	-COMP_WIDTH_Message,	-COMP_HEIGHT_Message,	0,	0}	},
	{	CompType_Message_MissedCall,{	-COMP_WIDTH_Message,	-COMP_HEIGHT_Message,	0,	0}	},
	{	CompType_Message_SMS,		{	-COMP_WIDTH_Message,	-COMP_HEIGHT_Message,	0,	0}	},
	{	CompType_Message_VoiceMail,	{	-COMP_WIDTH_Message,	-COMP_HEIGHT_Message,	0,	0}	},
    //{	CompType_Message_Wifi,	    {	-COMP_WIDTH_Message,	-COMP_HEIGHT_Message,	0,	0}	},
    //{	CompType_Message_Bluetooth,	{	-COMP_WIDTH_Message,	-COMP_HEIGHT_Message,	0,	0}	},
	{	(ComponentType)-1,			{	-1,	-1,	-1,	-1}	}
};




CDlgAddTodayItem::CDlgAddTodayItem(CIssGDIEx* gdiMem, CGuiBackground* guiBackground)
:m_hWndList(NULL)
,m_gdiMem(gdiMem)
,m_guiBackground(guiBackground)
{	
	m_hCaptionFont	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*13/32, FW_BOLD, TRUE);
	m_hBtnFont		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
}

CDlgAddTodayItem::~CDlgAddTodayItem(void)
{
	// delete items
	for(int iItemIndex = 0; iItemIndex < ListView_GetItemCount(m_hWndList); iItemIndex++)
	{
		LV_ITEM lvi = {0};
		lvi.mask		= LVIF_PARAM;
		lvi.iItem		= iItemIndex;
		lvi.iSubItem	= 0;
		::SendMessage(m_hWndList, LVM_GETITEM, 0, (LPARAM)&lvi); 

		RegTodayItem* pRegItem = (RegTodayItem*)lvi.lParam;
		if(pRegItem)
		{
			delete pRegItem;
			pRegItem = NULL;
		}
	}

	if(m_hWndList)
	{
		ListView_DeleteAllItems(m_hWndList);
		m_hWndList = NULL;
	}
	CIssGDIEx::DeleteFont(m_hCaptionFont);
	CIssGDIEx::DeleteFont(m_hBtnFont);

	Destroy();
}

BOOL CDlgAddTodayItem::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//MessageBox(m_hWnd, _T("CDlgAddTodayItem::OnInitDialog()"), _T(""), MB_OK);

	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLGFULLSCREEN|SHIDIF_EMPTYMENU;
	DlgInfo.hDlg	= hWnd;

	SHInitDialog(&DlgInfo);

    SHMENUBARINFO mbi;
    memset(&mbi, 0, sizeof(mbi)); 
    mbi.cbSize		= sizeof(mbi);
    mbi.hwndParent	= hWnd;
    mbi.nToolBarId	= IDR_MENU_OkCancel;
    mbi.hInstRes	= m_hInst;
    SHCreateMenuBar(&mbi);

	InitControls();
	//CreateButtons();

	return TRUE;
}

BOOL CDlgAddTodayItem::InitControls()
{
	// create list window
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
	LVColumn.cx			= 240;		
	LVColumn.iSubItem	= 0;
	ListView_InsertColumn(m_hWndList, 0, &LVColumn);

	// add all items that are not selected
	RegTodayItem* pRegItem = 0;
	int iTableIndex = 0;
	TodayItemDefaults eDefault = s_TodayDefaults[iTableIndex++];
	while(eDefault.eType != -1)
	{
		pRegItem = new RegTodayItem;
		if(!GetTodayRegItem(GetComponentName(eDefault.eType), *pRegItem))
		{
			pRegItem->lID = 0;
			pRegItem->lType = eDefault.eType;
			pRegItem->rcPosition = eDefault.rcLocation;

			LVITEM lvItem	= {0};
			lvItem.mask		= LVIF_TEXT | LVIF_PARAM;
			lvItem.lParam	= (LPARAM)pRegItem;
			lvItem.pszText = GetComponentName(eDefault.eType);

			ListView_InsertItem(m_hWndList, &lvItem);	
		}
		else
			delete pRegItem;
		eDefault = s_TodayDefaults[iTableIndex++];
	}
    // add setting item
    RECT rcUndefined = {-COMP_WIDTH_Message, -COMP_HEIGHT_Message, 0, 0};
    pRegItem = new RegTodayItem;
    pRegItem->lID = 0;
    pRegItem->lType = CompType_Launch_Setting;
    pRegItem->rcPosition = rcUndefined;
    LVITEM lvItem	= {0};
    lvItem.mask		= LVIF_TEXT | LVIF_PARAM;
    lvItem.lParam	= (LPARAM)pRegItem;
    lvItem.pszText = _T("Add setting...");
    ListView_InsertItem(m_hWndList, &lvItem);

    // add program item
    SetRect(&rcUndefined, -COMP_WIDTH_Message, -COMP_HEIGHT_Message, 0, 0);
    pRegItem = new RegTodayItem;
    pRegItem->lID = 0;
    pRegItem->lType = CompType_Launch_Program;
    pRegItem->rcPosition = rcUndefined;
    ZeroMemory(&lvItem, sizeof(LVITEM));
    lvItem.mask		= LVIF_TEXT | LVIF_PARAM;
    lvItem.lParam	= (LPARAM)pRegItem;
    lvItem.pszText = _T("Add program...");
    ListView_InsertItem(m_hWndList, &lvItem);

	// add contact item
	SetRect(&rcUndefined, -COMP_WIDTH_Contact, -COMP_HEIGHT_Contact, 0, 0);
	pRegItem = new RegTodayItem;
	pRegItem->lID = 0;
	pRegItem->lType = CompType_Contact;
	pRegItem->rcPosition = rcUndefined;
	ZeroMemory(&lvItem, sizeof(LVITEM));
	lvItem.mask		= LVIF_TEXT | LVIF_PARAM;
	lvItem.lParam	= (LPARAM)pRegItem;
	lvItem.pszText = _T("Add contact...");
	ListView_InsertItem(m_hWndList, &lvItem);

	// select the first item
	ZeroMemory(&lvItem, sizeof(LVITEM));
	lvItem.mask		= LVIF_STATE;
	lvItem.iItem	= 0;
	lvItem.state	= LVIS_FOCUSED|LVIS_SELECTED;
	lvItem.stateMask= LVIS_FOCUSED|LVIS_SELECTED;
	ListView_SetItem(m_hWndList, &lvItem);

	return TRUE;
}

void CDlgAddTodayItem::CreateButtons()
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	m_btnOK.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnOK.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnOK.Init(m_rcAddBtn, _T("Add"), m_hWnd, WM_Add, 0, RGB(0,0,0), BTN_Bevel, NULL);

	m_btnCancel.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnCancel.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnCancel.Init(m_rcRemoveBtn, _T("Cancel"), m_hWnd, WM_Cancel, 0, RGB(0,0,0), BTN_Bevel, NULL);
}


BOOL CDlgAddTodayItem::GetTodayRegItem(TCHAR* szName, RegTodayItem& sItem)
{
	HKEY hKey;
	DWORD rc;  

	//open the Key  
	rc = RegOpenKeyEx(HKEY_CURRENT_USER, REG_TodayItems_Path, 0, KEY_QUERY_VALUE, &hKey);  
	if(rc != ERROR_SUCCESS)  
		return FALSE;  

	//get the data from the key  
	DWORD dwSize = sizeof(RegTodayItem);
	rc = RegQueryValueEx(hKey, szName, NULL, NULL, (LPBYTE)&sItem, &dwSize);  

	//close the key  
	RegCloseKey(hKey);  
	hKey = NULL;  

	return (rc == ERROR_SUCCESS);
}

int CDlgAddTodayItem::CompareItems(void* pt1, void* pt2)
{
	DWORD* dw1 = (DWORD*)pt1;
	DWORD* dw2 = (DWORD*)pt2;

	if(dw1 && dw2)
		return dw1 - dw2;
	return -1;
}

BOOL CDlgAddTodayItem::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem->GetDC() == NULL ||
		m_gdiMem->GetWidth() != (rcClient.right - rcClient.left) || 
		m_gdiMem->GetHeight() != (rcClient.bottom - rcClient.top))
	{
		m_gdiMem->Destroy();
		m_gdiMem->Create(hDC, rcClient, FALSE, FALSE, FALSE);
	}

	FillRect(m_gdiMem->GetDC(), rcClient, 0x00ffffff);

	// draw the caption
	DrawText(m_gdiMem->GetDC(), _T("Today: Add item..."), 
		m_rcCaption, DT_SINGLELINE|DT_VCENTER|DT_LEFT, m_hCaptionFont, ::GetSysColor(COLOR_HIGHLIGHT));

	// Paint lines
	RECT rcLine = rcClient;
	rcLine.top = m_rcCaption.bottom;
	rcLine.bottom = rcLine.top + 1;
	FillRect(m_gdiMem->GetDC(), rcLine, 0);

	// draw buttons
	//m_btnOK.Draw(m_hBtnFont, m_gdiMem.GetDC());
	//m_btnCancel.Draw(m_hBtnFont, m_gdiMem.GetDC());

	BitBlt(hDC,
		0,0,
		rcClient.right-rcClient.left, rcClient.bottom-rcClient.top,
		m_gdiMem->GetDC(),
		0,0,
		SRCCOPY);

	return TRUE;
}

BOOL CDlgAddTodayItem::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_Ok:
    case IDOK:
	{
        int iItemindex = ListView_GetNextItem(m_hWndList, -1, LVNI_SELECTED);
        if(iItemindex == -1)
            break;
		AddItem();
		ListView_DeleteAllItems(m_hWndList);
		EndDialog(hWnd, IDOK);
	}
	    break;
	case IDMENU_Cancel:
		ListView_DeleteAllItems(m_hWndList);
		EndDialog(hWnd, FALSE);
		break;
	}
	return TRUE;
}

BOOL CDlgAddTodayItem::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnmh = (LPNMHDR)lParam;
	switch(lpnmh->code)
	{
	case LVN_DELETEITEM:
	{
		NMLISTVIEW* lplv = (NMLISTVIEW*)lpnmh;
		RegTodayItem* pReg = (RegTodayItem*)lplv->lParam;
		if(pReg)
			delete pReg;
		break;
	}
    case NM_RECOGNIZEGESTURE:
        return TRUE; // we don't accept gestures
        break;
    case GN_CONTEXTMENU:
        break;
    case NM_DBLCLK:
    case NM_CLICK:
    case NM_RETURN:
        {
        int iItemindex = ListView_GetNextItem(m_hWndList, -1, LVNI_SELECTED);
        if(iItemindex == -1)
            break;
        AddItem();
        ListView_DeleteAllItems(m_hWndList);
        EndDialog(hWnd, IDOK);
        break;
        }
	default:
		return UNHANDLED;
		break;
	}
	return TRUE;
}

BOOL CDlgAddTodayItem::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_Add)
	{
		AddItem();
		ListView_DeleteAllItems(m_hWndList);
		EndDialog(hWnd, IDOK);
	}
	else if(uiMessage == WM_Cancel)
	{
		ListView_DeleteAllItems(m_hWndList);
		EndDialog(hWnd, FALSE);
	}

	return UNHANDLED;
}

void CDlgAddTodayItem::AddItem()
{
	// get the selected item and add to registry
	int iItemindex = ListView_GetNextItem(m_hWndList, -1, LVNI_SELECTED);
	if(iItemindex < 0)
		return;

	LV_ITEM lvi = {0};
	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= iItemindex;
	lvi.iSubItem	= 0;
	::SendMessage(m_hWndList, LVM_GETITEM, 0, (LPARAM)&lvi); 
	
	TCHAR szName[STRING_NORMAL];
	ListView_GetItemText(m_hWndList,iItemindex, 0, szName, sizeof(szName));

	RegTodayItem* pItem = (RegTodayItem*)lvi.lParam;
	if(pItem)
	{
		if(pItem->lType == CompType_Contact)
		{
			CDlgChooseContact dlg(m_hWnd);
            dlg.SetPreventDeleteList(TRUE);
            dlg.Init(m_gdiMem, m_guiBackground);
			if(dlg.Launch(m_hWnd, m_hInst, FALSE) == IDOK)
			{
				CPoomContacts* poPoom = CPoomContacts::Instance();
				long lOid = dlg.GetSelectedOID();
				TCHAR* szNewName = poPoom->GetUserName(lOid);
				SetTodayRegItem(szNewName, CompType_Contact, pItem->rcPosition, lOid);
                m_oStr->Delete(&szNewName);
			}
		}
        else if(pItem->lType == CompType_Launch_Program)
        {   
            CDlgChoosePrograms dlg;
            dlg.SetPreventDeleteList(TRUE);
            dlg.Init(m_gdiMem, m_guiBackground);
            if(dlg.Launch(m_hWnd, m_hInst, FALSE) == IDOK)
            {
                SetTodayRegLauncherItem(LT_Program, dlg.GetFriendlyName(), dlg.GetPath());
            }
        }
        else if(pItem->lType == CompType_Launch_Setting)
        {
            CDlgChooseSettings dlg;
            dlg.SetPreventDeleteList(TRUE);
            dlg.Init(m_gdiMem, m_guiBackground);
            if(dlg.Launch(m_hWnd, m_hInst, FALSE) == IDOK)
            {
                SetTodayRegLauncherItem(LT_Setting, dlg.GetFriendlyName(), dlg.GetPath());
            }
        }
		else
			SetTodayRegItem(szName, pItem->lType, pItem->rcPosition, pItem->lID);
	}

}

BOOL CDlgAddTodayItem::SetTodayRegItem(TCHAR* szName, long lType, RECT& rc, long lID)
{
	HKEY hKey;

	RegTodayItem sItem = {lType, rc, lID};

	if(RegCreateKeyEx(HKEY_CURRENT_USER, REG_TodayItems_Path, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, szName, 0,REG_BINARY, (LPBYTE)&sItem, sizeof(RegTodayItem));
        RegFlushKey(hKey);
        RegCloseKey(hKey);
		return TRUE;
	}

	return FALSE;
}

BOOL CDlgAddTodayItem::SetTodayRegLauncherItem(EnumLaunchType eType, TCHAR* szFriendlyName, TCHAR* szPath)
{
    HKEY hKey;

    if(m_oStr->IsEmpty(szPath) || m_oStr->IsEmpty(szFriendlyName))
        return FALSE;

    TCHAR szName[STRING_MAX];

    if(RegCreateKeyEx(HKEY_CURRENT_USER, REG_TodayItems_Path, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
        return FALSE;

    // find a key that's not taken
    int i;
    RegTodayLaunchItem sItem;
    DWORD dwSize;
    for(i=0; i<100; i++)
    {
        m_oStr->Format(szName, _T("Launcher_%.3d"), i);
        dwSize = sizeof(RegTodayLaunchItem);
        if(RegQueryValueEx(hKey, szName, 0, 0, (LPBYTE)&sItem, &dwSize))
            break;
        
    }

    ZeroMemory(&sItem, sizeof(RegTodayLaunchItem));
    sItem.lType = eType==LT_Program?CompType_Launch_Program:CompType_Launch_Setting;
    SetRect(&sItem.rcPosition, -COMP_WIDTH_Message, -COMP_HEIGHT_Message, 0, 0);
    m_oStr->StringCopy(sItem.szPath, szPath);
    m_oStr->StringCopy(sItem.szFriendlyName, szFriendlyName);

    m_oStr->Format(szName, _T("Launcher_%.3d"), i);
    RegSetValueEx(hKey, szName, 0,REG_BINARY, (LPBYTE)&sItem, sizeof(RegTodayLaunchItem));
        
    RegFlushKey(hKey);
    RegCloseKey(hKey);

    return TRUE;
}


BOOL CDlgAddTodayItem::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgAddTodayItem::OnLButtonDown(HWND hWnd, POINT& pt)
{
	m_btnOK.OnLButtonDown(pt);
	m_btnCancel.OnLButtonDown(pt);

	return UNHANDLED;
}

BOOL CDlgAddTodayItem::OnLButtonUp(HWND hWnd, POINT& pt)
{
	m_btnOK.OnLButtonUp(pt);
	m_btnCancel.OnLButtonUp(pt);

	return UNHANDLED;
}
