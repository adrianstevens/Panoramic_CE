#include "StdAfx.h"
#include "DlgChoosePrograms.h"
#include "resource.h"
#include "PoomContacts.h"
#include "IssCommon.h"
#include "IssGDIDraw.h"
#include "ContactsGuiDefines.h"
#include "ObjSkinEngine.h"
#include "IssDebug.h"
#include "IssLocalisation.h"

#define IDT_TIMER_Search                WM_USER + 200
#define Search_Timeout                  1000

#define IDMENU_SelectItem               5000

CIssKineticList*  CDlgChoosePrograms::m_oMenu = NULL;
BOOL             CDlgChoosePrograms::m_bPreventDeleteList = FALSE;           // Should we not delete the list for faster loading
DWORD            CDlgChoosePrograms::m_dwFullListCount = 0;
DWORD            CDlgChoosePrograms::m_dwLastPopulate = 0;
BOOL             CDlgChoosePrograms::m_bProgamsList = TRUE;

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

void TypeLaunchItem::Launch(HWND hWnd)
{
    CIssString* oStr = CIssString::Instance();
    if(oStr->IsEmpty(szPath))
    {
        MessageBeep(MB_ICONHAND);
        return;
    }

    if(eType == LT_Program)
    {
        if(0 == oStr->Compare(oStr->GetText(IDS_Today), szPath))
        {
            HWND hWndDesktop = FindWindow(_T("DesktopExplorerWindow"), NULL);
            if(hWndDesktop)
                SetForegroundWindow((HWND)((ULONG) hWndDesktop | 0x00000001));
        }
        else
        {
            SHELLEXECUTEINFO sei = {0};
            sei.cbSize	    = sizeof(sei);
            sei.hwnd	    = hWnd;
            sei.nShow	    = SW_SHOWNORMAL;
            sei.lpFile      = szPath;
            ShellExecuteEx(&sei);
        }
    }
    else if(eType == LT_FileOrFolder)
    {
        DWORD dwFileAttributes = GetFileAttributes(szPath);
        if(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            CreateProcess(_T("fexplore.exe"), 
                szPath,
                NULL, NULL, 
                FALSE, 0, 
                NULL, NULL, 
                NULL, NULL);
        }
        else
        {
            SHELLEXECUTEINFO sei = {0};
            sei.cbSize	    = sizeof(sei);
            sei.hwnd	    = hWnd;
            sei.nShow	    = SW_SHOWNORMAL;
            sei.lpFile      = szPath;
            ShellExecuteEx(&sei);
        }
    }
    else
    {
        SHELLEXECUTEINFO sei = {0};
        sei.cbSize	    = sizeof(sei);
        sei.hwnd	    = hWnd;
        sei.nShow	    = SW_SHOWNORMAL;
        sei.lpFile      = _T("\\Windows\\ctlpnl.exe");
        sei.lpParameters= szPath;
        ShellExecuteEx(&sei);
    }
}

CDlgChoosePrograms::CDlgChoosePrograms(CIssVector<TypeLaunchItem>* arrExcludeItems /*= 0*/)
:m_bShowSearch(FALSE)
,m_hFontSearch(NULL)
,m_bAddTodayIcon(FALSE)
,m_hWndParent(NULL)
{

	if(!m_oMenu)
	{
		m_oMenu = new CIssKineticList;
	}

    m_arrExcludeList= arrExcludeItems;

    MyCheckProgramsList();

    m_hFontSearch           = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON), FW_BOLD, TRUE);
}

CDlgChoosePrograms::~CDlgChoosePrograms(void)
{
    if(!m_bPreventDeleteList)
       ResetListContent();

    CIssGDIEx::DeleteFont(m_hFontSearch);
}

BOOL CDlgChoosePrograms::DoModal(HWND hWndParent, HINSTANCE hInst, UINT uiDlgID)
{
    m_hWndParent = hWndParent;
    return CIssWnd::DoModal(hWndParent, hInst, uiDlgID); 
}

