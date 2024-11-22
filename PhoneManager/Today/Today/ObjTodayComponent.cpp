#include "StdAfx.h"
#include "ObjTodayComponent.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"
#include "IssRegistry.h"
#include "CommonDefines.h"
#include "IssStateAndNotify.h"
#include "PoomContacts.h"
#include <phone.h>
#include "resource.h"
#include "IssCommon.h"
#include "DlgChoosePrograms.h"
#include "DlgChooseSettings.h"
#include "ObjVoicemail.h"
#include "DlgSetVoicemail.h"
#include "DlgCallVoicemail.h"

#include "Shlobj.h"
#include <Cpl.h>

#ifndef CPL_IDNAME
#define CPL_IDNAME    100
#endif

typedef LONG (*CPLAPPLET) (HWND hwndCPl, UINT  msg, LPARAM  lParam1, LPARAM  lParam2);


void MakeCall(TCHAR* szAddress)
{
	PHONEMAKECALLINFO mci = {0};
	mci.cbSize = sizeof(mci);
	mci.dwFlags = 0;
	mci.pszDestAddress = szAddress;
	PhoneMakeCall(&mci);
}

void ZeroBaseRect(RECT& rc)
{
	rc.right = WIDTH(rc);
	rc.bottom = HEIGHT(rc);
	rc.left = rc.top = 0;
}

void InsetRect(RECT& rc, int x, int y)
{
	rc.left += x;
	rc.right -= x;
	rc.top += y;
	rc.bottom -= y;
}

int GetItemWidth(ComponentType eType)
{
    switch(eType)
    {
    case CompType_Info_Provider:
        return COMP_WIDTH_Provider;
    case CompType_Info_TimeLarge:
    case CompType_Info_TimeSmall:
        return COMP_WIDTH_Time;
    case CompType_Sys_Profile:
    case CompType_Message_Email:
    case CompType_Message_MissedCall:
    case CompType_Message_SMS:
    case CompType_Message_VoiceMail:
    case CompType_Contact:
    //case CompType_Message_Wifi:
    //case CompType_Message_Bluetooth:
    case CompType_Launch_Program:
    case CompType_Launch_Setting:
        return COMP_WIDTH_Message;
    }
    return COMP_WIDTH_Message;  // some default value
}

int GetItemHeight(ComponentType eType)
{
    switch(eType)
    {
    case CompType_Info_Provider:
        return COMP_HEIGHT_Provider;
    case CompType_Info_TimeLarge:
        return COMP_HEIGHT_TimeLarge;
    case CompType_Info_TimeSmall:
        return COMP_HEIGHT_TimeSmall;
    case CompType_Sys_Profile:
    case CompType_Message_Email:
    case CompType_Message_MissedCall:
    case CompType_Message_SMS:
    case CompType_Message_VoiceMail:
    case CompType_Contact:
        //case CompType_Message_Wifi:
        //case CompType_Message_Bluetooth:
    case CompType_Launch_Program:
    case CompType_Launch_Setting:
        return COMP_HEIGHT_Message;
    }
    return COMP_HEIGHT_Message;  // some default value
}

///////////////////////////////////////////////////////////////////////////////
//
//	CObjTodayComponentBase class
//
int CObjTodayComponentBase::m_iBlockSize = IsVGA()?32:16; //GetSystemMetrics(SM_CXSMICON);
const int CObjTodayComponentBase::m_iFrameIndent = IsVGA()?4:2;
HWND CObjTodayComponentBase::m_hWnd = NULL;
HINSTANCE CObjTodayComponentBase::m_hInst = NULL;
CIssImageSliced* CObjTodayComponentBase::m_imgSelector = NULL;

CObjTodayComponentBase::CObjTodayComponentBase(ComponentType eType, RECT rcPosition, HWND hWndParent, HINSTANCE hInst, long lID)
: m_rcMatrixPos(rcPosition)
, m_lID(lID)
, m_eCompType(eType)
, m_oStr(CIssString::Instance())
{
    m_hWnd = hWndParent;
    m_hInst= hInst;

	if(!m_imgSelector)
		m_imgSelector = new CIssImageSliced;

    // if the position has been defined then we make an override here because people might have older layouts set
    if(rcPosition.left >= 0 || rcPosition.top >= 0)
    {
        rcPosition.right    = rcPosition.left + GetItemWidth(eType);
        rcPosition.bottom   = rcPosition.top + GetItemHeight(eType);
        m_rcMatrixPos = rcPosition;
    }
}

CObjTodayComponentBase::~CObjTodayComponentBase(void)
{
}

void CObjTodayComponentBase::DeleteAllContent()
{
	if(m_imgSelector)
		delete m_imgSelector;
	m_imgSelector = NULL;
}

RECT CObjTodayComponentBase::GetFrame()
{
	RECT rc = { m_rcMatrixPos.left * m_iBlockSize,
		        m_rcMatrixPos.top * m_iBlockSize,
		        m_rcMatrixPos.right * m_iBlockSize,
		        m_rcMatrixPos.bottom * m_iBlockSize};

	return rc;
}

void CObjTodayComponentBase::Refresh(HWND hWnd)
{
	InvalidateRect(hWnd, &GetFrame(), FALSE);
}
void CObjTodayComponentBase::SetLocation(RECT& rc)
{
	// it is assumed there is room to place item here
	m_rcMatrixPos.left		= rc.left	/ m_iBlockSize;
	m_rcMatrixPos.right		= rc.right	/ m_iBlockSize;
	m_rcMatrixPos.top		= rc.top	/ m_iBlockSize;
	m_rcMatrixPos.bottom	= rc.bottom / m_iBlockSize;
}

void CObjTodayComponentBase::SetMatrixLocation(RECT& rc)
{
	// it is assumed there is room to place item here
	m_rcMatrixPos.left		= rc.left;
	m_rcMatrixPos.right		= rc.right;
	m_rcMatrixPos.top		= rc.top;
	m_rcMatrixPos.bottom	= rc.bottom;
}

void CObjTodayComponentBase::SetLocationUndefined()
{
	RECT rcUND = {-WIDTH(GetMatrixRect()),-HEIGHT(GetMatrixRect()),0,0};
	SetMatrixLocation(rcUND);
}

BOOL CObjTodayComponentBase::Draw(CIssGDIEx& gdi, 
					             RECT rcClient, 
					             COLORREF crText, 
					             COLORREF crBg, 
					             BOOL bHasFocus,
					             BOOL bSelected,
                                 BOOL bZeroBase,
                                 BOOL bAddAlpha)	
{ 
	RECT rcFrame = GetFrame();
    //InsetRect(rcFrame, m_iFrameIndent, m_iFrameIndent);
	if(bZeroBase)
		ZeroBaseRect(rcFrame);
	//InsetRect(rcFrame, m_iFrameIndent, m_iFrameIndent);

	if(bSelected && bHasFocus && m_imgSelector)
    {
        if(!m_imgSelector->IsLoaded())
            m_imgSelector->Initialize(m_hWnd, m_hInst, IDR_PNG_TodaySelector/*IsVGA()?IDR_PNG_TodaySelectorVGA:IDR_PNG_TodaySelector*/);

        int iWidth = WIDTH(rcFrame);
        int iHeight= HEIGHT(rcFrame);
        if(m_imgSelector->GetWidth() != iWidth || m_imgSelector->GetHeight() != iHeight)
            m_imgSelector->SetSize(iWidth, iHeight);

        m_imgSelector->DrawImage(gdi, rcFrame.left, rcFrame.top, bAddAlpha?ALPHA_AddValue:ALPHA_Normal);
    }

		//RoundRectangle(gdi, rcFrame, crBg, 0x000000, m_iBlockSize/2, m_iBlockSize/2);

	return TRUE; 
}

