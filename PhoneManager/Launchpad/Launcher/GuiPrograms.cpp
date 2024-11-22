#include "StdAfx.h"
#include "resource.h"
#include "GuiPrograms.h"
#include "IssCommon.h"
#include "IssRect.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "GuiToolbar.h"
#include "ContactsGuiDefines.h"
#include "issdebug.h"
#include "IssLocalisation.h"

#include "ObjGui.h"
#include "DlgOptions.h"
#include "GuiToolBar.h"

extern CObjGui*				g_gui;
extern TypeOptions*			g_sOptions;
extern CGuiToolBar*			g_guiToolbar;

#define UND				    -1

#define IDT_TIMER_Search                WM_USER + 200
#define Search_Timeout                  1000

#ifdef DEBUG
//#define DEBUG_CONTACTS
#endif

#define IDMENU_Select	    5000
#define IDMENU_SelectBack	5001
#define ITEM_Size           (GetSystemMetrics(SM_CXICON) + INDENT)

TypeLink::TypeLink()
:szName(NULL)
,szPath(NULL)
,iIconIndex(0)
,eLinkType(LINK_Normal)
{}

TypeLink::~TypeLink()
{
	CIssString* oStr = CIssString::Instance();
	oStr->Delete(&szName);
	oStr->Delete(&szPath);
}

CGuiPrograms::CGuiPrograms(void) 
:m_bCompactView(FALSE)
,m_bShowSearch(FALSE)
{
    
	m_ptScreenSize.x		= 0;
	m_ptScreenSize.y		= 0;

	SHGetSpecialFolderPath(NULL, m_szDir, CSIDL_PROGRAMS, FALSE);
	SHGetSpecialFolderPath(NULL, m_szStartDir, CSIDL_PROGRAMS, FALSE);

	SHFILEINFO sfi = {0};

    if(m_bCompactView)
        m_hImageList = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
            SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
    else
	    m_hImageList = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
			SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);
	
}

CGuiPrograms::~CGuiPrograms(void)
{
	EraseList();
}

BOOL CGuiPrograms::Init(HWND hWndParent, HINSTANCE hInst)
{
	CGuiBase::Init(hWndParent, hInst);

    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, g_gui->GetSkin().crSelected);
    Initialize(hWndParent, hWndParent, m_hInst, 0);
    SetCustomDrawFunc(DrawListItem, this);
    //SetImageArray(g_gui->GetSkin().uiImageArray);
	SetSelected(g_gui->GetSkin().uiSelector);

    // preload list images
	PreloadImages(hWndParent, m_hInst);

    if(m_gdiBackArrow.GetDC() == NULL)
    { 
        m_gdiBackArrow.LoadImage((IsVGA()?IDR_FOLDER_Arrow_Flip_VGA:IDR_FOLDER_Arrow_Flip), hWndParent, hInst, TRUE);
    }

    if(m_gdiForwardArrow.GetDC() == NULL)
    {
        m_gdiForwardArrow.LoadImage((IsVGA()?IDR_FOLDER_Arrow_VGA:IDR_FOLDER_Arrow), hWndParent, hInst, TRUE);
    }

    SetDeleteItemFunc(DeleteMyItem);

    RefreshLinks();
	return TRUE;
}

void CGuiPrograms::ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance)
{
    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, g_gui->GetSkin().crSelected);
    SetImageArray(g_gui->GetSkin().uiImageArray);
	SetSelected(g_gui->GetSkin().uiSelector);

    // preload list images
    PreloadImages(hWnd, hInstance);
}
    
BOOL CGuiPrograms::HasFocus()
{
    return FALSE;
}


BOOL CGuiPrograms::Draw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    CIssKineticList::OnDraw(gdi, rcClient, rcClip);

    DrawSearch(gdi, rcClip);

	return TRUE;
}

void CGuiPrograms::DrawSearch(CIssGDIEx& gdi, RECT& rcDraw)
{
    if(m_bShowSearch == FALSE)
        return;

    DrawTextShadow(gdi, m_szSearch, m_rcLocation, DT_CENTER|DT_VCENTER, g_gui->GetFontSearch(), RGB(150,150,150), 0);
}
	
    
/*void CGuiPrograms::DrawSelector(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem)
{
    if(sItem->lpItem)
    {
        TypeLink* sLink = (TypeLink*)sItem->lpItem;
        if(sLink->eLinkType == LINK_MRU && m_oMRU.GetLinkCount())
        {
            RECT rc = rcDraw;
            rc.left     = rcDraw.left + m_iCurMRU*ITEM_Size;
            rc.right    = rc.left + ITEM_Size;
            FillRect(gdi, rc, RGB(150,150,150));
            return;
        }
    }
    CIssKineticList::DrawSelector(gdi, rcDraw, sItem);
}*/

