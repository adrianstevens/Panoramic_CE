#include "StdAfx.h"
#include "DlgOptions.h"
#include "resource.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "Resource.h"
#include "IssGDIFX.h"
#include "IssLocalisation.h"

extern CObjGui* g_gui;

#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Item          0xD0D0D0
#define TEXTCOLOR_Selected      RGB(149,223,49) //RGB(255,246,0) //RGB(119,175,57)
#define IDMENU_Select           5000
#define IDMENU_SelectMenu       5001
#define IDMENU_SelectImage      5002
#define IDMENU_SelectSkin       5003
#define BUTTON_Height           (GetSystemMetrics(SM_CXICON)*3/4)
#define HEIGHT_Text             (GetSystemMetrics(SM_CXICON)*4/9)
#define INDENT                  (GetSystemMetrics(SM_CXSMICON)/4)
#define WM_ImageLoaded          (WM_USER+500)

#define HEIGHT_Button           (IsVGA()?54:43)
#define SIZE_PicIndent          (IsVGA()?20:10) 
#define SIZE_PicBorder          (IsVGA()?4:2)
#define SIZE_Item               ((IsVGA()?208:104) + 2*SIZE_PicIndent)

TypePicSkinItem::TypePicSkinItem()
:sPic(NULL)
{}

TypePicSkinItem::~TypePicSkinItem()
{
    if(sPic)
        sPic->bIsDeleted = TRUE;
    sPic = NULL;
}

CDlgOptions::CDlgOptions(void)
:m_ePicSelection(PIC_None)
,m_eSelection(SEL_None)
{
    m_hFontLabel    = CIssGDIEx::CreateFont(HEIGHT_Text, FW_BOLD, TRUE);
    m_hFontBtnText	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_NORMAL, TRUE);

    InitializeCriticalSection(&m_crLoad);
}

CDlgOptions::~CDlgOptions(void)
{
    CIssGDIEx::DeleteFont(m_hFontLabel);
    CIssGDIEx::DeleteFont(m_hFontBtnText);

    ResetPicItems();

    DeleteCriticalSection(&m_crLoad);
}

void CDlgOptions::Init(HINSTANCE hInst, 
                       TypeOptions& sOptions)
{
    m_hInst         = hInst;

    memcpy(&m_sOptions, &sOptions, sizeof(TypeOptions));
    
    
}

BOOL CDlgOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    if(WIDTH(rc) != GetSystemMetrics(SM_CXSCREEN))
        return TRUE;
    if(HEIGHT(rc) != GetSystemMetrics(SM_CYSCREEN))
        return TRUE;

    g_gui->OnSize(hWnd, rc);
    m_oMenu.OnSize(rc.left,rc.top, WIDTH(rc), HEIGHT(rc) - HEIGHT(g_gui->rcBottomBar));

    m_rcPicSkin = rc;
    m_rcPicSkin.bottom = g_gui->rcBottomBar.top;
    m_rcPicSkin.top     = m_rcPicSkin.bottom - 2*BUTTON_Height - SIZE_Item;

    m_oPicSkin.OnSize(m_rcPicSkin.left+SIZE_PicBorder, m_rcPicSkin.top, WIDTH(m_rcPicSkin)-2*SIZE_PicBorder, HEIGHT(m_rcPicSkin));

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

    m_oPicSkin.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_DrawTitle|OPTION_DrawDots|OPTION_SnapCenter);
    m_oPicSkin.SetItemSize(SIZE_Item, SIZE_Item);
    m_oPicSkin.SetCustomDrawFunc(this, DrawListItem, DrawTitle, DrawDots);
    m_oPicSkin.SetDeleteItemFunc(DeleteMyPicListItem);
    m_oPicSkin.SetColors(TEXTCOLOR_Normal);
    m_oPicSkin.SetIndentSize(IsVGA()?80:40);

    m_oMenu.SetColors(TEXTCOLOR_Normal, TEXTCOLOR_Normal, TEXTCOLOR_Selected);
    m_oMenu.Initialize(hWnd, hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteItem);
    m_oMenu.SetSelected(IDB_PNG_MenuSelector);
    m_oMenu.SetImageArray(IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray);
    m_oMenu.PreloadImages(hWnd, m_hInst);

    m_oPicLoader.StartEngine(m_hWnd, m_hInst);

    LoadImages();

    PopulateOptions();
    SetPreview();

	InvalidateRect(hWnd, NULL, FALSE);
	return TRUE;
}

void CDlgOptions::SaveSettings()
{

}

//to initialize thumb based on current settings
void CDlgOptions::SetPreview()
{
    if(m_oStr->GetLength(m_sOptions.szPath))//we have an image ... just set it to the picture 
    {
        AddPicture(m_sOptions.szPath);//gui will ignore but it'll generate the preview        
        m_ePicSelection = PIC_Picture;
    }
    //otherwise are we using a custom gradient?
    else// if(m_sOptions.eSkin > SSKIN_Grey)
    {
        SetCustomPreview();//create the custom preview
        m_ePicSelection = PIC_Given;
    }
//    else
//    {
//        m_ePicSelection = PIC_None;
//        m_gdiPictureThumb.Destroy();//just in case 
//    }


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

    if(m_eSelection == SEL_Skin)
    {
        DrawPicControl(g_gui->gdiMem, rcClient, rcClip);
    }
    else
    {
        ::Draw(g_gui->gdiMem, rcClip, m_gdiBackground, rcClip.left, rcClip.top);

        m_oMenu.OnDraw(g_gui->gdiMem, rcClient, rcClip);
    	
        TCHAR szCancel[STRING_NORMAL];
        m_oStr->StringCopy(szCancel, ID(IDS_MENU_Cancel), STRING_NORMAL, m_hInst);

        if(g_gui->wndMenu.IsWindowUp(FALSE))
            g_gui->DrawBottomBar(rcClient, NULL, szCancel);
        else
        {
            TCHAR szTemp[STRING_NORMAL];
            m_oStr->StringCopy(szTemp, ID(IDS_MENU_Save), STRING_NORMAL, m_hInst);
            g_gui->DrawBottomBar(rcClient, szTemp, szCancel);
        }
    }   
        
	g_gui->DrawScreen(rcClient, hDC);

	return TRUE;
}