//////////////////////////////////////////////////////////////////////////////
//
// CObjTodayProfile class
//

#define NUM_PROFILE_Images 7
CObjTodayProfile::CObjTodayProfile(ComponentType eType, RECT rcPosition, HWND hWndParent, HINSTANCE hInst)
: CObjTodayComponentBase(eType, rcPosition, hWndParent, hInst)
{
    // check times and airplane mode
    m_objProfile.GetPhoneProfile(TRUE);	
}
CObjTodayProfile::~CObjTodayProfile()
{
}

void CObjTodayProfile::Refresh(HWND hWnd)
{
    // check times and airplane mode
    m_objProfile.LoadRegistry();
    m_objProfile.GetPhoneProfile(TRUE);	
    CObjTodayComponentBase::Refresh(hWnd);
}

BOOL CObjTodayProfile::Draw(CIssGDIEx& gdi, 
							RECT rcClient, 
							COLORREF crText, 
							COLORREF crBg, 
							BOOL bHasFocus,
							BOOL bSelected,
                            BOOL bZeroBase,
                            BOOL bAddAlpha)
{
	RECT rcFrame = GetFrame();
    InsetRect(rcFrame, m_iFrameIndent, m_iFrameIndent);
	if(bZeroBase)
		ZeroBaseRect(rcFrame);

	if(m_gdiProfile.GetDC() == NULL)
		m_gdiProfile.LoadImage(IsVGA()?IDR_ProfileVGA:IDR_Profile, m_hWnd, m_hInst, TRUE);

	int iIndex = 0;
	switch(m_objProfile.GetPhoneProfile(FALSE))
	{
	case PP_Normal:
        iIndex = 0;
        break;
	case PP_Loud:
		iIndex = 5;
		break;
    case PP_Quiet:
        iIndex = 6;
        break;
	case PP_Airplane:
		iIndex = 4;
		break;
	case PP_Vibrate:
		iIndex = 1;
		break;
	case PP_Silent:
		iIndex = 2;
		break;
	case PP_Silent1hour:
	case PP_Silent2hours:
	case PP_Silent3hours:
		iIndex = 3;
		break;
	}

	int iWidth = m_gdiProfile.GetWidth()/NUM_PROFILE_Images;
	::Draw(gdi, 
		   rcFrame.left + (WIDTH(rcFrame)-iWidth)/2, rcFrame.top + (HEIGHT(rcFrame)-m_gdiProfile.GetHeight())/2,
		   iWidth, m_gdiProfile.GetHeight(),
		   m_gdiProfile, 
		   iIndex*iWidth, 0,
		   (bAddAlpha?ALPHA_AddValue:ALPHA_Normal));

	CObjTodayComponentBase::Draw(gdi,rcClient, crText, crBg, bHasFocus, bSelected, bZeroBase, bAddAlpha);

	return TRUE;
}

void CObjTodayProfile::DrawMenuItem(CIssGDIEx& gdi, RECT rc, EnumPhoneProfile eProfile, COLORREF crText)
{
    int iIndex = 0;
    TCHAR szText[STRING_LARGE] = _T("");
    switch(eProfile)
    {
    case PP_Normal:
        iIndex = 0;
        m_oStr->StringCopy(szText, _T("Normal"));
        break;
    case PP_Loud:
        iIndex = 5;
        m_oStr->StringCopy(szText, _T("Loud"));
        break;
    case PP_Quiet:
        iIndex = 6;
        m_oStr->StringCopy(szText, _T("Quiet"));
        break;
    case PP_Airplane:
        iIndex = 4;
        m_oStr->StringCopy(szText, _T("Airplane"));
        break;
    case PP_Vibrate:
        iIndex = 1;
        m_oStr->StringCopy(szText, _T("Vibrate"));
        break;
    case PP_Silent:
        iIndex = 2;
        m_oStr->StringCopy(szText, _T("Silent"));
        break;
    case PP_Silent1hour:
        iIndex = 3;
        m_oStr->StringCopy(szText, _T("Silent 1 hour"));
        break;
    case PP_Silent2hours:
        iIndex = 3;
        m_oStr->StringCopy(szText, _T("Silent 2 hours"));
        break;
    case PP_Silent3hours:
        iIndex = 3;
        m_oStr->StringCopy(szText, _T("Silent 3 hours"));
        break;
    default:
        return;
    }

    int iWidth = m_gdiProfile.GetWidth()/NUM_PROFILE_Images;
    ::Draw(gdi,
          rc.left + INDENT, rc.top + (HEIGHT(rc) - m_gdiProfile.GetHeight())/2,
          iWidth, m_gdiProfile.GetHeight(),
          m_gdiProfile,
          iIndex*iWidth, 0);

    rc.left = rc.left + iWidth + INDENT;
    ::DrawText(gdi, szText, rc, DT_LEFT|DT_VCENTER, 0, crText);
}

void CObjTodayProfile::ExecuteItem(HWND hWnd, POINT pt)
{
    HMENU hMenuContext		= CreatePopupMenu();
    if(!hMenuContext)
        return;

    AppendMenu(hMenuContext, MF_OWNERDRAW, IDMENU_Profile+PP_Loud,  (LPCWSTR)PP_Loud);
    AppendMenu(hMenuContext, MF_OWNERDRAW, IDMENU_Profile+PP_Normal,  (LPCWSTR)PP_Normal);
    AppendMenu(hMenuContext, MF_OWNERDRAW, IDMENU_Profile+PP_Quiet,  (LPCWSTR)PP_Quiet);
    AppendMenu(hMenuContext, MF_OWNERDRAW, IDMENU_Profile+PP_Vibrate,  (LPCWSTR)PP_Vibrate);
    AppendMenu(hMenuContext, MF_OWNERDRAW, IDMENU_Profile+PP_Silent,  (LPCWSTR)PP_Silent);
    AppendMenu(hMenuContext, MF_OWNERDRAW, IDMENU_Profile+PP_Silent2hours,  (LPCWSTR)PP_Silent2hours);
    AppendMenu(hMenuContext, MF_OWNERDRAW, IDMENU_Profile+PP_Airplane,  (LPCWSTR)PP_Airplane);

    RECT rcFrame = GetFrame();
    pt.x = rcFrame.right;
    pt.y = rcFrame.top;
    ClientToScreen(hWnd, &pt);
    //Display it.
    long lSelection = TrackPopupMenu(	hMenuContext, 
        TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD , 
        pt.x, pt.y,
        0, 
        hWnd, NULL);

    DestroyMenu(hMenuContext);

    if(lSelection)
    {
        int iCmdIndex = lSelection - IDMENU_Profile;
        switch(iCmdIndex)
        {
        case PP_Loud:
        case PP_Quiet:
        case PP_Normal:
        case PP_Airplane:
        case PP_Vibrate:
        case PP_Silent:
        case PP_Silent2hours:
            m_objProfile.SetPhoneProfile((EnumPhoneProfile)iCmdIndex);
            InvalidateRect(hWnd, &rcFrame, FALSE);
            break;
        }
    }

}

