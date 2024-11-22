#include "StdAfx.h"
#include "DlgOptions.h"
#include "resource.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "Resource.h"
#include "IssLocalisation.h"


extern CObjGui* g_gui;

#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Item          0xD0D0D0
#define TEXTCOLOR_Selected      RGB(149,223,49) //RGB(255,246,0) //RGB(119,175,57)
#define IDMENU_Select           5000
#define IDMENU_SelectMenu       5001
#define BUTTON_Height           (GetSystemMetrics(SM_CXICON)*3/4)
#define HEIGHT_Text             (GetSystemMetrics(SM_CXICON)*13/32)
#define INDENT                  (GetSystemMetrics(SM_CXSMICON)/4)

CDlgOptions::CDlgOptions(void)
:m_bPlaySounds(TRUE)
,m_bShowAni(TRUE)
,m_oGame(NULL)
{
    m_hFontLabel    = CIssGDIEx::CreateFont(HEIGHT_Text, FW_BOLD, TRUE);
    m_hFontBtnText	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_NORMAL, TRUE);
}

CDlgOptions::~CDlgOptions(void)
{
    CIssGDIEx::DeleteFont(m_hFontLabel);
    CIssGDIEx::DeleteFont(m_hFontBtnText);
}


BOOL CDlgOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    g_gui->OnSize(hWnd, rc);
    m_oMenu.OnSize(rc.left,rc.top, WIDTH(rc), g_gui->rcBottomBar.top - rc.top);

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
    
/*
#ifdef WIN32_PLATFORM_WFSP
	::SendMessage(	mbi.hwndMB, 
		SHCMBM_OVERRIDEKEY, 
		VK_TBACK, 
		MAKELPARAM(SHMBOF_NODEFAULT | SHMBOF_NOTIFY, SHMBOF_NODEFAULT | SHMBOF_NOTIFY));
#endif*/

#endif

    m_oMenu.SetColors(TEXTCOLOR_Normal, TEXTCOLOR_Normal, TEXTCOLOR_Selected);
    m_oMenu.Initialize(hWnd, hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteItem);
    m_oMenu.SetSelected(IDB_PNG_MenuSelector);
    m_oMenu.SetImageArray(IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray);
    m_oMenu.PreloadImages(hWnd, m_hInst);


    if(m_oGame)
    {
     /*   m_bPlaySounds = m_oGame->GetPlaySounds();
        m_eAutoPlay = m_oGame->GetAutoPlay();
        m_eBGColor = m_oGame->GetBackgroundColor();
        m_eBGColorOrig = m_eBGColor;
        m_bInverse = m_oGame->GetInverse();
        m_bShowTimer = m_oGame->GetShowTimer();
        m_bStrict = m_oGame->GetStrict();
        m_eFCType = m_oGame->GetGameType();
        m_eFCCount = m_oGame->m_eFCCount;*/
    }
    
    PopulateOptions();
	InvalidateRect(hWnd, NULL, FALSE);
	return TRUE;
}

void CDlgOptions::SaveSettings()
{
    if(m_oGame)
    {
  /*      m_oGame->SetPlaySounds(m_bPlaySounds);
        m_oGame->SetAutoPlay(m_eAutoPlay);
        m_oGame->SetInverse(m_bInverse);
        m_oGame->SetStrict(m_bStrict);
        m_oGame->SetShowTimer(m_bShowTimer);
        m_oGame->m_eFCCount = m_eFCCount;//don't really need to reset
        m_oGame->SetGameType(m_eFCType);
     
        if(m_eBGColor != m_oGame->GetBackgroundColor())
        {
            m_oGame->SetBackgroundColor(m_eBGColor);
            g_gui->m_eBackground = m_eBGColor;

            RECT rcClient;
            GetClientRect(m_hWnd, &rcClient);

            g_gui->gdiBackground.Destroy();
            g_gui->gdiMem.Destroy();
         
            HDC dc = NULL;
            dc = GetDC(m_hWnd);
            g_gui->InitGDI(rcClient, dc);
            ReleaseDC(m_hWnd, dc);
            
        }*/
    }
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
	
    TCHAR szTemp[STRING_NORMAL];
    m_oStr->StringCopy(szTemp, ID(IDS_MENU_Cancel), STRING_NORMAL, m_hInst);
    g_gui->DrawBottomBar(rcClient, m_oStr->GetText(ID(IDS_MENU_Save), m_hInst), szTemp);

	g_gui->DrawScreen(rcClient, hDC);

	return TRUE;
}

