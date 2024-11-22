#include "StdAfx.h"
#include "DlgOptions.h"
#include "resource.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "Resource.h"
#include "IssGDIFx.h"
#include "IssLocalisation.h"

extern CObjGui* g_gui;
extern CObjOptions* g_options;

#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Item          0xD0D0D0
#define TEXTCOLOR_Selected      RGB(149,223,49) //RGB(255,246,0) //RGB(119,175,57)
#define IDMENU_Select           5000
#define IDMENU_SelectMenu       5001
#define BUTTON_Height           (GetSysMets(SM_CXICON)*3/4)
#define HEIGHT_Text             (GetSysMets(SM_CXICON)*13/32)
#define INDENT                  (GetSysMets(SM_CXSMICON)/4)

CDlgOptions::CDlgOptions(void)
{
	m_hWndParent = NULL;

    m_hFontLabel    = CIssGDIEx::CreateFont(HEIGHT_Text, FW_BOLD, TRUE);
    m_hFontBtnText	= CIssGDIEx::CreateFont(GetSysMets(SM_CXICON)*3/7, FW_NORMAL, TRUE);
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
    m_oMenu.OnSize(rc.left,rc.top, WIDTH(rc), HEIGHT(rc) - MENU_HEIGHT);

    //set the menu rects
    SetRect(&m_rcMenuLeft, rc.left, rc.bottom - MENU_HEIGHT, rc.left + WIDTH(rc)/2, rc.bottom);
    SetRect(&m_rcMenuRight, m_rcMenuLeft.right, m_rcMenuLeft.top, rc.right, m_rcMenuLeft.bottom);

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

    m_bTransition = TRUE;


    m_oMenu.SetColors(TEXTCOLOR_Normal, TEXTCOLOR_Normal, TEXTCOLOR_Selected);
    m_oMenu.Initialize(hWnd, hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteItem);
    m_oMenu.SetSelected(IDB_PNG_MenuSelector);
    m_oMenu.SetImageArray(IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray);
    m_oMenu.PreloadImages(hWnd, m_hInst);

    m_dwOrigLanguage    = g_cLocale.GetCurrentLanguage();

    //get all of the current settings
    m_eSkin = g_gui->GetSkin();
    m_bLeftHanded	= g_options->GetLeftHanded();
    m_bPlaySounds	= g_gui->GetPlaySounds();
    m_eFretmarkers	= g_options->GetFretMarkers();
    m_eSampleFreq	= g_options->GetSampleFreq();
    m_iTrigger		= g_options->GetiTrigger();
    m_eStrum		= g_options->GetStrumStyle();
    m_eMetSounds	= g_options->GetMetroSound();
	m_eInst			= g_options->GetGen()->GetInstrument();
   
    PopulateOptions();
	InvalidateRect(hWnd, NULL, FALSE);
	return TRUE;
}

void CDlgOptions::SaveSettings()
{
    g_gui->SetSkin(m_eSkin);
    g_gui->SetPlaySounds(m_bPlaySounds);
    g_options->SetLeftHanded(m_bLeftHanded);
    g_options->SetFretMarkers(m_eFretmarkers);
    g_options->SetSampleFreq(m_eSampleFreq);
    g_options->SetTrigger(m_iTrigger);
    g_options->SetStrumStyle(m_eStrum);
	g_options->SetMetroSound(m_eMetSounds);
	g_options->GetGen()->SetInstrument(m_eInst, TRUE);
}

void CDlgOptions::CancelSettings()
{
    // set the language back if it needs it
    DWORD dwCurrent = g_cLocale.GetCurrentLanguage();
    if(dwCurrent != m_dwOrigLanguage)
        g_cLocale.SetCurrentLanguage(m_dwOrigLanguage);
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

    BitBlt(*g_gui->GetGDI(), rcClip.left, rcClip.top, WIDTH(rcClip), HEIGHT(rcClip), g_gui->GetAltBack()->GetDC(), rcClip.left, rcClip.top, SRCCOPY);

    m_oMenu.OnDraw(*g_gui->GetGDI(), rcClient, rcClip);

    g_gui->DrawMenu(rcClip, g_gui->GetGDI(), FALSE);

    //draw the bottom menu text
    DrawMenuText(rcClip, *g_gui->GetGDI());

    if(m_bTransition)
        FadeInScreen();
	
	g_gui->DrawToScreen(rcClient, hDC);

	return TRUE;
}

