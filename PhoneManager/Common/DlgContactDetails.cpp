#include "StdAfx.h"
#include "resource.h"
#include "DlgContactDetails.h"
#include "ObjSkinEngine.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include <phone.h>
#include "ContactsGuiDefines.h"
#include "IssRect.h"
#include "IssImageSliced.h"
#include "IssCommon.h"
#include "CommonDefines.h"
#include "DlgContactMoreInfo.h"
#include "DlgContactRingtone.h"

#ifndef OMIT_CONVERSATION
#include "DlgPerContactConv.h"
#include "DlgContacts.h"
#endif


#define IDMENU_Buttons      5000
#define IDMENU_MoreInfo     5001
#define BUTTON_Height       (GetSystemMetrics(SM_CXICON)*3/2)
#define PICTURE_Width       (GetSystemMetrics(SM_CXSMICON)*6)
#define PICTURE_Height      (GetSystemMetrics(SM_CXSMICON)*9)

CDlgContactDetails::CDlgContactDetails(BOOL bLastNameFirst /*= TRUE*/)
:m_iIndex(0)
,m_bAllowConvHistory(TRUE)
,m_bLastNameFirst(bLastNameFirst)
,m_bAnimate(TRUE)
{
    SetRect(&m_rcPicture, 0,0,0,0);
	m_hFontText		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
	m_hFontBtnText	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_NORMAL, TRUE);
}

CDlgContactDetails::~CDlgContactDetails(void)
{
    CIssGDIEx::DeleteFont(m_hFontText);
    CIssGDIEx::DeleteFont(m_hFontBtnText);
}

BOOL CDlgContactDetails::SetIndex(int iIndex)
{
	m_iIndex= iIndex;
	return TRUE;
}

void CDlgContactDetails::CloseWindow(int iReturn)
{
    m_sContact.Clear();
    m_gdiReflection.Destroy();
    m_gdiPicture.Destroy();
    m_oMenu.ResetContent();
    m_bAllowConvHistory = TRUE;

    SafeCloseWindow(iReturn);
}

void CDlgContactDetails::PreloadImages(HWND hWnd, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssWndTouchMenu* wndMenu)
{
    m_gdiMem    = gdiMem;
    m_wndMenu   = wndMenu;
    if(!m_imgMask.IsLoaded())
    {
        m_imgMask.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_FavoritesImgAlphaVGA:IDR_PNG_FavoritesImgAlpha);
        // make sure we have an alpha value
        //m_imgMask.GetImage().InitAlpha(FALSE);
    }
    if(!m_imgBorder.IsLoaded())
        m_imgBorder.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_FavoritesImgBorderVGA:IDR_PNG_FavoritesImgBorder);
    if(m_gdiBackArrow.GetDC() == NULL)
        m_gdiBackArrow.LoadImage(SKIN(IDR_PNG_DetailsBackGreen), hWnd, hInst, TRUE);

    // i have to load the default pic here too
    if(m_gdiDefaultPic.GetDC() == NULL)
    {
        CIssGDIEx gdiTemp;
        gdiTemp.LoadImage(IsVGA()?IDR_PNG_DetailsDefaultVGA:IDR_PNG_DetailsDefault, hWnd, hInst, TRUE);
        SIZE sz;
        sz.cx   = sz.cy = PICTURE_Width;
        ScaleImage(gdiTemp, m_gdiDefaultPic, sz, FALSE, 0);
    }
}

void CDlgContactDetails::ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInst)
{
    m_imgButtonSel.Destroy();
    m_gdiBackArrow.LoadImage(SKIN(IDR_PNG_DetailsBackGreen), hWnd, hInst, TRUE);
}

BOOL CDlgContactDetails::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    
    // update the contact info
    if(UpdateContact())
        UpdateMenu();

 //   m_bFadeIn = TRUE;
	return TRUE;
}

BOOL CDlgContactDetails::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
	DrawContactPic(gdi, rcClient, rcClip);
    m_oMenu.OnDraw(gdi, rcClient, rcClip);

	/*if(m_bFadeIn == TRUE)
    {
        FadeInScreen();
		return FALSE;
    }*/

	return TRUE;
}

