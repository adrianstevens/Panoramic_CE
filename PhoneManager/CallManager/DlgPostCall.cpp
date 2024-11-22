#include "StdAfx.h"
#include "DlgPostCall.h"
#include "IssDebug.h"
#include "Resource.h"
#include "ObjStateAndNotifyMgr.h"
#include "IssGDIFX.h"
#include "ContactsGuiDefines.h"
#include "ObjSkinEngine.h"
#include "PoomContacts.h"
#include "phone.h"
#include "sms.h"


#define BUTTON_Height       (GetSystemMetrics(SM_CXICON)*7/8)
#define PICTURE_Width       (GetSystemMetrics(SM_CXSMICON)*7/2)
#define PICTURE_Height      (GetSystemMetrics(SM_CXSMICON)*7/2)
#define CALL_BTN_Height     (GetSystemMetrics(SM_CXSMICON)*2)


//#define BTN_Height          (IsVGA()?80:40)//nah!
#define BTN_Width           (GetSystemMetrics(SM_CXSCREEN)-INDENT*4)
#define IDMENU_CallBack     WM_USER + 100
#define IDMENU_SMS          WM_USER + 101
#define IDMENU_AddContacts  WM_USER + 102
#define IDMENU_SMS_Busy     WM_USER + 200
#define IDMENU_SMS_BusyYou  WM_USER + 201
#define IDMENU_SMS_Meeting  WM_USER + 202
#define IDMENU_SMS_Car      WM_USER + 203
#define IDMENU_SMS_Custom   WM_USER + 204


#define IDT_TIME_HIDE       1
#define IDT_SHOW            2

CDlgPostCall::CDlgPostCall(void)
{
    m_oStr->Empty(m_szName);
    m_oStr->Empty(m_szTime);
    m_oStr->Empty(m_szPhoneNum);
    m_oStr->Empty(m_szCarrier);
    m_oStr->Empty(m_szPhoneType);


    m_oStr->StringCopy(m_szName, _T("Unknown"));
#ifdef DEBUG
    m_oStr->StringCopy(m_szPhoneNum, _T("+64-9-402-6220"));
    m_oStr->StringCopy(m_szTime, _T("duration: 12:06"));

#endif

    m_oStr->StringCopy(m_szSliderText, _T("Slide to Block Caller"));

    m_hFontStatus       = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*17/32, FW_BOLD, TRUE);
    m_hFontText         = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*13/32, FW_BOLD, TRUE);
    m_hFontLocation     = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*2/5, FW_BOLD, TRUE);
    m_hFontButtons      = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*4/9, FW_BOLD, TRUE);
    m_hLabelFont        = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_BOLD, TRUE);
    
    m_iSliderPos        = 0;
}

CDlgPostCall::~CDlgPostCall(void)
{
    CIssGDIEx::DeleteFont(m_hFontStatus);
    CIssGDIEx::DeleteFont(m_hFontText);
    CIssGDIEx::DeleteFont(m_hFontLocation);
    CIssGDIEx::DeleteFont(m_hFontButtons);
    CIssGDIEx::DeleteFont(m_hLabelFont);

    m_oMenu.ResetContent();
    m_wndMenu.ResetContent();
}

