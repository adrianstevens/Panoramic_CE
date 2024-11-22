#include "StdAfx.h"
#include "GuiCallLog.h"
#include "ContactsGuiDefines.h"
#include "DlgContactDetails.h"
#include "Resource.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "IssDebug.h"
#include "GuiToolBar.h"


#ifndef NEW_CALLLOG_GUI

CGuiCallLog::CGuiCallLog(void)
: m_oCallList(0)
, m_iTitleHeight(GetSystemMetrics(SM_CXICON))
{
	m_hFontHeading		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
	m_hFontEntry		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
	m_oCallList = new CCallList(this);
}

CGuiCallLog::~CGuiCallLog(void)
{
	CIssGDIEx::DeleteFont(m_hFontHeading);
	CIssGDIEx::DeleteFont(m_hFontEntry);

	if(m_oCallList)
	{
		delete m_oCallList;
		m_oCallList = 0;
	}

}

BOOL CGuiCallLog::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBackground, TypeOptions* sOptions)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBackground, sOptions);

	if(m_oCallList)
		m_oCallList->Init(hWndParent, hInst, gdiMem, gdiBackground, sOptions);

	//m_bFadeIn = TRUE;

	//PostMessage(m_hWndParent, WM_FadeIn, 0,0);
	return TRUE;
}

BOOL CGuiCallLog::Draw(HDC hDC, RECT& rcClient, RECT& rcClip)
{
	if(m_gdiMem->GetDC() == NULL)
		m_gdiMem->Create(hDC, m_rcLocation, FALSE, FALSE, FALSE);

    RECT rcZero = {0,0,WIDTH(rcClient), HEIGHT(rcClient)};
    DrawGradientGDI(m_gdiMem->GetDC(), rcZero, 0, 0xFF7836);

	DrawHeader(m_gdiMem->GetDC(), m_rcHeading);

	if(m_oCallList)
		m_oCallList->Draw(m_gdiMem->GetDC(), m_rcCallList);

    // draw the selector part over top if initiated from this class
    if(HEIGHT(rcClient) > HEIGHT(m_rcLocation))
        CGuiToolBar::GetToolbar()->DrawSelector(*m_gdiMem, m_rcLocation, rcClip);

    BitBlt(hDC,
        m_rcLocation.left,m_rcLocation.top,
        WIDTH(m_rcLocation), HEIGHT(m_rcLocation),
        m_gdiMem->GetDC(),
        0, 0,
        SRCCOPY);

	return TRUE;
}

void CGuiCallLog::DrawGradientGDI(HDC tdc, RECT rc, COLORREF StartRGB, COLORREF EndRGB)
{
    unsigned int Shift = 8;
    TRIVERTEX        vert[2] ;
    GRADIENT_RECT    gRect;
    vert [0] .x      = rc.left;
    vert [0] .y      = rc.top;
    vert [0] .Red    = GetRValue(StartRGB) << Shift;
    vert [0] .Green  = GetGValue(StartRGB) << Shift;
    vert [0] .Blue   = GetBValue(StartRGB) << Shift;
    vert [0] .Alpha  = 0x0000;
    vert [1] .x      = rc.right;
    vert [1] .y      = rc.bottom; 
    vert [1] .Red    = GetRValue(EndRGB) << Shift;
    vert [1] .Green  = GetGValue(EndRGB) << Shift;
    vert [1] .Blue   = GetBValue(EndRGB) << Shift;
    vert [1] .Alpha  = 0x0000;
    gRect.UpperLeft  = 0;
    gRect.LowerRight = 1;
    GradientFill(tdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);
}