void CGuiPrograms::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeLink* sLink, BOOL bIsHighlighted)
{
    /*if(sLink->eLinkType == LINK_MRU)
    {
        DrawMRUList(gdi, rcDraw, sLink, bIsHighlighted);
        return;
    }*/

    if(sLink->szPath)
        ImageList_Draw(m_hImageList, sLink->iIconIndex, gdi, rcDraw.left + INDENT, rcDraw.top + (HEIGHT(rcDraw)-GetSystemMetrics(SM_CYICON))/2, ILD_TRANSPARENT);
    else
        ::Draw(gdi, 
                rcDraw.left + INDENT,rcDraw.top + (HEIGHT(rcDraw)-m_gdiBackArrow.GetHeight())/2,
                m_gdiBackArrow.GetWidth()/2, m_gdiBackArrow.GetHeight(),
                m_gdiBackArrow,
                0/*(bIsHighlighted?0:m_gdiBackArrow.GetWidth()/2)*/,//backwards because of the flip
                0);

	RECT rc = rcDraw;
	rc.left	= rc.left + 2*INDENT + GetSystemMetrics(SM_CXICON);
    if(sLink->eLinkType == LINK_Directory)
    {
        rc.right    = rcDraw.right - m_gdiForwardArrow.GetWidth()/2 - 2*INDENT;
        ::Draw(gdi, 
               rc.right,rcDraw.top + (HEIGHT(rcDraw)-m_gdiForwardArrow.GetHeight())/2,
               m_gdiForwardArrow.GetWidth()/2, m_gdiForwardArrow.GetHeight(),
               m_gdiForwardArrow,
               0/*(bIsHighlighted?0:m_gdiForwardArrow.GetWidth()/2)*/,0);
        
    }
	DrawTextShadow(gdi, sLink->szName, rc, DT_LEFT|DT_VCENTER|DT_NOPREFIX|DT_END_ELLIPSIS, g_gui->GetFontLetters(), g_gui->GetSkin().crDrawText , RGB(0,0,0));
}

void CGuiPrograms::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CGuiPrograms* pPrograms = (CGuiPrograms*)lpClass;
	if(!sItem || !sItem->lpItem)
		return;
	TypeLink* sLink = (TypeLink*)sItem->lpItem;
    pPrograms->DrawListItem(gdi, rcDraw, sLink, bIsHighlighted);
}

BOOL CGuiPrograms::OnTimer(WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDT_TIMER_Search)
    {
        KillTimer(m_hWndParent, IDT_TIMER_Search);
        m_oStr->Empty(m_szSearch);
        m_bShowSearch = FALSE;
        //could fade out here ....
        InvalidateRect(m_hWndParent, NULL, FALSE);
        return TRUE;
    }
    return CIssKineticList::OnTimer(wParam, lParam);
}

BOOL CGuiPrograms::MoveGui(RECT rcLocation)
{
	CGuiBase::MoveGui(rcLocation);

    DBG_CONTACTS(_T("CGuiPrograms::MoveGui()"), TRUE);

    m_rcContactList = /*m_rcAlpha =*/ rcLocation;
    //int iAlphaWidth = IsVGA() ? CONTACTS_ALPHA_WIDTH*2 : CONTACTS_ALPHA_WIDTH;
    //m_rcAlpha.left = m_rcAlpha.right - iAlphaWidth;
    //m_rcContactList.right = m_rcAlpha.left;
    
	//int iListInset = (IsVGA() ? CONTACTS_LIST_INSET * 2 : CONTACTS_LIST_INSET);
    int iListInset = 0;
	CIssKineticList::OnSize(m_rcContactList.left + iListInset, 
                            rcLocation.top + iListInset/2, 
                            WIDTH(m_rcContactList) - iListInset*2, 
                            HEIGHT(m_rcContactList) - iListInset/2
                            );

    DBG_CONTACTS(_T("CGuiPrograms::MoveGui()"), FALSE);
	return TRUE;
}

BOOL CGuiPrograms::OnLButtonDown(POINT pt)
{
	return CIssKineticList::OnLButtonDown(pt);
}

BOOL CGuiPrograms::OnLButtonUp(POINT pt)
{
    return CIssKineticList::OnLButtonUp(pt);
}

BOOL CGuiPrograms::OnMouseMove(POINT pt)
{
    return CIssKineticList::OnMouseMove(pt);
}

