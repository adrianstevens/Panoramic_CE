
#include "DlgPerContactBase.h"
#include "IssCommon.h"
#include "IssDebug.h"
#include "IssGDIFX.h"
#include "Resource.h"
#include "ObjSkinEngine.h"
#include "ContactsGuiDefines.h"

#define             HEIGHT_Pic          (GetSystemMetrics(SM_CXICON)*3/2)
#define             HEIGHT_Bar          (HEIGHT_Pic+2*INDENT)

CIssGDIEx*		    CDlgPerContactBase::m_gdiMem = NULL;
CIssWndTouchMenu*   CDlgPerContactBase::m_wndMenu = NULL;
CIssImageSliced*     CDlgPerContactBase::m_imgBg = NULL;
CIssImageSliced*     CDlgPerContactBase::m_imgMask = NULL;              // picture's mask
CIssImageSliced*     CDlgPerContactBase::m_imgBorder = NULL;            // picture's border
CIssGDIEx*           CDlgPerContactBase::m_gdiDefaultPic = NULL;        // our default picture
CIssGDIEx*           CDlgPerContactBase::m_gdiBackArrow = NULL;         // back arrow
CIssKineticList*     CDlgPerContactBase::m_oMenu = NULL;

CDlgPerContactBase::CDlgPerContactBase()
:m_iContactIndex(-1)
,m_szNameOverride(NULL)
,m_bAnimate(TRUE)
{
    m_hFontTimeDate = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*13/32, FW_BOLD, TRUE);

	if(!m_oMenu)
		m_oMenu = new CIssKineticList;
	if(!m_gdiBackArrow)
		m_gdiBackArrow = new CIssGDIEx;
	if(!m_gdiDefaultPic)
		m_gdiDefaultPic = new CIssGDIEx;
	if(!m_imgBorder)
		m_imgBorder = new CIssImageSliced;
	if(!m_imgMask)
		m_imgMask = new CIssImageSliced;
	if(!m_imgBg)
		m_imgBg = new CIssImageSliced;
}

CDlgPerContactBase::~CDlgPerContactBase()
{
	if(m_oMenu)
		m_oMenu->ResetContent();
    m_gdiPicture.Destroy();
    m_oStr->Delete(&m_szNameOverride);

    CIssGDIEx::DeleteFont(m_hFontTimeDate);
}

void CDlgPerContactBase::DeleteAllContent()
{
	if(m_oMenu)
		delete m_oMenu;
	if(m_gdiBackArrow)
		delete m_gdiBackArrow;
	if(m_gdiDefaultPic)
		delete m_gdiDefaultPic;
	if(m_imgBorder)
		delete m_imgBorder;
	if(m_imgMask)
		delete m_imgMask;
	if(m_imgBg)
		delete m_imgBg;

	m_oMenu		= NULL;
	m_gdiBackArrow = NULL;
	m_gdiDefaultPic= NULL;
	m_imgBorder	= NULL;
	m_imgMask	= NULL;
	m_imgBg		= NULL;
}

void CDlgPerContactBase::PreloadImages(HWND hWnd, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssWndTouchMenu* wndMenu)
{
    m_gdiMem    = gdiMem;
    m_wndMenu   = wndMenu;

    if(!m_oMenu)
        m_oMenu = new CIssKineticList;
    if(!m_gdiBackArrow)
        m_gdiBackArrow = new CIssGDIEx;
    if(!m_gdiDefaultPic)
        m_gdiDefaultPic = new CIssGDIEx;
    if(!m_imgBorder)
        m_imgBorder = new CIssImageSliced;
    if(!m_imgMask)
        m_imgMask = new CIssImageSliced;
    if(!m_imgBg)
        m_imgBg = new CIssImageSliced;

	if(!m_imgMask || !m_imgBg || !m_imgBorder || !m_gdiDefaultPic)
		return;

    if(!m_imgMask->IsLoaded())
        m_imgMask->Initialize(hWnd, hInst, IsVGA()?IDR_PNG_FavoritesImgAlphaVGA:IDR_PNG_FavoritesImgAlpha);

    if(!m_imgBg->IsLoaded())
        m_imgBg->Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CallBgVGA:IDR_PNG_CallBg);

    if(!m_imgBorder->IsLoaded())
        m_imgBorder->Initialize(hWnd, hInst, IsVGA()?IDR_PNG_FavoritesImgBorderVGA:IDR_PNG_FavoritesImgBorder);

    // i have to load the default pic here too
    if(m_gdiDefaultPic->GetDC() == NULL)
    {
        CIssGDIEx gdiTemp;
        gdiTemp.LoadImage(IsVGA()?IDR_PNG_DetailsDefaultVGA:IDR_PNG_DetailsDefault, hWnd, hInst, TRUE);
        SIZE sz;
        sz.cx   = sz.cy = HEIGHT_Pic;
        ScaleImage(gdiTemp, *m_gdiDefaultPic, sz, FALSE, 0);

        if(m_imgMask->GetWidth() != HEIGHT_Pic || m_imgMask->GetHeight() != HEIGHT_Pic)
            m_imgMask->SetSize(HEIGHT_Pic, HEIGHT_Pic);
        if(m_imgBorder->GetWidth() != HEIGHT_Pic || m_imgBorder->GetHeight() != HEIGHT_Pic)
            m_imgBorder->SetSize(HEIGHT_Pic, HEIGHT_Pic);
        m_gdiDefaultPic->SetAlphaMask(m_imgMask->GetImage());
        m_imgBorder->DrawImage(*m_gdiDefaultPic, 0,0, ALPHA_AddValue);
    }

    ReloadColorSchemeItems(hWnd, hInst);
}

