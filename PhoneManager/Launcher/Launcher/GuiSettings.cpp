#include "StdAfx.h"
#include "resource.h"
#include "GuiSettings.h"
#include "IssCommon.h"
#include "IssRect.h"
#include "DlgContactDetails.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "GuiToolbar.h"
#include "DlgLauncher.h"
#include "ContactsGuiDefines.h"
#include "issdebug.h"
#include "Shlobj.h"
#include <Cpl.h>
#include "IssLocalisation.h"

#define UND				-1

#define IDT_TIMER_Search                WM_USER + 200
#define Search_Timeout                  1000

#ifdef DEBUG
//#define DEBUG_CONTACTS
#endif

#ifndef CPL_IDNAME
#define CPL_IDNAME    100
#endif

#define IDMENU_Select	    5000

typedef LONG (*CPLAPPLET) (HWND hwndCPl, UINT  msg, LPARAM  lParam1, LPARAM  lParam2);

TypeCPL::TypeCPL()
:szName(NULL)
,eGroup(GROUP_Settings)
,szLaunch(NULL)
,iIconIndex(0)
{}

TypeCPL::~TypeCPL()
{
	CIssString* oStr = CIssString::Instance();
	oStr->Delete(&szName);
	oStr->Delete(&szLaunch);

    //if(iIconIndex != 0)
    //    ImageList_Remove(g_hImageList, iIconIndex);
}

CGuiSettings::CGuiSettings(void) 
:m_hImageList(NULL)
,m_bShowSearch(FALSE)
,m_hFontSearch(NULL)
{
    m_hFontSearch           = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON), FW_BOLD, TRUE);
	m_hFontLetters			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);
	m_hFontContact			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_NORMAL, TRUE);
 	m_hFontContactBold		= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);

	m_ptScreenSize.x		= 0;
	m_ptScreenSize.y		= 0;
}

CGuiSettings::~CGuiSettings(void)
{
	CIssGDIEx::DeleteFont(m_hFontLetters);
	CIssGDIEx::DeleteFont(m_hFontContact);
	CIssGDIEx::DeleteFont(m_hFontContactBold);
    CIssGDIEx::DeleteFont(m_hFontSearch);
    //m_hImgFontAlpha.Destroy();
	EraseList();

    if(m_hImageList)
    {
        ImageList_Destroy(m_hImageList);
        m_hImageList = NULL;
    }
}

BOOL CGuiSettings::Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions)
{
	CGuiBase::Init(hWndParent, hInst, gdiMem, gdiBg, guiBackground, sOptions);

    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    Initialize(hWndParent, hWndParent, m_hInst, 0);
    SetCustomDrawFunc(DrawListItem, this);
    SetImageArray(SKIN(IDR_PNG_MenuArray));
	SetSelected(IDR_PNG_ContactsSelector);

    // preload list images
	PreloadImages(hWndParent, m_hInst);

    if(m_gdiBackArrow.GetDC() == NULL)
    { 
        m_gdiBackArrow.LoadImage((IsVGA()?IDR_PNG_FolderArrowFlipVGA:IDR_PNG_FolderArrowFlip), hWndParent, hInst, TRUE);
    }

    if(m_gdiForwardArrow.GetDC() == NULL)
    {
        m_gdiForwardArrow.LoadImage((IsVGA()?IDR_PNG_FolderArrowVGA:IDR_PNG_FolderArrow), hWndParent, hInst, TRUE);
    }

    SetDeleteItemFunc(DeleteMyItem);

	RefreshCPL();
	return TRUE;
}

void CGuiSettings::ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance)
{
    SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    SetImageArray(SKIN(IDR_PNG_MenuArray));

    // preload list images
    PreloadImages(hWnd, hInstance);
}
    
BOOL CGuiSettings::HasFocus()
{
    return !CGuiToolBar::GetToolbar()->HasFocus();
}

