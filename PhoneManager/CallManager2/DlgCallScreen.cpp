#include "StdAfx.h"
#include "DlgCallScreen.h"
#include "Resource.h"
#include "ContactsGuiDefines.h"
#include "CommonDefines.h"
#include "IssCommon.h"
#include "IssGDIFX.h"
#include "tapi.h"
#include "GeneralOptions.h"
#include "IssRegistry.h"
#include "ServerCommunicate.h"

//#include "ril-xdadev.h"
//#include "regext.h"
//#pragma comment(lib, "ril.lib")

#define BUTTON_Height       (GetSystemMetrics(SM_CXICON)*3/2)
#define PICTURE_Width       (GetSystemMetrics(SM_CXSMICON)*6)
#define PICTURE_Height      (GetSystemMetrics(SM_CXSMICON)*9)
#define CALL_BTN_Height     (GetSystemMetrics(SM_CXSMICON)*2)

#define GLOBE_HEIGHT        (3*GetSystemMetrics(SM_CXICON))

#define IDT_GLOBE_TIMER     2
#define IDT_SHOW            7   //for the agressive show ... shouldn't need it but I'll leave the code in place

#define NUMBER_OF_GLOBE_FRAMES  31
#define GLOBE_ANI_RATE      33 //30 fps


CDlgCallScreen::CDlgCallScreen(void)
{
    //m_gdiMem            = new CIssGDIEx;
    m_bDrawReflection   = TRUE;
    m_bUseCheekGuard    = FALSE;
    m_iCallTimer        = 0;
    m_iGlobeFrame       = 0;
    m_iBtnSel           = -1;//nothing for now
    m_iSliderPos        = 0;
    m_eCallState        = CSTATE_Disconnected;
    m_iBattLevel        = 50;
    m_iSignalLevel      = 75;
    m_bScreenLocked     = FALSE;

    m_sAreaCode         = NULL;
    
    m_oStr->Empty(m_szName);
    m_oStr->Empty(m_szLocation);
    m_oStr->Empty(m_szTime);
    m_oStr->Empty(m_szPhoneNum);
    m_oStr->Empty(m_szCarrier);

    m_oStr->StringCopy(m_szSliderText, _T("Unlock Cheek Guard"));

    m_hFontStatus       = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);
    m_hFontText         = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*13/32, FW_BOLD, TRUE);
    m_hFontLocation     = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*2/5, FW_BOLD, TRUE);
    m_hFontButtons      = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*4/9, FW_BOLD, TRUE);
}

CDlgCallScreen::~CDlgCallScreen(void)
{
    CIssGDIEx::DeleteFont(m_hFontStatus);
    CIssGDIEx::DeleteFont(m_hFontText);
    CIssGDIEx::DeleteFont(m_hFontLocation);
    CIssGDIEx::DeleteFont(m_hFontButtons);

    KillAllTimers();
}

BOOL CDlgCallScreen::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    InvalidateRect(m_hWnd, NULL, FALSE);//for now
    return CDlgBase::OnInitDialog(hWnd, wParam, lParam);
}

BOOL CDlgCallScreen::Show(BOOL bAgressive /* = FALSE */)
{
    if(m_hWnd == NULL)
        return FALSE;

    // foreground
    BOOL bResult = FALSE;

    SetForegroundWindow((HWND)((ULONG) (m_hWnd)|0x00000001));
    SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON);
    MoveWindow(m_hWnd, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), TRUE);
    ShowWindow(m_hWnd, SW_SHOW);//apparently needed ... hehe

    bResult = SetForegroundWindow(m_hWnd);
    if(!bResult)
    {
        DebugOut(_T("SetForegroundWindow() failed"));
    }

    if(bAgressive)
        SetTimer(m_hWnd, IDT_SHOW, 250, NULL);
    

    return bResult;
}

BOOL CDlgCallScreen::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    DrawPicture(gdi, rcClip);   
    DrawBackground(gdi, rcClip);
    DrawText(gdi, rcClip);
    DrawGlobe(gdi, rcClip);
    DrawButtons(gdi, rcClip);

    // draw it all to the screen
  /*  BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem->GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);*/

    return TRUE;
}

BOOL CDlgCallScreen::DrawPicture(CIssGDIEx& gdi, RECT& rcClip)
{
    if(!IsRectInRect(rcClip, m_rcPicture) || m_gdiPicture.GetDC() == NULL)
        return TRUE;

    int iX, iY, iHeight;
    iX = m_rcPicture.left + (WIDTH(m_rcPicture) - m_gdiPicture.GetWidth())/ 2;
    iY = m_rcPicture.top + (HEIGHT(m_rcPicture) - m_gdiPicture.GetHeight())/2;

    iHeight = m_gdiPicture.GetHeight();

    Draw(gdi,
        iX,iY,
        m_gdiPicture.GetWidth(), m_gdiPicture.GetHeight(),
        m_gdiPicture,
        0, 0);

    Draw(gdi,
        iX, iY + iHeight,
        m_gdiReflection.GetWidth(), m_gdiReflection.GetHeight(),
        m_gdiReflection,
        0,0);

    return TRUE;
}

