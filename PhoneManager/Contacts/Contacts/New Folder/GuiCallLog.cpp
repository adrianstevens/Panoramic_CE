#include "StdAfx.h"
#include "GuiCallLog.h"
#include "ContactsGuiDefines.h"
#include "DlgContactDetails.h"
#include "Resource.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "IssDebug.h"
#include "GuiToolBar.h"



CGuiCallLog::CGuiCallLog(void) 
{
	m_iLineHeight			= GetSystemMetrics(SM_CXSMICON);//GetSystemMetrics(SM_CXICON)/2;
	////m_iTitleHeight			= (int)(m_iLetterHeight * 1.5);

	//m_hFontNormal			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
 //	m_hFontBold		        = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);

	m_ptScreenSize.x		= 0;
	m_ptScreenSize.y		= 0;
}

CGuiCallLog::~CGuiCallLog(void)
{
	//CIssGDIEx::DeleteFont(m_hFontNormal);
	//CIssGDIEx::DeleteFont(m_hFontBold);
	EraseList();
}

BOOL CGuiCallLog::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions/*, CDlgContactDetails* dlgContactDetails*/)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBg, guiBackground, sOptions);

    //m_dlgContactDetails = dlgContactDetails;

    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    Initialize(hWndParent, hWndParent, m_hInst, 0);
    SetCustomDrawFunc(DrawListItem, this);
	SetDeleteItemFunc(DeleteCallItem);
    SetImageArray(SKIN(IDR_PNG_MenuArray));
	SetSelected(IDR_PNG_ContactsSelector);

    // preload list images
	PreloadImages(hWndParent, m_hInst);

    UINT uiRes = IsVGA() ? IDR_PNG_HistoryArrayVGA : IDR_PNG_HistoryArray;
    m_gdiImgIconArray.LoadImage(uiRes, hWndParent, hInst, TRUE);

    // initialize calllog
    //m_oCallLog.Initialize();

	PopulateList();
	return TRUE;
}

void CGuiCallLog::DeleteCallItem(LPVOID lpItem)
{
	if(!lpItem)
		return;
	CObjHistoryItem* objHistoryItem = (CObjHistoryItem*)lpItem;
	delete objHistoryItem;
}
    
BOOL CGuiCallLog::HasFocus()
{
    return !CGuiToolBar::GetToolbar()->HasFocus();
}

CIssImageSliced& CGuiCallLog::GetBorder()
{
    if(!m_imgBorder.IsLoaded())
        m_imgBorder.Initialize(m_hWndParent, m_hInst, IsVGA() ? IDR_PNG_ContactsBorderVGA : IDR_PNG_ContactsBorder);

    if(WIDTH(m_rcHistoryList) != m_imgBorder.GetWidth() || HEIGHT(m_rcHistoryList) != m_imgBorder.GetHeight())
        m_imgBorder.SetSize(WIDTH(m_rcHistoryList), HEIGHT(m_rcHistoryList));

    return m_imgBorder;
}
        
CIssGDIEx& CGuiCallLog::GetIconArray()
{
    if(!m_gdiImgIconArray.GetDC())
    {
        UINT uiRes = IsVGA() ? IDR_PNG_HistoryArrayVGA : IDR_PNG_HistoryArray;
        m_gdiImgIconArray.LoadImage(uiRes, m_hWndParent, m_hInst, TRUE);
    }

    return m_gdiImgIconArray;
}

