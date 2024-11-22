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
#include "DlgMsgBox.h"
#include "DlgContacts.h"
#include "commondefines.h"

#define UND				-1
#define CATEGORY_SEPARATOR_STRING _T("...")

#define PICTURE_Size    (40*GetSystemMetrics(SM_CXICON)/32)

#define IDT_TIMER_Search    WM_USER + 200
#define Search_Timeout      1000

EnumSortContacts g_eSort;
TCHAR* g_szCompare1 = NULL;
TCHAR* g_szCompare2 = NULL;
TCHAR g_szEndOfList = _T('z') + 1;

TypeListItem::TypeListItem()
:lOid(0)
,szTitle(NULL)
,szSecondary(NULL)
{}

TypeListItem::~TypeListItem()
{
    CIssString* oStr = CIssString::Instance();
    oStr->Delete(&szTitle);
    oStr->Delete(&szSecondary);
    lOid = 0;
}

CGuiContacts::CGuiContacts(void) 
:m_pPoom(NULL)
,m_iKeyRepeatCount(0)
{
	m_iLetterHeight			= GetSystemMetrics(SM_CXICON)/2;
	m_iTitleHeight			= (int)(m_iLetterHeight * 1.5);
	m_iContactHeight		= GetSystemMetrics(SM_CXICON)/2;
	//m_iContactHeightSelected = m_iContactHeight + m_iLetterHeight;  // try this for now
    m_iContactHeightSelected= PICTURE_Size;

	if(!g_szCompare1)
	{
		g_szCompare1	= new TCHAR[STRING_MAX];
		ZeroMemory(g_szCompare1, sizeof(TCHAR)*STRING_MAX);
	}
	if(!g_szCompare2)
	{
		g_szCompare2	= new TCHAR[STRING_MAX];
		ZeroMemory(g_szCompare2, sizeof(TCHAR)*STRING_MAX);
	}

	m_hFontLetters			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
	m_hFontContact			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
 	m_hFontContactBold		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);

	m_ptScreenSize.x		= 0;
	m_ptScreenSize.y		= 0;
	m_bScrollAlpha          = FALSE;
    m_bFadeIn               = FALSE;
    m_bShowSearch           = FALSE;

}

CGuiContacts::~CGuiContacts(void)
{
	CIssGDIEx::DeleteFont(m_hFontLetters);
	CIssGDIEx::DeleteFont(m_hFontContact);
	CIssGDIEx::DeleteFont(m_hFontContactBold);
    m_hImgFontAlpha.Destroy();
	m_oStr->Delete(&g_szCompare1);
	m_oStr->Delete(&g_szCompare2);
	EraseList();
}

BOOL CGuiContacts::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContactDetails* dlgContactDetails)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBg, guiBackground, sOptions);

	m_pPoom = CPoomContacts::Instance();

    m_dlgContactDetails = dlgContactDetails;

    m_imgContact.Initialize(m_iContactHeightSelected, m_iContactHeightSelected, hWndParent, hInst);

    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    Initialize(hWndParent, hWndParent, m_hInst, OPTION_Bounce);
    SetCustomDrawFunc(DrawListItem, this);
	SetDeleteItemFunc(DeleteContactItem);
    SetImageArray(SKIN(IDR_PNG_MenuArray));
	SetSelected(IDR_PNG_ContactsSelector);

    // preload list images
	PreloadImages(hWndParent, m_hInst);

	PopulateList();
	return TRUE;
}

void CGuiContacts::DeleteContactItem(LPVOID lpItem)
{
	if(!lpItem)
		return;
	
    TypeListItem* sList = (TypeListItem*)lpItem;
    delete sList;
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
	CGuiToolBar* guiToolbar = CGuiToolBar::GetToolbar();

	if(!guiToolbar)
		return FALSE;
	else
		return !guiToolbar->HasFocus();
}