BOOL CDlgCallScreen::DrawButtons(CIssGDIEx& gdi, RECT& rcClip)
{
    if(m_bUseCheekGuard && m_bScreenLocked)
    {
        DrawSlider(gdi, rcClip, rcClip);
        return TRUE;
    }

    RECT rcTemp;
    int iIconIndex;
    //we'll just do the icon and text calculations on the fly

    if(IsRectInRect(rcClip, m_rcAnswer))
    {
        if(m_iBtnSel == 0)
        {
            m_imgBtnGreen.DrawImage(gdi, m_rcAnswer.left, m_rcAnswer.top);
            iIconIndex = 1;
        }
        else
        {    
            m_imgBtn.DrawImage(gdi, m_rcAnswer.left, m_rcAnswer.top);
            iIconIndex = 0;
        }
        rcTemp = m_rcAnswer;
        rcTemp.left += INDENT;
        rcTemp.right -= HEIGHT(rcTemp);
        ::DrawText(gdi.GetDC(), _T("ANSWER"), rcTemp, DT_CENTER | DT_VCENTER, m_hFontButtons, 0xFFFFFF);

        rcTemp.left = rcTemp.right;
        rcTemp.top += (CALL_BTN_Height - m_gdiIcons.GetHeight())/2;
        rcTemp.bottom = rcTemp.top + m_gdiIcons.GetHeight();
        rcTemp.right = rcTemp.left + m_gdiIcons.GetHeight();

        ::Draw(gdi, rcTemp, m_gdiIcons, iIconIndex*m_gdiIcons.GetHeight(), 0);
    }

    if(IsRectInRect(rcClip, m_rcIgnore))
    {
        if(m_iBtnSel == 1)
        {
            m_imgBtnRed.DrawImage(gdi, m_rcIgnore.left, m_rcIgnore.top);
            iIconIndex = 3;
        }
        else
        {
            m_imgBtn.DrawImage(gdi, m_rcIgnore.left, m_rcIgnore.top);
            iIconIndex = 2;
        }
        rcTemp = m_rcIgnore;
        rcTemp.left += INDENT;
        rcTemp.right -= HEIGHT(rcTemp);
        ::DrawText(gdi.GetDC(), _T("IGNORE"), rcTemp, DT_CENTER | DT_VCENTER, m_hFontButtons, 0xFFFFFF);

        rcTemp.left = rcTemp.right;
        rcTemp.top += (CALL_BTN_Height - m_gdiIcons.GetHeight())/2;
        rcTemp.bottom = rcTemp.top + m_gdiIcons.GetHeight();
        rcTemp.right = rcTemp.left + m_gdiIcons.GetHeight();

        ::Draw(gdi, rcTemp, m_gdiIcons, iIconIndex*m_gdiIcons.GetHeight(), 0);
    }

    return TRUE;
}

BOOL CDlgCallScreen::DrawGlobe(CIssGDIEx& gdi, RECT& rcClip)
{
    if(m_gdiGlobe.GetDC() == NULL || IsRectInRect(rcClip, m_rcGlobe) == FALSE || m_iGlobeFrame == -1)
        return FALSE;

#define FLAG_SIZE  (GetSystemMetrics(SM_CXICON)/4)

    Draw(gdi,
        m_rcGlobe.left,m_rcGlobe.top,
        m_gdiGlobe.GetHeight(), m_gdiGlobe.GetHeight(),
        m_gdiGlobe,
        m_gdiGlobe.GetHeight()*m_iGlobeFrame,0);

    if(m_sAreaCode && (m_sAreaCode->iFrame == m_iGlobeFrame) && m_sAreaCode->iX > 0 && m_sAreaCode->iY > 0)
    {
        //draw the point
        int iX = m_sAreaCode->iX*m_gdiGlobe.GetHeight()/100;
        int iY = m_sAreaCode->iY*m_gdiGlobe.GetHeight()/100;

        RECT rcLoc;
        SetRect(&rcLoc, iX - FLAG_SIZE/2 + m_rcGlobe.left, iY - FLAG_SIZE/2 + m_rcGlobe.top, iX + FLAG_SIZE/2 + m_rcGlobe.left, iY + FLAG_SIZE/2 + m_rcGlobe.top);

    //    SetPixel(gdi.GetDC(), iX + m_rcGlobe.left, iY+m_rcGlobe.top, 0x00FFFF);//for now
        RoundRectangle(gdi.GetDC(), rcLoc, 0x00FFFF, 0, FLAG_SIZE, FLAG_SIZE);
    }

    return TRUE;
}

BOOL CDlgCallScreen::DrawBackground(CIssGDIEx& gdi, RECT& rcClip)
{
    if(IsRectInRect(rcClip, m_rcCallInfo))
        m_imgCallBack.DrawImage(gdi, m_rcCallInfo.left, m_rcCallInfo.top);
    if(IsRectInRect(rcClip, m_rcYellow))
        m_imgLocation.DrawImage(gdi, m_rcYellow.left, m_rcYellow.top);


    return TRUE;
}