BOOL CGuiCallLog::Draw(HDC hDC, RECT& rcClient, RECT& rcClip)
{
    if(!IsRectInRect(m_rcLocation, rcClip))
        return TRUE;

    DBG_CONTACTS(_T("CGuiCallLog::Draw()"), TRUE);
    /*if(m_gdiBg->GetDC() == NULL)
    {
        RECT rc;
        ::GetWindowRect(m_hWndParent, &rc);
		m_gdiBg->Create(hDC, rc);
    }*/
    //m_guiBackground->DrawBackground(*m_gdiMem, rcClip, rcClient);
    RECT rcClipClient = rcClip;
    rcClipClient.right = min(rcClip.right, rcClient.right);
    rcClipClient.bottom = min(rcClip.bottom, rcClient.bottom);
    m_guiBackground->DrawBackground(*m_gdiBg, rcClipClient, rcClient);

	//DrawBackground(m_gdiBg->GetDC(), m_rcLocation, rcClip);

    if(!m_arrItems.GetSize())
        DrawText(m_gdiBg->GetDC(), _T("no entries"), m_rcList, DT_CENTER | DT_VCENTER, /*m_hFontNormal*/0, m_crText);
    else
        CIssKineticList::OnDraw(*m_gdiBg, rcClient, rcClip);

    // draw the top part of the border only here
    //DrawBorderOverlay(*m_gdiBg, rcClient, rcClip);
    m_guiBackground->DrawTopSlice(*m_gdiBg, rcClipClient, rcClient);

    RECT rcBlit = rcClip;
    rcBlit.bottom = min(rcBlit.bottom, m_rcLocation.bottom);
    rcBlit.left = max(rcBlit.left, m_rcLocation.left);
    BitBlt(hDC,
		rcBlit.left,rcBlit.top,
		WIDTH(rcBlit), HEIGHT(rcBlit),
		m_gdiBg->GetDC(),
		rcBlit.left,rcBlit.top,
		SRCCOPY);

    DBG_CONTACTS(_T("CGuiCallLog::Draw()"), FALSE);
	return TRUE;
}
	
