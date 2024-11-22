#include "StdAfx.h"
#include "DlgChooseContact.h"
#include "resource.h"
#include "PoomContacts.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "ContactsGuiDefines.h"
#include "ObjSkinEngine.h"

#define IDMENU_SelectItem               5000

#define IDT_TIMER_Search                WM_USER + 200
#define Search_Timeout                  1000

CIssKineticList*  CDlgChooseContact::m_oMenu = NULL;
BOOL             CDlgChooseContact::m_bPreventDeleteList = FALSE;           // Should we not delete the list for faster loading
DWORD            CDlgChooseContact::m_dwFullListCount = 0;
DWORD            CDlgChooseContact:: m_dwLastPopulate = 0;

CDlgChooseContact::CDlgChooseContact(HWND hWndMainApp, CIssVector<CObjItemContact>* arrExcludeItems /*= 0*/, BOOL bLastNameFirst /*= TRUE*/)
:m_iContactOid(-1)
,m_bLastNameFirst(bLastNameFirst)
,m_arrExcludeList(arrExcludeItems)
,m_bShowSearch(FALSE)
,m_hFontSearch(NULL)
{
	if(!m_oMenu)
		m_oMenu = new CIssKineticList;

    m_hFontSearch = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON), FW_BOLD, TRUE);
}

CDlgChooseContact::~CDlgChooseContact(void)
{
    if(!m_bPreventDeleteList)
       ResetListContent();

    CIssGDIEx::DeleteFont(m_hFontSearch);
}

BOOL CDlgChooseContact::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
    
    CPoomContacts* pPoom = CPoomContacts::Instance();
    if(!pPoom || !m_oMenu)
    {
        ::SetCursor(hCursor);
        return TRUE;
    }
    
    // we only fully repopulate if we don't have the same amount of contacts as before or our current list is empty
    if(pPoom->GetNumContacts() != m_dwFullListCount || m_oMenu->GetItemCount() == 0)
        LoadContacts();
    else
    {
        FilterContacts();

        DWORD dwFlags   = OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_Bounce;
        if(m_oMenu->GetItemCount() > 20)
            dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar);

        // do other menu initialization here
        m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, dwFlags, TRUE);
        m_oMenu->SetSelectedItemIndex(0, TRUE);
    }

    ::SetCursor(hCursor);

	return TRUE;
}

void CDlgChooseContact::FilterContacts()
{
    // anything to exclude?
    if(!m_arrExcludeList || !m_oMenu || m_arrExcludeList->GetSize() == 0)
        return;

    for(int i=m_oMenu->GetItemCount()-1; i>=0; i--)
    {
        TypeItems* sItem = m_oMenu->GetItem(i);
        if(!sItem)
            continue;

        // if we are on the exclude list
        if(IsExcludedItem((LONG)sItem->lParam))
        {
            m_oMenu->RemoveItem(i);
        }
    }
}


void CDlgChooseContact::ScrollTo(TCHAR* szText)
{
    TypeItems* sItem = NULL;

    int iEntryPos = 0;
    int iScrollToPos = 0;

	if(!m_oMenu)
		return;

    //find the scroll position
  //  for(; iEntryPos < m_arrItems.GetSize(); iEntryPos++)
        //find the scroll position
    for(; iEntryPos < m_oMenu->GetItemCount(); iEntryPos++)
    {
        sItem = m_oMenu->GetItem(iEntryPos);
        if(!sItem || !sItem->szText)
            return;

        if(sItem->eType == ITEM_Text)
        {
            int iFind = m_oStr->FindNoCase(sItem->szText, szText, 0);
            if(0 == iFind)
            {
                iScrollToPos = iEntryPos;    
                break;
            }
            if(0 < m_oStr->CompareNoCase(sItem->szText, szText))
                break;

            iScrollToPos = iEntryPos;
        }
    }

    //otherwise do nothing
    //Error:

    if(iScrollToPos == m_oMenu->GetSelectedItemIndex())
        return;

    m_oMenu->SetSelectedItemIndex(iScrollToPos, TRUE);
    m_oMenu->ShowSelectedItem(TRUE);
}