//////////////////////////////////////////////////////////////////////////////
//
// CObjTodayInfoComponent class
//
CObjTodayInfoComponent::CObjTodayInfoComponent(ComponentType eType, RECT rcPosition, HWND hWndParent, HINSTANCE hInst)
: CObjTodayComponentBase(eType, rcPosition, hWndParent, hInst)
{
    m_hFontAlarms = CIssGDIEx::CreateFont(IsVGA()?24:12, FW_BOLD, TRUE);
}
CObjTodayInfoComponent::~CObjTodayInfoComponent()
{
    CIssGDIEx::DeleteFont(m_hFontAlarms);
}

BOOL CObjTodayInfoComponent::Draw(CIssGDIEx& gdi, 
								  RECT rcClient, 
								  COLORREF crText, 
								  COLORREF crBg, 
								  BOOL bHasFocus,
								  BOOL bSelected,
                                  BOOL bZeroBase,
                                  BOOL bAddAlpha)
{
    RECT rcFrame = GetFrame();
    InsetRect(rcFrame, 2*m_iFrameIndent, 0);
	if(bZeroBase)
		ZeroBaseRect(rcFrame);

    InitImages();

    m_imgBK.DrawImage(gdi, rcFrame.left, rcFrame.top, (bAddAlpha?ALPHA_AddValue:ALPHA_Normal));

    rcFrame = GetFrame();
    InsetRect(rcFrame, m_iFrameIndent, m_iFrameIndent);
    if(bZeroBase)
        ZeroBaseRect(rcFrame);
	
	switch(m_eCompType)
	{
	case CompType_Info_TimeSmall:
		DrawTimeSmall(gdi, rcFrame, bAddAlpha);
		break;
	case CompType_Info_Provider:
		{
			TCHAR szProvider[STRING_LARGE];
			if(CIssStateAndNotify::GetServiceProviderName(szProvider, sizeof(szProvider)))
                ::DrawText(gdi, szProvider, rcFrame, DT_CENTER | DT_VCENTER | DT_NOPREFIX, 0, crText);
		}
		break;
	case CompType_Info_TimeLarge:
        DrawTimeLarge(gdi, rcFrame, bAddAlpha);
		break;
	}

    CObjTodayComponentBase::Draw(gdi,rcClient, crText, crBg, bHasFocus, bSelected, bZeroBase, bAddAlpha);

	return TRUE;
}

void CObjTodayInfoComponent::DrawText(CIssGDIEx& gdi, HFONT hFontText, BOOL bZeroBase)
{
    RECT rcFrame = GetFrame();
    InsetRect(rcFrame, m_iFrameIndent, m_iFrameIndent);
    if(bZeroBase)
        ZeroBaseRect(rcFrame);

    switch(m_eCompType)
    {
    case CompType_Info_Provider:
    {
        TCHAR szProvider[STRING_LARGE];
        if(CIssStateAndNotify::GetServiceProviderName(szProvider, sizeof(szProvider)))
            ::DrawText(gdi, szProvider, rcFrame, DT_CENTER | DT_VCENTER | DT_NOPREFIX, hFontText, RGB(255,255,255));
    }
        break;
    case CompType_Info_TimeLarge:
	case CompType_Info_TimeSmall:
        DrawTextTime(gdi, hFontText, rcFrame);
        break;
    }
}

void CObjTodayInfoComponent::DrawTextTime(CIssGDIEx& gdi, HFONT hFontText, RECT rcLocation)
{
    RECT rc = rcLocation;

    rc.top      += m_iFrameIndent;
    //rc.bottom   = rc.top + size.cy;
    rc.right    -= 4*m_iFrameIndent;
    rc.left     += 4*m_iFrameIndent;

    TCHAR szText[STRING_MAX] = _T("");
    SYSTEMTIME sysTime;
    ::GetLocalTime(&sysTime);
    GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, szText, STRING_MAX);
    ::DrawText(gdi, szText, rc, DT_RIGHT|DT_TOP, hFontText, RGB(255,255,255));

    m_oStr->Empty(szText);
    if(CIssStateAndNotify::GetServiceProviderName(szText, sizeof(szText)))
        ::DrawText(gdi, szText, rc, DT_LEFT | DT_TOP | DT_NOPREFIX, hFontText, RGB(255,255,255));
}

//rcLocation is the frame .... 
void CObjTodayInfoComponent::DrawTimeLarge(CIssGDIEx& gdi, RECT rcLocation, BOOL bAddAlpha)
{
    SYSTEMTIME sysTime;
    ::GetLocalTime(&sysTime);
    TCHAR szTimeStr[STRING_LARGE] = _T("");
    int iHour = sysTime.wHour <= 12 ? sysTime.wHour : sysTime.wHour - 12;
    if(iHour == 0)
        iHour = 12;
    /*m_oStr->Format(szTimeStr, 
                    _T("%d:%02d"), 
                    iHour, 
                    sysTime.wMinute);
                    //sysTime.wHour < 12 ? _T("<"): _T(";"));*/
    GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &sysTime, NULL, szTimeStr, STRING_LARGE);

    // we have to offset for the image font
    for(int i=0; i<m_oStr->GetLength(szTimeStr); i++)
        szTimeStr[i] = szTimeStr[i] - 16;

    RECT rc = rcLocation;
    rc.top += 9*m_iFrameIndent;

    m_imgFont.DrawText(gdi, szTimeStr, rc, DT_CENTER|DT_TOP);

    rc.top += m_iFrameIndent;
    rc.right -= 4*m_iFrameIndent;
    rc.left += 4*m_iFrameIndent;

    m_oStr->Format(szTimeStr,
                   _T("%s"),
                   sysTime.wHour < 12 ? _T("<"): _T(";"));
    for(int i=0; i<m_oStr->GetLength(szTimeStr); i++)
        szTimeStr[i] = szTimeStr[i] - 16;

    m_imgFont.DrawText(gdi, szTimeStr, rc, DT_RIGHT|DT_TOP);

    //draw the alarm symbol
    int iWidth = m_gdiAlarm.GetWidth()/2;
    ::Draw(gdi, 
        rc.left, rc.top,
        iWidth, m_gdiAlarm.GetHeight(),
        m_gdiAlarm, 
        (m_oAlarms.IsAlarmActive()?0:iWidth), 0,
        (bAddAlpha?ALPHA_AddValue:ALPHA_Normal));

    rc.top += m_gdiAlarm.GetHeight();
    TCHAR szTemp[STRING_LARGE];

    //draw the alarms
    for(int i = 0; i < NUMBER_OF_ALARMS; i++)
    {
        if(m_oAlarms.IsAlarmActiveIn24(i))
        {
            m_oAlarms.GetFormattedAlarmTime(i, szTemp);
            ::DrawText(gdi, szTemp, rc, DT_LEFT | DT_TOP, m_hFontAlarms, 0xFFFFFF);
            
            rc.top += (IsVGA()?22:11);
        }
    }
}

