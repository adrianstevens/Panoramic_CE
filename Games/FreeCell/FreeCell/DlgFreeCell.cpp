#include "StdAfx.h"
#include "DlgFreeCell.h"
#include "resource.h"
#include "IssRegistry.h"
#include "IssCommon.h"
#include "IssGDIFx.h"
#include "IssGDIDraw.h"
#include "DlgSelectGame.h"
#include "DlgMoreApps.h"

#include "DlgOptions.h"
#include "DlgMsgBox.h"
#include "DlgSplashScreen.h"
#include "DlgAbout.h"
#include "DlgMainMenu.h"
#include "IssLocalisation.h"

enum EnumCardSuit
{
    SUIT_Heart,
    SUIT_Club,
    SUIT_Spade,
    SUIT_Diamond,
};


#define NUMBER_OF_FRAMES	20

//use function GetCardOffSets() instead
#define CARD_OFFSETS	(m_sizeCards.cy/3)
#define CARD_OFFSETS_S  (m_sizeCards.cy/5)

#define TOP_INDENT		(IsVGA()?53:27)
#define TOP_TEXT_INDENT (IsVGA()?12:6)


#define TOP_SPACING		1	//in pixels
#define TOP_MIDGAP		8   //for all resolutions (really)

#define CARD_INDENT		1
#define CARD_SPACING	2
#define IDT_Preview_Timer	1
#define IDT_GameTime_Timer  2
#define PREVIEW_TIME		500

extern CObjGui* g_gui;

CDlgFreeCell::CDlgFreeCell(void)
:m_hFontText(NULL)
,m_bCardMoved(FALSE)
,m_bCardSelected(FALSE)
,m_bMessageBoxUp(FALSE)
,m_bShowSplashScreen(FALSE)
,m_bDrawSelector(TRUE)
,m_bShowInvalidMove(FALSE)
,m_bDrawRedAces(FALSE)
,m_iSelector(0)
,m_iSelectedCount(-1)
,m_iHorizontalOffset(0)
,m_rcTransparentColor(TRANSPARENT_COLOR)
{
	ZeroMemory(&m_sizeWindow, sizeof(SIZE));

    // create our fonts
    m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)/2, FW_BOLD, TRUE);

    //first things first
    g_gui->m_eBackground = m_oGame.GetBackgroundColor();

    if(m_oGame.GetBackgroundColor() == BGCOLOR_Random)
        g_gui->m_eBackground = (EnumBackgroundColor)(rand()%BGCOLOR_Random);
}



CDlgFreeCell::~CDlgFreeCell(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
}

BOOL CDlgFreeCell::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	// first check if we need to reload the images first
	if(m_sizeWindow.cx	== (rcClient.right - rcClient.left)&&
		m_sizeWindow.cy	== (rcClient.bottom - rcClient.top))
		return TRUE;

	// update our size structure
	m_sizeWindow.cx		= rcClient.right - rcClient.left;
	m_sizeWindow.cy		= rcClient.bottom - rcClient.top;

    g_gui->OnSize(hWnd, rcClient);
	g_gui->gdiMem.Destroy();
	g_gui->gdiBackground.Destroy();
	HDC dc = GetDC(hWnd);
	g_gui->InitGDI(rcClient, dc);
	ReleaseDC(hWnd, dc);

	// start the wait cursor because this could take a few seconds
	SetCursor(LoadCursor(NULL, IDC_WAIT)); 
	ShowCursor(TRUE); 

	m_gdiMoveBackground.Destroy();

	m_rcTopBar.left			    = rcClient.left;
    m_rcTopBar.right			= rcClient.right;
    m_rcTopBar.top              = rcClient.top;
    m_rcTopBar.bottom           = m_rcTopBar.top + TOP_INDENT;

 /*   m_rcCardsLeft               = m_rcTopBar;
    m_rcCardsLeft.right         -= 3*TOP_INDENT/2;

    m_rcGameNum                 = m_rcTopBar;
    m_rcGameNum.left            = rcClient.left;  */

	m_rcMsgText.left			= GetSystemMetrics(SM_CXICON)/6;
	m_rcMsgText.right			= GetSystemMetrics(SM_CXSCREEN) - m_rcMsgText.left;
	m_rcMsgText.bottom			= rcClient.bottom - GetSystemMetrics(SM_CXICON)*2/3;
	m_rcMsgText.top				= m_rcMsgText.bottom - GetSystemMetrics(SM_CXICON)*2/3;

    m_rcUndo.right              = 0; //we'll set this in the draw function

	if(m_gdiArrow.GetDC())
	{	//we'll only reload em
		LoadImages(hWnd, 100);//we'll ignore progress msgs
		m_imgTopBar.SetSize(WIDTH(m_rcTopBar), HEIGHT(m_rcTopBar));
	}
	// End the Wait cursor
	ShowCursor(FALSE); 
	SetCursor(NULL);

	return TRUE;
}

void CDlgFreeCell::ResizeImages()
{
    CIssGDIEx gdiTemp;

    SIZE szNew;
    szNew.cx = (GetSystemMetrics(SM_CXSCREEN) - 16)/8;
    szNew.cy = szNew.cx * 42 / 28; //ratio of the low res

    SIZE szTemp;
    szTemp.cx = szNew.cx * 13;
    szTemp.cy = szNew.cy;

    gdiTemp.LoadImage(IDB_BMP_Clubs, m_hWnd, m_hInst);
    ScaleImage(gdiTemp, m_gdiCards[SUIT_Club], szTemp, FALSE, 0);

    gdiTemp.LoadImage(IDB_BMP_Spades, m_hWnd, m_hInst);
    ScaleImage(gdiTemp, m_gdiCards[SUIT_Spade], szTemp, FALSE, 0);

    gdiTemp.LoadImage(IDB_BMP_Diamonds, m_hWnd, m_hInst);
    ScaleImage(gdiTemp, m_gdiCards[SUIT_Diamond], szTemp, FALSE, 0);

    gdiTemp.LoadImage(IDB_BMP_Hearts, m_hWnd, m_hInst);
    ScaleImage(gdiTemp, m_gdiCards[SUIT_Heart], szTemp, FALSE, 0);


    szTemp.cx = (szNew.cx - 2)*5;
    szTemp.cy = (szNew.cy - 2);

    gdiTemp.LoadImage(IDB_PNG_Top, m_hWnd, m_hInst);
    ScaleImage(gdiTemp, m_gdiCardBack, szTemp, FALSE, 0);
}

BOOL CDlgFreeCell::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    m_oStr->SetResourceInstance(m_hInst);
    g_cLocale.Init(REG_KEY, m_hInst, SUPPORT_English|SUPPORT_Portuguese|SUPPORT_Korean|SUPPORT_French|SUPPORT_German|SUPPORT_Dutch|SUPPORT_Japanese, TRUE);

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);
#endif
    g_gui->Init(hWnd, m_hInst);

	// put up the main menu first
	m_bShowSplashScreen = TRUE;
	return TRUE;
}