void CDlgChoosePrograms::DeleteAllContent()
{
	if(m_oMenu)
		delete m_oMenu;
	m_oMenu = NULL;
}

void CDlgChoosePrograms::MyCheckProgramsList()
{
    
}

BOOL CDlgChoosePrograms::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));


    RECT rcParent;
    GetClientRect(m_hWndParent, &rcParent);

    SHFILEINFO sfi = {0};
    m_hImageList = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
        SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);

    if(!m_bProgamsList)
    {
        ResetListContent();
        m_bProgamsList = TRUE;
    }

	if(!m_oMenu)
		return TRUE;
    
       
    // we only fully repopulate if we don't have the same amount of contacts as before or our current list is empty
    if(m_oMenu->GetItemCount() == 0)
        PopulateList();
    else
    {
        FilterList();

        DWORD dwFlags   = OPTION_AlwaysShowSelector|OPTION_CircularList;
        if(m_oMenu->GetItemCount() > 20)
            dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar);

        // do other menu initialization here
        m_oMenu->SetCustomDrawFunc(DrawListItem, this);
        m_oMenu->SetDeleteItemFunc(DeleteMyItem);
        m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, dwFlags, TRUE);
        m_oMenu->SetSelectedItemIndex(0, TRUE);
    }

    ::SetCursor(hCursor);

	return TRUE;
}

void CDlgChoosePrograms::FilterList()
{
    // anything to exclude?
    if(!m_arrExcludeList || m_arrExcludeList->GetSize() == 0 || !m_oMenu)
        return;

    for(int i=m_oMenu->GetItemCount()-1; i>=0; i--)
    {
        TypeItems* sItem = m_oMenu->GetItem(i);
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

	if(!m_oMenu)
		return;

    //find the scroll position
    for(; iEntryPos < m_oMenu->GetItemCount(); iEntryPos++)
    {
        sItem = m_oMenu->GetItem(iEntryPos);
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
    sItem = m_oMenu->GetItem(m_oMenu->GetSelectedItemIndex());

    if(sItem && sItem->lpItem)
    {
        sLink = (TypeLaunchItem*)sItem->lpItem;
        szCur = sLink->szFriendlyName[0];
        m_oStr->Capitalize(&szChar);

        if(szCur == c)//same letter
        {
            sItem = NULL;
            //see what's one position past the current
            sItem = m_oMenu->GetItem(m_oMenu->GetSelectedItemIndex() + 1);
            if(sItem && sItem->lpItem)
            {
                sLink = (TypeLaunchItem*)sItem->lpItem;
                szNext = sLink->szFriendlyName[0];

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

BOOL CDlgChoosePrograms::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    TCHAR c = LOWORD(wParam);

    static int iLen = 0;

    if(m_oStr->IsEmpty(m_szSearch))
        iLen = 0;

    if(iLen + 1 == STRING_SMALL)
        return UNHANDLED;

    if(iswalpha(c) || c == _T(' '))
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

BOOL CDlgChoosePrograms::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
	if(!m_oMenu)
		return TRUE;

    if(m_oMenu->GetItemCount() == 0)
    {
        DrawTextShadow(gdi, m_oStr->GetText(ID(IDS_NO_Programs), m_hInst), rcClient, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), 0);
    }
    else
    {
        m_oMenu->OnDraw(gdi, rcClient, rcClip);
        DrawSearch(gdi, rcClip);
    }

	return TRUE;
}

void CDlgChoosePrograms::DrawSearch(CIssGDIEx& gdi, RECT& rcDraw)
{
    if(m_bShowSearch == FALSE)
        return;

    DrawTextShadow(gdi, m_szSearch, m_rcArea, DT_CENTER|DT_VCENTER, m_hFontSearch, RGB(150,150,150), 0);
}

BOOL CDlgChoosePrograms::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(!m_oMenu)
		return TRUE;

    return m_oMenu->OnLButtonDown(pt);
}

BOOL CDlgChoosePrograms::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_oMenu && m_oMenu->OnLButtonUp(pt))
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
	if(!m_oMenu)
		return TRUE;

    return m_oMenu->OnMouseMove(pt);
}

BOOL CDlgChoosePrograms::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(!m_oMenu)
		return TRUE;

    return m_oMenu->OnKeyDown(wParam, lParam);
}