BOOL CGuiCallLog::DrawBorderOverlay(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rcBorder = m_rcHistoryList;
    rcBorder.bottom = rcBorder.top + CONTACTS_LIST_INSET;
    if(!IsRectInRect(rcBorder, rcClip))
        return TRUE;

    ::Draw(gdi, rcBorder, GetBorder().GetImage());

    return TRUE;
}
/* 
CIssGDIEx& CGuiCallLog::GetBgGDI()
{
	if(m_gdiBackground.GetDC() == NULL || m_gdiBackground.GetWidth() != WIDTH(m_rcLocation) ||
        m_gdiBackground.GetHeight() != HEIGHT(m_rcLocation))
	{
		m_gdiBackground.Create(m_gdiMem->GetDC(), m_rcLocation, FALSE);

        // background gradients
        RECT rcGDI = {0,0,m_gdiBackground.GetWidth(), m_gdiBackground.GetHeight()};
        RECT rc1, rc2;
        rc1 = rc2 = rcGDI;
        rc1.right = rc1.left + (long)(WIDTH(m_rcHistoryList) * 0.70f);
        rc2.left = rc1.right;
        
        GradientFillRect(m_gdiBackground, rc1, COLOR_CONTACTS_BG_GRAD1_ST, COLOR_CONTACTS_BG_GRAD1_END, FALSE);
        GradientFillRect(m_gdiBackground, rc2, COLOR_CONTACTS_BG_GRAD2_ST, COLOR_CONTACTS_BG_GRAD2_END, FALSE);

        // draw the left and right border parts
        RECT rcBorder = m_rcHistoryList;
        rcBorder.top += CONTACTS_LIST_INSET;
        ::Draw(m_gdiBackground, rcBorder, GetBorder().GetImage(), rcBorder.left, rcBorder.top);
	}
    return m_gdiBackground;
}
*/
/*
BOOL CGuiCallLog::DrawBackground(HDC hdc, RECT& rcClient, RECT& rcClip)
{

    RECT rc = rcClip;
    rc.bottom = min(rc.bottom, rcClient.bottom);
    rc.left = max(rc.left, rcClient.left);

	return BitBlt(hdc,
		rc.left,rc.top,
		WIDTH(rc), HEIGHT(rc),
		m_gdiBackground.GetDC(),
		rc.left - m_rcLocation.left,rc.top,
		SRCCOPY);
}
*/
void CGuiCallLog::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{
    if(sItem->eType != ITEM_Unknown)
        return;

    CObjHistoryItem* objHistoryItem = (CObjHistoryItem*)sItem->lpItem;
    if(!objHistoryItem)
        return;

    objHistoryItem->Draw(gdi, GetIconArray(), rcDraw, bIsHighlighted, bIsHighlighted ? m_crHighlight : m_crText);
    /*
    TypeHistoryItem* pHistoryItem = (TypeHistoryItem*)sItem->lpItem;
    if(!pHistoryItem)
        return;

    static TCHAR szText[STRING_LARGE];
    m_oStr->Empty(szText);

    // icon
    int iIconWidth = GetIconArray().GetWidth() / NUM_HISTORY_ICONS;
    RECT rcIcon = rcDraw;
    rcIcon.bottom = rcIcon.top + GetIconArray().GetHeight();
    rcIcon.right = rcIcon.left + iIconWidth;
    DrawListIcon(gdi, pHistoryItem->eType, pHistoryItem->bIsIncoming, pHistoryItem->bMissed, rcIcon);

    //////////////////////////////////////////////////////////////
    // line 1
    // name
    RECT rcText = rcDraw;
    rcText.left += iIconWidth + INDENT*2;
    rcText.right -=INDENT;
    rcText.bottom = rcText.top + m_iLineHeight;

    DrawText(gdi.GetDC(), pHistoryItem->szName, rcText, DT_LEFT | DT_VCENTER, m_hFontBold, bIsHighlighted ? m_crHighlight : m_crText);
    
    // date/time
    SYSTEMTIME sysTime;
    FILETIME ftLocal;
    FileTimeToLocalFileTime(&pHistoryItem->ftTime, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &sysTime);
    m_oStr->Format(szText, _T("%d:%02d %s"), sysTime.wHour == 0 ? 12 : sysTime.wHour <= 12 ? sysTime.wHour : sysTime.wHour - 12, 
                            sysTime.wMinute, 
                            sysTime.wHour < 12 ? _T("a") : _T("p"));
    DrawText(gdi.GetDC(), szText, rcText, DT_RIGHT | DT_VCENTER, m_hFontNormal, bIsHighlighted ? m_crHighlight : m_crText);
    
    ///////////////////////////////////////////////////////////
    // line 2
    // phone number
    OffsetRect(&rcText, 0, m_iLineHeight);
    rcText.left += INDENT*2;
    DrawText(gdi.GetDC(), pHistoryItem->szNumber, rcText, DT_LEFT | DT_VCENTER, m_hFontNormal, bIsHighlighted ? m_crHighlight : m_crText);

    // date
    m_oStr->Format(szText, _T("%02d/%02d/%02d"), sysTime.wMonth, sysTime.wDay, sysTime.wYear);
    DrawText(gdi.GetDC(), szText, rcText, DT_RIGHT | DT_VCENTER, m_hFontNormal, bIsHighlighted ? m_crHighlight : m_crText);

    // other stuff

    RECT rcLine = rcDraw;
    rcLine.top = rcLine.bottom - 1;// for now
    ::Line(gdi.GetDC(), rcLine.left + 4, rcLine.top, rcLine.right - 4, rcLine.top, 0x454545);

    //if(bIsHighlighted)
    //{
    //}
    */
}
/*    
void CGuiCallLog::DrawListIcon(CIssGDIEx& gdi, EnumHistoryItemType eType, BOOL bIncoming, BOOL bMissed, RECT rc)
{
    int iIconIndex = 0;
    switch(eType)
    {
    case HI_Call:  
        iIconIndex = bMissed ? 4 : bIncoming ? 2 : 3;
            break;
    case HI_SMS:         
        iIconIndex = bIncoming ? 0 : 1;
            break;
    case HI_Email:  
        iIconIndex = bIncoming ? 5 : 6;
            break;
        break;
    }

    int iIconWidth = GetIconArray().GetWidth() / NUM_HISTORY_ICONS;
    ::Draw(gdi, rc, GetIconArray(), iIconIndex * iIconWidth, 0);
}
*/