//we'll make this smart - if its already on that letter it'll go to the next one of the same letter
//otherwise back to the start
//cases we need to check: 1 of the letter, 2 or more, none, none at the bottom
void CDlgChooseContact::ScrollTo(TCHAR szChar)
{
    TypeItems* sItem = NULL;

    int iEntryPos = 0;
    int iScrollToPos = 0;

    TCHAR c = 0;

	if(!m_oMenu)
		return;

    //find the scroll position
    for(; iEntryPos < m_oMenu->GetItemCount(); iEntryPos++)
    {
        sItem = m_oMenu->GetItem(iEntryPos);
        if(!sItem || !sItem->szText)
            return;

        c = sItem->szText[0];
        m_oStr->Capitalize(&c);
        if(c == szChar)
        {
            iScrollToPos = iEntryPos;
            break;
        }
        if(c > szChar)
        {
            break;
        }
        iScrollToPos = iEntryPos;
    }

    //get the current first letter
    TCHAR szCur = 0;
    TCHAR szNext = 0;
    sItem = NULL;
    sItem = m_oMenu->GetItem(m_oMenu->GetSelectedItemIndex());

    if(sItem && sItem->szText)
    {
        szCur = sItem->szText[0];
        m_oStr->Capitalize(&szChar);

        if(szCur == c)//same letter
        {
            sItem = NULL;
            //see what's one position past the current
            sItem = m_oMenu->GetItem(m_oMenu->GetSelectedItemIndex() + 1);
            if(sItem && sItem->szText)
            {
                szNext = sItem->szText[0];

                if(szNext == szCur)//we're golden
                    iScrollToPos = m_oMenu->GetSelectedItemIndex() + 1;
                //otherwise do nothing
            }
        }
    }

    if(iScrollToPos == m_oMenu->GetSelectedItemIndex())
        return;

    m_oMenu->SetSelectedItemIndex(iScrollToPos, TRUE);
    m_oMenu->ShowSelectedItem(TRUE);
}

BOOL CDlgChooseContact::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    TCHAR c = LOWORD(wParam);

    static int iLen = 0;

    if(m_oStr->IsEmpty(m_szSearch))
        iLen = 0;

    if(iLen + 1 == STRING_SMALL)
        return UNHANDLED;

    if(iswalpha(c) || c == _T(' ') || c == _T('-') || c == _T('#') || c == _T('_') || c == _T(',') || c == _T('*'))
    {   
        //    ScrollTo(toupper(c));

        //hack for now
        m_szSearch[iLen] = toupper(c);
        m_szSearch[iLen+1] = _T('\0');
        KillTimer(m_hWnd, IDT_TIMER_Search);
        SetTimer(m_hWnd, IDT_TIMER_Search, Search_Timeout, NULL);
        iLen++;

        m_bShowSearch = TRUE;
        InvalidateRect(m_hWnd, NULL, FALSE);
        ScrollTo(m_szSearch);
    }
    //backspace I think
    if(c == VK_BACK && iLen > 0)
    {
        m_szSearch[iLen-1] = m_szSearch[iLen];
        m_szSearch[iLen] = _T('\0');
        iLen--;
        KillTimer(m_hWnd, IDT_TIMER_Search);
        SetTimer(m_hWnd, IDT_TIMER_Search, Search_Timeout, NULL);

        m_bShowSearch = TRUE;
        InvalidateRect(m_hWnd, NULL, FALSE);
        ScrollTo(m_szSearch);
    }

    return UNHANDLED;
}

BOOL CDlgChooseContact::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
	if(!m_oMenu)
		return TRUE;

    if(m_oMenu->GetItemCount() == 0)
    {
        DrawTextShadow(gdi, _T("No contacts found"), rcClient, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), 0);
    }
    else
    {
        m_oMenu->OnDraw(gdi, rcClient, rcClip);
        DrawSearch(gdi, rcClient);
    }

	return TRUE;
}

void CDlgChooseContact::DrawSearch(CIssGDIEx& gdi, RECT& rcDraw)
{
    if(m_bShowSearch == FALSE)
        return;

    DrawTextShadow(gdi, m_szSearch, rcDraw, DT_CENTER|DT_VCENTER, m_hFontSearch, RGB(150,150,150), 0);
}

BOOL CDlgChooseContact::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(!m_oMenu)
		return TRUE;

    return m_oMenu->OnLButtonDown(pt);
}

BOOL CDlgChooseContact::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_oMenu && m_oMenu->OnLButtonUp(pt))
    {}
    else if(PtInRect(&m_rcBottomBar, pt))
    {
        if(pt.x < WIDTH(m_rcBottomBar)/2)
        {
            FindOIDFromSelection();
            SafeCloseWindow(IDOK);
        }
        else
        {
            SafeCloseWindow(IDCANCEL);
        }
    }
    return TRUE;
}

