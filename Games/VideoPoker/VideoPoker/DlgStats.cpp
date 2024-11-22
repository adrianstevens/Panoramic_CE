#include "DlgStats.h"
#include "resource.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssImageSliced.h"
#include "IssRect.h"
#include "DlgMsgBox.h"

extern CObjGui* g_gui;

#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Item          0xD0D0D0
#define TEXTCOLOR_Selected      RGB(149,223,49) //RGB(255,246,0) //RGB(119,175,57)

CDlgStats::CDlgStats(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
,m_oGame(NULL)
{
    m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*3/6, FW_BOLD, TRUE);
    m_hFontSelected			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);
}

CDlgStats::~CDlgStats(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);
}

void CDlgStats::Init(CIssVPGame* oGame)
{
    m_oGame = oGame;
}

BOOL CDlgStats::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

    m_oMenu.SetColors(TEXTCOLOR_Normal, TEXTCOLOR_Normal, TEXTCOLOR_Selected);
    m_oMenu.Initialize(hWnd, hWnd, m_hInst, OPTION_CircularList);
 //   m_oMenu.SetCustomDrawFunc(DrawButtonItem, this);
 //   m_oMenu.SetDeleteItemFunc(DeleteItem);
    m_oMenu.SetSelected(IDB_PNG_MenuSelector);
    m_oMenu.SetImageArray(IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray);
    m_oMenu.PreloadImages(hWnd, m_hInst);

    PopulateList();
    InvalidateRect(hWnd, NULL, FALSE);


	return TRUE;
}

BOOL CDlgStats::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    if(m_bFirstTime)
	{
		FadeInScreen(hDC, rcClient);
		m_bFirstTime = FALSE;
	}

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

	g_gui->DrawBackground(rcClient);
	//DrawText(g_gui->gdiMem.GetDC());

    m_oMenu.OnDraw(g_gui->gdiMem, rcClient, rcClip);

    g_gui->DrawBottomBar(rcClient, _T("Back"), _T("Reset"));

    g_gui->DrawScreen(rcClient, hDC);


	return TRUE;
}

BOOL CDlgStats::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnTimer(wParam, lParam);
}

BOOL CDlgStats::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenu.OnKeyDown(wParam, lParam);
}

BOOL CDlgStats::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(LOWORD(wParam) == IDMENU_Left)
    {
        SafeCloseWindow(0);
    }
    if(LOWORD(wParam) == IDMENU_Right)
    {
        ResetStats();
    }
    return UNHANDLED;
}

BOOL CDlgStats::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    g_gui->OnSize(hWnd, rcClient);
    m_oMenu.OnSize(rcClient.left,rcClient.top, WIDTH(rcClient), HEIGHT(rcClient) - HEIGHT(g_gui->rcBottomBar));
	return TRUE;
}

BOOL CDlgStats::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_TSOFT1:
    case VK_ESCAPE:
        SafeCloseWindow(0);
        break;
    case VK_TSOFT2:
        ResetStats();
        break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgStats::OnLButtonDown(HWND hWnd, POINT& pt)
{
    if(m_oMenu.OnLButtonDown(pt))
    {}

	return g_gui->OnLButtonDown(hWnd, pt);;
}

BOOL CDlgStats::OnLButtonUp(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnLButtonUp(pt))
    {}

    return g_gui->OnLButtonUp(hWnd, pt);;
}

BOOL CDlgStats::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(m_oMenu.OnMouseMove(pt))
    {}

    return TRUE;
}

BOOL CDlgStats::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
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



BOOL CDlgStats::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgStats::FadeInScreen(HDC hDC, RECT rcClient)
{

	int iWidth	= GetSystemMetrics(SM_CXSCREEN);
	int iHeight = GetSystemMetrics(SM_CYSCREEN);

	g_gui->DrawBackground(rcClient);
	m_oMenu.OnDraw(g_gui->gdiMem, rcClient, rcClient);

	FadeIn(hDC, g_gui->gdiMem);

	return TRUE;
}