void CGuiCallLog::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CGuiCallLog* pGuiCallLog = (CGuiCallLog*)lpClass;
    pGuiCallLog->DrawListItem(gdi, rcDraw, sItem, bIsHighlighted);
}

BOOL CGuiCallLog::OnTimer(WPARAM wParam, LPARAM lParam)
{
	CIssKineticList::OnTimer(wParam, lParam);

	return UNHANDLED;
}

BOOL CGuiCallLog::SelecteItem(int iIndex)
{

	return TRUE;
}

BOOL CGuiCallLog::MoveGui(RECT rcLocation)
{
    if(EqualRect(&rcLocation, &m_rcLocation))
        return TRUE;

	CGuiBase::MoveGui(rcLocation);

    DBG_CONTACTS(_T("CGuiCallLog::MoveGui()"), TRUE);

    m_rcHistoryList  = rcLocation;
    int iListInset = (IsVGA() ? CONTACTS_LIST_INSET * 2 : CONTACTS_LIST_INSET);
    CIssKineticList::OnSize(m_rcHistoryList.left + iListInset, 
        rcLocation.top + iListInset/2, 
        WIDTH(m_rcHistoryList) - iListInset*2, 
        HEIGHT(m_rcHistoryList) - iListInset/2
        );

    // reload images
    //GetBgGDI();
    GetBorder();

    DBG_CONTACTS(_T("CGuiCallLog::MoveGui()"), FALSE);
	return TRUE;
}

void CGuiCallLog::DrawSelector(CIssGDIEx& gdi, RECT& rcDraw)
{
    // fine-tune the RECT
    RECT rcSelector = rcDraw;
    int iIconIndent = GetIconArray().GetWidth() / NUM_HISTORY_ICONS;
    rcSelector.left += (iIconIndent + INDENT/2);
    rcSelector.bottom -= INDENT/2;
    CIssKineticList::DrawSelector(gdi, rcSelector);
}


BOOL CGuiCallLog::OnLButtonDown(POINT pt)
{
	//if(PtInRect(&m_rcLocation, pt) == FALSE)
	//	return FALSE;

	m_ptMouseDown = pt;

    return CIssKineticList::OnLButtonDown(pt);
}

BOOL CGuiCallLog::OnLButtonUp(POINT pt)
{
	//if(PtInRect(&m_rcLocation, pt) == FALSE)
	//	return FALSE;

    BOOL bRet = (m_eMouse != GRAB_None?TRUE:FALSE);
    bRet |=  CIssKineticList::OnLButtonUp(pt);
    return bRet;
}

BOOL CGuiCallLog::OnMouseMove(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

    return CIssKineticList::OnMouseMove(pt);
}

BOOL CGuiCallLog::OnChar(WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == _T('q'))
	{
		EndModalDialog(m_hWndParent, 0);
		return 1;
	}
	if(LOWORD(wParam) == _T('d'))
	{
		CDlgContactDetails dlgDetails;
		dlgDetails.DoModal(m_hWndParent, m_hInst, IDD_DLG_Basic);
	}

	return UNHANDLED;
}

BOOL CGuiCallLog::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(HasFocus())
        return CIssKineticList::OnKeyDown(wParam, lParam);

    return FALSE;
}

BOOL CGuiCallLog::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}
   
BOOL CGuiCallLog::HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    switch(uiMessage)
    {
    //case PIM_ITEM_CREATED_LOCAL: 
    //case PIM_ITEM_CREATED_REMOTE: 
    //    PoomItemAdded(wParam);
    //    break;
    //case PIM_ITEM_DELETED_LOCAL: 
    //case PIM_ITEM_DELETED_REMOTE: 
    //    PoomItemDeleted(wParam);
    //    break;
    //case PIM_ITEM_CHANGED_LOCAL: 
    //case PIM_ITEM_CHANGED_REMOTE: 
    //    PoomItemChanged(wParam);
        break;
    }
    return TRUE;
}


