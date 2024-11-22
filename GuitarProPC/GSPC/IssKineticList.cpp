#include "IssKineticList.h"
#include "IssGDIDraw.h"
#include "resource.h"
#include "IssCommon.h"
#include "IssGDIEx.h"
#include "math.h"
#include "issdebug.h"
#include "issRect.h"
#include "IssString.h"

#define MAX_SPEED			    1000.0
#define MIN_ACTIVATE		    0.1
#define MIN_SPEED			    0.01
#define IDT_SROLL_TIMER		    3100
#define IDT_SELECT_TIMER        3101
#define UPDATE_TIME			    0
#define	FRICTION			    30.0
#define SPEED_MULT			    50.0
#define REFRESH_RATE		    22
#define INC_ACCEL_FACTOR	    1.2f


#ifdef UNDER_CE

#define ITEMHEIGHT_Text         (GetSysMets(SM_CXICON)*29/32)
#define TEXT_Height             (GetSysMets(SM_CXICON)*15/32)
#define ITEMHEIGHT_Separator    (IsVGA()?2:1)
#define INDENT                  (GetSysMets(SM_CXICON)/4)
#define SCROLLBAR_Height        (GetSysMets(SM_CXICON)*5/7)
#define MAX_SelectDistance      (IsVGA()?16:8)

#else
//eyed balled values on Windows 7 ... nice compromise of size vs info on screen
#define ITEMHEIGHT_Text         36
#define TEXT_Height             22
#define ITEMHEIGHT_Separator    2
#define INDENT                  16
#define SCROLLBAR_Height        46
#define MAX_SelectDistance      16

#endif

#define MAX_SelectItemTime      700
#define UND					    -1	// Not defined

CIssGDIEx* CIssKineticList::m_gdiList = NULL;
CIssGDIEx* CIssKineticList::m_gdiSeparator = NULL;
CIssGDIEx* CIssKineticList::m_gdiImageArray = NULL;

TypeItems::TypeItems()
:lpItem(NULL)
,szText(NULL)
,eType(ITEM_Unknown)
,iHeight(0)
,iHeightSel(0)
,uiMenuID(0)
,dwFlags(0)
,lParam(0)
{}

TypeItems::~TypeItems()
{
    CIssString* oStr = CIssString::Instance();
    oStr->Delete(&szText);
    if(lpItem)
    {
        DBG_OUT((_T("TypeItems::~TypeItems() - lpItem is defined.  This is a memory leak")));
        delete lpItem;       // must be self deleting
    }
}



CIssKineticList::CIssKineticList(void)
:m_iScrollPos(0)
,m_dbSpeed(0.0) 
,m_dbFriction(FRICTION)
,m_eScroll(SCROLL_Stopped)
,m_eScrollDir(DIR_Down)
,m_iLastReDrawInc(0)
,m_hWndNotif(NULL)
,m_hWndMyParent(NULL)
,m_hInstance(NULL)
,m_iCurrentHeight(UND)
,m_hFontText(NULL)
,m_bDrawSelector(TRUE)
,m_iCurSelector(0)
,m_dwFlags(0)
,m_uiCategory(0)
,m_uiBackground(0)
,m_uiArray(0)
,m_uiSelected(0)
,m_crText(RGB(255,255,255))
,m_crSelected(RGB(255,255,255))
,m_crHighlight(RGB(255,255,255))
,m_pDrawFunc(NULL)
,m_pDeleteItemFunc(NULL)
,m_lpClass(NULL)
,m_dwTickLastUsed(0)
,m_bFlagsSet(FALSE)
,m_eMouse(GRAB_None)
{
	if(!m_gdiList)
		m_gdiList = new CIssGDIEx;
	if(!m_gdiSeparator)
		m_gdiSeparator = new CIssGDIEx;
    if(!m_gdiImageArray)
        m_gdiImageArray = new CIssGDIEx;

    m_hFontText = CIssGDIEx::CreateFont(TEXT_Height, FW_BOLD, TRUE);
}

CIssKineticList::~CIssKineticList(void)
{
    DestroyList();
    CIssGDIEx::DeleteFont(m_hFontText);
}

void CIssKineticList::DeleteAllContent()
{
	if(m_gdiList)
		delete m_gdiList;
	if(m_gdiSeparator)
		delete m_gdiSeparator;
    if(m_gdiImageArray)
        delete m_gdiImageArray;
	m_gdiList = NULL;
	m_gdiSeparator = NULL;
    m_gdiImageArray = NULL;
}

void CIssKineticList::DestroyList()
{
    ResetContent();
    //m_gdiSeparator.Destroy();
    //m_gdiList.Destroy();
    //m_gdiImageArray->Destroy();
    m_imgCategory.Destroy();
    m_imgSelected.Destroy();
}

void CIssKineticList::Reset()
{
    m_iScrollPos        = 0;
    m_dbSpeed           = 0.00;
    m_dbFriction        = FRICTION;
    m_eScroll           = SCROLL_Stopped;
    m_eScrollDir        = DIR_Down;
    m_iLastReDrawInc    = 0;
    m_iCurrentHeight    = UND;
}

void CIssKineticList::CheckFlags(DWORD dwFlags)
{
    if(m_bFlagsSet)
        return;

    if(dwFlags & FLAG_Check || dwFlags & FLAG_Radio)
        m_bFlagsSet = TRUE;
}

HRESULT CIssKineticList::SetItemHeights(int iHeight, int iHeightSelected)
{
    for(int i=0; i<m_arrItems.GetSize(); i++)
    {
        TypeItems* sItem = m_arrItems[i];
        if(!sItem || sItem->eType == ITEM_Category || sItem->eType == ITEM_Separator)
            continue;

        sItem->iHeight      = iHeight;
        sItem->iHeightSel   = iHeightSelected;
    }
    return S_OK;
}

HRESULT CIssKineticList::SetColors(COLORREF crText, COLORREF crHighlight, COLORREF crSelected)
{
    m_crText        = crText;
    m_crHighlight   = crHighlight;
    m_crSelected    = crSelected;
    return S_OK;
}

HRESULT CIssKineticList::SetCustomDrawFunc(DRAWITEM pDrawFunc, LPVOID lpClass)
{
    m_pDrawFunc     = pDrawFunc;
    m_lpClass       = lpClass;
    return S_OK;
}

HRESULT CIssKineticList::SetDeleteItemFunc(DELETEITEM pDeleteItemFunc)
{
	m_pDeleteItemFunc = pDeleteItemFunc;
	return S_OK;
}

HRESULT CIssKineticList::SetCategory(UINT uiCategory)
{
    m_uiCategory = uiCategory;
    m_imgCategory.Destroy();
    return S_OK;
}

HRESULT CIssKineticList::SetSelected(UINT uiSelected)
{
    m_uiSelected = uiSelected;
    m_imgSelected.Destroy();
    return S_OK;
}

HRESULT CIssKineticList::SetImageArray(UINT uiImageArray)
{
    m_uiArray = uiImageArray;
    m_gdiImageArray->Destroy();
    m_imgCategory.Destroy();
    return S_OK;
}

HRESULT CIssKineticList::PreloadImages(HWND hWnd, HINSTANCE hInstance)
{
    HRESULT hr = S_OK;
    if(m_uiArray != 0 && m_gdiImageArray->GetDC() == NULL)
    {
        hr = m_gdiImageArray->LoadImage(m_uiArray, hWnd, hInstance, TRUE);
        CHR(hr, _T("m_gdiImageArray->LoadImage"));
    }
    if(m_uiCategory != 0 && !m_imgCategory.IsLoaded())
    {
        hr = m_imgCategory.Initialize(hWnd, hInstance, m_uiCategory);
        CHR(hr, _T("m_imgCategory.Initialize"));
    }
    if(m_uiSelected != 0 && !m_imgSelected.IsLoaded())
    {
        hr = m_imgSelected.Initialize(hWnd, hInstance, m_uiSelected);
        CHR(hr, _T("m_imgSelected.Initialize"));
    }

Error:
    return hr;
}

HRESULT CIssKineticList::Initialize(HWND hWndNotif,
                                    HWND hWndParent, 
                                    HINSTANCE hInst,
                                    DWORD dwFlags, 
                                    BOOL bDontResetFunctions)
{
    HRESULT hr = S_OK;

	m_ptLastButtonUp.x	= 0;
	m_ptLastButtonUp.y	= 0;

    if(!bDontResetFunctions)
    {
        ResetContent();
        m_pDrawFunc = NULL;
        m_lpClass   = NULL;
	    m_pDeleteItemFunc = NULL;

        // save some variables
        m_iCurSelector     = 0;
        m_iScrollPos       = 0;
        m_dbSpeed          = 0.0; 
        m_dbFriction       = FRICTION;
        m_eScroll          = SCROLL_Stopped;
        m_eScrollDir       = DIR_Down;
    }

    m_iCurrentHeight	= -1; // Height needs to be re-calculated
    m_hWndNotif         = hWndNotif;
    m_hWndMyParent      = hWndParent;
    m_hInstance         = hInst;
    m_dwFlags           = dwFlags;
    m_dwTickLastKeyDown = GetTickCount();

    if(m_dwFlags&OPTION_AlwaysShowSelector)
        m_bDrawSelector = TRUE;

    // some flags may have changed so we have to realign everything
    if(WIDTH(m_rcLoc) != 0 && HEIGHT(m_rcLoc) != 0)
        OnSize(m_rcLoc.left, m_rcLoc.top, WIDTH(m_rcLoc), HEIGHT(m_rcLoc));

    return hr;
}