void CDlgFreeCell::LoadImages(HWND hWndSplash, int iMaxPercent)
{
    int iCurPercent = 0;
    if(hWndSplash)
        iCurPercent = SendMessage(hWndSplash, WM_GET_Percent, 0, 0);

    //undo
	if(m_gdiArrow.GetDC() == NULL)
		m_gdiArrow.LoadImage(IsVGA()?IDB_PNG_Arrow_HR:IDB_PNG_Arrow, hWndSplash, m_hInst);

	//separator
	if(m_gdiSeparator.GetDC() == NULL)
		m_gdiSeparator.LoadImage(IsVGA()?IDB_PNG_Seperator_HR:IDB_PNG_Seperator, hWndSplash, m_hInst);

    BOOL b44 = FALSE;
    
    if(g_gui->GetIsSmartPhone() == FALSE)
    {
        if(GetSystemMetrics(SM_CXICON) > 32 && GetSystemMetrics(SM_CXICON) < 64)
            b44 = TRUE;
    }

    int iStep = (iMaxPercent-iCurPercent)/6;	// 8 steps

    UINT uInt;
    if(b44) uInt = IDB_BMP_Clubs_44;
    else uInt = IsVGA()?IDB_BMP_Clubs_HR:IDB_BMP_Clubs;

	if(m_gdiCards[SUIT_Club].GetDC() == NULL)
		m_gdiCards[SUIT_Club].LoadImage(uInt, m_hWnd, m_hInst);
    
    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    if(b44) uInt = IDB_BMP_Spades_44;
    else uInt = IsVGA()?IDB_BMP_Spades_HR:IDB_BMP_Spades;

	if(m_gdiCards[SUIT_Spade].GetDC() == NULL)
		m_gdiCards[SUIT_Spade].LoadImage(uInt, m_hWnd, m_hInst);

    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    if(b44) uInt = IDB_BMP_Diamonds_44;
    else uInt = IsVGA()?IDB_BMP_Diamonds_HR:IDB_BMP_Diamonds;

	if(m_gdiCards[SUIT_Diamond].GetDC() == NULL)
		m_gdiCards[SUIT_Diamond].LoadImage(uInt, m_hWnd, m_hInst);
    
    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    if(b44)uInt = IDB_BMP_Hearts_44;
    else uInt = IsVGA()?IDB_BMP_Hearts_HR:IDB_BMP_Hearts;

	if(m_gdiCards[SUIT_Heart].GetDC() == NULL)
		m_gdiCards[SUIT_Heart].LoadImage(uInt, m_hWnd, m_hInst);
    
    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);
    
    if(b44)uInt = IDB_PNG_Top_44;
    else uInt = IsVGA()?IDB_PNG_Top_HR:IDB_PNG_Top;

	if(m_gdiCardBack.GetDC() == NULL)
		m_gdiCardBack.LoadImage(uInt, m_hWnd, m_hInst);

    //now ... if its low-res smartphone .....
    if(GetSystemMetrics(SM_CXSCREEN) < 240)
    {
        ResizeImages();
    }

    //just the way the images are sized
	m_sizeCards.cx = m_gdiCardBack.GetWidth()/5+2;
	m_sizeCards.cy = m_gdiCardBack.GetHeight()+2;

    //and finally ... determine the x offset (centering for landscape)
    m_iHorizontalOffset = (GetSystemMetrics(SM_CXSCREEN) - 
        CARD_INDENT*2 - CARD_SPACING*7 - m_sizeCards.cx*CARD_COLUMNS)/2;

	int i = 0;
	for(i = 0; i < 4; i++)
	{
		m_rcFreeCells[i].left	= CARD_INDENT+(i*(m_sizeCards.cx+CARD_INDENT)) + m_iHorizontalOffset;
		m_rcFreeCells[i].right  = m_rcFreeCells[i].left + m_sizeCards.cx;
		m_rcFreeCells[i].top	= TOP_INDENT;
		m_rcFreeCells[i].bottom = m_rcFreeCells[i].top + m_sizeCards.cy;
	}

	for(i = 0; i < 4; i++)
	{
		m_rcAceRack[i].left		= CARD_INDENT+(i*(m_sizeCards.cx+CARD_INDENT))+m_rcFreeCells[3].right+TOP_MIDGAP;
		m_rcAceRack[i].right	= m_rcAceRack[i].left + m_sizeCards.cx;
		m_rcAceRack[i].top		= TOP_INDENT;
		m_rcAceRack[i].bottom	= m_rcAceRack[i].top + m_sizeCards.cy;
	}

	if(m_imgSelector.IsLoaded() == FALSE)
		m_imgSelector.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_Selector_HR:IDB_PNG_Selector);
    m_imgSelector.SetSize(m_gdiCards->GetWidth()/13, m_gdiCards->GetHeight());

	if(m_imgTopBar.IsLoaded() == FALSE)
		m_imgTopBar.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_TopBar_HR:IDB_PNG_TopBar);
    
	if(m_imgShadow.IsLoaded() == FALSE)
		m_imgShadow.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_Shadow_HR:IDB_PNG_Shadow);

    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    //calc cards per column before we need to resize
    //so .. remove the bottom bar and find the distance from here to the top of the columns
    //and remove one extra card so we can quickly divide
    int iTemp = GetSystemMetrics(SM_CYSCREEN) - TOP_INDENT - CARD_INDENT - 2*m_sizeCards.cy;

    m_iMaxCardsPerColumn = iTemp/(CARD_OFFSETS);
    m_iMaxCardsPerColumn++;//for the full card
}

BOOL CDlgFreeCell::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    g_gui->InitGDI(rcClient, hDC);

	RECT rcClip;
	GetClipBox(hDC, &rcClip);

	// draw the background
	g_gui->DrawBackground(rcClip);

    DrawTopBar(g_gui->gdiMem, rcClip);

    // draw all the text
	DrawText(g_gui->gdiMem.GetDC(), rcClip);
	
	// draw the cards
	DrawCards(g_gui->gdiMem, rcClip);

	// draw the selector
	DrawSelector(g_gui->gdiMem, rcClip);

	// and if a card is selected
	DrawSelected(g_gui->gdiMem, rcClip);

    TCHAR szTemp[STRING_NORMAL];
    TCHAR szTemp2[STRING_NORMAL];

    m_oStr->StringCopy(szTemp, ID(IDS_MENU_Menu), STRING_NORMAL, m_hInst);
    m_oStr->StringCopy(szTemp2, ID(IDS_MENU_Game), STRING_NORMAL, m_hInst);

    g_gui->DrawBottomBar(rcClip, szTemp, szTemp2);

    g_gui->DrawScreen(rcClip, hDC);

	return TRUE;
}

BOOL CDlgFreeCell::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case _T('1')://pile 1
	case _T('2')://pile 2
	case _T('3')://pile 3
	case _T('4')://pile 4
	case _T('5')://pile 5
	case _T('6')://pile 6
	case _T('7')://pile 7
		break;
	case _T('8'):
	case _T('d'):
		break;
	case _T('N'):
	case _T('n')://new game
		OnNewGame();
		break;
	case _T('p')://time cheat
		break;
	case _T('0'):
	case _T('u'):
		m_oGame.OnUndo();
		break;
	case _T('r'):
	case _T('R'):
		m_oGame.NewGame(FALSE);
		break;
	case _T('c'):
		m_oGame.CheckGameState();
		break;
	case _T('e'):
		{
			HDC dc = GetDC(m_hWnd);
			AnimateSolitaire(dc);
			ReleaseDC(m_hWnd, dc);
		}
        break;
    case _T('q'):
        PostQuitMessage(0);
		break;	
	default:
		return FALSE;
	    break;
	}
	//drop that sip
	SHSipPreference(m_hWnd, SIP_DOWN);
	InvalidateRect(m_hWnd, NULL, FALSE);
	return TRUE;

}

BOOL CDlgFreeCell::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case _T('0'):
		break;
	case _T('1'):
		break;
	case _T('2'):
		break;
	case _T('3'):
		break;
	case _T('4'):
		break;
	case _T('5'):
		break;
	case _T('6'):
		break;
	case _T('7'):
		break;
	case _T('8'):
		break;
	case _T('9'):
		break;
	case VK_RETURN:
		break;
	}
	return TRUE;
}

BOOL CDlgFreeCell::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_bDrawSelector = TRUE;
	int iOldSel = m_iSelector;

    g_gui->OnKeyUp(hWnd, wParam, lParam);
    
    switch(LOWORD(wParam))
	{
	case _T('0'):
		break;
	case _T('1'):
		break;
	case _T('2'):
		break;
	case _T('3'):
		break;
	case _T('4'):
		break;
	case _T('5'):
		break;
	case _T('6'):
		break;
	case _T('7'):
		break;
	case _T('8'):
		break;
	case _T('9'):
		break;
	case VK_RETURN:
		//lets fake a mouse click
		POINT pt;
		if(m_iSelector < 8)
		{
			pt.x = m_rcCards[m_iSelector].left + 1;
			pt.y = m_rcCards[m_iSelector].top + 1;
		}
		else if(m_iSelector < 12)
		{
			pt.x = m_rcFreeCells[m_iSelector-CL_FreeCell1].left + 1;
			pt.y = m_rcFreeCells[m_iSelector-CL_FreeCell1].top + 1;
		}
		else if(m_iSelector < 16) 
		{
			pt.x = m_rcAceRack[m_iSelector-CL_AceRack1].left + 1;
			pt.y = m_rcAceRack[m_iSelector-CL_AceRack1].top + 1;
		}
		else
			return FALSE;

		OnLButtonUp(m_hWnd, pt);
		break;
	case VK_RIGHT:
		//lets wrap the top and the bottom
		m_bDrawSelector = TRUE;
		m_iSelector++;
		if(m_iSelector%8 == 0)
			m_iSelector -=8;
		UpdateSelector(iOldSel);
		break;
	case VK_LEFT:
		//lets wrap the top and the bottom
		m_bDrawSelector = TRUE;
		m_iSelector--;
		if((m_iSelector+1)%8 == 0)
			m_iSelector += 8;
		UpdateSelector(iOldSel);
		break;
	case VK_UP:
	case VK_DOWN:
		if(m_iSelector > CL_Column8)
			m_iSelector -= 8;
		else 
			m_iSelector += 8;
		UpdateSelector(iOldSel);
		break;
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

BOOL CDlgFreeCell::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(wParam == IDT_Time_Timer)
	{	// so it doesn't go dark
		SystemIdleTimerReset();

		if(!m_oGame.IsGameInPlay())
			KillTimer(m_hWnd, IDT_Time_Timer);
	}
	if(wParam == IDT_NewGame_Timer)
	{
		KillTimer(m_hWnd, IDT_NewGame_Timer);
		OnNewGame();
	}
	if(wParam == IDT_InvalidMove_Timer)
	{
		KillTimer(m_hWnd, IDT_InvalidMove_Timer);
		m_bShowInvalidMove = FALSE;
		InvalidateRect(m_hWnd, &m_rcMsgText, FALSE);
	}
	if(wParam == IDT_Preview_Timer)
	{
		KillTimer(m_hWnd, IDT_Preview_Timer);
		InvalidateRect(m_hWnd, NULL, FALSE);
	}
	return TRUE;
}

