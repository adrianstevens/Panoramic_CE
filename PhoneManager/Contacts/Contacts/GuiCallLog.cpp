#include "StdAfx.h"
#include "GuiCallLog.h"
#include "ContactsGuiDefines.h"
#include "DlgContactDetails.h"
#include "Resource.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "IssDebug.h"
#include "GuiToolBar.h"
#include "DlgPerContactSMS.h"
#include "IssStateAndNotify.h"
#include "GuiToolbar.h"
#include "DlgPerContactCall.h"
#include "DlgPerContactConv.h"
#include "DlgContacts.h"
#include "CallLog.h"
#include "DlgMsgBox.h"

#define IDMENU_Call             6000
#define IDMENU_SMS              6001
#define IDMENU_NewCall          6002
#define IDMENU_NewSMS           6003
#define IDMENU_AddContact       6004
#define IDT_SaveOptions         4001

#define MAX_ENTRIES             300

#ifdef DEBUG
#define DBG_FT(printf_exp)    DebugFileTime printf_exp;
#else
#define DBG_FT(printf_exp)    (void)0
#endif

static CGuiCallLog* g_Calllog = NULL;

CGuiCallLog::CGuiCallLog(void) 
:m_iCallHistoryCount(0)
,m_iSMSSentCount(0)
,m_iSMSReceivedCount(0)
,m_dlgContactDetails(NULL)
,m_bDropBarDown(FALSE)
{
    ASSERT (g_Calllog == NULL);
    g_Calllog = this;
	m_iLineHeight			= GetSystemMetrics(SM_CXSMICON);//GetSystemMetrics(SM_CXICON)/2;
	m_ptScreenSize.x		= 0;
	m_ptScreenSize.y		= 0;
    m_hFontToggleHistory    = CIssGDIEx::CreateFont(m_iLineHeight*3/4, FW_BOLD, TRUE);
}

CGuiCallLog::~CGuiCallLog(void)
{
    g_Calllog = NULL;
    DestroyHistory();

	CSMSInterface* oMapi = CSMSInterface::Instance();
	oMapi->DeleteInstance();
    CIssGDIEx::DeleteFont(m_hFontToggleHistory);
}

BOOL CGuiCallLog::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContactDetails* dlgContactDetails)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBg, guiBackground, sOptions);

    m_dlgContactDetails = dlgContactDetails;

    Initialize(hWndParent, hWndParent, m_hInst, 0);
    SetCustomDrawFunc(DrawListItem, this);
	SetDeleteItemFunc(DeleteCallItem);
	SetSelected(IDR_PNG_ContactsSelector);

    ReloadColorSchemeItems(hWndParent, hInst);

    m_gdiArrows.LoadImage(IsVGA()?IDR_PNG_HistoryArrowVGA:IDR_PNG_HistoryArrow, hWndParent, m_hInst, TRUE);

	PopulateList(TRUE, TRUE, TRUE);
    
	return TRUE;
}

void CGuiCallLog::DestroyHistory(int iStart)
{
    int iCount = m_arrHistoryFull.GetSize();

    if(iStart >= iCount)
        return;

    for(int i=iStart; i<m_arrHistoryFull.GetSize(); i++)
    {
        CObjHistoryItem* sItem = m_arrHistoryFull[iStart];
        if(!sItem)
            continue;
        delete sItem;
        m_arrHistoryFull.RemoveElementAt(iStart);
    }
    //redundant at this point but just in case
    if(iStart == 0)
        m_arrHistoryFull.RemoveAll();

    ResetContent();
}

void CGuiCallLog::ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance)
{
	SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
	SetImageArray(SKIN(IDR_PNG_MenuArray));

	// preload list images
	PreloadImages(hWnd, hInstance);
}

CIssKineticList*  CGuiCallLog::GetCallList()
{
    if(g_Calllog)
        return (CIssKineticList*)g_Calllog;
        
    return NULL;
}

void CGuiCallLog::DeleteCallItem(LPVOID lpItem)
{
	if(!lpItem)
		return;
    // this is no longer needed because the actual data is handled by the full vector

	//CObjHistoryItem* objHistoryItem = (CObjHistoryItem*)lpItem;
	//    delete objHistoryItem;
}
    
BOOL CGuiCallLog::HasFocus()
{
    CGuiToolBar* guiToolBar = CGuiToolBar::GetToolbar();
    if(!guiToolBar)
        return FALSE;
    else
        return !guiToolBar->HasFocus();
}

CIssGDIEx& CGuiCallLog::GetIconArray()
{
    return CDlgContacts::GetCallIcons();
}

