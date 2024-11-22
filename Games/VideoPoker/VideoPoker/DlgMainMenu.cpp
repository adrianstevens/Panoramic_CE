#include "DlgMainMenu.h"
#include "DlgAbout.h"
//#include "DlgOptions.h"
#include "resource.h"
#include "IssGDIFx.h"
#include "stdafx.h"
#include "DlgMsgBox.h"
#include "IssCommon.h"


extern CObjGui* g_gui;

CDlgMainMenu::CDlgMainMenu(void)
{

}

CDlgMainMenu::~CDlgMainMenu(void)
{

}

void CDlgMainMenu::Init(BOOL bIsGameInPlay, 
                        BOOL bPlaysounds, 
                        BOOL bAutoHold,
                        BOOL bQuickPlay,
                        EnumBackgroundColor eBG, 
                        EnumBetAmounts eBets, 
                        EnumMultiPlay eMulti)
{
    m_bIsGameInPlay = bIsGameInPlay;
    m_bPlaySounds = bPlaysounds;
    m_eBGColor = eBG;
    m_eBetAmount = eBets;
    m_eMultiplay = eMulti;
    m_bQuickPlay = bQuickPlay;
    m_bAutoHold = m_bAutoHold;
}

BOOL CDlgMainMenu::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);
#endif

    g_gui->Init(hWnd, m_hInst);

	m_bFirstTime = TRUE;

	return TRUE;
}


void CDlgMainMenu::LoadImages()
{
    if(m_gdiTitle.GetDC() == NULL)
    {
        CIssGDIEx gdiTemp;
        gdiTemp.LoadImage(IsVGA()?IDB_PNG_Title_HR:IDB_PNG_Title, m_hWnd, m_hInst);

        RECT rcTemp;
        SetRect(&rcTemp, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        m_gdiTitle.Create(gdiTemp.GetDC(), rcTemp);

        int iXOffSet = (gdiTemp.GetWidth() - WIDTH(rcTemp))/2;

        Draw(m_gdiTitle, rcTemp, gdiTemp, iXOffSet, 0);
    }
}

BOOL CDlgMainMenu::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    g_gui->InitGDI(rcClient, hDC);
    LoadImages();

	RECT rcClip;
	GetClipBox(hDC, &rcClip);

    Draw(g_gui->gdiMem, rcClip, m_gdiTitle, rcClip.left, rcClip.top);

    if(m_bIsGameInPlay == FALSE)
        g_gui->DrawBottomBar(g_gui->gdiMem, rcClip, _T("New Game"), _T("Menu"));
    else
        g_gui->DrawBottomBar(g_gui->gdiMem, rcClip, _T("Resume"), _T("Menu"));

	if(m_bFirstTime)
	{
		FadeInScreen(hDC, rcClient);
		m_bFirstTime = FALSE;
	}

	// draw all to the screen
    g_gui->DrawScreen(rcClip, hDC);

	return TRUE;
}

BOOL CDlgMainMenu::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}

BOOL CDlgMainMenu::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
	{
   	case IDMENU_About:
		ShowAboutScreen();
		break;
	case IDMENU_Options:
		ShowOptionsScreen();
		break;
	case IDMENU_Help:
#ifdef WIN32_PLATFORM_PSPC
        CreateProcess(_T("peghelp"), _T("PanoVideoPoker.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
#else
        LaunchHelp( _T("PanoBlackjack.htm"), m_hInst);
#endif
		break;
	case IDMENU_Exit:
	{
		CDlgMsgBox dlgMsg;
		if(IDYES != dlgMsg.PopupMessage(IDS_MSG_Exit, m_hWnd, m_hInst, MB_YESNO))
			break;
	}
    case IDMENU_NewGame:
	case IDMENU_Resume:
        SafeCloseWindow(LOWORD(wParam));	// let the main window handle this
        break;
    case IDMENU_Left:
        if(m_bIsGameInPlay)
            SafeCloseWindow(IDMENU_Resume);	// let the main window handle this
        else
            SafeCloseWindow(IDMENU_NewGame);	// let the main window handle this
		break;
    case IDMENU_Right:
        ShowMenu();
        break;   
	case IDOK://return pressed on menu ... gotta  get the index


		break;
    default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgMainMenu::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgMainMenu::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CDlgMainMenu::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case VK_TSOFT1:
        SafeCloseWindow(m_bIsGameInPlay?IDMENU_Resume:IDMENU_NewGame);
        break;
    case VK_TSOFT2:
        ShowMenu();
        break;
    case VK_ESCAPE:
        SafeCloseWindow(0);
        break;

    }
	return TRUE;
}

BOOL CDlgMainMenu::OnLButtonDown(HWND hWnd, POINT& pt)
{
    g_gui->OnLButtonDown(hWnd, pt);

    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    return UNHANDLED;
}

BOOL CDlgMainMenu::OnLButtonUp(HWND hWnd, POINT& pt)
{
    g_gui->OnLButtonUp(hWnd, pt);

    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    return UNHANDLED;
}

BOOL CDlgMainMenu::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    return UNHANDLED;
}