void CDlgCallScreen::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rcTemp = m_rcIndicators;
    rcTemp.bottom++;

    FillRect(gdi.GetDC(), rcTemp, 0x818181);
    FillRect(gdi.GetDC(), m_rcIndicators, 0);

    ::DrawText(gdi.GetDC(), m_szCarrier, m_rcIndicators, DT_VCENTER | DT_CENTER | DT_NOPREFIX, m_hFontLocation, 0xFFFFFF);

    rcTemp = m_rcIndicators;
    rcTemp.top += (HEIGHT(m_rcIndicators)-m_gdiIndicatorIcons.GetHeight())/2;
    rcTemp.bottom = rcTemp.top + m_gdiIndicatorIcons.GetHeight();
    rcTemp.left += (HEIGHT(m_rcIndicators)-m_gdiIndicatorIcons.GetWidth()/2)/2;
    rcTemp.right = rcTemp.left + m_gdiIndicatorIcons.GetWidth()/2;

    //draw signal indicator icon
    ::Draw(gdi, rcTemp, m_gdiIndicatorIcons, 0, 0);

    //draw signal indicator
    rcTemp = m_rcIndicators;
    rcTemp.top += (HEIGHT(m_rcIndicators)-m_gdiSignalIndicator.GetHeight())/2;
    rcTemp.bottom = rcTemp.top + m_gdiSignalIndicator.GetHeight();
    rcTemp.left += HEIGHT(m_rcIndicators);
    rcTemp.right = rcTemp.left + m_gdiSignalIndicator.GetWidth()/10;

    ::Draw(gdi, rcTemp, m_gdiSignalIndicator, (m_gdiSignalIndicator.GetWidth()/10)*m_iSignalLevel, 0);

    //draw battery indicator
    rcTemp = m_rcIndicators;
    rcTemp.top += (HEIGHT(m_rcIndicators)-m_gdiIndicatorIcons.GetHeight())/2;
    rcTemp.bottom = rcTemp.top + m_gdiIndicatorIcons.GetHeight();
    rcTemp.right -= (HEIGHT(m_rcIndicators)-m_gdiIndicatorIcons.GetWidth()/2)/2;
    rcTemp.left = rcTemp.right - m_gdiIndicatorIcons.GetWidth()/2;

    
    ::Draw(gdi, rcTemp, m_gdiIndicatorIcons, m_gdiIndicatorIcons.GetWidth()/2, 0);

    //draw battery meter
    rcTemp = m_rcIndicators;
    rcTemp.top += (HEIGHT(m_rcIndicators)-m_gdiBatteryIndicator.GetHeight())/2;
    rcTemp.bottom = rcTemp.top + m_gdiBatteryIndicator.GetHeight();
    rcTemp.right -= HEIGHT(m_rcIndicators);
    rcTemp.left = rcTemp.right - m_gdiBatteryIndicator.GetWidth()/10;

    ::Draw(gdi, rcTemp, m_gdiBatteryIndicator, (m_gdiBatteryIndicator.GetWidth()/10)*(9-m_iBattLevel), 0);




    return;

}

BOOL CDlgCallScreen::DrawText(CIssGDIEx& gdi, RECT& rcClip)
{
    static RECT rcTemp;
    static TCHAR szTemp[STRING_LARGE];
    static TCHAR* pszTemp;

    if(IsRectInRect(rcClip, m_rcStatus))
    {
        switch(m_eCallState)
        {
        case CSTATE_Ringing:
            m_oStr->StringCopy(szTemp, _T("Incoming call..."));
            break;
        case CSTATE_Connected:
            m_oStr->StringCopy(szTemp, _T("Connected"));
            break;
        case CSTATE_Dialing:
            m_oStr->StringCopy(szTemp, _T("Dialing"));
            break;
        case CSTATE_OnHold:
            m_oStr->StringCopy(szTemp, _T("On hold"));
            break;
        case CSTATE_Disconnected:
        case CSTATE_Unknown:
            m_oStr->StringCopy(szTemp, _T("Disconnected"));
        default:
            break;
        }

        rcTemp = m_rcStatus;
        OffsetRect(&rcTemp, 1, 1);
        ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER, m_hFontStatus, 0);
        ::DrawText(gdi.GetDC(), szTemp, m_rcStatus, DT_LEFT | DT_VCENTER, m_hFontStatus, 0xFFFFFF);
    }

    //call timer  
    if(m_eCallState == CSTATE_Connected && IsRectInRect(rcClip, m_rcTime))
    {
        int iHour = m_iCallTimer/3600;
        int iMin = (m_iCallTimer - iHour*3600)/60;
        int iSec = m_iCallTimer - iHour*3600 - iMin*60;

        if(iHour > 0)
            m_oStr->Format(szTemp, _T("%.2i:%.2i:%.2i"), iHour, iMin, iSec);
        else
            m_oStr->Format(szTemp, _T("%.2i:%.2i"), iMin, iSec);
        
        rcTemp = m_rcTime;
        OffsetRect(&rcTemp, 1, 1);
        ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER, m_hFontText, 0);
        ::DrawText(gdi.GetDC(), szTemp, m_rcTime, DT_LEFT | DT_VCENTER, m_hFontText, 0xFFFFFF);
    }

    m_oStr->StringCopy(szTemp, _T("Unknown"));
    if(IsRectInRect(rcClip, m_rcName))
    {
        rcTemp = m_rcName;
        OffsetRect(&rcTemp, 1, 1);
        ::DrawText(gdi.GetDC(), m_szName, rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontText, 0);
        ::DrawText(gdi.GetDC(), m_szName, m_rcName, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontText, 0xFFFFFF);
    }

    if(IsRectInRect(rcClip, m_rcPhoneNum))
    {
   //     if(m_oStr->IsEmpty(m_szPhoneType) == FALSE)
   //         m_oStr->Format(szTemp, _T("%s %s"), m_szPhoneNum, m_szPhoneType);
   //     else
            m_oStr->StringCopy(szTemp, m_szPhoneNum);

        rcTemp = m_rcPhoneNum;
        OffsetRect(&rcTemp, 1, 1);
        ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontText, 0);
        ::DrawText(gdi.GetDC(), szTemp, m_rcPhoneNum, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontText, 0xFFFFFF);
    }

    if(IsRectInRect(rcClip, m_rcYellow))
    {
        ::DrawText(gdi.GetDC(), m_szLocation, m_rcLocation, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontLocation, 0);
        ::DrawText(gdi.GetDC(), m_szTime, m_rcLocalTime, DT_CENTER | DT_TOP | DT_END_ELLIPSIS, m_hFontLocation, 0);
    }

    return TRUE;
}

BOOL CDlgCallScreen::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_eCallState == CSTATE_Connected && m_bUseCheekGuard)
        return (OnSliderButtonUp(pt));

    if(PtInRect(&m_rcAnswer, pt))
    {
        OnAnswer();
        m_iBtnSel = -1;
        InvalidateRect(m_hWnd, &m_rcAnswer, FALSE);
    }

    else if(PtInRect(&m_rcIgnore, pt))
    {
        OnIgnore();
        m_iBtnSel = -1;
        InvalidateRect(m_hWnd, &m_rcIgnore, FALSE);
    }

    return UNHANDLED;
}