BOOL CDlgContactDetails::DrawContactPic(CIssGDIEx& gdi, RECT rcClient, RECT rcClip)
{

    if(!IsRectInRect(rcClip, m_rcPicture) || m_gdiPicture.GetDC() == NULL)
        return TRUE;

    int iX, iY, iHeight;
    iX = m_rcPicture.left + (WIDTH(m_rcPicture) - m_gdiPicture.GetWidth())/ 2;
    iY = m_rcPicture.top + (HEIGHT(m_rcPicture) - m_gdiPicture.GetHeight())/2;
    iHeight = m_gdiPicture.GetHeight();

    Draw(gdi,
        iX,iY,
        m_gdiPicture.GetWidth(), m_gdiPicture.GetHeight(),
        m_gdiPicture,
        0, 0);

	Draw(gdi,
         iX, iY + iHeight,
         m_gdiReflection.GetWidth(), m_gdiReflection.GetHeight(),
         m_gdiReflection,
         0,0);

	return TRUE;
}

void CDlgContactDetails::DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
	TCHAR szName[STRING_MAX] = _T("");
	/*if(!m_oStr->IsEmpty(m_sContact.szFirstName))
	{
		m_oStr->Concatenate(szName, m_sContact.szFirstName);
		if(!m_oStr->IsEmpty(m_sContact.szLastName))
			m_oStr->Concatenate(szName, _T(" "));
	}
	if(!m_oStr->IsEmpty(m_sContact.szLastName))
		m_oStr->Concatenate(szName, m_sContact.szLastName);*/
	CPoomContacts* pPoom = CPoomContacts::Instance();
	pPoom->GetFormatedNameText(szName, 
								m_sContact.szFirstName,
								m_sContact.szMiddleName,
								m_sContact.szLastName,
                                m_sContact.szCompany,
                                NULL,
								m_bLastNameFirst);
    RECT rc = m_rcTitle;
    rc.left += INDENT;
	DrawText(gdi, szName, rc, DT_LEFT | DT_VCENTER|DT_NOPREFIX, m_hFontLabel, RGB(51,51,51));

}

void CDlgContactDetails::DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgContactDetails* pThis = (CDlgContactDetails*)lpClass;
    if(!pThis || !sItem || !sItem->lpItem)
        return;

    EnumContactInfo* eInfo = (EnumContactInfo*)sItem->lpItem;
    pThis->DrawButtonItem(gdi, bIsHighlighted, rcDraw, eInfo);
}

