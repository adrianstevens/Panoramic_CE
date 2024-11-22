#include "DlgPerContactConv.h"
#include <phone.h>
#include "CommonDefines.h"
#include "SMSMessaging.h"
#include "DlgPerContactSMS.h"
#include "DlgPerContactCall.h"
#include "DlgContacts.h"
#include "Resource.h"

#define IDMENU_Call             6000
#define IDMENU_SMS              6001
#define IDMENU_Details          6002

CDlgPerContactConv::CDlgPerContactConv(BOOL bEnableDetails, CDlgContactDetails* dlg)
:m_bEnableDetails(bEnableDetails)
,m_dlgDetails(dlg)
,m_iLastListIndex(-1)
{
}

CDlgPerContactConv::~CDlgPerContactConv(void)
{
}

void CDlgPerContactConv::SetContact(CObjHistoryItem* sItem, TypeOptions* sOptions, CIssGDIEx* gdiIconArray)
{
	m_sOptions = sOptions;
	m_szDefaultName = sItem->GetName();
	m_szDefaultNumber = sItem->GetNumber();

	if(m_oStr->GetLength(m_szDefaultName) && m_oStr->Compare(m_szDefaultName, _T("Unknown"))!=0)
		SetNameOverride(m_szDefaultName);
	else if(m_oStr->GetLength(m_szDefaultNumber))
		SetNameOverride(m_szDefaultNumber);

	if(m_iContactIndex == -1)
	{
		CPoomContacts* pPoom = CPoomContacts::Instance();
		m_iContactIndex = pPoom->FindContactFromName(sItem->GetName());
		if(m_iContactIndex != -1)
			UpdateContact();
	}
}
void CDlgPerContactConv::SetContact(TypeContact* sItem, TypeOptions* sOptions, CIssGDIEx* gdiIconArray)
{
	m_sOptions = sOptions;
	CPoomContacts* pPoom = CPoomContacts::Instance();
	m_iContactIndex = pPoom->GetIndexFromOID(sItem->lOid);
	UpdateContact();

	// build name string
    m_oStr->Delete(&m_szDefaultName);
	m_szDefaultName = pPoom->GetUserData(CI_FileAs, sItem->lOid);
    if(!m_szDefaultName)
    {
        MessageBeep(MB_ICONHAND);
        return;
    }

	// get a default number
	TCHAR* szNumber;
	if(m_oStr->GetLength(m_sContact.szHomeNumber))
		szNumber = m_sContact.szHomeNumber;
	else if(m_oStr->GetLength(m_sContact.szWorkNumber))
		szNumber = m_sContact.szWorkNumber;
	else if(m_oStr->GetLength(m_sContact.szMobileNumber))
		szNumber = m_sContact.szMobileNumber;
    m_oStr->Delete(&m_szDefaultNumber);
	m_szDefaultNumber = m_oStr->CreateAndCopy(szNumber);

	if(m_oStr->GetLength(m_szDefaultName) && m_oStr->Compare(m_szDefaultName, _T("Unknown"))!=0)
		SetNameOverride(m_szDefaultName);
	else if(m_oStr->GetLength(m_szDefaultNumber))
		SetNameOverride(m_szDefaultNumber);
}

void CDlgPerContactConv::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    CObjHistoryItem* objHistoryItem = (CObjHistoryItem*)lpItem;
    delete objHistoryItem;
}

void CDlgPerContactConv::PopulateList()
{
	if(!m_oMenu)
		return;

    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    m_oMenu->SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_Bounce);
	m_oMenu->SetSelected(IDR_PNG_ContactsSelector);
    m_oMenu->SetCustomDrawFunc(DrawListItem, this);
    m_oMenu->SetDeleteItemFunc(DeleteMyItem);
    m_oMenu->SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu->ResetContent();
	m_oMenu->PreloadImages(m_hWnd, m_hInst);

    ////////////////////////////////////////////////////
    // add all necessary items
    CIssKineticList* pSrcList = CGuiCallLog::GetCallList();
    if(pSrcList)
    {
        for(int i = 0; i < pSrcList->GetItemCount(); i++)
        {
            BOOL bFound = FALSE;
            TypeItems* sItem = pSrcList->GetItem(i); 
            if(sItem)
            {
                CObjHistoryItem* oHistoryItem = (CObjHistoryItem*)sItem->lpItem;
                if(oHistoryItem)
                {
                    if(m_oStr->Compare(m_szNameOverride, oHistoryItem->GetName()) == 0)
                        bFound = TRUE;
                    if(!bFound && m_oStr->Compare(m_szNameOverride, oHistoryItem->GetNumber()) == 0)
                        bFound = TRUE;
                }
                if(bFound)
                {
                    // create our own - source can be deleted any time
                    CObjHistoryItem* pNewItem = oHistoryItem->Clone();
                    if(pNewItem)
                        m_oMenu->AddItem(pNewItem, pNewItem->GetType() == HI_Call ? IDMENU_SelectCall : IDMENU_SelectSMS);
                }
            }
        }
    }

    int iLineSpace = IsVGA() ? 8 : 4;
    int iLineHeight	= GetSystemMetrics(SM_CXSMICON);
    m_oMenu->SetItemHeights(iLineHeight*2 + iLineSpace, iLineHeight*3 + iLineSpace);

    DWORD dwFlags   = 0;
    if(m_oMenu->GetItemCount() > 20)
        dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar);
    m_oMenu->Initialize(GetWnd(), GetWnd(), m_hInst, dwFlags, TRUE);

    // select something
	if(m_iLastListIndex > -1 && m_iLastListIndex < m_oMenu->GetItemCount())
		m_oMenu->SetSelectedItemIndex(m_iLastListIndex, TRUE);
	else
		m_oMenu->SetSelectedItemIndex(0, TRUE);
    m_oMenu->ShowSelectedItem(FALSE);

    ::SetCursor(hCursor);
}