BOOL CDlgFreeCell::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return CIssWnd::OnDestroy(hWnd, wParam, lParam);
}

BOOL CDlgFreeCell::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	//left and right have been switched for Soleus UI 

	switch(LOWORD(wParam))
	{
//	case IDMENU_Menu:
    case IDMENU_Left:
		OnMenuMenu();
		break;
	case IDMENU_Game:
    case IDMENU_Right:
		OnGameMenu();
		break;
	case IDMENU_NewGame:
		if(m_oGame.IsGameInPlay() && m_oGame.GetCardsRemaining() < 52) //won't prompt if they haven't done anything yet
		{	
            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(m_oStr->GetText(ID(IDS_MSG_ResignNew), m_hInst), m_hWnd, m_hInst, MB_YESNO))
                OnNewGame();
		}
		else
			OnNewGame();
		

		break;
	case IDMENU_RestartGame:
		if(m_oGame.IsGameInPlay())
		{
            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(m_oStr->GetText(ID(IDS_MSG_Restart), m_hInst), m_hWnd, m_hInst, MB_YESNO))
                m_oGame.NewGame(FALSE);	
		}
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case IDMENU_SelectGame:
		if(m_oGame.IsGameInPlay() && m_oGame.GetCardsRemaining() < 52) //won't prompt if they haven't done anything yet
		{
            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(m_oStr->GetText(ID(IDS_MSG_Resign), m_hInst), m_hWnd, m_hInst, MB_YESNO))
           	{
				CDlgSelectGame oDlg;
				if(oDlg.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
				{
					int iGameNum = oDlg.GetGameNumber();
					if(iGameNum > -1 && iGameNum < SEED_MAX)
						m_oGame.SelectGame(iGameNum);
				}
			}	
		}
		else
		{	//just do it
			CDlgSelectGame oDlg;
			if(oDlg.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
			{
				int iGameNum = oDlg.GetGameNumber();
				if(iGameNum > -1 && iGameNum < SEED_MAX)
					m_oGame.SelectGame(iGameNum);
			}
		}
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case IDMENU_Stats:
        {
            TCHAR szTemp[STRING_MAX];
		    m_oStr->Format(szTemp, m_oStr->GetText(ID(IDS_MSG_StatsDetail), m_hInst),
			    m_oGame.GetGamesWon(), m_oGame.GetGamesLost(), m_oGame.GetWinPcnt(), m_oGame.GetCurrentStreak(), m_oGame.GetLongestStreak());
    		
            CDlgMsgBox dlgMsg;
            dlgMsg.PopupMessage(szTemp, m_hWnd, m_hInst, MB_OK);
		    InvalidateRect(m_hWnd, NULL, FALSE);
        }
		break;
	case IDMENU_ResetStats:
        {    
            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(m_oStr->GetText(ID(IDS_MSG_ResetStats), m_hInst), m_hWnd, m_hInst, MB_YESNO))
                m_oGame.ClearStats();//done and done
		    InvalidateRect(m_hWnd, NULL, FALSE);
        }
		break;
	case IDMENU_Undo:
		if(m_oGame.OnUndo())
            PlaySounds(_T("IDR_WAVE_PUTDOWN"));
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case IDMENU_AutoPlay:
		m_oGame.AutoPlay();
		break;
    case IDMENU_Quit:
    case IDMENU_Exit:
        {   
            CDlgMsgBox dlgMsg;
            if(IDYES == dlgMsg.PopupMessage(ID(IDS_MSG_Exit), m_hWnd, m_hInst, MB_YESNO))
                PostQuitMessage(0);
        }
        break;
    case IDMENU_Options:
        {
            CDlgOptions dlgOptions;
            dlgOptions.SetGame(&m_oGame);
            dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
        }
        break;
    case IDMENU_Help:
		CDlgMainMenu::LaunchHelp(m_hWnd, m_hInst);
        break;
	case IDMENU_MoreApps:
		{
			CDlgMoreApps dlgMoreApps;
			dlgMoreApps.Launch(m_hInst, m_hWnd);
		}
		break;
    case IDMENU_About:
        {
            CDlgAbout dlgAbout;
            dlgAbout.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
        }

        break;

	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CDlgFreeCell::DrawSelector(CIssGDIEx& gdi, RECT& rcClient)
{
	if(!m_bDrawSelector)
		return FALSE;

	if(m_iSelector < 0)
		return FALSE;

	RECT rcTemp;
	if(m_iSelector < 8)
    {
        rcTemp = m_rcCards[m_iSelector];
        
    }
	else if(m_iSelector > 7 && m_iSelector < 12)
    {
		rcTemp = m_rcFreeCells[m_iSelector-8];
    }
	else//ace rack
    {
        rcTemp = m_rcAceRack[m_iSelector-12];
    }
	
	m_imgSelector.DrawImage(gdi, rcTemp.left, rcTemp.top);
	return TRUE;
}

BOOL CDlgFreeCell::DrawSelected(CIssGDIEx& gdi, RECT& rcClip)
{
	int iSelected = m_oGame.GetSelectedCard();

	if(iSelected == NULL_CARD)
		return FALSE;

	if(iSelected > 7 && iSelected < 12)
		m_rcOldSelectedCard = m_rcFreeCells[iSelected-8];
	else
		m_rcOldSelectedCard = m_rcCards[iSelected];

	if(rcClip.left > m_rcOldSelectedCard.right)
		return FALSE;
	if(rcClip.right < m_rcOldSelectedCard.left)
		return FALSE;
	if(rcClip.top > m_rcOldSelectedCard.bottom)
		return FALSE;
	if(rcClip.bottom < m_rcOldSelectedCard.top)
		return FALSE;
	
	AlphaFillRect(gdi, m_rcOldSelectedCard, RGB(0,0,0), 100);
	
	return TRUE;
}

