#include "StdAfx.h"
#include "resourceppc.h"
#include "DlgAreaCode.h"
#include "IssRegistry.h"
#include "PoomContacts.h"
#include "CommonDefines.h"
#include <regext.h>
#include <snapi.h>
#include "ContactsGuiDefines.h"
#include "IssGDIDraw.h"
#include "IssGDIFX.h"
#include "DlgMoreApps.h"
//#include "DlgSearch.h"
#include "ObjSkinEngine.h"
#include "DlgList.h"

//#define GLOBE_HEIGHT        (3*GetSystemMetrics(SM_CXICON))
#define GLOBE_HEIGHT        (IsVGA()?200:100)

#define NUMBER_OF_GLOBE_FRAMES  31
#define GLOBE_ANI_RATE          33 //30 fps

#define IDT_GLOBE_TIMER         1
#define IDT_INVALID             8

#define IDMENU_Menu             (WM_USER + 380)
#define IDMENU_Area             (WM_USER + 400) //if you're gonna use it for math include the brackets

CDlgAreaCode::CDlgAreaCode(void)
{
    int iFontSize = GetSystemMetrics(SM_CXICON)*4/9;

	// PA: it's actually declared twice so I'll leave in the second on
    //m_hFontSmText       = CIssGDIEx::CreateFont(iFontSize, FW_BOLD, TRUE);
    m_hFontSmallerText  = CIssGDIEx::CreateFont(iFontSize-1, FW_BOLD, TRUE);
    m_hFontSmText       = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*4/9, FW_BOLD, TRUE);
    m_hFontText         = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)/2, FW_BOLD, TRUE);
    m_hFontButton       = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/4, FW_NORMAL, TRUE);
    m_hFontAreaCode     = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*2/3/*/2*/, FW_BOLD, TRUE);

    for(int i = 0; i < 10; i++)
        m_bNumBtns[i] = FALSE;
    m_bClearBtn     = FALSE;
    m_bLocateBtn    = FALSE;

    m_eState        = AS_Search;
    m_eLookupType   = LT_CountryCodes;
    
    m_oStr->Empty(m_szAreaCode);

    m_iGlobeFrame = 0;

    //load in the current skin
    DWORD dwItem;
    if(S_OK == GetKey(REG_Save, _T("ColorScheme"), dwItem))
        g_eColorScheme = (EnumColorScheme)dwItem;
}

CDlgAreaCode::~CDlgAreaCode(void)
{
    CIssGDIEx::DeleteFont(m_hFontText);
    CIssGDIEx::DeleteFont(m_hFontSmText);
    CIssGDIEx::DeleteFont(m_hFontSmallerText);
    CIssGDIEx::DeleteFont(m_hFontButton);
    CIssGDIEx::DeleteFont(m_hFontAreaCode);
	m_wndMenu.ResetContent();
	m_oArea.Destroy();
	CDlgList::ResetListContent();
	CIssKineticList::DeleteAllContent();
	m_oStr->DeleteInstance();
}

BOOL CDlgAreaCode::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    m_bDrawTopBar = FALSE;
    m_bDrawBottomBar = TRUE;

    m_guiBackground->Init(hWnd, m_hInst);

    return CDlgBase::OnInitDialog(hWnd, wParam, lParam);
}

BOOL CDlgAreaCode::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == WA_ACTIVE) 
    { 
        SHFullScreen(m_hWnd, SHFS_HIDETASKBAR /*| SHFS_HIDESTARTICON*/ | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
    }
    return TRUE;
}

BOOL CDlgAreaCode::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) >= IDMENU_Area && LOWORD(wParam) < IDMENU_Area + 500)
    {
        int iIndex = LOWORD(wParam) - IDMENU_Area;

        //do some cool shit
        AreaCodeType* area = NULL;
        if(m_eLookupType == LT_AreaCodes)
        {
            area = m_oArea.GetAreaCodeInfo(wParam - IDMENU_Area);
            if(area == NULL)
                return TRUE;
            m_oStr->IntToString(m_szAreaCode, area->iAreaCode);
            return OnLocate();
        }
        else if(m_eLookupType == LT_CountryCodes)
        {
            area = m_oArea.GetCountryCodeInfo(wParam - IDMENU_Area);
            if(area == NULL)
                return TRUE;
            m_oStr->IntToString(m_szAreaCode, area->iCountryCode);
            return OnLocate();
        }
        else //other codes
        {
            area = m_oArea.GetOtherInfo(wParam - IDMENU_Area);
            if(area == NULL)
                return TRUE;
            m_oStr->IntToString(m_szAreaCode, area->iAreaCode);
            return OnLocate();
        }
        return TRUE;
    }

	switch(wParam)
	{
	case IDMENU_Quit:
		//m_oSound.StopRecording();
		PostQuitMessage(0);
		break;
    case IDMENU_Help:
        CreateProcess(_T("peghelp"), _T("areacodehelp.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
        break;
	case IDMENU_MoreApps:
		LaunchMoreApps();
		break;
    case IDMENU_About:
        ChangeState(AS_About);
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    
    case IDMENU_World:
    case IDMENU_NorthAmerica:
    case IDMENU_Australia:
    case IDMENU_NewZealand:
    case IDMENU_UK:
        m_eLookupType = (EnumLookupType)(wParam - IDMENU_World);
        m_oArea.SetLookupType(m_eLookupType);//might as well do it here
        break;
	default:
		return FALSE;
	}
	return TRUE;
}

void CDlgAreaCode::LaunchMoreApps()
{
	CDlgMoreApps dlgMoreApps;
	dlgMoreApps.Launch(m_hInst, m_hWnd);
}

BOOL CDlgAreaCode::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(CDlgBase::OnLButtonDown(hWnd, pt) == TRUE)
        return TRUE;

    //check state
    if(m_eState == AS_Search)
    {
        //just check the buttons and change the draw state
        if(PtInRect(&m_rcClearBtn, pt))
        {
            m_bClearBtn = TRUE;
            InvalidateRect(m_hWnd, &m_rcClearBtn, FALSE);
            UpdateWindow(m_hWnd);
            return TRUE;
        }
        if(PtInRect(&m_rcLocateBtn, pt))
        {
            m_bLocateBtn = TRUE;
            InvalidateRect(m_hWnd, &m_rcLocateBtn, FALSE);
            UpdateWindow(m_hWnd);
            return TRUE;
        }

        for(int i = 0; i < 10; i++)
        {
            if(PtInRect(&m_rcNumBtns[i], pt))
            {
                m_bNumBtns[i] = TRUE;
                InvalidateRect(m_hWnd, &m_rcNumBtns[i], FALSE);
                UpdateWindow(m_hWnd);
                return TRUE;
            }
        }
        
        //probably need to handle menus here too ... 
    }

	return UNHANDLED;
}

BOOL CDlgAreaCode::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_wndMenu.IsWindowUp(TRUE))
        return UNHANDLED;
    
    if(CDlgBase::OnLButtonUp(hWnd, pt) == TRUE)
        return TRUE;

    //check state first
    if(m_eState == AS_Search)
    {
        //just check the buttons and change the draw state
        if(PtInRect(&m_rcClearBtn, pt) && m_bClearBtn)
        {
            m_bClearBtn = FALSE;
            InvalidateRect(m_hWnd, &m_rcClearBtn, FALSE);
            OnClear();
            return TRUE;
        }
        if(PtInRect(&m_rcLocateBtn, pt) && m_bLocateBtn)
        {
            m_bLocateBtn = FALSE;
            InvalidateRect(m_hWnd, &m_rcLocateBtn, FALSE);
            OnLocate();
            return TRUE;
        }

        for(int i = 0; i < 10; i++)
        {
            if(PtInRect(&m_rcNumBtns[i], pt) == TRUE 
                && m_bNumBtns[i] == TRUE)
            {
                m_bNumBtns[i] = FALSE;
                InvalidateRect(m_hWnd, &m_rcNumBtns[i], FALSE);
                if(i == 9)
                    OnNumButton(0);
                else
                    OnNumButton(i+1);
                return TRUE;
            }
            m_bNumBtns[i] = FALSE;
        }

        //probably need to handle menus here too ... 


        //otherwise clear the button states and redraw ... 
        m_bLocateBtn = FALSE;
        m_bClearBtn = FALSE;

        if(PtInRect(&m_rcSearchBar, pt))
        {
        //    if(m_eLookupType == LT_AreaCodes)
        //        m_eLookupType = LT_CountryCodes;
        //    else
        //        m_eLookupType = LT_AreaCodes;
            OnSearchBy();
        }

        InvalidateRect(m_hWnd, NULL, FALSE);
        return TRUE;        
    }

    return UNHANDLED;
}