BOOL CDlgContactDetails::DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumContactInfo* eInfo)
{
	if(bIsHighlighted)
    {
        if(!m_imgButtonSel.IsLoaded() || WIDTH(rc) != m_imgButtonSel.GetWidth() || HEIGHT(rc) != m_imgButtonSel.GetHeight())
        {
            m_imgButtonSel.Initialize(m_hWnd, m_hInst, SKIN(IDR_PNG_DetailsBtnGreen));
            m_imgButtonSel.SetSize(WIDTH(rc), HEIGHT(rc));
        }
        m_imgButtonSel.DrawImage(gdi, rc.left, rc.top);
    }
    else
    {
        if(!m_imgButton.IsLoaded() || WIDTH(rc) != m_imgButton.GetWidth() || HEIGHT(rc) != m_imgButton.GetHeight())
        {
            m_imgButton.Initialize(m_hWnd, m_hInst, IsVGA()?IDR_PNG_DetailsBtnVGA:IDR_PNG_DetailsBtn);
            m_imgButton.SetSize(WIDTH(rc), HEIGHT(rc));
        }
        m_imgButton.DrawImage(gdi, rc.left, rc.top);
    }

	RECT rcText;

	TCHAR szTitle[STRING_MAX] = _T("");
	TCHAR szInfo[STRING_MAX]  = _T("");

	switch(*eInfo)
	{
	case CI_MobileNumber:
		m_oStr->StringCopy(szTitle, _T("Mobile"));
		m_oStr->StringCopy(szInfo, m_sContact.szMobileNumber);
		break;
	case CI_HomeNumber:
		m_oStr->StringCopy(szTitle, _T("Home"));
		m_oStr->StringCopy(szInfo, m_sContact.szHomeNumber);
		break;
    case CI_HomeNumber2:
        m_oStr->StringCopy(szTitle, _T("Home2"));
        m_oStr->StringCopy(szInfo, m_sContact.szHomeNumber2);
        break;
    case CI_HomeFax:
        m_oStr->StringCopy(szTitle, _T("Home Fax"));
        m_oStr->StringCopy(szInfo, m_sContact.szHomeFax);
        break;
	case CI_SMS:
		m_oStr->StringCopy(szTitle, _T("SMS"));
		m_oStr->StringCopy(szInfo, m_sContact.szMobileNumber);
		break;
	case CI_WorkNumber:
		m_oStr->StringCopy(szTitle, _T("Work"));
		m_oStr->StringCopy(szInfo, m_sContact.szWorkNumber);
		break;
    case CI_WorkNumber2:
        m_oStr->StringCopy(szTitle, _T("Work2"));
        m_oStr->StringCopy(szInfo, m_sContact.szWorkNumber2);
        break;
    case CI_WorkFax:
        m_oStr->StringCopy(szTitle, _T("Work Fax"));
        m_oStr->StringCopy(szInfo, m_sContact.szWorkFax);
        break;
    case CI_Car:
        m_oStr->StringCopy(szTitle, _T("Car"));
        m_oStr->StringCopy(szInfo, m_sContact.szCar);
        break;
    case CI_Pager:
        m_oStr->StringCopy(szTitle, _T("Pager"));
        m_oStr->StringCopy(szInfo, m_sContact.szPager);
        break;

	case CI_Email:
		m_oStr->StringCopy(szTitle, _T("E-mail"));
		m_oStr->StringCopy(szInfo, m_sContact.szEmailAddr);
		break;
    case CI_Email2:
        m_oStr->StringCopy(szTitle, _T("E-mail2"));
        m_oStr->StringCopy(szInfo, m_sContact.szEmailAddr2);
        break;
    case CI_Email3:
        m_oStr->StringCopy(szTitle, _T("E-mail3"));
        m_oStr->StringCopy(szInfo, m_sContact.szEmailAddr3);
        break;
    case CI_Count:
        m_oStr->StringCopy(szTitle, _T("More Info"));
        m_oStr->Empty(szInfo);
     //   DrawTextShadow(gdi, szTitle, rc, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));
     //   return TRUE;
        break;
	}

    // title text
	rcText			= rc;
	rcText.bottom	-= HEIGHT(rc)/2;
	rcText.left		+= GetSystemMetrics(SM_CXSMICON)/2;
    rcText.right    -= GetSystemMetrics(SM_CXSMICON)/2;
	DrawTextShadow(gdi, szTitle, rcText, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

    // value text
	rcText.top		+= HEIGHT(rc)/2;
	rcText.bottom	+= HEIGHT(rc)/2;
    DrawText(gdi, szInfo, rcText, DT_LEFT | DT_TOP| DT_END_ELLIPSIS, m_hFontBtnText, (bIsHighlighted?RGB(0,0,0):RGB(167,167,167)));

	return TRUE;
}

void CDlgContactDetails::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rc = m_rcBottomBar;
    rc.right= rcClient.right/2;
    if(IsRectInRect(rc, rcClip))
    {
        DrawTextShadow(gdi, _T("Back"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

        // draw the arrow too
        Draw(gdi,
             WIDTH(m_rcBottomBar)/8 - m_gdiBackArrow.GetWidth(), rc.top + (HEIGHT(m_rcBottomBar)-m_gdiBackArrow.GetHeight())/2,
             m_gdiBackArrow.GetWidth(), m_gdiBackArrow.GetHeight(),
             m_gdiBackArrow,
             0,0);
    }
    rc.left = rc.right;
    rc.right= rcClient.right;
    DrawTextShadow(gdi, _T("Menu"), rc, DT_CENTER|DT_VCENTER, m_hFontLabel, RGB(255,255,255), RGB(0,0,0));

}

void CDlgContactDetails::OnMenuLeft()
{
    CloseWindow(IDOK);
}

void CDlgContactDetails::OnMenuRight()
{
    ShowMenu();
}

BOOL CDlgContactDetails::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnTimer(wParam, lParam);
}


BOOL CDlgContactDetails::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnKeyDown(wParam, lParam);
}

