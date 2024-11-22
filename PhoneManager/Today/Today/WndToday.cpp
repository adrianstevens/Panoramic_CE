#include "StdAfx.h"
#include "WndToday.h"
#include "resource.h"
#include <todaycmn.h>
#include "ObjTodayComponent.h"
#include "IssCommon.h"
#include "IssRect.h"
#include "IssDebug.h"
#include "IssGDIDraw.h"
#include "CommonDefines.h"
#include "CommonGuiDefines.h"
#include "DlgOptions.h"
#include "DlgAddTodayItem.h"
#include "PoomContacts.h"
#include "PimStore.h"
#include "ObjPhoneProfile.h"
#include "DlgPerContactBase.h"
#include "DlgChoosePrograms.h"

#ifndef WM_TODAYCUSTOM_RECEIVEDSELECTION
#define WM_TODAYCUSTOM_RECEIVEDSELECTION    (WM_USER + 244)
#define WM_TODAYCUSTOM_LOSTSELECTION        (WM_USER + 245)
#define WM_TODAYCUSTOM_USERNAVIGATION       (WM_USER + 246)
#define WM_TODAYCUSTOM_ACTION               (WM_USER + 247)
#define WM_TODAYCUSTOM_CLOSEDATABASES       (WM_USER + 248)

#define TODAYM_TOOKSELECTION        (WM_USER + 102)
#define TODAYCOLOR_TEXT            0x10000004
#define TODAYCOLOR_HIGHLIGHT       0x10000022
#define TODAYCOLOR_HIGHLIGHTEDTEXT 0x10000023
#endif

#define WND_TodayClass		_T("ClassToday")
#define WND_TodayName		_T("TodayWindowChild")

#define MIN_DragMoveThresh  16 
#define MAX_TapLaunchTime   1000 
#define IDT_SaveTimer       1000

ATOM		g_aAtom = -1;

extern CDlgOptions* g_dlgOptions;

CWndToday::CWndToday(void)
:m_crText(RGB(255,255,255))
, m_bFocus(FALSE)
, m_bContextMenu(FALSE)
, m_bHoldAndMove(FALSE)
, m_bPreventUsage(FALSE)
{
    CIssStateAndNotify::InitializeNotifications();
}

CWndToday::~CWndToday(void)
{
	m_objItems.Destroy();
    CIssStateAndNotify::DestroyNotifications();

	CObjTodayMessageComponent::DeleteNotify();
	CObjTodayComponentBase::DeleteAllContent();
	CIssKineticList::DeleteAllContent();
	CDlgPerContactBase::DeleteAllContent();
	CPoomContacts::DeleteInstance();
	m_oStr->DeleteInstance();
}

BOOL CWndToday::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(m_oStr)
        m_oStr->SetResourceInstance(m_hInst);

    CIssStateAndNotify::DestroyNotifications();
	CIssStateAndNotify::RegisterWindowNotification(GetWnd(), WM_NOTIFY_TIME);
	CIssStateAndNotify::RegisterWindowNotification(GetWnd(), WM_NOTIFY_DATE);
	CIssStateAndNotify::RegisterWindowNotification(GetWnd(), WM_NOTIFY_MISSED);
	CIssStateAndNotify::RegisterWindowNotification(GetWnd(), WM_NOTIFY_SMS);
	CIssStateAndNotify::RegisterWindowNotification(GetWnd(), WM_NOTIFY_VOICEMAIL);
	CIssStateAndNotify::RegisterWindowNotification(GetWnd(), WM_NOTIFY_EMAIL);
    CIssStateAndNotify::RegisterWindowNotification(GetWnd(), WM_NOTIFY_PROFILE);
    CIssStateAndNotify::RegisterCustomWindowNotification(GetWnd(),
                                                        HKEY_CURRENT_USER,
                                                        REG_KEY_ISS_PATH,
                                                        REG_PhoneProfile,
                                                        WM_NOTIFY_PROFILE);
    CIssStateAndNotify::RegisterCustomWindowNotification(GetWnd(),
                                                        HKEY_CURRENT_USER,
                                                        REG_SoundCategories,
                                                        REG_SoundInitVolume,
                                                        WM_NOTIFY_RINGVOLUME);
    CIssStateAndNotify::RegisterCustomWindowNotification(GetWnd(),
                                                        HKEY_CURRENT_USER,
                                                        REG_Ringtones,
                                                        REG_Sound,
                                                        WM_NOTIFY_RINGNAME);
    CIssStateAndNotify::RegisterCustomWindowNotification(GetWnd(),
                                                        SN_PHONERADIOOFF_ROOT,
                                                        SN_PHONERADIOOFF_PATH,
                                                        SN_PHONERADIOOFF_VALUE,
                                                        WM_NOTIFY_FLIGHTMODE);

    CDlgPerContactBase::PreloadImages(m_hWnd, m_hInst, &m_gdiMem, NULL);

    CPoomContacts* pPoom = new CPoomContacts(GetWnd(), TRUE);

    CheckDemo();    

	return TRUE;
}