BOOL CGuiPrograms::OnChar(WPARAM wParam, LPARAM lParam)
{
    if(g_sOptions->bShowSettingsHeaders == TRUE)
        return TRUE;// no text scrolling for that

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
        KillTimer(m_hWndParent, IDT_TIMER_Search);
        SetTimer(m_hWndParent, IDT_TIMER_Search, Search_Timeout, NULL);
        iLen++;

        m_bShowSearch = TRUE;
        InvalidateRect(m_hWndParent, NULL, FALSE);
        ScrollTo(m_szSearch);
    }
    //backspace I think
    if(c == VK_BACK && iLen > 0)
    {
        m_szSearch[iLen-1] = m_szSearch[iLen];
        m_szSearch[iLen] = _T('\0');
        iLen--;
        KillTimer(m_hWndParent, IDT_TIMER_Search);
        SetTimer(m_hWndParent, IDT_TIMER_Search, Search_Timeout, NULL);

        m_bShowSearch = TRUE;
        InvalidateRect(m_hWndParent, NULL, FALSE);
        ScrollTo(m_szSearch);
    }

    return UNHANDLED;

}

BOOL CGuiPrograms::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return CIssKineticList::OnKeyDown(wParam, lParam);
}

BOOL CGuiPrograms::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}
   
void CGuiPrograms::RefreshLinks(void)
{
	EraseList();
	PopulateList(m_szDir);

    m_arrItems.Sort(CompareLinks);

    
    SetSelectedItemIndex(2, TRUE);
    SetItemHeights(ITEM_Size, ITEM_Size);
}

void CGuiPrograms::PopulateList(TCHAR* szPath, BOOL bAddDirectories)
{
    m_oStr->Empty(m_szSearch);

	if(0 == m_oStr->Compare(szPath, m_szStartDir))
	{
		TCHAR szMenu[MAX_PATH];
		SHGetSpecialFolderPath(NULL, szMenu, CSIDL_STARTMENU, FALSE);
		PopulateList(szMenu, FALSE);
	}	
    else if(bAddDirectories)
    {
        // add the Back item
        TypeLink* sLink = new TypeLink;
        sLink->szName = m_oStr->CreateAndCopy(ID(IDS_Back), m_hInst);
        AddItem((LPVOID)sLink, IDMENU_SelectBack);        
    }

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
				if(!bAddDirectories && 0 == m_oStr->CompareNoCase(m_szStartDir, szSearch))
					continue;

                // there has to be some actual files in the directory before we add it
                if(!IsLinksInDir(szSearch))
                    continue;

				TypeLink* sLink = new TypeLink;

				SHFILEINFO sfi = {0};
				// get the icon index
				if (SHGetFileInfo(szSearch, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ATTRIBUTES))
					sLink->iIconIndex = sfi.iIcon;

				sLink->eLinkType = LINK_Directory;
				sLink->szName	= m_oStr->CreateAndCopy(wfd.cFileName);
				sLink->szPath	= m_oStr->CreateAndCopy(szSearch);

				AddItem((LPVOID)sLink, IDMENU_Select);
			}
			else
			{
				// we're only looking for .lnk & .exe files
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
                
				TypeLink* sLink = new TypeLink;

				SHFILEINFO sfi = {0};
				// get the icon index
				if (SHGetFileInfo(szSearch, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ATTRIBUTES))
					sLink->iIconIndex = sfi.iIcon;

				sLink->szName	= m_oStr->CreateAndCopy(szFriendlyName);
				sLink->szPath	= m_oStr->CreateAndCopy(szSearch);

				// add to our vector
				AddItem((LPVOID)sLink, IDMENU_Select);

			}
		} while(FindNextFile(hFind, &wfd));
		FindClose(hFind);
	} 

    DWORD dwFlags   = 0;
    if(GetItemCount() > 15)
        dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar|OPTION_Bounce);
    else
        dwFlags |= OPTION_DrawScrollArrows | OPTION_Bounce;
    Initialize(m_hWndParent, m_hWndParent, m_hInst, dwFlags, TRUE);
}

void CGuiPrograms::EraseList()
{
    ResetContent();
}


BOOL CGuiPrograms::AddMenuItems()
{
//	m_wndMenu->AddItem(IDS_MENU_NewContact, m_hInst, IDMENU_NewContact);
//	m_wndMenu->AddItem(IDS_MENU_EditContact, m_hInst, IDMENU_EditContact);
//	m_wndMenu->AddItem(IDS_MENU_DeleteContact, m_hInst, IDMENU_DeleteContact);

	return TRUE;
}

