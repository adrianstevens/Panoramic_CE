#include "DlgMainMenu.h"
#include "DlgAbout.h"
//#include "DlgOptions.h"
#include "resource.h"
#include "IssGDIFx.h"
#include "stdafx.h"
#include "DlgMsgBox.h"
#include "IssCommon.h"
#include "IssLocalisation.h"
#include "DlgMoreApps.h"

#define IDMENU_Sudoku   5000
#define IDMENU_Killer   5001
#define IDMENU_Kakuro   5002
#define IDMENU_KenKen   5003
#define IDMENU_Greater  5004

#define IDMENU_New      5010
#define IDMENU_Custom   5011

#define IDMENU_5x5      5020
#define IDMENU_7x7      5021
#define IDMENU_9x9      5022

#define IDMENU_VeryEasy 5030
#define IDMENU_Easy     5031
#define IDMENU_Medium   5032
#define IDMENU_Hard     5033
#define IDMENU_VeryHard 5034
#define IDMENU_Genius   5035
#define IDMENU_Blank    5036


extern CObjGui* g_gui;

CIssKineticList*    CDlgMainMenu::m_oMenu = NULL;
BOOL                CDlgMainMenu::m_bPreventDeleteList = FALSE;           // Should we not delete the list for faster loading
DWORD               CDlgMainMenu::m_dwFullListCount = 0;

CDlgMainMenu::CDlgMainMenu(void)
:m_oGame(NULL)
,m_eState(MSTATE_SelectGame)
,m_hFontMenu(NULL)
,m_sOptions(NULL)
,m_bDrawSelector(FALSE)
{
    if(!m_oMenu)
        m_oMenu = new CIssKineticList;
    m_hFontMenu = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*17/32, FW_BOLD, TRUE);
}

CDlgMainMenu::~CDlgMainMenu(void)
{
    CIssGDIEx::DeleteFont(m_hFontMenu);

    if(m_oMenu)
        delete m_oMenu;
    m_oMenu = NULL;
}

void CDlgMainMenu::Init(CSEngine* oGame,
                        TypeOptions& sOptions)
{
    m_oGame         = oGame;
    m_sOptions      = &sOptions;
    m_bIsGameInPlay = !m_oGame->IsGameComplete();
    
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

    //if(g_gui)
    //    g_gui->Init(hWnd, m_hInst);

    //list options ... gonna keep it pretty minimal
    DWORD dwFlags   = OPTION_CircularList;

    // do other menu initialization here
    m_oMenu->Initialize(hWnd, hWnd, m_hInst, dwFlags, TRUE);

    if(m_oMenu->GetItemCount() == 0)
    {
        m_oMenu->SetSelectedItemIndex(0, TRUE);
        m_oMenu->SetCustomDrawFunc(DrawListItem, this);
        m_oMenu->SetDeleteItemFunc(DeleteListItem);
        m_oMenu->ResetContent();
    }

    m_oMenu->ResetContent();
    m_eState = MSTATE_SelectGame;
    PopulateList();

	m_bFirstTime = TRUE;

	return TRUE;
}


void CDlgMainMenu::LoadImages()
{

}

BOOL CDlgMainMenu::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    g_gui->InitGDI(rcClient, hDC);
    LoadImages();

	RECT rcClip;
	GetClipBox(hDC, &rcClip);

    Draw(g_gui->gdiMem, rcClip, g_gui->gdiTitle, rcClip.left, rcClip.top);

  //  DrawButtons(g_gui->gdiMem, rcClip);
    m_oMenu->OnDraw(g_gui->gdiMem, rcClient, rcClip);

    TCHAR szMenu[STRING_NORMAL];
    TCHAR szTemp[STRING_NORMAL];
    m_oStr->StringCopy(szMenu, ID(IDS_MENU_Menu), STRING_NORMAL, m_hInst);

    if(m_eState == MSTATE_SelectGame)
    {
        if(IsNewGame() == TRUE)
        {
            m_oStr->StringCopy(szTemp, ID(IDS_MENU_NewGame), STRING_NORMAL, m_hInst);
            g_gui->DrawBottomBar(g_gui->gdiMem, rcClip, szTemp, szMenu);

        }
        else
        {
            m_oStr->StringCopy(szTemp, ID(IDS_MENU_Resume), STRING_NORMAL, m_hInst);
            g_gui->DrawBottomBar(g_gui->gdiMem, rcClip, szTemp, szMenu);
        }
    }
    else
    {
        m_oStr->StringCopy(szTemp, ID(IDS_MENU_Back), STRING_NORMAL, m_hInst);
        g_gui->DrawBottomBar(g_gui->gdiMem, rcClip, szTemp, szMenu);
    }

    

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
    return m_oMenu->OnTimer(wParam, lParam);
}

