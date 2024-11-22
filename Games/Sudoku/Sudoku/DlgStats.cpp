#include "DlgStats.h"
#include "resource.h"
#include "IssGDIFX.h"
#include "IssGDIDraw.h"
#include "stdafx.h"
#include "IssCommon.h"
#include "IssImageSliced.h"
#include "IssRect.h"
#include "DlgMsgBox.h"
#include "IssLocalisation.h"

extern CObjGui* g_gui;

#define TEXTCOLOR_Normal        RGB(255,255,255)
#define TEXTCOLOR_Item          0xD0D0D0
#define TEXTCOLOR_Selected      RGB(149,223,49) //RGB(255,246,0) //RGB(119,175,57)


#define IDMENU_SudokuVeryEasy   6000
#define IDMENU_SudokuEasy       6001
#define IDMENU_SudokuMedium     6002
#define IDMENU_SudokuHard       6003
#define IDMENU_SudokuVeryHard   6004
#define IDMENU_SudokuGenius     6005

#define IDMENU_KillerVeryEasy         6010
#define IDMENU_KillerEasy             6011
#define IDMENU_KillerMedium           6012
#define IDMENU_KillerHard             6013
#define IDMENU_KillerVeryHard         6014

#define IDMENU_KakuroVeryEasy         6020
#define IDMENU_KakuroEasy             6021
#define IDMENU_KakuroMedium           6022
#define IDMENU_KakuroHard             6023
#define IDMENU_KakuroVeryHard         6024

#define IDMENU_KenKenVeryEasy         6030
#define IDMENU_KenKenEasy             6031
#define IDMENU_KenKenMedium           6032
#define IDMENU_KenKenHard             6033
#define IDMENU_KenKenVeryHard         6034

#define IDMENU_GreaterVeryEasy         6040
#define IDMENU_GreaterEasy             6041
#define IDMENU_GreaterMedium           6042
#define IDMENU_GreaterHard             6043
#define IDMENU_GreaterVeryHard         6044


CDlgStats::CDlgStats(void)
:m_hFontSelected(NULL)
,m_hFontText(NULL)
,m_eEngine(NULL)
{
    m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*2/5, FW_BOLD, TRUE);
    m_hFontSelected			= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*5/9, FW_BOLD, TRUE);
}

CDlgStats::~CDlgStats(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
	CIssGDIEx::DeleteFont(m_hFontSelected);
}

void CDlgStats::Init(CSEngine* oEngine)
{
    m_eEngine = oEngine;
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
    m_oMenu.Initialize(hWnd, hWnd, m_hInst, OPTION_CircularList | OPTION_DrawScrollArrows | OPTION_Bounce);
    m_oMenu.SetCustomDrawFunc(DrawListItem, this);
    m_oMenu.SetDeleteItemFunc(DeleteListItem);
    m_oMenu.SetSelected(IDB_PNG_MenuSelector);
    m_oMenu.SetImageArray(IsVGA()?IDB_PNG_MenuArray_HR:IDB_PNG_MenuArray);
    m_oMenu.PreloadImages(hWnd, m_hInst);

    PopulateList();
    InvalidateRect(hWnd, NULL, FALSE);


	return TRUE;
}

BOOL CDlgStats::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
/*    if(m_bFirstTime)
	{
		FadeInScreen(hDC, rcClient);
		m_bFirstTime = FALSE;
	}*/

    RECT rcClip;
    GetClipBox(hDC, &rcClip);

    FillRect(g_gui->gdiMem.GetDC(), rcClip, 0);

	//g_gui->DrawTitle(rcClient);
	//DrawText(g_gui->gdiMem.GetDC());

    m_oMenu.OnDraw(g_gui->gdiMem, rcClient, rcClip);

    TCHAR szBack[STRING_NORMAL];
    m_oStr->StringCopy(szBack, ID(IDS_MENU_Back), STRING_NORMAL, m_hInst);

    TCHAR szTemp[STRING_NORMAL];
    m_oStr->StringCopy(szTemp, ID(IDS_MENU_Reset), STRING_NORMAL, m_hInst);

    g_gui->DrawBottomBar(rcClient, szBack, NULL);

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

	g_gui->DrawTitle(rcClient);
	m_oMenu.OnDraw(g_gui->gdiMem, rcClient, rcClient);

	FadeIn(hDC, g_gui->gdiMem);

	return TRUE;
}

