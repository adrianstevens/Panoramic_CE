#include "IssPicControl.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"
#include "IssDebug.h"

#define INDENT					(GetSystemMetrics(SM_CXSMICON)/4)
#define TEXT_HEIGHT_Large       (GetSystemMetrics(SM_CXICON)*14/32)
#define IDT_SROLL_TIMER		    4100
#define IDT_SELECT_TIMER        4101
#define MAX_SPEED			    1000.0
#define MIN_ACTIVATE		    0.1
#define MIN_SPEED			    0.01
#define UPDATE_TIME			    0
#define	FRICTION			    30.0
#define SPEED_MULT			    50.0
#define REFRESH_RATE		    22
#define INC_ACCEL_FACTOR	    1.2f
#define UND					    -1	// Not defined
#define MAX_SelectDistance      (IsVGA()?16:8)
#define MAX_SelectItemTime      700

#define MIN_SNAP_SPEED          20.0

TypePicItems::TypePicItems()
:szTitle(NULL)
,sPic(NULL)
,uiMenuID(0)
,lParam(0)
,pfnLoadPic(NULL)
,lpItem(NULL)
{}

TypePicItems::~TypePicItems()
{
	Destroy();
}

void TypePicItems::Destroy()
{
	CIssString* oStr = CIssString::Instance();
	oStr->Delete(&szTitle);
	if(sPic)
		sPic->bIsDeleted = TRUE;
	sPic = NULL;
	uiMenuID = 0;
	lParam	 = 0;
	pfnLoadPic= NULL;

    if(lpItem)
    {
        DBG_OUT((_T("TypeItems::~TypeItems() - lpItem is defined.  This is a memory leak")));
        delete lpItem;       // must be self deleting
    }
    lpItem = NULL;
}

CIssPicControl::CIssPicControl(void)
:m_oStr(CIssString::Instance())
,m_uiDefaultImage(0)
,m_crText(RGB(255,255,255))
,m_hWndNotif(NULL)
,m_hWndMyParent(NULL)
,m_hInstance(NULL)
,m_dwFlags(0)
,m_iScrollPos(0)
,m_dbSpeed(0.00)
,m_dbFriction(0.00)
,m_eScroll(PICSCROLL_Stopped)
,m_eScrollDir(DIR_Left)
,m_iCurrentWidth(-1)
,m_iCurSelector(UND)
,m_dwTickLastKeyDown(0)
,m_bWasScrolling(FALSE)
,m_hFontTitle(NULL)
,m_pDrawFunc(NULL)
,m_pDrawTitle(NULL)
,m_pDrawDots(NULL)
,m_pDeleteItemFunc(NULL)
,m_lpClass(NULL)
,m_iIndent(2*INDENT)
{
	m_sizeItem.cx	= 0;
	m_sizeItem.cy	= 0;
}

CIssPicControl::~CIssPicControl(void)
{
	Destroy();
}

void CIssPicControl::Destroy()
{
	ResetContent();
	m_gdiDefaultImage.Destroy();
    CIssGDIEx::DeleteFont(m_hFontTitle);
    m_pDrawFunc = NULL;
    m_pDeleteItemFunc = NULL;
    m_lpClass = NULL;
    m_pDrawTitle = NULL;
    m_pDrawDots = NULL;
}

void CIssPicControl::ResetContent()
{
	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		TypePicItems* sItem = m_arrItems[i];
		if(!sItem)
			continue;

        if(sItem->lpItem && m_pDeleteItemFunc)
        {
            m_pDeleteItemFunc(sItem->lpItem);
            sItem->lpItem = NULL;
        }

		delete sItem;
	}
	m_arrItems.RemoveAll();
}

HRESULT CIssPicControl::Initialize(HWND hWndNotif,
								   HWND hWndParent, 
								   HINSTANCE hInst,
								   DWORD dwFlags)
{
	m_hWndNotif		= hWndNotif;
	m_hWndMyParent	= hWndParent;
	m_hInstance		= hInst;
	m_dwFlags		= dwFlags;    
	return S_OK;
}

HRESULT CIssPicControl::StartThreadEngine()
{
    if(!m_oPics.IsEngineStarted())
        return m_oPics.StartEngine(m_hWndMyParent, m_hInstance);
    else 
        return S_OK;
}

HRESULT CIssPicControl::SetDefaultImage(UINT uiDefaultImage)
{
	m_uiDefaultImage = uiDefaultImage;
	m_gdiDefaultImage.Destroy();	
	return S_OK;
}

HRESULT CIssPicControl::SetCustomDrawFunc(LPVOID lpClass, DRAWPICITEM pDrawFunc /*= NULL*/, DRAWTITLE pDrawTitle /*= NULL*/, DRAWDOTS pDrawDots /*= NULL*/)
{
    m_pDrawFunc     = pDrawFunc;
    m_lpClass       = lpClass;
    m_pDrawTitle    = pDrawTitle;
    m_pDrawDots     = pDrawDots;
    return S_OK;
}

HRESULT CIssPicControl::SetDeleteItemFunc(DELETEPICITEM pDeleteItemFunc)
{
    m_pDeleteItemFunc = pDeleteItemFunc;
    return S_OK;
}

HRESULT	CIssPicControl::SetItemSize(int iWidth, int iHeight)
{
	m_sizeItem.cx	= iWidth;
	m_sizeItem.cy	= iHeight;
    m_iCurrentWidth = -1;
	m_gdiDefaultImage.Destroy();
	return S_OK;
}

HRESULT CIssPicControl::SetIndentSize(int iWidth)
{
    m_iIndent       = iWidth;
    m_iCurrentWidth = -1;
    return S_OK;
}