CIssGDIEx& CGuiContacts::GetAlphaPopupGDI()
{
    if(!m_gdiAlphaPopUp.GetDC())
    {
        if(IsVGA())
        {
          /*  CIssGDIEx gdiTemp;
            gdiTemp.LoadImage(IDR_PNG_ContactsLetterBubble, m_hWndParent, m_hInst, TRUE);
            SIZE sz;
            sz.cx   = gdiTemp.GetWidth()*2;
            sz.cy   = gdiTemp.GetHeight()*2;
            ScaleImage(gdiTemp, m_gdiAlphaPopUp, sz, FALSE, 0);*/
            m_gdiAlphaPopUp.LoadImage(IDR_PNG_ContactsLetterBubbleVGA, m_hWndParent, m_hInst, TRUE);
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
}

BOOL CGuiContacts::Draw(CIssGDIEx& gdiMem, HDC hDC, RECT& rcClient, RECT& rcClip)
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

    RECT rcClipClient = rcClip;
    rcClipClient.right = min(rcClip.right, rcClient.right);
    rcClipClient.bottom = min(rcClip.bottom, rcClient.bottom);
    rcClipClient.left = max(rcClip.left, rcClient.left);
    
    // since this background is a full screen bg, must pass in that RECT
    RECT rcBg = {0,0,GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    m_guiBackground->DrawBackground(*m_gdiBg, rcClipClient, rcBg);

    DrawAlphaBg(m_gdiBg->GetDC(), m_rcLocation, rcClip);

    if(!GetItemCount())
        DrawText(m_gdiBg->GetDC(), _T("no contacts found"), m_rcList, DT_CENTER , CDlgContacts::GetFontNormal(), m_crText);
    else
        CIssKineticList::OnDraw(*m_gdiBg, rcClient, rcClip);

    DrawSearch(*m_gdiBg, rcClip);

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

void CGuiContacts::DrawSearch(CIssGDIEx& gdi, RECT& rcDraw)
{
    if(m_bShowSearch == FALSE)
        return;

    m_hImgFontAlpha.DrawText(gdi, m_szSearch, m_rcLocation, DT_CENTER | DT_VCENTER);
}

CIssGDIEx& CGuiContacts::GetBgGDI()
{
	if(m_gdiAphaBg.GetDC() == NULL || m_gdiAphaBg.GetWidth() != WIDTH(m_rcAlpha) ||
        m_gdiAphaBg.GetHeight() != HEIGHT(m_rcAlpha))
	{
		m_gdiAphaBg.Create(m_gdiMem->GetDC(), m_rcAlpha/*rcClient*/, FALSE);

        RECT rcAlphaFrame = {0,0,WIDTH(m_rcAlpha), HEIGHT(m_rcAlpha)};
        rcAlphaFrame.left += m_guiBackground->GetSideSliceWidth();
        Rectangle(m_gdiAphaBg, rcAlphaFrame, COLOR_CONTACTS_ALPHA_BG,COLOR_CONTACTS_ALPHA_BG);

		int iFontSize = HEIGHT(m_rcAlpha)/26 + 2;//11 worked nicely on PPC 

		HFONT hFontAlpha = CIssGDIEx::CreateFont(iFontSize, FW_NORMAL, TRUE);
		m_iAlphaTextSpacing = (iFontSize-2);
		m_iAlphaWidth = m_iAlphaTextSpacing*5/3;
		int iTotalSize = m_iAlphaTextSpacing*26;
		int iStart = (m_rcLocation/*rcClient*/.bottom - m_rcLocation/*rcClient*/.top)/2 - iTotalSize/2 + m_rcLocation/*rcClient*/.top;

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
			DrawText(m_gdiAphaBg.GetDC(), szLetter, rcAlpha, DT_CENTER | DT_TOP, hFontAlpha, 0);
			rcAlpha.top += m_iAlphaTextSpacing;
			rcAlpha.bottom += m_iAlphaTextSpacing;
		}
		CIssGDIEx::DeleteFont(hFontAlpha);

        // draw the right border part
        int iWidth = GetSystemMetrics(SM_CXSCREEN);
        int iHeight= GetSystemMetrics(SM_CYSCREEN);

        RECT rcBorder = {0,0,m_guiBackground->GetSideSliceWidth(),HEIGHT(m_rcAlpha)};
        if(m_guiBackground->GetBackground().GetDC() == NULL ||
            m_guiBackground->GetBackground().GetWidth() != iWidth ||
            m_guiBackground->GetBackground().GetHeight() != iHeight)
        {
            RECT rcClient = {0,0,iWidth, iHeight};
            m_guiBackground->DrawBackground(*m_gdiBg, rcClient, rcClient);
        }
        ::Draw(m_gdiAphaBg, rcBorder, 
            m_guiBackground->GetBackground(), 
            m_rcAlpha.right - m_guiBackground->GetSideSliceWidth(), 
            m_rcAlpha.top);
	}
    return m_gdiAphaBg;
}

BOOL CGuiContacts::DrawAlphaBg(HDC hdc, RECT& rcClient, RECT& rcClip)
{
    if(!IsRectInRect(rcClip, m_rcAlpha))
        return FALSE;

    RECT rc = rcClip;
    rc.bottom = min(rc.bottom, rcClient.bottom);
    rc.left = max(rc.left, rcClient.left);

	return BitBlt(hdc,
		m_rcAlpha.left, m_rcAlpha.top,
		WIDTH(m_rcAlpha), HEIGHT(m_rcAlpha),
		m_gdiAphaBg.GetDC(),
		0,0,
		SRCCOPY);

    return FALSE;
}

void CGuiContacts::GetPrimaryPhoneNumber(TCHAR* szDestination, IContact* pContact)
{
    if(!pContact || !szDestination || !m_oStr || !m_pPoom)
        return;

    m_oStr->Empty(szDestination);

    // for now return the first one you find
    TCHAR* szText = m_pPoom->GetUserHomeNum(pContact);
    if(szText)
        goto End;
    szText = m_pPoom->GetUserMobileNum(pContact);
    if(szText)
        goto End;
    szText = m_pPoom->GetUserWorkNum(pContact);
    if(szText)
        goto End;
    szText = m_pPoom->GetUserEmail(pContact);
    if(szText)
        goto End;
    
End:
    if(szText)
    {
        m_oStr->StringCopy(szDestination, szText);
        m_oStr->Delete(&szText);
    }

    return;
}

void CGuiContacts::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{

    if(!sItem || sItem->lpItem == NULL || sItem->eType != ITEM_Unknown || !m_sOptions || !m_pPoom)
        return;

    TypeListItem* sList = (TypeListItem*)sItem->lpItem;
    IContact* pContact = NULL;

    int iIndent = INDENT/2;
	InflateRect(&rcDraw, -iIndent, 0);

    // create draw text
    ZeroMemory(g_szCompare1, sizeof(TCHAR)*STRING_MAX);
    if(sList->szTitle)
    {
        m_oStr->StringCopy(g_szCompare1, sList->szTitle);
    }
    else
    {
        pContact = m_pPoom->GetContactFromOID(sList->lOid);
        if(!pContact)
            return;
    }

    if(pContact && m_sOptions->eSortContacts == SORT_Company)
    {
		m_pPoom->GetFormatedNameText(g_szCompare1, 
									pContact,
									TRUE);
    }
    else if(pContact && m_sOptions->eSortContacts == SORT_LastName)
    {
		m_pPoom->GetFormatedNameText(g_szCompare1, 
									pContact,
									TRUE);
    }
    else if(pContact && m_sOptions->eSortContacts == SORT_FirstName)
    {
		m_pPoom->GetFormatedNameText(g_szCompare1, 
									pContact,
									FALSE);
    }
    else if(pContact && m_sOptions->eSortContacts == SORT_Email)
    {
        TCHAR* szEmail = m_pPoom->GetUserEmail(pContact);
        if(szEmail)
        {
            m_oStr->StringCopy(g_szCompare1, szEmail);
            m_oStr->Delete(&szEmail);
        }
        else
        {
			m_pPoom->GetFormatedNameText(g_szCompare1, 
										pContact,
										TRUE);
        }
    }

    if(!sList->szTitle && !m_oStr->IsEmpty(g_szCompare1))
    {
        // save it for the next draw
        sList->szTitle = m_oStr->CreateAndCopy(g_szCompare1);
    }

    // draw it
    if(bIsHighlighted)
    {
        POINT ptLoc = {rcDraw.right - m_iContactHeightSelected - 3*iIndent, rcDraw.top + iIndent};

        //m_imgContact.SetContactOid(lOid);

        if(m_bFadeIn == FALSE)
            m_imgContact.DrawContactPic(gdi, sList->lOid, ptLoc.x, ptLoc.y);
            
            
		RECT rcLine2 = rcDraw;
		rcLine2.left = rcDraw.left + 4*iIndent;
        rcLine2.top += 2*iIndent;
        ::DrawText(gdi.GetDC(), g_szCompare1, 
             rcLine2, DT_LEFT|DT_NOPREFIX, m_hFontContactBold, COLOR_TEXT_NORMAL);
        rcLine2 = rcDraw;
        //OffsetRect(&rcLine2, GetSystemMetrics(SM_CXICON)/2, GetSystemMetrics(SM_CXSMICON));
		rcLine2.left += (4*iIndent);
		rcLine2.top += (GetSystemMetrics(SM_CXSMICON) + 2*iIndent);

        if(!sList->szSecondary)
        {
            if(!pContact)
            {
                pContact = m_pPoom->GetContactFromOID(sList->lOid);
                if(!pContact)
                    return;
            }
            GetPrimaryPhoneNumber(g_szCompare2, pContact);
            sList->szSecondary = m_oStr->CreateAndCopy(g_szCompare2);
        }
        else
            m_oStr->StringCopy(g_szCompare2, sList->szSecondary);
        
        ::DrawText(gdi.GetDC(), g_szCompare2, rcLine2,
             DT_LEFT|DT_END_ELLIPSIS, CDlgContacts::GetFontNormal(), COLOR_TEXT_NORMAL);

        if(m_bFadeIn == TRUE && m_iKeyRepeatCount < 2 && m_eScroll != SCROLL_Scrolling && m_eScroll != SCROLL_Moving)
        {
            HDC dc = GetDC(m_hWndParent);
            m_imgContact.FadeIn(sList->lOid, dc, 
                ptLoc.x + m_rcLoc.left, ptLoc.y + m_rcLoc.top, 
                500);
            ReleaseDC(m_hWndParent, dc);

            m_bFadeIn = FALSE;
            m_imgContact.DrawContactPic(gdi, sList->lOid, ptLoc.x, ptLoc.y);
        }
    }
    else if(m_sOptions->bContactsVerboseMode)
    {
        RECT rcLine2 = rcDraw;
        rcLine2.left = rcDraw.left + 4*iIndent;
        rcLine2.top += 2*iIndent;
        ::DrawText(gdi.GetDC(), g_szCompare1, 
             rcLine2, DT_LEFT|DT_NOPREFIX, m_hFontContactBold, COLOR_TEXT_NORMAL);
        rcLine2 = rcDraw;
        //OffsetRect(&rcLine2, GetSystemMetrics(SM_CXICON)/2, GetSystemMetrics(SM_CXSMICON));
        rcLine2.left += (4*iIndent);
        rcLine2.top += (GetSystemMetrics(SM_CXSMICON) + 2*iIndent);

        if(!sList->szSecondary)
        {
            if(!pContact)
            {
                pContact = m_pPoom->GetContactFromOID(sList->lOid);
                if(!pContact)
                    return;
            }
            GetPrimaryPhoneNumber(g_szCompare2, pContact);
            sList->szSecondary = m_oStr->CreateAndCopy(g_szCompare2);
        }
        else
            m_oStr->StringCopy(g_szCompare2, sList->szSecondary);

        ::DrawText(gdi.GetDC(), g_szCompare2, rcLine2,
             DT_LEFT|DT_END_ELLIPSIS, CDlgContacts::GetFontNormal(), COLOR_TEXT_NORMAL);
    }
    else
    {
		rcDraw.left += 4*iIndent;
        ::DrawText(gdi.GetDC(), g_szCompare1, 
            rcDraw, DT_LEFT|DT_NOPREFIX, CDlgContacts::GetFontNormal(), COLOR_TEXT_NORMAL);
    }

    if(pContact)
        pContact->Release();
    
}

void CGuiContacts::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CGuiContacts* pContacts = (CGuiContacts*)lpClass;
    pContacts->DrawListItem(gdi, rcDraw, sItem, bIsHighlighted);
}

BOOL CGuiContacts::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if(CIssKineticList::OnTimer(wParam, lParam) == TRUE)
        return TRUE;

    if(LOWORD(wParam) == IDT_LoadPic)
    {
        KillTimer(m_hWndParent, IDT_LoadPic);
        TypeItems* sItem = GetSelectedItem();
        if(sItem)
        {
            TypeListItem* sList = (TypeListItem*)sItem->lpItem;
            if(sList && IsValidOID(sList->lOid))
                m_imgContact.SetContactOid(sList->lOid);
        }
    }     
    else if(LOWORD(wParam) == IDT_TIMER_Search)
    {
        KillTimer(m_hWndParent, IDT_TIMER_Search);
        m_oStr->Empty(m_szSearch);
        m_bShowSearch = FALSE;
        //could fade out here ....
        InvalidateRect(m_hWndParent, NULL, FALSE);
    }
	return UNHANDLED;
}