CIssImageSliced& CGuiSettings::GetBorder()
{
    if(!m_imgBorder.IsLoaded())
        m_imgBorder.Initialize(m_hWndParent, m_hInst, IsVGA() ? IDR_PNG_ContactsBorderVGA : IDR_PNG_ContactsBorder);

    if(WIDTH(m_rcContactList) != m_imgBorder.GetWidth() || HEIGHT(m_rcContactList) != m_imgBorder.GetHeight())
    {
        RECT rcTemp = m_rcContactList;

        //if(IsLandscape())
        //    rcTemp.right += GetSystemMetrics(SM_CXICON);

        m_imgBorder.SetSize(WIDTH(rcTemp), HEIGHT(rcTemp));

    }

    return m_imgBorder;
}

/*CIssGDIEx& CGuiSettings::GetAlphaPopupGDI()
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
    
CIssImgFont& CGuiSettings::GetAlphaFont()
{
    if(!m_hImgFontAlpha.IsLoaded())
    {
        int h = GetSystemMetrics(SM_CXICON)*4/5;
        UINT uiRes = IsVGA() ? IDR_PNG_FontVGA : IDR_PNG_Font;
        m_hImgFontAlpha.Initialize(uiRes, m_hWndParent, m_hInst, -1);
    }

    return m_hImgFontAlpha;
}*/

BOOL CGuiSettings::Draw(HDC hDC, RECT& rcClient, RECT& rcClip)
{
    if(!IsRectInRect(m_rcLocation, rcClip))
        return TRUE;

    DBG_CONTACTS(_T("CGuiSettings::Draw()"), TRUE);
    if(m_gdiBg->GetDC() == NULL)
    {
        RECT rc;
        ::GetWindowRect(m_hWndParent, &rc);
		m_gdiBg->Create(hDC, rc);
    }

    //Render();
	DrawBackground(m_gdiBg->GetDC(), m_rcLocation, rcClip);
    CIssKineticList::OnDraw(*m_gdiBg, rcClient, rcClip);
    DrawSearch(*m_gdiBg, rcClip);

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

    DBG_CONTACTS(_T("CGuiSettings::Draw()"), FALSE);
	return TRUE;
}

void CGuiSettings::DrawSearch(CIssGDIEx& gdi, RECT& rcDraw)
{
    if(m_bShowSearch == FALSE)
        return;

    DrawTextShadow(gdi, m_szSearch, m_rcLocation, DT_CENTER|DT_VCENTER, m_hFontSearch, RGB(150,150,150), 0);
}
	
BOOL CGuiSettings::DrawBorderOverlay(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rcBorder = m_rcContactList;
    rcBorder.bottom = rcBorder.top + CONTACTS_LIST_INSET;
    if(!IsRectInRect(rcBorder, rcClip))
        return TRUE;

    ::Draw(gdi, rcBorder, GetBorder().GetImage());

    return TRUE;
}
    
CIssGDIEx& CGuiSettings::GetBgGDI()
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
        ::Draw(m_gdiBackground, rcGDI, GetBorder().GetImage(), rcGDI.left, rcGDI.top);
	}
    return m_gdiBackground;
}

BOOL CGuiSettings::DrawBackground(HDC hdc, RECT& rcClient, RECT& rcClip)
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


void CGuiSettings::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeCPL* sLink, BOOL bIsHighlighted)
{
   /* ::Draw(gdi, 
            rcDraw.left + INDENT,rcDraw.top + (HEIGHT(rcDraw)-sLink->gdi.GetHeight())/2,
            sLink->gdi.GetWidth(), sLink->gdi.GetHeight(),
            sLink->gdi,
            0,0);*/
    int iSize = GetSystemMetrics(SM_CYICON);
    ImageList_Draw(m_hImageList, sLink->iIconIndex, gdi, rcDraw.left + INDENT, rcDraw.top + (HEIGHT(rcDraw)-iSize)/2, ILD_TRANSPARENT);

	RECT rc = rcDraw;
	rc.left	= rc.left + 2*INDENT + iSize;  
	DrawTextShadow(gdi, sLink->szName, rc, DT_LEFT|DT_VCENTER|DT_NOPREFIX, m_hFontLetters, RGB(255,255,255), RGB(0,0,0));
}