BOOL CDlgChooseContact::OnMouseMove(HWND hWnd, POINT& pt)
{
	if(!m_oMenu)
		return TRUE;

    return m_oMenu->OnMouseMove(pt);
}

BOOL CDlgChooseContact::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(!m_oMenu)
		return TRUE;

    return m_oMenu->OnKeyDown(wParam, lParam);
}

BOOL CDlgChooseContact::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDT_TIMER_Search)
    {
        KillTimer(hWnd, IDT_TIMER_Search);
        m_oStr->Empty(m_szSearch);
        m_bShowSearch = FALSE;
        //could fade out here ....
        InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    }
	if(!m_oMenu)
		return TRUE;

    return m_oMenu->OnTimer(wParam, lParam);
}

BOOL CDlgChooseContact::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    if(GetSystemMetrics(SM_CXSCREEN) != WIDTH(rc))
        return TRUE;

    int iIndent = INDENT;

    //m_oMenu.OnSize(iIndent,HEIGHT(m_rcTitle) + iIndent,WIDTH(rc)-2*iIndent, HEIGHT(rc)-2*iIndent - HEIGHT(m_rcBottomBar) - HEIGHT(m_rcBottomBar));
    if(m_oMenu)
		m_oMenu->OnSize(m_rcArea.left,m_rcArea.top, WIDTH(m_rcArea), HEIGHT(m_rcArea));

	return TRUE;
}

void CDlgChooseContact::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Select Contact"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgChooseContact::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, _T("Ok"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
        
    rc = m_rcBottomBar;
    rc.left = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, _T("Cancel"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

void CDlgChooseContact::OnMenuLeft()
{
    FindOIDFromSelection();
    SafeCloseWindow(IDOK);
}

void CDlgChooseContact::OnMenuRight()
{
    SafeCloseWindow(IDCANCEL);
}

void CDlgChooseContact::LoadContacts()
{
	if(!m_oMenu)
		return;

    m_oMenu->SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu->SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu->SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu->PreloadImages(m_hWnd, m_hInst);

	CPoomContacts*  pOPoom = CPoomContacts::Instance();

	// if we want to save selected, store it first...
    TCHAR szName[STRING_MAX];

	// empty the list
    m_oMenu->ResetContent();

    m_oStr->Empty(m_szSearch);

    m_dwLastPopulate    = GetTickCount();
    m_dwFullListCount   = pOPoom->GetNumContacts();

    long lOid;

	// add all the items
	for(int i = 0; i < (int)m_dwFullListCount; i++)
	{
#ifdef FIX_ANNOYING_POOM_ONEOFF_ERROR
        IContact* pListItem = pOPoom->GetContactFromIndex(i+1);
#else
        IContact* pListItem = pOPoom->GetContactFromIndex(i);
#endif		
		if(!pListItem)
			continue;
        
        pListItem->get_Oid(&lOid);

        // is this item on our exclude list?
        if(!IsExcludedItem(lOid) && pOPoom->GetFormatedNameText(szName, pListItem, m_bLastNameFirst))
        {
            // add the item here to the menu
            m_oMenu->AddItem(szName, IDMENU_SelectItem, NULL, (LPARAM)lOid, FALSE);
        }

        pListItem->Release();       
    }

    DWORD dwFlags   = OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_Bounce;
    if(m_oMenu->GetItemCount() > 20)
        dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar);

    // do other menu initialization here
    m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, dwFlags, TRUE);
    m_oMenu->SortNamedList();
    m_oMenu->SetSelectedItemIndex(0, TRUE);

}

BOOL CDlgChooseContact::IsExcludedItem(long lOid)
{
    if(!m_arrExcludeList)
        return FALSE;

    // see if this oid is in the exclude list
    for(int i = 0; i < m_arrExcludeList->GetSize(); i++)
    {
        CObjItemContact* pContact = (CObjItemContact*)m_arrExcludeList->GetElement(i);
        if(pContact && pContact->GetOid() == lOid)
            return TRUE;
    }

    return FALSE;
}

BOOL CDlgChooseContact::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case IDOK:
	case IDMENU_Ok:
    case IDMENU_SelectItem:
		FindOIDFromSelection();
		SafeCloseWindow(IDOK);
		break;
	case IDMENU_Cancel:
		SafeCloseWindow(IDCANCEL);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

void CDlgChooseContact::FindOIDFromSelection()
{
	if(!m_oMenu)
		return;

    TypeItems* sItem = m_oMenu->GetSelectedItem();
    if(!sItem)
        return;

    m_iContactOid = (long)sItem->lParam;
}