BOOL CDlgMainMenu::HandleMenu(WPARAM wParam)
{
    DWORD dwMenu = LOWORD(wParam);

    if(m_oGame == NULL)
        return FALSE;

    switch(m_eState)
    {
    case MSTATE_SelectGame:
        switch(dwMenu)
        {
        case IDMENU_Sudoku:
            m_oGame->SetGameType(STYPE_Sudoku);
            m_eState = MSTATE_Difficulty;
            //m_eState = MSTATE_NewOrCustom;
        	break;
        case IDMENU_Kakuro:
            m_oGame->SetGameType(STYPE_Kakuro);
            m_eState = MSTATE_Size;
        	break;
        case IDMENU_Killer:
            m_oGame->SetGameType(STYPE_Killer);
            m_eState = MSTATE_Difficulty;
            break;
        case IDMENU_KenKen:
            m_oGame->SetGameType(STYPE_KenKen);
            m_eState = MSTATE_Difficulty;
            break;
        case IDMENU_Greater:
            m_oGame->SetGameType(STYPE_Greater);
            m_eState = MSTATE_Difficulty;
            break;
        default:
            return FALSE;
            break;
        }
	    break;
    case MSTATE_NewOrCustom:
        if(m_oGame->GetGameType() != STYPE_Sudoku)
            return FALSE;//just incase
        switch(dwMenu)
        {
        case IDMENU_New:
        case IDMENU_Custom:
            m_eState = MSTATE_Difficulty;
            break;
        default:
            return FALSE;
            break;
        }
	    break;
    case MSTATE_Size:
        if(m_oGame->GetGameType() != STYPE_Kakuro)
            return FALSE;//just incase
        switch(dwMenu)
        {
        case IDMENU_5x5:
            m_oGame->SetBoardSize(KSIZE_5);
        	break;
        case IDMENU_7x7:
            m_oGame->SetBoardSize(KSIZE_7);
            break;
        case IDMENU_9x9:
            m_oGame->SetBoardSize(KSIZE_9);
            break;
        default:
            return FALSE;
            break;
        }
        m_eState = MSTATE_Difficulty;
        break;
    case MSTATE_Difficulty:
        switch(dwMenu)
        {
        case IDMENU_VeryEasy:
            m_oGame->SetDifficulty(DIFF_VeryEasy);
        	break;
        case IDMENU_Easy:
            m_oGame->SetDifficulty(DIFF_Easy);
        	break;
        case IDMENU_Medium:
            m_oGame->SetDifficulty(DIFF_Medium);
            break;
        case IDMENU_Hard:
            m_oGame->SetDifficulty(DIFF_Hard);
            break;
        case IDMENU_VeryHard:
            m_oGame->SetDifficulty(DIFF_VeryHard);
            break;
        case IDMENU_Genius:
            m_oGame->SetDifficulty(DIFF_Hardcore);
            break;
        default:
            return FALSE;
            break;
        }
        OnNewGame();
        return TRUE;
        break;
    default:
        return FALSE;
        break;
    }

    //gotta change the menu 
    m_bIsGameInPlay = FALSE;//once you've touched the menu you can't resume
    PopulateList();
    InvalidateRect(m_hWnd, NULL, FALSE);
    return TRUE;
}

void CDlgMainMenu::OnResume()
{
    SafeCloseWindow(IDMENU_Resume);	// let the main window handle this
}

void CDlgMainMenu::OnNewGame()
{
    SafeCloseWindow(IDMENU_NewGame);	// let the main window handle this


}


