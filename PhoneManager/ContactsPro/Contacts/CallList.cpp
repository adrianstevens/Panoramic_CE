#include "StdAfx.h"
#include "CallList.h"
#include "IssCommon.h"
#include "IssRect.h"
#include "IssString.h"
#include "IssGDIDraw.h"
#include "IssDebug.h"

#define LIST_COLOR_DARK		COLORREF(0x735D24)
#define LIST_COLOR_LIGHT	COLORREF(0x8E6331)

#define TEXT_COLOR_MAIN		0xFFFFFF
#define TEXT_COLOR_ALT		0xBBBBBB

CCallList::CCallList(void)
: m_Contact(0)
, m_scrollbar(0)
, m_guiOwner(NULL)
{
}

CCallList::CCallList(CGuiBase* guiOwner)
: m_Contact(0)
, m_scrollbar(0)
, m_guiOwner(guiOwner)
{
}


CCallList::~CCallList(void)
{
	CIssGDIEx::DeleteFont(m_hFontEntry);
	CIssGDIEx::DeleteFont(m_hFontEntryBold);
	DestroyCallList();
	m_oCallLog.Destroy();
	if(!m_scrollbar)
		delete m_scrollbar;

}
BOOL CCallList::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBackground, CGuiBackground* guiBackground, TypeOptions* sOptions)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBackground, guiBackground, sOptions);

    m_iLetterHeight		= GetSystemMetrics(SM_CXICON)/2;
    m_iEntryHeight		= GetSystemMetrics(SM_CXICON);
    m_hFontEntry		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
    m_hFontEntryBold	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);

    m_colorBG[0] = LIST_COLOR_LIGHT;
    m_colorBG[1] = LIST_COLOR_DARK;

    if(!m_scrollbar)
        m_scrollbar = new CScrollBar();
	m_oCallLog.Initialize();
	ReBuildCallList();

	return TRUE;

}

BOOL CCallList::MoveGui(RECT rcLocation)
{
	//m_gdiLogEntry.Destroy();
	m_gdiLogEntryBg[0].Destroy();
	m_gdiLogEntryBg[1].Destroy();

	m_gdiBackground.Destroy();
	m_gdiOffscreen.Destroy();

	CGuiBase::MoveGui(rcLocation);

	return TRUE;
}

BOOL CCallList::Draw(HDC hDC, RECT& rcClient)
{
	if(m_gdiOffscreen.GetDC() == NULL)
		m_gdiOffscreen.Create(hDC, m_rcLocation, FALSE, FALSE, FALSE);

	m_iBgColorIndex = 0;

	DrawBackground(m_gdiOffscreen.GetDC(), m_rcLocation);

	RECT rcLocation = m_rcLocation;
	if(ShouldDrawScrollbar())
		rcLocation.right -= GetSystemMetrics(SM_CXSMICON)/2;
	DrawEntries(m_gdiOffscreen.GetDC(), rcLocation);
	if(ShouldDrawScrollbar() && m_scrollbar)
		m_scrollbar->Draw(m_gdiOffscreen.GetDC(), m_rcLocation, CIssKineticList::GetScrollPos(), GetEndOfListYPos());

	BitBlt(hDC,
		m_rcLocation.left,m_rcLocation.top,
		WIDTH(m_rcLocation), HEIGHT(m_rcLocation),
		m_gdiOffscreen.GetDC(),
		0, 0,
		SRCCOPY);
	
	return TRUE;
}

BOOL CCallList::DrawEntries(HDC hdc, RECT rcClient)
{
	CALLLOGENTRY* pEntry;

	for(int i=0; i<m_callItems.GetSize(); i++)
	{
		++m_iBgColorIndex &= 1;	// toggle bg color
		if(m_iEntryHeight * i + m_iEntryHeight < GetScrollPos())
			continue;//we're off screen completely

		if(m_iEntryHeight * i > GetScrollPos() + GetSystemMetrics(SM_CYSCREEN))
			return TRUE;//all done

		pEntry = m_callItems[i];
		if(pEntry)
		{
			DrawLogEntry(hdc, rcClient, *pEntry, (m_iEntryHeight * i) - GetScrollPos());
		}
	}


	return FALSE;
}
	
