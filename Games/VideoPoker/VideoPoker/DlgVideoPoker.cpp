#include "StdAfx.h"
#include "DlgVideoPoker.h"
#include "resource.h"
#include "IssRegistry.h"
#include "IssCommon.h"
#include "IssGDIFx.h"
#include "IssGDIDraw.h"
#include "DlgStats.h"
#include "DlgOptions.h"
#include "DlgMsgBox.h"
#include "DlgSplashScreen.h"
#include "DlgAbout.h"
#include "DlgMainMenu.h"

#define REG_KEY _T("SOFTWARE\\Pano\\VideoPoker")

#define NUMBER_OF_FRAMES	20

#define TOP_INDENT		(IsVGA()?53:27)
#define TOP_TEXT_INDENT (IsVGA()?12:6)
#define SELECTOR_EXPANSE (IsVGA()?10:5)
#define SMSELECTOR_EXPANSE (IsVGA()?5:2)

#define CARD_SPACING (IsVGA()?11:4)
#define CARD_INDENT (IsVGA()?10:4)//should be about right ... can bump both by 1 if you do choose

#define SMCARD_SPACING (IsVGA()?3:1)
#define SMCARD_INDENT (IsVGA()?5:2)

#define CARD_HEIGHT (IsVGA()?136:69)
#define CARD_WIDTH (IsVGA()?83:43)
#define SMCARD_HEIGHT (IsVGA()?69:35)
#define SMCARD_WIDTH (IsVGA()?43:22)

#define WM_BTN_BetOne   WM_USER + 100
#define WM_BTN_BetMax   WM_USER + 101
#define WM_BTN_Menu     WM_USER + 102
#define WM_BTN_X2       WM_USER + 103
#define WM_Lose         WM_USER + 150
#define WM_BrokeBank    WM_USER + 151
#define WM_BTN_Hold		WM_USER + 200
#define WM_BTN_Deal		WM_USER + 300
#define WM_ANI_WIN		WM_USER + 400

#define IDT_Deal		100	
#define IDT_DelayReset	101
#define IDT_DelayText	102

#define DEAL_DELAY		250


#define REG_Bank		_T("Bank")
#define REG_Bet			_T("Bet")
#define REG_PS			_T("PlaySounds")
#define REG_BG			_T("Background")
#define REG_GT			_T("GameType")
#define REG_BA          _T("BetAmount")
#define REG_MP          _T("Multiplay")
#define REG_QP          _T("Quickplay")
#define REG_AH          _T("Autohold")
#define REG_BB          _T("Broke")

extern CObjGui* g_gui;

CDlgVideoPoker::CDlgVideoPoker(void)
:m_hFontText(NULL)
,m_bShowSplashScreen(FALSE)
,m_bDrawSelector(TRUE)
,m_iSelector(0)
,m_bMessageBoxUp(FALSE)
,m_iHorizontalOffset(0)
,m_iCurrentSelectedBtn(0)
,m_bSelHold(FALSE)
,m_oCalc(NULL)
,m_bQuickPlay(FALSE)
,m_eBGColor(BGCOLOR_Blue)
{
	ZeroMemory(&m_sizeWindow, sizeof(SIZE));

    // create our fonts ... should be find on 128 DPI
    m_hFontText				= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*13/32, FW_BOLD, TRUE);
    m_hFontPayout       	= CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*11/32, FW_NORMAL, TRUE);
	m_hFontTitle            = CIssGDIEx::CreateFont(GetSystemMetrics(SM_CXICON)*17/32, FW_BOLD, TRUE);

    //used for the payout table
    m_oCalc = m_oGame.GetScoreCalc();
    m_oGame.Reset();

    // read values from the registry
    DWORD dwVal	= 0;

    if(S_OK == GetKey(REG_KEY, REG_Bank, dwVal))
        m_oGame.SetBank((int)dwVal); 
    
    if(S_OK == GetKey(REG_KEY, REG_Bet, dwVal))
        m_oGame.SetBet((int)dwVal);

    //this should always be 1...and we reshuffle every hand
    m_oGame.SetNumDecks(1);

    if(S_OK == GetKey(REG_KEY, REG_PS, dwVal))
        m_bPlaySounds = (BOOL)dwVal;
    else
        m_bPlaySounds = TRUE;

    if(S_OK == GetKey(REG_KEY, REG_BB, dwVal))
        m_oGame.m_bBankBroken = (BOOL)dwVal;

    if(S_OK == GetKey(REG_KEY, REG_QP, dwVal))
        m_bQuickPlay = (BOOL)dwVal;
    else
        m_bQuickPlay = FALSE;

    if(S_OK == GetKey(REG_KEY, REG_AH, dwVal))
        m_oGame.m_bAutoHold = (BOOL)dwVal;

    if(S_OK == GetKey(REG_KEY, REG_BG, dwVal))
        m_eBGColor = (EnumBackgroundColor)dwVal;
    else
        m_eBGColor = BGCOLOR_Blue;

    if(S_OK == GetKey(REG_KEY, REG_GT, dwVal))
        m_oGame.SetGameType((EnumPokerType)dwVal);

    if(S_OK == GetKey(REG_KEY, REG_BA, dwVal))
        m_oGame.SetBetAmount((EnumBetAmounts)dwVal);

    if(S_OK == GetKey(REG_KEY, REG_MP, dwVal))
        m_oGame.SetMultiplay((EnumMultiPlay)dwVal);
    
	
    //stats
    if(S_OK == GetKey(REG_KEY, _T("HiBank"), dwVal))
        m_oGame.m_iHighestBank = (int)dwVal;
    if(S_OK == GetKey(REG_KEY, _T("HiWin"), dwVal))
        m_oGame.m_iBiggestWin = (int)dwVal;

    TCHAR szTemp[STRING_NORMAL];
    TCHAR szDouble[STRING_NORMAL];

    for (int i = 0; i < 6; i++)
    {
        m_oStr->Format(szTemp, _T("HandsPlayed%i"), i);
        if(S_OK == GetKey(REG_KEY, szTemp, dwVal))
            m_oGame.m_iNumHandsPlayed[i] = (int)dwVal;

        m_oStr->Format(szTemp, _T("Payout%i"), i);
        if(S_OK == GetKey(REG_KEY, szTemp, szDouble, STRING_NORMAL))
            m_oGame.m_dbPayout[i] = m_oStr->StringToDouble(szDouble);        
        
    }
    
    
    
    
    if(g_gui)
		g_gui->m_eBackground = m_eBGColor;


}

CDlgVideoPoker::~CDlgVideoPoker(void)
{
	CIssGDIEx::DeleteFont(m_hFontText);
    CIssGDIEx::DeleteFont(m_hFontPayout);
	CIssGDIEx::DeleteFont(m_hFontTitle);

    SaveRegistry();


}

void CDlgVideoPoker::SaveRegistry()
{
    // write values to the registry
    DWORD dwVal	= (DWORD)m_oGame.GetBank();
    SetKey(REG_KEY, REG_Bank, dwVal);
    dwVal		= (DWORD)m_oGame.GetBet();
    SetKey(REG_KEY, REG_Bet , dwVal);
    dwVal		= (DWORD)m_bPlaySounds;
    SetKey(REG_KEY, REG_PS, dwVal);
    dwVal		= (DWORD)m_bQuickPlay;
    SetKey(REG_KEY, REG_QP, dwVal);
    dwVal		= (DWORD)m_oGame.m_bBankBroken;
    SetKey(REG_KEY, REG_BB, dwVal);
    dwVal		= (DWORD)m_oGame.m_bAutoHold;
    SetKey(REG_KEY, REG_AH, dwVal);
    dwVal		= (DWORD)m_eBGColor;
    SetKey(REG_KEY, REG_BG, dwVal);
    dwVal		= (DWORD)m_oGame.GetGameType();
    SetKey(REG_KEY, REG_GT, dwVal);
    dwVal       = (DWORD)m_oGame.GetBetAmount();
    SetKey(REG_KEY, REG_BA, dwVal);
    dwVal       = (DWORD)m_oGame.GetMultiplay();
    SetKey(REG_KEY, REG_MP, dwVal);




    dwVal       = (DWORD)m_oGame.m_iHighestBank;
    SetKey(REG_KEY, _T("HiBank"), dwVal);
    dwVal       = (DWORD)m_oGame.m_iBiggestWin;
    SetKey(REG_KEY, _T("HiWin"), dwVal);

    TCHAR szTemp[STRING_NORMAL];
    TCHAR szDouble[STRING_NORMAL];

    for (int i = 0; i < 6; i++)
    {
        m_oStr->Format(szTemp, _T("HandsPlayed%i"), i);
        dwVal = (DWORD)m_oGame.m_iNumHandsPlayed[i];
        SetKey(REG_KEY, szTemp, dwVal);

        m_oStr->DoubleToString(szDouble, m_oGame.m_dbPayout[i]);
        m_oStr->Format(szTemp, _T("Payout%i"), i);

        SetKey(REG_KEY, szTemp, szDouble, STRING_NORMAL);
    }

}