void CGuiSettings::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CGuiSettings* pContacts = (CGuiSettings*)lpClass;
	if(!sItem || !sItem->lpItem)
		return;
    TypeCPL* sLink = (TypeCPL*)sItem->lpItem;
    pContacts->DrawListItem(gdi, rcDraw, sLink, bIsHighlighted);
}

BOOL CGuiSettings::OnTimer(WPARAM wParam, LPARAM lParam)
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

/*BOOL CGuiSettings::SelecteItem(int iIndex)
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

BOOL CGuiSettings::MoveGui(RECT rcLocation)
{
    if(EqualRect(&rcLocation, &m_rcLocation))
        return TRUE;

	CGuiBase::MoveGui(rcLocation);

    DBG_CONTACTS(_T("CGuiSettings::MoveGui()"), TRUE);

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
    GetBorder();

    DBG_CONTACTS(_T("CGuiSettings::MoveGui()"), FALSE);
	return TRUE;
}

BOOL CGuiSettings::OnLButtonDown(POINT pt)
{
    if(PtInRect(&m_rcLocation, pt) == FALSE)
    {
        m_eMouse = GRAB_None;
        return FALSE;
    }

    m_ptMouseDown = pt;

    return CIssKineticList::OnLButtonDown(pt);
}

BOOL CGuiSettings::OnLButtonUp(POINT pt)
{
    if(PtInRect(&m_rcLocation, pt) == FALSE && m_eMouse == GRAB_None)
        return FALSE;


    BOOL bRet = (m_eMouse == GRAB_List?TRUE:FALSE);
    bRet |=  CIssKineticList::OnLButtonUp(pt);
    return bRet;
}

BOOL CGuiSettings::OnMouseMove(POINT pt)
{
    /*if(PtInRect(&m_rcLocation, pt) == FALSE)
    return FALSE;*/

    return CIssKineticList::OnMouseMove(pt);
}

BOOL CGuiSettings::OnChar(WPARAM wParam, LPARAM lParam)
{
    if(m_sOptions->bShowSettingsHeaders == TRUE)
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

BOOL CGuiSettings::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(HasFocus())
        return CIssKineticList::OnKeyDown(wParam, lParam);

    return FALSE;
}

BOOL CGuiSettings::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

void CGuiSettings::RefreshCPL(void)
{
#ifdef DEBUG
    return; // just takes so damn long
#endif

    if(m_hImageList)
    {
        ImageList_Destroy(m_hImageList);
        m_hImageList = NULL;
    }
    m_hImageList = ImageList_Create(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), ILC_COLOR|ILC_MASK, 0, 100);

	EraseList();
    CIssVector<TypeCPL> arrCPL;
	PopulateList(arrCPL);

    // sort arrCPL
    if(m_sOptions->bShowSettingsHeaders)
        arrCPL.Sort(CompareCPL);
    else
        arrCPL.Sort(CompareCPLNoSections);

    // add pointers to kinetic list with categories
    EnumCPLGroups eGroup = GROUP_Unknown;
    for(int i=0; i<arrCPL.GetSize(); i++)
    {
        TypeCPL* sCPL = arrCPL[i];
        if(!sCPL)
            continue;
        if(m_sOptions->bShowSettingsHeaders && eGroup != sCPL->eGroup)
        {
            eGroup = sCPL->eGroup;
            switch(eGroup)
            {
            case GROUP_Personal:
                AddCategory(ID(IDS_SLIDER_Personal), m_hInst);
                break;
            case GROUP_Settings:
                AddCategory(ID(IDS_SLIDER_Settings), m_hInst);
                break;
            case GROUP_Connection:
                AddCategory(ID(IDS_Connections), m_hInst);
                break;
            }
        }
        AddItem((LPVOID)sCPL, IDMENU_Select);
    }
    arrCPL.RemoveAll();

    SetItemHeights(GetSystemMetrics(SM_CYICON) + INDENT, GetSystemMetrics(SM_CYICON) + INDENT);
    SetSelectedItemIndex(0, TRUE);
}