BOOL CDlgPostCall::Initialize(PostCallType* sType)
{
    //set the indexes for our meters
    OnBattery();
    OnSignal();
    SetCarrier();

    m_iSliderPos = 0;

    if(sType->szName)
        m_oStr->StringCopy(m_szName, sType->szName);
    if(sType->szPhoneNum)
        m_oStr->StringCopy(m_szPhoneNum, sType->szPhoneNum);
    if(sType->szType)
        m_oStr->StringCopy(m_szPhoneType, sType->szType);
    else
        m_oStr->Empty(m_szPhoneType);
    if(sType->szLocation)
        m_oStr->StringCopy(m_szLocation, sType->szLocation);
    else
        m_oStr->Empty(m_szLocation);

    m_sCallStartTime = sType->sStartTime;
    m_objBlockList   = sType->objBlockList;
    m_eCallType      = sType->eCallType;
    m_eAllow         = sType->eAllow;
    
    int iHour = sType->iDurration/3600;
    int iMin = (sType->iDurration - iHour*3600)/60;
    int iSec = sType->iDurration - iHour*3600 - iMin*60;

    m_iCallDurration = sType->iDurration;

    if(iHour > 0)
        m_oStr->Format(m_szTime, _T("%.2i:%.2i:%.2i"), iHour, iMin, iSec);
    else
        m_oStr->Format(m_szTime, _T("%.2i:%.2i"), iMin, iSec);

    m_lOid = sType->lOid;

    m_bShowSMSList = ShowSMSList();
    m_bBlocked = FALSE;
    m_oStr->StringCopy(m_szSliderText, _T("Slide to Block Caller"));

    switch(m_eCallType)
    {
    case CTYPE_IncomingIngored:
      //  m_oStr->StringCopy(m_szCall, _T("Call Ignored"));
        switch(m_eAllow)
        {
        case ACALL_Allow:
            m_oStr->StringCopy(m_szCall, _T("Call Ignored"));
            break;
        case ACALL_Block:
            m_oStr->StringCopy(m_szCall, _T("Call Blocked"));
            break;
        case ACALL_Private:
            m_oStr->StringCopy(m_szCall, _T("Privacy Rule"));
            break;
        default:
            break;
        }
        break;
    case CTYPE_IncomingMissed:
        m_oStr->StringCopy(m_szCall, _T("Missed Call"));
        break;
    case CTYPE_IncomingAnswered:
    case CTYPE_OutgoingAnswered:
        m_oStr->StringCopy(m_szCall, _T("Call Ended"));
        break;
    case CTYPE_OutgoingDropped:
        m_oStr->StringCopy(m_szCall, _T("Call Incomplete"));
        break;
    default:
        break;
    }

    if(m_lOid == -1)//try and load the contact picture
    {
        m_gdiPicture.Destroy();
        LoadImages(m_hWnd, m_hInst);//reload the default
        return TRUE;
    }

    // update all fields - return true if any changed
    BOOL bResult = FALSE;
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
    {
        m_gdiPicture.Destroy();
        LoadImages(m_hWnd, m_hInst);//reload the default
        return FALSE;
    }

    IContact* pContact	= oPoom->GetContactFromOID(m_lOid);

    if(!pContact)
    {   //hackish 
        m_gdiPicture.Destroy();
        LoadImages(m_hWnd, m_hInst);//reload the default
        return FALSE;
    }

    m_sContact.Clear();

    // retrieve all the contact info as well as the potential info
    bResult = oPoom->GetContactInfo(pContact, m_sContact, FALSE, PICTURE_Width, PICTURE_Height, COLOR_TEXT_NORMAL);

    m_sContact.gdiPicture = oPoom->GetUserPicture(pContact, PICTURE_Width, PICTURE_Height, COLOR_TEXT_NORMAL, TRUE);

    pContact->Release();

    if(m_sContact.gdiPicture)
    {
        int iWidth = PICTURE_Width;
        int iHeight= PICTURE_Height;
        if(m_imgMask.GetWidth() != iWidth || m_imgMask.GetHeight() != iHeight)
            m_imgMask.SetSize(iWidth, iHeight);
        if(m_imgBorder.GetWidth() != iWidth || m_imgBorder.GetHeight() != iHeight)
            m_imgBorder.SetSize(iWidth, iHeight);
        m_gdiPicture.Create(*m_sContact.gdiPicture, iWidth, iHeight, FALSE, TRUE);
        RECT rc;
        SetRect(&rc, 0,0,iWidth, iHeight);
        FillRect(m_gdiPicture, rc, RGB(0,0,0));
        // draw it centered
        BitBlt(m_gdiPicture,
            0,0,
            iWidth, iHeight,
            *m_sContact.gdiPicture,
            (m_sContact.gdiPicture->GetWidth()-iWidth)/2, (m_sContact.gdiPicture->GetHeight()-iHeight)/2,
            SRCCOPY);
        m_gdiPicture.SetAlphaMask(m_imgMask.GetImage());
        m_imgBorder.DrawImage(m_gdiPicture, 0,0, ALPHA_AddValue);

        // we don't need it anymore
        delete m_sContact.gdiPicture;
        m_sContact.gdiPicture = NULL;
    }
    else
    {   //load in the default picture
        m_gdiPicture.Destroy();
        LoadImages(m_hWnd, m_hInst);
    }

    return bResult;
}

BOOL CDlgPostCall::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
    //Show as a Pocket PC full screen dialog.
    /*SHINITDLGINFO DlgInfo;
    DlgInfo.dwMask	= SHIDIM_FLAGS;
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR | SHIDIF_SIPDOWN;
    DlgInfo.hDlg	= hWnd;
    SHInitDialog(&DlgInfo);*/
#endif

    OnActivate(hWnd, WA_ACTIVE, 0);

    //set the indexes for our meters
    OnBattery();
    OnSignal();
    SetCarrier();

    InvalidateRect(m_hWnd, NULL, FALSE);//for now
    return CDlgBase::OnInitDialog(hWnd, wParam, lParam);
}


void CDlgPostCall::OnBattery()
{
    /*  int iTemp;
    if(CObjStateAndNotifyMgr::GetBatteryLevel(&iTemp))
    {
    //now set our int and redraw the indicators
    m_iBattLevel = (iTemp)/10;
    if(m_iBattLevel > 9)
    m_iBattLevel = 9;
    InvalidateRect(m_hWnd, &m_rcIndicators, FALSE);
    }*/

    SYSTEM_POWER_STATUS_EX2 sNewBattery = {0};

    sNewBattery.BatteryFlag			= BATTERY_FLAG_UNKNOWN;
    sNewBattery.BatteryLifePercent	= 10;

    if(!GetSystemPowerStatusEx2(&sNewBattery, sizeof(SYSTEM_POWER_STATUS_EX2), TRUE))
        return;

    m_iBattLevel = sNewBattery.BatteryLifePercent / 10;
    if(m_iBattLevel > 9)
        m_iBattLevel = 9;
    else if(m_iBattLevel < 0)
        m_iBattLevel = 0;//should never happen
}

void CDlgPostCall::OnSignal()
{
    int iTemp;
    if(CObjStateAndNotifyMgr::GetSignalStrength(&iTemp))
    {
        //now set our int and redraw the indicators
        m_iSignalLevel = (iTemp)/10;
        if(m_iSignalLevel > 9)
            m_iSignalLevel = 9;
        InvalidateRect(m_hWnd, &m_rcIndicators, FALSE);
    }
}


BOOL CDlgPostCall::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    DrawPicture(gdi, rcClip);   
    DrawBackground(gdi, rcClip);
    DrawIndicators(gdi, rcClip);
    DrawText(gdi, rcClip);
    DrawButtons(gdi, rcClip);
    DrawBottomBar(gdi, rcClient, rcClip);

    if(ShowCallBack())
        m_oMenu.OnDraw(gdi, rcClient, rcClip);

    return TRUE;
}


