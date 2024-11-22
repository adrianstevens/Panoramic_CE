#include "StdAfx.h"
#include "DlgCallOptions.h"
#include "IssGDIeX.h"
#include "resource.h"
#include "DlgBlocklist.h"
#include "DlgCallingRules.h"
#include "DlgGeneralOptions.h"
#include "DlgTodayScreen.h"
#include "DlgRingtones.h"
#include "PoomContacts.h"
#include "CallOptionsGuiDefines.h"
#include "IssCommon.h"
#include "CommonDefines.h"
#include "DlgMsgBox.h"

#define BTN_Height          (GetSystemMetrics(SM_CXICON))
#define BTN_Width           (GetSystemMetrics(SM_CXICON)*5)
#define IDMENU_Option        5000

CDlgCallOptions::CDlgCallOptions(void)
{
    m_hFont = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*16/32, FW_BOLD, TRUE);
}

CDlgCallOptions::~CDlgCallOptions(void)
{
    CIssGDIEx::DeleteFont(m_hFont);

	// destroy all the static gdi's and such
	m_gdi.Destroy();
	m_gdiBackArrow.Destroy();
	m_gdiForwardArrow.Destroy();
	m_guiBG.Destroy();
	m_oMenu.ResetContent();
	m_wndMenu.ResetContent();
	m_imgBtn.Destroy();
	m_imgBtnSelected.Destroy();

	m_oStr->DeleteInstance();
	CPoomContacts::DeleteInstance();
}

BOOL CDlgCallOptions::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PopulateContent();
	return TRUE;
}

BOOL CDlgCallOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    //if(m_oMenu.GetOwnerWindow() != m_hWnd)
    //    return TRUE;

    CDlgBase::OnSize(hWnd, wParam, lParam);

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    int iCount = OPTION_Count;
    
   // int iHeight = min((iCount*BTN_Height), (HEIGHT(rcClient) - HEIGHT(m_rcBottomBar)*2 - BTN_Height));

    int iHeight = HEIGHT(rcClient) - HEIGHT(m_rcBottomBar)*2 - BTN_Height/2;

    while(iHeight < iCount*BTN_Height)
    {
        iCount--;
    }

    iHeight = iCount*BTN_Height;
    
    m_oMenu.OnSize((GetSystemMetrics(SM_CXSCREEN) - BTN_Width)/ 2, 
                    BTN_Height,
                    BTN_Width, iHeight);
    //              BTN_Width, OPTION_Count*BTN_Height);

	return TRUE;
}

void CDlgCallOptions::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    EnumOption* eCall = (EnumOption*)lpItem;
    delete eCall;
}

void CDlgCallOptions::PopulateContent(int iSelectedIndex)
{
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteMyItem);
    m_oMenu.SetSelected(0);
    m_oMenu.ResetContent();

    EnumOption* eOption = NULL;
    for(int i=0; i<OPTION_Count; i++)
    {
        eOption = new EnumOption;
        *eOption = (EnumOption)i;
        m_oMenu.AddItem(eOption, IDMENU_Option);
    }

    m_oMenu.SetItemHeights(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CXICON));
    m_oMenu.SetSelectedItemIndex(iSelectedIndex, TRUE);
}


BOOL CDlgCallOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDOK:
		m_oMenu.OnKeyDown(VK_RETURN, 0);
		break;
    case IDMENU_Option:
        LaunchOption();
        break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgCallOptions::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{

	case '3':
		{
			CDlgBlocklist dgl;
            dgl.Launch(hWnd, m_hInst, TRUE);
		}
		break;
	case '2':
		{
			CDlgCallingRules dgl;
			dgl.Launch(hWnd, m_hInst, TRUE);
		}
		break;
	case '1':
		{
			CDlgGeneralOptions dgl;
			dgl.Launch(hWnd, m_hInst, TRUE);
		}
		break;
	case '4':
		{
			CDlgRingtones dgl;
			dgl.Launch(hWnd, m_hInst, TRUE);
		}
		break;
	}

	return UNHANDLED;
}

BOOL CDlgCallOptions::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	if(uiMessage == WM_InvalTitle)
	{
		// hack to get window caption to refresh 
		// when returning to a modal window from a nested modal window
		TCHAR* szTitle = (TCHAR*)lParam;
		if(szTitle)
		{
			//::ShowWindow(m_hWnd, SW_SHOW);
			::SetWindowText(m_hWnd, szTitle);
		}
	}

	return UNHANDLED;
}

