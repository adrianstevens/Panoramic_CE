#include "DlgHighScores.h"
#include "resource.h"
#include "IssGDIEffects.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssBeveledEdge.h"


CDlgHighScores::CDlgHighScores(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
{
}

CDlgHighScores::~CDlgHighScores(void)
{
	CIssGDI::DeleteFont(m_hFontText);
	CIssGDI::DeleteFont(m_hFontSelected);
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

	CIssGDI::DeleteFont(m_hFontText);
	CIssGDI::DeleteFont(m_hFontSelected);
	m_hFontText				= CIssGDI::CreateFont(GetSystemMetrics(SM_CXICON)*3/6, FW_BOLD, TRUE);
	m_hFontSelected			= CIssGDI::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int iShadowSpace	= 2*HEIGHT(rcClient)/100;
	
#ifdef WIN32_PLATFORM_WFSP
    int iBtnHeight	= GetSystemMetrics(SM_CXICON)*6/5;
#else
    int iBtnHeight	= GetSystemMetrics(SM_CXICON)*3/2;
#endif

    m_rcBack.left	= 0-iShadowSpace;
    m_rcBack.right	= m_rcBack.left + GetSystemMetrics(SM_CXSCREEN)/2;
    m_rcBack.top	= rcClient.bottom - iBtnHeight + iShadowSpace;
    m_rcBack.bottom	= m_rcBack.top + iBtnHeight;

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
	if(LOWORD(wParam) == WA_ACTIVE) 
	{ 
		SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
		RECT rc; 
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 

		m_oKeys->RegisterHotKeys(m_hWnd, m_hInst, KEY_Soft1|KEY_Soft2);

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

	CIssGDIEffects::FadeIn(hDC, m_gdiMem);

	return TRUE;
}

void CDlgHighScores::InitBackground()
{
	if(!m_gdiBackground.GetDC())
	{
		CIssGDI gdiTile;
		LoadImage(gdiTile, IDR_TILE_BG, m_hWnd, m_hInst);		

		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		m_gdiBackground.Create(m_gdiMem.GetDC(), rcClient, FALSE, TRUE);

		// copy the background over
		CIssGDI::TileBackGround(m_gdiBackground.GetDC(),
			rcClient,
			gdiTile,
			FALSE);

		// create the back button
		CIssBeveledEdge btnItem;
		btnItem.Initialize(m_hWnd, m_hInst, 
			IDR_MN_TL, IDR_MN_T, IDR_MN_TR, IDR_MN_R, IDR_MN_BR, IDR_MN_B, IDR_MN_BL, IDR_MN_L, IDR_MN_M);
		btnItem.InitializeAlpha(IDR_MNA_TL, IDR_MNA_T, IDR_MNA_TR, IDR_MNA_R, IDR_MNA_BR, IDR_MNA_B, IDR_MNA_BL, IDR_MNA_L, IDR_MNA_M);
		btnItem.SetLocation(m_rcBack);
		btnItem.Draw(m_gdiBackground, 255);
		TCHAR szText[STRING_MAX];
		m_oStr->StringCopy(szText, IDS_MENU_Back, STRING_MAX, m_hInst);
		CIssRect rcZero;
		rcZero.Set(m_rcBack);
		rcZero.Translate(1,1);
		CIssGDI::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(100,100,100));
		rcZero.Translate(-1,-1);
		CIssGDI::DrawText(m_gdiBackground.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255));
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
	int iY, iX, iThird;
	RECT rc;
	int iItemHeight = GetSystemMetrics(SM_CXICON)/2;
	iThird			= GetSystemMetrics(SM_CXSCREEN)/3;

	iY = iItemHeight/2;
	iX = 0;
	::SetRect(&rc, iX, iY, iX + iThird, iY + iItemHeight);
	MyDrawText(IDS_LBL_Score, rc, m_hFontSelected);

	iX += iThird;
	::SetRect(&rc, iX, iY, iX + iThird, iY + iItemHeight);
	MyDrawText(IDS_LBL_Level, rc, m_hFontSelected);

	iX += iThird;
	::SetRect(&rc, iX, iY, iX + iThird, iY + iItemHeight);
	MyDrawText(IDS_LBL_Date, rc, m_hFontSelected);

	iY += 3*iItemHeight/2;

	TCHAR szText[STRING_MAX];
	CIssHighScores* oHighScore = m_oGame->GetHighScoreTable();
	for(int i=0; i<NUM_HIGHSCORES; i++)
	{
		TypeScore* sScore = oHighScore->GetScore(i);
		if(sScore && sScore->lpScore)
		{
			TypeHighScore* sHi = (TypeHighScore*)sScore->lpScore;
			if(!sHi)
				continue;

			iX = 0;
			::SetRect(&rc, iX, iY, iX + iThird, iY + iItemHeight);
			m_oStr->Format(szText, _T("%6d"), sHi->iScore);
			MyDrawText(szText, rc, m_hFontText);

			iX += iThird;
			/*switch(sHi->eGameType)
			{
			case GT_Challenge:
				m_oStr->StringCopy(szText, IDS_LBL_GM2, STRING_MAX, m_hInst);
				break;
			case GT_Ultra:
				m_oStr->StringCopy(szText, IDS_LBL_GM3, STRING_MAX, m_hInst);
				break;
			case GT_40:
				m_oStr->StringCopy(szText, IDS_LBL_GM4, STRING_MAX, m_hInst);
				break;
			case GT_Classic:
			default:
				m_oStr->StringCopy(szText, IDS_LBL_GM1, STRING_MAX, m_hInst);
				break;
			}
			::SetRect(&rc, iX, iY, iX + iThird, iY + iItemHeight);
			MyDrawText(szText, rc, m_hFontText);*/

			::SetRect(&rc, iX, iY, iX + iThird, iY + iItemHeight);
			m_oStr->IntToString(szText, sHi->iLevel);
			MyDrawText(szText, rc, m_hFontText);

			iX += iThird;
			GetDateFormat(NULL, DATE_SHORTDATE, &sScore->sysTime, NULL, szText, STRING_MAX);
			::SetRect(&rc, iX, iY, iX + iThird, iY + iItemHeight);
			MyDrawText(szText, rc, m_hFontText);
		}
		else
		{
			m_oStr->StringCopy(szText, _T("--"));

			iX = 0;
			::SetRect(&rc, iX, iY, iX + iThird, iY + iItemHeight);
			MyDrawText(szText, rc, m_hFontText);

			iX += iThird;
			::SetRect(&rc, iX, iY, iX + iThird, iY + iItemHeight);
			MyDrawText(szText, rc, m_hFontText);

			iX += iThird;
			::SetRect(&rc, iX, iY, iX + iThird, iY + iItemHeight);
			MyDrawText(szText, rc, m_hFontText);
		}

		iY += iItemHeight;
	}
}


void CDlgHighScores::MyDrawText(TCHAR* szText, RECT rcLocation, HFONT hFont, UINT uiFormat, COLORREF crText, COLORREF crShadow)
{
	rcLocation.left ++;
	rcLocation.top ++;
	CIssGDI::DrawText(m_gdiMem.GetDC(), szText, rcLocation, uiFormat, hFont, crShadow);
	rcLocation.left --;
	rcLocation.top --;
	CIssGDI::DrawText(m_gdiMem.GetDC(), szText, rcLocation, uiFormat, hFont, crText);
}

void CDlgHighScores::MyDrawText(UINT uiText, RECT rcLocation, HFONT hFont, UINT uiFormat, COLORREF crText, COLORREF crShadow)
{
	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, uiText, STRING_MAX, m_hInst);
	
	MyDrawText(szText, rcLocation, hFont, uiFormat, crText, crShadow);	
}

