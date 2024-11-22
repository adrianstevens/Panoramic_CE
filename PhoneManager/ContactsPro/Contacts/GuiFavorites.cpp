#include "StdAfx.h"
#include "resource.h"
#include "GuiFavorites.h"
#include "IssCommon.h"
#include "IssRect.h"
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

#define IDMENU_Select	                5000
#define IDMENU_SelectRunningPrograms    5001
#define IDMENU_SelectProgramItem        5002


CGuiFavorites::CGuiFavorites(void) 
:m_bShowRunningList(FALSE)
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

}

CGuiFavorites::~CGuiFavorites(void)
{
	CIssGDIEx::DeleteFont(m_hFontLetters);
	CIssGDIEx::DeleteFont(m_hFontContact);
	CIssGDIEx::DeleteFont(m_hFontContactBold);
    //m_hImgFontAlpha.Destroy();
	EraseList();

}

BOOL CGuiFavorites::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContactDetails* dlgContactDetails)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBg, guiBackground, sOptions);

    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    Initialize(hWndParent, hWndParent, m_hInst, 0);
    SetCustomDrawFunc(DrawListItem, this);
    SetImageArray(SKIN(IDR_PNG_MenuArray));
	SetSelected(IDR_PNG_ContactsSelector);

    // preload list images
	PreloadImages(hWndParent, m_hInst);

	RefreshToday();
	return TRUE;
}

void CGuiFavorites::ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance)
{
    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    SetImageArray(SKIN(IDR_PNG_MenuArray));

    // preload list images
    PreloadImages(hWnd, hInstance);
}
    
BOOL CGuiFavorites::HasFocus()
{
    return !CGuiToolBar::GetToolbar()->HasFocus();
}

BOOL CGuiFavorites::Draw(HDC hDC, RECT& rcClient, RECT& rcClip)
{
    if(!IsRectInRect(m_rcLocation, rcClip))
        return TRUE;

    DBG_CONTACTS(_T("CGuiFavorites::Draw()"), TRUE);
    if(m_gdiBg->GetDC() == NULL)
    {
        RECT rc;
        ::GetWindowRect(m_hWndParent, &rc);
		m_gdiBg->Create(hDC, rc);
    }

    //Render();
	DrawBackground(m_gdiBg->GetDC(), m_rcLocation, rcClip);
    CIssKineticList::OnDraw(*m_gdiBg, rcClient, rcClip);

    RECT rcBlit = rcClip;
    rcBlit.bottom = min(rcBlit.bottom, m_rcLocation.bottom);
    rcBlit.left = max(rcBlit.left, m_rcLocation.left);
    BitBlt(hDC,
		rcBlit.left,rcBlit.top,
		WIDTH(rcBlit), HEIGHT(rcBlit),
		m_gdiBg->GetDC(),
		rcBlit.left,rcBlit.top,
		SRCCOPY);

    DBG_CONTACTS(_T("CGuiFavorites::Draw()"), FALSE);
	return TRUE;
}
	
CIssGDIEx& CGuiFavorites::GetBgGDI()
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
        /*RECT rcBorder = m_rcContactList;
        rcBorder.top += CONTACTS_LIST_INSET;
        ::Draw(m_gdiBackground, rcBorder, GetBorder().GetImage(), rcBorder.left, rcBorder.top);*/
	}
    return m_gdiBackground;
}

BOOL CGuiFavorites::DrawBackground(HDC hdc, RECT& rcClient, RECT& rcClip)
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


void CGuiFavorites::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, EnumTodayItems* eToday, BOOL bIsHighlighted)
{
   /* ::Draw(gdi, 
            rcDraw.left + INDENT,rcDraw.top + (HEIGHT(rcDraw)-sLink->gdi.GetHeight())/2,
            sLink->gdi.GetWidth(), sLink->gdi.GetHeight(),
            sLink->gdi,
            0,0);*/
    /*int iSize = GetSystemMetrics(SM_CYICON);
    ImageList_Draw(m_hImageList, sLink->iIconIndex, gdi, rcDraw.left + INDENT, rcDraw.top + (HEIGHT(rcDraw)-iSize)/2, ILD_TRANSPARENT);

	RECT rc = rcDraw;
	rc.left	= rc.left + 2*INDENT + iSize;  
	DrawTextShadow(gdi, sLink->szName, rc, DT_LEFT|DT_VCENTER|DT_NOPREFIX, m_hFontLetters, RGB(255,255,255), RGB(0,0,0));*/
}