void CDlgVideoPoker::CalcCardPositions()
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    int iVert = (GetSystemMetrics(SM_CYSCREEN)%240)/3;

    if(m_oGame.GetMultiplay() > MPLAY_Off)
        iVert = 0;

    static int iIndent = IsVGA()?10:5;

    for(int i = 0; i < 5; i++)
    {
        m_rcCards[i].bottom = m_rcLowerText.top - iIndent - iVert;
        m_rcCards[i].top = m_rcCards[i].bottom - CARD_HEIGHT;
        m_rcCards[i].left = m_iHorizontalOffset + CARD_INDENT + i*(CARD_SPACING + CARD_WIDTH);
        m_rcCards[i].right = m_rcCards[i].left + CARD_WIDTH;


        //start in the bottom right
        m_rcCardsM[1][i].bottom = m_rcCards[i].top - iIndent;
        m_rcCardsM[1][i].top = m_rcCardsM[1][i].bottom - SMCARD_HEIGHT;
        m_rcCardsM[1][i].left = m_iHorizontalOffset + SMCARD_INDENT + i*(SMCARD_SPACING + SMCARD_WIDTH);
        m_rcCardsM[1][i].right = m_rcCardsM[1][i].left + SMCARD_WIDTH;

        m_rcCardsM[0][i].bottom = m_rcCardsM[1][i].bottom;
        m_rcCardsM[0][i].top = m_rcCardsM[1][i].top;
        m_rcCardsM[0][i].left = WIDTH(rcClient)/2 + SMCARD_INDENT + i*(SMCARD_SPACING + SMCARD_WIDTH);
        m_rcCardsM[0][i].right = m_rcCardsM[0][i].left + SMCARD_WIDTH;
        //done
        m_rcCardsM[3][i].bottom = m_rcCardsM[1][i].top - iIndent;
        m_rcCardsM[3][i].top = m_rcCardsM[3][i].bottom - SMCARD_HEIGHT;
        m_rcCardsM[3][i].left = m_rcCardsM[1][i].left;
        m_rcCardsM[3][i].right = m_rcCardsM[1][i].right;
        //done
        m_rcCardsM[2][i].bottom = m_rcCardsM[0][i].top - iIndent;
        m_rcCardsM[2][i].top = m_rcCardsM[2][i].bottom - SMCARD_HEIGHT;
        m_rcCardsM[2][i].left = m_rcCardsM[0][i].left;
        m_rcCardsM[2][i].right = m_rcCardsM[0][i].right;

        //easy enough to add more ...
    }

    m_rcDisplay = rcClient;
    if(m_oGame.GetMultiplay() == MPLAY_Off)
    {    //display text 
        m_rcDisplay.bottom = m_rcCards[0].top - 2*iIndent;
        m_rcDisplay.top = m_rcDisplay.bottom - GetSystemMetrics(SM_CXSMICON);
        SetRect(&m_rcCardArea, m_rcCards[0].left, m_rcCards[0].top - CARD_INDENT, m_rcCards[4].right, m_rcCards[4].bottom);
    }
    else if(m_oGame.GetMultiplay() == MPLAY_3)
    {
        m_rcDisplay.bottom = m_rcCardsM[0][0].top - 2*iIndent;
        m_rcDisplay.top = m_rcDisplay.bottom - GetSystemMetrics(SM_CXSMICON);
        SetRect(&m_rcCardArea, m_rcCardsM[1][0].left, m_rcCardsM[0][0].top, m_rcCardsM[0][4].right, m_rcCards[4].bottom);
    }
    else if(m_oGame.GetMultiplay() == MPLAY_5)
    {
        m_rcDisplay.bottom = m_rcCardsM[2][0].top - 2*iIndent;
        m_rcDisplay.top = m_rcDisplay.bottom - GetSystemMetrics(SM_CXSMICON);
        SetRect(&m_rcCardArea, m_rcCardsM[1][0].left, m_rcCardsM[2][0].top, m_rcCardsM[0][4].right, m_rcCards[4].bottom);
    }
    
    if(m_rcDisplay.top < 0)//correct for landscape
    {   
        m_rcDisplay.top = rcClient.top;
        m_rcDisplay.bottom = m_rcCardArea.top;
    }

    InflateRect(&m_rcCardArea, SELECTOR_EXPANSE, SELECTOR_EXPANSE);
  
}

BOOL CDlgVideoPoker::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

#define SCORE_WIDTH (IsVGA()?114:60)

	// first check if we need to reload the images first
	if(m_sizeWindow.cx	== (rcClient.right - rcClient.left)&&
		m_sizeWindow.cy	== (rcClient.bottom - rcClient.top))
		return TRUE;

    //static int iIndent = GetSystemMetrics(SM_CXSMICON)/3;
    static int iIndent = (IsVGA()?10:5);
    static int iScoreIn = (IsVGA()?4:2);

    RECT rcTemp;
    HDC dc = NULL;
    dc = GetDC(hWnd);

	// update our size structure
	m_sizeWindow.cx		= rcClient.right - rcClient.left;
	m_sizeWindow.cy		= rcClient.bottom - rcClient.top;

    //bottom to top
    m_rcMenu = rcClient;
    m_rcMenu.top = rcClient.bottom - (IsVGA()?91:46);

    //lower bar info during game
    m_rcLowerText = rcClient;
    m_rcLowerText.bottom = m_rcMenu.top;
    m_rcLowerText.top = m_rcLowerText.bottom - (IsVGA()?38:19);

    SetRect(&rcTemp, 0, 0, 0, 0);
    ::DrawText(dc, _T("0000"), rcTemp, DT_CALCRECT, m_hFontText);
    m_rcBetText = m_rcLowerText;
    m_rcBetText.left = m_rcLowerText.left + iIndent;
    m_rcBetText.right = m_rcBetText.left + WIDTH(rcTemp);

    m_rcBetAmount = m_rcLowerText;
    m_rcBetAmount.left = m_rcBetText.right + iIndent;
    m_rcBetAmount.right = m_rcBetAmount.left + SCORE_WIDTH;
    InflateRect(&m_rcBetAmount, 0, -2);

    //now from the right side
    SetRect(&rcTemp, 0, 0, 0, 0);
    ::DrawText(dc, _T("Credits:"), rcTemp, DT_CALCRECT, m_hFontText);
    m_rcCreditsAmount = m_rcLowerText;
    m_rcCreditsAmount.right = m_rcLowerText.right - iIndent;
    m_rcCreditsAmount.left = m_rcCreditsAmount.right - SCORE_WIDTH;
    InflateRect(&m_rcCreditsAmount, 0, -2);

    m_rcCreditsText = m_rcLowerText;
    m_rcCreditsText.right = m_rcCreditsAmount.left - iIndent;
    m_rcCreditsText.left = m_rcCreditsText.right - WIDTH(rcTemp);

    
    //lets lay out some cards
    m_iHorizontalOffset = (WIDTH(rcClient) - 4*CARD_SPACING - 5*CARD_WIDTH)/2;
    //we're just going to manually place these bad boys
    //we'll indent this much to always fit
    m_iHorizontalOffset = (GetSystemMetrics(SM_CXSCREEN) - (IsVGA()?480:240))/2;

    CalcCardPositions();    
   
    m_rcBtn[GBTN_BetOne].left     = m_iHorizontalOffset;
    m_rcBtn[GBTN_BetOne].right    = m_rcBtn[0].left + (IsVGA()?105:53);
    m_rcBtn[GBTN_BetOne].top      = m_rcMenu.top + iIndent;
    m_rcBtn[GBTN_BetOne].bottom   = m_rcBtn[0].top + (IsVGA()?84:43);

    m_rcBtn[GBTN_BetMax].left     = m_rcBtn[GBTN_BetOne].right - 1;//try it
    m_rcBtn[GBTN_BetMax].right    = m_rcBtn[GBTN_BetMax].left + (IsVGA()?99:49);
    m_rcBtn[GBTN_BetMax].bottom   = m_rcBtn[0].bottom;
    m_rcBtn[GBTN_BetMax].top      = m_rcBtn[0].top;
    
    m_rcBtn[GBTN_Draw].left       = m_iHorizontalOffset + (IsVGA()?244:122);
    m_rcBtn[GBTN_Draw].right      = m_rcBtn[GBTN_Draw].left + (IsVGA()?99:49);
    m_rcBtn[GBTN_Draw].bottom     = m_rcBtn[0].bottom;
    m_rcBtn[GBTN_Draw].top        = m_rcBtn[0].top;
    
    m_rcBtn[GBTN_Menu].left       = m_iHorizontalOffset + (IsVGA()?378:189);
    m_rcBtn[GBTN_Menu].right      = m_rcBtn[GBTN_Menu].left + (IsVGA()?103:52);
    m_rcBtn[GBTN_Menu].bottom     = m_rcBtn[0].bottom;
    m_rcBtn[GBTN_Menu].top        = m_rcBtn[0].top;
    

    //and set some image slices
    if(m_img50.IsLoaded())
        m_img50.SetSize(WIDTH(m_rcLowerText), HEIGHT(m_rcLowerText));

    if(m_img33.IsLoaded())
        m_img33.SetSize(WIDTH(m_rcLowerText), HEIGHT(m_rcLowerText));

    if(m_imgWinGlow.IsLoaded())
        m_imgWinGlow.SetSize(WIDTH(m_rcLowerText)/2, HEIGHT(m_rcLowerText));

    if(m_imgMenu.IsLoaded())
        m_imgMenu.SetSize(WIDTH(m_rcMenu), HEIGHT(m_rcMenu));

    if(m_imgScore.IsLoaded())
        m_imgScore.SetSize(WIDTH(m_rcCreditsAmount), HEIGHT(m_rcCreditsAmount));
   

    g_gui->OnSize(hWnd, rcClient);
	g_gui->gdiMem.Destroy();
	g_gui->gdiBackground.Destroy();
	g_gui->InitGDI(rcClient, dc);
	ReleaseDC(hWnd, dc);

	return TRUE;
}

