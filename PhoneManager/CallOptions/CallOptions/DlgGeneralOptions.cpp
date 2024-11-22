#include "StdAfx.h"
#include "DlgGeneralOptions.h"
#include "IssRegistry.h"
#include "Resource.h"
#include "CallOptionsGuiDefines.h"
#include "CommonDefines.h"
#include "IssCommon.h"
#include "ObjSkinEngine.h"
#include "objringtoneengine.h"


#define     IDMENU_Selection        5000
#define     IDMENU_Popup            5001

// go through and find the CProg process to kill it
BOOL EnumCProgWindowsProc(HWND hWnd, DWORD lParam)
{
    DWORD pid;
    BOOL bRet = TRUE;

    // get the processid for this window 
    if (!GetWindowThreadProcessId(hWnd, &pid))  
        return bRet;

    // let's get the full path of the process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if(hProcess)
    {
        CIssString* oStr = CIssString::Instance();
        TCHAR szFullName[MAX_PATH] = _T("");
        GetModuleFileName((HMODULE)hProcess, szFullName, MAX_PATH);

        // we're looking for the cprog process so we can kill it
        if(-1 != oStr->FindNoCase(szFullName, _T("cprog.exe")))
        {
            TerminateProcess(hProcess, 0);
            bRet = FALSE;
        }
        CloseHandle(hProcess);
    }   

    return bRet; // keep going
}


CDlgGeneralOptions::CDlgGeneralOptions()
:m_dwValues(FLAG_SHOW_Cheekguard|FLAG_SHOW_IncomingCall|FLAG_SHOW_PostCall/*|FLAG_SHOW_HandleRingtone*/)
,m_eLookupMode(LT_AreaCodes)
{
    m_hFontBtnText	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_NORMAL, TRUE);
}

CDlgGeneralOptions::~CDlgGeneralOptions(void)
{	
    CIssGDIEx::DeleteFont(m_hFontBtnText);
}

BOOL CDlgGeneralOptions::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RefreshList();
	return TRUE;
}


/*void CDlgGeneralOptions::InitControls()
{
	for(int i = 0; i < kNumOptions; i++)
	{
		m_hChkOptions[i]	= CreateWindowEx(0, _T("Button"), 
			NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX | BS_MULTILINE, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);
		SetWindowText(m_hChkOptions[i], kGeneralOptionsText[i]);

	}

	// read flags from registry
	UINT uFlags = 0;
	DWORD dwSize  = sizeof(UINT);
	GetKey(REG_KEY_ISS_PATH, REG_GeneralOptions, (LPBYTE)&uFlags, dwSize);

	for(int i = 0; i < kNumOptions; i++)
	{
		SendMessage(m_hChkOptions[i], BM_SETCHECK, (WPARAM) uFlags & (1 << i) ? BST_CHECKED : BST_UNCHECKED, 0);
	}

	m_hLstPhoneProfile = CreateWindowEx(0, _T("ComboBox"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL| WS_BORDER, 0, 0, 0, 0, m_hWnd, NULL, m_hInst, NULL);

	// phone profile
	ListAddString(m_hLstPhoneProfile, _T("Normal"));
	ListAddString(m_hLstPhoneProfile, _T("Airplane"));
	ListAddString(m_hLstPhoneProfile, _T("Silent"));
	ListAddString(m_hLstPhoneProfile, _T("Silent for 2 hours"));
	ListAddString(m_hLstPhoneProfile, _T("Silent for 3 hours"));
	ListSetCurSel(m_hLstPhoneProfile, (int)m_objPhoneProfile.GetPhoneProfile());
}*/