BOOL CGuiCallLog::DrawHeader(HDC hdc, RECT rcClient)
{
	if(m_gdiHeader.GetDC() == NULL)
	{
		m_gdiHeader.Create(hdc, m_rcHeading);
		ShinyRoundRectangle(m_gdiHeader, m_rcHeading, 0xD3BD84, 0, 0, 0xD3BD84, 0x9E6331, 0x7D3108, 0xDF425A);
	}

	BitBlt(hdc,
		rcClient.left,rcClient.top,
		WIDTH(m_rcHeading), HEIGHT(m_rcHeading),
		m_gdiHeader.GetDC(),
		0,0,
		SRCCOPY);

	// create 4 rects for text...
	RECT rcLineOneLeft, rcLineOneRight, rcLineTwoLeft, rcLineTwoRight;
	rcLineOneLeft = rcLineOneRight = rcLineTwoLeft = rcLineTwoRight = m_rcHeading;
	rcLineOneLeft.right = rcLineTwoLeft.right = rcLineOneRight.left = rcLineTwoRight.left = m_rcHeading.left + WIDTH(m_rcHeading) / 2;
	rcLineOneLeft.bottom = rcLineOneRight.bottom = rcLineTwoLeft.top = rcLineTwoRight.top = m_rcHeading.top + HEIGHT(m_rcHeading) / 2;
	
	static BSTR szHeading1 = _T("Call Log for: ");
	static BSTR szHeading2 = _T("Show: ");
	TCHAR* szContactName = m_oCallList->GetCurContactName();
	TCHAR* szDisplayType = m_oCallList->GetCallTypeString();

	DrawText(hdc, szHeading1,		rcLineOneLeft,	DT_RIGHT | DT_END_ELLIPSIS, m_hFontHeading, 0xFFFFFF);
	DrawText(hdc, szContactName,	rcLineOneRight, DT_LEFT | DT_END_ELLIPSIS,	m_hFontEntry,	0xA0FFFF);
	DrawText(hdc, szHeading2,		rcLineTwoLeft,	DT_RIGHT | DT_END_ELLIPSIS, m_hFontHeading, 0xFFFFFF);
	DrawText(hdc, szDisplayType,	rcLineTwoRight, DT_LEFT | DT_END_ELLIPSIS,	m_hFontEntry,	0xA0FFFF);

	return TRUE;

}

void CGuiCallLog::OnContextMenu(POINT pt)
{
	HMENU hMenuContext		= CreatePopupMenu();
	if(!hMenuContext)
		return;

	EnumShowCall eCallType = m_oCallList->GetShowCallType();

	AppendMenu(hMenuContext, MF_STRING|(eCallType== CALL_All?MF_CHECKED:NULL), 
		IDMENU_ChangeItem+ CALL_All,    _T("All"));
	AppendMenu(hMenuContext, MF_STRING|(eCallType== CALL_Incoming?MF_CHECKED:NULL), 
		IDMENU_ChangeItem+ CALL_Incoming,    _T("Incoming"));
	AppendMenu(hMenuContext, MF_STRING|(eCallType== CALL_Outgoing?MF_CHECKED:NULL), 
		IDMENU_ChangeItem+ CALL_Outgoing,    _T("Outgoing"));
	AppendMenu(hMenuContext, MF_STRING|(eCallType== CALL_Missed?MF_CHECKED:NULL), 
		IDMENU_ChangeItem+ CALL_Missed,    _T("Missed"));
	

	//Display it.
	TrackPopupMenu(	hMenuContext, 
		TPM_LEFTALIGN|TPM_TOPALIGN, 
		pt.x,pt.y, 
		0, 
		m_hWndParent, NULL);

	DestroyMenu(hMenuContext);
	return;
}

BOOL CGuiCallLog::OnLButtonDown(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	m_ptMouseDown = pt;

	if(PtInRect(&m_rcHeading, pt))
	{
		SHRGINFO    shrg;
		shrg.cbSize		= sizeof(shrg);
		shrg.hwndClient = m_hWndParent;
		shrg.ptDown.x	= pt.x;
		shrg.ptDown.y	= pt.y;
		shrg.dwFlags	= SHRG_RETURNCMD/*|SHRG_LONGDELAY*/;
		if (SHRecognizeGesture(&shrg) == GN_CONTEXTMENU)
		{
			// put the drop down menu
			OnContextMenu(pt);
		}
	}
	else
	{
		if(m_oCallList)
			m_oCallList->OnLButtonDown(pt);
	}

	return UNHANDLED;

}

