#include "CalcMenu.h"
#include "Resource.h"
#include "IssDebug.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"
#include "globals.h"

#define CENTER_ICON_WIDTH 40

CCalcMenu::CCalcMenu(void)
:m_hFontMenu(NULL)
,m_eMenuPressed(MENU_None)
,m_hWnd(NULL)
{
    SetRect(&m_rcMenu, 0, 0, 0, 0);
    SetRect(&m_rcLeft, 0, 0, 0, 0);
    SetRect(&m_rcRight, 0, 0, 0, 0);
    SetRect(&m_rcIcon, 0, 0, 0, 0);
}

CCalcMenu::~CCalcMenu(void)
{
    CIssGDIEx::DeleteFont(m_hFontMenu);
}

HRESULT CCalcMenu::Init(HWND hWnd, HINSTANCE hInst, EnumCalcProSkins eSkin)
{
    m_gdiMem.Destroy();

	m_eCurrent = eSkin;

    HRESULT hr = S_OK;

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    CIssGDIEx gdiBack;

    CHHRE(hWnd, _T("CCalcMenu - Init - hWnd is NULL"), E_INVALIDARG);
    CHHRE(hInst, _T("CCalcMenu - Init - hInst is NULL"), E_INVALIDARG);

    switch(eSkin)
    {
    case SKIN_Blue:
        hr = gdiBack.LoadImage(IDR_PNG_MenuBarBlue, hWnd, hInst);
    	break;
    case SKIN_Pink:
        hr = gdiBack.LoadImage(IDR_PNG_MenuBarPink, hWnd, hInst);
    	break;
    case SKIN_Silver:
        hr = gdiBack.LoadImage(IDR_PNG_MenuBarSteel, hWnd, hInst);
        break;
    default:
        hr = gdiBack.LoadImage(IDR_PNG_MenuBar, hWnd, hInst);
        break;
    }
    
    
    CHRE(hr, _T("CalcMenu - LoadImage Failed"), E_FAIL);

//#ifdef UNDER_CE
    int iMenuHeight = GetSysMets(SM_CXICON)*5/8; //20 height at 32
//#else
//	int iMenuHeight = 40;
//#endif

    int iCenterIcon = CENTER_ICON_WIDTH;

    if(iMenuHeight != gdiBack.GetHeight())
    {
        CIssGDIEx gdiTemp;

        SIZE szNew;
        szNew.cx = iMenuHeight*gdiBack.GetWidth()/gdiBack.GetHeight();
        szNew.cy = iMenuHeight;

        int iOldWidth = gdiBack.GetWidth();

        ScaleImage(gdiBack, gdiTemp, szNew, FALSE, 0);

        gdiBack.Destroy();//we're blank
        gdiTemp.MoveGdiMemory(gdiBack);

        iCenterIcon = iCenterIcon*szNew.cx/iOldWidth;
    }
    
//#ifdef UNDER_CE
    m_hFontMenu = CIssGDIEx::CreateFont(GetSysMets(SM_CXICON)*3/7, FW_BOLD, TRUE);
//#else
//	m_hFontMenu = CIssGDIEx::CreateFont(28, FW_BOLD, TRUE);
//#endif
    
    SetRect(&m_rcMenu, rcClient.left, rcClient.bottom - iMenuHeight, rcClient.right, rcClient.bottom);
    SetRect(&m_rcLeft, rcClient.left, rcClient.bottom - iMenuHeight, rcClient.left + WIDTH(rcClient)/2 - iCenterIcon/2, rcClient.bottom);
    SetRect(&m_rcRight, m_rcLeft.right + iCenterIcon, rcClient.bottom - iMenuHeight, rcClient.right, rcClient.bottom);
    SetRect(&m_rcIcon, m_rcLeft.right, m_rcLeft.top, m_rcRight.left, m_rcRight.bottom);

    int iXBlit = (gdiBack.GetWidth()-WIDTH(rcClient))/2;

    if(iXBlit < 0)
        iXBlit = 0;

    m_gdiMem.Destroy();

    HDC dc = GetDC(hWnd);

    m_gdiMem.Create(dc, WIDTH(rcClient), HEIGHT(m_rcMenu), FALSE, TRUE);
    m_gdiTemp.Create(dc, WIDTH(rcClient), HEIGHT(m_rcMenu), FALSE, TRUE);

    ReleaseDC(hWnd, dc);
    
    //draw the background onto our gdiMem
    BitBlt(m_gdiMem.GetDC(), 0, 0, WIDTH(rcClient), HEIGHT(rcClient), gdiBack.GetDC(), iXBlit, 0, SRCCOPY);

    //draw the dividing line
 /*   int iIndent = HEIGHT(m_rcMenu)/6;//we'll try this

    HPEN hPen = CIssGDIEx::CreatePen(0x222222, 0, 1);
    HPEN hPenOld = (HPEN)SelectObject(m_gdiMem.GetDC(), hPen);

    POINT pt[2];
    pt[0].y = iIndent;
    pt[1].y = pt[0].y + HEIGHT(m_rcMenu) - 2*iIndent;
    pt[0].x = m_rcMenu.left + WIDTH(m_rcMenu)/2;
    pt[1].x = pt[0].x;

    Polyline(m_gdiMem.GetDC(), pt, 2);

    pt[0].x++;
    pt[1].x++;

    DeleteObject(hPen);
    hPen = CIssGDIEx::CreatePen(0xDDDDDD, 0, 1);
    SelectObject(m_gdiMem, hPen);

    Polyline(m_gdiMem.GetDC(), pt, 2);

    SelectObject(m_gdiMem.GetDC(), hPenOld);
    DeleteObject(hPen);*/

    m_hWnd = hWnd;
    if(m_gdiGlow.GetDC() == NULL)
        m_gdiGlow.LoadImage((IsVGA()?IDR_PNG_MenuGlowVGA:IDR_PNG_MenuGlow), hWnd, hInst, TRUE);

Error:
    return hr;
}

