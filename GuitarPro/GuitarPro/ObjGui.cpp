#include "ObjGui.h"
#include "Resource.h"
#include "IssCommon.h"
#include "IssGDIFX.h"
#include "ObjOptions.h"
#include "globals.h"

extern CObjOptions* g_options;

#define TEXT_COLOR 0xDDDDDD

#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Selected      RGB(149,149,149)

CObjGui::CObjGui(void)
:m_hWndParent(NULL)
,m_hInst(NULL)
,m_eMenuSelect(MENU_None)
,m_eSkin(SKIN_4)
,m_oStr(CIssString::Instance())
,m_bPlaySounds(2)
{
    SetRect(&m_rcBottomBar, 0, GetSystemMetrics(SM_CYSCREEN) - MENU_HEIGHT, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    SetRect(&m_rcButtons, 0, 0, GetSystemMetrics(SM_CXSCREEN), BUTTONS_HEIGHT);
    SetRect(&m_rcNut, 0, BUTTONS_HEIGHT, GetSystemMetrics(SM_CXSCREEN), BUTTONS_HEIGHT + NUT_HEIGHT);

    for(int i = 0; i < 6; i++)
    {
        int iIndent = IsVGA()?78:39;
        int iSpacing = IsVGA()?64:32;

        if(GetSystemMetrics(SM_CXSCREEN)%240 != 0)
        {   //this would probably work for everything
            iIndent = 78*GetSystemMetrics(SM_CXSCREEN)/480;
            iSpacing = 64*GetSystemMetrics(SM_CXSCREEN)/480;
        }

        SetRect(&m_rcStrings[i], iIndent+i*iSpacing, BUTTONS_HEIGHT, 
            iIndent+i*iSpacing + STRING_WIDTH, GetSystemMetrics(SM_CYSCREEN) - MENU_HEIGHT);

        iIndent = IsVGA()?80:40;
        iSpacing = IsVGA()?62:31;

        if(GetSystemMetrics(SM_CXSCREEN)%240 != 0)
        {   //this would probably work for everything
            iIndent = 78*GetSystemMetrics(SM_CXSCREEN)/480;
            iSpacing = 64*GetSystemMetrics(SM_CXSCREEN)/480;
        }
        
        SetRect(&m_rcStringsLeft[i], iIndent+i*iSpacing, BUTTONS_HEIGHT, 
            iIndent+i*iSpacing + STRING_WIDTH, GetSystemMetrics(SM_CYSCREEN) - MENU_HEIGHT);


    }

    for(int i = 0; i < FONT_Count; i++)
        m_hFonts[i] = NULL;
}


//by string number
int CObjGui::GetStringX(int iString)
{
    if(iString < 0 || iString > 5)
        return 0;//tough

    if(g_options->GetLeftHanded())
    {
        return m_rcStringsLeft[5-iString].left + WIDTH(m_rcStringsLeft[5-iString])/2;
    }
    else
    {
        return m_rcStrings[iString].left + WIDTH(m_rcStrings[iString])/2;
    }
}

CObjGui::~CObjGui(void)
{
    Destroy();
}

void CObjGui::Destroy()
{
    m_gdiMem.Destroy();
    m_gdiBottomBar.Destroy();
    m_gdiBackground.Destroy();

    for(int i = 0; i < FONT_Count; i++)
        CIssGDIEx::DeleteFont(m_hFonts[i]);

    m_oSoundFX.Destroy();
}

void CObjGui::Init(HWND hWndParent, HINSTANCE hInst)
{
    Destroy();
    m_hInst         = hInst;
    m_hWndParent    = hWndParent;

    if(m_wndMenu.GetBackground() == IDR_PNG_MenuBack &&
        m_wndMenu.GetSelected() == IDB_PNG_MenuSelector &&
        m_wndMenu.GetImageArray() == (IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray))
        return;

    m_wndMenu.SetColors(TEXTCOLOR_Normal, TEXTCOLOR_Normal, TEXTCOLOR_Selected);
    m_wndMenu.SetBackground(IDR_PNG_MenuBack);
    m_wndMenu.SetSelected(IDB_PNG_MenuSelector);
    m_wndMenu.SetImageArray(IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray);
    m_wndMenu.PreloadImages(m_hWndParent, m_hInst);
}

BOOL CObjGui::DrawToScreen(RECT& rcClip, HDC dc)
{
    BitBlt(dc,
        rcClip.left, rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem.GetDC(),
        rcClip.left, rcClip.top,
        SRCCOPY);

    return TRUE;
}

BOOL CObjGui::DrawBackground(RECT& rcClip, CIssGDIEx* gdi /* = NULL */)
{
    if(m_gdiBackground.GetDC() == NULL)
    {
        if(m_hWndParent == NULL)
            return FALSE;

        RECT rcClient;
        GetClientRect(m_hWndParent, &rcClient);
        InitBackground(rcClient);
    }

    if(!IsRectInRect(rcClip, m_rcBackground))
        return FALSE;

    RECT rc;
    IntersectRect(&rc, &rcClip, &m_rcBackground);

    if(gdi == NULL)
        gdi = GetGDI();

    if(gdi == NULL)
        return FALSE;

    BitBlt(gdi->GetDC(), rc.left, rc.top, 
        WIDTH(rc), HEIGHT(rc),
        m_gdiBackground.GetDC(),
        rc.left, rc.top - m_rcBackground.top, SRCCOPY);


 /*   Draw(*gdi,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        m_gdiBackground,
        rc.left, rc.top - m_rcBackground.top, ALPHA_None);*/
    
    return TRUE;
}

BOOL CObjGui::DrawNut(RECT& rcClip, CIssGDIEx* gdi /* = NULL */)
{
    if(m_gdiNut.GetDC() == NULL)
    {
        if(GetSystemMetrics(SM_CXSCREEN)%240 != 0)//we need to scale
        {
            CIssGDIEx gdiTemp;
            gdiTemp.LoadImage(IsVGA()?IDB_PNG_Nut_HR:IDB_PNG_Nut, m_hWndParent, m_hInst);

            //we're only going to scale in the x direction .. not worried about height
            SIZE sz;
            sz.cy = gdiTemp.GetHeight();
            sz.cx = GetSystemMetrics(SM_CXSCREEN);

            //and now scale into our background DC
            ScaleImage(gdiTemp, m_gdiNut, sz, FALSE, 0);
        }
        else
        {
            m_gdiNut.LoadImage(IsVGA()?IDB_PNG_Nut_HR:IDB_PNG_Nut, m_hWndParent, m_hInst);
        }

        //for left handed
        if(g_options && g_options->GetLeftHanded() == TRUE)
        {
            m_gdiNut.FlipVertical();
        }
    }

    static RECT rc;
    IntersectRect(&rc, &rcClip, &m_rcNut);

    if(gdi == NULL)
        gdi = GetGDI();

    if(gdi == NULL)
        return FALSE;

    Draw(*gdi,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        m_gdiNut,
        rc.left, rc.top - m_rcNut.top);

    return TRUE;
}

void CObjGui::VibrateString(int iString)
{
    //we'll keep this pretty simple
/*    HDC dc = GetDC(m_hWndParent);

    //thinking right, then left then back to normal
    RECT rcTemp = m_rcStrings[iString];

    int iSpread = IsVGA()?2:1;

    rcTemp.left -= iSpread;
    rcTemp.right += iSpread;

    CIssGDIEx gdiTemp;
    gdiTemp.Create(dc, rcTemp);

    BitBlt(gdiTemp.GetDC(), 0, 0, WIDTH(rcTemp), HEIGHT(rcTemp), m_gdiMem.GetDC(), rcTemp.left, rcTemp.top, SRCCOPY);
    ::Draw(gdiTemp, 0, 0, m_gdiStrings[iString].GetWidth(), m_gdiStrings[iString].GetHeight(), m_gdiStrings[iString], 0, 0, ALPHA_Normal, 50);
    BitBlt(dc, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), gdiTemp.GetDC(), 0, 0, SRCCOPY);
    Sleep(300);


    ReleaseDC(m_hWndParent, dc);

    InvalidateRect(m_hWndParent, &rcTemp, FALSE);*/

    /*

    //erase the string
    
 //   BitBlt(dc, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), gdiTemp.GetDC(), 0, 0, SRCCOPY);
    
    //draw left
    BitBlt(gdiTemp.GetDC(), 0, 0, WIDTH(rcTemp), HEIGHT(rcTemp), m_gdiBackground.GetDC(), rcTemp.left, rcTemp.top, SRCCOPY);
    ::Draw(gdiTemp, 0, 0, m_gdiStrings[iString].GetWidth(), m_gdiStrings[iString].GetHeight(), m_gdiStrings[iString], 0, 0, ALPHA_Normal, 150);
    BitBlt(dc, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), gdiTemp.GetDC(), 0, 0, SRCCOPY);
    Sleep(50);

    //draw right
    BitBlt(gdiTemp.GetDC(), 0, 0, WIDTH(rcTemp), HEIGHT(rcTemp), m_gdiBackground.GetDC(), rcTemp.left, rcTemp.top, SRCCOPY);
    ::Draw(gdiTemp, 2*iSpread, 0, m_gdiStrings[iString].GetWidth(), m_gdiStrings[iString].GetHeight(), m_gdiStrings[iString], 0, 0, ALPHA_Normal, 150);
    BitBlt(dc, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), gdiTemp.GetDC(), 0, 0, SRCCOPY);
    Sleep(50);

    //redraw
    BitBlt(gdiTemp.GetDC(), 0, 0, WIDTH(rcTemp), HEIGHT(rcTemp), m_gdiBackground.GetDC(), rcTemp.left, rcTemp.top, SRCCOPY);
    ::Draw(gdiTemp, rcTemp.left, rcTemp.top, 0, 0, m_gdiStrings[iString], 150);

    ReleaseDC(m_hWndParent, dc);

    */
}



BOOL CObjGui::DrawStrings(RECT& rcClip, int iFade, CIssGDIEx* gdi /* = NULL */)
{
    if(m_gdiStrings[0].GetDC() == NULL)
    {
        if(GetSystemMetrics(SM_CYSCREEN) > 400 || IsVGA())
        {
            m_gdiStrings[0].LoadImage(IDB_PNG_String1_HR, m_hWndParent, m_hInst);
            m_gdiStrings[1].LoadImage(IDB_PNG_String2_HR, m_hWndParent, m_hInst);
            m_gdiStrings[2].LoadImage(IDB_PNG_String3_HR, m_hWndParent, m_hInst);
            m_gdiStrings[3].LoadImage(IDB_PNG_String4_HR, m_hWndParent, m_hInst);
            m_gdiStrings[4].LoadImage(IDB_PNG_String5_HR, m_hWndParent, m_hInst);
            m_gdiStrings[5].LoadImage(IDB_PNG_String6_HR, m_hWndParent, m_hInst);
        }
        else
        {
            m_gdiStrings[0].LoadImage(IDB_PNG_String1, m_hWndParent, m_hInst);
            m_gdiStrings[1].LoadImage(IDB_PNG_String2, m_hWndParent, m_hInst);
            m_gdiStrings[2].LoadImage(IDB_PNG_String3, m_hWndParent, m_hInst);
            m_gdiStrings[3].LoadImage(IDB_PNG_String4, m_hWndParent, m_hInst);
            m_gdiStrings[4].LoadImage(IDB_PNG_String5, m_hWndParent, m_hInst);
            m_gdiStrings[5].LoadImage(IDB_PNG_String6, m_hWndParent, m_hInst);
        }
    }

    static RECT rc;

    if(gdi == NULL)
        gdi = GetGDI();

    if(gdi == NULL)
        return FALSE;

    RECT rcTemp;

    int iAlpha = 255;
    int iString;//needed for lefty mode

    for(int i = 0; i < 6; i++)
    {
        iString = i;
        if(g_options && g_options->GetLeftHanded() == TRUE)
            iString = 5 - i;

        if(g_options->GetLeftHanded())
            rcTemp = m_rcStringsLeft[iString];
        else
            rcTemp = m_rcStrings[i];

        if(IsRectInRect(rcClip, rcTemp) == FALSE)
            continue;

        IntersectRect(&rc, &rcClip, &rcTemp);

        if((iFade & 1<<i) == 1<<i) 
            iAlpha = 100;
        else 
            iAlpha = 255;

        Draw(*gdi,
            rc.left, rc.top,
            WIDTH(rc), HEIGHT(rc),
            m_gdiStrings[i],
            rc.left - rcTemp.left, rc.top - rcTemp.top,
            ALPHA_Normal, iAlpha);
    }
    return TRUE;
}

BOOL CObjGui::DrawMenu(RECT& rcClip, CIssGDIEx* gdi /* = NULL */, BOOL bDrawIcons /* TRUE */ )
{
    if(m_imgMenu.GetImage().GetDC() == NULL)
    {
        m_imgMenu.Initialize(m_hWndParent, m_hInst, IsVGA()?IDB_PNG_Menu_HR: IDB_PNG_Menu);
        m_imgMenu.SetSize(GetSystemMetrics(SM_CXSCREEN), HEIGHT(m_rcBottomBar));
    }
  
    static RECT rc;
    IntersectRect(&rc, &rcClip, &m_rcBottomBar);

    if(gdi == NULL)
        gdi = GetGDI();

    Draw(*gdi,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        m_imgMenu.GetImage(),
        rc.left, rc.top - m_rcBottomBar.top);

    if(bDrawIcons == FALSE)
        return TRUE;

    //and the icons
    //align top/center
    POINT ptLoc;
    int iMW = WIDTH(m_rcBottomBar)/4; //width of each menu button .. ie 1/4 of the whole thing
    int iIW = GetMenuIcons()->GetWidth()/(MICON_Count); //width of the individual icon images in the array
    ptLoc.y = m_rcBottomBar.top;
    
    ptLoc.x = (iMW - iIW)/2;
 //   if(iHLIndex == MICON_Chords)
 //       Draw(*gdi, ptLoc.x, ptLoc.y, GetGlow()->GetWidth(), GetGlow()->GetHeight(), *GetGlow());
    Draw(*gdi, ptLoc.x, ptLoc.y, iIW, GetMenuIcons()->GetHeight(), *GetMenuIcons(), iIW*MICON_Chords, 0);

    ptLoc.x += iMW;//so just move along the width of the menu button
//    if(iHLIndex == MICON_Tuner)
//        Draw(*gdi, ptLoc.x, ptLoc.y, GetGlow()->GetWidth(), GetGlow()->GetHeight(), *GetGlow());
    Draw(*gdi, ptLoc.x, ptLoc.y, iIW, GetMenuIcons()->GetHeight(), *GetMenuIcons(), iIW*MICON_Tuner, 0);

    ptLoc.x += iMW;
//    if(iHLIndex == MICON_Metronome)
 //       Draw(*gdi, ptLoc.x, ptLoc.y, GetGlow()->GetWidth(), GetGlow()->GetHeight(), *GetGlow());
    Draw(*gdi, ptLoc.x, ptLoc.y, iIW, GetMenuIcons()->GetHeight(), *GetMenuIcons(), iIW*MICON_Metronome, 0);

    ptLoc.x += iMW;
//    if(iHLIndex == MICON_Menu)
//        Draw(*gdi, ptLoc.x, ptLoc.y, GetGlow()->GetWidth(), GetGlow()->GetHeight(), *GetGlow());
    Draw(*gdi, ptLoc.x, ptLoc.y, iIW, GetMenuIcons()->GetHeight(), *GetMenuIcons(), iIW*MICON_Menu, 0);

    //and finally ... the dividers
    int iSpacing = WIDTH(m_rcBottomBar)/4;
    int iIndent = HEIGHT(m_rcBottomBar)/5;

    POINT ptS, ptE;

    for(int i = 0; i < 3; i++)
    {
        ptS.x = iSpacing + i*iSpacing-1;
        ptS.y = m_rcBottomBar.top + iIndent;
        ptE.x = iSpacing + i*iSpacing-1; 
        ptE.y = m_rcBottomBar.bottom - iIndent;

        Line(gdi->GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0);

        ptE.x++;
        ptS.x++;

        Line(gdi->GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0x666666);
    }

    return TRUE;
}

BOOL CObjGui::DrawTopButtons(RECT& rcClip, CIssGDIEx* gdi /* = NULL */)
{
    if(m_imgButtons.GetImage().GetDC() == NULL)
    {
        m_imgButtons.Initialize(m_hWndParent, m_hInst, IsVGA()?IDB_PNG_Menu_HR: IDB_PNG_Menu);
        m_imgButtons.SetSize(GetSystemMetrics(SM_CXSCREEN), HEIGHT(m_rcButtons));
    }

    static RECT rc;
    IntersectRect(&rc, &rcClip, &m_rcButtons);

    //if its null draw to our memDC
    if(gdi == NULL)
        gdi = GetGDI();

    Draw(*gdi,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        m_imgButtons.GetImage(),
        rc.left, rc.top - m_rcButtons.top);

    //and finally ... the dividers
    int iSpacing = WIDTH(m_rcButtons)/4;
    int iIndent = HEIGHT(m_rcButtons)/5;

    POINT ptS, ptE;

    for(int i = 0; i < 3; i++)
    {
        ptS.x = iSpacing + i*iSpacing-1;
        ptS.y = m_rcButtons.top + iIndent;
        ptE.x = ptS.x;
        ptE.y = m_rcButtons.bottom - iIndent;

        Line(gdi->GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0);

        ptE.x++;
        ptS.x++;

        Line(gdi->GetDC(), ptS.x, ptS.y, ptE.x, ptE.y, 0x666666);
    }

    return TRUE;
}

void CObjGui::InitBackground(RECT& rcClient)
{
    if(m_gdiBackground.GetWidth() == WIDTH(rcClient))
        return;

    if(m_hInst == NULL || m_hWndParent == NULL)
        return;//save the error messages ....

    m_rcBackground.top		= 0;
    m_rcBackground.left		= 0;
    m_rcBackground.right	= GetSystemMetrics(SM_CXSCREEN);
    m_rcBackground.bottom	= GetSystemMetrics(SM_CYSCREEN);

    if(m_gdiBackground.GetDC() == NULL)
    {
        CIssGDIEx gdiTemp;
        m_gdiBackground.Create(m_gdiMem, m_rcBackground);
        FillRect(m_gdiBackground.GetDC(), m_rcBackground, 0);

        UINT idRes = 0;

        switch(m_eSkin)
        {
        case SKIN_1:
            idRes = IsVGA()?IDB_PNG_Neck1_HR:IDB_PNG_Neck1;
        	break;
        case SKIN_2:
            idRes = IsVGA()?IDB_PNG_Neck2_HR:IDB_PNG_Neck2;
        	break;
        case SKIN_3:
            idRes = IsVGA()?IDB_PNG_Neck3_HR:IDB_PNG_Neck3;
            break;
        case SKIN_4:
            idRes = IsVGA()?IDB_PNG_Neck4_HR:IDB_PNG_Neck4;
            break;
        case SKIN_5:
            idRes = IsVGA()?IDB_PNG_Neck5_HR:IDB_PNG_Neck5;
        	break;
        case SKIN_6:
            idRes = IsVGA()?IDB_PNG_Neck6_HR:IDB_PNG_Neck6;
        	break;
        case SKIN_7:
            idRes = IsVGA()?IDB_PNG_Neck8_HR:IDB_PNG_Neck8;
            break;

        }

        gdiTemp.LoadImage(idRes, m_hWndParent, m_hInst);
  
        int iOffSet = (gdiTemp.GetWidth() - m_gdiBackground.GetWidth())/2;
        if(iOffSet < 0)
            iOffSet = 0;

        if( WIDTH(m_rcBackground) == HEIGHT(m_rcBackground))
        {
            CIssGDIEx gdiTemp2;

            //draw the unscaled version into the new buffer
            RECT rcTemp;
            rcTemp.left = 0;
            rcTemp.top = 0;
            rcTemp.right = gdiTemp.GetWidth();
            rcTemp.bottom = IsVGA()?640:320 - BUTTONS_HEIGHT;//easy enough to hard code

            gdiTemp2.Create(gdiTemp.GetDC(), rcTemp);
            Draw(gdiTemp2, rcTemp, gdiTemp);

            SIZE size;
            size.cx = m_gdiBackground.GetWidth();
            size.cy = m_gdiBackground.GetHeight() - BUTTONS_HEIGHT;

            //and now scale into our background DC
            ScaleImage(gdiTemp2, gdiTemp, size, FALSE, 0);
        }
        //hard coded for the HTC HD Mini
        else if(WIDTH(m_rcBackground) == 320 && HEIGHT(m_rcBackground) == 480) 
        {
            CIssGDIEx gdiTemp2;

            //draw the unscaled version into the new buffer
            RECT rcTemp;
            rcTemp.left = 0;
            rcTemp.top = 0;
            rcTemp.right = gdiTemp.GetWidth();
            rcTemp.bottom = 400 - BUTTONS_HEIGHT;//easy enough to hard code

            gdiTemp2.Create(gdiTemp.GetDC(), rcTemp);
            Draw(gdiTemp2, rcTemp, gdiTemp);

            SIZE size;
            size.cx = m_gdiBackground.GetWidth();
            size.cy = m_gdiBackground.GetHeight() - BUTTONS_HEIGHT;

            //and now scale into our background DC
            ScaleImage(gdiTemp2, gdiTemp, size, FALSE, 0);
        }

        Draw(m_gdiBackground, 0, BUTTONS_HEIGHT, m_gdiBackground.GetWidth(), m_gdiBackground.GetHeight() - BUTTONS_HEIGHT, gdiTemp, iOffSet);

        //for left handed
        if(g_options && g_options->GetLeftHanded() == TRUE)
        {
            m_gdiBackground.FlipVertical();
        }

        //and the details
        DrawMenu(rcClient, &m_gdiBackground, FALSE);
        DrawTopButtons(rcClient, &m_gdiBackground);
    }
}


BOOL CObjGui::OnLButtonUp(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow)
{
    if(PtInRect(&m_rcBottomBar, pt))
    {
        if(pt.x > WIDTH(m_rcBottomBar)/2)
            PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Right,0), 0);
        else
            PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Left,0), 0);
    }
    return TRUE;
}

