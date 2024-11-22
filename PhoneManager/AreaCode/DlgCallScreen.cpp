#include "StdAfx.h"
#include "DlgCallScreen.h"
#include "Resource.h"
#include "ContactsGuiDefines.h"
#include "CommonDefines.h"
#include "IssCommon.h"
#include "IssGDIFX.h"
#include "PoomContacts.h"
#include "tapi.h"

#define BUTTON_Height       (GetSystemMetrics(SM_CXICON)*3/2)
#define PICTURE_Width       (GetSystemMetrics(SM_CXSMICON)*6)
#define PICTURE_Height      (GetSystemMetrics(SM_CXSMICON)*9)
#define CALL_BTN_Height     (GetSystemMetrics(SM_CXSMICON)*2)

#define GLOBE_HEIGHT        (3*GetSystemMetrics(SM_CXICON))

#define IDT_CALL_TIMER      1
#define IDT_GLOBE_TIMER     2
#define IDT_TIME_TIMER      3
#define IDT_KILL_POPUP      4
#define IDT_HIDE_WINDOW     5

#define NUMBER_OF_GLOBE_FRAMES  31
#define GLOBE_ANI_RATE      33 //30 fps


CDlgCallScreen::CDlgCallScreen(void)
{
    m_gdiMem            = new CIssGDIEx;
    m_bDrawMenus        = FALSE;
    m_bDrawReflection   = TRUE;
    m_bUseCheekGuard    = FALSE;
    m_iCallTimer        = 0;
    m_iGlobeFrame       = 0;
    m_iBtnSel           = -1;//nothing for now
    m_iSliderPos        = 0;
    m_eCallState        = CSTATE_Disconnected;

    m_oStr->Empty(m_szName);
    m_oStr->Empty(m_szLocation);
    m_oStr->Empty(m_szTime);
    m_oStr->Empty(m_szPhoneNum);

    m_oStr->StringCopy(m_szSliderText, _T("Slide to unlock"));

    m_hFontStatus       = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);
    m_hFontText         = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*13/32, FW_BOLD, TRUE);
    m_hFontLocation     = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*2/5, FW_BOLD, TRUE);
    m_hFontButtons      = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*4/9, FW_BOLD, TRUE);
    m_hLabelFont        = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_BOLD, TRUE);
}

CDlgCallScreen::~CDlgCallScreen(void)
{
    delete m_gdiMem;

    CIssGDIEx::DeleteFont(m_hFontStatus);
    CIssGDIEx::DeleteFont(m_hFontText);
    CIssGDIEx::DeleteFont(m_hFontLocation);
    CIssGDIEx::DeleteFont(m_hFontButtons);

    KillAllTimers();
}

BOOL CDlgCallScreen::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
    //Show as a Pocket PC full screen dialog.
    SHINITDLGINFO DlgInfo;
    DlgInfo.dwMask	= SHIDIM_FLAGS;
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR | SHIDIF_SIPDOWN;
    DlgInfo.hDlg	= hWnd;
    SHInitDialog(&DlgInfo);
#endif

//    LoadImages(m_hWnd, m_hInst);
    InvalidateRect(m_hWnd, NULL, FALSE);//for now
    return CDlgBase::OnInitDialog(hWnd, wParam, lParam);
}

BOOL CDlgCallScreen::Show()
{
    if(m_hWnd == NULL)
        return FALSE;

    // foreground
    BOOL bResult = FALSE;
    MoveWindow(m_hWnd, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), TRUE);
    ShowWindow(m_hWnd, SW_SHOW);

    bResult = SetForegroundWindow(m_hWnd);
    if(!bResult)
    {
        DebugOut(_T("SetForegroundWindow() failed"));
    }

    return bResult;
}