/*BOOL CDlgGeneralOptions::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, FALSE, FALSE);

	FillRect(m_gdiMem.GetDC(), rcClient, RGB(255,255,255));
	::DrawText(m_gdiMem.GetDC(), _T("Choose options:"), m_rcTitleText, DT_SINGLELINE|DT_TOP|DT_LEFT, m_hTitleFont, 0);

	::DrawText(m_gdiMem.GetDC(), _T("Phone Profile:"), m_rcTitlePhoneProfile, DT_SINGLELINE|DT_TOP|DT_LEFT, m_hTitleFont, 0);

	EnumPhoneProfile eProfile = m_objPhoneProfile.GetPhoneProfile();
	if(eProfile == PP_Silent2hours || eProfile == PP_Silent3hours)
	{
		SYSTEMTIME sysTime = m_objPhoneProfile.GetEndSilentTime();
		TCHAR szText[STRING_LARGE] = _T("");
		DWORD dwLocale;
		dwLocale    = GetUserDefaultLCID();
		dwLocale	= MAKELCID( MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), SORT_DEFAULT) ;
		GetTimeFormat(dwLocale,TIME_NOSECONDS,&sysTime,NULL,szText,STRING_LARGE);
		m_oStr->Insert(szText, _T("End time: "));
		::DrawText(m_gdiMem.GetDC(), szText, m_rcTextPhoneProfile, DT_SINGLELINE|DT_TOP|DT_RIGHT, m_hTextFont, 0);
	}

	BitBlt(hDC,
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiMem.GetDC(),
		0,0,
		SRCCOPY);

	return TRUE;
}*/

BOOL CDlgGeneralOptions::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    CDlgBaseOptions::OnSize(hWnd, wParam, lParam);

    // so we only do this once
    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rcClient) != GetSystemMetrics(SM_CYSCREEN))
        return FALSE;

    m_oMenu.OnSize(m_rcArea.left, 
        m_rcArea.top,
        WIDTH(m_rcArea), HEIGHT(m_rcArea));

	return TRUE;
}


BOOL CDlgGeneralOptions::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	/*if(lParam == (LPARAM)m_hLstPhoneProfile)
	{
		if(HIWORD(wParam) == CBN_SELCHANGE)
		{
			int iIndex = ListGetCurSel(m_hLstPhoneProfile);
			if(iIndex != (int)m_objPhoneProfile.GetPhoneProfile())
				m_objPhoneProfile.SetPhoneProfile((EnumPhoneProfile)iIndex);
			InvalidateRect(m_hWnd, NULL, FALSE);
		}
		return TRUE;
	}*/

	switch(LOWORD(wParam))
	{
    case IDOK:
        m_oMenu.OnKeyDown(VK_RETURN, 0);
        break;
	case IDMENU_Selection:
        LaunchPopupMenu();
		break;
	case IDMENU_Popup:
        SetSelection();
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

void CDlgGeneralOptions::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    EnumGeneralOptions* eCall = (EnumGeneralOptions*)lpItem;
    delete eCall;
}

void CDlgGeneralOptions::RefreshList()
{
    // if we want to save selected, store it first...
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteMyItem);
    m_oMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu.ResetContent();

    ReadRegValues();
    m_objPhoneProfile.GetPhoneProfile();

    EnumGeneralOptions* eOption = new EnumGeneralOptions;
    *eOption = GO_ShowIncomingCall;
    m_oMenu.AddItem(eOption, IDMENU_Selection);

    eOption = new EnumGeneralOptions;
    *eOption = GO_ShowCheekGuard;
    m_oMenu.AddItem(eOption, IDMENU_Selection);
    
    eOption = new EnumGeneralOptions;
    *eOption = GO_ShowPostCallScreen;
    m_oMenu.AddItem(eOption, IDMENU_Selection);

    eOption = new EnumGeneralOptions;
    *eOption = GO_LookupMode;
    m_oMenu.AddItem(eOption, IDMENU_Selection);

    eOption = new EnumGeneralOptions;
    *eOption = GO_HandleRingtones;
    m_oMenu.AddItem(eOption, IDMENU_Selection);

    eOption = new EnumGeneralOptions;
    *eOption = GO_PhoneProfile;
    m_oMenu.AddItem(eOption, IDMENU_Selection);
    TypeItems* sItem = m_oMenu.GetItem(m_oMenu.GetItemCount()-1);
    sItem->iHeight      = sItem->iHeight*3/2;
    sItem->iHeightSel   = sItem->iHeightSel*3/2;
    
    m_oMenu.SetSelectedItemIndex(0, TRUE);

}

