#include "StdAfx.h"
#include "resource.h"
#include "GuiContacts.h"
#include "IssCommon.h"
#include "IssRect.h"
#include "DlgContactDetails.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "GuiToolbar.h"
#include "DlgContacts.h"
#include "ContactsGuiDefines.h"
#include "issdebug.h"

#define UND				-1

#ifdef DEBUG
//#define DEBUG_CONTACTS
#endif

#define IDMENU_Select	    5000
#define IDMENU_SelectBack	5001
#define ITEM_Size           (GetSystemMetrics(SM_CYICON) + INDENT)

EnumSortContacts g_eSort;


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

CGuiContacts::CGuiContacts(void) 
:m_iCurMRU(0)
,m_iNumMRU(0)
{
	m_iLetterHeight			= GetSystemMetrics(SM_CXICON)/2;
	m_iTitleHeight			= (int)(m_iLetterHeight * 1.5);
	m_iContactHeight		= GetSystemMetrics(SM_CXICON)/2;
	m_iContactHeightSelected = m_iContactHeight + m_iLetterHeight;  // try this for now
	m_iContactPicHeight		= GetSystemMetrics(SM_CXICON)*3/2;

	m_hFontLetters			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
	m_hFontContact			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
 	m_hFontContactBold		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);

	m_ptScreenSize.x		= 0;
	m_ptScreenSize.y		= 0;

	SHGetSpecialFolderPath(NULL, m_szDir, CSIDL_PROGRAMS, FALSE);
	SHGetSpecialFolderPath(NULL, m_szStartDir, CSIDL_PROGRAMS, FALSE);

	SHFILEINFO sfi = {0};
	m_hImageList = (HIMAGELIST) SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO),
											  SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);
	
}

CGuiContacts::~CGuiContacts(void)
{
	CIssGDIEx::DeleteFont(m_hFontLetters);
	CIssGDIEx::DeleteFont(m_hFontContact);
	CIssGDIEx::DeleteFont(m_hFontContactBold);
    //m_hImgFontAlpha.Destroy();
	EraseList();
}

BOOL CGuiContacts::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContactDetails* dlgContactDetails)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBg, guiBackground, sOptions);

    m_dlgContactDetails = dlgContactDetails;

    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    Initialize(hWndParent, hWndParent, m_hInst, 0);
    SetCustomDrawFunc(DrawListItem, this);
    SetImageArray(SKIN(IDR_PNG_MenuArray));
	SetSelected(IDR_PNG_ContactsSelector);

    // preload list images
	PreloadImages(hWndParent, m_hInst);

    if(m_gdiBackArrow.GetDC() == NULL)
        m_gdiBackArrow.LoadImage(SKIN(IDR_PNG_DetailsBackGreen), hWndParent, hInst, TRUE);

    if(m_gdiForwardArrow.GetDC() == NULL)
    {
        m_gdiForwardArrow.LoadImage(SKIN(IDR_PNG_DetailsBackGreen), hWndParent, hInst, TRUE);
        m_gdiForwardArrow.FlipVertical();
    }

	RefreshLinks();
	return TRUE;
}

void CGuiContacts::ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance)
{
    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    SetImageArray(SKIN(IDR_PNG_MenuArray));

    // preload list images
    PreloadImages(hWnd, hInstance);
}
    
BOOL CGuiContacts::HasFocus()
{
    return !CGuiToolBar::GetToolbar()->HasFocus();
}

CIssImageSliced& CGuiContacts::GetBorder()
{
    if(!m_imgBorder.IsLoaded())
        m_imgBorder.Initialize(m_hWndParent, m_hInst, IsVGA() ? IDR_PNG_ContactsBorderVGA : IDR_PNG_ContactsBorder);

    if(WIDTH(m_rcContactList) != m_imgBorder.GetWidth() || HEIGHT(m_rcContactList) != m_imgBorder.GetHeight())
        m_imgBorder.SetSize(WIDTH(m_rcContactList), HEIGHT(m_rcContactList));

    return m_imgBorder;
}

