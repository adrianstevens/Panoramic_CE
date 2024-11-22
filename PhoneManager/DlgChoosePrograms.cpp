#include "StdAfx.h"
#include "DlgChoosePrograms.h"
#include "resource.h"
#include "PoomContacts.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "ContactsGuiDefines.h"
#include "ObjSkinEngine.h"
#include "IssDebug.h"

#define IDMENU_SelectItem               5000

CIssKineticList  CDlgChoosePrograms::m_oMenu;
BOOL             CDlgChoosePrograms::m_bPreventDeleteList = FALSE;           // Should we not delete the list for faster loading
DWORD            CDlgChoosePrograms::m_dwFullListCount = 0;
DWORD            CDlgChoosePrograms::m_dwLastPopulate = 0;

TypeLaunchItem::TypeLaunchItem()
:eType(LT_Program)
,szFriendlyName(NULL)
,szPath(NULL)
,iIconIndex(0)
{}

TypeLaunchItem::~TypeLaunchItem()
{
    Destroy();
}

void TypeLaunchItem::Destroy()
{
    CIssString* oStr = CIssString::Instance();
    oStr->Delete(&szFriendlyName);
    oStr->Delete(&szPath);
}

void TypeLaunchItem::Clone(TypeLaunchItem* sItem)
{
    Destroy();
    if(!sItem)
        return;

    CIssString* oStr = CIssString::Instance();
    if(sItem->szPath)
        szPath  = oStr->CreateAndCopy(sItem->szPath);
    if(sItem->szFriendlyName)
        szFriendlyName = oStr->CreateAndCopy(sItem->szFriendlyName);
    eType = sItem->eType;
    iIconIndex = sItem->iIconIndex;
}

CDlgChoosePrograms::CDlgChoosePrograms(CIssVector<TypeLaunchItem>* arrExcludeItems /*= 0*/)
{
    m_arrExcludeList= arrExcludeItems;
}

CDlgChoosePrograms::~CDlgChoosePrograms(void)
{
    if(!m_bPreventDeleteList)
       ResetListContent();
}

BOOL CDlgChoosePrograms::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));

    SHFILEINFO sfi = {0};
    m_hImageList = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
        SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);
    

    
    // we only fully repopulate if we don't have the same amount of contacts as before or our current list is empty
    //if(pPoom->GetNumContacts() != m_dwFullListCount || m_oMenu.GetItemCount() == 0)
        PopulateList();
    /*else
    {
        FilterList();

        DWORD dwFlags   = OPTION_AlwaysShowSelector|OPTION_CircularList;
        if(m_oMenu.GetItemCount() > 20)
            dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar);

        // do other menu initialization here
        m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, dwFlags, TRUE);
        m_oMenu.SetSelectedItemIndex(0, TRUE);
    }*/

    ::SetCursor(hCursor);

	return TRUE;
}

void CDlgChoosePrograms::FilterList()
{
    // anything to exclude?
    if(!m_arrExcludeList || m_arrExcludeList->GetSize() == 0)
        return;

    for(int i=m_oMenu.GetItemCount()-1; i>=0; i--)
    {
        TypeItems* sItem = m_oMenu.GetItem(i);
        if(!sItem)
            continue;

        // if we are on the exclude list
        /*if(IsExcludedItem((LONG)sItem->lParam))
        {
            m_oMenu.RemoveItem(i);
        }*/
    }
}



//we'll make this smart - if its already on that letter it'll go to the next one of the same letter
//otherwise back to the start
//cases we need to check: 1 of the letter, 2 or more, none, none at the bottom
void CDlgChoosePrograms::ScrollTo(TCHAR szChar)
{
    TypeItems* sItem = NULL;
    TypeLaunchItem* sLink = NULL;

    int iEntryPos = 0;
    int iScrollToPos = 0;

    TCHAR c = 0;

    //find the scroll position
    for(; iEntryPos < m_oMenu.GetItemCount(); iEntryPos++)
    {
        sItem = m_oMenu.GetItem(iEntryPos);
        if(!sItem || !sItem->lpItem)
            continue;

        sLink = (TypeLaunchItem*)sItem->lpItem;
        if(!sLink->szFriendlyName)
            continue;

        c = sLink->szFriendlyName[0];
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
    sItem = m_oMenu.GetItem(m_oMenu.GetSelectedItemIndex());

    if(sItem && sItem->lpItem)
    {
        sLink = (TypeLaunchItem*)sItem->lpItem;
        szCur = sLink->szFriendlyName[0];
        m_oStr->Capitalize(&szChar);

        if(szCur == c)//same letter
        {
            sItem = NULL;
            //see what's one position past the current
            sItem = m_oMenu.GetItem(m_oMenu.GetSelectedItemIndex() + 1);
            if(sItem && sItem->lpItem)
            {
                sLink = (TypeLaunchItem*)sItem->lpItem;
                szNext = sLink->szFriendlyName[0];

                if(szNext == szCur)//we're golden
                    iScrollToPos = m_oMenu.GetSelectedItemIndex() + 1;
                //otherwise do nothing
            }
        }
    }

    if(iScrollToPos == m_oMenu.GetSelectedItemIndex())
        return;

    m_oMenu.SetSelectedItemIndex(iScrollToPos, TRUE);
    m_oMenu.ShowSelectedItem(TRUE);
}

BOOL CDlgChoosePrograms::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    TCHAR c = LOWORD(wParam);
    if(iswalpha(c))
    {
        ScrollTo(toupper(c));
        InvalidateRect(hWnd, NULL, FALSE);
    }

    return UNHANDLED;
}