BOOL CDlgGeneralOptions::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    m_oMenu.OnDraw(gdi, rcClient, rcClip);

    return TRUE;
}

void CDlgGeneralOptions::OnMenuLeft()
{
    SafeCloseWindow(IDOK);
}

void CDlgGeneralOptions::OnMenuRight()
{
}

void CDlgGeneralOptions::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("General Options"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgGeneralOptions::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    if(IsRectInRect(rc, rcClip))
    {
        DrawTextShadow(gdi, _T("Back"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

        // draw the arrow too
        Draw(gdi,
            WIDTH(m_rcBottomBar)/8 - m_gdiBackArrow.GetWidth(), rc.top + (HEIGHT(m_rcBottomBar)-m_gdiBackArrow.GetHeight())/2,
            m_gdiBackArrow.GetWidth(), m_gdiBackArrow.GetHeight(),
            m_gdiBackArrow,
            0,0);
    }
}

void CDlgGeneralOptions::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgGeneralOptions* pThis = (CDlgGeneralOptions*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumGeneralOptions* eOption = (EnumGeneralOptions*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eOption);
}

BOOL CDlgGeneralOptions::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumGeneralOptions* eOption)
{
    RECT rcText = rc;
    if(*eOption == GO_PhoneProfile)
        rcText.bottom   = rc.top + HEIGHT(rc)/2;

    Draw(gdi,
        rcText.right - m_oMenu.GetImageArrayGDI().GetHeight(), rcText.top + (HEIGHT(rcText)-m_oMenu.GetImageArrayGDI().GetHeight())/2,
        m_oMenu.GetImageArrayGDI().GetHeight(),m_oMenu.GetImageArrayGDI().GetHeight(),
        m_oMenu.GetImageArrayGDI(),
        (bIsHighlighted?2:3)*m_oMenu.GetImageArrayGDI().GetHeight(), 0);  

    TCHAR szTitle[STRING_MAX] = _T("");
    TCHAR szInfo[STRING_MAX]  = _T("");

    GetItemTitle(*eOption, szTitle);


    switch(*eOption)
    {
    case GO_ShowIncomingCall:
        if(m_dwValues&FLAG_SHOW_IncomingCall)
            m_oStr->StringCopy(szInfo, _T("Yes"));
        else
            m_oStr->StringCopy(szInfo, _T("No"));
        break;
    case GO_ShowPostCallScreen:
        if(m_dwValues&FLAG_SHOW_PostCall)
            m_oStr->StringCopy(szInfo, _T("Yes"));
        else
            m_oStr->StringCopy(szInfo, _T("No"));
        break;
    case GO_ShowCheekGuard:
        if(m_dwValues&FLAG_SHOW_Cheekguard)
            m_oStr->StringCopy(szInfo, _T("Yes"));
        else
            m_oStr->StringCopy(szInfo, _T("No"));
        break;
    case GO_LookupMode:
        GetLookupMode(m_eLookupMode, szInfo);
        break;
    case GO_HandleRingtones:
        if(m_dwValues&FLAG_SHOW_HandleRingtone)
            m_oStr->StringCopy(szInfo, _T("Yes"));
        else
            m_oStr->StringCopy(szInfo, _T("No"));
        break;
    case GO_PhoneProfile:
        GetPhoneProfileText(m_objPhoneProfile.GetPhoneProfile(FALSE), szInfo);
        break;
    default:
        return TRUE;
    }

    // title text
    rcText.left		+= INDENT;
    DrawTextShadow(gdi, szTitle, rcText, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    // value text
    rcText.right	= rc.right - m_oMenu.GetImageArrayGDI().GetHeight();
    DrawText(gdi, szInfo, rcText, DT_RIGHT | DT_VCENTER, m_hFontBtnText, (bIsHighlighted?RGB(255,255,255):RGB(167,167,167)));

    if(*eOption == GO_PhoneProfile)
    {
        // if there is a timeout then show the actual end time
        EnumPhoneProfile eProfile = m_objPhoneProfile.GetPhoneProfile(FALSE);
        if(eProfile == PP_Silent2hours || eProfile == PP_Silent3hours || eProfile == PP_Silent1hour)
        {
            SYSTEMTIME sysTime = m_objPhoneProfile.GetEndSilentTime();
            DWORD dwLocale;
            dwLocale    = GetUserDefaultLCID();
            dwLocale	= MAKELCID( MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), SORT_DEFAULT) ;
            GetTimeFormat(dwLocale,TIME_NOSECONDS,&sysTime,NULL,szInfo,STRING_LARGE);
            m_oStr->Insert(szInfo, _T("End time: "));
            rcText.top      = rcText.bottom;
            rcText.bottom   = rc.bottom;
            DrawText(gdi, szInfo, rcText, DT_RIGHT | DT_VCENTER, m_hFontBtnText, (bIsHighlighted?RGB(255,255,255):RGB(167,167,167)));
        }
    }
    return TRUE;
}

void CDlgGeneralOptions::ReadRegValues()
{
    DWORD dwTemp;
    if(S_OK == GetKey(REG_KEY_ISS_PATH, REG_LookupMode, dwTemp))
        m_eLookupMode = (EnumLookupType)dwTemp;

    GetKey(REG_KEY_ISS_PATH, REG_GeneralOptions, m_dwValues);
}

void CDlgGeneralOptions::SaveRegValues()
{
    DWORD dwTemp = (DWORD)m_eLookupMode;
    SetKey(REG_KEY_ISS_PATH, REG_GeneralOptions, m_dwValues);
    SetKey(REG_KEY_ISS_PATH, REG_LookupMode, dwTemp);

    if(m_dwValues & FLAG_SHOW_HandleRingtone)
    {
        CObjRingtoneEngine objRing;
        objRing.SetRingTone0ToNone();
        objRing.SetIssRingTone();
    }
    else
    {
        CObjRingtoneEngine objRing;
        objRing.SetRingTone0ToDefault();
        objRing.DestroyIssRingTone();
    }
}

void CDlgGeneralOptions::GetPhoneProfileText(EnumPhoneProfile ePhoneProfile, TCHAR* szText)
{
    switch(ePhoneProfile)
    {
    case PP_Normal:
        m_oStr->StringCopy(szText, _T("Normal"));
        break;
    case PP_Airplane:
        m_oStr->StringCopy(szText, _T("Airplane"));
        break;
    case PP_Loud:
        m_oStr->StringCopy(szText, _T("Loud"));
        break;
    case PP_Quiet:
        m_oStr->StringCopy(szText, _T("Quiet"));
        break;
    case PP_Vibrate:
        m_oStr->StringCopy(szText, _T("Vibrate"));
        break;
    case PP_Silent:
        m_oStr->StringCopy(szText, _T("Silent"));
        break;
    case PP_Silent1hour:
        m_oStr->StringCopy(szText, _T("Silent 1 hour"));
        break;
    case PP_Silent2hours:
        m_oStr->StringCopy(szText, _T("Silent 2 hours"));
        break;
    case PP_Silent3hours:
        m_oStr->StringCopy(szText, _T("Silent 3 hours"));
        break;
    }
}

void CDlgGeneralOptions::GetLookupMode(EnumLookupType eNumLookup, TCHAR* szText)
{
    switch(eNumLookup)
    {
    case LT_AreaCodes:
        m_oStr->StringCopy(szText, _T("North America"));
        break;
    case LT_Australia:
        m_oStr->StringCopy(szText, _T("Australia"));
        break;
    case LT_NewZealand:
        m_oStr->StringCopy(szText, _T("New Zealand"));
        break;
    case LT_UnitedKingdom:
        m_oStr->StringCopy(szText, _T("United Kingdom"));
        break;
    case LT_CountryCodes:
        m_oStr->StringCopy(szText, _T("International"));
        break;
    case LT_Off:
        m_oStr->StringCopy(szText, _T("Off"));
        break;
    }
}

void CDlgGeneralOptions::GetItemTitle(EnumGeneralOptions eOption, TCHAR* szTitle)
{
    switch(eOption)
    {
    case GO_ShowCheekGuard:
        m_oStr->StringCopy(szTitle, _T("Show cheek guard:"));
        break;
    case GO_ShowIncomingCall:
        m_oStr->StringCopy(szTitle, _T("Show incoming call screen:"));
        break;
    case GO_ShowPostCallScreen:
        m_oStr->StringCopy(szTitle, _T("Show post call screen:"));
        break;
    case GO_HandleRingtones:
        m_oStr->StringCopy(szTitle, _T("Enable ringtone handling:"));
        break;
    case GO_PhoneProfile:
        m_oStr->StringCopy(szTitle, _T("Phone profile:"));
        break;
    case GO_LookupMode:
        m_oStr->StringCopy(szTitle, _T("Lookup Region:"));
        break;
    }
}

void CDlgGeneralOptions::LaunchPopupMenu()
{
    m_wndMenu.ResetContent();

    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    EnumGeneralOptions* eOption = (EnumGeneralOptions*)sItem->lpItem;
    TCHAR szTitle[STRING_MAX];
    GetItemTitle(*eOption, szTitle);
    m_wndMenu.AddCategory(szTitle);

    switch(*eOption)
    {
    case GO_ShowCheekGuard:
        m_wndMenu.AddItem(_T("Yes"), IDMENU_Popup, (m_dwValues&FLAG_SHOW_Cheekguard?FLAG_Radio:NULL), TRUE);
        m_wndMenu.AddItem(_T("No"), IDMENU_Popup, (!(m_dwValues&FLAG_SHOW_Cheekguard)?FLAG_Radio:NULL), FALSE);
        break;
    case GO_ShowIncomingCall:
        m_wndMenu.AddItem(_T("Yes"), IDMENU_Popup, (m_dwValues&FLAG_SHOW_IncomingCall?FLAG_Radio:NULL), TRUE);
        m_wndMenu.AddItem(_T("No"), IDMENU_Popup, (!(m_dwValues&FLAG_SHOW_IncomingCall)?FLAG_Radio:NULL), FALSE);
        break;
    case GO_ShowPostCallScreen:
        m_wndMenu.AddItem(_T("Yes"), IDMENU_Popup, (m_dwValues&FLAG_SHOW_PostCall?FLAG_Radio:NULL), TRUE);
        m_wndMenu.AddItem(_T("No"), IDMENU_Popup, (!(m_dwValues&FLAG_SHOW_PostCall)?FLAG_Radio:NULL), FALSE);
        break;
    case GO_HandleRingtones:
        m_wndMenu.AddItem(_T("Yes"), IDMENU_Popup, (m_dwValues&FLAG_SHOW_HandleRingtone?FLAG_Radio:NULL), TRUE);
        m_wndMenu.AddItem(_T("No"), IDMENU_Popup, (!(m_dwValues&FLAG_SHOW_HandleRingtone)?FLAG_Radio:NULL), FALSE);
        break;
    case GO_LookupMode:
        for(int i = 0; i < (int)LT_Count; i++)
        {
            GetLookupMode((EnumLookupType)i, szTitle);
            m_wndMenu.AddItem(szTitle, IDMENU_Popup, (m_eLookupMode == (EnumLookupType)i?FLAG_Radio:NULL), i);
        }
        break;
    case GO_PhoneProfile:
        /*for(int i=PP_Normal; i<=PP_Silent3hours; i++)
        {
            GetPhoneProfileText((EnumPhoneProfile)i, szTitle);
            m_wndMenu.AddItem(szTitle, IDMENU_Popup, (m_objPhoneProfile.GetPhoneProfile(FALSE) == (EnumPhoneProfile)i?FLAG_Radio:NULL), i);
        }*/
        GetPhoneProfileText(PP_Loud, szTitle);
        m_wndMenu.AddItem(szTitle, IDMENU_Popup, (m_objPhoneProfile.GetPhoneProfile(FALSE) == PP_Loud?FLAG_Radio:NULL), PP_Loud);
        GetPhoneProfileText(PP_Normal, szTitle);
        m_wndMenu.AddItem(szTitle, IDMENU_Popup, (m_objPhoneProfile.GetPhoneProfile(FALSE) == PP_Normal?FLAG_Radio:NULL), PP_Normal);
        GetPhoneProfileText(PP_Quiet, szTitle);
        m_wndMenu.AddItem(szTitle, IDMENU_Popup, (m_objPhoneProfile.GetPhoneProfile(FALSE) == PP_Quiet?FLAG_Radio:NULL), PP_Quiet);
        GetPhoneProfileText(PP_Airplane, szTitle);
        m_wndMenu.AddItem(szTitle, IDMENU_Popup, (m_objPhoneProfile.GetPhoneProfile(FALSE) == PP_Airplane?FLAG_Radio:NULL), PP_Airplane);
        GetPhoneProfileText(PP_Vibrate, szTitle);
        m_wndMenu.AddItem(szTitle, IDMENU_Popup, (m_objPhoneProfile.GetPhoneProfile(FALSE) == PP_Vibrate?FLAG_Radio:NULL), PP_Vibrate);
        GetPhoneProfileText(PP_Silent, szTitle);
        m_wndMenu.AddItem(szTitle, IDMENU_Popup, (m_objPhoneProfile.GetPhoneProfile(FALSE) == PP_Silent?FLAG_Radio:NULL), PP_Silent);
        GetPhoneProfileText(PP_Silent3hours, szTitle);
        m_wndMenu.AddItem(szTitle, IDMENU_Popup, (m_objPhoneProfile.GetPhoneProfile(FALSE) == PP_Silent3hours?FLAG_Radio:NULL), PP_Silent3hours);
        break;
    default:
        return;
    }

    m_wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.right/4,m_rcTitle.bottom,
        rcClient.right*3/4, HEIGHT(m_rcArea),
        0,0,0,0, ADJUST_Bottom);
}