BOOL CDlgAreaCode::OnMouseMove(HWND hWnd, POINT& pt)
{
	return TRUE;
}

BOOL CDlgAreaCode::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case IDT_INVALID:
        KillTimer(hWnd, IDT_INVALID);
        OnClear();
        break;
    case IDT_GLOBE_TIMER:
        {
            m_iGlobeFrame++;
            AreaCodeType* area = m_oArea.GetCodeInfo();

            if(m_iGlobeFrame >= NUMBER_OF_GLOBE_FRAMES)
            {
                m_iGlobeFrame = 0;
            }
            if(area == NULL)
            {
                KillTimer(hWnd, IDT_GLOBE_TIMER);
                return TRUE;
            }
            else if(area->iFrame == m_iGlobeFrame)
            {
                KillTimer(hWnd, IDT_GLOBE_TIMER);
                ChangeState(AS_Display);
                InvalidateRect(m_hWnd, NULL, FALSE);//since we're changing states
                return TRUE;
            }
            InvalidateRect(m_hWnd, &m_rcGlobe, FALSE);
            UpdateWindow(m_hWnd);//and redraw now
        }
        break;
    default:
        break;
    }
	return TRUE;
}

BOOL CDlgAreaCode::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgAreaCode::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(CDlgBase::OnKeyDown(hWnd, wParam, lParam) == TRUE)
            return TRUE;
	return UNHANDLED;
}

BOOL CDlgAreaCode::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(CDlgBase::OnKeyUp(hWnd, wParam, lParam) == TRUE)
        return TRUE;
    if(LOWORD(wParam) == VK_RETURN && m_eState == AS_Search)
        return OnLocate();


    return UNHANDLED;
}