void CGuiCallLog::RefreshList(void)
{
	EraseList();
	PopulateList();
}
  

BOOL CGuiCallLog::NeedMoreHistoryItems(int iCount)
{
    if(m_sOptions->eShowHistory == HISTORY_Last100)
    {
        return (iCount <= 100);
    }
    else if(m_sOptions->eShowHistory == HISTORY_Last200)
    {
        return (iCount <= 200);
    }
    else    // HISTORY_All
        return TRUE;

}

BOOL CGuiCallLog::CheckFilters(CALLLOGENTRY& ce)
{
    if(!m_sOptions)
        return FALSE;

    if(m_sOptions->eShowCall == CALL_Incoming && ce.iom != IOM_INCOMING)
        return false;
    else if(m_sOptions->eShowCall == CALL_Outgoing && ce.iom != IOM_OUTGOING)
        return false;
    else if(m_sOptions->eShowCall == CALL_Missed && ce.iom != IOM_MISSED)
        return false;

    return TRUE;
}

BOOL CGuiCallLog::CheckCallDate(CALLLOGENTRY& ce)
{
    if(!m_sOptions)
        return FALSE;

    if(m_sOptions->eShowHistory == HISTORY_LastMonth)
    {
        SYSTEMTIME st;
        GetSystemTime(&st);

        st.wMonth -= 1;
        if(st.wMonth < 1)
        {
            st.wMonth = 12;
            st.wYear --;
        }

        FILETIME ftCompare, ftLocal;
        SystemTimeToFileTime(&st, &ftCompare);
        FileTimeToLocalFileTime(&ce.ftStartTime, &ftLocal);

        return (CompareFileTime(&ftLocal, &ftCompare) > 0);
    }
    return TRUE;
}

void CGuiCallLog::DestroyCallEntry(CALLLOGENTRY& ce)
{
    LocalFree(ce.pszName);
    LocalFree(ce.pszNameType);
    LocalFree(ce.pszNote);
    LocalFree(ce.pszNumber);
}

void CGuiCallLog::CollectCallItems(void)
{
    PhoneOpenCallLog(&m_hCallLog);

    int iCount = 0;
    FILETIME ftEntry;
    while(TRUE)
    {
        CALLLOGENTRY ce;
	    ce.cbSize = sizeof(CALLLOGENTRY);
	    if(S_OK == PhoneGetCallLogEntry(m_hCallLog, &ce))
        {
            // check if item is beyond expiry date
            if(!CheckFilters(ce))
            {
                // skip this one
                DestroyCallEntry(ce);
                continue;
            }
            if(!CheckCallDate(ce))
            {
                // skip all the rest
                DestroyCallEntry(ce);
                break;            }
            else
            {
                CObjHistoryCallItem* objItem = new CObjHistoryCallItem();
                objItem->SetProperties(!ce.fOutgoing, (ce.iom == IOM_MISSED), ce.fRoam); 
                objItem->SetTimes(ce.ftStartTime, ce.ftEndTime);

                if(ce.pszNumber)
                    objItem->SetNumber(m_oStr->CreateAndCopy(ce.pszNumber));
                else
                    objItem->SetNumber(m_oStr->CreateAndCopy(_T("# unavailable")));
                if(ce.pszName)
                    objItem->SetName(m_oStr->CreateAndCopy(ce.pszName));
                else
                    objItem->SetName(m_oStr->CreateAndCopy(_T("Unknown")));

                ftEntry = ce.ftStartTime;
                AddItem(objItem, 0);
                iCount++;            
            }

            // clean up
            DestroyCallEntry(ce);
        }
        else
            break;

        // do we have enough?
        if(!NeedMoreHistoryItems(iCount))
            break;
    }

    PhoneCloseCallLog(m_hCallLog);
}
    