BOOL CGuiCallLog::Draw(CIssGDIEx& gdiMem, HDC hDC, RECT& rcClient, RECT& rcClip)
{
    if(!IsRectInRect(m_rcLocation, rcClip))
        return TRUE;

    DBG_CONTACTS(_T("CGuiCallLog::Draw()"), TRUE);
    RECT rcClipClient = rcClip;
    rcClipClient.right = min(rcClip.right, rcClient.right);
    rcClipClient.bottom = min(rcClip.bottom, rcClient.bottom);
    rcClipClient.left = max(rcClip.left, rcClient.left);
    
    // since this background is a full screen bg, must pass in that RECT
    RECT rcBg = {0,0,GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    m_guiBackground->DrawBackground(*m_gdiBg, rcClipClient, rcBg, m_rcDropBar.bottom - m_guiBackground->GetSliceHeight());

    if(!m_arrItems.GetSize())
        DrawText(m_gdiBg->GetDC(), _T("\r\nno entries"), m_rcHistoryList, DT_CENTER|DT_WORDBREAK , CDlgContacts::GetFontNormal(), m_crText);
    else
        CIssKineticList::OnDraw(*m_gdiBg, rcClient, rcClip);

    if(!m_bDropBarDown)
    {
        FillRect(m_gdiBg->GetDC(), m_rcDropBar, COLOR_HISTORY_DROPBAR);

        ::Draw(*m_gdiBg, 
            m_rcDropBar.left + (WIDTH(m_rcDropBar)-m_gdiArrows.GetWidth()/2)/2, m_rcDropBar.top + (HEIGHT(m_rcDropBar)-m_gdiArrows.GetHeight())/2, 
            m_gdiArrows.GetWidth()/2, m_gdiArrows.GetHeight(), 
            m_gdiArrows, 
            0,0);

        // draw the top part of the border only here
        m_guiBackground->DrawTopSlice(*m_gdiBg, rcClipClient, rcBg, m_rcDropBar.bottom);
    }

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

void CGuiCallLog::AnimateQuickSetIn()
{
    if(m_sOptions->bShowAnimations == FALSE)
    {
        InvalidateRect(m_hWndParent, NULL, FALSE);
        return;
    }

    RECT rcClient;
    GetClientRect(m_hWndParent, &rcClient);

    CIssGDIEx gdiQuickButtons;
    gdiQuickButtons.Create(m_gdiMem->GetDC(), m_rcDropFull, FALSE, TRUE);
    DrawQuickSetButtons(gdiQuickButtons, rcClient);

    int iDropHeight = HEIGHT(m_rcDropFull) - HEIGHT(m_rcDropBar);

    HDC dc = GetDC(m_hWndParent);
    DWORD dwStart = GetTickCount();
    int iNumFrames = 1000;
    int iTimeout = 500;
    int i = GetCurrentFrame(dwStart, iNumFrames, iTimeout);
    while(i < iNumFrames)
    {
             
        BitBlt(m_gdiBg->GetDC(),
               0,0,
               WIDTH(rcClient), HEIGHT(rcClient),
               m_gdiMem->GetDC(),
               0,0,
               SRCCOPY);

        AlphaFillRect(*m_gdiBg, rcClient, 0, 150*i/iNumFrames);

        ::Draw(*m_gdiBg,
               0, iDropHeight*i/iNumFrames - iDropHeight,
               gdiQuickButtons.GetWidth(), gdiQuickButtons.GetHeight(),
               gdiQuickButtons,
               0,0);

        // all to screen
        BitBlt(dc,
            0,0,
            WIDTH(rcClient), HEIGHT(rcClient),
            m_gdiBg->GetDC(),
            0,0,
            SRCCOPY);

        i = GetCurrentFrame(dwStart, iNumFrames, iTimeout);

    }
    ReleaseDC(m_hWndParent, dc);

}

void CGuiCallLog::AnimateQuickSetOut()
{
    if(m_sOptions->bShowAnimations == FALSE)
    {
        InvalidateRect(m_hWndParent, NULL, FALSE);
        return;
    }

    RECT rcClient;
    GetClientRect(m_hWndParent, &rcClient);

    CIssGDIEx gdiQuickButtons;
    gdiQuickButtons.Create(m_gdiMem->GetDC(), m_rcDropFull, FALSE, TRUE);
    DrawQuickSetButtons(gdiQuickButtons, rcClient);

    int iDropHeight = HEIGHT(m_rcDropFull) - HEIGHT(m_rcDropBar);

    HDC dc = GetDC(m_hWndParent);
    DWORD dwStart = GetTickCount();
    int iNumFrames = 1000;
    int iTimeout = 500;
    int i = GetCurrentFrame(dwStart, iNumFrames, iTimeout);
    while(i < iNumFrames)
    {

        BitBlt(m_gdiMem->GetDC(),
            0,0,
            WIDTH(rcClient), HEIGHT(rcClient),
            m_gdiBg->GetDC(),
            0,0,
            SRCCOPY);

        AlphaFillRect(*m_gdiMem, rcClient, 0, 150 - 150*i/iNumFrames);

        ::Draw(*m_gdiMem,
            0, -iDropHeight*i/iNumFrames,
            gdiQuickButtons.GetWidth(), gdiQuickButtons.GetHeight(),
            gdiQuickButtons,
            0,0);

        // all to screen
        BitBlt(dc,
            0,0,
            WIDTH(rcClient), HEIGHT(rcClient),
            m_gdiMem->GetDC(),
            0,0,
            SRCCOPY);

        i = GetCurrentFrame(dwStart, iNumFrames, iTimeout);

    }
    ReleaseDC(m_hWndParent, dc);
}

void CGuiCallLog::ToggleQuickSet(int iQuadrant)
{
    if(!m_sOptions)
        return;

    switch(iQuadrant)
    {
    case 1: // SMS incoming
        if(m_sOptions->eShowSMS == SMS_All)
            m_sOptions->eShowSMS = SMS_Outgoing;
        else if(m_sOptions->eShowSMS == SMS_Incoming)
            m_sOptions->eShowSMS = SMS_Off;
        else if(m_sOptions->eShowSMS == SMS_Outgoing)
            m_sOptions->eShowSMS = SMS_All;
        else
            m_sOptions->eShowSMS = SMS_Incoming;
        break;
    case 2: // SMS outgoing
        if(m_sOptions->eShowSMS == SMS_All)
            m_sOptions->eShowSMS = SMS_Incoming;
        else if(m_sOptions->eShowSMS == SMS_Incoming)
            m_sOptions->eShowSMS = SMS_All;
        else if(m_sOptions->eShowSMS == SMS_Outgoing)
            m_sOptions->eShowSMS = SMS_Off;
        else
            m_sOptions->eShowSMS = SMS_Outgoing;
        break;
    case 3: // CALL incoming
        if(m_sOptions->eShowCall == CALL_All)
            m_sOptions->eShowCall = CALL_OutgoingMissed;
        else if(m_sOptions->eShowCall == CALL_Outgoing)
            m_sOptions->eShowCall = CALL_IncomingOutgoing;
        else if(m_sOptions->eShowCall == CALL_Missed)
            m_sOptions->eShowCall = CALL_IncomingMissed;
        else if(m_sOptions->eShowCall == CALL_Incoming)
            m_sOptions->eShowCall = CALL_Off;
        else if(m_sOptions->eShowCall == CALL_IncomingOutgoing)
            m_sOptions->eShowCall = CALL_Outgoing;
        else if(m_sOptions->eShowCall == CALL_IncomingMissed)
            m_sOptions->eShowCall = CALL_Missed;
        else if(m_sOptions->eShowCall == CALL_OutgoingMissed)
            m_sOptions->eShowCall = CALL_All;
        else
            m_sOptions->eShowCall = CALL_Incoming;
        break;
    case 4: // CALL outgoing
        if(m_sOptions->eShowCall == CALL_All)
            m_sOptions->eShowCall = CALL_IncomingMissed;
        else if(m_sOptions->eShowCall == CALL_Outgoing)
            m_sOptions->eShowCall = CALL_Off;
        else if(m_sOptions->eShowCall == CALL_Missed)
            m_sOptions->eShowCall = CALL_OutgoingMissed;
        else if(m_sOptions->eShowCall == CALL_Incoming)
            m_sOptions->eShowCall = CALL_IncomingOutgoing;
        else if(m_sOptions->eShowCall == CALL_IncomingOutgoing)
            m_sOptions->eShowCall = CALL_Incoming;
        else if(m_sOptions->eShowCall == CALL_IncomingMissed)
            m_sOptions->eShowCall = CALL_All;
        else if(m_sOptions->eShowCall == CALL_OutgoingMissed)
            m_sOptions->eShowCall = CALL_Missed;
        else
            m_sOptions->eShowCall = CALL_Outgoing;
        break;
    case 5: // CALL missed
        if(m_sOptions->eShowCall == CALL_All)
            m_sOptions->eShowCall = CALL_IncomingOutgoing;
        else if(m_sOptions->eShowCall == CALL_Outgoing)
            m_sOptions->eShowCall = CALL_OutgoingMissed;
        else if(m_sOptions->eShowCall == CALL_Missed)
            m_sOptions->eShowCall = CALL_Off;
        else if(m_sOptions->eShowCall == CALL_Incoming)
            m_sOptions->eShowCall = CALL_IncomingMissed;
        else if(m_sOptions->eShowCall == CALL_IncomingOutgoing)
            m_sOptions->eShowCall = CALL_All;
        else if(m_sOptions->eShowCall == CALL_IncomingMissed)
            m_sOptions->eShowCall = CALL_Incoming;
        else if(m_sOptions->eShowCall == CALL_OutgoingMissed)
            m_sOptions->eShowCall = CALL_Outgoing;
        else
            m_sOptions->eShowCall = CALL_Missed;
        break;
    default:
        // no toggle
        return;
    }

    // resync
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
    SynchronizeHistory();

    // Save the new options (but only do it after 15 seconds
    KillTimer(m_hWndParent, IDT_SaveOptions);
    SetTimer(m_hWndParent, IDT_SaveOptions, 15000, NULL);

    ::SetCursor(hCursor);
}

void CGuiCallLog::DrawQuickSetButtons(CIssGDIEx& gdi, RECT& rcClient)
{
    //if(!m_bDropBarDown)
    //    return;

    int iIndent    = INDENT;

    RECT rc = m_rcDropBar;
    rc.bottom   = rc.top + m_iLineHeight;
    FillRect(gdi, m_rcDropFull, COLOR_HISTORY_BG);
    

    // draw the top bar and text
    FillRect(gdi, rc, COLOR_HISTORY_DROPBAR);
    rc.left += iIndent;
    rc.right-= iIndent;
    DrawText(gdi, _T("Toggle History"), rc, DT_LEFT|DT_VCENTER, m_hFontToggleHistory, COLOR_HISTORY_BG);
    DrawText(gdi, _T("x"), rc, DT_RIGHT|DT_VCENTER, m_hFontToggleHistory, COLOR_HISTORY_BG);
    rc.left -= iIndent;
    rc.right+= iIndent;

    // draw the bottom bar
    rc = m_rcDropFull;
    rc.top = rc.bottom - HEIGHT(m_rcDropBar);
    FillRect(gdi, rc, COLOR_HISTORY_DROPBAR);
    ::Draw(gdi, 
        rc.left + (WIDTH(rc)-m_gdiArrows.GetWidth()/2)/2, rc.top + (HEIGHT(rc)-m_gdiArrows.GetHeight())/2, 
        m_gdiArrows.GetWidth()/2, m_gdiArrows.GetHeight(), 
        m_gdiArrows, 
        m_gdiArrows.GetWidth()/2,0);

    int iTemp = 3;
#ifdef USE_SMS
    iTemp = 5;
#endif


    int iWidthItem = WIDTH(rcClient)/iTemp;
    int iIconWidth = CDlgContacts::GetCallIcons().GetWidth() / NUM_HISTORY_ICONS;
    int iIconHeight= CDlgContacts::GetCallIcons().GetHeight();

    rc.bottom  = rc.top;
    rc.top      = m_rcDropBar.top + m_iLineHeight;
    rc.right    = iWidthItem;

    CIssGDIEx* gdiGray = &CDlgContacts::GetCallIconsGray();
    CIssGDIEx* gdiColor = &CDlgContacts::GetCallIcons();

    CIssGDIEx* gdiIcon = gdiGray;

#ifdef USE_SMS
    if(m_sOptions->eShowSMS == SMS_All || m_sOptions->eShowSMS == SMS_Incoming)
        gdiIcon = gdiColor;
    ::Draw(gdi, rc.left + (iWidthItem-iIconWidth)/2, rc.top + 2*iIndent, iIconWidth, iIconHeight, *gdiIcon, 0*iIconWidth, 0);
    Line(gdi, rc.right, rc.top+iIndent, rc.right, rc.bottom-iIndent, COLOR_HISTORY_DROPBAR);

    gdiIcon = gdiGray;
    if(m_sOptions->eShowSMS == SMS_All || m_sOptions->eShowSMS == SMS_Outgoing)
        gdiIcon = gdiColor;
    OffsetRect(&rc, iWidthItem, 0);
    ::Draw(gdi, rc.left + (iWidthItem-iIconWidth)/2, rc.top + 2*iIndent, iIconWidth, iIconHeight, *gdiIcon, 1*iIconWidth, 0);
    Line(gdi, rc.right, rc.top+iIndent, rc.right, rc.bottom-iIndent, COLOR_HISTORY_DROPBAR);

    OffsetRect(&rc, iWidthItem, 0);
#endif

    gdiIcon = gdiGray;
    if(m_sOptions->eShowCall == CALL_All || m_sOptions->eShowCall == CALL_Incoming|| m_sOptions->eShowCall == CALL_IncomingOutgoing|| m_sOptions->eShowCall == CALL_IncomingMissed)
        gdiIcon = gdiColor;
    
    ::Draw(gdi, rc.left + (iWidthItem-iIconWidth)/2, rc.top + 2*iIndent, iIconWidth, iIconHeight, *gdiIcon, 2*iIconWidth, 0);
    Line(gdi, rc.right, rc.top+iIndent, rc.right, rc.bottom-iIndent, COLOR_HISTORY_DROPBAR);

    gdiIcon = gdiGray;
    if(m_sOptions->eShowCall == CALL_All || m_sOptions->eShowCall == CALL_Outgoing|| m_sOptions->eShowCall == CALL_IncomingOutgoing|| m_sOptions->eShowCall == CALL_OutgoingMissed)
        gdiIcon = gdiColor;
    OffsetRect(&rc, iWidthItem, 0);
    ::Draw(gdi, rc.left + (iWidthItem-iIconWidth)/2, rc.top + 2*iIndent, iIconWidth, iIconHeight, *gdiIcon, 3*iIconWidth, 0);
    Line(gdi, rc.right, rc.top+iIndent, rc.right, rc.bottom-iIndent, COLOR_HISTORY_DROPBAR);

    gdiIcon = gdiGray;
    if(m_sOptions->eShowCall == CALL_All || m_sOptions->eShowCall == CALL_Missed|| m_sOptions->eShowCall == CALL_IncomingMissed|| m_sOptions->eShowCall == CALL_OutgoingMissed)
        gdiIcon = gdiColor;
    OffsetRect(&rc, iWidthItem, 0);
    ::Draw(gdi, rc.left + (iWidthItem-iIconWidth)/2, rc.top + 2*iIndent, iIconWidth, iIconHeight, *gdiIcon, 4*iIconWidth, 0);
}

void CGuiCallLog::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{
    if(sItem->eType != ITEM_Unknown)
        return;

    CObjHistoryItem* objHistoryItem = (CObjHistoryItem*)sItem->lpItem;
    if(!objHistoryItem)
        return;

    rcDraw.right -= INDENT/4;
    objHistoryItem->Draw(gdi, CDlgContacts::GetCallIcons(), rcDraw, bIsHighlighted, bIsHighlighted ? m_crHighlight : m_crText);
}

void CGuiCallLog::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CGuiCallLog* pGuiCallLog = (CGuiCallLog*)lpClass;
    pGuiCallLog->DrawListItem(gdi, rcDraw, sItem, bIsHighlighted);
}

BOOL CGuiCallLog::OnTimer(WPARAM wParam, LPARAM lParam)
{
	CIssKineticList::OnTimer(wParam, lParam);

    if(wParam == IDT_SaveOptions)
    {
        KillTimer(m_hWndParent, IDT_SaveOptions);
        CDlgContacts::SaveMyOptions();
    }

	return UNHANDLED;
}

BOOL CGuiCallLog::MoveGui(RECT rcLocation)
{
    if(EqualRect(&rcLocation, &m_rcLocation))
        return TRUE;

	CGuiBase::MoveGui(rcLocation);

    DBG_CONTACTS(_T("CGuiCallLog::MoveGui()"), TRUE);

    m_rcDropBar     = rcLocation;
    m_rcDropBar.bottom = rcLocation.top + IsVGA()?HISTORY_DROPTAB_HEIGHT*2:HISTORY_DROPTAB_HEIGHT;

    m_rcDropFull    = rcLocation;
    m_rcDropFull.bottom = HEIGHT(m_rcDropBar) + m_iLineHeight + CDlgContacts::GetCallIcons().GetHeight() + 4*INDENT;


    m_rcHistoryList = rcLocation;
    m_rcHistoryList.top = m_rcDropBar.bottom;

    int iListInset = (IsVGA() ? CONTACTS_LIST_INSET * 2 : CONTACTS_LIST_INSET);
    CIssKineticList::OnSize(m_rcHistoryList.left + iListInset, 
        m_rcHistoryList.top + iListInset/2, 
        WIDTH(m_rcHistoryList) - iListInset*2, 
        HEIGHT(m_rcHistoryList) - iListInset/2
        );

    DBG_CONTACTS(_T("CGuiCallLog::MoveGui()"), FALSE);
	return TRUE;
}
   
void CGuiCallLog::ResetCounts(BOOL bCalls, BOOL bSMS)
{
    // reset missed call count
    if(bCalls && m_sOptions && m_sOptions->eShowCall != CALL_Off)
        CIssStateAndNotify::ResetMissedCallsCount();

    // set SMS recieved messages as read
    // 2-18-2009 we're not doing this now
    /*if(bSMS && m_sOptions && m_sOptions->eShowCall != SMS_Off)
    {
        int iUnreadCount = CIssStateAndNotify::GetUnreadSMSCount();
        CSMSInterface::Instance()->SetSMSMessagesRead(iUnreadCount);
    }*/
}

void CGuiCallLog::Show()
{
    CGuiBase::Show();
    //ResetCounts(TRUE, TRUE);
}

void CGuiCallLog::DrawSelector(CIssGDIEx& gdi, RECT& rcDraw)
{
    // fine-tune the RECT
    RECT rcSelector = rcDraw;
    int iIconIndent = GetIconArray().GetWidth() / NUM_HISTORY_ICONS;
    rcSelector.left += (iIconIndent + INDENT/2);
    if(m_dwFlags & OPTION_DrawScrollBar)
        rcSelector.right += INDENT/4;
    rcSelector.top += INDENT/4;
    rcSelector.bottom -= INDENT/2;
    CIssKineticList::DrawSelector(gdi, rcSelector);
}


BOOL CGuiCallLog::OnLButtonDown(POINT pt)
{
    if(m_bDropBarDown)
    {
        if(pt.y < m_rcDropFull.top + m_iLineHeight || pt.y > m_rcDropFull.bottom - HEIGHT(m_rcDropBar))
        {
            return TRUE;
        }
        else
        {
#ifdef USE_SMS
            int iItemWidth = WIDTH(m_rcDropFull)/5;

            // toggle one of the items
            for(int i=1; i<=5; i++)
            {
                if(pt.x <= i*iItemWidth)
                {
                    ToggleQuickSet(i);
                    break;
                }
            }
#else
            int iItemWidth = WIDTH(m_rcDropFull)/3;

            // toggle one of the items
            for(int i=1; i<=3; i++)
            {
                if(pt.x <= i*iItemWidth)
                {
                    ToggleQuickSet(i+2);
                    break;
                }
            }
#endif


            SendMessage(m_hWndParent, WM_CHANGE_Gui, GUI_CallLog, 0);
            InvalidateRect(m_hWndParent, NULL, FALSE);
            return TRUE;
        }

    }
    else if(PtInRect(&m_rcDropBar, pt))
    {
        m_eMouse = GRAB_None;
        return TRUE;
    }

    

	if(PtInRect(&m_rcLoc, pt) == FALSE)
    {
        m_eMouse = GRAB_None;
        return FALSE;
    }

	m_ptMouseDown = pt;

    return CIssKineticList::OnLButtonDown(pt);
}

BOOL CGuiCallLog::OnLButtonUp(POINT pt)
{
    if(m_bDropBarDown)
    {
        if(pt.y < m_rcDropFull.top + m_iLineHeight || pt.y > m_rcDropFull.bottom - HEIGHT(m_rcDropBar))
        {
            m_bDropBarDown = FALSE;

            // animate back up
            AnimateQuickSetOut();

            // full redraw
            SendMessage(m_hWndParent, WM_CHANGE_Gui, GUI_CallLog, 0);
            InvalidateRect(m_hWndParent, NULL, FALSE);
            return TRUE;
        }
        else
        {
            return TRUE;
        }
    }
    else if(PtInRect(&m_rcDropBar, pt) && m_eMouse != GRAB_List)
    {
        m_bDropBarDown = TRUE;

        // animate down
        AnimateQuickSetIn();

        // full redraw
        SendMessage(m_hWndParent, WM_CHANGE_Gui, GUI_CallLog, 0);
        InvalidateRect(m_hWndParent, NULL, FALSE);
        return TRUE;
    }    

    if(PtInRect(&m_rcLocation, pt) == FALSE && m_eMouse == GRAB_None)
        return FALSE;

    BOOL bRet = (m_eMouse == GRAB_List?TRUE:FALSE);
    bRet |=  CIssKineticList::OnLButtonUp(pt);
    return bRet;
}

BOOL CGuiCallLog::OnMouseMove(POINT pt)
{
	//if(PtInRect(&m_rcLocation, pt) == FALSE)
	//	return FALSE;

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
    return FALSE;
}


void CGuiCallLog::RefreshList(void)
{
	PopulateList(TRUE, TRUE, TRUE);
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
    {
        return iCount <= MAX_ENTRIES;
    }
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

void CGuiCallLog::SmartCollectCallItems(int iNumItems)
{
    HANDLE hCallLog = NULL;
    if(S_OK != PhoneOpenCallLog(&hCallLog) || hCallLog == NULL)
        return;

    /*DWORD dwIndex;
    if(S_OK != PhoneSeekCallLog(hCallLog, CALLLOGSEEK_BEGINNING, iStartIndex, &dwIndex))
    {
        PhoneCloseCallLog(hCallLog);
        return;
    }*/

    int iCount = 0;
    FILETIME ftRecent = m_ftRecentCall;

    BOOL bLoop = TRUE;

    while(bLoop)
    {
        CALLLOGENTRY ce;
        ce.cbSize = sizeof(CALLLOGENTRY);
        if(S_OK == PhoneGetCallLogEntry(hCallLog, &ce))
        {
            if(0 >= CompareFileTime(&ce.ftStartTime, &ftRecent))
            {
                DestroyCallEntry(ce);
                break;
            }
            else
                ftRecent = ce.ftStartTime;

            // check if item is beyond expiry date
            /*if(!CheckFilters(ce))
            {
                // skip this one
                DestroyCallEntry(ce);
                continue;
            }*/
            if(!CheckCallDate(ce))
            {
                // skip all the rest
                DestroyCallEntry(ce);
                break;           
            }
            else
            {
                CObjHistoryCallItem* objItem = new CObjHistoryCallItem();
                objItem->SetProperties(!ce.fOutgoing, (ce.iom == IOM_MISSED), ce.fRoam, ce.fConnected, ce.fEnded); 
                objItem->SetTimes(ce.ftStartTime, ce.ftEndTime);

                BOOL bCanSaveToContacts = FALSE;

                if(ce.pszNumber)
                {
                    objItem->SetNumber(m_oStr->CreateAndCopy(ce.pszNumber));
                }
                else
                {
                    objItem->SetNumber(m_oStr->CreateAndCopy(_T("# unavailable")));
                }
                if(ce.pszName)
                {
                    objItem->SetName(m_oStr->CreateAndCopy(ce.pszName));
                }
                else
                {
                    objItem->SetName(m_oStr->CreateAndCopy(_T("Unknown")));

                    if(ce.pszNumber)
                        bCanSaveToContacts = TRUE;//number but no name ... perfect
                }
                if(ce.pszNameType)
                {
                    objItem->SetNameType(m_oStr->CreateAndCopy(ce.pszNameType));
                }
                else
                {
                    objItem->SetNameType(NULL);
                }

                objItem->SetUnknown(bCanSaveToContacts);

                 // first check if this item exists
                if(!FindItem(objItem))
                {
                    m_arrHistoryFull.AddElement(objItem);
                    //AddItem(objItem, IDMENU_SelectCall);
                    SortLastItem(); // we do in-place sorting so it's faster
                    iCount++;     

                    if(iCount >= MAX_ENTRIES)
                        bLoop = FALSE;//we're done

                }
                else
                    delete objItem;
            }

            // clean up
            DestroyCallEntry(ce);
        }
        else
            break;

    }

    m_ftRecentCall = ftRecent;

    PhoneCloseCallLog(hCallLog);
}

void CGuiCallLog::RemoveCallItems()
{
    for(int i=m_arrHistoryFull.GetSize()-1; i>=0; i--)
    {
        CObjHistoryItem* sCall = m_arrHistoryFull[i];
        if(!sCall)
            continue;
        if(sCall->GetType() == HI_Call)
            DeleteHistoryItem(i);
    }
}

void CGuiCallLog::RemoveSMSMessage(BOOL bIncludeRecieved, BOOL bIncludeSent)
{
#ifndef USE_SMS
    return;
#endif
    for(int i=m_arrHistoryFull.GetSize()-1; i>=0; i--)
    {
        CObjHistoryItem* sHistory = m_arrHistoryFull[i];
        if(!sHistory)
            continue;
        if(sHistory->GetType() == HI_SMS)
        {
            CObjHistorySMSItem* sSMS = (CObjHistorySMSItem*)sHistory;
            TypeSMSMessage* sMessage = sSMS->GetSMS();
            if(sMessage && sMessage->bReceived && bIncludeRecieved)
                DeleteHistoryItem(i);
            else if(sMessage && !sMessage->bReceived && bIncludeSent)
                DeleteHistoryItem(i);
        }
    }
}

void CGuiCallLog::DeleteHistoryItem(int iItem)
{
    CObjHistoryItem* sItem = m_arrHistoryFull[iItem];
    if(!sItem)
        return;
    delete sItem;
    m_arrHistoryFull.RemoveElementAt(iItem);
}

void CGuiCallLog::CollectCallItems(void)
{
    HANDLE hCallLog = NULL;
    if(S_OK != PhoneOpenCallLog(&hCallLog) || hCallLog == NULL)
        return;

    int iCount = 0;
    FILETIME ftEntry;

    HRESULT hr;
    BOOL bLoop = TRUE;

    while(bLoop)
    {
        CALLLOGENTRY ce;
	    ce.cbSize = sizeof(CALLLOGENTRY);
	    if(S_OK == PhoneGetCallLogEntry(hCallLog, &ce))
        {
            // check if item is beyond expiry date
            /*if(!CheckFilters(ce))
            {
                // skip this one
                DestroyCallEntry(ce);
                continue;
            }*/
            if(!CheckCallDate(ce))
            {
                // skip all the rest
                DestroyCallEntry(ce);
                break;           
            }
            else
            {
                BOOL bCanSaveToContacts = FALSE;

                CObjHistoryCallItem* objItem = new CObjHistoryCallItem();
                if(!objItem)
                {
                    DestroyCallEntry(ce);
                    break;
                }

                objItem->SetProperties(!ce.fOutgoing, (ce.iom == IOM_MISSED), ce.fRoam, ce.fConnected, ce.fEnded); 
                objItem->SetTimes(ce.ftStartTime, ce.ftEndTime);

                if(ce.pszNumber)
                    objItem->SetNumber(m_oStr->CreateAndCopy(ce.pszNumber));
                else
                    objItem->SetNumber(m_oStr->CreateAndCopy(_T("# unavailable")));
                if(ce.pszName)
                    objItem->SetName(m_oStr->CreateAndCopy(ce.pszName));
                else
                {
                    objItem->SetName(m_oStr->CreateAndCopy(_T("Unknown")));

                    if(ce.pszNumber)
                        bCanSaveToContacts = TRUE;//number but no name ... perfect
                }
                if(ce.pszName)
                    objItem->SetNameType(m_oStr->CreateAndCopy(ce.pszNameType));
                
                objItem->SetUnknown(bCanSaveToContacts);

                ftEntry = ce.ftStartTime;
                //AddItem(objItem, IDMENU_SelectCall);
                hr = m_arrHistoryFull.AddElement(objItem);
                if(S_OK != hr)
                {
                    delete objItem;
                    DestroyCallEntry(ce);
                    break;
                }
                //SortLastItem(); // we do in-place sorting so it's faster
                iCount++;         

                if(iCount >= MAX_ENTRIES)
                    bLoop = FALSE;

                // update the most recent call item
                if((m_ftRecentCall.dwHighDateTime == 0 && m_ftRecentCall.dwLowDateTime == 0) || 0 < CompareFileTime(&ce.ftStartTime, &m_ftRecentCall))
                    m_ftRecentCall = ce.ftStartTime;
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

    // if there was nothing added we need the most recent time
    if(iCount == 0)
    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
        FILETIME ft;
        SystemTimeToFileTime(&sysTime, &ft);
        LocalFileTimeToFileTime(&ft, &m_ftRecentCall);
    }

    PhoneCloseCallLog(hCallLog);
}

void CGuiCallLog::DebugFileTime(TCHAR* szText, FILETIME& ft)
{
    SYSTEMTIME sysTime;
    FILETIME ftLocal;
    FileTimeToLocalFileTime(&ft, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &sysTime);
    TCHAR szString[STRING_MAX];
    m_oStr->Format(szString, _T("%s - %d:%02d%s %02d/%02d/%04d"),
        szText, 
        sysTime.wHour, 
        sysTime.wMinute, 
        sysTime.wHour < 12 ? _T("am") : _T("pm"),
        sysTime.wMonth,
        sysTime.wDay,
        sysTime.wYear
        );
    DBG_OUT((szString));
}

BOOL CGuiCallLog::FindItem(CObjHistoryItem* oItem)
{
    if(!oItem)
        return TRUE;    // pretend it was found

    if(m_arrHistoryFull.GetSize() == 0)
        return FALSE;

    LONG lCompare;
    // we are assuming we are working on a fully sorted list
    for(int i=0; i < m_arrHistoryFull.GetSize(); i++)
    {
        CObjHistoryItem* oCheckItem = m_arrHistoryFull[i];
        if(!oCheckItem)
            continue;

        lCompare = CompareFileTime(&oItem->GetTime(), &oCheckItem->GetTime()); 
        if(0 ==  lCompare && oItem->GetType() == oCheckItem->GetType())
            return TRUE;
        else if(lCompare <= 0)   // we know the first item is earlier so that means it's not there
            return FALSE;
    }

    return FALSE;
}

void CGuiCallLog::OnFocus()
{
    // reset missed call count
    if(m_sOptions && m_sOptions->eShowCall != CALL_Off)
        CIssStateAndNotify::ResetMissedCallsCount();
}

void CGuiCallLog::SortLastItem()
{
    // is there anything to sort?
    if(m_arrHistoryFull.GetSize() < 2)
        return;

   CObjHistoryItem* oNewItem = m_arrHistoryFull[m_arrHistoryFull.GetSize() - 1];
    if(!oNewItem)
        return;

    LONG lCompare;

    // we are assuming we are working on a fully sorted list
    for(int i=0; i < m_arrHistoryFull.GetSize()-1; i++)
    {
        CObjHistoryItem* oCheckItem = m_arrHistoryFull[i];
        if(!oCheckItem)
            continue;
        lCompare = CompareFileTime(&oNewItem->GetTime(), &oCheckItem->GetTime());
        if(lCompare >= 0)
        {
            // have to make sure we are actually moving to a different location
            m_arrHistoryFull.Move(m_arrHistoryFull.GetSize()-1, i);
            return;
        }
    }


}

FILETIME CGuiCallLog::GetSMSReceivedRecentTime()
{
#ifdef USE_SMS
    CSMSInterface* oMapi = CSMSInterface::Instance();
    if(!oMapi)
        return m_ftRecentSMSReceived;

    CIssVector<TypeSMSMessage> arrSMSMsg;
    FILETIME ft;// = m_ftRecentSMSReceived;
    FileTimeToLocalFileTime(&m_ftRecentSMSReceived, &ft);
    oMapi->GetSMSMessages(arrSMSMsg, 1, ft, Enum_CE_IPM_INBOX_ENTRYID);
    if(arrSMSMsg.GetSize() > 0)
    {
        for(int i=0; i<arrSMSMsg.GetSize(); i++)
        {
            TypeSMSMessage* sMsg = arrSMSMsg[i];
            if(sMsg)
            {
                if(0 == i)
                    ft = sMsg->ft;
                delete sMsg;
            }
        }
        arrSMSMsg.RemoveAll();
        return ft;
    }
    else
#endif
        return m_ftRecentSMSReceived;
    
}

FILETIME CGuiCallLog::GetSMSSentRecentTime()
{
#ifdef USE_SMS
    CSMSInterface* oMapi = CSMSInterface::Instance();
    if(!oMapi)
        return m_ftRecentSMSSent;

    CIssVector<TypeSMSMessage> arrSMSMsg;
    FILETIME ft;// = m_ftRecentSMSSent;
    FileTimeToLocalFileTime(&m_ftRecentSMSSent, &ft);
    oMapi->GetSMSMessages(arrSMSMsg, 1, ft, Enum_CE_IPM_DRAFTS_ENTRYID);
    oMapi->GetSMSMessages(arrSMSMsg, 1, ft, Enum_IPM_SENTMAIL_ENTRYID);
    if(arrSMSMsg.GetSize() > 0)
    {
        for(int i=0; i<arrSMSMsg.GetSize(); i++)
        {
            TypeSMSMessage* sMsg = arrSMSMsg[i];
            if(sMsg)
            {
                if(0 == i)
                    ft = sMsg->ft;
                delete sMsg;
            }
        }
        arrSMSMsg.RemoveAll();
        return ft;
    }
    else
#endif
        return m_ftRecentSMSSent;
}

FILETIME CGuiCallLog::GetCallRecentTime()
{
    FILETIME ft = m_ftRecentCall;

    HANDLE hCallLog = NULL;
    if(S_OK != PhoneOpenCallLog(&hCallLog) || hCallLog == NULL)
        return ft;

    CALLLOGENTRY ce;
    ce.cbSize = sizeof(CALLLOGENTRY);
    if(S_OK == PhoneGetCallLogEntry(hCallLog, &ce))
    {
        ft = ce.ftStartTime;
        DestroyCallEntry(ce);        
    }

    PhoneCloseCallLog(hCallLog);

    return ft;
}
        
DWORD CGuiCallLog::GetCalllogCount(void)
{
    DWORD dwCount = 0;

    HANDLE hCallLog = NULL;
    if(S_OK != PhoneOpenCallLog(&hCallLog) || hCallLog == NULL)
        return 0;
    PhoneSeekCallLog(hCallLog, CALLLOGSEEK_END, 0, &dwCount);
    PhoneCloseCallLog(hCallLog);
    dwCount++;

    return dwCount;
}

DWORD CGuiCallLog::GetSMSReceivedCount(void)
{
#ifndef USE_SMS
    return 0;
#endif

    CSMSInterface* oMapi = CSMSInterface::Instance();
    if(!oMapi)
        return 0;

    int iSMSReceived = 0;

    oMapi->CountReceivedMessages(iSMSReceived);


    return iSMSReceived;
}

DWORD CGuiCallLog::GetSMSSentCount(void)
{
#ifndef USE_SMS
    return 0;
#endif

    CSMSInterface* oMapi = CSMSInterface::Instance();
    if(!oMapi)
        return 0;

    int iSMSSent = 0;
    oMapi->CountSentMessages(iSMSSent);

    return iSMSSent;
}
    
BOOL CGuiCallLog::CheckReadSMSMessages()
{
#ifndef USE_SMS
    return 0;
#endif

    CSMSInterface* oMapi = CSMSInterface::Instance();
    if(!oMapi)
        return 0;

    // check all our unread messages to see if they have been read
    CIssVector<TypeSMSMessage> arrMsgs;
    for(int i = 0; i < m_arrHistoryFull.GetSize(); i++)
    {
        CObjHistoryItem* sHistoryItem = m_arrHistoryFull[i];
        if(!sHistoryItem)
            continue;

        if(sHistoryItem->GetType() == HI_SMS)
        {
            CObjHistorySMSItem* sSMSItem = (CObjHistorySMSItem*)sHistoryItem;
            TypeSMSMessage* sMsg = sSMSItem->GetSMS();
            if(sMsg && !sMsg->bRead)
            {   
                arrMsgs.AddElement(sMsg);
            }
        }
    }

    if(arrMsgs.GetSize() > 0)
        oMapi->CheckMessageRead(arrMsgs);

    return FALSE;
}

void CGuiCallLog::SmartCollectSMSMessages(BOOL bIncoming, int iNumberToGet)
{
#ifndef USE_SMS
    return;
#endif

    CSMSInterface* oMapi = CSMSInterface::Instance();
    if(!oMapi)
        return;

    iNumberToGet = MAX_ENTRIES;
    FILETIME ftExpired;
    if(bIncoming)
        FileTimeToLocalFileTime(&m_ftRecentSMSReceived, &ftExpired);
    else 
        FileTimeToLocalFileTime(&m_ftRecentSMSSent, &ftExpired);

    CIssVector<TypeSMSMessage> arrSMSMsg;
    if(bIncoming)
    {
        oMapi->GetSMSMessages(arrSMSMsg, iNumberToGet, ftExpired, Enum_CE_IPM_INBOX_ENTRYID);
    }
    if(!bIncoming)
    {
        oMapi->GetSMSMessages(arrSMSMsg, iNumberToGet, ftExpired, Enum_CE_IPM_DRAFTS_ENTRYID);
        oMapi->GetSMSMessages(arrSMSMsg, iNumberToGet, ftExpired, Enum_IPM_SENTMAIL_ENTRYID);
    }

    FILETIME ftRecent = bIncoming?m_ftRecentSMSReceived:m_ftRecentSMSSent;

    // now assemble them into our list
    for(int i = 0; i < arrSMSMsg.GetSize(); i++)
    {
        TypeSMSMessage* sMsg = arrSMSMsg[i];
        if(!sMsg)
            continue;

        if(0 >= CompareFileTime(&sMsg->ft, &ftRecent))
        {
            delete sMsg;
            continue;
        }
        else
            ftRecent = sMsg->ft;
        
        CObjHistorySMSItem* pSMSItem = new CObjHistorySMSItem();
        if(!pSMSItem)
        {
            delete sMsg;
            continue;
        }
        pSMSItem->SetMessage(sMsg);
        if(!FindItem(pSMSItem))
        {
            m_arrHistoryFull.AddElement(pSMSItem);
            //AddItem(pSMSItem, IDMENU_SelectSMS);
            SortLastItem(); // we do in-place sorting so it's faster
        }
        else
            delete pSMSItem;
    }
    arrSMSMsg.RemoveAll();

    if(bIncoming)
        m_ftRecentSMSReceived = ftRecent;
    else
        m_ftRecentSMSSent = ftRecent;
}   

void CGuiCallLog::CollectSMSMessages(BOOL bIncludeRecieved, BOOL bIncludeSent)
{
#ifndef USE_SMS
    return;
#endif

    CSMSInterface* oMapi = CSMSInterface::Instance();
    if(!oMapi)
        return;

    int iNumberToGet = MAX_ENTRIES;//-1;  // all of them
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

    CIssVector<TypeSMSMessage> arrSMSMsgRecieved, arrSMSMsgSent;
    if(m_sOptions && bIncludeRecieved)
    {
        oMapi->GetSMSMessages(arrSMSMsgRecieved, iNumberToGet, ftExpired, Enum_CE_IPM_INBOX_ENTRYID);
    }
    if(m_sOptions && bIncludeSent)
    {
        oMapi->GetSMSMessages(arrSMSMsgSent, iNumberToGet, ftExpired, Enum_CE_IPM_DRAFTS_ENTRYID);
        oMapi->GetSMSMessages(arrSMSMsgSent, iNumberToGet, ftExpired, Enum_IPM_SENTMAIL_ENTRYID);
    }

    // now assemble them into our list
    for(int i = 0; i < arrSMSMsgRecieved.GetSize(); i++)
    {
        TypeSMSMessage* sMsg = arrSMSMsgRecieved[i];
        if(!sMsg)
            continue;

        CObjHistorySMSItem* pSMSItem = new CObjHistorySMSItem();
        if(!pSMSItem)
        {
            delete sMsg;
            continue;
        }

        pSMSItem->SetMessage(sMsg);
        //AddItem(pSMSItem, IDMENU_SelectSMS);
        m_arrHistoryFull.AddElement(pSMSItem);
        //SortLastItem(); // we do in-place sorting so it's faster

        // update the most recent sms item
        if((m_ftRecentSMSReceived.dwHighDateTime == 0 && m_ftRecentSMSReceived.dwLowDateTime == 0) || 0 < CompareFileTime(&sMsg->ft, &m_ftRecentSMSReceived))
            m_ftRecentSMSReceived = sMsg->ft;
    }
    
    // if there was nothing added we need the most recent time
    if(arrSMSMsgRecieved.GetSize() == 0)
    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
        FILETIME ft;
        SystemTimeToFileTime(&sysTime, &ft);
        LocalFileTimeToFileTime(&ft, &m_ftRecentSMSReceived);
    }
    arrSMSMsgRecieved.RemoveAll();

    // now assemble them into our list
    for(int i = 0; i < arrSMSMsgSent.GetSize(); i++)
    {
        TypeSMSMessage* sMsg = arrSMSMsgSent[i];
        if(!sMsg)
            continue;

        CObjHistorySMSItem* pSMSItem = new CObjHistorySMSItem();
        if(!pSMSItem)
        {
            delete sMsg;
            continue;
        }

        pSMSItem->SetMessage(sMsg);
        //AddItem(pSMSItem, IDMENU_SelectSMS);
        m_arrHistoryFull.AddElement(pSMSItem);
        //SortLastItem(); // we do in-place sorting so it's faster

        // update the most recent sms item
        if((m_ftRecentSMSSent.dwHighDateTime == 0 && m_ftRecentSMSSent.dwLowDateTime == 0) || 0 < CompareFileTime(&sMsg->ft, &m_ftRecentSMSSent))
            m_ftRecentSMSSent = sMsg->ft;

    }

    // if there was nothing added we need the most recent time
    if(arrSMSMsgSent.GetSize() == 0)
    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
        FILETIME ft;
        SystemTimeToFileTime(&sysTime, &ft);
        LocalFileTimeToFileTime(&ft, &m_ftRecentSMSSent);
    }
    arrSMSMsgSent.RemoveAll();
}   

void CGuiCallLog::PopulateList(BOOL bIncludeCalls, BOOL bIncludeSMSRecieved, BOOL bIncludeSMSSent, BOOL bPreventSync)
{
#ifdef DEBUG
    //return;
#endif
    DBG_OUT((_T("CGuiCallLog::PopulateList - IncludeCalls:%d, IncludeSMSRecieved%d, IncludeSMSSent:%d, PreventSync:%d"), bIncludeCalls, bIncludeSMSRecieved, bIncludeSMSSent, bPreventSync));

    ////////////////////////////////////////////////////
    // add all necessary items
    // if list has a certain count, get the full number of items needed, then trim to proper size
    // otherwise, no way to tell which ones make the top of the list
    if(bIncludeCalls && bIncludeSMSRecieved && bIncludeSMSSent)
        DestroyHistory();
    else
    {
        if(bIncludeCalls)
            RemoveCallItems();
        else
            RemoveSMSMessage(bIncludeSMSRecieved, bIncludeSMSSent);
    }

    if(bIncludeCalls)
        ZeroMemory(&m_ftRecentCall, sizeof(FILETIME));
    if(bIncludeSMSRecieved)
        ZeroMemory(&m_ftRecentSMSReceived, sizeof(FILETIME));
    if(bIncludeSMSSent)
        ZeroMemory(&m_ftRecentSMSSent, sizeof(FILETIME));

    // calls
    if(m_sOptions && bIncludeCalls)
        CollectCallItems();

    // SMS
    if(m_sOptions && (bIncludeSMSRecieved||bIncludeSMSSent))
        CollectSMSMessages(bIncludeSMSRecieved, bIncludeSMSSent);

    DBG_FT((_T("CGuiCallLog::PopulateList() m_ftRecentCall"), m_ftRecentCall));
    DBG_FT((_T("CGuiCallLog::PopulateList() m_ftRecentSMSReceived"), m_ftRecentSMSReceived));
    DBG_FT((_T("CGuiCallLog::PopulateList() m_ftRecentSMSSent"), m_ftRecentSMSSent));    

    if(bIncludeCalls)
        m_iCallHistoryCount = GetCalllogCount();
    if(bIncludeSMSRecieved)
        m_iSMSReceivedCount = GetSMSReceivedCount();
    if(bIncludeSMSSent)
        m_iSMSSentCount     = GetSMSSentCount();

    // sort the list
    m_arrHistoryFull.Sort(CompareItems);

    //trim the fat
    if(m_sOptions->eShowHistory == HISTORY_Last100)
        DestroyHistory(100);
    else if(m_sOptions->eShowHistory == HISTORY_Last200)
        DestroyHistory(200);
    else
        DestroyHistory(MAX_ENTRIES);

    // reset the visible list
    if(!bPreventSync)
        SynchronizeHistory();
}

void CGuiCallLog::SynchronizeHistory()
{
    DBG_OUT((_T("CGuiCallLog::SynchronizeHistory()")));
    ResetContent();

    if(!m_sOptions)
        return;

    // add the items as needed.  NOTE: it's already sorted
    for(int i=0; i<m_arrHistoryFull.GetSize(); i++)
    {
        CObjHistoryItem* sHistory = m_arrHistoryFull[i];
        if(!sHistory)
            continue;

        if(sHistory->GetType() == HI_SMS)
        {
            CObjHistorySMSItem* sSMS = (CObjHistorySMSItem*)sHistory;
            TypeSMSMessage* sMsg = sSMS->GetSMS();
            if(!sMsg)
                continue;
            if(m_sOptions->eShowSMS == SMS_All)
                AddItem(sSMS, IDMENU_SelectSMS);
            else if(m_sOptions->eShowSMS == SMS_Incoming && sMsg->bReceived)
                AddItem(sSMS, IDMENU_SelectSMS);
            else if(m_sOptions->eShowSMS == SMS_Outgoing && !sMsg->bReceived)
                AddItem(sSMS, IDMENU_SelectSMS);
        }
        else if(sHistory->GetType() == HI_Call)
        {
            CObjHistoryCallItem* sCall = (CObjHistoryCallItem*)sHistory;
            if(m_sOptions->eShowCall == CALL_All)
                AddItem(sCall, IDMENU_SelectCall);
            else if(m_sOptions->eShowCall == CALL_Missed && sCall->IsMissed())
                AddItem(sCall, IDMENU_SelectCall);
            else if(m_sOptions->eShowCall == CALL_Incoming && sCall->IsIncoming() && !sCall->IsMissed())
                AddItem(sCall, IDMENU_SelectCall);            
            else if(m_sOptions->eShowCall == CALL_Outgoing && !sCall->IsIncoming() && !sCall->IsMissed())
                AddItem(sCall, IDMENU_SelectCall);
            else if(m_sOptions->eShowCall == CALL_IncomingOutgoing && !sCall->IsMissed())
                AddItem(sCall, IDMENU_SelectCall);
            else if(m_sOptions->eShowCall == CALL_IncomingMissed && (sCall->IsIncoming() || sCall->IsMissed()))
                AddItem(sCall, IDMENU_SelectCall);
            else if(m_sOptions->eShowCall == CALL_OutgoingMissed && (!sCall->IsIncoming() || sCall->IsMissed()))
                AddItem(sCall, IDMENU_SelectCall);
        }
    }

    int iLineSpace = IsVGA() ? 8 : 4;
    SetItemHeights(m_iLineHeight*2 + iLineSpace, m_iLineHeight*3 + iLineSpace);


    ////////////////////////////////////////////////////
    // trim list if necessary
    int iMaxListCount = CALL_HISTORY_MAX_COUNT;//-1;
    if(m_sOptions && m_sOptions->eShowHistory == HISTORY_Last100)
        iMaxListCount = 100;
    else if(m_sOptions && m_sOptions->eShowHistory == HISTORY_Last200)
        iMaxListCount = 200;
    if(iMaxListCount > 0)
    {
        while(m_arrItems.GetSize() > iMaxListCount)
        {
            DeleteItem(m_arrItems.GetSize()-1);
            m_arrItems.RemoveElementAt(m_arrItems.GetSize()-1);
        }
    }

    ////////////////////////////////////////////////////
    DWORD dwFlags   = OPTION_Bounce;
    if(GetItemCount() > 20)
        dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar);
    Initialize(m_hWndParent, m_hWndParent, m_hInst, dwFlags, TRUE);

    // select something
    SetSelectedItemIndex(0, TRUE);
    ResetScrollPos();

    // if this screen is visible, reset missed call count to get rid of the notification
    CGuiToolBar* guiToolbar = CGuiToolBar::GetToolbar();
    if(guiToolbar && guiToolbar->GetCurSelection() == GUI_CallLog)
    {
        ResetCounts(TRUE, TRUE);
    }
}

BOOL CGuiCallLog::AddMenuItems()
{
    TypeItems* sItem = GetSelectedItem();

    BOOL bIsCallItem = TRUE;
    BOOL bUnknownContact = FALSE;
    if(sItem && sItem->lpItem)
    {
        CObjHistoryItem* oHistoryItem = (CObjHistoryItem*)sItem->lpItem;
        if(oHistoryItem && oHistoryItem->GetType() == HI_SMS)
            bIsCallItem = FALSE;
    }

    if(sItem && sItem->lpItem)
    {   //is it an unknown contact?
        CObjHistoryItem* oHistoryItem = (CObjHistoryItem*)sItem->lpItem;
        if(oHistoryItem->GetType() == HI_Call)
        {
            CObjHistoryCallItem* oHistoryItem = (CObjHistoryCallItem*)sItem->lpItem;
            if(oHistoryItem->IsUnknown())
                bUnknownContact = TRUE;
        }
    }

    if(bIsCallItem == FALSE)
        m_wndMenu->AddItem(_T("SMS Reply"), IDMENU_SMS, bIsCallItem?FLAG_Grayed:0);
    if(bUnknownContact == TRUE)
        m_wndMenu->AddItem(_T("Add to Contacts"), IDMENU_AddContact);

    m_wndMenu->AddItem(_T("Call"), IDMENU_Call, 0);
    m_wndMenu->AddItem(_T("Compose SMS"), IDMENU_NewSMS);
    m_wndMenu->AddItem(IDS_MENU_Conversation, m_hInst, IDMENU_Conversation);
	m_wndMenu->AddItem(_T("Mark All as Read"), IDMENU_MarkAllAsRead);
    m_wndMenu->AddItem(_T("Delete"), IDMENU_Delete);
    m_wndMenu->AddSeparator();

	return TRUE;
}

BOOL CGuiCallLog::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case IDMENU_SelectSMS:
        OnSelectSMS();
        break;
    case IDMENU_SelectCall:
        OnSelectCall();
        break;
    case IDMENU_Conversation:
        OnSelectConv();
        break;
    case IDMENU_Call:
        {
            if(GetSelectedNumber())
            {
                PHONEMAKECALLINFO mci = {0};
                mci.cbSize = sizeof(mci);
                mci.dwFlags = 0;
                mci.pszDestAddress = GetSelectedNumber();
                PhoneMakeCall(&mci);
            }
        }
        break;
    case IDMENU_SMS:
        {
            if(GetSelectedNumber())
            {
                PROCESS_INFORMATION pi;
                TCHAR szCommandLine[STRING_MAX*2];
                m_oStr->Format(szCommandLine,  _T("-service \"SMS\" -to \"%s\""), GetSelectedNumber());
                CreateProcess(_T("tmail.exe"), szCommandLine, NULL, NULL, FALSE, NULL, NULL, NULL, NULL, &pi);
            }
        }
        break;
    case IDMENU_NewSMS:
        {
        PROCESS_INFORMATION pi;
        TCHAR szCommandLine[STRING_MAX*2];
        m_oStr->Format(szCommandLine,  _T("-service \"SMS\" -to \"\""));
        CreateProcess(_T("tmail.exe"), szCommandLine, NULL, NULL, FALSE, NULL, NULL, NULL, NULL, &pi);
        break;
        }
    case IDMENU_Delete:
        OnDelete();
        break;
    case IDMENU_DeleteEntry:
        OnDeleteEntry();
        break;
    case IDMENU_DeleteAllSMS:
        OnDeleteAllSMS();
        break;
    case IDMENU_DeleteAllCalls:
        OnDeleteAllCalls();
        break;
    case IDMENU_DeleteAll:
        OnDeleteAll();
        break;
	case IDMENU_MarkAllAsRead:
		MarkAllAsRead();
		break;
    case IDMENU_AddContact:
        ShowWindow(m_hWndParent, SW_HIDE);
        CPoomContacts::Instance()->CreateNew(m_hWndParent, GetSelectedNumber());
        ShowWindow(m_hWndParent, SW_SHOW);
        break;
    }
	return TRUE;
}

