#include "StdAfx.h"
#include "DlgCallingRules.h"
#include "DlgAddCallingRule.h"
#include "IssRegistry.h"
#include "resource.h"
#include "CallOptionsGuiDefines.h"
#include "IssCommon.h"
#include "DlgAddTimeSlot.h"
#include "ObjSkinEngine.h"

#define WM_Add		WM_USER + 200
#define WM_Remove	WM_USER + 201

CDlgCallingRules::CDlgCallingRules()
{
	m_hBtnFont		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON)*14/16, FW_BOLD, TRUE);
}

CDlgCallingRules::~CDlgCallingRules(void)
{
	CIssGDIEx::DeleteFont(m_hBtnFont);
}

BOOL CDlgCallingRules::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RefreshList();

	return TRUE;
}
void CDlgCallingRules::CreateButtons()
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	m_btnAdd.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnAdd.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnAdd.Init(m_rcAddBtn, _T("Add"), m_hWnd, WM_Add, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);

	m_btnRemove.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnRemove.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnRemove.Init(m_rcRemoveBtn, _T("Remove"), m_hWnd, WM_Remove, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);
}


/*BOOL CDlgCallingRules::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, FALSE, FALSE);

	FillRect(m_gdiMem.GetDC(), rcClient, RGB(255,255,255));
	::DrawText(m_gdiMem.GetDC(), _T("Calling Rules:"), m_rcTitleText, DT_SINGLELINE|DT_TOP|DT_LEFT, m_hTitleFont, 0);

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
}*/

BOOL CDlgCallingRules::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    CDlgBaseOptions::OnSize(hWnd, wParam, lParam);

    // so we only do this once
    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rcClient) != GetSystemMetrics(SM_CYSCREEN))
        return FALSE;

    // buttons
    m_rcAddBtn      = m_rcRemoveBtn = m_rcArea;
    m_rcAddBtn.bottom  = m_rcRemoveBtn.bottom = m_rcArea.bottom - INDENT/2;
    m_rcAddBtn.top  = m_rcRemoveBtn.top = m_rcArea.bottom - GetSystemMetrics(SM_CYICON);
    

    m_rcAddBtn.right = m_rcAddBtn.left + WIDTH(m_rcArea)/2 - INDENT/4;
    m_rcRemoveBtn.left = m_rcAddBtn.right + INDENT/2;

    CreateButtons();

    m_oMenu.OnSize(m_rcArea.left, 
        m_rcArea.top,
        WIDTH(m_rcArea), HEIGHT(m_rcArea) - GetSystemMetrics(SM_CYICON));

	return TRUE;
}

BOOL CDlgCallingRules::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(CDlgBaseOptions::OnLButtonDown(hWnd, pt))
    {}
    else if(m_btnAdd.OnLButtonDown(pt))
    {}
    else if(m_btnRemove.OnLButtonDown(pt))
    {}

	return UNHANDLED;
}

BOOL CDlgCallingRules::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(CDlgBaseOptions::OnLButtonUp(hWnd, pt))
    {}
    else if(m_btnAdd.OnLButtonUp(pt))
    {}
    else if(m_btnRemove.OnLButtonUp(pt))
    {}

	return UNHANDLED;
}

BOOL CDlgCallingRules::OnMenu()
{
	//Create the menu.
	/*HMENU hMenu			= CreatePopupMenu();

	if(!hMenu)
		return FALSE;

	int iIndex =  ListView_GetNextItem(m_hWndList, -1, LVNI_SELECTED);
	int iCount =  ListView_GetItemCount(m_hWndList);

	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, IDS_MENU_Add, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_Add, szText);

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

	DestroyMenu(hMenu);*/
	return TRUE;
}


BOOL CDlgCallingRules::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_Add:
		AddItem();
		break;
	case IDMENU_Remove:
		DeleteItem();
		break;
	case IDMENU_Menu:
		OnMenu();
		break;
	case IDMENU_Back:
		SafeCloseWindow(IDOK);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgCallingRules::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_Add)
		AddItem();
	else if(uiMessage == WM_Remove)
		DeleteItem();

	return UNHANDLED;
}

void CDlgCallingRules::AddItem()
{
    int iSelectedIndex = m_oMenu.GetSelectedItemIndex();

	// launch the edit rules window
	CDlgAddCallingRule dlg;
	if(dlg.Launch(m_hWnd, m_hInst, FALSE) == IDOK)
	{
		m_objCallOptionList.AddItem(dlg.GetRule());
        iSelectedIndex = m_objCallOptionList.GetListCount() - 1;
	}

    RefreshList(iSelectedIndex);

}