BOOL CDlgCallOptions::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    m_oMenu.OnDraw(gdi, rcClient, rcClip);

    return TRUE;
}

void CDlgCallOptions::OnMenuLeft()
{
    //SafeCloseWindow(IDOK);
    PostQuitMessage(0);
}

void CDlgCallOptions::OnMenuRight()
{
    TCHAR szTemp[STRING_MAX*2];
    m_oStr->Format(szTemp, _T("Phone Genius\r\n%s\r\n\r\nCall Genius\r\n%s\r\n\r\nCopyright 2009\r\nPanoramic Software Inc\r\nAll Rights Reserved\r\nwww.panoramicsoft.com"), VERSION_PhoneGenius, VERSION_CallGenius);
    CDlgMsgBox dlgMsg;
    dlgMsg.Init(m_gdiMem, &m_guiBG);
     dlgMsg.PopupMessage(szTemp, _T("About"), m_hWnd, m_hInst, MB_OK);
    //MessageBox(m_hWnd, szTemp, _T("About Call Genius"), MB_OK | MB_ICONINFORMATION);

}

void CDlgCallOptions::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Phone Options"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgCallOptions::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, _T("Exit"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rc = m_rcBottomBar;
    rc.left= rcClient.right/2;

    if(IsRectInRect(rc, rcClip))
    {
        DrawTextShadow(gdi, _T("About"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }
}

void CDlgCallOptions::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgCallOptions* pThis = (CDlgCallOptions*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eOption);
}

BOOL CDlgCallOptions::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption)
{
    DrawButton(gdi, bIsHighlighted, rc);

    Draw(gdi,
         rc.right - m_gdiForwardArrow.GetWidth() - 2*INDENT, rc.top + (HEIGHT(rc)-m_gdiForwardArrow.GetHeight())/2,
         m_gdiForwardArrow.GetWidth(), m_gdiForwardArrow.GetHeight(),
         m_gdiForwardArrow,
         0,0);

    TCHAR szTitle[STRING_MAX] = _T("");
    switch(*eOption)
    {
    case OPTION_GeneralOptions:
        m_oStr->StringCopy(szTitle, _T("General Options"));
        break;
    case OPTION_TodayScreen:
        m_oStr->StringCopy(szTitle, _T("Today Screen"));
        break;
    case OPTION_Ringtones:
        m_oStr->StringCopy(szTitle, _T("Ringtones"));
        break;
    case OPTION_BlockList:
        m_oStr->StringCopy(szTitle, _T("Phone Block List"));
        break;
    case OPTION_CallingRules:
        m_oStr->StringCopy(szTitle, _T("Calling Rules"));
        break;
    case OPTION_Help:
        m_oStr->StringCopy(szTitle, _T("Help"));
        break;
    default:
        return TRUE;
    }

    RECT rcDraw = rc;
    rcDraw.right = rcDraw.right - m_gdiForwardArrow.GetWidth() - 2*INDENT;
    //rcDraw.left  += INDENT;

    DrawTextShadow(gdi, szTitle, rcDraw, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFont, RGB(255,255,255), RGB(0,0,0));
    return TRUE;
}

void CDlgCallOptions::LaunchOption()
{
    int iSelectedIndex = m_oMenu.GetSelectedItemIndex();
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;


    EnumOption* eOption = (EnumOption*)sItem->lpItem;
    switch(*eOption)
    {
    case OPTION_BlockList:
        {
            CDlgBlocklist dgl;
            dgl.Launch(m_hWnd, m_hInst, TRUE);
        }
        break;
    case OPTION_GeneralOptions:
        {
            CDlgGeneralOptions dgl;
            dgl.Launch(m_hWnd, m_hInst, TRUE);
        }
        break;
    case OPTION_CallingRules:
        {
            CDlgCallingRules dgl;
            dgl.Launch(m_hWnd, m_hInst, TRUE);
        }
        break;
    case OPTION_Ringtones:
        {
            CDlgRingtones dgl;
            dgl.Launch(m_hWnd, m_hInst, TRUE);
        }
        break;
    case OPTION_TodayScreen:
        {
            CDlgTodayScreen dgl;
            dgl.Launch(m_hWnd, m_hInst, TRUE);
        }
        break;
    case OPTION_Help:
        {
            SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_SHOWSIPBUTTON);
            CreateProcess(_T("peghelp"), _T("incominghelp.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
            return;
        }
        break;
    }

    PopulateContent(iSelectedIndex);
}