BOOL CGuiCallLog::OnLButtonUp(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	if(PtInRect(&m_rcHeading, pt))
	{	//change the view 
		/*EnumShowCall eType = m_oCallList->GetShowCallType();

		eType = (Call_Type)((int)eType+1);

		if(eType > CALL_Missed)
			eType = CALL_All;
		m_oCallList->SetShowCallType(eType);

		InvalidateRect(m_hWndParent, NULL, FALSE);*/
		return TRUE;

	}

	if(m_oCallList)
		m_oCallList->OnLButtonUp(pt);

	if(m_oCallList->IsStopped())
	{
		if(m_ptMouseDown.x == pt.x && m_ptMouseDown.y == pt.y)
			LaunchItem(pt.y  - m_rcCallList.top + m_oCallList->GetScrollPos());
		return UNHANDLED;
	}


	return UNHANDLED;
}

BOOL CGuiCallLog::OnMouseMove(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	if(m_oCallList)
		m_oCallList->OnMouseMove(pt);

	return TRUE;
}

BOOL CGuiCallLog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case (IDMENU_ChangeItem +  CALL_All):
	case (IDMENU_ChangeItem +  CALL_Incoming):
	case (IDMENU_ChangeItem +  CALL_Missed):
	case (IDMENU_ChangeItem +  CALL_Outgoing):
		{
			if(m_oCallList)
			{
				//m_oCallList->SetShowCallType(( Call_Type)(wParam - IDMENU_ChangeItem));
				m_oCallList->ResetScrollPos();
			}
			InvalidateRect(m_hWndParent, NULL, FALSE);
		}
		break;
	}

	return TRUE;
}

BOOL CGuiCallLog::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if(m_oCallList)
		m_oCallList->OnTimer(wParam, lParam);

	return UNHANDLED;

}
BOOL CGuiCallLog::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_oCallList->ReBuildCallList();

	return FALSE;
}

BOOL CGuiCallLog::MoveGui(RECT rcLocation)
{
    if(EqualRect(&rcLocation, &m_rcLocation))
        return TRUE;

	CGuiBase::MoveGui(rcLocation);

	m_gdiHeader.Destroy();

	m_rcCallList = rcLocation;
	m_rcCallList.top += m_iTitleHeight;
	if(m_oCallList)
		m_oCallList->MoveGui(m_rcCallList);

	m_rcHeading = rcLocation;
	m_rcHeading.bottom = m_rcHeading.top + m_iTitleHeight;

	return TRUE;
}

void CGuiCallLog::LaunchItem(int iYPos)
{
	int iItemIndex = m_oCallList->GetItemIndexAtPos(iYPos);
	CALLLOGENTRY* pEntry = m_oCallList->GetCallEntry(iItemIndex);
	if(!pEntry)
		return;

	// find the poom contact
	TypeContact eContact;
	CIssString * oStr = CIssString::Instance();
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return;
	TCHAR szCompareName[STRING_MAX];
	IContact* pContact; 
	int iContactIndex = 0;
	for(int i = 0; i < oPoom->GetNumContacts(); i++)
	{
		pContact = oPoom->GetContactFromIndex(i);
		if(!pContact)
			continue;
		// format compare name
		oPoom->GetContactInfo(pContact, eContact, TRUE, 60, 60, RGB(50,50,50));
		oStr->Empty(szCompareName);
		oStr->Concatenate(szCompareName, eContact.szLastName);
		oStr->Concatenate(szCompareName, _T(", "));
		oStr->Concatenate(szCompareName, eContact.szFirstName);

		int iCompare = oStr->Compare(pEntry->pszName, szCompareName);
		if(iCompare == 0)
		{
			iContactIndex = i;
			break;
		}
	}

	CDlgContactDetails dlgContact;
	dlgContact.Init(iContactIndex/*, oPoom*/);
	dlgContact.DoModal(m_hWndParent, m_hInst, IDD_DLG_Basic);

	HDC hDC = GetDC(m_hWndParent);
	RECT rcClient;
	GetClientRect(m_hWndParent, &rcClient);
	CIssGDIEx gdiTemp;
	gdiTemp.Create(m_gdiMem->GetDC(), rcClient, TRUE, TRUE, FALSE);
	SweepLeft(hDC, gdiTemp);
	ReleaseDC(m_hWndParent, hDC);

	return;
}

#else   //NEW_CALLLOG_GUI