BOOL CDlgMainMenu::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(HandleMenu(wParam) == TRUE)
        return TRUE;

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
	case IDMENU_Exit:
	{
		CDlgMsgBox dlgMsg;
		if(IDYES != dlgMsg.PopupMessage(ID(IDS_MSG_Exit), m_hWnd, m_hInst, MB_YESNO))
			break;
        PostQuitMessage(0);
        break;
	}
	case IDMENU_MoreApps:
		{
			CDlgMoreApps dlgMore;
			dlgMore.Launch(m_hInst, m_hWnd);
		}
		break;
    case IDMENU_NewGame:
        OnNewGame();
        break;
	case IDMENU_Resume:
        OnResume();
        break;
    case IDMENU_Left:
        OnLeftMenu();        
		break;
    case IDMENU_Right:
        OnRightMenu();
        break;   
	case IDOK://return pressed on menu ... gotta  get the index


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
	case LANG_PORTUGUESE:
		oStr->StringCopy(szHelpFile, _T("PanoSudoku_pt.htm"));
		break;
    case LANG_SPANISH:
        oStr->StringCopy(szHelpFile, _T("PanoSudoku_es.htm"));
        break;
    case LANG_FRENCH:
        oStr->StringCopy(szHelpFile, _T("PanoSudoku_fr.htm"));
        break;
    case LANG_GERMAN:
        oStr->StringCopy(szHelpFile, _T("PanoSudoku_de.htm"));
        break;
    case LANG_DUTCH:
        oStr->StringCopy(szHelpFile, _T("PanoSudoku_du.htm"));
        break;
    case LANG_JAPANESE:
        oStr->StringCopy(szHelpFile, _T("PanoSudoku_jp.htm"));
        break;
	default:
		oStr->StringCopy(szHelpFile, _T("PanoSudoku.htm"));
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
    //calc some rects
    static int iWWidth = 0;
    static int iWHeight = 0;

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    if(WIDTH(rcClient) == iWWidth &&
        HEIGHT(rcClient) == iWHeight)
        return TRUE;

    if(WIDTH(rcClient) != GetSystemMetrics(SM_CXSCREEN) &&
        HEIGHT(rcClient) != GetSystemMetrics(SM_CYSCREEN))
        return TRUE;

    iWWidth = WIDTH(rcClient);
    iWHeight = HEIGHT(rcClient);

    int iTop = (IsVGA()?268:134);
    int iLeft = (IsVGA()?22:11);
      
    if(m_oMenu)
        m_oMenu->OnSize(iLeft,iTop, WIDTH(rcClient)-iLeft, HEIGHT(rcClient) - iTop);

    g_gui->OnSize(hWnd, rcClient);

    return TRUE;
}

BOOL CDlgMainMenu::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
//    case VK_RETURN:
        m_bDrawSelector = TRUE;
        m_oMenu->OnKeyDown(wParam, lParam);
        InvalidateRect(m_hWnd, NULL, FALSE);
        return TRUE;
    	break;
    }
	return UNHANDLED;
}

BOOL CDlgMainMenu::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case VK_ESCAPE:
    case VK_TSOFT1:
        OnLeftMenu();
        break;
    case VK_TSOFT2:
        OnRightMenu();
        break;
    case VK_RETURN:
        m_bDrawSelector = TRUE;
        m_oMenu->OnKeyDown(wParam, lParam);
        InvalidateRect(m_hWnd, NULL, FALSE);
        return TRUE;
    default:
        return UNHANDLED;        
        break;
    }
	return TRUE;
}

BOOL CDlgMainMenu::OnLButtonDown(HWND hWnd, POINT& pt)
{
    g_gui->OnLButtonDown(hWnd, pt);

    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(!m_oMenu)
        return UNHANDLED;
    return m_oMenu->OnLButtonDown(pt);
}

BOOL CDlgMainMenu::OnLButtonUp(HWND hWnd, POINT& pt)
{
    m_bDrawSelector = FALSE;
    g_gui->OnLButtonUp(hWnd, pt);

    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(!m_oMenu)
        return UNHANDLED;
    return m_oMenu->OnLButtonUp(pt);
}