BOOL CDlgOptions::DrawMenuText(RECT& rcClip, CIssGDIEx& gdi)
{
    //SAVE CANCEL
    DrawText(gdi.GetDC(), m_oStr->GetText(ID(IDS_MENU_Save), m_hInst), m_rcMenuLeft, DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_MenuLarge), 0xFFFFFF);
    DrawText(gdi.GetDC(), m_oStr->GetText(ID(IDS_MENU_Cancel), m_hInst), m_rcMenuRight, DT_CENTER | DT_VCENTER, g_gui->GetFont(FONT_MenuLarge), 0xFFFFFF);


    RECT rc;
    GetClientRect(m_hWnd, &rc);

    //and finally ... the divider
    int iSpacing = (WIDTH(rc))/2;
    int iIndent = (MENU_HEIGHT)/5;

    POINT ptS, ptE;

    ptS.x = iSpacing-1;
    ptS.y = rc.bottom - MENU_HEIGHT + iIndent;
    ptE.x = iSpacing-1; 
    ptE.y = rc.bottom - iIndent;

    Line(gdi.GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0);
    ptE.x++;
    ptS.x++;
    Line(gdi.GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0x666666);

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
        CancelSettings();
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
    switch(LOWORD(wParam))
    {
    case VK_LEFT:
        DrawMenuHilite(TRUE);
        break;
    case VK_RIGHT:
        DrawMenuHilite(FALSE);
        break;
    }

    return m_oMenu.OnKeyDown(wParam, lParam);
}

BOOL CDlgOptions::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(g_gui->m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonDown(pt))
    {
    }


    if(PtInRect(&m_rcMenuLeft, pt))
    {
        DrawMenuHilite(TRUE);
    }
    else if(PtInRect(&m_rcMenuRight, pt))
    {
        DrawMenuHilite(FALSE);
    }

	return g_gui->OnLButtonDown(hWnd, pt);
}

BOOL CDlgOptions::DrawMenuHilite(BOOL bLeft)
{
    RECT rcTemp;
    if(bLeft)
        rcTemp = m_rcMenuLeft;
    else 
        rcTemp = m_rcMenuRight;


    POINT ptDraw;
    ptDraw.y = rcTemp.top + (HEIGHT(rcTemp) - g_gui->GetGlow()->GetHeight())/2;

    ptDraw.x = rcTemp.left + (GetSysMets(SM_CXSCREEN)/2 - g_gui->GetGlow()->GetWidth())/2;

    ::Draw(*g_gui->GetGDI(), ptDraw.x, ptDraw.y, 
        g_gui->GetGlow()->GetWidth(), g_gui->GetGlow()->GetHeight(),
        *g_gui->GetGlow());

    HDC dc = GetDC(m_hWnd);
    BitBlt(dc, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp),
        g_gui->GetGDI()->GetDC(), rcTemp.left, rcTemp.top, SRCCOPY);
    ReleaseDC(m_hWnd, dc);

    return TRUE;
}

BOOL CDlgOptions::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(g_gui->m_wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonUp(pt))
    {}
    if(PtInRect(&m_rcMenuLeft, pt))
    {
        SaveSettings();
        SafeCloseWindow(IDOK);   
    }
    else if(PtInRect(&m_rcMenuRight, pt))
    {
        CancelSettings();
        SafeCloseWindow(IDCANCEL);    
    }
    return g_gui->OnLButtonUp(hWnd, pt);
}

BOOL CDlgOptions::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(g_gui->m_wndMenu.IsWindowUp(TRUE))
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
  /*  switch(LOWORD(wParam))
    {
   case VK_TSOFT1:
        DrawMenuHilite(TRUE);
        SaveSettings();
        SafeCloseWindow(IDOK);
        break;
    case VK_ESCAPE:
    case VK_TSOFT2:
        DrawMenuHilite(FALSE);
        SafeCloseWindow(IDCANCEL);
        break;
    default:

        return UNHANDLED;
    }*/
    return TRUE;
}