BOOL CWndToday::OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	/*TODAYDRAWWATERMARKINFO dwi;
	GetClientRect(hWnd, &dwi.rc);

	if(m_gdiBackground.GetDC() == NULL ||
	   m_gdiBackground.GetWidth() != (dwi.rc.right - dwi.rc.left) || 
	   m_gdiBackground.GetHeight() != (dwi.rc.bottom - dwi.rc.top))
	{
		HDC dc = (HDC)wParam;
		m_gdiBackground.Destroy();
#ifdef DEBUG
		dc = ::GetDC(::GetParent(hWnd));
		RECT rcContext;
		::GetWindowRect(hWnd, &rcContext);
		RECT rcParentContext;
		::GetWindowRect(::GetParent(hWnd), &rcParentContext);
		int iOffset = rcContext.top - rcParentContext.top;
		RECT rcBg = {0,iOffset,rcContext.right,iOffset + HEIGHT(rcContext)};

		m_gdiBackground.Create(dc, rcBg, TRUE, FALSE, FALSE);
#else
		m_gdiBackground.Create(dc, dwi.rc, TRUE, FALSE, FALSE);
#endif
	}

	
	dwi.hdc = m_gdiBackground.GetDC();
	dwi.hwnd = hWnd;
	SendMessage(GetParent(hWnd), TODAYM_DRAWWATERMARK, 0, (LPARAM)&dwi);*/

	return TRUE;
}

BOOL CWndToday::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL ||
		m_gdiMem.GetWidth() != (rcClient.right - rcClient.left) || 
		m_gdiMem.GetHeight() != (rcClient.bottom - rcClient.top))
    {
		m_gdiMem.Create(hDC, rcClient, FALSE, TRUE, FALSE);
        FillRect(m_gdiMem, rcClient, 0);
    }

	// this could be where we're getting unwanted stuff drawn to the screen when starting a drag
	if(m_bHoldAndMove)
		return FALSE;

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

	// copy the background
	DrawBackground(m_gdiMem, rcClient);

#ifndef DEBUG
    if(m_bPreventUsage)
    {
        ::DrawText(m_gdiMem, _T("Thank you for trying Panoramic Phone Genius.  Your trial period is now over.  Please return to the location that provided your download to purchase"), rcClient, DT_TOP|DT_CENTER|DT_WORDBREAK, m_objItems.GetFont(), RGB(255,255,255));
    }
    else
