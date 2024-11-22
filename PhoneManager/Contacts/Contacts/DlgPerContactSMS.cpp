#include "DlgPerContactSMS.h"
#include <phone.h>
#include "CommonDefines.h"

#define IDMENU_Call             6000
#define IDMENU_SMS              6001
#define IDMENU_Details          6002

CDlgPerContactSMS::CDlgPerContactSMS(BOOL bEnableDetails, CDlgContactDetails* dlg)
:m_bEnableDetails(bEnableDetails)
,m_dlgDetails(dlg)
{
}

CDlgPerContactSMS::~CDlgPerContactSMS(void)
{
}

void CDlgPerContactSMS::SetSMSMessage(TypeSMSMessage* sSMS)
{
    m_sSMS.Clone(sSMS);

    // can we now mark this as read???
    CSMSInterface* oMapi = CSMSInterface::Instance();
    if(oMapi && !m_sSMS.bRead)
    {
        oMapi->SetSMSMessageRead(&m_sSMS);
    }


}

void CDlgPerContactSMS::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    int* iOption = (int*)lpItem;
    delete iOption;
}

void CDlgPerContactSMS::PopulateList()
{
    if(!m_oMenu || !m_sSMS.szSubject || !m_sSMS.szNumber)
        return;

    m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_Bounce);
    m_oMenu->SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu->SetDeleteItemFunc(DeleteMyItem);
    m_oMenu->SetSelected(0);
    m_oMenu->ResetContent();

    if(m_iContactIndex == -1)
    {
        CPoomContacts* pPoom = CPoomContacts::Instance();
        m_iContactIndex = pPoom->FindContactFromName(m_sSMS.szNumber);
        if(m_iContactIndex != -1)
            UpdateContact();
    }

    SIZE sizeTxt;
    HDC hDC = GetDC(m_hWnd);
    GetTextExtentPoint( hDC, m_sSMS.szSubject, (int)_tcslen( m_sSMS.szSubject ), &sizeTxt);
    ReleaseDC(m_hWnd, hDC);

    int* iVal = new int;
    m_oMenu->AddItem(iVal, IDMENU_Selection);
    TypeItems* sItem = m_oMenu->GetItem(0);
	if(!sItem)
		return;
    
    // change the height of the text
    sItem->iHeight      = sItem->iHeight*(sizeTxt.cx/GetSystemMetrics(SM_CXSCREEN) + 2);
    sItem->iHeightSel   = sItem->iHeightSel*(sizeTxt.cx/GetSystemMetrics(SM_CXSCREEN) + 2);

    m_oMenu->SetSelectedItemIndex(0, TRUE);
}

void CDlgPerContactSMS::OnSelectPopupMenuItem(int iIndex, TypeItems* sItem)
{
    TCHAR* szNumber = NULL;

    if(!m_oStr->IsEmpty(m_sContact.szMobileNumber))
        szNumber = m_sContact.szMobileNumber;
    else if(!m_oStr->IsEmpty(m_sSMS.szNumber))
        szNumber = m_sSMS.szNumber;
    else
        return;

    if(sItem->lParam == IDMENU_Call)
    {
        PHONEMAKECALLINFO mci = {0};
        mci.cbSize = sizeof(mci);
        mci.dwFlags = 0;
        mci.pszDestAddress = szNumber;
        PhoneMakeCall(&mci);
    }
    else if(sItem->lParam == IDMENU_SMS)
    {
        PROCESS_INFORMATION pi;
        TCHAR szCommandLine[STRING_MAX*2];
        m_oStr->Format(szCommandLine,  _T("-service \"SMS\" -to \"%s\""), szNumber);
        CreateProcess(_T("tmail.exe"), szCommandLine, NULL, NULL, FALSE, NULL, NULL, NULL, NULL, &pi);
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

void CDlgPerContactSMS::OnSelectMenuItem(int iIndex, TypeItems* sItem)
{
    // don't do anything
}

void CDlgPerContactSMS::OnMenu()
{
    m_wndMenu->ResetContent();

    m_wndMenu->AddItem(_T("SMS Reply"), IDMENU_PopupMenu, 0, IDMENU_SMS);
    m_wndMenu->AddItem(_T("Call"), IDMENU_PopupMenu, 0, IDMENU_Call);

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

void CDlgPerContactSMS::DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw)
{
    RECT rc = rcDraw;
    rc.bottom = rc.top + HEIGHT(rcDraw)/2;

    if(!m_oStr->IsEmpty(m_sContact.szMobileNumber))
        DrawTextShadow(gdi, m_sContact.szMobileNumber, rc, DT_LEFT|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    rc.top  = rc.bottom;
    rc.bottom = rcDraw.bottom;

    TCHAR szText[STRING_MAX];

    // date/time
    SYSTEMTIME sysTime;
    FILETIME ftLocal;
    FileTimeToLocalFileTime(&m_sSMS.ft, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &sysTime);
    m_oStr->Format(szText, _T("%d:%02d - %02d/%02d/%02d"), 
                            sysTime.wHour, 
                            sysTime.wMinute, 
                            //sysTime.wHour < 12 ? _T("am") : _T("pm"),
                            sysTime.wMonth,
                            sysTime.wDay,
                            sysTime.wYear - 2000
                            );
    m_oStr->Insert(szText, (m_sSMS.bReceived?_T("Received: "):_T("Sent: ")));
    DrawTextShadow(gdi, szText, rc, DT_LEFT|DT_VCENTER, m_hFontTimeDate, RGB(255,255,255), RGB(0,0,0));
}

void CDlgPerContactSMS::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("SMS Message"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgPerContactSMS::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgPerContactSMS* pThis = (CDlgPerContactSMS*)lpClass;
    if(!pThis)
        return;

    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw);
}

BOOL CDlgPerContactSMS::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc)
{
    if(m_oStr->IsEmpty(m_sSMS.szSubject))
        return TRUE;

    DrawText(gdi, m_sSMS.szSubject, rc, DT_LEFT|DT_WORDBREAK|DT_NOPREFIX, m_hFontLabel, RGB(255,255,255));
    return TRUE;
}