BOOL CDlgPostCall::DrawPicture(CIssGDIEx& gdi, RECT& rcClip)
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

    return TRUE;
}

BOOL CDlgPostCall::DrawButtons(CIssGDIEx& gdi, RECT& rcClip)
{
    if(IsRectInRect(rcClip, GetSliderRect()) &&  ShowCallBack())
        DrawSlider(gdi, rcClip, rcClip);
    if(IsRectInRect(rcClip, m_rcInfoBtn))
    {
        m_imgInfoBtn.DrawImage(gdi, m_rcInfoBtn.left, m_rcInfoBtn.top);
   
        RECT rcTemp = m_rcInfoBtn;
        OffsetRect(&rcTemp, 1, 1);
        ::DrawText(gdi.GetDC(), _T("More Info"), rcTemp, DT_CENTER | DT_VCENTER, m_hFontButtons, 0);
        ::DrawText(gdi.GetDC(), _T("More Info"), m_rcInfoBtn, DT_CENTER | DT_VCENTER, m_hFontButtons, 0xFFFFFF);
    }
    return TRUE;
}


BOOL CDlgPostCall::DrawBackground(CIssGDIEx& gdi, RECT& rcClip)
{
    if(IsRectInRect(rcClip, m_rcShadow))
        m_imgCallBack.DrawImage(gdi, m_rcShadow.left, m_rcShadow.top);

    return TRUE;
}

BOOL CDlgPostCall::DrawIndicators(CIssGDIEx& gdi, RECT& rcClip)
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

    return TRUE;
}


BOOL CDlgPostCall::DrawText(CIssGDIEx& gdi, RECT& rcClip)
{
    static TCHAR szTemp[STRING_LARGE];
    static RECT rcTemp;

    if(IsRectInRect(rcClip, m_rcStatus))
    {
        rcTemp = m_rcStatus;
        OffsetRect(&rcTemp, 1, 1);
        ::DrawText(gdi.GetDC(), m_szCall, rcTemp, DT_LEFT | DT_VCENTER, m_hFontStatus, 0);
        ::DrawText(gdi.GetDC(), m_szCall, m_rcStatus, DT_LEFT | DT_VCENTER, m_hFontStatus, 0xFFFFFF);

        rcTemp = m_rcStatus;

        if(m_eCallType == CTYPE_IncomingAnswered || m_eCallType == CTYPE_OutgoingAnswered)
        {
            OffsetRect(&rcTemp, 1, 1);
            ::DrawText(gdi.GetDC(), m_szTime, rcTemp, DT_RIGHT | DT_VCENTER, m_hFontText, 0);
            ::DrawText(gdi.GetDC(), m_szTime, m_rcStatus, DT_RIGHT | DT_VCENTER, m_hFontText, 0xFFFFFF);
        }
    }

    if(IsRectInRect(rcClip, m_rcName))
    {
        rcTemp = m_rcName;
        OffsetRect(&rcTemp, 1, 1);
        ::DrawText(gdi.GetDC(), m_szName, rcTemp, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS, m_hFontStatus, 0);
        ::DrawText(gdi.GetDC(), m_szName, m_rcName, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS, m_hFontStatus, 0xFFFFFF);
    }

    if(IsRectInRect(rcClip, m_rcPhoneNum))
    {
        if(m_oStr->IsEmpty(m_szPhoneType))
            m_oStr->StringCopy(szTemp, m_szPhoneNum);
        else
            m_oStr->Format(szTemp, _T("%s %s"), m_szPhoneNum, m_szPhoneType);

        rcTemp = m_rcPhoneNum;
        OffsetRect(&rcTemp, 1, 1);
        ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS, m_hFontStatus, 0);
        ::DrawText(gdi.GetDC(), szTemp, m_rcPhoneNum, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS, m_hFontStatus, 0xFFFFFF);
    }

    return TRUE;
}

void CDlgPostCall::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rcTemp = m_rcBottomBar;
    rcTemp.right -= WIDTH(m_rcBottomBar)/2;

    OffsetRect(&rcTemp, 1, 1);
    ::DrawText(gdi.GetDC(), _T("Quit"), rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontButtons, 0);
    OffsetRect(&rcTemp, -1, -1);
    ::DrawText(gdi.GetDC(), _T("Quit"), rcTemp, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, m_hFontButtons, 0xFFFFFF);

}

void CDlgPostCall::OnMenuLeft()
{
    Hide();//should be fine ...
}
    
BOOL CDlgPostCall::Show(BOOL bAgressive)
{
    if(m_hWnd == NULL)
        return FALSE;

    m_dlgMore.CloseWindow();

    KillTimer(m_hWnd, IDT_TIME_HIDE);

    // foreground
    BOOL bResult = FALSE;
    MoveWindow(m_hWnd, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), TRUE);
    ShowWindow(m_hWnd, SW_SHOW);

    bResult = SetForegroundWindow(m_hWnd);
    if(!bResult)
    {
        DebugOut(_T("SetForegroundWindow() failed"));
    }

    if(bResult)
        PopulateContent(-1);

    SetTimeOut();

    if(bAgressive)
    {   //I feel dirty ... 
        SetTimer(m_hWnd, IDT_SHOW, 350, NULL);
        m_iForgroundCount = 0;
    }

    return bResult;
}

void CDlgPostCall::SetTimeOut()
{
    KillTimer(m_hWnd, IDT_TIME_HIDE);
    SetTimer(m_hWnd, IDT_TIME_HIDE, 20000, NULL);//20
}

