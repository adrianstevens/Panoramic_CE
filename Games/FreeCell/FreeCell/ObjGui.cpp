#include "ObjGui.h"
#include "Resource.h"
#include "IssCommon.h"
#include "IssGDIFX.h"

#define TEXT_COLOR 0xDDDDDD

#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Selected      RGB(149,149,149)

CObjGui::CObjGui(void)
:m_hWndParent(NULL)
,m_hInst(NULL)
,eMenuSelect(MENU_None)
,m_eBackground(BGCOLOR_Green)
,m_oStr(CIssString::Instance())
,m_bIsSmartphone(FALSE)
{
    // check if we're using smartphone
    TCHAR szPlatform[STRING_LARGE];
    ZeroMemory(szPlatform, sizeof(TCHAR)*STRING_LARGE);
    if(SystemParametersInfo(SPI_GETPLATFORMTYPE, STRING_LARGE, szPlatform, 0))
    {
        if(0 == m_oStr->Compare(szPlatform, _T("SmartPhone")))
        {
            m_bIsSmartphone = TRUE;
        }
    }

    hFontLarge    = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*14/32, FW_BOLD, TRUE);
}

CObjGui::~CObjGui(void)
{
    Destroy();
    CIssGDIEx::DeleteFont(hFontLarge);
}

void CObjGui::Destroy()
{
    gdiMem.Destroy();
    gdiBottomBar.Destroy();
    gdiBackground.Destroy();
    gdiMenuArray.Destroy();
    imgSelector.Destroy();
    imgGlow.Destroy();
}


void CObjGui::Init(HWND hWndParent, HINSTANCE hInst)
{
    Destroy();
    m_hInst = hInst;
    m_hWndParent = hWndParent;
}


void CObjGui::InitGDI(RECT& rcClient, HDC dc)
{
    if(m_hWndParent == NULL || m_hInst == NULL)
        return;

    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN))
        return;

    if(WIDTH(rcClient) == gdiMem.GetWidth() && HEIGHT(rcClient) == gdiMem.GetHeight())
        return;

    gdiMem.Create(dc, WIDTH(rcClient), HEIGHT(rcClient), FALSE, TRUE);

    InitBottomBar(rcClient);
    InitBackground(rcClient);

    if(!imgSelector.IsLoaded())
        imgSelector.Initialize(m_hWndParent, m_hInst, IDB_PNG_MenuSelector);

    if(!imgGlow.IsLoaded())
        imgGlow.Initialize(m_hWndParent, m_hInst, IsVGA()?IDB_PNG_MenuGlow_HR:IDB_PNG_MenuGlow);

    if(!gdiMenuArray.GetDC())
        gdiMenuArray.LoadImage(IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray, m_hWndParent, m_hInst, TRUE);


    if(wndMenu.GetBackground() == IDR_PNG_MenuBack &&
        wndMenu.GetSelected() == IDB_PNG_MenuSelector &&
        wndMenu.GetImageArray() == (IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray))
        return;

    wndMenu.SetColors(TEXTCOLOR_Normal, TEXTCOLOR_Normal, TEXTCOLOR_Selected);
    wndMenu.SetBackground(IDR_PNG_MenuBack);
    wndMenu.SetSelected(IDB_PNG_MenuSelector);
    wndMenu.SetImageArray(IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray);
    wndMenu.PreloadImages(m_hWndParent, m_hInst);
}

void CObjGui::DrawScreen(RECT& rcClip, HDC dc)
{
    BitBlt(dc,
        rcClip.left, rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        gdiMem.GetDC(),
        rcClip.left, rcClip.top,
        SRCCOPY);
}

void CObjGui::DrawBottomBar(RECT& rcClip, TCHAR* szLeftMenu, TCHAR* szRightMenu, BOOL bLeftMenuGrayed /* = FALSE */, BOOL bRightMenuGrayed /* = FALSE */)
{
    DrawBottomBar(gdiMem, rcClip, szLeftMenu, szRightMenu, bLeftMenuGrayed, bRightMenuGrayed);
}