void CGuiCallLog::OnDelete()
{
    m_wndMenu->ResetContent();
    m_wndMenu->AddCategory(_T("Delete"));

    TypeItems* sItem = GetSelectedItem();

    m_wndMenu->AddItem(_T("Delete entry"), IDMENU_DeleteEntry, (!sItem?FLAG_Grayed:NULL));
    m_wndMenu->AddItem(_T("Delete all calls"), IDMENU_DeleteAllCalls);
#ifdef USE_SMS
    m_wndMenu->AddItem(_T("Delete all SMS"), IDMENU_DeleteAllSMS);
#endif
    m_wndMenu->AddItem(_T("Delete all"), IDMENU_DeleteAll);

    m_wndMenu->SetSelectedItemIndex(0, TRUE);

    int iX = WIDTH(GetFrame())/6;

    Sleep(500);
    m_wndMenu->PopupMenu(m_hWndParent, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        iX,10,WIDTH(GetFrame())*2/3,GetFrame().bottom - 10,
        GetFrame().left, GetFrame().bottom - 10, iX,10, ADJUST_Bottom);
}

void CGuiCallLog::OnDeleteEntry()
{
    int iIndex = GetSelectedItemIndex();
    TypeItems* sItem = GetSelectedItem();
    if(iIndex == -1 || !sItem || !sItem->lpItem)
    {
        MessageBeep(MB_ICONHAND);
        return;
    }

    CObjHistoryItem* oItem = (CObjHistoryItem*)sItem->lpItem;

    int iFullIndex = FindFullIndex(oItem);
    if(iFullIndex == -1)
    {
        MessageBeep(MB_ICONHAND);
        return;
    }

    if(oItem->GetType() == HI_Call)
    {
        // delete the call
        CObjHistoryCallItem* oCall = (CObjHistoryCallItem*)sItem->lpItem;
        FILETIME ftTime = oCall->GetTime();

        if(S_OK == CCallLog::DeleteCallEntry(ftTime))
        {
            // take it off the list
            delete oItem;
            m_arrHistoryFull.RemoveElementAt(iFullIndex);
            RemoveItem(iIndex);
            InvalidateRect(m_hWndParent, &GetFrame(), FALSE);
            m_iCallHistoryCount = GetCalllogCount();
        }
        else
        {
            MessageBeep(MB_ICONHAND);
        }
    }
    else
    {
        // delete the SMS
        CSMSInterface* oMapi = CSMSInterface::Instance();

        CObjHistorySMSItem* oSMS = (CObjHistorySMSItem*)sItem->lpItem;
        TypeSMSMessage* sSMS = oSMS->GetSMS();
        if(!sSMS)
        {
            // didn't work.. don't know why
            MessageBeep(MB_ICONHAND);
            return;
        }

        if(S_OK == oMapi->DeleteSMSMessage(sSMS))
        {
            if(sSMS->bReceived)
                m_iSMSReceivedCount = GetSMSReceivedCount();
            else
                m_iSMSSentCount     = GetSMSSentCount();
            delete oItem;
            m_arrHistoryFull.RemoveElementAt(iFullIndex);
            RemoveItem(iIndex);
            InvalidateRect(m_hWndParent, &GetFrame(), FALSE);
        }
        else
        {
            MessageBeep(MB_ICONHAND);
        }
    }    
}