void CDlgPerContactConv::OnSelectPopupMenuItem(int iIndex, TypeItems* sItem)
{
    TCHAR* szNumber = NULL;

	if(!m_oStr->IsEmpty(m_szDefaultNumber))
		szNumber = m_szDefaultNumber;
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

void CDlgPerContactConv::OnSelectMenuItem(int iIndex, TypeItems* sItem)
{
    // don't do anything
}

void CDlgPerContactConv::OnMenu()
{
    m_wndMenu->ResetContent();

	if(!m_oMenu)
		return;

    TypeItems* sItem = m_oMenu->GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    CObjHistoryItem* oCall = (CObjHistoryItem*)sItem->lpItem;

	UINT uiReplyFlags = 0;
    if(oCall->GetType() == HI_Call)
        uiReplyFlags = FLAG_Grayed;

	m_wndMenu->AddItem(_T("SMS Reply"), IDMENU_PopupMenu, uiReplyFlags, IDMENU_SMS);
	m_wndMenu->AddItem(_T("Call"), IDMENU_PopupMenu, 0, IDMENU_Call);

	m_wndMenu->AddSeparator();
    BOOL bEnable = (m_bEnableDetails && m_dlgDetails && IsValidOID(m_sContact.lOid));
    m_wndMenu->AddItem(_T("Contact details"), IDMENU_PopupMenu, bEnable?NULL:FLAG_Grayed, IDMENU_Details);

    m_wndMenu->SetSelectedItemIndex(0, TRUE);

    m_wndMenu->PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        GetSystemMetrics(SM_CXSCREEN)*1/4,0,
        GetSystemMetrics(SM_CXSCREEN)*3/4, GetSystemMetrics(SM_CYSCREEN) - HEIGHT(m_rcBottomBar),
        0, 0, 10,10, ADJUST_Bottom);
}

void CDlgPerContactConv::DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw)
{
    RECT rc = rcDraw;
    //rc.bottom = rc.top + HEIGHT(rcDraw)/2;

    if(m_sContact.szCompany)
    {
        if(m_oStr->GetLength(m_sContact.szCompany))
            DrawTextShadow(gdi, m_sContact.szCompany, rc, DT_LEFT|DT_WORDBREAK|DT_NOPREFIX, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }
}

void CDlgPerContactConv::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Conversation History"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgPerContactConv::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgPerContactConv* pThis = (CDlgPerContactConv*)lpClass;
    if(!pThis)
        return;

    pThis->DrawListItem(gdi, sItem, bIsHighlighted, rcDraw);
}

BOOL CDlgPerContactConv::DrawListItem(CIssGDIEx& gdi, TypeItems* sItem, BOOL bIsHighlighted, RECT& rc)
{

    CObjHistoryItem* objHistoryItem = (CObjHistoryItem*)sItem->lpItem;
    if(objHistoryItem)
        objHistoryItem->Draw(gdi, CDlgContacts::GetCallIcons(), rc, bIsHighlighted, RGB(255,255,255), FALSE);

    return TRUE;
}

BOOL CDlgPerContactConv::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case IDMENU_SelectSMS:
		{
			if(!m_oMenu)
				break;
			m_iLastListIndex = m_oMenu->GetSelectedItemIndex();
			OnSelectSMS();
			PopulateList();
		}
        break;
    case IDMENU_SelectCall:
		{
			if(!m_oMenu)
				break;
			m_iLastListIndex = m_oMenu->GetSelectedItemIndex();
			OnSelectCall();
			PopulateList();
		}
        break;
    default:
        CDlgPerContactBase::OnCommand(hWnd, wParam, lParam);
        break;
    }
	return TRUE;
}
void CDlgPerContactConv::OnSelectSMS()
{
	if(!m_oMenu)
		return;
    TypeItems* sItem = m_oMenu->GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    CObjHistorySMSItem* oSMS = (CObjHistorySMSItem*)sItem->lpItem;
    TypeSMSMessage* sSMS = oSMS->GetSMS();
    if(!sSMS)
        return;

    CDlgPerContactSMS dlg(TRUE, m_dlgDetails);
    dlg.SetSMSMessage(sSMS);
    dlg.SetNameOverride(m_szDefaultNumber/*sSMS->szNumber*/);    // for now until we get the OID out
    dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
    dlg.Launch(GetWnd(), m_hInst, m_bAnimate);
    sSMS->bRead = TRUE;
}

void CDlgPerContactConv::OnSelectCall()
{
	if(!m_oMenu)
		return;
    TypeItems* sItem = m_oMenu->GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    CObjHistoryCallItem* oCall = (CObjHistoryCallItem*)sItem->lpItem;

    CDlgPerContactCall dlg(TRUE, m_dlgDetails);
    dlg.SetCallItem(oCall);
    dlg.SetNameOverride(oCall->GetName());    // for now until we get the OID out
    dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
    dlg.Launch(GetWnd(), m_hInst, m_bAnimate);
}
    