void CCallList::ReBuildCallList()
{
	DBG_OUT((_T("CCallList::BuildCallList()")));

	// clear it 
	m_callItems.RemoveAll();

	int iNumEnteries = GetNumCallEntries();

	CALLLOGENTRY entry;
	CALLLOGENTRY* pEntry = 0;

	entry.cbSize = sizeof(CALLLOGENTRY);

	for(int i = 0; i < iNumEnteries; i++)
	{
		if(GetCallEntry(i, entry))
		{
			// filter by user
			if(!m_Contact)				// show all
			{
				CALLLOGENTRY* pEntry = new CALLLOGENTRY;
				*pEntry = entry;
				m_callItems.AddElement(pEntry);
			}
			else						// show for this contact
			{
				if(CompareContactName(entry))
				{
					CALLLOGENTRY* pEntry = new CALLLOGENTRY;
					*pEntry = entry;
					m_callItems.AddElement(pEntry);
				}
			}
		}
	}
	//DebugOut(_T("num call entrys = %i"), m_callItems.GetSize());
	ResetScrollPos();
}

void CCallList::DestroyCallList()
{
	for(int i=0; i<m_callItems.GetSize(); i++)
	{
		CALLLOGENTRY* pEntry = m_callItems[i];
		if(pEntry)
			delete pEntry;
	}
	m_callItems.RemoveAll();
}

CALLLOGENTRY* CCallList::GetCallEntry(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_callItems.GetSize())
		return 0;
	return m_callItems[iIndex];
}

BOOL CCallList::CompareContactName(CALLLOGENTRY &entry)
{
	if(!m_Contact)
		return FALSE;

	static TCHAR szCompareName[STRING_MAX];
	FormatContactName(szCompareName);

	CIssString * oStr = CIssString::Instance();
	return (oStr->Compare(szCompareName, entry.pszName) == 0);
}

BOOL CCallList::FormatContactName(TCHAR* pszName)
{
	CIssString * oStr = CIssString::Instance();
	oStr->Empty(pszName);
	BSTR szText = NULL;

	//Last Name
	m_Contact->get_LastName(&szText);
	oStr->Concatenate(pszName, szText);
	SysFreeString(szText);

	oStr->Concatenate(pszName, _T(", "));
	
	//First Name
	m_Contact->get_FirstName(&szText);
	oStr->Concatenate(pszName, szText);
	SysFreeString(szText);

	return TRUE;
}

int CCallList::GetNumCallEntries()
{
    if(!m_sOptions)
        return 0;

	switch(m_sOptions->eShowCall)
	{
	case CALL_All:		return m_oCallLog.GetAllCallsCount();
	case CALL_Incoming:	return m_oCallLog.GetIncomingCallsCount();
	case CALL_Outgoing:	return m_oCallLog.GetOutgoingCallsCount();
	case CALL_Missed:	return m_oCallLog.GetMissedCallsCount();
	}
	return 0;
}
	
BOOL CCallList::GetCallEntry(int iIndex, CALLLOGENTRY& entry)
{
    if(!m_sOptions)
        return FALSE;

	switch(m_sOptions->eShowCall)
	{
	case CALL_All:	
		{
			if(m_oCallLog.GetAllCallsEntry(iIndex, entry))
				return TRUE;
			break;
		}
	case CALL_Incoming:	
		{
			if(m_oCallLog.GetIncomingCallsEntry(iIndex, entry))
				return TRUE;
			break;
		}
	case CALL_Outgoing:
		{
			if(m_oCallLog.GetOutGoingCallsEntry(iIndex, entry))
				return TRUE;
			break;
		}
	case CALL_Missed:
		{
			if(m_oCallLog.GetMissedCallsEntry(iIndex, entry))
				return TRUE;
			break;
		}
    default:
        return TRUE;
	}

	return FALSE;
}