void CDlgOptions::DrawPicControl(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    //if(!IsRectInRect(rcClip, m_rcPicSkin))
    //    return;

    if(m_gdiPicBackground.GetWidth() != WIDTH(rcClient) || m_gdiPicBackground.GetHeight() != HEIGHT(rcClient))
    {
        m_gdiPicBackground.Create(gdi, rcClient, FALSE, TRUE);

        ::Draw(m_gdiPicBackground, rcClip, m_gdiBackground, rcClip.left, rcClip.top);
        m_oMenu.OnDraw(m_gdiPicBackground, rcClient, rcClip);

        //grey it out
        AlphaFillRect(m_gdiPicBackground, rcClient, 0, 150);

        TCHAR szTemp[STRING_NORMAL];
        m_oStr->StringCopy(szTemp, ID(IDS_MENU_Cancel), STRING_NORMAL, m_hInst);

        g_gui->DrawBottomBar(m_gdiPicBackground, rcClient, NULL, szTemp);

        if(g_gui->wndMenu.GetBackgroundSlice().GetWidth() != WIDTH(m_rcPicSkin) || g_gui->wndMenu.GetBackgroundSlice().GetHeight() != HEIGHT(m_rcPicSkin))
            g_gui->wndMenu.GetBackgroundSlice().SetSize(WIDTH(m_rcPicSkin), HEIGHT(m_rcPicSkin));

        g_gui->wndMenu.GetBackgroundSlice().DrawImage(m_gdiPicBackground, m_rcPicSkin.left, m_rcPicSkin.top);
    }

    BitBlt(gdi,
           rcClip.left, rcClip.top,
           WIDTH(rcClip), HEIGHT(rcClip),
           m_gdiPicBackground,
           rcClip.left, rcClip.top,
           SRCCOPY);

   /* if(rcClip.left < (IsVGA()?4:2))
        rcClip.left = (IsVGA()?4:2);
    if(rcClip.right > GetSystemMetrics(SM_CXSCREEN) - (IsVGA()?4:2))
        rcClip.right = GetSystemMetrics(SM_CXSCREEN) - (IsVGA()?4:2);*/

    m_oPicSkin.OnDraw(g_gui->gdiMem, rcClient, rcClip);
}


BOOL CDlgOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int iReturn = 0;
	switch(LOWORD(wParam))
	{
    case IDMENU_Left:
	case IDMENU_Save:
        OnMenuLeft();		
        break;
    case IDMENU_Right:
	case IDMENU_Cancel:
        OnMenuRight();
		break;
    case IDMENU_Select:
    case IDOK:
        if(m_eSelection == SEL_Skin)
            PostMessage(m_hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_SelectSkin,0), 0);
        else
            LaunchPopupMenu();
        break;
    case IDMENU_SelectImage:
        OnHandleImageSelect();
        break;
    case IDMENU_SelectMenu:
        SetMenuSelection();
        break;
    case IDMENU_SelectSkin:
        m_sOptions.eSkin    = (EnumSudokuSkins)m_oPicSkin.GetSelectedItemIndex();
        m_eSelection = SEL_None;
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
	}

	return FALSE;
}

void CDlgOptions::OnMenuLeft()
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return;
    else if(m_eSelection == SEL_Skin)
    {
        m_eSelection = SEL_None;
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
    else
    {
        SaveSettings();
        SafeCloseWindow(IDOK);
    }
}

void CDlgOptions::OnMenuRight()
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return;
    else if(m_eSelection == SEL_Skin)
    {
        m_eSelection = SEL_None;
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
    else
    {
        SafeCloseWindow(IDCANCEL);
    }
}

BOOL CDlgOptions::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(m_eSelection)
    {
    case SEL_Skin:
        return m_oPicSkin.OnTimer(wParam, lParam);
        break;
    default:
        return m_oMenu.OnTimer(wParam, lParam);
    }
}

BOOL CDlgOptions::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(m_eSelection)
    {
    case SEL_Skin:
        return m_oPicSkin.OnKeyDown(wParam, lParam);
        break;
    default:
        return m_oMenu.OnKeyDown(wParam, lParam);
    }
}

BOOL CDlgOptions::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_eSelection == SEL_None && m_oMenu.OnLButtonDown(pt))
    {}
    else if(m_eSelection == SEL_Skin)
    {
        m_oPicSkin.OnLButtonDown(pt);
        return TRUE;
    }
	return g_gui->OnLButtonDown(hWnd, pt);
}

BOOL CDlgOptions::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    m_ptLastLButtonUp = pt;
    if(m_eSelection == SEL_None && m_oMenu.OnLButtonUp(pt))
    {}
    else if(m_eSelection == SEL_Skin)
    {
        if(m_oPicSkin.OnLButtonUp(pt) == FALSE)
        {
            m_eSelection = SEL_None;
            InvalidateRect(m_hWnd, NULL, FALSE);
            return TRUE;
        }
    }
   // else if(m_eSelection == SEL_Skin && m_oPicSkin.OnLButtonUp(pt))
   // {}
    
	return g_gui->OnLButtonUp(hWnd, pt);
}

BOOL CDlgOptions::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_eSelection == SEL_None && m_oMenu.OnMouseMove(pt))
    {}
    else if(m_eSelection == SEL_Skin && m_oPicSkin.OnMouseMove(pt))
    {}

    return TRUE;
}

BOOL CDlgOptions::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return UNHANDLED;
}

BOOL CDlgOptions::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return g_gui->OnKeyUp(hWnd, wParam, lParam);
}

