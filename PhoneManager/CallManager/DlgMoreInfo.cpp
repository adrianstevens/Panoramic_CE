#include "StdAfx.h"
#include "DlgMoreInfo.h"

#define HEIGHT_Text             (GetSystemMetrics(SM_CXICON)*5/7)

CDlgMoreInfo::CDlgMoreInfo(void)
:m_iDuration(0)
,m_szNumber(NULL)
,m_szNameType(NULL)
,m_szLocation(NULL)
,m_bIsRoaming(FALSE)
,m_bIsMissed(FALSE)
,m_bIsConnected(FALSE)
,m_bIsCallEnded(FALSE)
,m_bIsIncoming(FALSE)
,m_bUnknown(FALSE)
{
}

CDlgMoreInfo::~CDlgMoreInfo(void)
{
    m_oStr->Delete(&m_szNumber);
    m_oStr->Delete(&m_szNameType);
    m_oStr->Delete(&m_szLocation);
}

void CDlgMoreInfo::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    EnumCalling* eOption = (EnumCalling*)lpItem;
    delete eOption;
}

void CDlgMoreInfo::SetCallItem(TCHAR* szName,
                               TCHAR* szNumber,
                               TCHAR* szNameType,
                               TCHAR* szLocation,
                               BOOL   bIsRoaming,
                               BOOL   bIsMissed,
                               BOOL   bIsConnected,
                               BOOL   bIsCallEnded,
                               BOOL   bIsIncoming,
                               BOOL   bIsUnknown,
                               int    iDurration,
                               SYSTEMTIME sStartTime,
                               LONG   lOid)
{
    m_oStr->Delete(&m_szNameOverride);
    m_oStr->Delete(&m_szNumber);
    m_oStr->Delete(&m_szNameType);
    m_oStr->Delete(&m_szLocation);
    m_bIsRoaming    = FALSE;
    m_bIsMissed     = FALSE;
    m_bIsConnected  = FALSE;
    m_bIsCallEnded  = FALSE;
    m_bIsIncoming   = FALSE;
    m_bUnknown      = FALSE;

    if(szName)
        m_szNameOverride    = m_oStr->CreateAndCopy(szName);
    if(szNumber)
        m_szNumber  = m_oStr->CreateAndCopy(szNumber);
    if(szNameType)
        m_szNameType = m_oStr->CreateAndCopy(szNameType);
    if(szLocation)
        m_szLocation = m_oStr->CreateAndCopy(szLocation);

    m_bIsRoaming    = bIsRoaming;
    m_bIsMissed     = bIsMissed;
    m_bIsConnected  = bIsConnected; 
    m_bIsCallEnded  = bIsCallEnded;
    m_bIsIncoming   = bIsIncoming;
    m_bUnknown      = bIsUnknown;

    m_iDuration     = iDurration;

    ZeroMemory(&m_ftStart, sizeof(FILETIME));

    SystemTimeToFileTime(&sStartTime, &m_ftStart);

    CPoomContacts* oPoom = CPoomContacts::Instance();
    m_iContactIndex = oPoom->GetIndexFromOID(lOid);
    UpdateContact();
}

void CDlgMoreInfo::PopulateList()
{
    if(!m_oMenu)
        return;

    m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, 0);
    m_oMenu->SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu->SetDeleteItemFunc(DeleteMyItem);
    m_oMenu->SetSelected(0);
    m_oMenu->ResetContent();

    for(int i=CALL_Type; i<= CALL_Location; i++)
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

void CDlgMoreInfo::DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw)
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
 //   FILETIME ftLocal;
 //   FileTimeToLocalFileTime(&m_ftStart, &ftLocal);
    FileTimeToSystemTime(&m_ftStart, &sysTime);
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

void CDlgMoreInfo::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("More Info"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgMoreInfo::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgMoreInfo* pThis = (CDlgMoreInfo*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumCalling* eCall = (EnumCalling*)sItem->lpItem;

    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eCall);
}


BOOL CDlgMoreInfo::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumCalling* eCall)
{
    TCHAR szTitle[STRING_MAX] = _T("");
    TCHAR szInfo[STRING_MAX] = _T("");

    switch(*eCall)
    {
    /*case CALL_Roaming:
        m_oStr->StringCopy(szTitle, _T("In-call roaming:"));
        m_oStr->StringCopy(szInfo, m_bIsRoaming?_T("Yes"):_T("No"));
        break;*/
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
    case CALL_Location:
        m_oStr->StringCopy(szTitle, _T("Location:"));
        if(m_oStr->IsEmpty(m_szLocation) == FALSE)
            m_oStr->StringCopy(szInfo, m_szLocation);
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

void CDlgMoreInfo::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    if(IsRectInRect(rc, rcClip))
    {
        DrawTextShadow(gdi, _T("Back"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }
 }