BOOL CCallList::DrawLogEntry(HDC hdc, RECT rcClient, CALLLOGENTRY& entry, int iYpos)
{
	RECT rcTemp = {rcClient.left, 0, rcClient.right, m_iEntryHeight};
/*	if(m_gdiLogEntryBg[m_iBgColorIndex].GetDC() == NULL)
	{
		//rcTemp.top		= 0;
		//rcTemp.bottom	= m_iEntryHeight;
		//rcTemp.right	= WIDTH(rcClient);
		m_gdiLogEntryBg[m_iBgColorIndex].Create(hdc, rcTemp);
		CIssGDI::GradriantFillRect(m_gdiLogEntryBg[m_iBgColorIndex].GetDC(), 
			rcTemp, m_colorBG[m_iBgColorIndex] + 0x303030, m_colorBG[m_iBgColorIndex]);
	}


	BitBlt(hdc,
		0,iYpos,
		WIDTH(rcTemp), HEIGHT(rcTemp),
		m_gdiLogEntryBg[m_iBgColorIndex].GetDC(),
		0,0,
		SRCCOPY);*/


	RECT rcEntry = {rcClient.left + 2, iYpos, rcClient.right - 2, iYpos + m_iEntryHeight};

	TCHAR szText[64];
	CIssString* oStrText = CIssString::Instance();

	// divide entry space into 4 rects
	int iIndent = GetSystemMetrics(SM_CXSMICON)/2;
	RECT rcRowOneLeft, rcRowOneRight, rcRowTwoLeft, rcRowTwoRight;
	rcRowOneLeft = rcRowOneRight = rcRowTwoLeft = rcRowTwoRight = rcEntry;
	rcRowOneLeft.right = rcRowTwoLeft.right = rcRowOneRight.left = rcRowTwoRight.left = rcEntry.left + WIDTH(rcEntry)/2 + iIndent;
	rcRowOneLeft.bottom = rcRowOneRight.bottom = rcRowTwoLeft.top = rcRowTwoRight.top = rcEntry.top + HEIGHT(rcEntry)/2;

	BOOL bHasName = entry.pszName != 0;

	// row 1 left - name
	if(bHasName)
		DrawText(hdc, entry.pszName, rcRowOneLeft, DT_LEFT | DT_END_ELLIPSIS, m_hFontEntryBold, TEXT_COLOR_MAIN);
	else
		DrawText(hdc, _T("Unknown"), rcRowOneLeft, DT_LEFT | DT_END_ELLIPSIS, m_hFontEntryBold, TEXT_COLOR_MAIN);

	// row 1 right - call type (incoming), roaming('R'), call time, duration
	TCHAR szTime[64];
	GetEntryTimeString(entry, szTime);
	oStrText->Format(szText, _T("%s"), szTime);
	DrawText(hdc, szText, rcRowOneRight, DT_RIGHT | DT_END_ELLIPSIS, m_hFontEntry, TEXT_COLOR_MAIN);

	// row 2 left - number, name type ('w')
	//if(bHasName)
	{
		//oStrText->Format(szText, _T("%s %s"), entry.pszNumber, entry.pszNameType);
		//CIssGDIEx::DrawText(hdc, szText, rcRowTwoLeft, DT_RIGHT | DT_END_ELLIPSIS, m_hFontEntry, TEXT_COLOR_ALT);

		DrawText(hdc, entry.pszNumber, rcRowTwoLeft, DT_LEFT | DT_END_ELLIPSIS, m_hFontEntry, TEXT_COLOR_ALT);
	}

	// row 2 right - call type (incoming), roaming('R'), duration
	if(entry.iom == IOM_MISSED)
		//oStrText->Format(szText, _T("%s%s"), _T(" missed "), entry.fRoam == 1 ? _T("(R) ") : _T(""));
		oStrText->Format(szText, _T("%s"), _T("Missed"));
	else if(entry.iom == IOM_INCOMING)
		oStrText->Format(szText, _T("%s%s"), _T("Incoming "), entry.fRoam == 1 ? _T("(R) ") : _T(""));
	else if(entry.iom == IOM_OUTGOING)
		oStrText->Format(szText, _T("%s%s"), _T("Outgoing "), entry.fRoam == 1 ? _T("(R) ") : _T(""));

	if(entry.iom != IOM_MISSED)
	{
		GetEntryDurationString(entry, szTime);
		oStrText->Concatenate(szText, szTime);
	}

	DrawText(hdc, szText, rcRowTwoRight, DT_RIGHT | DT_END_ELLIPSIS, m_hFontEntry, TEXT_COLOR_ALT);

	return TRUE;
}

TCHAR* CCallList::GetCurContactName()
{
	if(!m_Contact)
		return _T("All contacts");

	static TCHAR szName[STRING_MAX];

	FormatContactName(szName);
	return szName;

}
	