BOOL CDlgMainMenu::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
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


BOOL CDlgMainMenu::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{

	return TRUE;
}

BOOL CDlgMainMenu::FadeInScreen(HDC hDC, RECT rcClient)
{
	FadeIn(hDC, g_gui->gdiMem, 900, fnAnimateIn, this);
	return TRUE;
}



void CDlgMainMenu::fnAnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal)
{
	CDlgMainMenu* lpThis = (CDlgMainMenu*)lpClass;
	lpThis->AnimateIn(gdiDest, rcClient, iAnimeStep, iAnimTotal);
}

void CDlgMainMenu::AnimateIn(CIssGDIEx& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal)
{

	BitBlt(gdiDest.GetDC(),
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		g_gui->gdiMem.GetDC(),
		0,0,
		SRCCOPY);
}

void CDlgMainMenu::fnAnimateOut(CIssGDIEx& gdiDest, RECT& rcClient, LPVOID lpClass, int iAnimeStep, int iAnimTotal)
{
	CDlgMainMenu* lpThis = (CDlgMainMenu*)lpClass;
	lpThis->AnimateOut(gdiDest, rcClient, iAnimeStep, iAnimTotal);
}

void CDlgMainMenu::AnimateOut(CIssGDIEx& gdiDest, RECT& rcClient, int iAnimeStep, int iAnimTotal)
{

	BitBlt(gdiDest.GetDC(),
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		g_gui->gdiMem.GetDC(),
		0,0,
		SRCCOPY);

}

BOOL CDlgMainMenu::AnimateOutScreen()
{
/*	HDC hDC = GetDC(m_hWnd);
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	BitBlt(m_gdiMem.GetDC(),
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiImage.GetDC(),
		0,0,
		SRCCOPY);

	FadeIn(hDC, m_gdiMem, 900, fnAnimateOut, this);
	ReleaseDC(m_hWnd, hDC);*/
	return TRUE;
}

void CDlgMainMenu::ShowMenu()
{
    g_gui->wndMenu.ResetContent();

    g_gui->wndMenu.AddItem(IDS_MENU_NewGame, m_hInst, IDMENU_NewGame);

        
    g_gui->wndMenu.AddItem(IDS_MENU_Options, m_hInst, IDMENU_Options);
    g_gui->wndMenu.AddItem(IDS_MENU_About, m_hInst, IDMENU_About);
    g_gui->wndMenu.AddItem(IDS_MENU_Help, m_hInst, IDMENU_Help);
    g_gui->wndMenu.AddItem(IDS_MENU_Exit, m_hInst, IDMENU_Exit);

    g_gui->wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.right/4,rcClient.top,
        rcClient.right*3/4, HEIGHT(rcClient) - HEIGHT(g_gui->rcBottomBar),
        0,0,0,0, ADJUST_Bottom);

    return;
}

void CDlgMainMenu::ShowAboutScreen()
{
	CDlgAbout dlgAbout;
	dlgAbout.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	m_bFirstTime = TRUE;
}

void CDlgMainMenu::ShowOptionsScreen()
{
	CDlgOptions dlgOptions;

    dlgOptions.Init(m_hInst, m_bPlaySounds, m_bAutoHold, m_bQuickPlay, m_eBGColor, m_eBetAmount, m_eMultiplay);
	if(IDOK == dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
    {
        m_eBGColor = dlgOptions.m_eBGColor;
        m_bPlaySounds = dlgOptions.m_bPlaySounds;
        m_eBetAmount = dlgOptions.m_eBet;
    }
	m_bFirstTime = TRUE;
}