HRESULT CGuiSettings::PopulateList(CIssVector<TypeCPL>& arrCPL)
{
    m_oStr->Empty(m_szSearch);

    //Go through the \\windows directory looking for *.cpl files.
    WIN32_FIND_DATA wfd;
    TypeCPL* sCPL;
    HRESULT hr = S_OK;

    TCHAR szSearch[MAX_PATH];

    m_oStr->StringCopy(szSearch, _T("\\windows\\*.cpl"));

    HANDLE hFind = FindFirstFile(szSearch, &wfd);
    CBARG(hFind != INVALID_HANDLE_VALUE, _T(""));

    TCHAR szParam[MAX_PATH] = _T("");						//L"cplmain.cpl,";
    TCHAR szName[MAX_PATH]	= _T("");
    TCHAR szRegName[MAX_PATH] = _T("");

    do
    {
        //Got a control panel applet.
        m_oStr->StringCopy(szName, wfd.cFileName);
        HINSTANCE hCPL = LoadLibrary(szName);
        if(!hCPL)
            continue;

        DBG_OUT((_T("CGuiSettings::PopulateList - loaded %s"), szName));

        CPLAPPLET pCPLApplet = (CPLAPPLET)GetProcAddress(hCPL, _T("CPlApplet"));
        if(pCPLApplet)
        {
            pCPLApplet(m_hWndParent, CPL_INIT, 0, 0);

            LONG lNumApplets = pCPLApplet(m_hWndParent, CPL_GETCOUNT, 0, 0);

            DBG_OUT((_T("CGuiSettings::PopulateList - Applet count %d"), (int)lNumApplets));

            for(int i=0; i<lNumApplets; i++)
            {
                //Find the unique name for this CPL applet.
                pCPLApplet(m_hWndParent, CPL_IDNAME, i, (LPARAM)&szRegName);

                //Find the name of this CPL applet.
                NEWCPLINFO CplInfo = {0};
                CplInfo.dwSize = sizeof(NEWCPLINFO);

                LONG lResult = pCPLApplet(m_hWndParent, CPL_NEWINQUIRE, i, (LPARAM)&CplInfo);

                if(CplInfo.szName && CplInfo.hIcon)
                {
                    m_oStr->Format(szName, _T(",%d"), i);
                    m_oStr->StringCopy(szParam, wfd.cFileName);
                    m_oStr->Concatenate(szParam, szName);

                    sCPL = new TypeCPL;
                    CPHR(sCPL, _T("sCPL is NULL"));
                    sCPL->szName = m_oStr->CreateAndCopy(CplInfo.szName);
                    CPHR(sCPL->szName, _T("sCPL->szName is NULL"));
                    sCPL->szLaunch = m_oStr->CreateAndCopy(szParam);
                    CPHR(sCPL->szLaunch, _T("sCPL->szLaunch is NULL"));
                    sCPL->eGroup = GetCPLGroup(szRegName);
                    sCPL->iIconIndex = ImageList_ReplaceIcon(m_hImageList, -1, CplInfo.hIcon);

                    DBG_OUT((_T("CGuiSettings::PopulateList - Applet Name: %s Param: %s"), sCPL->szName, sCPL->szLaunch));
                    
                    arrCPL.AddElement(sCPL);
                    sCPL = NULL;
                }
            }

            pCPLApplet(m_hWndParent, CPL_EXIT, 0, 0);
        }

        FreeLibrary(hCPL);

    } while(FindNextFile(hFind, &wfd));

    DWORD dwFlags   = 0;
    if(arrCPL.GetSize() > 15)
        dwFlags     |= (OPTION_DrawScrollArrows|OPTION_DrawScrollBar|OPTION_Bounce);
    else
        dwFlags |= OPTION_DrawScrollArrows|OPTION_Bounce;
    Initialize(m_hWndParent, m_hWndParent, m_hInst, dwFlags, TRUE);
    
Error:
    if(hFind != INVALID_HANDLE_VALUE)
        FindClose(hFind);
    if(sCPL && hr != S_OK)
        delete sCPL;
    return hr;
}