BOOL CDlgOptions::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    switch(uiMessage)
    {
    case WM_ImageLoaded:
        if(m_eSelection == SEL_Skin)
            InvalidateRect(m_hWnd, &m_rcPicSkin, FALSE);
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
    RECT rcText;

    TCHAR szTitle[STRING_MAX] = _T("");
    TCHAR szInfo[STRING_MAX]  = _T("");

    GetItemTitle(*eOption, szTitle);

    TCHAR szYes[STRING_NORMAL];
    m_oStr->StringCopy(szYes, ID(IDS_MENU_Yes), STRING_NORMAL, m_hInst);

    TCHAR szNo[STRING_NORMAL];
    m_oStr->StringCopy(szNo, ID(IDS_MENU_No), STRING_NORMAL, m_hInst);

    switch(*eOption)
    {
    case OPTION_ShowBlankCells:
        m_oStr->StringCopy(szInfo, m_sOptions.bDrawBlank?szYes:szNo);
        break;
    case OPTION_ShowHighlight:
        m_oStr->StringCopy(szInfo, m_sOptions.bShowHighlights?szYes:szNo);
        break;
    case OPTION_HintInidicators:
        m_oStr->StringCopy(szInfo, m_sOptions.bShowHints?szYes:szNo);
        break;
    case OPTION_PlaySounds:
        m_oStr->StringCopy(szInfo, m_sOptions.bPlaySounds?szYes:szNo);
        break;
    case OPTION_AutoRemovePencil:
        m_oStr->StringCopy(szInfo, m_sOptions.bAutoRemovePencil?szYes:szNo);
        break;
    case OPTION_Skin:
        GetSkinText(m_sOptions.eSkin, szInfo);
        break;
    case OPTION_Background:
        GetBackgroundText(m_sOptions.eBackground, szInfo);
        break;
    case OPTION_ShowErrors:
        m_oStr->StringCopy(szInfo, m_sOptions.bShowErrors?szYes:szNo);
        break;
    case OPTION_ShowComplete:
        m_oStr->StringCopy(szInfo, m_sOptions.bShowComplete?szYes:szNo);
        break;
    case OPTION_Image:
        DrawImageControls(gdi, bIsHighlighted, rc);
        return TRUE;
        break;
    case OPTION_Language:
        {
        DWORD dwLanguage = g_cLocale.GetCurrentLanguage();
        g_cLocale.GetLanguageText(szInfo, dwLanguage);
        break;
        }
    }

    // draw the down arrow
    Draw(gdi,
        rc.right - m_oMenu.GetImageArrayGDI().GetHeight(), rc.top + (HEIGHT(rc)-m_oMenu.GetImageArrayGDI().GetHeight())/2,
        m_oMenu.GetImageArrayGDI().GetHeight(),m_oMenu.GetImageArrayGDI().GetHeight(),
        m_oMenu.GetImageArrayGDI(),
        (bIsHighlighted?2:3)*m_oMenu.GetImageArrayGDI().GetHeight(), 0);

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

void CDlgOptions::DrawImageControls(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc)
{
    //draw the 3 icons 
    //camera, image and/or custom background colors

    int i3rdOfRect = WIDTH(rc)/3;
    int iWidthImage = m_gdiIcons.GetWidth()/2;

    POINT ptLocation;
    POINT ptIndicator;

    RECT  rcTemp = rc;

    COLORREF crTxt = 0xFFFFFF;
    int iAlpha = 255;

    CIssGDIEx gdiAlpha;

    if(m_sOptions.eSkin < SSKIN_Custom0)
    {
        crTxt = 0x666666;
        iAlpha = 76;
     //   gdiAlpha.Create(m_gdiIcons.GetDC(), iWidthImage, m_imgButton.GetHeight());
     //   RECT rc;
     //   SetRect(&rc, 0, 0, iWidthImage, m_imgButton.GetHeight());
     //   FillRect(gdiAlpha.GetDC(), rc, 0);
    }

    ptLocation.x  = (i3rdOfRect -iWidthImage)/2;
    ptLocation.y  = rc.top + INDENT;

    //camera
    ptIndicator.x = ptLocation.x + m_imgButton.GetWidth() - m_gdiArray.GetWidth()/2;
    ptIndicator.y = ptLocation.y + m_imgButton.GetHeight() - m_gdiArray.GetHeight();
    if(m_ePicSelection == PIC_Camera)
    {
        ::Draw(gdi, ptLocation.x, ptLocation.y,
            m_gdiPictureThumb.GetWidth(), m_gdiPictureThumb.GetHeight(), 
            m_gdiPictureThumb, 0, 0, ALPHA_Normal, iAlpha);

        if(m_sOptions.eSkin >= SSKIN_Custom0)
        {
            ::Draw(gdi, ptIndicator.x, ptIndicator.y, m_gdiArray.GetWidth()/2, m_gdiArray.GetHeight(),
                m_gdiArray, m_gdiArray.GetWidth()/2);
        }
        else
        {

        }
    }
    else
    {
        ::Draw(gdi, ptLocation.x, ptLocation.y,
            iWidthImage, m_gdiIcons.GetHeight(), 
            m_gdiIcons, 0, 0, ALPHA_Normal, iAlpha);

        if(m_sOptions.eSkin >= SSKIN_Custom0)
        {
            ::Draw(gdi, ptIndicator.x, ptIndicator.y, m_gdiArray.GetWidth()/2, m_gdiArray.GetHeight(),
                m_gdiArray);
        }
        else
        {   

        }
        
    }

    TCHAR szCamera[STRING_NORMAL];
    m_oStr->StringCopy(szCamera, ID(IDS_MENU_Camera), STRING_NORMAL, m_hInst);

    rcTemp.right = rcTemp.left + i3rdOfRect;
    ::DrawText(gdi.GetDC(), szCamera, rcTemp, DT_BOTTOM | DT_CENTER, m_hFontBtnText, crTxt);
    
     ptLocation.x += i3rdOfRect;
    ptIndicator.x += i3rdOfRect;
    //picture
    if(m_ePicSelection == PIC_Picture)
    {
        ::Draw(gdi, ptLocation.x, ptLocation.y,
            m_gdiPictureThumb.GetWidth(), m_gdiPictureThumb.GetHeight(), 
            m_gdiPictureThumb, 0, 0,  ALPHA_Normal, iAlpha);

        if(m_sOptions.eSkin >= SSKIN_Custom0)
        {
            ::Draw(gdi, ptIndicator.x, ptIndicator.y, m_gdiArray.GetWidth()/2, m_gdiArray.GetHeight(),
                m_gdiArray, m_gdiArray.GetWidth()/2);
        }
        else
        {
        }
        
    }
    else
    {
        ::Draw(gdi, ptLocation.x, ptLocation.y,
            iWidthImage, m_gdiIcons.GetHeight(), 
            m_gdiIcons, iWidthImage, 0, ALPHA_Normal, iAlpha);

        if(m_sOptions.eSkin >= SSKIN_Custom0)
        {
            ::Draw(gdi, ptIndicator.x, ptIndicator.y, m_gdiArray.GetWidth()/2, m_gdiArray.GetHeight(),
                m_gdiArray);
        }
        else
        {

        }
    }
    TCHAR szImage[STRING_NORMAL];
    m_oStr->StringCopy(szImage, ID(IDS_MENU_Image), STRING_NORMAL, m_hInst);

    rcTemp.left += i3rdOfRect;
    rcTemp.right = rcTemp.left + i3rdOfRect;
    ::DrawText(gdi.GetDC(), szImage, rcTemp, DT_BOTTOM | DT_CENTER, m_hFontBtnText, crTxt);

    //custom
    ptLocation.x += i3rdOfRect;
    ptIndicator.x += i3rdOfRect;
    if(m_ePicSelection == PIC_Given)
    {
        ::Draw(gdi, ptLocation.x, ptLocation.y,
            m_gdiPictureThumb.GetWidth(), m_gdiPictureThumb.GetHeight(), 
            m_gdiPictureThumb, 0, 0, ALPHA_Normal, iAlpha);

        if(m_sOptions.eSkin >= SSKIN_Custom0)
        {
            ::Draw(gdi, ptIndicator.x, ptIndicator.y, m_gdiArray.GetWidth()/2, m_gdiArray.GetHeight(),
                m_gdiArray, m_gdiArray.GetWidth()/2);
        }
        else
        {

        }
        
    }
    else
    {
        ::Draw(gdi, ptLocation.x, ptLocation.y,
            m_imgButton.GetWidth(), m_imgButton.GetHeight(), 
            m_imgButton.GetImage(), 0, 0, ALPHA_Normal, iAlpha);

        if(m_sOptions.eSkin >= SSKIN_Custom0)
        {
            ::Draw(gdi, ptIndicator.x, ptIndicator.y, m_gdiArray.GetWidth()/2, m_gdiArray.GetHeight(),
            m_gdiArray);
        }
        else
        {

        }
    }

    rcTemp.left += i3rdOfRect;
    rcTemp.right = rcTemp.left + i3rdOfRect;
    if(m_ePicSelection == PIC_Given)
    {
        TCHAR szTemp[STRING_LARGE];
        GetBackgroundText(m_sOptions.eBackground, szTemp);
        ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_BOTTOM | DT_CENTER, m_hFontBtnText, crTxt);
    }
    else
    {
        TCHAR szTemp[STRING_NORMAL];
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Custom), STRING_NORMAL, m_hInst);
        ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_BOTTOM | DT_CENTER, m_hFontBtnText, crTxt);
    }
}