TCHAR* CCallList::GetCallTypeString()
{
	/*Call_Type callType = GetShowCallType();

	switch(callType)
	{
		case CALL_All:			return _T("All calls");
		case CALL_Incoming:		return _T("Incoming calls");
		case CALL_Outgoing:		return _T("Outgoing calls");
		case CALL_Missed:		return _T("Missed calls");
	}*/

	return _T("");
}

EnumShowCall CCallList::GetShowCallType()
{
    if(!m_sOptions)
        return CALL_All;
    return m_sOptions->eShowCall;
}
	
/*void CCallList::SetShowCallType(Call_Type type)		
{ 
	m_eShowCallType = type; 
	ReBuildCallList();
}*/

void CCallList::SetCurContact(IContact* pContact)	
{ 
	m_Contact = pContact; 
}

BOOL CCallList::GetEntryTimeString(CALLLOGENTRY& entry, TCHAR* szTime)
{
	// format time as: 7:32p Today or 4:59a 7/23/08
	CIssString* oStrText = CIssString::Instance();

	FILETIME ftNow, ftNowLocal, ftEntryLocal;
	GetCurrentFT(&ftNow);

	SYSTEMTIME stEntry, stNow;
	FileTimeToLocalFileTime(&entry.ftStartTime, &ftEntryLocal);
	FileTimeToLocalFileTime(&ftNow, &ftNowLocal);


	FileTimeToSystemTime(&ftEntryLocal, &stEntry);
	FileTimeToSystemTime(&ftNowLocal, &stNow);
 
	BOOL bIsToday = (	stEntry.wYear == stNow.wYear &&
						stEntry.wMonth == stNow.wMonth &&
						stEntry.wDay == stNow.wDay);

	int iHour = stEntry.wHour;
	if(iHour == 0)
		iHour = 12;
	else if(iHour > 12)
		iHour -= 12;

	if(bIsToday)
	{
		oStrText->Format(szTime, _T("%d:%d%s %s"), 
			iHour, 
			stEntry.wMinute, 
			stEntry.wHour > 11 ? _T("p") : _T("a"),
			_T("Today")
			);
	}
	else
	{
		oStrText->Format(szTime, _T("%d:%d%s %d/%d/%02d"), 
			iHour, 
			stEntry.wMinute, 
			stEntry.wHour > 11 ? _T("p") : _T("a"),
			stEntry.wMonth,
			stEntry.wDay,
			stEntry.wYear % 1000
			);
	}

	return TRUE;
}

BOOL CCallList::GetEntryDurationString(CALLLOGENTRY& entry, TCHAR* szDuration)
{
	// simply get the time diff - shouldn't need to convert local
	// format as "2m" if more than 1 minute or "35s" if less
	CIssString* oStrText = CIssString::Instance();

	// caclculate total seconds
	UINT uMicroSeconds = (entry.ftEndTime.dwLowDateTime - entry.ftStartTime.dwLowDateTime) / 10;
	UINT uSeconds = uMicroSeconds / 1000 / 1000;
	
	if(uSeconds < 60)
		oStrText->Format(szDuration, _T("%ds"), uSeconds);
	else
		oStrText->Format(szDuration, _T("%d%m"), uSeconds / 60);

	return 0;
}

void CCallList::ResetScrollPos()
{
	CIssKineticList::ResetScrollPos();
}

BOOL CCallList::ShouldDrawScrollbar()
{
	return (GetEndOfListYPos() > HEIGHT(m_rcLocation));
}

void CCallList::ReDrawList(void)
{
	/*HDC hdc= GetDC(m_hWndParent);
	RECT rc;
	GetClientRect(m_hWndParent, &rc);
    if(m_guiOwner)
        m_guiOwner->Draw(hdc, rc);
    else
	    Draw(hdc, rc);
	ReleaseDC(m_hWndParent, hdc);*/
}

int	CCallList::GetEndOfListYPos(void)
{
	return m_callItems.GetSize() * m_iEntryHeight;
}