BOOL CDlgPostCall::Hide()
{
    // hide this
    KillTimer(m_hWnd, IDT_SHOW);
    if(m_wndMenu.IsWindowUp(TRUE))
        m_wndMenu.CloseWindow();
    return ShowWindow(m_hWnd, SW_HIDE);
}


BOOL CDlgPostCall::CreateWin(HWND hWndParent)
{
    if(m_hWnd)
        return TRUE;

    if(!Create(_T("PostCall"), hWndParent, m_hInst, _T("ClassPostCall"), 
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), WS_EX_TOPMOST, WS_VISIBLE))
        return FALSE;

    ShowWindow(m_hWnd, SW_HIDE);

    return TRUE;
}

HRESULT CDlgPostCall::LoadImages(HWND hWnd, HINSTANCE hInst)
{
    HRESULT hr = S_OK;

    m_oMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu.PreloadImages(hWnd, hInst);

    if(!m_imgCallBack.IsLoaded())
        hr = m_imgCallBack.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_BKVGA:IDR_PNG_CALL_BK);

    if(!m_imgMask.IsLoaded())
        m_imgMask.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ImgAlphaVGA:IDR_PNG_CALL_ImgAlpha);

    if(!m_imgBorder.IsLoaded())
        m_imgBorder.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ImgBorderVGA:IDR_PNG_CALL_ImgBorder);

    if(!m_imgInfoBtn.IsLoaded())
        m_imgInfoBtn.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ButtonVGA:IDR_PNG_CALL_Button);

    if(!m_imgBtn.IsLoaded())
        m_imgBtn.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ButtonVGA:IDR_PNG_CALL_Button);

    if(!m_imgBtnGrn.IsLoaded())
        m_imgBtnGrn.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ButtonGreenVGA:IDR_PNG_CALL_ButtonGreen);

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
    }

    //menu bitches
    m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_wndMenu.SetBackground(IDR_PNG_Group);
    m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_wndMenu.PreloadImages(hWnd, hInst);

    DebugOut(_T("Images Loaded"));

    m_hInst = hInst;

    return hr;
}

BOOL CDlgPostCall::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
    m_rcStatus.left     = rc.left + iIndent;
    m_rcStatus.right    = rc.right - iIndent;
    m_rcStatus.top      = m_rcIndicators.bottom + INDENT;
    m_rcStatus.bottom   = m_rcStatus.top + GetSystemMetrics(SM_CXSMICON);

    m_rcShadow.left     = rc.left + INDENT;
    m_rcShadow.right    = rc.right - INDENT;
    m_rcShadow.top      = m_rcStatus.bottom + INDENT/2;
    m_rcShadow.bottom   = m_rcShadow.top + GetSystemMetrics(SM_CXSMICON)*9/2;//maybe??
    

    m_rcPicture.left	= iIndent + m_rcShadow.left;
    m_rcPicture.top		= iIndent + m_rcShadow.top;
    m_rcPicture.right	= PICTURE_Width + m_rcPicture.left;
    m_rcPicture.bottom	= PICTURE_Height + m_rcPicture.top;

    m_rcName.left     = m_rcPicture.right + INDENT;
    m_rcName.top      = m_rcPicture.top;
    m_rcName.right    = rc.right - iIndent;
    m_rcName.bottom   = m_rcName.top + GetSystemMetrics(SM_CXSMICON);

    m_rcPhoneNum.left       = m_rcPicture.right + INDENT;
    m_rcPhoneNum.top        = m_rcName.bottom;
    m_rcPhoneNum.right      = rc.right - iIndent;
    m_rcPhoneNum.bottom     = m_rcPhoneNum.top + GetSystemMetrics(SM_CXSMICON);

    m_rcInfoBtn.right       = m_rcShadow.right - INDENT;
    m_rcInfoBtn.bottom      = m_rcShadow.bottom - INDENT;
    m_rcInfoBtn.left        = m_rcInfoBtn.right - GetSystemMetrics(SM_CXSMICON)*5;
    m_rcInfoBtn.top         = m_rcInfoBtn.bottom - BUTTON_Height;

    m_rcGreenBar.top        = m_rcShadow.bottom;
    m_rcGreenBar.left       = rc.left + iIndent;
    m_rcGreenBar.right      = rc.right - iIndent;
    m_rcGreenBar.bottom     = m_rcGreenBar.top + GetSystemMetrics(SM_CXSMICON);
   
    m_rcSlider.left     = iIndent;
    m_rcSlider.right    = rc.right - iIndent;
    m_rcSlider.bottom   = m_rcArea.bottom - iIndent;
    m_rcSlider.top      = m_rcSlider.bottom - CALL_BTN_Height;

    m_imgCallBack.SetSize(WIDTH(m_rcShadow), HEIGHT(m_rcShadow));
    m_imgInfoBtn.SetSize(WIDTH(m_rcInfoBtn), HEIGHT(m_rcInfoBtn));
//    m_imgInfoGrn.SetSize(WIDTH(m_rcInfoBtn), HEIGHT(m_rcInfoBtn));

    m_oMenu.OnSize((GetSystemMetrics(SM_CXSCREEN) - BTN_Width)/ 2, 
        m_rcGreenBar.top,
        BTN_Width, m_rcSlider.top - iIndent - m_rcGreenBar.top);





    return TRUE;
}

BOOL CDlgPostCall::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case _T('a'):
    case _T('A'):
        OnActivate(hWnd, WA_ACTIVE, 0);
        break;
    case _T('g'):
    case _T('G'):
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