void CGuiCallLog::CollectSMSMessages(void)
{
    CSMSInterface* oMapi = CSMSInterface::Instance();

    int iNumberToGet = -1;  // all of them
    if(m_sOptions && m_sOptions->eShowHistory == HISTORY_Last100)
        iNumberToGet = 100; 
    else if(m_sOptions && m_sOptions->eShowHistory == HISTORY_Last200)
        iNumberToGet = 200;

    FILETIME ftExpired = {0x0, 0x0};
    if(m_sOptions && m_sOptions->eShowHistory == HISTORY_LastMonth)
    {
        SYSTEMTIME st;
        GetSystemTime(&st);
        st.wMonth -= 1;
        if(st.wMonth < 1)
        {
            st.wMonth = 12;
            st.wYear --;
        }

        SystemTimeToFileTime(&st, &ftExpired);

    }

    CIssVector<TypeSMSMessage> arrSMSMsg;
    if(m_sOptions && (m_sOptions->eShowSMS == SMS_All || m_sOptions->eShowSMS == SMS_Incoming))
    {
        oMapi->GetSMSMessages(arrSMSMsg, iNumberToGet, ftExpired, Enum_CE_IPM_INBOX_ENTRYID);
    }
    if(m_sOptions && (m_sOptions->eShowSMS == SMS_All || m_sOptions->eShowSMS == SMS_Outgoing))
    {
        oMapi->GetSMSMessages(arrSMSMsg, iNumberToGet, ftExpired, Enum_CE_IPM_DRAFTS_ENTRYID);
        oMapi->GetSMSMessages(arrSMSMsg, iNumberToGet, ftExpired, Enum_IPM_SENTMAIL_ENTRYID);
    }

    // now assemble them into our list
    for(int i = 0; i < arrSMSMsg.GetSize(); i++)
    {
        if(arrSMSMsg[i])
        {
            CObjHistorySMSItem* pSMSItem = new CObjHistorySMSItem();
            pSMSItem->SetMessage(arrSMSMsg[i]);
            AddItem(pSMSItem, 0);
        }
    }
}   

void CGuiCallLog::PopulateList()
{
    ////////////////////////////////////////////////////
    // add all necessary items
    // if list has a certain count, get the full number of items needed, then trim to proper size
    // otherwise, no way to tell which ones make the top of the list

    // calls
    if(m_sOptions && m_sOptions->eShowCall != CALL_Off)
        CollectCallItems();

    // SMS
    if(m_sOptions && m_sOptions->eShowSMS != SMS_Off)
        CollectSMSMessages();

    int iLineSpace = IsVGA() ? 10 : 5;
    SetItemHeights(m_iLineHeight*2 + iLineSpace, m_iLineHeight*3 + iLineSpace);

    ////////////////////////////////////////////////////
    // sort them
    m_arrItems.Sort(CompareItems);

    ////////////////////////////////////////////////////
    // trim list if necessaary
    int iMaxListCount = -1;
    if(m_sOptions && m_sOptions->eShowHistory == HISTORY_Last100)
        iMaxListCount = 100;
    else if(m_sOptions && m_sOptions->eShowHistory == HISTORY_Last200)
        iMaxListCount = 200;
    if(iMaxListCount > 0)
    {
        while(m_arrItems.GetSize() > iMaxListCount)
        {
            m_arrItems.RemoveElementAt(m_arrItems.GetSize()-1);
        }
    }

    ////////////////////////////////////////////////////
    // select something
    SetSelectedItemIndex(0, TRUE);
    ResetScrollPos();
}

void CGuiCallLog::EraseList()
{
    ResetContent();
}


BOOL CGuiCallLog::AddMenuItems()
{

	return TRUE;

}

BOOL CGuiCallLog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

int CGuiCallLog::CompareItems( const void *arg1, const void *arg2 )
{
    TypeItems& sListItem1 = **(TypeItems **)arg1;
    TypeItems& sListItem2 = **(TypeItems **)arg2;

    CObjHistoryItem* pEntry1	= (CObjHistoryItem*)sListItem1.lpItem;
    CObjHistoryItem* pEntry2	= (CObjHistoryItem*)sListItem2.lpItem;

    if(!pEntry1 || !pEntry2)
        return 0;

    return CompareFileTime(&pEntry2->GetTime(), &pEntry1->GetTime());
}