BOOL CDlgAreaCode::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

    CDlgBase::OnSize(hWnd, wParam, lParam);

    // so we only do this once
    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN) || HEIGHT(rcClient) != GetSystemMetrics(SM_CYSCREEN))
        return FALSE;

    //ok this should be fun ....
    int iIndent = INDENT*2;

    m_rcTopGreenBar.left    = rcClient.left + iIndent;
    m_rcTopGreenBar.right   = rcClient.right - iIndent;
    m_rcTopGreenBar.top     = rcClient.top + iIndent;
    m_rcTopGreenBar.bottom  = m_rcTopGreenBar.top + GetSystemMetrics(SM_CXSMICON)*3/2;

    m_rcTopText             = m_rcTopGreenBar;
    m_rcTopText.left        += iIndent;

    m_rcGlobe.top           = m_rcTopGreenBar.top;
    m_rcGlobe.right         = m_rcTopGreenBar.right;
    m_rcGlobe.left          = m_rcTopGreenBar.right - GLOBE_HEIGHT;
    m_rcGlobe.bottom        = m_rcGlobe.top + GLOBE_HEIGHT;

    m_rcShadow.top          = m_rcTopGreenBar.bottom + iIndent;
    m_rcShadow.left         = rcClient.left + iIndent;
    m_rcShadow.right        = rcClient.right - iIndent;
    m_rcShadow.bottom       = m_rcShadow.top + GetSystemMetrics(SM_CXICON)*8/3;

    m_rcShadowLong          = m_rcShadow;
    m_rcShadowLong.bottom   = rcClient.bottom - BAR_BOTTOM_Height - iIndent;


    int iBtnWidth = (m_rcGlobe.left + INDENT - m_rcShadow.left)/2;

    m_rcAreaCodeDisplay.left    = m_rcShadow.left + 2*iIndent;
    m_rcAreaCodeDisplay.top     = m_rcShadow.top + 3*INDENT;
    m_rcAreaCodeDisplay.right   = m_rcGlobe.left - 2*iIndent;
    m_rcAreaCodeDisplay.bottom  = m_rcAreaCodeDisplay.top + GetSystemMetrics(SM_CXSMICON)*17/10;

    m_rcClearBtn.left       = m_rcShadow.left + INDENT;
    m_rcClearBtn.top        = m_rcAreaCodeDisplay.bottom + iIndent;
    m_rcClearBtn.bottom     = m_rcShadow.bottom - INDENT;
    m_rcClearBtn.right      = m_rcClearBtn.left + iBtnWidth;

    m_rcLocateBtn           = m_rcClearBtn;
    m_rcLocateBtn.left      = m_rcClearBtn.right+ INDENT;
    m_rcLocateBtn.right     = m_rcLocateBtn.left + iBtnWidth;

    m_rcSearchBar           = m_rcShadow;
    m_rcSearchBar.top       = m_rcShadow.bottom + INDENT;
    m_rcSearchBar.bottom    = m_rcSearchBar.top + GetSystemMetrics(SM_CXSMICON)*3/2;

    m_rcSearchBy            = m_rcSearchBar;
    m_rcSearchBy.left       += iIndent;

    m_rcSearchType          = m_rcSearchBar;
    m_rcSearchType.right    -= iIndent;

    //text for info screen
    int iTextSpacing        = GetSystemMetrics(SM_CXSMICON);

    m_rcCountryCode.left    = m_rcShadow.left + iIndent;
    m_rcCountryCode.top     = m_rcShadow.top  + iIndent;
    m_rcCountryCode.right   = m_rcShadow.right - iIndent;
    m_rcCountryCode.bottom  = m_rcCountryCode.top + iTextSpacing;

    m_rcCountry             = m_rcCountryCode;
    m_rcCountry.top         = m_rcCountryCode.bottom;
    m_rcCountry.bottom      = m_rcCountry.top + iTextSpacing;

    m_rcRegion              = m_rcCountry;
    m_rcRegion.top          = m_rcCountry.bottom;
    m_rcRegion.bottom       = m_rcRegion.top + iTextSpacing;

    m_rcLocalTime           = m_rcRegion;
    m_rcLocalTime.top       = m_rcRegion.bottom + iTextSpacing;
    m_rcLocalTime.bottom    = m_rcLocalTime.top + iTextSpacing;

    m_rcLanguage            = m_rcLocalTime;
    m_rcLanguage.top        = m_rcLocalTime.bottom;
    m_rcLanguage.bottom     = m_rcLanguage.top + iTextSpacing;

    m_rcCapital             = m_rcLanguage;
    m_rcCapital.top         = m_rcLanguage.bottom;
    m_rcCapital.bottom      = m_rcCapital.top + iTextSpacing;

    m_rcCurrency             = m_rcCapital;
    m_rcCurrency.top         = m_rcCapital.bottom;
    m_rcCurrency.bottom      = m_rcCurrency.top + iTextSpacing;

    m_rcPopulation           = m_rcCurrency;
    m_rcPopulation.top       = m_rcCurrency.bottom;
    m_rcPopulation.bottom    = m_rcPopulation.top + iTextSpacing;

    m_rcCodes                = m_rcPopulation;
    m_rcCodes.top            = m_rcPopulation.bottom;
    m_rcCodes.bottom         = m_rcCodes.top + iTextSpacing;

 
    //set some btn heights
    int iBtnHeight = GetSystemMetrics(SM_CXICON)*5/4;
    //if(iBtnHeight > (rcClient.bottom - m_rcSearchBar.bottom - 3*INDENT)/2)
    if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
    {
        //set some btn widths
        iBtnWidth = (WIDTH(rcClient))/10;

        m_rcNumBtns[0].left     = rcClient.left;
        m_rcNumBtns[0].right    = m_rcNumBtns[0].left + iBtnWidth;
        m_rcNumBtns[1].left     = m_rcNumBtns[0].right + 0;
        m_rcNumBtns[1].right    = m_rcNumBtns[1].left + iBtnWidth;
        m_rcNumBtns[2].left     = m_rcNumBtns[1].right + 0;
        m_rcNumBtns[2].right    = m_rcNumBtns[2].left + iBtnWidth;
        m_rcNumBtns[3].left     = m_rcNumBtns[2].right + 0;
        m_rcNumBtns[3].right    = m_rcNumBtns[3].left + iBtnWidth;
        m_rcNumBtns[4].left     = m_rcNumBtns[3].right + 0;
        m_rcNumBtns[4].right    = m_rcNumBtns[4].left + iBtnWidth;

        m_rcNumBtns[5].left     = m_rcNumBtns[4].right + 0;
        m_rcNumBtns[5].right    = m_rcNumBtns[5].left + iBtnWidth;
        m_rcNumBtns[6].left     = m_rcNumBtns[5].right + 0;
        m_rcNumBtns[6].right    = m_rcNumBtns[6].left + iBtnWidth;
        m_rcNumBtns[7].left     = m_rcNumBtns[6].right + 0;
        m_rcNumBtns[7].right    = m_rcNumBtns[7].left + iBtnWidth;
        m_rcNumBtns[8].left     = m_rcNumBtns[7].right + 0;
        m_rcNumBtns[8].right    = m_rcNumBtns[8].left + iBtnWidth;
        m_rcNumBtns[9].left     = m_rcNumBtns[8].right + 0;
        m_rcNumBtns[9].right    = m_rcNumBtns[9].left + iBtnWidth;

        for(int i = 0; i < 10; i++)
        {
            m_rcNumBtns[i].top = m_rcSearchBar.bottom + INDENT;
            m_rcNumBtns[i].bottom = m_rcNumBtns[i].top + iBtnHeight;
        }

  /*      iBtnHeight = (rcClient.bottom - m_rcSearchBar.bottom - 3*INDENT)/2;

        m_rcNumBtns[0].top = m_rcSearchBar.bottom + INDENT;
        m_rcNumBtns[1].top = m_rcSearchBar.bottom + INDENT;
        m_rcNumBtns[2].top = m_rcSearchBar.bottom + INDENT;
        m_rcNumBtns[3].top = m_rcSearchBar.bottom + INDENT;
        m_rcNumBtns[4].top = m_rcSearchBar.bottom + INDENT;

        m_rcNumBtns[0].bottom = m_rcNumBtns[0].top + iBtnHeight;
        m_rcNumBtns[1].bottom = m_rcNumBtns[1].top + iBtnHeight;
        m_rcNumBtns[2].bottom = m_rcNumBtns[2].top + iBtnHeight;
        m_rcNumBtns[3].bottom = m_rcNumBtns[3].top + iBtnHeight;
        m_rcNumBtns[4].bottom = m_rcNumBtns[4].top + iBtnHeight;

        m_rcNumBtns[5].top = m_rcNumBtns[0].bottom + INDENT;
        m_rcNumBtns[6].top = m_rcNumBtns[1].bottom + INDENT;
        m_rcNumBtns[7].top = m_rcNumBtns[2].bottom + INDENT;
        m_rcNumBtns[8].top = m_rcNumBtns[3].bottom + INDENT;
        m_rcNumBtns[9].top = m_rcNumBtns[4].bottom + INDENT;

        m_rcNumBtns[5].bottom = m_rcNumBtns[5].top + iBtnHeight;
        m_rcNumBtns[6].bottom = m_rcNumBtns[6].top + iBtnHeight;
        m_rcNumBtns[7].bottom = m_rcNumBtns[7].top + iBtnHeight;
        m_rcNumBtns[8].bottom = m_rcNumBtns[8].top + iBtnHeight;
        m_rcNumBtns[9].bottom = m_rcNumBtns[9].top + iBtnHeight;*/
    }
    else
    {
         if(iBtnHeight > (rcClient.bottom - m_rcSearchBar.bottom - 3*INDENT - HEIGHT(m_rcBottomBar))/2)
            iBtnHeight = (rcClient.bottom - m_rcSearchBar.bottom - 3*INDENT - HEIGHT(m_rcBottomBar))/2;

        //set some btn widths
        iBtnWidth = (WIDTH(rcClient))/5;

        m_rcNumBtns[0].left     = rcClient.left;
        m_rcNumBtns[0].right    = m_rcNumBtns[0].left + iBtnWidth;
        m_rcNumBtns[1].left     = m_rcNumBtns[0].right + 0;
        m_rcNumBtns[1].right    = m_rcNumBtns[1].left + iBtnWidth;
        m_rcNumBtns[2].left     = m_rcNumBtns[1].right + 0;
        m_rcNumBtns[2].right    = m_rcNumBtns[2].left + iBtnWidth;
        m_rcNumBtns[3].left     = m_rcNumBtns[2].right + 0;
        m_rcNumBtns[3].right    = m_rcNumBtns[3].left + iBtnWidth;
        m_rcNumBtns[4].left     = m_rcNumBtns[3].right + 0;
        m_rcNumBtns[4].right    = m_rcNumBtns[4].left + iBtnWidth;

        m_rcNumBtns[5].left     = rcClient.left;
        m_rcNumBtns[5].right    = m_rcNumBtns[5].left + iBtnWidth;
        m_rcNumBtns[6].left     = m_rcNumBtns[5].right + 0;
        m_rcNumBtns[6].right    = m_rcNumBtns[6].left + iBtnWidth;
        m_rcNumBtns[7].left     = m_rcNumBtns[6].right + 0;
        m_rcNumBtns[7].right    = m_rcNumBtns[7].left + iBtnWidth;
        m_rcNumBtns[8].left     = m_rcNumBtns[7].right + 0;
        m_rcNumBtns[8].right    = m_rcNumBtns[8].left + iBtnWidth;
        m_rcNumBtns[9].left     = m_rcNumBtns[8].right + 0;
        m_rcNumBtns[9].right    = m_rcNumBtns[9].left + iBtnWidth;

        int iBtnSpacing = (rcClient.bottom - m_rcSearchBar.bottom - INDENT - 2*iBtnHeight - GetSystemMetrics(SM_CXICON))/2;

        m_rcNumBtns[0].top = m_rcSearchBar.bottom + iBtnSpacing;
        m_rcNumBtns[1].top = m_rcSearchBar.bottom + iBtnSpacing;
        m_rcNumBtns[2].top = m_rcSearchBar.bottom + iBtnSpacing;
        m_rcNumBtns[3].top = m_rcSearchBar.bottom + iBtnSpacing;
        m_rcNumBtns[4].top = m_rcSearchBar.bottom + iBtnSpacing;

        m_rcNumBtns[0].bottom = m_rcNumBtns[0].top + iBtnHeight;
        m_rcNumBtns[1].bottom = m_rcNumBtns[1].top + iBtnHeight;
        m_rcNumBtns[2].bottom = m_rcNumBtns[2].top + iBtnHeight;
        m_rcNumBtns[3].bottom = m_rcNumBtns[3].top + iBtnHeight;
        m_rcNumBtns[4].bottom = m_rcNumBtns[4].top + iBtnHeight;

        m_rcNumBtns[5].top = m_rcNumBtns[0].bottom + INDENT;
        m_rcNumBtns[6].top = m_rcNumBtns[1].bottom + INDENT;
        m_rcNumBtns[7].top = m_rcNumBtns[2].bottom + INDENT;
        m_rcNumBtns[8].top = m_rcNumBtns[3].bottom + INDENT;
        m_rcNumBtns[9].top = m_rcNumBtns[4].bottom + INDENT;

        m_rcNumBtns[5].bottom = m_rcNumBtns[5].top + iBtnHeight;
        m_rcNumBtns[6].bottom = m_rcNumBtns[6].top + iBtnHeight;
        m_rcNumBtns[7].bottom = m_rcNumBtns[7].top + iBtnHeight;
        m_rcNumBtns[8].bottom = m_rcNumBtns[8].top + iBtnHeight;
        m_rcNumBtns[9].bottom = m_rcNumBtns[9].top + iBtnHeight;
    }
   
    m_rcMenuLeft = rcClient;
    m_rcMenuLeft.top = m_rcMenuLeft.bottom - BAR_BOTTOM_Height;
    m_rcMenuLeft.right -= WIDTH(rcClient)/2;

    m_rcMenuRight = rcClient;
    m_rcMenuRight.top = m_rcMenuRight.bottom - BAR_BOTTOM_Height;
    m_rcMenuRight.left += WIDTH(rcClient)/2;

    //time to set a whole fuck load of image slices
    m_imgBtnNum.SetSize(WIDTH(m_rcNumBtns[0]), HEIGHT(m_rcNumBtns[0]));
    m_imgBtn.SetSize(WIDTH(m_rcClearBtn), HEIGHT(m_rcClearBtn));
    m_imgBtnNumDown.SetSize(WIDTH(m_rcNumBtns[0]), HEIGHT(m_rcNumBtns[0]));
    m_imgBtnDown.SetSize(WIDTH(m_rcClearBtn), HEIGHT(m_rcClearBtn));

    m_imgShadow.SetSize(WIDTH(m_rcShadow), HEIGHT(m_rcShadow));
    m_imgShadowLong.SetSize(WIDTH(m_rcShadowLong), HEIGHT(m_rcShadowLong));
    m_imgGreenBar.SetSize(WIDTH(m_rcTopGreenBar), HEIGHT(m_rcTopGreenBar));

    //it'll re-create itself as needed
    m_oDisplay.Destroy();

	return TRUE;
}