BOOL CDlgPostCall::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    SetTimeOut();
    return m_oMenu.OnKeyDown(wParam, lParam);
}

BOOL CDlgPostCall::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return CDlgBase::OnKeyUp(hWnd, wParam, lParam);
}

BOOL CDlgPostCall::OnLButtonUp(HWND hWnd, POINT& pt)
{
    SetTimeOut();
    if(m_wndMenu.IsWindowUp(TRUE))
        return UNHANDLED;
    if(m_oMenu.OnLButtonUp(pt))
        return TRUE;
    else if(OnSliderButtonUp(pt))
        return TRUE;
    else if(PtInRect(&m_rcInfoBtn, pt))
        OnMoreInfo();

    return CDlgBase::OnLButtonUp(hWnd, pt);
}

BOOL CDlgPostCall::OnLButtonDown(HWND hWnd, POINT& pt)
{
    SetTimeOut();
    if(m_oMenu.OnLButtonDown(pt))
        return TRUE;
    else if(OnSliderButtonDown(pt))
        return TRUE;

    return  CDlgBase::OnLButtonDown(hWnd, pt);
}

BOOL CDlgPostCall::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_oMenu.OnMouseMove(pt))
        return TRUE;

    if(m_bSliderDrag)
    {
        int iPos = pt.x - m_iSliderStartX;
        int iMax = WIDTH(m_rcSlider) - 2*HEIGHT(m_rcSlider);
        int iMin = 0;

        if(m_bBlocked)
        {
        //    iMax = WIDTH(m_rcSlider);
        //    iMin = 2*HEIGHT(m_rcSlider);
            iPos = WIDTH(m_rcSlider) + pt.x - m_iSliderStartX - m_imgSlider.GetWidth();
        }

        if(iPos < iMin)
            iPos = iMin;
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

BOOL CDlgPostCall::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDT_TIME_HIDE)
    {
        KillTimer(hWnd, IDT_TIME_HIDE);
        Hide();//done!
        return TRUE;
    }
    else if(LOWORD(wParam) == IDT_SHOW)
    {
        m_iForgroundCount++;

        SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON);

        if(m_iForgroundCount > 3)
            KillTimer(hWnd, IDT_SHOW);
    }
    return m_oMenu.OnTimer(wParam, lParam);
}

BOOL CDlgPostCall::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case IDOK:
        m_oMenu.OnKeyDown(VK_RETURN, 0);
        break;
    case IDMENU_CallBack:
        OnCallBack();
        break;
    case IDMENU_SMS:
        OnSMS();
        break;
    case IDMENU_AddContacts:
        OnAddToContacts();
        break;
    case IDMENU_SMS_Busy:  
        SendSMSMessage(_T("I'm busy, I'll call you later."));
        break;
    case IDMENU_SMS_BusyYou:
        SendSMSMessage(_T("I'm busy, try calling later."));
        break;
    case IDMENU_SMS_Meeting:
        SendSMSMessage(_T("I'm in a meeting."));
        break;
    case IDMENU_SMS_Car:
        SendSMSMessage(_T("I'm in the car."));
        break;
    case IDMENU_SMS_Custom:
        SendSMSMessage(_T(""));
        break;
    default:
        return UNHANDLED;
    }
    return TRUE;
}

BOOL CDlgPostCall::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
    return UNHANDLED;
}

BOOL CDlgPostCall::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{

    return UNHANDLED;
}

void CDlgPostCall::SetCallerName()
{
    m_oStr->Empty(m_szName);
    CObjStateAndNotifyMgr::GetIncomingCallerName(m_szName, STRING_MAX);
    InvalidateRect(m_hWnd, &m_rcName, FALSE);
}

void CDlgPostCall::SetCallerNumber()
{
    CObjStateAndNotifyMgr::GetIncomingCallerNumber(m_szPhoneNum, STRING_LARGE);
    InvalidateRect(m_hWnd, &m_rcPhoneNum, FALSE);
}

void CDlgPostCall::SetCallType()
{
    m_oStr->Empty(m_szPhoneType);
    CObjStateAndNotifyMgr::GetPhoneType(m_szPhoneType, STRING_SMALL);
    InvalidateRect(m_hWnd, &m_rcPhoneNum, FALSE);
}

void CDlgPostCall::SetCarrier()
{
    CObjStateAndNotifyMgr::GetPhoneOperator(m_szCarrier, STRING_LARGE);
    InvalidateRect(m_hWnd, &m_rcIndicators, FALSE);
}


void CDlgPostCall::DrawSlider(CIssGDIEx& gdi, RECT& rc, RECT& rcClip)
{
    //   if(!IsRectInRect(rcClip, m_rcSlider))
    //     return;

    GetSliderTrack().DrawImage(gdi, m_rcSlider.left, m_rcSlider.top);

    DWORD dwArg = DT_VCENTER | DT_RIGHT;
    if(m_bBlocked)
        dwArg = DT_VCENTER | DT_LEFT;

    RECT rcTemp = m_rcSlider;
    rcTemp.right -= INDENT*4;
    rcTemp.left += INDENT*4;
    rcTemp.top   += INDENT;
    ::DrawText(gdi.GetDC(), m_szSliderText, rcTemp, dwArg, m_hFontButtons, 0);
    rcTemp.right--;
    rcTemp.top -= 2;
    ::DrawText(gdi.GetDC(), m_szSliderText, rcTemp, dwArg, m_hFontButtons, 0x888888);

    DrawSliderKnob(gdi, rc);
}