BOOL CDlgFreeCell::DrawTopBar(CIssGDIEx& gdi, RECT& rcClip)
{
    if(rcClip.top > TOP_INDENT)
        return FALSE;

    if(m_imgTopBar.GetHeight() == 0)
        m_imgTopBar.SetSize(WIDTH(m_rcTopBar), HEIGHT(m_rcTopBar));

    //set up the rect
    if(WIDTH(m_rcUndo) != m_gdiArrow.GetWidth())
    {
        m_rcUndo.top = m_rcTopBar.top + (HEIGHT(m_rcTopBar) - m_gdiArrow.GetHeight())/2 + 1;
        m_rcUndo.bottom = m_rcUndo.top + m_gdiArrow.GetHeight();
        m_rcUndo.right = m_rcTopBar.right - TOP_TEXT_INDENT;
        m_rcUndo.left = m_rcUndo.right - m_gdiArrow.GetWidth();
    }    

    m_imgTopBar.DrawImage(gdi, m_rcTopBar.left, m_rcTopBar.top);
    Draw(gdi, m_rcUndo, m_gdiArrow);

    //draw the text and the score
    static TCHAR szGameNum[STRING_NORMAL];
	static TCHAR szCardsLeft[STRING_NORMAL];
    m_oStr->IntToString(szGameNum, m_oGame.GetGameNumber());   
    m_oStr->Insert(szGameNum, m_oStr->GetText(ID(IDS_MENU_GameNumber), m_hInst));
	RECT rcTemp = m_rcTopBar;
	::DrawText(gdi.GetDC(), szGameNum, rcTemp, DT_CALCRECT, m_hFontText);
    int iGame = WIDTH(rcTemp); 


//	::DrawText(gdi.GetDC(), szTemp, m_rcGameNum, DT_LEFT | DT_VCENTER, m_hFontText, (m_oGame.IsSolvable()?0xFFFFFF:0x0000DD));
    m_oStr->IntToString(szCardsLeft, m_oGame.GetCardsRemaining());
    m_oStr->Insert(szCardsLeft, m_oStr->GetText(ID(IDS_MENU_Cards), m_hInst));
    
	rcTemp = m_rcTopBar;
	::DrawText(gdi.GetDC(), szCardsLeft, rcTemp, DT_CALCRECT | DT_RIGHT, m_hFontText);
	int iCards = WIDTH(rcTemp);
//	::DrawText(gdi.GetDC(), szTemp, m_rcCardsLeft, DT_RIGHT | DT_VCENTER, m_hFontText, 0xFFFFFF);

	//and finally...the separators
	rcTemp = m_rcUndo;
	rcTemp.left = m_rcUndo.left - m_gdiArrow.GetWidth()/2;
	rcTemp.right = rcTemp.left + m_gdiSeparator.GetWidth();
	Draw(gdi, rcTemp, m_gdiSeparator); 

	int iGap = (rcTemp.right - iCards - iGame)/4;
	rcTemp = m_rcTopBar;
	rcTemp.left = iGap;
	::DrawText(gdi.GetDC(), szGameNum, rcTemp, DT_LEFT | DT_VCENTER, m_hFontText, (m_oGame.IsSolvable()?0xFFFFFF:0x0000DD));

	rcTemp.top = m_rcUndo.top;
	rcTemp.left = rcTemp.left + iGame + iGap;
	rcTemp.right = rcTemp.left + m_gdiSeparator.GetWidth();
	Draw(gdi, rcTemp, m_gdiSeparator); 

	rcTemp.top = m_rcTopBar.top;
	rcTemp.left = rcTemp.left + iGap;
	rcTemp.right = m_rcTopBar.right;
	::DrawText(gdi.GetDC(), szCardsLeft, rcTemp, DT_LEFT | DT_VCENTER, m_hFontText, 0xFFFFFF);

    return TRUE;
}

BOOL CDlgFreeCell::DrawText(HDC hDC, RECT& rcClient)
{

//    TCHAR szTemp[STRING_NORMAL];

	if(m_bShowInvalidMove && IsRectInRect(rcClient, m_rcMsgText))
	{
		::DrawText(hDC, m_oStr->GetText(ID(IDS_MSG_InvalidMove), m_hInst), m_rcMsgText, DT_RIGHT | DT_VCENTER, m_hFontText, 0xDDDDDD);
	}


	return TRUE;
}

void CDlgFreeCell::DrawCard(CIssGDIEx& gdi, int iCard, POINT ptLocation)
{
	if(iCard >= CARD_BLANK)
	{
        //recopy the background first
        Draw(gdi, ptLocation.x+1, ptLocation.y+1, 
            m_sizeCards.cx-2,
            m_sizeCards.cy-2,
            g_gui->gdiBackground,
            ptLocation.x+1, ptLocation.y+1);


        Draw(gdi, ptLocation.x+1, ptLocation.y+1, 
            m_sizeCards.cx-2,
            m_sizeCards.cy-2,
            m_gdiCardBack,
            (m_sizeCards.cx-2)*(iCard-CARD_BLANK));
	}
	else
	{
		if(iCard < 1 || iCard > 52)
			return;//bounds check yo

        //someone put aces at the end ...
        int iValue = iValue = (iCard-1)%13-1;
        if(iValue < 0)
            iValue = 12;
        if(m_oGame.GetInverse() == TRUE)
        {   
            //switch the king and the ace
            if(iValue == 12)
                iValue = 11;
            else if(iValue == 11)
                iValue = 12;
            else // just swap
                iValue = 10-iValue;
        }
        
        Draw(gdi, ptLocation.x, ptLocation.y, 
            m_sizeCards.cx,
            m_sizeCards.cy, 
            m_gdiCards[(iCard-1)/13],
            iValue*m_sizeCards.cx,
            0);
	}
}


void CDlgFreeCell::DrawCard(HDC hDC, int iCard, POINT ptLocation)
{
    if(iCard == CARD_BLANK)
    {
    }
    else
    {
        if(iCard < 1 || iCard > 52)
            return;//bounds check yo

        //someone put aces at the end ...
        int iValue = (iCard-1)%13-1;
        if(iValue < 0)
            iValue = 12;

        TransparentBlt(hDC, ptLocation.x, ptLocation.y, 
        m_sizeCards.cx,
        m_sizeCards.cy,
        m_gdiCards[(iCard-1)/13].GetDC(),
        iValue*m_sizeCards.cx,
        0,
        m_sizeCards.cx,
        m_sizeCards.cy,
        TRANSPARENT_COLOR);
    }
}


BOOL CDlgFreeCell::DrawCards(CIssGDIEx& gdi, RECT& rcClip)
{
	//make sure the card GDIs exist
	if(m_gdiCards[0].GetDC() == NULL)
		return FALSE;

	int redraw = -1;

	int i = 0;
	POINT pt;
	//free cells
	for(i = 0; i < 4; i++)
	{
		if(rcClip.right < m_rcFreeCells[i].left)
			continue;
		if(rcClip.left > m_rcFreeCells[i].right)
			continue;

		pt.x = m_rcFreeCells[i].left;
		pt.y = m_rcFreeCells[i].top;

		if(m_oGame.m_btFreeCells[i] > 0 && m_oGame.m_btFreeCells[i] < CARD_BLANK)
			DrawCard(gdi, m_oGame.m_btFreeCells[i], pt);
		else if(i < m_oGame.GetNumFC())//only draw blanks if we're showing that many
			DrawCard(gdi, CARD_BLANK, pt);
	}

	for(i = 0; i < 4; i++)
	{
		if(rcClip.right < m_rcAceRack[i].left)
			continue;
		if(rcClip.left > m_rcAceRack[i].right)
			continue;

		pt.x = m_rcAceRack[i].left;
		pt.y = m_rcAceRack[i].top;

		if (m_oGame.m_btAceRacks[i] && m_oGame.m_btAceRacks[i] < CARD_BLANK)
			DrawCard(gdi, m_oGame.m_btAceRacks[i], pt);
		else
			DrawCard(gdi, CARD_BLANK+1+i, pt);
	}

	for(i = 0; i < CARD_COLUMNS; i++)
	{
		for(int j = 0; j < MAX_CARDS_PER_COLUMN; j++)
		{
			if(m_oGame.m_btColumns[i][j] != NULL_CARD)
			{
				pt.x = CARD_INDENT + i*(m_sizeCards.cx + CARD_SPACING) + m_iHorizontalOffset;
				pt.y = TOP_INDENT + CARD_INDENT + m_sizeCards.cy + j*GetCardOffset(i);

				//and set the top card rect
				m_rcCards[i].left	= pt.x;
				m_rcCards[i].right	= pt.x + m_sizeCards.cx;
				m_rcCards[i].top	= pt.y;
				m_rcCards[i].bottom = pt.y + m_sizeCards.cy;

                //optimized
                if(IsRectInRect(rcClip, m_rcCards[i]) == FALSE)
                    continue;

				DrawCard(gdi, m_oGame.m_btColumns[i][j], pt);

				//now check for flashing aces
				if(m_bDrawRedAces)
				{
					//if its an ace
					if(m_oGame.m_btColumns[i][j]%13 == 1)
						AlphaFillRect(gdi, m_rcCards[i], RGB(255,0,0), 127);
				}


			}
			else if(j == 0)
			{
				pt.x = CARD_INDENT + i*(m_sizeCards.cx + CARD_SPACING) + m_iHorizontalOffset;
				pt.y = TOP_INDENT + CARD_INDENT + m_sizeCards.cy + j*GetCardOffset(i);
				//and set the top card rect
				m_rcCards[i].left	= pt.x;
				m_rcCards[i].right	= pt.x + m_sizeCards.cx;
				m_rcCards[i].top	= pt.y;
				m_rcCards[i].bottom = pt.y + m_sizeCards.cy;
			}
		}
		
	}
	return TRUE;
}

