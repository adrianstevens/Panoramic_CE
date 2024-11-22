#include "StdAfx.h"
#include "DlgOptions.h"
#include "resource.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "Resource.h"

extern CObjGui* g_gui;

#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Item          0xD0D0D0
#define TEXTCOLOR_Selected      RGB(149,223,49) //RGB(255,246,0) //RGB(119,175,57)
#define IDMENU_Select           5000
#define IDMENU_SelectMenu       5001
#define BUTTON_Height           (GetSystemMetrics(SM_CXICON)*3/4)
#define HEIGHT_Text             (GetSystemMetrics(SM_CXICON)*4/9)
#define INDENT                  (GetSystemMetrics(SM_CXSMICON)/4)

CDlgOptions::CDlgOptions(void)
:m_bPlaySounds(TRUE)
{
    m_hFontLabel    = CIssGDIEx::CreateFont(HEIGHT_Text, FW_BOLD, TRUE);
    m_hFontBtnText	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_NORMAL, TRUE);
}

CDlgOptions::~CDlgOptions(void)
{
    CIssGDIEx::DeleteFont(m_hFontLabel);
    CIssGDIEx::DeleteFont(m_hFontBtnText);
}

void CDlgOptions::Init(HINSTANCE hInst, 
                       BOOL bPlaysounds, 
                       BOOL bAutoHold,
                       BOOL bQuickPlay,
                       EnumBackgroundColor eBG, 
                       EnumBetAmounts eBets, 
                       EnumMultiPlay eMulti)
{
    m_hInst         = hInst;
    m_bPlaySounds   = bPlaysounds;
    m_bAutoHold     = bAutoHold;
    m_bQuickPlay    = bQuickPlay;
    m_eBGColor      = eBG;
    m_eBet          = eBets;
    m_eMultiplay    = eMulti;
}

BOOL CDlgOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    g_gui->OnSize(hWnd, rc);
    m_oMenu.OnSize(rc.left,rc.top, WIDTH(rc), HEIGHT(rc));

    return TRUE;
}

BOOL CDlgOptions::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
    //Show as a Pocket PC full screen dialog.
    SHINITDLGINFO DlgInfo;

    DlgInfo.dwMask	= SHIDIM_FLAGS;
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
    DlgInfo.hDlg	= hWnd;
    SHInitDialog(&DlgInfo);
    

#ifdef WIN32_PLATFORM_WFSP
	::SendMessage(	mbi.hwndMB, 
		SHCMBM_OVERRIDEKEY, 
		VK_TBACK, 
		MAKELPARAM(SHMBOF_NODEFAULT | SHMBOF_NOTIFY, SHMBOF_NODEFAULT | SHMBOF_NOTIFY));
#endif

#endif

    m_oMenu.SetColors(TEXTCOLOR_Normal, TEXTCOLOR_Normal, TEXTCOLOR_Selected);
    m_oMenu.Initialize(hWnd, hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteItem);
    m_oMenu.SetSelected(IDB_PNG_MenuSelector);
    m_oMenu.SetImageArray(IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray);
    m_oMenu.PreloadImages(hWnd, m_hInst);

    PopulateOptions();
	InvalidateRect(hWnd, NULL, FALSE);
	return TRUE;
}

void CDlgOptions::SaveSettings()
{

}

BOOL CDlgOptions::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef WIN32_PLATFORM_WFSP
	if(HIWORD(lParam)==VK_TBACK && (0 != (MOD_KEYUP & LOWORD(lParam))))
	{
		EndDialog(m_hWnd, IDCANCEL);
	}
#endif
	return UNHANDLED;
}

BOOL CDlgOptions::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    g_gui->DrawBackground(rcClient);

    m_oMenu.OnDraw(g_gui->gdiMem, rcClient, rcClip);
	
    g_gui->DrawBottomBar(rcClient, _T("Save"), _T("Cancel"));

	g_gui->DrawScreen(rcClient, hDC);

	return TRUE;
}


BOOL CDlgOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int iReturn = 0;
	switch(LOWORD(wParam))
	{
    case IDMENU_Left:
	case IDMENU_Save:
		SaveSettings();
        SafeCloseWindow(IDOK);
        break;
    case IDMENU_Right:
	case IDMENU_Cancel:
		SafeCloseWindow(IDCANCEL);
		break;
    case IDMENU_Select:
    case IDOK:
        LaunchPopupMenu();
        break;
    case IDMENU_SelectMenu:
        SetMenuSelection();
        break;
	}

	return FALSE;
}

BOOL CDlgOptions::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnTimer(wParam, lParam);
}

BOOL CDlgOptions::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnKeyDown(wParam, lParam);
}

BOOL CDlgOptions::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonDown(pt))
    {}

	return g_gui->OnLButtonDown(hWnd, pt);
}

BOOL CDlgOptions::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonUp(pt))
    {}
    
	return g_gui->OnLButtonUp(hWnd, pt);
}

BOOL CDlgOptions::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnMouseMove(pt))
    {}

    return TRUE;
}