BOOL CCalcMenu::DrawMenu(HDC hdc, RECT& rcClip, TCHAR* szLeft, TCHAR* szRight)
{
    if(!IsRectInRect(rcClip, m_rcMenu))
        return TRUE;

	if(m_eMenuPressed == MENU_Left)
	{
		POINT pt;
		pt.y = (HEIGHT(m_rcLeft) - m_gdiGlow.GetHeight())/2;
		pt.x = m_rcLeft.left + (WIDTH(m_rcLeft) - m_gdiGlow.GetWidth())/2;

		// copy background over
		BitBlt(m_gdiTemp.GetDC(), 0, 0, WIDTH(m_rcMenu), HEIGHT(m_rcMenu), m_gdiMem, 0, 0, SRCCOPY);

		//draw glow
		::Draw(m_gdiTemp, pt.x, pt.y, m_gdiGlow.GetWidth(), m_gdiGlow.GetHeight(), m_gdiGlow, 0, 0);

		//we can safely assume the menu is always at the bottom
		BitBlt(hdc, rcClip.left, m_rcMenu.top, WIDTH(rcClip), m_gdiTemp.GetHeight(), 
			m_gdiTemp.GetDC(), rcClip.left, 0, SRCCOPY);
	}
	else if(m_eMenuPressed == MENU_Right)
	{
		POINT pt;
		pt.y = (HEIGHT(m_rcRight) - m_gdiGlow.GetHeight())/2;
		pt.x = m_rcRight.left + (WIDTH(m_rcRight) - m_gdiGlow.GetWidth())/2;

		// copy background over
		BitBlt(m_gdiTemp.GetDC(), 0, 0, WIDTH(m_rcMenu), HEIGHT(m_rcMenu), m_gdiMem, 0, 0, SRCCOPY);

		//draw glow
		::Draw(m_gdiTemp, pt.x, pt.y, m_gdiGlow.GetWidth(), m_gdiGlow.GetHeight(), m_gdiGlow, 0, 0);

		//we can safely assume the menu is always at the bottom
		BitBlt(hdc, rcClip.left, m_rcMenu.top, WIDTH(rcClip), m_gdiTemp.GetHeight(), 
			m_gdiTemp.GetDC(), rcClip.left, 0, SRCCOPY);
	}
	else
	{
		//we can safely assume the menu is always at the bottom
		BitBlt(hdc, rcClip.left, m_rcMenu.top, WIDTH(rcClip), m_gdiMem.GetHeight(), 
			m_gdiMem.GetDC(), rcClip.left, 0, SRCCOPY);
	}

    //Draw the text
    RECT rcTemp = m_rcLeft;
 
    if(szLeft)
    {
        OffsetRect(&rcTemp, 1, 1);
        DrawText(hdc, szLeft, rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontMenu, 0);
        OffsetRect(&rcTemp, -1, -1);
  //      if(m_eMenuPressed != MENU_Left) //seemss wrong - Adrian Dec 19/09     
        DrawText(hdc, szLeft, rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontMenu, 0xFFFFFF);
    }

    rcTemp = m_rcRight;

    if(szRight)
    {
        OffsetRect(&rcTemp, 1, 1);
        DrawText(hdc, szRight, rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontMenu, 0);
        OffsetRect(&rcTemp, -1, -1);
   //     if(m_eMenuPressed != MENU_Right)      
        DrawText(hdc, szRight, rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontMenu, 0xFFFFFF);
    }

    return TRUE;
}