/*CIssGDIEx& CGuiContacts::GetAlphaPopupGDI()
{
    if(!m_gdiAlphaPopUp.GetDC())
    {
        if(IsVGA())
        {
            CIssGDIEx gdiTemp;
            gdiTemp.LoadImage(IDR_PNG_ContactsLetterBubble, m_hWndParent, m_hInst, TRUE);
            SIZE sz;
            sz.cx   = gdiTemp.GetWidth()*2;
            sz.cy   = gdiTemp.GetHeight()*2;
            ScaleImage(gdiTemp, m_gdiAlphaPopUp, sz, FALSE, 0);
        }
        else
        {
            m_gdiAlphaPopUp.LoadImage(IDR_PNG_ContactsLetterBubble, m_hWndParent, m_hInst, TRUE);
        }
    }

    return m_gdiAlphaPopUp;
}
    
CIssImgFont& CGuiContacts::GetAlphaFont()
{
    if(!m_hImgFontAlpha.IsLoaded())
    {
        int h = GetSystemMetrics(SM_CXICON)*4/5;
        UINT uiRes = IsVGA() ? IDR_PNG_FontVGA : IDR_PNG_Font;
        m_hImgFontAlpha.Initialize(uiRes, m_hWndParent, m_hInst, -1);
    }

    return m_hImgFontAlpha;
}*/

BOOL CGuiContacts::Draw(HDC hDC, RECT& rcClient, RECT& rcClip)
{
    if(!IsRectInRect(m_rcLocation, rcClip))
        return TRUE;

    DBG_CONTACTS(_T("CGuiContacts::Draw()"), TRUE);
    if(m_gdiBg->GetDC() == NULL)
    {
        RECT rc;
        ::GetWindowRect(m_hWndParent, &rc);
		m_gdiBg->Create(hDC, rc);
    }

    //Render();
	DrawBackground(m_gdiBg->GetDC(), m_rcLocation, rcClip);
    CIssKineticList::OnDraw(*m_gdiBg, rcClient, rcClip);

    // draw the top part of the border only here
    DrawBorderOverlay(*m_gdiBg, rcClient, rcClip);

    RECT rcBlit = rcClip;
    rcBlit.bottom = min(rcBlit.bottom, m_rcLocation.bottom);
    rcBlit.left = max(rcBlit.left, m_rcLocation.left);
    BitBlt(hDC,
		rcBlit.left,rcBlit.top,
		WIDTH(rcBlit), HEIGHT(rcBlit),
		m_gdiBg->GetDC(),
		rcBlit.left,rcBlit.top,
		SRCCOPY);

    DBG_CONTACTS(_T("CGuiContacts::Draw()"), FALSE);
	return TRUE;
}
	
BOOL CGuiContacts::DrawBorderOverlay(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rcBorder = m_rcContactList;
    rcBorder.bottom = rcBorder.top + CONTACTS_LIST_INSET;
    if(!IsRectInRect(rcBorder, rcClip))
        return TRUE;

    ::Draw(gdi, rcBorder, GetBorder().GetImage());

    return TRUE;
}
    
