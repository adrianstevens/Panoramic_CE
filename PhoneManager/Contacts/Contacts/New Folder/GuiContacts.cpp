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

#define UND				-1
#define CATEGORY_SEPARATOR_STRING _T("...")

#ifdef DEBUG
//#define DEBUG_CONTACTS
#endif

EnumSortContacts g_eSort;

CGuiContacts::CGuiContacts(void) 
{
	m_iLetterHeight			= GetSystemMetrics(SM_CXICON)/2;
	m_iTitleHeight			= (int)(m_iLetterHeight * 1.5);
	m_iContactHeight		= GetSystemMetrics(SM_CXICON)/2;
	m_iContactHeightSelected = m_iContactHeight + m_iLetterHeight;  // try this for now


	m_hFontLetters			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
	m_hFontContact			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
 	m_hFontContactBold		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);

	m_ptScreenSize.x		= 0;
	m_ptScreenSize.y		= 0;

	m_bScrollAlpha = FALSE;
}

CGuiContacts::~CGuiContacts(void)
{
	CIssGDIEx::DeleteFont(m_hFontLetters);
	CIssGDIEx::DeleteFont(m_hFontContact);
	CIssGDIEx::DeleteFont(m_hFontContactBold);
    m_hImgFontAlpha.Destroy();
	EraseList();
}

BOOL CGuiContacts::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContactDetails* dlgContactDetails)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBg, guiBackground, sOptions);

    m_dlgContactDetails = dlgContactDetails;

    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    Initialize(hWndParent, hWndParent, m_hInst, 0);
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
	TypeContact* sItem = (TypeContact*)lpItem;
	delete sItem;
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

CIssGDIEx& CGuiContacts::GetAlphaPopupGDI()
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
}

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

        RECT rcAlphaFrame = m_rcAlpha;
        OffsetRect(&rcAlphaFrame, -m_rcLocation.left, -m_rcLocation.top);
        Rectangle(m_gdiBackground, rcAlphaFrame, COLOR_CONTACTS_ALPHA_BG,COLOR_CONTACTS_ALPHA_BG);

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
			DrawText(m_gdiBackground.GetDC(), szLetter, rcAlpha, DT_CENTER | DT_TOP, hFontAlpha, 0);
			rcAlpha.top += m_iAlphaTextSpacing;
			rcAlpha.bottom += m_iAlphaTextSpacing;
		}
		CIssGDIEx::DeleteFont(hFontAlpha);

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

TCHAR* CGuiContacts::GetPrimaryPhoneNumber(TypeContact* sContact)
{
    if(!sContact)
        return _T("");

    // for now return the first one you find
    if(m_oStr->GetLength(sContact->szHomeNumber))
        return sContact->szHomeNumber;
    if(m_oStr->GetLength(sContact->szMobileNumber))
        return sContact->szMobileNumber;
    if(m_oStr->GetLength(sContact->szWorkNumber))
        return sContact->szWorkNumber;
    if(m_oStr->GetLength(sContact->szEmailAddr))
        return sContact->szEmailAddr;

    return _T("");
}