void CDlgCallingRules::DeleteItem()
{
    if(m_oMenu.GetItemCount() == 0)
        return;

    int iIndex = m_oMenu.GetSelectedItemIndex();
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lParam || iIndex == -1)
        return;

	TypeCallingRule* sInfo = (TypeCallingRule*)sItem->lParam;
	if(!sInfo)
		return;

	m_objCallOptionList.DeleteItem(sInfo);
    m_oMenu.RemoveItem(iIndex);
    m_oMenu.SetSelectedItemIndex(iIndex, TRUE);
    m_oMenu.ShowSelectedItem(TRUE);
    InvalidateRect(m_hWnd, NULL, FALSE);
}

/*BOOL CDlgCallingRules::OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// check to see if we need a resize of column width
	// shouldn't be needed, but checking because I have seen this condition here...
	BOOL bScrollbar = (GetWindowLong(m_hWndList, GWL_STYLE) & WS_VSCROLL) != 0;
	if(bScrollbar != m_bIsScrollBar)
	{
		SetListViewWidth();
		return FALSE;
	}

	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	if(m_gdiListMem.GetDC() == NULL)
		m_gdiListMem.Create(lpDrawItem->hDC, rcClient, FALSE, FALSE, FALSE);

	// Get the item pararm
	LV_ITEM lvi;
	lvi.mask		= LVIF_PARAM;
	lvi.iItem		= lpDrawItem->itemID;
	lvi.iSubItem	= 0;
	::SendMessage(m_hWndList, LVM_GETITEM, 0, (LPARAM)&lvi); 

	TypeCallingRule* sRule = (TypeCallingRule*)lvi.lParam;
	if(!sRule)
		return FALSE;

	int iHeight = HEIGHT(lpDrawItem->rcItem);
	int iWidth  = WIDTH(lpDrawItem->rcItem);

	RECT rcZero;
	rcZero.left		= 0;
	rcZero.right	= iWidth;
	rcZero.top		= 0;
	rcZero.bottom	= iHeight;
	RECT rcInset = rcZero;
	rcInset.top += 2;
	rcInset.left += 2;
	rcInset.right -= 2;
	rcInset.bottom -= 2;

	COLORREF crText = RGB(0,0,0);

	// if we have to recreate the selector
	if(lpDrawItem->itemState & LVIS_SELECTED  || lpDrawItem->itemState & LVIS_FOCUSED)
	{
		FillRect(m_gdiListMem.GetDC(), rcZero, GetSysColor(COLOR_HIGHLIGHT));
		crText = GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else
	{
		FillRect(m_gdiListMem.GetDC(), rcZero, RGB(255,255,255));
		Rectangle(m_gdiListMem.GetDC(), rcInset, GetListBGColor(lpDrawItem->itemID & 1), RGB(200,200,200))	;
	}


	// draw the contents
	int iTextIndent = GetSystemMetrics(SM_CXICON)/6;
	int iNumLinesOfText = HEIGHT(lpDrawItem->rcItem) > TEXT_HEIGHT*2 ? 2 : 1;
	RECT rcTemp = lpDrawItem->rcItem;
	rcTemp.left		+= iTextIndent;
	rcTemp.right	-= iTextIndent;
	rcTemp.top		= (HEIGHT(lpDrawItem->rcItem) - TEXT_HEIGHT*iNumLinesOfText)/2;

	static TCHAR szDisplayText[STRING_MAX];
	FormatDisplayText(sRule, szDisplayText);
	::DrawText(m_gdiListMem.GetDC(), 
		szDisplayText, 
		rcTemp, DT_WORDBREAK | DT_LEFT , m_hListFont, crText);

	// Draw focus rectangle if item has focus
	if (lpDrawItem->itemState & LVIS_FOCUSED && (GetFocus() == m_hWndList))
		DrawFocusRect(m_gdiListMem.GetDC(), &lpDrawItem->rcItem);

	// Draw all to the screen
	BitBlt(lpDrawItem->hDC,
		lpDrawItem->rcItem.left, lpDrawItem->rcItem.top,
		iWidth, iHeight,
		m_gdiListMem.GetDC(),
		0,0,
		SRCCOPY);

	return TRUE;
}

BOOL CDlgCallingRules::OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) lParam;
	lpMeasureItem->itemHeight = GetSystemMetrics(SM_CXICON)/2*3;

	return TRUE;
}*/