void CObjTodayInfoComponent::DrawTimeSmall(CIssGDIEx& gdi, RECT rcLocation, BOOL bAddAlpha)
{
    SYSTEMTIME sysTime;
    ::GetLocalTime(&sysTime);
    TCHAR szTimeStr[STRING_LARGE] = _T("");
    int iHour = sysTime.wHour <= 12 ? sysTime.wHour : sysTime.wHour - 12;
    if(iHour == 0)
        iHour = 12;
    /*m_oStr->Format(szTimeStr, 
        _T("%d:%02d"), 
        iHour, 
        sysTime.wMinute);*/
    GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &sysTime, NULL, szTimeStr, STRING_LARGE);
   
    // we have to offset for the image font
    for(int i=0; i<m_oStr->GetLength(szTimeStr); i++)
        szTimeStr[i] = szTimeStr[i] - 16;

    RECT rc = rcLocation;
    rc.top += 7*m_iFrameIndent;
    m_imgFont.DrawText(gdi, szTimeStr, rc, DT_CENTER|DT_TOP);

    m_oStr->Format(szTimeStr,
        _T("%s"),
        sysTime.wHour < 12 ? _T("<"): _T(";"));
    for(int i=0; i<m_oStr->GetLength(szTimeStr); i++)
        szTimeStr[i] = szTimeStr[i] - 16;

    rc.top += m_iFrameIndent;//a little massaging for appearance sake
    rc.right -= 4*m_iFrameIndent;
    rc.left += 4*m_iFrameIndent;

    m_imgFont.DrawText(gdi, szTimeStr, rc, DT_RIGHT|DT_TOP);
    int iY = (HEIGHT(rc) - m_gdiAlarm.GetHeight())/2;

    //draw the alarm symbol centered 
    int iWidth = m_gdiAlarm.GetWidth()/2;
    ::Draw(gdi, 
        rc.left, iY + rc.top,
        iWidth, m_gdiAlarm.GetHeight(),
        m_gdiAlarm, 
        (m_oAlarms.IsAlarmActive()?0:iWidth), 0,
        (bAddAlpha?ALPHA_AddValue:ALPHA_Normal));
}

void CObjTodayInfoComponent::InitImages()
{
    if(!m_imgBK.IsLoaded())  
        m_imgBK.Initialize(m_hWnd, m_hInst, IsVGA()?IDR_PNG_TimeVGA:IDR_PNG_Time);

    RECT rcFrame = GetFrame();
    int iWidth = WIDTH(rcFrame) - 4*m_iFrameIndent;
    int iHeight= HEIGHT(rcFrame);

    if(m_imgBK.GetWidth() != iWidth || m_imgBK.GetHeight() != iHeight)
        m_imgBK.SetSize(iWidth, iHeight);
   
    if(GetType() == CompType_Info_TimeLarge)
    {
        if(!m_imgFont.IsLoaded())
            m_imgFont.Initialize(IsVGA()?IDR_PNG_TimeFontVGA:IDR_PNG_TimeFont, m_hWnd, m_hInst);

        /*if(m_gdiShadow.GetDC() == NULL)
            m_gdiShadow.LoadImage(IsVGA()?IDR_PNG_TimeShadowVGA:IDR_PNG_TimeShadow, m_hWnd, m_hInst);*/
    }
	else if(GetType() == CompType_Info_TimeSmall)
	{
		if(!m_imgFont.IsLoaded())
			m_imgFont.Initialize(IsVGA()?IDR_PNG_TimeFont:IDR_PNG_TimeFontSmall, m_hWnd, m_hInst);

		/*if(m_gdiShadow.GetDC() == NULL)
			m_gdiShadow.LoadImage(IsVGA()?IDR_PNG_TimeShadow:IDR_PNG_TimeShadowSmall, m_hWnd, m_hInst);*/
	}

    if(m_gdiAlarm.GetDC() == NULL)
    {
        m_gdiAlarm.LoadImage((IsVGA()?IDR_PNG_AlarmVGA:IDR_PNG_Alarm), m_hWnd, m_hInst, TRUE);
    }
}

void CObjTodayInfoComponent::ExecuteItem(HWND hWnd, POINT pt)
{
	switch(m_eCompType)
	{
	case CompType_Info_TimeLarge:
	case CompType_Info_TimeSmall:
		{
			CreateProcess(_T("\\Windows\\clock.exe"), 
				NULL, 
				NULL, NULL,
				FALSE, 0, 
				NULL, NULL, 
				NULL, NULL);

		}
		break;
	}
}

void CObjTodayInfoComponent::Refresh(HWND hWnd)
{
    static int iCount = 0;

    iCount++;

#ifndef DEBUG
    if(iCount >= 2)
#endif
    {
        m_oAlarms.GetAlarmInfo();
        iCount = 0;
    }
    CObjTodayComponentBase::Refresh(hWnd);
}


/////////////////////////////////////////////////////////////////////////////
//
// CObjTodayMessageComponent class
//

CIssImageSliced* CObjTodayMessageComponent::m_imgNotify = NULL;
//CObjWirelessDevices CObjTodayMessageComponent::m_objDevices;

CObjTodayMessageComponent::CObjTodayMessageComponent(ComponentType eType, RECT rcPosition, HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdi, CGuiBackground* guiBackground)
:CObjTodayComponentBase(eType, rcPosition, hWndParent, hInst)
,m_dwCount(0)
,m_hNotify(NULL)
,m_gdiMem(gdi)
,m_guiBackground(guiBackground)
{

	if(!m_imgNotify)
		m_imgNotify = new CIssImageSliced;

    /*if(eType == CompType_Message_Bluetooth && m_objDevices.IsBTInstalled())
    {
        RegistryNotifyWindow(
            HKEY_LOCAL_MACHINE,
            REG_PowerKey,
            m_objDevices.GetBTKey(),
            hWndParent, 
            WM_NOTIFY_BLUETOOTH,      
            0,
            NULL,
            &m_hNotify);
    }
    else if(eType == CompType_Message_Wifi && m_objDevices.IsWifiInstalled())
    {
        RegistryNotifyWindow(
            HKEY_LOCAL_MACHINE,
            REG_PowerKey,
            m_objDevices.GetWifiKey(),
            hWndParent, 
            WM_NOTIFY_WIFI,      
            0,
            NULL,
            &m_hNotify);
    }*/
	Refresh(hWndParent);
}
CObjTodayMessageComponent::~CObjTodayMessageComponent()
{
    if(m_hNotify)
    {
        RegistryCloseNotification(m_hNotify);
        m_hNotify = NULL;
    }
}

void CObjTodayMessageComponent::DeleteNotify()
{
	if(m_imgNotify)
		delete m_imgNotify;
	m_imgNotify = NULL;
}