int CGuiCallLog::FindFullIndex(CObjHistoryItem* oItem)
{
    for(int i=0; i<m_arrHistoryFull.GetSize(); i++)
    {
        if(oItem == m_arrHistoryFull[i])
            return i;
    }

    return -1;
}

void CGuiCallLog::OnDeleteAllSMS(BOOL bUseMsg)
{
    int iReturnVal = IDYES;
    if(bUseMsg)
    {
        CDlgMsgBox msgBox;
        msgBox.Init(m_gdiMem, m_guiBackground);
        iReturnVal = msgBox.PopupMessage(_T("Are you sure you want to permanently delete your SMS history?"), _T("Delete SMS"), m_hWndParent, m_hInst, MB_YESNO);
    }

    if(iReturnVal == IDYES)
    {
       CSMSInterface* oMapi = CSMSInterface::Instance(); 

       //PA: We don't care if it fails because it could be already gone from another app
       oMapi->DeleteAllSMS();      

       BOOL bDeleted = FALSE;
       for(int i = m_arrHistoryFull.GetSize() - 1; i >= 0; i--)
       {
           CObjHistoryItem* oItem = m_arrHistoryFull[i];
           if(!oItem)
               continue;

           if(oItem->GetType() == HI_SMS)
           {
               bDeleted = TRUE;
               delete oItem;
               m_arrHistoryFull.RemoveElementAt(i);
           }
       }

       // resync the visible history
       if(bDeleted)
           SynchronizeHistory();

       m_iSMSSentCount      = GetSMSSentCount();
       m_iSMSReceivedCount  = GetSMSReceivedCount();

       if(bUseMsg)
           InvalidateRect(m_hWndParent, &GetFrame(), FALSE);
    }

}