void CObjGui::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClip, TCHAR* szLeftMenu, TCHAR* szRightMenu, BOOL bLeftMenuGrayed, BOOL bRightMenuGrayed)
{
    if(!IsRectInRect(rcClip, rcBottomBar))
        return;

    Draw(gdi, rcBottomBar, gdiBottomBar, 0, 0);

    if(imgGlow.GetWidth() != WIDTH(rcBottomBar)/2 || imgGlow.GetHeight() != HEIGHT(rcBottomBar))
        imgGlow.SetSize(WIDTH(rcBottomBar)/2, HEIGHT(rcBottomBar));

    RECT rcText = rcBottomBar;
    if(szLeftMenu)
    {
        if(eMenuSelect == MENU_Left)
            imgGlow.DrawImage(gdi, rcBottomBar.left, rcBottomBar.top);

        rcText.right    = rcText.right/2;
        ::DrawText(gdi, szLeftMenu, rcText, DT_CENTER | DT_VCENTER | DT_NOPREFIX, hFontLarge, 0xDDDDDD);
    }

    rcText = rcBottomBar;
    if(szRightMenu)
    {
        if(eMenuSelect == MENU_Right)
            imgGlow.DrawImage(gdi, rcBottomBar.right/2, rcBottomBar.top);

        rcText.left    = rcText.right/2;
        ::DrawText(gdi, szRightMenu, rcText, DT_CENTER | DT_VCENTER | DT_NOPREFIX, hFontLarge, 0xDDDDDD);
    }
}


void CObjGui::DrawBackground(RECT& rcClip)
{
    if(!IsRectInRect(rcClip, rcBackground))
        return;

    RECT rc;
    IntersectRect(&rc, &rcClip, &rcBackground);

    Draw(gdiMem,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        gdiBackground,
        rc.left, rc.top - rcBackground.top);
}


void CObjGui::InitBottomBar(RECT& rcClient)
{
    if(gdiBottomBar.GetWidth() == WIDTH(rcClient))
        return;

    CIssGDIEx gdiBar;


    gdiBar.LoadImage(IsVGA()?IDB_PNG_Menu_HR:IDB_PNG_Menu, m_hWndParent, m_hInst, TRUE);

    gdiBottomBar.Create(gdiMem, WIDTH(rcClient), gdiBar.GetHeight(), FALSE, TRUE);
    gdiBottomBar.InitAlpha(TRUE);

    Draw(gdiBottomBar, 0,0, gdiBar.GetWidth(), gdiBar.GetHeight(), gdiBar, 0,0, ALPHA_Copy);
    gdiBar.FlipVertical();
    Draw(gdiBottomBar, WIDTH(rcClient) - gdiBar.GetWidth(), 0, gdiBar.GetWidth(), gdiBar.GetHeight(), gdiBar, 0,0, ALPHA_Copy);

    if(WIDTH(rcClient) - 2*gdiBar.GetWidth() > 0)
    {
        CIssGDIEx gdiMiddle;
        gdiMiddle.LoadImage(IsVGA()?IDB_PNG_MenuMid_HR:IDB_PNG_MenuMid, m_hWndParent, m_hInst, TRUE);
        RECT rc;
        SetRect(&rc, gdiBar.GetWidth(), 0, WIDTH(rcClient)-gdiBar.GetWidth(), gdiBar.GetHeight());
        TileBackGround(gdiBottomBar, rc, gdiMiddle, TRUE);
    }

    rcBottomBar = rcClient;
    rcBottomBar.top = rcBottomBar.bottom - gdiBar.GetHeight();
}