CIssGDIEx& CGuiContacts::GetBgGDI()
{
	if(m_gdiBackground.GetDC() == NULL || m_gdiBackground.GetWidth() != WIDTH(m_rcLocation) ||
        m_gdiBackground.GetHeight() != HEIGHT(m_rcLocation))
	{
		m_gdiBackground.Create(m_gdiMem->GetDC(), m_rcLocation/*rcClient*/, FALSE);

        // background gradients
        RECT rcGDI = {0,0,m_gdiBackground.GetWidth(), m_gdiBackground.GetHeight()};
        RECT rc1, rc2;
        rc1 = rc2 = rcGDI;
        rc1.right = rc1.left + (long)(WIDTH(m_rcContactList/*rcClient*/) * 0.70f);
        rc2.left = rc1.right;
        
        GradientFillRect(m_gdiBackground, rc1, COLOR_CONTACTS_BG_GRAD1_ST, COLOR_CONTACTS_BG_GRAD1_END, FALSE);
        GradientFillRect(m_gdiBackground, rc2, COLOR_CONTACTS_BG_GRAD2_ST, COLOR_CONTACTS_BG_GRAD2_END, FALSE);

        /*RECT rcAlphaFrame = m_rcAlpha;
        OffsetRect(&rcAlphaFrame, -m_rcLocation.left, -m_rcLocation.top);
        Rectangle(m_gdiBackground, rcAlphaFrame, COLOR_CONTACTS_ALPHA_BG,COLOR_CONTACTS_ALPHA_BG);

		int iFontSize = HEIGHT(m_rcAlpha)/26 + 2;//11 worked nicely on PPC 

		HFONT hFontAlpha = CIssGDIEx::CreateFont(iFontSize, FW_NORMAL, TRUE);
		m_iAlphaTextSpacing = (iFontSize-2);
		m_iAlphaWidth = m_iAlphaTextSpacing*5/3;
		int iTotalSize = m_iAlphaTextSpacing*26;
		int iStart = (m_rcLocation.bottom - m_rcLocation.top)/2 - iTotalSize/2 + m_rcLocation.top;

		TCHAR szLetter[2];
		szLetter[1] = _T('\0');

        m_rcAlphaText = m_rcAlpha;
		m_rcAlphaText.top = iStart;
        m_rcAlphaText.bottom -= (iStart - m_rcAlpha.top);

		RECT rcAlpha = rcAlphaFrame;
		rcAlpha.top		 = iStart;
		rcAlpha.bottom	 = rcAlpha.top + m_iAlphaTextSpacing;

		//Time to draw the Alphabet
		for(int i = 0; i < 26; i++)
		{
			szLetter[0] = _T('A')+i;
			DrawText(m_gdiBackground.GetDC(), szLetter, rcAlpha, DT_CENTER | DT_TOP, hFontAlpha, 0);
			rcAlpha.top += m_iAlphaTextSpacing;
			rcAlpha.bottom += m_iAlphaTextSpacing;
		}
		CIssGDIEx::DeleteFont(hFontAlpha);*/

        // draw the left and right border parts
        RECT rcBorder = m_rcContactList;
        rcBorder.top += CONTACTS_LIST_INSET;
        ::Draw(m_gdiBackground, rcBorder, GetBorder().GetImage(), rcBorder.left, rcBorder.top);
	}
    return m_gdiBackground;
}

BOOL CGuiContacts::DrawBackground(HDC hdc, RECT& rcClient, RECT& rcClip)
{

    RECT rc = rcClip;
    rc.bottom = min(rc.bottom, rcClient.bottom);
    rc.left = max(rc.left, rcClient.left);

	return BitBlt(hdc,
		rc.left,rc.top,
		WIDTH(rc), HEIGHT(rc),
		m_gdiBackground.GetDC(),
		rc.left - m_rcLocation.left,rc.top,
		SRCCOPY);
}

void CGuiContacts::DrawSelector(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem)
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
}

void CGuiContacts::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeLink* sLink, BOOL bIsHighlighted)
{
    if(sLink->eLinkType == LINK_MRU)
    {
        DrawMRUList(gdi, rcDraw, sLink, bIsHighlighted);
        return;
    }

    if(sLink->szPath)
        ImageList_Draw(m_hImageList, sLink->iIconIndex, gdi, rcDraw.left + INDENT, rcDraw.top + (HEIGHT(rcDraw)-GetSystemMetrics(SM_CYICON))/2, ILD_TRANSPARENT);
    else
        ::Draw(gdi, 
                rcDraw.left + INDENT,rcDraw.top + (HEIGHT(rcDraw)-m_gdiBackArrow.GetHeight())/2,
                m_gdiBackArrow.GetWidth(), m_gdiBackArrow.GetHeight(),
                m_gdiBackArrow,
                0,0);

	RECT rc = rcDraw;
	rc.left	= rc.left + 2*INDENT + GetSystemMetrics(SM_CXICON);
    if(sLink->eLinkType == LINK_Directory)
    {
        rc.right    = rcDraw.right - m_gdiForwardArrow.GetWidth() - INDENT;
        ::Draw(gdi, 
               rc.right,rcDraw.top + (HEIGHT(rcDraw)-m_gdiForwardArrow.GetHeight())/2,
               m_gdiForwardArrow.GetWidth(), m_gdiForwardArrow.GetHeight(),
               m_gdiForwardArrow,
               0,0);
        
    }
	DrawTextShadow(gdi, sLink->szName, rc, DT_LEFT|DT_VCENTER|DT_NOPREFIX, m_hFontLetters, RGB(255,255,255), RGB(0,0,0));
}