EnumCPLGroups CGuiSettings::GetCPLGroup(TCHAR* szRegName)
{
    HKEY hKey = NULL;
    TCHAR szKey[STRING_MAX];
    m_oStr->StringCopy(szKey, _T("ControlPanel\\"));
    m_oStr->Concatenate(szKey, szRegName);
    if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, NULL, NULL, &hKey))
        return GROUP_Settings;

    DWORD dwType	= REG_DWORD;
    DWORD dwDataSize= sizeof(DWORD);
    DWORD dwData;
    long lResult = RegQueryValueEx(hKey, _T("Group"), NULL, &dwType, (LPBYTE)&dwData, &dwDataSize);
    RegCloseKey(hKey);
    if(lResult == ERROR_SUCCESS)
        return (EnumCPLGroups)dwData;
    return GROUP_Settings;
}

void CGuiSettings::EraseList()
{
    ResetContent();
}


BOOL CGuiSettings::AddMenuItems()
{
//	m_wndMenu->AddItem(IDS_MENU_NewContact, m_hInst, IDMENU_NewContact);
//	m_wndMenu->AddItem(IDS_MENU_EditContact, m_hInst, IDMENU_EditContact);
//	m_wndMenu->AddItem(IDS_MENU_DeleteContact, m_hInst, IDMENU_DeleteContact);

	return TRUE;
}

BOOL CGuiSettings::OnCommand(WPARAM wParam, LPARAM lParam)
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
        //SHFullScreen(m_hWndParent, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON);
        if(m_sOptions->bHideOnAppLaunch)
            ShowWindow(m_hWndParent, SW_HIDE);
        ShellExecuteEx(&sei);
	}
	break;
  
	default:
		return UNHANDLED;
	}

	return TRUE;
}

int CGuiSettings::CompareCPL( const void *arg1, const void *arg2)
{
    CIssString* oStr = CIssString::Instance();

    TypeCPL& sLink1 = **(TypeCPL **)arg1;
    TypeCPL& sLink2 = **(TypeCPL **)arg2;

    if(sLink1.eGroup == sLink2.eGroup)
        return oStr->CompareNoCase(sLink1.szName, sLink2.szName);
    else
        return (int)sLink1.eGroup - (int)sLink2.eGroup;
}

int CGuiSettings::CompareCPLNoSections( const void *arg1, const void *arg2)
{
    CIssString* oStr = CIssString::Instance();

    TypeCPL& sLink1 = **(TypeCPL **)arg1;
    TypeCPL& sLink2 = **(TypeCPL **)arg2;

    return oStr->CompareNoCase(sLink1.szName, sLink2.szName);
}

void CGuiSettings::DeleteMyItem(LPVOID lpItem)
{
    if(!lpItem)
        return;

    CIssString* oStr = CIssString::Instance();
    TypeCPL* sCPL = (TypeCPL*)lpItem;

    if(sCPL)
        delete sCPL;
}


void CGuiSettings::ScrollTo(TCHAR* szText)
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
           TypeCPL* sCPL = NULL;
           sCPL = (TypeCPL*)sItem->lpItem;

           if(sCPL == NULL || sCPL->szName == NULL)
               break;

            int iFind = m_oStr->FindNoCase(sCPL->szName, szText, 0);
            if(0 == iFind)
            {
                iScrollToPos = iEntryPos;    
                break;
            }
            if(0 < m_oStr->CompareNoCase(sCPL->szName, szText))
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