BOOL CDlgAreaCode::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	/*switch(uiMessage)
	{
	default:
		break;
	}*/

	return UNHANDLED;
}

BOOL CDlgAreaCode::OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    DrawBackgroundElements(gdi, rcClip);

    DrawGlobe(gdi, rcClip);
    DrawFlag(gdi, rcClip);
    DrawText(gdi, rcClip);//just in case we clip the globe
    

    return TRUE;
}

BOOL CDlgAreaCode::DrawFlag(CIssGDIEx& gdi, RECT& rcClip)
{
    if(m_eState != AS_Display)
        return FALSE;
    if(m_gdiFlag.GetDC() == NULL)
        return FALSE;
    if(m_oArea.GetLookupType() != LT_CountryCodes)
        return FALSE;

    POINT ptLoc;

    ptLoc.x = m_rcCountry.left;
    ptLoc.y = m_rcCountry.bottom + INDENT;

    //portrait
/*    if(GetSystemMetrics(SM_CXSCREEN) < GetSystemMetrics(SM_CYSCREEN))
    {   //center at the bottom
        ptLoc.x = (GetSystemMetrics(SM_CXSCREEN) - m_gdiFlag.GetWidth())/2;
        ptLoc.y = m_rcCodes.bottom + GetSystemMetrics(SM_CXSMICON);
    }
    //landscape
    else if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
    {   
        ptLoc.x = m_rcGlobe.left + (m_gdiGlobe.GetHeight() - m_gdiFlag.GetWidth())/2;
        ptLoc.y = m_rcGlobe.bottom +2*GetSystemMetrics(SM_CXICON);
    }
    //square
    else
    {
        ptLoc.x = m_rcGlobe.left + (m_gdiGlobe.GetHeight() - m_gdiFlag.GetWidth())/2;
        ptLoc.y = m_rcGlobe.bottom + GetSystemMetrics(SM_CXSMICON);
    }*/

    //lets draw a flag

    ::Draw(gdi,
        ptLoc.x,ptLoc.y,
        m_gdiFlag.GetWidth(), m_gdiFlag.GetHeight(),
        m_gdiFlag,
        0, 0,
        ALPHA_Copy);

    return TRUE;
}

void CDlgAreaCode::DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip)
{
    RECT rcTemp;
    TCHAR szTemp[STRING_NORMAL];

    //draw some text
    if(m_wndMenu.IsWindowUp(FALSE))
    {
        rcTemp = m_rcMenuRight;
        rcTemp.left += 2;
        rcTemp.top += 2;
        m_oStr->StringCopy(szTemp, _T("Cancel"));
        ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_CENTER | DT_VCENTER, m_hFontText, 0);
        ::DrawText(gdi.GetDC(), szTemp, m_rcMenuRight, DT_CENTER | DT_VCENTER, m_hFontText, 0xFFFFFF);
    }
    else if(m_eState == AS_Search)
    {
        if(IsRectInRect(rcClip, m_rcMenuLeft))
        {     
            rcTemp = m_rcMenuLeft;
            rcTemp.left += 2;
            rcTemp.top += 2;
            m_oStr->StringCopy(szTemp, _T("Menu"));
            ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_CENTER | DT_VCENTER, m_hFontText, 0);
            ::DrawText(gdi.GetDC(), szTemp, m_rcMenuLeft, DT_CENTER | DT_VCENTER, m_hFontText, 0xFFFFFF);

        }

        if(IsRectInRect(rcClip, m_rcMenuRight))
        {     
            rcTemp = m_rcMenuRight;
            rcTemp.left += 2;
            rcTemp.top += 2;
            m_oStr->StringCopy(szTemp, _T("List"));
            ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_CENTER | DT_VCENTER, m_hFontText, 0);
            ::DrawText(gdi.GetDC(), szTemp, m_rcMenuRight, DT_CENTER | DT_VCENTER, m_hFontText, 0xFFFFFF);

        }
    }
    else if(m_eState == AS_Display)
    {
        if(IsRectInRect(rcClip, m_rcMenuRight))
        {     
            rcTemp = m_rcMenuRight;
            rcTemp.left += 2;
            rcTemp.top += 2;
            m_oStr->StringCopy(szTemp, _T("Back"));
            ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_CENTER | DT_VCENTER, m_hFontText, 0);
            ::DrawText(gdi.GetDC(), szTemp, m_rcMenuRight, DT_CENTER | DT_VCENTER, m_hFontText, 0xFFFFFF);
        }
    }
    else
    {
        if(IsRectInRect(rcClip, m_rcMenuRight))
        {     
            rcTemp = m_rcMenuRight;
            rcTemp.left += 2;
            rcTemp.top += 2;
            m_oStr->StringCopy(szTemp, _T("Cancel"));
            ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_CENTER | DT_VCENTER, m_hFontText, 0);
            ::DrawText(gdi.GetDC(), szTemp, m_rcMenuRight, DT_CENTER | DT_VCENTER, m_hFontText, 0xFFFFFF);
        }
    }
    CDlgBase::DrawBottomBar(gdi, rcClient, rcClip);
}

