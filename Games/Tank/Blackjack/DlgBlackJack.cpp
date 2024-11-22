#include "StdAfx.h"
#include "DlgBlackJack.h"
#include "resource.h"
#include "IssRegistry.h"
#include "DlgOptions.h"
#include "IssCommon.h"
#include "resource.h"
#include "IssGDIFX.h"
#include "DlgSplashScreen.h"
#include "DlgMainMenu.h"
#include "DlgMsgBox.h"
#include "DlgAbout.h"
#include "DlgHighScores.h"
#include "DlgOptions.h"

#define CARD_SPACING (m_gdiCardBack.GetWidth()/4)


#define TEXT_COLOR RGB(255,255,255)
#define TEXT_SHADOW 0

#define IDT_GAME_TIMER 1234



#define REG_Diff	_T("Diff")
#define REG_PS		_T("PS")

CDlgBackJack::CDlgBackJack()
:m_bPlaySounds(TRUE)
,m_iYBtnStart(0)
,m_bShowSplashScreen(FALSE)
{
	// read values from the registry
	LoadRegSettings();

	ZeroMemory(&m_sizeWindow, sizeof(SIZE));
		
	m_oStr = CIssString::Instance();

}

CDlgBackJack::~CDlgBackJack(void)
{
	CIssGDIEx::DeleteFont(m_hfButtonText);
	CIssGDIEx::DeleteFont(m_hFontCount);
	CIssGDIEx::DeleteFont(m_hfScreenText);

	// write values to the registry
	SaveRegSettings();
}

void CDlgBackJack::Init(HINSTANCE hInst)
{
	m_hInst = hInst;

}

BOOL CDlgBackJack::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	// first check if we need to reload the images first
	if(m_sizeWindow.cx	== (rcClient.right - rcClient.left)&&
		m_sizeWindow.cy	== (rcClient.bottom - rcClient.top))
		return TRUE;

	// update our size structure
	m_sizeWindow.cx		= rcClient.right - rcClient.left;
	m_sizeWindow.cy		= rcClient.bottom - rcClient.top;

	// start the wait cursor because this could take a few seconds
	SetCursor(LoadCursor(NULL, IDC_WAIT)); 
	ShowCursor(TRUE); 

	m_gdiMem.Destroy();
	m_gdiBackground.Destroy();


	// End the Wait cursor
	ShowCursor(FALSE); 
	SetCursor(NULL);

	return TRUE;
}


BOOL CDlgBackJack::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR | SHIDIF_SIPDOWN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

#endif

    m_bShowSplashScreen = TRUE;

    SetTimer(hWnd, IDT_GAME_TIMER, 1, NULL);

    m_oTank.Initialize(2);//tank I think
    m_oTank.SetPosition(0,0,10);
	
	return TRUE;
}

BOOL CDlgBackJack::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	RECT rcClip;
	GetClipBox(hDC, &rcClip);

	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, TRUE);

	DrawBackground(m_gdiMem, rcClip);
    DrawTableText(m_gdiMem, rcClip);

	BitBlt(hDC,
			rcClip.left, rcClip.top, 
			rcClip.right - rcClip.left,
			rcClip.bottom - rcClip.top,
			m_gdiMem.GetDC(),
			rcClip.left,
			rcClip.top,
			SRCCOPY);

	return TRUE;
}


BOOL CDlgBackJack::DrawBackground(CIssGDIEx& gdi, RECT& rc)
{
	if(m_gdiBackground.GetDC() == NULL)
	{
		RECT rcBack;
        
        GetClientRect(m_hWnd, &rcBack);

		m_gdiBackground.Create(gdi.GetDC(), rcBack, FALSE, TRUE, FALSE, 16);

        CIssGDIEx gdiTemp;

        if(GetSystemMetrics(SM_CXICON) > 44)
            gdiTemp.LoadImage(IDB_PNG_TableVGA, m_hWnd, m_hInst, TRUE);
        else
            gdiTemp.LoadImage(IDB_PNG_Table, m_hWnd, m_hInst, TRUE);

        POINT ptStart;
        ptStart.y = 0;
        ptStart.x = 0;

        if(gdiTemp.GetWidth() > m_gdiBackground.GetWidth())
            ptStart.x = (gdiTemp.GetWidth() - m_gdiBackground.GetWidth())/2;

        BitBlt(m_gdiBackground.GetDC(), 0, 0, m_gdiBackground.GetWidth(), m_gdiBackground.GetHeight(),
            gdiTemp.GetDC(), ptStart.x, ptStart.y, SRCCOPY);
    }

	// draw the background
	BitBlt(gdi.GetDC(),
		rc.left, rc.top,
		WIDTH(rc), HEIGHT(rc),
		m_gdiBackground.GetDC(),
		rc.left, rc.top,
		SRCCOPY);


	return TRUE;
}