BOOL CObjGui::OnLButtonDown(HWND hWnd, POINT& pt, BOOL bCheckScrollArrow)
{
/*    m_eMenuSelect = MENU_None;
    if(PtInRect(&m_rcBottomBar, pt))
    {        
        if(pt.x < GetSystemMetrics(SM_CXSCREEN)/2)
            m_eMenuSelect = MENU_Left;
        else
            m_eMenuSelect = MENU_Right;
        // redraw the menu
        InvalidateRect(hWnd, &m_rcBottomBar, FALSE);
        ::UpdateWindow(hWnd);
        return TRUE;
    }*/
    return FALSE;
}


BOOL CObjGui::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {   
    case 0://VK_TSOFT1:
        m_eMenuSelect = MENU_Left;
        PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Left,0), 0);
        break;
    case 1://VK_TSOFT2:
        m_eMenuSelect = MENU_Right;
        PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDMENU_Right,0), 0);
        break;
    default:
        return UNHANDLED;
    }

    // redraw the menu
    InvalidateRect(hWnd, &m_rcBottomBar, FALSE);
    ::UpdateWindow(hWnd);
    m_eMenuSelect = MENU_None;
    InvalidateRect(hWnd, &m_rcBottomBar, FALSE);
    ::UpdateWindow(hWnd);
    return TRUE;
}