BOOL CGuiContacts::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(WM_DrawContactPic == uiMessage)
    {
        if(m_sOptions->bShowAnimations)
            m_bFadeIn = TRUE;
        InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    }
    return UNHANDLED;
}

BOOL CGuiContacts::SelecteItem(int iIndex)
{
    if(!m_pPoom)
        return FALSE;

	TypeItems * sListItem = m_arrItems[iIndex];
	if(!sListItem || !sListItem->lpItem)
		return FALSE;

    TypeListItem* sList = (TypeListItem*)sListItem->lpItem;
	if(!IsValidOID(sList->lOid))
		return FALSE;
    
#ifdef FIX_ANNOYING_POOM_ONEOFF_ERROR
    int iPoomIndex = m_pPoom->GetIndexFromOID(sList->lOid);
#else
    int iPoomIndex = m_pPoom->GetIndexFromOID(sList->lOid) - 1;
#endif

    BOOL bLastNameFirst = TRUE;

    if(m_sOptions && m_sOptions->eSortContacts == SORT_FirstName)
        bLastNameFirst = FALSE;

    m_dlgContactDetails->SetLastNameFirst(bLastNameFirst);
	m_dlgContactDetails->SetIndex(iPoomIndex);
    m_dlgContactDetails->Init(m_gdiMem, m_guiBackground);
	int iReturn = m_dlgContactDetails->Launch(m_hWndParent, m_hInst, m_sOptions->bShowAnimations);

    if(iReturn == IDOK)
    {
    }

	return TRUE;
}