BOOL CDlgOptions::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return UNHANDLED;
}

BOOL CDlgOptions::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case VK_TSOFT1:
        SaveSettings();
        SafeCloseWindow(IDOK);
        break;
    case VK_ESCAPE:
    case VK_TSOFT2:
        SafeCloseWindow(IDCANCEL);
        break;
    default:

        return UNHANDLED;
    }
    return TRUE;
}

BOOL CDlgOptions::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) 
    { 
        SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);   

        return TRUE; 
    }
    return UNHANDLED;
}

void CDlgOptions::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgOptions* pThis = (CDlgOptions*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumOption* eInfo = (EnumOption*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eInfo);
}


BOOL CDlgOptions::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption)
{
    // draw the down arrow
    Draw(gdi,
        rc.right - m_oMenu.GetImageArrayGDI().GetHeight(), rc.top + (HEIGHT(rc)-m_oMenu.GetImageArrayGDI().GetHeight())/2,
        m_oMenu.GetImageArrayGDI().GetHeight(),m_oMenu.GetImageArrayGDI().GetHeight(),
        m_oMenu.GetImageArrayGDI(),
        (bIsHighlighted?2:3)*m_oMenu.GetImageArrayGDI().GetHeight(), 0);

    RECT rcText;

    TCHAR szTitle[STRING_MAX] = _T("");
    TCHAR szInfo[STRING_MAX]  = _T("");

    GetItemTitle(*eOption, szTitle);

    switch(*eOption)
    {

    case OPTION_PlaySounds:
        m_oStr->StringCopy(szInfo, m_bPlaySounds?_T(" On"):_T(" Off"));
        break;
    case OPTION_BetAmount:
        GetBetText(m_eBet, szInfo);
        break;
    case OPTION_Skin:
        GetSkinText(m_eBGColor, szInfo);
        break;
    case OPTION_Speed:
        m_oStr->StringCopy(szInfo, m_bQuickPlay?_T(" On"):_T(" Off"));
        break;
    case OPTION_AutoHold:
        m_oStr->StringCopy(szInfo, m_bAutoHold?_T(" On"):_T(" Off"));
        break;
    case OPTION_Multihand:
        GetMultiText(m_eMultiplay, szInfo);
        break;

    }

    // title text
    rcText			= rc;
    rcText.left		+= INDENT;
    DrawTextShadow(gdi, szTitle, rcText, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    // value text
    rcText.right	= rc.right - m_oMenu.GetImageArrayGDI().GetHeight();
    DrawText(gdi, szInfo, rcText, DT_RIGHT | DT_VCENTER, m_hFontBtnText, (bIsHighlighted?TEXTCOLOR_Normal:TEXTCOLOR_Item));

    return TRUE;
}

void CDlgOptions::GetMultiText(EnumMultiPlay eMulti, TCHAR* szInfo)
{
    switch(eMulti)
    {
    default:
    case MPLAY_Off:
        m_oStr->StringCopy(szInfo, _T(" Off"));
        break;
    case MPLAY_3:
        m_oStr->StringCopy(szInfo, _T(" 3 hands"));
        break;
    case MPLAY_5:
        m_oStr->StringCopy(szInfo, _T(" 5 hands"));
        break;
    }
}

void CDlgOptions::GetSkinText(EnumBackgroundColor eBGC, TCHAR* szInfo)
{
    switch(eBGC)
    {
    case BGCOLOR_Green:
        m_oStr->StringCopy(szInfo, _T(" Green"));
    	break;
    case BGCOLOR_Orange:
        m_oStr->StringCopy(szInfo, _T(" Orange"));
    	break;
    case BGCOLOR_Purple:
        m_oStr->StringCopy(szInfo, _T(" Purple"));
        break;
    case BGCOLOR_Red:
        m_oStr->StringCopy(szInfo, _T(" Red"));
        break;
    default://blue
        m_oStr->StringCopy(szInfo, _T(" Blue"));
        break;
    }
}

void CDlgOptions::GetBetText(EnumBetAmounts eBet, TCHAR* szInfo)
{
	switch(eBet)
	{
	default:
	case BA_1:
		m_oStr->StringCopy(szInfo, _T("1 Credit"));
		break;
	case BA_5:
		m_oStr->StringCopy(szInfo, _T("5 Credits"));
		break;
	case BA_10:
		m_oStr->StringCopy(szInfo, _T("10 Credits"));
	    break;
	case BA_20:
		m_oStr->StringCopy(szInfo, _T("20 Credits"));
	    break;
	case BA_50:
		m_oStr->StringCopy(szInfo, _T("50 Credits"));
		break;
	case BA_100:
		m_oStr->StringCopy(szInfo, _T("100 Credits"));
		break;
	}
}

void CDlgOptions::GetItemTitle(EnumOption eOption, TCHAR* szTitle)
{
    switch(eOption)
    {
    case OPTION_PlaySounds:
        m_oStr->StringCopy(szTitle, _T("Play sounds:"));
        break;
     case OPTION_Skin:
        m_oStr->StringCopy(szTitle, _T("Skin:"));
        break;
    case OPTION_BetAmount:
        m_oStr->StringCopy(szTitle, _T("Bet amount:"));
        break;
    case OPTION_Speed:
        m_oStr->StringCopy(szTitle, _T("Quick play"));
        break;
    case OPTION_AutoHold:
        m_oStr->StringCopy(szTitle, _T("Auto-hold"));
        break;
    case OPTION_Multihand:
        m_oStr->StringCopy(szTitle, _T("Multi-play:"));
        break;
    }
}


void CDlgOptions::PopulateOptions()
{
    m_oMenu.ResetContent();

//    m_oMenu.AddCategory(_T("General"));

    EnumOption* eOption = new EnumOption;
    *eOption = OPTION_Skin;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_Multihand;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_PlaySounds;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_AutoHold;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_Speed;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_BetAmount;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    m_oMenu.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}


void CDlgOptions::LaunchPopupMenu()
{
    g_gui->wndMenu.ResetContent();

    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;
//    TCHAR szTitle[STRING_MAX];
    TCHAR szInfo[STRING_MAX];
//    GetItemTitle(*eOption, szTitle);
//    g_gui->wndMenu.AddCategory(szTitle);
    int i;

    switch(*eOption)
    {
    case OPTION_PlaySounds:
        g_gui->wndMenu.AddItem(_T("On"), IDMENU_SelectMenu, (m_bPlaySounds?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(_T("Off"), IDMENU_SelectMenu, (!m_bPlaySounds?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_AutoHold:
        g_gui->wndMenu.AddItem(_T("On"), IDMENU_SelectMenu, (m_bAutoHold?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(_T("Off"), IDMENU_SelectMenu, (!m_bAutoHold?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_Speed:
        g_gui->wndMenu.AddItem(_T("On"), IDMENU_SelectMenu, (m_bQuickPlay?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(_T("Off"), IDMENU_SelectMenu, (!m_bQuickPlay?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_Multihand:
        for(i = 0; i < 3; i++)
        {
            EnumMultiPlay eMulti = (EnumMultiPlay)i;
            GetMultiText(eMulti, szInfo);
            g_gui->wndMenu.AddItem(szInfo, IDMENU_SelectMenu, (m_eMultiplay == eMulti?FLAG_Radio:NULL), eMulti);
        }
        break;
    case OPTION_Skin:
        for(i=0; i < BGCOLOR_Count; i++)
        {
            EnumBackgroundColor eSkins = (EnumBackgroundColor)i;
            GetSkinText(eSkins, szInfo);
            g_gui->wndMenu.AddItem(szInfo, IDMENU_SelectMenu, (m_eBGColor == eSkins?FLAG_Radio:NULL), eSkins);
        }
        break;
    case OPTION_BetAmount:
        for(i = 0; i < BA_Count - 1; i++)
        {
            EnumBetAmounts eBets = (EnumBetAmounts)i;
            GetBetText(eBets, szInfo);
            g_gui->wndMenu.AddItem(szInfo, IDMENU_SelectMenu, (m_eBet == eBets?FLAG_Radio:NULL), eBets);
        }
        break;
    }

    g_gui->wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.right/4,rcClient.top,
        rcClient.right*3/4, HEIGHT(rcClient) - HEIGHT(g_gui->rcBottomBar),
        0,0,0,0, ADJUST_Bottom);
}

void CDlgOptions::SetMenuSelection()
{
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    TypeItems* sMenuItem = g_gui->wndMenu.GetSelectedItem();
    if(!sMenuItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;

    switch(*eOption)
    {
    case OPTION_PlaySounds:
        m_bPlaySounds = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_Skin:
        {
            m_eBGColor = (EnumBackgroundColor)sMenuItem->lParam;
            g_gui->m_eBackground = m_eBGColor;
            g_gui->gdiBackground.Destroy();
            g_gui->gdiMem.Destroy();

            HDC dc = GetDC(m_hWnd);

            RECT rcClient;
            GetClientRect(m_hWnd, &rcClient);
            g_gui->InitGDI(rcClient, g_gui->gdiMem.GetDC());
            ReleaseDC(m_hWnd, dc);
            InvalidateRect(m_hWnd, NULL, FALSE);
        }
        break;
    case OPTION_BetAmount:
        m_eBet = (EnumBetAmounts)sMenuItem->lParam;
        break;
    case OPTION_Speed:
        m_bQuickPlay = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_Multihand:
        m_eMultiplay = (EnumMultiPlay)sMenuItem->lParam;
        break;
    case OPTION_AutoHold:
        m_bAutoHold = (BOOL)sMenuItem->lParam;
        break;
    }
}

void CDlgOptions::DeleteItem(LPVOID lpItem)
{
    if(lpItem == NULL)
        return;

    EnumOption* eOption = (EnumOption*)lpItem;
    if(eOption)
        delete eOption;
}