void CGuiContacts::DrawMRUList(CIssGDIEx& gdi, RECT& rcDraw, TypeLink* sLink, BOOL bIsHighlighted)
{
    int iIndent = INDENT/2;
    for(int i=0; i<min(m_iNumMRU, m_oMRU.GetLinkCount()); i++)
    {
        int iIndex = m_oMRU.GetLinkIconIndex(i);
        ImageList_Draw(m_hImageList, 
                       iIndex, gdi, 
                       rcDraw.left + i*ITEM_Size + iIndent, rcDraw.top + iIndent, ILD_TRANSPARENT);
    }
}

void CGuiContacts::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CGuiContacts* pContacts = (CGuiContacts*)lpClass;
	if(!sItem || !sItem->lpItem)
		return;
	TypeLink* sLink = (TypeLink*)sItem->lpItem;
    pContacts->DrawListItem(gdi, rcDraw, sLink, bIsHighlighted);
}

BOOL CGuiContacts::OnTimer(WPARAM wParam, LPARAM lParam)
{
	CIssKineticList::OnTimer(wParam, lParam);

	return UNHANDLED;
}

/*BOOL CGuiContacts::SelecteItem(int iIndex)
{
	TypeItems * sItem = m_arrItems[iIndex];
	if(!sItem || !sItem->lpItem)
		return FALSE;

    TypeLink* sLink = (TypeLink*)sItem->lpItem;

	if(sLink->bDirectory)
	{
		m_oStr->StringCopy(m_szDir, sLink->szPath);
		RefreshContacts();
		InvalidateRect(m_hWndParent, NULL, FALSE);
	}
	else
	{

	}

	return TRUE;
}*/

BOOL CGuiContacts::MoveGui(RECT rcLocation)
{
    if(EqualRect(&rcLocation, &m_rcLocation))
        return TRUE;

	CGuiBase::MoveGui(rcLocation);

    DBG_CONTACTS(_T("CGuiContacts::MoveGui()"), TRUE);

    m_rcContactList = /*m_rcAlpha =*/ rcLocation;
    //int iAlphaWidth = IsVGA() ? CONTACTS_ALPHA_WIDTH*2 : CONTACTS_ALPHA_WIDTH;
    //m_rcAlpha.left = m_rcAlpha.right - iAlphaWidth;
    //m_rcContactList.right = m_rcAlpha.left;
    int iListInset = (IsVGA() ? CONTACTS_LIST_INSET * 2 : CONTACTS_LIST_INSET);
    CIssKineticList::OnSize(m_rcContactList.left + iListInset, 
                            rcLocation.top + iListInset/2, 
                            WIDTH(m_rcContactList) - iListInset*2, 
                            HEIGHT(m_rcContactList) - iListInset/2
                            );

    m_iNumMRU = WIDTH(rcLocation)/ITEM_Size;
    m_iCurMRU = 0;

    // reload images
    GetBgGDI();
    GetBorder();

    DBG_CONTACTS(_T("CGuiContacts::MoveGui()"), FALSE);
	return TRUE;
}

BOOL CGuiContacts::OnLButtonDown(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	m_ptMouseDown = pt;

	return CIssKineticList::OnLButtonDown(pt);
}

BOOL CGuiContacts::OnLButtonUp(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	return CIssKineticList::OnLButtonUp(pt);
}

BOOL CGuiContacts::OnMouseMove(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

    return CIssKineticList::OnMouseMove(pt);
}

BOOL CGuiContacts::OnChar(WPARAM wParam, LPARAM lParam)
{
    TCHAR c = LOWORD(wParam);
	if(iswalpha(c))
        ScrollTo(toupper(c));

	return UNHANDLED;
}

BOOL CGuiContacts::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(HasFocus())
        return CIssKineticList::OnKeyDown(wParam, lParam);

    return FALSE;
}