HRESULT CIssKineticList::ResetContent()
{
    m_bFlagsSet = FALSE;

    HRESULT hr = S_OK;
    for(int i=0; i<m_arrItems.GetSize(); i++)
    {
        hr = DeleteItem(i);
        CHR(hr, _T("DeleteItem failed"));
    }
    hr = m_arrItems.RemoveAll();
    CHR(hr, _T("m_arrItems.RemoveAll failed"));

    // reset the selector cause there is nothing there now
    m_iCurSelector     = 0;
    m_iScrollPos       = 0;
    m_dbSpeed          = 0.0; 
    m_dbFriction       = FRICTION;
    m_eScroll          = SCROLL_Stopped;
    m_eScrollDir       = DIR_Down;
    m_iCurrentHeight   = UND;
Error:
    return hr;
}

HRESULT CIssKineticList::AddSeparator()
{
    HRESULT hr = S_OK;
    TypeItems* sItem = new TypeItems;
    CPHR(sItem, _T("failed to create sItem"));
    sItem->eType    = ITEM_Separator;
    sItem->iHeight  = ITEMHEIGHT_Separator;
    hr = m_arrItems.AddElement(sItem);
    CHR(hr, _T("m_arrItems.AddElement failed"));
    m_iCurrentHeight = -1;
Error:
    return hr;
}

HRESULT CIssKineticList::AddCategory(TCHAR* szCategory)
{
    HRESULT hr = S_OK;
    CIssString* oStr = CIssString::Instance();

    CBARG(!oStr->IsEmpty(szCategory), _T("szCategory is empty"));

    TypeItems* sItem = new TypeItems;
    CPHR(sItem, _T("failed to create sItem"));
    sItem->eType    = ITEM_Category;
    sItem->iHeight  = ITEMHEIGHT_Text;
    sItem->szText   = oStr->CreateAndCopy(szCategory);
    CBARG(!oStr->IsEmpty(sItem->szText), _T("sItem->szText is empty"));
    hr = m_arrItems.AddElement(sItem);
    CHR(hr, _T("m_arrItems.AddElement failed"));
    m_iCurrentHeight = -1;
Error:
    return hr;
}

HRESULT CIssKineticList::AddCategory(UINT uiCategory, HINSTANCE hInst)
{
    HRESULT hr = S_OK;
    CIssString* oStr = CIssString::Instance();
    CBARG(uiCategory&&hInst, _T("bad values for AddCategory"));
    TCHAR szText[STRING_MAX] = _T("");
    oStr->StringCopy(szText, uiCategory, STRING_MAX, hInst);
    CBARG(!oStr->IsEmpty(szText), _T("szText is empty"));
    hr = AddCategory(szText);
    CHR(hr, _T("AddCategory failed"));
    m_iCurrentHeight = -1;
Error:
    return hr;
}

HRESULT CIssKineticList::AddItem(LPVOID lpItem, UINT uiMenu, DWORD dwFlags, LPARAM lParam)
{
    HRESULT hr = S_OK;
    CBARG(lpItem, _T("bad values for AddItem"));
    CBARG(m_pDeleteItemFunc, _T("CIssKineticList::AddItem failed! m_pDeleteItemFunc not defined"));
    TypeItems* sItem = new TypeItems;
    CPHR(sItem, _T("failed to create sItem"));
    sItem->eType    = ITEM_Unknown;
    sItem->iHeight  = ITEMHEIGHT_Text;
    sItem->iHeightSel= ITEMHEIGHT_Text;
    sItem->lpItem   = lpItem;   // we now own this
    sItem->uiMenuID = uiMenu;
    sItem->dwFlags  = dwFlags;
    sItem->lParam   = lParam;
    hr = m_arrItems.AddElement(sItem);
    CHR(hr, _T("m_arrItems.AddElement failed"));
    CheckFlags(dwFlags);
    m_iCurrentHeight = -1;
Error:
    return hr;
}

HRESULT CIssKineticList::AddItem(TCHAR* szItem, UINT uiMenu, DWORD dwFlags, LPARAM lParam, BOOL bAddSorted)
{
    HRESULT hr = S_OK;
    CIssString* oStr = CIssString::Instance();
    CBARG(!oStr->IsEmpty(szItem), _T("szItem is empty"));

    TypeItems* sItem = new TypeItems;
    CPHR(sItem, _T("failed to create sItem"));
    sItem->eType    = ITEM_Text;
    sItem->iHeight  = ITEMHEIGHT_Text;
    sItem->iHeightSel= ITEMHEIGHT_Text;
    sItem->szText   = oStr->CreateAndCopy(szItem);
    sItem->uiMenuID = uiMenu;
    sItem->dwFlags  = dwFlags;
    sItem->lParam   = lParam;
    CBARG(!oStr->IsEmpty(sItem->szText), _T("sItem->szText is empty"));
    if(bAddSorted)
        hr = m_arrItems.AddSortedElement(sItem, CompareNameItems);
    else
        hr = m_arrItems.AddElement(sItem);
    CHR(hr, _T("m_arrItems.AddElement failed"));
    CheckFlags(dwFlags);
    m_iCurrentHeight = -1;
Error:
    return hr;
}

HRESULT CIssKineticList::AddItem(UINT uiItem, HINSTANCE hInst, UINT uiMenu, DWORD dwFlags, LPARAM lParam, BOOL bAddSorted)
{
    HRESULT hr = S_OK;
    CIssString* oStr = CIssString::Instance();
    CBARG(uiItem&&hInst, _T("bad values for AddItem"));
    TCHAR szText[STRING_MAX] = _T("");
    oStr->StringCopy(szText, uiItem, STRING_MAX, hInst);
    CBARG(!oStr->IsEmpty(szText), _T("szText is empty"));
    hr = AddItem(szText, uiMenu, dwFlags, lParam, bAddSorted);
    CHR(hr, _T("AddItem failed"));
    CheckFlags(dwFlags);
    m_iCurrentHeight = -1;
Error:
    return hr;
}

HRESULT CIssKineticList::SetItemHeight(int iIndex, int iHeight, int iHeightSelected)
{
    HRESULT hr = S_OK;
    TypeItems* sItem = m_arrItems[iIndex];
    CPHR(sItem, _T("sItem does not exits"));

    CBARG(IsItemSelectable(iIndex), _T(""));

    sItem->iHeight      = iHeight;
    sItem->iHeightSel   = iHeightSelected;
    m_iCurrentHeight = -1;
Error:
    return hr;
}

HRESULT CIssKineticList::RemoveItem(int iIndex)
{
    HRESULT hr = S_OK;
    hr = DeleteItem(iIndex);
    CHR(hr, _T("DeleteItem failed"));
    hr = m_arrItems.RemoveElementAt(iIndex);
    CHR(hr, _T("m_arrItems.RemoveElementAt failed"));

    int iNewIndex = GetNextSelectableItem((iIndex==m_arrItems.GetSize()?iIndex-1:iIndex));
    if(iNewIndex == UND)
        m_iCurSelector = 0;
    else
        m_iCurSelector = iNewIndex;
    m_iCurrentHeight   = UND;
    ShowSelectedItem(TRUE);
Error:
    return hr;
}