BOOL CGuiPrograms::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case IDMENU_Select:
		{
			TypeItems* sItem = GetSelectedItem();
			if(!sItem || !sItem->lpItem)
				return TRUE;

			TypeLink* sLink = (TypeLink*)sItem->lpItem;

			if(sLink->eLinkType == LINK_Directory)
			{
				m_oStr->StringCopy(m_szDir, sLink->szPath);
				AnimateDirChange(FALSE);
			}
			else if(sLink->eLinkType == LINK_Normal)
			{
                SHELLEXECUTEINFO sei = {0};
                sei.cbSize	= sizeof(sei);
                sei.hwnd	= m_hWndParent;
                sei.nShow	= SW_SHOWNORMAL;
                sei.lpFile = sLink->szPath;

                // give back taskbar so user can close the outlook window
                //SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
                if(g_sOptions->bHideOnAppLaunch)
                    ShowWindow(m_hWndParent, SW_MINIMIZE);

                ShellExecuteEx(&sei);

			}

		}
		break;
    case IDMENU_SelectBack:
        {
            int iIndex = m_oStr->FindLastOccurance(m_szDir, _T("\\"));
            if(iIndex == -1)
                return TRUE;

            // take off the last directory
            m_oStr->Delete(iIndex, m_oStr->GetLength(m_szDir)-iIndex, m_szDir);

            TCHAR szStartMenu[MAX_PATH];
            SHGetSpecialFolderPath(NULL, szStartMenu, CSIDL_STARTMENU, FALSE);

            // if this is the start menu then just go to the programs menu
            if(0 == m_oStr->CompareNoCase(m_szDir, szStartMenu))
                m_oStr->StringCopy(m_szDir, m_szStartDir);

            AnimateDirChange(TRUE);

        }
        break;
	default:
		return UNHANDLED;
	}

	return TRUE;
}

int CGuiPrograms::CompareLinks( const void *arg1, const void *arg2 )
{
    CIssString* oStr = CIssString::Instance();

    TypeItems& sListItem1 = **(TypeItems **)arg1;
    TypeItems& sListItem2 = **(TypeItems **)arg2;
    if(!sListItem1.lpItem || !sListItem2.lpItem)
        return 0;

    TypeLink* sLink1    = (TypeLink*)sListItem1.lpItem;
    TypeLink* sLink2    = (TypeLink*)sListItem2.lpItem;

    /*if(sLink1->eLinkType == LINK_MRU)
        return -1;
    else if(sLink2->eLinkType == LINK_MRU)
        return 1;
    else if(sListItem1.eType == ITEM_Separator)
        return -1;
    else if(sListItem2.eType == ITEM_Separator)
        return 1;
    else */if(sListItem1.uiMenuID == IDMENU_SelectBack)
        return -1;
    else if(sListItem2.uiMenuID == IDMENU_SelectBack)
        return 1;
    else if(sLink1->eLinkType == sLink2->eLinkType)
        return oStr->CompareNoCase(sLink1->szName, sLink2->szName);
    else if(sLink1->eLinkType == LINK_Directory)
        return -1;
    else 
        return 1;
}

