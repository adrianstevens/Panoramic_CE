#include "IssMenuHandler.h"
#include "IssCommon.h"
#include "IssRect.h"
#include "IssGDIDraw.h"

CIssMenuHandler* g_pMenu = NULL;

#define		MENU_WIDTH				(min(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN))*3/4)
#define		MENU_INDENT				0//GetSystemMetrics(SM_CXSMICON)/8
#define		MENU_CHECK				GetSystemMetrics(SM_CXSMICON)

CIssMenuHandler::CIssMenuHandler(void)
:m_hWnd(NULL)
,m_hWndMenu(NULL)
,m_hInst(NULL)
,m_crText(RGB(255,255,255))
,m_crSelector(RGB(0,0,255))
,m_bDropShadowText(TRUE)
,m_uiBevel(0)
,m_bIsMenuUp(FALSE)
,m_oStr(CIssString::Instance())
{
	m_hFontText = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
}

CIssMenuHandler::~CIssMenuHandler(void)
{
	Destroy();
	CIssGDIEx::DeleteFont(m_hFontText);
}

BOOL CIssMenuHandler::Initialize(HWND hWnd, HINSTANCE hInst, COLORREF crText, COLORREF crSelector, BOOL bDropShadowText, UINT uiBevelArray)
{
	Destroy();

	g_pMenu			= this;
	m_hWnd			= hWnd;
	m_hInst			= hInst;
	m_crSelector	= crSelector;
	m_crText		= crText;
	m_uiBevel		= uiBevelArray;
	m_bDropShadowText = bDropShadowText;

	HDC dc = GetDC(NULL);
	RECT rcClient;
	SetRect(&rcClient, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	m_gdiScreen.Create(dc, rcClient, TRUE, TRUE);
	ReleaseDC(NULL, dc);

	return TRUE;
}

BOOL CIssMenuHandler::Destroy()
{
	m_gdiScreen.Destroy();
	m_gdiMem.Destroy();
	m_gdiBackground.Destroy();
	g_pMenu		= NULL;
	m_hWnd		= NULL;
	m_hWndMenu	= NULL;
	m_hInst		= NULL;
	m_uiBevel	= 0;
	m_crText	= RGB(255,255,255);
	m_crSelector= RGB(0,0,255);
	m_bDropShadowText = TRUE;

	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		TypeMenuEntry* sEntry = m_arrItems[i];
		if(sEntry)
			delete sEntry;
	}
	m_arrItems.RemoveAll();
	m_bIsMenuUp = FALSE;

	return TRUE;
}

BOOL CIssMenuHandler::OnEnterMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HWND hWndMenu = FindWindow( _T("MNU"),NULL );
	if ( hWndMenu == NULL ) 
		return FALSE;

	//m_hWndMenu = hWndMenu;
	//RECT rcWindow = {0};
	//GetWindowRect(m_hWndMenu, &rcWindow);
	//DebugOut(_T("CIssMenuHandler::OnEnterMenuLoop - 0x%X,%d,%d,%d,%d"), m_hWndMenu,rcWindow.left, rcWindow.top,rcWindow.right,rcWindow.bottom);
	
	m_wndProcMenu = (WNDPROC)SetWindowLong( hWndMenu,GWL_WNDPROC,(LONG)ProcDefault );

	return FALSE;	// process the message
}

BOOL CIssMenuHandler::OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT lpItem = (LPMEASUREITEMSTRUCT)lParam;

	//DebugOut(_T("CIssMenuHandler::OnMeasureItem"));

	MENUITEMINFO sMenuItem = {0};
	if(!FindMenuItem(lpItem->itemID, &sMenuItem))
	{
		lpItem->itemHeight	= 0;
		lpItem->itemWidth	= 0;
		return TRUE;
	}

	// find the height
	if(sMenuItem.fType & MFT_SEPARATOR)
		lpItem->itemHeight	= GetSystemMetrics(SM_CXICON)*1/6;
	else
		lpItem->itemHeight	= GetSystemMetrics(SM_CXSMICON)+GetSystemMetrics(SM_CXSMICON)/4;//small icon + 4


	HDC hDC = GetDC(hWnd);
	HFONT hOldFont = (HFONT)SelectObject(hDC, m_hFontText);
	SIZE sizeText = {0,0};

	TCHAR szText[STRING_MAX] = _T("");
	
	if(sMenuItem.dwItemData)
		m_oStr->StringCopy(szText, (UINT)sMenuItem.dwItemData, STRING_MAX, m_hInst);
	
	if(!m_oStr->IsEmpty(szText))
	{
		GetTextExtentExPoint(hDC, 
			szText, 
			m_oStr->GetLength(szText), 
			0,
			NULL,NULL,
			&sizeText);
	}
	SelectObject(hDC, hOldFont);
	ReleaseDC(hWnd, hDC);

	lpItem->itemWidth = sizeText.cx + 2*MENU_INDENT;

	//if(sMenuItem.fState & MFS_CHECKED || sMenuItem.fType & MFT_RADIOCHECK)
		lpItem->itemWidth += MENU_CHECK;


	return TRUE;
}