BOOL CDlgCallScreen::OnPaint(HWND hWnd, HDC hDC, RECT &rcClient)
{
    if(m_gdiMem->GetDC() == NULL || WIDTH(rcClient) != m_gdiMem->GetWidth() || HEIGHT(rcClient) != m_gdiMem->GetHeight())
        m_gdiMem->Create(hDC, rcClient, FALSE, TRUE, TRUE);

    static RECT rcClip;

    GetClipBox(hDC, &rcClip);
    
    DrawBackground(m_gdiMem, rcClip);
    DrawIndicators(m_gdiMem, rcClip);
    DrawText(m_gdiMem, rcClip);
    DrawGlobe(m_gdiMem, rcClip);
    DrawPicture(m_gdiMem, rcClip);
    DrawButtons(m_gdiMem, rcClip);

    // draw it all to the screen
    BitBlt(hDC,
        rcClip.left,rcClip.top,
        WIDTH(rcClip), HEIGHT(rcClip),
        m_gdiMem->GetDC(),
        rcClip.left,rcClip.top,
        SRCCOPY);

    return TRUE;
}

BOOL CDlgCallScreen::DrawPicture(CIssGDIEx* gdi, RECT& rcClip)
{
    if(!IsRectInRect(rcClip, m_rcPicture) || m_gdiPicture.GetDC() == NULL)
        return TRUE;

    int iX, iY, iHeight;
    iX = m_rcPicture.left + (WIDTH(m_rcPicture) - m_gdiPicture.GetWidth())/ 2;
    iY = m_rcPicture.top + (HEIGHT(m_rcPicture) - m_gdiPicture.GetHeight())/2;

    iHeight = m_gdiPicture.GetHeight();

    Draw(*gdi,
        iX,iY,
        m_gdiPicture.GetWidth(), m_gdiPicture.GetHeight(),
        m_gdiPicture,
        0, 0);

    Draw(*gdi,
        iX, iY + iHeight,
        m_gdiReflection.GetWidth(), m_gdiReflection.GetHeight(),
        m_gdiReflection,
        0,0);

    return TRUE;
}

BOOL CDlgCallScreen::DrawButtons(CIssGDIEx* gdi, RECT& rcClip)
{
    if(m_eCallState == CSTATE_Connected && m_bUseCheekGuard)
    {
        DrawSlider(*gdi, rcClip, rcClip);
        return TRUE;
    }

    RECT rcTemp;
    int iIconIndex;
    //we'll just do the icon and text calculations on the fly

    if(IsRectInRect(rcClip, m_rcAnswer))
    {
        if(m_iBtnSel == 0)
        {
            m_imgBtnGreen.DrawImage(*gdi, m_rcAnswer.left, m_rcAnswer.top);
            iIconIndex = 1;
        }
        else
        {    
            m_imgBtn.DrawImage(*gdi, m_rcAnswer.left, m_rcAnswer.top);
            iIconIndex = 0;
        }
        rcTemp = m_rcAnswer;
        rcTemp.left += INDENT;
        rcTemp.right -= HEIGHT(rcTemp);
        ::DrawText(gdi->GetDC(), _T("ANSWER"), rcTemp, DT_CENTER | DT_VCENTER, m_hFontButtons, 0xFFFFFF);

        rcTemp.left = rcTemp.right;
        rcTemp.top += (CALL_BTN_Height - m_gdiIcons.GetHeight())/2;
        rcTemp.bottom = rcTemp.top + m_gdiIcons.GetHeight();
        rcTemp.right = rcTemp.left + m_gdiIcons.GetHeight();

        ::Draw(*gdi, rcTemp, m_gdiIcons, iIconIndex*m_gdiIcons.GetHeight(), 0);
    }

    if(IsRectInRect(rcClip, m_rcIgnore))
    {
        if(m_iBtnSel == 1)
        {
            m_imgBtnRed.DrawImage(*gdi, m_rcIgnore.left, m_rcIgnore.top);
            iIconIndex = 3;
        }
        else
        {
            m_imgBtn.DrawImage(*gdi, m_rcIgnore.left, m_rcIgnore.top);
            iIconIndex = 2;
        }
        rcTemp = m_rcIgnore;
        rcTemp.left += INDENT;
        rcTemp.right -= HEIGHT(rcTemp);
        ::DrawText(gdi->GetDC(), _T("IGNORE"), rcTemp, DT_CENTER | DT_VCENTER, m_hFontButtons, 0xFFFFFF);

        rcTemp.left = rcTemp.right;
        rcTemp.top += (CALL_BTN_Height - m_gdiIcons.GetHeight())/2;
        rcTemp.bottom = rcTemp.top + m_gdiIcons.GetHeight();
        rcTemp.right = rcTemp.left + m_gdiIcons.GetHeight();

        ::Draw(*gdi, rcTemp, m_gdiIcons, iIconIndex*m_gdiIcons.GetHeight(), 0);
    }

    return TRUE;
}