void CGuiContacts::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{
    TypeContact* sContact = (sItem->eType == ITEM_Unknown) ? (TypeContact*)sItem->lpItem : 0;
    if(!sContact || !m_sOptions)
        return;

    int iIndent = GetSystemMetrics(SM_CXICON)/2;

    // create draw text
    TCHAR szDrawText[STRING_LARGE];
    m_oStr->Empty(szDrawText);
    if(m_sOptions->eSortContacts == SORT_Company)
    {
        m_oStr->Concatenate(szDrawText, sContact->szLastName);
        if(m_oStr->GetLength(szDrawText))
            m_oStr->Concatenate(szDrawText, _T(", "));
        m_oStr->Concatenate(szDrawText, sContact->szFirstName);
    }
    else if(m_sOptions->eSortContacts == SORT_LastName)
    {
        m_oStr->Concatenate(szDrawText, sContact->szLastName);
        if(m_oStr->GetLength(szDrawText))
            m_oStr->Concatenate(szDrawText, _T(", "));
        m_oStr->Concatenate(szDrawText, sContact->szFirstName);
    }
    else if(m_sOptions->eSortContacts == SORT_FirstName)
    {
        m_oStr->Concatenate(szDrawText, sContact->szFirstName);
        if(m_oStr->GetLength(szDrawText))
            m_oStr->Concatenate(szDrawText, _T(" "));
        m_oStr->Concatenate(szDrawText, sContact->szLastName);
    }
    else if(m_sOptions->eSortContacts == SORT_Email)
    {
        m_oStr->Concatenate(szDrawText, sContact->szEmailAddr);
        if(!m_oStr->GetLength(szDrawText))
        {
            m_oStr->Concatenate(szDrawText, sContact->szLastName);
            if(m_oStr->GetLength(szDrawText))
                m_oStr->Concatenate(szDrawText, _T(", "));
            m_oStr->Concatenate(szDrawText, sContact->szFirstName);
        }
    }

    // draw it
    if(bIsHighlighted)
    {
        ::DrawText(gdi.GetDC(), szDrawText, 
            rcDraw.left + iIndent, rcDraw.top, DT_LEFT, m_hFontContactBold, COLOR_TEXT_NORMAL);
        RECT rcLine2 = rcDraw;
        OffsetRect(&rcLine2, GetSystemMetrics(SM_CXICON)/2, GetSystemMetrics(SM_CXSMICON));
        ::DrawText(gdi.GetDC(), GetPrimaryPhoneNumber(sContact), 
            rcLine2.left + iIndent, rcLine2.top, DT_LEFT, 0, COLOR_TEXT_NORMAL);
    }
    else
    {
        ::DrawText(gdi.GetDC(), szDrawText, 
            rcDraw.left + iIndent, rcDraw.top, DT_LEFT, 0, COLOR_TEXT_NORMAL);
    }
}

void CGuiContacts::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CGuiContacts* pContacts = (CGuiContacts*)lpClass;
    pContacts->DrawListItem(gdi, rcDraw, sItem, bIsHighlighted);
}

BOOL CGuiContacts::OnTimer(WPARAM wParam, LPARAM lParam)
{
	CIssKineticList::OnTimer(wParam, lParam);

	return UNHANDLED;
}

BOOL CGuiContacts::SelecteItem(int iIndex)
{
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return FALSE;

	TypeItems * sListItem = m_arrItems[iIndex];
	if(!sListItem)
		return FALSE;

    TypeContact* sContact = (TypeContact*)sListItem->lpItem;
	if(!sContact)
		return FALSE;
    
#ifdef FIX_ANNOYING_POOM_ONEOFF_ERROR
    int iPoomIndex = oPoom->GetIndexFromOID(sContact->lOid);
#else
    int iPoomIndex = oPoom->GetIndexFromOID(sContact->lOid) - 1;
#endif

	m_dlgContactDetails->SetIndex(iPoomIndex);
    m_dlgContactDetails->Init(m_gdiMem, m_guiBackground);
	int iReturn = m_dlgContactDetails->Launch(m_hWndParent, m_hInst, TRUE);

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
    GetBorder();

    DBG_CONTACTS(_T("CGuiContacts::MoveGui()"), FALSE);
	return TRUE;
}

void CGuiContacts::DrawSelector(CIssGDIEx& gdi, RECT& rcDraw)
{
    // fine-tune the RECT
    RECT rcSelector = rcDraw;
    int iIndent = GetSystemMetrics(SM_CXICON)/4;
    InflateRect(&rcSelector, -iIndent, 0);
    CIssKineticList::DrawSelector(gdi, rcSelector);
}