BOOL CDlgContactDetails::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
    {
    case IDOK:
        m_oMenu.OnKeyDown(VK_RETURN, 0);
        break;
    case IDMENU_EditContact:
        EditContact();
        break;
    case IDMENU_SetContactPicture:
        SetContactPicture();
        break;
    case IDMENU_SetContactCamera:
        SetContactCamera();
        break;
    case IDMENU_SetContactRingtone:
        ContactRingtone();
        break;
    case IDMENU_MoreInfo:
        MoreInfo();
        break;
    case IDMENU_Options:
        // should launch the options here
        break;
    case IDMENU_Help:
        //SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_SHOWSIPBUTTON);
        ShowWindow(hWnd, SW_HIDE);
        ShowWindow(GetParent(hWnd), SW_HIDE);
        CreateProcess(_T("peghelp"), _T("contactshelp.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
        ShowWindow(GetParent(hWnd), SW_SHOW);
        ShowWindow(hWnd, SW_SHOW);
        break;
    case IDMENU_Conversation:
        OpenHistory();
        break;
    case IDMENU_Buttons:
        PerformOperation();
        break;
    default:
        return UNHANDLED;
    }

	return TRUE;
}

void CDlgContactDetails::MoreInfo()
{
    if(!IsValidOID(m_sContact.lOid))
        return;

    CDlgContactMoreInfo dlgInfo;

    dlgInfo.SetIndex(m_iIndex);
    dlgInfo.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
    dlgInfo.Launch(GetWnd(), m_hInst, m_bAnimate);
}

void CDlgContactDetails::ContactRingtone()
{
    if(!IsValidOID(m_sContact.lOid))
        return;

    CDlgContactRingtone dlgRingtone;

    dlgRingtone.SetIndex(m_iIndex);
    dlgRingtone.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
    dlgRingtone.Launch(GetWnd(), m_hInst, FALSE);
}

void CDlgContactDetails::SetContactCamera()
{
    if(!IsValidOID(m_sContact.lOid))
        return;

    CPoomContacts* pPoom = CPoomContacts::Instance();
    if(!pPoom)
        return;

    TCHAR* szTitle = pPoom->GetUserData(CI_FileAs, m_sContact.lOid);
    if(!szTitle)
    {
        MessageBeep(MB_ICONHAND);
        return;
    }

    SHCAMERACAPTURE shcc;
    ZeroMemory(&shcc, sizeof(shcc));
    shcc.cbSize             = sizeof(shcc);
    shcc.hwndOwner          = m_hWnd;
    shcc.pszInitialDir      = NULL;
    shcc.pszDefaultFileName = NULL;
    shcc.pszTitle           = szTitle;
    shcc.StillQuality       = CAMERACAPTURE_STILLQUALITY_HIGH;
    shcc.VideoTypes         = CAMERACAPTURE_VIDEOTYPE_STANDARD ;
    shcc.nResolutionWidth   = 0;
    shcc.nResolutionHeight  = 0;
    shcc.nVideoTimeLimit    = 0;
    shcc.Mode               = CAMERACAPTURE_MODE_STILL;

    // Call SHCameraCapture() function
    SHFullScreen(m_hWnd, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
    HRESULT hReturn = SHCameraCapture(&shcc);
    if(hReturn == S_OK && !m_oStr->IsEmpty(shcc.szFile))
    {
        pPoom->AddPictureToContact(m_sContact.lOid, shcc.szFile);
        DeleteFile(shcc.szFile);
        UpdateContact();
    }

}

void CDlgContactDetails::FadeInScreen()
{
    m_bFadeIn = FALSE;
    HDC hDC = GetDC(m_hWnd);
    SlideLeft(hDC, *m_gdiMem);
    ReleaseDC(m_hWnd, hDC);
}

BOOL CDlgContactDetails::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    // so we only do this once
	if(WIDTH(rc) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rc) != GetSystemMetrics(SM_CYSCREEN))
		return FALSE;

	int iIndent = INDENT/4;

	m_rcPicture.left	= iIndent + m_rcArea.left;
	m_rcPicture.top		= iIndent + m_rcArea.top;
	m_rcPicture.right	= PICTURE_Width + m_rcPicture.left;
	m_rcPicture.bottom	= PICTURE_Height + m_rcPicture.top;

    m_rcBackBtn             = m_rcBottomBar;
    m_rcBackBtn.right       = WIDTH(m_rcBottomBar)/2;

    m_rcMenu                = m_rcBottomBar;
    m_rcMenu.left           = WIDTH(m_rcBottomBar)/2;

    m_oMenu.OnSize(m_rcPicture.right + iIndent,
                   m_rcArea.top + iIndent,
                   m_rcArea.right - m_rcPicture.right,
                   HEIGHT(m_rcArea) - 2*iIndent);
    return UNHANDLED;
}

