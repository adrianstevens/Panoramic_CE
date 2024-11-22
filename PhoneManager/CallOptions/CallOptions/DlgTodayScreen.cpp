#include "StdAfx.h"
#include "DlgTodayScreen.h"
#include "IssRegistry.h"
#include "Resource.h"
#include "CallOptionsGuiDefines.h"
#include "IssCommon.h"

#define IDMENU_Selection    5000
#define BTN_Height          (GetSystemMetrics(SM_CXICON)*5/4)

typedef void (*LAUNCHOPTION)(HWND); 

CDlgTodayScreen::CDlgTodayScreen()
{
}

CDlgTodayScreen::~CDlgTodayScreen(void)
{

}

BOOL CDlgTodayScreen::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RefreshList();

	return TRUE;
}


BOOL CDlgTodayScreen::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgTodayScreen::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case IDOK:
        m_oMenu.OnKeyDown(VK_RETURN, 0);
        break;
	case IDMENU_Selection:
        {
            TypeItems* sItem = m_oMenu.GetSelectedItem();
            if(sItem && sItem->lpItem)
            {
                EnumTodayScreen* eItem = (EnumTodayScreen*)sItem->lpItem;
                LaunchOption(*eItem);
            }
        }
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}


void CDlgTodayScreen::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    EnumTodayScreen* eItem = (EnumTodayScreen*)lpItem;
    delete eItem;
}

void CDlgTodayScreen::RefreshList()
{
	// if we want to save selected, store it first...
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteMyItem);
    m_oMenu.SetSelected(0);
    m_oMenu.ResetContent();   


	// add blank items - we draw them later
	for(int i=0; i < TS_NumItems; i++)
	{
		 EnumTodayScreen* eItem = new EnumTodayScreen;
         *eItem = (EnumTodayScreen)i;
         m_oMenu.AddItem(eItem, IDMENU_Selection);
	}

    m_oMenu.SetItemHeights(BTN_Height, BTN_Height);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}

BOOL CDlgTodayScreen::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    m_oMenu.OnDraw(gdi, rcClient, rcClip);

    return TRUE;
}

void CDlgTodayScreen::OnMenuLeft()
{
    SafeCloseWindow(IDOK);
}

void CDlgTodayScreen::OnMenuRight()
{
}

void CDlgTodayScreen::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Today Screen Options"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgTodayScreen::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
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
}

void CDlgTodayScreen::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgTodayScreen* pThis = (CDlgTodayScreen*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumTodayScreen* eItem = (EnumTodayScreen*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eItem);
}

BOOL CDlgTodayScreen::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumTodayScreen* eItem)
{
    DrawButton(gdi, bIsHighlighted, rc);

    TCHAR szText[STRING_MAX];
    switch(*eItem)
    {
    case TS_AddRemoveTodayScreen:
        m_oStr->StringCopy(szText, _T("Add Phone Genius today plugin..."));
        break;
    case TS_AddRemoveItems:
        m_oStr->StringCopy(szText, _T("Add/Remove plugin items..."));
        break;
    default:
        return TRUE;
    }

    RECT rcDraw = rc;
    rcDraw.left += 2*INDENT;
    rcDraw.top  += INDENT;
    rcDraw.right-= 2*INDENT;
    rcDraw.bottom-= INDENT;

    UINT uiFormat = DT_CENTER|DT_VCENTER;

    DrawTextShadow(gdi, szText, rcDraw, uiFormat, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    return TRUE;
}

void CDlgTodayScreen::LaunchOption(EnumTodayScreen& eItem)
{
    switch(eItem)
    {
    case TS_AddRemoveItems:
    {
        HMODULE hDll = NULL;
        LAUNCHOPTION fnLaunchOption = NULL;
        hDll = LoadLibrary(_T("\\windows\\panophonetoday.dll"));
        if(hDll)
        {
            fnLaunchOption	= (LAUNCHOPTION) GetProcAddress(hDll, _T("LaunchOptions"));
        }
        if(fnLaunchOption && hDll)
        {
            SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_SHOWSIPBUTTON);
            fnLaunchOption(m_hWnd);
        }
        if(hDll)
            FreeLibrary(hDll);
        break;
    }
    case TS_AddRemoveTodayScreen:
    {
        SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_SHOWSIPBUTTON);

        SHELLEXECUTEINFO sei = {0};
        sei.cbSize	= sizeof(sei);
        sei.hwnd	= m_hWnd;
        sei.nShow	= SW_SHOWNORMAL;
        sei.lpFile = _T("\\Windows\\ctlpnl.exe");
        sei.lpParameters = _T("cplmain.cpl,13,1");
        ShellExecuteEx(&sei);
        break;
    }
    }
}