void CDlgOptions::GetSkinText(EnumSudokuSkins eSkin, TCHAR* szInfo)
{
    TCHAR szTemp[STRING_NORMAL];

    switch(eSkin)
    {
    default://blue
    case SSKIN_Glass:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Glass), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
    	break;
    case SSKIN_Wood:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Wood), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
    	break;
    case SSKIN_Notepad:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Notepad), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case SSKIN_Grey:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Grey), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case SSKIN_Custom0:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Cust1), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case SSKIN_Custom1:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Cust2), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case SSKIN_Custom2:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Cust3), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case SSKIN_Custom3:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Cust4), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case SSKIN_Custom4:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Cust5), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case SSKIN_Custom5:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Cust6), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    }
}

void CDlgOptions::GetBackgroundText(EnumBackground eBack, TCHAR* szInfo)
{
    TCHAR szTemp[STRING_NORMAL];

    switch(eBack)
    {
    case BACKGROUND_Default:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_DefWhite), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_Custom:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_CustImage), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_White:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_White), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_Black:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Black), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_Grey:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Grey), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_Green:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Green), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_LPink:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_LtPink), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_Orange:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Orange), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_Blue:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Blue), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_DBlue:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_DkBlue), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_DGreen:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_DkGreen), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_Pink:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Pink), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_Purple:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Purple), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_PurpleWhite:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_PurpleWhite), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_BlackWhite:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_BlackWhite), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_GreenWhite:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_GreenWhite), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_BlueWhite:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_BlueWhite), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_DBlueWhite:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_DkBlueWhite), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_PinkWhite:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_PinkWhite), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_OrangeWhite:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_OrangeWhite), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_GreyBlack:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_GreyBlack), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_RedBlack:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_RedBlack), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_PurpleBlack:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_PurpleBlack), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_BlueBlack:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_BlueBlack), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_GreenBlack:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_GreenBlack), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_DBlueBlue:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_DkBlueBlue), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_BlueGreen:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_BlueGreen), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
        break;
    case BACKGROUND_DGReenGreen:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_DkGreenGreen), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szInfo, szTemp);
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
    case OPTION_ShowBlankCells:
        m_oStr->StringCopy(szTitle, ID(IDS_PREF_DrawCells), STRING_MAX, m_hInst);
        break;
    case OPTION_ShowHighlight:
        m_oStr->StringCopy(szTitle, ID(IDS_PREF_ShowHighlights), STRING_MAX, m_hInst);
        break;
    case OPTION_PlaySounds:
        m_oStr->StringCopy(szTitle, ID(IDS_PREF_PlaySounds), STRING_MAX, m_hInst);
        break;
     case OPTION_Skin:
        m_oStr->StringCopy(szTitle, ID(IDS_PREF_Skin), STRING_MAX, m_hInst);
        break;
     case OPTION_Background:
         m_oStr->StringCopy(szTitle, ID(IDS_PREF_Background), STRING_MAX, m_hInst);
         break;
     case OPTION_ShowComplete:
         m_oStr->StringCopy(szTitle, ID(IDS_PREF_LockCompleted), STRING_MAX, m_hInst);
         break;
     case OPTION_ShowErrors:
         m_oStr->StringCopy(szTitle, ID(IDS_PREF_ShowErrors), STRING_MAX, m_hInst);
         break;
     case OPTION_HintInidicators:
         m_oStr->StringCopy(szTitle, ID(IDS_PREF_ShowHints), STRING_MAX, m_hInst);
         break;
     case OPTION_AutoRemovePencil:
         m_oStr->StringCopy(szTitle, ID(IDS_PREF_AutoClear), STRING_MAX, m_hInst);
         break;
    }
}


