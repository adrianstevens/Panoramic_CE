#include "StdAfx.h"
#include "DlgBlocklist.h"
#include "IssGDIEX.h"
#include "resource.h"
#include "IssDynBtn.h"
#include "DlgChooseContact.h"
#include "DlgAddPhoneNumber.h"
#include "DlgCallOptions.h"
#include "PoomContacts.h"
#include "CallOptionsGuiDefines.h"
#include "CommonDefines.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "ObjSkinEngine.h"

#define WM_AddNew		WM_USER + 200
#define WM_AddContact	WM_USER + 201
#define WM_Remove		WM_USER + 202

CDlgBlocklist::CDlgBlocklist()
{
	m_hBtnFont	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON)*14/16, FW_BOLD, TRUE);
}

CDlgBlocklist::~CDlgBlocklist(void)
{
	CIssGDIEx::DeleteFont(m_hBtnFont);
}

BOOL CDlgBlocklist::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RefreshList();

	return TRUE;
}

BOOL CDlgBlocklist::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
    CDlgBaseOptions::OnSize(hWnd, wParam, lParam);

    // so we only do this once
    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rcClient) != GetSystemMetrics(SM_CYSCREEN))
        return FALSE;

    CreateButtons();

    m_oMenu.OnSize(m_rcArea.left, 
        m_rcArea.top,
        WIDTH(m_rcArea), HEIGHT(m_rcArea) - GetSystemMetrics(SM_CYICON));
	return TRUE;
}

BOOL CDlgBlocklist::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(CDlgBaseOptions::OnLButtonDown(hWnd, pt))
    {}
    else if(m_btnAddNew.OnLButtonDown(pt))
    {}
	else if(m_btnAddContact.OnLButtonDown(pt))
    {}
	else if(m_btnRemove.OnLButtonDown(pt))
    {}

	return TRUE;
}

BOOL CDlgBlocklist::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(CDlgBaseOptions::OnLButtonUp(hWnd, pt))
    {}
    else if(m_btnAddNew.OnLButtonUp(pt))
    {}
    else if(m_btnAddContact.OnLButtonUp(pt))
    {}
    else if(m_btnRemove.OnLButtonUp(pt))
    {}

    return TRUE;
}

BOOL CDlgBlocklist::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_Back:
	case IDOK:
		//EndDialog(hWnd, IDOK);
		break;
	case IDMENU_Menu:
		OnMenu();
		break;
	case IDMENU_AddNew:
		AddNew();
		break;
	case IDMENU_AddContact:
		AddContact();
		break;
	case IDMENU_Remove:
		DeleteItem();
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgBlocklist::OnMenu()
{
	//Create the menu.
	HMENU hMenu			= CreatePopupMenu();

	if(!hMenu)
		return FALSE;

	int iIndex = 0; // ListView_GetNextItem(m_hWndList, -1, LVNI_SELECTED);
	int iCount = 0; // ListView_GetItemCount(m_hWndList);

	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, IDS_MENU_AddNew, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_AddNew, szText);

	m_oStr->StringCopy(szText, IDS_MENU_AddContact, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_AddContact, szText);

	m_oStr->StringCopy(szText, IDS_MENU_Remove, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING|(iIndex==-1&&iCount>0?MF_GRAYED:NULL), IDMENU_Remove, szText);

	POINT pt;
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	pt.x	= rc.left +  GetSystemMetrics(SM_CXVSCROLL);
	pt.y	= GetSystemMetrics(SM_CYSCREEN) - rc.top;
	

	//Display it.
	TrackPopupMenu(	hMenu, 
					TPM_LEFTALIGN|TPM_BOTTOMALIGN, 
					pt.x,pt.y, 
					0, 
					m_hWnd, NULL);

	DestroyMenu(hMenu);
	return TRUE;
}