HRESULT CIssPicControl::SetColors(COLORREF crText)
{
	m_crText = crText;
	return S_OK;
}

HRESULT CIssPicControl::AddItem(TCHAR* szTitle, 
                                UINT uiMenu, 
                                LPARAM lParam /*=0*/,
                                TCHAR*  szURL /*= NULL*/,
                                TCHAR*  szFilename /*= NULL*/,
                                UINT    uiItem /*= 0*/,
                                PFN_LOADPIC pfnLoadPic /*= NULL*/)
{
	HRESULT hr = S_OK;
	TypePicItems* sNew = NULL;

	m_iCurrentWidth = -1;

	CBARG(!m_oStr->IsEmpty(szTitle), _T("szTitle is empty"));
    CBARG(m_oStr->GetLength(szFilename) > 0 || uiItem != 0 || pfnLoadPic != NULL, _T(""));

	sNew = new TypePicItems;
	CPHR(sNew, _T("failed to create sNew"));
	sNew->szTitle   = m_oStr->CreateAndCopy(szTitle);
	CPHR(sNew->szTitle, _T("sNew->szTitle   = m_oStr->CreateAndCopy(szTitle);"));
	sNew->uiMenuID = uiMenu;
	sNew->lParam   = lParam;

    hr = StartThreadEngine();
    CHR(hr, _T("hr = StartThreadEngine();"));

    if(pfnLoadPic)  
        sNew->sPic = m_oPics.AddPicToQueue(pfnLoadPic, m_lpClass, lParam, m_hWndNotif, WM_PicLoaded, m_sizeItem.cx, m_sizeItem.cy);
    else if(uiItem != 0)
        sNew->sPic = m_oPics.AddPicToQueue(uiItem, m_hWndNotif, WM_PicLoaded, m_sizeItem.cx, m_sizeItem.cy);
    else if(m_oStr->GetLength(szURL))
        sNew->sPic = m_oPics.AddPicToQueue(szURL, szFilename, m_hWndNotif, WM_PicLoaded, m_sizeItem.cx, m_sizeItem.cy);
    else
        sNew->sPic = m_oPics.AddPicToQueue(szFilename, m_hWndNotif, WM_PicLoaded, m_sizeItem.cx, m_sizeItem.cy);

    CPHR(sNew->sPic, _T("m_oPics.AddPicToQueue failed "));

	hr = m_arrItems.AddElement(sNew);
	CHR(hr, _T("m_arrItems.AddElement failed"));

Error:
	if(sNew && hr != S_OK)
		delete sNew;

	return hr;
}

HRESULT CIssPicControl::AddItem(TCHAR* szTitle,
                                UINT uiMenu,
                                LPARAM lParam /*= 0*/,
                                LPVOID lpItem /*= NULL*/)
{
    HRESULT hr = S_OK;
    TypePicItems* sNew = NULL;

    m_iCurrentWidth = -1;

    CBARG(!m_oStr->IsEmpty(szTitle) && lpItem, _T(""));
    CBARG(m_pDeleteItemFunc, _T("CIssPicControl::AddItem failed! m_pDeleteItemFunc not defined"));

    sNew = new TypePicItems;
    CPHR(sNew, _T("failed to create sNew"));
    sNew->szTitle   = m_oStr->CreateAndCopy(szTitle);
    CPHR(sNew->szTitle, _T("sNew->szTitle   = m_oStr->CreateAndCopy(szTitle);"));
    sNew->uiMenuID = uiMenu;
    sNew->lParam   = lParam;
    sNew->lpItem   = lpItem;

    hr = m_arrItems.AddElement(sNew);
    CHR(hr, _T("m_arrItems.AddElement failed"));

Error:
    if(sNew && hr != S_OK)
        delete sNew;

    return hr;
}

HRESULT CIssPicControl::RemoveItem(int iIndex)
{
	HRESULT hr = S_OK;

	hr = DeleteItem(iIndex);
    CHR(hr, _T("hr = DeleteItem(iIndex);"));

	hr = m_arrItems.RemoveElementAt(iIndex);
	CHR(hr, _T("hr = m_arrItems.RemoveElementAt(iIndex);"));

Error:
	return hr;
}

HRESULT CIssPicControl::DeleteItem(int iIndex)
{
    HRESULT hr = S_OK;
    TypePicItems* sItem = m_arrItems[iIndex];
    CPHR(sItem, _T("iIndex not found in m_arrItems"));

    if(sItem->lpItem && m_pDeleteItemFunc)
    {
        m_pDeleteItemFunc(sItem->lpItem);
        sItem->lpItem = NULL;
    }

    delete sItem;
Error:
    return hr;
}

BOOL CIssPicControl::OnLButtonDown(POINT& pt)
{
    if(!PtInRect(&m_rcLoc, pt))
    {
        StopScrolling();
        m_eMouse  = PICGRAB_None;
        return FALSE;
    }

    m_eMouse = PICGRAB_None;

    m_ptXStartGrab       = pt;
    m_dwTickCountGrab   = GetTickCount();

    if(PtInRect(&m_rcList, pt))
    {
        if(m_eScroll == PICSCROLL_Moving && (int)abs((int)m_dbSpeed) > 3)
        {
            // stop scrolling
            StopScrolling();

            int iSelected = GetItemIndexAtPos(pt.x - m_rcList.left);
            if(iSelected != -1 && IsItemSelectable(iSelected))
            {
                m_iCurSelector = iSelected;

				if(m_hWndNotif)
					PostMessage(m_hWndNotif, WM_PicItemChanged, m_iCurSelector,0);
                ReDrawList();
            }
            m_bWasScrolling = TRUE;
        }
        else
            m_bWasScrolling = FALSE;


        if(PtInRect(&m_rcList, pt) == FALSE)
            return FALSE;

        m_eMouse = PICGRAB_List;        

        m_iScrollXStart = pt.x;
        m_iScrollXPos	= pt.x;
        m_dbSpeed		= 0.0;
        m_dwStartTime	= GetTickCount();
        KillTimer(m_hWndMyParent, IDT_SROLL_TIMER);
        KillTimer(m_hWndMyParent, IDT_SELECT_TIMER);
        SetTimer(m_hWndMyParent, IDT_SELECT_TIMER, 600, NULL);

    }

    return TRUE;
}