void CDlgCallScreen::OnMenuLeft()
{
    if(m_eCallState == CSTATE_Connected && m_bUseCheekGuard)
    {
        // just unlock
        OnUnlock();
    }
    else
    {
        m_iBtnSel = 0;
        InvalidateRect(m_hWnd, &m_rcAnswer, FALSE);
        UpdateWindow(m_hWnd);
        m_iBtnSel = -1;
        OnAnswer();
        InvalidateRect(m_hWnd, &m_rcAnswer, FALSE);
    }

}

void CDlgCallScreen::OnMenuRight()
{
    if(m_eCallState == CSTATE_Connected && m_bUseCheekGuard)
    {
        // just unlock
        OnUnlock();
    }
    else
    {
        m_iBtnSel = 1;
        InvalidateRect(m_hWnd, &m_rcIgnore, FALSE);
        UpdateWindow(m_hWnd);
        m_iBtnSel = -1;
        OnIgnore();
        InvalidateRect(m_hWnd, &m_rcIgnore, FALSE);
    }
}

BOOL CDlgCallScreen::OnLButtonDown(HWND hWnd, POINT& pt)
{   
    if(m_eCallState == CSTATE_Connected && m_bUseCheekGuard)
    {
        OnSliderButtonDown(pt);
        return TRUE;
    }

    if(PtInRect(&m_rcAnswer, pt))
    {  
        m_iBtnSel = 0;
        InvalidateRect(m_hWnd, &m_rcAnswer, FALSE);
    }
    else if(PtInRect(&m_rcIgnore, pt))
    {
        m_iBtnSel = 1;
        InvalidateRect(m_hWnd, &m_rcIgnore, FALSE);
    }
    return UNHANDLED;
}

BOOL CDlgCallScreen::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_bSliderDrag)
    {
        int iPos = pt.x - m_iSliderStartX;
        static int iMax = WIDTH(m_rcSlider) - 2*HEIGHT(m_rcSlider);

        if(iPos < 0)
            iPos = 0;
        if(iPos > iMax)
            iPos = iMax;

        m_iSliderPos = iPos;

   //     DebugOut(_T("Slider: %i\r\n"), m_iSliderPos);

        InvalidateRect(m_hWnd, &m_rcSlider, FALSE);
        UpdateWindow(m_hWnd);//we'll see if this is fast enough

        return TRUE;
    }

    return UNHANDLED;
}  

BOOL CDlgCallScreen::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return UNHANDLED;
}

BOOL CDlgCallScreen::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    return UNHANDLED;
}

void CDlgCallScreen::SetDefaults()
{
    GetLocalTime(&m_sCallStartTime);

    SetCallerName(NULL);
    SetCallerNumber(NULL);
    
    m_oStr->StringCopy(m_szLocation, _T("Unknown"));
    m_oStr->StringCopy(m_szTime, _T("Unknown"));
    m_sAreaCode = NULL;

    m_bScreenLocked     = FALSE;
    m_bUseCheekGuard    = FALSE;

    m_lOid = -1;

    m_iCallTimer = 0;
}