RECT CDlgPostCall::GetSliderRect()
{
    RECT rc;

    int iSliderWidth = m_imgSlider.GetWidth();

    SetRect(&rc, m_rcSlider.left + m_iSliderPos, 
        m_rcSlider.top, 
        m_rcSlider.left + m_iSliderPos + iSliderWidth,
        m_rcSlider.bottom);

    return rc;
}

void CDlgPostCall::DrawSliderKnob(CIssGDIEx& gdi, RECT& rc)
{
    //1) slider image
    RECT rcSlider = GetSliderRect();
    GetSliderImg().DrawImage(gdi, rcSlider.left, rcSlider.top);

    // 3) arrow
    DrawSliderArrow(gdi);
}

void CDlgPostCall::DrawSliderArrow(CIssGDIEx& gdi)
{
    RECT rcSlider = GetSliderRect();

    int iArrowIndex = 2;

    if(m_bBlocked)
        iArrowIndex = 3;

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

CIssImageSliced& CDlgPostCall::GetSliderTrack()
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

CIssImageSliced& CDlgPostCall::GetSliderImg()
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

BOOL CDlgPostCall::OnSliderButtonDown(POINT pt)
{
    if(PtInRect(&GetSliderRect(), pt))
    {
        m_bSliderDrag = TRUE;
        m_iSliderPos = 0;
        if(m_bBlocked)
            m_iSliderPos = WIDTH(m_rcSlider) - m_imgSlider.GetWidth();
        m_iSliderStartX = pt.x;
    }

    return UNHANDLED;
}

BOOL CDlgPostCall::OnSliderButtonUp(POINT pt)
{
    if(!m_bSliderDrag)
        return FALSE;

    if(m_bBlocked == FALSE && GetSliderRect().right > m_rcSlider.right - GetSystemMetrics(SM_CXSMICON)/4)
    {
         OnBlock();
        return TRUE;
    }
    else if(m_bBlocked && GetSliderRect().left < m_rcSlider.left + GetSystemMetrics(SM_CXSMICON)/4)
    {
        OnUnblock();
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

void CDlgPostCall::AnimateSlider(POINT pt)
{
    DWORD dwStart = GetTickCount();
    int iNumFrames = m_iSliderPos;
    if(m_bBlocked)
        iNumFrames = WIDTH(m_rcSlider) - m_iSliderPos - m_imgSlider.GetWidth();//I hope ...

    int iMaxTravel = WIDTH(m_rcSlider)/2;
    float fAnimateTime = 100.0f + (float)FAVORITES_SLIDER_ANIMATE_TIME * iNumFrames / iMaxTravel;
    int i = GetCurrentFrame(dwStart, iNumFrames);
    int iLast;
    while(i < iNumFrames)
    {
        iLast = i;
        i = GetCurrentFrame(dwStart, iNumFrames, fAnimateTime);
        int iAmount = i-iLast;

        if(m_bBlocked)
            m_iSliderPos += (i - iLast);//maybe
        else
            m_iSliderPos -= (i - iLast);

        InvalidateRect(m_hWnd, &m_rcSlider, FALSE);
        UpdateWindow(m_hWnd);
    }
}


void CDlgPostCall::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgPostCall* pThis = (CDlgPostCall*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumPostCall* eOption = (EnumPostCall*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eOption);
}

BOOL CDlgPostCall::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumPostCall* eOption)
{
    RECT rcTemp = rc;
    rcTemp.left += GetSystemMetrics(SM_CXICON);
    rcTemp.right -= GetSystemMetrics(SM_CXICON);

    DrawButton(gdi, bIsHighlighted, rcTemp);

  /*  Draw(gdi,
        rc.right - m_gdiForwardArrow.GetWidth() - 2*INDENT, rc.top + (HEIGHT(rc)-m_gdiForwardArrow.GetHeight())/2,
        m_gdiForwardArrow.GetWidth(), m_gdiForwardArrow.GetHeight(),
        m_gdiForwardArrow,
        0,0);*/

    TCHAR szTitle[STRING_MAX] = _T("");
    switch(*eOption)
    {
    case POSTCALL_CallBack:
        m_oStr->StringCopy(szTitle, _T("Call Back"));
        break;
    case POSTCALL_AddToContacts:
        m_oStr->StringCopy(szTitle, _T("Add to Contacts"));
        break;
    case POSTCALL_SMS:
        m_oStr->StringCopy(szTitle, _T("SMS Response"));
        break;
    default:
        return TRUE;
        break;
    }

    RECT rcDraw = rcTemp;
    rcDraw.left += GetSystemMetrics(SM_CXSMICON)/2;
    rcDraw.top += INDENT;

    DrawTextShadow(gdi, szTitle, rcDraw, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS, m_hFontButtons, RGB(255,255,255), RGB(0,0,0));
    return TRUE;
}

void CDlgPostCall::DrawButton(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc)
{
    if(bIsHighlighted)
    {
        if(WIDTH(rc) != m_imgBtnGrn.GetWidth() || HEIGHT(rc) != m_imgBtnGrn.GetHeight())
            m_imgBtnGrn.SetSize(WIDTH(rc), HEIGHT(rc));

        m_imgBtnGrn.DrawImage(gdi, rc.left, rc.top);
    }
    else 
    {
        if(WIDTH(rc) != m_imgBtn.GetWidth() || HEIGHT(rc) != m_imgBtn.GetHeight())
            m_imgBtn.SetSize(WIDTH(rc), HEIGHT(rc));

        m_imgBtn.DrawImage(gdi, rc.left, rc.top);
    }
}

void CDlgPostCall::OnColorSchemeChange(HWND hWndParent, HINSTANCE hInst)
{
    m_oMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu.PreloadImages(hWndParent, hInst);
}

void CDlgPostCall::PopulateContent(int iSelectedIndex)
{
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteMyItem);
    m_oMenu.SetSelected(0);
    m_oMenu.ResetContent();

    if(GetSystemMetrics(SM_CXSCREEN) < GetSystemMetrics(SM_CYSCREEN))
        m_oMenu.AddCategory(_T("Options"));

    EnumPostCall* eOption = NULL;


    //add to contacts
    if(m_lOid == -1)
    {
        eOption = new EnumPostCall;
        *eOption = POSTCALL_AddToContacts;
        m_oMenu.AddItem(eOption, IDMENU_AddContacts);
    }

    //SMS responses
    if(m_bShowSMSList)
    {
        eOption = new EnumPostCall;
        *eOption = POSTCALL_SMS;
        m_oMenu.AddItem(eOption, IDMENU_SMS);
    }

    //call back
    if(ShowCallBack())
    {
        eOption = new EnumPostCall;
        *eOption = POSTCALL_CallBack;
        m_oMenu.AddItem(eOption, IDMENU_CallBack);
    }

    m_oMenu.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oMenu.SetSelectedItemIndex(iSelectedIndex, TRUE);
}

void CDlgPostCall::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    EnumPostCall* eCall = (EnumPostCall*)lpItem;
    delete eCall;
}