BOOL CIssPicControl::OnLButtonUp(POINT& pt)
{
    BOOL bRet = FALSE;

    if(m_eMouse == PICGRAB_None)
        return FALSE;

    KillTimer(m_hWndMyParent, IDT_SELECT_TIMER);

    if(m_eMouse == PICGRAB_List)
    {
        bRet = OnLButtonUpList(pt);

        // see if we should be selecting this item
        if((GetTickCount()-m_dwTickCountGrab) < MAX_SelectItemTime &&
            (abs(m_ptXStartGrab.y-pt.y) < MAX_SelectDistance) && 
            (abs(m_ptXStartGrab.x-pt.x) < MAX_SelectDistance) && 
            //    IsItemSelectable(m_iCurSelector)  &&  //selector may not be at the current location AS
            !m_bWasScrolling)
        {

            int iSelected = GetItemIndexAtPos(pt.x - m_rcList.left);
            if(iSelected != -1 && IsItemSelectable(iSelected))
            {
                m_iCurSelector = iSelected;

				if(m_hWndNotif)
					PostMessage(m_hWndNotif, WM_PicItemChanged, m_iCurSelector,0);

                TypePicItems* sItem = GetSelectedItem();
                if(sItem)
                    GrabItem(m_ptXStartGrab, sItem);
                ReDrawList();
                SelecteItem(m_iCurSelector);
            }

        }
        bRet = TRUE; // we always return TRUE
    }
    
    m_bWasScrolling = FALSE;

    return bRet;
}

BOOL CIssPicControl::OnLButtonUpList(POINT& pt)
{
    if(PtInRect(&m_rcList, pt) == FALSE)
    {
        EnsureWithinBounds();
        return FALSE;
    }

    if(m_eScroll == PICSCROLL_Scrolling)
    {
        m_dwStopTime = GetTickCount();
        int iPos = pt.x;

        int iTicks = m_dwStopTime - m_dwStartTime;

        iTicks > 0 ?  iTicks = iTicks : iTicks = 1;
        m_dbSpeed = (pt.x - m_iScrollXStart) * SPEED_MULT;
        m_dbSpeed /= (float)iTicks;

        if(fabs(m_dbSpeed) > MIN_ACTIVATE)
        {
            if(m_dbSpeed > MAX_SPEED)
                m_dbSpeed = MAX_SPEED;
            else if(m_dbSpeed < MAX_SPEED * -1.0)
                m_dbSpeed = MAX_SPEED*-1.0;

            m_eScroll = PICSCROLL_Moving;
            m_dbFriction = FRICTION;
			m_iScrollToStart = m_iCurSelector;
            SetTimer(m_hWndMyParent, IDT_SROLL_TIMER, REFRESH_RATE, NULL);
        }
        else
        {
            m_eScroll = PICSCROLL_Stopped;
            m_dbSpeed = 0.0;
            //ReDrawList(0);
            EnsureSelectedItemCentered();

        }
        return TRUE;
    }
    else if(m_eScroll == PICSCROLL_Moving)
    {
        m_eScroll = PICSCROLL_Stopped;
        m_dbSpeed = 0.0;

        EnsureSelectedItemCentered();

        return TRUE;
    }
    else if(m_eScroll == PICSCROLL_Stopped)
    {
        return TRUE;
    }
    else if(m_eScroll == PICSCROLL_To)	
    {
        m_eScroll = PICSCROLL_Stopped;
        EnsureWithinBounds();
    }

    return FALSE;	
}

BOOL CIssPicControl::OnMouseMove(POINT& pt)
{
    if(m_eMouse == PICGRAB_None)
        return FALSE;
    else if(m_eMouse == PICGRAB_List)
    {   //
        if(pt.x != m_ptXStartGrab.x || pt.x != m_ptXStartGrab.x) 
            return OnMouseMoveList(pt);
    }
    
    return TRUE;
}

BOOL CIssPicControl::OnMouseMoveList(POINT& pt)
{
    if(m_eScroll != PICSCROLL_Scrolling)
    {
        int iXDiff = IsVGA()?16:8;
        if( abs(pt.x-m_iScrollXStart) > iXDiff)
        {
            m_eScroll = PICSCROLL_Scrolling; //since we're actually dragging
            KillTimer(m_hWndMyParent, IDT_SELECT_TIMER);
        }
    }	

    ReDrawList(pt.x - m_iScrollXPos);

    if(pt.x < m_iScrollXPos)
    {
        if(m_eScrollDir != DIR_Right)
        {
            m_dwStartTime = GetTickCount();
            m_iScrollXStart = pt.x;
        }
         m_eScrollDir = DIR_Right;
    }
    else if(pt.x > m_iScrollXPos)
    {
        if(m_eScrollDir != DIR_Left)
        {
            m_dwStartTime = GetTickCount();
            m_iScrollXStart = pt.x;
        }
        m_eScrollDir = DIR_Left;
    }

    m_iScrollXPos = pt.x;

    return TRUE;
}