void CDlgStats::PopulateList()
{
    m_oMenu.AddItem(LPVOID(IDMENU_SudokuVeryEasy), IDMENU_SudokuVeryEasy, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_SudokuEasy), IDMENU_SudokuEasy, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_SudokuMedium), IDMENU_SudokuMedium, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_SudokuHard), IDMENU_SudokuHard, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_SudokuVeryHard), IDMENU_SudokuVeryHard, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_SudokuGenius), IDMENU_SudokuGenius, 0, 0);

    m_oMenu.AddItem(LPVOID(IDMENU_KillerVeryEasy), IDMENU_KillerVeryEasy, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KillerEasy), IDMENU_KillerEasy, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KillerMedium), IDMENU_KillerMedium, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KillerHard), IDMENU_KillerHard, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KillerVeryHard), IDMENU_KillerVeryHard, 0, 0);

    m_oMenu.AddItem(LPVOID(IDMENU_KakuroVeryEasy), IDMENU_KakuroVeryEasy, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KakuroEasy), IDMENU_KakuroEasy, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KakuroMedium), IDMENU_KakuroMedium, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KakuroHard), IDMENU_KakuroHard, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KakuroVeryHard), IDMENU_KakuroVeryHard, 0, 0);

    m_oMenu.AddItem(LPVOID(IDMENU_KenKenVeryEasy), IDMENU_KenKenVeryEasy, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KenKenEasy), IDMENU_KenKenEasy, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KenKenMedium), IDMENU_KenKenMedium, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KenKenHard), IDMENU_KenKenHard, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_KenKenVeryHard), IDMENU_KenKenVeryHard, 0, 0);

    m_oMenu.AddItem(LPVOID(IDMENU_GreaterVeryEasy), IDMENU_GreaterVeryEasy, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_GreaterEasy), IDMENU_GreaterEasy, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_GreaterMedium), IDMENU_GreaterMedium, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_GreaterHard), IDMENU_GreaterHard, 0, 0);
    m_oMenu.AddItem(LPVOID(IDMENU_GreaterVeryHard), IDMENU_GreaterVeryHard, 0, 0);

    m_oMenu.SetItemHeights((IsVGA()?40:20), (IsVGA()?40:20));
}

void CDlgStats::ResetStats()
{
    TCHAR szTemp[STRING_MAX];
    m_oStr->StringCopy(szTemp, ID(IDS_MSG_ClearStatistics), STRING_NORMAL, m_hInst);

    CDlgMsgBox dlgMsg;
    if(IDYES == dlgMsg.PopupMessage(szTemp, m_hWnd, m_hInst, MB_YESNO))
    {

    }
}

void CDlgStats::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgStats* pList = (CDlgStats*)lpClass;
    pList->DrawListItem(gdi, rcDraw, sItem, bIsHighlighted);
}