void CGuiFavorites::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CGuiFavorites* pContacts = (CGuiFavorites*)lpClass;
	if(!sItem || !sItem->lpItem)
		return;
    EnumTodayItems* eItem = (EnumTodayItems*)sItem->lpItem;
    pContacts->DrawListItem(gdi, rcDraw, eItem, bIsHighlighted);
}

BOOL CGuiFavorites::OnTimer(WPARAM wParam, LPARAM lParam)
{
	CIssKineticList::OnTimer(wParam, lParam);

	return UNHANDLED;
}

/*BOOL CGuiFavorites::SelecteItem(int iIndex)
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

BOOL CGuiFavorites::MoveGui(RECT rcLocation)
{
    if(EqualRect(&rcLocation, &m_rcLocation))
        return TRUE;

	CGuiBase::MoveGui(rcLocation);

    DBG_CONTACTS(_T("CGuiFavorites::MoveGui()"), TRUE);

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

    // reload images
    GetBgGDI();

    DBG_CONTACTS(_T("CGuiFavorites::MoveGui()"), FALSE);
	return TRUE;
}

BOOL CGuiFavorites::OnLButtonDown(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	m_ptMouseDown = pt;

	return CIssKineticList::OnLButtonDown(pt);
}

BOOL CGuiFavorites::OnLButtonUp(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

	return CIssKineticList::OnLButtonUp(pt);
}

BOOL CGuiFavorites::OnMouseMove(POINT pt)
{
	if(PtInRect(&m_rcLocation, pt) == FALSE)
		return FALSE;

    return CIssKineticList::OnMouseMove(pt);
}

BOOL CGuiFavorites::OnChar(WPARAM wParam, LPARAM lParam)
{
    TCHAR c = LOWORD(wParam);
	if(iswalpha(c))
        ScrollTo(toupper(c));

	return UNHANDLED;
}

BOOL CGuiFavorites::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(HasFocus())
        return CIssKineticList::OnKeyDown(wParam, lParam);

    return FALSE;
}

BOOL CGuiFavorites::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}
   
void CGuiFavorites::RefreshToday(void)
{
	EraseList();
	PopulateList();

    SetSelectedItemIndex(0, TRUE);
}

HRESULT CGuiFavorites::PopulateList()
{
    HRESULT hr = S_OK;

    EnumTodayItems* eItem = NULL;

    // add the running programs link
    eItem = new EnumTodayItems;
    CPHR(eItem, _T("eItem"));
    *eItem = TODAY_RunningPrograms;
    hr = AddItem((LPVOID)eItem, IDMENU_SelectRunningPrograms);
    CHR(hr, _T("AddItem"));

        
Error:
    if(eItem && hr != S_OK)
        delete eItem;

    return hr;
}

void CGuiFavorites::EraseList()
{
    ResetContent();
}


BOOL CGuiFavorites::AddMenuItems()
{
	return TRUE;
}

BOOL CGuiFavorites::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case IDMENU_Select:
	{
        TypeItems* sItem = GetSelectedItem();
        if(!sItem || !sItem->lpItem)
            return TRUE;

        TypeCPL* sLink = (TypeCPL*)sItem->lpItem;

        SHELLEXECUTEINFO sei = {0};
        sei.cbSize	= sizeof(sei);
        sei.hwnd	= m_hWndParent;
        sei.nShow	= SW_SHOWNORMAL;
        sei.lpFile = _T("\\Windows\\ctlpnl.exe");
        sei.lpParameters = sLink->szLaunch;

        // give back taskbar so user can close the outlook window
        SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);

        ShellExecuteEx(&sei);
	}
	break;
  
	default:
		return UNHANDLED;
	}

	return TRUE;
}

int CGuiFavorites::CompareCPL( const void *arg1, const void *arg2 )
{
    CIssString* oStr = CIssString::Instance();

    TypeCPL& sLink1 = **(TypeCPL **)arg1;
    TypeCPL& sLink2 = **(TypeCPL **)arg2;

    if(sLink1.eGroup == sLink2.eGroup)
        return oStr->Compare(sLink1.szName, sLink2.szName);
    else
        return (int)sLink1.eGroup - (int)sLink2.eGroup;
}

