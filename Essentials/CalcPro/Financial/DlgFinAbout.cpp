#include "IssGDIDraw.h"
#include "DlgFinAbout.h"
#include "Resource.h"
#include "stdafx.h"
#include "IssGDIFX.h"
#include "IssHardwareKeys.h"
#include "IssKey.h"

CDlgAbout::CDlgAbout(void)
:m_oMenu(NULL)
{
    m_hFont = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)/2, FW_BOLD, TRUE);
}

CDlgAbout::~CDlgAbout(void)
{
    CIssGDIEx::DeleteFont(m_hFont);
}

BOOL CDlgAbout::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
#ifdef WIN32_PLATFORM_WFSP
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
#else
    DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR|SHIDIF_SIPDOWN;
#endif
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

#endif

#ifdef WIN32_PLATFORM_WFSP
    //	BOOL bRet = SendMessage(m_oEdit.GetWnd(), EM_SETINPUTMODE, 0, EIM_TEXT );

    //	BOOL bRet = Edit_SetInputMode(m_oEdit.GetWnd(), IM_NUMBERS);
    SHFullScreen(m_hWnd, SHFS_HIDESIPBUTTON); 
    RECT rc; 
    GetClientRect(hWnd, &rc);
    SetRect(&rc, rc.top, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)-rc.top); 
    MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 
#endif

    CIssKey key;

    // check if we're running a legit copy
    key.Init(_T("SOFTWARE\\Panoramic\\PanoFinCalc"), _T("SOFTWARE\\Pano\\PFC2"), 0x0, 0xCDF4312A, 0x541AD34B, 4, TRUE);
    key.CheckSetKey();
    key.CheckDemo();

    m_eRetailState = RS_Expired;
    m_iDaysRemaining = 0;

    if(key.m_bDemo)
    {
        if(key.m_bGood)
        {
            m_eRetailState = RS_Demo;
            m_iDaysRemaining = key.GetDaysLeft();
        }
    }
    else if(key.m_bGood)
    {
        m_eRetailState = RS_Full;
    }

	return TRUE;
}

BOOL CDlgAbout::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgAbout::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	RECT rcClip;
    GetClipBox(hDC, &rcClip);

    if(m_gdiMem.GetDC() == NULL)
    {
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        m_gdiMem.Create(hDC, WIDTH(rcClient), HEIGHT(rcClient), FALSE, TRUE);
    }

    DrawBackground(m_gdiMem.GetDC(), rcClip);
    DrawAboutText(m_gdiMem.GetDC(), rcClip);

    if(m_oMenu)
        m_oMenu->DrawMenu(m_gdiMem.GetDC(), rcClip, _T("Back"), NULL);
    
    BitBlt(hDC, rcClip.left, rcClip.top, WIDTH(rcClip), HEIGHT(rcClip), m_gdiMem.GetDC(), rcClip.left, rcClip.top, SRCCOPY);

	return TRUE;
}

BOOL CDlgAbout::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(m_oMenu->OnLButtonDown(pt, hWnd))
        return TRUE;
    return FALSE;
}

BOOL CDlgAbout::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(m_oMenu->OnLButtonUp(pt, hWnd))
        return TRUE;
    
    return FALSE;
}

BOOL CDlgAbout::DrawBackground(HDC hdc, RECT rcClip)
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    
    if(m_gdiBack.GetDC() == NULL)
    {
        m_gdiBack.LoadImage(IDR_PNG_About, m_hWnd, m_hInst, TRUE);

        if(m_gdiBack.GetWidth() < WIDTH(rcClient) ||
            m_gdiBack.GetHeight() < HEIGHT(rcClient))
        {   //we gotta resize this sucker
            SIZE size;

            double dbXScale = (double)WIDTH(rcClient)/(double)m_gdiBack.GetWidth();
            double dbYScale = (double)HEIGHT(rcClient)/(double)m_gdiBack.GetHeight();

            double dbScale = max(dbXScale, dbYScale);

            size.cx = (int)(dbScale*m_gdiBack.GetWidth() + 1);
            size.cy = (int)(dbScale*m_gdiBack.GetHeight() + 1);

            CIssGDIEx gdiTemp;

            gdiTemp.Create(hdc, size.cx, size.cy, FALSE, FALSE);
            
          //  ScaleImage(m_gdiBack, gdiTemp, size, FALSE, 0);
            StretchBlt(gdiTemp.GetDC(), 0, 0, size.cx, size.cy,
                m_gdiBack.GetDC(), 0, 0, m_gdiBack.GetWidth(), m_gdiBack.GetHeight(), SRCCOPY);

            m_gdiBack.Destroy();
            gdiTemp.MoveGdiMemory(m_gdiBack);
        }
    }
  

    //blit this bad boy centered and top aligned
    BitBlt(hdc, 0, 0, WIDTH(rcClient), HEIGHT(rcClient),
        m_gdiBack.GetDC(),(m_gdiBack.GetWidth() - WIDTH(rcClient))/2, 0, SRCCOPY); 

    return TRUE;
}