BOOL CDlgCallScreen::DrawGlobe(CIssGDIEx* gdi, RECT& rcClip)
{
    if(m_gdiGlobe.GetDC() == NULL || IsRectInRect(rcClip, m_rcGlobe) == FALSE || m_iGlobeFrame == -1)
        return FALSE;

#define FLAG_SIZE  (GetSystemMetrics(SM_CXICON)/4)

    Draw(*gdi,
        m_rcGlobe.left,m_rcGlobe.top,
        m_gdiGlobe.GetHeight(), m_gdiGlobe.GetHeight(),
        m_gdiGlobe,
        m_gdiGlobe.GetHeight()*m_iGlobeFrame,0);

    AreaCodeType* sArea = m_oArea.GetAreaCodeInfo();

    if(sArea && (sArea->iFrame == m_iGlobeFrame))
    {
        //draw the point
        int iX = sArea->iX*m_gdiGlobe.GetHeight()/100;
        int iY = sArea->iY*m_gdiGlobe.GetHeight()/100;

        RECT rcLoc;
        SetRect(&rcLoc, iX - FLAG_SIZE/2 + m_rcGlobe.left, iY - FLAG_SIZE/2 + m_rcGlobe.top, iX + FLAG_SIZE/2 + m_rcGlobe.left, iY + FLAG_SIZE/2 + m_rcGlobe.top);

    //    SetPixel(gdi->GetDC(), iX + m_rcGlobe.left, iY+m_rcGlobe.top, 0x00FFFF);//for now
        RoundRectangle(gdi->GetDC(), rcLoc, 0x00FFFF, 0, FLAG_SIZE, FLAG_SIZE);
    }

    return TRUE;
}

BOOL CDlgCallScreen::DrawBackground(CIssGDIEx* gdi, RECT& rcClip)
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    InitBackground(m_bDrawMenus);
    CDlgBase::DrawBackground(*m_gdiMem, rcClient, rcClip);
    if(IsRectInRect(rcClip, m_rcCallInfo))
        m_imgCallBack.DrawImage(*gdi, m_rcCallInfo.left, m_rcCallInfo.top);
    if(IsRectInRect(rcClip, m_rcYellow))
        m_imgLocation.DrawImage(*gdi, m_rcYellow.left, m_rcYellow.top);


    return TRUE;
}

BOOL CDlgCallScreen::DrawIndicators(CIssGDIEx* gdi, RECT& rcClip)
{
    RECT rcTemp = m_rcIndicators;
    rcTemp.bottom++;

    FillRect(gdi->GetDC(), rcTemp, 0x818181);
    FillRect(gdi->GetDC(), m_rcIndicators, 0);

    ::DrawText(gdi->GetDC(), _T("Rogers"), m_rcIndicators, DT_VCENTER | DT_CENTER, m_hFontLocation, 0xFFFFFF);

    return TRUE;

}