BOOL CDlgAreaCode::DrawBackgroundElements(CIssGDIEx& gdi, RECT& rcClip)
{
    //its go time
    if(IsRectInRect(rcClip, m_rcTopGreenBar))
        m_imgGreenBar.DrawImage(gdi, m_rcTopGreenBar.left, m_rcTopGreenBar.top);

    switch(m_eState)
    {
    case AS_Search:
        DrawBackgroundSearch(gdi, rcClip);
    	break;
    case AS_About:
        DrawBackgroundSearch(gdi, rcClip);
        DrawBackgroundAbout(gdi, rcClip);
    	break;
    case AS_Display:
        DrawBackgroundDisplay(gdi, rcClip);
        break;
    default:
        return FALSE;
        break;
    }

    return TRUE;
}

BOOL CDlgAreaCode::DrawBackgroundAbout(CIssGDIEx& gdi, RECT& rcClip)
{
    //alpha blend baby
    RECT rcTemp = rcClip;
    if(rcClip.bottom > m_rcMenuLeft.top)
        rcTemp.bottom = m_rcMenuLeft.top;
    if(rcTemp.top >= rcTemp.bottom)
        return TRUE;//nothing to do

    //haxxed
    ChangeState(AS_Search);
    DrawGlobe(gdi, rcClip);
    DrawText(gdi, rcClip);
    ChangeState(AS_About);
    
    AlphaFillRect(gdi, rcTemp, RGB(0,0,0), 164);    
    return TRUE;
}

BOOL CDlgAreaCode::DrawBackgroundDisplay(CIssGDIEx& gdi, RECT& rcClip)
{
    if(IsRectInRect(rcClip, m_rcShadowLong))
        m_imgShadowLong.DrawImage(gdi, m_rcShadowLong.left, m_rcShadowLong.top);


    return TRUE;
}

BOOL CDlgAreaCode::DrawBackgroundSearch(CIssGDIEx& gdi, RECT& rcClip)
{
    if(IsRectInRect(rcClip, m_rcShadow))
        m_imgShadow.DrawImage(gdi, m_rcShadow.left, m_rcShadow.top);

    if(IsRectInRect(rcClip, m_rcSearchBar))
        ::Rectangle(gdi.GetDC(), m_rcSearchBar, RGB(107, 105, 108), RGB(52,52,52));

    if(IsRectInRect(rcClip, m_rcAreaCodeDisplay))
    {
        if(m_oDisplay.GetDisplayDC() == NULL)
        {
            m_oDisplay.Init(m_rcAreaCodeDisplay, 
                RGB(151,151,151),
                RGB(210,210,210),
                RGB(151,151,151),
                RGB(51,51,51),
                DISPLAY_Grad_DS);
        }
        m_oDisplay.Draw(gdi.GetDC());            
    }

    if(IsRectInRect(rcClip, m_rcClearBtn))
    {
        if(m_bClearBtn == TRUE)         
            m_imgBtnDown.DrawImage(gdi, m_rcClearBtn.left, m_rcClearBtn.top);
        else
            m_imgBtn.DrawImage(gdi, m_rcClearBtn.left, m_rcClearBtn.top);

    }

    if(IsRectInRect(rcClip, m_rcLocateBtn))
    {
        if(m_bLocateBtn == TRUE)
            m_imgBtnDown.DrawImage(gdi, m_rcLocateBtn.left, m_rcLocateBtn.top);
        else
            m_imgBtn.DrawImage(gdi, m_rcLocateBtn.left, m_rcLocateBtn.top);

    }

    for(int i = 0; i < 10; i++)
    {
        if(IsRectInRect(rcClip, m_rcNumBtns[i]))
        {
            if(m_bNumBtns[i] == TRUE)
                m_imgBtnNumDown.DrawImage(gdi, m_rcNumBtns[i].left, m_rcNumBtns[i].top);
            else    
                m_imgBtnNum.DrawImage(gdi, m_rcNumBtns[i].left, m_rcNumBtns[i].top);
        }
    }
    return TRUE;
}