BOOL CDlgAbout::DrawAboutText(HDC hdc, RECT rcClip)
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    rcClient.top += GetSystemMetrics(SM_CXICON);
    if(m_oMenu)
        rcClient.bottom -= 2*m_oMenu->GetMenuHeight();

    TCHAR szTemp[STRING_MAX];
    TCHAR szState[STRING_NORMAL];

    switch(m_eRetailState)
    {
    case RS_Full:
        m_oStr->StringCopy(szState, _T("Retail Version"));
    	break;
    case RS_Demo:
        m_oStr->Format(szState, _T("Demo (%i Day"), m_iDaysRemaining);
        if(m_iDaysRemaining != 1)
            m_oStr->Concatenate(szState, _T("s)"));
        else
            m_oStr->Concatenate(szState, _T(")"));

    	break;
    case RS_Expired:
    default:
        m_oStr->StringCopy(szState, _T("Expired"));
        break;
    }
    
    m_oStr->Format(szTemp, _T("Panoramic Financial Calc\r\n%s 2.9.2\r\nCopyright 2007-2010\r\nAll rights reserved\r\n\r\n"), szState);

    OffsetRect(&rcClient, 1, 1);

    DrawText(hdc, szTemp,
        rcClient, DT_TOP | DT_CENTER, 
        m_hFont, 0);

    DrawText(hdc, _T("www.panoramicsoft.com"),
        rcClient, DT_BOTTOM | DT_CENTER, 
        m_hFont, 0);

    OffsetRect(&rcClient, -1, -1);

    DrawText(hdc, szTemp,
        rcClient, DT_TOP | DT_CENTER, 
        m_hFont, 0xFFFFFF);

    DrawText(hdc, _T("www.panoramicsoft.com"),
        rcClient, DT_BOTTOM | DT_CENTER, 
        m_hFont, 0xFFFFFF);

    return TRUE;
}

BOOL CDlgAbout::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef UNDER_CE
    switch(wParam)
    {
    case VK_ESCAPE://back key
    case VK_TTALK: 
    case VK_TEND:
    case VK_TSOFT1:
    case VK_TSOFT2:
        // just close
        SafeCloseWindow(0);
        break;
    }
#endif
    return TRUE;
}


BOOL CDlgAbout::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDOK)
    {
        SafeCloseWindow(0);
        return TRUE;

    }
	return UNHANDLED;
}
BOOL CDlgAbout::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
	GetClientRect(m_hWnd, &rc);

#ifdef UNDER_CE
	if(WIDTH(rc) != GetSystemMetrics(SM_CXSCREEN))
		return TRUE;
#endif

	m_oMenu->Init(hWnd, m_hInst, m_oMenu->GetCurrentSkin());
	
	return TRUE;
}

BOOL CDlgAbout::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HWND hWndSip = NULL;

    if(LOWORD(wParam) == WA_ACTIVE)
    {
#ifdef UNDER_CE
        SHFullScreen(m_hWnd, SHFS_HIDESIPBUTTON);
#endif
        CIssHardwareKeys* oKeys = CIssHardwareKeys::Instance();
        if(oKeys)
            oKeys->RegisterHotKeys(m_hWnd, m_hInst, KEY_Soft1|KEY_Soft2);
        return TRUE;
    }

    return FALSE;

}

BOOL CDlgAbout::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    if(uiMessage == WM_LEFT_SOFTKEY)
    {
        SafeCloseWindow(0);
        return TRUE;
    }
    return FALSE;
}