BOOL CDlgVideoPoker::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
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

void CDlgVideoPoker::LoadImages(HWND hWndSplash, int iMaxPercent)
{
    int iCurPercent = 0;
    if(hWndSplash)
        iCurPercent = SendMessage(hWndSplash, WM_GET_Percent, 0, 0);

    //undo
	if(m_gdiDigitalFont.GetDC() == NULL)
		m_gdiDigitalFont.LoadImage(IsVGA()?IDB_PNG_DigitalFont_HR:IDB_PNG_DigitalFont, hWndSplash, m_hInst);

    if(m_gdiHold.GetDC() == NULL)
        m_gdiHold.LoadImage(IsVGA()?IDB_PNG_Hold_HR:IDB_PNG_Hold, hWndSplash, m_hInst);

    int iStep = (iMaxPercent-iCurPercent)/6;	// 8 steps

    UINT uInt;
    uInt = IsVGA()?IDB_BMP_Clubs_HR:IDB_BMP_Clubs;
	if(m_gdiCards[SUIT_Club].GetDC() == NULL)
		m_gdiCards[SUIT_Club].LoadImage(uInt, m_hWnd, m_hInst);

    uInt = IsVGA()?IDB_BMP_Clubs:IDB_BMP_Clubs_Sm;
    if(m_gdiCardsSm[SUIT_Club].GetDC() == NULL)
        m_gdiCardsSm[SUIT_Club].LoadImage(uInt, m_hWnd, m_hInst);
    
    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    uInt = IsVGA()?IDB_BMP_Spades_HR:IDB_BMP_Spades;
	if(m_gdiCards[SUIT_Spade].GetDC() == NULL)
		m_gdiCards[SUIT_Spade].LoadImage(uInt, m_hWnd, m_hInst);

    uInt = IsVGA()?IDB_BMP_Spades:IDB_BMP_Spades_Sm;
    if(m_gdiCardsSm[SUIT_Spade].GetDC() == NULL)
        m_gdiCardsSm[SUIT_Spade].LoadImage(uInt, m_hWnd, m_hInst);

    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    uInt = IsVGA()?IDB_BMP_Diamonds_HR:IDB_BMP_Diamonds;
	if(m_gdiCards[SUIT_Diamond].GetDC() == NULL)
		m_gdiCards[SUIT_Diamond].LoadImage(uInt, m_hWnd, m_hInst);

    uInt = IsVGA()?IDB_BMP_Diamonds:IDB_BMP_Diamonds_Sm;
    if(m_gdiCardsSm[SUIT_Diamond].GetDC() == NULL)
        m_gdiCardsSm[SUIT_Diamond].LoadImage(uInt, m_hWnd, m_hInst);
    
    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    uInt = IsVGA()?IDB_BMP_Hearts_HR:IDB_BMP_Hearts;
	if(m_gdiCards[SUIT_Heart].GetDC() == NULL)
		m_gdiCards[SUIT_Heart].LoadImage(uInt, m_hWnd, m_hInst);

    uInt = IsVGA()?IDB_BMP_Hearts:IDB_BMP_Hearts_Sm;
    if(m_gdiCardsSm[SUIT_Heart].GetDC() == NULL)
        m_gdiCardsSm[SUIT_Heart].LoadImage(uInt, m_hWnd, m_hInst);
    
    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);
    
    uInt = IsVGA()?IDB_PNG_CardBack_HR:IDB_PNG_CardBack;
	if(m_gdiCardBack.GetDC() == NULL)
		m_gdiCardBack.LoadImage(uInt, m_hWnd, m_hInst);

    uInt = IsVGA()?IDB_PNG_CardBack:IDB_PNG_CardBack_Sm;
    if(m_gdiCardBackSm.GetDC() == NULL)
        m_gdiCardBackSm.LoadImage(uInt, m_hWnd, m_hInst);

    //just the way the images are sized
	m_sizeCards.cx = m_gdiCardBack.GetWidth();
	m_sizeCards.cy = m_gdiCardBack.GetHeight();


    //Buttons
    if(m_gdiBtn[GBTN_BetOne].GetDC() == NULL)
        m_gdiBtn[GBTN_BetOne].LoadImage(IsVGA()?IDB_PNG_BetOne_HR:IDB_PNG_BetOne, m_hWnd, m_hInst);

    if(m_gdiBtn[GBTN_BetMax].GetDC() == NULL)
        m_gdiBtn[GBTN_BetMax].LoadImage(IsVGA()?IDB_PNG_BetMax_HR:IDB_PNG_BetMax, m_hWnd, m_hInst);

    if(m_gdiBtn[GBTN_Draw].GetDC() == NULL)
        m_gdiBtn[GBTN_Draw].LoadImage(IsVGA()?IDB_PNG_Deal_HR:IDB_PNG_Deal, m_hWnd, m_hInst);

    if(m_gdiBtn[GBTN_Menu].GetDC() == NULL)
        m_gdiBtn[GBTN_Menu].LoadImage(IsVGA()?IDB_PNG_MenuBtn_HR:IDB_PNG_MenuBtn, m_hWnd, m_hInst);

    if(m_gdiX2.GetDC() == NULL)
        m_gdiX2.LoadImage(IsVGA()?IDB_PNG_BetX2_HR:IDB_PNG_BetX2, m_hWnd, m_hInst);


	if(m_imgSelector.IsLoaded() == FALSE)
		m_imgSelector.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_Selector_HR:IDB_PNG_Selector);
    m_imgSelector.SetSize(m_sizeCards.cx + 2*SELECTOR_EXPANSE, m_sizeCards.cy + 2*SELECTOR_EXPANSE);

    if(m_imgWinSelector.IsLoaded() == FALSE)
        m_imgWinSelector.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_WinSelector_HR:IDB_PNG_WinSelector);
    m_imgWinSelector.SetSize(m_sizeCards.cx + 2*SELECTOR_EXPANSE, m_sizeCards.cy + 2*SELECTOR_EXPANSE);

    if(m_imgWinSelectorSm.IsLoaded() == FALSE)
        m_imgWinSelectorSm.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_WinSelector:IDB_PNG_WinSelector_Sm);
    m_imgWinSelectorSm.SetSize(SMCARD_WIDTH + 2*SMSELECTOR_EXPANSE,SMCARD_HEIGHT + 2*SMSELECTOR_EXPANSE);

    if(m_imgWinGlow.IsLoaded() == FALSE)
        m_imgWinGlow.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_WinGlow_HR:IDB_PNG_WinGlow);
    m_imgWinGlow.SetSize(WIDTH(m_rcLowerText)/2, HEIGHT(m_rcLowerText));


    if(m_img33.IsLoaded() == FALSE)
        m_img33.Initialize(hWndSplash, m_hInst, IDB_PNG_Grid33);
    m_img33.SetSize(WIDTH(m_rcLowerText), HEIGHT(m_rcLowerText));

    if(m_img50.IsLoaded() == FALSE)
        m_img50.Initialize(hWndSplash, m_hInst, IDB_PNG_Grid50);
    m_img50.SetSize(WIDTH(m_rcLowerText), HEIGHT(m_rcLowerText));

	if(m_imgScore.IsLoaded() == FALSE)
		m_imgScore.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_ScoreBox_HR:IDB_PNG_ScoreBox);
    m_imgScore.SetSize(WIDTH(m_rcBetAmount), HEIGHT(m_rcBetAmount));

    if(m_imgMenu.IsLoaded() == FALSE)
        m_imgMenu.Initialize(hWndSplash, m_hInst, IsVGA()?IDB_PNG_MenuSlice_HR:IDB_PNG_MenuSlice);
    m_imgMenu.SetSize(WIDTH(m_rcMenu), HEIGHT(m_rcMenu));

    iCurPercent += iStep; // increment the percent
    if(hWndSplash) SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);
}