HRESULT CIssKineticList::DeleteItem(int iIndex)
{
    HRESULT hr = S_OK;
    TypeItems* sItem = m_arrItems[iIndex];
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

HRESULT CIssKineticList::SetSelectedItemIndex(int iIndex, BOOL bDisplaySelected)
{
    HRESULT hr = S_OK;

    if(m_arrItems.GetSize() == 0)
        return E_FAIL;

    /*TypeItems* sItem = m_arrItems[iIndex];
    CPHR(sItem, _T("iIndex not found in m_arrItems"));

    if(!IsItemSelectable(sItem))
    {
        // loop through and find a selectable item to highlight
        int iNewIndex = iIndex;
        do
        {
            iNewIndex++;
            if(iNewIndex >= m_arrItems.GetSize())
                iNewIndex = 0;

            if(iNewIndex == iIndex)
                return E_FAIL;

            sItem = m_arrItems[iNewIndex];
            CPHR(sItem, _T("iIndex not found in m_arrItems"));

            if(IsItemSelectable(sItem))
            {
                iIndex = iNewIndex;
                break;
            }
        }while(TRUE);
    }*/
    int iNewItem = GetNextSelectableItem(iIndex, FALSE);
    //CBARG(iNewItem != UND, _T(""));
    if(iNewItem == UND)
    {
        hr = E_FAIL;
        goto Error;
    }

    m_iCurSelector     = iNewItem;
    m_bDrawSelector    = bDisplaySelected;

Error:
    return hr;
}

BOOL CIssKineticList::OnSize(int iX, int iY, int iWidth, int iHeight)
{
    //m_gdiSeparator.Destroy();
    //m_imgCategory.Destroy();
    //m_imgSelected.Destroy();

	m_ptLastButtonUp.x	= 0;
	m_ptLastButtonUp.y	= 0;
	m_iCurrentHeight	= -1;

    m_rcLoc.left   = iX;
    m_rcLoc.top    = iY;
    m_rcLoc.right  = iX + iWidth;
    m_rcLoc.bottom = iY + iHeight;

    int iScrollWidth = (m_gdiImageArray->GetHeight()?m_gdiImageArray->GetHeight():INDENT);
    if(m_dwFlags&OPTION_DrawScrollArrows || m_dwFlags&OPTION_DrawScrollBar)
    {

        m_rcScrollUp.left       = m_rcLoc.right - iScrollWidth;
        m_rcScrollUp.right      = m_rcLoc.right;
        m_rcScrollUp.top        = m_rcLoc.top;
        m_rcScrollUp.bottom     = m_rcLoc.top + iScrollWidth;
        m_rcScrollDown          = m_rcScrollUp;
        m_rcScrollDown.top      = m_rcLoc.bottom - iScrollWidth;
        m_rcScrollDown.bottom   = m_rcLoc.bottom;
        m_rcScrollArea          = m_rcScrollUp;
        m_rcScrollArea.top      = m_rcScrollUp.bottom;
        m_rcScrollArea.bottom   = m_rcScrollDown.top;
    }
    else
    {
        SetRect(&m_rcScrollDown, 0,0,0,0);
        SetRect(&m_rcScrollUp, 0,0,0,0);
    }

    m_rcList;
    m_rcList.left    = m_rcLoc.left;
    m_rcList.right   = m_rcLoc.left + WIDTH(m_rcLoc) - ((m_dwFlags&OPTION_DrawScrollArrows || m_dwFlags&OPTION_DrawScrollBar)?iScrollWidth:0);
    m_rcList.top     = m_rcLoc.top;
    m_rcList.bottom  = m_rcLoc.bottom;
    return TRUE;
}

BOOL CIssKineticList::OnDraw(CIssGDIEx& gdiMem, RECT& rcClient, RECT& rcClip)
{
    if(!IsRectInRect(rcClip, m_rcLoc))
        return TRUE;

    DrawEntries(gdiMem, rcClip);
    DrawScrollArrows(gdiMem, rcClip);
    DrawScrollBar(gdiMem, rcClip);
    return TRUE;
}

BOOL CIssKineticList::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
    int iNewSelected = UND;
    //int iRepeatCount = LOWORD(lParam);

    if(m_iCurSelector == UND || m_bDrawSelector == FALSE)
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
        case VK_UP:
            iNewSelected = GetNextItemIndex(m_iCurSelector, TRUE);
            break;
        case VK_DOWN:
            iNewSelected = GetNextItemIndex(m_iCurSelector);
            break;
        case VK_LEFT:
            iNewSelected = GetNextPageItemIndex(TRUE);
            break;
        case VK_RIGHT:
            iNewSelected = GetNextPageItemIndex(FALSE);
            break;
        }


    }

    if(iNewSelected != m_iCurSelector && iNewSelected != UND)
    {
        m_iCurSelector = iNewSelected;

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

BOOL CIssKineticList::OnLButtonDown(POINT& pt)
{
    if(!PtInRect(&m_rcLoc, pt))
    {
        StopScrolling();
        m_eMouse  = GRAB_None;
        return FALSE;
    }

    m_eMouse = GRAB_None;

	m_ptLastButtonUp.x	= 0;
	m_ptLastButtonUp.y	= 0;
    m_ptYStartGrab      = pt;
    m_dwTickCountGrab   = GetTickCount();

    /*if(GetMovementState() == SCROLL_Moving && (int)abs((int)GetScrollSpeed()) > 3)
    {
        // stop scrolling
        StopScrolling();

        // this means we're scrolling
        if(PtInRect(&m_rcList, pt))
        {
            int iSelected = GetItemIndexAtPos(pt.y - m_rcList.top);
            if(iSelected != -1 && IsItemSelectable(iSelected))
            {
                m_iCurSelector = iSelected;
                ReDrawList();
            }
        }
    }
    else*/ if(PtInRect(&m_rcList, pt))
    {
        if(GetMovementState() == SCROLL_Moving && (int)abs((int)GetScrollSpeed()) > 3)
        {
            // stop scrolling
            StopScrolling();

            int iSelected = GetItemIndexAtPos(pt.y - m_rcList.top);
            if(iSelected != -1 && IsItemSelectable(iSelected))
            {
                m_iCurSelector = iSelected;
                ReDrawList();
            }
            m_bWasScrolling = TRUE;
        }
        else
            m_bWasScrolling = FALSE;

        // stop scrolling
        //StopScrolling();

        /*int iSelected = GetItemIndexAtPos(pt.y - m_rcList.top);
        if(iSelected != -1 && IsItemSelectable(iSelected))
        {
        m_iCurSelector = iSelected;
        ReDrawList();
        }*/
        if(PtInRect(&GetOwnerFrame(), pt) == FALSE)
            return FALSE;

        m_eMouse = GRAB_List;        

        m_iScrollYStart = pt.y;
        m_iScrollYPos	= pt.y;
        m_dbSpeed		= 0.0;
        m_dwStartTime	= GetTickCount();
        KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
        KillTimer(GetOwnerWindow(), IDT_SELECT_TIMER);
        //DBG_OUT((_T("CIssKineticList::OnLButtonDown - Calling SetTimer")));
        SetTimer(GetOwnerWindow(), IDT_SELECT_TIMER, 600, NULL);
        
    }
    else if(PtInRect(&m_rcScrollDown, pt))
    {
        m_eMouse = GRAB_ScrollArrowDown;
        return TRUE;
    }
    else if(PtInRect(&m_rcScrollUp, pt))
    {
        m_eMouse = GRAB_ScrollArrowUp;
        return TRUE;
    }
    else if(PtInRect(&m_rcScrollBar, pt))
    {
        m_eMouse = GRAB_Scrollbar;
        return TRUE;
    }
    else if(PtInRect(&m_rcScrollArea, pt))
    {
        int iScrollPosition = (pt.y-m_rcScrollUp.bottom)*m_iCurrentHeight/(m_rcScrollDown.top - m_rcScrollUp.bottom);

        ScrollTo(iScrollPosition, TRUE);
        ReDrawList();
        m_eMouse = GRAB_Scrollbar;
        return TRUE;
    }

	return TRUE;
}

BOOL CIssKineticList::OnLButtonUp(POINT& pt)
{
    BOOL bRet = FALSE;

    if(m_eMouse == GRAB_None)
        return FALSE;

    KillTimer(GetOwnerWindow(), IDT_SELECT_TIMER);

    if(m_eMouse == GRAB_List)
    {
        bRet = OnLButtonUpList(pt);

        // see if we should be selecting this item
        if((GetTickCount()-m_dwTickCountGrab) < MAX_SelectItemTime &&
            (abs(m_ptYStartGrab.y-pt.y) < MAX_SelectDistance) && 
            (abs(m_ptYStartGrab.x-pt.x) < MAX_SelectDistance) && 
        //    IsItemSelectable(m_iCurSelector)  &&  //selector may not be at the current location AS
            !m_bWasScrolling)
        {

			m_ptLastButtonUp = pt;

            int iSelected = GetItemIndexAtPos(pt.y - m_rcList.top);// AS
            if(iSelected != -1 && IsItemSelectable(iSelected))//AS
            {
                m_iCurSelector = iSelected;
                TypeItems* sItem = GetSelectedItem();
                if(sItem)
                    GrabItem(m_ptYStartGrab, sItem);
                ReDrawList();
                SelecteItem(m_iCurSelector);
            }

        }
        //DBG_OUT((_T("CIssKineticList::OnLButtonUp - EnsureWithinBounds")));
        //EnsureWithinBounds();
		bRet = TRUE; // we always return TRUE
    }
    else if(m_eMouse == GRAB_ScrollArrowUp && PtInRect(&m_rcScrollUp, pt))
    {
        int iScroll = GetScrollPos();
        // no need to scroll we're at the beginning
        if(iScroll == 0)
            return TRUE;

        iScroll -= HEIGHT(m_rcList)/2;
        if(iScroll < 0)
            iScroll = 0;
        ScrollTo(iScroll);  
        bRet = TRUE;
    }
    else if(m_eMouse == GRAB_ScrollArrowDown && PtInRect(&m_rcScrollDown, pt))
    {
        int iMaxScroll = GetEndOfListYPos();
        int iScroll = GetScrollPos();
        // no need to scroll we're at the end
        if(iScroll == iMaxScroll)
            return TRUE;

        iScroll += HEIGHT(m_rcList)/2;
        if(iScroll > iMaxScroll)
            iScroll = iMaxScroll;
        ScrollTo(iScroll);
        bRet = TRUE;
    }
    else if(m_eMouse == GRAB_Scrollbar || m_eMouse == GRAB_ScrollArrowDown ||
        m_eMouse == GRAB_ScrollArrowUp)
    {
        bRet = TRUE; //added Nov 23 since I now actually check the return for Calc Pro PC
    }
    m_bWasScrolling = FALSE;
    m_eMouse = GRAB_None;//added Nov 3 (Adrian) ... looks right to me ... added to fix PC 'sticky' list
    return bRet;
}

BOOL CIssKineticList::OnLButtonUpList(POINT& pt)
{
    if(PtInRect(&GetOwnerFrame(), pt) == FALSE)
    {
        EnsureWithinBounds();
        return FALSE;
    }

    if(m_eScroll == SCROLL_Scrolling)
    {
        m_dwStopTime = GetTickCount();
        int iPos = pt.y;

        //m_dbSpeed = (pt.y - m_iScrollYStart) * SPEED_MULT / (m_dwStopTime - m_dwStartTime);

        int iTicks = m_dwStopTime - m_dwStartTime;

        iTicks > 0 ?  iTicks = iTicks : iTicks = 1;
        m_dbSpeed = (pt.y - m_iScrollYStart) * SPEED_MULT;
        m_dbSpeed /= (float)iTicks;

        if(fabs(m_dbSpeed) > MIN_ACTIVATE)
        {
            if(m_dbSpeed > MAX_SPEED)
                m_dbSpeed = MAX_SPEED;
            else if(m_dbSpeed < MAX_SPEED * -1.0)
                m_dbSpeed = MAX_SPEED*-1.0;

            m_eScroll = SCROLL_Moving;
            m_dbFriction = FRICTION;
            //DebugOut(_T("CIssKineticList::OnLButtonUp() SCROLL_Scrolling - SCROLL_Moving %.2f "), m_dbSpeed);
            SetTimer(GetOwnerWindow(), IDT_SROLL_TIMER, REFRESH_RATE, NULL);
        }
        else
        {
            m_eScroll = SCROLL_Stopped;
            m_dbSpeed = 0.0;
            //ReDrawList(0);

        }
        return TRUE;
    }
    else if(m_eScroll == SCROLL_Moving)
    {
        m_eScroll = SCROLL_Stopped;
        m_dbSpeed = 0.0;

        return TRUE;
    }
    else if(m_eScroll == SCROLL_Stopped)
    {
        return TRUE;
        //SeleteItem(pt.y + m_iScrollPos);
    }
    else if(m_eScroll == SCROLL_To)	
    {
        m_eScroll = SCROLL_Stopped;
        if(m_dwFlags & OPTION_Bounce)
            EnsureWithinBounds();
    }

    return FALSE;	
}

BOOL CIssKineticList::OnMouseMove(POINT& pt)
{
    /*switch(m_eMouse)
    {
    case GRAB_None:
        //DebugOut(_T("GRAB_None"));    
        break;
    case GRAB_List:
        //DebugOut(_T("GRAB_List"));
    	break;
    case GRAB_Scrollbar:
        //DebugOut(_T("GRAB_Scrollbar"));
        break;
    case GRAB_ScrollArrowUp:
        //DebugOut(_T("GRAB_ScrollArrowUp"));
        break;
    case GRAB_ScrollArrowDown:
        //DebugOut(_T("GRAB_ScrollArrowDown"));
        break;
    
    }*/


    if(m_eMouse == GRAB_None)
        return FALSE;
    else if(m_eMouse == GRAB_List)
    {   //
        if(pt.x != m_ptYStartGrab.x || pt.y != m_ptYStartGrab.y)  //added march 2nd - AS   
            return OnMouseMoveList(pt);
    }
    else if(m_eMouse == GRAB_Scrollbar)
    {
        int iScrollPosition = 0;
        if(pt.y < m_rcScrollUp.bottom)
            iScrollPosition = 0;
        else if(pt.y > m_rcScrollDown.top)
            iScrollPosition = m_iCurrentHeight;
        else
            iScrollPosition = (pt.y-m_rcScrollUp.bottom)*(m_iCurrentHeight)/(m_rcScrollDown.top - m_rcScrollUp.bottom);

        ScrollTo(iScrollPosition, TRUE);
        ReDrawList();
    }
    return TRUE;
}

BOOL CIssKineticList::OnMouseMoveList(POINT& pt)
{
	if(m_eScroll != SCROLL_Scrolling)
	{
        int iYDiff = IsVGA()?16:8;
        if( abs(pt.y-m_iScrollYStart) > iYDiff)
        {
			m_eScroll = SCROLL_Scrolling; //since we're actually dragging
            //DBG_OUT((_T("CIssKineticList::OnMouseMoveList - killing SetTimer m_iScrollYStart: %d, pt.y: %d, iYDiff: %d"), m_iScrollYStart, pt.y, iYDiff));
            KillTimer(GetOwnerWindow(), IDT_SELECT_TIMER);
        }
	}	

	ReDrawList(pt.y - m_iScrollYPos);

	if(pt.y < m_iScrollYPos)
	{
		if(m_eScrollDir != DIR_Down)
		{
			m_dwStartTime = GetTickCount();
			m_iScrollYStart = pt.y;
		}
		//DebugOut(_T("setting m_eScrollDir = DIR_Down"));
		m_eScrollDir = DIR_Down;
	}
	else if(pt.y > m_iScrollYPos)
	{
		if(m_eScrollDir != DIR_Up)
		{
			m_dwStartTime = GetTickCount();
			m_iScrollYStart = pt.y;
		}
		//DebugOut(_T("setting m_eScrollDir = DIR_Up"));
		m_eScrollDir = DIR_Up;
	}

	m_iScrollYPos = pt.y;

	return TRUE;
}

BOOL CIssKineticList::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == IDT_SROLL_TIMER)
	{
		if(m_eScroll == SCROLL_To)
		{
			m_iScrollPos = GetNextScrollToPos();
			if(ScrollToDestReached())
			{
				// we're done
				KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
				m_eScroll = SCROLL_Stopped;
				m_iScrollPos = m_iScrollToDest;
			}
			ReDrawList();

			return TRUE;
		}

		if (fabs(m_dbSpeed) <= 0.01)
		{
			KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
			m_eScroll = SCROLL_Stopped;//since we're no longer moving
			m_dbSpeed = 0;
            m_dbFriction = FRICTION;
            //EnsureWithinBounds();            
		} 
		else 
		{
			m_eScroll = SCROLL_Moving;
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
        KillTimer(GetOwnerWindow(), IDT_SELECT_TIMER);

        int iSelected = GetItemIndexAtPos(m_ptYStartGrab.y - m_rcList.top);
        if(iSelected != -1 && IsItemSelectable(iSelected))
        {
            m_eMouse = GRAB_None;
            m_iCurSelector = iSelected;
            TypeItems* sItem = GetSelectedItem();
            if(sItem)
                GrabItem(m_ptYStartGrab, sItem);
            ReDrawList();
        }
    }

	return UNHANDLED;
}