BOOL CDlgChoosePrograms::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDT_TIMER_Search)
    {
        KillTimer(m_hWnd, IDT_TIMER_Search);
        m_oStr->Empty(m_szSearch);
        m_bShowSearch = FALSE;
        //could fade out here ....
        InvalidateRect(m_hWnd, NULL, FALSE);
        return TRUE;
    }
	if(!m_oMenu)
		return TRUE;

    return m_oMenu->OnTimer(wParam, lParam);
}

BOOL CDlgChoosePrograms::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    if(GetSystemMetrics(SM_CXSCREEN) != WIDTH(rc))
        return TRUE;

    int iIndent = INDENT;

	if(!m_oMenu)
		return TRUE;

    //m_oMenu.OnSize(iIndent,HEIGHT(m_rcTitle) + iIndent,WIDTH(rc)-2*iIndent, HEIGHT(rc)-2*iIndent - HEIGHT(m_rcBottomBar) - HEIGHT(m_rcBottomBar));
    m_oMenu->OnSize(m_rcArea.left,m_rcArea.top, WIDTH(m_rcArea), HEIGHT(m_rcArea));

	return TRUE;
}

void CDlgChoosePrograms::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcTitle;
    rc.left += INDENT;
    DrawText(gdi, m_oStr->GetText(ID(IDS_SELECT_Program), m_hInst), rc, DT_LEFT | DT_VCENTER, m_hFontLabel, RGB(51,51,51));
}

void CDlgChoosePrograms::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, m_oStr->GetText(ID(IDS_MENU_Ok), m_hInst), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
        
    rc = m_rcBottomBar;
    rc.left = m_rcBottomBar.right/2;
    DrawTextShadow(gdi, m_oStr->GetText(ID(IDS_MENU_Cancel), m_hInst), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
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
    m_oStr->Empty(m_szSearch);

	if(!m_oMenu)
		return;

    m_oMenu->SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu->SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_oMenu->SetSelected(SKIN(IDR_PNG_Selector));
    m_oMenu->SetCustomDrawFunc(DrawListItem, this);
    m_oMenu->SetDeleteItemFunc(DeleteMyItem);
    m_oMenu->PreloadImages(m_hWnd, m_hInst);

    TCHAR szPath[STRING_MAX];
    // find all the programs
    SHGetSpecialFolderPath(NULL, szPath, CSIDL_STARTMENU, FALSE);
    SearchItems(szPath);

    // add the special case Today link
    if(m_bAddTodayIcon)
    {
        TypeLaunchItem* sItem  = new TypeLaunchItem;
        if(sItem)
        { 
            sItem->iIconIndex       = -1;
            sItem->eType            = LT_Program;
            sItem->szFriendlyName	= m_oStr->CreateAndCopy(m_oStr->GetText(IDS_Today));
            sItem->szPath	        = m_oStr->CreateAndCopy(m_oStr->GetText(IDS_Today));

            m_oMenu->AddItem((LPVOID)sItem, IDMENU_SelectItem);
        }
    }

    DWORD dwFlags   = OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_Bounce;
    if(m_oMenu->GetItemCount() > 20)
        dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar|OPTION_Bounce);

    // do other menu initialization here
    m_oMenu->Initialize(m_hWnd, m_hWnd, m_hInst, dwFlags, TRUE);
    m_oMenu->SortList(CompareLinks);
    m_oMenu->SetItemHeights(GetSystemMetrics(SM_CXICON) + INDENT, GetSystemMetrics(SM_CXICON) + INDENT);
    m_oMenu->SetSelectedItemIndex(0, TRUE);

}