BOOL CDlgVideoPoker::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
    g_gui->InitGDI(rcClient, hDC);

	RECT rcClip;
	GetClipBox(hDC, &rcClip);

	// draw the background
	g_gui->DrawBackground(rcClip);

    // draw all the text
    DrawText(g_gui->gdiMem, rcClip);

    // draw the cards
    DrawCards(g_gui->gdiMem, rcClip);

    // draw the selector
    DrawSelector(g_gui->gdiMem, rcClip);
    DrawWinHighlight(g_gui->gdiMem, rcClip);

    // and if a card is selected
    DrawSelected(g_gui->gdiMem, rcClip);

    DrawButtons(g_gui->gdiMem, rcClip);

    if(m_oGame.GetGameState() == VPSTATE_Bet || m_oGame.GetGameState() == VPSTATE_EndGame)
        DrawPayoutTable(g_gui->gdiMem, rcClip);

	

    

    //until we get buttons
  //  g_gui->DrawBottomBar(rcClip, _T("Menu"), _T("Game"));

    g_gui->DrawScreen(rcClip, hDC);

	return TRUE;
}

BOOL CDlgVideoPoker::DrawScore(CIssGDIEx& gdi, POINT pt, int iScore)
{
    if(m_imgScore.IsLoaded() == FALSE)
        return FALSE;

    if(iScore < 0)
        iScore = 0;

#define SCORE_DIGITS 6

    static TCHAR szScore[8];//plenty

    m_imgScore.DrawImage(gdi, pt.x, pt.y);

    m_oStr->IntToString(szScore, iScore);


    int iXIndent = (IsVGA()?6:3);
    int iYIndent = (HEIGHT(m_rcBetAmount) - m_gdiDigitalFont.GetHeight())/2;

    int iDigitWidth = m_gdiDigitalFont.GetWidth()/11;

    int i = 0;

    //7 digits for now ... might edit later
    for(i = 0; i < SCORE_DIGITS; i++)
    {   //draw the 0s
        Draw(gdi, pt.x + iXIndent + i*iDigitWidth, pt.y + iYIndent, iDigitWidth, m_gdiDigitalFont.GetHeight(), m_gdiDigitalFont);
    }

    i = SCORE_DIGITS - m_oStr->GetLength(szScore);
    int iDigit;


    for(int j = 0; j < m_oStr->GetLength(szScore); j++)
    {
        iDigit = szScore[j] - _T('0') + 1; //I'm smarter than I look
        Draw(gdi, pt.x + iXIndent + (i+j)*iDigitWidth, pt.y + iYIndent, iDigitWidth, m_gdiDigitalFont.GetHeight(), m_gdiDigitalFont, iDigit*iDigitWidth);
    }

  /*  for(int j = m_oStr->GetLength(szScore); j < 7; j++)
    {
        iDigit = szScore[j-i] - _T('0') + 1; //I'm smarter than I look
        Draw(gdi, pt.x + iXIndent + i*iDigitWidth, pt.y + iYIndent, iDigitWidth, m_gdiDigitalFont.GetHeight(), m_gdiDigitalFont, iDigit*iDigitWidth);
    }*/
    
    //cool
    return TRUE;
}

BOOL CDlgVideoPoker::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
	case _T('p')://time cheat
		break;
	case _T('0'):
	case _T('u'):
		break;
#ifdef DEBUG
    case _T('s'):
        m_oGame.ResetStats();
        break;
	case _T('r'):
	case _T('R'):
		m_oGame.Reset();
		break;
    case _T('N'):
    case _T('n')://new game
        OnNewGame();
        break;
#endif
	case _T('c'):
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

BOOL CDlgVideoPoker::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgVideoPoker::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_bDrawSelector = TRUE;
	int iOldSel = m_iSelector;

    g_gui->OnKeyUp(hWnd, wParam, lParam);
    
    switch(LOWORD(wParam))
	{
    case VK_TSOFT1:
        PostMessage(hWnd, WM_BTN_Deal, 0,0);
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
    case VK_TSOFT2:
        OnMenuMenu();
        break;
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
	}
	return TRUE;
}

BOOL CDlgVideoPoker::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(wParam == IDT_Deal)
    {
        // deal out another card
        m_oGame.DealNextCard();

        if(m_oGame.GetGameState() != VPSTATE_Deal && m_oGame.GetGameState() != VPSTATE_Deal2)
        {
            KillTimer(m_hWnd, IDT_Deal);

            // if we're in the EndGame state we want to get a new game start 
            if(m_oGame.GetGameState() == VPSTATE_EndGame)
            {
                SetTimer(m_hWnd, IDT_DelayReset, 3000, NULL);
                SetTimer(m_hWnd, IDT_DelayText, 4000, NULL);
                PostMessage(m_hWnd, WM_PAINT, 0, 0);
                PostMessage(m_hWnd, WM_ANI_WIN, 0, 0);
                
                if(m_oGame.m_eBestResult != VPRESULT_Lose)
                {
                    if(m_oGame.m_eBestResult < VPRESULT_FullHouse)
                        PlaySounds(_T("IDR_WAVE_BIGWIN"));
                    else if(m_oGame.m_eBestResult < VPRESULT_Straight)
                        PlaySounds(_T("IDR_WAVE_JACKPOT"));
                    else
                        PlaySounds(_T("IDR_WAVE_WIN"));
                }
				else
					PlaySounds(_T("IDR_WAVE_LOSE"));
            }
            InvalidateRect(m_hWnd, NULL, FALSE);
        }
        else
        {
            InvalidateRect(m_hWnd, &m_rcCardArea, FALSE);        
            PlaySounds(_T("IDR_WAVE_DEAL"));//at least a bit optomized ....
        }
    }
    else if(wParam == IDT_DelayReset)
    {
        KillTimer(m_hWnd, IDT_DelayReset);

        m_bSelHold = FALSE;
        m_iCurrentSelectedBtn = 0;

        // go back to the betting stage
        m_oGame.NewHand();
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
    else if(wParam == IDT_DelayText)
    {
        InvalidateRect(m_hWnd, NULL, FALSE);
        //Check for End Game State
        
        //if(m_oGame.IsBroke())
        //    PostMessage(hWnd, WM_Lose, 0, 0);
    }
    return FALSE;
}

BOOL CDlgVideoPoker::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return CIssWnd::OnDestroy(hWnd, wParam, lParam);
}

void CDlgVideoPoker::OnSelectGame()
{
    m_dlgGameType.Init(m_oGame.IsGameInProgress(), m_oGame.GetGameType());
    int iResult = (INT)m_dlgGameType.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);

    if(iResult > -1 && m_oGame.GetGameType() != (EnumPokerType)iResult)
    {
        m_oGame.SetGameType((EnumPokerType)iResult);
        if(m_oGame.GetGameState() != VPSTATE_EndGame &&
            m_oGame.GetGameState() != VPSTATE_Bet)
            m_oGame.NewHand();
        SaveRegistry();
        CalcCardPositions();
        InvalidateRect(m_hWnd, NULL, FALSE);
    }  
}