BOOL CCallList::DrawBackground(HDC hdc, RECT rcClient)
{
	if(m_gdiBackground.GetDC() == NULL)
	{
		CIssRect rc(rcClient);
		rc.ZeroBase();
		m_gdiBackground.Create(hdc, rc.Get(), FALSE);
		DrawGradientGDI(m_gdiBackground.GetDC(), rc.Get(), 0, 0xFF7836);
	}

	return BitBlt(hdc,
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiBackground.GetDC(),
		0,0,
		SRCCOPY);
}

void CCallList::DrawGradientGDI(HDC tdc, RECT rc, COLORREF StartRGB, COLORREF EndRGB)
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

BOOL CCallList::OnLButtonDown(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	if(m_scrollbar && ShouldDrawScrollbar() && pt.x >= m_rcLocation.right - GetSystemMetrics(SM_CXSMICON)/2)
		return m_scrollbar->OnLButtonDown(pt);

	return CIssKineticList::OnLButtonDown(pt);

	return -1;
}

BOOL CCallList::OnLButtonUp(POINT pt)
{
	if(m_scrollbar)
		m_scrollbar->OnLButtonUp(pt);

	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;


	if(CIssKineticList::OnLButtonUp(pt) == -1) 
		return -1;

	return -1;
}

BOOL CCallList::OnMouseMove(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	if(m_scrollbar && m_scrollbar->IsScrollActive() && ShouldDrawScrollbar())
	{
		float fScrollMult = m_scrollbar->OnMouseMove(pt, m_rcLocation, GetEndOfListYPos());
		int iScrollAmount = (int)(fScrollMult * (GetEndOfListYPos() - HEIGHT(GetOwnerFrame())/2));
		CIssKineticList::ReDrawList(iScrollAmount);
		return TRUE;
	}

	return CIssKineticList::OnMouseMove(pt);
}

BOOL CCallList::OnTimer(WPARAM wParam, LPARAM lParam)
{
	CIssKineticList::OnTimer(wParam, lParam);

	return -1;
}

BOOL CCallList::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	ReBuildCallList();

	return UNHANDLED;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////// scrollbar class /////////////////////////////////////////
CScrollBar::CScrollBar()
: m_iLastMousePos(0)
, m_bScrollActive(FALSE)
{
}

BOOL CScrollBar::Draw(HDC hdc, RECT& rcClient, int iYPos, int iListHeight)
{
	RECT rcTemp(rcClient);
	rcTemp.left = rcTemp.right - GetSystemMetrics(SM_CXSMICON)/2;
	rcTemp.bottom -= rcTemp.top;
	rcTemp.top = 0;

	// draw back
	Rectangle(hdc, rcTemp, 0x404040, 0x808080);

	// draw bar
	int iFullTravel = iListHeight - HEIGHT(rcClient)/2;	
	int iFullListHeight = iListHeight + HEIGHT(rcClient)/2;	
	float fBarHeight = (float)HEIGHT(rcClient) / (float)iFullListHeight * (float)HEIGHT(rcClient);
	float fBarTravel = HEIGHT(rcClient) - fBarHeight;

	rcTemp.top = (int)((float)iYPos/(float)iFullTravel * fBarTravel);
	rcTemp.bottom = rcTemp.top + (int)fBarHeight;
	m_rcScrollbar = rcTemp;		// store it

	RoundRectangle(hdc, rcTemp, 0x909090, 0xffffff, 4, 4);

	return FALSE;
}

BOOL CScrollBar::OnLButtonDown(POINT pt)
{
	m_iLastMousePos = pt.y;
	//if(PtInRect(&m_rcScrollbar, pt))
		m_bScrollActive = TRUE;

	return TRUE;
}
BOOL CScrollBar::OnLButtonUp(POINT pt)
{
	m_bScrollActive = FALSE;
	return FALSE;
}

float CScrollBar::OnMouseMove(POINT pt, RECT& rcClient, int iListHeight)
{
	// return amount of list scroll movement as a multiplier
	float fBarMovement = (float)(m_iLastMousePos - pt.y);
	m_iLastMousePos = pt.y;

	int iFullListHeight = iListHeight + HEIGHT(rcClient)/2;	
	float iBarHeight = (float)HEIGHT(rcClient) / (float)iFullListHeight * (float)HEIGHT(rcClient);
	float fBarTravel = HEIGHT(rcClient) - iBarHeight;

	return fBarMovement / fBarTravel;
}