BOOL CDlgAreaCode::DrawText(CIssGDIEx& gdi, RECT& rcClip)
{
    static RECT rcTemp;
    static TCHAR szTemp[2*STRING_MAX];

    if(m_eState == AS_About)
    {
        RECT rc;
        GetClientRect(m_hWnd, &rc);
        rc.top += GetSystemMetrics(SM_CXICON)*3/2;

        m_oStr->Format(szTemp, _T("Phone Genius\r\n%s\r\n\r\nArea Code Genius\r\n%s\r\n\r\nCopyright 2009\r\nPanoramic Software Inc\r\nAll Rights Reserved\r\nwww.panoramicsoft.com\r\n"), VERSION_PhoneGenius, VERSION_AreaCodeGenius);
        ::DrawText(gdi.GetDC(), szTemp, rc, DT_CENTER | DT_TOP, m_hFontText, 0);
        rc.top--;
        rc.left -= 2;
        ::DrawText(gdi.GetDC(), szTemp, rc, DT_CENTER | DT_TOP, m_hFontText, 0xFFFFFF);
    }
    else if(m_eState == AS_Search)
    {
        if(IsRectInRect(rcClip, m_rcTopText))
        {
            m_oStr->StringCopy(szTemp, _T("Search"));
            rcTemp = m_rcTopText;
            rcTemp.left += 1;
            rcTemp.top += 2;
            ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER, m_hFontSmText, 0);
            ::DrawText(gdi.GetDC(), szTemp, m_rcTopText, DT_LEFT | DT_VCENTER, m_hFontSmText, 0xFFFFFF);
        }

        m_oStr->StringCopy(szTemp, _T("Clear"));
        if(IsRectInRect(rcClip, m_rcClearBtn))
            ::DrawText(gdi.GetDC(), szTemp, m_rcClearBtn, DT_CENTER | DT_VCENTER, m_hFontText, 0xFFFFFF);

        m_oStr->StringCopy(szTemp, _T("Locate"));
        if(IsRectInRect(rcClip, m_rcLocateBtn))
            ::DrawText(gdi.GetDC(), szTemp, m_rcLocateBtn, DT_CENTER | DT_VCENTER, m_hFontText, 0xFFFFFF);

        if(IsRectInRect(rcClip, m_rcAreaCodeDisplay))
            ::DrawText(gdi.GetDC(), m_szAreaCode, m_rcAreaCodeDisplay, DT_CENTER | DT_VCENTER, m_hFontAreaCode, 0);

        if(IsRectInRect(rcClip, m_rcSearchBy))
        {
            m_oStr->StringCopy(szTemp, _T("Search By:"));
            rcTemp = m_rcSearchBy;
            rcTemp.left += 1;
            rcTemp.top += 2;
            ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER, m_hFontSmText, 0);
            ::DrawText(gdi.GetDC(), szTemp, m_rcSearchBy, DT_LEFT | DT_VCENTER, m_hFontSmText, 0xFFFFFF);
        }

        if(IsRectInRect(rcClip, m_rcSearchType))
        {
            //we'll draw the down arrow here too
            rcTemp = m_rcSearchBar;
            rcTemp.top = rcTemp.bottom - m_gdiDownArrow.GetHeight();
            rcTemp.left = rcTemp.right - m_gdiDownArrow.GetWidth();

            ::Draw(gdi, rcTemp, m_gdiDownArrow);

            rcTemp = m_rcSearchType;
            rcTemp.right = rcTemp.right - m_gdiDownArrow.GetWidth();
            

            switch(m_eLookupType)
            {
            case LT_CountryCodes:
                m_oStr->StringCopy(szTemp, _T("World"));
            	break;
            case LT_AreaCodes:
                m_oStr->StringCopy(szTemp, _T("North America"));
                break;
            case LT_NewZealand:
                m_oStr->StringCopy(szTemp, _T("New Zealand"));
                break;
            case LT_Australia:
                m_oStr->StringCopy(szTemp, _T("Australia"));
                break;
            case LT_UnitedKingdom:
                m_oStr->StringCopy(szTemp, _T("United Kingdom"));
                break;
            default:
                break;
            }

            ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_RIGHT | DT_VCENTER, m_hFontSmText, 0x222222);
        }
        

        for(int i = 0; i < 10; i++)
        {
            if(IsRectInRect(rcClip, m_rcNumBtns[i]))
            {     
                rcTemp = m_rcNumBtns[i];
                rcTemp.left += 2;
                rcTemp.top += 2;
                if(i == 9)
                    m_oStr->IntToString(szTemp, 0);
                else
                    m_oStr->IntToString(szTemp, i+1);
                ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_CENTER | DT_VCENTER, m_hFontButton, 0);
                ::DrawText(gdi.GetDC(), szTemp, m_rcNumBtns[i], DT_CENTER | DT_VCENTER, m_hFontButton, 0xFFFFFF);
            }
        }
    }
    else
    {
        if(IsRectInRect(rcClip, m_rcTopText))
        {
            m_oStr->StringCopy(szTemp, _T("Location"));
            rcTemp = m_rcTopText;
            rcTemp.left += 1;
            rcTemp.top += 2;
            ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER | DT_NOPREFIX, m_hFontSmText, 0);
            ::DrawText(gdi.GetDC(), szTemp, m_rcTopText, DT_LEFT | DT_VCENTER, m_hFontSmText, 0xFFFFFF);
        }

        AreaCodeType* area = m_oArea.GetCodeInfo();

        if(area == NULL)
            return FALSE;

        if(IsRectInRect(rcClip, m_rcCountryCode))
        {
			if(m_eLookupType == LT_UnitedKingdom)
				m_oStr->Format(szTemp, _T("Area Code: 0%i"), area->iAreaCode);
            else if(m_eLookupType != LT_CountryCodes)
                m_oStr->Format(szTemp, _T("Area Code: %i"), area->iAreaCode);
            else
                m_oStr->Format(szTemp, _T("Country Code: %i"), area->iCountryCode);

            ::DrawText(gdi.GetDC(), szTemp, m_rcCountryCode, DT_LEFT | DT_VCENTER, m_hFontSmText, 0xFFFFFF);
        }

        if(IsRectInRect(rcClip, m_rcCountry))
        {
            if(area->szCountry)
                ::DrawText(gdi.GetDC(), area->szCountry, m_rcCountry, DT_LEFT | DT_VCENTER | DT_NOPREFIX, m_hFontSmText, 0xFFFFFF);
        }

        if(IsRectInRect(rcClip, m_rcRegion))
        {
            if(m_eLookupType != LT_CountryCodes && area->szRegion && m_oStr->Compare(area->szCountry, area->szRegion) != 0)            
                ::DrawText(gdi.GetDC(), area->szRegion, m_rcRegion, DT_LEFT | DT_VCENTER | DT_NOPREFIX, m_hFontSmallerText, 0xFFFFFF);
        }

        if(IsRectInRect(rcClip, m_rcLocalTime))
        {
            SYSTEMTIME sTime = m_oArea.GetLocalTime();
            TCHAR szMore[STRING_SMALL];
            if(sTime.wHour > 12)
            {
                sTime.wHour -= 12;
                m_oStr->StringCopy(szMore, _T("pm"));
            }
            else
            {
                m_oStr->StringCopy(szMore, _T("am"));
            }
            
            m_oStr->Format(szTemp, _T("Local Time: %i:%.2i%s"), sTime.wHour, sTime.wMinute, szMore);
            ::DrawText(gdi.GetDC(), szTemp, m_rcLocalTime, DT_LEFT | DT_VCENTER, m_hFontSmallerText, 0xFFFFFF);
        }

        if(IsRectInRect(rcClip, m_rcLanguage))
        {
            if(area->szLanguages)
            
            {
                m_oStr->Format(szTemp, _T("Language: %s"), area->szLanguages);
                ::DrawText(gdi.GetDC(), szTemp, m_rcLanguage, DT_LEFT | DT_VCENTER, m_hFontSmallerText, 0xFFFFFF);
            }
        }

        if(IsRectInRect(rcClip, m_rcCapital))
        {
            if(area->szCapital)
            {         
                m_oStr->Format(szTemp, _T("Capital: %s"), area->szCapital);
                ::DrawText(gdi.GetDC(), szTemp, m_rcCapital, DT_LEFT | DT_VCENTER, m_hFontSmallerText, 0xFFFFFF);
            }
        }

        if(IsRectInRect(rcClip, m_rcCurrency))
        {
            if(area->szCurrency)
            {
                m_oStr->Format(szTemp, _T("Currency: %s"), area->szCurrency);
                ::DrawText(gdi.GetDC(), szTemp, m_rcCurrency, DT_LEFT | DT_VCENTER, m_hFontSmallerText, 0xFFFFFF);
            }
        }

        if(IsRectInRect(rcClip, m_rcPopulation))
        {
            if(area->iPopulation)
            {
                TCHAR szPop[STRING_NORMAL];
                m_oStr->IntToString(szPop, area->iPopulation);
                int Len = m_oStr->GetLength(szPop);

                for(int i = Len - 3; i > 0; i -= 3)
                {
                    m_oStr->Insert(szPop, _T(","), i);
                }

                m_oStr->Format(szTemp, _T("Population: %s (%i)"), szPop, area->iDataYear);
                ::DrawText(gdi.GetDC(), szTemp, m_rcPopulation, DT_LEFT | DT_VCENTER, m_hFontSmallerText, 0xFFFFFF);
            }
        }

        if(IsRectInRect(rcClip, m_rcCodes))
        {
            m_oStr->Empty(szTemp);
            if(area->szExitCode && area->szLocalCode)
            {
                m_oStr->Format(szTemp, _T("Exit Code: %s  Trunk Code: %s"), area->szExitCode, area->szLocalCode);
            }
            else if(area->szExitCode)
            {
                m_oStr->Format(szTemp, _T("Exit Code: %s"), area->szExitCode);
            }
            ::DrawText(gdi.GetDC(), szTemp, m_rcCodes, DT_LEFT | DT_VCENTER, m_hFontSmallerText, 0xFFFFFF);
        }
    }
    return TRUE;
}

BOOL CDlgAreaCode::DrawGlobe(CIssGDIEx& gdi, RECT& rcClip)
{
    if(m_eState == AS_About)
        return FALSE;

    if(IsRectInRect(rcClip, m_rcGlobe) == FALSE)
        return FALSE;

    Draw(gdi,
        m_rcGlobe.left,m_rcGlobe.top,
        m_gdiGlobe.GetHeight(), m_gdiGlobe.GetHeight(),
        m_gdiGlobe,
        m_gdiGlobe.GetHeight() *m_iGlobeFrame,0);

//#define FLAG_SIZE  (GetSystemMetrics(SM_CXICON)/4)
#define FLAG_SIZE  (IsVGA()?11:7)

    AreaCodeType* sArea = m_oArea.GetCodeInfo();

    if(m_eState == AS_Display && sArea && (sArea->iFrame == m_iGlobeFrame))
    {
       if(sArea->iX < 1 || sArea->iY < 1)
           return TRUE; //bad co-ordinate
        //draw the point
        int iX = sArea->iX*m_gdiGlobe.GetHeight()/100;
        int iY = sArea->iY*m_gdiGlobe.GetHeight()/100;

        RECT rcLoc;
        SetRect(&rcLoc, iX - FLAG_SIZE/2 + m_rcGlobe.left, iY - FLAG_SIZE/2 + m_rcGlobe.top, iX + FLAG_SIZE/2 + m_rcGlobe.left, iY + FLAG_SIZE/2 + m_rcGlobe.top);

        //    SetPixel(gdi->GetDC(), iX + m_rcGlobe.left, iY+m_rcGlobe.top, 0x00FFFF);//for now
        RoundRectangle(gdi.GetDC(), rcLoc, 0x00FFFF, 0, FLAG_SIZE, FLAG_SIZE);
    }

    return TRUE;
}