void CDlgOptions::PopulateOptions()
{
    m_oMenu.ResetContent();
    int iIndex = 0;

//    m_oMenu.AddCategory(_T("General"));

    EnumOption* eOption = new EnumOption;
    *eOption = OPTION_PlaySounds;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    iIndex++;

    eOption = new EnumOption;
    *eOption = OPTION_Language;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    iIndex++;

//    eOption = new EnumOption;
//    *eOption = OPTION_Background;
//    m_oMenu.AddItem(eOption, IDMENU_Select);
//    iIndex++;

    eOption = new EnumOption;
    *eOption = OPTION_ShowErrors;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    iIndex++;

    eOption = new EnumOption;
    *eOption = OPTION_ShowComplete;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    iIndex++;

    eOption = new EnumOption;
    *eOption = OPTION_HintInidicators;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    iIndex++;

    eOption = new EnumOption;
    *eOption = OPTION_ShowHighlight;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    iIndex++;

    eOption = new EnumOption;
    *eOption = OPTION_AutoRemovePencil;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    iIndex++;

    eOption = new EnumOption;
    *eOption = OPTION_ShowBlankCells;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    iIndex++;

    eOption = new EnumOption;
    *eOption = OPTION_Skin;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    iIndex++;



    //4
    eOption = new EnumOption;
    *eOption = OPTION_Image;
    m_oMenu.AddItem(eOption, IDMENU_SelectImage);

	SIZE sz;
	HDC dc = GetDC(m_hWnd);
	HFONT hOld = (HFONT)SelectObject(dc, m_hFontLabel);
	GetTextExtentPoint(dc, _T("APp"), 3, &sz);
	SelectObject(dc, hOld);
	ReleaseDC(m_hWnd, dc);

	m_oMenu.SetItemHeights(2*sz.cy + INDENT, 2*sz.cy + INDENT);

    m_oMenu.SetItemHeight(iIndex, m_imgButton.GetHeight() + BUTTON_Height, m_imgButton.GetHeight() + BUTTON_Height);

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

    TCHAR szYes[STRING_NORMAL];
    m_oStr->StringCopy(szYes, ID(IDS_MENU_Yes), STRING_NORMAL, m_hInst);

    TCHAR szNo[STRING_NORMAL];
    m_oStr->StringCopy(szNo, ID(IDS_MENU_No), STRING_NORMAL, m_hInst);

    switch(*eOption)
    {
    case OPTION_Language:
        {
            TCHAR szText[STRING_MAX];
            DWORD dwCurrent = g_cLocale.GetCurrentLanguage();
            g_cLocale.GetLanguageText(szText, LANG_ENGLISH);
            g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_ENGLISH?FLAG_Radio:NULL), LANG_ENGLISH);
            g_cLocale.GetLanguageText(szText, LANG_SPANISH);
            g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_SPANISH?FLAG_Radio:NULL), LANG_SPANISH);
            g_cLocale.GetLanguageText(szText, LANG_FRENCH);
            g_gui->wndMenu.AddItem(szText,  IDMENU_SelectMenu, (dwCurrent == LANG_FRENCH?FLAG_Radio:NULL), LANG_FRENCH);
            g_cLocale.GetLanguageText(szText, LANG_GERMAN);
            g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_GERMAN?FLAG_Radio:NULL), LANG_GERMAN);
            g_cLocale.GetLanguageText(szText, LANG_DUTCH);
            g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_DUTCH?FLAG_Radio:NULL), LANG_DUTCH);
            //g_cLocale.GetLanguageText(szText, LANG_HINDI);
            //m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_HINDI?FLAG_Radio:NULL), LANG_HINDI);
            g_cLocale.GetLanguageText(szText, LANG_ITALIAN);
            g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_ITALIAN?FLAG_Radio:NULL), LANG_ITALIAN);
            g_cLocale.GetLanguageText(szText, LANG_JAPANESE);
            g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_JAPANESE?FLAG_Radio:NULL), LANG_JAPANESE);
            //g_cLocale.GetLanguageText(szText, LANG_KOREAN);
            //g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_KOREAN?FLAG_Radio:NULL), LANG_KOREAN);   
            g_cLocale.GetLanguageText(szText, LANG_PORTUGUESE);
            g_gui->wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_PORTUGUESE?FLAG_Radio:NULL), LANG_PORTUGUESE);            

            //g_cLocale.GetLanguageText(szText, LANG_CHINESE);
            //m_wndMenu.AddItem(szText, IDMENU_SelectMenu, (dwCurrent == LANG_CHINESE?FLAG_Radio:NULL), LANG_CHINESE);
            break;
        }
    case OPTION_ShowBlankCells:
        g_gui->wndMenu.AddItem(szYes, IDMENU_SelectMenu, (m_sOptions.bDrawBlank?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(szNo, IDMENU_SelectMenu, (!m_sOptions.bDrawBlank?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_ShowHighlight:
        g_gui->wndMenu.AddItem(szYes, IDMENU_SelectMenu, (m_sOptions.bShowHighlights?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(szNo, IDMENU_SelectMenu, (!m_sOptions.bShowHighlights?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_PlaySounds:
        g_gui->wndMenu.AddItem(szYes, IDMENU_SelectMenu, (m_sOptions.bPlaySounds?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(szNo, IDMENU_SelectMenu, (!m_sOptions.bPlaySounds?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_AutoRemovePencil:
        g_gui->wndMenu.AddItem(szYes, IDMENU_SelectMenu, (m_sOptions.bAutoRemovePencil?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(szNo, IDMENU_SelectMenu, (!m_sOptions.bAutoRemovePencil?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_ShowComplete:
        g_gui->wndMenu.AddItem(szYes, IDMENU_SelectMenu, (m_sOptions.bShowComplete?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(szNo, IDMENU_SelectMenu, (!m_sOptions.bShowComplete?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_ShowErrors:
        g_gui->wndMenu.AddItem(szYes, IDMENU_SelectMenu, (m_sOptions.bShowErrors?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(szNo, IDMENU_SelectMenu, (!m_sOptions.bShowErrors?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_HintInidicators:
        g_gui->wndMenu.AddItem(szYes, IDMENU_SelectMenu, (m_sOptions.bShowHints?FLAG_Radio:NULL), TRUE);
        g_gui->wndMenu.AddItem(szNo, IDMENU_SelectMenu, (!m_sOptions.bShowHints?FLAG_Radio:NULL), FALSE);
        break;
    case OPTION_Skin:
        PopulatePicControl();
        m_eSelection = SEL_Skin;
        InvalidateRect(m_hWnd, NULL, FALSE);
        return;
        /*for(i=0; i < SSKIN_Count; i++)
        {
            EnumSudokuSkins eSkins = (EnumSudokuSkins)i;
            GetSkinText(eSkins, szInfo);
            g_gui->wndMenu.AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.eSkin == eSkins?FLAG_Radio:NULL), eSkins);
        }*/
        break;
    case OPTION_Background:
        for(i=2; i < BACKGROUND_Count; i++)
        {
            EnumBackground eBack = (EnumBackground)i;
            GetBackgroundText(eBack, szInfo);
            g_gui->wndMenu.AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.eBackground == eBack?FLAG_Radio:NULL), eBack);
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

void CDlgOptions::OnHandleImageSelect()
{
    if(IsCustomizable() == FALSE)
        return;//this isn't for us

    int i3rdOfRect = GetSystemMetrics(SM_CXSCREEN)/3;

    if(m_ptLastLButtonUp.x > 0 && m_ptLastLButtonUp.x <= i3rdOfRect)
    {    // camera
        OnHandleTakePicture();
    }
    else if(m_ptLastLButtonUp.x > i3rdOfRect && m_ptLastLButtonUp.x <= 2*i3rdOfRect)
    {    // picture
        OnHandleAddPicture();
    }
    else
    {   // custom
        OnHandleCustomBackground();
    }
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
    case OPTION_ShowBlankCells:
        m_sOptions.bDrawBlank = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_ShowHighlight:
        m_sOptions.bShowHighlights = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_PlaySounds:
        m_sOptions.bPlaySounds = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_AutoRemovePencil:
        m_sOptions.bAutoRemovePencil = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_Skin:
        m_sOptions.eSkin = (EnumSudokuSkins)sMenuItem->lParam;
//        g_gui->LoadSkin(m_sOptions.eSkin, m_sOptions.eBackground);
        break;
    case OPTION_ShowErrors:
        m_sOptions.bShowErrors = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_ShowComplete:
        m_sOptions.bShowComplete = (BOOL)sMenuItem->lParam;
        break;
    case OPTION_Image://so .. this works because we only pop the menu when we're selecting a custom menu on the 
        //image selection entry ... really I think this is totally fine
    case OPTION_Background:
        m_sOptions.eBackground = (EnumBackground)sMenuItem->lParam;
        m_oStr->Empty(m_sOptions.szPath);
        //g_gui->LoadSkin(m_sOptions.eSkin, m_sOptions.eBackground);    
        SetCustomPreview();
        break;
    case OPTION_HintInidicators:
        m_sOptions.bShowHints = (BOOL)sMenuItem->lParam;
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

void CDlgOptions::LoadImages()
{
    if(m_gdiBackground.GetDC() == NULL)
    {
        m_gdiBackground.Create(g_gui->gdiTitle.GetDC(), 
            g_gui->gdiTitle.GetWidth(),
            g_gui->gdiTitle.GetHeight(),
            TRUE);

        RECT rc;
        rc.left = 0;
        rc.top  = 0;
        rc.right = m_gdiBackground.GetWidth();
        rc.bottom = m_gdiBackground.GetHeight();

        AlphaFillRect(m_gdiBackground, rc, 0, 150);
    }

    if(m_gdiArray.GetDC() == NULL)
        m_gdiArray.LoadImage(IsVGA()?IDB_PNG_OptionsArray_HR:IDB_PNG_OptionsArray, m_hWnd, m_hInst);

    if(m_gdiIcons.GetDC() == NULL)
        m_gdiIcons.LoadImage(IsVGA()?IDB_PNG_OptionsIcons_HR:IDB_PNG_OptionsIcons, m_hWnd, m_hInst);

    if(m_imgButton.IsLoaded() == FALSE)
    {
        int iWidthImage = m_gdiIcons.GetWidth()/2;
        m_imgButton.Initialize(m_hWnd, m_hInst, IsVGA()?IDB_PNG_OptionsOutLine_HR:IDB_PNG_OptionsOutLine);
        m_imgButton.SetSize(iWidthImage, m_gdiIcons.GetHeight());
    }

    if(m_imgOutline.IsLoaded() == FALSE)
    {
        int iWidthImage = m_gdiIcons.GetWidth()/2;
        m_imgOutline.Initialize(m_hWnd, m_hInst, IsVGA()?IDB_PNG_OptionsAlpha_HR:IDB_PNG_OptionsAlpha);
        m_imgOutline.SetSize(iWidthImage, m_gdiIcons.GetHeight());
    }
}

void CDlgOptions::OnHandleAddPicture()
{
    if(m_oStr->GetLength(m_szDeleteFile))
        DeleteFile(m_szDeleteFile);
    m_gdiPicCtrlBG.Destroy();
    ResetPicItems();
    m_gdiPicBackground.Destroy();
    m_gdiPictureThumb.Destroy();
    m_oStr->Empty(m_szPicFileName);
    m_ePicSelection = PIC_None;

    OPENFILENAMEEX  ofnex   = {0};
    TCHAR           szPictureFile[MAX_PATH];
    BOOL            bResult = FALSE;

    // Setup the Picture picker structure
    ofnex.lStructSize   = sizeof(ofnex);
    ofnex.ExFlags       = OFN_EXFLAG_THUMBNAILVIEW;    
    ofnex.lpstrFile     = szPictureFile;
    ofnex.nMaxFile      = MAX_PATH;

    // Call the Picture picker UI. Lets the user select a pic.

    AdjustWindow(m_hWnd, FALSE);

    bResult = GetOpenFileNameEx(&ofnex);
    if(bResult)
    {
        if(AddPicture(ofnex.lpstrFile))
            m_ePicSelection = PIC_Picture;
    }

    AdjustWindow(m_hWnd, TRUE);
    SetFocus(m_hWnd);

    m_sOptions.eBackground = BACKGROUND_Custom;

  /*  HDC dc = GetDC(m_hWnd);
    BitBlt(dc, 0, 0, g_gui->gdiBackground.GetWidth(), g_gui->gdiBackground.GetHeight(), 
        g_gui->gdiBackground.GetDC(), 0, 0, SRCCOPY);

    ReleaseDC(m_hWnd, dc);*/
}

void CDlgOptions::OnHandleTakePicture()
{
    if(m_oStr->GetLength(m_szDeleteFile))
        DeleteFile(m_szDeleteFile);
    m_gdiPicCtrlBG.Destroy();
    ResetPicItems();
    m_gdiPicBackground.Destroy();
    m_gdiPictureThumb.Destroy();
    m_oStr->Empty(m_szPicFileName);
    m_ePicSelection = PIC_None;

    TCHAR szTemp[STRING_NORMAL];
    m_oStr->StringCopy(szTemp, ID(IDS_MENU_Picture), STRING_NORMAL, m_hInst);

    SHCAMERACAPTURE shcc;
    ZeroMemory(&shcc, sizeof(shcc));
    shcc.cbSize             = sizeof(shcc);
    shcc.hwndOwner          = m_hWnd;
    shcc.pszInitialDir      = NULL;
    shcc.pszDefaultFileName = NULL;
    shcc.pszTitle           = szTemp; //BUGBUG m_oStr->GetText(ID(IDS_MSG_UploadPic));
    shcc.StillQuality       = CAMERACAPTURE_STILLQUALITY_HIGH;
    shcc.VideoTypes         = CAMERACAPTURE_VIDEOTYPE_STANDARD ;
    shcc.nResolutionWidth   = 0;
    shcc.nResolutionHeight  = 0;
    shcc.nVideoTimeLimit    = 0;
    shcc.Mode               = CAMERACAPTURE_MODE_STILL;

    // Call SHCameraCapture() function
    SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
    HRESULT hReturn = SHCameraCapture(&shcc);
    if(hReturn == S_OK && !m_oStr->IsEmpty(shcc.szFile))
    {
        m_oStr->StringCopy(m_szDeleteFile, shcc.szFile);
        if(AddPicture(shcc.szFile))
            m_ePicSelection = PIC_Camera;
    }

    SetFocus(m_hWnd);

    m_sOptions.eBackground = BACKGROUND_Custom;
}

void CDlgOptions::OnHandleCustomBackground()
{
    g_gui->wndMenu.ResetContent();
    m_gdiPicCtrlBG.Destroy();
    m_gdiPicBackground.Destroy();
    ResetPicItems();

    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    TCHAR szInfo[STRING_MAX];
    
    for(int i=0; i < BACKGROUND_Count; i++)
    {
        EnumBackground eBack = (EnumBackground)i;
        GetBackgroundText(eBack, szInfo);
        g_gui->wndMenu.AddItem(szInfo, IDMENU_SelectMenu, (m_sOptions.eBackground == eBack?FLAG_Radio:NULL), eBack);
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

void CDlgOptions::SetCustomPreview()
{
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = m_gdiIcons.GetWidth()/2;
    rc.bottom = m_gdiIcons.GetHeight();

    m_gdiPictureThumb.Destroy();
    m_gdiPictureThumb.Create(m_gdiBackground.GetDC(), rc);

    //GradientFillRect(m_gdiPictureThumb, rc, g_gui->GetBackgroundColor(FALSE), g_gui->GetBackgroundColor(TRUE), FALSE);
    GradientFillRect(m_gdiPictureThumb, rc, g_gui->GetBackgroundColor(FALSE, m_sOptions.eBackground), g_gui->GetBackgroundColor(TRUE, m_sOptions.eBackground), FALSE);
    m_gdiPictureThumb.SetAlphaMask(m_imgOutline.GetImage());
    Draw(m_gdiPictureThumb, 0, 0, m_imgButton.GetWidth(), m_imgButton.GetHeight(), m_imgButton.GetImage(), 0, 0, ALPHA_Normal);

    m_ePicSelection = PIC_Given;
}

BOOL CDlgOptions::AddPicture(TCHAR* szFileName)
{
    if(m_oStr->IsEmpty(szFileName))
        return FALSE;

    if(!IsFileExists(szFileName))
        return FALSE;

    m_oStr->StringCopy(m_szPicFileName, szFileName);


    m_gdiPictureThumb.Destroy();
    
    if(S_OK != m_gdiPictureThumb.LoadImage(szFileName, m_hWnd, TRUE, m_gdiIcons.GetWidth()/2, m_gdiIcons.GetHeight(), ASPECT_Cropped))
    {
        // BUGBUG error message
        MessageBeep(MB_ICONHAND);
        return FALSE;
    }


    m_gdiPictureThumb.SetAlphaMask(m_imgOutline.GetImage());
    Draw(m_gdiPictureThumb, 0, 0, m_imgButton.GetWidth(), m_imgButton.GetHeight(), m_imgButton.GetImage(), 0, 0, ALPHA_Normal);

    m_oStr->StringCopy(m_sOptions.szPath, szFileName);
    return TRUE;

/*    CIssGDIEx gdiTemp;
    if(S_OK != gdiTemp.LoadImage(szFileName, m_hWnd, TRUE))
    {
        // BUGBUG error message
        MessageBeep(MB_ICONHAND);
        return FALSE;
    }

    SIZE sz;

    sz.cy = m_gdiIcons.GetHeight();
    sz.cx = m_gdiIcons.GetWidth()/2;

    ScaleImage(gdiTemp, m_gdiPictureThumb, sz, TRUE, TRANSPARENT_COLOR);
    m_gdiPictureThumb.SetAlphaMask(m_imgOutline.GetImage());
    Draw(m_gdiPictureThumb, 0, 0, m_imgButton.GetWidth(), m_imgButton.GetHeight(), m_imgButton.GetImage(), 0, 0, ALPHA_Normal);


    SetFocus(m_hWnd);

    m_oStr->StringCopy(m_sOptions.szPath, szFileName);
    //and tell the gui
    //g_gui->SetImageBackground(szFileName);

    return TRUE;*/
}

BOOL CDlgOptions::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case _T('c'):
    case _T('C'):
        OnHandleTakePicture();
    	break;
    case _T('p'):
    case _T('P'):
        OnHandleAddPicture();
        break;
    default:
        return UNHANDLED;
        break;
    }

    return TRUE;
}


BOOL CDlgOptions::IsCustomizable()
{
    switch(m_sOptions.eSkin)
    {
    case SSKIN_Glass:
    case SSKIN_Notepad:
    case SSKIN_Wood:
    case SSKIN_Grey:
        return FALSE;
        break;
    }
    return TRUE;
}

void CDlgOptions::DeleteMyPicListItem(LPVOID lpItem)
{

}

void CDlgOptions::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    //FrameRect(gdi, rcDraw, 0);
    CDlgOptions* oGui = (CDlgOptions*)lpClass;

    if(!sItem->lpItem)
        return;

    oGui->DrawMyListItem(gdi, rcDraw, sItem, bIsHighlighted);
}

void CDlgOptions::DrawTitle(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, LPVOID lpClass)
{
    CDlgOptions* oGui = (CDlgOptions*)lpClass;
    DrawText(gdi, sItem->szTitle, rcDraw, DT_CENTER|DT_BOTTOM|DT_NOPREFIX, oGui->m_hFontLabel, TEXTCOLOR_Normal);
}

void CDlgOptions::DrawDots(CIssGDIEx& gdi, RECT& rcDraw, int iCurrentIndex, int iTotalCount, LPVOID lpClass)
{
    CDlgOptions* oGui = (CDlgOptions*)lpClass;
    oGui->m_oStr->Format(oGui->m_szTemp, _T("%d/%d"), iCurrentIndex, iTotalCount);
    DrawText(gdi, oGui->m_szTemp, rcDraw, DT_CENTER|DT_TOP, oGui->m_hFontLabel, TEXTCOLOR_Normal);
}

void CDlgOptions::DrawMyListItem(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, BOOL bIsHighlighted)
{
    TypePicSkinItem* sMyPic = (TypePicSkinItem*)sItem->lpItem;

    if(sMyPic == NULL || sMyPic->sPic == NULL || sMyPic->sPic->gdiImage == NULL)
        return;

    RECT rc = rcDraw;

    int iXDrawStart = rcDraw.left + (WIDTH(rcDraw) - sMyPic->sPic->gdiImage->GetWidth())/2;
    int iYDrawStart = rcDraw.top + (HEIGHT(rcDraw) - sMyPic->sPic->gdiImage->GetHeight())/2;
    int iXWidth     = sMyPic->sPic->gdiImage->GetWidth();
    int iXDiff      = 0;

    //left side
    if(iXDrawStart < m_rcPicSkin.left + SIZE_PicBorder)
    {
        iXDiff = m_rcPicSkin.left + SIZE_PicBorder - iXDrawStart;
        iXDrawStart += iXDiff;//left edge
        iXWidth -= iXDiff;
    }

    //right side
    if(iXDrawStart + iXWidth > (m_rcPicSkin.right - SIZE_PicBorder))
    {
        int iDiff = iXDrawStart + iXWidth - m_rcPicSkin.right + SIZE_PicBorder;
        iXWidth -= iDiff;
    }

    if(iXWidth > 0 && sMyPic->sPic && sMyPic->sPic->gdiImage)
    {
        BitBlt(gdi,
             iXDrawStart, 
             iYDrawStart,
             iXWidth, 
             sMyPic->sPic->gdiImage->GetHeight(),
             *sMyPic->sPic->gdiImage,
             iXDiff,0,
             SRCCOPY);
    }
  /*  else if(sMyPic->sPic->gdiImage->GetWidth() == 0)
    {
        SetRect(&rc, rcDraw.left + SIZE_PicIndent, rcDraw.top + SIZE_PicIndent, rcDraw.left + SIZE_Item - 2*SIZE_PicIndent, rcDraw.top + SIZE_Item - 2*SIZE_PicIndent);
        FillRect(gdi, rc, RGB(100,100,100));
    }*/

}

void CDlgOptions::PopulatePicItems()
{
    ResetPicItems();

    UINT uiItem = 0;
    for(int i=0; i<(int)SSKIN_Count; i++)
    {
        TypePicSkinItem* sNew = new TypePicSkinItem;
        if(!sNew)
            continue;        

        sNew->sPic = m_oPicLoader.AddPicToQueue(LoadPicItem, this, i, m_hWnd, WM_ImageLoaded);
        m_arrSkins.AddElement(sNew);
    }
}

void CDlgOptions::ResetPicItems()
{
    for(int i=0; i<m_arrSkins.GetSize(); i++)
    {
        TypePicSkinItem* sItem = m_arrSkins[i];
        if(sItem)
            delete sItem;
    }
    m_arrSkins.RemoveAll();
}

void CDlgOptions::PopulatePicControl()
{

    if(m_arrSkins.GetSize() == 0)
        PopulatePicItems();

    m_oPicSkin.ResetContent();

    TCHAR szTitle[STRING_MAX];
    for(int i=0; i<m_arrSkins.GetSize(); i++)
    {
        TypePicSkinItem* sItem = m_arrSkins[i];
        if(!sItem)
            continue;

        GetSkinText((EnumSudokuSkins)i, szTitle);
        m_oPicSkin.AddItem(szTitle, IDMENU_SelectSkin, 0, (LPVOID)sItem);
    }

    m_oPicSkin.SetSelectedItemIndex((int)m_sOptions.eSkin, TRUE);
    m_oPicSkin.ShowSelectedItem(FALSE);
}

void CDlgOptions::SetPicControlBackground(EnumSudokuSkins eSkin)
{
    m_gdiPicCtrlBG.Destroy();

    RECT rc;
    SetRect(&rc, 0, 0, SIZE_Item, SIZE_Item);
    m_gdiPicCtrlBG.Create(g_gui->gdiMem.GetDC(), rc, FALSE, TRUE);

    switch(eSkin)
    {
    case SSKIN_Glass:
        GradientFillRect(m_gdiPicCtrlBG, rc, RGB(12,51,104), 0, FALSE);
        break;
    case SSKIN_Wood:
        {
            CIssGDIEx gdiTemp;
            m_gdiPicCtrlBG.LoadImage(IDB_PNG_BGWood, m_hWnd, m_hInst, m_gdiPicCtrlBG.GetWidth(), m_gdiPicCtrlBG.GetHeight());//always load the lowres for speed
        //    BitBlt(m_gdiPicCtrlBG.GetDC(), 0, 0, m_gdiPicCtrlBG.GetWidth(), m_gdiPicCtrlBG.GetHeight(), gdiTemp.GetDC(), 0, 0, SRCCOPY);
        }
        break;
    case SSKIN_Notepad:
        FillRect(m_gdiPicCtrlBG, rc, 0xFFFFFF);
        break;
    case SSKIN_Grey:
        GradientFillRect(m_gdiPicCtrlBG, rc, RGB(40,44,49), RGB(103,111,122), FALSE);
        break;
    default:
        if(m_oStr->GetLength(m_sOptions.szPath) > 0)
        {
            CIssGDIEx gdiPic;
            if(S_OK == m_gdiPicCtrlBG.LoadImage(m_sOptions.szPath, m_hWnd, TRUE, WIDTH(rc), HEIGHT(rc), ASPECT_Cropped))
                break;
        }
        GradientFillRect(m_gdiPicCtrlBG, rc, g_gui->GetBackgroundColor(FALSE, m_sOptions.eBackground), g_gui->GetBackgroundColor(TRUE, m_sOptions.eBackground), FALSE);
        break;
    }

    rc.right--;
    rc.bottom--;

    FrameRect(m_gdiPicCtrlBG.GetDC(), rc, RGB(100,100,100));
}

HRESULT CDlgOptions::LoadPicItem(CIssGDIEx& gdiDest, TypePLItem* sItem, LPVOID lpClass, LPARAM lpItem)
{
    CDlgOptions* oGui = (CDlgOptions*)lpClass;
    return oGui->LoadMyPicItem(gdiDest, lpItem);
}

HRESULT CDlgOptions::LoadMyPicItem(CIssGDIEx& gdiDest, LPARAM lpItem)
{
    EnterCriticalSection(&m_crLoad);

    EnumSudokuSkins eItem = (EnumSudokuSkins)lpItem;

    UINT uiItem = 0;
    switch(eItem)
    {
    case SSKIN_Glass:
        uiItem = IsVGA()?IDR_PNG_SkinGlassVGA:IDR_PNG_SkinGlass;
        m_gdiPicCtrlBG.Destroy();
        break;
    case SSKIN_Wood:
        uiItem = IsVGA()?IDR_PNG_SkinWoodVGA:IDR_PNG_SkinWood;
        m_gdiPicCtrlBG.Destroy();
        break;
    case SSKIN_Notepad:
        uiItem = IsVGA()?IDR_PNG_SkinNotepadVGA:IDR_PNG_SkinNotepad;
        m_gdiPicCtrlBG.Destroy();
        break;
    case SSKIN_Grey:
        uiItem = IsVGA()?IDR_PNG_SkinGreyVGA:IDR_PNG_SkinGrey;
        m_gdiPicCtrlBG.Destroy();
        break;
    case SSKIN_Custom0:
        uiItem = IsVGA()?IDR_PNG_SkinCustomVGA:IDR_PNG_SkinCustom;
        m_gdiPicCtrlBG.Destroy();
        break;
    case SSKIN_Custom1:
        uiItem = IsVGA()?IDR_PNG_SkinCustom1VGA:IDR_PNG_SkinCustom1;
        break;
    case SSKIN_Custom2:
        uiItem = IsVGA()?IDR_PNG_SkinCustom2VGA:IDR_PNG_SkinCustom2;
        break;
    case SSKIN_Custom3:
        uiItem = IsVGA()?IDR_PNG_SkinCustom3VGA:IDR_PNG_SkinCustom3;
        break;
    case SSKIN_Custom4:
        uiItem = IsVGA()?IDR_PNG_SkinCustom4VGA:IDR_PNG_SkinCustom4;
        break;
    case SSKIN_Custom5:
    default:
        uiItem = IsVGA()?IDR_PNG_SkinCustom5VGA:IDR_PNG_SkinCustom5;
        break;
    }

    if(m_gdiPicCtrlBG.GetDC() == NULL)
        SetPicControlBackground(eItem);
    
    gdiDest.Create(m_gdiPicCtrlBG.GetDC(), SIZE_Item, SIZE_Item, FALSE, TRUE);
    gdiDest.InitAlpha(FALSE);
    Draw(gdiDest,
        0,0,
        SIZE_Item, SIZE_Item,
        m_gdiPicCtrlBG,
        0,0);
    LeaveCriticalSection(&m_crLoad);

    CIssGDIEx gdiTemp;

    gdiTemp.LoadImage(uiItem, m_hWnd, m_hInst, TRUE);
    Draw(gdiDest, 
         SIZE_PicIndent,SIZE_PicIndent,
         SIZE_Item-2*SIZE_PicIndent, SIZE_Item-2*SIZE_PicIndent,
         gdiTemp,
         0,0);

    return S_OK;
}


void CDlgOptions::AdjustWindow(HWND hWnd, BOOL bFullScreen)
{
    DWORD dwState;
    if(bFullScreen)
        dwState = SHFS_HIDETASKBAR|SHFS_HIDESTARTICON|SHFS_HIDESIPBUTTON;
    else
        dwState = SHFS_SHOWTASKBAR|SHFS_SHOWSTARTICON|SHFS_HIDESIPBUTTON;

    ShowWindow(hWnd, SW_SHOW);
    SHFullScreen(hWnd, dwState); 

    if(bFullScreen)
    {
        //get rid of the damn sip too ... just in case 
        HWND hWndSip = FindWindow(_T("MS_SIPBUTTON"), NULL); 
        if(hWndSip) 
            ShowWindow(hWndSip, SW_HIDE);
    }

    RECT rc, rcTaskBar; 

    HWND hWndTaskbar = NULL;

    hWndTaskbar= FindWindow(_T("HHTaskBar"), NULL);

    int iMenu = 0;
    if(hWndTaskbar)
    {
        GetWindowRect(hWndTaskbar,&rcTaskBar);
        iMenu = HEIGHT(rcTaskBar);
    }

    if(iMenu == 0)
    {
#ifdef WIN32_PLATFORM_PSPC 
        switch(GetSystemMetrics(SM_CXICON))
        {
        case 32:
            iMenu = 26;
            break;
        case 44:
            iMenu = 35;
            break;
        case 64:
            iMenu = 52;
            break;
        default:
            iMenu = GetSystemMetrics(SM_CXICON)*26/32;
            break;
        }
#else //smartphone
        //try this for now ....
        iMenu = GetSystemMetrics(SM_CXICON)*27/44;
#endif
    }

    int iYBottom = GetSystemMetrics(SM_CYSCREEN);
    if(bFullScreen == FALSE)
        iYBottom -= iMenu;

    if(bFullScreen)
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), iYBottom); 
    else
        SetRect(&rc, 0, iMenu, GetSystemMetrics(SM_CXSCREEN), iYBottom); 
    MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 

}