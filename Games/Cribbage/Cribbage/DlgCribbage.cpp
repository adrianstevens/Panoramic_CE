#include "StdAfx.h"
#include "DlgCribbage.h"
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

#define REG_KEY 0


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

CDlgCribbage::CDlgCribbage(void)
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
//    g_gui->m_eBackground = m_oGame.GetBackgroundColor();

//    if(m_oGame.GetBackgroundColor() == BGCOLOR_Random)
//        g_gui->m_eBackground = (EnumBackgroundColor)(rand()%BGCOLOR_Random);
}



CDlgCribbage::~CDlgCribbage(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
}

BOOL CDlgCribbage::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

void CDlgCribbage::ResizeImages()
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

BOOL CDlgCribbage::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

void CDlgCribbage::LoadImages(HWND hWndSplash, int iMaxPercent)
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

BOOL CDlgCribbage::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
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

BOOL CDlgCribbage::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
		//m_oGame.OnUndo();
		break;
	case _T('r'):
	case _T('R'):
//		m_oGame.NewGame(FALSE);
		break;
	case _T('c'):
//		m_oGame.CheckGameState();
		break;
	case _T('e'):

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

BOOL CDlgCribbage::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgCribbage::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
		break;
	case VK_RIGHT:
		break;
	case VK_LEFT:
		break;
	case VK_UP:
	case VK_DOWN:
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

BOOL CDlgCribbage::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(wParam == IDT_Time_Timer)
	{	// so it doesn't go dark
		SystemIdleTimerReset();

		//if(!m_oGame.IsGameInPlay())
		//	KillTimer(m_hWnd, IDT_Time_Timer);
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

BOOL CDlgCribbage::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return CIssWnd::OnDestroy(hWnd, wParam, lParam);
}

BOOL CDlgCribbage::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
	/*	if(m_oGame.IsGameInPlay() && m_oGame.GetCardsRemaining() < 52) //won't prompt if they haven't done anything yet
		{	
            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(m_oStr->GetText(ID(IDS_MSG_ResignNew), m_hInst), m_hWnd, m_hInst, MB_YESNO))
                OnNewGame();
		}
		else
			OnNewGame();*/
		

		break;
	case IDMENU_RestartGame:

		break;
	case IDMENU_SelectGame:
		
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case IDMENU_Stats:

		break;
	case IDMENU_ResetStats:
     
		break;
	case IDMENU_Undo:
		
		break;
	case IDMENU_AutoPlay:
		
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

BOOL CDlgCribbage::DrawSelector(CIssGDIEx& gdi, RECT& rcClient)
{

	return TRUE;
}

BOOL CDlgCribbage::DrawSelected(CIssGDIEx& gdi, RECT& rcClip)
{
		
	return TRUE;
}

BOOL CDlgCribbage::DrawTopBar(CIssGDIEx& gdi, RECT& rcClip)
{
/*    if(rcClip.top > TOP_INDENT)
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
*/

    return TRUE;
}

BOOL CDlgCribbage::DrawText(HDC hDC, RECT& rcClient)
{

//    TCHAR szTemp[STRING_NORMAL];

	if(m_bShowInvalidMove && IsRectInRect(rcClient, m_rcMsgText))
	{
		::DrawText(hDC, m_oStr->GetText(ID(IDS_MSG_InvalidMove), m_hInst), m_rcMsgText, DT_RIGHT | DT_VCENTER, m_hFontText, 0xDDDDDD);
	}


	return TRUE;
}

void CDlgCribbage::DrawCard(CIssGDIEx& gdi, int iCard, POINT ptLocation)
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
       
        Draw(gdi, ptLocation.x, ptLocation.y, 
            m_sizeCards.cx,
            m_sizeCards.cy, 
            m_gdiCards[(iCard-1)/13],
            iValue*m_sizeCards.cx,
            0);
	}
}


void CDlgCribbage::DrawCard(HDC hDC, int iCard, POINT ptLocation)
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


BOOL CDlgCribbage::DrawCards(CIssGDIEx& gdi, RECT& rcClip)
{
	//make sure the card GDIs exist
	if(m_gdiCards[0].GetDC() == NULL)
		return FALSE;

	int redraw = -1;

	int i = 0;
//	POINT pt;
	
	return TRUE;
}

void CDlgCribbage::OnNewGame()
{
 
}

void CDlgCribbage::PlaySounds(TCHAR* szWave)
{
//	if(m_oGame.GetPlaySounds())
//		PlaySound(szWave, m_hInst, SND_RESOURCE | SND_ASYNC | SND_NOWAIT);
}

BOOL CDlgCribbage::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
/*	switch(uiMessage)
	{
	default:
		return FALSE;
	}*/
	return TRUE;
}

void CDlgCribbage::LaunchMenu(BOOL bFirstTime)
{
    ShowMainMenu();
    InvalidateRect(m_hWnd, NULL, FALSE);
}

BOOL CDlgCribbage::OnMouseMove(HWND hWnd, POINT& pt)
{

    return TRUE;
}


BOOL CDlgCribbage::OnLButtonDown(HWND hWnd, POINT& pt)
{
    return g_gui->OnLButtonDown(hWnd, pt);
}

BOOL CDlgCribbage::OnLButtonUp(HWND hWnd, POINT& pt)
{
    return TRUE;

}

void CDlgCribbage::UpdateSelector(int iOldSel)
{
	
}



BOOL CDlgCribbage::ShouldAbort()
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

BOOL CDlgCribbage::OnMenuMenu()
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


BOOL CDlgCribbage::OnGameMenu()
{
    TCHAR szText[STRING_MAX];

    g_gui->wndMenu.ResetContent();


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

    g_gui->wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.right/4,rcClient.top,
        rcClient.right*3/4, HEIGHT(rcClient) - HEIGHT(g_gui->rcBottomBar),
        0,0,0,0, ADJUST_Bottom);

    return TRUE;
}

BOOL CDlgCribbage::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
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



BOOL CDlgCribbage::ShowSplashScreen()
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

BOOL CDlgCribbage::ShowMainMenu()
{
    CDlgMainMenu	dlgMenu;
/*    dlgMenu.Init(m_oGame.IsGameInPlay(), &m_oGame);
    UINT uiResult = (UINT)dlgMenu.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);

    switch(uiResult)
    {
    case IDMENU_NewGame:

        break;
    case IDMENU_Resume:

        break;
    case IDMENU_Exit:

        return TRUE;
    }*/


    return TRUE;
}