//////////////////////////////////////////////////////////////////////////
// history object classes implementation

//////////////////////////////////////////////////////////////////////////
// CObjHistoryItem base class implementation
CObjHistoryItem::CObjHistoryItem(EnumHistoryItemType eType) : m_eType(eType)   
{
	m_iLineHeight			= GetSystemMetrics(SM_CXSMICON);

	m_hFontNormal			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
 	m_hFontBold		        = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
}


CObjHistoryItem::~CObjHistoryItem()
{
	CIssGDIEx::DeleteFont(m_hFontNormal);
	CIssGDIEx::DeleteFont(m_hFontBold);
}


HRESULT CObjHistoryItem::Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText)
{
    HRESULT hr = S_OK;

    // draw icon
    int iIconWidth = gdiIcon.GetWidth() / NUM_HISTORY_ICONS;
    RECT rcIcon = rcDraw;
    rcIcon.bottom = rcIcon.top + gdiIcon.GetHeight();
    rcIcon.right = rcIcon.left + iIconWidth;
    ::Draw(gdiDest, rcIcon, gdiIcon, GetIconIndex() * iIconWidth, 0);


    // draw line separator
    RECT rcLine = rcDraw;
    rcLine.top = rcLine.bottom - 1;// for now
    ::Line(gdiDest.GetDC(), rcLine.left + 4, rcLine.top, rcLine.right - 4, rcLine.top, 0x454545);

    return hr;
}
    

//////////////////////////////////////////////////////////////////////////
// CObjHistoryCallItem implementation
CObjHistoryCallItem::CObjHistoryCallItem():CObjHistoryItem(HI_Call)
, m_szName(NULL)
, m_szNumber(NULL)
, m_szNameType('\0')
{
}

CObjHistoryCallItem::~CObjHistoryCallItem()
{
}
    
int CObjHistoryCallItem::GetIconIndex()
{
    if(m_bMissed)
        return 4;
    else if(m_bIsIncoming)
        return 2;
    else
        return 3;
}

HRESULT CObjHistoryCallItem::Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText)
{
    HRESULT hr;
    RECT rcText, rcTextLeft;
    hr = CObjHistoryItem::Draw(gdiDest, gdiIcon, rcDraw, bIsHighlighted, crText);
    CHR(hr, _T("CObjHistoryItem::Draw() failed"));

    CIssString* oStr = CIssString::Instance();
    static TCHAR szText[STRING_LARGE];
    oStr->Empty(szText);

    // line 1
    int iIconWidth = gdiIcon.GetWidth() / NUM_HISTORY_ICONS;
    // name
    rcText = rcDraw;
    rcText.left += iIconWidth + INDENT*2;
    rcText.right -=INDENT;
    rcText.bottom = rcText.top + m_iLineHeight;

    rcTextLeft = rcText;
    rcTextLeft.right -= (int)(WIDTH(rcText)*0.4);
    DrawText(gdiDest.GetDC(), m_szName, rcText, DT_LEFT | DT_VCENTER, m_hFontBold, crText);
    
    // date/time
    SYSTEMTIME sysTime;
    FILETIME ftLocal;
    FileTimeToLocalFileTime(&GetTime(), &ftLocal);
    FileTimeToSystemTime(&ftLocal, &sysTime);
    oStr->Format(szText, _T("%d:%02d%s %02d/%02d"), 
                            sysTime.wHour == 0 ? 12 : sysTime.wHour <= 12 ? sysTime.wHour : sysTime.wHour - 12, 
                            sysTime.wMinute, 
                            sysTime.wHour < 12 ? _T("a") : _T("p"),
                            sysTime.wMonth,
                            sysTime.wDay
                            //,sysTime.wYear % 100
                            );

    DrawText(gdiDest.GetDC(), szText, rcText, DT_RIGHT | DT_VCENTER, m_hFontNormal, crText);
    
    
    ///////////////////////////////////////////////////////////
    // line 2
    // phone number
    OffsetRect(&rcText, 0, m_iLineHeight);
    rcText.left += INDENT;
    DrawText(gdiDest.GetDC(), m_szNumber, rcText, DT_LEFT | DT_VCENTER, m_hFontNormal, crText);

    if(bIsHighlighted)
    {
        // draw some other stuff
    }

Error:
    return hr;
}

