#include "DlgPerContactCall.h"
#include <phone.h>
#include "GuiCallLog.h"
#include "IssCommon.h"
#include "CommonDefines.h"

#define IDMENU_Call             6000
#define IDMENU_Details          6001
#define IDMENU_AddContact       6004

#define HEIGHT_Text             (GetSystemMetrics(SM_CXICON)*5/7)

CDlgPerContactCall::CDlgPerContactCall(BOOL bEnableDetails, CDlgContactDetails* dlg)
:m_iDuration(0)
,m_bEnableDetails(bEnableDetails)
,m_dlgDetails(dlg)
,m_szName(NULL)
,m_szNumber(NULL)
,m_szNameType(NULL)
,m_bIsRoaming(FALSE)
,m_bIsMissed(FALSE)
,m_bIsConnected(FALSE)
,m_bIsCallEnded(FALSE)
,m_bIsIncoming(FALSE)
,m_bUnknown(FALSE)
{
}

CDlgPerContactCall::~CDlgPerContactCall(void)
{
    m_oStr->Delete(&m_szName);
    m_oStr->Delete(&m_szNumber);
}

void CDlgPerContactCall::SetCallItem(CObjHistoryCallItem* oCallItem)
{
    m_oStr->Delete(&m_szName);
    m_oStr->Delete(&m_szNumber);
    m_oStr->Delete(&m_szNameType);
    m_bIsRoaming    = FALSE;
    m_bIsMissed     = FALSE;
    m_bIsConnected  = FALSE;
    m_bIsCallEnded  = FALSE;
    m_bIsIncoming   = FALSE;
    m_bUnknown      = FALSE;

    if(!oCallItem)
        return;

    if(oCallItem->GetName())
        m_szName    = m_oStr->CreateAndCopy(oCallItem->GetName());
    if(oCallItem->GetNumber())
        m_szNumber  = m_oStr->CreateAndCopy(oCallItem->GetNumber());
    if(oCallItem->GetNameType())
        m_szNameType = m_oStr->CreateAndCopy(oCallItem->GetNameType());

    m_bIsRoaming    = oCallItem->IsRoaming();
    m_bIsMissed     = oCallItem->IsMissed();
    m_bIsConnected  = oCallItem->IsConnected();
    m_bIsCallEnded  = oCallItem->IsCallEnded();
    m_bIsIncoming   = oCallItem->IsIncoming();
    m_bUnknown      = oCallItem->IsUnknown();
    m_ftStart       = oCallItem->GetStartTime();
    m_ftEnd         = oCallItem->GetEndTime();
}

void CDlgPerContactCall::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    EnumCalling* eOption = (EnumCalling*)lpItem;
    delete eOption;
}

void CDlgPerContactCall::PopulateList()
{
	if(!m_oMenu)
		return;

    m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_Bounce);
    m_oMenu->SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu->SetDeleteItemFunc(DeleteMyItem);
    m_oMenu->SetSelected(0);
    m_oMenu->ResetContent();

    if(m_iContactIndex == -1)
    {
        CPoomContacts* pPoom = CPoomContacts::Instance();
        m_iContactIndex = pPoom->FindContactFromName(m_szName);
        if(m_iContactIndex != -1)
            UpdateContact();
    }

    SYSTEMTIME sysTime;
    FILETIME ftLocal;
    int iStart, iEnd;
    FileTimeToLocalFileTime(&m_ftStart, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &sysTime);
    SystemTimeToUtc(sysTime, iStart);
    FileTimeToLocalFileTime(&m_ftEnd, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &sysTime);
    SystemTimeToUtc(sysTime, iEnd);
    m_iDuration = iEnd - iStart;

    for(int i=CALL_Type; i<= CALL_Roaming; i++)
    {
        if(i == CALL_PhoneType && m_oStr->IsEmpty(m_szNameType))
            continue;

        EnumCalling* eCall = new EnumCalling;
        *eCall = (EnumCalling)i;
        m_oMenu->AddItem(eCall, 5000);
        m_oMenu->AddSeparator();
    }

    m_oMenu->SetItemHeights(HEIGHT_Text, HEIGHT_Text);
    m_oMenu->SetSelectedItemIndex(0, TRUE);
}