BOOL CDlgMainMenu::OnMouseMove(HWND hWnd, POINT& pt)
{
    if(g_gui->wndMenu.IsWindowUp(TRUE))
        return TRUE;

    if(!m_oMenu)
        return UNHANDLED;
    return m_oMenu->OnMouseMove(pt);
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

    g_gui->wndMenu.AddItem(ID(IDS_MENU_NewGame), m_hInst, IDMENU_NewGame);
	g_gui->wndMenu.AddSeparator();

#ifndef NMA
	g_gui->wndMenu.AddItem(ID(IDS_MENU_MoreApps), m_hInst, IDMENU_MoreApps);
#endif
        
    g_gui->wndMenu.AddItem(ID(IDS_MENU_Options), m_hInst, IDMENU_Options);
    g_gui->wndMenu.AddItem(ID(IDS_MENU_About), m_hInst, IDMENU_About);
    g_gui->wndMenu.AddItem(ID(IDS_MENU_Help), m_hInst, IDMENU_Help);
	g_gui->wndMenu.AddSeparator();
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

void CDlgMainMenu::ShowAboutScreen()
{
	CDlgAbout dlgAbout;
	dlgAbout.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	m_bFirstTime = TRUE;
}

void CDlgMainMenu::ShowOptionsScreen()
{
	CDlgOptions dlgOptions;
    m_sOptions->eSkin = g_gui->sSkin.eSkin;//make sure we're in sync
    m_sOptions->eBackground = g_gui->sSkin.eBackground;
    m_sOptions->bShowHints = m_oGame->GetShowHints();//again .. stay in sync

    dlgOptions.Init(m_hInst, *m_sOptions);
    if(IDOK == dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
    {
        //just copy the whole damn thing
        memcpy(m_sOptions, &dlgOptions.m_sOptions, sizeof(TypeOptions));
        m_oGame->SetShowHints(m_sOptions->bShowHints);
        m_oGame->SetShowLocked(m_sOptions->bShowComplete);
        
        g_gui->gdiBackground.Destroy();//lets just do this every time
        g_gui->SetImageBackground(m_sOptions->szPath);
        g_gui->LoadSkin(m_sOptions->eSkin, m_sOptions->eBackground);
    }
}


void CDlgMainMenu::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted)
{
    if(!sItem)
        return;

    TCHAR szText[STRING_NORMAL];
    TCHAR szTemp[STRING_NORMAL];


    switch(sItem->uiMenuID)
    {
    case IDMENU_Sudoku:
        m_oStr->StringCopy(szTemp, ID(IDS_Sudoku), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_Killer: 
        m_oStr->StringCopy(szTemp, ID(IDS_KillerSudoku), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_Kakuro:  
        m_oStr->StringCopy(szTemp, ID(IDS_Kakuro), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_KenKen:
        m_oStr->StringCopy(szTemp, ID(IDS_KenKen), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_Greater:
        m_oStr->StringCopy(szTemp, ID(IDS_Greater), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;;
    case IDMENU_New: 
        m_oStr->StringCopy(szTemp, ID(IDS_MENU_NewGame), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_Custom:  
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Custom), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;

    case IDMENU_5x5:  
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_5x5), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_7x7:     
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_7x7), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_9x9:  
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_9x9), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;

    case IDMENU_VeryEasy:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_VeryEasy), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_Easy:    
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Easy), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_Medium:  
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Normal), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_Hard:   
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Hard), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_VeryHard:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Expert), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    case IDMENU_Genius:
        m_oStr->StringCopy(szTemp, ID(IDS_PREF_Genius), STRING_NORMAL, m_hInst);
        m_oStr->StringCopy(szText, szTemp);
        break;
    }

 //   AlphaFillRect(gdi, rcDraw, 0xFFFFFF, 50);
 //   ::DrawTextShadow(gdi.GetDC(), szText, rcDraw, DT_CENTER | DT_VCENTER, m_hFontMenu, 0xFFFFFF, 0);
    ::DrawText(gdi.GetDC(), szText, rcDraw, DT_CENTER | DT_VCENTER, m_hFontMenu, RGB(32,29,21));

    if(bIsHighlighted && m_bDrawSelector)
    {
        rcDraw.left += GetSystemMetrics(SM_CXICON);
        rcDraw.right -= GetSystemMetrics(SM_CXICON);
        FrameRect(gdi.GetDC(), rcDraw, RGB(32,29,21));
    }

}

void CDlgMainMenu::DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass)
{
    CDlgMainMenu* pList = (CDlgMainMenu*)lpClass;
    pList->DrawListItem(gdi, rcDraw, sItem, bIsHighlighted);
}