BOOL CDlgOptions::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
  /*  if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) 
    { 
        SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSysMets(SM_CXSCREEN), GetSysMets(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);   

        return TRUE; 
    }*/
    return UNHANDLED;
}
void CDlgOptions::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgOptions* pThis = (CDlgOptions*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    RECT rc = rcDraw;

    rc.top += INDENT/2;
    rc.bottom -= INDENT/2;
    rc.left += INDENT;
    rc.right -= INDENT;

    EnumOption* eInfo = (EnumOption*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rc, eInfo);
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
    case OPTION_Language:
        {
            DWORD dwLanguage = g_cLocale.GetCurrentLanguage();
            g_cLocale.GetLanguageText(szInfo, dwLanguage);
        }
        break;
    case OPTION_Left:
        m_oStr->StringCopy(szInfo, m_bLeftHanded?m_oStr->GetText(ID(IDS_MENU_Yes), m_hInst):m_oStr->GetText(ID(IDS_MENU_No), m_hInst));
        break;
    case OPTION_PlaySounds:
        //m_oStr->StringCopy(szInfo, m_bPlaySounds?m_oStr->GetText(ID(IDS_OPT_On), m_hInst):m_oStr->GetText(ID(IDS_OPT_Off), m_hInst));
        switch(m_bPlaySounds)
        {
        case 0:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Off), m_hInst));
            break;
        case 1:
            m_oStr->StringCopy(szInfo, _T("1"));
            break;
        case 2:
            m_oStr->StringCopy(szInfo, _T("2"));
            break;
        case 3:
            m_oStr->StringCopy(szInfo, _T("3"));
            break;
        case 4:
        default:
            m_oStr->StringCopy(szInfo, _T("4 (max)"));
            break;
        }
        break;
    case OPTION_Skin:
        GetSkinText(m_eSkin, szInfo);
        break;
    case OPTION_TunerSampleFreq:
        m_oStr->Format(szInfo, m_oStr->GetText(ID(IDS_INFO_Hz), m_hInst), (int)m_eSampleFreq);
        break;
    case OPTION_TunerTrigger:
        m_oStr->Format(szInfo, _T("%i %%"), m_iTrigger*100/32);
        break;
    case OPTION_FretMarkers:
        if(m_eFretmarkers == FMARKERS_Roman)
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_RomanNumerals), m_hInst));
        else
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Numbers), m_hInst));
        break;
    case OPTION_Strum:
        switch(m_eStrum)
        {
        case STRUM_Slow:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_SlowStrum), m_hInst));
            break;
        case STRUM_Fast:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_FastStrum), m_hInst));
            break;
        case STRUM_Off:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_FreeStrum), m_hInst));
            break;
        }
        break;
    case OPTION_MetroSounds:
        switch(m_eMetSounds)
        {
        case METSND_Standard:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_TUN_Standard), m_hInst));
            break;
        case METSND_Metal:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Metal), m_hInst));
            break;
        case METSND_Wood:
            m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Wood), m_hInst));
            break;
        }
        break;
	case OPTION_Instrument:
		GetInstrumentText(m_eInst, szInfo);
		break;
    }

    // title text
    rcText			= rc;
    rcText.left		+= INDENT;
    DrawTextShadow(gdi, szTitle, rcText, DT_LEFT | DT_WORDBREAK, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    // value text
    rcText.right	= rc.right - m_oMenu.GetImageArrayGDI().GetHeight();
    DrawText(gdi, szInfo, rcText, DT_RIGHT, m_hFontBtnText, (bIsHighlighted?TEXTCOLOR_Normal:TEXTCOLOR_Item));

    return TRUE;
}

void CDlgOptions::GetInstrumentText(EnumInstrument eInst, TCHAR* szInfo)
{
	switch(eInst)
	{
	case INST_4StringBass:
		m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_BassGuitar), m_hInst));
		break;
    case INST_Lute:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Lute), m_hInst));
        break;
	case INST_Ukulele:
		m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Ukulele), m_hInst));
		break;
    case INST_Banjo:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Banjo), m_hInst));
        break;
//	case INST_5StringBass:
//		m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_BassGuitar5), m_hInst));
//		break;
	case INST_Guitar:
	default:
		m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Guitar), m_hInst));
		break;
	}
}

void CDlgOptions::GetSkinText(EnumSkin eSkin, TCHAR* szInfo)
{
//    m_oStr->Format(szInfo, _T("Skin %i"), (int)eSkin);
    switch(eSkin)
    {
    case SKIN_1:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Rosewood), m_hInst));
    	break;
    case SKIN_2:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Alder), m_hInst));
    	break;
    case SKIN_3:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Rebel), m_hInst));
        break;
    case SKIN_4:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Fire), m_hInst));
        break;
    case SKIN_5:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_SonicBlue), m_hInst));
    	break;
    case SKIN_6:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_Carbon), m_hInst));
    	break;
    case SKIN_7:
        m_oStr->StringCopy(szInfo, m_oStr->GetText(ID(IDS_OPT_PinkBubbles), m_hInst));
        break;
   }
}