#define NUM_HISTORY_ICONS   7

CGuiCallLog::CGuiCallLog(void) 
{
	m_iLineHeight			= GetSystemMetrics(SM_CXSMICON);//GetSystemMetrics(SM_CXICON)/2;
	//m_iTitleHeight			= (int)(m_iLetterHeight * 1.5);

	m_hFontNormal			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
 	m_hFontBold		        = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);

	m_ptScreenSize.x		= 0;
	m_ptScreenSize.y		= 0;
}

CGuiCallLog::~CGuiCallLog(void)
{
	CIssGDIEx::DeleteFont(m_hFontNormal);
	CIssGDIEx::DeleteFont(m_hFontBold);
	EraseList();
}

BOOL CGuiCallLog::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions/*, CDlgContactDetails* dlgContactDetails*/)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBg, guiBackground, sOptions);

    //m_dlgContactDetails = dlgContactDetails;

    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    Initialize(hWndParent, hWndParent, m_hInst, 0);
    SetCustomDrawFunc(DrawListItem, this);
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
    if(m_gdiBg->GetDC() == NULL)
    {
        RECT rc;
        ::GetWindowRect(m_hWndParent, &rc);
		m_gdiBg->Create(hDC, rc);
    }

	DrawBackground(m_gdiBg->GetDC(), m_rcLocation, rcClip);
    CIssKineticList::OnDraw(*m_gdiBg, rcClient, rcClip);

    // draw the top part of the border only here
    DrawBorderOverlay(*m_gdiBg, rcClient, rcClip);

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
    