void CDlgPerContactBase::ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInst)
{
	if(m_gdiBackArrow)
		m_gdiBackArrow->LoadImage(SKIN(IDR_PNG_DetailsBackGreen), hWnd, hInst, TRUE);
}

void  CDlgPerContactBase::CloseWindow(int iReturn)
{
    if(m_wndMenu)
        m_wndMenu->ResetContent();
    if(m_oMenu)
		m_oMenu->ResetContent();
    m_gdiPicture.Destroy();
    m_oStr->Delete(&m_szNameOverride);

    SafeCloseWindow(iReturn);
}

BOOL CDlgPerContactBase::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    // get the contact info if there is any
    UpdateContact();

    PopulateList();

    return TRUE;
}

BOOL CDlgPerContactBase::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    DrawContactBg(gdi, rcClient, rcClip);
    DrawContactPic(gdi, rcClient, rcClip);
    DrawContactText(gdi, rcClient, rcClip);

	if(!m_oMenu)
		return TRUE;

    if(m_oMenu->GetItemCount() == 0)
    {
        RECT rc = m_rcArea;
        rc.top  = m_rcConctactArea.bottom + INDENT;
        DrawTextShadow(gdi, _T("No entries found"), rc, DT_CENTER|DT_TOP, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }
    else
        m_oMenu->OnDraw(gdi, rcClient, rcClip);

    return TRUE;
}

BOOL CDlgPerContactBase::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(m_oMenu)
		return TRUE;

    return m_oMenu->OnKeyDown(wParam, lParam);
}

BOOL CDlgPerContactBase::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    // so we only do this once
    if(WIDTH(rc) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rc) != GetSystemMetrics(SM_CYSCREEN))
        return FALSE;

    int iIndent = INDENT/4;

    m_rcConctactArea        = m_rcArea;
    m_rcConctactArea.left   += iIndent;
    m_rcConctactArea.top    += iIndent;
    m_rcConctactArea.right  -= iIndent;
    m_rcConctactArea.bottom = m_rcConctactArea.top + HEIGHT_Bar + INDENT;

	if(!m_gdiDefaultPic)
		return TRUE;

    int iCentered       = (HEIGHT(m_rcConctactArea) - m_gdiDefaultPic->GetHeight())/2;
    m_rcPicture.left	= m_rcConctactArea.left + iCentered;
    m_rcPicture.top		= m_rcConctactArea.top + iCentered;
    m_rcPicture.right	= HEIGHT_Pic + m_rcPicture.left;
    m_rcPicture.bottom	= HEIGHT_Pic + m_rcPicture.top;

	if(m_oMenu)
		m_oMenu->OnSize(m_rcConctactArea.left,
			m_rcConctactArea.bottom + INDENT,
			WIDTH(m_rcConctactArea),
			m_rcArea.bottom - (m_rcConctactArea.bottom + INDENT) - INDENT);

    return TRUE;
}

BOOL CDlgPerContactBase::OnLButtonUp(HWND hWnd, POINT& pt)
{
    // if the popup menu is going
    if(m_wndMenu && m_wndMenu->IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu && m_oMenu->OnLButtonUp(pt))
    {}
    else if(PtInRect(&m_rcPicture, pt))
    {
        EditContact();
        return TRUE;
    }
    else if(CDlgBase::OnLButtonUp(hWnd, pt))
    {}
    return TRUE;
}

