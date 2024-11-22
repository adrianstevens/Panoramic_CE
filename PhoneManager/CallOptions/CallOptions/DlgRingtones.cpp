#include "StdAfx.h"
#include "DlgRingtones.h"
#include "CallOptionsGuiDefines.h"
#include "IssCommon.h"
#include "Resource.h"
#include "DlgAddRingtone.h"
#include "PoomContacts.h"
#include "ObjSkinEngine.h"
#include "CommonDefines.h"
#include "GeneralOptions.h"
#include "IssRegistry.h"
#include "DlgMsgBox.h"

#define WM_Add					WM_USER + 200
#define WM_Edit					WM_USER + 201
#define WM_Remove				WM_USER + 202


CDlgRingtones::CDlgRingtones()
{
    m_hBtnFont	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON)*14/16, FW_BOLD, TRUE);
    m_hFontBtnText	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_NORMAL, TRUE);
}

CDlgRingtones::~CDlgRingtones(void)
{
    CIssGDIEx::DeleteFont(m_hBtnFont);
    CIssGDIEx::DeleteFont(m_hFontBtnText);
}

BOOL CDlgRingtones::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RefreshList();

	return TRUE;
}


void CDlgRingtones::CreateButtons()
{
	// add, remove, edit
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

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
    
	m_btnAdd.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnAdd.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnAdd.Init(rcAddNew, _T("Add"), m_hWnd, WM_Add, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);

	m_btnEdit.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnEdit.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnEdit.Init(rcAddContact, _T("Edit"), m_hWnd, WM_Edit, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);

	m_btnRemove.InitAdvanced(MENU_BTNS_Grad1, MENU_BTNS_Grad2, MENU_BTNS_Outline1, MENU_BTNS_Outline2);
	m_btnRemove.SetTextColors(MENU_BTNS_TextColor, RGB(0,0,0));
	m_btnRemove.Init(rcRemove, _T("Remove"), m_hWnd, WM_Remove, 0, RGB(0,0,0), MENU_BTNS_Style, NULL);

}

BOOL CDlgRingtones::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_Add)
	{
		AddItem();
	}
	else if(uiMessage == WM_Edit)
	{
		EditItem();
	}
	else if(uiMessage == WM_Remove)
	{
		RemoveItem();
	}

	return UNHANDLED;
}

BOOL CDlgRingtones::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgRingtones::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(CDlgBaseOptions::OnLButtonDown(hWnd, pt))
    {}
    else if(m_btnAdd.OnLButtonDown(pt))
    {}
    else if(m_btnEdit.OnLButtonDown(pt))
    {}
    else if(m_btnRemove.OnLButtonDown(pt))
    {}

	return UNHANDLED;
}

BOOL CDlgRingtones::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(CDlgBaseOptions::OnLButtonUp(hWnd, pt))
    {}
    else if(m_btnAdd.OnLButtonUp(pt))
    {}
    else if(m_btnEdit.OnLButtonUp(pt))
    {}
    else if(m_btnRemove.OnLButtonUp(pt))
    {}

	return UNHANDLED;
}


BOOL CDlgRingtones::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_Back:
	case IDOK:
		break;
	case IDMENU_Add:
		AddItem();
		break;
	case IDMENU_Remove:
		RemoveItem();
		break;
	case IDMENU_Edit:
		EditItem();
		break;
	default:
		return UNHANDLED;
	}

	return TRUE;
}