void CGuiCallLog::OnDeleteAllCalls(BOOL bUseMsg)
{
    int iReturnVal = IDYES;
    if(bUseMsg)
    {
        CDlgMsgBox msgBox;
        msgBox.Init(m_gdiMem, m_guiBackground);
        iReturnVal = msgBox.PopupMessage(_T("Are you sure you want to permanently delete your call history?"), _T("Delete Call History"), m_hWndParent, m_hInst, MB_YESNO);
    }

    if(iReturnVal == IDYES)
    {
        // PA: We dont' care if it fails because it could be already gone from another app
        CCallLog::DeleteAllCallEntry();

        // delete all the calls
        BOOL bDeleted = FALSE;
        for(int i=m_arrHistoryFull.GetSize()-1; i>=0; i--)
        {
            CObjHistoryItem* oItem = m_arrHistoryFull[i];
            if(!oItem)
                continue;

            if(oItem->GetType() == HI_Call)
            {
                bDeleted = TRUE;
                delete oItem;
                m_arrHistoryFull.RemoveElementAt(i);
            }
        }

        // resync the visible history
        if(bDeleted)
            SynchronizeHistory();

        m_iCallHistoryCount = GetCalllogCount();

        if(bUseMsg)
            InvalidateRect(m_hWndParent, &GetFrame(), FALSE);
    }

}