BOOL CDlgCallScreen::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    // so we only do this once
    if(WIDTH(rc) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rc) != GetSystemMetrics(SM_CYSCREEN))
        return FALSE;

    int iIndent = INDENT*2;

    int iIndicatorHeight = (IsVGA()?26:13);
    m_rcIndicators      = rc;
    m_rcIndicators.bottom = rc.top + iIndicatorHeight;

    //indicators
    




    //end indicators??

    m_rcPicture.left	= iIndent;
    m_rcPicture.top		= iIndent/2 + m_rcIndicators.bottom;
    m_rcPicture.right	= PICTURE_Width + m_rcPicture.left;
    m_rcPicture.bottom	= PICTURE_Height + m_rcPicture.top;

    m_rcStatus.left     = m_rcPicture.right + iIndent;
    m_rcStatus.top      = iIndent + m_rcIndicators.bottom;
    m_rcStatus.right    = rc.right - iIndent;
    m_rcStatus.bottom   = m_rcStatus.top + GetSystemMetrics(SM_CXSMICON);

    m_rcTime.left       = m_rcPicture.right + iIndent;
    m_rcTime.top        = m_rcStatus.bottom;
    m_rcTime.right      = rc.right - iIndent;
    m_rcTime.bottom     = m_rcTime.top + GetSystemMetrics(SM_CXSMICON);

    m_rcCallInfo.left   = m_rcPicture.right + iIndent;
    m_rcCallInfo.top    = m_rcTime.bottom + iIndent;
    m_rcCallInfo.right  = rc.right - iIndent;
    m_rcCallInfo.bottom = m_rcTime.top + 9*GetSystemMetrics(SM_CXSMICON)/2;//good enough for now

    m_rcName            = m_rcCallInfo;
    m_rcName.bottom     -= HEIGHT(m_rcName)/2;

    m_rcPhoneNum        = m_rcCallInfo;
    m_rcPhoneNum.top    += HEIGHT(m_rcPhoneNum)/2;

    if(GetSystemMetrics(SM_CYSCREEN) > GetSystemMetrics(SM_CXSCREEN))
    {
        m_rcGlobe.top       = m_rcCallInfo.bottom + iIndent;
        m_rcGlobe.left      = m_rcPicture.right + iIndent + (WIDTH(m_rcCallInfo) - GLOBE_HEIGHT)/2;
        m_rcGlobe.right     = m_rcGlobe.left + GLOBE_HEIGHT;
        m_rcGlobe.bottom    = m_rcGlobe.top + GLOBE_HEIGHT;//good enough for now

        m_rcYellow.left     = m_rcPicture.right + iIndent;
        m_rcYellow.top      = m_rcGlobe.bottom + iIndent;
        m_rcYellow.right    = rc.right - iIndent;
        m_rcYellow.bottom   = m_rcYellow.top + 2*GetSystemMetrics(SM_CXSMICON);//good enough for now

        m_rcLocation        = m_rcYellow;
        m_rcLocation.bottom -= HEIGHT(m_rcYellow)/2;

        m_rcLocalTime       = m_rcYellow;
        m_rcLocalTime.top   += HEIGHT(m_rcYellow)/2;
    }

    else
    {
        m_rcGlobe.top       = m_rcCallInfo.bottom + iIndent/2;
        m_rcGlobe.right     = rc.right - iIndent;
        m_rcGlobe.left      = rc.right - iIndent - GLOBE_HEIGHT;
        m_rcGlobe.bottom    = m_rcGlobe.top + GLOBE_HEIGHT;//good enough for now

        m_rcYellow.left     = iIndent;
        m_rcYellow.top      = m_rcPicture.bottom + iIndent;
        m_rcYellow.right    = m_rcGlobe.left - iIndent;
        m_rcYellow.bottom   = m_rcYellow.top + 2*GetSystemMetrics(SM_CXSMICON);//good enough for now

        m_rcLocation        = m_rcYellow;
        m_rcLocation.bottom -= HEIGHT(m_rcYellow)/2;

        m_rcLocalTime       = m_rcYellow;
        m_rcLocalTime.top   += HEIGHT(m_rcYellow)/2;
    }
    


    m_rcAnswer.left     = iIndent;
    m_rcAnswer.right    = WIDTH(rc)/2 - iIndent;
    m_rcAnswer.bottom   = rc.bottom - iIndent;
    m_rcAnswer.top      = m_rcAnswer.bottom - CALL_BTN_Height;

    m_rcIgnore          = m_rcAnswer;
    m_rcIgnore.left     = m_rcAnswer.right + 2*iIndent;
    m_rcIgnore.right    = rc.right - iIndent;

    m_rcSlider          = m_rcAnswer;
    m_rcSlider.right    = m_rcIgnore.right;

    if(m_rcSlider.top < m_rcYellow.bottom + INDENT)
        m_rcSlider.top = m_rcYellow.bottom + INDENT;

    m_imgCallBack.SetSize(WIDTH(m_rcCallInfo), HEIGHT(m_rcCallInfo));
    m_imgLocation.SetSize(WIDTH(m_rcYellow), HEIGHT(m_rcYellow));

    m_imgBtn.SetSize(WIDTH(m_rcAnswer), HEIGHT(m_rcAnswer));
    m_imgBtnGreen.SetSize(WIDTH(m_rcAnswer), HEIGHT(m_rcAnswer));
    m_imgBtnRed.SetSize(WIDTH(m_rcAnswer), HEIGHT(m_rcAnswer));
    


    return TRUE;
}

HRESULT CDlgCallScreen::LoadImages(HWND hWnd, HINSTANCE hInst)
{
    HRESULT hr = S_OK;

    if(!m_imgCallBack.IsLoaded())
        hr = m_imgCallBack.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_BKVGA:IDR_PNG_CALL_BK);

    if(!m_imgLocation.IsLoaded())
        hr = m_imgLocation.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_CallVGA:IDR_PNG_CALL_Call);

    if(!m_imgMask.IsLoaded())
        m_imgMask.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ImgAlphaVGA:IDR_PNG_CALL_ImgAlpha);

    if(!m_imgBorder.IsLoaded())
        m_imgBorder.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ImgBorderVGA:IDR_PNG_CALL_ImgBorder);

    if(!m_imgBtn.IsLoaded())
        m_imgBtn.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ButtonVGA:IDR_PNG_CALL_Button);

    if(!m_imgBtnRed.IsLoaded())
        m_imgBtnRed.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ButtonRedVGA:IDR_PNG_CALL_ButtonRed);

    if(!m_imgBtnGreen.IsLoaded())
        m_imgBtnGreen.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ButtonGreenVGA:IDR_PNG_CALL_ButtonGreen);

    if(m_gdiGlobe.GetDC() == NULL)
    {
#ifdef DEBUG
        m_gdiGlobe.LoadImage(IDR_PNG_CALL_Globe, hWnd, hInst);
#else
        CIssGDIEx gdiTemp;

        gdiTemp.LoadImage(IDR_PNG_CALL_Globe, hWnd, hInst);

        SIZE sz;

        sz.cy = GLOBE_HEIGHT;
        sz.cx = sz.cy*NUMBER_OF_GLOBE_FRAMES;

        ScaleImage(gdiTemp, m_gdiGlobe, sz, FALSE, 0);
#endif
    }


    if(m_gdiIcons.GetDC() == NULL)
        m_gdiIcons.LoadImage(IsVGA()?IDR_PNG_CALL_IconsVGA:IDR_PNG_CALL_Icons, hWnd, hInst);

    if(m_gdiImgArrowArray.GetDC() == NULL)
        m_gdiImgArrowArray.LoadImage(IsVGA()?IDR_PNG_FavoritesArrowArrayVGA:IDR_PNG_FavoritesArrowArray, hWnd, hInst);

    if(m_gdiBatteryIndicator.GetDC() == NULL)
        m_gdiBatteryIndicator.LoadImage(IsVGA()?IDR_PNG_CALL_BarVGA:IDR_PNG_CALL_Bar, hWnd, hInst);

    if(m_gdiSignalIndicator.GetDC() == NULL)
    {
        m_gdiSignalIndicator.LoadImage(IsVGA()?IDR_PNG_CALL_BarVGA:IDR_PNG_CALL_Bar, hWnd, hInst);
        m_gdiSignalIndicator.FlipVertical();
    }
    if(m_gdiIndicatorIcons.GetDC() == NULL)
        m_gdiIndicatorIcons.LoadImage(IsVGA()?IDR_PNG_CALL_IndicatorsVGA:IDR_PNG_CALL_Indicators, hWnd, hInst);


    if(m_gdiDefaultPic.GetDC() == NULL)
    {
        CIssGDIEx gdiTemp;
        gdiTemp.LoadImage(IsVGA()?IDR_PNG_DetailsDefaultVGA:IDR_PNG_DetailsDefault, hWnd, hInst, TRUE);
        SIZE sz;
        sz.cx   = sz.cy = PICTURE_Width;
        ScaleImage(gdiTemp, m_gdiDefaultPic, sz, FALSE, 0);
    }

    if(m_gdiPicture.GetDC() == NULL)
    {
        int iWidth = m_gdiDefaultPic.GetWidth();
        int iHeight= m_gdiDefaultPic.GetHeight();       
        if(m_imgMask.GetWidth() != iWidth || m_imgMask.GetHeight() != iHeight)
            m_imgMask.SetSize(iWidth, iHeight);
        if(m_imgBorder.GetWidth() != iWidth || m_imgBorder.GetHeight() != iHeight)
            m_imgBorder.SetSize(iWidth, iHeight);
        m_gdiPicture.Create(m_gdiDefaultPic, iWidth, iHeight, TRUE, TRUE);
        m_gdiPicture.SetAlphaMask(m_imgMask.GetImage());
        m_imgBorder.DrawImage(m_gdiPicture, 0,0);

        m_gdiReflection.CreateReflection(m_gdiPicture);
    }

    //lets set the user number
    TCHAR szTemp[STRING_LARGE];
    m_oStr->Empty(szTemp);

    DebugOut(_T("Images Loaded"));

    m_hInst = hInst;

    return hr;
}