void CDlgBackJack::LoadFonts(HWND hWndSplash, int iMaxPercent)
{
    int iCurPercent = 0;
    if(hWndSplash)
        iCurPercent = SendMessage(hWndSplash, WM_GET_Percent, 0, 0);

    int iStep = (iMaxPercent-iCurPercent)/3;	// 3 steps

    int iFontHeight = 17*GetSystemMetrics(SM_CXICON)/32;

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

}

void CDlgBackJack::LoadImages(HWND hWndSplash, int iMaxPercent)

{
    int iCurPercent = 0;
    if(hWndSplash)
        iCurPercent = SendMessage(hWndSplash, WM_GET_Percent, 0, 0);

    int iStep = (iMaxPercent-iCurPercent)/8;	// 8 steps

    CIssGDIEx gdiTemp;

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

 

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);


    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);


    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);



    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);
}


BOOL CDlgBackJack::DrawTableText(CIssGDIEx& gdiDest, RECT& rc)
{
 
    return TRUE;
}



BOOL CDlgBackJack::OnLButtonDown(HWND hWnd, POINT& pt)
{
	return UNHANDLED;
}


BOOL CDlgBackJack::OnLButtonUp(HWND hWnd, POINT& pt)
{
  

    return UNHANDLED;
}

BOOL CDlgBackJack::OnMouseMove(HWND hWnd, POINT& pt)
{

	return FALSE;
}


void CDlgBackJack::PlaySounds(TCHAR* szWave)
{
	BOOL bRet = FALSE;
	if(m_bPlaySounds)
		bRet = PlaySound(szWave, m_hInst, SND_RESOURCE | SND_ASYNC | SND_NOWAIT);
		//bRet = PlaySound(szWave, m_hInst, SND_RESOURCE);
}

BOOL CDlgBackJack::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDOK:
		//OnKeyDown(hWnd, VK_RETURN, NULL);
		//OnKeyUp(hWnd, VK_RETURN, NULL);
		break;
    case IDMENU_Menu:
        ShowMenu();
        break;
    case IDMENU_NewGame:
    {
        CDlgMsgBox dlgMsg;
        if(IDYES != dlgMsg.PopupMessage(_T("Are you sure you want to start a new game?"), m_hWnd, m_hInst, MB_YESNO))
            break;

        NewGame();
        break;
    }
    case IDMENU_Options:
    {
     
        break;
    }
    case IDMENU_Exit:
    {
        CDlgMsgBox dlgMsg;
        if(IDYES == dlgMsg.PopupMessage(IDS_MSG_Exit, m_hWnd, m_hInst, MB_YESNO))
            PostQuitMessage(0);
        break;
    }
    case IDMENU_About:
    {
        CDlgAbout dlgAbout;
        dlgAbout.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
        break;
    }
    case IDMENU_HiScores:
    {
        CDlgHighScores dlgHighScores;
        dlgHighScores.Init(&m_oHighScores);
        dlgHighScores.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
        break;
    }
    case IDMENU_Help:
        
#ifdef WIN32_PLATFORM_PSPC
        CreateProcess(_T("peghelp"), _T("PanoBlackjack.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
#else
        LaunchHelp( _T("PanoBlackjack.htm"), m_hInst);
#endif
        break;
	}

	return FALSE;
}

BOOL CDlgBackJack::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return CIssWnd::OnDestroy(hWnd, wParam, lParam);
}