BOOL CIssPicControl::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	int iNewSelected = UND;
	//int iRepeatCount = LOWORD(lParam);

	if(m_iCurSelector == UND)
	{
		iNewSelected = 0;
	}
	else
	{
		// we have a selection
		switch(wParam)
		{
		case VK_RETURN:
			SelecteItem(m_iCurSelector);
			return TRUE;
			break;
		case VK_LEFT:
			iNewSelected = GetNextItemIndex(m_iCurSelector, TRUE);
			break;
		case VK_RIGHT:
			iNewSelected = GetNextItemIndex(m_iCurSelector);
			break;
		}
	}

	if(iNewSelected != m_iCurSelector && iNewSelected != UND)
	{
		m_iCurSelector = iNewSelected;

		if(m_hWndNotif)
			PostMessage(m_hWndNotif, WM_PicItemChanged, m_iCurSelector,0);

		// make sure to show the selected item, only animate if we're not holding down the key
		int iRepeatCount = (lParam&65535);
		//DBG_OUT((_T("RepeatCount - %d"),iRepeatCount ));

		if(GetTickCount()-m_dwTickLastKeyDown < 300 || iRepeatCount > 1)
		{
			ShowSelectedItem(FALSE);

		}
		else
			ShowSelectedItem(TRUE);

		ReDrawList();
		m_dwTickLastKeyDown = GetTickCount();


	}
	else
	{
		// DH: if no change, return false
		//return FALSE;
	}

	return TRUE;
}

BOOL CIssPicControl::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == IDT_SROLL_TIMER)
	{
		if(m_eScroll == PICSCROLL_To)
		{
			m_iScrollPos = GetNextScrollToPos();
			if(ScrollToDestReached())
			{
				// we're done
				KillTimer(m_hWndMyParent, IDT_SROLL_TIMER);
				m_eScroll = PICSCROLL_Stopped;
				m_iScrollPos = m_iScrollToDest;
			}
			ReDrawList();

			return TRUE;
		}

		if(m_dwFlags & OPTION_SlidePerPage)
		{
			int iSelected = GetItemIndexAtPos(m_rcList.right/2);
			if(iSelected != -1 && IsItemSelectable(iSelected) && iSelected != m_iScrollToStart && m_eScroll != PICSCROLL_To)
			{
				m_iCurSelector = iSelected;

				// tell the parent that the selected item has changed
				if(m_hWndNotif)
					PostMessage(m_hWndNotif, WM_PicItemChanged, m_iCurSelector, 0);

				TypePicItems* sItem = m_arrItems[m_iCurSelector];
				if(!sItem)
					return TRUE;

				int iSizeItem = m_sizeItem.cx + m_iIndent;
				int iXStart = GetXStartPosition(sItem) - (WIDTH(m_rcList) - iSizeItem)/2; 

				ScrollTo(iXStart, FALSE);
				InvalidateRect(m_hWndMyParent, &m_rcLoc, FALSE);
				return TRUE;
			}
		}

        if(m_dwFlags & OPTION_SnapCenter && fabs(m_dbSpeed) <= MIN_SNAP_SPEED)
        {
            KillTimer(m_hWndMyParent, IDT_SROLL_TIMER);
            int iRight = max(0, GetListWidth() - WIDTH(m_rcList));
            int iLeft  = 0;
            int iSizeItem = m_sizeItem.cx + m_iIndent;

            iLeft = iLeft - (WIDTH(m_rcList) - iSizeItem)/2;
            iRight= iRight + (WIDTH(m_rcList) - iSizeItem)/2;

            int iXStart = 0;
            int iSelected = GetItemIndexAtPos(m_rcList.right/2);
            if(iSelected != -1 && IsItemSelectable(iSelected))
            {
                m_iCurSelector = iSelected;

				// tell the parent that the selected item has changed
				if(m_hWndNotif)
					PostMessage(m_hWndNotif, WM_PicItemChanged, m_iCurSelector, 0);

                TypePicItems* sItem = m_arrItems[m_iCurSelector];
                if(!sItem)
                    return TRUE;
                
                iXStart = GetXStartPosition(sItem) - (WIDTH(m_rcList) - iSizeItem)/2;                
            }
            else if(m_iScrollPos < iLeft)
                iXStart = iLeft;
            else
                iXStart = iRight;

            ScrollTo(iXStart, FALSE);
            InvalidateRect(m_hWndMyParent, &m_rcLoc, FALSE);
        }
		else if (fabs(m_dbSpeed) <= 0.01)
		{
			KillTimer(m_hWndMyParent, IDT_SROLL_TIMER);
			m_eScroll = PICSCROLL_Stopped;//since we're no longer moving
			m_dbSpeed = 0;
			m_dbFriction = FRICTION;
			//EnsureWithinBounds();            
		} 
        else 
		{
			m_eScroll = PICSCROLL_Moving;
			m_dbSpeed -= m_dbSpeed / m_dbFriction;

			//DBG_OUT((_T("m_dbSpeed: %f"), m_dbSpeed));
			//DBG_OUT((_T("m_dbFriction: %f"), m_dbFriction));
			if(m_iLastReDrawInc != (int)m_dbSpeed)
			{
				// prevent unnecessary draws
				m_iLastReDrawInc = (int)m_dbSpeed;
				ReDrawList(m_iLastReDrawInc);
			}
		}
		return TRUE;
	}
	else if(LOWORD(wParam) == IDT_SELECT_TIMER)
	{
		DBG_OUT((_T("CIssKineticList::OnTimer - IDT_SELECT_TIMER")));
		KillTimer(m_hWndMyParent, IDT_SELECT_TIMER);

		int iSelected = GetItemIndexAtPos(m_ptXStartGrab.x - m_rcList.left);
		if(iSelected != -1 && IsItemSelectable(iSelected))
		{
			m_eMouse = PICGRAB_None;
			m_iCurSelector = iSelected;

			if(m_hWndNotif)
				PostMessage(m_hWndNotif, WM_PicItemChanged, m_iCurSelector,0);

			TypePicItems* sItem = GetSelectedItem();
			if(sItem)
				GrabItem(m_ptXStartGrab, sItem);
			ReDrawList();
            EnsureSelectedItemCentered();
		}
	}

	return UNHANDLED;
}

