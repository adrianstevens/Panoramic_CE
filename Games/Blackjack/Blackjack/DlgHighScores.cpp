#include "DlgHighScores.h"
#include "resource.h"
#include "IssGDIFx.h"
#include "IssGDIDraw.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssRect.h"
#include "IssImageSliced.h"
#include "IssLocalisation.h"


CDlgHighScores::CDlgHighScores(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
{
}

CDlgHighScores::~CDlgHighScores(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);
}

BOOL CDlgHighScores::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgHighScores::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, FALSE);

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

BOOL CDlgHighScores::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return UNHANDLED;
}


BOOL CDlgHighScores::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_gdiBackground.Destroy();
	m_gdiMem.Destroy();

	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);

#ifdef WIN32_PLATFORM_WFSP
    m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/7, FW_BOLD, TRUE);
    m_hFontSelected			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*1/2, FW_BOLD, TRUE);
#else
	m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/6, FW_BOLD, TRUE);
	m_hFontSelected			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);
#endif

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int iShadowSpace	= 2*HEIGHT(rcClient)/100;
    int iIndent = GetSystemMetrics(SM_CXICON)/4;
    m_rcBack.left	= iIndent;
    m_rcBack.right	= m_rcBack.left + GetSystemMetrics(SM_CXSCREEN)/4;
    m_rcBack.top	= rcClient.bottom - GetSystemMetrics(SM_CYICON) - iIndent;
    m_rcBack.bottom	= rcClient.bottom - iIndent;

	return TRUE;
}

BOOL CDlgHighScores::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_TSOFT1:
    case VK_ESCAPE:
		SafeCloseWindow(0);
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgHighScores::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(PtInRect(&m_rcBack, pt))
	{
		SafeCloseWindow(IDOK);
	}
	return TRUE;
}

BOOL CDlgHighScores::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
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



BOOL CDlgHighScores::FadeInScreen(HDC hDC, RECT rcClient)
{

	int iWidth	= GetSystemMetrics(SM_CXSCREEN);
	int iHeight = GetSystemMetrics(SM_CYSCREEN);

	DrawBackground(rcClient);
	DrawText();

	::FadeIn(hDC, m_gdiMem);

	return TRUE;
}

void CDlgHighScores::InitBackground()
{
	if(!m_gdiBackground.GetDC())
	{
        CIssGDIEx gdiTile;
        gdiTile.LoadImage(IDB_PNG_Felt, m_hWnd, m_hInst);	

		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		m_gdiBackground.Create(m_gdiMem.GetDC(), rcClient, FALSE, TRUE);

        // copy the background over
        TileBackGround(m_gdiBackground,
            rcClient,
            gdiTile,
            FALSE);

		// create the back button
        CIssImageSliced btnItem;
        btnItem.Initialize(m_hWnd, m_hInst, (IsVGA()?IDR_PNG_MenuBtnVGA:IDR_PNG_MenuBtn));
        btnItem.SetSize(WIDTH(m_rcBack), HEIGHT(m_rcBack));
        btnItem.DrawImage(m_gdiBackground, m_rcBack.left, m_rcBack.top);
		TCHAR szText[STRING_MAX];
		m_oStr->StringCopy(szText, ID(IDS_MENU_Back), STRING_MAX, m_hInst);
		CIssRect rcZero;
		rcZero.Set(m_rcBack);
		rcZero.Translate(1,1);
		::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(100,100,100));
		rcZero.Translate(-1,-1);
        ::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255));
	}
}

void CDlgHighScores::DrawBackground(RECT rcLocation)
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

void CDlgHighScores::DrawText()
{
	int iY, iX, iHalf;
	RECT rc;
	int iItemHeight = GetSystemMetrics(SM_CXICON)/2;
	iHalf			= GetSystemMetrics(SM_CXSCREEN)/2;

	iY = iItemHeight/2;
	iX = 0;
	::SetRect(&rc, iX, iY, iX + iHalf, iY + iItemHeight);
	MyDrawText(ID(IDS_LBL_Bank), rc, m_hFontSelected);

	iX += iHalf;
	::SetRect(&rc, iX, iY, iX + iHalf, iY + iItemHeight);
	MyDrawText(ID(IDS_LBL_Date), rc, m_hFontSelected);

	iY += 3*iItemHeight/2;

	TCHAR szText[STRING_MAX];
	for(int i=0; i<NUM_HIGHSCORES; i++)
	{
		TypeScore* sScore = m_oHighScores->GetScore(i);
		if(sScore && sScore->lpScore)
		{
			TypeHighScore* sHi = (TypeHighScore*)sScore->lpScore;
			if(!sHi)
				continue;

			iX = 0;
			::SetRect(&rc, iX, iY, iX + iHalf, iY + iItemHeight);
			m_oStr->Format(szText, _T("$%6d"), sHi->iBank);
			MyDrawText(szText, rc, m_hFontText);

			iX += iHalf;
			GetDateFormat(NULL, DATE_SHORTDATE, &sScore->sysTime, NULL, szText, STRING_MAX);
			::SetRect(&rc, iX, iY, iX + iHalf, iY + iItemHeight);
			MyDrawText(szText, rc, m_hFontText);
		}
		else
		{
			m_oStr->StringCopy(szText, _T("--"));

			iX = 0;
			::SetRect(&rc, iX, iY, iX + iHalf, iY + iItemHeight);
			MyDrawText(szText, rc, m_hFontText);


			iX += iHalf;
			::SetRect(&rc, iX, iY, iX + iHalf, iY + iItemHeight);
			MyDrawText(szText, rc, m_hFontText);
		}

		iY += iItemHeight;
	}
}


void CDlgHighScores::MyDrawText(TCHAR* szText, RECT rcLocation, HFONT hFont, UINT uiFormat, COLORREF crText, COLORREF crShadow)
{
	rcLocation.left ++;
	rcLocation.top ++;
	::DrawText(m_gdiMem.GetDC(), szText, rcLocation, uiFormat, hFont, crShadow);
	rcLocation.left --;
	rcLocation.top --;
	::DrawText(m_gdiMem.GetDC(), szText, rcLocation, uiFormat, hFont, crText);
}

void CDlgHighScores::MyDrawText(UINT uiText, RECT rcLocation, HFONT hFont, UINT uiFormat, COLORREF crText, COLORREF crShadow)
{
	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, uiText, STRING_MAX, m_hInst);
	
	MyDrawText(szText, rcLocation, hFont, uiFormat, crText, crShadow);	
}