BOOL CDlgPerContactBase::OnLButtonDown(HWND hWnd, POINT& pt)
{
    // if the popup menu is going
    if(m_wndMenu && m_wndMenu->IsWindowUp(TRUE))
        return TRUE;


    if(m_oMenu && m_oMenu->OnLButtonDown(pt))
    {}

    return TRUE;
}

BOOL CDlgPerContactBase::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_oMenu && m_oMenu->OnMouseMove(pt))
    {}
    return TRUE;
}

void CDlgPerContactBase::EditContact()
{
    // just open the contact info screen
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(oPoom)
    {
        IContact* pContact	= oPoom->GetContactFromOID(m_sContact.lOid);
        if(pContact)
        {
            // dh try setting our window fullscreen flags so that user can close contact
            //SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON);	// original FullScreen call
            //SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_SHOWSIPBUTTON);

            ShowWindow(m_hWnd, SW_HIDE);
            ShowWindow(GetParent(m_hWnd), SW_HIDE);
            pContact->Display();
            pContact->Release();
            pContact = NULL;
            ShowWindow(GetParent(m_hWnd), SW_SHOW);
            ShowWindow(m_hWnd, SW_SHOW);
        }
    }

}

BOOL CDlgPerContactBase::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(!m_oMenu)
		return TRUE;

    return m_oMenu->OnTimer(wParam, lParam);
}

BOOL CDlgPerContactBase::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case IDOK:
		if(m_oMenu)
			m_oMenu->OnKeyDown(VK_RETURN, 0);
        break;
    case IDMENU_Selection:
        {
			if(!m_oMenu)
				break;
            TypeItems* sItem = m_oMenu->GetSelectedItem();
            int iIndex = m_oMenu->GetSelectedItemIndex();
            if(!sItem || iIndex == -1)
                break;
            OnSelectMenuItem(iIndex, sItem);
        }
        break;
    case IDMENU_PopupMenu:
        {
            if(!m_wndMenu)
                break;
            TypeItems* sItem = m_wndMenu->GetSelectedItem();
            int iIndex = m_wndMenu->GetSelectedItemIndex();
            if(!sItem || iIndex == -1)
                break;
            OnSelectPopupMenuItem(iIndex, sItem);
        }
        break;
    default:
        return UNHANDLED;
    }

    return TRUE;
}