BOOL CIssMenuHandler::OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpItem = (LPDRAWITEMSTRUCT)lParam;

	//DebugOut(_T("ID:%d, action:%d, state:%d"), lpItem->itemID, lpItem->itemAction, lpItem->itemState);

	if(m_hWndMenu == NULL)
	{
		m_hWndMenu = FindWindow( _T("MNU"),NULL );		

		// find out where the window is placed on the screen
		GetWindowRect(m_hWndMenu, &m_rcMenuWindow);

		// this is to offset for that damn dialog border
		int iLineWidth = GetSystemMetrics(SM_CXBORDER);

		m_rcMenuWindow.left		+= iLineWidth;
		m_rcMenuWindow.top		+= iLineWidth;
		m_rcMenuWindow.bottom	-= iLineWidth;
		m_rcMenuWindow.right	-= iLineWidth;

		m_gdiBackground.Create(lpItem->hDC, m_rcMenuWindow, FALSE, TRUE);

		// draw whatever is on the screen first
		BitBlt(m_gdiBackground.GetDC(),
			0,0,
			WIDTH(m_rcMenuWindow), HEIGHT(m_rcMenuWindow),
			m_gdiScreen.GetDC(),
			m_rcMenuWindow.left+lpItem->rcItem.left,m_rcMenuWindow.top+lpItem->rcItem.top,
			SRCCOPY);

		m_gdiScreen.Destroy();	// we don't use it anymore

		CIssRect rc(m_rcMenuWindow);
		rc.ZeroBase();

		if(m_uiBevel)
		{
			// get the bevel
			CIssImageSliced oSlice;
			oSlice.Initialize(m_hWnd, m_hInst, m_uiBevel);
			oSlice.SetSize(rc.GetWidth(), rc.GetHeight());
			oSlice.DrawImage(m_gdiBackground, 0,0);
		}
		else
			// just alpha fill for now
			AlphaFillRect(m_gdiBackground, rc.Get(), RGB(40,40,40), 200);

		// draw the selected background
		m_gdiBackSelected.Create(m_gdiBackground, rc.Get(), TRUE, TRUE, FALSE);
		AlphaFillRect(m_gdiBackSelected, rc.Get(), m_crSelector, 100);
	}

	if(m_gdiMem.GetWidth() != WIDTH(lpItem->rcItem) || m_gdiMem.GetHeight() != HEIGHT(lpItem->rcItem))
		m_gdiMem.Create(lpItem->hDC, lpItem->rcItem, FALSE, FALSE);

	CIssRect rcZero(lpItem->rcItem);
	rcZero.ZeroBase();


	if(lpItem->itemState & ODS_SELECTED)
	{
		// copy the selected background over
		BitBlt(m_gdiMem.GetDC(),
			0,0,
			WIDTH(lpItem->rcItem), HEIGHT(lpItem->rcItem),
			m_gdiBackSelected.GetDC(),
			lpItem->rcItem.left,lpItem->rcItem.top,
			SRCCOPY);
	}
	else
	{
		// copy the background over
		BitBlt(m_gdiMem.GetDC(),
			0,0,
			WIDTH(lpItem->rcItem), HEIGHT(lpItem->rcItem),
			m_gdiBackground.GetDC(),
			lpItem->rcItem.left,lpItem->rcItem.top,
			SRCCOPY);
	}

	if(lpItem->itemData)
	{
		rcZero.left += (MENU_CHECK + MENU_INDENT);
		rcZero.right -= MENU_INDENT;
		DrawText(m_gdiMem.GetDC(), (UINT)lpItem->itemData, lpItem->itemState, rcZero.Get());
	}
	else
	{
		MENUITEMINFO sMenuItem = {0};
		if(FindMenuItem(lpItem->itemID, &sMenuItem) && sMenuItem.fType & MFT_SEPARATOR)
		{
			DrawMenuSeparator(rcZero);
		}
	}

	// draw it all to the screen
	BitBlt(lpItem->hDC,
		lpItem->rcItem.left,lpItem->rcItem.top,
		WIDTH(lpItem->rcItem), HEIGHT(lpItem->rcItem),
		m_gdiMem.GetDC(),
		0,0,
		SRCCOPY);
	return TRUE;
}