CIssGDIEx& CGuiCallLog::GetBgGDI()
{
	if(m_gdiBackground.GetDC() == NULL || m_gdiBackground.GetWidth() != WIDTH(m_rcLocation) ||
        m_gdiBackground.GetHeight() != HEIGHT(m_rcLocation))
	{
		m_gdiBackground.Create(m_gdiMem->GetDC(), m_rcLocation/*rcClient*/, FALSE);

        // background gradients
        RECT rcGDI = {0,0,m_gdiBackground.GetWidth(), m_gdiBackground.GetHeight()};
        RECT rc1, rc2;
        rc1 = rc2 = rcGDI;
        rc1.right = rc1.left + (long)(WIDTH(m_rcHistoryList/*rcClient*/) * 0.70f);
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

void CGuiCallLog::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{
    if(sItem->eType != ITEM_Unknown)
        return;

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

    // name
    RECT rcText = rcDraw;
    rcText.left += iIconWidth + 4;
    rcText.right -=2;
    rcText.bottom = rcText.top + m_iLineHeight;

    DrawText(gdi.GetDC(), pHistoryItem->szName, rcText, DT_LEFT | DT_VCENTER, m_hFontBold, bIsHighlighted ? m_crHighlight : m_crText);
    
    // date/time
    SYSTEMTIME sysTime;
    FILETIME ftLocal;
    FileTimeToLocalFileTime(&pHistoryItem->ftTime, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &sysTime);
    m_oStr->Format(szText, _T("%02d/%02d/%02d"), sysTime.wMonth, sysTime.wDay, sysTime.wYear);
    DrawText(gdi.GetDC(), szText, rcText, DT_RIGHT | DT_VCENTER, m_hFontNormal, bIsHighlighted ? m_crHighlight : m_crText);
    
    // line 2
    OffsetRect(&rcText, INDENT, m_iLineHeight);
    DrawText(gdi.GetDC(), pHistoryItem->szNumber, rcText, DT_LEFT | DT_VCENTER, m_hFontNormal, bIsHighlighted ? m_crHighlight : m_crText);

    // other stuff

    RECT rcLine = rcDraw;
    rcLine.top = rcLine.bottom - 3;// for now
    ::Line(gdi.GetDC(), rcLine.left + 4, rcLine.top, rcLine.right - 4, rcLine.top + 1, 0x454545);

    //if(bIsHighlighted)
    //{
    //}
}
    
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
    GetBgGDI();
    GetBorder();

    DBG_CONTACTS(_T("CGuiCallLog::MoveGui()"), FALSE);
	return TRUE;
}

void CGuiCallLog::DrawSelector(CIssGDIEx& gdi, RECT& rcDraw)
{
    // fine-tune the RECT
    RECT rcSelector = rcDraw;
    int iIndent = GetIconArray().GetWidth() / NUM_HISTORY_ICONS;
    rcSelector.left += (iIndent + 2);
    CIssKineticList::DrawSelector(gdi, rcSelector);
}


BOOL CGuiCallLog::OnLButtonDown(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	m_ptMouseDown = pt;

    return CIssKineticList::OnLButtonDown(pt);
}

BOOL CGuiCallLog::OnLButtonUp(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	return CIssKineticList::OnLButtonUp(pt);
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
    
BOOL CGuiCallLog::NeedMoreHistoryItems(FILETIME ftLast, int iCount)
{
    static const int iSanityAmount = 1024 * 12;
    if(m_sOptions->eShowHistory == HISTORY_Last100)
    {
        return iCount <= 100;
    }
    else if(m_sOptions->eShowHistory == HISTORY_Last200)
    {
        return iCount <= 200;
    }
    else if(m_sOptions->eShowHistory == HISTORY_LastMonth)
    {
        SYSTEMTIME st;
        GetSystemTime(&st);
        st.wMonth -= 1;
        if(st.wMonth < 1)
        {
            st.wMonth = 12;
            st.wYear --;
        }

        FILETIME ftCompare;
        SystemTimeToFileTime(&st, &ftCompare);
        return CompareFileTime(&ftLast, &ftCompare);
    }
    else    // HISTORY_All
        return (iCount < iSanityAmount);

    return TRUE;
}

BOOL CGuiCallLog::GetNextHistoryItem(FILETIME& ftEntry, TypeHistoryItem* pItem, int& iCurCall, int& iCurSms, int& iCurEmail)
{
    FILETIME ftCurrent = ftEntry;

    CALLLOGENTRY ce;
    // get the values out of this entry
	ce.cbSize = sizeof(CALLLOGENTRY);
	if(S_OK == PhoneGetCallLogEntry(m_hCallLog, &ce))
    {
        pItem->eType = HI_Call;
        pItem->iItemIndex = iCurCall++;
        pItem->bIsIncoming = !ce.fOutgoing;
        pItem->bMissed = (ce.iom == IOM_MISSED);
        if(ce.pszNumber)
            pItem->szNumber = m_oStr->CreateAndCopy(ce.pszNumber);
        if(ce.pszName)
            pItem->szName = m_oStr->CreateAndCopy(ce.pszName);
        else
            pItem->szName = m_oStr->CreateAndCopy(_T("Unknown"));

        pItem->ftTime = ce.ftStartTime;
        ftEntry = ce.ftStartTime;

        // clean up
        LocalFree(ce.pszName);
        LocalFree(ce.pszNameType);
        LocalFree(ce.pszNote);
        LocalFree(ce.pszNumber);
		return TRUE;
    }

    return FALSE;
}

void CGuiCallLog::PopulateList()
{
    PhoneOpenCallLog(&m_hCallLog);
    FILETIME ftEntry;

    int iEntryCount = 0;
    int iCallCount = 0;
    int iSMSCount = 0;
    int iEmailCount = 0;
    do
    {
        TypeHistoryItem* pItem = new TypeHistoryItem;
        if(!GetNextHistoryItem(ftEntry, pItem, iCallCount, iSMSCount, iEmailCount))
            break;
        AddItem(pItem, 0);
    }
    while(NeedMoreHistoryItems(ftEntry, iEntryCount));

    PhoneCloseCallLog(m_hCallLog);

    int iLineSpace = IsVGA() ? 5 : 10;
    SetItemHeights(m_iLineHeight*2 + iLineSpace, m_iLineHeight*3 + iLineSpace);
    ////////////////////////////////////////////////////
    // add all necessary items
    // calls
    // SMS
    // email

    ////////////////////////////////////////////////////
    // sort them
    ////////////////////////////////////////////////////
    // trim list if necessaary
    ////////////////////////////////////////////////////
    // select something

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
	switch(wParam)
	{
    }
	return TRUE;
}


#endif  //NEW_CALLLOG_GUI