void CDlgPerContactCall::OnSelectPopupMenuItem(int iIndex, TypeItems* sItem)
{
    if(sItem->lParam == IDMENU_AddContact)
    {
        ShowWindow(m_hWnd, SW_HIDE);
        ShowWindow(GetParent(m_hWnd), SW_HIDE);
        CPoomContacts::Instance()->CreateNew(m_hWnd, m_szNumber);
        ShowWindow(GetParent(m_hWnd), SW_SHOW);
        ShowWindow(m_hWnd, SW_SHOW);
    }
    else if(sItem->lParam == IDMENU_Call)
    {
        if(m_oStr->IsEmpty(m_szNumber))
            return;

        PHONEMAKECALLINFO mci = {0};
        mci.cbSize = sizeof(mci);
        mci.dwFlags = 0;
        mci.pszDestAddress = m_szNumber;
        PhoneMakeCall(&mci);
    }
    else if(sItem->lParam == IDMENU_Details)
    {
        if(!m_dlgDetails || !m_bEnableDetails || !IsValidOID(m_sContact.lOid))
            return;

        CPoomContacts* oPoom = CPoomContacts::Instance();
#ifdef FIX_ANNOYING_POOM_ONEOFF_ERROR
        int iIndex = oPoom->GetIndexFromOID(m_sContact.lOid);
#else
        int iIndex = oPoom->GetIndexFromOID(m_sContact.lOid) - 1;
#endif
        if(iIndex < 0)
            return;

        m_dlgDetails->SetIndex(iIndex);
        m_dlgDetails->SetAllowConvHistory(FALSE);   // don't allow them to go to conv history screen
        m_dlgDetails->Init(m_gdiMem, m_guiBackground);
        int iReturn = m_dlgDetails->Launch(m_hWnd, m_hInst, m_bAnimate);
        if(iReturn == IDOK)
        {
            return;
        }
    }
    else
        return;

    CloseWindow(IDCANCEL);
}

void CDlgPerContactCall::OnSelectMenuItem(int iIndex, TypeItems* sItem)
{
    // don't do anything
}

void CDlgPerContactCall::OnMenu()
{
    m_wndMenu->ResetContent();

    if(m_bUnknown)
        m_wndMenu->AddItem(_T("Add to contacts"), IDMENU_PopupMenu, NULL, IDMENU_AddContact);

    m_wndMenu->AddItem(_T("Call"), IDMENU_PopupMenu, m_oStr->IsEmpty(m_szNumber)?FLAG_Grayed:NULL, IDMENU_Call);

    BOOL bEnable = (m_bEnableDetails && m_dlgDetails && IsValidOID(m_sContact.lOid));
    m_wndMenu->AddSeparator();
    m_wndMenu->AddItem(_T("Contact details"), IDMENU_PopupMenu, bEnable?NULL:FLAG_Grayed, IDMENU_Details);

    m_wndMenu->SetSelectedItemIndex(0, TRUE);

    m_wndMenu->PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        GetSystemMetrics(SM_CXSCREEN)*1/4,0,
        GetSystemMetrics(SM_CXSCREEN)*3/4, GetSystemMetrics(SM_CYSCREEN) - HEIGHT(m_rcBottomBar),
        0, 0, 10,10, ADJUST_Bottom);
}