void CDlgCallingRules::FormatDisplayText(TypeCallingRule* sItem, TCHAR* szText)
{
	m_oStr->Empty(szText);
	m_oStr->Concatenate(szText, (TCHAR*)kAddCallingRuleTable[GetCallingRuleTableIndex(sItem->eCallOptionType)].szRuleText);
	switch(sItem->eCallOptionType)
	{
	case 	CO_AllowContactNever:
	case	CO_AllowContactAlways:
	case	CO_AllowCategoryNever:
	case	CO_AllowCategoryAlways:
    case    CO_DenyUnknownCallers:
		{
			// remove trailing "..."
			//int iDeletePos = m_oStr->FindLastOccurance(szText, _T("..."));	// bug
			//if(iDeletePos > 0 && iDeletePos < m_oStr->GetLength(szText))
			//	m_oStr->Delete(iDeletePos, 3, szText);
			int iDeletePos = m_oStr->GetLength(szText) - 3;
			if(iDeletePos > 0)
				m_oStr->Delete(iDeletePos, 3, szText);
			m_oStr->Concatenate(szText, _T(": "));
			m_oStr->Concatenate(szText, sItem->szDesc);
		}
		break;
	case	CO_RingerOffTime:	
		{
			TCHAR szStart[STRING_NORMAL];
			TCHAR szEnd[STRING_NORMAL];
			CDlgAddTimeSlot::FormatTimeString(szStart, sItem->uiStartHour);
			CDlgAddTimeSlot::FormatTimeString(szEnd, sItem->uiEndHour);
			m_oStr->Format(szText, _T("Turn ringer off between %s and %s"), szStart, szEnd);
		}
		break;
	case	CO_PhoneOffBusy:
		break;
	}
}

void CDlgCallingRules::RefreshList(int iSelectedIndex)
{
	// if we want to save selected, store it first...
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu.ResetContent();

	// load vector from file
	m_objCallOptionList.ReloadList();

    TCHAR szText[STRING_MAX];
    TCHAR szTemp[STRING_MAX];
	// ad all the items
	for(int i=0; i<m_objCallOptionList.GetListCount(); i++)
	{
		TypeCallingRule* pListItem = m_objCallOptionList.GetItem(i);
		if(!pListItem)
			continue;

        m_oStr->Empty(szText);
        switch(pListItem->eCallOptionType)
        {
        case CO_RingerOffTime:
            CDlgAddTimeSlot::FormatTimeString(szTemp, pListItem->uiEndHour);
            CDlgAddTimeSlot::FormatTimeString(szText, pListItem->uiStartHour);
            m_oStr->Insert(szText, _T("Ringer off: "));
            m_oStr->Concatenate(szText, _T(" - "));
            m_oStr->Concatenate(szText, szTemp);
            break;
        case CO_PhoneOffBusy:
            m_oStr->StringCopy(szText, _T("Ignore during busy appointments"));
            break;
        case CO_DenyUnknownCallers:
            m_oStr->StringCopy(szText, _T("Ignore callers not in contact list"));
            break;
        case CO_AllowContactNever:
            m_oStr->StringCopy(szText, _T("Never: "));
            m_oStr->Concatenate(szText, pListItem->szDesc);
            break;
        case CO_AllowContactAlways:
            m_oStr->StringCopy(szText, _T("Always: "));
            m_oStr->Concatenate(szText, pListItem->szDesc);
            break;
        case CO_AllowCategoryNever:
            m_oStr->StringCopy(szText, _T("Never: "));
            m_oStr->Concatenate(szText, pListItem->szDesc);
            break;
        case CO_AllowCategoryAlways:
            m_oStr->StringCopy(szText, _T("Always: "));
            m_oStr->Concatenate(szText, pListItem->szDesc);
            break;
        default:
            continue;
        }
        m_oMenu.AddItem(szText, 5000, 0, (LPARAM)pListItem);

	}
    // sort by string compare
    //m_oMenu.SortList(CompareItems);
    m_oMenu.SetSelectedItemIndex(iSelectedIndex, TRUE);


}

int CDlgCallingRules::CompareItems(const void* lp1, const void* lp2)
{
	int* i1	= (int*)lp1;
	int* i2	= (int*)lp2;

	return i1 > i2;
}

BOOL CDlgCallingRules::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
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
    m_btnAdd.Draw(m_hBtnFont, gdi);
    m_btnRemove.Draw(m_hBtnFont, gdi);

    return TRUE;
}

void CDlgCallingRules::OnMenuLeft()
{
    SafeCloseWindow(IDOK);
}

void CDlgCallingRules::OnMenuRight()
{

}

void CDlgCallingRules::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Calling Rules"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgCallingRules::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
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