HRESULT CDlgAreaCode::LoadImages(HWND hWnd, HINSTANCE hInst)
{
    HRESULT hr = S_OK;

    if(!m_imgShadow.IsLoaded())
        hr = m_imgShadow.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_BKVGA:IDR_PNG_CALL_BK);

    if(!m_imgShadowLong.IsLoaded())
        hr = m_imgShadowLong.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_BKVGA:IDR_PNG_CALL_BK);

    if(!m_imgBtn.IsLoaded())
        hr = m_imgBtn.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ButtonVGA:IDR_PNG_CALL_Button);

    if(!m_imgBtnNum.IsLoaded())
        hr = m_imgBtnNum.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ButtonVGA:IDR_PNG_CALL_Button);

    if(!m_imgBtnDown.IsLoaded())
        hr = m_imgBtnDown.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ButtonDownVGA:IDR_PNG_CALL_ButtonDown);

    if(!m_imgBtnNumDown.IsLoaded())
        hr = m_imgBtnNumDown.Initialize(hWnd, hInst, IsVGA()?IDR_PNG_CALL_ButtonDownVGA:IDR_PNG_CALL_ButtonDown);

    if(!m_imgGreenBar.IsLoaded())
    {
        CIssGDIEx gdiTemp;
        gdiTemp.LoadImage(FindSkin(IDR_PNG_MenuArray), hWnd, hInst);

        CIssGDIEx gdiTempSm;
        gdiTempSm.Create(gdiTemp.GetDC(), gdiTemp.GetHeight(), gdiTemp.GetHeight(), FALSE, TRUE);
        gdiTempSm.InitAlpha(TRUE);

        m_gdiDownArrow.Create(gdiTemp.GetDC(), gdiTemp.GetHeight(), gdiTemp.GetHeight(), FALSE, TRUE);
        m_gdiDownArrow.InitAlpha(TRUE);


        ::Draw(gdiTempSm,
            0,0,
            gdiTemp.GetHeight(), gdiTemp.GetHeight(),
            gdiTemp,
            7*gdiTemp.GetHeight(), 0,
            ALPHA_Copy);

        //down arrow
        ::Draw(m_gdiDownArrow,
            0,0,
            gdiTemp.GetHeight(), gdiTemp.GetHeight(),
            gdiTemp,
            2*gdiTemp.GetHeight(), 0,
            ALPHA_Copy);

        m_imgGreenBar.CutImage(gdiTempSm);
    }

//    if(m_gdiGlobe.GetDC() == NULL)
//        m_gdiGlobe.LoadImage(IsVGA()?IDR_PNG_CALL_GlobeVGA:IDR_PNG_CALL_Globe, hWnd, hInst);

    if(m_gdiGlobe.GetDC() == NULL)
    {
        if(IsVGA() == FALSE)
        {
            m_gdiGlobe.LoadImage(IDR_PNG_CALL_Globe, hWnd, hInst);
        }
        else
        {
            CIssGDIEx gdiTemp;

            gdiTemp.LoadImage(IDR_PNG_CALL_Globe, hWnd, hInst);

            SIZE sz;

            sz.cy = GLOBE_HEIGHT;
            sz.cx = sz.cy*NUMBER_OF_GLOBE_FRAMES;

            ScaleImageFast(gdiTemp, m_gdiGlobe, sz, FALSE, 0);
        }
    }

    //menu bitches
    m_wndMenu.SetColors(COLOR_TEXT_NORMAL, COLOR_TEXT_NORMAL, COLOR_TEXT_SELECTED);
    m_wndMenu.SetBackground(IDR_PNG_Group);
    m_wndMenu.SetImageArray(SKIN(IDR_PNG_MenuArray));
    m_wndMenu.SetSelected(SKIN(IDR_PNG_Selector));
    m_wndMenu.PreloadImages(hWnd, hInst);


    m_oArea.Init(hInst);
    m_oArea.LoadAreaCodes();

    DebugOut(_T("Images Loaded"));
    
    return hr;
}

BOOL CDlgAreaCode::OnNumButton(int iNum)
{
    if(m_eState != AS_Search)
        return FALSE;
    if(iNum < 0 || iNum > 9)
        return FALSE;
    if(m_oStr->GetLength(m_szAreaCode) > 5)
        m_oStr->Empty(m_szAreaCode);
    else if(m_oStr->GetLength(m_szAreaCode) > 5)
        return FALSE;
 //   if(m_szAreaCode[0] != _T('1') && m_oStr->GetLength(m_szAreaCode) > 2)
 //       return FALSE;

    TCHAR szTemp[2];
    m_oStr->IntToString(szTemp, iNum);

    m_oStr->Concatenate(m_szAreaCode, szTemp);    

    InvalidateRect(m_hWnd, &m_rcAreaCodeDisplay, FALSE);
    return TRUE;
};

BOOL CDlgAreaCode::OnClear()
{
    if(m_eState != AS_Search)
        return FALSE;

    m_oStr->Empty(m_szAreaCode);

    InvalidateRect(m_hWnd, &m_rcAreaCodeDisplay, FALSE);
    return TRUE;
}

BOOL CDlgAreaCode::OnLocate(BOOL bLookup /* = TRUE */)
{
    if(m_eState != AS_Search)
        return FALSE;

    if(bLookup)
    {
        //check if the area code is valid
        if(m_oStr->GetLength(m_szAreaCode) < 1)
            return FALSE;

        //we'll always try twice
        EnumLookupType eAlt = LT_CountryCodes;
        if(m_eLookupType == LT_CountryCodes)
            eAlt = LT_AreaCodes;

        int iArea = m_oStr->StringToInt(m_szAreaCode);

        if(m_oArea.SetAreaCode(iArea, m_eLookupType) == FALSE)
        {   //the user entered a bad code/type combination ... lets try a diff type (usually world)
            if(m_oArea.SetAreaCode(iArea, eAlt) == FALSE)
            {   //ok .. that didn't work at all
                m_oStr->StringCopy(m_szAreaCode, _T("unknown"));
                InvalidateRect(m_hWnd, &m_rcAreaCodeDisplay, FALSE);
                SetTimer(m_hWnd, IDT_INVALID, 10000, NULL);
                return FALSE;
            }
            //otherwise our 2nd guess worked .. update the type
            m_eLookupType = eAlt;
        }
    }
    
    
    //get the info ... this really should never fail
    AreaCodeType* area = m_oArea.GetCodeInfo();

    if(area)
    {
        //start the timer ... when the ani finishes we'll switch to the info screen
        SetTimer(m_hWnd, IDT_GLOBE_TIMER, GLOBE_ANI_RATE, NULL);
        if(area->iFrame == m_iGlobeFrame)
            m_iGlobeFrame++;
        if(m_iGlobeFrame == NUMBER_OF_GLOBE_FRAMES)
            m_iGlobeFrame = 0;//so we get at least one rotation
        return TRUE;
    }
    //pretty terrible if it fails here...
    return FALSE;
}