BOOL CObjGui::OnSize(HWND hWnd, RECT rcClient)
{
    if(WIDTH(rcClient) != m_gdiMem.GetWidth())
    {
        HDC dc = NULL;
        dc = GetDC(hWnd);
        ReleaseDC(hWnd, dc);
    }
    return TRUE;
}

CIssGDIEx* CObjGui::GetGDI(BOOL bDestroy /* = FALSE */)
{
    if(m_hWndParent == NULL)
        return NULL;
    
    if(bDestroy)
        m_gdiMem.Destroy();

    if(m_gdiMem.GetDC() == NULL)
    {
        HDC dc = GetDC(m_hWndParent);
        m_gdiMem.Create(dc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        ReleaseDC(m_hWndParent, dc);
    }    
    return &m_gdiMem;
}

CIssGDIEx* CObjGui::GetBackground(BOOL bDestroy /* = FALSE */)
{
    if(m_hWndParent == NULL)
        return NULL;

    if(bDestroy)
    {
        m_gdiBackground.Destroy();
        m_gdiNut.Destroy(); //a bit lazy but not really a problem
    }

    RECT rcClient;
    GetClientRect(m_hWndParent, &rcClient);
    InitBackground(rcClient);

    return &m_gdiBackground;
}

CIssGDIEx* CObjGui::GetAltBack(BOOL bDestroy)
{
    if(m_hWndParent == NULL)
        return NULL;

    if(bDestroy)
        m_gdiAltBack.Destroy();
    
    if(m_gdiAltBack.GetDC() == NULL)
    {
        RECT rcClient;
        GetClientRect(m_hWndParent, &rcClient);
        m_gdiAltBack.Create(m_gdiBackground.GetDC(), m_gdiBackground.GetWidth(), m_gdiBackground.GetHeight(), TRUE);
        AlphaFillRect(m_gdiAltBack, rcClient, 0, 127);
    }
    return &m_gdiAltBack;
}

CIssGDIEx* CObjGui::GetMenu()
{
    if(m_imgMenu.GetImage().GetDC() == NULL)
    {
        m_imgMenu.Initialize(m_hWndParent, m_hInst, IsVGA()?IDB_PNG_Menu_HR: IDB_PNG_Menu);
        m_imgMenu.SetSize(GetSystemMetrics(SM_CXSCREEN), HEIGHT(m_rcBottomBar));
    }

    return &m_imgMenu.GetImage();
}

CIssGDIEx* CObjGui::GetMenuIcons()
{
    if(m_gdiMenuIcons.GetDC() == NULL)
    {
        m_gdiMenuIcons.LoadImage(IsVGA()?IDB_PNG_Icons_HR:IDB_PNG_Icons, m_hWndParent, m_hInst);
    }
    return &m_gdiMenuIcons;
}

CIssGDIEx* CObjGui::GetMenuCheck()
{
    if(m_gdiMenuCheck.GetDC() == NULL)
        m_gdiMenuCheck.LoadImage(IsVGA()?IDB_PNG_Check_HR:IDB_PNG_Check, m_hWndParent, m_hInst);
    return &m_gdiMenuCheck;
}

CIssGDIEx* CObjGui::GetMenuArrow()
{
    if(m_gdiMenuUp.GetDC() == NULL)
        m_gdiMenuUp.LoadImage(IsVGA()?IDB_PNG_MenuUp_HR:IDB_PNG_MenuUp, m_hWndParent, m_hInst);
    return &m_gdiMenuUp;
}

CIssGDIEx* CObjGui::GetFinger()
{
    if(m_gdiFinger.GetDC() == NULL)
    {
        m_gdiFinger.LoadImage(IsVGA()?IDB_PNG_Note_HR:IDB_PNG_Note, m_hWndParent, m_hInst);
    }
    return &m_gdiFinger;
}

CIssGDIEx* CObjGui::GetFingerDark()
{
    if(m_gdiFingerDark.GetDC() == NULL)
    {
        m_gdiFingerDark.LoadImage(IsVGA()?IDB_PNG_NoteDark_HR:IDB_PNG_NoteDark, m_hWndParent, m_hInst);
    }
    return &m_gdiFingerDark;
}

CIssGDIEx* CObjGui::GetFingerRed()
{
    if(m_gdiFingerRed.GetDC() == NULL)
    {
        m_gdiFingerRed.LoadImage(IsVGA()?IDB_PNG_NoteRed_HR:IDB_PNG_NoteRed, m_hWndParent, m_hInst);
    }
    return &m_gdiFingerRed;
}

CIssGDIEx* CObjGui::GetFingerGrey()
{
    if(m_gdiFingerGrey.GetDC() == NULL)
    {
        m_gdiFingerGrey.LoadImage(IsVGA()?IDB_PNG_NoteGrey_HR:IDB_PNG_NoteGrey, m_hWndParent, m_hInst);
    }
    return &m_gdiFingerGrey;
}

CIssGDIEx* CObjGui::GetNoNote()
{
    if(m_gdiNoNote.GetDC() == NULL)
    {
        m_gdiNoNote.LoadImage(IsVGA()?IDB_PNG_NoNote_HR:IDB_PNG_NoNote, m_hWndParent, m_hInst);

    }
    return &m_gdiNoNote;
}

CIssGDIEx* CObjGui::GetStar(BOOL bGrey)
{
    if(bGrey)
    {
        if(m_gdiStarGrey.GetDC() == NULL)
            m_gdiStarGrey.LoadImage(IsVGA()?IDB_PNG_StarG_HR:IDB_PNG_StarG, m_hWndParent, m_hInst);
        return &m_gdiStarGrey;
    }

    if(m_gdiStar.GetDC() == NULL)
        m_gdiStar.LoadImage(IsVGA()?IDB_PNG_Star_HR:IDB_PNG_Star, m_hWndParent, m_hInst);
    return &m_gdiStar;
}

CIssGDIEx* CObjGui::GetGlow()
{
    if(m_gdiGlow.GetDC() == NULL)
        m_gdiGlow.LoadImage(IsVGA()?IDB_PNG_MenuGlow_HR:IDB_PNG_MenuGlow, m_hWndParent, m_hInst);
    return &m_gdiGlow;
}

//not all fonts are here ... should really move em all
HFONT CObjGui::GetFont(EnumFont eFont)
{
    if(eFont < 0 || eFont >= FONT_Count)
        return NULL;

    if(m_hFonts[eFont] == NULL)
    {
        switch(eFont)
        {
        case FONT_Button://intentionally not exact scale
            //tweak it up a little for the Palm
            if(GetSystemMetrics(SM_CXSCREEN) == 320 && GetSystemMetrics(SM_CYSCREEN) == 320)
                m_hFonts[eFont] = CIssGDIEx::CreateFont(16, FW_BOLD, TRUE);
            else
                m_hFonts[eFont] = CIssGDIEx::CreateFont(IsVGA()?24:13, FW_BOLD, TRUE);
            break;
        case FONT_Menu://just this one is scaled
            m_hFonts[eFont] = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*11/32, FW_BOLD, TRUE);
            break;
        case FONT_MenuSmall://just this one is scaled
            m_hFonts[eFont] = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*11/32, FW_NORMAL, TRUE);
            break;
        case FONT_MenuLarge://just this one is scaled
            m_hFonts[eFont] = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*14/32, FW_BOLD, TRUE);
            break;
        case FONT_Fingers:
             m_hFonts[eFont] = CIssGDIEx::CreateFont(IsVGA()?32:16, FW_BOLD, TRUE);
            break;
        case FONT_Frets:
            m_hFonts[eFont] = CIssGDIEx::CreateFont(IsVGA()?24:12, FW_BOLD, TRUE);//roman numerals don't currently fit ...
            break;
        case FONT_MetroLabels:
            m_hFonts[eFont] = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*14/32, FW_BOLD, TRUE);
            break;
        default:
            return NULL;//good ol default
            break;
        }
    }
    return m_hFonts[eFont];
}