void CDlgPerContactBase::DrawContactText(CIssGDIEx& gdi, RECT rcClient, RECT rcClip)
{
    if(!IsRectInRect(rcClip, m_rcConctactArea))
        return;

    RECT rc = m_rcConctactArea;
    rc.bottom = rc.top + HEIGHT(m_rcConctactArea)/3;
    rc.left  = m_rcPicture.right + (m_rcPicture.top - m_rcConctactArea.top);
    rc.right -= INDENT;

    if(!m_oStr->IsEmpty(m_szNameOverride))
    {
        DrawTextShadow(gdi, m_szNameOverride, rc, DT_LEFT|DT_VCENTER|DT_NOPREFIX, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }
    else if(m_oStr->IsEmpty(m_sContact.szFirstName) && m_oStr->IsEmpty(m_sContact.szLastName))
    {
        DrawTextShadow(gdi, _T("Unknown"), rc, DT_LEFT|DT_VCENTER|DT_NOPREFIX, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }
    else
    {
        CPoomContacts* oPoom = CPoomContacts::Instance();
        TCHAR szText[STRING_MAX];
        oPoom->GetFormatedNameText(szText, m_sContact.lOid);
        DrawTextShadow(gdi, szText, rc, DT_LEFT|DT_VCENTER|DT_NOPREFIX, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
    }

    rc.top      = rc.bottom;
    rc.bottom   = m_rcConctactArea.bottom;

    DrawContactOtherText(gdi, rc);

}

void CDlgPerContactBase::DrawContactBg(CIssGDIEx& gdi, RECT rcClient, RECT rcClip)
{
    if(!m_imgBg || !IsRectInRect(rcClip, m_rcConctactArea))
        return;

    if(m_imgBg->GetWidth() != WIDTH(m_rcConctactArea) || m_imgBg->GetHeight() != HEIGHT(m_rcConctactArea))
        m_imgBg->SetSize(WIDTH(m_rcConctactArea), HEIGHT(m_rcConctactArea));

    m_imgBg->DrawImage(gdi, m_rcConctactArea.left, m_rcConctactArea.top);
}

BOOL CDlgPerContactBase::DrawContactPic(CIssGDIEx& gdi, RECT rcClient, RECT rcClip)
{
    if(!m_gdiDefaultPic || !IsRectInRect(rcClip, m_rcPicture))
        return TRUE;

    CIssGDIEx* gdiPic = m_gdiDefaultPic;
    if(m_gdiPicture.GetDC() != NULL)
        gdiPic = &m_gdiPicture;

    int iX, iY, iHeight;
    iX = m_rcPicture.left + (WIDTH(m_rcPicture) - m_gdiDefaultPic->GetWidth())/ 2;
    iY = m_rcPicture.top + (HEIGHT(m_rcPicture) - m_gdiDefaultPic->GetHeight())/2;
    iHeight = m_gdiDefaultPic->GetHeight();

    Draw(gdi,
        iX,iY,
        m_gdiDefaultPic->GetWidth(), m_gdiDefaultPic->GetHeight(),
        *gdiPic,
        0, 0);

    return TRUE;
}

void CDlgPerContactBase::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    if(m_gdiBackArrow && IsRectInRect(rc, rcClip))
    {
        DrawTextShadow(gdi, _T("Back"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

        // draw the arrow too
        Draw(gdi,
            WIDTH(m_rcBottomBar)/8 - m_gdiBackArrow->GetWidth(), rc.top + (HEIGHT(m_rcBottomBar)-m_gdiBackArrow->GetHeight())/2,
            m_gdiBackArrow->GetWidth(), m_gdiBackArrow->GetHeight(),
            *m_gdiBackArrow,
            0,0);
    }
    rc.left = rc.right;
    rc.right= rcClient.right;
    DrawTextShadow(gdi, _T("Menu"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

}

void CDlgPerContactBase::OnMenuLeft()
{
    CloseWindow(IDOK);
}

void CDlgPerContactBase::OnMenuRight()
{
    // put up the menu
    OnMenu();
}

void CDlgPerContactBase::UpdateContact()
{
    m_sContact.Clear();
    m_gdiPicture.Destroy();

    if(m_iContactIndex == -1)
        return;

    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return;

    IContact* pContact	= oPoom->GetContactFromIndex(m_iContactIndex);
    if(!pContact)
        return;

    // retrieve all the contact info as well as the potential info
    BOOL bResult = oPoom->GetContactInfo(pContact, m_sContact, FALSE, HEIGHT_Pic, HEIGHT_Pic, COLOR_TEXT_NORMAL);

    m_sContact.gdiPicture = oPoom->GetUserPicture(pContact, HEIGHT_Pic, HEIGHT_Pic, COLOR_TEXT_NORMAL, TRUE);

    pContact->Release();

    if(m_sContact.gdiPicture && m_imgMask && m_imgBorder)
    {
        int iWidth = HEIGHT_Pic;
        int iHeight= HEIGHT_Pic;
        if(m_imgMask->GetWidth() != iWidth || m_imgMask->GetHeight() != iHeight)
            m_imgMask->SetSize(iWidth, iHeight);
        if(m_imgBorder->GetWidth() != iWidth || m_imgBorder->GetHeight() != iHeight)
            m_imgBorder->SetSize(iWidth, iHeight);
        m_gdiPicture.Create(*m_sContact.gdiPicture, iWidth, iHeight, FALSE, TRUE);
        RECT rc;
        SetRect(&rc, 0,0,iWidth, iHeight);
        FillRect(m_gdiPicture, rc, RGB(0,0,0));
        // draw it centered
        BitBlt(m_gdiPicture,
            0,0,
            iWidth, iHeight,
            *m_sContact.gdiPicture,
            (m_sContact.gdiPicture->GetWidth()-iWidth)/2, (m_sContact.gdiPicture->GetHeight()-iHeight)/2,
            SRCCOPY);
        m_gdiPicture.SetAlphaMask(m_imgMask->GetImage());
        m_imgBorder->DrawImage(m_gdiPicture, 0,0, ALPHA_AddValue);

        // we don't need it anymore
        delete m_sContact.gdiPicture;
        m_sContact.gdiPicture = NULL;
    }
}

void CDlgPerContactBase::SetNameOverride(TCHAR* szName)
{
    m_oStr->Delete(&m_szNameOverride);

    if(m_oStr->IsEmpty(szName))
        return;

    m_szNameOverride = m_oStr->CreateAndCopy(szName);
}

BOOL CDlgPerContactBase::Launch(HWND hWndParent, HINSTANCE hInst, BOOL bAnimate)
{
    m_bAnimate = bAnimate;
    return CDlgBase::Launch(hWndParent, hInst, bAnimate);
}