BOOL CObjTodayMessageComponent::Draw(CIssGDIEx& gdi, 
									 RECT rcClient, 
									 COLORREF crText, 
									 COLORREF crBg, 
									 BOOL bHasFocus,
									 BOOL bSelected,
                                     BOOL bZeroBase,
                                     BOOL bAddAlpha)
{
	RECT rcFrame = GetFrame();
    InsetRect(rcFrame, m_iFrameIndent, m_iFrameIndent);
	if(bZeroBase)
		ZeroBaseRect(rcFrame);

	InitImages();

	int iWidth = m_gdiImage.GetWidth()/2;
	::Draw(gdi, 
		rcFrame.left + (WIDTH(rcFrame)-iWidth)/2, rcFrame.top + (HEIGHT(rcFrame)-m_gdiImage.GetHeight())/2,
		iWidth, m_gdiImage.GetHeight(),
		m_gdiImage, 
		(m_dwCount==0?0:1)*iWidth, 0, 
		(bAddAlpha?ALPHA_AddValue:ALPHA_Normal));

	if(m_dwCount > 0 && m_eCompType != CompType_Message_VoiceMail && m_imgNotify)/*&& m_eCompType != CompType_Message_Bluetooth && m_eCompType != CompType_Message_Wifi)*/
	{
		TCHAR szNum[STRING_SMALL];
		m_oStr->IntToString(szNum, m_dwCount);
		m_oStr->Concatenate(szNum, _T("  "));
		SIZE sizeItem;
		GetTextExtentPoint(gdi, szNum, m_oStr->GetLength(szNum), &sizeItem);

		int iMinW, iMinH;
		iMinW	= max(m_imgNotify->GetMinWidth(), sizeItem.cx);
		iMinH	= max(m_imgNotify->GetMinHeight(), sizeItem.cy);

		if(iMinW != m_imgNotify->GetWidth() || iMinH != m_imgNotify->GetHeight())
			m_imgNotify->SetSize(iMinW, iMinH);

		m_imgNotify->DrawImage(gdi,
								rcFrame.left + (WIDTH(rcFrame)-m_imgNotify->GetWidth())/2,
								rcFrame.top + m_iFrameIndent, (bAddAlpha?ALPHA_AddValue:ALPHA_Normal));

		//rcFrame.top += m_iFrameIndent;
		//DrawTextShadow(gdi, szNum, rcFrame, DT_TOP|DT_CENTER, NULL, RGB(255,255,255), RGB(0,0,0));
	}
    else if(m_dwCount > 0 && m_eCompType == CompType_Message_VoiceMail && m_imgNotify)
    {
        TCHAR szNum[STRING_SMALL];
        m_oStr->StringCopy(szNum, _T("+"));
        m_oStr->Concatenate(szNum, _T("  "));
        SIZE sizeItem;
        GetTextExtentPoint(gdi, szNum, m_oStr->GetLength(szNum), &sizeItem);

        int iMinW, iMinH;
        iMinW	= max(m_imgNotify->GetMinWidth(), sizeItem.cx);
        iMinH	= max(m_imgNotify->GetMinHeight(), sizeItem.cy);

        if(iMinW != m_imgNotify->GetWidth() || iMinH != m_imgNotify->GetHeight())
            m_imgNotify->SetSize(iMinW, iMinH);

        m_imgNotify->DrawImage(gdi,
            rcFrame.left + (WIDTH(rcFrame)-m_imgNotify->GetWidth())/2,
            rcFrame.top + m_iFrameIndent, (bAddAlpha?ALPHA_AddValue:ALPHA_Normal));

        //rcFrame.top += m_iFrameIndent;
        //DrawTextShadow(gdi, szNum, rcFrame, DT_TOP|DT_CENTER, NULL, RGB(255,255,255), RGB(0,0,0));
    }

    CObjTodayComponentBase::Draw(gdi,rcClient, crText, crBg, bHasFocus, bSelected, bZeroBase, bAddAlpha);

	return TRUE;
}

void CObjTodayMessageComponent::DrawText(CIssGDIEx& gdi, HFONT hFontText, BOOL bZeroBase )
{
    // NOTE: we don't show voicemail count... only if it's active
	if(m_dwCount == 0)/*|| m_eCompType == CompType_Message_Bluetooth || m_eCompType == CompType_Message_Wifi)*/
		return;

	RECT rcFrame = GetFrame();
    InsetRect(rcFrame, m_iFrameIndent, m_iFrameIndent);
	if(bZeroBase)
		ZeroBaseRect(rcFrame);

	TCHAR szNum[STRING_SMALL];
    if(m_eCompType == CompType_Message_VoiceMail)
        m_oStr->StringCopy(szNum, _T("+"));
    else
	    m_oStr->IntToString(szNum, m_dwCount);
	rcFrame.top += m_iFrameIndent;
    ::DrawText(gdi, szNum, rcFrame, DT_TOP|DT_CENTER, hFontText, RGB(0,0,0));
}



void CObjTodayMessageComponent::InitImages()
{
	if(m_imgNotify && !m_imgNotify->IsLoaded())
		m_imgNotify->Initialize(m_hWnd, m_hInst, IsVGA()?IDR_PNG_NotifyVGA:IDR_PNG_Notify);

	if(!m_gdiImage.GetDC())
	{
		switch(m_eCompType)
		{
		case CompType_Message_Email:
			m_gdiImage.LoadImage(IsVGA()?IDR_PNG_MailVGA:IDR_PNG_Mail, m_hWnd, m_hInst, TRUE);
			break;
		case CompType_Message_SMS:
			m_gdiImage.LoadImage(IsVGA()?IDR_PNG_SmsVGA:IDR_PNG_Sms, m_hWnd, m_hInst, TRUE);
			break;
		case CompType_Message_VoiceMail:
			m_gdiImage.LoadImage(IsVGA()?IDR_PNG_VoiceVGA:IDR_PNG_Voice, m_hWnd, m_hInst, TRUE);
			break;
		case CompType_Message_MissedCall:
			m_gdiImage.LoadImage(IsVGA()?IDR_PNG_MissedVGA:IDR_PNG_Missed, m_hWnd, m_hInst, TRUE);
			break;
        /*case CompType_Message_Wifi:
            m_gdiImage.LoadImage(IsVGA()?IDR_PNG_WifiVGA:IDR_PNG_Wifi, m_hWnd, m_hInst, TRUE);
            break;
        case CompType_Message_Bluetooth:
            m_gdiImage.LoadImage(IsVGA()?IDR_PNG_BTVGA:IDR_PNG_BT, m_hWnd, m_hInst, TRUE);
            break;*/
		}
	}

}