int CIssPicControl::GetTitleHeight()
{
    return (INDENT + TEXT_HEIGHT_Large);
}

BOOL CIssPicControl::OnSize(int iX, int iY, int iWidth, int iHeight)
{
	m_rcLoc.left   = iX;
	m_rcLoc.top    = iY;
	m_rcLoc.right  = iX + iWidth;
	m_rcLoc.bottom = iY + iHeight;

    CIssGDIEx::DeleteFont(m_hFontTitle);

	m_rcTitle		= m_rcLoc;
    m_rcList		= m_rcLoc;
    m_rcDots		= m_rcLoc;


    if(m_dwFlags & OPTION_DrawTitle)
    {
        m_hFontTitle     = CIssGDIEx::CreateFont(TEXT_HEIGHT_Large, FW_BOLD, TRUE);
	    m_rcTitle.bottom = m_rcTitle.top + GetTitleHeight();
    }
    else
    {
        m_rcTitle.bottom = m_rcTitle.top;
    }

	if(m_dwFlags & OPTION_DrawDots)
    {
	    m_rcDots.top = m_rcDots.bottom - GetTitleHeight();
        //m_rcDots.top = m_rcDots.bottom; 
    }
    else
    {
        m_rcDots.top = m_rcDots.bottom;
    }

	
	m_rcList.top	= m_rcTitle.bottom;
	m_rcList.bottom	= m_rcDots.top;

	m_iCurrentWidth = -1;

	return TRUE;
}

BOOL CIssPicControl::OnDraw(CIssGDIEx& gdiMem, RECT& rcClient, RECT& rcClip)
{
    m_rcClip = rcClip;

	if(!IsRectInRect(rcClip, m_rcLoc))
		return TRUE;

    if(m_dwFlags & OPTION_DrawTitle && IsRectInRect(rcClip, m_rcTitle))
        DrawTitle(gdiMem);

    if(m_dwFlags & OPTION_DrawDots && IsRectInRect(rcClip, m_rcDots))
        DrawDots(gdiMem);

	DrawEntries(gdiMem, rcClip);
	return TRUE;
}

void CIssPicControl::DrawEntries(CIssGDIEx& gdi, RECT& rcClip)
{
	TypePicItems* sItem = NULL;
	int iWidth = 0;
	RECT rcDraw;
    int iSizeItem = m_sizeItem.cx + m_iIndent;

	rcDraw.top     = m_rcList.top + (HEIGHT(m_rcList) - m_sizeItem.cy)/2;
	rcDraw.bottom  = rcDraw.top + m_sizeItem.cy; 

	/*if(m_gdiList.GetDC() == NULL || m_gdiList.GetWidth() != WIDTH(m_rcList) || m_gdiList.GetHeight() != HEIGHT(m_rcList))
		m_gdiList.Create(gdi.GetDC(), WIDTH(m_rcList), HEIGHT(m_rcList), FALSE, TRUE);

	BitBlt(m_gdiList,
		0,0,
		WIDTH(m_rcList), HEIGHT(m_rcList),
		gdi,
		m_rcList.left, m_rcList.top,
		SRCCOPY);*/

	//now here's where we draw relative to the scroll position ... 
	for(int i = 0; i < m_arrItems.GetSize(); i++)
	{
		sItem = m_arrItems[i];
		if(!sItem)
			return;

		// get the with in pixels of this item
		iWidth = i * iSizeItem;

		rcDraw.left		= iWidth - m_iScrollPos;
		rcDraw.right	= rcDraw.left + iSizeItem;

		if(iWidth + iSizeItem < m_iScrollPos)
			continue;//we're off screen completely

		if(iWidth > m_iScrollPos + WIDTH(m_rcList))
			break;//all done

        rcDraw.left += m_rcList.left;
        rcDraw.right+= m_rcList.left;

        if(i == m_iCurSelector && !sItem->lpItem && !(m_dwFlags&OPTION_SnapCenter))
        {
            // draw the selector
            DrawSelector(gdi, rcDraw);
        }

        if(sItem->lpItem || (sItem->sPic && sItem->sPic->gdiImage))
			DrawItem(gdi, rcDraw, sItem, i==m_iCurSelector);
		else
			DrawDefaultItem(gdi, rcDraw);

	}

		

}

void CIssPicControl::DrawTitle(CIssGDIEx& gdi)
{
    TypePicItems* sItem = m_arrItems[m_iCurSelector];
    if(sItem && m_pDrawTitle)
        m_pDrawTitle(gdi, m_rcTitle, sItem, m_lpClass);
    else if(sItem && sItem->szTitle)
        DrawText(gdi, sItem->szTitle, m_rcTitle, DT_CENTER|DT_VCENTER|DT_NOPREFIX, m_hFontTitle, m_crText);
}

void CIssPicControl::DrawDots(CIssGDIEx& gdi)
{
    if(m_pDrawDots)
        m_pDrawDots(gdi, m_rcDots, m_iCurSelector+1, m_arrItems.GetSize(), m_lpClass);
}