void CDlgPerContactCall::DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw)
{
    RECT rc = rcDraw;
    rc.bottom = rc.top + HEIGHT(rcDraw)/2;

    if(!m_oStr->IsEmpty(m_szNumber))
        DrawTextShadow(gdi, m_szNumber, rc, DT_LEFT|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rc.top  = rc.bottom;
    rc.bottom = rcDraw.bottom;

    TCHAR szText[STRING_MAX];

    // date/time
    SYSTEMTIME sysTime;
    FILETIME ftLocal;
    FileTimeToLocalFileTime(&m_ftStart, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &sysTime);
    m_oStr->Format(szText, _T("%d:%02d %s - %02d/%02d/%04d"), 
                            sysTime.wHour == 0 ? 12 : sysTime.wHour <= 12 ? sysTime.wHour : sysTime.wHour - 12, 
                            sysTime.wMinute, 
                            sysTime.wHour < 12 ? _T("AM") : _T("PM"),
                            sysTime.wMonth,
                            sysTime.wDay,
                            sysTime.wYear
                            );
    
    DrawTextShadow(gdi, szText, rc, DT_LEFT|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

void CDlgPerContactCall::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Call Details"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgPerContactCall::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgPerContactCall* pThis = (CDlgPerContactCall*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumCalling* eCall = (EnumCalling*)sItem->lpItem;

    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eCall);
}

BOOL CDlgPerContactCall::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumCalling* eCall)
{
    TCHAR szTitle[STRING_MAX] = _T("");
    TCHAR szInfo[STRING_MAX] = _T("");

    switch(*eCall)
    {
    case CALL_Roaming:
        m_oStr->StringCopy(szTitle, _T("In-call roaming:"));
        m_oStr->StringCopy(szInfo, m_bIsRoaming?_T("Yes"):_T("No"));
        break;
    case CALL_CallStatus:
        m_oStr->StringCopy(szTitle, _T("Call status:"));
        if(m_bIsMissed)
            m_oStr->StringCopy(szInfo, _T("None"));
        else
        {
            if(!m_bIsConnected)
                m_oStr->StringCopy(szInfo, _T("Busy/No answer"));
            else
                m_oStr->StringCopy(szInfo, m_bIsCallEnded?_T("Completed"):_T("Call dropped"));
        }
        break;
    case CALL_Duration:
        {
            m_oStr->StringCopy(szTitle, _T("Duration:"));
            int iDuration = m_iDuration;
            int	iHour = iDuration/3600;
            iDuration -= (iHour*3600);
            int iMinute = iDuration/60;
            iDuration -= (iMinute*60);
            m_oStr->Format(szInfo, _T("%02d:%02d"), iMinute, iDuration);
        }
        break;
    case CALL_Type:
        m_oStr->StringCopy(szTitle, _T("Type:"));
        if(m_bIsMissed)
            m_oStr->StringCopy(szInfo, _T("Missed call"));
        else
            m_oStr->StringCopy(szInfo, (m_bIsIncoming?_T("Incoming call"):_T("Outgoing call")));
        break;
    case CALL_PhoneType:
        m_oStr->StringCopy(szTitle, _T("Phone:"));

        if(m_oStr->Compare(m_szNameType, _T("w"))==0)
            m_oStr->StringCopy(szInfo, _T("Work"));
        else if(m_oStr->Compare(m_szNameType, _T("wfx"))==0)
            m_oStr->StringCopy(szInfo, _T("Work Fax"));
        else if(m_oStr->Compare(m_szNameType, _T("m"))==0)
            m_oStr->StringCopy(szInfo, _T("Mobile"));
        else if(m_oStr->Compare(m_szNameType, _T("h"))==0)
            m_oStr->StringCopy(szInfo, _T("Home"));
        else if(m_oStr->Compare(m_szNameType, _T("pgr"))==0)
            m_oStr->StringCopy(szInfo, _T("Pager"));
        else if(m_oStr->Compare(m_szNameType, _T("car"))==0)
            m_oStr->StringCopy(szInfo, _T("Car"));
        else if(m_oStr->Compare(m_szNameType, _T("hfx"))==0)
            m_oStr->StringCopy(szInfo, _T("Home Fax"));
        else if(m_oStr->Compare(m_szNameType, _T("co"))==0)
            m_oStr->StringCopy(szInfo, _T("Company"));
        else if(m_oStr->Compare(m_szNameType, _T("w2"))==0)
            m_oStr->StringCopy(szInfo, _T("Work 2"));
        else if(m_oStr->Compare(m_szNameType, _T("h2"))==0)
            m_oStr->StringCopy(szInfo, _T("Home 2"));
        else if(m_oStr->Compare(m_szNameType, _T("rdo"))==0)
            m_oStr->StringCopy(szInfo, _T("Radio"));
        else if(m_oStr->Compare(m_szNameType, _T("ast"))==0)
            m_oStr->StringCopy(szInfo, _T("Assistant"));
        else
            m_oStr->StringCopy(szInfo, _T("Unknown"));

        break;
    default:
        return FALSE;
    }

    rc.left += INDENT/2;
    rc.right-= INDENT/2;
    DrawTextShadow(gdi, szTitle, rc, DT_LEFT|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    DrawTextShadow(gdi, szInfo, rc, DT_RIGHT|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    return TRUE;
}