BOOL CDlgPostCall::FormatStrictNum(TCHAR* szSource, TCHAR* szDest, BOOL bRemove1 /* = FALSE */)
{
    if(szDest == NULL || szSource == NULL)
        return FALSE;

    if(m_oStr->GetLength(szSource)  < 10)
        return FALSE;

    int iPos = 0;

    for(int i = 0; i < m_oStr->GetLength(szSource); i++)
    {
        if(iPos == 0 && bRemove1 == TRUE && szSource[i] == _T('1'))
            continue;

        if(szSource[i] >= _T('0') && szSource[i] <= _T('9'))
        {
            szDest[iPos] = szSource[i];
            iPos++;
        }
    }

    szDest[iPos] = _T('\0');

    return TRUE;
}

BOOL CDlgPostCall::ShowCallBack()
{
    int iLen = m_oStr->GetLength(m_szPhoneNum);

    if(iLen < 10)
        return FALSE;

    int iCount = 0;

    for(int i = 0; i < iLen; i++)
    {
        if(m_szPhoneNum[i] >= _T('0') && m_szPhoneNum[i] <= _T('9'))
            iCount++;
    }

    if(iCount < 10)
        return FALSE;

    return TRUE;
}

BOOL CDlgPostCall::ShowBlocked()
{
    if(m_eAllow == ACALL_Block)
        return FALSE;

    if(ShowCallBack() == FALSE)
        return FALSE;

    return TRUE;
}


BOOL CDlgPostCall::ShowSMSList()
{
    if(m_iCallDurration > 0)
        return FALSE;

    if(m_lOid == -1)
        return FALSE;

    //check with poom if its a mobile
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return FALSE;

    TCHAR* szTemp = NULL;

    szTemp = oPoom->GetUserData(CI_MobileNumber, m_lOid);

    if(szTemp == NULL)
        return FALSE;
    if(m_oStr->IsEmpty(szTemp))
    {
        m_oStr->Delete(&szTemp);
        return FALSE;
    }

    //ok ... lets compare these bad boys
    BOOL bRet = FALSE;

    TCHAR szNum1[STRING_NORMAL];
    TCHAR szNum2[STRING_NORMAL];

    FormatStrictNum(m_szPhoneNum, szNum1, TRUE);
    FormatStrictNum(szTemp, szNum2, TRUE);
   
    if(m_oStr->Compare(szNum1, szNum2) == 0)
        bRet = TRUE;

    m_oStr->Delete(&szTemp);

    return bRet;
}

BOOL CDlgPostCall::OnBlock()
{
    //add to block list
    if(m_objBlockList == NULL)
        return FALSE;

    TypeBlocklist* pListItem = new TypeBlocklist;
    if(!pListItem)
        return FALSE;

    if(!IsValidOID(m_lOid))//block by number
    {
        pListItem->eType	= BlocklistItem_phoneNumber;
        m_oStr->StringCopy(pListItem->szID, m_szPhoneNum);     
        if(!m_objBlockList->AddItem(pListItem))
            delete pListItem;
    }
    else //block the whole ID ... ah yeah
    {
        pListItem->eType = BlocklistItem_contact;

        // get the name text
        TCHAR szText[STRING_MAX];
        CPoomContacts* pPoom = CPoomContacts::Instance();
        m_oStr->Empty(szText);	

        if(!pPoom->GetFormatedNameText(pListItem->szID, m_lOid, TRUE))
        {
            delete pListItem;
            return FALSE;
        }

        pListItem->lOid = m_lOid;

        if(m_objBlockList->AddItem(pListItem) == FALSE)
        {
            delete pListItem;
            return FALSE;
        }
 
    }

    m_bBlocked = TRUE;
    m_oStr->StringCopy(m_szSliderText, _T("Slide to Unblock"));
   
    InvalidateRect(m_hWnd, &m_rcSlider, FALSE);
    return TRUE;
}

