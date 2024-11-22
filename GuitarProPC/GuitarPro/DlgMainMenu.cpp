#include "DlgMainMenu.h"
#include "DlgAbout.h"
//#include "DlgOptions.h"

#include "resource.h"
#include "IssGDIFx.h"
#include "stdafx.h"
#include "DlgMsgBox.h"
#include "IssCommon.h"

#define	WM_MENU_Button		WM_USER+100
#define TEXT_INDENT         (GetSystemMetrics(SM_CXICON)/4) 

extern CObjGui* g_gui;

CDlgMainMenu::CDlgMainMenu(void)
{
}

CDlgMainMenu::~CDlgMainMenu(void)
{

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

BOOL CDlgMainMenu::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    g_gui->InitGDI(rcClient, hDC);

	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, TRUE);

    if(m_bFirstTime)
    {
        FadeInScreen(hDC, rcClient);
        m_bFirstTime = FALSE;
    }
    
    RECT rcClip;
	GetClipBox(hDC, &rcClip);

    DrawBackground(rcClip);

    if(m_bIsGameInPlay == FALSE)
        g_gui->DrawBottomBar(m_gdiMem, rcClip, _T("New Game"), _T("Menu"));
    else
        g_gui->DrawBottomBar(m_gdiMem, rcClip, _T("Resume"), _T("Menu"));

	// draw all to the screen
	BitBlt(hDC,
		rcClip.left,rcClip.top,
		WIDTH(rcClip), HEIGHT(rcClip),
		m_gdiMem.GetDC(),
		rcClip.left,rcClip.top,
    	SRCCOPY);

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
        CreateProcess(_T("peghelp"), _T("PanoGuitarPro.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
#else
        LaunchHelp( _T("PanoGuitarPro.htm"), m_hInst);
#endif
		break;
	case IDMENU_Exit:
	{
		CDlgMsgBox dlgMsg;
		if(IDYES != dlgMsg.PopupMessage(IDS_MSG_Exit, m_hWnd, m_hInst, MB_YESNO))
			break;
	}
    case IDMENU_Left:
		SafeCloseWindow(LOWORD(wParam));	// let the main window handle this
		break;
    case IDMENU_Right:
        ShowMenu();
        break;    
    default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CDlgMainMenu::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_gdiImage.Destroy();
	m_gdiMem.Destroy();
	
	return TRUE;
}

BOOL CDlgMainMenu::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
//	g_gui->wndMenu.OnKeyDown(hWnd, wParam, lParam);
	return TRUE;
}

BOOL CDlgMainMenu::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case VK_TSOFT1:
        SafeCloseWindow(0);
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

	return TRUE;
}

BOOL CDlgMainMenu::OnLButtonUp(HWND hWnd, POINT& pt)
{
    g_gui->OnLButtonUp(hWnd, pt);

    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;
	
	return TRUE;
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
	if(uiMessage == WM_MENU_Button + BTN_Menu)
		ShowMenu();
	/*else if(uiMessage == WM_MENU_Button + BTN_Resume)
	{}*/
	else if(uiMessage == WM_MENU_Button + BTN_NewGame)
	{}
/*	else if(uiMessage == WM_MENU_Button + BTN_Quit)
	{}*/

	return TRUE;
}

BOOL CDlgMainMenu::FadeInScreen(HDC hDC, RECT rcClient)
{
	InitBackground();
//	InitButtons();
	FadeIn(hDC, m_gdiMem, 900, fnAnimateIn, this);
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
		m_gdiImage.GetDC(),
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
		m_gdiImage.GetDC(),
		0,0,
		SRCCOPY);
}

BOOL CDlgMainMenu::AnimateOutScreen()
{
	HDC hDC = GetDC(m_hWnd);
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	BitBlt(m_gdiMem.GetDC(),
		0,0,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiImage.GetDC(),
		0,0,
		SRCCOPY);

	FadeIn(hDC, m_gdiMem, 900, fnAnimateOut, this);
	ReleaseDC(m_hWnd, hDC);
	return TRUE;
}

void CDlgMainMenu::ShowMenu()
{
    g_gui->wndMenu.ResetContent();

        
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

void CDlgMainMenu::InitBackground()
{
	if(!m_gdiImage.GetDC())
	{
		CIssGDIEx gdiBackground;

		int iWidth		= GetSystemMetrics(SM_CXSCREEN);
		int iHeight		= GetSystemMetrics(SM_CYSCREEN);

		if(GetSystemMetrics(SM_CXICON) > 44)
			//LoadImage(gdiBackground, IDR_SPLASH_VGA, m_hWnd, m_hInst);
            gdiBackground.LoadImage(IDR_PNG_TitleVGA, m_hWnd, m_hInst, FALSE);
		else
			//LoadImage(gdiBackground, IDR_SPLASH, m_hWnd, m_hInst);
            gdiBackground.LoadImage(IDR_PNG_Title, m_hWnd, m_hInst, FALSE);

		m_gdiImage.Create(m_gdiMem.GetDC(), iWidth, iHeight, FALSE, TRUE);

		BitBlt(m_gdiImage.GetDC(),
			0,0,
			iWidth, iHeight,
			gdiBackground.GetDC(),
			0, 0,
			SRCCOPY);
	}
}

void CDlgMainMenu::DrawBackground(RECT rcLocation)
{
	InitBackground();

	// copy the background over
	BitBlt(m_gdiMem.GetDC(),
		rcLocation.left,rcLocation.top,
		WIDTH(rcLocation), HEIGHT(rcLocation),
		m_gdiImage.GetDC(),
		rcLocation.left,rcLocation.top,
		SRCCOPY);
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
	dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	m_bFirstTime = TRUE;
}

void CDlgMainMenu::ShowHighScoreScreen()
{

	m_bFirstTime = TRUE;
}