void CDlgGeneralOptions::SetSelection()
{
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    TypeItems* sMenuItem = m_wndMenu.GetSelectedItem();
    if(!sMenuItem || !sItem->lpItem)
        return;

    EnumGeneralOptions* eOption = (EnumGeneralOptions*)sItem->lpItem;

    switch(*eOption)
    {
    case GO_ShowIncomingCall:
    {
        m_dwValues &= ~FLAG_SHOW_IncomingCall;
        if((BOOL)sMenuItem->lParam)
            m_dwValues |= FLAG_SHOW_IncomingCall;

        // we have to stop and start CProg so it reloads everything
        EnumWindows((WNDENUMPROC)EnumCProgWindowsProc, 0);    // kill cprog
        // restart dialer hidden
        PROCESS_INFORMATION pi = {0};
        CreateProcess(_T("\\Windows\\cprog.exe"), _T("-n"), NULL, NULL, NULL, 0, NULL, NULL, NULL, &pi);
        break;
    }
    case GO_ShowPostCallScreen:
        m_dwValues &= ~FLAG_SHOW_PostCall;
        if((BOOL)sMenuItem->lParam)
            m_dwValues |= FLAG_SHOW_PostCall;
        break;
    case GO_ShowCheekGuard:
        m_dwValues &= ~FLAG_SHOW_Cheekguard;
        if((BOOL)sMenuItem->lParam)
            m_dwValues |= FLAG_SHOW_Cheekguard;
        break;
    case GO_LookupMode:
        m_eLookupMode = (EnumLookupType)sMenuItem->lParam;
        break;
    case GO_HandleRingtones:
        m_dwValues &= ~FLAG_SHOW_HandleRingtone;
        if((BOOL)sMenuItem->lParam)
            m_dwValues |= FLAG_SHOW_HandleRingtone;
        break;
    case GO_PhoneProfile:
        m_objPhoneProfile.SetPhoneProfile((EnumPhoneProfile)sMenuItem->lParam);
        break;
    default:
        return;
    }

    SaveRegValues();
}



