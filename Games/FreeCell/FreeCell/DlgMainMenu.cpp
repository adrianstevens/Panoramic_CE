#include "DlgMainMenu.h"
#include "DlgAbout.h"
#include "DlgMoreApps.h"
//#include "DlgOptions.h"

#include "resource.h"
#include "IssGDIFx.h"
#include "stdafx.h"
#include "DlgMsgBox.h"
#include "IssCommon.h"
#include "IssLocalisation.h"


#define MENU_BTNS_Grad1		RGB(204,225,255)
#define MENU_BTNS_Grad2		RGB(145,173,209)
#define MENU_BTNS_Outline1	RGB(13,50,90)
#define MENU_BTNS_Outline2	RGB(255,255,255)
#define MENU_BTNS_TextColor	RGB(13,50,90)
#define	WM_MENU_Button		WM_USER+100
#define TEXT_INDENT         (GetSystemMetrics(SM_CXICON)/4) 

extern CObjGui* g_gui;

CDlgMainMenu::CDlgMainMenu(void)
:m_oGame(NULL)
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

    //set the color on first load
    if(m_oGame)
        g_gui->m_eBackground = m_oGame->GetBackgroundColor();
    
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

    TCHAR szMenu[STRING_NORMAL];
    m_oStr->StringCopy(szMenu, ID(IDS_MENU_Menu), STRING_NORMAL, m_hInst);

    if(m_bIsGameInPlay == FALSE)
        g_gui->DrawBottomBar(m_gdiMem, rcClip, m_oStr->GetText(ID(IDS_MENU_NewGame), m_hInst), szMenu);
    else
        g_gui->DrawBottomBar(m_gdiMem, rcClip, m_oStr->GetText(ID(IDS_MENU_Resume), m_hInst), szMenu);

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
		LaunchHelp(m_hWnd, m_hInst);
		break;
	case IDMENU_MoreApps:
		{
			CDlgMoreApps dlgMoreApps;
			dlgMoreApps.Launch(m_hInst, m_hWnd);
		}
		break;
	case IDMENU_Exit:
	{
		CDlgMsgBox dlgMsg;
		if(IDYES != dlgMsg.PopupMessage(ID(IDS_MSG_Exit), m_hWnd, m_hInst, MB_YESNO))
			break;
	}
	case IDMENU_NewGame:
	case IDMENU_Resume:
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

void CDlgMainMenu::LaunchHelp(HWND hWnd, HINSTANCE hInst)
{
	TCHAR szHelpFile[STRING_MAX];

	CIssString* oStr = CIssString::Instance();
	oStr->Empty(szHelpFile);

	ShowWindow(hWnd, SW_MINIMIZE);

	switch(g_cLocale.GetCurrentLanguage())
	{
	case LANG_KOREAN:
		oStr->StringCopy(szHelpFile, _T("PanoFreecell_ko.htm"));
		break;
	case LANG_PORTUGUESE:
		oStr->StringCopy(szHelpFile, _T("PanoFreecell_pt.htm"));
		break;
	case LANG_FRENCH:
		oStr->StringCopy(szHelpFile, _T("PanoFreecell_fr.htm"));
		break;
	case LANG_GERMAN:
		oStr->StringCopy(szHelpFile, _T("PanoFreecell_de.htm"));
		break;
	case LANG_DUTCH:
		oStr->StringCopy(szHelpFile, _T("PanoFreecell_du.htm"));
		break;
    case LANG_JAPANESE:
        oStr->StringCopy(szHelpFile, _T("PanoFreecell_jp.htm"));
        break;
	default:
		oStr->StringCopy(szHelpFile, _T("PanoFreecell.htm"));
		break;
	}

	if(IsSmartphone())
	{
		SHELLEXECUTEINFO shell;
		shell.cbSize		= sizeof(SHELLEXECUTEINFO);
		shell.lpVerb		= _T("OPEN"); 
		shell.fMask			= SEE_MASK_NOCLOSEPROCESS ;
		shell.hwnd			= NULL;
		shell.lpFile		= _T("iexplore.exe");
		shell.lpDirectory	= NULL;
		shell.hInstApp		= hInst;
		shell.nShow			= SW_SHOW;

		//TCHAR szTemp[STRING_MAX];
		//GetExeDirectory(szTemp);

		//oStr->Concatenate(szTemp, szHelpFile);
		oStr->Insert(szHelpFile, _T("\\Windows\\"));
		shell.lpParameters	= szHelpFile; 
		ShellExecuteEx(&shell);
	}
	else
	{
		CreateProcess(_T("peghelp"), szHelpFile, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
	}
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
        SafeCloseWindow(m_bIsGameInPlay?IDMENU_Resume:IDMENU_NewGame);
        break;
    case VK_TSOFT2:
        ShowMenu();
        break;
    //case VK_ESCAPE:
    //    SafeCloseWindow(0);
    //    break;
	case VK_TTALK:
	case VK_TBACK:
		if(IsSmartphone())
		{
			ShowWindow(hWnd, SW_MINIMIZE);
			if(LOWORD(wParam) == VK_TTALK)
			{
				// Simulate a key press
				keybd_event( VK_TTALK,
					0x45,
					KEYEVENTF_EXTENDEDKEY | 0,
					0 );

				// Simulate a key release
				keybd_event( VK_TTALK,
					0x45,
					KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
					0);
			}
		}
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

    g_gui->wndMenu.AddItem(ID(IDS_MENU_NewGame), m_hInst, IDMENU_NewGame);
	g_gui->wndMenu.AddSeparator();

#ifndef NMA
	g_gui->wndMenu.AddItem(ID(IDS_MENU_MoreApps), m_hInst, IDMENU_MoreApps);
#endif        
    g_gui->wndMenu.AddItem(ID(IDS_MENU_Options), m_hInst, IDMENU_Options);
    g_gui->wndMenu.AddItem(ID(IDS_MENU_About), m_hInst, IDMENU_About);
    if(g_gui->GetIsSmartPhone() == FALSE)
        g_gui->wndMenu.AddItem(ID(IDS_MENU_Help), m_hInst, IDMENU_Help);
    g_gui->wndMenu.AddItem(ID(IDS_MENU_Exit), m_hInst, IDMENU_Exit);

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
    dlgOptions.SetGame(m_oGame);
	dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	m_bFirstTime = TRUE;
}

void CDlgMainMenu::ShowHighScoreScreen()
{

	m_bFirstTime = TRUE;
}