#endif
	{
		if(m_objItems.IsLoading())
		{
			::DrawTextShadow(m_gdiMem, _T("Loading Panoramic Phone Genius..."), rcClient, DT_VCENTER|DT_CENTER, m_objItems.GetFont(), RGB(255,255,255), RGB(0,0,0));
			BitBlt(hDC,
				rcClip.left,rcClip.top,
				WIDTH(rcClip), HEIGHT(rcClip),
				m_gdiMem.GetDC(),
				rcClip.left,rcClip.top,
				SRCCOPY);
			DrawBackground(m_gdiMem, rcClient);
			m_objItems.ResetLoading();
		}

		m_objItems.Draw(m_gdiMem, 
			rcClient,
			rcClip,
			m_crText, 
			m_crHighlight, 
			m_bFocus,			
			-1	//m_iHoldingIndex// if holding, don't draw it
			);	
	}

	BitBlt(hDC,
		   rcClip.left,rcClip.top,
		   WIDTH(rcClip), HEIGHT(rcClip),
		   m_gdiMem.GetDC(),
		   rcClip.left,rcClip.top,
		   SRCCOPY);

	return TRUE;
}
void CWndToday::DrawBackground(CIssGDIEx& gdi, RECT& rcClip)
{
    TODAYDRAWWATERMARKINFO dwi;
    dwi.hdc     = gdi.GetDC();
    dwi.hwnd    = m_hWnd;
    dwi.rc      = rcClip;
    SendMessage(GetParent(m_hWnd), TODAYM_DRAWWATERMARK, 0, (LPARAM)&dwi);
	
	
}
BOOL CWndToday::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uiMessage)
	{
	case WM_TODAYCUSTOM_CLEARCACHE:
		return OnTodayClearCache(hWnd, wParam, lParam);
	case WM_TODAYCUSTOM_QUERYREFRESHCACHE:
		return OnTodayQueryRefreshCache(hWnd, wParam, lParam);
	case WM_TODAYCUSTOM_RECEIVEDSELECTION:
		{
			m_crText = (COLORREF)SendMessage(GetParent(hWnd), TODAYM_GETCOLOR, TODAYCOLOR_HIGHLIGHTEDTEXT, 0);
			m_bFocus = TRUE;
            CObjTodayComponentBase* pItem = m_objItems.GetSelectedItem();
            if(!pItem)
            {
                m_objItems.SetSelection(0); // set a default one
                pItem = m_objItems.GetSelectedItem();
            }
            if(pItem)
            {
                RECT rcItem = pItem->GetFrame();
                InvalidateRect(hWnd, &rcItem, FALSE);
            }
		}
		break;
	case WM_TODAYCUSTOM_LOSTSELECTION:
		{
			m_crText = (COLORREF)SendMessage(GetParent(hWnd), TODAYM_GETCOLOR, TODAYCOLOR_TEXT, 0);
			m_bFocus = FALSE;
			CObjTodayComponentBase* pItem = m_objItems.GetSelectedItem();
            if(pItem)
            {
                RECT rcItem = pItem->GetFrame();
                InvalidateRect(hWnd, &rcItem, FALSE);
            }
		}
		break;
	case WM_TODAYCUSTOM_ACTION:
#ifndef DEBUG
        if(m_bPreventUsage)
            return FALSE;
#endif
		m_objItems.OnKeyDown(hWnd, wParam, lParam);
		break;
	case WM_TODAYCUSTOM_USERNAVIGATION:
		{
			BOOL bResult = m_objItems.OnKeyDown(hWnd, wParam, lParam);
			//if(bResult)
				//InvalidateRect(m_hWnd, NULL, TRUE);
			return bResult;
		}
		break;
	case WM_NOTIFY_TIME:
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

        if(sysTime.wMinute % 10 == 0)
        {
            //m_objItems.RefreshItem(NumComponentTypes);  // refresh everything
            CheckDemo();
        }
        //else
        {
		    m_objItems.RefreshItem(CompType_Info_TimeLarge);
		    m_objItems.RefreshItem(CompType_Info_TimeSmall);
            m_objItems.RefreshItem(CompType_Sys_Profile);
        }   

        // if the demo is over
        if(m_bPreventUsage)
            InvalidateRect(m_hWnd, NULL, FALSE);

		break;
	case WM_NOTIFY_DATE:
		//m_objItems.RefreshItem(CompType_Info_Date);
		break;
	case WM_NOTIFY_MISSED:
		m_objItems.RefreshItem(CompType_Message_MissedCall);
		break;
	case WM_NOTIFY_SMS:
		m_objItems.RefreshItem(CompType_Message_SMS);
		break;
	case WM_NOTIFY_VOICEMAIL:
		m_objItems.RefreshItem(CompType_Message_VoiceMail);
		break;
	case WM_NOTIFY_EMAIL:
		m_objItems.RefreshItem(CompType_Message_Email);
		break;
    case WM_NOTIFY_BLUETOOTH:
        //m_objItems.RefreshItem(CompType_Message_Bluetooth);
        break;
    case WM_NOTIFY_WIFI:
        //m_objItems.RefreshItem(CompType_Message_Wifi);
        break;
    case WM_NOTIFY_PROFILE:
    case WM_NOTIFY_RINGVOLUME:
    case WM_NOTIFY_RINGNAME:
    case WM_NOTIFY_FLIGHTMODE:
        m_objItems.RefreshItem(CompType_Sys_Profile);
        break;
    case PIM_ITEM_CHANGED_LOCAL:
    case PIM_ITEM_CHANGED_REMOTE:
        {
            LONG lOID = (LONG)wParam;
            m_objItems.RefreshItem(CompType_Contact, lOID);
        }
        break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CWndToday::OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) lParam;

	lpMeasureItem->itemHeight = m_objItems.GetMenuItemHeight();
	lpMeasureItem->itemWidth = GetSystemMetrics(SM_CXSCREEN)/2;
	return TRUE;
}

BOOL CWndToday::OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);


	COLORREF crText = GetSysColor(COLOR_MENUTEXT);
	// if we have to recreate the selector
	if(lpDrawItem->itemState & LVIS_SELECTED  || lpDrawItem->itemState & LVIS_FOCUSED)
	{
		FillRect(lpDrawItem->hDC, lpDrawItem->rcItem, GetSysColor(COLOR_HIGHLIGHT));
		crText = GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else
	{
        FillRect(lpDrawItem->hDC, lpDrawItem->rcItem, GetSysColor(COLOR_MENU));
	}


    m_objItems.DrawMenuItem(lpDrawItem->hDC, lpDrawItem->rcItem, lpDrawItem->itemData, crText);
	return TRUE;
}