BOOL CGuiContacts::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}
   
BOOL CGuiContacts::HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    /*switch(uiMessage)
    {
    case PIM_ITEM_CREATED_LOCAL: 
    case PIM_ITEM_CREATED_REMOTE: 
        PoomItemAdded(wParam);
        break;
    case PIM_ITEM_DELETED_LOCAL: 
    case PIM_ITEM_DELETED_REMOTE: 
        PoomItemDeleted(wParam);
        break;
    case PIM_ITEM_CHANGED_LOCAL: 
    case PIM_ITEM_CHANGED_REMOTE: 
        PoomItemChanged(wParam);
        break;
    }*/
    return TRUE;
}

int CGuiContacts::GetNextPageItemIndex(BOOL bSearchUp)
{
    if(m_oMRU.GetLinkCount() == 0)
        return CIssKineticList::GetNextPageItemIndex(bSearchUp);

   TypeItems* sItem = GetSelectedItem();
   if(!sItem || !sItem->lpItem)
       return CIssKineticList::GetNextPageItemIndex(bSearchUp);

    TypeLink* sLink = (TypeLink*)sItem->lpItem;
    if(sLink->eLinkType != LINK_MRU)
        return CIssKineticList::GetNextPageItemIndex(bSearchUp);

    int iItemCount = min(m_iNumMRU, m_oMRU.GetLinkCount());
    if(bSearchUp)   // we're going left
    {
        if(m_iCurMRU == 0)
            return CIssKineticList::GetNextPageItemIndex(bSearchUp);
        m_iCurMRU--;
    }
    else    // going right
    {
        if(m_iCurMRU == iItemCount-1)
            return CIssKineticList::GetNextPageItemIndex(bSearchUp);
        m_iCurMRU++;
    }
    ReDrawList();
    return GetSelectedItemIndex();
}

void CGuiContacts::GrabItem(POINT& pt, TypeItems* sItem)
{
    if(!sItem || !sItem->lpItem)
        return;

    TypeLink* sLink = (TypeLink*)sItem->lpItem;
    if(sLink->eLinkType != LINK_MRU)
        return;

    int iLow, iHi;
    for(int i=0; i<min(m_iNumMRU, m_oMRU.GetLinkCount()); i++)
    {
        iLow    = i*ITEM_Size;
        iHi     = iLow + ITEM_Size;
        if(iLow <= pt.x && iHi >= pt.x)
        {
            m_iCurMRU = i;
            break;
        }
    }
}




void CGuiContacts::RefreshLinks(void)
{
	EraseList();
	PopulateList(m_szDir);

    m_arrItems.Sort(CompareLinks);

    m_iCurMRU = 0;
    m_oMRU.LoadRegistry();
    TypeLink* sMRU = new TypeLink;
    if(sMRU)
    {
        sMRU->eLinkType = LINK_MRU;
        AddItem(sMRU, IDMENU_Select);
        m_arrItems.Move(m_arrItems.GetSize()-1, 0);
    }
    AddSeparator();
    m_arrItems.Move(m_arrItems.GetSize()-1, 1);
    
    SetSelectedItemIndex(2, TRUE);
    SetItemHeights(ITEM_Size, ITEM_Size);
}

void CGuiContacts::PopulateList(TCHAR* szPath, BOOL bAddDirectories)
{
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
        sLink->szName = m_oStr->CreateAndCopy(_T("Back"));
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
}

void CGuiContacts::EraseList()
{
    ResetContent();
}


BOOL CGuiContacts::AddMenuItems()
{
	m_wndMenu->AddItem(IDS_MENU_NewContact, m_hInst, IDMENU_NewContact);
	m_wndMenu->AddItem(IDS_MENU_EditContact, m_hInst, IDMENU_EditContact);
	m_wndMenu->AddItem(IDS_MENU_DeleteContact, m_hInst, IDMENU_DeleteContact);

	return TRUE;
}