BOOL CGuiContacts::MoveGui(RECT rcLocation)
{
    if(EqualRect(&rcLocation, &m_rcLocation))
        return TRUE;

	CGuiBase::MoveGui(rcLocation);

    DBG_CONTACTS(_T("CGuiContacts::MoveGui()"), TRUE);

    m_rcContactList = m_rcAlpha = rcLocation;
    int iAlphaWidth = IsVGA() ? CONTACTS_ALPHA_WIDTH*2 : CONTACTS_ALPHA_WIDTH;
    m_rcAlpha.left = m_rcAlpha.right - iAlphaWidth;
    m_rcContactList.right = m_rcAlpha.left;

    // allow for the border part with m_rcAlpha
    m_rcAlpha.left -= m_guiBackground->GetSideSliceWidth();

    int iListInset = (IsVGA() ? CONTACTS_LIST_INSET * 2 : CONTACTS_LIST_INSET);
    CIssKineticList::OnSize(m_rcContactList.left + iListInset, 
        rcLocation.top + iListInset/2, 
        WIDTH(m_rcContactList) - iListInset*2, 
        HEIGHT(m_rcContactList) - iListInset/2
        );

    // reload images
    GetBgGDI();
    GetAlphaPopupGDI();
    GetAlphaFont();

    DBG_CONTACTS(_T("CGuiContacts::MoveGui()"), FALSE);
	return TRUE;
}

void CGuiContacts::DrawSelector(CIssGDIEx& gdi, RECT& rcDraw)
{
    // fine-tune the RECT
    RECT rcSelector = rcDraw;
    int iIndent = GetSystemMetrics(SM_CXICON)/8;
    InflateRect(&rcSelector, -iIndent, 0);
    CIssKineticList::DrawSelector(gdi, rcSelector);
}


BOOL CGuiContacts::OnLButtonDown(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
    {
        m_eMouse = GRAB_None;
		return FALSE;
    }

	m_ptMouseDown = pt;
	if(PtInRect(&m_rcAlpha, pt))
	{
		m_bScrollAlpha = TRUE;
		DrawAlphabetPopUp(pt);
	}
	return CIssKineticList::OnLButtonDown(pt);
}

BOOL CGuiContacts::OnLButtonUp(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE && m_eMouse == GRAB_None)
		return FALSE;

	if(m_bScrollAlpha)
	{
		m_bScrollAlpha = FALSE;
		InvalidateRect(m_hWndParent, NULL, FALSE);

		TCHAR szChar = _T('A' + (TCHAR)((pt.y - m_rcAlphaText.top)/(m_iAlphaTextSpacing))) ;
		ScrollTo(szChar, TRUE);

		return UNHANDLED;
	}

    BOOL bRet = (m_eMouse == GRAB_List?TRUE:FALSE);
    bRet |=  CIssKineticList::OnLButtonUp(pt);
    return bRet;

}

BOOL CGuiContacts::OnMouseMove(POINT pt)
{
	//if(PtInRect(&m_rcLocation, pt) == FALSE)
	//	return FALSE;

	if(m_bScrollAlpha)
	{
		DrawAlphabetPopUp(pt);
		return TRUE;
	}

    return CIssKineticList::OnMouseMove(pt);
}

BOOL CGuiContacts::OnChar(WPARAM wParam, LPARAM lParam)
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

BOOL CGuiContacts::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(HasFocus())
    {
        return CIssKineticList::OnKeyDown(wParam, lParam);
    }

    return FALSE;
}

BOOL CGuiContacts::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    /*TypeItems* sItem = GetSelectedItem();
    if(!sItem)
        return TRUE;

    long lOid = (long)sItem->lpItem;
    if(IsValidOID(lOid))
        m_imgContact.SetContactOid(lOid);
    InvalidateRect(hWnd, &m_rcLocation, FALSE);*/
    return TRUE;
}

BOOL CGuiContacts::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}
   
BOOL CGuiContacts::HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    switch(uiMessage)
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
    }
    return TRUE;
}

HRESULT CGuiContacts::PoomItemAdded(long lOid)
{
    DBG_CONTACTS(_T("PoomItemAdded()"), TRUE);
    HRESULT hr = S_OK;

    IContact* pPoomContact	= m_pPoom->GetContactFromOID(lOid);
    CPHRE(pPoomContact, _T("pPoomContact is NULL"), E_POINTER);
    CPHRE(m_sOptions, _T("m_sOptions is NULL"), E_POINTER);
   
    TypeListItem* pNew = new TypeListItem;
    CPHR(pNew, _T("TypeListItem* pNew = new TypeListItem;"));

    hr = pPoomContact->get_Oid(&pNew->lOid);
    CHR(hr, _T("hr = pPoomContact->get_Oid(&pNew->lOid);"));

    hr = AddItem((LPVOID)pNew, 5000);
    CHR(hr, _T("hr = AddItem((LPVOID)lOid, 0);"));

    // check if a letter category is needed
    CheckItemCategoryNeeded(pPoomContact);

    // sort - set heights
    g_eSort = m_sOptions->eSortContacts;
    m_arrItems.Sort(CompareContacts);
    SetItemHeights(m_sOptions->bContactsVerboseMode?m_iContactHeightSelected-INDENT:m_iContactHeight, m_iContactHeightSelected + INDENT);
    // select it
    int iNewIndex = FindContact(lOid);
    SetSelectedItemIndex(iNewIndex >= 0 ? iNewIndex : 0, TRUE);
    DBG_OUT((_T("HandlePoomMessage() added item %d"), lOid));

Error:
    if(pPoomContact)
        pPoomContact->Release();

    DBG_CONTACTS(_T("PoomItemAdded()"), FALSE);

    return hr;
}