void CDlgStats::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{
    if(!sItem)
        return;

    RECT rcTemp = rcDraw;
    rcTemp.left += GetSystemMetrics(SM_CXSMICON)/2;
    rcTemp.right -= GetSystemMetrics(SM_CXSMICON)/2;

    UINT id1, id2;

    TCHAR szText[STRING_NORMAL];
    TCHAR szTemp[STRING_NORMAL];

    int iTime, iType, iDiff;

    switch(sItem->uiMenuID)
    {
    case IDMENU_SudokuVeryEasy:
        id1 = ID(IDS_Sudoku);
        id2 = ID(IDS_PREF_VeryEasy);
        iType = STYPE_Sudoku;                
        iDiff = DIFF_VeryEasy;
        break;
    case IDMENU_SudokuEasy:    
        id1 = ID(IDS_Sudoku);
        id2 = ID(IDS_PREF_Easy);
        iType = STYPE_Sudoku;
        iDiff = DIFF_Easy;
        break;
    case IDMENU_SudokuMedium:
        id1 = ID(IDS_Sudoku);
        id2 = ID(IDS_PREF_Normal);
        iType = STYPE_Sudoku;
        iDiff = DIFF_Medium;
        break;
    case IDMENU_SudokuHard:  
        id1 = ID(IDS_Sudoku);
        id2 = ID(IDS_PREF_Hard);
        iType = STYPE_Sudoku;
        iDiff = DIFF_Hard;
        break;
    case IDMENU_SudokuVeryHard:   
        id1 = ID(IDS_Sudoku);
        id2 = ID(IDS_PREF_Expert);
        iType = STYPE_Sudoku;
        iDiff = DIFF_VeryHard;
        break;
    case IDMENU_SudokuGenius:  
        id1 = ID(IDS_Sudoku);
        id2 = ID(IDS_PREF_Genius);
        iType = STYPE_Sudoku;
        iDiff = DIFF_Hardcore;
        break;
    case IDMENU_KillerVeryEasy:   
        id1 = ID(IDS_KillerSudoku);
        id2 = ID(IDS_PREF_VeryEasy);
        iType = STYPE_Killer;
        iDiff = DIFF_VeryEasy;
        break;
    case IDMENU_KillerEasy:       
        id1 = ID(IDS_KillerSudoku);
        id2 = ID(IDS_PREF_Easy);
        iType = STYPE_Killer;
        iDiff = DIFF_Easy;
        break;
    case IDMENU_KillerMedium:     
        id1 = ID(IDS_KillerSudoku);
        id2 = ID(IDS_PREF_Normal);
        iType = STYPE_Killer;
        iDiff = DIFF_Medium;
        break;
    case IDMENU_KillerHard:       
        id1 = ID(IDS_KillerSudoku);
        id2 = ID(IDS_PREF_Hard);
        iType = STYPE_Killer;
        iDiff = DIFF_Hard;
        break;
    case IDMENU_KillerVeryHard:   
        id1 = ID(IDS_KillerSudoku);
        id2 = ID(IDS_PREF_Expert);
        iType = STYPE_Killer;
        iDiff = DIFF_VeryHard;
        break;

    case IDMENU_KakuroVeryEasy:   
        id1 = ID(IDS_Kakuro);
        id2 = ID(IDS_PREF_VeryEasy);
        iType = STYPE_Kakuro;
        iDiff = DIFF_VeryEasy;
        break;
    case IDMENU_KakuroEasy:     
        id1 = ID(IDS_Kakuro);
        id2 = ID(IDS_PREF_Easy);
        iType = STYPE_Kakuro;
        iDiff = DIFF_Easy;
        break;
    case IDMENU_KakuroMedium:     
        id1 = ID(IDS_Kakuro);
        id2 = ID(IDS_PREF_Normal);
        iType = STYPE_Kakuro;
        iDiff = DIFF_Medium;
        break;
    case IDMENU_KakuroHard:       
        id1 = ID(IDS_Kakuro);
        id2 = ID(IDS_PREF_Hard);
        iType = STYPE_Kakuro;
        iDiff = DIFF_Hard;
        break;
    case IDMENU_KakuroVeryHard:   
        id1 = ID(IDS_Kakuro);
        id2 = ID(IDS_PREF_Expert);
        iType = STYPE_Kakuro;
        iDiff = DIFF_VeryHard;
        break;
    case IDMENU_KenKenVeryEasy:   
        id1 = ID(IDS_KenKen);
        id2 = ID(IDS_PREF_VeryEasy);
        iType = STYPE_KenKen;
        iDiff = DIFF_VeryEasy;
        break;
    case IDMENU_KenKenEasy:       
        id1 = ID(IDS_KenKen);
        id2 = ID(IDS_PREF_Easy);
        iType = STYPE_KenKen;
        iDiff = DIFF_Easy;
        break;
    case IDMENU_KenKenMedium:     
        id1 = ID(IDS_KenKen);
        id2 = ID(IDS_PREF_Normal);
        iType = STYPE_KenKen;
        iDiff = DIFF_Medium;
        break;
    case IDMENU_KenKenHard:
        id1 = ID(IDS_KenKen);
        id2 = ID(IDS_PREF_Hard);
        iType = STYPE_KenKen;
        iDiff = DIFF_Hard;
        break;
    case IDMENU_KenKenVeryHard:
        id1 = ID(IDS_KenKen);
        id2 = ID(IDS_PREF_Expert);
        iType = STYPE_KenKen;
        iDiff = DIFF_VeryHard;
        break;
    case IDMENU_GreaterVeryEasy:   
        id1 = ID(IDS_Greater);
        id2 = ID(IDS_PREF_VeryEasy);
        iType = STYPE_Greater;
        iDiff = DIFF_VeryEasy;
        break;
    case IDMENU_GreaterEasy:       
        id1 = ID(IDS_Greater);
        id2 = ID(IDS_PREF_Easy);
        iType = STYPE_Greater;
        iDiff = DIFF_Easy;
        break;
    case IDMENU_GreaterMedium:     
        id1 = ID(IDS_Greater);
        id2 = ID(IDS_PREF_Normal);
        iType = STYPE_Greater;
        iDiff = DIFF_Medium;
        break;
    case IDMENU_GreaterHard:
        id1 = ID(IDS_Greater);
        id2 = ID(IDS_PREF_Hard);
        iType = STYPE_Greater;
        iDiff = DIFF_Hard;
        break;
    case IDMENU_GreaterVeryHard:
        id1 = ID(IDS_Greater);
        id2 = ID(IDS_PREF_Expert);
        iType = STYPE_Greater;
        iDiff = DIFF_VeryHard;
        break;
    }

    m_oStr->StringCopy(szTemp, id1, STRING_NORMAL, m_hInst);
    m_oStr->StringCopy(szText, szTemp);
    m_oStr->StringCopy(szTemp, id2, STRING_NORMAL, m_hInst);
    m_oStr->Concatenate(szText, _T(" "));
    m_oStr->Concatenate(szText, szTemp);
    m_oStr->Concatenate(szText, _T(" ("));
    m_oStr->IntToString(szTemp, m_eEngine->GetGameCount((EnumSudokuType)iType, (EnumDifficulty)iDiff));
    m_oStr->Concatenate(szText, szTemp);
    m_oStr->Concatenate(szText, _T(")"));

    static SYSTEMTIME sTime;
    ZeroMemory(&sTime, sizeof(SYSTEMTIME));

    COLORREF crClr = 0x888888;//grey out the unset times

    if(m_eEngine)
    {
        iTime = m_eEngine->GetBestTime((EnumSudokuType)iType, (EnumDifficulty)iDiff);
        sTime.wMinute = iTime/60;
        sTime.wSecond = iTime%60;

        if(iTime != DEFAULT_TIME)
            crClr = 0xffffff;
    }
    else
    {
        sTime.wMinute = 59;
        sTime.wSecond = 59;
    }

    ::DrawText(gdi.GetDC(), szText, rcTemp, DT_LEFT | DT_VCENTER, m_hFontText, 0xDEDFDE);

    GetTimeFormat(LOCALE_USER_DEFAULT, 0, &sTime, _T("mm':'ss"), szText, STRING_LARGE);
    while(szText[0] == _T('0'))
        m_oStr->Delete(0, 1, szText);
    ::DrawText(gdi.GetDC(), szText, rcTemp, DT_RIGHT | DT_VCENTER, m_hFontText, crClr);
}

void CDlgStats::DeleteListItem(LPVOID lpItem)
{

}