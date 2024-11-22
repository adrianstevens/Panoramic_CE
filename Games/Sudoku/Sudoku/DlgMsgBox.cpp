#include "DlgMsgBox.h"
#include "resource.h"
#include "IssGDIFX.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssRect.h"
#include "IssImageSliced.h"
#include "IssLocalisation.h"


CDlgMsgBox::CDlgMsgBox(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
,m_bYesNo(FALSE)
,m_szText(NULL)
{
}

CDlgMsgBox::~CDlgMsgBox(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);
    m_oStr->Delete(&m_szText);
}

int CDlgMsgBox::PopupMessage(UINT uiMessage, HWND hWndParent, HINSTANCE hInst, UINT uiFlag)
{
    TCHAR szText[STRING_MAX];
    m_oStr->StringCopy(szText, uiMessage, STRING_MAX, hInst);

    return PopupMessage(szText, hWndParent, hInst, uiFlag);
}

int CDlgMsgBox::PopupMessage(TCHAR* szMessage, HWND hWndParent, HINSTANCE hInst, UINT uiFlag)
{
    m_oStr->Delete(&m_szText);
    m_szText = m_oStr->CreateAndCopy(szMessage);

    if(uiFlag == MB_OK)
        m_bYesNo = FALSE;
    else
        m_bYesNo = TRUE;

    BOOL bRet = DoModal(hWndParent, hInst, IDD_DLG_Basic);
    return (int)bRet;
}

BOOL CDlgMsgBox::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);
#endif

	m_bFirstTime = TRUE;


	return TRUE;
}

BOOL CDlgMsgBox::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, TRUE);

	if(m_bFirstTime)
	{
		FadeInScreen(hDC, rcClient);
		m_bFirstTime = FALSE;
	}

	DrawBackground(rcClient);
	DrawText();

	// draw all to the screen
	BitBlt(hDC,
		rcClient.left,rcClient.top,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiMem.GetDC(),
		rcClient.left,rcClient.top,
		SRCCOPY);


	return TRUE;
}

BOOL CDlgMsgBox::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}

BOOL CDlgMsgBox::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_gdiBackground.Destroy();
	m_gdiMem.Destroy();

	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);
	m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/6, FW_BOLD, TRUE);
	m_hFontSelected			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	int iShadowSpace	= 2*HEIGHT(rcClient)/100;
	int iBtnWidth		= GetSystemMetrics(SM_CXSCREEN)/4;
	int iBtnHeight		= GetSystemMetrics(SM_CYICON);
	int iTextHeight		= GetSystemMetrics(SM_CXICON)*3/6;
	int iTotalHeight	= 0;

	iTotalHeight		= iBtnHeight /*for buttons */ + 3*iTextHeight /* 4 Text Height */;
	int iYpos			= (rcClient.bottom - iTotalHeight)/3;

	m_rcText			= rcClient;
	m_rcText.top		= iYpos;
	m_rcText.bottom		= iYpos + 7*iTextHeight;

    if(m_bYesNo)
    {
	    iYpos				= m_rcText.bottom;
	    m_rcYes.left		= rcClient.right/2 - iBtnWidth - iShadowSpace;
	    m_rcYes.right		= m_rcYes.left + iBtnWidth;
	    m_rcYes.top			= iYpos;
	    m_rcYes.bottom		= m_rcYes.top + iBtnHeight;

	    m_rcNo				= m_rcYes;
	    m_rcNo.left			= rcClient.right/2 + iShadowSpace;
	    m_rcNo.right		= m_rcNo.left + iBtnWidth;
    }
    else
    {
        iYpos				= m_rcText.bottom;
        m_rcYes.left		= rcClient.right/2 - iBtnWidth/2;
        m_rcYes.right		= m_rcYes.left + iBtnWidth;
        m_rcYes.top			= iYpos;
        m_rcYes.bottom		= m_rcYes.top + iBtnHeight;

        SetRect(&m_rcNo, 0,0,0,0);
    }

	return TRUE;
}