HRESULT CGuiContacts::PoomItemChanged(long lOid)
{
    HRESULT hr = S_OK;
    IContact* pContact = NULL;

    CPHRE(m_sOptions, _T("m_sOptions is NULL"), E_POINTER);

    pContact	= m_pPoom->GetContactFromOID(lOid);
    CPHRE(pContact, _T("pContact is NULL"), E_POINTER);

    int iIndex = FindContact(lOid);
    if(iIndex != -1)
    {
        TypeItems* sItem = GetItem(iIndex);
        if(sItem && sItem->lpItem)
        {
            TypeListItem* sList = (TypeListItem*)sItem->lpItem;
            m_oStr->Delete(&sList->szTitle);
            m_oStr->Delete(&sList->szSecondary);
        }
    }

    CheckItemCategoryNeeded(pContact);
    g_eSort = m_sOptions->eSortContacts;
    int iCurIndex = GetSelectedItemIndex();
    m_arrItems.Sort(CompareContacts);
    SetSelectedItemIndex(GetNextItemIndex(iCurIndex), TRUE);
    DBG_OUT((_T("HandlePoomMessage() edited item 0x%08x re-sort list"), lOid));
    

Error:
    if(pContact)
        pContact->Release();

    return hr;
}


HRESULT CGuiContacts::PoomItemDeleted(long lOid)
{
    HRESULT hr = S_OK;

    // get the item from array
    int iContactIndex = FindContact(lOid);
    if(iContactIndex != -1)
        RemoveItem(iContactIndex);

    return hr;
}

BOOL CGuiContacts::ApplyContactChanges(TypeContact* pSrc, TypeContact* pDst)
{
    // Notes:
    // apply changes to our struct
    // return TRUE if list needs a re-sort (i.e. a name has changed)
    // otherwise, a re-draw will suffice
    if(!m_sOptions)
        return FALSE;

    //pSrc is temporary, so copy src to dst

    BOOL bResult = FALSE;
    // first name
    if(m_oStr->Compare(pSrc->szFirstName, pDst->szFirstName) != 0)
    {
        m_oStr->Delete(&pDst->szFirstName);
        pDst->szFirstName = m_oStr->CreateAndCopy(pSrc->szFirstName);
        if(m_sOptions->eSortContacts == SORT_FirstName)
            bResult = TRUE;
    }

    // last name
    if(m_oStr->Compare(pSrc->szLastName, pDst->szLastName) != 0)
    {
        m_oStr->Delete(&pDst->szLastName);
        pDst->szLastName = m_oStr->CreateAndCopy(pSrc->szLastName);
        if(m_sOptions->eSortContacts == SORT_LastName)
            bResult = TRUE;
    }

    // company
    if(m_oStr->Compare(pSrc->szCompany, pDst->szCompany) != 0)
    {
        m_oStr->Delete(&pDst->szCompany);
        pDst->szCompany = m_oStr->CreateAndCopy(pSrc->szCompany);
        if(m_sOptions->eSortContacts == SORT_Company)
            bResult = TRUE;
    }

    // email
    if(m_oStr->Compare(pSrc->szEmailAddr, pDst->szEmailAddr) != 0)
    {
        m_oStr->Delete(&pDst->szEmailAddr);
        pDst->szEmailAddr = m_oStr->CreateAndCopy(pSrc->szEmailAddr);
        if(m_sOptions->eSortContacts == SORT_Email)
            bResult = TRUE;
    }

    // other info
    if(m_oStr->Compare(pSrc->szHomeNumber, pDst->szHomeNumber) != 0)
    {
        m_oStr->Delete(&pDst->szHomeNumber);
        pDst->szHomeNumber = m_oStr->CreateAndCopy(pSrc->szHomeNumber);
    }

    if(m_oStr->Compare(pSrc->szMobileNumber, pDst->szMobileNumber) != 0)
    {
        m_oStr->Delete(&pDst->szMobileNumber);
        pDst->szMobileNumber = m_oStr->CreateAndCopy(pSrc->szMobileNumber);
    }

    if(m_oStr->Compare(pSrc->szWorkNumber, pDst->szWorkNumber) != 0)
    {
        m_oStr->Delete(&pDst->szWorkNumber);
        pDst->szWorkNumber = m_oStr->CreateAndCopy(pSrc->szWorkNumber);
    }

    // picture
    // we'll assume that if a picture is provided, it has changed
    // src pic will be destroyed, so copy it to dst
    if(pSrc->gdiPicture)
    {
        // if dst gdi, then create
        if(!pDst->gdiPicture)
            pDst->gdiPicture = new CIssGDIEx();
        int w = pSrc->gdiPicture->GetWidth();
        int h = pSrc->gdiPicture->GetHeight();
        pDst->gdiPicture->Create(pSrc->gdiPicture->GetDC(), w, h, TRUE);
    }


    return bResult;
}