BOOL CDlgAreaCode::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case _T('0'):
    case _T('1'):
    case _T('2'):
    case _T('3'):
    case _T('4'):
    case _T('5'):
    case _T('6'):
    case _T('7'):
    case _T('8'):
    case _T('9'):
        if(m_eState == AS_Search)
        {
            int iBtn = LOWORD(wParam) - _T('0');
            m_bNumBtns[iBtn] = TRUE;
            OnNumButton(iBtn);
            InvalidateRect(m_hWnd, &m_rcNumBtns[iBtn], FALSE);
            UpdateWindow(m_hWnd);	     
            m_bNumBtns[iBtn] = FALSE;   
            Sleep(50);//so we see the button light up
            InvalidateRect(m_hWnd, &m_rcNumBtns[iBtn], FALSE);
        }
        break;
    case VK_BACK:
        if(m_eState == AS_Search)
        {
            if(m_oStr->GetLength(m_szAreaCode) > 0)
            {
                m_oStr->Delete(m_oStr->GetLength(m_szAreaCode) - 1, 1, m_szAreaCode);
                InvalidateRect(m_hWnd, &m_rcAreaCodeDisplay, FALSE);
            }
        }
        break;
    case _T('r'):
    case _T('R'):
        //always useful when testing
        InvalidateRect(m_hWnd, NULL, FALSE);
        UpdateWindow(m_hWnd);
    	break;
    case _T('c'):
    case _T('C'):
        g_eColorScheme = EnumColorScheme(g_eColorScheme + 1);
        if(g_eColorScheme >= COLOR_Count)
            g_eColorScheme = COLOR_Green;
        m_imgGreenBar.Destroy();
        LoadImages(m_hWnd, m_hInst);
        m_imgGreenBar.SetSize(WIDTH(m_rcTopGreenBar), HEIGHT(m_rcTopGreenBar));
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case _T('q'):
    case _T('Q'):
        SafeCloseWindow();
        break;
    case _T('s'):
    case _T('S'):
        if(m_eState == AS_Search)
            ChangeState(AS_Display);
        else 
            ChangeState(AS_Search);
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    default:
        return UNHANDLED;
        break;
    }

    return TRUE;
}

void CDlgAreaCode::OnMenuLeft()
{
    if(m_eState != AS_Search)
        return;
   
    //    Pop the Menu
    m_wndMenu.ResetContent();
//    TCHAR szText[STRING_LARGE];

#ifndef NMA
	m_wndMenu.AddItem(_T("More Apps"), IDMENU_MoreApps);
#endif
    m_wndMenu.AddItem(_T("About"), IDMENU_About);
    m_wndMenu.AddItem(_T("Help"), IDMENU_Help);
    m_wndMenu.AddSeparator();
    m_wndMenu.AddItem(_T("Quit"), IDMENU_Quit);

    RECT rc;
    GetWindowRect(m_hWnd, &rc);

    RECT rcBtn;
    rcBtn.left  = rc.left;
    rcBtn.top   = rc.bottom - HEIGHT(m_rcMenuRight);
    rcBtn.bottom= rc.bottom;
    rcBtn.right = rc.right/2;

    POINT ptBtnScreen;
    ClientToScreen(m_hWnd, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
    rc.bottom       -= HEIGHT(m_rcMenuRight);

    m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn)
        ,ADJUST_Bottom);
   
}

void CDlgAreaCode::Locate(int iIndex)
{
    AreaCodeType* area = NULL;

    switch(m_oArea.GetLookupType())
    {
    case LT_AreaCodes:
        area = m_oArea.GetAreaCodeInfo(iIndex);
        m_oStr->IntToString(m_szAreaCode, area->iAreaCode);
    	break;
    case LT_CountryCodes:
        area = m_oArea.GetCountryCodeInfo(iIndex);
        m_oStr->IntToString(m_szAreaCode, area->iCountryCode);
    	break;
    //anything else
    default:
        area = m_oArea.GetOtherInfo(iIndex);
        m_oStr->IntToString(m_szAreaCode, area->iAreaCode);
        break;
    }

    
    OnLocate();
}

//we'll simply show a list based off of the current mode
void CDlgAreaCode::OnMenuRight()
{
    if(m_eState != AS_Search)
    {
        ChangeState(AS_Search);
        OnClear();
        InvalidateRect(m_hWnd, NULL, FALSE);
        return;
    }


    CDlgList dlgAdd(m_hWnd, &m_oArea, m_eLookupType);
    dlgAdd.SetPreventDeleteList(TRUE);
    dlgAdd.Init(m_gdiMem, m_guiBackground);
    
    AreaCodeType* area = NULL;

    if(IDOK == dlgAdd.Launch(m_hWnd, m_hInst, TRUE))
    {
        int iIndex = dlgAdd.GetSelected();
        m_eLookupType = m_oArea.GetLookupType();//keep em synced .. probably don't need the local copy
        
        //USA Canada Hacks
        if(m_eLookupType == LT_CountryCodes)
        {
            TypeItems* sItem = dlgAdd.GetSelectedEntry();

            if(sItem == NULL || sItem->lpItem == NULL)
                return;

            MenuType* sMenu = (MenuType*)sItem->lpItem;

            if(sMenu == NULL)
                return;

            if(sMenu->iImage == 1)
            {
                m_oArea.SetCanada();
                m_oStr->IntToString(m_szAreaCode, 1);
                OnLocate(FALSE);
                return;
            }
            else if(sMenu->iImage == 2)
            {
                m_oArea.SetUSA();
                m_oStr->IntToString(m_szAreaCode, 1);
                OnLocate(FALSE);
                return;
            }
        }
        
        
        Locate(iIndex);
        

        
    }
}

void CDlgAreaCode::OnSearchBy()
{
    if(m_eState != AS_Search)
        return;

    //    Pop the Menu
    m_wndMenu.ResetContent();
    //    TCHAR szText[STRING_LARGE];

    m_wndMenu.AddItem(_T("World"), IDMENU_World);
    m_wndMenu.AddItem(_T("North America"), IDMENU_NorthAmerica);
    m_wndMenu.AddSeparator();
    m_wndMenu.AddItem(_T("Australia"), IDMENU_Australia);
    m_wndMenu.AddItem(_T("New Zealand"), IDMENU_NewZealand);
    m_wndMenu.AddItem(_T("United Kingdom"), IDMENU_UK);
    
    RECT rc;
    GetWindowRect(m_hWnd, &rc);

    RECT rcBtn;
    rcBtn.left  = rc.left;
    rcBtn.top   = rc.bottom - HEIGHT(m_rcMenuRight);
    rcBtn.bottom= rc.bottom;
    rcBtn.right = rc.right/2;

    POINT ptBtnScreen;
    ClientToScreen(m_hWnd, &ptBtnScreen);
    rcBtn.top       += ptBtnScreen.y;
    rcBtn.bottom    += ptBtnScreen.y;
    rc.bottom       -= HEIGHT(m_rcMenuRight);

    m_wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rc.left, rc.top,
        WIDTH(rc), HEIGHT(rc),
        rcBtn.left, rcBtn.top,
        WIDTH(rcBtn), HEIGHT(rcBtn)
        ,ADJUST_Bottom);
}

void CDlgAreaCode::ChangeState(EnumAreaState eState)
{
    if(m_eState == eState)
        return;

    m_eState = eState;

    if(m_eState == AS_Display)
        LoadCurrentFlag();
}

void CDlgAreaCode::LoadCurrentFlag()
{
    int iTemp = m_oArea.GetCodeInfo()->iImageIndex;

    m_gdiFlag.Destroy();

    if(iTemp < 1)
        return;

    if(IsVGA() == TRUE)
    {
        m_gdiFlag.LoadImage(IDR_PNG_1 + iTemp - 1, m_hWnd, m_hInst);
    }
    else
    {
        CIssGDIEx gdiTemp;

        gdiTemp.LoadImage(IDR_PNG_1 + iTemp - 1, m_hWnd, m_hInst);

        SIZE sz;

        sz.cy = gdiTemp.GetHeight()*GetSystemMetrics(SM_CXICON)/64;
        sz.cx = gdiTemp.GetWidth()*GetSystemMetrics(SM_CXICON)/64;
    
        //ScaleImageFast
        ScaleImage(gdiTemp, m_gdiFlag, sz, FALSE, 0);
        
    }
}