BOOL CDlgContactDetails::IsContactValid(void)
{
    // if the contact has not been set yet then just return TRUE
    if(m_sContact.lOid == 0)
        return TRUE;

    BOOL bResult = TRUE;
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return FALSE;

	IContact* pContact	= oPoom->GetContactFromIndex(m_iIndex);
	if(pContact)
	{
		TypeContact testContact;
		pContact->get_Oid(&testContact.lOid);
		if(m_sContact.lOid != testContact.lOid)
			bResult = FALSE;
        pContact->Release();
	}

	return bResult;
}

// TRUE will tell you if there is an update
BOOL CDlgContactDetails::UpdateContact(void)
{
	// update all fields - return true if any changed
	BOOL bResult = FALSE;
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return FALSE;

	IContact* pContact	= oPoom->GetContactFromIndex(m_iIndex);
	if(!pContact)
        return FALSE;

    m_sContact.Clear();
	// retrieve all the contact info as well as the potential info
	bResult = oPoom->GetContactInfo(pContact, m_sContact, TRUE, PICTURE_Width, PICTURE_Height, COLOR_TEXT_NORMAL);

    pContact->Release();

    if(!m_sContact.gdiPicture)
    {
        int iWidth = m_gdiDefaultPic.GetWidth();
        int iHeight= m_gdiDefaultPic.GetHeight();       
        if(m_imgMask.GetWidth() != iWidth || m_imgMask.GetHeight() != iHeight)
            m_imgMask.SetSize(iWidth, iHeight);
        if(m_imgBorder.GetWidth() != iWidth || m_imgBorder.GetHeight() != iHeight)
            m_imgBorder.SetSize(iWidth, iHeight);
        m_gdiPicture.Create(m_gdiDefaultPic, iWidth, iHeight, TRUE, TRUE);
        m_gdiPicture.SetAlphaMask(m_imgMask.GetImage());
        m_imgBorder.DrawImage(m_gdiPicture, 0,0, ALPHA_AddValue);
        
    }
    else
    {
        int iWidth = m_sContact.gdiPicture->GetWidth();
        int iHeight= m_sContact.gdiPicture->GetHeight();
        if(m_imgMask.GetWidth() != iWidth || m_imgMask.GetHeight() != iHeight)
            m_imgMask.SetSize(iWidth, iHeight);
        if(m_imgBorder.GetWidth() != iWidth || m_imgBorder.GetHeight() != iHeight)
            m_imgBorder.SetSize(iWidth, iHeight);
        m_gdiPicture.Create(*m_sContact.gdiPicture, iWidth, iHeight, TRUE, TRUE);
        m_gdiPicture.SetAlphaMask(m_imgMask.GetImage());
        m_imgBorder.DrawImage(m_gdiPicture, 0,0, ALPHA_AddValue);

        // we don't need it anymore
        m_sContact.gdiPicture->Destroy();
    }

    m_gdiReflection.CreateReflection(m_gdiPicture);

	return bResult;
}

void CDlgContactDetails::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    EnumContactInfo* eOption = (EnumContactInfo*)lpItem;
    delete eOption;
}

void CDlgContactDetails::UpdateMenu()
{
    m_oMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_oMenu.Initialize(m_hWnd, m_hWnd, m_hInst, OPTION_CircularList|OPTION_Bounce);
    m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteMyItem);
    m_oMenu.ResetContent();

    if(!m_oStr->IsEmpty(m_sContact.szMobileNumber))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_MobileNumber;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }
    if(!m_oStr->IsEmpty(m_sContact.szHomeNumber))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_HomeNumber;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons );
    }
    if(!m_oStr->IsEmpty(m_sContact.szHomeNumber2))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_HomeNumber2;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }
    if(!m_oStr->IsEmpty(m_sContact.szHomeFax))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_HomeFax;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }
    if(!m_oStr->IsEmpty(m_sContact.szMobileNumber))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_SMS;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }
    if(!m_oStr->IsEmpty(m_sContact.szWorkNumber))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_WorkNumber;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }
    if(!m_oStr->IsEmpty(m_sContact.szWorkNumber2))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_WorkNumber2;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }

    if(!m_oStr->IsEmpty(m_sContact.szWorkFax))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_WorkFax;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }

    if(!m_oStr->IsEmpty(m_sContact.szPager))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_Pager;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }

    if(!m_oStr->IsEmpty(m_sContact.szCar))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_Car;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }

    if(!m_oStr->IsEmpty(m_sContact.szEmailAddr))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_Email;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }
    if(!m_oStr->IsEmpty(m_sContact.szEmailAddr2))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_Email2;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }
    if(!m_oStr->IsEmpty(m_sContact.szEmailAddr3))
    {
        EnumContactInfo* eInfo = new EnumContactInfo;
        *eInfo = CI_Email3;
        m_oMenu.AddItem(eInfo, IDMENU_Buttons);
    }

    // add the more info button
    EnumContactInfo* eInfo = new EnumContactInfo;
    *eInfo = CI_Count;
    m_oMenu.AddItem(eInfo, IDMENU_MoreInfo);

    m_oMenu.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}