void CDlgFreeCell::OnNewGame()
{
    //clear up some variables
    m_bCardMoved = FALSE;
    m_bCardSelected = FALSE;
    m_iSelectedCount = -1;
    m_bShowInvalidMove = FALSE;

    if(m_oGame.GetBackgroundColor() == BGCOLOR_Random)
    {
        g_gui->m_eBackground = EnumBackgroundColor(rand()%BGCOLOR_Random);
        g_gui->gdiBackground.Destroy();
        g_gui->gdiMem.Destroy();

        RECT rcClient;
        GetClientRect(m_hWnd, &rcClient);
		HDC dc = GetDC(m_hWnd);
        g_gui->InitGDI(rcClient, dc);
		ReleaseDC(m_hWnd, dc);
    }

	m_oGame.NewGame(TRUE);
	//and toggle the red aces
	HDC hDC = GetDC(m_hWnd);
	RECT rc;
	GetClientRect(m_hWnd, &rc);

	for(int i = 0; i < 3; i++)
	{
		m_bDrawRedAces = TRUE;
		OnPaint(m_hWnd, hDC, rc);
		m_bDrawRedAces = FALSE;
		OnPaint(m_hWnd, hDC, rc);
		Sleep(50);
	}
	ReleaseDC(m_hWnd, hDC);

	KillTimer(m_hWnd, IDT_Time_Timer);
	SetTimer(m_hWnd, IDT_Time_Timer, 1000, NULL);
}

void CDlgFreeCell::PlaySounds(TCHAR* szWave)
{
	if(m_oGame.GetPlaySounds())
		PlaySound(szWave, m_hInst, SND_RESOURCE | SND_ASYNC | SND_NOWAIT);
}

BOOL CDlgFreeCell::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uiMessage)
	{
	case WM_INVALID_MOVE:
		KillTimer(m_hWnd, IDT_InvalidMove_Timer);
		SetTimer(m_hWnd, IDT_InvalidMove_Timer, DISPLAY_MESSAGE_TIME, NULL);
		m_bShowInvalidMove = TRUE;
		InvalidateRect(m_hWnd, &m_rcMsgText, FALSE);
		InvalidateRect(m_hWnd, &m_rcOldSelectedCard, FALSE);//redraw the previously selected card
		break;
	case WM_GAME_LOSE:
		if(m_bMessageBoxUp == FALSE)
		{
			m_bMessageBoxUp = TRUE;
			TCHAR szTemp[STRING_MAX];
			m_oStr->Format(szTemp, m_oStr->GetText(ID(IDS_MSG_NoMoves), m_hInst),
				m_oGame.GetGamesWon(), m_oGame.GetGamesLost(), m_oGame.GetWinPcnt(), m_oGame.GetCurrentStreak(), m_oGame.GetLongestStreak());
			
            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(szTemp, m_hWnd, m_hInst, MB_YESNO))
			{
				m_bMessageBoxUp = FALSE;
				OnNewGame();
			}
			else//go to the menu
			{
				m_bMessageBoxUp = FALSE;			
				LaunchMenu(FALSE);
			}
		}
		break;
	case WM_GAME_WIN:
		if(m_bMessageBoxUp == FALSE)
		{
			//FTW!
			HDC dc = GetDC(m_hWnd);
			AnimateSolitaire(dc);
			ReleaseDC(m_hWnd, dc);
			
			m_bMessageBoxUp = TRUE;
			TCHAR szTemp[STRING_MAX];
			m_oStr->Format(szTemp, m_oStr->GetText(ID(IDS_MSG_Win), m_hInst),
				m_oGame.GetGamesWon(), m_oGame.GetGamesLost(), m_oGame.GetWinPcnt(), m_oGame.GetCurrentStreak(), m_oGame.GetLongestStreak());

            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(szTemp, m_hWnd, m_hInst, MB_YESNO))
			{
				m_bMessageBoxUp = FALSE;
				OnNewGame();
			}
			else//go to the menu
			{
				m_bMessageBoxUp = FALSE;			
				LaunchMenu(FALSE);
			}
			
		}
		break;
	case WM_ANI_COLUMN_ACE:
		AnimateCardtoAceRack((int)wParam, (int)lParam, TRUE);
		break;
	case WM_ANI_FREECELL_ACE:
		AnimateCardtoAceRack((int)wParam, (int)lParam, FALSE);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void CDlgFreeCell::LaunchMenu(BOOL bFirstTime)
{
    ShowMainMenu();
    InvalidateRect(m_hWnd, NULL, FALSE);
}

BOOL CDlgFreeCell::OnMouseMove(HWND hWnd, POINT& pt)
{
#define MOVE_THRESHOLD 3

    if(m_bCardSelected)
    {
        POINT ptTemp = pt;
        ptTemp.x -= m_ptDragOffset.x;
        ptTemp.y -= m_ptDragOffset.y;

        if(abs(pt.x - m_ptDragStart.x) > MOVE_THRESHOLD ||
            abs(pt.y - m_ptDragStart.y) > MOVE_THRESHOLD)
        {
            if(m_bCardMoved == FALSE)
                PlaySounds(_T("IDR_WAVE_PICKUP"));
            m_bCardMoved = TRUE;
            DragCard(m_iSelectedCard, m_iSelectedLocation, ptTemp);
        }
    }
    return TRUE;
}

BOOL CDlgFreeCell::OnLBDColumns(POINT pt, int iCol)
{
    RECT rcTemp;

    rcTemp = m_rcCards[iCol];
    rcTemp.top = m_rcFreeCells[0].bottom + 1;

    //Columns
    if(PtInRect(&rcTemp, pt))
    {
        int iCard;

        if(PtInRect(&m_rcCards[iCol], pt) == FALSE)
        {   //we're not on the top card so we've got some work to do ....
            int iTemp = 1; //at least one from the top
            iTemp += (m_rcCards[iCol].top - pt.y)/GetCardOffset(iCol);

            m_iSelectedCount = iTemp;

            iTemp = m_oGame.GetNumCardsInColumn(iCol)-1 - iTemp;
            iCard = m_oGame.m_btColumns[iCol][iTemp];

            if(m_oGame.IsCardLocked(iCol, iTemp))
                return TRUE;

            m_iSelectedCard[0] = iCard;

            //grab the other cards
            for(int c = 0; c < m_iSelectedCount; c++)
            {
                m_iSelectedCard[c+1] = m_oGame.m_btColumns[iCol][iTemp+c+1];
            }
        }
        else //top card
        {
            iCard = m_oGame.m_btColumns[iCol][m_oGame.GetNumCardsInColumn(iCol)-1];
            m_iSelectedCard[0] = iCard;
            m_iSelectedCount = 0;
        }

        if(iCard < CARD_BLANK)
        {

            m_iSelectedLocation = (int)CL_Column1 + iCol;
            m_bCardSelected = TRUE;
            m_gdiMoveBackground.Destroy();
            m_ptDragOffset.x = pt.x - m_rcCards[iCol].left;
            m_ptDragOffset.y = pt.y - m_rcCards[iCol].top + m_iSelectedCount*GetCardOffset(iCol);//if we're down the stack
        }
        return TRUE;
    }

    return FALSE;
}

BOOL CDlgFreeCell::OnLButtonDown(HWND hWnd, POINT& pt)
{
    //nice ... not really used right now ...
    m_ptDragStart = pt;
    
    //have we clicked on a card??
    int i = 0;

    BOOL bBreak = FALSE;

    for(i = 0; i < 4; i++)
    {
        if(bBreak)
            break;

        //Free Cells
        if(PtInRect(&m_rcFreeCells[i], pt))
        {
            if(m_oGame.m_btFreeCells[i] < CARD_BLANK)
            {
                m_iSelectedCard[0] = m_oGame.m_btFreeCells[i];
                m_iSelectedLocation = (int)CL_FreeCell1 + i;
                m_bCardSelected = TRUE;
                m_gdiMoveBackground.Destroy();
                m_ptDragOffset.x = pt.x - m_rcFreeCells[i].left;
                m_ptDragOffset.y = pt.y - m_rcFreeCells[i].top;
                m_iSelectedCount = 0;
            }
            bBreak = TRUE;

        }
        else if(OnLBDColumns(pt, i))
            bBreak = TRUE;
        else if(OnLBDColumns(pt, i+4))
            bBreak = TRUE;
    }
    return g_gui->OnLButtonDown(hWnd, pt);
}