void CGuiCallLog::OnDeleteAll()
{
    CDlgMsgBox msgBox;
    msgBox.Init(m_gdiMem, m_guiBackground);
    int iReturnVal = msgBox.PopupMessage(_T("Are you sure you want to permanently delete your history?"), _T("Delete History"), m_hWndParent, m_hInst, MB_YESNO);

    // delete it
    if(iReturnVal == IDYES)
    {
        OnDeleteAllSMS(FALSE);
        OnDeleteAllCalls(FALSE);
        InvalidateRect(m_hWndParent, &GetFrame(), FALSE);
    }
}

void CGuiCallLog::MarkAllAsRead()
{
	// reset missed call count
    CIssStateAndNotify::ResetMissedCallsCount();

	// set SMS recieved messages as read
	int iUnreadCount = CIssStateAndNotify::GetUnreadSMSCount();
    
    CSMSInterface* oMapi = CSMSInterface::Instance();
    if(oMapi && iUnreadCount > 0)
        oMapi->SetSMSMessagesRead(iUnreadCount);

    // show all messages have been read
    for(int i=0; i<m_arrHistoryFull.GetSize(); i++)
    {
        CObjHistoryItem* sHistory = m_arrHistoryFull[i];
        if(!sHistory)
            continue;

        if(sHistory->GetType() == HI_SMS)
        {
            CObjHistorySMSItem* sSMS = (CObjHistorySMSItem*)sHistory;
            TypeSMSMessage* sMsg = sSMS->GetSMS();
            if(!sMsg)
                continue;
            sMsg->bRead     = TRUE;
            
        }
    }

}
    