void CDlgStats::PopulateList()
{
    TCHAR szTemp[STRING_MAX];

#define HANDS_FORMAT _T("  Hands played: %i")
#define PAYOUT_FORMAT _T("  Payout %: %f%%")

    int i = 0;

    m_oMenu.ResetContent();

    m_oMenu.AddCategory(_T("General"));

    m_oStr->Format(szTemp, _T("  Highest Bank: $%i"), m_oGame->m_iHighestBank);
    m_oMenu.AddItem(szTemp, 5000);

    m_oStr->Format(szTemp, _T("  Highest Payout: $%i"), m_oGame->m_iBiggestWin);
    m_oMenu.AddItem(szTemp, 5000);

    int iTemp = 0;
    double dbTemp = 0;
    for(int p = 0; p < 6; p++)
    {
        iTemp += m_oGame->m_iNumHandsPlayed[p];
        dbTemp += m_oGame->m_iNumHandsPlayed[p]*m_oGame->m_dbPayout[p];
    }

    dbTemp /= (double)iTemp;
    dbTemp *= 100;

    if(iTemp == 0)
        dbTemp = 0;

    m_oStr->Format(szTemp, HANDS_FORMAT, iTemp);
    m_oMenu.AddItem(szTemp, 5000);

    m_oStr->Format(szTemp, PAYOUT_FORMAT, dbTemp);
    m_oMenu.AddItem(szTemp, 5000);



    m_oMenu.AddCategory(IDS_GAMETYPE_10s, m_hInst);
    m_oStr->Format(szTemp, HANDS_FORMAT, m_oGame->m_iNumHandsPlayed[i]);
    m_oMenu.AddItem(szTemp, 5000);
    m_oStr->Format(szTemp, PAYOUT_FORMAT, m_oGame->m_dbPayout[i++]*100);
    m_oMenu.AddItem(szTemp, 5000);
    
    
    m_oMenu.AddCategory(IDS_GAMETYPE_Jacks, m_hInst);
    m_oStr->Format(szTemp, HANDS_FORMAT, m_oGame->m_iNumHandsPlayed[i]);
    m_oMenu.AddItem(szTemp, 5000);
    m_oStr->Format(szTemp, PAYOUT_FORMAT, m_oGame->m_dbPayout[i++]*100);
    m_oMenu.AddItem(szTemp, 5000);
    
        
    m_oMenu.AddCategory(IDS_GAMETYPE_BonusDeluxe, m_hInst);
    m_oStr->Format(szTemp, HANDS_FORMAT, m_oGame->m_iNumHandsPlayed[i]);
    m_oMenu.AddItem(szTemp, 5000);
    m_oStr->Format(szTemp, PAYOUT_FORMAT, m_oGame->m_dbPayout[i++]*100);
    m_oMenu.AddItem(szTemp, 5000);
    
       
    m_oMenu.AddCategory(IDS_GAMETYPE_Aces, m_hInst);
    m_oStr->Format(szTemp, HANDS_FORMAT, m_oGame->m_iNumHandsPlayed[i]);
    m_oMenu.AddItem(szTemp, 5000);
    m_oStr->Format(szTemp, PAYOUT_FORMAT, m_oGame->m_dbPayout[i++]*100);
    m_oMenu.AddItem(szTemp, 5000);
    
        
    m_oMenu.AddCategory(IDS_GAMETYPE_Double, m_hInst);
    m_oStr->Format(szTemp, HANDS_FORMAT, m_oGame->m_iNumHandsPlayed[i]);
    m_oMenu.AddItem(szTemp, 5000);
    m_oStr->Format(szTemp, PAYOUT_FORMAT, m_oGame->m_dbPayout[i++]*100);
    m_oMenu.AddItem(szTemp, 5000);

    
    m_oMenu.AddCategory(IDS_GAMETYPE_Stud, m_hInst);
    m_oStr->Format(szTemp, HANDS_FORMAT, m_oGame->m_iNumHandsPlayed[i]);
    m_oMenu.AddItem(szTemp, 5000);
    m_oStr->Format(szTemp, PAYOUT_FORMAT, m_oGame->m_dbPayout[i++]*100);
    m_oMenu.AddItem(szTemp, 5000);

    m_oMenu.AddItem(_T(" "), 5000);
}

void CDlgStats::ResetStats()
{
    CDlgMsgBox dlgMsg;
    if(IDYES == dlgMsg.PopupMessage(_T("Are you sure you want to clear all statistics?"), m_hWnd, m_hInst, MB_YESNO))
    {
        m_oGame->ResetStats();
        PopulateList();
    }
}