BOOL CGuiContacts::CheckItemCategoryNeeded(IContact* pContact)
{
    if(!m_sOptions || !pContact || !m_pPoom)
        return FALSE;

    TCHAR* szCompany    = m_pPoom->GetUserData(CI_Company, pContact);
    TCHAR* szFirstName  = m_pPoom->GetUserData(CI_FirstName, pContact);
    TCHAR* szLastName   = m_pPoom->GetUserData(CI_LastName, pContact);
    TCHAR* szEmail      = m_pPoom->GetUserData(CI_Email, pContact);

    TCHAR* pszCategoryStr = NULL;
    if(m_sOptions->eSortContacts == SORT_Company && szCompany)
        pszCategoryStr = m_oStr->CreateAndCopy(szCompany);
    else if(m_sOptions->eSortContacts == SORT_FirstName && szFirstName)
        pszCategoryStr = m_oStr->CreateAndCopy(szFirstName, 0, 1);
    else if(m_sOptions->eSortContacts == SORT_LastName)
        pszCategoryStr = m_oStr->CreateAndCopy(szLastName, 0, 1);
    else if(m_sOptions->eSortContacts == SORT_Email)
	{
		pszCategoryStr = m_oStr->CreateAndCopy(szEmail, 0, 1);
		m_oStr->Capitalize(pszCategoryStr);
	}

	BOOL bReturn = FALSE;
    if(pszCategoryStr && m_oStr->GetLength(pszCategoryStr))
    {
        if(!CategoryExists(pszCategoryStr))
        {
            if(S_OK == AddCategory(pszCategoryStr))
				bReturn = TRUE;
        }
    }

	m_oStr->Delete(&pszCategoryStr);
    m_oStr->Delete(&szCompany);
    m_oStr->Delete(&szFirstName);
    m_oStr->Delete(&szLastName);
    m_oStr->Delete(&szEmail);

    return bReturn;
}

int CGuiContacts::FindContact(long lOid)
{
    // start at the end (if it's a new item that's where it will be)
    int i = GetItemCount();
    while(--i >= 0)
    {
        TypeItems* sItem = m_arrItems[i];
        if(sItem && sItem->lpItem)
        {
            TypeListItem* sList = (TypeListItem*)sItem->lpItem;
            if(sList->lOid == lOid)
                return i;
        }
    }

    return -1;
}
    
BOOL CGuiContacts::CategoryExists(TCHAR* szText)
{
    for(int i = 0; i < m_arrItems.GetSize(); i++)
    {
        TypeItems* sItem = m_arrItems[i];
        if(sItem && sItem->eType == ITEM_Category)
        {
            if(m_oStr->CompareNoCase(szText, sItem->szText) == 0)
                return TRUE;
        }
    }
    return FALSE;
}


void CGuiContacts::RefreshContacts(void)
{
	EraseList();
	PopulateList();
}

void CGuiContacts::PopulateList()
{
    DBG_CONTACTS(_T("PopulateList()"), TRUE);

	if(!m_pPoom || !m_sOptions)
		return;

    m_oStr->Empty(m_szSearch);

    int iNumContacts = m_pPoom->GetNumContacts();
	if(iNumContacts == 0)
        return;
    
	for(int i=0; i < iNumContacts; i++)
	{
#ifdef FIX_ANNOYING_POOM_ONEOFF_ERROR
		IContact* pPoomContact	= m_pPoom->GetContactFromIndex(i + 1);    // poom index 1 based
#else
		IContact* pPoomContact	= m_pPoom->GetContactFromIndex(i);
#endif
		if(!pPoomContact)
			continue;//return;

        TypeListItem* sNewItem = new TypeListItem;
        if(!sNewItem)
            continue;

        pPoomContact->get_Oid(&sNewItem->lOid);
        

        if(sNewItem->lOid == 0 ||S_OK != AddItem((LPVOID)sNewItem, 5000))
        {
            pPoomContact->Release();
            continue;
        }
        CheckItemCategoryNeeded(pPoomContact);
        pPoomContact->Release();
	}


    // add any necessary spacers
    AddCategory(CATEGORY_SEPARATOR_STRING);

	// now that we have all the entries we have to sort them
    g_eSort = m_sOptions->eSortContacts;
	m_arrItems.Sort(CompareContacts);

    // now remove final separator if it is the last item
    TypeItems* slastItem = (TypeItems *)m_arrItems.LastElement();
    if(slastItem && slastItem->eType == ITEM_Category && m_arrItems.GetSize() > 0)
    {
        int ilast = m_arrItems.GetSize() - 1;
        DeleteItem(ilast);
        m_arrItems.RemoveElementAt(ilast);
    }

    SetItemHeights(m_sOptions->bContactsVerboseMode?m_iContactHeightSelected-INDENT:m_iContactHeight, m_iContactHeightSelected + INDENT);
    SetSelectedItemIndex(0, TRUE);
    ResetScrollPos();

    DBG_CONTACTS(_T("PopulateList()"), FALSE);
}

void CGuiContacts::GetCompareString(TCHAR* pszCompare, long& lOid, int iCompareCount)
{
    // Notes:
    // if no sort criteria, put at end of list

    if(!IsValidOID(lOid) || !pszCompare)
        return;

	CIssString* oStr = CIssString::Instance();
	CPoomContacts* pPoom = CPoomContacts::Instance();

    oStr->Empty(pszCompare);

    TCHAR szEndOfList = _T('z') + 2;

    switch(g_eSort)
    {
        case SORT_LastName:
            {
				pPoom->GetFormatedNameText(pszCompare, 
											lOid,
											TRUE);
            }
            break;
        case SORT_FirstName:
            {
				pPoom->GetFormatedNameText(pszCompare, 
											 lOid,
											 FALSE);
            }
            break;
        case SORT_Email:
            {
                if(iCompareCount == 0)
                {
                    TCHAR* szEmail = pPoom->GetUserEmail(lOid);
                    if(szEmail)
                    {
                        oStr->Concatenate(pszCompare, szEmail);
                        oStr->Delete(&szEmail);
                    }
                }
				else
				{
					pPoom->GetFormatedNameText(pszCompare, 
													lOid,
													TRUE);
				}
            }
            break;
        case SORT_Company:
            {
                if(iCompareCount == 0)
                {
                    TCHAR* szCompany = pPoom->GetUserData(CI_Company, lOid);
                    if(szCompany)
                    {
                        oStr->Concatenate(pszCompare, szCompany);
					    oStr->Capitalize(pszCompare);
                        oStr->Delete(&szCompany);
                    }
                    // add something to string so sort will place item after category
                    if(oStr->GetLength(pszCompare))
                        oStr->Concatenate(pszCompare, _T(" a")); 
                }
				else
				{
					pPoom->GetFormatedNameText(pszCompare, 
												lOid,
												TRUE);
				}
            }
            break;
    }

    if(oStr->GetLength(pszCompare) == 0)
    {
        // put it at end of list
        oStr->Concatenate(pszCompare, szEndOfList);
    }

}