void CDlgBackJack::LoadRegSettings()
{
	DWORD dwVal	= 0;
	if(S_OK ==GetKey(REG_KEY, _T("PlaySounds"), dwVal))
		m_bPlaySounds = (BOOL)dwVal;

    // read in the high scores
    m_oHighScores.Init(NUM_HIGHSCORES);
    m_oHighScores.LoadRegistry(REG_KEY);
}

void CDlgBackJack::SaveRegSettings()
{
	DWORD dwTemp;

	dwTemp	= (DWORD)m_bPlaySounds;
	SetKey(REG_KEY, _T("PlaySounds"), dwTemp);

    m_oHighScores.SaveRegistry(REG_KEY);
}

BOOL CDlgBackJack::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	switch(wParam)
	{
	case _T('0'):
		break;
	case _T('1'):
	case _T('2'):	
	case _T('3'):	
	case _T('4'):	
	case _T('5'):	
	case _T('6'):	
		break;
	case _T('7'):
		break;
	case _T('8'):
		break;
	case _T('9'):
		break;
	case VK_RETURN:

		OnLButtonDown(m_hWnd, pt);
		break;
	}
	return TRUE;
}

BOOL CDlgBackJack::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	switch(wParam)
	{
	case _T('0'):
		break;
	case _T('1'):
	case _T('2'):	
	case _T('3'):	
	case _T('4'):	
	case _T('5'):	
	case _T('6'):	
		break;
	case _T('7'):
		break;
	case _T('8'):
		break;
	case _T('9'):
		break;
	case VK_LEFT:
        
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
    case VK_TSOFT1:
    case VK_RETURN:
        OnHandleNavBtn(m_hWnd, wParam);
		break;
	case VK_ESCAPE:
        PostQuitMessage(0); 
		break;
    case VK_TSOFT2:
        ShowMenu();
        break;
	}
	return TRUE;
}

BOOL CDlgBackJack::IsLowRes()
{
	if(GetSystemMetrics(SM_CXSCREEN) < 240)
		return TRUE;
	else
		return FALSE;
}

BOOL CDlgBackJack::OnHandleNavBtn(HWND hWnd, WPARAM wParam)
{
    if(wParam == VK_TSOFT1)
        wParam = VK_RETURN;

   
	InvalidateRect(hWnd, NULL, FALSE);
	return TRUE;
} 

BOOL CDlgBackJack::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(wParam)
    {
    case IDT_GAME_TIMER:
        //hacking away
        {
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            HDC dc = GetDC(hWnd);
            DrawBackground(m_gdiMem, rcClient);

            m_oTank.Spin(1);
            m_oTank.Draw(m_gdiMem.GetDC(), rcClient);

            BitBlt(dc, rcClient.left, rcClient.top, WIDTH(rcClient), HEIGHT(rcClient), m_gdiMem.GetDC(), rcClient.left, rcClient.top, SRCCOPY);

            ReleaseDC(hWnd, dc);
        }
    	break;
    default:
        return UNHANDLED;
        break;
    }

	return TRUE;
}

BOOL CDlgBackJack::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
   
	return UNHANDLED;
}

BOOL CDlgBackJack::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL bRet = UNHANDLED;
#ifdef UNDER_CE
    HWND hWndSip = NULL;

    if(LOWORD(wParam) == WA_ACTIVE)
    {	//this should only exist when using a smartphone in landscape orientation (for now anyways)
        bRet = SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
        RECT rc; 
        SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);   

        if(m_bShowSplashScreen == TRUE)
        {
            m_bShowSplashScreen = FALSE;			

            // put up the splash screen
            ShowSplashScreen();		
        }
    }
#endif
    return bRet;
}

BOOL CDlgBackJack::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
  
    return TRUE;
}


