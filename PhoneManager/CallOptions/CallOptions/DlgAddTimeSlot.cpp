#include "StdAfx.h"
#include "DlgAddTimeSlot.h"
#include "Resource.h"
#include "IssCommon.h"
#include "CallOptionsGuiDefines.h"
#include "ObjSkinEngine.h"

#define IDMENU_Selection        5000
#define IDMENU_Popup            5001

CDlgAddTimeSlot::CDlgAddTimeSlot()
:m_iStart(0)
,m_iEnd(0)
{
}

CDlgAddTimeSlot::~CDlgAddTimeSlot(void)
{

}

BOOL CDlgAddTimeSlot::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RefreshList();
	return TRUE;
}

void CDlgAddTimeSlot::FormatTimeString(TCHAR* szText, int iHour)
{
	BOOL bAM = (iHour < 12);
	int iDisplayHour = iHour % 12;
	if(iDisplayHour == 0)
		iDisplayHour = 12;

	CIssString::Instance()->Format(szText, _T("%i %s"), iDisplayHour, bAM ? _T("am") : _T("pm") );
}

BOOL CDlgAddTimeSlot::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgAddTimeSlot::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case IDMENU_Selection:
        LaunchPopupMenu();
		break;
	case IDMENU_Popup:
        SetSelection();
		break;
	default:
		return UNHANDLED;
	}

	return TRUE;
}

void CDlgAddTimeSlot::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    EnumTime* eOption = (EnumTime*)lpItem;
    delete eOption;
}

void CDlgAddTimeSlot::RefreshList()
{
    // if we want to save selected, store it first...
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteMyItem);
    m_oMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu.ResetContent();


    EnumTime* eTime = new EnumTime;
    *eTime = TIME_Start;
    m_oMenu.AddItem(eTime, IDMENU_Selection);

    eTime = new EnumTime;
    *eTime = TIME_End;
    m_oMenu.AddItem(eTime, IDMENU_Selection);
    
    //m_oMenu.SetItemHeights(BTN_Height, BTN_Height);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}

BOOL CDlgAddTimeSlot::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    m_oMenu.OnDraw(gdi, rcClient, rcClip);

    return TRUE;
}

void CDlgAddTimeSlot::OnMenuLeft()
{
    // let's do a check here that we have valid values
    UINT uiResult = IDOK;

    if(m_iStart == m_iEnd)
        uiResult = IDCANCEL;

    SafeCloseWindow(uiResult);
}

void CDlgAddTimeSlot::OnMenuRight()
{
    SafeCloseWindow(IDCANCEL);
}

void CDlgAddTimeSlot::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Select Time Slot"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgAddTimeSlot::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    if(!m_wndMenu.IsWindowUp(FALSE)) 
        DrawTextShadow(gdi, _T("Ok"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rc.left = rc.right;
    rc.right= rcClient.right;
    DrawTextShadow(gdi, _T("Cancel"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

void CDlgAddTimeSlot::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgAddTimeSlot* pThis = (CDlgAddTimeSlot*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumTime* eTime = (EnumTime*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eTime);
}

BOOL CDlgAddTimeSlot::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumTime* eTime)
{
    //DrawButton(gdi, bIsHighlighted, rc);

    Draw(gdi,
        rc.right - m_oMenu.GetImageArrayGDI().GetHeight(), rc.top + (HEIGHT(rc)-m_oMenu.GetImageArrayGDI().GetHeight())/2,
        m_oMenu.GetImageArrayGDI().GetHeight(),m_oMenu.GetImageArrayGDI().GetHeight(),
        m_oMenu.GetImageArrayGDI(),
        (bIsHighlighted?2:3)*m_oMenu.GetImageArrayGDI().GetHeight(), 0); 

    RECT rcDraw = rc;
    rcDraw.left += 2*INDENT;
    rcDraw.top  += INDENT;
    rcDraw.right= rc.right - m_oMenu.GetImageArrayGDI().GetHeight() - INDENT;
    rcDraw.bottom-= INDENT;

    UINT uiFormat = DT_VCENTER;
    TCHAR szTitle[STRING_LARGE];
    TCHAR szTime[STRING_LARGE];

    if(*eTime == TIME_Start)
    {
        m_oStr->StringCopy(szTitle, _T("Start Time:"));
        FormatTimeString(szTime, m_iStart);
    }
    else
    {
        m_oStr->StringCopy(szTitle, _T("End Time:"));
        FormatTimeString(szTime, m_iEnd);
    }

    DrawTextShadow(gdi, szTitle, rcDraw, uiFormat, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    uiFormat = DT_VCENTER|DT_RIGHT;

    DrawTextShadow(gdi, szTime, rcDraw, uiFormat, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    return TRUE;
}

void CDlgAddTimeSlot::LaunchPopupMenu()
{
    m_wndMenu.ResetContent();

    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    EnumTime* eTime = (EnumTime*)sItem->lpItem;
    TCHAR szTitle[STRING_MAX];
    int iTime;
    if(*eTime == TIME_Start)
    {
        m_oStr->StringCopy(szTitle, _T("Start Time:"));
        iTime = m_iStart;
    }
    else
    {
        m_oStr->StringCopy(szTitle, _T("End Time:"));
        iTime = m_iEnd;
    }
    m_wndMenu.AddCategory(szTitle);

    for(int i=0; i<24; i++)
    {
        FormatTimeString(szTitle, i);
        m_wndMenu.AddItem(szTitle, IDMENU_Popup, (iTime == i?FLAG_Radio:NULL), i);
    }

    m_wndMenu.SetSelectedItemIndex(iTime+1, TRUE);

    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_DrawScrollArrows,
        rcClient.right/4,m_rcTitle.bottom,
        rcClient.right*3/4, HEIGHT(m_rcArea),
        0,0,0,0, ADJUST_Bottom);
}

void CDlgAddTimeSlot::SetSelection()
{
    TypeItems* sItem = m_wndMenu.GetSelectedItem();
    if(!sItem || !sItem->lParam)
        return;

    int iSelection = (int)sItem->lParam;
    m_wndMenu.ResetContent();

    sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    EnumTime* eTime = (EnumTime*)sItem->lpItem;
    
    if(*eTime == TIME_Start)
        m_iStart = iSelection;
    else
        m_iEnd = iSelection;
    
}