BOOL CDlgFreeCell::OnLButtonUp(HWND hWnd, POINT& pt)
{
#ifdef WIN32_PLATFORM_PSPC
	m_bDrawSelector = FALSE;
#endif

    if(m_bCardSelected)
    {   //more here later .....
        m_bCardSelected = FALSE;
        
        InvalidateRect(m_hWnd, NULL, FALSE);

        if(m_bCardMoved)
        {
            if(CheckCardDrag(pt))
                PlaySounds(_T("IDR_WAVE_DEAL"));//I want a sound no matter what
            else 
            {
                PlaySounds(_T("IDR_WAVE_PUTDOWN"));
                
            }
            return TRUE;
        }
        
    }


	//are we in the top row ... free cells or ace spots?
	int i;
	int iOldSel = m_iSelector;
	RECT rcTemp;

#define X_INFLATE 1
#define Y_INFLATE TOP_TEXT_INDENT

	for(i = 0; i < 4; i++)
	{
        rcTemp = m_rcFreeCells[i];
        InflateRect(&rcTemp, X_INFLATE, X_INFLATE);//otherwise we inflate into the game field
		//Free Cells
		if(PtInRect(&rcTemp, pt))
		{
			m_iSelector = CL_FreeCell1 + i;
			if(m_oGame.OnFreeCell(i))
			{
				PlaySounds(_T("IDR_WAVE_DEAL"));
				InvalidateRect(m_hWnd, NULL, FALSE);
			}
			else
				UpdateSelector(iOldSel);
			return 0;
		}

        rcTemp = m_rcAceRack[i];
        InflateRect(&rcTemp, X_INFLATE, X_INFLATE);//otherwise we inflate into the game field
		//Ace Rack
		if(PtInRect(&rcTemp, pt))
		{
			m_iSelector = CL_AceRack1 + i;
			if(m_oGame.OnAceRack(i))
			{
				PlaySounds(_T("IDR_WAVE_DEAL"));
				InvalidateRect(m_hWnd, NULL, FALSE);
			}
			else
				UpdateSelector(iOldSel);
			return 0;
		}

        rcTemp = m_rcCards[i];
        InflateRect(&rcTemp, X_INFLATE, Y_INFLATE);
		//Columns
		if(PtInRect(&rcTemp, pt))
		{
			m_iSelector = i;
			if(m_oGame.OnColumn(i))
			{
				PlaySounds(_T("IDR_WAVE_DEAL"));
				InvalidateRect(m_hWnd, NULL, FALSE);
			}
			else
				UpdateSelector(iOldSel);
			return 0;
		}

        rcTemp = m_rcCards[i + 4];
        InflateRect(&rcTemp, X_INFLATE, Y_INFLATE);
		if(PtInRect(&rcTemp, pt))
		{
			m_iSelector = i + 4;
			if(m_oGame.OnColumn(i + 4))
			{
				PlaySounds(_T("IDR_WAVE_DEAL"));
				InvalidateRect(m_hWnd, NULL, FALSE);
			}
			else
				UpdateSelector(iOldSel);
			return 0;
		}
	}

	//if we get this far and we're on the bottom of the screen .. lets unselect
	if(CheckAndDrawPreview(pt))
	{

	}
	else if(pt.y > GetSystemMetrics(SM_CYSCREEN)/2)
	{
		m_oGame.OnTable();
		InvalidateRect(m_hWnd, NULL, FALSE);
	}

    if(PtInRect(&m_rcUndo, pt))
    {
        if(m_oGame.OnUndo())
            PlaySounds(_T("IDR_WAVE_PUTDOWN"));
        InvalidateRect(m_hWnd, NULL, FALSE);
    }

    g_gui->OnLButtonUp(hWnd, pt);


    return TRUE;

	return 0;
}

void CDlgFreeCell::UpdateSelector(int iOldSel)
{
	RECT rcTemp;

#define CARD_EXPAND 2

	if(iOldSel > -1)
	{
		if(iOldSel < CARD_COLUMNS)
			rcTemp = m_rcCards[iOldSel];
		else if(iOldSel < (int)CL_AceRack1)
			rcTemp = m_rcFreeCells[iOldSel-8];
		else if(iOldSel < (int)CL_None)
			rcTemp = m_rcAceRack[iOldSel-12];

		//and expand
		rcTemp.left -= CARD_EXPAND;
		rcTemp.right+= CARD_EXPAND;
		rcTemp.bottom+= CARD_EXPAND;
		rcTemp.top	-= CARD_EXPAND;

		InvalidateRect(m_hWnd, &rcTemp, FALSE);
	}

	if(m_iSelector > -1)
	{
		if(m_iSelector < CARD_COLUMNS)
			rcTemp = m_rcCards[m_iSelector];
		else if(m_iSelector < (int)CL_AceRack1)
			rcTemp = m_rcFreeCells[m_iSelector-8];
		else if(m_iSelector < (int)CL_None)
			rcTemp = m_rcAceRack[m_iSelector-12];

		//and expand
		rcTemp.left -= CARD_EXPAND;
		rcTemp.right+= CARD_EXPAND;
		rcTemp.bottom+= CARD_EXPAND;
		rcTemp.top	-= CARD_EXPAND;

		InvalidateRect(m_hWnd, &rcTemp, FALSE);
	}
}


void CDlgFreeCell::AnimateSolitaire(HDC hDC)
{
    PlaySounds(_T("IDR_WAVE_WIN"));

	POINT ptV;
	POINT pt = {0,0};

	int dxp, dyp;

	int iGravity = rand()%6 + 6;//default is 8
	

	RECT rc;
	GetClientRect(m_hWnd, &rc);

	OnPaint(m_hWnd, hDC, rc);

	dxp		= rc.right;
	dyp		= rc.bottom - m_sizeCards.cy;

	for(int y = 0; y < 13; y++) //number of cards per suit
	{
		for(int x = 0; x < 4; x++) //suits
		{
			pt.x	= m_rcAceRack[x].left;
			pt.y	= m_rcAceRack[x].top;

			ptV.x = rand() % 110 - 65;  // favor up
			
			if(abs(ptV.x) < 20)  // kludge so doesn't bounce forever 
			{
				if(ptV.x < 0)
					ptV.x = -20;
				else
					ptV.x = 20;
			}
			ptV.y = rand() % 110 - 75;

			while(pt.x > - m_sizeCards.cx && pt.x < dxp)
			{
				DrawCard(hDC, (13 - y) + x*13, pt);
				pt.x += ptV.x/10;//speed is here
				pt.y += ptV.y/10;
				ptV.y+= 5;
				if(pt.y > dyp && ptV.y > 0)
				{	//bounce yo
					ptV.y = -(ptV.y*iGravity)/10;
					if(ptV.y != 0)
						PlaySounds(_T("IDR_WAVE_PUTDOWN"));
				}

				if(ShouldAbort())
					return;
			}
		}
	}
	return;
}


BOOL CDlgFreeCell::ShouldAbort()
{
    MSG msg;
    
    Sleep(1);

    if(PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
    {
        switch(LOWORD(msg.message))
        {
        //	case WM_KEYUP:
        case WM_CHAR:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_COMMAND:
        case WM_USER:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_ACTIVATE:
            TranslateMessage((LPMSG)&msg);
            DispatchMessage((LPMSG)&msg);
            return TRUE;
        }
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
    }
	return FALSE;
}

BOOL CDlgFreeCell::OnMenuMenu()
{
    g_gui->wndMenu.ResetContent();


#ifndef NMA
	g_gui->wndMenu.AddItem(ID(IDS_MENU_MoreApps), m_hInst, IDMENU_MoreApps);
#endif
    g_gui->wndMenu.AddItem(ID(IDS_MENU_Options), m_hInst, IDMENU_Options);
    g_gui->wndMenu.AddItem(ID(IDS_MENU_About), m_hInst, IDMENU_About);
    if(g_gui->GetIsSmartPhone() == FALSE)
        g_gui->wndMenu.AddItem(ID(IDS_MENU_Help), m_hInst, IDMENU_Help);
    g_gui->wndMenu.AddSeparator();
    g_gui->wndMenu.AddItem(ID(IDS_MENU_Exit), m_hInst, IDMENU_Exit);

    g_gui->wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        0,rcClient.top,
        rcClient.right*3/4, HEIGHT(rcClient) - HEIGHT(g_gui->rcBottomBar),
        0,0,0,0, ADJUST_Bottom);

    return TRUE;
}