BOOL CDlgCallScreen::DrawText(CIssGDIEx* gdi, RECT& rcClip)
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
        ::DrawText(gdi->GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER, m_hFontStatus, 0);
        ::DrawText(gdi->GetDC(), szTemp, m_rcStatus, DT_LEFT | DT_VCENTER, m_hFontStatus, 0xFFFFFF);
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
        ::DrawText(gdi->GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER, m_hFontText, 0);
        ::DrawText(gdi->GetDC(), szTemp, m_rcTime, DT_LEFT | DT_VCENTER, m_hFontText, 0xFFFFFF);
    }

    m_oStr->StringCopy(szTemp, _T("Unknown"));
    if(IsRectInRect(rcClip, m_rcName))
    {
        rcTemp = m_rcName;
        OffsetRect(&rcTemp, 1, 1);
        ::DrawText(gdi->GetDC(), m_szName, rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontText, 0);
        ::DrawText(gdi->GetDC(), m_szName, m_rcName, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontText, 0xFFFFFF);
    }

    if(IsRectInRect(rcClip, m_rcPhoneNum))
    {
        rcTemp = m_rcPhoneNum;
        OffsetRect(&rcTemp, 1, 1);
        ::DrawText(gdi->GetDC(), m_szPhoneNum, rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontText, 0);
        ::DrawText(gdi->GetDC(), m_szPhoneNum, m_rcPhoneNum, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontText, 0xFFFFFF);
    }

    if(IsRectInRect(rcClip, m_rcYellow))
    {
        ::DrawText(gdi->GetDC(), m_szLocation, m_rcLocation, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontLocation, 0);
        ::DrawText(gdi->GetDC(), m_szTime, m_rcLocalTime, DT_CENTER | DT_TOP | DT_END_ELLIPSIS, m_hFontLocation, 0);
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
    switch(uiMessage)
    {
    case WM_NOTIFY_INCOMING:
        if(wParam & SN_PHONEINCOMINGCALL_BITMASK)
        {
            OnIncomingCall();
            Show();
            StartGlobeAnimation();
            SetCallerName();
            UpdateContact();
        }
        else
        {
            if(m_eCallState != CSTATE_Connected)
            {
                KillAllTimers();
                ShowWindow(m_hWnd, SW_HIDE);
            }
        }
    case WM_NOTIFY_CALLERNAME:	
        if(m_eCallState == CSTATE_Ringing)
        {
            SetCallerName();
            UpdateContact();
        }
        break;
    case WM_NOTIFY_BLOCKLISTCHANGE:
        break;
    case WM_NOTIFY_RINGRULECHANGE:
        break;
    case WM_NOTIFY_CALLOPTIONCHANGE:
        break;
    case WM_NOTIFY_OUTGOING:			
        break;
    case WM_NOTIFY_CONNECTED:
        if(wParam & SN_PHONECALLTALKING_BITMASK)
            OnCallConnected();
        else
            OnCallDisconnected();
        break;
    default:
        return UNHANDLED;
        break;
    }

    return UNHANDLED;
}

void CDlgCallScreen::OnIncomingCall()
{
    KillTimer(m_hWnd, IDT_KILL_POPUP);
    SetTimer(m_hWnd, IDT_KILL_POPUP, 50, NULL);//might be a little aggressive ... hmm
    
    InvalidateRect(m_hWnd, &m_rcStatus, FALSE);
    m_oStr->StringCopy(m_szName, _T("Unknown"));
    m_oStr->StringCopy(m_szPhoneNum, _T("Unknown"));
    m_oStr->StringCopy(m_szLocation, _T("Unknown"));
    m_oStr->StringCopy(m_szTime, _T("Unknown"));

    SetCallerNumber();
    HandleAreaCode();
    SetTime();
    m_eCallState = CSTATE_Ringing;
    InvalidateRect(m_hWnd, NULL, FALSE);
}