void CDlgOptions::GetItemTitle(EnumOption eOption, TCHAR* szTitle)
{
    switch(eOption)
    {
    case OPTION_PlaySounds:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_PlaySounds), m_hInst));
        break;
    case OPTION_Left:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_LeftHanded), m_hInst));
        break;
    case OPTION_Skin:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_Skin), m_hInst));
        break;
    case OPTION_FretMarkers:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_FretIndicators), m_hInst));
        break;
    case OPTION_TunerSampleFreq:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_TunerFrequency), m_hInst));
        break;
    case OPTION_TunerTrigger:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_TunerCutoff), m_hInst));
        break;
    case OPTION_Strum:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_ChordStrumMode), m_hInst));
        break;
    case OPTION_MetroSounds:
        m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_MetronomeSound), m_hInst));
        break;
	case OPTION_Instrument:
		m_oStr->StringCopy(szTitle, m_oStr->GetText(ID(IDS_OPT_Instrument), m_hInst));
		break;
    case OPTION_Language:
        g_cLocale.GetLanguageOptionText(szTitle, g_cLocale.GetCurrentLanguage());
        break;
    }
}


void CDlgOptions::PopulateOptions()
{
    m_oMenu.ResetContent();

 //   m_oMenu.AddCategory(m_oStr->GetText(ID(IDS_OPT_Options), m_hInst));

    EnumOption* eOption = new EnumOption;
    *eOption = OPTION_Skin;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_FretMarkers;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_Left;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_Language;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_PlaySounds;
    m_oMenu.AddItem(eOption, IDMENU_Select);

	eOption = new EnumOption;
	*eOption = OPTION_Instrument;
	m_oMenu.AddItem(eOption, IDMENU_Select);

  //  m_oMenu.AddCategory(_T("Chords"));

    eOption = new EnumOption;
    *eOption = OPTION_Strum;
    m_oMenu.AddItem(eOption, IDMENU_Select);

  //  m_oMenu.AddCategory(_T("Tuner"));

    eOption = new EnumOption;
    *eOption = OPTION_TunerSampleFreq;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_TunerTrigger;
    m_oMenu.AddItem(eOption, IDMENU_Select);

	eOption = new EnumOption;
	*eOption = OPTION_MetroSounds;
	m_oMenu.AddItem(eOption, IDMENU_Select);

    SIZE sz;
    HDC dc = GetDC(m_hWnd);
    HFONT hOld = (HFONT)SelectObject(dc, m_hFontLabel);
    GetTextExtentPoint(dc, _T("APp"), 3, &sz);
    SelectObject(dc, hOld);
    ReleaseDC(m_hWnd, dc);

    m_oMenu.SetItemHeights(2*sz.cy, 2*sz.cy);

    //m_oMenu.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}


void CDlgOptions::LaunchPopupMenu()
{
    g_gui->m_wndMenu.ResetContent();

    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;
    TCHAR szInfo[STRING_MAX];

    int i = 0;

    switch(*eOption)
    {
    case OPTION_Language:
        {
            TCHAR szText[STRING_MAX];
            DWORD dwCurrent = g_cLocale.GetCurrentLanguage();
            g_cLocale.GetLanguageText(szText, LANG_ENGLISH);
            g_gui->m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_ENGLISH?FLAG_Radio:NULL), LANG_ENGLISH);
            g_cLocale.GetLanguageText(szText, LANG_SPANISH);
            g_gui->m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_SPANISH?FLAG_Radio:NULL), LANG_SPANISH);
            g_cLocale.GetLanguageText(szText, LANG_FRENCH);
            g_gui->m_wndMenu.AddItem(szText,  IDMENU_SelectMenu, (dwCurrent == LANG_FRENCH?FLAG_Radio:NULL), LANG_FRENCH);
            g_cLocale.GetLanguageText(szText, LANG_GERMAN);
            g_gui->m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_GERMAN?FLAG_Radio:NULL), LANG_GERMAN);
            g_cLocale.GetLanguageText(szText, LANG_DUTCH);
            g_gui->m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_DUTCH?FLAG_Radio:NULL), LANG_DUTCH);
            //g_cLocale.GetLanguageText(szText, LANG_RUSSIAN);
            //m_wndMenu->AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_RUSSIAN?FLAG_Radio:NULL), LANG_RUSSIAN);
            g_cLocale.GetLanguageText(szText, LANG_ITALIAN);
            g_gui->m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_ITALIAN?FLAG_Radio:NULL), LANG_ITALIAN);
            //g_cLocale.GetLanguageText(szText, LANG_PORTUGUESE);
            //g_gui->m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_PORTUGUESE?FLAG_Radio:NULL), LANG_PORTUGUESE); 
            g_cLocale.GetLanguageText(szText, LANG_JAPANESE);
            g_gui->m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_JAPANESE?FLAG_Radio:NULL), LANG_JAPANESE);
            //g_cLocale.GetLanguageText(szText, LANG_KOREAN);
            //g_gui->m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_KOREAN?FLAG_Radio:NULL), LANG_KOREAN);   
            //g_cLocale.GetLanguageText(szText, LANG_CHINESE);
            //g_gui->m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_CHINESE?FLAG_Radio:NULL), LANG_CHINESE);
        }
        break;
    case OPTION_PlaySounds:
        //g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_On), m_hInst), IDMENU_SelectMenu, (m_bPlaySounds?FLAG_Radio:NULL), TRUE);
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_Off), m_hInst), IDMENU_SelectMenu, (!m_bPlaySounds?FLAG_Radio:NULL), FALSE);
        g_gui->m_wndMenu.AddItem(_T("1"), IDMENU_SelectMenu, ((m_bPlaySounds==1)?FLAG_Radio:NULL), 1);
   //     g_gui->m_wndMenu.AddItem(_T("2"), IDMENU_SelectMenu, ((m_bPlaySounds==2)?FLAG_Radio:NULL), 2);
   //     g_gui->m_wndMenu.AddItem(_T("3"), IDMENU_SelectMenu, ((m_bPlaySounds==3)?FLAG_Radio:NULL), 3);
   //     g_gui->m_wndMenu.AddItem(_T("4"), IDMENU_SelectMenu, ((m_bPlaySounds==4)?FLAG_Radio:NULL), 4);
        break;    
    case OPTION_Left:
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_MENU_Yes), m_hInst), IDMENU_SelectMenu, (m_bLeftHanded?FLAG_Radio:NULL), TRUE);
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_MENU_No), m_hInst), IDMENU_SelectMenu, (!m_bLeftHanded?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_Skin:
        for(int i = 0; i < SKIN_Count; i++)
        {
            TCHAR szName[STRING_NORMAL];
            GetSkinText((EnumSkin)i, szName);
            g_gui->m_wndMenu.AddItem(szName, IDMENU_SelectMenu, (m_eSkin == (EnumSkin)i)?FLAG_Radio:NULL, i);
        }
        break;
    case OPTION_TunerSampleFreq:
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_11025Hz), m_hInst), IDMENU_SelectMenu, (m_eSampleFreq==FQ_SAMP_Low?FLAG_Radio:NULL), FQ_SAMP_Low);
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_22050Hz), m_hInst), IDMENU_SelectMenu, (m_eSampleFreq==FQ_SAMP_Med?FLAG_Radio:NULL), FQ_SAMP_Med);
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_44100Hz), m_hInst), IDMENU_SelectMenu, (m_eSampleFreq==FQ_SAMP_High?FLAG_Radio:NULL), FQ_SAMP_High);
        break;
    case OPTION_TunerTrigger:
        for(int i = 0; i < 32; i++)
        {
            m_oStr->Format(szInfo, _T("%i %%"), i*100/32);         
            g_gui->m_wndMenu.AddItem(szInfo, IDMENU_SelectMenu, (m_iTrigger==i?FLAG_Radio:NULL), i);
        }
        break;
    case OPTION_FretMarkers:
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_RomanNumerals), m_hInst), IDMENU_SelectMenu, ((m_eFretmarkers==FMARKERS_Roman)?FLAG_Radio:NULL), FMARKERS_Roman);
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_Numbers), m_hInst), IDMENU_SelectMenu, ((m_eFretmarkers==FMARKERS_Numbered)?FLAG_Radio:NULL), FMARKERS_Numbered);
        break;
    case OPTION_Strum:
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_SlowStrum), m_hInst), IDMENU_SelectMenu, (m_eSampleFreq==STRUM_Slow?FLAG_Radio:NULL), STRUM_Slow);
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_FastStrum), m_hInst), IDMENU_SelectMenu, (m_eSampleFreq==STRUM_Fast?FLAG_Radio:NULL), STRUM_Fast);
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_FreeStrum), m_hInst), IDMENU_SelectMenu, (m_eSampleFreq==STRUM_Off?FLAG_Radio:NULL), STRUM_Off);
        break;
    case OPTION_MetroSounds:
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_TUN_Standard), m_hInst), IDMENU_SelectMenu, (m_eSampleFreq==METSND_Standard?FLAG_Radio:NULL), METSND_Standard);
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_Wood), m_hInst), IDMENU_SelectMenu, (m_eSampleFreq==METSND_Wood?FLAG_Radio:NULL), METSND_Wood);
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_Metal), m_hInst), IDMENU_SelectMenu, (m_eSampleFreq==METSND_Metal?FLAG_Radio:NULL), METSND_Metal);
        break;
	case OPTION_Instrument:
		g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_Guitar), m_hInst), IDMENU_SelectMenu, (m_eInst==INST_Guitar?FLAG_Radio:NULL), INST_Guitar);