BOOL CDlgFreeCell::OnGameMenu()
{
    TCHAR szText[STRING_MAX];

    g_gui->wndMenu.ResetContent();

    if(m_oGame.GetAutoPlay() == AUTOPLAY_Off) //add an auto play entry
    {
        m_oStr->StringCopy(szText, m_oStr->GetText(ID(IDS_MENU_Autoplay), m_hInst));
        g_gui->wndMenu.AddItem(szText, IDMENU_AutoPlay);
    }

    m_oStr->StringCopy(szText, ID(IDS_MENU_NewGame), STRING_MAX, m_hInst);
    g_gui->wndMenu.AddItem(szText, IDMENU_NewGame);
//    if(g_gui->GetIsSmartPhone() == FALSE)
    {
        m_oStr->StringCopy(szText, ID(IDS_MENU_SelectGame), STRING_MAX, m_hInst);
        g_gui->wndMenu.AddItem(szText, IDMENU_SelectGame);
    }
    m_oStr->StringCopy(szText, ID(IDS_MENU_RestartGame), STRING_MAX, m_hInst);
    g_gui->wndMenu.AddItem(szText, IDMENU_RestartGame, m_oGame.IsGameInPlay()?NULL:FLAG_Grayed);

    g_gui->wndMenu.AddSeparator();

    m_oStr->StringCopy(szText, ID(IDS_MENU_Stats), STRING_MAX, m_hInst);
    g_gui->wndMenu.AddItem(szText, IDMENU_Stats);
    m_oStr->StringCopy(szText, ID(IDS_MENU_ResetStats), STRING_MAX, m_hInst);
    g_gui->wndMenu.AddItem(szText, IDMENU_ResetStats);

    g_gui->wndMenu.AddSeparator();

    m_oStr->StringCopy(szText, ID(IDS_MENU_Undo), STRING_MAX, m_hInst);
    g_gui->wndMenu.AddItem(szText, IDMENU_Undo, m_oGame.CanUndo()?NULL:FLAG_Grayed);

    g_gui->wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.right/4,rcClient.top,
        rcClient.right*3/4, HEIGHT(rcClient) - HEIGHT(g_gui->rcBottomBar),
        0,0,0,0, ADJUST_Bottom);

    return TRUE;

/*	HMENU hMenu			= CreatePopupMenu();

	//and build the menu
	TCHAR szText[STRING_MAX];

	if(m_oGame.GetAutoPlay() == AUTOPLAY_Off) //add an auto play entry
	{
		m_oStr->StringCopy(szText, _T("Autoplay"));
		AppendMenu(hMenu, MF_STRING, IDMENU_AutoPlay, szText);
		AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
	}

	m_oStr->StringCopy(szText, IDS_MENU_NewGame, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_NewGame, szText);
	m_oStr->StringCopy(szText, IDS_MENU_SelectGame, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_SelectGame, szText);
	m_oStr->StringCopy(szText, IDS_MENU_RestartGame, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING|(m_oGame.IsGameInPlay()?NULL:MF_GRAYED), IDMENU_RestartGame, szText);

	AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);

	m_oStr->StringCopy(szText, IDS_MENU_Stats, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_Stats, szText);
	m_oStr->StringCopy(szText, IDS_MENU_ResetStats, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_ResetStats, szText);

	AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);

	m_oStr->StringCopy(szText, IDS_MENU_Undo, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING|(m_oGame.CanUndo()?NULL:MF_GRAYED), IDMENU_Undo, szText);

		
	POINT pt;
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	pt.x	= rc.right - GetSystemMetrics(SM_CXVSCROLL);
	pt.y	= GetSystemMetrics(SM_CYSCREEN) - rc.top - HEIGHT(g_gui->rcBottomBar);

	TrackPopupMenu(hMenu, TPM_RIGHTALIGN|TPM_BOTTOMALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
	
	if(hMenu)
		DestroyMenu(hMenu);

	return TRUE;*/

}

#define SHADOW_FADE (IsVGA()?8:4)

void CDlgFreeCell::DragCard(int* iCard, int iLocation, POINT pt)
{
    HDC dc = NULL;
    RECT rc;
    static RECT rcPrev;
    GetClientRect(m_hWnd, &rc);
    POINT ptTemp;

    static int iSOffSet = CARD_OFFSETS/3;

    if(m_gdiMoveBackground.GetDC() == NULL || m_gdiMoveBackground.GetWidth() != GetSystemMetrics(SM_CXSCREEN))
    {
        m_gdiMoveBackground.Create(g_gui->gdiMem.GetDC(), g_gui->gdiMem.GetWidth(), g_gui->gdiMem.GetHeight());
        
        OnPaint(m_hWnd, m_gdiMoveBackground.GetDC(), rc);
        //draw the free cell blank
        if(iLocation > CL_Column8)
        {
            ptTemp.x = m_rcFreeCells[iLocation - CL_FreeCell1].left;
            ptTemp.y = m_rcFreeCells[iLocation - CL_FreeCell1].top;
            DrawCard(m_gdiMoveBackground, CARD_BLANK, ptTemp);
            rcPrev = m_rcFreeCells[iLocation - CL_FreeCell1];
        }
        else //blank the column and redraw
        {   
            rcPrev = m_rcCards[iLocation];
            rcPrev.top = m_rcFreeCells[0].bottom + 1;
            ptTemp.x = m_rcCards[iLocation].left;
            ptTemp.y = m_rcCards[iLocation].top;
            Draw(m_gdiMoveBackground, rcPrev, g_gui->gdiBackground, rcPrev.left, rcPrev.top);

            //now draw the column back in ....
            ptTemp.x = CARD_INDENT + iLocation*(m_sizeCards.cx + CARD_SPACING) + m_iHorizontalOffset;
            for(int i = 0; i < m_oGame.GetNumCardsInColumn(iLocation) - 1 - m_iSelectedCount; i++)
            {   
                // adjust here for stacks
                ptTemp.y = TOP_INDENT + CARD_INDENT + m_sizeCards.cy + i*GetCardOffset(iLocation);
                DrawCard(m_gdiMoveBackground, m_oGame.m_btColumns[iLocation][i], ptTemp); 
            }

            //init shadow
            m_imgShadow.SetSize(m_sizeCards.cx+SHADOW_FADE, m_sizeCards.cy + m_iSelectedCount*3*iSOffSet+SHADOW_FADE);
        }
    }

    rc.left = pt.x;
    rc.top = pt.y;
    rc.right = rc.left + m_sizeCards.cx;
    rc.bottom = rc.top + m_sizeCards.cy;

    //for the shadow
    rc.right += (iSOffSet + SHADOW_FADE);
    rc.bottom += (iSOffSet + SHADOW_FADE);

    UnionRect(&rcPrev, &rcPrev, &rc);

    Draw(g_gui->gdiMem, rcPrev, m_gdiMoveBackground, rcPrev.left, rcPrev.top);


    //draw the shadow
    m_imgShadow.DrawImage(g_gui->gdiMem, pt.x + iSOffSet, pt.y + iSOffSet);

    ptTemp = pt;
    //draw the card(s)
    for(int i = 0; i < m_iSelectedCount + 1; i++)
    {
        DrawCard(g_gui->gdiMem, iCard[i], ptTemp);
        ptTemp.y += iSOffSet*3;//hack ... save some CPU .. make sure its inline with what's above
        rc.bottom += iSOffSet*3;
    }


       
    // and finally, push it the the screen
    //get screen DC
    dc = GetDC(m_hWnd);	

    BitBlt( dc,
        rcPrev.left, rcPrev.top, 
        WIDTH(rcPrev),
        HEIGHT(rcPrev),
        g_gui->gdiMem.GetDC(),
        rcPrev.left,
        rcPrev.top,
        SRCCOPY);

    //clean up and call it a day
    ReleaseDC(m_hWnd, dc);

    rcPrev = rc;//for the next pass
}