BOOL CWndToday::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case IDMENU_SmartArrange:
		m_objItems.SmartArrangeItems(hWnd);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
    case IDMENU_RestoreDefaults:
        {
            if(IDYES == MessageBox(m_hWnd, _T("Restore plugin defaults?"), _T("Default"), MB_YESNO|MB_ICONHAND))
                m_objItems.SetDefaultLayout();
        }
        break;
	case IDMENU_LockItems:
		m_objItems.ToggleLock();
		break;
	case IDMENU_RemoveItem:
		m_objItems.RemoveItem(hWnd);
		break;
    case IDMENU_Help:
        CreateProcess(_T("peghelp"), _T("incominghelp.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
        break;
    case IDMENU_PhoneOptions:
        break;
	case IDMENU_AddItem:
		{
			// launch the edit rules window
			/*CDlgAddTodayItem dlg(&m_gdiMem, &m_guiB;
			if(dlg.DoModal(m_hWnd, m_hInst, IDD_DLG_Modal) == IDOK)
				m_objItems.Refresh(hWnd);*/
		}
		break;

		
	case IDMENU_EditItems:
		{
            // make sure we save what we have first
            KillTimer(hWnd, IDT_SaveTimer);
            m_objItems.Save();

#ifdef DEBUG
			CDlgOptions dlgOptions;
			dlgOptions.DoModal(hWnd, m_hInst);
#else
			if(g_dlgOptions)
			{
				g_dlgOptions->DoModal(hWnd, m_hInst);
			}
#endif
            
			m_objItems.Refresh(hWnd);
		}

		break;
	}
	return TRUE;

}

BOOL CWndToday::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//MessageBox(hWnd, _T("CWndToday::OnKeyDown"), _T(""), MB_OK);
	return UNHANDLED;
}

BOOL CWndToday::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CWndToday::OnTodayClearCache(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// do a full reload from registry...
	//MessageBox(hWnd, _T("OnTodayClearCache"), _T(""), MB_OK);
	m_objItems.Refresh(hWnd);

    // if there has been nothing set then put up some defaults
    if(m_objItems.CountItems() == 0)
        m_objItems.SetDefaultLayout();

	// read the text color
	m_crText			= (COLORREF)SendMessage(GetParent(hWnd), TODAYM_GETCOLOR, TODAYCOLOR_TEXT, 0);
	m_crTextHighlight	= (COLORREF)SendMessage(GetParent(hWnd), TODAYM_GETCOLOR, TODAYCOLOR_HIGHLIGHTEDTEXT, 0);
	m_crHighlight		= (COLORREF)SendMessage(GetParent(hWnd), TODAYM_GETCOLOR, TODAYCOLOR_HIGHLIGHT, 0);

	m_gdiMem.Destroy();
	//m_gdiBackground.Destroy();

	//InvalidateRect(m_hWnd, NULL, TRUE);
	return FALSE;
}
#ifdef DEBUG
void CWndToday::RefreshRegistry()
{
	m_objItems.Refresh(GetWnd());
}
#endif

BOOL CWndToday::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}


BOOL CWndToday::OnTodayQueryRefreshCache(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//MessageBox(hWnd, _T("OnTodayQueryRefreshCache"), _T(""), MB_OK);
	BOOL bReturn = FALSE;

	if(wParam)
	{
		int iMenuHeight = m_objItems.GetWindowHeight();

#ifndef DEBUG
        if(m_bPreventUsage)
            iMenuHeight = IsVGA()?120:60;
#endif
		TODAYLISTITEM* pTLI = (TODAYLISTITEM*)wParam;
		if(pTLI->cyp == 0 || iMenuHeight != (int)pTLI->cyp)
		{
			pTLI->cyp = iMenuHeight;
			bReturn = TRUE;
		}
	}

	// see if there is any new data to read
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
    rcClient.right  = GetSystemMetrics(SM_CXSCREEN);

	if(WIDTH(rcClient) != m_gdiMem.GetWidth())
	{
		if(m_objItems.CheckAutoArrangeItems(rcClient.right))
			InvalidateRect(m_hWnd, NULL, TRUE);
	}

	//bReturn |= TRUE;

	return bReturn;
}