BOOL CDlgCallScreen::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case IDT_GLOBE_TIMER:
       {
            if(m_sAreaCode == NULL)
            {
                KillTimer(m_hWnd, IDT_GLOBE_TIMER);
                break;
            }

            m_iGlobeFrame++;

            if(m_iGlobeFrame >= NUMBER_OF_GLOBE_FRAMES)
                m_iGlobeFrame = 0;

            if(m_iGlobeFrame == m_sAreaCode->iFrame)
            {
                KillTimer(m_hWnd, IDT_GLOBE_TIMER);
            }
            InvalidateRect(m_hWnd, &m_rcGlobe, FALSE);
            UpdateWindow(m_hWnd);//and redraw now
        }
        break;
   case IDT_SHOW:
        KillTimer(m_hWnd, IDT_SHOW);
        Show();
        break;
    default:
        return UNHANDLED;
        break;
    }
    return TRUE;
}

BOOL CDlgCallScreen::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case _T('g'):
    case _T('G'):
        SetTimer(hWnd, IDT_GLOBE_TIMER, GLOBE_ANI_RATE, NULL);
        m_iGlobeFrame = 0;
    	break;
    case _T('t'):
    case _T('T'):
        break;
    case _T('q'):
    case _T('Q'):
        PostQuitMessage(0);
        break;
    case _T('s'):
    case _T('S'):
        Show();
        break;
    case _T('r'):
    case _T('R'):
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    default:
        return UNHANDLED;
        break;
    }

    return TRUE;
}

BOOL CDlgCallScreen::CreateWin(HWND hWndParent)
{
    if(m_hWnd)
        return TRUE;

    if(!Create(_T("Cheek Guard"), hWndParent, m_hInst, _T("ClassCheekGuard"), 
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), WS_EX_TOPMOST, WS_VISIBLE))
        return FALSE;

    ShowWindow(m_hWnd, SW_HIDE);

    return TRUE;
}

BOOL CDlgCallScreen::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case VK_TTALK:
        //handled by the single message method now
        break;
    case VK_LEFT:
        m_iBtnSel--;
        if(m_iBtnSel < 0)
            m_iBtnSel = 1;
    	break;
    case VK_RIGHT:
        m_iBtnSel++;
        if(m_iBtnSel > 1)
            m_iBtnSel = 0;
        break;
    case VK_UP:
    case VK_DOWN:
        m_iBtnSel = -1;
        //m_oCallMan.StopRingTone();
        break;
    case VK_RETURN:
        {
            switch(m_iBtnSel)
            {
            case 0:
                OnAnswer();
            	break;
            case 1:
                OnIgnore();
                break;
            default:
                //m_oCallMan.StopRingTone();
                break;
            }
        }
        break;
    default:
        return CDlgBase::OnKeyUp(hWnd, wParam, lParam);
        break;
    }

    InvalidateRect(m_hWnd, NULL, FALSE);

    return TRUE;
}

BOOL CDlgCallScreen::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return UNHANDLED;
}

BOOL CDlgCallScreen::OnAnswer()
{
    if(m_eCallState)
        DoVerbAction(PH_VERB_ACCEPT_INCOMING);
    else
        DoVerbAction(PH_VERB_TALK);
    return TRUE;
    


 /*   HRIL  rilHandle;
    HRESULT hr = RIL_Initialize(1, &ResultCallBack, &NotifyCallBack, RIL_NCLASS_ALL, (DWORD) this, &rilHandle);

    RIL_Answer()

    hr = RIL_Answer(rilHandle);

    if(SUCCEEDED(hr))
    {
        RegistrySetDWORD(SN_PHONECALLTALKING_ROOT, SN_PHONECALLTALKING_PATH, SN_PHONECALLTALKING_VALUE, SN_PHONECALLTALKING_BITMASK);
    }

    RIL_Deinitialize(rilHandle);

    if(hr == S_OK)
        return TRUE;
    return FALSE;  */

}