void CIssKineticList::DrawEntries(CIssGDIEx& gdi, RECT& rcClip)
{
    TypeItems* sItem = NULL;
    int iHeight = 0;
    int iItemHeight = 0;
    RECT rcDraw;
    BOOL bIsHighlighted;
	CIssGDIEx* myGDI = &gdi;

    if(!m_gdiList || !m_gdiSeparator)
		return;

	//if(m_dwFlags & OPTION_UseSecondGDI)
	{
		if(m_gdiList->GetDC() == NULL || m_gdiList->GetWidth() != WIDTH(m_rcList) || m_gdiList->GetHeight() != HEIGHT(m_rcList))
			m_gdiList->Create(gdi.GetDC(), WIDTH(m_rcList), HEIGHT(m_rcList), FALSE, TRUE);

		BitBlt(*m_gdiList,
			0,0,
			WIDTH(m_rcList), HEIGHT(m_rcList),
			gdi,
			m_rcList.left, m_rcList.top,
			SRCCOPY);

		myGDI = m_gdiList;

		rcDraw.left     = 0;
		rcDraw.right    = WIDTH(m_rcList); 
	}
/*	else
	{
		rcDraw.left		= m_rcList.left;
		rcDraw.right	= m_rcList.right;
	}*/

    //now here's where we draw relative to the scroll position ... 
    for(int i = 0; i < m_arrItems.GetSize(); i++)
    {
        sItem = m_arrItems[i];
        if(!sItem)
        {
            ASSERT(0);
            return;
        }
        bIsHighlighted = FALSE;

        // get the height in pixels of this item
        iItemHeight = GetItemHeight(i, sItem);

		rcDraw.top  = /*(m_dwFlags&OPTION_UseSecondGDI?0:m_rcList.top) +*/ iHeight - GetScrollPos();

        if(iHeight + iItemHeight < GetScrollPos())
        {
            iHeight += iItemHeight;
            continue;//we're off screen completely
        }

        if(iHeight > GetScrollPos() + HEIGHT(m_rcList))
            break;//all done

        // when we add the selector then we'll put more logic here
        iHeight += iItemHeight;

        rcDraw.bottom   = rcDraw.top + iItemHeight;

		/*if(!(m_dwFlags&OPTION_UseSecondGDI) && !IsRectInRect(rcDraw, rcClip))
			continue;*/

        // check to see if this item is currently selected
        if(i == m_iCurSelector  && 
            m_bDrawSelector      && 
            sItem->eType != ITEM_Separator &&
            sItem->eType != ITEM_Category)
        {
            RECT rc = rcDraw;

            // only indent if there is a scroll bar
            if(m_dwFlags&OPTION_DrawScrollArrows || m_dwFlags&OPTION_DrawScrollBar)
                rc.right-= INDENT/2;
            bIsHighlighted = TRUE;
            DrawSelector(*myGDI, rc, sItem);            
        }

        //if(sItem->dwFlags & FLAG_Radio)
          //  bIsHighlighted = TRUE;

        //otherwise ... draw those suckers
        switch(sItem->eType)
        {
        case ITEM_Unknown:
            {
                RECT rc = rcDraw;
                rc.right    = rcDraw.left + m_gdiImageArray->GetHeight();
                DrawItemAttributes(*myGDI, rc, sItem);
                rc = rcDraw;
                // only indent if we actually have any flags set
                if(m_bFlagsSet)
                    rc.left += m_gdiImageArray->GetHeight();
                DrawItem(*myGDI, rc, sItem, bIsHighlighted);
                break;
            }
        case ITEM_Text:
            {
                RECT rc = rcDraw;
                rc.right    = rcDraw.left + m_gdiImageArray->GetHeight();
                DrawItemAttributes(*myGDI, rc, sItem);
                rc = rcDraw;
                // only indent if we actually have any flags set
                if(m_bFlagsSet)
                    rc.left += m_gdiImageArray->GetHeight();
                else
                    rc.left += INDENT/2;
                DrawItemText(*myGDI, rc, sItem, bIsHighlighted);
                break;
            }
        case ITEM_Category:
            DrawCategory(*myGDI, rcDraw, sItem);
            break;
        case ITEM_Separator:
            DrawSeparator(*myGDI, rcDraw, sItem);
            break;
        default:
            break;
        }	
    }

	//if(m_dwFlags & OPTION_UseSecondGDI)
		BitBlt(gdi,
			m_rcList.left, m_rcList.top,
			WIDTH(m_rcList), HEIGHT(m_rcList),
			*m_gdiList,
			0,0,
			SRCCOPY);
    return;
}