BOOL CWndToday::OnLButtonDown(HWND hWnd, POINT& pt)
{
	PostMessage(GetParent(hWnd), TODAYM_TOOKSELECTION, (WPARAM)hWnd, 0);
	m_bFocus = TRUE;

#ifndef DEBUG
    if(m_bPreventUsage)
        return TRUE;
#endif


	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	if(PtInRect(&rcClient, pt) == FALSE)
		return FALSE;

    m_bContextMenu      = FALSE;
    m_bHoldAndMove      = FALSE;
	m_ptLastLButtonDown = pt;
    m_dwTickCountGrab   = GetTickCount();
	m_iHoldingIndex = m_objItems.GetItemMouseOver(pt);
    if(m_iHoldingIndex != UND)
    {
	    m_objItems.SetSelection(m_iHoldingIndex);
	    InvalidateRect(hWnd, NULL, FALSE);
        UpdateWindow(hWnd); // draw now
    }

	SHRGINFO    shrg;
	shrg.cbSize		= sizeof(shrg);
	shrg.hwndClient = m_hWnd;
	shrg.ptDown.x	= pt.x;
	shrg.ptDown.y	= pt.y;
	shrg.dwFlags	= SHRG_RETURNCMD/*|SHRG_LONGDELAY*/;
	if (SHRecognizeGesture(&shrg) == GN_CONTEXTMENU)
	{
		// put the drop down menu
		m_bContextMenu = TRUE;
		OnContextMenu(pt);
	}
	else if(m_iHoldingIndex != UND)
	{
		CObjTodayComponentBase* sItem = m_objItems.GetItem(m_iHoldingIndex);
		if(!sItem)
			return TRUE;

		// figure out the offset so we can draw if we start moving it around
		m_sizeOffset.cx = pt.x - sItem->GetFrame().left;
		m_sizeOffset.cy = pt.y - sItem->GetFrame().top;

		SetCapture(m_hWnd);
		m_ptLastHolding = pt;

		// save the background without the item that we are moving
		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		m_gdiMovingBack.Create(m_gdiMem.GetDC(), rcClient, FALSE, TRUE, FALSE);
		DrawBackground(m_gdiMovingBack, rcClient);
		m_objItems.Draw(m_gdiMovingBack, rcClient, rcClient, m_crText, m_crHighlight, TRUE, m_iHoldingIndex);

		// save the item image that we are moving
		RECT rcItem = {0,0,WIDTH(sItem->GetFrame()), HEIGHT(sItem->GetFrame())};
		m_gdiMovingItem.Create(m_gdiMem.GetDC(), WIDTH(rcItem), HEIGHT(rcItem), FALSE, TRUE, FALSE);
		FillRect(m_gdiMovingItem, rcItem, 0);
        m_gdiMovingItem.InitAlpha(TRUE);
		sItem->Draw(m_gdiMovingItem, /*rcClient*/rcItem, m_crText, m_crHighlight, FALSE, FALSE, TRUE, TRUE);
		sItem->DrawText(m_gdiMovingItem, m_objItems.GetFont(), TRUE);
	}

	return TRUE;
}


BOOL CWndToday::OnLButtonUp(HWND hWnd, POINT& pt)
{
	// we actually allow OnLButtonUp in non-client area if user is dragging past bottom of window
	//if(PtInRect(&m_rcLocation, pt) == FALSE)
	//	return FALSE;

    // make sure we have valid values
    /*pt.x    = max(0, pt.x);
    pt.x    = min(GetSystemMetrics(SM_CXSCREEN), pt.x);
    pt.y    = max(0, pt.y);
    pt.y    = min(GetSystemMetrics(SM_CYSCREEN), pt.y);*/
    //DBG_OUT((_T("CWndToday::OnLButtonUp - %d,%d"), pt.x, pt.y));
    if(pt.y > GetSystemMetrics(SM_CYSCREEN))
    {
        pt.y = m_ptLastHolding.y;
    }

#ifndef DEBUG
    if(m_bPreventUsage)
        return TRUE;
#endif

    BOOL bSaveChanges = FALSE;

	if(m_iHoldingIndex != UND && !m_bContextMenu)
	{
		// this is now the currently selected item
		m_objItems.SetSelection(m_iHoldingIndex);

		if(m_bHoldAndMove)
		{
			if(!m_objItems.IsLocked())
			{
				// try and place the icon down
                bSaveChanges = IsSwapStraight(m_ptLastHolding);
				if(!bSaveChanges)
				{
					bSaveChanges = IsSwapAndFindRoom(m_ptLastHolding);
				}

			}
		}
		else
		{
			// select item
			int iItem = m_objItems.GetItemMouseOver(pt);
			int iLButtonDown = m_objItems.GetItemMouseOver(m_ptLastLButtonDown);
			if(iItem != UND && iItem == iLButtonDown)
			{
				CObjTodayComponentBase* pItem = m_objItems.GetItem(iItem);
				if(pItem)
					pItem->ExecuteItem(hWnd, pt);
			}
		}

		ReleaseCapture();
		m_gdiMovingItem.Destroy();
		m_gdiMovingBack.Destroy();
		m_iHoldingIndex = UND;
		m_bHoldAndMove	= FALSE; 
	}

	InvalidateRect(m_hWnd, NULL, FALSE);
    UpdateWindow(m_hWnd);
    if(bSaveChanges)
    {
        // wait 15 seconds before saving
        KillTimer(hWnd, IDT_SaveTimer);
        SetTimer(hWnd, IDT_SaveTimer, 15000, NULL);
    }

	m_bContextMenu = FALSE;
	return TRUE;
}