int CGuiContacts::CompareContacts( const void *arg1, const void *arg2 )
{
	CIssString* oStr = CIssString::Instance();    
    if(!arg1 || !arg2)
        return 0;

    TypeItems& sListItem1 = **(TypeItems **)arg1;
    TypeItems& sListItem2 = **(TypeItems **)arg2;

    ZeroMemory(g_szCompare1, sizeof(TCHAR)*STRING_MAX);
    ZeroMemory(g_szCompare2, sizeof(TCHAR)*STRING_MAX);

    if(sListItem1.eType == ITEM_Category)
    {
        if(oStr->Compare(sListItem1.szText, CATEGORY_SEPARATOR_STRING) == 0)
            oStr->Concatenate(g_szCompare1, g_szEndOfList);
        else
            oStr->Concatenate(g_szCompare1, sListItem1.szText);
    }

    if(sListItem2.eType == ITEM_Category)
    {
        if(oStr->Compare(sListItem2.szText, CATEGORY_SEPARATOR_STRING) == 0)
            oStr->Concatenate(g_szCompare2, g_szEndOfList);
        else
            oStr->Concatenate(g_szCompare2, sListItem2.szText);
    }

    TypeListItem* sList1	= (TypeListItem*)sListItem1.lpItem;
    TypeListItem* sList2	= (TypeListItem*)sListItem2.lpItem;

    int iCompCnt = -1;
    int iReturnVal = 0;
    while(!iReturnVal && ++iCompCnt < 3)
    {
        if(sList1)
            GetCompareString(g_szCompare1, sList1->lOid, iCompCnt);
        if(sList2)
            GetCompareString(g_szCompare2, sList2->lOid, iCompCnt);
		iReturnVal = oStr->CompareNoCase(g_szCompare1, g_szCompare2);
    }

    return iReturnVal;

}

void CGuiContacts::EraseList()
{
    ResetContent();
}

BOOL CGuiContacts::DrawAlphabetPopUp(POINT pt)
{
	if(pt.y < m_rcAlphaText.top)
		pt.y = m_rcAlphaText.top;
	else if(pt.y >= m_rcAlphaText.bottom)
		pt.y = m_rcAlphaText.bottom - 1;

	HDC hdc = GetDC(m_hWndParent);
	static RECT rcTemp;
	static POINT ptOldPos = pt;
    static TCHAR szText[2] = {_T('\0'),_T('\0')};

	CIssRect rc;
	rc.right	= m_rcAlpha.left;// GetSystemMetrics(SM_CXICON);
	rc.left	    = rc.right - m_gdiAlphaPopUp.GetWidth();
	rc.top		= ptOldPos.y;
	rc.bottom	= rc.top + m_gdiAlphaPopUp.GetHeight();

	BitBlt(m_gdiMem->GetDC(), 
		rc.left, rc.top,
		rc.GetWidth(), rc.GetHeight(),
		m_gdiBg->GetDC(),//m_gdiTempBack.GetDC(), 
		rc.left, rc.top, 
		SRCCOPY);

	int iCharOffset = max(0, (pt.y - m_rcAlphaText.top)/(m_iAlphaTextSpacing));

	//now we're going to center the popup
    int iAlphaHeight = GetAlphaPopupGDI().GetHeight();
	pt.y -= (iAlphaHeight/2);
	if(pt.y < m_rcAlpha.top)
		pt.y = m_rcAlpha.top;
	if(pt.y > m_rcAlpha.bottom - iAlphaHeight)
		pt.y = m_rcAlpha.bottom - iAlphaHeight;


	szText[0] = _T('A') + iCharOffset;

	RECT rcNew = rc.Get();
	rcNew.top		= pt.y;
	rcNew.bottom	= rcNew.top + iAlphaHeight;

    ::Draw(*m_gdiMem, rcNew,  GetAlphaPopupGDI());

    RECT rcText = rcNew;
    rcText.right = rcText.left + (long)(WIDTH(rcText)*3/4);
    rcText.top   = rcText.top + (GetAlphaPopupGDI().GetHeight() - GetAlphaFont().GetHeight())/2 + GetAlphaFont().GetHeight()/12;
    GetAlphaFont().DrawText(*m_gdiMem, szText, rcText, DT_CENTER | DT_TOP);

	rc.Concatenate(rcNew);

	BitBlt(hdc, 
		rc.left, rc.top,
		rc.GetWidth(), rc.GetHeight(),
		m_gdiMem->GetDC(), 
		rc.left, rc.top, 
		SRCCOPY);

	ptOldPos = pt;
	
	ReleaseDC(m_hWndParent, hdc);
	return TRUE;
}

void CGuiContacts::ScrollTo(TCHAR* szText)
{
    TCHAR szTemp[STRING_MAX];
    m_oStr->Empty(szTemp);

    TypeItems* sItem = NULL;

    int iEntryPos = 0;
    int iScrollToPos = 0;

    TypeListItem* sList;

    //find the scroll position
    for(; iEntryPos < m_arrItems.GetSize(); iEntryPos++)
    {
        sItem = m_arrItems[iEntryPos];
        if(sItem == NULL)
            return;

        if(sItem->eType == ITEM_Unknown)
        {
            m_oStr->Empty(szTemp);            
            sList = (TypeListItem*)sItem->lpItem;

            if(!sList)
                continue;

            //get name and place it in temp
            if(sList->szTitle)
                m_oStr->StringCopy(szTemp, sList->szTitle);
            else
                GetCompareString(szTemp, sList->lOid, 0);

            if(m_oStr->IsEmpty(szTemp))
                continue;

            int iFind = m_oStr->FindNoCase(szTemp, szText, 0);
            if(0 == iFind)
            {
                iScrollToPos = iEntryPos;    
                break;
            }
            if(0 < m_oStr->CompareNoCase(szTemp, szText))
                break;
            
            iScrollToPos = iEntryPos;
        }
    }

    //otherwise do nothing
//Error:

    if(iScrollToPos == m_iCurSelector)
        return;

    SetSelectedItemIndex(iScrollToPos, TRUE);
    CIssKineticList::ScrollTo(GetYStartPosition(iScrollToPos), !m_sOptions->bShowAnimations);
}
	