void CDlgFreeCell::AnimateCardtoAceRack(int iCard, int iAceRack, BOOL bFromColumn)
{
	if(iCard < 0 || iCard > 7 || iAceRack < 0 || iAceRack > 3)
		return;

	if(m_oGame.ShowAnimations() == FALSE)
		return;

    //play the sound... we want this as early as possible
	PlaySounds(_T("IDR_WAVE_DEAL"));

    if(m_gdiMoveBackground.GetDC() == NULL || m_gdiMoveBackground.GetWidth() != GetSystemMetrics(SM_CXSCREEN))
    {
        m_gdiMoveBackground.Create(g_gui->gdiMem.GetDC(), g_gui->gdiMem.GetWidth(), g_gui->gdiMem.GetHeight());
    }

	HDC dcScreen;
	RECT rc;
	POINT pt = {0,0};
	int iAceCard = m_oGame.m_btAceRacks[iAceRack];
	int iOldAceCard = 0;

	POINT ptTo;
	ptTo.x = m_rcAceRack[iAceRack].left;
	ptTo.y = m_rcAceRack[iAceRack].top;

	if((iAceCard-1)%13 == 0)//is it an ace?? 1, 14, 27, or 40
		iOldAceCard = CARD_BLANK;
	else
		iOldAceCard = iAceCard - 1;
	
	CIssGDIEx gdiCard;

	GetClientRect(m_hWnd, &rc);

    OnPaint(m_hWnd, m_gdiMoveBackground.GetDC(), rc);

    // redraw the previous ace rack card
    DrawCard(m_gdiMoveBackground, iOldAceCard, ptTo);
 

	// and finally, push it the the screen
	//get screen DC
	dcScreen = GetDC(m_hWnd);	

	BitBlt( dcScreen,
		rc.left, rc.top, 
		rc.right - rc.left,
		rc.bottom - rc.top,
		m_gdiMoveBackground.GetDC(),
		rc.left,
		rc.top,
		SRCCOPY);

	//create card buffer .. ie the card to animate
	gdiCard.Create(dcScreen, m_sizeCards.cx, m_sizeCards.cy, FALSE);

	//copy the correct card into the buffer
	DrawCard(gdiCard, m_oGame.m_btAceRacks[iAceRack], pt);

	if(bFromColumn)
	{
		pt.x = m_rcCards[iCard].left;
		pt.y = m_rcCards[iCard].top;
	}
	else //from freecell
	{
		pt.x = m_rcFreeCells[iCard].left;
		pt.y = m_rcFreeCells[iCard].top;
	}
	
	//now animate using the temp buffer and card buffer for x frames
	AnimateFromTo( dcScreen, 
		m_gdiMoveBackground, 
		gdiCard, 
		pt, 
		ptTo, 
		0,0, 
		m_sizeCards.cx, m_sizeCards.cy, 
		NUMBER_OF_FRAMES);

//	OnPaint(m_hWnd, dcScreen, rc);

	//clean up and call it a day
	ReleaseDC(m_hWnd, dcScreen);
}

BOOL CDlgFreeCell::CheckAndDrawPreview(POINT pt)
{
	//see if we're on a card
	RECT rcTemp;
	POINT ptTemp;
	ptTemp.x = -1;
	ptTemp.y = -1;

	int iColumn		= -1;
	int iCardIndex	= -1;
	
	if(pt.y < TOP_INDENT + CARD_INDENT + m_sizeCards.cy)
		return FALSE;

	for(int i = 0; i < CARD_COLUMNS; i++)
	{
		for(int j = 0; j < MAX_CARDS_PER_COLUMN; j++)
		{
			if(m_oGame.m_btColumns[i][j] != NULL_CARD)
			{
				ptTemp.x = CARD_INDENT + i*(m_sizeCards.cx + CARD_SPACING) + m_iHorizontalOffset;
				ptTemp.y = TOP_INDENT + CARD_INDENT + m_sizeCards.cy + j*GetCardOffset(i);

				//and set the top card rect
				rcTemp.left		= ptTemp.x;
				rcTemp.right	= ptTemp.x + m_sizeCards.cx;
				rcTemp.top		= ptTemp.y;
				rcTemp.bottom	= ptTemp.y + GetCardOffset(i);

				if(PtInRect(&rcTemp, pt))
				{
					ptTemp.x	= rcTemp.left;
					ptTemp.y	= rcTemp.top;
					iColumn		= i;
					iCardIndex	= j;
					goto EndForLoops;
				}
			}
		}
	}
	return FALSE;

EndForLoops:

	//get the screen DC
	HDC dcScreen = GetDC(m_hWnd);

	int iCard = m_oGame.m_btColumns[iColumn][iCardIndex];

	//draw the card
	DrawCard(dcScreen, iCard, ptTemp);

	//release the screen DC
	ReleaseDC(m_hWnd, dcScreen);

	//set the redraw timer
	SetTimer(m_hWnd, IDT_Preview_Timer, PREVIEW_TIME, NULL);


	return TRUE;
}

BOOL CDlgFreeCell::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL bRet = UNHANDLED;
#ifdef UNDER_CE
    HWND hWndSip = NULL;

    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
    {	
        KillTimer(m_hWnd, IDT_Time_Timer);
        if(m_oGame.IsGameInPlay())
            SetTimer(m_hWnd, IDT_Time_Timer, 1000, NULL);
        
        //this should only exist when using a smartphone in landscape orientation (for now anyways)
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
    else if(LOWORD(wParam) == WA_INACTIVE)
    {
        KillTimer(m_hWnd, IDT_Time_Timer);
    }
#endif
    return bRet;
}



BOOL CDlgFreeCell::ShowSplashScreen()
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

            break;
        case WM_LOAD_Fonts:
            break;
        case WM_LOAD_Background:

            break;
        case WM_lOAD_Images:
            LoadImages(dlgSplashScreen.GetWnd(), 100);
            m_oGame.Init(m_hWnd);
            SendMessage(dlgSplashScreen.GetWnd(), WM_SET_Percent, 100, 0);
            break;
        }

        if(msg.message == WM_lOAD_Images)
            break;
    }

    ShowWindow(dlgSplashScreen.GetWnd(), SW_HIDE);
    ShowMainMenu();
    return TRUE;
}

BOOL CDlgFreeCell::ShowMainMenu()
{
    CDlgMainMenu	dlgMenu;
    dlgMenu.Init(m_oGame.IsGameInPlay(), &m_oGame);
    UINT uiResult = (UINT)dlgMenu.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);

    switch(uiResult)
    {
    case IDMENU_NewGame:
        m_oGame.Init(m_hWnd);
        m_oGame.NewGame(TRUE);
        break;
    case IDMENU_Resume:
        m_oGame.Init(m_hWnd);
        break;
    case IDMENU_Exit:
        PostQuitMessage(0);
        return TRUE;
    }
    m_oGame.SaveRegistry();

    return TRUE;
}

EnumCardLocations CDlgFreeCell::GetLocation(POINT pt)
{
    int iDest = -1;
    for(int i = 0; i < 4; i++)
    {
        if(iDest != -1)
            break;

        if(PtInRect(&m_rcCards[i], pt))
            iDest = i;
        else if(PtInRect(&m_rcCards[i+4], pt))
            iDest = i + 4;
        else if(PtInRect(&m_rcFreeCells[i], pt))
            iDest = i + (int)CL_FreeCell1;
        else if(PtInRect(&m_rcAceRack[i], pt))
            iDest = i + (int)CL_AceRack1;
    }

    return (EnumCardLocations)iDest;
}

BOOL CDlgFreeCell::CheckCardDrag(POINT ptEnd)
{
    m_bCardMoved = FALSE;//done with that ....

    POINT ptCorrected;
    ptCorrected.x = ptEnd.x - m_ptDragOffset.x + WIDTH(m_rcCards[0])/2;
    ptCorrected.y = ptEnd.y - m_ptDragOffset.y + HEIGHT(m_rcCards[0])/2;

    int iDest = (int)GetLocation(ptCorrected);

    if(iDest == -1)
        return FALSE;

    POINT ptTemp = m_ptDragStart;
    //m_ptDragStart.y += m_iSelectedCount*CARD_OFFSETS;
    m_ptDragStart.y += m_iSelectedCount*GetCardOffset(GetLocation(m_ptDragStart));

    int iStart = (int)GetLocation(m_ptDragStart);
    if(iStart == -1)
        return FALSE;

    return m_oGame.MoveCard(iStart, iDest, m_iSelectedCount);
}

int CDlgFreeCell::GetCardOffset(int iColumn /* = -1 */)
{
    if(iColumn == -1)
        return CARD_OFFSETS;
    else if(m_oGame.GetNumCardsInColumn(iColumn) > m_iMaxCardsPerColumn)
        return CARD_OFFSETS_S;
    return CARD_OFFSETS;
}