void CIssPicControl::DrawSelector(CIssGDIEx& gdi, RECT& rcDraw)
{
    FrameRect(gdi, rcDraw, 0);
    // no selector to draw
    /*if(m_uiSelected == 0)
        return;

    if(!m_imgSelected.IsLoaded())
        m_imgSelected.Initialize(m_hWndMyParent, m_hInstance, m_uiSelected);
    if(WIDTH(rcDraw) != m_imgSelected.GetWidth() || HEIGHT(rcDraw) != m_imgSelected.GetHeight())
        m_imgSelected.SetSize(WIDTH(rcDraw), HEIGHT(rcDraw));

    m_imgSelected.DrawImage(gdi, rcDraw.left, rcDraw.top);*/

}

void CIssPicControl::DrawItem(CIssGDIEx& gdi, RECT& rc, TypePicItems* sItem, BOOL bIsHighlighted)
{

    if(sItem->lpItem && m_pDrawFunc && m_lpClass)
    {
        m_pDrawFunc(gdi, rc, sItem, bIsHighlighted, m_lpClass);
    }
    else if(sItem->sPic && sItem->sPic->gdiImage)
    {
	    Draw(gdi,
		     rc.left + INDENT, rc.top,
		     WIDTH(rc), HEIGHT(rc),
		     *sItem->sPic->gdiImage,
		     0,0);
    }
}

void CIssPicControl::DrawDefaultItem(CIssGDIEx& gdi, RECT& rc)
{
	if(m_gdiDefaultImage.GetWidth() != m_sizeItem.cx || m_gdiDefaultImage.GetHeight() != m_sizeItem.cy)
	{
		if(m_uiDefaultImage == 0)
			return;

		CIssGDIEx gdiDefault;
		gdiDefault.LoadImage(m_uiDefaultImage, m_hWndMyParent, m_hInstance, TRUE);
		if(m_sizeItem.cx == gdiDefault.GetWidth() && m_sizeItem.cy == gdiDefault.GetHeight())
		{
			m_gdiDefaultImage.Create(gdiDefault, m_sizeItem.cx, m_sizeItem.cy, FALSE, TRUE);
			m_gdiDefaultImage.InitAlpha(TRUE);
			Draw(m_gdiDefaultImage,
				 0,0,
				 m_sizeItem.cx, m_sizeItem.cy,
				 gdiDefault,
				 0,0);
		}
		else
		{
			ScaleImage(gdiDefault, m_gdiDefaultImage, m_sizeItem, TRUE, RGB(0,0,0));
		}
	}

	Draw(gdi,
		rc.left + INDENT, rc.top,
		m_gdiDefaultImage.GetWidth(), m_gdiDefaultImage.GetHeight(),
		m_gdiDefaultImage,
		0,0);
}

int	CIssPicControl::GetNextScrollToPos()
{
	static float fInc = 0.0;

	// 1)calculate the scroll increment once at beginning of scroll
	if(m_iScrollToStart == m_iScrollPos)
	{
		// calculate increment
		fInc = CalcScrollIncrement();
		if(m_eScrollDir == DIR_Left)
			fInc *= -1.0;
	}

	// 2) adjust the increment for where in the scroll area we are
	// first 1/3 of travel = accelerating
	BOOL bAccelerate = FALSE;
	if(m_eScrollDir == DIR_Right)
	{
		if((m_iScrollPos /*+ (int)fInc*/ - m_iScrollToStart) < (m_iScrollToDest - m_iScrollToStart) / 3)
			bAccelerate = TRUE;
	}
	else
	{
		if((m_iScrollToStart - m_iScrollPos /*+ (int)fInc*/) < (m_iScrollToStart - m_iScrollToDest) / 3)
			bAccelerate = TRUE;
	}

	// last 1/3 of travel = accelerating
	BOOL bDeccelerate = FALSE;
	if(m_eScrollDir == DIR_Right)
	{
		if((m_iScrollPos - m_iScrollToStart) > (m_iScrollToDest - m_iScrollToStart) * 2 / 3)
			bDeccelerate = TRUE;
	}
	else
	{
		if((m_iScrollToStart - m_iScrollPos) > (m_iScrollToStart - m_iScrollToDest) * 2 / 3)
			bDeccelerate = TRUE;
	}

	if(bAccelerate)
		fInc *= INC_ACCEL_FACTOR;
	if(bDeccelerate)
		fInc /= INC_ACCEL_FACTOR;

	// ensure fInc >= 1.0
	if(m_eScrollDir == DIR_Right)
		fInc = fInc > (float)1.0 ? fInc : (float)1.0;
	else
		fInc = fInc < (float)-1.0 ? fInc : (float)-1.0;

	return m_iScrollPos + (int)fInc;
}

BOOL CIssPicControl::ScrollToDestReached()
{
	if(m_eScrollDir == DIR_Right && m_iScrollPos >= m_iScrollToDest)
		return TRUE;
	if(m_eScrollDir == DIR_Left && m_iScrollPos <= m_iScrollToDest)
		return TRUE;

	return FALSE;
}

float CIssPicControl::CalcScrollIncrement()
{
    return (IsVGA()?32.0f:16.0f);
}

void CIssPicControl::ReDrawList(void)
{
	// draw to the screen now
	InvalidateRect(m_hWndMyParent, &m_rcLoc, FALSE);
	UpdateWindow(m_hWndMyParent);
}

void CIssPicControl::EnsureSelectedItemCentered()
{
    if(!(m_dwFlags&OPTION_SnapCenter))
        return;

    ShowSelectedItem(TRUE);
}