//Ok .. this is the scroll to used by the alphabet list on the right side
void CGuiContacts::ScrollTo(TCHAR szChar, BOOL bLettersOnly)
{
    TCHAR szTemp[STRING_MAX];
    m_oStr->Empty(szTemp);

    TypeItems* sItem = NULL;

    int iEntryPos = 0;
    int iScrollToPos = 0;

    TCHAR c = 0;

    TypeListItem* sList;

    //find the scroll position
    for(; iEntryPos < m_arrItems.GetSize(); iEntryPos++)
    {
        sItem = m_arrItems[iEntryPos];
        if(sItem == NULL)
            return;

        if(sItem->eType == ITEM_Unknown)
        {
            m_oStr->Empty(szTemp);            
            sList = (TypeListItem*)sItem->lpItem;

            if(!sList)
                continue;

            if(sList->szTitle)
                m_oStr->StringCopy(szTemp, sList->szTitle);
            else
                GetCompareString(szTemp, sList->lOid, 0);

            if(m_oStr->IsEmpty(szTemp))
                continue;

            c = szTemp[0];
            m_oStr->Capitalize(&c);

            if(bLettersOnly && 
                (c < 65 || c > 91))
                continue;
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
    }

    TCHAR szCur = 0;
    TCHAR szNext = 0;

    sItem = NULL;
    sItem = m_arrItems[m_iCurSelector];// m_poMenu->GetItem(m_poMenu->GetSelectedItemIndex());

    if(sItem == NULL || sItem->eType != ITEM_Unknown)
        goto Error;
    
    m_oStr->Empty(szTemp);            
    sList = (TypeListItem*)sItem->lpItem;

    if(!sList)
        goto Error;

    if(sList->szTitle)
        m_oStr->StringCopy(szTemp, sList->szTitle);
    else
        GetCompareString(szTemp, sList->lOid, 0);

    if(m_oStr->IsEmpty(szTemp))
        goto Error;

    szCur =szTemp[0];
    m_oStr->Capitalize(&szChar);

    if(szCur != c)//same letter
        goto Error;


    sItem = NULL;
    sItem = m_arrItems[m_iCurSelector + 1];// m_poMenu->GetItem(m_poMenu->GetSelectedItemIndex());

    if(sItem == NULL || sItem->eType != ITEM_Unknown)
        goto Error;

    m_oStr->Empty(szTemp);            
    sList = (TypeListItem*)sItem->lpItem;

    if(!sList)
        goto Error;

    if(sList->szTitle)
        m_oStr->StringCopy(szTemp, sList->szTitle);
    else
        GetCompareString(szTemp, sList->lOid, 0);

    szNext = szTemp[0];

    if(szNext == szCur)//we're golden
        iScrollToPos = m_iCurSelector + 1;
        //otherwise do nothing
Error:

    if(iScrollToPos == m_iCurSelector)
        return;

    SetSelectedItemIndex(iScrollToPos, TRUE);
    CIssKineticList::ScrollTo(GetYStartPosition(iScrollToPos), !m_sOptions->bShowAnimations);
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
	case IDMENU_EditContact:
        {
             // give back taskbar so user can close the outlook window
            TypeItems* pItem = GetSelectedItem();
            if(!pItem || !pItem->lpItem)
            {
                MessageBeep(MB_ICONHAND);
                return TRUE;
            }
            TypeListItem* sItem = (TypeListItem*)pItem->lpItem;
            if(!IsValidOID(sItem->lOid))
            {
                MessageBeep(MB_ICONHAND);
                return TRUE;
            }
            SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
			//ShowWindow(m_hWndParent, SW_HIDE);
            m_pPoom->LaunchEditContactScreen(m_hWndParent, /*pEntry->*/sItem->lOid);
			ShowWindow(m_hWndParent, SW_SHOW);
			//InvalidateRect(m_hWndParent, NULL, FALSE);

       }
		break;
	case IDMENU_NewContact:
        {
            // give back taskbar so user can close the outlook window
		    SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
		//	ShowWindow(m_hWndParent, SW_HIDE);
            m_pPoom->CreateNew(m_hWndParent);
			ShowWindow(m_hWndParent, SW_SHOW);
		//	InvalidateRect(m_hWndParent, NULL, FALSE);
        }
		break;
	case IDMENU_DeleteContact:
        {
            // get selected contact
            TypeItems* pItem = GetSelectedItem();
            if(!pItem || !pItem->lpItem)
            {
                MessageBeep(MB_ICONHAND);
                return TRUE;
            }
            TypeListItem* sItem = (TypeListItem*)pItem->lpItem;
            if(!IsValidOID(sItem->lOid))
            {
                MessageBeep(MB_ICONHAND);
                return TRUE;
            }

        	CDlgMsgBox msgBox;
			TCHAR szMsg[STRING_MAX];
			m_oStr->Format(szMsg, _T("Are you sure you want to permanently delete contact: "));

            if(sItem->szTitle)
                m_oStr->Concatenate(szMsg, sItem->szTitle);
            else
            {
                IContact* pContact = m_pPoom->GetContactFromOID(sItem->lOid);
                if(!pContact)
                {
                    MessageBeep(MB_ICONHAND);
                    return TRUE;
                }
                m_pPoom->GetFormatedNameText(g_szCompare1, 
                    pContact,
                    TRUE);
                pContact->Release();
                m_oStr->Concatenate(szMsg, g_szCompare1);
            }
			
            msgBox.Init(m_gdiMem, m_guiBackground);
			int iReturnVal = msgBox.PopupMessage(szMsg, _T("Delete Contact"), m_hWndParent, m_hInst, MB_YESNO);

             // delete it
			if(iReturnVal == IDYES)
				m_pPoom->DeleteContact(sItem->lOid);

        }
		break;
	}
	return TRUE;
}