void CObjGui::InitBackground(RECT& rcClient)
{
    if(gdiBackground.GetWidth() == WIDTH(rcClient))
        return;

    if(m_hInst == NULL || m_hWndParent == NULL)
        return;//save the error messages ....

    rcBackground.top		= 0;
    rcBackground.left		= 0;
    rcBackground.right	= GetSystemMetrics(SM_CXSCREEN);
    rcBackground.bottom	= GetSystemMetrics(SM_CYSCREEN);

    //if(gdiBackground.GetDC() == NULL)
    {
        CIssGDIEx gdiTemp;
        gdiBackground.Create(gdiMem, rcBackground);
  
        switch(m_eBackground)
        {
        default:
        case BGCOLOR_Green:
            gdiTemp.LoadImage(IDB_PNG_BgGreen, m_hWndParent, m_hInst);
            break;
        case BGCOLOR_Blue:
            gdiTemp.LoadImage(IDB_PNG_BgBlue, m_hWndParent, m_hInst);
            break;
        case BGCOLOR_Purple:
            gdiTemp.LoadImage(IDB_PNG_BgPurple, m_hWndParent, m_hInst);
            break;
        case BGCOLOR_Red:
            gdiTemp.LoadImage(IDB_PNG_BgRed, m_hWndParent, m_hInst);
            break;
        case BGCOLOR_Orange:
            gdiTemp.LoadImage(IDB_PNG_BgOrange, m_hWndParent, m_hInst);
            break;
        case BGCOLOR_Pink:
            gdiTemp.LoadImage(IDB_PNG_BgPink, m_hWndParent, m_hInst);
            break;
        }

        //FillRect(gdiBackground, rcBackground, 0);//for now
        if(GetSystemMetrics(SM_CXSCREEN) > gdiTemp.GetWidth() && gdiTemp.GetDC() != NULL)
        {
            //rotate
            gdiTemp.Rotate(FALSE);
        }

        int iOffSet = (gdiTemp.GetWidth() - gdiBackground.GetWidth())/2;
        if(iOffSet < 0)
            iOffSet = 0;

        Draw(gdiBackground, 0, 0, gdiBackground.GetWidth(), gdiBackground.GetHeight(), gdiTemp, iOffSet);

        if(GetSystemMetrics(SM_CXICON) < 64)
        {
            CIssGDIEx gdi;
            gdi.LoadImage(IDB_PNG_Logo, m_hWndParent, m_hInst);
            SIZE sz;
            sz.cx = gdi.GetWidth()/2;
            sz.cy = gdi.GetHeight()/2;
            ScaleImage(gdi, gdiTemp, sz, FALSE, 0);
        }
        else
        {
            gdiTemp.LoadImage(IDB_PNG_Logo, m_hWndParent, m_hInst);
        }

        POINT pt;
        pt.x = (gdiBackground.GetWidth() - gdiTemp.GetWidth())/2;
        pt.y = (gdiBackground.GetHeight())/2 + (gdiBackground.GetHeight()/2 - gdiTemp.GetHeight())/2;

        Draw(gdiBackground, pt.x, pt.y, gdiTemp.GetWidth(), gdiTemp.GetHeight(), gdiTemp);
    }
}


BOOL CObjGui::OnLButtonUp(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow)
{
    if(eMenuSelect != MENU_None)
    {   
        if(PtInRect(&rcBottomBar, pt))
        {
            if(pt.x < GetSystemMetrics(SM_CXSCREEN)/2 && eMenuSelect == MENU_Left)
                PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Left,0), 0);
            else if(pt.x >= GetSystemMetrics(SM_CXSCREEN)/2 && eMenuSelect == MENU_Right)
                PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Right,0), 0);
        }

        eMenuSelect = MENU_None;
        // redraw the menu
        InvalidateRect(hWnd, &rcBottomBar, FALSE);
        ::UpdateWindow(hWnd);
        return TRUE;
    }
    return FALSE;
}

BOOL CObjGui::OnLButtonDown(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow)
{
    eMenuSelect = MENU_None;
    if(PtInRect(&rcBottomBar, pt))
    {        
        if(pt.x < GetSystemMetrics(SM_CXSCREEN)/2)
            eMenuSelect = MENU_Left;
        else
            eMenuSelect = MENU_Right;
        // redraw the menu
        InvalidateRect(hWnd, &rcBottomBar, FALSE);
        ::UpdateWindow(hWnd);
        return TRUE;
    }
    return FALSE;
}


BOOL CObjGui::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {   
    case VK_TSOFT1:
        eMenuSelect = MENU_Left;
        PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Left,0), 0);
        break;
    case VK_TSOFT2:
        eMenuSelect = MENU_Right;
        PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Right,0), 0);
        break;
    default:
        return UNHANDLED;
    }

    // redraw the menu
    InvalidateRect(hWnd, &rcBottomBar, FALSE);
    ::UpdateWindow(hWnd);
    eMenuSelect = MENU_None;
    InvalidateRect(hWnd, &rcBottomBar, FALSE);
    ::UpdateWindow(hWnd);
    return TRUE;
}

BOOL CObjGui::OnSize(HWND hWnd, RECT rcClient)
{
    if(WIDTH(rcClient) != gdiMem.GetWidth())
    {
        HDC dc = NULL;
        dc = GetDC(hWnd);
        InitGDI(rcClient, dc);
        ReleaseDC(hWnd, dc);
    }
    return TRUE;

}