void CDlgOptions::OnOk()
{
    SaveSettings();
    SafeCloseWindow(IDOK);
}

void CDlgOptions::OnCancel()
{
    if(m_eBGColor != m_eBGColorOrig)
    {   
        m_eBGColor = m_eBGColorOrig;
        if(m_eBGColorOrig == BGCOLOR_Random)
        {

        }
        else
        {
            g_gui->m_eBackground = m_eBGColorOrig;
            g_gui->gdiBackground.Destroy();
            g_gui->gdiMem.Destroy();

            RECT rcClient;
            GetClientRect(m_hWnd, &rcClient);
			HDC dc = GetDC(m_hWnd);
            g_gui->InitGDI(rcClient, dc);
			ReleaseDC(m_hWnd, dc);

            InvalidateRect(m_hWnd, NULL, FALSE);
        }

    }

    SafeCloseWindow(IDCANCEL);
}

BOOL CDlgOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int iReturn = 0;
	switch(LOWORD(wParam))
	{
    case IDMENU_Left:
	case IDMENU_Save:
		OnOk();
        break;
    case IDMENU_Right:
	case IDMENU_Cancel:
		OnCancel();
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
	else
		g_gui->OnLButtonDown(hWnd, pt);
	return TRUE;
}

BOOL CDlgOptions::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonUp(pt))
    {}
	else
		g_gui->OnLButtonUp(hWnd, pt);

	return TRUE;
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
        OnOk();
        break;
    case VK_ESCAPE:
    case VK_TSOFT2:
        OnCancel();
        break;
    default:
#ifdef WIN32_PLATFORM_WFSP

        return m_oMenu.OnKeyDown(wParam, lParam);
#endif
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

    rc.top += INDENT/2;
    rc.bottom -= INDENT/2;

    RECT rcText;

    TCHAR szTitle[STRING_MAX] = _T("");
    TCHAR szInfo[STRING_MAX]  = _T("");

    GetItemTitle(*eOption, szTitle);

    switch(*eOption)
    {
	case OPTION_Language:
	{
		DWORD dwLanguage = g_cLocale.GetCurrentLanguage();
		g_cLocale.GetLanguageText(szInfo, dwLanguage);
		break;
	}
    case OPTION_PlaySounds:
        m_oStr->StringCopy(szInfo, m_bPlaySounds?m_oStr->GetText(ID(IDS_OPT_On), m_hInst):m_oStr->GetText(ID(IDS_OPT_Off), m_hInst));
        break;
    case OPTION_Skin:
        GetSkinText(m_eBGColor, szInfo);
        break;
    case OPTION_ShowTimer:
        m_oStr->StringCopy(szInfo, m_bShowTimer?m_oStr->GetText(ID(IDS_OPT_On), m_hInst):m_oStr->GetText(ID(IDS_OPT_Off), m_hInst));
        break;
   }

	SIZE sz;
	HDC dc = GetDC(m_hWnd);
	HFONT hOld = (HFONT)SelectObject(dc, m_hFontLabel);
	GetTextExtentPoint(dc, szInfo, m_oStr->GetLength(szInfo), &sz);
	SelectObject(dc, hOld);
	ReleaseDC(m_hWnd, dc);

    // title text
    rcText			= rc;
    rcText.left		+= INDENT;
	rcText.right	= rc.right - m_oMenu.GetImageArrayGDI().GetHeight() - sz.cx;
    DrawTextShadow(gdi, szTitle, rcText, DT_LEFT | DT_WORDBREAK, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    // value text
    rcText.right	= rc.right - m_oMenu.GetImageArrayGDI().GetHeight();
    DrawText(gdi, szInfo, rcText, DT_RIGHT, m_hFontBtnText, (bIsHighlighted?TEXTCOLOR_Normal:TEXTCOLOR_Item));

    return TRUE;
}


void CDlgOptions::GetSkinText(EnumBackgroundColor eBGC, TCHAR* szInfo)
{
    switch(eBGC)
    {
    case BGCOLOR_Green:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_CLR_Green), m_hInst));
    	break;
    case BGCOLOR_Orange:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_CLR_Orange), m_hInst));
    	break;
    case BGCOLOR_Purple:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_CLR_Purple), m_hInst));
        break;
    case BGCOLOR_Red:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_CLR_Red), m_hInst));
        break;
    case BGCOLOR_Pink:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_CLR_Pink), m_hInst));
        break;
    case BGCOLOR_Random:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_CLR_Random), m_hInst));
        break;
    case BGCOLOR_Blue:
    default://blue
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_CLR_Blue), m_hInst));
        break;
    }
}