void CObjGui::LoadSounds()
{
    if(m_oSoundFX.IsLoaded())
        return;

    m_iClickIndex = 0;

    m_iClickIndex = m_oSoundFX.Initialize(m_hInst);

    for(UINT i = LOWEST_SOUND_RES; i <= HIGHEST_SOUND_RES; i++)
        m_oSoundFX.LoadSFX(i);

    m_iClickIndex = m_oSoundFX.LoadSFX(IDR_WAV_Click1);

    m_oSoundFX.SetVolumeSFX(25);
}

void CObjGui::PlayClick()
{
    if(m_oSoundFX.IsLoaded() == FALSE)
        LoadSounds();
    m_oSoundFX.PlaySFX(m_iClickIndex);
}

void CObjGui::PlaySFX(int iIndex)
{
    if(m_oSoundFX.IsLoaded() == FALSE)
        LoadSounds();

    m_oSoundFX.PlaySFX(iIndex);
}

void CObjGui::SetSkin(EnumSkin eSkin)
{
    if(eSkin == m_eSkin)
        return;

    m_gdiBackground.Destroy();
    m_gdiAltBack.Destroy();
    m_eSkin = eSkin;
}

CIssGDIEx* CObjGui::GetLEDs()
{
    if(m_gdiLEDs.GetDC() == NULL)
    {
        m_gdiLEDs.LoadImage(IsVGA()?IDB_PNG_LEDs_HR:IDB_PNG_LEDs, m_hWndParent, m_hInst);
    }
    return &m_gdiLEDs;
}