void CDlgChoosePrograms::SearchItems(TCHAR* szPath)
{
    WIN32_FIND_DATA wfd;

	if(!m_oMenu)
		return;

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
                //check for .exe files

                int iIndex;
                TCHAR szFriendlyName[MAX_PATH];

                if(m_oStr->Find(szSearch, _T(".exe")) != -1)
                {
                    // get the friendly name
                    m_oStr->StringCopy(szFriendlyName, wfd.cFileName);
                    iIndex		= m_oStr->Find(szFriendlyName, _T(".exe"));
                    int iLen	= m_oStr->GetLength(szFriendlyName);
                    if(iIndex != -1)
                        m_oStr->Delete(iIndex, 4, szFriendlyName);

                }
                else if(m_oStr->Find(szSearch, _T(".lnk")) != -1)
                {
                    // special case
                    if(m_oStr->CompareNoCase(wfd.cFileName, _T("icon.lnk"))==0)
                        continue;
                    else if(m_oStr->CompareNoCase(wfd.cFileName, _T("Panoramic Launchpad.lnk"))==0)
                        continue;

                    // get the friendly name
                    m_oStr->StringCopy(szFriendlyName, wfd.cFileName);
                    iIndex		= m_oStr->Find(szFriendlyName, _T(".lnk"));
                    int iLen	= m_oStr->GetLength(szFriendlyName);
                    if(iIndex != -1)
                        m_oStr->Delete(iIndex, 4, szFriendlyName);
                }
                else 
                {
                    continue;
                }
                
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
                m_oMenu->AddItem((LPVOID)sItem, IDMENU_SelectItem);

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
    if(sItem->iIconIndex != -1)
        ImageList_Draw(m_hImageList, sItem->iIconIndex, gdi, rcDraw.left + INDENT/2, rcDraw.top + INDENT/2, ILD_TRANSPARENT);

    rcDraw.left += (GetSystemMetrics(SM_CXICON) + INDENT);

    DrawTextShadow(gdi, sItem->szFriendlyName, rcDraw, DT_LEFT|DT_VCENTER|DT_NOPREFIX|DT_END_ELLIPSIS, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
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

    return oStr->CompareNoCase(sLink1->szFriendlyName, sLink2->szFriendlyName);
}

TCHAR* CDlgChoosePrograms::GetPath()
{
	if(!m_oMenu)
		return NULL;

    TypeItems* sItem = m_oMenu->GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return NULL;

    TypeLaunchItem* sLaunch = (TypeLaunchItem*)sItem->lpItem;
    return sLaunch->szPath;
}

TCHAR* CDlgChoosePrograms::GetFriendlyName()
{
	if(!m_oMenu)
		return NULL;

    TypeItems* sItem = m_oMenu->GetSelectedItem();
    if(!sItem || !sItem->lpItem)
        return NULL;

    TypeLaunchItem* sLaunch = (TypeLaunchItem*)sItem->lpItem;
    return sLaunch->szFriendlyName;
}



void CDlgChoosePrograms::ScrollTo(TCHAR* szText)
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
        if(!sItem)
            return;

        if(sItem->eType == ITEM_Unknown)//probably it
        {
            TypeLaunchItem* sLaunch = (TypeLaunchItem*)sItem->lpItem;

            if(sLaunch == NULL)
                return; //something bad happened


            int iFind = m_oStr->FindNoCase(sLaunch->szFriendlyName, szText, 0);
            if(0 == iFind)
            {
                iScrollToPos = iEntryPos;    
                break;
            }
            if(0 < m_oStr->CompareNoCase(sLaunch->szFriendlyName, szText))
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

BOOL CDlgChoosePrograms::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
        m_oStr->Empty(m_szSearch);
    return CDlgBase::OnActivate(hWnd, wParam, lParam);
}