BOOL CDlgRingtones::AddItem()
{
    CheckHandleRingtones();

	CDlgAddRingtone dlg(&m_objRingtoneRuleList);
    dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
	if(dlg.DoModal(m_hWnd, m_hInst, FALSE) == IDOK)
	{
        TypeRingtoneRule* sNewRule = new TypeRingtoneRule;
        if(sNewRule)
        {
            memcpy(sNewRule, dlg.GetRule(), sizeof(TypeRingtoneRule));
            if(!m_objRingtoneRuleList.AddItem(sNewRule))
                delete sNewRule;			
        }		
	}
    RefreshList();

	return TRUE;
}
BOOL CDlgRingtones::RemoveItem()
{
    CheckHandleRingtones();

	// get selection
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lParam)
        return FALSE;

	TypeRingtoneRule* sInfo = (TypeRingtoneRule*)sItem->lParam;

	m_objRingtoneRuleList.DeleteItem(sInfo);
	RefreshList();
    InvalidateRect(m_hWnd, NULL, FALSE);

	return TRUE;
}
BOOL CDlgRingtones::EditItem()
{
    CheckHandleRingtones();

    // get selection
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lParam)
        return FALSE;

    TypeRingtoneRule* sInfo = (TypeRingtoneRule*)sItem->lParam;

	CDlgAddRingtone dlg(&m_objRingtoneRuleList);
    dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
	dlg.SetEditItem(sInfo);
	if(IDOK == dlg.Launch(m_hWnd, m_hInst, FALSE))
    {
        // copy the new settings over
        memcpy(sInfo, dlg.GetRule(), sizeof(TypeRingtoneRule));
        m_objRingtoneRuleList.SaveList();

		if(dlg.GetRule()->eRuleType == RR_Contact && IsValidOID(dlg.GetRule()->iOID))
		{
			// set it in outlook
			CPoomContacts*  pOPoom = CPoomContacts::Instance();
			if(pOPoom)
			{
				IContact* pContact = pOPoom->GetContactFromOID(dlg.GetRule()->iOID);
				if(pContact)
				{
					pOPoom->SetContactRingtone(pContact, &dlg.GetRule()->ringToneInfo);
					pContact->Release();
				}
			}
		}
    }
    RefreshList();
    

	return TRUE;
}

void CDlgRingtones::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    BYTE* bt = (BYTE*)lpItem;
    delete bt;
}


void CDlgRingtones::RefreshList(BOOL bPreventSorting)
{
	// if we want to save selected, store it first...
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteMyItem);
    m_oMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu.ResetContent();

	// load vector from file
	m_objRingtoneRuleList.ReloadList();

	// add all the items

	// make sure we have a stored default item
	/*if(m_objRingtoneRuleList.GetListCount()  == 0 ||
		m_objRingtoneRuleList.GetItem(0)->eRuleType != RR_Default)
	{
		// add a default record to the list
		TypeRingtoneRule* defaultRingtoneRule = new TypeRingtoneRule;
		defaultRingtoneRule->eRuleType = RR_Default;
        defaultRingtoneRule->eRingType = RT_Ring;
		defaultRingtoneRule->iOID = 0;
		m_oStr->StringCopy(defaultRingtoneRule->szDesc, _T("Default ringtone"));

		HRESULT hr;
		SNDFILEINFO	sfRingtone1;
		hr = SndGetSound(SND_EVENT_RINGTONELINE1, &sfRingtone1);
		if(hr == S_OK)
			defaultRingtoneRule->ringToneInfo = sfRingtone1;
		m_objRingtoneRuleList.InsertItem(defaultRingtoneRule, 0);
	}*/

	// add the stored items...
	BOOL bAnyChanged = FALSE;
	for(int i=0; i<m_objRingtoneRuleList.GetListCount(); i++)
	{
		TypeRingtoneRule* pListItem = m_objRingtoneRuleList.GetItem(i);
		if(!pListItem)
			continue;
		// qualify this item...
		if(pListItem->eRuleType == RR_Contact)
		{
			SNDFILEINFO sfInfo;
			CPoomContacts* pPoom = CPoomContacts::Instance();
			if(pPoom)
			{
				IContact* pContact = pPoom->GetContactFromOID(pListItem->iOID);
				if(pContact)
				{
					if(pPoom->GetContactRingtone(pContact, &sfInfo))
					{
						if(m_oStr->Compare(pListItem->ringToneInfo.szDisplayName, sfInfo.szDisplayName) != 0)
						{
							// user has changed ringtone elsewhere
							// apply the new setting
							DebugOut(_T("changing contact ringtone %s - %s"), pListItem->ringToneInfo.szDisplayName, sfInfo.szDisplayName);
							pListItem->ringToneInfo = sfInfo;
							bAnyChanged = TRUE;
						}
					}
					pContact->Release();
					pContact = NULL;
				}
			}
		}

        BYTE* bt = new BYTE;
        *bt = 1;
        m_oMenu.AddItem(bt, 5000, 0, (LPARAM)pListItem);
	}
	if(bAnyChanged)
		m_objRingtoneRuleList.SaveList();

	TypeItems* sItem = m_oMenu.GetItem(0);
    if(sItem)
        m_oMenu.SetItemHeights(sItem->iHeight*3/2, sItem->iHeightSel*3/2);
	m_oMenu.SortList(CompareItems);
    m_oMenu.SetSelectedItemIndex(0, TRUE);

}