//////////////////////////////////////////////////////////////////////////
// CObjHistorySMSItem implementation
CObjHistorySMSItem::CObjHistorySMSItem():CObjHistoryItem(HI_SMS)
{
}

CObjHistorySMSItem::~CObjHistorySMSItem()
{
    if(m_sMsg)
        delete m_sMsg;
}

HRESULT CObjHistorySMSItem::Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText)
{
    HRESULT hr;
    RECT rcText, rcTextLeft;
    hr = CObjHistoryItem::Draw(gdiDest, gdiIcon, rcDraw, bIsHighlighted, crText);
    CHR(hr, _T("CObjHistoryItem::Draw() failed"));

    CPHRE(m_sMsg, _T("CObjHistorySMSItem::m_sMsg is NULL"), E_INVALIDARG);

    CIssString* oStr = CIssString::Instance();
    static TCHAR szText[STRING_LARGE];
    oStr->Empty(szText);


    //////////////////////////////////////////////////////////////
    // line 1
    int iIconWidth = gdiIcon.GetWidth() / NUM_HISTORY_ICONS;
    // name
    rcText = rcDraw;
    rcText.left += iIconWidth + INDENT*2;
    rcText.right -=INDENT;
    rcText.bottom = rcText.top + m_iLineHeight;

    rcTextLeft = rcText;
    rcTextLeft.right -= (int)(WIDTH(rcText)*0.4);
    if(m_sMsg->szNumber)
        DrawText(gdiDest.GetDC(), m_sMsg->szNumber, rcTextLeft, DT_LEFT | DT_VCENTER, m_hFontBold, crText);
    
    // date/time
    SYSTEMTIME sysTime;
    FILETIME ftLocal;
    FileTimeToLocalFileTime(&GetTime(), &ftLocal);
    FileTimeToSystemTime(&ftLocal, &sysTime);
    oStr->Format(szText, _T("%d:%02d%s %02d/%02d"), 
                            sysTime.wHour == 0 ? 12 : sysTime.wHour <= 12 ? sysTime.wHour : sysTime.wHour - 12, 
                            sysTime.wMinute, 
                            sysTime.wHour < 12 ? _T("a") : _T("p"),
                            sysTime.wMonth,
                            sysTime.wDay
                            //,sysTime.wYear % 100
                            );
    DrawText(gdiDest.GetDC(), szText, rcText, DT_RIGHT | DT_VCENTER, m_hFontNormal, crText);
    
    ///////////////////////////////////////////////////////////
    // line 2
    OffsetRect(&rcText, 0, m_iLineHeight);
    rcText.left += INDENT;
    if(!bIsHighlighted)
    {
        // 1 line of message
        if(m_sMsg->szSubject)
            DrawText(gdiDest.GetDC(), m_sMsg->szSubject, rcText, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS, m_hFontNormal, crText);
    }
    else
    {
        // try to draw all the text
        rcText.bottom = rcDraw.bottom;
        if(m_sMsg->szSubject)
            DrawText(gdiDest.GetDC(), m_sMsg->szSubject, rcText, DT_LEFT | DT_WORDBREAK , m_hFontNormal, crText);
    }

Error:
    return hr;
}

int CObjHistorySMSItem::GetIconIndex()
{
    if(m_sMsg && m_sMsg->bReceived)
        return 0;

    return 1;
}

FILETIME CObjHistorySMSItem::GetTime()      
{ 
    FILETIME ft = { 0x0, 0x0 };
    if(m_sMsg)
        return m_sMsg->ft; 

    return ft;

}