void CDlgCallScreen::OnCallConnected()
{
    m_eCallState = CSTATE_Connected;



    if(!m_bUseCheekGuard)
    {
        ShowWindow(m_hWnd, SW_HIDE);
        return;
    }
    else
    {
        m_iSliderPos = 0;
        SetTimer(m_hWnd, IDT_CALL_TIMER, 1000, NULL);
        Show();
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
}

void CDlgCallScreen::OnCallDisconnected()
{
    //show post call state
    m_eCallState = CSTATE_Disconnected;
    KillAllTimers();
    ShowWindow(m_hWnd, SW_HIDE);
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

    

    m_oArea.Init(hInst);
    m_oArea.LoadAreaCodes();

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
            AreaCodeType* sArea = m_oArea.GetAreaCodeInfo();

            if(sArea == NULL)
            {
                KillTimer(m_hWnd, IDT_GLOBE_TIMER);
                break;
            }

            m_iGlobeFrame++;

            if(m_iGlobeFrame >= NUMBER_OF_GLOBE_FRAMES)
                m_iGlobeFrame = 0;

            if(m_iGlobeFrame == sArea->iFrame)
            {
                KillTimer(m_hWnd, IDT_GLOBE_TIMER);
            }
            InvalidateRect(m_hWnd, &m_rcGlobe, FALSE);
            UpdateWindow(m_hWnd);//and redraw now
        }


        break;
    case IDT_CALL_TIMER:
        m_iCallTimer++;
        InvalidateRect(m_hWnd, &m_rcTime, FALSE);
        //no need to force a speedy redraw here
        break;
    case IDT_KILL_POPUP:
        {
            HWND hwndIncoming = NULL;

            hwndIncoming = FindWindow(_T("Dialog"), _T("Phone - Incoming"));
            if(hwndIncoming != NULL)
            {
                ShowWindow(hwndIncoming, SW_HIDE);
                Sleep(250);
                ShowWindow(hwndIncoming, SW_HIDE);
                MoveWindow(hwndIncoming, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, 0, FALSE);
                KillTimer(hWnd, IDT_KILL_POPUP);//BUGBUG ... should default kill this somewhere
                SetForegroundWindow((HWND)((ULONG) (m_hWnd)|0x00000001));
                SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON);
                MoveWindow(m_hWnd, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), FALSE);
           
       //         Sleep(100);

                //we'll have to do more later but this is cool for now
                //StartGlobeAnimation();
            }
        }
        break;
    case IDT_TIME_TIMER:
        SetTime();
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
        SetTimer(hWnd, IDT_CALL_TIMER, 1000, NULL);
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

BOOL CDlgCallScreen::CreateWin()
{
    if(m_hWnd)
        return TRUE;

    if(!Create(_T("Cheek Guard"), NULL, m_hInst, _T("ClassCheekGuard"), 
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, WS_VISIBLE))
        return FALSE;

    ShowWindow(m_hWnd, SW_HIDE);

    return TRUE;
}

BOOL CDlgCallScreen::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
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
                break;
            }
        }
        break;
    default:
        return UNHANDLED;
        break;
    }

    InvalidateRect(m_hWnd, NULL, FALSE);

    return TRUE;
}

BOOL CDlgCallScreen::OnAnswer()
{
    ShowWindow(m_hWnd, SW_HIDE);

    // Simulate a key press
    keybd_event( VK_TTALK,
        0x45,
        KEYEVENTF_EXTENDEDKEY | 0,
        0 );

    // Simulate a key release
    keybd_event( VK_TTALK,
        0x45,
        KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
        0);

    // first keypress activates the windows phone screen
    // try a second keypress to answer call
    HWND hWndForground = NULL;
    TCHAR szWindowText[STRING_LARGE];
    for(int i = 0; i < 10; i++)
    {
        Sleep(50);
        //DebugOut(_T("%i"), i);
        hWndForground = ::GetForegroundWindow();
        ::GetWindowText(hWndForground, szWindowText, STRING_LARGE);
        m_oStr->Trim(szWindowText);
        if(m_oStr->Compare(szWindowText, _T("Phone")) == 0)
            break;
    }
    // Simulate a key press
    keybd_event( VK_TTALK,
        0x45,
        KEYEVENTF_EXTENDEDKEY | 0,
        0 );

    // Simulate a key release
    keybd_event( VK_TTALK,
        0x45,
        KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
        0);

    return TRUE;
}

BOOL CDlgCallScreen::OnIgnore()
{
     // Simulate a key press
     keybd_event( VK_TEND,
         0x45,
         KEYEVENTF_EXTENDEDKEY | 0,
         0 );

     // Simulate a key release
     keybd_event( VK_TEND,
         0x45,
         KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
         0);

     //this is a little sloppy ... BUGBUG ... should be handled in OnUser
     m_eCallState = CSTATE_Disconnected;
     KillAllTimers();

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
    KillTimer(m_hWnd, IDT_CALL_TIMER);
    KillTimer(m_hWnd, IDT_GLOBE_TIMER);
    KillTimer(m_hWnd, IDT_TIME_TIMER);
    KillTimer(m_hWnd, IDT_KILL_POPUP);
    KillTimer(m_hWnd, IDT_HIDE_WINDOW);   


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

        KillTimer(m_hWnd, IDT_TIME_TIMER);
        SetTimer(hWnd, IDT_TIME_TIMER, 15000, NULL);
    }
    else if(LOWORD(wParam) == WA_INACTIVE)
    {
        KillTimer(m_hWnd, IDT_TIME_TIMER);

    }
    return UNHANDLED;
}