BOOL CIssMenuHandler::ProcDefault(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{	
	if(g_pMenu)
		return g_pMenu->ProcMenu(hWnd, uiMessage, wParam, lParam);
	else
		return FALSE;
}

BOOL CIssMenuHandler::ProcMenu(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	//DebugOut(_T("CIssMenuHandler::ProcMenu 0x%X,%d,%d,%d"), hWnd, uiMessage, wParam, lParam);

	// just default it for now
	switch(uiMessage)
	{
	case WM_CREATE:
	case WM_INITDIALOG:
		//DebugOut(_T("CIssMenuHandler::ProcMenu - InitDialog"));
		break;
	case WM_SIZE:
		//DebugOut(_T("CIssMenuHandler::ProcMenu - OnSize"));
		break;
	case WM_ERASEBKGND:
		//DebugOut(_T("CIssMenuHandler::ProcMenu - EraseBkgnd"));
		return TRUE;
	}
	return CallWindowProc(m_wndProcMenu,hWnd,uiMessage,wParam,lParam );
}

BOOL CIssMenuHandler::AppendMenu(HMENU hMenu, UINT uFlags, UINT uIDNewItem, UINT uiText)
{
	if(!m_hWnd)
		return FALSE;

	uFlags	|= MF_OWNERDRAW;	// we owner draw everything

	// add it to our list
	TypeMenuEntry* sEntry = new TypeMenuEntry;
	ZeroMemory(sEntry, sizeof(TypeMenuEntry));

	sEntry->hMenu	= hMenu;
	sEntry->uItemID	= uIDNewItem;
	
	// save so we can get info out of the WM_MEASUREITEM
	m_arrItems.AddElement(sEntry);

	::AppendMenu(hMenu, uFlags, uIDNewItem, (TCHAR*)uiText);

	return TRUE;
}

BOOL CIssMenuHandler::PopupMenu(HMENU hMenu, UINT uFlags, int x, int y)
{
	if(!m_hWnd)
		return FALSE;

	m_bIsMenuUp = TRUE;
	BOOL bCmd = TrackPopupMenu(hMenu, uFlags, x, y, NULL, m_hWnd, NULL);

	Destroy();	// clear out all memory cause we don't need it anymore

	return bCmd;
}

BOOL CIssMenuHandler::FindMenuItem(UINT uItemID, LPMENUITEMINFO lpMenuItem)
{
	for(int i=0; i<m_arrItems.GetSize(); i++)
	{
		TypeMenuEntry* sItem = m_arrItems[i];
		if(sItem && sItem->uItemID == uItemID)
		{
			lpMenuItem->cbSize  = sizeof(LPMENUITEMINFO);
			lpMenuItem->fMask	= MIIM_TYPE|MIIM_STATE|MIIM_DATA;
			if(GetMenuItemInfo(sItem->hMenu, uItemID, FALSE, lpMenuItem))
				return TRUE;
			else
				return FALSE;
		}
	}
	return FALSE;
}

void CIssMenuHandler::DrawText(HDC dc, UINT uiText, UINT uiItemState, RECT& rcLocation)
{
	RECT rcTemp = rcLocation;

	TCHAR szText[STRING_MAX] = _T("");

	if(uiText)
		m_oStr->StringCopy(szText, uiText, STRING_MAX, m_hInst);

	if(m_bDropShadowText)
	{
		rcTemp.bottom	+= 2*GetSystemMetrics(SM_CXBORDER);
		rcTemp.right	+= 2*GetSystemMetrics(SM_CXBORDER);
        ::DrawText(dc, szText, rcTemp, DT_LEFT|DT_VCENTER, m_hFontText, RGB(50,50,50));
		rcTemp.bottom	-= 2*GetSystemMetrics(SM_CXBORDER);
		rcTemp.right	-= 2*GetSystemMetrics(SM_CXBORDER);
	}

    ::DrawText(dc, szText, rcTemp, DT_LEFT|DT_VCENTER, m_hFontText, (uiItemState&ODS_GRAYED?RGB(100,100,100):m_crText));
}

void CIssMenuHandler::DrawMenuSeparator(CIssRect& rc)
{
#define LINE_COLOR_2		RGB(220,220,220)

	HPEN hPenText	= CIssGDIEx::CreatePen(m_crText);
	HPEN hPenGray	= CIssGDIEx::CreatePen(LINE_COLOR_2);

	// draw the gray line
	HPEN hOldPen = (HPEN)SelectObject(m_gdiMem.GetDC(), hPenGray);
	POINT pt[2];
	pt[0].x = rc.left;
	pt[0].y = rc.top   + rc.GetHeight()/2 + 1;
	pt[1].x = rc.right;
	pt[1].y = pt[0].y;
	Polyline(m_gdiMem.GetDC(), pt, 2);

	// draw the black line
	SelectObject(m_gdiMem.GetDC(), hPenText);
	pt[0].x = rc.left;
	pt[0].y = rc.top   + rc.GetHeight()/2;
	pt[1].x = rc.right;
	pt[1].y = pt[0].y;
	Polyline(m_gdiMem.GetDC(), pt, 2);

	SelectObject(m_gdiMem.GetDC(), hOldPen);
	DeleteObject((HPEN)hPenGray);
	DeleteObject((HPEN)hPenText);
}