BOOL CDlgVideoPoker::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
	switch(LOWORD(wParam))
	{
	case IDMENU_Menu:
        ShowMainMenu();
        break;
    case IDMENU_Left:
		OnMenuMenu();
		break;
    case IDMENU_Right:
		break;
	case IDMENU_NewGame:
		break;
	case IDMENU_RestartGame:
		{
            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(_T("Do you want to restart this game?\r\n\r\nYour bank will be reset."), m_hWnd, m_hInst, MB_YESNO))
                m_oGame.Reset();	
		}
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
    case IDMENU_GameSelect:
        {
            OnSelectGame();
        }
        break;
	case IDMENU_Undo:
		break;
	case IDMENU_Quit:
    case IDMENU_Exit:
        {   
            CDlgMsgBox dlgMsg;
            if(IDYES == dlgMsg.PopupMessage(IDS_MSG_Exit, m_hWnd, m_hInst, MB_YESNO))
                PostQuitMessage(0);
        }
        break;
    case IDMENU_Options:
        {
            CDlgOptions dlgOptions;
            dlgOptions.Init(m_hInst, m_bPlaySounds, m_oGame.m_bAutoHold, m_bQuickPlay, m_eBGColor, m_oGame.GetBetAmount(), m_oGame.GetMultiplay());
            if(IDOK == dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
            {
                m_eBGColor = dlgOptions.m_eBGColor;
                m_bPlaySounds = dlgOptions.m_bPlaySounds;
                m_oGame.m_bAutoHold = dlgOptions.m_bAutoHold;
                m_bQuickPlay = dlgOptions.m_bQuickPlay;
                m_oGame.SetBetAmount(dlgOptions.m_eBet);//will this work mid???
                m_oGame.SetMultiplay(dlgOptions.m_eMultiplay);//so this will work mid game
                SaveRegistry();
                CalcCardPositions();
                InvalidateRect(m_hWnd, NULL, FALSE);
            }
        }
        break;
    case IDMENU_Help:
        CreateProcess(_T("peghelp"), _T("PanoVideoPoker.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
        break;
    case IDMENU_Stats:
        {
            CDlgStats dlgStats;
            dlgStats.Init(&m_oGame);
            dlgStats.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
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

BOOL CDlgVideoPoker::DrawSelector(CIssGDIEx& gdi, RECT& rcClient)
{
	return TRUE;
}

BOOL CDlgVideoPoker::DrawWinHighlight(CIssGDIEx& gdi, RECT& rcClip)
{
    if(m_oGame.GetGameState() != VPSTATE_EndGame)
        return FALSE;

    static RECT rcTemp;

    for(int i =0; i < 5; i++)
    {
        if(m_oGame.IsWinner(i) && IsRectInRect(rcClip, m_rcCards[i]))
        {
            rcTemp = m_rcCards[i];
            InflateRect(&rcTemp, SELECTOR_EXPANSE, SELECTOR_EXPANSE);
            m_imgWinSelector.DrawImage(gdi, rcTemp.left, rcTemp.top);
        }
    }

    for(int j = 1; j < m_oGame.GetNumHands(); j++)
    {
        for(int i =0; i < 5; i++)
        {
            if(m_oGame.IsWinner(i, j))
            {
                rcTemp = m_rcCardsM[j-1][i];
                InflateRect(&rcTemp, SMSELECTOR_EXPANSE, SMSELECTOR_EXPANSE);
                if(IsRectInRect(rcClip, rcTemp))
                    m_imgWinSelectorSm.DrawImage(gdi, rcTemp.left, rcTemp.top);
            }
        }
    }
    
    return TRUE;
}

BOOL CDlgVideoPoker::DrawSelected(CIssGDIEx& gdi, RECT& rcClip)
{
    static RECT rcTemp;

    int iOffSet = CARD_SPACING;
    if(m_oGame.GetNumHands() > 1)
        iOffSet = 0;

    for(int i = 0; i < 5; i++)
    {
        if(m_oGame.IsHeld(i) && IsRectInRect(rcClip, m_rcCards[i]))//slightly lazy but should be fine
        {
            rcTemp = m_rcCards[i];
            InflateRect(&rcTemp, SELECTOR_EXPANSE, SELECTOR_EXPANSE);
            m_imgSelector.DrawImage(gdi, rcTemp.left, rcTemp.top - iOffSet);

            rcTemp.top = rcTemp.top + (HEIGHT(rcTemp) - m_gdiHold.GetHeight())/2;
            rcTemp.bottom = rcTemp.top + m_gdiHold.GetHeight();
            rcTemp.left += SELECTOR_EXPANSE/2;
            rcTemp.right = rcTemp.left + m_gdiHold.GetWidth();

            Draw(gdi, rcTemp, m_gdiHold);
        }
    }

	return TRUE;
}

BOOL CDlgVideoPoker::DrawButtons(CIssGDIEx& gdi, RECT& rcClip)
{
    if(IsRectInRect(rcClip, m_rcMenu) == FALSE)
        return FALSE;

    m_imgMenu.DrawImage(gdi, m_rcMenu.left, m_rcMenu.top);


    for(int i = 0; i < (int)GBTN_Count; i++)
    {
        if(IsRectInRect(rcClip, m_rcBtn[i]) == FALSE)
            continue;//only draw if we need to

        if(m_bBtnDown[i] == TRUE)
        {
            if(m_oGame.GetGameType() == PTYPE_StudVP && 
                i == 1 && 
                m_oGame.GetGameState() != VPSTATE_Bet && 
                m_oGame.GetGameState() != VPSTATE_Deal)
                Draw(gdi, m_rcBtn[i].left, m_rcBtn[i].top, WIDTH(m_rcBtn[i]), HEIGHT(m_rcBtn[i]),  m_gdiX2,WIDTH(m_rcBtn[i]));
            else
                Draw(gdi, m_rcBtn[i].left, m_rcBtn[i].top, WIDTH(m_rcBtn[i]), HEIGHT(m_rcBtn[i]),  m_gdiBtn[i],WIDTH(m_rcBtn[i]));

        }
        else
        {
            if(m_oGame.GetGameType() == PTYPE_StudVP && 
                i == 1 && 
                m_oGame.GetGameState() != VPSTATE_Bet && 
                m_oGame.GetGameState() != VPSTATE_Deal)
                Draw(gdi, m_rcBtn[i].left, m_rcBtn[i].top, WIDTH(m_rcBtn[i]), HEIGHT(m_rcBtn[i]),  m_gdiX2, 0);
            else
                Draw(gdi, m_rcBtn[i].left, m_rcBtn[i].top, WIDTH(m_rcBtn[i]), HEIGHT(m_rcBtn[i]), m_gdiBtn[i], 0);
        }
    }

    return TRUE;
}

BOOL CDlgVideoPoker::DrawText(CIssGDIEx& gdi, RECT& rcClip)
{
    //two large ifs
    if(IsRectInRect(rcClip, m_rcDisplay))
    {
	    BOOL bDrawGameType = TRUE;
	    if(GetSystemMetrics(SM_CYSCREEN)%240 == 0)
		    bDrawGameType = FALSE;

	    switch(m_oGame.GetGameState())
	    {
	    case VPSTATE_Deal:
		    bDrawGameType = TRUE;
		    break;
	    case VPSTATE_Hold:
		    bDrawGameType = FALSE;
		    if(m_oGame.GetGameType() == PTYPE_StudVP)
			    ::DrawTextShadow(gdi.GetDC(), _T("Deal or double bet"), m_rcDisplay, DT_CENTER | DT_VCENTER, m_hFontTitle, 0xFFFFFF, 0);
		    else
			    ::DrawTextShadow(gdi.GetDC(), _T("Tap cards to hold"), m_rcDisplay, DT_CENTER | DT_VCENTER, m_hFontTitle, 0xFFFFFF, 0);

	        break;
	    case VPSTATE_Deal2:
		    bDrawGameType = FALSE;
	        break;
        case VPSTATE_Bet:
            if(m_oGame.GetMultiplay() == MPLAY_5 &&
                GetSystemMetrics(SM_CYSCREEN)%320 == 0)
                bDrawGameType = FALSE;
            break;
	    case VPSTATE_EndGame:
            if(m_oGame.GetMultiplay() == MPLAY_5 &&
                GetSystemMetrics(SM_CYSCREEN)%320 == 0)
                bDrawGameType = FALSE;

		    if(bDrawGameType)
		    {
			    if(m_oGame.m_eBestResult != VPRESULT_Lose)
				    ::DrawTextShadow(gdi.GetDC(), _T("Winner!"), m_rcDisplay, DT_CENTER | DT_VCENTER, m_hFontTitle, 0x06F598, 0);
			    else
				    ::DrawTextShadow(gdi.GetDC(), _T("Lose"), m_rcDisplay, DT_CENTER | DT_VCENTER, m_hFontTitle, 0x06F598, 0);
		    }
		    bDrawGameType = FALSE;
		    break;
	    default:
	        break;
	    }


	    if(bDrawGameType == TRUE)
	    {
		    TCHAR szTemp[STRING_LARGE];
		    switch(m_oGame.GetGameType())
		    {
		    case PTYPE_10sOrBetter:
			    m_oStr->StringCopy(szTemp, _T("10s or Better"));
			    break;
		    case PTYPE_JacksOrBetter:
			    m_oStr->StringCopy(szTemp, _T("Jacks or Better"));
			    break;
		    case PTYPE_AcesAndFaces:
			    m_oStr->StringCopy(szTemp, _T("Aces and Faces"));
		        break;
		    case PTYPE_DoubleBonus:
			    m_oStr->StringCopy(szTemp, _T("Double Bonus"));
		        break;
		    case PTYPE_BonusDeluxe:
			    m_oStr->StringCopy(szTemp, _T("Bonus Deluxe"));
			    break;
		    case PTYPE_StudVP:
			    m_oStr->StringCopy(szTemp, _T("Stud Poker"));
			    break;
		    default:
			    m_oStr->Empty(szTemp);
		        break;
		    }
		    ::DrawTextShadow(gdi.GetDC(), szTemp, m_rcDisplay, DT_CENTER | DT_VCENTER, m_hFontTitle, 0xFFFFFF, 0);
	    }
    }

    //lets do this
    if(IsRectInRect(rcClip, m_rcLowerText) == FALSE)
        return TRUE;
    m_img50.DrawImage(gdi, m_rcLowerText.left, m_rcLowerText.top);

    if(IsRectInRect(rcClip, m_rcBetText))
    {
        if(m_oGame.GetGameState() != VPSTATE_EndGame)
            ::DrawTextShadow(gdi.GetDC(), _T("Bet:"), m_rcBetText, DT_LEFT | DT_VCENTER, m_hFontText, 0xFFFFFF, 0);
        else
            ::DrawTextShadow(gdi.GetDC(), _T("Won:"), m_rcBetText, DT_LEFT | DT_VCENTER, m_hFontText, 0xFFFFFF, 0);
    }

    POINT pt;

    if(IsRectInRect(rcClip, m_rcBetAmount))
    {
        pt.x = m_rcBetAmount.left;
        pt.y = m_rcBetAmount.top;
        if(m_oGame.GetGameState() != VPSTATE_EndGame)
            DrawScore(gdi, pt, m_oGame.GetBet());
        else
            DrawScore(gdi, pt, m_oGame.GetAmountWon());
    }

    if(IsRectInRect(rcClip, m_rcCreditsText))
        ::DrawTextShadow(gdi.GetDC(), _T("Credits:"), m_rcCreditsText, DT_RIGHT | DT_VCENTER, m_hFontText, 0xFFFFFF, 0);

    if(IsRectInRect(rcClip, m_rcCreditsAmount))
    {
        pt.x = m_rcCreditsAmount.left;
        pt.y = m_rcCreditsAmount.top;
        DrawScore(gdi, pt, m_oGame.GetBank());
    }


	return TRUE;
}

BOOL CDlgVideoPoker::DrawCardsMulti(CIssGDIEx &gdi, RECT &rcClip)
{
    //make sure the card GDIs exist
    if(m_gdiCardsSm[0].GetDC() == NULL)
        return FALSE;

    CIssCard* oCard = NULL;
    CIssCard* oCardSm = NULL;

    HRESULT hr = S_OK;

    for(int i = 0; i < 5; i++)
    {
        if(IsRectInRect(rcClip, m_rcCards[i]))
        {    
            oCard = m_oGame.GetCard(i);
           
            if(oCard)
                Draw(gdi, m_rcCards[i], m_gdiCards[oCard->m_eSuit], m_gdiCardBack.GetWidth()*(oCard->m_eRank-1));
            else //the back
                Draw(gdi, m_rcCards[i], m_gdiCardBack);
        }

        for(int j = 0; j < m_oGame.GetNumHands() - 1; j++)
        {
            if(IsRectInRect(rcClip, m_rcCardsM[j][i]))
            {
                oCardSm = m_oGame.GetCard(i, j+1);

                if(oCard && m_oGame.GetGameState() == VPSTATE_Hold && m_oGame.IsHeld(i))
                    hr = Draw(gdi, m_rcCardsM[j][i], m_gdiCardsSm[oCard->m_eSuit], m_gdiCardBackSm.GetWidth()*(oCard->m_eRank-1));
                //stud poker
                else if(oCard && m_oGame.GetGameType() == PTYPE_StudVP && i < 4 && 
                    (m_oGame.GetGameState() == VPSTATE_Deal || m_oGame.GetGameState() == VPSTATE_Hold ))
                    hr = Draw(gdi, m_rcCardsM[j][i], m_gdiCardsSm[oCard->m_eSuit], m_gdiCardBackSm.GetWidth()*(oCard->m_eRank-1));
                else if(oCardSm)//draw the card
                    hr = Draw(gdi, m_rcCardsM[j][i], m_gdiCardsSm[oCardSm->m_eSuit], m_gdiCardBackSm.GetWidth()*(oCardSm->m_eRank-1));
                
                else //draw the back
                    hr = Draw(gdi, m_rcCardsM[j][i], m_gdiCardBackSm);

                if(hr != S_OK)
                    DebugOut(_T("%i,%i"), i, j);
            }
        }
    }
    return TRUE;
}


BOOL CDlgVideoPoker::DrawCards(CIssGDIEx& gdi, RECT& rcClip)
{
	//make sure the card GDIs exist
	if(m_gdiCards[0].GetDC() == NULL)
		return FALSE;

    if(m_oGame.GetNumHands() > 1)
        return DrawCardsMulti(gdi, rcClip);

    CIssCard* oCard = NULL;
    CIssCard* oCardMulti = NULL;

    for(int i = 0; i < 5; i++)
    {
        if(IsRectInRect(rcClip, m_rcCards[i]) == FALSE)
            continue;
        
        oCard = m_oGame.GetCard(i);

        if(oCard && m_oGame.IsHeld(i))
            Draw(gdi, m_rcCards[i].left, m_rcCards[i].top  - CARD_SPACING, WIDTH(m_rcCards[i]), HEIGHT(m_rcCards[i]), m_gdiCards[oCard->m_eSuit], m_gdiCardBack.GetWidth()*(oCard->m_eRank-1));
        else if(oCard)
            Draw(gdi, m_rcCards[i], m_gdiCards[oCard->m_eSuit], m_gdiCardBack.GetWidth()*(oCard->m_eRank-1));
        else //the back
            Draw(gdi, m_rcCards[i], m_gdiCardBack);
    }
	return TRUE;
}

void CDlgVideoPoker::OnNewGame()
{
	m_oGame.Reset();
	KillTimer(m_hWnd, IDT_Time_Timer);
	SetTimer(m_hWnd, IDT_Time_Timer, 1000, NULL);
}

void CDlgVideoPoker::PlaySounds(TCHAR* szWave)
{
	if(m_bPlaySounds)
		PlaySound(szWave, m_hInst, SND_RESOURCE | SND_ASYNC);
}

void CDlgVideoPoker::OnDeal()
{
    if(m_bQuickPlay)
        SetTimer(m_hWnd, IDT_Deal, 0, NULL);
    else if(m_oGame.GetNumHands() == 1 || m_oGame.GetGameType() == PTYPE_StudVP)
        SetTimer(m_hWnd, IDT_Deal, DEAL_DELAY, NULL);
    else
        SetTimer(m_hWnd, IDT_Deal, 100, NULL);
    InvalidateRect(m_hWnd, &m_rcCreditsAmount, FALSE);
}

BOOL CDlgVideoPoker::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    switch(uiMessage)
    {
    case (WM_BTN_Deal):
        // if we pressed the deal button and we're in the betting state move the selector
        // to the hold buttons
        KillTimer(m_hWnd, IDT_DelayReset);
        if(m_oGame.OnDeal())
            OnDeal();
        else if(m_oGame.IsBroke())
            PostMessage(hWnd, WM_Lose, 0, 0);
        break;
    case (WM_BTN_Hold):
    case (WM_BTN_Hold + 1):
    case (WM_BTN_Hold + 2):
    case (WM_BTN_Hold + 3):
    case (WM_BTN_Hold + 4):
        {
            int iBtn = uiMessage - (WM_BTN_Hold);
            if(m_oGame.Hold(iBtn))
            {
                if(m_oGame.IsHeld(iBtn))
                    PlaySounds(_T("IDR_WAVE_HOLDON"));	
                else
                    PlaySounds(_T("IDR_WAVE_HOLDOFF"));	
            }
        }
        break;
    case WM_BTN_BetOne:
		if(m_oGame.OnBetOne() == FALSE)
            if(m_oGame.IsBroke())
                PostMessage(hWnd, WM_Lose, 0, 0);
        break;
    case WM_BTN_BetMax:
		KillTimer(m_hWnd, IDT_DelayReset);
        if(m_oGame.OnBetMax())
            OnDeal();
        else if(m_oGame.IsBroke())
            PostMessage(hWnd, WM_Lose, 0, 0);
        break;
    case WM_BTN_X2:
        if(m_oGame.OnDouble())
			OnDeal();
        else if(m_oGame.IsBroke())
            PostMessage(hWnd, WM_Lose, 0, 0);
        break;
    case WM_BTN_Menu:
        OnMenuMenu();
        break;
    case WM_ANI_WIN:
        AnimateBankIncrease(m_oGame.GetPreviousBank(), m_oGame.GetBank(NULL));
        InvalidateRect(m_hWnd, NULL, FALSE);
        if(m_oGame.GetBank(NULL) >= MAX_BANK)
            PostMessage(hWnd, WM_BrokeBank, 0, 0);
        break;
    case WM_Lose:
        {
            PlaySounds(_T("IDR_WAVE_LOSE"));
            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(_T("You're out of credits.\r\n\r\nWould you like to start a new game?"), m_hWnd, m_hInst, MB_YESNO))
            {
                m_oGame.Reset();
                InvalidateRect(m_hWnd, NULL, FALSE);
            }
            else 
            {  
                m_oGame.Reset();
                ShowMainMenu();
            }
        }
        break;
    case WM_BrokeBank:
        {
            PlaySounds(_T("IDR_WAVE_BIGWIN"));
            CDlgMsgBox msg;
            if(IDYES == msg.PopupMessage(_T("Congratulations!!!\r\n\r\nYou've broken the bank.  You're a world class Video Poker player!\r\nNew games will now start with 50000 credits.\r\nStart a new game?"), m_hWnd, m_hInst, MB_YESNO))
            {
                m_oGame.Reset();
                InvalidateRect(m_hWnd, NULL, FALSE);
            }
            else 
            {  
                m_oGame.Reset();
                ShowMainMenu();
            }
        }

        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL CDlgVideoPoker::OnMouseMove(HWND hWnd, POINT& pt)
{
    return TRUE;
}

BOOL CDlgVideoPoker::OnLButtonDown(HWND hWnd, POINT& pt)
{
    m_bSelHold				= FALSE;

    //check cards
    for(int i = 0; i < 5; i++)
    {
        if(PtInRect(&m_rcCards[i], pt))
        {
            m_bSelHold				= TRUE;
            m_iCurrentSelectedBtn	= i;
            PostMessage(hWnd, WM_BTN_Hold+i, 0,0);
            InvalidateRect(m_hWnd, &m_rcCardArea, FALSE);
            break;
        }
    }

    //check buttons
    if(PtInRect(&m_rcBtn[GBTN_BetOne], pt))
    {
        m_bBtnDown[GBTN_BetOne] = TRUE;
        PostMessage(hWnd, WM_BTN_BetOne, 0,0);
        InvalidateRect(m_hWnd, &m_rcBtn[GBTN_BetOne], FALSE);
    }
    else if(PtInRect(&m_rcBtn[GBTN_BetMax], pt))
    {
        m_bBtnDown[GBTN_BetMax] = TRUE;
        if(m_oGame.GetGameType() == PTYPE_StudVP && 
            m_oGame.GetGameState() != VPSTATE_Bet && 
            m_oGame.GetGameState() != VPSTATE_EndGame)
            PostMessage(hWnd, WM_BTN_X2, 0,0);
        else
            PostMessage(hWnd, WM_BTN_BetMax, 0,0);
        InvalidateRect(m_hWnd, &m_rcBtn[GBTN_BetMax], FALSE);
    }
    else if(PtInRect(&m_rcBtn[GBTN_Draw], pt))
    {
        m_bBtnDown[GBTN_Draw] = TRUE;
        PostMessage(hWnd, WM_BTN_Deal, 0,0);
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
    else if(PtInRect(&m_rcBtn[GBTN_Menu], pt))
    {
        m_bBtnDown[GBTN_Menu] = TRUE;
        PostMessage(hWnd, WM_BTN_Menu, 0,0);
        InvalidateRect(m_hWnd, &m_rcBtn[GBTN_Menu], FALSE);
    }

    return TRUE; //g_gui->OnLButtonDown(hWnd, pt);
}

BOOL CDlgVideoPoker::OnLButtonUp(HWND hWnd, POINT& pt)
{
    RECT rcTemp;
    for(int i = 0; i < GBTN_Count; i++)
    {
        if(m_bBtnDown[i])
        {
            m_bBtnDown[i] = FALSE;
            rcTemp = m_rcBtn[i];
            
			if(i < 1)//bet buttons
            {
                UnionRect(&rcTemp, &rcTemp, &m_rcBetAmount);
                PlaySounds(_T("IDR_WAVE_BET"));
            }
            if(i == 2)//deal
            {
                UnionRect(&rcTemp, &rcTemp, &m_rcCreditsAmount);
            }
            InvalidateRect(m_hWnd, &rcTemp, FALSE);

        }
    }

    return TRUE;
}

void CDlgVideoPoker::UpdateSelector(int iOldSel)
{

}

BOOL CDlgVideoPoker::ShouldAbort()
{
	MSG msg;

	if (MsgWaitForMultipleObjects(0, NULL, FALSE, 5, QS_ALLINPUT) != WAIT_OBJECT_0)
		return FALSE;

	if(PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
	{
		switch(LOWORD(msg.message))
		{
	//	case WM_KEYUP:
		case WM_CHAR:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_COMMAND:
		case WM_USER:
		case WM_SETFOCUS:
			return TRUE;
		}

		//	PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE);
		//	TranslateMessage((LPMSG)&msg);
		//	DispatchMessage((LPMSG)&msg);
	}
	return FALSE;
}

BOOL CDlgVideoPoker::OnMenuMenu()
{
    g_gui->wndMenu.ResetContent();

    g_gui->wndMenu.AddItem(_T("Choose Game"), IDMENU_GameSelect);
    g_gui->wndMenu.AddSeparator();
    g_gui->wndMenu.AddItem(_T("Restart Game"), IDMENU_RestartGame);
    g_gui->wndMenu.AddSeparator();
    g_gui->wndMenu.AddItem(IDS_MENU_Options, m_hInst, IDMENU_Options);
    g_gui->wndMenu.AddItem(IDS_MENU_Stats, m_hInst, IDMENU_Stats);
    g_gui->wndMenu.AddItem(IDS_MENU_About, m_hInst, IDMENU_About);
    g_gui->wndMenu.AddItem(IDS_MENU_Help, m_hInst, IDMENU_Help);
    g_gui->wndMenu.AddSeparator();
    g_gui->wndMenu.AddItem(IDS_MENU_Exit, m_hInst, IDMENU_Exit);

    g_gui->wndMenu.SetSelectedItemIndex(0, TRUE);

    RECT rcClient;
    GetWindowRect(m_hWnd, &rcClient);

    g_gui->wndMenu.PopupMenu(m_hWnd, m_hInst,
        OPTION_AlwaysShowSelector|OPTION_CircularList,
        rcClient.right/4,rcClient.top,
        rcClient.right*3/4, HEIGHT(rcClient) - HEIGHT(m_rcMenu) + (IsVGA()?32:16),
        0,0,0,0, ADJUST_Bottom);

    return TRUE;
}

#define SHADOW_FADE (IsVGA()?8:4)

BOOL CDlgVideoPoker::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL bRet = UNHANDLED;
#ifdef UNDER_CE
    HWND hWndSip = NULL;

    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
    {	
       
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



BOOL CDlgVideoPoker::ShowSplashScreen()
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
            SendMessage(dlgSplashScreen.GetWnd(), WM_SET_Percent, 100, 0);
            break;
        }

        if(msg.message == WM_lOAD_Images)
            break;
    }

    ShowMainMenu();
    return TRUE;
}

BOOL CDlgVideoPoker::ShowMainMenu()
{
    CDlgMainMenu	dlgMenu;
    dlgMenu.Init(m_oGame.IsGameInProgress(), m_bPlaySounds, m_oGame.m_bAutoHold, m_bQuickPlay, m_eBGColor, m_oGame.GetBetAmount(), m_oGame.GetMultiplay());
    UINT uiResult = (UINT)dlgMenu.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);

    m_bPlaySounds = dlgMenu.m_bPlaySounds;
    m_eBGColor = dlgMenu.m_eBGColor;
    m_oGame.SetBetAmount(dlgMenu.m_eBetAmount);

    if(uiResult >= 5000 && uiResult < 5000 + 10)
    {   //we need to change game modes
        m_oGame.SetGameType(EnumPokerType(uiResult - 5000));           
    }
    else
        switch(uiResult)
        {
        case IDMENU_NewGame:
            m_oGame.Reset();
            OnSelectGame();
            break;
        case IDMENU_Exit:
            PostQuitMessage(0);
            return TRUE;
        default:
        case IDMENU_Resume:
            break;
        }

    return TRUE;
}

BOOL CDlgVideoPoker::AnimateBankIncrease(int iOldBank, int iNewBank)
{
#define MAX_CREDIT_FRAMES 100

    if(m_bQuickPlay)
        return TRUE;

    HDC hdc;
    hdc = GetDC(m_hWnd);

    //copy the little piece of background into the GDI
    CIssGDIEx gdiBackGround;
    CIssGDIEx gdiTemp;
    
    gdiBackGround.Create(hdc, m_rcCreditsAmount, FALSE, TRUE);
    gdiTemp.Create(hdc, m_rcCreditsAmount, FALSE, TRUE);

	Draw(gdiBackGround, 0, 0, WIDTH(m_rcCreditsAmount), HEIGHT(m_rcCreditsAmount), g_gui->gdiBackground, m_rcCreditsAmount.left, m_rcCreditsAmount.top);
    m_img50.DrawImage(gdiBackGround, 0, 0);

    TCHAR szTemp[STRING_NORMAL];
    POINT ptTemp;
    ptTemp.x = 0;
    ptTemp.y = 0;

    int iGap = 1;
    
    if(iNewBank - iOldBank > MAX_CREDIT_FRAMES)
        iGap = (iNewBank - iOldBank)/MAX_CREDIT_FRAMES - 1;


    for(int i = iOldBank; i <= iNewBank; i+= iGap)
    {
        m_oStr->IntToString(szTemp, i);
        Draw(gdiTemp, 0, 0, WIDTH(m_rcCreditsAmount), HEIGHT(m_rcCreditsAmount), gdiBackGround);
        
        DrawScore(gdiTemp, ptTemp, i);
        BitBlt(hdc, m_rcCreditsAmount.left, m_rcCreditsAmount.top, WIDTH(m_rcCreditsAmount), HEIGHT(m_rcCreditsAmount), gdiTemp.GetDC(), 0, 0, SRCCOPY);

        if(ShouldAbort())
            break;
    }

    ReleaseDC(m_hWnd, hdc);
    return TRUE;
}

BOOL CDlgVideoPoker::DrawPayoutTable(CIssGDIEx& gdi, RECT& rcClip)
{
    //a little optimized ....
    if(rcClip.top > m_rcCards[0].top)
        return TRUE;

    int iSpacing = m_img33.GetHeight();
    int iRows = 5;
    int iPayoutCount = m_oGame.GetPayoutCount();
    RECT rcTemp;
    POINT ptTemp;
    TCHAR szTemp[STRING_NORMAL];
    int iReturn;

    //how many rows?
    if(GetSystemMetrics(SM_CYSCREEN)%240 == 0)
    {
        iRows = 5;
    }
    else
    {   //we'll calculate
        iRows = (iPayoutCount+1)/2;
    }

    int iIndex = 0;

    //should really optimize this a bit
    for(int i = 0; i < iRows; i++)
    {
        if(i%2 == 0)
            m_img50.DrawImage(gdi, 0, i*iSpacing);
        else
            m_img33.DrawImage(gdi, 0, i*iSpacing);

        while(m_oCalc->GetPayoutRatio(m_oGame.GetGameType(), (EnumPokerResult)iIndex, m_oGame.IsMaxBet()) == 0)
        {
            iIndex++;
            if(iIndex > 30)
                return TRUE;//works for me
        }

        iReturn = m_oCalc->GetPayoutRatio(m_oGame.GetGameType(), (EnumPokerResult)iIndex, m_oGame.IsMaxBet());
        
        GetResultsLabel(szTemp, (EnumPokerResult)iIndex);

        //now draw some text and stuff
        rcTemp.top      = i*iSpacing;
        rcTemp.bottom   = (i+1)*iSpacing;
        rcTemp.left     = 0;
        rcTemp.right    = GetSystemMetrics(SM_CXSCREEN)/2;

        //is this the winning hand?
        if(m_oGame.GetGameState() == VPSTATE_EndGame &&
            m_oGame.IsWinningResult(EnumPokerResult(iIndex)) )
        {
            m_imgWinGlow.DrawImage(gdi, rcTemp.left, rcTemp.top);
            rcTemp.left     = (IsVGA()?6:3);
            ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER, m_hFontPayout, 0x555555);
        }
        else
        {
            rcTemp.left     = (IsVGA()?6:3);
            ::DrawTextShadow(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER, m_hFontPayout, 0xFFFFFF, 0);
        }

        ptTemp.y = iSpacing*i + (iSpacing - m_imgScore.GetHeight())/2;
        ptTemp.x = rcTemp.right - m_imgScore.GetWidth();
        DrawScore(gdi, ptTemp, iReturn*m_oGame.GetBet());


        iIndex++;
        while(m_oCalc->GetPayoutRatio(m_oGame.GetGameType(), (EnumPokerResult)iIndex, m_oGame.IsMaxBet()) == 0)
        {
            iIndex++;
            if(iIndex > 30)
                return TRUE;//works for me
        }

        iReturn = m_oCalc->GetPayoutRatio(m_oGame.GetGameType(), (EnumPokerResult)iIndex, m_oGame.IsMaxBet());

        rcTemp.left     = rcTemp.right;
        rcTemp.right    += GetSystemMetrics(SM_CXSCREEN)/2;

        GetResultsLabel(szTemp, (EnumPokerResult)iIndex);

        //is this the winning hand?
        if(m_oGame.GetGameState() == VPSTATE_EndGame &&
            m_oGame.IsWinningResult(EnumPokerResult(iIndex)) )
        {
            rcTemp.left     += (IsVGA()?6:3);        
            m_imgWinGlow.DrawImage(gdi, rcTemp.left, rcTemp.top);
            ::DrawText(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER, m_hFontPayout, 0x555555);
        }
        else
        {
            rcTemp.left     += (IsVGA()?6:3);
            ::DrawTextShadow(gdi.GetDC(), szTemp, rcTemp, DT_LEFT | DT_VCENTER, m_hFontPayout, 0xFFFFFF, 0);
        }

        ptTemp.x = rcTemp.right - m_imgScore.GetWidth();
        DrawScore(gdi, ptTemp, iReturn*m_oGame.GetBet());

        iIndex++;
    }




    return TRUE;
}

BOOL CDlgVideoPoker::GetResultsLabel(TCHAR* szResult, EnumPokerResult eResult)
{
    switch(eResult)
    {
    case VPRESULT_Lose:
        m_oStr->StringCopy(szResult, _T("Lose"));
        break;
    case VPRESULT_JacksOrBetter:
        m_oStr->StringCopy(szResult, _T("2 J-A"));
        break;
    case VPRESULT_2Pairs:
        m_oStr->StringCopy(szResult, _T("2 Pairs"));
        break;
    case VPRESULT_3OfaKind:
        m_oStr->StringCopy(szResult, _T("3 of a Kind"));
        break;
    case VPRESULT_4OfaKind:
        m_oStr->StringCopy(szResult, _T("4 of a Kind"));
        break;
    case VPRESULT_FullHouse:
        m_oStr->StringCopy(szResult, _T("Full House"));
        break;
    case VPRESULT_Straight:
        m_oStr->StringCopy(szResult, _T("Straight"));
        break;
    case VPRESULT_Flush:
        m_oStr->StringCopy(szResult, _T("Flush"));
        break;
    case VPRESULT_StraightFlush:
        m_oStr->StringCopy(szResult, _T("Straight Flush"));
        break;
    case VPRESULT_RoyalFlush:
        m_oStr->StringCopy(szResult, _T("Royal Flush"));
        break;
    case VPRESULT_6sOrBetter:
        m_oStr->StringCopy(szResult, _T("Low Pair 6-10"));
        break;
    case VPRESULT_10sOrBetter:
        m_oStr->StringCopy(szResult, _T("2 10-A"));
        break;
    case VPRESULT_4OfaKind2s4s:
        m_oStr->StringCopy(szResult, _T("4 2-4"));
        break;
    case VPRESULT_4OfaKind2s4swA:
        m_oStr->StringCopy(szResult, _T("4 2-4 +Ace"));
        break;
    case VPRESULT_4OfaKind5sKs:
        m_oStr->StringCopy(szResult, _T("4 5-K"));
        break;
    case VPRESULT_4OfaKind2s10s:
        m_oStr->StringCopy(szResult, _T("4 2-10"));
        break;
    case VPRESULT_4OfaKindJsKs:
        m_oStr->StringCopy(szResult, _T("4 J-K"));
        break;
    case VPRESULT_4OfaKindAces:
        m_oStr->StringCopy(szResult, _T("4 Aces"));
        break;
    case VPRESULT_4OfaKindAcesw234:
        m_oStr->StringCopy(szResult, _T("4 Aces +234"));
        break;
    case VPRESULT_4OfaKind2s:
        m_oStr->StringCopy(szResult, _T("4 2s"));
        break;
    default:
        m_oStr->Empty(szResult);
        return FALSE;
        break;
   }
        
    return TRUE;


}