void CDlgCallScreen::SetTime()
{
    SYSTEMTIME sTime = m_oArea.GetLocalTime();

    m_oStr->Format(m_szTime, _T("Local Time: %.2i:%.2i"), sTime.wHour, sTime.wMinute);
}

void CDlgCallScreen::SetCallerName()
{
    CObjStateAndNotifyMgr::GetIncomingCallerName(m_szName, STRING_MAX);
    InvalidateRect(m_hWnd, &m_rcName, FALSE);
}

void CDlgCallScreen::SetCallerNumber()
{
    CObjStateAndNotifyMgr::GetIncomingCallerNumber(m_szPhoneNum, STRING_LARGE);
    InvalidateRect(m_hWnd, &m_rcPhoneNum, FALSE);
}

void CDlgCallScreen::HandleAreaCode()
{
    TCHAR szTemp[STRING_NORMAL];

    MakeCanonicalNum(m_szPhoneNum, szTemp, TRUE);

    m_oArea.SetAreaCode(m_oStr->StringToInt(szTemp));

    AreaCodeType* sArea = m_oArea.GetAreaCodeInfo();

    if(sArea)
    {
        m_oStr->Format(m_szLocation, _T("%s, %s"), sArea->szRegion, sArea->szCountry);
        SetTime();
    }
}

// TRUE will tell you if there is an update
BOOL CDlgCallScreen::UpdateContact()
{
    LONG lOid = -1;
    CObjStateAndNotifyMgr::GetIncomingCallerContactOID(&lOid);

    if(lOid == -1)
        return FALSE;

    // update all fields - return true if any changed
    BOOL bResult = FALSE;
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return FALSE;

    IContact* pContact	= oPoom->GetContactFromOID(lOid);

    if(!pContact)
    {   //hackish 
        m_gdiPicture.Destroy();
        LoadImages(m_hWnd, m_hInst);//reload the deafault
        return FALSE;
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
        LoadImages(m_hWnd, m_hInst);//reload the deafault
    }


    m_gdiReflection.CreateReflection(m_gdiPicture);

    return bResult;
}

void CDlgCallScreen::StartGlobeAnimation()
{
    AreaCodeType* sArea = m_oArea.GetAreaCodeInfo();

    if(sArea == NULL)
        return;

 //   m_iGlobeFrame = sArea->iFrame + 1;
 //   if(m_iGlobeFrame >= NUMBER_OF_GLOBE_FRAMES)
 //       m_iGlobeFrame = 0;//could happen

    SetTimer(m_hWnd, IDT_GLOBE_TIMER, GLOBE_ANI_RATE, NULL);
}