BOOL CDlgMsgBox::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_TSOFT1:
        SafeCloseWindow(m_bYesNo?IDYES:IDOK);
		break;
	case VK_TSOFT2:
    case VK_ESCAPE:
        SafeCloseWindow(m_bYesNo?IDNO:IDOK);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;

}

BOOL CDlgMsgBox::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(PtInRect(&m_rcYes, pt))
		SafeCloseWindow(m_bYesNo?IDYES:IDOK);
	else if(PtInRect(&m_rcNo, pt))
		SafeCloseWindow(m_bYesNo?IDNO:IDOK);

	return TRUE;
}

BOOL CDlgMsgBox::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) 
	{ 
		SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
		RECT rc; 
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 

		return TRUE; 
	}
	return UNHANDLED;
}



BOOL CDlgMsgBox::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgMsgBox::FadeInScreen(HDC hDC, RECT rcClient)
{

	int iWidth	= GetSystemMetrics(SM_CXSCREEN);
	int iHeight = GetSystemMetrics(SM_CYSCREEN);

	DrawBackground(rcClient);
	DrawText();

	FadeIn(hDC, m_gdiMem);

	return TRUE;
}

void CDlgMsgBox::InitBackground()
{
	if(!m_gdiBackground.GetDC())
	{
		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		HDC dc = GetDC(m_hWnd);
		m_gdiBackground.Create(dc, rcClient, TRUE, TRUE);
		ReleaseDC(m_hWnd, dc);

		// darken it up a bit
		AlphaFillRect(m_gdiBackground, rcClient, RGB(0,0,0), 192);

		// create the Yes button
		CIssImageSliced btnItem;
		btnItem.Initialize(m_hWnd, m_hInst, (IsVGA()?IDR_PNG_MenuBtnVGA:IDR_PNG_MenuBtn));
		btnItem.SetSize(WIDTH(m_rcYes), HEIGHT(m_rcYes));
		btnItem.DrawImage(m_gdiBackground, m_rcYes.left, m_rcYes.top);
		TCHAR szText[STRING_MAX];
        CIssRect rcZero;

        if(m_bYesNo)
        {
		    m_oStr->StringCopy(szText, ID(IDS_MENU_Yes), STRING_MAX, m_hInst);		
		    rcZero.Set(m_rcYes);
		    rcZero.Translate(1,1);
		    ::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(100,100,100));
		    rcZero.Translate(-1,-1);
		    ::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255));

		    // the no button
		    btnItem.DrawImage(m_gdiBackground, m_rcNo.left, m_rcNo.top);
		    m_oStr->StringCopy(szText, ID(IDS_MENU_No), STRING_MAX, m_hInst);
		    rcZero.Set(m_rcNo);
		    rcZero.Translate(1,1);
		    ::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(100,100,100));
		    rcZero.Translate(-1,-1);
		    ::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255));
        }
        else
        {
            m_oStr->StringCopy(szText, ID(IDS_MENU_Ok), STRING_MAX, m_hInst);		
            rcZero.Set(m_rcYes);
            rcZero.Translate(1,1);
            ::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(100,100,100));
            rcZero.Translate(-1,-1);
            ::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255));
        }
	}
}

void CDlgMsgBox::DrawBackground(RECT rcLocation)
{
	InitBackground();

	// copy the background over
	BitBlt(m_gdiMem.GetDC(),
		   rcLocation.left, rcLocation.top,
		   WIDTH(rcLocation), HEIGHT(rcLocation),
		   m_gdiBackground.GetDC(),
		   rcLocation.left, rcLocation.top,
		   SRCCOPY);
}

void CDlgMsgBox::DrawText()
{
	RECT rc = m_rcText;
	rc.left ++;
	rc.top ++;
	::DrawText(m_gdiMem.GetDC(), m_szText, rc, DT_CENTER|DT_WORDBREAK, m_hFontText, RGB(0,0,0));
	rc.left --;
	rc.top --;
	::DrawText(m_gdiMem.GetDC(), m_szText, rc, DT_CENTER|DT_WORDBREAK, m_hFontText, RGB(255,255,255));	
}