BOOL CGuiContacts::OnLButtonDown(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

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
	//if(PtInRect(&m_rcLocation, pt) == FALSE)
	//	return FALSE;

	if(m_bScrollAlpha)
	{
		m_bScrollAlpha = FALSE;
		InvalidateRect(m_hWndParent, NULL, FALSE);

		TCHAR szChar = _T('A' + (TCHAR)((pt.y - m_rcAlphaText.top)/(m_iAlphaTextSpacing))) ;
		ScrollTo(szChar);

		return UNHANDLED;
	}

    BOOL bRet = (m_eMouse != GRAB_None?TRUE:FALSE);
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
    CPoomContacts * oPoom = CPoomContacts::Instance();
    IContact* pPoomContact	= oPoom->GetContactFromOID(lOid);
    CPHRE(pPoomContact, _T("pPoomContact is NULL"), E_POINTER);
    CPHRE(m_sOptions, _T("m_sOptions is NULL"), E_POINTER);

    TypeContact* sContact    = new TypeContact();

    // retrieve all the contact info
    BOOL bResult = oPoom->GetContactInfo(pPoomContact, *sContact);
    CBHRE(bResult, _T("GetContactInfo() failed"), E_FAIL);
    
    AddItem(sContact, 0);

    // check if a letter category is needed
    CheckItemCategoryNeeded(sContact);

    // sort - set heights
    g_eSort = m_sOptions->eSortContacts;
    m_arrItems.Sort(CompareContacts);
    SetItemHeights(m_iContactHeight, m_iContactHeightSelected);
    // select it
    int iNewIndex = -1;
    FindContact(lOid, iNewIndex);
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
    TypeContact sNewInfo;

    // get the item from array
    int iContactIndex = -1;
    TypeContact* sContact = FindContact(lOid, iContactIndex);
    CPHRE(sContact, _T("sContact is NULL"), E_POINTER);
    CPHRE(m_sOptions, _T("m_sOptions is NULL"), E_POINTER);

    CPoomContacts * oPoom = CPoomContacts::Instance();
    IContact* pContact	= oPoom->GetContactFromOID(lOid);
    CPHRE(pContact, _T("pContact is NULL"), E_POINTER);

    // retrieve all the contact info
    BOOL bResult = oPoom->GetContactInfo(pContact, sNewInfo);
    CBHRE(bResult, _T("GetContactInfo() failed"), E_FAIL);

    if(ApplyContactChanges(&sNewInfo , sContact))
    {
        CheckItemCategoryNeeded(sContact);
        g_eSort = m_sOptions->eSortContacts;
        int iCurIndex = GetSelectedItemIndex();
        m_arrItems.Sort(CompareContacts);
        SetSelectedItemIndex(GetNextItemIndex(iCurIndex), TRUE);
        DBG_OUT((_T("HandlePoomMessage() edited item 0x%08x re-sort list"), lOid));
    }
    else
    {
        ReDrawList();
        DBG_OUT((_T("HandlePoomMessage() edited item 0x%08x re-draw list"), lOid));
    }

Error:
    if(pContact)
        pContact->Release();

    return hr;
}