/********************************************************************
Function    MakeCanonicalNum

Arguments:    szPhoneNum - retrieve and set number
szAreaCode - current area code
bUseSpaces - wether spaces should be used or not

Returns:    

Comments:    Return the phone number in canonical form
*********************************************************************/
BOOL CDlgCallScreen::MakeCanonicalNum(TCHAR* szPhoneNum, TCHAR* szAreaCode, BOOL bUseSpaces)
{
    int    iLength      = 0;
    int    iStartPos    = 0;

    CIssString* oStr = CIssString::Instance();

    if(oStr->IsEmpty(szPhoneNum))
        return FALSE;

    TCHAR szCanPhoneNum[TAPIMAXDESTADDRESSSIZE + 1];

    for(int i = 0; i < oStr->GetLength(szPhoneNum); i++)
    {
        if (iswdigit (szPhoneNum[i]))
        {
            szPhoneNum[iLength] = szPhoneNum[i];
            iLength += 1;
        }
    }

    // Terminate the string with NULL.
    szPhoneNum[iLength] = _T('\0');

    if (iLength == 10)
    {
        //Make the phone number in the format "+1 (xxx) xxx-xxxx"
        oStr->StringCopy(szCanPhoneNum, _T("+1 "));
        szCanPhoneNum[3]  = '(';
        szCanPhoneNum[4]  = szPhoneNum[iStartPos];
        szCanPhoneNum[5]  = szPhoneNum[iStartPos + 1];
        szCanPhoneNum[6]  = szPhoneNum[iStartPos + 2];
        szCanPhoneNum[7]  = _T(')');
        szCanPhoneNum[8]  = _T(' ');
        szCanPhoneNum[9]  = szPhoneNum[iStartPos + 3];
        szCanPhoneNum[10] = szPhoneNum[iStartPos + 4];
        szCanPhoneNum[11] = szPhoneNum[iStartPos + 5];
        szCanPhoneNum[12] = _T('-');
        szCanPhoneNum[13] = szPhoneNum[iStartPos + 6];
        szCanPhoneNum[14] = szPhoneNum[iStartPos + 7];
        szCanPhoneNum[15] = szPhoneNum[iStartPos + 8];
        szCanPhoneNum[16] = szPhoneNum[iStartPos + 9];
        szCanPhoneNum[17] = _T('\0');

        if(szAreaCode != NULL)
        {
            oStr->Empty(szAreaCode);
            szAreaCode[0] = szPhoneNum[iStartPos];
            szAreaCode[1] = szPhoneNum[iStartPos + 1];
            szAreaCode[2] = szPhoneNum[iStartPos + 2];
            szAreaCode[3] = _T('\0');
        }
    }
    else if(iLength == 11)
    {
        if (szPhoneNum[0] != '1') //if its 11 digits it better start with a 1 for now ... 
            //could also be xx xx xxx xxxx but we'll worry about that later
            return FALSE;      
        else
        {
            //Make the phone number in the format "+1 (xxx) xxx-xxxx"
            szCanPhoneNum[0]  = _T('+');
            szCanPhoneNum[1]  = szPhoneNum[iStartPos];
            szCanPhoneNum[2]  = _T(' ');
            szCanPhoneNum[3]  = _T('(');
            szCanPhoneNum[4]  = szPhoneNum[iStartPos + 1];
            szCanPhoneNum[5]  = szPhoneNum[iStartPos + 2];
            szCanPhoneNum[6]  = szPhoneNum[iStartPos + 3];
            szCanPhoneNum[7]  = _T(')');
            szCanPhoneNum[8]  = _T(' ');
            szCanPhoneNum[9]  = szPhoneNum[iStartPos + 4];
            szCanPhoneNum[10] = szPhoneNum[iStartPos + 5];
            szCanPhoneNum[11] = szPhoneNum[iStartPos + 6];
            szCanPhoneNum[12] = _T('-');
            szCanPhoneNum[13] = szPhoneNum[iStartPos + 7];
            szCanPhoneNum[14] = szPhoneNum[iStartPos + 8];
            szCanPhoneNum[15] = szPhoneNum[iStartPos + 9];
            szCanPhoneNum[16] = szPhoneNum[iStartPos + 10];
            szCanPhoneNum[17] = _T('\0');

            if(szAreaCode != NULL)
            {
                oStr->Empty(szAreaCode);
                szAreaCode[0] = szPhoneNum[iStartPos + 1];
                szAreaCode[1] = szPhoneNum[iStartPos + 2];
                szAreaCode[2] = szPhoneNum[iStartPos + 3];
                szAreaCode[3] = _T('\0');
            }
        }
    }
    else
    {
        return FALSE;
    }

    // Copy the newly created phone number back to lpszPhoneNum.
    oStr->StringCopy(szPhoneNum, szCanPhoneNum);

    return TRUE;
}

void CDlgCallScreen::DrawSlider(CIssGDIEx& gdi, RECT& rc, RECT& rcClip)
{
 //   if(!IsRectInRect(rcClip, m_rcSlider))
 //     return;

    GetSliderTrack().DrawImage(gdi, m_rcSlider.left, m_rcSlider.top);

    ::DrawText(gdi.GetDC(), m_szSliderText, m_rcSlider, DT_VCENTER | DT_CENTER, m_hFontText, 0xEEEEEE);

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