TCHAR* CGuiCallLog::GetSelectedNumber()
{
    TypeItems* sItem = GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return NULL;

    CObjHistoryItem* oHistoryItem = (CObjHistoryItem*)sItem->lpItem;
    if(oHistoryItem)
    {
        return oHistoryItem->GetNumber();
    }

    return NULL;
}

void CGuiCallLog::OnSelectSMS()
{
    TypeItems* sItem = GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    CObjHistorySMSItem* oSMS = (CObjHistorySMSItem*)sItem->lpItem;
    TypeSMSMessage* sSMS = oSMS->GetSMS();
    if(!sSMS)
        return;

    CDlgPerContactSMS dlg(TRUE, m_dlgContactDetails);
    dlg.SetSMSMessage(sSMS);
    dlg.SetNameOverride(sSMS->szNumber);    // for now until we get the OID out
    dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
    dlg.Launch(m_hWndParent, m_hInst, m_sOptions->bShowAnimations);
    // set read
    sSMS->bRead = TRUE;
}

void CGuiCallLog::OnSelectCall()
{
    TypeItems* sItem = GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    CObjHistoryCallItem* oCall = (CObjHistoryCallItem*)sItem->lpItem;

    CDlgPerContactCall dlg(TRUE, m_dlgContactDetails);
    dlg.SetCallItem(oCall);
    dlg.SetNameOverride(oCall->GetName());    // for now until we get the OID out
    dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
    dlg.Launch(m_hWndParent, m_hInst, m_sOptions->bShowAnimations);
}
    
void CGuiCallLog::OnSelectConv()
{
    TypeItems* sItem = GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return;

    CObjHistoryItem* oHistoryItem = (CObjHistoryItem*)sItem->lpItem;

    CDlgPerContactConv dlg(TRUE, m_dlgContactDetails);
	dlg.SetContact(oHistoryItem, m_sOptions, &GetIconArray());
	dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
    dlg.Launch(m_hWndParent, m_hInst, m_sOptions->bShowAnimations);
}

int CGuiCallLog::CompareItems( const void *arg1, const void *arg2 )
{
    if(!arg1 || !arg2)
        return 0;
    //TypeItems& sListItem1 = **(TypeItems **)arg1;
    //TypeItems& sListItem2 = **(TypeItems **)arg2;

    CObjHistoryItem& pEntry1	= **(CObjHistoryItem**)arg1;
    CObjHistoryItem& pEntry2	= **(CObjHistoryItem**)arg2;

    return CompareFileTime(&pEntry2.GetTime(), &pEntry1.GetTime());
}

BOOL CGuiCallLog::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    DWORD dwCallCount           = m_iCallHistoryCount;
    DWORD dwSMSRecievedCount    = m_iSMSReceivedCount;
    DWORD dwSMSSentCount        = m_iSMSSentCount;
    FILETIME ftCall             = m_ftRecentCall;
    FILETIME ftSent             = m_ftRecentSMSSent;
    FILETIME ftRecieved         = m_ftRecentSMSReceived;

    if(!m_sOptions)
        return TRUE;

    switch(uiMessage)
    {
    case WM_NOTIFY_TIME:
    {
        DBG_OUT((_T("WM_NOTIFY_TIME")));

        if(!IsWindowVisible(hWnd))
            return TRUE;

        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

#ifndef DEBUG
        if(sysTime.wMinute % 2 == 0)
#endif
        {
            BOOL bSyncUp = FALSE;
            //if(m_sOptions->eShowCall != CALL_Off)
            {
                dwCallCount = GetCalllogCount();
                DBG_OUT((_T("Call count NEW: %d, OLD: %d"), dwCallCount, m_iCallHistoryCount));

                if(dwCallCount < (DWORD)m_iCallHistoryCount)
                {
                    // rebuild the call list
                    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
                    PopulateList(TRUE, FALSE, FALSE, TRUE);
                    ::SetCursor(hCursor);
                    bSyncUp = TRUE;
                }
            }

            //if(m_sOptions->eShowSMS == SMS_Incoming || m_sOptions->eShowSMS == SMS_All)
            {
                dwSMSRecievedCount = GetSMSReceivedCount();
                DBG_OUT((_T("SMS recieved NEW: %d, OLD: %d"), dwSMSRecievedCount, m_iSMSReceivedCount));
                if(dwSMSRecievedCount < (DWORD)m_iSMSReceivedCount)
                {
                    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
                    PopulateList(FALSE, TRUE, FALSE, TRUE);
                    ::SetCursor(hCursor);
                    bSyncUp = TRUE;
                }
            }

            //if(m_sOptions->eShowSMS == SMS_Outgoing || m_sOptions->eShowSMS == SMS_All)
            {
                dwSMSSentCount = GetSMSSentCount();
                DBG_OUT((_T("SMS Sent NEW: %d, OLD: %d"), dwSMSSentCount, m_iSMSSentCount));
                if(dwSMSSentCount < (DWORD)m_iSMSSentCount)
                {
                    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
                    PopulateList(FALSE, FALSE, TRUE, TRUE);
                    ::SetCursor(hCursor);
                    bSyncUp = TRUE;
                }
            }

            // now sync only once with the visible list
            if(bSyncUp)
                SynchronizeHistory();
        }

        // check sms sent count
        //if(m_sOptions->eShowSMS == SMS_Incoming || m_sOptions->eShowSMS == SMS_All)
            //dwSMSRecievedCount = GetSMSReceivedCount();
            ftRecieved = GetSMSReceivedRecentTime();

        //if(m_sOptions->eShowSMS == SMS_Outgoing || m_sOptions->eShowSMS == SMS_All)
            //dwSMSSentCount = GetSMSSentCount();
            ftSent = GetSMSSentRecentTime();

        // check call log count
        //if(m_sOptions->eShowCall != CALL_Off)
            //dwCallCount = GetCalllogCount();
            ftCall = GetCallRecentTime();

        break;
    }
    case WM_NOTIFY_MISSED:
    case WM_NOTIFY_TALKING:

        DBG_OUT((_T("WM_NOTIFY_MISSED or WM_NOTIFY_TALKING")));
        // call must be ended
        if(WM_NOTIFY_TALKING == uiMessage && (wParam & SN_PHONECALLTALKING_BITMASK))
            return TRUE;

        // check call log count
        //if(m_sOptions->eShowCall != CALL_Off)
        {
            //dwCallCount = GetCalllogCount();
            //DBG_OUT((_T("GetCalllogCount - %d"), dwCallCount));
            ftCall = GetCallRecentTime();
        }
        break;
    case WM_NOTIFY_SMS:

        DBG_OUT((_T("WM_NOTIFY_SMS")));

        // check sms sent count
        //if(m_sOptions->eShowSMS == SMS_Incoming || m_sOptions->eShowSMS == SMS_All)
            //dwSMSRecievedCount = GetSMSReceivedCount();
            ftRecieved = GetSMSReceivedRecentTime();

        //if(m_sOptions->eShowSMS == SMS_Outgoing || m_sOptions->eShowSMS == SMS_All)
            //dwSMSSentCount = GetSMSSentCount();
            ftSent = GetSMSSentRecentTime();

        // we know there has been a change in the SMS counter so check if something was read
        //CheckReadSMSMessages();
        break;
    }

    //DBG_OUT((_T("dwCallCount - %d  m_iCallHistoryCount - %d"), dwCallCount, m_iCallHistoryCount));
    //DBG_OUT((_T("dwSMSRecievedCount - %d  m_iSMSReceivedCount - %d"), dwSMSRecievedCount, m_iSMSReceivedCount));
    //DBG_OUT((_T("dwSMSSentCount - %d  m_iSMSSentCount - %d"), dwSMSSentCount, m_iSMSSentCount));
    //if(dwCallCount != m_iCallHistoryCount || dwSMSRecievedCount != m_iSMSReceivedCount || dwSMSSentCount != m_iSMSSentCount)
    /*DBG_FT((_T("CGuiCallLog::OnUser - m_ftRecentCall"), m_ftRecentCall));
    DBG_FT((_T("CGuiCallLog::OnUser - m_ftRecentSMSReceived"), m_ftRecentSMSReceived));
    DBG_FT((_T("CGuiCallLog::OnUser - m_ftRecentSMSSent"), m_ftRecentSMSSent));

    DBG_FT((_T("CGuiCallLog::OnUser - ftCall"), ftCall));
    DBG_FT((_T("CGuiCallLog::OnUser - ftRecieved"), ftRecieved));
    DBG_FT((_T("CGuiCallLog::OnUser - ftSent"), ftSent));*/

    if(0 < CompareFileTime(&ftCall, &m_ftRecentCall) || 0 < CompareFileTime(&ftRecieved, &m_ftRecentSMSReceived) || 0 < CompareFileTime(&ftSent, &m_ftRecentSMSSent))
    {
        HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
        SmartPopulateList(0 < CompareFileTime(&ftCall, &m_ftRecentCall), 
                          0 < CompareFileTime(&ftRecieved, &m_ftRecentSMSReceived), 
                          0 < CompareFileTime(&ftSent, &m_ftRecentSMSSent));
        ::SetCursor(hCursor);
        InvalidateRect(m_hWndParent, &m_rcList, FALSE);
    }

	return TRUE;
}

void CGuiCallLog::SmartPopulateList(BOOL bCall, BOOL bRecieved, BOOL bSent)
{
#ifdef DEBUG
    //return;
#endif
    DBG_OUT((_T("Smart PopulateList - Call:%d, Recieved:%d, Sent:%d"), bCall, bRecieved, bSent));

    //int iDiffCount;

    if(!m_sOptions)
        return;

    //////////////////////////////////////////////////////////////////////////
    // call history
    if(bCall)
    {
        //iDiffCount = abs(m_iCallHistoryCount - dwCallCount);
        //DBG_OUT((_T("Calling SmartCollectCallItems at iDiffCount - %d"), iDiffCount));
        SmartCollectCallItems(1);
        m_iCallHistoryCount = GetCalllogCount();
    }

    //////////////////////////////////////////////////////////////////////////
    // SMS received
    if(bRecieved)
    {
        //iDiffCount = abs(m_iSMSReceivedCount - dwSMSRecievedCount);
        //DBG_OUT((_T("Calling SmartCollectSMSMessages at iDiffCount - %d"), iDiffCount));
        SmartCollectSMSMessages(TRUE, 1);
        m_iSMSReceivedCount = GetSMSReceivedCount();
    }

    //////////////////////////////////////////////////////////////////////////
    // SMS Sent
    if(bSent)
    {
        //iDiffCount = abs(m_iSMSSentCount - dwSMSSentCount);
        //DBG_OUT((_T("Calling SmartCollectSMSMessages at iDiffCount - %d"), iDiffCount));
        SmartCollectSMSMessages(FALSE, 1);
        m_iSMSSentCount = GetSMSSentCount();
    }

    // resync the visible list
    SynchronizeHistory();
}


//////////////////////////////////////////////////////////////////////////
// history object classes implementation

//////////////////////////////////////////////////////////////////////////
SIZE CObjHistoryItem::m_sizeDate = {0};