BOOL CDlgChoosePrograms::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    if(m_oMenu.GetItemCount() == 0)
        DrawTextShadow(gdi, _T("No programs found"), rcClient, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), 0);
    else
        m_oMenu.OnDraw(gdi, rcClient, rcClip);

	return TRUE;
}

BOOL CDlgChoosePrograms::OnLButtonDown(HWND hWnd, POINT& pt)
{
    return m_oMenu.OnLButtonDown(pt);
}

BOOL CDlgChoosePrograms::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_oMenu.OnLButtonUp(pt))
    {}
    else if(PtInRect(&m_rcBottomBar, pt))
    {
        if(pt.x < WIDTH(m_rcBottomBar)/2)
        {
            //FindOIDFromSelection();
            SafeCloseWindow(IDOK);
        }
        else
        {
            SafeCloseWindow(IDCANCEL);
        }
    }
    return TRUE;
}

BOOL CDlgChoosePrograms::OnMouseMove(HWND hWnd, POINT& pt)
{
    return m_oMenu.OnMouseMove(pt);
}

BOOL CDlgChoosePrograms::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnKeyDown(wParam, lParam);
}

BOOL CDlgChoosePrograms::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnTimer(wParam, lParam);
}

BOOL CDlgChoosePrograms::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    if(GetSystemMetrics(SM_CXSCREEN) != WIDTH(rc))
        return TRUE;

    int iIndent = INDENT;

    //m_oMenu.OnSize(iIndent,HEIGHT(m_rcTitle) + iIndent,WIDTH(rc)-2*iIndent, HEIGHT(rc)-2*iIndent - HEIGHT(m_rcBottomBar) - HEIGHT(m_rcBottomBar));
    m_oMenu.OnSize(m_rcArea.left,m_rcArea.top, WIDTH(m_rcArea), HEIGHT(m_rcArea));

	return TRUE;
}

void CDlgChoosePrograms::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, _T("Select Program"), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgChoosePrograms::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, _T("Ok"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
        
    rc = m_rcBottomBar;
    rc.left = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, _T("Cancel"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

void CDlgChoosePrograms::OnMenuLeft()
{
    //FindOIDFromSelection();
    SafeCloseWindow(IDOK);
}

void CDlgChoosePrograms::OnMenuRight()
{
    SafeCloseWindow(IDCANCEL);
}

void CDlgChoosePrograms::PopulateList()
{
    m_oMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu.SetCustomDrawFunc(DrawListItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteMyItem);
    m_oMenu.PreloadImages(m_hWnd, m_hInst);

    TCHAR szPath[STRING_MAX];
    // find all the programs
    SHGetSpecialFolderPath(NULL, szPath, CSIDL_STARTMENU, FALSE);
    SearchItems(szPath);

    DWORD dwFlags   = OPTION_AlwaysShowSelector|OPTION_CircularList;
    if(m_oMenu.GetItemCount() > 20)
        dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar);

    // do other menu initialization here
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, dwFlags, TRUE);
    m_oMenu.SortList(CompareLinks);
    m_oMenu.SetItemHeights(GetSystemMetrics(SM_CXICON) + INDENT, GetSystemMetrics(SM_CXICON) + INDENT);
    m_oMenu.SetSelectedItemIndex(0, TRUE);

}

