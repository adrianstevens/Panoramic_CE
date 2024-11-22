#include "StdAfx.h"
#include "DlgChooseContact.h"
#include "resource.h"
#include "PoomContacts.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "ContactsGuiDefines.h"
#include "ObjSkinEngine.h"

#define IDMENU_SelectItem               5000

CDlgChooseContact::CDlgChooseContact(HWND hWndMainApp, CIssVector<CObjItemContact>* arrExcludeItems /*= 0*/)
:m_bIsMenuCreated(FALSE)
,m_poMenu(NULL)
,m_iContactOid(-1)
{
    m_arrExcludeList= arrExcludeItems;
}

CDlgChooseContact::~CDlgChooseContact(void)
{
    if(m_poMenu)
        m_poMenu->ResetContent();

    if(m_poMenu && m_bIsMenuCreated)
    {
        delete m_poMenu;
        m_poMenu = NULL;
    }
}

void CDlgChooseContact::SetKineticList(CIssKineticList* oList)
{
    if(m_poMenu && m_bIsMenuCreated)
    {
        delete m_poMenu;
        m_poMenu = NULL;
    }
    m_poMenu            = oList;
    m_bIsMenuCreated    = FALSE;
}

BOOL CDlgChooseContact::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    // should we create a new IssKinetic List?
    if(!m_poMenu)
    {
        m_poMenu = new CIssKineticList;
        if(!m_poMenu)
        {
            ::SetCursor(hCursor);
            return FALSE;
        }
        m_bIsMenuCreated = TRUE;

        m_poMenu->SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
        m_poMenu->SetImageArray(SKIN(IDR_PNG_MenuArray));
        m_poMenu->SetSelected(SKIN(IDR_PNG_Selector));
        m_poMenu->PreloadImages(hWnd, m_hInst);
    }

    LoadContacts();

    ::SetCursor(hCursor);

	return TRUE;
}

void CDlgChooseContact::ScrollTo(TCHAR szChar)
{
    if(!m_poMenu)
        return;

    TypeItems* sItem = NULL;

    int iEntryPos = 0;
    int iScrollToPos = 0;
    for(; iEntryPos < m_poMenu->GetItemCount(); iEntryPos++)
    {
        sItem = m_poMenu->GetItem(iEntryPos);
        if(!sItem || !sItem->szText)
            return;

        TCHAR c = sItem->szText[0];
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

    m_poMenu->SetSelectedItemIndex(iScrollToPos, TRUE);
    m_poMenu->ShowSelectedItem(TRUE);
}

BOOL CDlgChooseContact::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    TCHAR c = LOWORD(wParam);
    if(iswalpha(c))
        ScrollTo(toupper(c));

    return UNHANDLED;
}

BOOL CDlgChooseContact::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    if(m_poMenu->GetItemCount() == 0)
    {
        DrawTextShadow(gdi, _T("No profiles found"), rcClient, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), 0);
    }
    else
        m_poMenu->OnDraw(gdi, rcClient, rcClip);

	return TRUE;
}

BOOL CDlgChooseContact::OnLButtonDown(HWND hWnd, POINT& pt)
{
    return m_poMenu->OnLButtonDown(pt);
}

BOOL CDlgChooseContact::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_poMenu->OnLButtonUp(pt))
    {}
    else if(PtInRect(&m_rcBottomBar, pt))
    {
        if(pt.x < WIDTH(m_rcBottomBar)/2)
            FindOIDFromSelection();
        SafeCloseWindow(IDOK);
    }
    return TRUE;
}

BOOL CDlgChooseContact::OnMouseMove(HWND hWnd, POINT& pt)
{
    return m_poMenu->OnMouseMove(pt);
}

BOOL CDlgChooseContact::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_poMenu->OnKeyDown(wParam, lParam);
}

BOOL CDlgChooseContact::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_poMenu->OnTimer(wParam, lParam);
}

BOOL CDlgChooseContact::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    if(GetSystemMetrics(SM_CXSCREEN) != WIDTH(rc))
        return TRUE;

    int iIndent = INDENT;

    //m_poMenu->OnSize(iIndent,HEIGHT(m_rcTitle) + iIndent,WIDTH(rc)-2*iIndent, HEIGHT(rc)-2*iIndent - HEIGHT(m_rcBottomBar) - HEIGHT(m_rcBottomBar));
    m_poMenu->OnSize(m_rcArea.left,m_rcArea.top, WIDTH(m_rcArea), HEIGHT(m_rcArea));

	return TRUE;
}

void CDlgChooseContact::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Select contact"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));

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
    if(!m_poMenu)
        return;

	CPoomContacts*  pOPoom = CPoomContacts::Instance();

	// if we want to save selected, store it first...
    TCHAR szName[STRING_MAX];

	// empty the list
	m_poMenu->ResetContent();

    long lOid;

	// add all the items
	for(int i = 1; i <= pOPoom->GetNumContacts(); i++)
	{
		IContact* pListItem = pOPoom->GetContactFromIndex(i);
		if(!pListItem)
			continue;
        
        pListItem->get_Oid(&lOid);

        // is this item on our exclude list?
        if(!ExcludeItem(lOid) && pOPoom->GetFormatedNameText(szName, pListItem, TRUE))
        {
            // add the item here to the menu
            //m_oStr->Empty(szName);
            //FormatNameText(szName, (LPVOID)pListItem);
            m_poMenu->AddItem(szName, IDMENU_SelectItem, NULL, (LPARAM)lOid, FALSE);
        }

        pListItem->Release();       
    }

    DWORD dwFlags   = OPTION_AlwaysShowSelector|OPTION_CircularList;
    if(m_poMenu->GetItemCount() > 20)
        dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar);


    // do other menu initialization here
    m_poMenu->Initialize(m_hWnd, m_hWnd, m_hInst, dwFlags, TRUE);
    m_poMenu->SortNamedList();
    m_poMenu->SetSelectedItemIndex(0, TRUE);

}

BOOL CDlgChooseContact::ExcludeItem(long lOid)
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



void CDlgChooseContact::FormatNameText(TCHAR* szText, LPVOID pInfo)
{
	CIssString* oStr = CIssString::Instance();
	oStr->Empty(szText);

	CPoomContacts* pPoom = CPoomContacts::Instance();

	IContact* pContact = (IContact*)pInfo;
	if(!pContact || !pPoom)
		return;

    TCHAR* szTemp;
    szTemp = pPoom->GetUserLastName(pContact);
    if(oStr->GetLength(szTemp))
	    oStr->Concatenate(szText, szTemp);

    szTemp = pPoom->GetUserFirstName(pContact);

    if(oStr->GetLength(szText) && oStr->GetLength(szTemp))
	    oStr->Concatenate(szText, _T(", "));

	oStr->Concatenate(szText, pPoom->GetUserFirstName(pContact));
    oStr->Trim(szText);
}

void CDlgChooseContact::FindOIDFromSelection()
{
    if(!m_poMenu)
        return;

    TypeItems* sItem = m_poMenu->GetSelectedItem();
    if(!sItem)
        return;

    m_iContactOid = (long)sItem->lParam;
}