BOOL CDlgCallScreen::OnIgnore()
{
    if(m_eCallState == CSTATE_Ringing)
        DoVerbAction(PH_VERB_REJECT_INCOMING);
    else
        DoVerbAction(PH_VERB_END);
    return TRUE;
}

BOOL CDlgCallScreen::OnUnlock()
{
    KillAllTimers();
    ShowWindow(m_hWnd, SW_HIDE);
    return TRUE;
}

void CDlgCallScreen::KillAllTimers()
{
    KillTimer(m_hWnd, IDT_GLOBE_TIMER);
    KillTimer(m_hWnd, IDT_SHOW);
}

BOOL CDlgCallScreen::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE) 
    {
        BOOL bResult = SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON);

        RECT rc;
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        bResult = MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);
        return bResult;

        HWND hWndSip = FindWindow(_T("MS_SIPBUTTON"), NULL);
        if(hWndSip)
            ShowWindow(hWndSip, SW_HIDE);
    }
    else if(LOWORD(wParam) == WA_INACTIVE)
    {
        
    }
    return UNHANDLED;
}

void CDlgCallScreen::SetCallerName(TCHAR* szName)
{
    if(szName)
        m_oStr->StringCopy(m_szName, szName);
    else
        m_oStr->StringCopy(m_szName, _T("Unknown"));

    InvalidateRect(m_hWnd, &m_rcName, FALSE);
}

void CDlgCallScreen::SetCallerNumber(TCHAR* szNumber)
{
    if(szNumber)
        m_oStr->StringCopy(m_szPhoneNum, szNumber);
    else
        m_oStr->StringCopy(m_szPhoneNum, _T("Unknown"));

    InvalidateRect(m_hWnd, &m_rcPhoneNum, FALSE);
}

void CDlgCallScreen::SetCarrier(TCHAR* szCarrier)
{
    if(szCarrier)
        m_oStr->StringCopy(m_szCarrier, szCarrier);

    InvalidateRect(m_hWnd, &m_rcCarrier, FALSE);
}

void CDlgCallScreen::StartGlobeAnimation()
{
    AreaCodeType* sArea = m_sAreaCode;

    if(sArea == NULL)
        return;

    SetTimer(m_hWnd, IDT_GLOBE_TIMER, GLOBE_ANI_RATE, NULL);
}

void CDlgCallScreen::DrawSlider(CIssGDIEx& gdi, RECT& rc, RECT& rcClip)
{
 //   if(!IsRectInRect(rcClip, m_rcSlider))
 //     return;

    GetSliderTrack().DrawImage(gdi, m_rcSlider.left, m_rcSlider.top);


    RECT rcTemp = m_rcSlider;
    rcTemp.right -= INDENT*4;
    rcTemp.top;
    ::DrawText(gdi.GetDC(), m_szSliderText, rcTemp, DT_VCENTER | DT_RIGHT, m_hFontText, 0xEEEEEE);

    DrawSliderKnob(gdi, rc);
}


RECT CDlgCallScreen::GetSliderRect()
{
    RECT rc;

    int iSliderWidth = m_imgSlider.GetWidth();

    SetRect(&rc, m_rcSlider.left + m_iSliderPos, 
            m_rcSlider.top, 
            m_rcSlider.left + m_iSliderPos + iSliderWidth,
            m_rcSlider.bottom);
    
    return rc;
}

void CDlgCallScreen::DrawSliderKnob(CIssGDIEx& gdi, RECT& rc)
{
    //1) slider image
    RECT rcSlider = GetSliderRect();
    GetSliderImg().DrawImage(gdi, rcSlider.left, rcSlider.top);

    // 3) arrow
    DrawSliderArrow(gdi);
}

void  CDlgCallScreen::DrawSliderArrow(CIssGDIEx& gdi)
{
    RECT rcSlider = GetSliderRect();

    int iArrowIndex = 2;

    rcSlider.left   = rcSlider.left + (WIDTH(rcSlider) - m_gdiImgArrowArray.GetHeight())/2;
    rcSlider.top    = rcSlider.top + (HEIGHT(rcSlider) - m_gdiImgArrowArray.GetHeight())/2;
    rcSlider.right  = rcSlider.left + m_gdiImgArrowArray.GetHeight();
    rcSlider.bottom = rcSlider.bottom + m_gdiImgArrowArray.GetHeight();

    ::Draw(gdi,
        rcSlider.left, rcSlider.top,
        WIDTH(rcSlider), HEIGHT(rcSlider),
        m_gdiImgArrowArray,
        iArrowIndex*m_gdiImgArrowArray.GetHeight(), 0);  
}


CIssImageSliced& CDlgCallScreen::GetSliderTrack()
{
    int w, h;

    w = WIDTH(m_rcSlider);
    h = HEIGHT(m_rcSlider);

    UINT uiRes = IsVGA() ? IDR_PNG_FavoritesSliderBgVGA : IDR_PNG_FavoritesSliderBg;
    if(!m_imgSliderTrack.IsLoaded())
        m_imgSliderTrack.Initialize(m_hWnd, m_hInst, uiRes);

    if(w != m_imgSliderTrack.GetWidth() || h != m_imgSliderTrack.GetHeight())
        m_imgSliderTrack.SetSize(w, h);

    return m_imgSliderTrack;
    
}


CIssImageSliced& CDlgCallScreen::GetSliderImg()
{
    int w, h;

    h = HEIGHT(m_rcSlider);
    w = h*2;

    UINT uiRes = IsVGA() ? IDR_PNG_FavoritesSliderVGA : IDR_PNG_FavoritesSlider;
    if(!m_imgSlider.IsLoaded())
        m_imgSlider.Initialize(m_hWnd, m_hInst, uiRes);

    if(w != m_imgSlider.GetWidth() || h != m_imgSlider.GetHeight())
        m_imgSlider.SetSize(w, h);

    return m_imgSlider;
    
}