void CGuiPrograms::AnimateDirChange(BOOL bAnimateRight)
{
    // let's just draw both pages and slide it
    // we also have to draw the selector overlap

    HDC dc = GetDC(m_hWndParent);

    CIssGDIEx gdiTemp;
    gdiTemp.Create(dc, WIDTH(m_rcLocation)*2, HEIGHT(m_rcLocation));

    RECT rcAll = {0,0,gdiTemp.GetWidth(), gdiTemp.GetHeight()};
    RECT rc1, rc2, rcStoredBorder, rcNewLoc, rcStoredLoc;
    rc1 = rc2 = rcAll;
    rc1.right = rc2.left = WIDTH(rcAll)/2;

    rcStoredBorder = m_rcContactList;
    rcStoredLoc = m_rcLoc;
    rcNewLoc = m_rcLoc;

    OffsetRect(&rcNewLoc, -rcStoredBorder.left, 0);

	CIssGDIEx* gdiBackground = g_gui->GetBackground();

	RECT rcBackground = g_gui->RectScreen();

	::Draw(gdiTemp, rc1, *gdiBackground, rcBackground.left, rcBackground.top);
	::Draw(gdiTemp, rc2, *gdiBackground, rcBackground.left, rcBackground.top);
    
    OffsetRect(&rcNewLoc, bAnimateRight?rc2.left:0, 0);
    CIssKineticList::OnSize(rcNewLoc.left, rcNewLoc.top, WIDTH(rcNewLoc), HEIGHT(rcNewLoc));
    CIssKineticList::OnDraw(gdiTemp, bAnimateRight?rc2:rc1, bAnimateRight?rc2:rc1);

    RefreshLinks();

    OffsetRect(&rcNewLoc, bAnimateRight?-rc2.left:rc2.left, 0);
    CIssKineticList::OnSize(rcNewLoc.left, rcNewLoc.top, WIDTH(rcNewLoc), HEIGHT(rcNewLoc));
    CIssKineticList::OnDraw(gdiTemp, bAnimateRight?rc1:rc2, bAnimateRight?rc1:rc2);

    m_rcContactList = rcStoredBorder;
    CIssKineticList::OnSize(rcStoredLoc.left, rcStoredLoc.top, WIDTH(rcStoredLoc), HEIGHT(rcStoredLoc));


    DWORD dwStart = GetTickCount();
    BOOL bDec = bAnimateRight;

    int iNumFrames = WIDTH(rc1);

	CIssGDIEx* gdi = g_gui->GetGDI();

    float fAnimateTime = FAVORITES_PAGE_ANIMATE_TIME;
    int i = GetCurrentFrame(dwStart, iNumFrames, fAnimateTime);
    int iStartFrame = bDec ? rc1.right : 0;
    while(i < iNumFrames)
    {
#ifdef DEBUG
        //i += 10;
        i = GetCurrentFrame(dwStart, iNumFrames, fAnimateTime);
#else
        i = GetCurrentFrame(dwStart, iNumFrames, fAnimateTime);
#endif
        int iAmount = i;
        if(bDec)
            iAmount = -iAmount;

        // gdiTemp to offscreen
        BitBlt(gdi->GetDC(),
            m_rcLocation.left,m_rcLocation.top,
            WIDTH(m_rcLocation), HEIGHT(m_rcLocation),
            gdiTemp.GetDC(),
            iStartFrame + iAmount, 0,
            SRCCOPY);

        // selector
		g_guiToolbar->Draw(*gdi, m_rcLocation, m_rcLocation);

        // all to screen
        BitBlt(dc,
            m_rcLocation.left,m_rcLocation.top,
            WIDTH(m_rcLocation), HEIGHT(m_rcLocation),
            gdi->GetDC(),
            m_rcLocation.left,m_rcLocation.top,
            SRCCOPY);

    }
    ReleaseDC(m_hWndParent, dc);

    InvalidateRect(m_hWndParent, &m_rcLocation, FALSE);
}

BOOL CGuiPrograms::IsLinksInDir(TCHAR* szDir)
{
    TCHAR szSearch[MAX_PATH];

    m_oStr->StringCopy(szSearch, szDir);
    if(szSearch[m_oStr->GetLength(szSearch)-1] != _T('\\'))
        m_oStr->Concatenate(szSearch, _T("\\"));
    m_oStr->Concatenate(szSearch, _T("*.lnk"));

    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(szSearch, &wfd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return TRUE;
    }
    return FALSE;
}

void CGuiPrograms::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    CIssString* oStr = CIssString::Instance();

    TypeLink* sLink = (TypeLink*)lpItem;

    if(sLink)
    {
        if(sLink->szName)
            oStr->Delete(&sLink->szName);
        if(sLink->szPath)
            oStr->Delete(&sLink->szPath);
    }

    delete sLink;
}

void CGuiPrograms::ScrollTo(TCHAR* szText)
{
    TypeItems* sItem = NULL;

    int iEntryPos = 0;
    int iScrollToPos = 0;

    //find the scroll position
    //  for(; iEntryPos < m_arrItems.GetSize(); iEntryPos++)
    //find the scroll position
    for(; iEntryPos < GetItemCount(); iEntryPos++)
    {
        sItem = m_arrItems[iEntryPos];
        if(!sItem)
            return;

        if(sItem->eType == ITEM_Unknown)//probably it
        {
            TypeLink* sLink = NULL;
            sLink = (TypeLink*)sItem->lpItem;

            if(sLink == NULL || sLink->szName == NULL || sLink->eLinkType != LINK_Normal)
                continue;

            int iFind = m_oStr->FindNoCase(sLink->szName, szText, 0);
            if(0 == iFind)
            {
                iScrollToPos = iEntryPos;    
                break;
            }
            if(0 < m_oStr->CompareNoCase(sLink->szName, szText))
                break;

            iScrollToPos = iEntryPos;
        }
    }

    //otherwise do nothing
    //Error:

    if(iScrollToPos == GetSelectedItemIndex())
        return;

    SetSelectedItemIndex(iScrollToPos, TRUE);
    ShowSelectedItem(TRUE);
}