BOOL CWndToday::OnMouseMove(HWND hWnd, POINT& pt)
{
#ifndef DEBUG
    if(m_bPreventUsage)
        return TRUE;
#endif

    //DBG_OUT((_T("CWndToday::OnMouseMove - %d,%d"), pt.x, pt.y));

    // this check is used so we don't mistakenly make a move operation when a 
    // select was intended
    int iMoveThreshold = IsVGA() ? MIN_DragMoveThresh * 2 : MIN_DragMoveThresh;
    if((GetTickCount()-m_dwTickCountGrab < MAX_TapLaunchTime &&
        abs(m_ptLastLButtonDown.x - pt.x) < iMoveThreshold &&
        abs(m_ptLastLButtonDown.y - pt.y) < iMoveThreshold))
        return TRUE;

	if(!m_bHoldAndMove && m_ptLastLButtonDown.x == pt.x && m_ptLastLButtonDown.y == pt.y)
		return TRUE;

	if(m_iHoldingIndex == UND || m_bContextMenu)
		return UNHANDLED;

	// we're holding something
	m_bHoldAndMove = TRUE;

    // we don't need to save because we are moving something
    KillTimer(hWnd, IDT_SaveTimer);

    // if we're off the screen make sure we force it to be back on the screen
    if(pt.y > GetSystemMetrics(SM_CYSCREEN))
    {
        pt.y = m_ptLastHolding.y;
    }

	if(m_objItems.IsLocked())
		return TRUE;

	CObjTodayComponentBase* sItem = m_objItems.GetItem(m_iHoldingIndex);
	if(!sItem)
		return TRUE;

	// inval RECT includes 4 RECT's
	// old item RECT, old itemBG RECT, new item RECT, new itemBG RECT
	RECT rc, rcBack;

	// 1)last drawn item RECT
	rc.left		= m_ptLastHolding.x - m_sizeOffset.cx;
	rc.top		= m_ptLastHolding.y - m_sizeOffset.cy;
	rc.right	= rc.left + WIDTH(sItem->GetFrame());
	rc.bottom	= rc.top + HEIGHT(sItem->GetFrame());
	CIssRect rcFull(rc);

	// 2) last drawn item bg
	rcBack		= rc;
	int iBlockSize = CObjTodayComponentBase::GetBlockSize();
	SnapRectToGrid(rcBack, iBlockSize, iBlockSize);
	rcFull.Concatenate(rcBack);

	// do some quick checks to make sure our movement is not out of bounds,
	// and if it is then make the adjustments
	CheckBounds(pt);	

	// 3) new item RECT
	rc.left		= pt.x - m_sizeOffset.cx;
	rc.top		= pt.y - m_sizeOffset.cy;
	rc.right	= rc.left + WIDTH(sItem->GetFrame());
	rc.bottom	= rc.top + HEIGHT(sItem->GetFrame());
	rcFull.Concatenate(rc);

	// 4) new bg RECT
	rcBack			= rc;
	SnapRectToGrid(rcBack, iBlockSize, iBlockSize);
	rcFull.Concatenate(rcBack);

	m_ptLastHolding = pt;

	// draw the full background now
	BitBlt(m_gdiMem, /*m_objItems.GetGDI()*/
		rcFull.left, rcFull.top,
		rcFull.GetWidth(), rcFull.GetHeight(),
		m_gdiMovingBack.GetDC(),
		rcFull.left, rcFull.top,
		SRCCOPY);

	// draw the rectangle of exactly where the Icon will be placed if the user let's go
	AlphaFillRect(m_gdiMem /*m_objItems.GetGDI()*/, rcBack, GetSysColor(COLOR_HIGHLIGHT), 200);

	// draw the new item now that we are moving
	// do an Alpha draw to the destination image
    Draw(m_gdiMem/*m_objItems.GetGDI()*/,
         rc.left,
         rc.top,
         WIDTH(rc), HEIGHT(rc),
         m_gdiMovingItem,
         0,0,
         ALPHA_Normal,
         255);

	// draw it all to the screen now
	HDC hdcScreen = GetDC(m_hWnd);
	BitBlt(hdcScreen,
		rcFull.left, rcFull.top,
		rcFull.GetWidth(), rcFull.GetHeight(),
		m_gdiMem/*m_objItems.GetGDI()*/,
		rcFull.left, rcFull.top,
		SRCCOPY);
	ReleaseDC(m_hWnd, hdcScreen);

	return UNHANDLED;
}