BOOL CDlgContactDetails::OnLButtonDown(HWND hWnd, POINT& pt)
{
    // if the popup menu is going
    if(m_wndMenu->IsWindowUp(TRUE))
        return TRUE;


    if(m_oMenu.OnLButtonDown(pt))
    {}

	return TRUE;
}

BOOL CDlgContactDetails::OnLButtonUp(HWND hWnd, POINT& pt)
{

    // if the popup menu is going
    if(m_wndMenu->IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonUp(pt))
    {}
    else if(PtInRect(&m_rcBackBtn, pt))
        CloseWindow(IDOK);
    else if(PtInRect(&m_rcMenu, pt))
        ShowMenu();
    else if(PtInRect(&m_rcPicture, pt))
        EditContact();


	return TRUE;
}

BOOL CDlgContactDetails::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(m_oMenu.OnMouseMove(pt))
    {}

    return TRUE;
}

void CDlgContactDetails::PerformOperation()
{
    TypeItems* sItem = m_oMenu.GetSelectedItem();
    if(sItem == NULL || sItem->lpItem == NULL)
        return;

    EnumContactInfo* eInfo = (EnumContactInfo*)sItem->lpItem;

	if(*eInfo == CI_MobileNumber)
		MakeCall(m_sContact.szMobileNumber);
    else if(*eInfo == CI_HomeNumber)
		MakeCall(m_sContact.szHomeNumber);
    else if(*eInfo == CI_HomeNumber2)
        MakeCall(m_sContact.szHomeNumber2);
    else if(*eInfo == CI_HomeFax)
        MakeCall(m_sContact.szHomeFax);
    else if(*eInfo == CI_WorkNumber)
		MakeCall(m_sContact.szWorkNumber);
    else if(*eInfo == CI_WorkNumber2)
        MakeCall(m_sContact.szWorkNumber2);
    else if(*eInfo == CI_WorkFax)
        MakeCall(m_sContact.szWorkFax);
    else if(*eInfo == CI_Car)
        MakeCall(m_sContact.szCar);
    else if(*eInfo == CI_Pager)
        MakeCall(m_sContact.szPager);
	else if(*eInfo == CI_SMS)
	{
		PROCESS_INFORMATION pi;
		TCHAR szCommandLine[STRING_MAX*2];
		m_oStr->Format(szCommandLine,  _T("-service \"SMS\" -to \"%s\""), m_sContact.szMobileNumber);
		CreateProcess(_T("tmail.exe"), szCommandLine, NULL, NULL, FALSE, NULL, NULL, NULL, NULL, &pi);
	}
	else if(*eInfo == CI_Email)
	{
        PROCESS_INFORMATION pi = {0};
        TCHAR szCommandLine[STRING_MAX*2] = _T("");
        m_oStr->Format(szCommandLine,  TEXT("-service \"%s\" -to \"%s\""), _T("ActiveSync"), m_sContact.szEmailAddr);
        CreateProcess(_T("tmail.exe"), szCommandLine, NULL, NULL, FALSE, NULL, NULL, NULL, NULL, &pi);
	}
    else if(*eInfo == CI_Email2)
    {
        PROCESS_INFORMATION pi = {0};
        TCHAR szCommandLine[STRING_MAX*2] = _T("");
        m_oStr->Format(szCommandLine,  TEXT("-service \"%s\" -to \"%s\""), _T("ActiveSync"), m_sContact.szEmailAddr2);
        CreateProcess(_T("tmail.exe"), szCommandLine, NULL, NULL, FALSE, NULL, NULL, NULL, NULL, &pi);
    }
    else if(*eInfo == CI_Email3)
    {
        PROCESS_INFORMATION pi = {0};
        TCHAR szCommandLine[STRING_MAX*2] = _T("");
        m_oStr->Format(szCommandLine,  TEXT("-service \"%s\" -to \"%s\""), _T("ActiveSync"), m_sContact.szEmailAddr3);
        CreateProcess(_T("tmail.exe"), szCommandLine, NULL, NULL, FALSE, NULL, NULL, NULL, NULL, &pi);
    }
    Sleep(500);
	CloseWindow(IDCANCEL);
}