void CDlgChoosePrograms::SearchItems(TCHAR* szPath)
{
    WIN32_FIND_DATA wfd;

    TCHAR szSearch[MAX_PATH];

    m_oStr->StringCopy(szSearch, szPath);
    if(szSearch[m_oStr->GetLength(szSearch)-1] != _T('\\'))
        m_oStr->Concatenate(szSearch, _T("\\"));
    m_oStr->Concatenate(szSearch, _T("*.*"));

    HANDLE hFind = FindFirstFile(szSearch, &wfd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do 
        {
            // get the full path to the file or folder
            m_oStr->StringCopy(szSearch, szPath);
            if(m_oStr->Compare(szPath, _T("\\"))!=0)
                m_oStr->Concatenate(szSearch, _T("\\"));
            m_oStr->Concatenate(szSearch, wfd.cFileName);

            if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                SearchItems(szSearch);
            }
            else
            {
                // we're only looking for .lnk files
                int iIndex = m_oStr->Find(szSearch, _T(".lnk"));
                if(iIndex == -1)
                    continue;

                // special case
                if(m_oStr->Compare(wfd.cFileName, _T("icon.lnk"))==0)
                    continue;

                TCHAR szFriendlyName[MAX_PATH];
                // get the friendly name
                m_oStr->StringCopy(szFriendlyName, wfd.cFileName);
                iIndex		= m_oStr->Find(szFriendlyName, _T(".lnk"));
                int iLen	= m_oStr->GetLength(szFriendlyName);
                if(iIndex != -1)
                    m_oStr->Delete(iIndex, 4, szFriendlyName);

                TypeLaunchItem* sItem  = new TypeLaunchItem;
                if(!sItem)
                    continue;

                SHFILEINFO sfi = {0};
                // get the icon index
                if (SHGetFileInfo(szSearch, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ATTRIBUTES))
                    sItem->iIconIndex = sfi.iIcon;

                sItem->eType            = LT_Program;
                sItem->szFriendlyName	= m_oStr->CreateAndCopy(szFriendlyName);
                sItem->szPath	        = m_oStr->CreateAndCopy(szSearch);

                // check if we want to exclude this item
                if(IsExcludedItem(sItem))
                {
                    delete sItem;
                    continue;
                }

                // add to our vector
                m_oMenu.AddItem((LPVOID)sItem, IDMENU_SelectItem);

            }
        } while(FindNextFile(hFind, &wfd));
        FindClose(hFind);
    } 
}

BOOL CDlgChoosePrograms::IsExcludedItem(TypeLaunchItem* sItem)
{
    if(!m_arrExcludeList || !sItem)
        return FALSE;

    // see if this item is in the exclude list
    for(int i = 0; i < m_arrExcludeList->GetSize(); i++)
    {
        TypeLaunchItem* sCheck = (TypeLaunchItem*)m_arrExcludeList->GetElement(i);
        if(sCheck && 0 == m_oStr->Compare(sItem->szPath, sCheck->szPath))
            return TRUE;
    }

    return FALSE;
}

BOOL CDlgChoosePrograms::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
    case IDOK:
	case IDMENU_Ok:
    case IDMENU_SelectItem:
		//FindOIDFromSelection();
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

void CDlgChoosePrograms::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, BOOL bIsHighlighted, TypeLaunchItem* sItem)
{
    ImageList_Draw(m_hImageList, sItem->iIconIndex, gdi, rcDraw.left + INDENT/2, rcDraw.top + INDENT/2, ILD_TRANSPARENT);

    rcDraw.left += (GetSystemMetrics(SM_CXICON) + INDENT);

    DrawTextShadow(gdi, sItem->szFriendlyName, rcDraw, DT_LEFT|DT_VCENTER|DT_NOPREFIX, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
}

void CDlgChoosePrograms::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgChoosePrograms* pThis = (CDlgChoosePrograms*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;
    TypeLaunchItem* sLink = (TypeLaunchItem*)sItem->lpItem;
    pThis->DrawListItem(gdi, rcDraw, bIsHighlighted, sLink);
}

void CDlgChoosePrograms::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    TypeLaunchItem* sItem = (TypeLaunchItem*)lpItem;
    delete sItem;
}

int CDlgChoosePrograms::CompareLinks( const void *arg1, const void *arg2 )
{
    CIssString* oStr = CIssString::Instance();

    TypeItems& sListItem1 = **(TypeItems **)arg1;
    TypeItems& sListItem2 = **(TypeItems **)arg2;
    if(!sListItem1.lpItem || !sListItem2.lpItem)
        return 0;

    TypeLaunchItem* sLink1    = (TypeLaunchItem*)sListItem1.lpItem;
    TypeLaunchItem* sLink2    = (TypeLaunchItem*)sListItem2.lpItem;

    return oStr->Compare(sLink1->szFriendlyName, sLink2->szFriendlyName);
}