BOOL CDlgBackJack::ShowSplashScreen()
{
    // put up the splash screen first
    CDlgSplashScreen dlgSplashScreen;
    dlgSplashScreen.Create(_T("Splash"), NULL, m_hInst, _T("SplashClass"));
    ShowWindow(dlgSplashScreen.GetWnd(), SW_SHOW);
    MSG msg;

    // force the message pump to go through while waiting for the first WM_PAINT
    while( GetMessage( &msg,dlgSplashScreen.GetWnd(),0,0 ) ) 
    {
        TranslateMessage( &msg );
        DispatchMessage ( &msg );

        switch(msg.message)
        {
        case WM_LOAD_Sounds:
            //ReloadSounds();
            //SendMessage(dlgSplashScreen.GetWnd(), WM_SET_Percent, 10, 0);
            break;
        case WM_LOAD_Fonts:
            //ReloadFonts();
            LoadFonts(dlgSplashScreen.GetWnd(), 25);
            //SendMessage(dlgSplashScreen.GetWnd(), WM_SET_Percent, 15, 0);
            break;
        case WM_LOAD_Background:
            //ReloadBackground(dlgSplashScreen.GetWnd(), 60);
            //SendMessage(dlgSplashScreen.GetWnd(), WM_SET_Percent, 60, 0);
            break;
        case WM_lOAD_Images:
            //ReloadHexagons(dlgSplashScreen.GetWnd(), 100);
            LoadImages(dlgSplashScreen.GetWnd(), 100);

            SendMessage(dlgSplashScreen.GetWnd(), WM_SET_Percent, 100, 0);
            break;
        }

        if(msg.message == WM_lOAD_Images)
            break;
    }

    ShowMainMenu();
    return TRUE;
}

BOOL CDlgBackJack::ShowMainMenu()
{
    CDlgMainMenu	dlgMenu;

    UINT uiResult = (UINT)dlgMenu.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);



    switch(uiResult)
    {
    case IDMENU_NewGame:
        NewGame();
        break;
    case IDMENU_Resume:

        break;
    case IDMENU_Exit:
        PostQuitMessage(0);
        return TRUE;

    }

    return TRUE;
}

void CDlgBackJack::NewGame(BOOL bCheckHiScore)
{
    InvalidateRect(m_hWnd, NULL, FALSE);
    PlaySounds(_T("IDR_WAV_SHUFFLE"));
}

BOOL CDlgBackJack::ShowMenu()
{
    // don't show it twice
    if(m_oMenuHandler.IsMenuUp())
        return TRUE;

    //Create the menu.
    HMENU hMenu			= CreatePopupMenu();

    if(!hMenu)
    {
        return TRUE;
    }

    m_oMenuHandler.Initialize(m_hWnd, m_hInst, RGB(255,255,255), 0x8DEEA4, TRUE, 0);

 //   m_oMenuHandler.AppendMenu(hMenu, MF_SEPARATOR, IDMENU_Separator, 0);
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_NewGame, IDS_MENU_NewGame);
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_Options, IDS_MENU_Options);
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_HiScores, IDS_MENU_HiScores);
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_About, IDS_MENU_About);
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_Help, IDS_MENU_Help);
//    m_oMenuHandler.AppendMenu(hMenu, MF_SEPARATOR, IDMENU_Separator, 0);
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_Exit, IDS_MENU_Exit);

    POINT pt;
    int iShadowSpace= 3*GetSystemMetrics(SM_CYSCREEN)/100;

    RECT rcTemp;
    GetClientRect(m_hWnd, &rcTemp);

    pt.x = 0;
    pt.y = 200;

    //Display it.
    m_oMenuHandler.PopupMenu(	hMenu, 
        TPM_RIGHTALIGN|TPM_BOTTOMALIGN, 
        pt.x,pt.y);

    DestroyMenu(hMenu);

    return TRUE;
}

BOOL CDlgBackJack::OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenuHandler.OnMeasureItem(hWnd, wParam, lParam);
}

BOOL CDlgBackJack::OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenuHandler.OnDrawItem(hWnd, wParam, lParam);
}

BOOL CDlgBackJack::OnEnterMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    return m_oMenuHandler.OnEnterMenuLoop(hWnd, wParam, lParam);
}




BOOL CDlgBackJack::ForceScreenUpdate()
{
    HDC dc = GetDC(m_hWnd);
    if(dc == NULL)
        return FALSE;
    
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    OnPaint(m_hWnd, dc, rcClient);

    ReleaseDC(m_hWnd, dc);

    return TRUE;
}

void CDlgBackJack::GameLoop()
{




}