BOOL CDlgCallScreen::OnSliderButtonDown(POINT pt)
{
    if(PtInRect(&GetSliderRect(), pt))
    {
        m_bSliderDrag = TRUE;
        m_iSliderPos = 0;
        m_iSliderStartX = pt.x;
    }

    return UNHANDLED;
}

BOOL CDlgCallScreen::OnSliderButtonUp(POINT pt)
{
    if(!m_bSliderDrag)
        return FALSE;

    if(GetSliderRect().right > m_rcSlider.right - GetSystemMetrics(SM_CXSMICON)/4)
    {
        OnUnlock();
        return TRUE;
    }
    else
    {
        // now animate the slider
        AnimateSlider(pt);
    }

    m_bSliderDrag = FALSE;

    InvalidateRect(m_hWnd, &m_rcSlider, FALSE);
    return TRUE;
}


void CDlgCallScreen::AnimateSlider(POINT pt)
{
    DWORD dwStart = GetTickCount();
    int iNumFrames = m_iSliderPos;
    int iMaxTravel = WIDTH(m_rcSlider)/2;
    float fAnimateTime = 100.0f + (float)FAVORITES_SLIDER_ANIMATE_TIME * iNumFrames / iMaxTravel;
    int i = GetCurrentFrame(dwStart, iNumFrames);
    int iLast;
    while(i < iNumFrames)
    {
        iLast = i;
        i = GetCurrentFrame(dwStart, iNumFrames, fAnimateTime);
        int iAmount = i-iLast;

        m_iSliderPos -= (i - iLast);

        InvalidateRect(m_hWnd, &m_rcSlider, FALSE);
        UpdateWindow(m_hWnd);
    }
}

void CDlgCallScreen::SetBattery(int iPcnt)
{
    m_iBattLevel = iPcnt / 10;
    if(m_iBattLevel > 9)
        m_iBattLevel = 9;
    else if(m_iBattLevel < 0)
        m_iBattLevel = 0;//should never happen
}

void CDlgCallScreen::SetSignal(int iPcnt)
{
    //now set our int and redraw the indicators
    m_iSignalLevel = (iPcnt)/10;
    if(m_iSignalLevel > 9)
        m_iSignalLevel = 9;
}



void CDlgCallScreen::UnlockScreen()
{
    if(m_bScreenLocked)
    {
        // hide this dlg
        m_bScreenLocked = FALSE;
        Hide();
    }
}

BOOL CDlgCallScreen::Hide()
{
    // hide this
    return ShowWindow(m_hWnd, SW_HIDE);
}

BOOL CDlgCallScreen::OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(lParam >> 16)
    {
    case VK_TTALK:
        OnAnswer();
        break;
    default:
        return UNHANDLED;
        break;
    }

    return TRUE;
}

void CDlgCallScreen::SetCallState(EnumCallState eState)
{
    m_eCallState = eState;

    InvalidateRect(m_hWnd, NULL, FALSE);//lazy .. BUGBUG
}

void CDlgCallScreen::SetTime(TCHAR* szTime)
{
    if(szTime == NULL)
        return;

    m_oStr->StringCopy(m_szTime, szTime);
}

void CDlgCallScreen::SetOID(long lOid)
{
    if(m_lOid == lOid)
        return; // maybe ...

    m_lOid = lOid; 

    if(m_lOid == -1)
    {
        m_gdiPicture.Destroy();
        LoadImages(m_hWnd, m_hInst);//reload the default
        return;
    }
    
    // update all fields - return true if any changed
    BOOL bResult = FALSE;
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return;

    IContact* pContact	= oPoom->GetContactFromOID(m_lOid);

    if(!pContact)
    {   //hackish 
        m_gdiPicture.Destroy();
        LoadImages(m_hWnd, m_hInst);//reload the default
        return;
    }

    m_sContact.Clear();
    // retrieve all the contact info as well as the potential info
    bResult = oPoom->GetContactInfo(pContact, m_sContact, TRUE, PICTURE_Width, PICTURE_Height, RGB(50,50,50));

    pContact->Release();

    if(m_sContact.gdiPicture)
    {
        int iWidth = m_sContact.gdiPicture->GetWidth();
        int iHeight= m_sContact.gdiPicture->GetHeight();
        if(m_imgMask.GetWidth() != iWidth || m_imgMask.GetHeight() != iHeight)
            m_imgMask.SetSize(iWidth, iHeight);
        if(m_imgBorder.GetWidth() != iWidth || m_imgBorder.GetHeight() != iHeight)
            m_imgBorder.SetSize(iWidth, iHeight);
        m_gdiPicture.Create(*m_sContact.gdiPicture, iWidth, iHeight, TRUE, TRUE);
        m_gdiPicture.SetAlphaMask(m_imgMask.GetImage());
        m_imgBorder.DrawImage(m_gdiPicture, 0,0);

        // we don't need it anymore
        m_sContact.gdiPicture->Destroy();
    }
    else
    {   //load in the deafault picture
        m_gdiPicture.Destroy();
        LoadImages(m_hWnd, m_hInst);//reload the default
    }


    m_gdiReflection.CreateReflection(m_gdiPicture);

    InvalidateRect(m_hWnd, &m_rcPicture, FALSE);
}

void CALLBACK CDlgCallScreen::ResultCallBack(DWORD dwCode, HRESULT hrCmdID, const void *lpData, DWORD cbData, DWORD dwParam)
{
    return;
}

void CALLBACK CDlgCallScreen::NotifyCallBack(DWORD dwCode, const void *lpData, DWORD cbData, DWORD dwParam)
{
    return;
}