void CObjTodayMessageComponent::Refresh(HWND hWnd)
{
    int iValue = 0;
    m_dwCount = 0;
	switch(m_eCompType)
	{
	case CompType_Message_Email:
		iValue	= CIssStateAndNotify::GetUnreadEmailCount();
		break;
	case CompType_Message_SMS:
		iValue	= CIssStateAndNotify::GetUnreadSMSCount();
		break;
	case CompType_Message_VoiceMail:
		iValue	= CIssStateAndNotify::GetUnReadVoicemailCount();
		break;
	case CompType_Message_MissedCall:
		iValue	= CIssStateAndNotify::GetMissedCallsCount();
		break;
    /*case CompType_Message_Wifi:
        iValue  = m_objDevices.IsWifiOn(FALSE)?1:0;
        break;
    case CompType_Message_Bluetooth:
        iValue  = m_objDevices.IsBTOn(FALSE)?1:0;
        break;*/
	}
    m_dwCount = abs(iValue);
	CObjTodayComponentBase::Refresh(hWnd);
}


void CObjTodayMessageComponent::ExecuteItem(HWND hWnd, POINT pt)
{
	switch(m_eCompType)
	{
	case CompType_Message_Email:
		{
			CreateProcess(_T("\\Windows\\tmail.exe"), 
				NULL, 
				NULL, NULL,
				FALSE, 0, 
				NULL, NULL, 
				NULL, NULL);
		}
		break;
	case CompType_Message_SMS:
		{
         /*   TCHAR szPath[MAX_PATH];
            SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);
            m_oStr->Concatenate(szPath, _T("\\panocontacts.exe"));
            if(IsFileExists(szPath))
            {
                CreateProcess(szPath, _T("-h"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
            }
            else*/
            {
			    PROCESS_INFORMATION pi;
			    TCHAR szCommandLine[STRING_MAX*2];
			    m_oStr->Format(szCommandLine,  _T("-service \"SMS\""));
			    CreateProcess(_T("tmail.exe"), szCommandLine, NULL, NULL, FALSE, NULL, NULL, NULL, NULL, &pi);
            }
		}
		break;
	case CompType_Message_VoiceMail:
		{
            TCHAR szVMail[STRING_MAX];
            ZeroMemory(szVMail, STRING_MAX*sizeof(TCHAR));
            CObjVoicemail objVmail;
            HRESULT hr = objVmail.GetVmailNumber(szVMail);

            if(hr != S_OK || m_oStr->GetLength(szVMail) == 0)
            {         
                // put up the manual voicemail dialog
                CDlgSetVoicemail dlgVoice;
                if(dlgVoice.DoModal(m_hWnd, m_hInst, IDD_DLG_Modal) == IDOK)
                {
                    if(m_oStr->GetLength(dlgVoice.GetVmailNumber()))
                    {
                        // save the new voicemail number in the registry
                        objVmail.SetVmailNumber(dlgVoice.GetVmailNumber());
                        m_oStr->StringCopy(szVMail, dlgVoice.GetVmailNumber());
                    }
                }
            }

            if(m_oStr->GetLength(szVMail) > 0)
            {
                CDlgCallVoicemail dlgCall;
                dlgCall.SetVMailNumber(szVMail);
                dlgCall.Init(m_gdiMem, m_guiBackground);
                if(IDOK == dlgCall.Launch(m_hWnd, m_hInst, FALSE) && m_oStr->GetLength(szVMail) > 0)
                    MakeCall(szVMail);
            }

            /*TCHAR szVMail[STRING_MAX];
            objVmail.ReadSpeedDialDB(szVMail);*/
			// ask if the user wants voicemail dialed
			/*if(MessageBox(hWnd, _T("Dial Voicemail?"), _T("Voicemail..."), MB_YESNO) == IDYES)
			{
				TCHAR szVmailNumber[STRING_LARGE];
				if(CIssStateAndNotify::GetVoiceMailNumber(szVmailNumber, sizeof(szVmailNumber)))
				{
					if(m_oStr->GetLength(szVmailNumber))
						MakeCall(szVmailNumber);
				}
			}*/
		}
		break;
	case CompType_Message_MissedCall:
		{
            TCHAR szPath[MAX_PATH];
            ZeroMemory(szPath, sizeof(TCHAR)*MAX_PATH);

            if(S_OK != GetInstallDirectory(szPath, _T("Phone Genius")))
                GetInstallDirectory(szPath, _T("Call Genius"));

            //SHGetSpecialFolderPath(NULL, szPath, CSIDL_WINDOWS, FALSE);
            m_oStr->Concatenate(szPath, _T("panocontacts.exe"));
            if(IsFileExists(szPath))
            {
                CreateProcess(szPath, _T("-h"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
            }
            else
            {
			    HWND hPhoneMainWnd=::FindWindow(_T("Dialog"),_T("Phone"));
			    if (hPhoneMainWnd)
			    {
				    ::PostMessage(hPhoneMainWnd,WM_COMMAND,0x000057e6 /*History button*/,0);

				    HWND hPhoneHistoryWnd=::FindWindow(_T("MSClog"),_T("Phone"));
				    if(hPhoneHistoryWnd)
					    ::PostMessage(hPhoneHistoryWnd,WM_COMMAND,0x0000396d /*missed call filter*/,0);
			    }
            }
		}
		break;
    /*case CompType_Message_Wifi:
        if(m_objDevices.IsWifiInstalled())
        {
            HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
            if(m_objDevices.IsWifiOn(FALSE))
                m_objDevices.SetWifiMode(FALSE);
            else
                m_objDevices.SetWifiMode(TRUE);
            Refresh(m_hWnd);
            Sleep(1500);
            ::SetCursor(hCursor);
        }
        else
            MessageBeep(MB_ICONHAND);
        break;
    case CompType_Message_Bluetooth:
        if(m_objDevices.IsBTInstalled())
        {
            HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
            if(m_objDevices.IsBTOn(FALSE))
                m_objDevices.SetBTMode(FALSE);
            else
                m_objDevices.SetBTMode(TRUE);
            Refresh(m_hWnd);
            Sleep(1500);
            ::SetCursor(hCursor);
        }
        else
            MessageBeep(MB_ICONHAND);
        break;*/
	}
}


int CObjTodayMessageComponent::GetMessageCount(ComponentType eType)
{
	switch(eType)
	{
	case CompType_Message_Email:		return CIssStateAndNotify::GetUnreadEmailCount();
	case CompType_Message_MissedCall:	return CIssStateAndNotify::GetMissedCallsCount();
	case CompType_Message_VoiceMail:	return CIssStateAndNotify::GetUnReadVoicemailCount();
	case CompType_Message_SMS:			return CIssStateAndNotify::GetUnreadSMSCount();
	}

	return 0;
}

TCHAR* CObjTodayMessageComponent::GetMessageLabel(ComponentType eType)
{
	switch(eType)
	{
	case CompType_Message_Email:		return _T("Email");
	case CompType_Message_MissedCall:	return _T("Missed");
	case CompType_Message_SMS:			return _T("SMS");
	case CompType_Message_VoiceMail:	return _T("VMail");
    //case CompType_Message_Wifi:	        return _T("Wi-Fi");
    //case CompType_Message_Bluetooth:	return _T("Bluetooth");
	}
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////
//
// CObjTodayContactComponent class
//
CObjTodayContactComponent::CObjTodayContactComponent(TCHAR* szName, CDlgContactDetails* dlg, CGuiBackground* gui, CIssGDIEx* gdi,RECT rcPosition, HWND hWndParent, HINSTANCE hInst, long lOid)
: CObjTodayComponentBase(CompType_Contact, rcPosition, hWndParent, hInst, lOid)
,m_bCheckedImage(FALSE)
,m_dlgContactDetails(dlg)
,m_guiBackground(gui)
,m_gdiMem(gdi)
,m_bDefaultImage(TRUE)
{
	m_szName = m_oStr->CreateAndCopy(szName);
}
CObjTodayContactComponent::~CObjTodayContactComponent()
{
	m_oStr->Delete(&m_szName);
}

BOOL CObjTodayContactComponent::Draw(CIssGDIEx& gdi, 
									 RECT rcClient, 
									 COLORREF crText, 
									 COLORREF crBg, 
									 BOOL bHasFocus,
									 BOOL bSelected,
                                     BOOL bZeroBase,
                                     BOOL bAddAlpha)
{
	RECT rcFrame = GetFrame();
    InsetRect(rcFrame, m_iFrameIndent, m_iFrameIndent);

	if(bZeroBase)
		ZeroBaseRect(rcFrame);

    if(!m_bCheckedImage && !m_gdiPic.GetDC())
        InitImages(rcFrame);

    DrawImage(gdi, rcFrame, bAddAlpha);

    CObjTodayComponentBase::Draw(gdi, rcClient, crText, crBg, bHasFocus, bSelected, bZeroBase, bAddAlpha);

	return TRUE;
}

void CObjTodayContactComponent::Refresh(HWND hWnd)
{
    m_bCheckedImage = FALSE;
    m_gdiPic.Destroy();
    CObjTodayComponentBase::Refresh(hWnd);
}

void CObjTodayContactComponent::DrawText(CIssGDIEx& gdi, HFONT hFontText, BOOL bZeroBase )
{
	// no need for text when you have a working picture
	if(!m_bDefaultImage)
		return;

	RECT rcFrame = GetFrame();
    InsetRect(rcFrame, m_iFrameIndent, m_iFrameIndent);
	if(bZeroBase)
		ZeroBaseRect(rcFrame);

	CPoomContacts* pPoom = CPoomContacts::Instance();
	TCHAR* szFirstName = pPoom->GetUserFirstName(GetID());
	TCHAR* szLastName = pPoom->GetUserLastName(GetID());

	RECT rcText = rcFrame;
	rcText.bottom = rcText.top + HEIGHT(rcText) / 2;
	DrawTextShadow(gdi, szFirstName, rcText, DT_CENTER | DT_BOTTOM | DT_WORD_ELLIPSIS, hFontText, RGB(255,255,255), RGB(0,0,0));

	rcText = rcFrame;
	rcText.top = rcText.bottom - HEIGHT(rcText) / 2;
	DrawTextShadow(gdi, szLastName, rcText,DT_CENTER | DT_TOP | DT_WORD_ELLIPSIS, hFontText, RGB(255,255,255), RGB(0,0,0));

	m_oStr->Delete(&szFirstName);
	m_oStr->Delete(&szLastName);


}

void CObjTodayContactComponent::DrawImage(CIssGDIEx& gdi, RECT& rcFrame,
                                          BOOL bAddAlpha)
{
    ::Draw(gdi, rcFrame, m_gdiPic, 0,0,(bAddAlpha?ALPHA_AddValue:ALPHA_Normal));
}

void CObjTodayContactComponent::InitImages(RECT& rcFrame)
{
    int iWidth = WIDTH(rcFrame);
    int iHeight= HEIGHT(rcFrame);

    if(m_gdiPic.GetDC() == NULL || iWidth != m_gdiPic.GetWidth() || iHeight != m_gdiPic.GetHeight())
    {

        CIssImageSliced imgMask, imgBorder;

        imgMask.Initialize(m_hWnd, m_hInst, IsVGA()?IDR_PNG_FavoritesImgAlphaVGA:IDR_PNG_FavoritesImgAlpha);
        imgMask.SetSize(iWidth, iHeight);
        imgBorder.Initialize(m_hWnd, m_hInst, IsVGA()?IDR_PNG_FavoritesImgBorderVGA:IDR_PNG_FavoritesImgBorder);
        imgBorder.SetSize(iWidth, iHeight);

        m_bCheckedImage = TRUE;
        CPoomContacts* pPoom = CPoomContacts::Instance();
        CIssGDIEx* gdiPic    = pPoom->GetUserPicture(GetID(), iWidth, iHeight, RGB(0,0,0), TRUE);

        RECT rc;
        SetRect(&rc, 0,0,iWidth, iHeight);
		HDC dc = GetDC(m_hWnd);
        m_gdiPic.Create(dc, rc, FALSE, TRUE);
		ReleaseDC(m_hWnd, dc);
        FillRect(m_gdiPic, rc, RGB(0,0,0));

        if(gdiPic)
        {     
            m_bDefaultImage = FALSE;
            // draw it centered
            BitBlt(m_gdiPic,
                   0,0,
                   iWidth, iHeight,
                   *gdiPic,
                   (gdiPic->GetWidth()-iWidth)/2, (gdiPic->GetHeight()-iHeight)/2,
                   SRCCOPY);
                        
        }
        else
        {
            m_bDefaultImage = TRUE;
            CIssGDIEx gdiTemp;
            gdiTemp.LoadImage(IsVGA()?IDR_PNG_FavoritesImgDefaultVGA:IDR_PNG_FavoritesImgDefault, m_hWnd, m_hInst, TRUE);
            SIZE sz;
            sz.cx   = iWidth;
            sz.cy   = iHeight;
            ScaleImage(gdiTemp, m_gdiPic, sz, FALSE, 0);
        }

		// add the roundness and border
		m_gdiPic.SetAlphaMask(imgMask.GetImage());
		imgBorder.DrawImage(m_gdiPic, 0,0, ALPHA_AddValue);
    }
}

void CObjTodayContactComponent::ExecuteItem(HWND hWnd, POINT pt)
{
	//OnContextMenu(hWnd, pt);
    CPoomContacts* oPoom = CPoomContacts::Instance();
#ifdef FIX_ANNOYING_POOM_ONEOFF_ERROR
    int iPoomIndex = oPoom->GetIndexFromOID(GetID());
#else
    int iPoomIndex = oPoom->GetIndexFromOID(GetID()) - 1;
#endif

    m_dlgContactDetails->SetIndex(iPoomIndex);
    m_dlgContactDetails->Init(m_gdiMem, m_guiBackground);
    int iReturn = m_dlgContactDetails->Launch(m_hWnd, m_hInst, FALSE);
}


/////////////////////////////////////////////////////////////////////////////
//
// CObjTodayContactComponent class
//

CObjTodayLauncherComponent::CObjTodayLauncherComponent(RECT rcPosition, 
                                                     HWND hWndParent, 
                                                     HINSTANCE hInst, 
                                                     TCHAR* szPath, 
                                                     TCHAR* szFriendlyName,
                                                     TCHAR* szRegName,
                                                     EnumLaunchType eType)
                                                     : CObjTodayComponentBase((eType == LT_Program?CompType_Launch_Program:CompType_Launch_Setting), rcPosition, hWndParent, hInst, 0)
{
    m_szPath = m_oStr->CreateAndCopy(szPath);
    m_szFriendlyName = m_oStr->CreateAndCopy(szFriendlyName);
    m_szRegName = m_oStr->CreateAndCopy(szRegName);
}
CObjTodayLauncherComponent::~CObjTodayLauncherComponent()
{
    m_oStr->Delete(&m_szPath);
    m_oStr->Delete(&m_szFriendlyName);
}

BOOL CObjTodayLauncherComponent::Draw(CIssGDIEx& gdi, 
                                     RECT rcClient, 
                                     COLORREF crText, 
                                     COLORREF crBg, 
                                     BOOL bHasFocus,
                                     BOOL bSelected,
                                     BOOL bZeroBase,
                                     BOOL bAddAlpha)
{
    RECT rcFrame = GetFrame();
    InsetRect(rcFrame, m_iFrameIndent, m_iFrameIndent);

    if(bZeroBase)
        ZeroBaseRect(rcFrame);

    if(!m_gdiPic.GetDC())
        InitImages(rcFrame);

    DrawImage(gdi, rcFrame, bAddAlpha);

    CObjTodayComponentBase::Draw(gdi, rcClient, crText, crBg, bHasFocus, bSelected, bZeroBase, bAddAlpha);

    return TRUE;
}

void CObjTodayLauncherComponent::Refresh(HWND hWnd)
{
    m_gdiPic.Destroy();
    CObjTodayComponentBase::Refresh(hWnd);
}

void CObjTodayLauncherComponent::DrawImage(CIssGDIEx& gdi, RECT& rcFrame,
                                          BOOL bAddAlpha)
{
    ::Draw(gdi, rcFrame, m_gdiPic, 0,0,(bAddAlpha?ALPHA_AddValue:ALPHA_Normal));
}

void CObjTodayLauncherComponent::InitImages(RECT& rcFrame)
{
    int iWidth = WIDTH(rcFrame);
    int iHeight= HEIGHT(rcFrame);

    if(m_gdiPic.GetDC() == NULL || iWidth != m_gdiPic.GetWidth() || iHeight != m_gdiPic.GetHeight())
    {
        CIssGDIEx gdiShine, gdiBg;
        CIssImageSliced imgMask, imgBorder;

        imgMask.Initialize(m_hWnd, m_hInst, IsVGA()?IDR_PNG_FavoritesImgAlphaVGA:IDR_PNG_FavoritesImgAlpha);
        imgMask.SetSize(iWidth, iHeight);
        imgBorder.Initialize(m_hWnd, m_hInst, IsVGA()?IDR_PNG_FavoritesImgBorderVGA:IDR_PNG_FavoritesImgBorder);
        imgBorder.SetSize(iWidth, iHeight);

        gdiShine.LoadImage(IsVGA()?IDR_PNG_ShineVGA:IDR_PNG_Shine, m_hWnd, m_hInst, TRUE);
        gdiBg.LoadImage(IsVGA()?IDR_PNG_DropBgVGA:IDR_PNG_DropBg, m_hWnd, m_hInst, TRUE);

        if(GetType() == CompType_Launch_Program)
        {
            SHFILEINFO sfi = {0};
            HIMAGELIST hImg = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
                SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);

            ZeroMemory(&sfi, sizeof(SHFILEINFO));
            int iIndex = 0;
            // get the icon index
            if (SHGetFileInfo(m_szPath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_ATTRIBUTES))
                iIndex = sfi.iIcon;

            // draw the icon centered 
            ImageList_Draw( hImg, 
                            iIndex, gdiBg, 
                            (gdiBg.GetWidth()-GetSystemMetrics(SM_CXICON))/2, 
                            (gdiBg.GetHeight()-GetSystemMetrics(SM_CYICON))/2,
                            ILD_TRANSPARENT);
        }
        else
        {
            TCHAR szPath[STRING_MAX];
            TCHAR szItem[STRING_LARGE];
            ZeroMemory(szPath, sizeof(TCHAR)*STRING_MAX);
            ZeroMemory(szItem, sizeof(TCHAR)*STRING_LARGE);

            int iIndex = m_oStr->Find(m_szPath, _T(","));
            m_oStr->StringCopy(szPath, m_szPath, 0, iIndex);
            m_oStr->StringCopy(szItem, m_szPath, iIndex+1, m_oStr->GetLength(m_szPath)-iIndex-1);
            iIndex = m_oStr->StringToInt(szItem);

            //Got a control panel applet.
            HINSTANCE hCPL = LoadLibrary(szPath);
            if(hCPL)
            {
                CPLAPPLET pCPLApplet = (CPLAPPLET)GetProcAddress(hCPL, _T("CPlApplet"));
                if(pCPLApplet)
                {
                    pCPLApplet(m_hWnd, CPL_INIT, 0, 0);
                    //Find the name of this CPL applet.
                    NEWCPLINFO CplInfo = {0};
                    CplInfo.dwSize = sizeof(NEWCPLINFO);

                    if(pCPLApplet(m_hWnd, CPL_NEWINQUIRE, iIndex, (LPARAM)&CplInfo) == 0)
                    {
                        // draw the icon centered 
                        DrawIcon(gdiBg,
                            (gdiBg.GetWidth()-GetSystemMetrics(SM_CXICON))/2, 
                            (gdiBg.GetHeight()-GetSystemMetrics(SM_CYICON))/2,
                            CplInfo.hIcon);
                    }
                    pCPLApplet(m_hWnd, CPL_EXIT, 0, 0);
                }
                FreeLibrary(hCPL);
            }

        }

        // draw the shine on top
        ::Draw(gdiBg,
             0,0,
             gdiShine.GetWidth(), gdiShine.GetHeight(),
             gdiShine,
             0,0);

        SIZE sz;
        sz.cx   = iWidth;
        sz.cy   = iHeight;
        ScaleImage(gdiBg, m_gdiPic, sz, FALSE, 0);

        // add the roundness and border
        m_gdiPic.SetAlphaMask(imgMask.GetImage());
        imgBorder.DrawImage(m_gdiPic, 0,0, ALPHA_AddValue);
    }
}

void CObjTodayLauncherComponent::ExecuteItem(HWND hWnd, POINT pt)
{
    if(GetType() == CompType_Launch_Program)
    {
        SHELLEXECUTEINFO sei = {0};
        sei.cbSize	    = sizeof(sei);
        sei.hwnd	    = hWnd;
        sei.nShow	    = SW_SHOWNORMAL;
        sei.lpFile      = m_szPath;
        ShellExecuteEx(&sei);
    }
    else
    {
        SHELLEXECUTEINFO sei = {0};
        sei.cbSize	    = sizeof(sei);
        sei.hwnd	    = hWnd;
        sei.nShow	    = SW_SHOWNORMAL;
        sei.lpFile      = _T("\\Windows\\ctlpnl.exe");
        sei.lpParameters= m_szPath;
        ShellExecuteEx(&sei);
    }
}