CIssImageSliced* CObjGui::GetAlpha(int iW, int iH)
{
    if(m_imgAlpha.IsLoaded() == FALSE)
        m_imgAlpha.Initialize(m_hWndParent, m_hInst, IsVGA()?IDB_PNG_Alpha_HR:IDB_PNG_Alpha);
    if(m_imgAlpha.IsInitialized() == FALSE ||
        m_imgAlpha.GetWidth() != iW || 
        m_imgAlpha.GetHeight() != iH)
    {
        m_imgAlpha.SetSize(iW, iH);
    }

    return &m_imgAlpha;
}

CIssImageSliced* CObjGui::GetImgOutline(int iW, int iH)
{
    if(m_imgImgOutline.IsLoaded() == FALSE)
        m_imgImgOutline.Initialize(m_hWndParent, m_hInst, IsVGA()?IDB_PNG_ImgOutline_HR:IDB_PNG_ImgOutline);
    if(m_imgImgOutline.IsInitialized() == FALSE ||
        m_imgImgOutline.GetWidth() != iW || 
        m_imgImgOutline.GetHeight() != iH)
    {
        m_imgImgOutline.SetSize(iW, iH);
    }

    return &m_imgImgOutline;
}

void CObjGui::SetPlaySounds(BOOL bPlaySounds)
{
    m_bPlaySounds = bPlaySounds;

    switch(bPlaySounds)
    {
    case 0:
        m_oSoundFX.SetVolumeSFX(0);
        break;
    default:
    case 1:
        m_oSoundFX.SetVolumeSFX(12);
        break;
    case 2:
        m_oSoundFX.SetVolumeSFX(24);
        break;
    case 3:
        m_oSoundFX.SetVolumeSFX(36);
        break;
    case 4:
        m_oSoundFX.SetVolumeSFX(36);
        break;
    }
}