BOOL CDlgPostCall::OnUnblock()
{
    //add to block list
    if(m_objBlockList == NULL)
        return FALSE;

    TypeBlocklist* sBlocked = NULL;

    if(!IsValidOID(m_lOid))//unblock by number
    {
        sBlocked = m_objBlockList->FindItem(m_szPhoneNum);
    }
    else
    {
        sBlocked = m_objBlockList->FindItem(m_lOid);
    }

    if(sBlocked)
        m_objBlockList->DeleteItem(sBlocked);

    m_bBlocked = FALSE;
    m_oStr->StringCopy(m_szSliderText, _T("Slide to Block Caller"));
    InvalidateRect(m_hWnd, &m_rcSlider, FALSE);
    return TRUE;
}

BOOL CDlgPostCall::OnCallBack()
{
    TCHAR szTemp[STRING_NORMAL];
    FormatStrictNum(m_szPhoneNum, szTemp);

    PHONEMAKECALLINFO mci = {0};
    mci.cbSize = sizeof(mci);
    mci.dwFlags = 0;
    mci.pszDestAddress = szTemp;
    PhoneMakeCall(&mci);

    Hide();

    return TRUE;
}

BOOL CDlgPostCall::OnSMS()
{
    //    Pop the Menu
    m_wndMenu.ResetContent();
    //    TCHAR szText[STRING_LARGE];

    m_wndMenu.AddItem(_T("\"I'm busy, I'll call you back.\""), IDMENU_SMS_Busy );
    m_wndMenu.AddItem(_T("\"I'm busy, try calling later.\""), IDMENU_SMS_BusyYou );
    m_wndMenu.AddItem(_T("\"I'm in a meeting.\""), IDMENU_SMS_Meeting );
    m_wndMenu.AddItem(_T("\"I'm in the car.\""), IDMENU_SMS_Car);
    m_wndMenu.AddItem(_T("Custom..."), IDMENU_SMS_Custom);

    RECT rc;
    GetWindowRect(m_hWnd, &rc);

    RECT rcBtn;
    rcBtn.left  = rc.left;
    rcBtn.top   = rc.bottom;// - HEIGHT(m_rcMenuRight);
    rcBtn.bottom= rc.bottom;
    rcBtn.right = rc.right/2;

    POINT ptBtnScreen;
    ClientToScreen(m_hWnd, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
//    rc.bottom       -= HEIGHT(m_rcMenuRight);

    m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn)
        ,ADJUST_Bottom);

    return TRUE;
}

BOOL CDlgPostCall::OnAddToContacts()
{
    if(m_lOid == -1)
    {
        Hide();
        CPoomContacts::Instance()->CreateNew(m_hWnd, m_szPhoneNum);
    }
    return TRUE;
}

BOOL CDlgPostCall::OnMoreInfo()
{
    KillTimer(m_hWnd, IDT_TIME_HIDE);
    KillTimer(m_hWnd, IDT_SHOW);

    BOOL bIsRoaming     = FALSE;
    BOOL bIsMissed      = FALSE;
    BOOL bIsConnected   = TRUE; //really means .. did it connect?
    BOOL bIsCallEnded   = TRUE;  //
    BOOL bIsIncoming    = TRUE;
    BOOL bIsUnknown     = FALSE;

    //logic time
    bIsRoaming = CObjStateAndNotifyMgr::GetIsRoaming();

    if(m_eCallType == CTYPE_OutgoingAnswered || m_eCallType == CTYPE_OutgoingDropped)
        bIsIncoming = FALSE;

    if(m_eCallType == CTYPE_IncomingMissed)
        bIsMissed = TRUE;

    //m_bIsConnected
    //set to false if we call out and there's no answer ... and we don't display the post call anyways

    //this should be fine
    if(m_oStr->IsEmpty(m_szPhoneType))
        bIsUnknown = TRUE;

    m_dlgMore.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
    m_dlgMore.SetCallItem(m_szName, m_szPhoneNum, m_szPhoneType, m_szLocation, bIsRoaming, bIsMissed, bIsConnected, bIsCallEnded, bIsIncoming, bIsUnknown, m_iCallDurration, m_sCallStartTime, m_lOid);
    m_dlgMore.PreloadImages(m_hWnd, m_hInst, m_gdiMem, &m_wndMenu);
    m_dlgMore.Launch(m_hWnd, m_hInst, TRUE);

    Show();
    SetTimeOut();

    return TRUE;
}

BOOL CDlgPostCall::SendSMSMessage(TCHAR* szMsg)
{
/*    SMS_HANDLE  smshHandle;
    DWORD       dwMsgModes = SMS_MODE_SEND;
    TCHAR       szSms[] = _T("SMS_MSGTYPE_TEXT");

    HRESULT     hr = S_OK;

    if(FAILED(SmsOpen(_T("SMS_MSGTYPE_TEXT"), SMS_MODE_SEND, &smshHandle, NULL)))
        ASSERT(0);

    //ERROR_SUCCESS //0

*/

    PROCESS_INFORMATION pi;
    TCHAR* szCommandLine = new TCHAR[m_oStr->GetLength(szMsg) + STRING_LARGE];
    m_oStr->Format(szCommandLine,  _T("-service \"SMS\" -to \"%s\" -body \"%s\""), m_sContact.szMobileNumber, szMsg);
    CreateProcess(_T("tmail.exe"), szCommandLine, NULL, NULL, FALSE, NULL, NULL, NULL, NULL, &pi);
    m_oStr->Delete(&szCommandLine);

    Hide();
    return TRUE;
}