void CDlgBlocklist::CreateButtons()
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

    m_btnAddNew.Destroy();
    m_btnAddContact.Destroy();
    m_btnRemove.Destroy();

    int iBtnWidth;
    iBtnWidth = (WIDTH(m_rcArea)-INDENT)/3;
    int iBtnBig = WIDTH(m_rcArea) - 2*iBtnWidth - INDENT;

    RECT rcAddNew, rcAddContact, rcRemove;

    rcAddNew = rcAddContact = rcRemove = m_rcArea;
    rcAddNew.bottom = rcAddContact.bottom = rcRemove.bottom = m_rcArea.bottom - INDENT/2;
    rcAddNew.top = rcAddContact.top = rcRemove.top = m_rcArea.bottom - GetSystemMetrics(SM_CYICON);

    rcAddNew.right = rcAddNew.left + iBtnWidth;

    rcAddContact.left = rcAddNew.right + INDENT/2;
    rcAddContact.right = rcAddContact.left + iBtnBig;

    rcRemove.left = rcAddContact.right + INDENT/2;

	m_btnAddNew.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnAddNew.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnAddNew.Init(rcAddNew, _T("+ New"), m_hWnd, WM_AddNew, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);

	m_btnAddContact.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnAddContact.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnAddContact.Init(rcAddContact, _T("+ Contact"), m_hWnd, WM_AddContact, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);

	m_btnRemove.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnRemove.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnRemove.Init(rcRemove, _T("Remove"), m_hWnd, WM_Remove, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);


}

BOOL CDlgBlocklist::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_AddNew)
	{
		AddNew();
	}
	else if(uiMessage == WM_AddContact)
	{
		AddContact();
	}
	else if(uiMessage == WM_Remove)
	{
		DeleteItem();
	}

	return UNHANDLED;
}

void CDlgBlocklist::AddNew()
{
	// open the AddNew dlg
	CDlgAddPhoneNumber dlgAddPhone;
    dlgAddPhone.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
	if(dlgAddPhone.Launch(m_hWnd, m_hInst, FALSE) == IDOK)
	{
		TypeBlocklist* pItem = dlgAddPhone.GetSelectedItem();
		if(pItem && !m_oStr->IsEmpty(pItem->szID))
		{
			TypeBlocklist* sNew = new TypeBlocklist;
			if(sNew)
			{
				sNew->eType	= pItem->eType;
                m_oStr->StringCopy(sNew->szID, pItem->szID);
				m_objBlockList.AddItem(sNew);
			}
		}
	}
    RefreshList();
}

void CDlgBlocklist::AddContact()
{
	// open the AddContact dlg
	CDlgChooseContact dlgAddContact(m_hWnd, 0);
    dlgAddContact.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
	if(dlgAddContact.Launch(m_hWnd, m_hInst, FALSE) == IDOK)
	{
		int oid = dlgAddContact.GetSelectedOID();
		if(oid != -1)
			AddItem(oid);
	}
}

void CDlgBlocklist::AddItem(long oid)
{
	TypeBlocklist* pListItem = new TypeBlocklist;
    if(!pListItem)
        return;
	pListItem->eType = BlocklistItem_contact;

	// get the name text
	TCHAR szText[STRING_MAX];
	CPoomContacts* pPoom = CPoomContacts::Instance();
	m_oStr->Empty(szText);	

    if(!pPoom->GetFormatedNameText(szText, oid, TRUE))
    {
        delete pListItem;
        return;
    }
	
	m_oStr->StringCopy(pListItem->szID, szText);
    pListItem->lOid = oid;

	if(m_objBlockList.AddItem(pListItem))
    {
        m_oMenu.AddItem(pListItem->szID, 5000, 0, (LPARAM)pListItem);
        m_oMenu.SetSelectedItemIndex(m_oMenu.GetItemCount()-1, TRUE);
        m_oMenu.ShowSelectedItem(TRUE);
    }
    else
    {
        MessageBeep(MB_ICONHAND);
        delete pListItem;
    }
}