// CObjHistoryItem base class implementation
CObjHistoryItem::CObjHistoryItem(EnumHistoryItemType eType) 
: m_eType(eType)  
{
	m_iLineHeight	= GetSystemMetrics(SM_CXSMICON);
}

CObjHistoryItem::~CObjHistoryItem()
{
}

HRESULT CObjHistoryItem::Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText, BOOL bDrawName)
{
    HRESULT hr = S_OK;

    if(m_sizeDate.cx == 0)
    {
        TCHAR szDate[] = _T("10:00p 00/00");
        HFONT hOld = (HFONT)SelectObject(gdiDest.GetDC(), CDlgContacts::GetFontNormal());
        GetTextExtentPoint(gdiDest, szDate, _tcslen(szDate), &m_sizeDate);
        SelectObject(gdiDest.GetDC(), hOld);
        //m_iDateWidth = sz.cx + IsVGA()?4:2;
    }

    // draw icon
    int iIconWidth = gdiIcon.GetWidth() / NUM_HISTORY_ICONS;
    RECT rcIcon = rcDraw;
    rcIcon.bottom = rcIcon.top + gdiIcon.GetHeight();
    rcIcon.right = rcIcon.left + iIconWidth;
    hr = ::Draw(gdiDest, rcIcon, gdiIcon, GetIconIndex() * iIconWidth, 0);

    // draw line separator
    RECT rcLine = rcDraw;
    rcLine.top = rcLine.bottom - 1;// for now
    int iPenWidth = IsVGA() ? 2 : 1;
    hr = ::Line(gdiDest.GetDC(), rcLine.left + 4, rcLine.top, rcLine.right - 4, rcLine.top, 0x454545, iPenWidth);

    return hr;
}
    

//////////////////////////////////////////////////////////////////////////
// CObjHistoryCallItem implementation
CObjHistoryCallItem::CObjHistoryCallItem():CObjHistoryItem(HI_Call)
, m_szName(NULL)
, m_szNumber(NULL)
, m_szNameType(NULL)
, m_bUnknown(FALSE)
{
}

CObjHistoryCallItem::~CObjHistoryCallItem()
{
    CIssString* oStr = CIssString::Instance();
    if(m_szName)
        oStr->Delete(&m_szName);
    if(m_szNumber)
        oStr->Delete(&m_szNumber);
    if(m_szNameType)
        oStr->Delete(&m_szNameType);
}

CObjHistoryItem* CObjHistoryCallItem::Clone()
{
    CObjHistoryCallItem* objNewItem = new CObjHistoryCallItem();
    if(!objNewItem)
        return NULL;

    CopyMemory(objNewItem, this, sizeof(CObjHistoryCallItem));

    // create strings for name and number
    CIssString* oStr = CIssString::Instance();
    objNewItem->m_szName = oStr->CreateAndCopy(m_szName);
    objNewItem->m_szNumber = oStr->CreateAndCopy(m_szNumber);
    objNewItem->m_szNameType = oStr->CreateAndCopy(m_szNameType);

    return (CObjHistoryItem*)objNewItem;
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

HRESULT CObjHistoryCallItem::Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText, BOOL bDrawName)
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
    rcText.left += iIconWidth + INDENT;
    rcText.right -=INDENT;
    rcText.bottom = rcText.top + m_iLineHeight;

    rcTextLeft = rcText;
    rcTextLeft.right -= m_sizeDate.cx;
    if(bDrawName)
        DrawText(gdiDest.GetDC(), m_szName, rcTextLeft, 
            DT_LEFT | DT_VCENTER /*|DT_NOPREFIX*/ | DT_END_ELLIPSIS, 
            CDlgContacts::GetFontBold(), 
            crText);



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

    DrawText(gdiDest.GetDC(), szText, rcText, DT_RIGHT | DT_VCENTER, CDlgContacts::GetFontNormal(), crText);
    
    
    ///////////////////////////////////////////////////////////
    // line 2
    // phone number
    OffsetRect(&rcText, 0, m_iLineHeight);
    rcText.left += INDENT;

    if(oStr->IsEmpty(m_szNameType))
    {
        oStr->StringCopy(szText, m_szNumber);
    }
    else
    {
        oStr->Format(szText, _T("%s %s"), m_szNumber, m_szNameType);
    }

    DrawText(gdiDest.GetDC(), szText, rcText, DT_LEFT | DT_VCENTER, CDlgContacts::GetFontNormal(), crText);

    if(bIsHighlighted)
    {
        static TCHAR szDuration[STRING_NORMAL];

        if(m_bMissed == FALSE)
        {
            int iDuration = m_iDuration;
            int	iHour = iDuration/3600;
            iDuration -= (iHour*3600);
            int iMinute = iDuration/60;
            iDuration -= (iMinute*60);
            oStr->Format(szDuration, _T("%02d:%02d"), iMinute, iDuration);
        }


        OffsetRect(&rcText, 0, m_iLineHeight);

        // draw some other stuff perhaps
        if(m_bMissed)
        {
            DrawText(gdiDest.GetDC(), _T("Missed Call"), rcText, DT_LEFT | DT_BOTTOM, CDlgContacts::GetFontNormal(), crText);
        }
        else if(m_bIsIncoming)
        {
            oStr->Format(szText, _T("Incoming Call: %s"), szDuration);
            DrawText(gdiDest.GetDC(), szText, rcText, DT_LEFT | DT_BOTTOM, CDlgContacts::GetFontNormal(), crText);
        }
        else
        {
            oStr->Format(szText, _T("Outgoing Call: %s"), szDuration);
            DrawText(gdiDest.GetDC(), szText, rcText, DT_LEFT | DT_BOTTOM, CDlgContacts::GetFontNormal(), crText);
        }
    }

Error:
    return hr;
}

//////////////////////////////////////////////////////////////////////////
// CObjHistorySMSItem implementation
CObjHistorySMSItem::CObjHistorySMSItem():CObjHistoryItem(HI_SMS)
,m_sMsg(NULL)
,m_iTwoLineCount(-1)
,m_iWidthText(0)
{
}

CObjHistorySMSItem::~CObjHistorySMSItem()
{
    if(m_sMsg)
        delete m_sMsg;
}

CObjHistoryItem* CObjHistorySMSItem::Clone()
{
    CObjHistorySMSItem* objNewItem = new CObjHistorySMSItem();
    if(!objNewItem)
        return NULL;

    CopyMemory(objNewItem, this, sizeof(CObjHistorySMSItem));

    // create a new message
    objNewItem->SetMessage(new TypeSMSMessage);
    objNewItem->m_sMsg->Clone(m_sMsg);

    return (CObjHistoryItem*)objNewItem;
}
   

HRESULT CObjHistorySMSItem::Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText, BOOL bDrawName)
{
    HRESULT hr;
    RECT rcText, rcTextLeft;
    hr = CObjHistoryItem::Draw(gdiDest, gdiIcon, rcDraw, bIsHighlighted, crText);
    CHR(hr, _T("CObjHistoryItem::Draw() failed"));

    CPHRE(m_sMsg, _T("CObjHistorySMSItem::m_sMsg is NULL"), E_INVALIDARG);

    CIssString* oStr = CIssString::Instance();
    static TCHAR szText[STRING_MAX];
    oStr->Empty(szText);


    COLORREF cr = crText;
    if(!m_sMsg->bRead)
        cr = COLOR_HISTORY_SMS_UNREAD;
    //////////////////////////////////////////////////////////////
    // line 1
    int iIconWidth = gdiIcon.GetWidth() / NUM_HISTORY_ICONS;
    // name
    rcText = rcDraw;
    rcText.left += iIconWidth + INDENT;
    rcText.right -=INDENT;
    rcText.bottom = rcText.top + m_iLineHeight;

    rcTextLeft = rcText;
    rcTextLeft.right -= m_sizeDate.cx;
    if(m_sMsg->szNumber && bDrawName)
        DrawText(gdiDest.GetDC(), m_sMsg->szNumber, rcTextLeft, 
        DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS, CDlgContacts::GetFontBold(), cr);
    
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
    hr = DrawText(gdiDest.GetDC(), szText, rcText, DT_RIGHT | DT_VCENTER, CDlgContacts::GetFontNormal(), crText);
   
    ///////////////////////////////////////////////////////////
    // line 2
    // old code
    OffsetRect(&rcText, 0, m_iLineHeight);
    rcText.bottom = rcDraw.bottom;

    if(!bIsHighlighted)
    {
        // 1 line of message
        rcText.right -= INDENT*4;
        //leave the DT_VCENTER commented out otherwise it doesn't handle line breaks properly (draws boxes)
        if(m_sMsg->szSubject)
            hr = DrawText(gdiDest.GetDC(), m_sMsg->szSubject, rcText, 
            DT_LEFT/* | DT_VCENTER*/ | DT_END_ELLIPSIS | DT_NOPREFIX | DT_WORDBREAK, 
            CDlgContacts::GetFontNormal(), crText);
    }
    else
    {
        if(m_iTwoLineCount < 0 || m_iWidthText != WIDTH(rcText))
        {
            // draw 2 lines of text with DT_END_ELLIPSIS
            // start with 80 chars, shorten if it doesn't fit
            const int iMaxStringCount = 80;
            rcText.bottom = rcText.top + m_iLineHeight*2;
            _tcsncpy(szText, m_sMsg->szSubject, iMaxStringCount);
            if(oStr->GetLength(m_sMsg->szSubject) > iMaxStringCount)
            {
                oStr->Left(szText, szText, oStr->GetLength(szText)-3);
                oStr->Concatenate(szText, _T("..."));
            }
            // see if it fits out RECT
            HFONT hOldFont = (HFONT)SelectObject(gdiDest.GetDC(), CDlgContacts::GetFontNormal());    
            RECT rcTest = rcText;
            ::DrawText(gdiDest.GetDC(), szText, -1, &rcTest, DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT );
            if(rcTest.bottom > rcText.bottom + 1)
            {
                while(rcTest.bottom > rcText.bottom + 1)
                {
                    // shorten the string and test
                    // shorten by a few chars, don't want to do this very many times
                    if(oStr->GetLength(szText) < 16)
                        break;
                    oStr->Left(szText, szText, oStr->GetLength(szText)-8);  
                    oStr->Concatenate(szText, _T("..."));
                    SetRect(&rcTest, rcText.left, rcText.top, rcText.right, rcText.bottom);
                    ::DrawText(gdiDest.GetDC(), szText, -1, &rcTest, DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT );
                }
            }
            SelectObject(gdiDest.GetDC(), hOldFont); 

            m_iWidthText    = WIDTH(rcText);
            m_iTwoLineCount = oStr->GetLength(szText) - 3;
        }
        else
        {
            //only if it needs to be shortened
            if(oStr->GetLength(m_sMsg->szSubject) != m_iTwoLineCount + 3)
            {
                oStr->StringCopy(szText, m_sMsg->szSubject, 0, m_iTwoLineCount);
                oStr->Concatenate(szText, _T("..."));
            }
            //otherwise just draw it
            else
            {
                oStr->StringCopy(szText, m_sMsg->szSubject);
            }
        }

        DrawText(gdiDest.GetDC(), szText, rcText, DT_WORDBREAK | DT_NOPREFIX, CDlgContacts::GetFontNormal(), crText);
         
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