void CDlgRingtones::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgRingtones* pThis = (CDlgRingtones*)lpClass;
    if(!pThis || !sItem || !sItem->lParam)
        return;

    TypeRingtoneRule* sRing = (TypeRingtoneRule*)sItem->lParam;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, sRing);
}

BOOL CDlgRingtones::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, TypeRingtoneRule* sRing)
{
    RECT rcText = rc;
    rcText.left += INDENT;
    rcText.right-= INDENT;
    rcText.bottom = rc.top + HEIGHT(rc)/2;

    DrawTextShadow(gdi, sRing->szDesc, rcText, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rcText.top  = rcText.bottom;
    rcText.bottom   = rc.bottom;

    TCHAR szRingType[STRING_LARGE];
    CDlgAddRingtone::GetRintTypeText(sRing->eRingType, szRingType);

    SIZE sz;
    GetTextExtentPoint(gdi, szRingType, m_oStr->GetLength(szRingType), &sz);

    DrawText(gdi,  szRingType, rcText, DT_LEFT | DT_VCENTER, m_hFontBtnText, (bIsHighlighted?RGB(255,255,255):RGB(167,167,167)));

    rcText.left += sz.cx;

    DrawText(gdi,  sRing->ringToneInfo.szDisplayName, rcText, DT_RIGHT | DT_VCENTER, m_hFontBtnText, (bIsHighlighted?RGB(255,255,255):RGB(167,167,167)));
    return TRUE;
}


int CDlgRingtones::CompareItems(const void* lp1, const void* lp2)
{
	// permanent (default) rules first
	// category rules 2nd
	// contact rules last
    TypeItems& sListItem1 = **(TypeItems **)lp1;
    TypeItems& sListItem2 = **(TypeItems **)lp2;

    if(!sListItem1.lParam || !sListItem2.lParam)
        return 0;

	TypeRingtoneRule* sInfo1	= (TypeRingtoneRule*)sListItem1.lParam;
	TypeRingtoneRule* sInfo2	= (TypeRingtoneRule*)sListItem2.lParam;

	CIssString* oStr = CIssString::Instance();

	if(!sInfo1 || !sInfo2)
		return -1;

	if(sInfo2->eRuleType == RR_Default)
		return 1;
	if(sInfo1->eRuleType == RR_Default)
		return -1;
	if(sInfo1->eRuleType == RR_Category && sInfo2->eRuleType == RR_Contact)
		return -1;
	if(sInfo1->eRuleType == RR_Contact && sInfo2->eRuleType == RR_Category)
		return 1;

	return oStr->Compare(sInfo1->szDesc, sInfo2->szDesc);
}

BOOL CDlgRingtones::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
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
    m_btnEdit.Draw(m_hBtnFont, gdi);
    m_btnRemove.Draw(m_hBtnFont, gdi);

    return TRUE;
}

void CDlgRingtones::OnMenuLeft()
{
    SafeCloseWindow(IDOK);
}

void CDlgRingtones::OnMenuRight()
{

}

void CDlgRingtones::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Ringtones"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgRingtones::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
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

void CDlgRingtones::CheckHandleRingtones()
{
    DWORD dwOptions = 0;
    GetKey(REG_KEY_ISS_PATH, REG_GeneralOptions, dwOptions);

    // all good
    if(dwOptions & FLAG_SHOW_HandleRingtone)
        return;

    MessageBeep(MB_ICONHAND);
    CDlgMsgBox dlgMsg;
    dlgMsg.Init(m_gdiMem, m_guiBackground);
    if(IDYES == dlgMsg.PopupMessage(_T("Phone Genius is not currently set to handle ringtone playback.  Would you like to set Phone Genius to handle ringtones right now?"), _T("Handle Ringtones"), m_hWnd, m_hInst, MB_YESNO))
    {
        dwOptions |= FLAG_SHOW_HandleRingtone;
        SetKey(REG_KEY_ISS_PATH, REG_GeneralOptions, dwOptions);
    }

}