void CDlgContactDetails::EditContact()
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

void CDlgContactDetails::SetContactPicture()
{
    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return;

    if(!IsValidOID(m_sContact.lOid))
        return;

    ShowWindow(m_hWnd, SW_HIDE);
    ShowWindow(GetParent(m_hWnd), SW_HIDE);
    BOOL bReturn = oPoom->SetContactPicture(m_sContact.lOid);
    ShowWindow(GetParent(m_hWnd), SW_SHOW);
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateContact();

}

void CDlgContactDetails::OpenHistory()
{
    /*
	DlgContactCallLog dlgCallLog;

    CPoomContacts* oPoom = CPoomContacts::Instance();
    if(!oPoom)
        return;

	IContact* pContact	= oPoom->GetContactFromIndex(m_iIndex);
	if(!pContact)
		return;

	dlgCallLog.Init(pContact, &m_sContact, m_gdiMem);
	dlgCallLog.DoModal(GetWnd(), m_hInst, IDD_DLG_Basic);
    */
    //CObjHistoryItem* oHistoryItem = (CObjHistoryItem*)sItem->lpItem;

#ifndef OMIT_CONVERSATION
    CDlgPerContactConv dlg(FALSE, NULL);
    //CDlgPerContactConv dlg(&m_sContact, &CDlgContacts::GetOptions(), &CDlgContacts::GetCallIcons());
	dlg.SetContact(&m_sContact, &CDlgContacts::GetOptions(), &CDlgContacts::GetCallIcons());
    dlg.Init(m_gdiMem, m_guiBackground, TRUE, TRUE);
    dlg.Launch(GetWnd(), m_hInst, m_bAnimate);
#endif

}

void CDlgContactDetails::MakeCall(TCHAR* szNumber)
{
	PHONEMAKECALLINFO mci = {0};
	mci.cbSize = sizeof(mci);
	mci.dwFlags = 0;
	mci.pszDestAddress = szNumber;
	PhoneMakeCall(&mci);
}

void CDlgContactDetails::ShowMenu()
{
    m_wndMenu->ResetContent();

    m_wndMenu->AddItem(IDS_MENU_EditContact, m_hInst, IDMENU_EditContact);
    m_wndMenu->AddItem(IDS_MENU_SetContactPicture, m_hInst, IDMENU_SetContactPicture);
    m_wndMenu->AddItem(IDS_MENU_SetContactRingtone, m_hInst, IDMENU_SetContactRingtone);
    m_wndMenu->AddItem(IDS_MENU_SetContactCamera, m_hInst, IDMENU_SetContactCamera);
#ifndef OMIT_CONVERSATION
    m_wndMenu->AddItem(IDS_MENU_Conversation, m_hInst, IDMENU_Conversation, m_bAllowConvHistory?NULL:FLAG_Grayed);
#endif
    m_wndMenu->AddSeparator();
    //m_wndMenu->AddItem(IDS_MENU_Options, m_hInst, IDMENU_Options);
    m_wndMenu->AddItem(IDS_MENU_Help, m_hInst, IDMENU_Help);
    m_wndMenu->SetSelectedItemIndex(0, TRUE);

    m_wndMenu->PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        GetSystemMetrics(SM_CXSCREEN)*1/4,0,
        GetSystemMetrics(SM_CXSCREEN)*3/4, GetSystemMetrics(SM_CYSCREEN) - HEIGHT(m_rcBottomBar),
        m_rcMenu.left, m_rcMenu.top, 10,10, ADJUST_Bottom);
}

BOOL CDlgContactDetails::Launch(HWND hWndParent, HINSTANCE hInst, BOOL bAnimate)
{
    m_bAnimate = bAnimate;
    return CDlgBase::Launch(hWndParent, hInst, bAnimate);
}