void CIssKineticList::DrawSelector(CIssGDIEx& gdi, RECT& rcDraw)
{
    // no selector to draw
    if(m_uiSelected == 0)
        return;

    if(!m_imgSelected.IsLoaded())
        m_imgSelected.Initialize(m_hWndMyParent, m_hInstance, m_uiSelected);
    if(WIDTH(rcDraw) != m_imgSelected.GetWidth() || HEIGHT(rcDraw) != m_imgSelected.GetHeight())
        m_imgSelected.SetSize(WIDTH(rcDraw), HEIGHT(rcDraw));

    m_imgSelected.DrawImage(gdi, rcDraw.left, rcDraw.top);

}

void CIssKineticList::DrawItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{
    if(m_pDrawFunc && m_lpClass)
        m_pDrawFunc(gdi, rcDraw, sItem, bIsHighlighted, m_lpClass);
}

void CIssKineticList::DrawItemText(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{
    if(!sItem)
        return;

    COLORREF cr = m_crText;
    if(sItem->dwFlags & FLAG_Grayed)
        cr = RGB(125,125,125);
    else if(sItem->dwFlags & FLAG_Radio)
        cr = m_crSelected;
    else if(bIsHighlighted)
        cr = m_crHighlight;

    DrawText(gdi.GetDC(), sItem->szText, rcDraw, DT_LEFT|DT_VCENTER|DT_NOPREFIX, m_hFontText, cr);
}

void CIssKineticList::DrawItemAttributes(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem)
{
    if(sItem->dwFlags & FLAG_Check)
    {
        // draw the check mark
        Draw(gdi,
            rcDraw.left, rcDraw.top - (m_gdiImageArray->GetHeight()-HEIGHT(rcDraw))/2,
            m_gdiImageArray->GetHeight(), m_gdiImageArray->GetHeight(),
            *m_gdiImageArray,
            IA_Check*m_gdiImageArray->GetHeight(), 0);  
    }
    else if(sItem->dwFlags & FLAG_Radio)
    {
        // draw the radio item
        Draw(gdi,
            rcDraw.left, rcDraw.top - (m_gdiImageArray->GetHeight()-HEIGHT(rcDraw))/2,
            m_gdiImageArray->GetHeight(), m_gdiImageArray->GetHeight(),
            *m_gdiImageArray,
            IA_Radio*m_gdiImageArray->GetHeight(), 0);  
    }
}

void CIssKineticList::DrawCategory(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem)
{

    if(!m_imgCategory.IsLoaded())
    {
        // we need an image out of the image array
        if(m_gdiImageArray->GetDC() == NULL)
            return;

        CIssGDIEx gdiImage;
        gdiImage.Create(gdi.GetDC(), m_gdiImageArray->GetHeight(), m_gdiImageArray->GetHeight(), FALSE, TRUE);
        gdiImage.InitAlpha(TRUE);
        Draw(gdiImage,
            0,0,
            m_gdiImageArray->GetHeight(), m_gdiImageArray->GetHeight(),
            *m_gdiImageArray,
            IA_Scrollbar*m_gdiImageArray->GetHeight(), 0,
            ALPHA_Copy);
        m_imgCategory.CutImage(gdiImage);
    }

    if(WIDTH(rcDraw) != m_imgCategory.GetWidth() || HEIGHT(rcDraw) != m_imgCategory.GetHeight())
        m_imgCategory.SetSize(WIDTH(rcDraw), HEIGHT(rcDraw));

    m_imgCategory.DrawImage(gdi, rcDraw.left, rcDraw.top);

    RECT rc = rcDraw;
    rc.left += INDENT;
    DrawText(gdi.GetDC(), sItem->szText, rc, DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX, m_hFontText, 0x00);
}

void CIssKineticList::DrawSeparator(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem)
{
	if(!m_gdiSeparator)
		return;

    if(m_gdiSeparator->GetDC() == NULL || m_gdiSeparator->GetWidth() != WIDTH(rcDraw))
    {
        m_gdiSeparator->Create(gdi.GetDC(), rcDraw, FALSE, TRUE);

        CIssGDIEx gdiLine, gdiAlpha;
        gdiLine.Create(gdi.GetDC(), WIDTH(rcDraw), 1, FALSE, TRUE);
        gdiAlpha.Create(gdi.GetDC(), WIDTH(rcDraw), 1, FALSE, TRUE);

        RECT rc;
        // create the alpha gradiant first
        SetRect(&rc, 0, 0, WIDTH(rcDraw)/4, 1);
        GradientHorzFillRect(gdiAlpha, rc, RGB(255,255,255), 0);
        SetRect(&rc, WIDTH(rcDraw)*3/4, 0, WIDTH(rcDraw), 1);
        GradientHorzFillRect(gdiAlpha, rc, 0, RGB(255,255,255));
        SetRect(&rc, WIDTH(rcDraw)/4, 0, WIDTH(rcDraw)*3/4, 1);
        FillRect(gdiAlpha, rc, RGB(255,255,255));

        // set the single line with the alpha values
        gdiLine.SetAlphaMask(gdiAlpha);
        SetRect(&rc, 0, 0, WIDTH(rcDraw), 1);
        FillRect(gdiLine, rc, RGB(128,128,128));

        for(int i=0; i<HEIGHT(rcDraw); i++)
        {
            Draw(*m_gdiSeparator, 0, i, WIDTH(rcDraw), 1, gdiLine, 0, 0, ALPHA_Copy);
        }
    }

    Draw(gdi,
        rcDraw.left, rcDraw.top,
        WIDTH(rcDraw), HEIGHT(rcDraw),
        *m_gdiSeparator,
        0,0,
        ALPHA_Normal);

}

int CIssKineticList::GetScrollPercentageHeight(int iScrollbarHeight)
{
    if(GetEndOfListYPos() <= HEIGHT(GetOwnerFrame()))
        return 0;

    int iPercent = iScrollbarHeight*GetScrollPos()/(GetEndOfListYPos() - HEIGHT(GetOwnerFrame()));  

    if(iPercent < 0)
        iPercent = 0;
    else if(iPercent > iScrollbarHeight)
        iPercent = iScrollbarHeight;
    return iPercent;
}

void CIssKineticList::SetScrollPercentage(int iPercent)
{
    if(iPercent < 0)
        iPercent = 0;
    if(iPercent > 100)
        iPercent = 100;
    
    int iScrollPosition = iPercent*(GetEndOfListYPos() - HEIGHT(GetOwnerFrame()))/100;

    ScrollTo(iScrollPosition, TRUE);
    ReDrawList();
}

void CIssKineticList::DrawScrollBar(CIssGDIEx& gdi, RECT& rcClip)
{

    // should we be drawing the scroll bars
    if(!(m_dwFlags&OPTION_DrawScrollBar))
        return;

    m_rcScrollBar.left  = m_rcScrollUp.left;
    m_rcScrollBar.right = m_rcScrollUp.right;
    m_rcScrollBar.top   = m_rcScrollUp.bottom + GetScrollPercentageHeight(m_rcScrollDown.top-m_rcScrollUp.bottom - m_gdiImageArray->GetHeight());
    m_rcScrollBar.bottom= m_rcScrollBar.top + m_gdiImageArray->GetHeight();

    RECT rc = m_rcScrollBar;
    int iIndex = 0;
    while(TRUE)
    {
        rc.top  = m_rcScrollUp.bottom + iIndex*m_gdiImageArray->GetHeight();
        rc.bottom = rc.top + m_gdiImageArray->GetHeight();
        rc.bottom = min(m_rcScrollDown.top, rc.bottom);

        iIndex ++;

        if(rc.top > rcClip.bottom || rc.top > rc.bottom)
            break;

        if(!IsRectInRect(rcClip, rc))
            continue;

        Draw(gdi,
            rc.left, rc.top,
            WIDTH(rc), HEIGHT(rc),
            *m_gdiImageArray,
            IA_ScrollbarBackground*m_gdiImageArray->GetHeight(), 0);
    }

    /*if(m_rcScrollBar.bottom > m_rcScrollDown.top)
    {
    m_rcScrollBar.bottom   = m_rcScrollDown.top;
    m_rcScrollBar.top      = m_rcScrollBar.bottom - SCROLLBAR_Height;
    }*/

    /*if(m_gdiScrollBar.GetDC() == NULL)
    {
    CIssRect rc(m_rcScrollBar);
    rc.ZeroBase();
    m_gdiScrollBar.Create(gdi.GetDC(), rc.Get(), FALSE, TRUE);
    DrawShinyRoundRectangle(m_gdiScrollBar, rc.Get(), 0xD3BD84, 0, 0, 0xD3BD84, 0x9E6331, 0x7D3108, 0xDF425A);
    }

    BitBlt(gdi.GetDC(),
    m_rcScrollBar.left,m_rcScrollBar.top,
    WIDTH(m_rcScrollBar), HEIGHT(m_rcScrollBar),
    m_gdiScrollBar.GetDC(),
    0,0,
    SRCCOPY);*/

    Draw(gdi,
        m_rcScrollBar.left, m_rcScrollBar.top,
        m_gdiImageArray->GetHeight(), m_gdiImageArray->GetHeight(),
        *m_gdiImageArray,
        IA_Scrollbar*m_gdiImageArray->GetHeight(), 0);


}

void CIssKineticList::DrawScrollArrows(CIssGDIEx& gdi, RECT& rcClip)
{
    // should we be drawing the scroll arrows
    if(!(m_dwFlags&OPTION_DrawScrollArrows))
        return;

    /*if(m_gdiScroll.GetDC() == NULL)
    {
    m_gdiScroll.Create(gdi.GetDC(), 4*WIDTH(m_rcScrollUp), HEIGHT(m_rcScrollUp), FALSE, FALSE, TRUE);

    HPEN   hpSel, hpUnSel;
    HBRUSH brSel, brUnSel;

    hpSel			= CreatePen(PS_SOLID,1,RGB(255,255,255));
    brSel			= CreateSolidBrush(RGB(255,255,255));
    hpUnSel			= CreatePen(PS_SOLID,1,RGB(128,128,128));
    brUnSel			= CreateSolidBrush(RGB(128,128,128));

    POINT pt[4];

    int iWidth = WIDTH(m_rcScrollUp);
    int iHeight= HEIGHT(m_rcScrollUp);

    // draw selected up
    HPEN   hpOld	= (HPEN)SelectObject(m_gdiScroll, hpSel);
    HBRUSH brOld	= (HBRUSH)SelectObject(m_gdiScroll, brSel);
    pt[0].x     = 0;
    pt[0].y     = iHeight;
    pt[1].x     = iWidth/2;
    pt[1].y     = 0;
    pt[2].x     = iWidth;
    pt[2].y     = iHeight;
    pt[3]       = pt[0];
    Polygon(m_gdiScroll, pt, 4);

    // draw unselected up
    SelectObject(m_gdiScroll, hpUnSel);
    SelectObject(m_gdiScroll, brUnSel);
    pt[0].x     = iWidth;
    pt[0].y     = iHeight;
    pt[1].x     = iWidth + iWidth/2;
    pt[1].y     = 0;
    pt[2].x     = iWidth + iWidth;
    pt[2].y     = iHeight;
    pt[3]       = pt[0];
    Polygon(m_gdiScroll, pt, 4);

    // draw selected down
    SelectObject(m_gdiScroll, hpSel);
    SelectObject(m_gdiScroll, brSel);
    pt[0].x     = 2*iWidth;
    pt[0].y     = 0;
    pt[1].x     = 2*iWidth + iWidth;
    pt[1].y     = 0;
    pt[2].x     = 2*iWidth + iWidth/2;
    pt[2].y     = iHeight;
    pt[3]       = pt[0];
    Polygon(m_gdiScroll, pt, 4);

    // draw unselected down
    SelectObject(m_gdiScroll, hpUnSel);
    SelectObject(m_gdiScroll, brUnSel);
    pt[0].x     = 3*iWidth;
    pt[0].y     = 0;
    pt[1].x     = 3*iWidth + iWidth;
    pt[1].y     = 0;
    pt[2].x     = 3*iWidth + iWidth/2;
    pt[2].y     = iHeight;
    pt[3]       = pt[0];
    Polygon(m_gdiScroll, pt, 4);

    SelectObject(m_gdiScroll, hpOld);
    DeleteObject(hpSel);
    DeleteObject(hpUnSel);

    SelectObject(m_gdiScroll, brOld);
    DeleteObject(brSel);
    DeleteObject(brUnSel);
    }*/

    //DebugOut(_T("ScrollPos %d"), GetScrollPos());
    //DebugOut(_T("GetEndOfListYPos %d"), GetEndOfListYPos());

    if(GetScrollPos() > 0)
    {
        // draw the Up as selected
        Draw(gdi,
            m_rcScrollUp.left, m_rcScrollUp.top,
            m_gdiImageArray->GetHeight(), m_gdiImageArray->GetHeight(),
            *m_gdiImageArray,
            IA_ScrollArrowUpSel*m_gdiImageArray->GetHeight(), 0);  
    }
    else
    {
        // draw Up as unselected
        Draw(gdi,
            m_rcScrollUp.left, m_rcScrollUp.top,
            m_gdiImageArray->GetHeight(), m_gdiImageArray->GetHeight(),
            *m_gdiImageArray,
            IA_ScrollArrowUpUnSel*m_gdiImageArray->GetHeight(), 0);  
    }

    if(GetScrollPos() + HEIGHT(m_rcList) < GetEndOfListYPos())
    {
        // draw the Down as selected
        Draw(gdi,
            m_rcScrollDown.left, m_rcScrollDown.top,
            m_gdiImageArray->GetHeight(), m_gdiImageArray->GetHeight(),
            *m_gdiImageArray,
            IA_ScrollArrowDwnSel*m_gdiImageArray->GetHeight(), 0);  
    }
    else
    {
        // draw the Down as unselected
        Draw(gdi,
            m_rcScrollDown.left, m_rcScrollDown.top,
            m_gdiImageArray->GetHeight(), m_gdiImageArray->GetHeight(),
            *m_gdiImageArray,
            IA_ScrollArrowDwnUnSel*m_gdiImageArray->GetHeight(), 0);       
    }
}

int	CIssKineticList::GetNextScrollToPos()
{
	static float fInc = 0.0;

	// 1)calculate the scroll increment once at beginning of scroll
	if(m_iScrollToStart == m_iScrollPos)
	{
		// calculate increment
		fInc = CalcScrollIncrement();
		if(m_eScrollDir == DIR_Up)
			fInc *= -1.0;
	}

	// 2) adjust the increment for where in the scroll area we are
	// first 1/3 of travel = accelerating
	BOOL bAccelerate = FALSE;
	if(m_eScrollDir == DIR_Down)
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
	if(m_eScrollDir == DIR_Down)
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
	if(m_eScrollDir == DIR_Down)
		fInc = fInc > (float)1.0 ? fInc : (float)1.0;
	else
		fInc = fInc < (float)-1.0 ? fInc : (float)-1.0;

	//DebugOut(_T("increment: %.3f pos: %i %s%s"), 
	//	fInc, m_iScrollPos + (int)fInc, 
	//	bAccelerate ? _T("accelerate") : _T(""), 
	//	bDeccelerate ? _T("deccelerate") : _T(""));


	return m_iScrollPos + (int)fInc;
}

BOOL CIssKineticList::ScrollToDestReached()
{
	if(m_eScrollDir == DIR_Down && m_iScrollPos >= m_iScrollToDest)
		return TRUE;
	if(m_eScrollDir == DIR_Up && m_iScrollPos <= m_iScrollToDest)
		return TRUE;

	return FALSE;
}
	
float CIssKineticList::CalcScrollIncrement()
{
	// fill this in
	return 4.0;
}

void CIssKineticList::EnsureWithinBounds()
{
    int iBottom = max(0, GetEndOfListYPos() - HEIGHT(GetOwnerFrame()));

    //bounds checking
    if(m_iScrollPos < 0)
    {
        if(m_dwFlags & OPTION_Bounce && (m_eScroll == SCROLL_Stopped || m_eScroll == SCROLL_Scrolling))
            ScrollTo(0);
        else if(m_dwFlags & OPTION_Bounce && m_eScroll == SCROLL_To)
        {
            // don't do anything
        }
        else
        {
            m_dbSpeed = 0;
            m_dbFriction = FRICTION;
            KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
            m_eScroll = SCROLL_Stopped;
            m_iScrollPos = 0;
        }       
    }
    else if(m_iScrollPos >= iBottom)
    {
        if(m_dwFlags & OPTION_Bounce && (m_eScroll == SCROLL_Stopped || m_eScroll == SCROLL_Scrolling))
            ScrollTo(iBottom);
        else if(m_dwFlags & OPTION_Bounce && m_eScroll == SCROLL_To)
        {
            // don't do anything
        }
        else
        {
            m_dbSpeed = 0;
            m_dbFriction = FRICTION;
            KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
            m_iScrollPos = iBottom;
            m_eScroll = SCROLL_Stopped;
        }
    }


}

//really shouldn't be doing bounds checking here ... but good enough for now
void CIssKineticList::ReDrawList(int iScrollAmount)
{
	RECT rc = GetOwnerFrame();

	int iBottom = max(0, GetEndOfListYPos() - HEIGHT(rc));

	int iLastScrollPos = m_iScrollPos;
	m_iScrollPos -= iScrollAmount;

    if(iLastScrollPos == m_iScrollPos && m_eScroll == SCROLL_Moving)
    {
        m_dbSpeed = 0;
        m_eScroll = SCROLL_Stopped;
        m_dbFriction = FRICTION;
        KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
    }

    if((m_dwFlags & OPTION_Bounce) && (m_eScroll == SCROLL_Moving || m_eScroll == SCROLL_Scrolling))
    {
        if(m_eScroll == SCROLL_Moving && (m_iScrollPos < 0 || m_iScrollPos >= iBottom))
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

void CIssKineticList::ScrollList()
{
	if(m_eScroll != SCROLL_Moving)
		return;

	int iScrollAmnt = 0;

	RECT rc;
	GetClientRect(GetOwnerWindow(), &rc);

	int iBottom = GetEndOfListYPos() - HEIGHT(rc) + rc.top;

	while(m_dbSpeed != 0.0)
	{
		if(fabs(m_dbSpeed) < MIN_SPEED)
		{
			m_dbSpeed = 0.0;
			m_eScroll = SCROLL_Stopped;
			return;
		}
		else
		{ //we're scrolling
			m_eScroll = SCROLL_Moving;
			m_dbSpeed -= m_dbSpeed/FRICTION;
			iScrollAmnt = (int)m_dbSpeed;

			m_iScrollPos -= iScrollAmnt;
			//bounds checking
			if(m_iScrollPos < 0)
			{
				m_dbSpeed = 0.0;
                m_dbFriction = FRICTION;
                /*if(m_dwFlags & OPTION_Bounce)
                {
                    ScrollTo(0);
                }
                else*/
                    m_iScrollPos = 0;
			}
			else if(m_iScrollPos > iBottom)
			{
                m_dbSpeed = 0.0;
				m_dbFriction = FRICTION;
                /*if(m_dwFlags & OPTION_Bounce)
                    ScrollTo(iBottom);
                else*/
                    m_iScrollPos = iBottom;
			}

			ReDrawList();
		}

		if(ShouldAbort(GetOwnerWindow()))
			break;
	}
}

void CIssKineticList::StopScrolling()
{ 
	m_dbSpeed = 0;
    m_dbFriction = FRICTION;
	KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
	m_eScroll = SCROLL_Stopped; 
}


void CIssKineticList::ScrollTo(int iYPos, BOOL bNoWait)
{
	//DebugOut(_T("scrolling to %d px"), iYPos);
    RECT rc = GetOwnerFrame();
    if(bNoWait)//skip the animation
	{
        m_eScroll = SCROLL_Stopped;
		m_iScrollPos = min(iYPos, GetEndOfListYPos() - HEIGHT(rc) + rc.top);
	}
	else
	{
        int iBottom = max(0, GetEndOfListYPos() - HEIGHT(GetOwnerFrame()));
        m_iScrollToStart = m_iScrollPos;        
        m_iScrollToDest = min(iYPos, iBottom); // min(iYPos, GetEndOfListYPos() - HEIGHT(rc) + rc.top);
        m_eScrollDir = m_iScrollToStart > m_iScrollToDest ? DIR_Up : DIR_Down;
        //DBG_OUT((_T("CIssKineticList::ScrollTo iYPos: %d, scrollstart: %d, scrolldest: %d"), iYPos, m_iScrollToStart, m_iScrollToDest));
        m_eScroll = SCROLL_To;
        KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);
		SetTimer(GetOwnerWindow(), IDT_SROLL_TIMER, REFRESH_RATE, NULL);
	}
}

void CIssKineticList::ReDrawList(void)
{
    /*HDC hdc= GetDC(m_hWnd);
    CIssRect rc(m_rcList);
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    rc.Concatenate(m_rcScrollDown);
    rc.Concatenate(m_rcScrollUp);
    DrawBackground(m_gdiMem, rcClient, rc.Get());
    DrawEntries(m_gdiMem, m_rcList);
    DrawScrollArrows(m_gdiMem, m_rcList);
    DrawScrollBar(m_gdiMem, m_rcList);    
    BitBlt(hdc,
        rc.left,rc.top,
        rc.GetWidth(), rc.GetHeight(),
        m_gdiMem,
        rc.left,rc.top,
        SRCCOPY);
    ReleaseDC(m_hWnd, hdc);*/

    // draw to the screen now
    InvalidateRect(m_hWndMyParent, &m_rcLoc, FALSE);
    UpdateWindow(m_hWndMyParent);
}

void CIssKineticList::ShowSelectedItem(BOOL bAnimated)
{
    if(m_iCurSelector == UND || !m_bDrawSelector)
        return;

    TypeItems* sItem = m_arrItems[m_iCurSelector];
    if(!sItem)
        return;

    int iYStart = GetYStartPosition(sItem);

    // if its off screen (even partially)
    if(iYStart < GetScrollPos()) 
    {
        int iScroll = iYStart - HEIGHT(m_rcList)/2 + 2*INDENT;
        if(iScroll < 0)
            iScroll = 0;
        CIssKineticList::ScrollTo(iScroll, !bAnimated);
        InvalidateRect(m_hWndMyParent, &m_rcLoc, FALSE);
    }
    else if(iYStart + GetItemHeight(m_iCurSelector, sItem) > (GetScrollPos() + HEIGHT(m_rcList)))
    {
        int iScroll = iYStart - HEIGHT(m_rcList)/2 + 2*INDENT;
        if(iYStart > GetEndOfListYPos() - HEIGHT(m_rcList)/2)
            iScroll = iYStart;
        CIssKineticList::ScrollTo(iScroll, !bAnimated);
        InvalidateRect(m_hWndMyParent, &m_rcLoc, FALSE);
    }
}


int CIssKineticList::GetYStartPosition(int iIndex)
{
    TypeItems* sItem = m_arrItems[iIndex];
    if(!sItem)
        return UND;

    return GetYStartPosition(sItem);
}

int CIssKineticList::GetYStartPosition(TypeItems* sItem)
{
    if(!sItem)
        return UND;

    int iY = 0;
    for(int i=0; i<m_arrItems.GetSize(); i++)
    {
        TypeItems* sItemC = m_arrItems[i];
        if(!sItemC)
            continue;

        if(sItemC == sItem)
            return iY;

        iY += GetItemHeight(i, sItemC);
    }

    return 0;
}

int CIssKineticList::GetItemHeight(int iIndex, TypeItems* sItem)
{
    if(!sItem)
    {
        sItem = m_arrItems[iIndex];
        if(!sItem)
            return 0;
    }

    if(iIndex == m_iCurSelector && IsItemSelectable(sItem) && m_bDrawSelector)
        return sItem->iHeightSel;
    else
        return sItem->iHeight;
}

int	CIssKineticList::GetNextItemIndex(int iItemIndex, BOOL bSearchUp)
{
    int iNewIndex = iItemIndex + (bSearchUp?-1:1);
    return GetNextSelectableItem(iNewIndex, bSearchUp);
    /*int iNewIndex = iItemIndex;

    if(m_arrItems.GetSize() == 0)
        return UND;

    // end point checking, should we be rapping around
    if(m_dwFlags&OPTION_CircularList)
    {
        if(iItemIndex == 0 && bSearchUp)
            return GetNextSelectableItem(m_arrItems.GetSize() - 1);
        else if(iItemIndex == m_arrItems.GetSize() - 1 && !bSearchUp)
            return GetNextSelectableItem(0);
    }

    if(!bSearchUp)
    {
        while(++iNewIndex < m_arrItems.GetSize())
        {
            if(IsItemSelectable(iNewIndex))
                return iNewIndex;
        }
    }
    else
    {
        while(--iNewIndex >= 0)
        {
            if(IsItemSelectable(iNewIndex))
                return iNewIndex;
        }
    }

    // just return the same one
    return iItemIndex;*/
}

int	CIssKineticList::GetNextPageItemIndex(BOOL bSearchUp)
{
    if(m_arrItems.GetSize() == 0)
        return UND;

    int iNewIndex   = m_iCurSelector;
    int iHeight     = GetScrollPos();

    if(bSearchUp)
    {
        iHeight     -= 1;
        if(iHeight <= 0)
            return GetNextSelectableItem(0, FALSE);   // return the first item (we won't circle around)
    }
    else
    {
        iHeight     += HEIGHT(m_rcList);
        if(iHeight >= GetEndOfListYPos())
            return GetNextSelectableItem(m_arrItems.GetSize()-1, TRUE); // return the last item (we won't circle around)
    }

    int iYpos;
    for(int i=0; i<m_arrItems.GetSize(); i++)
    {
        TypeItems* sItem = m_arrItems[i];
        if(!sItem)
            continue;

        iYpos = GetYStartPosition(sItem);
        // see if this item is at this scroll position
        if(iYpos <= iHeight && iYpos+GetItemHeight(i, sItem) >= iHeight)
            return GetNextSelectableItem(i, bSearchUp);
    }

    return UND;
}

int CIssKineticList::GetNextSelectableItem(int iItem, BOOL bSearchUp)
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
        if(iNewIndex < 0 && m_dwFlags&OPTION_CircularList)
            iNewIndex = m_arrItems.GetSize() - 1;
        else if(iNewIndex < 0)
            break;
        else if(iNewIndex >= m_arrItems.GetSize() && m_dwFlags&OPTION_CircularList)
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

BOOL CIssKineticList::IsItemVisible(int iItemIndex)
{
    TypeItems* sItem = m_arrItems[iItemIndex];
    if(!sItem)
        return FALSE;

    int iYpos = GetYStartPosition(sItem);
    int iHeight = GetItemHeight(iItemIndex, sItem);

    if(iYpos+iHeight < m_iScrollPos)
        return FALSE;

    if(m_iScrollPos+HEIGHT(m_rcList) < iYpos)
        return FALSE;

    return TRUE;
}

RECT CIssKineticList::GetDrawRect(int iItemIndex)
{
    RECT rc;
    ZeroMemory(&rc, sizeof(RECT));

    TypeItems* sItem = m_arrItems[iItemIndex];
    if(!sItem)
        return rc;

    int iYpos = GetYStartPosition(sItem);
    int iHeight = GetItemHeight(iItemIndex, sItem);

    if(iYpos+iHeight < m_iScrollPos)
        return rc;

    if(m_iScrollPos+HEIGHT(m_rcList) < iYpos)
        return rc;

    rc.left = m_rcLoc.left;
    rc.right= m_rcLoc.left + WIDTH(m_rcList);
    rc.top  = m_rcLoc.top + iYpos - m_iScrollPos;
    rc.bottom = rc.top + iHeight;

    return rc;
}

BOOL CIssKineticList::IsItemSelectable(TypeItems* sItem)
{
    if(!sItem)
        return FALSE;

    if(sItem->eType == ITEM_Separator || sItem->eType == ITEM_Category || sItem->dwFlags & FLAG_Grayed)
        return FALSE;
    else
        return TRUE;
}

BOOL CIssKineticList::IsItemSelectable(int iIndex)
{
    TypeItems* sItem = m_arrItems[iIndex];
    return IsItemSelectable(sItem);
}
    
BOOL CIssKineticList::IsItemSelected(int iIndex)
{
    return (m_bDrawSelector && iIndex == m_iCurSelector);
}

BOOL CIssKineticList::SelecteItem(int iIndex)
{
    if(iIndex < 0 || iIndex >= m_arrItems.GetSize())
        return FALSE;

    if(!IsItemSelectable(iIndex))
        return FALSE;

    TypeItems* sItem = m_arrItems[iIndex];
    if(!sItem)
        return FALSE;

    // notify the parent
    PostMessage(m_hWndNotif, WM_COMMAND, sItem->uiMenuID, 0);

    return TRUE;
}

int CIssKineticList::GetItemIndexAtPos(int iYPos)
{
    TypeItems* sItem = NULL;

    if(iYPos < 0 || iYPos > HEIGHT(m_rcList))
        return -1;

    int iHeight = 0;
    int iItemHeight;
    RECT rcItem;
    for(int i = 0; i < m_arrItems.GetSize(); i++)
    {
        sItem = m_arrItems[i];
        if(sItem == NULL)
        {
            ASSERT(0);
            return -1;
        }

        iItemHeight = GetItemHeight(i, sItem);
        rcItem.top  = iHeight - GetScrollPos();
        rcItem.bottom  = rcItem.top + iItemHeight;

        iHeight     += iItemHeight;

        if(rcItem.top <= iYPos && rcItem.bottom >= iYPos)
        {
            return i;
        }
    }

    return -1;
}

int CIssKineticList::GetIndent()
{
    return INDENT;
}

int	CIssKineticList::GetEndOfListYPos(void)
{
    if(!m_arrItems.GetSize())
        return 0;

    if(m_iCurrentHeight == -1)
    {
        m_iCurrentHeight = 0;
        for(int i=0; i<m_arrItems.GetSize(); i++)
        {
            TypeItems* sItem = m_arrItems[i];
            if(!sItem)
                continue;
            m_iCurrentHeight += IsItemSelected(i) ? sItem->iHeightSel : sItem->iHeight;
        }
    }
    return m_iCurrentHeight;
}

HRESULT CIssKineticList::SortList(int (*compare)(const void* , const void*))
{
    return m_arrItems.Sort(compare);
}

int CIssKineticList::CompareNameItems(const void* lp1, const void* lp2)
{
    /*TypeItems* sListItem1 = (TypeItems *)lp1;
    TypeItems* sListItem2 = (TypeItems *)lp2;

    if(!sListItem1 || !sListItem2 || !sListItem1->szText || !sListItem2->szText)
        return 0;

    CIssString* oStr = CIssString::Instance();

    return oStr->CompareNoCase(sListItem1->szText, sListItem2->szText);*/
    TypeItems& sListItem1 = **(TypeItems **)lp1;
    TypeItems& sListItem2 = **(TypeItems **)lp2;
    CIssString* oStr = CIssString::Instance();

    return oStr->CompareNoCase(sListItem1.szText, sListItem2.szText);
}

void CIssKineticList::SortNamedList()
{
    m_arrItems.Sort(CompareNameItems);
}

void CIssKineticList::KillAllTimers()
{
    KillTimer(GetOwnerWindow(), IDT_SELECT_TIMER);
    KillTimer(GetOwnerWindow(), IDT_SROLL_TIMER);    
}