void CDlgOptions::GetItemTitle(EnumOption eOption, TCHAR* szTitle)
{
    switch(eOption)
    {
	case OPTION_Language:
		g_cLocale.GetLanguageOptionText(szTitle, g_cLocale.GetCurrentLanguage());
		break;
    case OPTION_PlaySounds:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_PlaySounds), m_hInst));
        break;
     case OPTION_Skin:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_TableColor), m_hInst));
        break; 
     case OPTION_ShowTimer:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_ShowTimer), m_hInst));
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
    *eOption = OPTION_PlaySounds;
    m_oMenu.AddItem(eOption, IDMENU_Select);

	eOption = new EnumOption;
	*eOption = OPTION_Language;
	m_oMenu.AddItem(eOption, IDMENU_Select);


    SIZE sz;
    HDC dc = GetDC(m_hWnd);
    HFONT hOld = (HFONT)SelectObject(dc, m_hFontLabel);
    GetTextExtentPoint(dc, _T("APp"), 3, &sz);
    SelectObject(dc, hOld);
    ReleaseDC(m_hWnd, dc);

    m_oMenu.SetItemHeights(2*sz.cy + INDENT, 2*sz.cy + INDENT);

    //m_oMenu.SetItemHeights(BUTTON_Height, BUTTON_Height);
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
	case OPTION_Language:
	{
		TCHAR szText[STRING_MAX];
		DWORD dwCurrent = g_cLocale.GetCurrentLanguage();
		g_cLocale.GetLanguageText(szText, LANG_ENGLISH);
		g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_ENGLISH?FLAG_Radio:NULL), LANG_ENGLISH);
		g_cLocale.GetLanguageText(szText, LANG_FRENCH);
		g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_FRENCH?FLAG_Radio:NULL), LANG_FRENCH); 
		g_cLocale.GetLanguageText(szText, LANG_GERMAN);
		g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_GERMAN?FLAG_Radio:NULL), LANG_GERMAN); 
		g_cLocale.GetLanguageText(szText, LANG_DUTCH);
		g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_DUTCH?FLAG_Radio:NULL), LANG_DUTCH); 
		g_cLocale.GetLanguageText(szText, LANG_PORTUGUESE);
		g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_PORTUGUESE?FLAG_Radio:NULL), LANG_PORTUGUESE); 
		g_cLocale.GetLanguageText(szText, LANG_KOREAN);
		g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_KOREAN?FLAG_Radio:NULL), LANG_KOREAN); 
        g_cLocale.GetLanguageText(szText, LANG_JAPANESE);
        g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_JAPANESE?FLAG_Radio:NULL), LANG_JAPANESE); 
		break;
	}
    case OPTION_ShowTimer:
        g_gui->wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_On2), m_hInst), IDMENU_SelectMenu, (m_bShowTimer?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_Off2), m_hInst), IDMENU_SelectMenu, (!m_bShowTimer?FLAG_Radio:NULL), FALSE);
        break;

    case OPTION_PlaySounds:
        g_gui->wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_On2), m_hInst), IDMENU_SelectMenu, (m_bPlaySounds?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_Off2), m_hInst), IDMENU_SelectMenu, (!m_bPlaySounds?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_Skin:
        for(i=0; i < BGCOLOR_Count; i++)
        {
            EnumBackgroundColor eSkins = (EnumBackgroundColor)i;
            GetSkinText(eSkins, szInfo);
            g_gui->wndMenu.AddItem(szInfo, IDMENU_SelectMenu, (m_eBGColor == eSkins?FLAG_Radio:NULL), eSkins);
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
	case OPTION_Language:
		g_cLocale.SetCurrentLanguage((DWORD)sMenuItem->lParam);
		PopulateOptions();
		break;
    case OPTION_ShowTimer:
        m_bShowTimer = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_PlaySounds:
        m_bPlaySounds = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_Skin:
        {
            m_eBGColor = (EnumBackgroundColor)sMenuItem->lParam;
            if(sMenuItem->lParam == BGCOLOR_Random)
                break;
            g_gui->m_eBackground = m_eBGColor;
            g_gui->gdiBackground.Destroy();
            g_gui->gdiMem.Destroy();

            RECT rcClient;
            GetClientRect(m_hWnd, &rcClient);
			HDC dc = GetDC(m_hWnd);
            g_gui->InitGDI(rcClient, dc);
			ReleaseDC(m_hWnd, dc);
			            
            InvalidateRect(m_hWnd, NULL, FALSE);
        }
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