HRESULT CGuiContacts::PoomItemDeleted(long lOid)
{
    HRESULT hr = S_OK;

    // get the item from array
    int iContactIndex = -1;
    TypeContact* sContact = FindContact(lOid, iContactIndex);
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

BOOL CGuiContacts::CheckItemCategoryNeeded(TypeContact* sContact)
{
    if(!m_sOptions)
        return FALSE;

    TCHAR* pszCategoryStr = 0;
    if(m_sOptions->eSortContacts == SORT_Company && sContact->szCompany)
        pszCategoryStr = sContact->szCompany;
    else if(m_sOptions->eSortContacts == SORT_FirstName && sContact->szFirstName)
        pszCategoryStr = m_oStr->CreateAndCopy(sContact->szFirstName, 0, 1);
    else if(m_sOptions->eSortContacts == SORT_LastName)
        pszCategoryStr = m_oStr->CreateAndCopy(sContact->szLastName, 0, 1);
    else if(m_sOptions->eSortContacts == SORT_Email)
	{
		pszCategoryStr = m_oStr->CreateAndCopy(sContact->szEmailAddr, 0, 1);
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

    return bReturn;
}

TypeContact* CGuiContacts::FindContact(long lOid, int& iIndex)
{
    // start at the end (if it's a new item that's where it will be)
    int i = GetItemCount();
    while(--i >= 0)
    {
        TypeItems* sItem = m_arrItems[i];
        if(sItem && sItem->lpItem)
        {
            TypeContact* sContact = (TypeContact*)sItem->lpItem;
            if(sContact && sContact->lOid == lOid)
            {
                iIndex = i;
                return sContact;
            }
        }
    }

    return 0;
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
    CPoomContacts* oPoom = CPoomContacts::Instance();
	if(!oPoom || !m_sOptions)
		return;

	if(oPoom->GetNumContacts() == 0)
		return;

    int iNumContacts = oPoom->GetNumContacts();
	for(int i=0; i < iNumContacts; i++)
	{
#ifdef FIX_ANNOYING_POOM_ONEOFF_ERROR
		IContact* pPoomContact	= oPoom->GetContactFromIndex(i + 1);    // poom index 1 based
#else
		IContact* pPoomContact	= oPoom->GetContactFromIndex(i);
#endif
		if(!pPoomContact)
			continue;//return;

        TypeContact* sContact = new TypeContact();
	
		// retrieve all the contact info
		if(!oPoom->GetContactInfo(pPoomContact, *sContact))
		{
            ASSERT(FALSE);
			delete sContact;
            pPoomContact->Release();
			return;
		}
        pPoomContact->Release();

        AddItem(sContact, 0);
        CheckItemCategoryNeeded(sContact);
	}


    // add any necesary spacers
    AddCategory(CATEGORY_SEPARATOR_STRING);

	// now that we have all the entries we have to sort them
    g_eSort = m_sOptions->eSortContacts;
	m_arrItems.Sort(CompareContacts);

    // now remove final separator if it is the last item
    TypeItems* slastItem = (TypeItems *)m_arrItems.LastElement();
    if(slastItem->eType == ITEM_Category)
    {
        int ilast = m_arrItems.GetSize() - 1;
        DeleteItem(ilast);
        m_arrItems.RemoveElementAt(ilast);
    }

    SetItemHeights(m_iContactHeight, m_iContactHeightSelected);
    SetSelectedItemIndex(0, TRUE);
    ResetScrollPos();

    DBG_CONTACTS(_T("PopulateList()"), FALSE);
}

void CGuiContacts::GetCompareString(TCHAR* pszCompare, TypeContact* sContact, int iCompareCount)
{
    // Notes:
    // if no sort criteria, put at end of list

    if(!sContact)
        return;

	CIssString* oStr = CIssString::Instance();
    oStr->Empty(pszCompare);

    TCHAR szEndOfList = _T('z') + 2;

    switch(g_eSort)
    {
        case SORT_LastName:
            {
                if(iCompareCount == 0)
                    oStr->Concatenate(pszCompare, sContact->szLastName);
                else if(iCompareCount > 0)
                    oStr->Concatenate(pszCompare, sContact->szFirstName);
            }
            break;
        case SORT_FirstName:
            {
                if(iCompareCount == 0)
                    oStr->Concatenate(pszCompare, sContact->szFirstName);
                else if(iCompareCount > 0)
                    oStr->Concatenate(pszCompare, sContact->szLastName);
            }
            break;
        case SORT_Email:
            {
                if(iCompareCount == 0)
                    oStr->Concatenate(pszCompare, sContact->szEmailAddr);
                else if(iCompareCount == 1)
                    oStr->Concatenate(pszCompare, sContact->szLastName);
                else if(iCompareCount > 1)
                    oStr->Concatenate(pszCompare, sContact->szFirstName);
            }
            break;
        case SORT_Company:
            {
                if(iCompareCount == 0)
                {
                    oStr->Concatenate(pszCompare, sContact->szCompany);
					oStr->Capitalize(pszCompare);
                    // add something to string so sort will place item after category
                    if(oStr->GetLength(pszCompare))
                        oStr->Concatenate(pszCompare, _T(" a")); 
                }
                else if(iCompareCount == 1)
                    oStr->Concatenate(pszCompare, sContact->szLastName);
                else if(iCompareCount > 1)
                    oStr->Concatenate(pszCompare, sContact->szFirstName);
            }
            break;
    }

    if(!oStr->GetLength(pszCompare))
    {
        // put it at end of list
        oStr->Concatenate(pszCompare, szEndOfList);
    }

}

int CGuiContacts::CompareContacts( const void *arg1, const void *arg2 )
{
	CIssString* oStr = CIssString::Instance();
    TCHAR szCompare1[STRING_LARGE] = {0};
	TCHAR szCompare2[STRING_LARGE] = {0};
    TCHAR szEndOfList = _T('z') + 1;

    TypeItems& sListItem1 = **(TypeItems **)arg1;
    TypeItems& sListItem2 = **(TypeItems **)arg2;

    if(sListItem1.eType == ITEM_Category)
    {
        if(oStr->Compare(sListItem1.szText, CATEGORY_SEPARATOR_STRING) == 0)
            oStr->Concatenate(szCompare1, szEndOfList);
        else
            oStr->Concatenate(szCompare1, sListItem1.szText);
    }

    if(sListItem2.eType == ITEM_Category)
    {
        if(oStr->Compare(sListItem2.szText, CATEGORY_SEPARATOR_STRING) == 0)
            oStr->Concatenate(szCompare2, szEndOfList);
        else
            oStr->Concatenate(szCompare2, sListItem2.szText);
    }

    TypeContact* sEntry1	= (TypeContact*)sListItem1.lpItem;
    TypeContact* sEntry2	= (TypeContact*)sListItem2.lpItem;

    int iCompCnt = -1;
    int iReturnVal = 0;
    while(!iReturnVal && ++iCompCnt < 3)
    {
        GetCompareString(szCompare1, sEntry1, iCompCnt);
        GetCompareString(szCompare2, sEntry2, iCompCnt);
		iReturnVal = oStr->CompareNoCase(szCompare1, szCompare2);
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
	
void CGuiContacts::ScrollTo(TCHAR szChar)
{
	TypeItems* sItem = NULL;

    int iEntryPos = 0;
    int iScrollToPos = 0;
	for(; iEntryPos < m_arrItems.GetSize(); iEntryPos++)
	{
		sItem = m_arrItems[iEntryPos];
		if(sItem == NULL)
			return;

        if(sItem->eType == ITEM_Category)
        {
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
	}

    SetSelectedItemIndex(iScrollToPos, TRUE);
	CIssKineticList::ScrollTo(GetYStartPosition(iScrollToPos));
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
            TypeContact* sContact = 0;
            if(pItem)
                sContact = (TypeContact*)pItem->lpItem;
            if(sContact)
            {
		        SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
                CPoomContacts::Instance()->LaunchEditContactScreen(/*pEntry->*/sContact->lOid);
            }
       }
		break;
	case IDMENU_NewContact:
        {
            // give back taskbar so user can close the outlook window
		    SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
            CPoomContacts::Instance()->CreateNew();
        }
		break;
	case IDMENU_DeleteContact:
        {
            // get selected contact
            TypeItems* pItem = GetSelectedItem();
            if(pItem && pItem->lpItem)
            {
				TypeContact* sContact = (TypeContact*)pItem->lpItem;
				CDlgMsgBox msgBox;
				TCHAR szMsg[STRING_MAX];
				m_oStr->Format(szMsg, _T("Are you sure you want to permanently delete contact: "));
				if(!m_oStr->IsEmpty(sContact->szFirstName))
					m_oStr->Concatenate(szMsg, sContact->szFirstName);
				if(!m_oStr->IsEmpty(sContact->szLastName))
				{
					m_oStr->Concatenate(szMsg, _T(" "));
					m_oStr->Concatenate(szMsg, sContact->szLastName);
				}

                msgBox.Init(m_gdiMem, m_guiBackground);
				int iReturnVal = msgBox.PopupMessage(szMsg, m_hWndParent, m_hInst, MB_YESNO);

                 // delete it
				if(iReturnVal == IDYES)
					CPoomContacts::Instance()->DeleteContact(sContact->lOid);
            }
        }
		break;
	}
	return TRUE;
}