void CDlgMainMenu::DeleteListItem(LPVOID lpItem)
{

}



void CDlgMainMenu::PopulateList()
{
    if(!m_oMenu)
        return;

    m_oMenu->ResetContent();

    switch(m_eState)
    {
    case MSTATE_SelectGame:
        m_oMenu->AddItem(LPVOID(IDMENU_Sudoku), IDMENU_Sudoku, 0, 0);
        m_oMenu->AddItem(LPVOID(IDMENU_Killer), IDMENU_Killer, 0, 0);
        m_oMenu->AddItem(LPVOID(IDMENU_Kakuro), IDMENU_Kakuro, 0, 0);
        m_oMenu->AddItem(LPVOID(IDMENU_KenKen), IDMENU_KenKen, 0, 0);
        m_oMenu->AddItem(LPVOID(IDMENU_Greater), IDMENU_Greater, 0, 0);
        break;
    case MSTATE_NewOrCustom:
        m_oMenu->AddItem(LPVOID(IDMENU_New), IDMENU_New, 0, 0);
        m_oMenu->AddItem(LPVOID(IDMENU_Custom), IDMENU_Custom, 0, 0);
        break;
    case MSTATE_Size:
        m_oMenu->AddItem(LPVOID(IDMENU_5x5), IDMENU_5x5, 0, 0);
        m_oMenu->AddItem(LPVOID(IDMENU_7x7), IDMENU_7x7, 0, 0);
        m_oMenu->AddItem(LPVOID(IDMENU_9x9), IDMENU_9x9, 0, 0);
        break;
    case MSTATE_Difficulty:
        m_oMenu->AddItem(LPVOID(IDMENU_VeryEasy), IDMENU_VeryEasy, 0, 0);
        m_oMenu->AddItem(LPVOID(IDMENU_Easy), IDMENU_Easy, 0, 0);
        m_oMenu->AddItem(LPVOID(IDMENU_Medium), IDMENU_Medium, 0, 0);
        m_oMenu->AddItem(LPVOID(IDMENU_Hard), IDMENU_Hard, 0, 0);
        m_oMenu->AddItem(LPVOID(IDMENU_VeryHard), IDMENU_VeryHard, 0, 0);

        if(m_oGame && m_oGame->GetGameType() == STYPE_Sudoku)
            m_oMenu->AddItem(LPVOID(IDMENU_Genius), IDMENU_Genius, 0, 0);
        break;
    default:
        break;
    }

    if(GetSystemMetrics(SM_CYSCREEN)%320 == 0)
        //m_oMenu->SetItemHeights((IsVGA()?64:32), (IsVGA()?64:32));
        m_oMenu->SetItemHeights((IsVGA()?52:26), (IsVGA()?52:26));
    else if(GetSystemMetrics(SM_CYSCREEN)%400 == 0)
        m_oMenu->SetItemHeights((IsVGA()?70:35), (IsVGA()?70:35));
    else
        m_oMenu->SetItemHeights((IsVGA()?34:17), (IsVGA()?34:17));
    

}

void CDlgMainMenu::OnBack()
{
    switch(m_eState)
    {
    case MSTATE_SelectGame:
        break;
    case MSTATE_Difficulty:
        if(m_oGame && m_oGame->GetGameType() == STYPE_Kakuro)
        {
            m_eState = MSTATE_Size;
            PopulateList();
            break;
        }
    case MSTATE_NewOrCustom:
    case MSTATE_Size:    
        m_eState = MSTATE_SelectGame;
        PopulateList();
        break;
    default:
        break;
    }

    InvalidateRect(m_hWnd, NULL, FALSE);
}

void CDlgMainMenu::OnLeftMenu()
{
    if(m_eState == MSTATE_SelectGame)
    {
        if(IsNewGame())
            OnNewGame();
        else
            OnResume();
    }
    else
    {
        OnBack();
    }
}

void CDlgMainMenu::OnRightMenu()
{
    ShowMenu();
}

BOOL CDlgMainMenu::IsNewGame()
{
    if(m_bIsGameInPlay == FALSE || m_oGame->IsGameComplete() == TRUE)
        return TRUE;
    return FALSE;



}