//		g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_BassGuitar5), m_hInst), IDMENU_SelectMenu, (m_eInst==INST_5StringBass?FLAG_Radio:NULL), INST_5StringBass);
		g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_BassGuitar), m_hInst), IDMENU_SelectMenu, (m_eInst==INST_4StringBass?FLAG_Radio:NULL), INST_4StringBass);
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_Lute), m_hInst), IDMENU_SelectMenu, (m_eInst==INST_Lute?FLAG_Radio:NULL), INST_Lute);
        g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_Banjo), m_hInst), IDMENU_SelectMenu, (m_eInst==INST_Banjo?FLAG_Radio:NULL), INST_Banjo);
		g_gui->m_wndMenu.AddItem(m_oStr->GetText(ID(IDS_OPT_Ukulele), m_hInst), IDMENU_SelectMenu, (m_eInst==INST_Ukulele?FLAG_Radio:NULL), INST_Ukulele);
		break;
    }
 
    g_gui->m_wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.left+WIDTH(rcClient)/4,rcClient.top,
        WIDTH(rcClient)*3/4, HEIGHT(rcClient) - HEIGHT(g_gui->m_rcBottomBar),
        rcClient.left,0,0,0, ADJUST_Bottom);
}

void CDlgOptions::SetMenuSelection()
{
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    TypeItems* sMenuItem = g_gui->m_wndMenu.GetSelectedItem();
    if(!sMenuItem)
        return;

    EnumOption* eOption = (EnumOption*)sItem->lpItem;

    switch(*eOption)
    {
    case OPTION_Language:
        g_cLocale.SetCurrentLanguage((DWORD)sMenuItem->lParam);
        PopulateOptions();
        break;
    case OPTION_PlaySounds://this still works
        m_bPlaySounds = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_Left:
        m_bLeftHanded = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_Skin:
        m_eSkin = (EnumSkin)sMenuItem->lParam;
        break;
    case OPTION_FretMarkers:
        m_eFretmarkers = (EnumFretMarkers)sMenuItem->lParam;
        break;
    case OPTION_TunerSampleFreq:
        m_eSampleFreq = (EnumSampleFreqID)sMenuItem->lParam;
        break;
    case OPTION_TunerTrigger:
        m_iTrigger = (int)sMenuItem->lParam;
        break;
    case OPTION_Strum:
        m_eStrum = (EnumStrum)sMenuItem->lParam;
        break;
    case OPTION_MetroSounds:
        m_eMetSounds = (EnumMetroSounds)sMenuItem->lParam;
        break;
	case OPTION_Instrument:
		m_eInst = (EnumInstrument)sMenuItem->lParam;
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

void CDlgOptions::FadeInScreen()
{
    m_bTransition = FALSE;
    HDC hDC = GetDC(m_hWnd);
    SlideLeft(hDC, *g_gui->GetGDI(), 500);
    ReleaseDC(m_hWnd, hDC);
}

BOOL CDlgOptions::OnMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcWindow;
	GetWindowRect(hWnd, &rcWindow);

	SetWindowPos(m_hWndParent, NULL, rcWindow.left, rcWindow.top, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
	return TRUE;
}