void CDlgBlocklist::DeleteItem()
{
    if(m_oMenu.GetItemCount() == 0)
        return;

    int iIndex = m_oMenu.GetSelectedItemIndex();
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lParam)
        return;

	TypeBlocklist* sInfo = (TypeBlocklist*)sItem->lParam;
	if(!sInfo)
		return;

	//if(m_objBlockList.FindItem(sInfo))		// FindItem bad... bug in CIssVector::SearchSortedArray
	//{
		m_objBlockList.DeleteItem(sInfo);
        m_oMenu.RemoveItem(iIndex);
        m_oMenu.SetSelectedItemIndex(iIndex, TRUE);
        m_oMenu.ShowSelectedItem(TRUE);
        InvalidateRect(m_hWnd, NULL, FALSE);
		//RefreshList();

	//}*/

}

void CDlgBlocklist::RefreshList(BOOL bPreventSorting)
{
	// if we want to save selected, store it first...
	m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
	m_oMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu.ResetContent();

	// load vector from file
	m_objBlockList.ReloadList();

	// ad all the items
	for(int i=0; i<m_objBlockList.GetListCount(); i++)
	{
		TypeBlocklist* pListItem = m_objBlockList.GetItem(i);
		if(!pListItem || !pListItem->szID)
			continue;

        m_oMenu.AddItem(pListItem->szID, 5000, 0, (LPARAM)pListItem);
	}

	// sort by string compare
    m_oMenu.SortList(CompareItems);

    m_oMenu.SetSelectedItemIndex(0, TRUE);

}

int CDlgBlocklist::CompareItems(const void* lp1, const void* lp2)
{
    TypeItems& sListItem1 = **(TypeItems **)lp1;
    TypeItems& sListItem2 = **(TypeItems **)lp2;

    if(!sListItem1.lParam || !sListItem2.lParam)
        return 0;

	TypeBlocklist* sBlock1	= (TypeBlocklist*)sListItem1.lParam;
	TypeBlocklist* sBlock2	= (TypeBlocklist*)sListItem2.lParam;

	CIssString* oStr = CIssString::Instance();

	if(!sBlock1 || !sBlock2 || oStr->IsEmpty(sBlock1->szID) || oStr->IsEmpty(sBlock2->szID))
		return -1;

	return oStr->Compare(sBlock1->szID, sBlock2->szID);
}


BOOL CDlgBlocklist::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    if(m_oMenu.GetItemCount() == 0)
    {
        RECT rc = m_rcArea;
        rc.top  += INDENT;
        DrawTextShadow(gdi, _T("No entries found"), rc, DT_CENTER|DT_TOP, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }
    else
        m_oMenu.OnDraw(gdi, rcClient, rcClip);

    // draw buttons
    m_btnAddNew.Draw(m_hBtnFont, gdi);
    m_btnAddContact.Draw(m_hBtnFont, gdi);
    m_btnRemove.Draw(m_hBtnFont, gdi);

    return TRUE;
}

void CDlgBlocklist::OnMenuLeft()
{
    SafeCloseWindow(IDOK);
}

void CDlgBlocklist::OnMenuRight()
{

}

void CDlgBlocklist::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Phone Block List"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgBlocklist::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    if(IsRectInRect(rc, rcClip))
    {
        DrawTextShadow(gdi, _T("Back"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

        // draw the arrow too
        Draw(gdi,
            WIDTH(m_rcBottomBar)/8 - m_gdiBackArrow.GetWidth(), rc.top + (HEIGHT(m_rcBottomBar)-m_gdiBackArrow.GetHeight())/2,
            m_gdiBackArrow.GetWidth(), m_gdiBackArrow.GetHeight(),
            m_gdiBackArrow,
            0,0);
    }
    rc.left = rc.right;
    rc.right= rcClient.right;
    //DrawTextShadow(gdi, _T("Menu"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

