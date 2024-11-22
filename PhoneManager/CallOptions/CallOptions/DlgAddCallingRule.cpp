#include "StdAfx.h"
#include "DlgAddCallingRule.h"
#include "IssRegistry.h"
#include "Resource.h"
#include "CallOptionsGuiDefines.h"
#include "ObjCallOptionList.h"
#include "DlgChooseContact.h"
#include "DlgChooseCategory.h"
#include "DlgAddTimeSlot.h"
#include "PoomContacts.h"
#include "IssCommon.h"

#define IDMENU_Selection    5000



#define BTN_Height          (GetSystemMetrics(SM_CXICON)*5/4)

CDlgAddCallingRule::CDlgAddCallingRule()
{
}

CDlgAddCallingRule::~CDlgAddCallingRule(void)
{

}

BOOL CDlgAddCallingRule::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RefreshList();

	return TRUE;
}


BOOL CDlgAddCallingRule::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    CDlgBaseOptions::OnSize(hWnd, wParam, lParam);

    // so we only do this once
    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rcClient) != GetSystemMetrics(SM_CYSCREEN))
        return FALSE;

    m_oMenu.OnSize(m_rcArea.left, 
        m_rcArea.top,
        WIDTH(m_rcArea), HEIGHT(m_rcArea));

	return TRUE;
}

BOOL CDlgAddCallingRule::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case IDOK:
        m_oMenu.OnKeyDown(VK_RETURN, 0);
        break;
	case IDMENU_Selection:
		if(!AddCallingRule())
            RefreshList();
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgAddCallingRule::TextIsSingleLine(CIssGDIEx& gdi, TCHAR* szText, RECT& rcTest)
{
	int iStringLength = m_oStr->GetLength(szText);

	SIZE sTemp;
	GetTextExtentPoint(gdi, szText, iStringLength, &sTemp);

	if(sTemp.cx < WIDTH(rcTest))
		return TRUE;

	return FALSE;
}

void CDlgAddCallingRule::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    CallRuleType* eCall = (CallRuleType*)lpItem;
    delete eCall;
}

void CDlgAddCallingRule::RefreshList()
{
	// if we want to save selected, store it first...
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteMyItem);
    m_oMenu.SetSelected(0);
    m_oMenu.ResetContent();   


	// add blank items - we draw them later
	for(int i=0; i < NumCallOptionTypes; i++)
	{
		 CallRuleType* eCall = new CallRuleType;
         *eCall = (CallRuleType)i;
         m_oMenu.AddItem(eCall, IDMENU_Selection);
	}

	// sort by string compare
	m_oMenu.SortList(CompareItems);

    m_oMenu.SetItemHeights(BTN_Height, BTN_Height);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}

BOOL CDlgAddCallingRule::AddCallingRule()
{
	// get rule type from selected list index
	int iItemindex = m_oMenu.GetSelectedItemIndex();
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || iItemindex == -1 || !sItem->lpItem)
        return FALSE;

	CallRuleType* eRuleType = (CallRuleType*)sItem->lpItem;
	if(*eRuleType < 0 || *eRuleType >= NumCallOptionTypes)
		return FALSE;

	m_sRule.eCallOptionType = *eRuleType;

	switch(*eRuleType)
	{
	case CO_AllowContactNever:
	case CO_AllowContactAlways:
		{
			CDlgChooseContact dlg(m_hWnd, 0);
            dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
            if(dlg.Launch(m_hWnd, m_hInst, FALSE) == IDOK)
			{
				m_sRule.lOID = dlg.GetSelectedOID();
				CPoomContacts::Instance()->GetFormatedNameText(m_sRule.szDesc, m_sRule.lOID);
			}
			else
				return FALSE;
		}
		break;
	case CO_AllowCategoryNever:
	case CO_AllowCategoryAlways:
		{
			CDlgChooseCategory dlg;
            dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
			if(dlg.Launch(m_hWnd, m_hInst, FALSE) == IDOK)
			{
				m_oStr->StringCopy(m_sRule.szDesc, dlg.GetCategory());
			}
			else
				return FALSE;
		}
		break;
	case CO_RingerOffTime:
		{
			CDlgAddTimeSlot dlg;
            dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
            if(dlg.Launch(m_hWnd, m_hInst, FALSE) == IDOK)
			{
				if(dlg.GetStartTime() >= 0 && dlg.GetEndTime() >= 0)
				{
					m_sRule.uiStartHour = (UINT)dlg.GetStartTime();
					m_sRule.uiEndHour = (UINT)dlg.GetEndTime();
				}
                else
                    return FALSE;
			}
			else
				return FALSE;
		}
		break;
	case CO_PhoneOffBusy:
		break;
	}

    SafeCloseWindow(IDOK);

	return TRUE;
}

int CDlgAddCallingRule::CompareItems(const void* lp1, const void* lp2)
{
	// sort text alphabetically
	int iTableIndex1	= (int)lp1;
	int iTableIndex2	= (int)lp2;

	CIssString* oStr = CIssString::Instance();

	if(iTableIndex1 < 0 || iTableIndex1 >= NumCallOptionTypes ||
		iTableIndex2 < 0 || iTableIndex2 >= NumCallOptionTypes)
		return -1;

	return oStr->Compare((TCHAR*)kAddCallingRuleTable[iTableIndex1].szRuleText, (TCHAR*)kAddCallingRuleTable[iTableIndex2].szRuleText);
}

BOOL CDlgAddCallingRule::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    m_oMenu.OnDraw(gdi, rcClient, rcClip);

    return TRUE;
}

void CDlgAddCallingRule::OnMenuLeft()
{
    if(!AddCallingRule())
        RefreshList();
}

void CDlgAddCallingRule::OnMenuRight()
{
    SafeCloseWindow(IDCANCEL);
}

void CDlgAddCallingRule::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Select Calling Rule"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgAddCallingRule::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    DrawTextShadow(gdi, _T("Ok"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rc.left = rc.right;
    rc.right= rcClient.right;
    DrawTextShadow(gdi, _T("Cancel"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

void CDlgAddCallingRule::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgAddCallingRule* pThis = (CDlgAddCallingRule*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    CallRuleType* eCall = (CallRuleType*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eCall);
}

BOOL CDlgAddCallingRule::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, CallRuleType* eCall)
{
    DrawButton(gdi, bIsHighlighted, rc);

    TCHAR* szText = (TCHAR*)GetCallingRuleTableText(*eCall);
    if(!szText)
        return TRUE;

    RECT rcDraw = rc;
    rcDraw.left += 2*INDENT;
    rcDraw.top  += INDENT;
    rcDraw.right-= 2*INDENT;
    rcDraw.bottom-= INDENT;

    UINT uiFormat = DT_WORDBREAK;
    if(TextIsSingleLine(gdi, szText, rcDraw))
        uiFormat |= DT_VCENTER;

    DrawTextShadow(gdi, szText, rcDraw, uiFormat, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    return TRUE;
}