void CIssPicControl::EnsureWithinBounds()
{
	int iRight = max(0, GetListWidth() - WIDTH(m_rcList));
    int iLeft  = 0;
    int iSizeItem = m_sizeItem.cx + m_iIndent;

    if(m_dwFlags & OPTION_SnapCenter)
    {
        iLeft = iLeft - (WIDTH(m_rcList) - iSizeItem)/2;
        iRight= iRight + (WIDTH(m_rcList) - iSizeItem)/2;
    }

	//bounds checking
	if(m_iScrollPos < iLeft)
	{
		if(m_eScroll == PICSCROLL_Stopped || m_eScroll == PICSCROLL_Scrolling)
			ScrollTo(iLeft);
		else if(m_eScroll == PICSCROLL_To)
		{
			// don't do anything
		}
		else
		{
			m_dbSpeed = 0;
			m_dbFriction = FRICTION;
			KillTimer(m_hWndMyParent, IDT_SROLL_TIMER);
			m_eScroll = PICSCROLL_Stopped;
			m_iScrollPos = iLeft;
		}       
	}
	else if(m_iScrollPos >= iRight)
	{
		if(m_eScroll == PICSCROLL_Stopped || m_eScroll == PICSCROLL_Scrolling)
			ScrollTo(iRight);
		else if(m_eScroll == PICSCROLL_To)
		{
			// don't do anything
		}
		else
		{
			m_dbSpeed = 0;
			m_dbFriction = FRICTION;
			KillTimer(m_hWndMyParent, IDT_SROLL_TIMER);
			m_iScrollPos = iRight;
			m_eScroll = PICSCROLL_Stopped;
		}
	}


}

//really shouldn't be doing bounds checking here ... but good enough for now
void CIssPicControl::ReDrawList(int iScrollAmount)
{
	RECT rc = m_rcList;

	int iBottom = max(0, GetListWidth() - WIDTH(rc));

	int iLastScrollPos = m_iScrollPos;
	m_iScrollPos -= iScrollAmount;

	if(iLastScrollPos == m_iScrollPos && m_eScroll == PICSCROLL_Moving)
	{
		m_dbSpeed = 0;
		m_eScroll = PICSCROLL_Stopped;
		m_dbFriction = FRICTION;
		KillTimer(m_hWndMyParent, IDT_SROLL_TIMER);
	}

	if(m_eScroll == PICSCROLL_Moving || m_eScroll == PICSCROLL_Scrolling)
	{
		if(m_eScroll == PICSCROLL_Moving && (m_iScrollPos < 0 || m_iScrollPos >= iBottom))
		{
			m_dbFriction = 1.5;
		}
	}
	else
	{
		//bounds checking
		EnsureWithinBounds();
	}

	if(m_iScrollPos != iLastScrollPos)
		ReDrawList();
}

int	CIssPicControl::GetListWidth()
{
	if(m_arrItems.GetSize() == 0)
		return 0;

	if(m_iCurrentWidth == -1)
	{
		m_iCurrentWidth = m_arrItems.GetSize()*(m_sizeItem.cx + m_iIndent);
	}
	return m_iCurrentWidth;
}

void CIssPicControl::ScrollTo(int iXPos, BOOL bNoWait)
{
    int iRight = max(0, GetListWidth() - WIDTH(m_rcList));
    int iSizeItem = m_sizeItem.cx + m_iIndent;
    if(m_dwFlags & OPTION_SnapCenter)
        iRight = iRight + (WIDTH(m_rcList) - iSizeItem)/2;
	
	if(bNoWait)//skip the animation
	{
		m_eScroll = PICSCROLL_Stopped;
		m_iScrollPos = min(iXPos, iRight);
	}
	else
	{		
		m_iScrollToStart = m_iScrollPos;        
		m_iScrollToDest = min(iXPos, iRight); // min(iYPos, GetEndOfListYPos() - HEIGHT(rc) + rc.top);
		m_eScrollDir = m_iScrollToStart > m_iScrollToDest ? DIR_Left : DIR_Right;
		//DBG_OUT((_T("CIssKineticList::ScrollTo iYPos: %d, scrollstart: %d, scrolldest: %d"), iYPos, m_iScrollToStart, m_iScrollToDest));
		m_eScroll = PICSCROLL_To;
		KillTimer(m_hWndMyParent, IDT_SROLL_TIMER);
		SetTimer(m_hWndMyParent, IDT_SROLL_TIMER, REFRESH_RATE, NULL);
	}
}

int	CIssPicControl::GetNextItemIndex(int iItemIndex, BOOL bSearchUp)
{
	int iNewIndex = iItemIndex + (bSearchUp?-1:1);
	return GetNextSelectableItem(iNewIndex, bSearchUp);
}

int CIssPicControl::GetNextSelectableItem(int iItem, BOOL bSearchUp)
{
	if(m_arrItems.GetSize() == 0)
		return UND;

	if(IsItemSelectable(iItem))
		return iItem;

	if(m_arrItems.GetSize() == 1)
		return UND;

	int iNewIndex = iItem + (bSearchUp?-1:1);

	while(iNewIndex != iItem)
	{
		if(iNewIndex < 0)
			iNewIndex = m_arrItems.GetSize() - 1;
		else if(iNewIndex < 0)
			break;
		else if(iNewIndex >= m_arrItems.GetSize())
			iNewIndex = 0;
		else if(iNewIndex >= m_arrItems.GetSize())
			break;

		if(iNewIndex == iItem)
			break;

		if(IsItemSelectable(iNewIndex))
			return iNewIndex;

		// increment
		iNewIndex += (bSearchUp?-1:1);
	}

	return UND;
}