BOOL CCalcMenu::OnLButtonDown(POINT& pt, HWND hWnd)
{
	m_eMenuPressed = MENU_None;

    if(PtInRect(&m_rcLeft, pt))
    {
        m_eMenuPressed = MENU_Left;

		InvalidateRect(hWnd, &m_rcLeft, FALSE);
		UpdateWindow(hWnd);

        /*POINT pt;
        pt.y = (HEIGHT(m_rcLeft) - m_gdiGlow.GetHeight())/2;
        pt.x = m_rcLeft.left + (WIDTH(m_rcLeft) - m_gdiGlow.GetWidth())/2;

        HDC dc = GetDC(hWnd);
        //draw the current screen to gdimem
        BitBlt(m_gdiTemp.GetDC(), 0, 0, m_rcMenu.right, HEIGHT(m_rcMenu), dc, 0, m_rcMenu.top, SRCCOPY);

        //draw glow
        ::Draw(m_gdiTemp, pt.x, pt.y, m_gdiGlow.GetWidth(), m_gdiGlow.GetHeight(), m_gdiGlow, 0, 0);

        //put it back on the screen
        BitBlt(dc, 0, m_rcMenu.top, m_rcMenu.right, m_rcMenu.bottom, m_gdiTemp.GetDC(), 0, 0, SRCCOPY);

        ReleaseDC(hWnd, dc);*/



      /*  ::Draw(*g_gui->GetGDI(), iX, m_rcMenu[i].top, 
            g_gui->GetGlow()->GetWidth(), g_gui->GetGlow()->GetHeight(),
            *g_gui->GetGlow());

        HDC dc = GetDC(hWnd);
        BitBlt(dc, m_rcMenu[i].left, m_rcMenu[i].top, WIDTH(m_rcMenu[i]), HEIGHT(m_rcMenu[i]),
            g_gui->GetGDI()->GetDC(), m_rcMenu[i].left, m_rcMenu[i].top, SRCCOPY);
        ReleaseDC(hWnd, dc);*/

        return TRUE;
    }

    if(PtInRect(&m_rcRight, pt))
    {
        m_eMenuPressed = MENU_Right;

        /*POINT pt;
        pt.y = (HEIGHT(m_rcRight) - m_gdiGlow.GetHeight())/2;
        pt.x = m_rcRight.left + (WIDTH(m_rcRight) - m_gdiGlow.GetWidth())/2;

        HDC dc = GetDC(hWnd);
        //draw the current screen to gdimem
        BitBlt(m_gdiTemp.GetDC(), 0, 0, m_rcMenu.right, HEIGHT(m_rcMenu), dc, 0, m_rcMenu.top, SRCCOPY);

        //draw glow
        ::Draw(m_gdiTemp, pt.x, pt.y, m_gdiGlow.GetWidth(), m_gdiGlow.GetHeight(), m_gdiGlow, 0, 0);

        //put it back on the screen
        BitBlt(dc, 0, m_rcMenu.top, m_rcMenu.right, m_rcMenu.bottom, m_gdiTemp.GetDC(), 0, 0, SRCCOPY);

        ReleaseDC(hWnd, dc);*/

		InvalidateRect(hWnd, &m_rcRight, FALSE);
		UpdateWindow(hWnd);

        return TRUE;
    }

    if(PtInRect((&m_rcIcon), pt))
    {
        m_eMenuPressed = MENU_Icon;
        return TRUE;
    }

    return UNHANDLED;
}

EnumMenuPressed CCalcMenu::OnLButtonUp(POINT& pt, HWND hWnd)
{
    if(m_eMenuPressed == MENU_Left)
    {        
		if(PtInRect(&m_rcLeft, pt))
			OnLeftMenu(hWnd);

		m_eMenuPressed = MENU_None;
		InvalidateRect(hWnd, &m_rcLeft, FALSE);
        return MENU_Left;
    }

    if(m_eMenuPressed == MENU_Right)
    {
        if(PtInRect(&m_rcRight, pt))
			OnRightMenu(hWnd);

		m_eMenuPressed = MENU_None;
		InvalidateRect(hWnd, &m_rcRight, FALSE);
        return MENU_Right;
    }

    if(m_eMenuPressed == MENU_Icon)
    {
        
		if(PtInRect(&m_rcIcon, pt))
			OnIconMenu(hWnd);

		m_eMenuPressed = MENU_None;
		InvalidateRect(hWnd, &m_rcIcon, FALSE);
        return MENU_Icon;
    }

	return MENU_None;
}

BOOL CCalcMenu::OnLeftMenu(HWND hWnd)
{
    return PostMessage(hWnd, WM_LEFT_SOFTKEY, 0, 0);
}

BOOL CCalcMenu::OnRightMenu(HWND hWnd)
{
    return PostMessage(hWnd, WM_RIGHT_SOFTKEY, 0, 0);
}

BOOL CCalcMenu::OnIconMenu(HWND hWnd)
{
    return PostMessage(hWnd, WM_ICON_SOFTKEY, 0, 0);
}