BOOL CWndToday::CreateHome(HWND hWndParent, HINSTANCE hInst)
{
	m_hInst = hInst;

	WNDCLASS    wc;
	memset(&wc, 0, sizeof(wc));

	wc.style         = 0;
	wc.lpfnWndProc   = (WNDPROC)ProcWnd;
	wc.hInstance     = hInst;
	wc.hIcon         = 0;
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = WND_TodayClass;

	g_aAtom = RegisterClass(&wc);

	// initiate the window
	//Create a child window.
	//From BPP
	/*m_hWnd = CreateWindowEx(WND_TodayClass, WND_TodayName, WS_VISIBLE | WS_CHILD,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, hWndParent, NULL, hInst, NULL);*/

	m_hWnd = CreateWindowEx(0,
		WND_TodayClass,//(LPCTSTR)g_aAtom, 
		WND_TodayName, 
		/*WS_VISIBLE |*/ WS_CHILD,
		CW_USEDEFAULT,CW_USEDEFAULT, 
		CW_USEDEFAULT,CW_USEDEFAULT,
		hWndParent, NULL, 
		m_hInst, 
		(LPVOID)this);

	if(m_hWnd == NULL)
	{	//Silently fail.
		return FALSE;
	}

	m_objItems.Initialize(m_hWnd, m_hInst, m_gdiMem);

	return TRUE;

}


BOOL CWndToday::DestroyHome()
{
	m_hWnd = NULL;
	CIssWnd::Destroy();

	return TRUE;
}

void CWndToday::OnContextMenu(POINT pt)
{
	HMENU hMenuContext		= CreatePopupMenu();
	if(!hMenuContext)
		return;

	CObjTodayComponentBase* pItem = NULL;
    int iItemIndex = m_objItems.GetItemMouseOver(pt);
    if(iItemIndex != UND)
        pItem   = m_objItems.GetItem(iItemIndex);

	// sanity check
	// PA: I don't understand this
    /*CObjTodayComponentBase* pTestItem = 0;
	int pTestIndex = m_objItems.GetItemMouseOver(pt);
	if(pTestIndex != UND)
		pTestItem = m_objItems.GetItem(pTestIndex);
	ASSERT(pItem == pTestItem);*/

	if(pItem)
	{
		//AppendMenu(hMenuContext, MF_SEPARATOR, 0,  NULL);
		AppendMenu(hMenuContext, MF_STRING, IDMENU_RemoveItem,  _T("Remove Item"));
	}
	else
	{
		//AppendMenu(hMenuContext, MF_STRING, IDMENU_AddItem,  _T("Add Item"));
	}
	AppendMenu(hMenuContext, MF_STRING, IDMENU_EditItems, _T("Add/Remove Items"));
	AppendMenu(hMenuContext, MF_STRING|(m_objItems.IsLocked()?MF_CHECKED:NULL), IDMENU_LockItems,  _T("Lock Items"));
    AppendMenu(hMenuContext, MF_SEPARATOR, 0,  NULL);
	AppendMenu(hMenuContext, MF_STRING, IDMENU_RestoreDefaults,  _T("Restore Defaults"));
    //AppendMenu(hMenuContext, MF_STRING, IDMENU_PhoneOptions,  _T("Phone Options")); // put this in later
    AppendMenu(hMenuContext, MF_STRING, IDMENU_Help,  _T("Help"));

	//Display it.
	TrackPopupMenu(	hMenuContext, 
		TPM_LEFTALIGN|TPM_TOPALIGN, 
		pt.x,pt.y, 
		0, 
		m_hWnd, NULL);

	DestroyMenu(hMenuContext);
	return;
}