BOOL CGuiContacts::OnCommand(WPARAM wParam, LPARAM lParam)
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

                // add it to the MRU
                m_oMRU.AddLink(sLink->szPath);

                // give back taskbar so user can close the outlook window
                SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);

                ShellExecuteEx(&sei);

			}
            else
            {
                TCHAR* szPath = m_oMRU.GetLink(m_iCurMRU);
                if(!szPath)
                    return TRUE;

                SHELLEXECUTEINFO sei = {0};
                sei.cbSize	= sizeof(sei);
                sei.hwnd	= m_hWndParent;
                sei.nShow	= SW_SHOWNORMAL;
                sei.lpFile = szPath;

                // give back taskbar so user can close the outlook window
                SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);

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

int CGuiContacts::CompareLinks( const void *arg1, const void *arg2 )
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
        return oStr->Compare(sLink1->szName, sLink2->szName);
    else if(sLink1->eLinkType == LINK_Directory)
        return -1;
    else 
        return 1;
}

void CGuiContacts::AnimateDirChange(BOOL bAnimateRight)
{
    // let's just draw both pages and slide it
    // we also have to draw the selector overlap

    HDC dc = GetDC(m_hWndParent);

    m_rcContactList;

    CIssGDIEx gdiTemp;
    gdiTemp.Create(dc, WIDTH(m_rcLocation)*2, HEIGHT(m_rcLocation));

    DrawBackground(m_gdiBg->GetDC(), m_rcLocation, m_rcLocation);
    CIssKineticList::OnDraw(*m_gdiBg, m_rcLocation, m_rcLocation);
    DrawBorderOverlay(*m_gdiBg, m_rcLocation, m_rcLocation);
    ::BitBlt(gdiTemp, (bAnimateRight?WIDTH(m_rcLocation):0),0, WIDTH(m_rcLocation), HEIGHT(m_rcLocation), m_gdiBg->GetDC(), 0,0,SRCCOPY);

    RefreshLinks();

    DrawBackground(m_gdiBg->GetDC(), m_rcLocation, m_rcLocation);
    CIssKineticList::OnDraw(*m_gdiBg, m_rcLocation, m_rcLocation);
    DrawBorderOverlay(*m_gdiBg, m_rcLocation, m_rcLocation);
    ::BitBlt(gdiTemp, (bAnimateRight?0:WIDTH(m_rcLocation)),0, WIDTH(m_rcLocation), HEIGHT(m_rcLocation), m_gdiBg->GetDC(), 0,0,SRCCOPY);

    RECT rcAll = {0,0,gdiTemp.GetWidth(), gdiTemp.GetHeight()};
    RECT rc1, rc2;
    rc1 = rc2 = rcAll;
    rc1.right = rc2.left = WIDTH(rcAll)/2;

    DWORD dwStart = GetTickCount();
    BOOL bDec = bAnimateRight;

    int iNumFrames = WIDTH(rc1);

    float fAnimateTime = FAVORITES_PAGE_ANIMATE_TIME;
    int i = GetCurrentFrame(dwStart, iNumFrames, fAnimateTime);
    int iStartFrame = bDec ? rc1.right : 0;
    while(i < iNumFrames)
    {
        i = GetCurrentFrame(dwStart, iNumFrames, fAnimateTime);
        int iAmount = i;
        if(bDec)
            iAmount = -iAmount;

        // gdiTemp to offscreen
        BitBlt(m_gdiBg->GetDC(),
            0,0,
            WIDTH(m_rcLocation), HEIGHT(m_rcLocation),
            gdiTemp.GetDC(),
            iStartFrame + iAmount, 0,
            SRCCOPY);

        // selector
        CGuiToolBar::GetToolbar()->DrawSelector(*m_gdiBg, m_rcLocation, m_rcLocation, FALSE);

        // all to screen
        BitBlt(dc,
            0,0,
            WIDTH(m_rcLocation), HEIGHT(m_rcLocation),
            m_gdiBg->GetDC(),
            0,0,
            SRCCOPY);

    }
    ReleaseDC(m_hWndParent, dc);

    InvalidateRect(m_hWndParent, &m_rcLocation, FALSE);
}

BOOL CGuiContacts::IsLinksInDir(TCHAR* szDir)
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
        CloseHandle(hFind);
        return TRUE;
    }
    return FALSE;
}