BOOL CIssPicControl::IsItemSelectable(TypePicItems* sItem)
{
	if(!sItem)
		return FALSE;

	return TRUE;
}

BOOL CIssPicControl::IsItemSelectable(int iIndex)
{
	TypePicItems* sItem = m_arrItems[iIndex];
	return IsItemSelectable(sItem);
}

void CIssPicControl::ShowSelectedItem(BOOL bAnimated)
{
	if(m_iCurSelector == UND)
		return;

	TypePicItems* sItem = m_arrItems[m_iCurSelector];
	if(!sItem)
		return;

	int iXStart = GetXStartPosition(sItem);

    int iSizeItem = m_sizeItem.cx + m_iIndent;

    if(m_dwFlags & OPTION_SnapCenter)
    {
        if(m_iScrollPos != (iXStart - (WIDTH(m_rcList) - iSizeItem)/2))
        {
            CIssPicControl::ScrollTo(iXStart - (WIDTH(m_rcList) - iSizeItem)/2, !bAnimated);
            InvalidateRect(m_hWndMyParent, &m_rcLoc, FALSE);
        }
    }
    else
    {
	    // if its off screen (even partially)
	    if(iXStart < m_iScrollPos) 
	    {
		    int iScroll = iXStart - WIDTH(m_rcList)/2 + 2*INDENT;
		    if(iScroll < 0)
			    iScroll = 0;
		    CIssPicControl::ScrollTo(iScroll, !bAnimated);
		    InvalidateRect(m_hWndMyParent, &m_rcLoc, FALSE);
	    }
	    else if(iXStart + iSizeItem > (m_iScrollPos + WIDTH(m_rcList)))
	    {
		    int iScroll = iXStart - WIDTH(m_rcList)/2 + 2*INDENT;
		    if(iXStart > GetListWidth() - WIDTH(m_rcList)/2)
			    iScroll = iXStart;
		    CIssPicControl::ScrollTo(iScroll, !bAnimated);
		    InvalidateRect(m_hWndMyParent, &m_rcLoc, FALSE);
	    }
    }
}


int CIssPicControl::GetXStartPosition(int iIndex)
{
	TypePicItems* sItem = m_arrItems[iIndex];
	if(!sItem)
		return UND;

	return GetXStartPosition(sItem);
}

int CIssPicControl::GetXStartPosition(TypePicItems* sItem)
{
	if(!sItem)
		return UND;

	int iX = 0;
    int iItemSize = m_sizeItem.cx + m_iIndent;
	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		TypePicItems* sItemC = m_arrItems[i];
		if(!sItemC)
			continue;

		if(sItemC == sItem)
			return iX;

		iX += iItemSize;
	}

	return 0;
}

void CIssPicControl::StopScrolling()
{ 
    m_dbSpeed = 0;
    m_dbFriction = FRICTION;
    KillTimer(m_hWndMyParent, IDT_SROLL_TIMER);
    m_eScroll = PICSCROLL_Stopped; 
}

int CIssPicControl::GetItemIndexAtPos(int iXPos)
{
    TypePicItems* sItem = NULL;

    if(iXPos < 0 || iXPos > WIDTH(m_rcList))
        return -1;

    int iWidth = 0;
    int iItemSize = m_sizeItem.cx + m_iIndent;
    RECT rcItem;
    for(int i = 0; i < m_arrItems.GetSize(); i++)
    {
        sItem = m_arrItems[i];
        if(sItem == NULL)
        {
            ASSERT(0);
            return -1;
        }

        rcItem.left  = iWidth - m_iScrollPos;
        rcItem.right  = rcItem.left + iItemSize;

        iWidth     += iItemSize;

        if(rcItem.left <= iXPos && rcItem.right >= iXPos)
        {
            return i;
        }
    }

    return -1;
}

BOOL CIssPicControl::SelecteItem(int iIndex)
{
    if(iIndex < 0 || iIndex >= m_arrItems.GetSize())
        return FALSE;

    if(!IsItemSelectable(iIndex))
        return FALSE;

    TypePicItems* sItem = m_arrItems[iIndex];
    if(!sItem)
        return FALSE;

    EnsureWithinBounds();
    EnsureSelectedItemCentered();

    // notify the parent
    PostMessage(m_hWndNotif, WM_COMMAND, sItem->uiMenuID, 0);

    return TRUE;
}

HRESULT CIssPicControl::SetSelectedItemIndex(int iIndex, BOOL bDisplaySelected)
{
    HRESULT hr = S_OK;

    if(m_arrItems.GetSize() == 0)
        return E_FAIL;

    int iNewItem = GetNextSelectableItem(iIndex, FALSE);
    if(iNewItem == UND)
    {
        hr = E_FAIL;
        goto Error;
    }

    m_iCurSelector     = iNewItem;

Error:
    return hr;
}

RECT CIssPicControl::GetDrawRect(int iItemIndex)
{
    RECT rc;
    ZeroMemory(&rc, sizeof(RECT));

    TypePicItems* sItem = m_arrItems[iItemIndex];
    if(!sItem)
        return rc;

    int iXpos = GetXStartPosition(sItem);
    int iItemSize = m_sizeItem.cx  + m_iIndent;

    if(iXpos+iItemSize < m_iScrollPos)
        return rc;

    if(m_iScrollPos+WIDTH(m_rcList) < iXpos)
        return rc;

    rc.left = m_rcLoc.left + iXpos - m_iScrollPos;
    rc.right= rc.left + iItemSize;
    rc.top  = m_rcList.top + (HEIGHT(m_rcList) - m_sizeItem.cy)/2;
    rc.bottom = rc.top + m_sizeItem.cy;

    return rc;
}