void CWndToday::CheckBounds(POINT& pt)
{
	CObjTodayComponentBase* pItem = m_objItems.GetItem(m_iHoldingIndex);
	if(pItem == NULL)
		return;
	int iBlockSize = CObjTodayComponentBase::GetBlockSize();

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
    rcClient.right  = GetSystemMetrics(SM_CXSCREEN);

	// left side
	if((pt.x - m_sizeOffset.cx) < 0)
		pt.x = m_sizeOffset.cx;

	// right side
	else if((pt.x - m_sizeOffset.cx + WIDTH(pItem->GetFrame())) >  rcClient.right)
	{
		pt.x = rcClient.right - WIDTH(pItem->GetFrame()) + m_sizeOffset.cx;
	}

	// top
	if((pt.y - m_sizeOffset.cy) < 0)
		pt.y = m_sizeOffset.cy;
	// bottom - leave bottom open 
}

// can we just do a straight swap or ?, if so then swap away
BOOL CWndToday::IsSwapStraight(POINT pt)
{
	// just in case
	if(m_iHoldingIndex == UND)
		return FALSE;

	// get the item
	CObjTodayComponentBase* objItem = m_objItems.GetItem(m_iHoldingIndex);
	if(!objItem)
		return FALSE;

	// get the desired location
	RECT rcDesired;
	rcDesired.left		= pt.x - m_sizeOffset.cx;
	rcDesired.top		= pt.y - m_sizeOffset.cy;
	rcDesired.right		= rcDesired.left + WIDTH(objItem->GetFrame());
	rcDesired.bottom	= rcDesired.top + HEIGHT(objItem->GetFrame());
	int iBlockSize = CObjTodayComponentBase::GetBlockSize();
	SnapRectToGrid(rcDesired, iBlockSize, iBlockSize);

	// if the space is empty, drop it there
	int iNumOverlapItems = m_objItems.GetNumItemsRectOver(rcDesired, objItem);
	if(iNumOverlapItems == 0)
	{
		objItem->SetLocation(rcDesired);
		return TRUE;
	}
	else if(iNumOverlapItems == 1)
	{
		// if the item RECT matches the desired RECT, then swap
		for(int i = 0; i < m_objItems.CountItems(); i++)
		{
			CObjTodayComponentBase* objTestItem = m_objItems.GetItem(i);
			if(!objTestItem)
				return FALSE;
			if(objTestItem == objItem)
				continue;

			RECT rcTest = objTestItem->GetFrame();
			if(rcTest.left == rcDesired.left && rcTest.right == rcDesired.right &&
				rcTest.top == rcDesired.top && rcTest.bottom == rcDesired.bottom)
			{
				objTestItem->SetMatrixLocation(objItem->GetMatrixRect());
				objItem->SetLocation(rcDesired);
				return TRUE;
			}
				
		}
	}
	return FALSE;

}

BOOL CWndToday::IsSwapAndFindRoom(POINT pt)
{
	// just in case
	if(m_iHoldingIndex == UND)
		return FALSE;

	// get the item
	CObjTodayComponentBase* objItem = m_objItems.GetItem(m_iHoldingIndex);
	if(!objItem)
		return FALSE;

	// get the desired location
	RECT rcDesired;
	rcDesired.left		= pt.x - m_sizeOffset.cx;
	rcDesired.top		= pt.y - m_sizeOffset.cy;
	rcDesired.right		= rcDesired.left + WIDTH(objItem->GetFrame());
	rcDesired.bottom	= rcDesired.top + HEIGHT(objItem->GetFrame());
	int iBlockSize = CObjTodayComponentBase::GetBlockSize();
	SnapRectToGrid(rcDesired, iBlockSize, iBlockSize);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
    rcClient.right = GetSystemMetrics(SM_CXSCREEN);

	// we already know there is a conflict - that's how we got here
	// remove conflicting items
	int iIndex = UND;
	do
	{
		iIndex = m_objItems.GetItemConflicting(rcDesired, objItem);
		if(iIndex != UND)
		{
			CObjTodayComponentBase* objRemoveItem = m_objItems.GetItem(iIndex);
			objRemoveItem->SetLocationUndefined();
		}
	}while(iIndex != UND);

	// set this item
	objItem->SetLocation(rcDesired);

	// do autoarrange
	if(m_objItems.CheckAutoArrangeItems(rcClient.right))
		return TRUE;

	return FALSE;
}

void CWndToday::CheckDemo()
{
    CIssKey oKey;
    oKey.Init(_T("SOFTWARE\\Panoramic\\CallManager"), _T("SOFTWARE\\Pano\\PS2"), 0x0, 0x1234CDEF, 0x67ABCEF1, 5, TRUE);
    oKey.CheckSetKey();
    oKey.CheckDemo();

    m_bPreventUsage = !oKey.m_bGood;
}

BOOL CWndToday::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam == IDT_SaveTimer)
    {
        KillTimer(hWnd, IDT_SaveTimer);
        m_objItems.Save();
    }
    return TRUE;
}



