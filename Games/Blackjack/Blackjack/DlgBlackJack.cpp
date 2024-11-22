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
#include "DlgMoreApps.h"
#include "IssLocalisation.h"
#include "IssWndTouchMenu.h"

#define CARD_SPACING (m_gdiCardBack.GetWidth()/4)


#define TEXT_COLOR RGB(255,255,255)
#define TEXT_SHADOW 0

#define END_HAND_TIME               300
#define CARD_FLIP_TIME              250
#define CARD_DEAL_TIMING			200 //in ms
#define CARD_DEAL_SOUND_DELAY		0 //MS
#define DRAW_TIMER_DELAY            15

#define IDT_PLAYSOUND_DEAL			99
#define IDT_PLAYSOUND_BJ			100
#define IDT_PLAYSOUND_WIN			101
#define IDT_PLAYSOUND_LOSE			102
#define IDT_PLAYSOUND_DRAW          103
#define IDT_PLAYSOUND_SURRENDER     104
#define IDT_DEAL_NEXT_CARD			105
#define IDT_ENDGAME					106
#define IDT_DRAW_TIMER              107
#define IDT_COUNT_TIMER             108

#define REG_Diff	_T("Diff")
#define REG_PS		_T("PS")

CIssWndTouchMenu* g_wndMenu = NULL;

CDlgBackJack::CDlgBackJack(BOOL bDemo)
:m_eSelGamePlay(GAMEBTN_Hit)
,m_eSelBetting(BET_Deal)
,m_bPlaySounds(TRUE)
,m_bShowCount(FALSE)
,m_iYBtnStart(0)
,m_bAniCardFlip(FALSE)
,m_bShowSplashScreen(FALSE)
,m_bDemo(bDemo)
{
	// read values from the registry
	LoadRegSettings();

	ZeroMemory(&m_sizeWindow, sizeof(SIZE));

	g_wndMenu = new CIssWndTouchMenu;
		
	m_oStr = CIssString::Instance();


    m_hfButtonText	= m_gdiMem.CreateFont(m_oMet.GetTextHeight(), FW_BOLD, TRUE);
    m_hfScreenText	= m_gdiMem.CreateFont(m_oMet.GetTextHeight(), FW_BOLD, TRUE);
    m_hFontCount    = m_gdiMem.CreateFont(m_oMet.GetCountTextHeight(),	 FW_BOLD, TRUE);
}

CDlgBackJack::~CDlgBackJack(void)
{
	CIssGDIEx::DeleteFont(m_hfButtonText);
	CIssGDIEx::DeleteFont(m_hFontCount);
	CIssGDIEx::DeleteFont(m_hfScreenText);

	// write values to the registry
	SaveRegSettings();

	if(g_wndMenu)
		delete g_wndMenu;
	g_wndMenu = NULL;

	CIssString::DeleteInstance();
	CIssKineticList::DeleteAllContent();
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

	// figure out where the cards are supposed to be displayed
	RecalcCardLocations();

	// End the Wait cursor
	ShowCursor(FALSE); 
	SetCursor(NULL);

    //top text rect .. one should do
    m_rcTopText.top = m_oMet.GetTextIndent();
    m_rcTopText.left = m_oMet.GetTextIndent();
    m_rcTopText.bottom = m_rcTopText.top + GetSystemMetrics(SM_CXSMICON);//should be plenty
    m_rcTopText.right = rcClient.right - m_oMet.GetTextIndent();

    m_ptCountBtn.x = m_oMet.GetTextIndent()*2;
    m_ptCountBtn.y = m_oMet.GetTextIndent()*5;
    m_oMenu.OnSize(hWnd);

	return TRUE;
}

BOOL CDlgBackJack::RecalcCardLocations()
{
	// find out where the dealer cards should be drawn
	HDC dcScreen  = GetDC(m_hWnd);
	HFONT oldFont = (HFONT)SelectObject(dcScreen, m_hfButtonText);
	SIZE sizeText;

    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

	// find out how much space the text will take up
	GetTextExtentExPoint(dcScreen, 
		m_oStr->GetText(ID(IDS_RESULT_Surrender), m_hInst), 
		m_oStr->GetLength(m_oStr->GetText(ID(IDS_RESULT_Surrender), m_hInst)), 
		0,
		NULL,NULL,
		&sizeText);

	SelectObject(dcScreen, oldFont);
	ReleaseDC(m_hWnd, dcScreen);

	int iFullHandWidth		= 3*m_gdiCardBack.GetWidth() + sizeText.cx;
	BOOL bShowingSplitCards	= m_oGame.IsSplitting();

	// just stack them one on top of the other
	m_ptDealer.x	= WIDTH(rcClient)/2 - m_gdiCardBack.GetWidth()*2/3;
    m_ptDealer.y	= m_gdiCardBack.GetWidth()/2;

	m_ptPlayer.x	= m_ptDealer.x;
	m_ptPlayer.y	= m_ptDealer.y + m_gdiCardBack.GetHeight()*6/5;

    m_ptPlayerwSplit.x = GetSystemMetrics(SM_CXSMICON)/2;
    m_ptPlayerwSplit.y = m_ptPlayer.y;

    m_ptSplit.x     = GetSystemMetrics(SM_CXSCREEN)/2 + GetSystemMetrics(SM_CXSMICON)/2;
    m_ptSplit.y     = m_ptPlayer.y;
    

    m_ptChips.x     = WIDTH(rcClient)/2 - m_gdiChips.GetWidth()/2;
    m_ptChips.y     = m_ptPlayer.y + m_gdiCardBack.GetHeight();


	return TRUE;
}

BOOL CDlgBackJack::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    m_oStr->SetResourceInstance(m_hInst);
    g_cLocale.Init(REG_KEY, m_hInst, SUPPORT_English|SUPPORT_Portuguese|SUPPORT_German|SUPPORT_French|SUPPORT_Dutch|SUPPORT_Spanish|SUPPORT_Japanese, TRUE);

#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR | SHIDIF_SIPDOWN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

/*	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= hWnd;
	mbi.nToolBarId	= IDR_MENU_Blackjack;
	mbi.hInstRes	= m_hInst;
	if(!SHCreateMenuBar(&mbi))
	{
		int i = 0;
	}*/
#endif

	g_wndMenu->SetColors(RGB(255,255,255), RGB(255,255,255), RGB(255,255,255));
	g_wndMenu->SetImageArray(IsVGA()?IDR_PNG_MenuArrayVGA:IDR_PNG_MenuArray);
	g_wndMenu->SetSelected(IDR_PNG_MenuSelect);
	g_wndMenu->SetBackground(IDR_PNG_MenuGroup);
	g_wndMenu->PreloadImages(m_hWnd, m_hInst);

    m_bShowSplashScreen = TRUE;
	
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
	DrawCards(m_gdiMem, rcClip);
	DrawChips(m_gdiMem, rcClip);
    DrawScores(m_gdiMem, rcClip);
    m_oMenu.DrawMenu(m_gdiMem, rcClip, &m_oGame, m_hInst);

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

        Draw(m_gdiBackground, m_ptCountBtn.x, m_ptCountBtn.y, 
            m_gdiCount.GetWidth(), 
            m_gdiCount.GetHeight(),
            m_gdiCount , 0, 0);
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
    SIZE sizeNew;

    //Card Back
    gdiTemp.LoadImage(IDB_PNG_CardBack, m_hWnd, m_hInst);
    sizeNew.cy = m_oMet.GetCardHeight();
    sizeNew.cx = gdiTemp.GetWidth()*sizeNew.cy/gdiTemp.GetHeight();
    ScaleImage(gdiTemp, m_gdiCardBack, sizeNew, FALSE, 0);

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    //Fip Animation
    m_aniCardFlip.Initialize(IDB_PNG_Flip, m_hWnd, m_hInst, 11, CARD_FLIP_TIME, FALSE);
    m_aniCardFlip.SetSize(sizeNew.cx, sizeNew.cy*2);   

    sizeNew.cx *= 13;

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    //Cards
    gdiTemp.LoadImage(IDB_PNG_Hearts, m_hWnd, m_hInst);
    ScaleImage(gdiTemp, m_gdiCards[SUIT_Heart], sizeNew, FALSE, 0);

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);
    gdiTemp.LoadImage(IDB_PNG_Diamonds, m_hWnd, m_hInst);
    ScaleImage(gdiTemp, m_gdiCards[SUIT_Diamond], sizeNew, FALSE, 0);

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    gdiTemp.LoadImage(IDB_PNG_Spades, m_hWnd, m_hInst);
    ScaleImage(gdiTemp, m_gdiCards[SUIT_Spade], sizeNew, FALSE, 0);

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    gdiTemp.LoadImage(IDB_PNG_Clubs, m_hWnd, m_hInst);
    ScaleImage(gdiTemp, m_gdiCards[SUIT_Club], sizeNew, FALSE, 0);

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

 
    //Chips
    gdiTemp.LoadImage(IDB_PNG_Chips, m_hWnd, m_hInst);
    sizeNew.cx = m_oMet.GetChipSize()*3;
    sizeNew.cy = m_oMet.GetChipSize();
    ScaleImage(gdiTemp, m_gdiChips, sizeNew, FALSE, 0);

    // increment the percent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);


    //count square
    gdiTemp.LoadImage(IDB_PNG_Count, m_hWnd, m_hInst);
    sizeNew.cx = m_oMet.GetCountWidth();
    sizeNew.cy = m_oMet.GetCountSize();
    ScaleImage(gdiTemp, m_gdiCount, sizeNew, FALSE, 0);

    // increment the percwareent
    iCurPercent += iStep;
    if(hWndSplash)
        SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

    // dollar
    m_gdiDollar.LoadImage(IsVGA()?IDR_PNG_DollarVGA:IDR_PNG_Dollar, m_hWnd, m_hInst);
}

BOOL CDlgBackJack::DrawCards(CIssGDIEx& gdi, RECT& rc)
{
/*	if(m_oGame.GetGameState()==GSTATE_Bet)
	{
		//Draw two blank cards as place holders (keeps it from looking plain)
		DrawCard(gdi, m_ptDealer.x, m_ptDealer.y, SUIT_Heart, RANK_Ace, rc, FALSE, FALSE);
		DrawCard(gdi, m_ptPlayer.x, m_ptPlayer.y, SUIT_Heart, RANK_Ace, rc, FALSE, FALSE);
	}
	else*/
	{
		DrawHand(HAND_Dealer, gdi, rc);
		DrawHand(HAND_Player, gdi, rc);
		DrawHand(HAND_Split, gdi, rc);
		if(m_oGame.DealAnotherCard())
			//DealNextCard(); - no good
			SetTimer(m_hWnd, IDT_DEAL_NEXT_CARD, CARD_DEAL_TIMING, 0);
	}
	
	return TRUE;
}

BOOL CDlgBackJack::DrawCard(CIssGDIEx& gdi, int iX, int iY, EnumCardSuit eSuit, EnumCardRank eRank,RECT& rc, BOOL bShowFace /* = TRUE */, BOOL bRotate)
{
	if(rc.left > iX + m_gdiCardBack.GetWidth() ||
		rc.right < iX ||
		rc.top > iY + m_gdiCardBack.GetHeight() ||
		rc.bottom < iY)
		return FALSE;//don't need to draw

    static int iCardWidth = m_gdiCards[0].GetWidth()/13; //might work

	if(bShowFace == FALSE)
   
    

		Draw(gdi, 
		     iX, iY, 
		     m_gdiCardBack.GetWidth(), m_gdiCardBack.GetHeight(), 
		     m_gdiCardBack,
		     0, 0
		     );	
	else
		Draw(gdi, 
		     iX, iY, 
		     m_gdiCardBack.GetWidth(), m_gdiCardBack.GetHeight(),  
		     m_gdiCards[eSuit],
		     iCardWidth*((int)eRank - 1), //-1 because we don't have jokers in the images yet
		     0
 		     );

	return TRUE;
}

BOOL CDlgBackJack::DrawScores(CIssGDIEx& gdi, RECT& rc)
{
    static TCHAR szScore[STRING_NORMAL];

    RECT rcScore;
    
    if(m_oGame.ShowPlayerScore() && m_oGame.IsSplitting() == FALSE)
    {
        m_oGame.GetPlayerScore(szScore);

        if(m_oStr->GetLength(szScore) > 0)
        {

            HFONT hOld = (HFONT)SelectObject(gdi.GetDC(), m_hfScreenText);
            SIZE sText;

            GetTextExtentPoint(gdi.GetDC(), szScore, (int)_tcslen(szScore), &sText);

            sText.cx += 4;
            sText.cy += 4;

            rcScore.right = m_ptPlayer.x;
            rcScore.top = m_ptPlayer.y;
            rcScore.left = rcScore.right - sText.cx;
            rcScore.bottom = rcScore.top + sText.cy;

            AlphaFillRect(gdi, rcScore, 0x2C2D2F, 167);
            FrameRect(gdi, rcScore, 0x2C2D2F);

            ::DrawText(gdi.GetDC(), szScore, rcScore, DT_CENTER | DT_VCENTER, m_hfScreenText, 0xFFFFFF);
            SelectObject(gdi.GetDC(), hOld);
        }

    }
    //splitting
    else if(m_oGame.ShowPlayerScore())
    {
        m_oGame.GetPlayerScore(szScore);

        if(m_oStr->GetLength(szScore) > 0)
        {
            HFONT hOld = (HFONT)SelectObject(gdi.GetDC(), m_hfScreenText);
            SIZE sText;

            GetTextExtentPoint(gdi.GetDC(), szScore, (int)_tcslen(szScore), &sText);

            sText.cx += 4;
            sText.cy += 4;
#ifdef WIN32_PLATFORM_WFSP
            rcScore.left = m_ptPlayerwSplit.x;
            rcScore.right = rcScore.left + sText.cx;

            rcScore.top = m_ptPlayerwSplit.y - 4 - sText.cy;
            rcScore.bottom = rcScore.top + sText.cy;

#else
            rcScore.left = m_ptPlayerwSplit.x;
            rcScore.right = rcScore.left + sText.cx;

            rcScore.top = m_ptPlayerwSplit.y + m_gdiCardBack.GetHeight() + 4;
            
            rcScore.bottom = rcScore.top + sText.cy;
#endif

            AlphaFillRect(gdi, rcScore, 0x2C2D2F, 167);
            FrameRect(gdi, rcScore, 0x2C2D2F);

            ::DrawText(gdi.GetDC(), szScore, rcScore, DT_CENTER | DT_VCENTER, m_hfScreenText, 0xFFFFFF);
            SelectObject(gdi.GetDC(), hOld);
        }
    }

    if(m_oGame.ShowSplitScore())
    {
        m_oGame.GetSplitScore(szScore);

        if(m_oStr->GetLength(szScore) > 0)
        {
            HFONT hOld = (HFONT)SelectObject(gdi.GetDC(), m_hfScreenText);
            SIZE sText;

            GetTextExtentPoint(gdi.GetDC(), szScore, (int)_tcslen(szScore), &sText);

            sText.cx += 4;
            sText.cy += 4;

#ifdef WIN32_PLATFORM_WFSP
            rcScore.left = m_ptSplit.x;
            rcScore.right = rcScore.left + sText.cx;

            rcScore.top = m_ptSplit.y - 4 - sText.cy;

            rcScore.bottom = rcScore.top + sText.cy;
#else

            rcScore.left = m_ptSplit.x;
            rcScore.right = rcScore.left + sText.cx;

            rcScore.top = m_ptSplit.y + m_gdiCardBack.GetHeight() + 4;

            rcScore.bottom = rcScore.top + sText.cy;
#endif

            AlphaFillRect(gdi, rcScore, 0x2C2D2F, 167);
            FrameRect(gdi, rcScore, 0x2C2D2F);

            ::DrawText(gdi.GetDC(), szScore, rcScore, DT_CENTER | DT_VCENTER, m_hfScreenText, 0xFFFFFF);
            SelectObject(gdi.GetDC(), hOld);
        }

    }

    if(m_oGame.ShowDealerScore())
    {
        m_oGame.GetDealerScore(szScore);

        if(m_oStr->GetLength(szScore) > 0)
        {

            HFONT hOld = (HFONT)SelectObject(gdi.GetDC(), m_hfScreenText);
            SIZE sText;

            GetTextExtentPoint(gdi.GetDC(), szScore, (int)_tcslen(szScore), &sText);

            sText.cx += 4;
            sText.cy += 4;

            rcScore.right = m_ptDealer.x;
            rcScore.top = m_ptDealer.y;
            rcScore.left = rcScore.right - sText.cx;
            rcScore.bottom = rcScore.top + sText.cy;

            AlphaFillRect(gdi, rcScore, 0x2C2D2F, 167);
            FrameRect(gdi, rcScore, 0x2C2D2F);

            ::DrawText(gdi.GetDC(), szScore, rcScore, DT_CENTER | DT_VCENTER, m_hfScreenText, 0xFFFFFF);



            SelectObject(gdi.GetDC(), hOld);
        }

    }




    return TRUE;
}

BOOL CDlgBackJack::DrawChips(CIssGDIEx& gdi, RECT& rc)
{
    POINT ptTemp;

    static int iTemp = GetSystemMetrics(SM_CXSMICON)/8;

    ptTemp = m_ptChips;
    ptTemp.y -= iTemp;

    for(int i = 0; i < m_oGame.Get25ChipCount(); i++)
    {
        Draw(gdi, ptTemp.x, ptTemp.y-i*3, m_gdiChips.GetWidth()/3, m_gdiChips.GetHeight(),m_gdiChips , 0, 0);
    }

    ptTemp.y += iTemp;
    ptTemp.x += m_gdiChips.GetWidth()/3;
    
    for(int i = 0; i < m_oGame.Get50ChipCount(); i++)
    {
        Draw(gdi, ptTemp.x, ptTemp.y-i*3, m_gdiChips.GetWidth()/3, m_gdiChips.GetHeight(),m_gdiChips , m_gdiChips.GetWidth()/3, 0);
    }

    ptTemp.y -= iTemp;
    ptTemp.x += m_gdiChips.GetWidth()/3;

    for(int i = 0; i < m_oGame.Get100ChipCount(); i++)
    {
        Draw(gdi, ptTemp.x, ptTemp.y-i*3, m_gdiChips.GetWidth()/3, m_gdiChips.GetHeight(),m_gdiChips , m_gdiChips.GetWidth()*2/3, 0);
    }

    //brute force for double and split
    if(m_oGame.IsSplitting() || m_oGame.IsDouble())
    {
        ptTemp = m_ptChips;
        ptTemp.y += m_gdiChips.GetHeight()*4/9;
        ptTemp.x += m_gdiChips.GetHeight()/2;


        ptTemp.y -= iTemp;

        for(int i = 0; i < m_oGame.Get25ChipCount(); i++)
        {
            Draw(gdi, ptTemp.x, ptTemp.y-i*3, m_gdiChips.GetWidth()/3, m_gdiChips.GetHeight(),m_gdiChips , 0, 0);
        }

        ptTemp.y += iTemp;
        ptTemp.x += m_gdiChips.GetWidth()/3;

        for(int i = 0; i < m_oGame.Get50ChipCount(); i++)
        {
            Draw(gdi, ptTemp.x, ptTemp.y-i*3, m_gdiChips.GetWidth()/3, m_gdiChips.GetHeight(),m_gdiChips , m_gdiChips.GetWidth()/3, 0);
        }

        ptTemp.y -= iTemp;
        ptTemp.x += m_gdiChips.GetWidth()/3;

        for(int i = 0; i < m_oGame.Get100ChipCount(); i++)
        {
            Draw(gdi, ptTemp.x, ptTemp.y-i*3, m_gdiChips.GetWidth()/3, m_gdiChips.GetHeight(),m_gdiChips , m_gdiChips.GetWidth()*2/3, 0);
        }


    }
    

    return TRUE;
}

BOOL CDlgBackJack::DrawTableText(CIssGDIEx& gdiDest, RECT& rc)
{
    //lets just do the easy stuff first ... bank and bet
    static TCHAR szText[STRING_NORMAL];
    RECT rcTemp = m_rcTopText;

    if(rc.top <= m_rcTopText.top)
    {
		if(m_bDemo)
			m_oStr->Format(szText, m_oStr->GetText(ID(IDS_INFO_BankDemo), m_hInst));
		else
			m_oStr->Format(szText, m_oStr->GetText(ID(IDS_INFO_BankAmt), m_hInst), m_oGame.GetBankAmount());

        OffsetRect(&rcTemp, 1, 1);
        DrawText(gdiDest.GetDC(), szText, rcTemp, DT_LEFT | DT_TOP, m_hfScreenText, TEXT_SHADOW);
        OffsetRect(&rcTemp, -1, -1);
        DrawText(gdiDest.GetDC(), szText, rcTemp, DT_LEFT | DT_TOP, m_hfScreenText, TEXT_COLOR);

		if(m_bDemo) 
			m_oStr->Format(szText, m_oStr->GetText(ID(IDS_INFO_BetDemo), m_hInst));
		else
			m_oStr->Format(szText, m_oStr->GetText(ID(IDS_INFO_BetAmt), m_hInst), m_oGame.GetBetAmount());

        OffsetRect(&rcTemp, 1, 1);
        DrawText(gdiDest.GetDC(), szText, rcTemp, DT_RIGHT | DT_TOP, m_hfScreenText, TEXT_SHADOW);
        OffsetRect(&rcTemp, -1, -1);
        DrawText(gdiDest.GetDC(), szText, rcTemp, DT_RIGHT | DT_TOP, m_hfScreenText, TEXT_COLOR);
    }

    if(rc.top <= m_ptCountBtn.y)
    {
        static int iYOffset = (m_gdiCount.GetHeight() - 2*m_oMet.GetTextHeight())/2;

        m_oStr->Empty(szText);

        SetRect(&rcTemp, m_ptCountBtn.x, m_ptCountBtn.y + iYOffset, 
            m_ptCountBtn.x + m_gdiCount.GetWidth(), m_ptCountBtn.y + m_gdiCount.GetHeight());

        if(m_oGame.GetDifficulty() == BJDIFF_Learn)
        {
            if(m_bShowCount == TRUE)
            {
                m_oStr->Format(szText, m_oStr->GetText(ID(IDS_INFO_Count), m_hInst), m_oGame.GetCardCount());
            }
            else
            {
                switch(m_oGame.GetNextMove())
                {
                case NM_H:
                    m_oStr->StringCopy(szText, ID(IDS_BTNS_Hit), STRING_NORMAL, m_hInst);
                    break;
                case NM_S:
                    m_oStr->StringCopy(szText, ID(IDS_BTNS_Stand), STRING_NORMAL, m_hInst);
                    break;
                case NM_Sp:
                    m_oStr->StringCopy(szText, ID(IDS_BTNS_Split), STRING_NORMAL, m_hInst);
                    break;
                case NM_Su:
                    m_oStr->StringCopy(szText, ID(IDS_BTNS_Late), STRING_NORMAL, m_hInst); //surrender
                    break;
                case NM_D:
                    m_oStr->StringCopy(szText, ID(IDS_BTNS_Double), STRING_NORMAL, m_hInst);
                    break;
                default:
                    break;
                }
            }
        }

        else if(m_oGame.GetCardCountView() == CC_On || (m_oGame.GetCardCountView() == CC_SemiHidden && m_bShowCount == TRUE))
        {
            m_oStr->Format(szText, m_oStr->GetText(ID(IDS_INFO_Count), m_hInst), m_oGame.GetCardCount());
        }
        else if(m_oGame.GetCardCountView() == CC_SemiHidden)
        {
            m_oStr->Format(szText, m_oStr->GetText(ID(IDS_INFO_CardCount), m_hInst));
        }
        else 
        {
            
        }

        if(m_oStr->GetLength(szText) > 0)
        {
            OffsetRect(&rcTemp, 1, 1);
            DrawText(gdiDest.GetDC(), szText, rcTemp, DT_CENTER | DT_TOP, m_hfScreenText, TEXT_SHADOW);
            OffsetRect(&rcTemp, -1, -1);
            DrawText(gdiDest.GetDC(), szText, rcTemp, DT_CENTER | DT_TOP, m_hfScreenText, TEXT_COLOR);
        }
    }






    return TRUE;
}


BOOL CDlgBackJack::DrawHand(EnumCardHands eHand, CIssGDIEx& gdi, RECT &rc)
{
	BOOL bDrawSelector = FALSE;

	CIssBJHand* oHand;

	int iXStart = 0;
	int iYStart = 0;
	int iNumCards;

	switch(eHand)
	{
	case HAND_Dealer:
		iXStart = m_ptDealer.x;
		iYStart = m_ptDealer.y;

		oHand		= m_oGame.GetDealerHand();
		iNumCards	= m_oGame.GetNumDealerCards();
		break;
	case HAND_Player:
        if(m_oGame.IsSplitting())
		    iXStart = m_ptPlayerwSplit.x;
        else iXStart = m_ptPlayer.x;
		iYStart = m_ptPlayer.y;

		oHand		= m_oGame.GetPlayerHand();
		iNumCards	= m_oGame.GetNumPlayerCards();
		//to test
		if(m_oGame.IsSplitting() && m_oGame.GetGameState() == GSTATE_PlayerMove)
			bDrawSelector = TRUE;
		break;
	case HAND_Split:
		iXStart = m_ptSplit.x;
		iYStart = m_ptSplit.y;

		oHand		= m_oGame.GetSplitHand();
		iNumCards	= m_oGame.GetNumSplitCards();
		//to test
		if(m_oGame.IsSplitting()&& m_oGame.GetGameState() == GSTATE_SplitMove)
			bDrawSelector = TRUE;
		break;
	}

	CIssCard* oCard;
	EnumCardRank eRank;
	EnumCardSuit eSuit;

	BOOL bShowFace = TRUE;
	BOOL bDealerHide = TRUE;
	int  iXOffset, iYOffset;

	

	for(int i = 0; i < iNumCards; i++)
	{
		oCard = oHand->GetCard(i);
		eRank = oCard->m_eRank;
		eSuit = oCard->m_eSuit;

		iXOffset	= iXStart + i*CARD_SPACING;
		iYOffset	= iYStart;
		// if there are more then 5 cards then we have to put
		// the rest on the second row
		if(i>4)
		{
			iXOffset= iXStart + (i-5)*2*CARD_SPACING;
			iYOffset= iYStart + m_gdiCardBack.GetHeight()/4;
		}

        if(eHand == HAND_Dealer && i == 1 && m_bAniCardFlip)
            continue;//yeah kinda dirtyd3d

        //2nd dealer card face down
        if(eHand == HAND_Dealer && m_oGame.DealerCardFaceUp() == FALSE && i == 1)
		{
			DrawCard(gdi, iXOffset, iYOffset, eSuit, eRank, rc, FALSE);
		}
		else if(eHand == HAND_Player && //ugly!
			m_oGame.m_bDoubleDown && 
			i ==2)
		{
			iXOffset	= iXStart + CARD_SPACING/2;
			iYOffset	+= CARD_SPACING;
			DrawCard(gdi, iXOffset, iYOffset, eSuit, eRank, rc, TRUE);
		}
		else
			DrawCard(gdi, iXOffset, iYOffset, eSuit, eRank, rc, TRUE);
	}

    if(bDrawSelector)
    {
        RECT rcSel;
        rcSel.left		=  iXStart - 3;
        rcSel.top		=  iYStart - 3;
        
        if(iNumCards < 6)
        {
            rcSel.right     = iXStart + m_gdiCardBack.GetWidth() + CARD_SPACING*(iNumCards - 1)+3;
            rcSel.bottom	= iYStart + m_gdiCardBack.GetHeight() + 3;
        }
        else
        {
            rcSel.right		= iXStart + m_gdiCardBack.GetWidth() + m_gdiCardBack.GetWidth()+3;
            rcSel.bottom	= iYStart + m_gdiCardBack.GetHeight() + 3 + m_gdiCardBack.GetWidth()/5;
        }
        
        

        FrameRect(gdi.GetDC(), rcSel, RGB(255,240,0), 1);

    }

	return FALSE;



}

void CDlgBackJack::DealNextCard()
{
	SetTimer(m_hWnd, IDT_PLAYSOUND_DEAL, CARD_DEAL_SOUND_DELAY, NULL);
    UpdateWindow(m_hWnd);//higher priority than invalidaterect
}

BOOL CDlgBackJack::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(!g_wndMenu || g_wndMenu->IsWindowUp(TRUE))
		return TRUE;

	return UNHANDLED;
}


BOOL CDlgBackJack::OnLButtonUp(HWND hWnd, POINT& pt)
{
	if(!g_wndMenu || g_wndMenu->IsWindowUp(TRUE))
		return TRUE;

    if(m_oMenu.OnLButtonUp(hWnd, pt, &m_oGame))
    {
        InvalidateRect(m_hWnd, NULL, FALSE);
        return TRUE;
    }
    
    //check the count rect
    if(m_oGame.GetCardCountView() == CC_SemiHidden)
    {
        RECT rcTemp;
        SetRect(&rcTemp, m_ptCountBtn.x, m_ptCountBtn.y, m_ptCountBtn.x + m_gdiCount.GetWidth(), m_ptCountBtn.y + m_gdiCount.GetHeight());
    
        if(PtInRect(&rcTemp, pt))
        {
            m_bShowCount = TRUE;
            SetTimer(m_hWnd, IDT_COUNT_TIMER, 2500, NULL);
            InvalidateRect(m_hWnd, &rcTemp, FALSE);
        }

    }

    return UNHANDLED;
}

BOOL CDlgBackJack::OnMouseMove(HWND hWnd, POINT& pt)
{
	if(!g_wndMenu || g_wndMenu->IsWindowUp(TRUE))
		return TRUE;

	if(m_oGame.GetGameState()==GSTATE_Bet)
	{
		
	}
	else
	{

	}
	return FALSE;
}

void CDlgBackJack::FormatCurrency(TCHAR* szCurrency)
{
	if(szCurrency == NULL)
		return;

	if(szCurrency[0] == _T('-'))
		return;

	int iLen = m_oStr->GetLength(szCurrency);
	int iNumCommas = (iLen - 1 - (iLen-1)%3)/3;
	if(iNumCommas < 0)//just making sure
		iNumCommas = 0;

	for(int i = 0; i < iNumCommas; i++)
	{
		m_oStr->Insert(szCurrency, _T(","), iLen - 3*(i+1) - i);
	}

	//And finally the Dollar Symbol...Localize??
    //Pounds, Euros, Yen
	m_oStr->Insert(szCurrency, _T("$"), 0);
}

void CDlgBackJack::PlaySounds(TCHAR* szWave)
{
	BOOL bRet = FALSE;
	if(m_bPlaySounds)
    {
        bRet = PlaySound(szWave, m_hInst, SND_RESOURCE | SND_ASYNC | SND_NOWAIT);
    }
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
        if(IDYES != dlgMsg.PopupMessage(m_oStr->GetText(ID(IDS_MSG_NewGame), m_hInst), m_hWnd, m_hInst, MB_YESNO))
            break;

        NewGame();
        break;
    }
	case IDMENU_MoreApps:
		{
			CDlgMoreApps dlgMoreApps;
			dlgMoreApps.Launch(m_hInst, m_hWnd);
		}
		break;
    case IDMENU_Options:
    {
        TypeOptions sOptions;

        sOptions.bPlaySounds    = m_bPlaySounds;
        sOptions.eCardCount     = m_oGame.GetCountSystem();
        sOptions.eCardCountView = m_oGame.GetCardCountView();
        sOptions.eGameDifficulty= m_oGame.GetDifficulty();
		//sOptions.dwLang			= g_cLocale.GetCurrentLanguage();

        CDlgOptions dlgOptions;
        dlgOptions.Init(&sOptions);
        dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);

        // they could have updated the options
        UpdateOptions(sOptions);
        break;
    }
    case IDMENU_Exit:
    {
        CDlgMsgBox dlgMsg;
        if(IDYES == dlgMsg.PopupMessage(ID(IDS_MSG_Exit), m_hWnd, m_hInst, MB_YESNO))
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
		CDlgMainMenu::LaunchHelp(m_hWnd, m_hInst);
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

	if(S_OK ==GetKey(REG_KEY, _T("Difficulty"), dwVal))
		m_oGame.SetDifficulty((EnumGameDifficulty)dwVal);

    if(S_OK ==GetKey(REG_KEY, _T("CardCounting"), dwVal))
        m_oGame.SetCountSystem((EnumCardCount)dwVal);

    if(S_OK ==GetKey(REG_KEY, _T("CardCountView"), dwVal))
        m_oGame.SetCardCountView((EnumCardCountView)dwVal);

	if(S_OK ==GetKey(REG_KEY, _T("Bank"), dwVal))
		m_oGame.SetBankAmount((int)dwVal);

	if(S_OK ==GetKey(REG_KEY, _T("Bet"), dwVal))
		m_oGame.SetBetAmount((int)dwVal);

    // read in the high scores
    m_oHighScores.Init(NUM_HIGHSCORES);
    m_oHighScores.LoadRegistry(REG_KEY);
}

void CDlgBackJack::SaveRegSettings()
{
	DWORD dwTemp;

	dwTemp	= (DWORD)m_bPlaySounds;
	SetKey(REG_KEY, _T("PlaySounds"), dwTemp);

	dwTemp = (DWORD)m_oGame.GetDifficulty();
	SetKey(REG_KEY, _T("Difficulty"), dwTemp);

    dwTemp = (DWORD)m_oGame.GetCountSystem();
    SetKey(REG_KEY, _T("CardCounting"), dwTemp);

    dwTemp = (DWORD)m_oGame.GetCardCountView();
    SetKey(REG_KEY, _T("CardCountView"), dwTemp);

	dwTemp = (DWORD)m_oGame.GetBankAmount();
	SetKey(REG_KEY, _T("Bank"), dwTemp);

	dwTemp = (DWORD)m_oGame.GetBetAmount();
	SetKey(REG_KEY, _T("Bet"), dwTemp);

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
		if(m_oGame.GetGameState()==GSTATE_Bet ||
		   m_oGame.GetGameState()==GSTATE_EndOfGame)
        {}
		else
        {}
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
	//case VK_ESCAPE:
    //    PostQuitMessage(0); 
	//		break;
    case VK_TSOFT2:
        ShowMenu();
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

BOOL CDlgBackJack::IsLowRes()
{
	if(GetSystemMetrics(SM_CXSCREEN) < 240)
		return TRUE;
	else
		return FALSE;
}

BOOL CDlgBackJack::OnHandleNavBtn(HWND hWnd, WPARAM wParam)
{
    m_oMenu.OnKeyUp(hWnd, wParam, &m_oGame);

    if(wParam == VK_TSOFT1)
        wParam = VK_RETURN;

   
	InvalidateRect(hWnd, NULL, FALSE);
	return TRUE;
} 

BOOL CDlgBackJack::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
    case IDT_COUNT_TIMER:
        KillTimer(m_hWnd, IDT_COUNT_TIMER);
        m_bShowCount = FALSE;
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
	case IDT_PLAYSOUND_DEAL:
		KillTimer(m_hWnd, IDT_PLAYSOUND_DEAL);
		PlaySounds(_T("IDR_WAV_DEAL"));
		break;
    case IDT_PLAYSOUND_BJ:
        KillTimer(m_hWnd, IDT_PLAYSOUND_DEAL);
        KillTimer(m_hWnd, IDT_PLAYSOUND_BJ);
        if(m_oGame.DealAnotherCard()== FALSE)
        {
            ForceScreenUpdate();

            PlaySounds(_T("IDR_WAV_BJ"));
            POINT pt = m_ptChips;
            POINT ptStart, ptEnd;
            ptStart.x = (GetSystemMetrics(SM_CXSCREEN)-m_gdiDollar.GetWidth())/2;
            ptStart.y = pt.y + m_gdiDollar.GetHeight();
            ptEnd.x     = ptStart.x;
            ptEnd.y     = ptStart.y - m_gdiDollar.GetHeight();
            AniDollar(ptStart, ptEnd, FALSE);
            AnimateBankIncrease(m_oGame.GetPrevBankAmount(), m_oGame.GetBankAmount());
        }
        else
        {
            SetTimer(m_hWnd, IDT_PLAYSOUND_BJ, END_HAND_TIME, NULL);
        }
        break;
    case IDT_PLAYSOUND_WIN:
        KillTimer(m_hWnd, IDT_PLAYSOUND_DEAL);
        KillTimer(m_hWnd, IDT_PLAYSOUND_WIN);
        if(m_oGame.DealAnotherCard()== FALSE)
        {
            ForceScreenUpdate();
		    
		    PlaySounds(_T("IDR_WAV_WIN"));
            POINT pt = m_ptChips;
            POINT ptStart, ptEnd;
            ptStart.x = (GetSystemMetrics(SM_CXSCREEN)-m_gdiDollar.GetWidth())/2;
            ptStart.y = pt.y + m_gdiDollar.GetHeight();
            ptEnd.x     = ptStart.x;
            ptEnd.y     = ptStart.y - m_gdiDollar.GetHeight();
            AniDollar(ptStart, ptEnd, FALSE);
            AnimateBankIncrease(m_oGame.GetPrevBankAmount(), m_oGame.GetBankAmount());
		}
        else
        {
            SetTimer(m_hWnd, IDT_PLAYSOUND_WIN, END_HAND_TIME, NULL);
        }
        break;
	case IDT_PLAYSOUND_LOSE:
        KillTimer(m_hWnd, IDT_PLAYSOUND_DEAL);
        KillTimer(m_hWnd, IDT_PLAYSOUND_LOSE);
        if(m_oGame.DealAnotherCard()== FALSE)
        {
            ForceScreenUpdate();
		    PlaySounds(_T("IDR_WAVE_LOSE"));
        }
        else
        {
            SetTimer(m_hWnd, IDT_PLAYSOUND_LOSE, END_HAND_TIME, NULL);
        }
	    break;
    case IDT_PLAYSOUND_SURRENDER:
        KillTimer(m_hWnd, IDT_PLAYSOUND_SURRENDER);
        if(m_oGame.DealAnotherCard()== FALSE)
        {
            ForceScreenUpdate();
            PlaySounds(_T("IDR_WAV_BET"));
        }
        else
        {
            SetTimer(m_hWnd, IDT_PLAYSOUND_SURRENDER, END_HAND_TIME, NULL);
        }
        break;
    case IDT_PLAYSOUND_DRAW:
        KillTimer(m_hWnd, IDT_PLAYSOUND_DRAW);
        if(m_oGame.DealAnotherCard()== FALSE)
        {
            ForceScreenUpdate();
            PlaySounds(_T("IDR_WAV_BET"));
        }
        else
        {
            SetTimer(m_hWnd, IDT_PLAYSOUND_DRAW, END_HAND_TIME, NULL);
        }
        break;
	case IDT_DEAL_NEXT_CARD:
        KillTimer(m_hWnd, IDT_DEAL_NEXT_CARD);
		DealNextCard();
	case IDT_ENDGAME:
		KillTimer(m_hWnd, IDT_ENDGAME);
		{
			//We'll check for the endgame state now
			if(m_oGame.GetGameState()==GSTATE_Bet ||
				m_oGame.GetGameState()==GSTATE_EndOfGame)
			{
				if(m_oGame.GetBankAmount() + m_oGame.GetBetAmount() < 25)
				{	
                    BOOL bHighScore = FALSE;
                    //Pop the end game message and handle
                    if(m_oGame.GetHiBankAmount() > BANK_DEFAULT)
                    {
                        bHighScore = TRUE;
                        TypeHighScore sHiScore;
                        sHiScore.iBank = m_oGame.GetHiBankAmount();
                        m_oHighScores.AddHighScore(m_oGame.GetHiBankAmount(), NULL, &sHiScore,  sizeof(TypeHighScore));
                    }

                    CDlgMsgBox dlgMsg;
                    if(IDYES == dlgMsg.PopupMessage(bHighScore?ID(IDS_MSG_HiEndGame):ID(IDS_MSG_Broke), m_hWnd, m_hInst, MB_YESNO))
					{
                        NewGame(FALSE);
					}
					else
					{  //go to the Menu
                        ShowMainMenu();
					}
				}
			}
		}
    case IDT_DRAW_TIMER:
        InvalidateRect(m_hWnd, NULL, FALSE);
        break;
	default:
		return FALSE;
	    break;
	}

	return TRUE;
}

BOOL CDlgBackJack::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    switch(uiMessage)
    {
    case WM_BJRESULTS:
        {
            switch(min((int)wParam, (int)lParam))//lowest is the best result
            {
            case GRESULT_BlackJack:
                SetTimer(m_hWnd, IDT_PLAYSOUND_BJ, END_HAND_TIME, NULL);
                break;
            case GRESULT_Win:
                SetTimer(m_hWnd, IDT_PLAYSOUND_WIN, END_HAND_TIME, NULL);
                break;
                //PlaySounds(_T("IDR_WAV_BET"));
                //AnimateBankIncrease(m_oGame.GetPrevBankAmount(), m_oGame.GetBankAmount());
            	//break;
            case GRESULT_Draw:
                SetTimer(m_hWnd, IDT_PLAYSOUND_DRAW, END_HAND_TIME, NULL);
                break;
            case GRESULT_Surrender:
                SetTimer(m_hWnd, IDT_PLAYSOUND_SURRENDER, END_HAND_TIME, NULL);
                break;
            case GRESULT_Lose:
                SetTimer(m_hWnd, IDT_PLAYSOUND_LOSE, 250, NULL);
                break;
            default:
                break;
            }
        }
        break;


    case WM_SHUFFLE:
        PlaySounds(_T("IDR_WAV_SHUFFLE"));
        break;
    case WM_BJ_ENDGAME:
        SetTimer(m_hWnd, IDT_ENDGAME, 800, NULL);
    	break;
    case WM_DEALER_FLIP:
        //should be done in the engine but oh well
        if(m_oGame.GetDifficulty() != BJDIFF_Easy)
            AniDealerFlip();
        break;
    case MESSAGE_25:
    {
        RECT rc = m_oMenu.GetMenuItemRect(MENU_25);
        POINT ptStart, ptEnd;
        ptStart.x = rc.left + (WIDTH(rc)-m_gdiDollar.GetWidth())/2;
        ptStart.y = rc.top - m_gdiDollar.GetHeight();
        ptEnd.x     = ptStart.x;
        ptEnd.y     = ptStart.y - m_gdiDollar.GetHeight();
        AniDollar(ptStart, ptEnd);
        break;
    }
    case MESSAGE_50:
    {
        RECT rc = m_oMenu.GetMenuItemRect(MENU_50);
        POINT ptStart, ptEnd;
        ptStart.x = rc.left + (WIDTH(rc)-m_gdiDollar.GetWidth())/2;
        ptStart.y = rc.top - m_gdiDollar.GetHeight();
        ptEnd.x     = ptStart.x;
        ptEnd.y     = ptStart.y - m_gdiDollar.GetHeight();
        AniDollar(ptStart, ptEnd);
        break;
    }
    case MESSAGE_100:
    {
        RECT rc = m_oMenu.GetMenuItemRect(MENU_100);
        POINT ptStart, ptEnd;
        ptStart.x = rc.left + (WIDTH(rc)-m_gdiDollar.GetWidth())/2;
        ptStart.y = rc.top - m_gdiDollar.GetHeight();
        ptEnd.x     = ptStart.x;
        ptEnd.y     = ptStart.y - m_gdiDollar.GetHeight();
        AniDollar(ptStart, ptEnd);
        break;
    }
    case MESSAGE_500:
    {
        RECT rc = m_oMenu.GetMenuItemRect(MENU_500);
        POINT ptStart, ptEnd;
        ptStart.x = rc.left + (WIDTH(rc)-m_gdiDollar.GetWidth())/2;
        ptStart.y = rc.top - m_gdiDollar.GetHeight();
        ptEnd.x     = ptStart.x;
        ptEnd.y     = ptStart.y - m_gdiDollar.GetHeight();
        AniDollar(ptStart, ptEnd);
        break;
    }
    default:
        break;
    }

	return UNHANDLED;
}

BOOL CDlgBackJack::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL bRet = UNHANDLED;
#ifdef UNDER_CE
    HWND hWndSip = NULL;

    if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
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
  
    if(m_oGame.GetGameState() == GSTATE_Bet)
    {
        switch(LOWORD(wParam))
        {       
        case _T('d'):
        case _T('D'):
        case _T('1'):
            m_oGame.Deal();
            break;
        case _T('2'):
        case _T('r'):
        case _T('R'):
            m_oGame.ResetBetAmount();
            break;
        case _T('3'):
            PostMessage(hWnd, MESSAGE_25, 0,0);
            break;
        case _T('4'):
            PostMessage(hWnd, MESSAGE_50, 0,0);
            break;
        case _T('5'):
            PostMessage(hWnd, MESSAGE_100, 0,0);
            break;
        case _T('6'):
            PostMessage(hWnd, MESSAGE_500, 0,0);
            break;
        default:
            return FALSE;
        }
    }
    else
    {
        switch(LOWORD(wParam))
        {     
        case _T('1'):
        case _T('h'):
        case _T('H'):
            m_oGame.Hit();
            break;
        case _T('6'):
        case _T('i'):
        case _T('I'):
            m_oGame.Insurance();
            break;
        case _T('2'):
        case _T('s'):
        case _T('S'):
            m_oGame.Stand();
            break;
        case _T('5'):
        case _T('p'):
        case _T('P'):
            m_oGame.Split();
            break;
        case _T('3'):
        case _T('d'):
        case _T('D'):
            m_oGame.DoubleDown();
            break;
        case _T('4'):
        case _T('n'):
        case _T('N'):
            m_oGame.Surrender();
            break;
        case _T('a'):
        case _T('A'):
            AniDealerFlip();
            break;
        default:
            return UNHANDLED;
            break;
        }
    }

  
    InvalidateRect(m_hWnd, NULL, FALSE);
    return TRUE;
}

void CDlgBackJack::AniDollar(POINT ptStart, POINT ptEnd, BOOL bPlaySound)
{
#define NUM_ANI_FRAMES			1000	//aribitrary as the animation control code will dictate actual frames
#define TOTAL_ANIMATION_TIME	500

    //we're lazy so we'll play the sound here too
    if(bPlaySound)
        PlaySounds(_T("IDR_WAV_BET"));

    int iTotalHeight    = abs(ptStart.y - ptEnd.y) + m_gdiDollar.GetHeight();
    int iTotalWidth     = abs(ptStart.x - ptEnd.x) + m_gdiDollar.GetWidth();
    int iLeft           = min(ptStart.x, ptEnd.x);
    int iTop            = min(ptStart.y, ptEnd.y);

    UpdateWindow(m_hWnd);

    HDC hDC = GetDC(m_hWnd);

    CIssGDIEx gdiTemp;
    gdiTemp.Create(hDC, iTotalWidth, iTotalHeight, FALSE, TRUE);

    int iAlphaLevel = 255;
    POINT ptDrawLocation = {0};
    DWORD dwCurTime = GetTickCount();
    //we'll do the work here ... of course we'll render the fades first as they go below the focus window
    int i = GetCurrentFrame(dwCurTime, NUM_ANI_FRAMES, TOTAL_ANIMATION_TIME);

    
    while(i < NUM_ANI_FRAMES)
    {
        iAlphaLevel = 255 - 255*i/NUM_ANI_FRAMES;

        // draw the background
        BitBlt(gdiTemp.GetDC(),
               0,0,
               iTotalWidth, iTotalHeight,
               m_gdiMem.GetDC(),
               iLeft, iTop,
               SRCCOPY);

        ptDrawLocation.x    = ptStart.x;
        ptDrawLocation.y    = ptStart.y - (ptStart.y-ptEnd.y)*i/NUM_ANI_FRAMES;

        // draw the dollar sign with the given value
        Draw(gdiTemp,
             ptDrawLocation.x - iLeft,ptDrawLocation.y - iTop,
             m_gdiDollar.GetWidth(), m_gdiDollar.GetHeight(),
             m_gdiDollar,
             0,0,
             ALPHA_Normal,
             iAlphaLevel);

        // draw to the screen
        BitBlt(hDC,
               iLeft, iTop,
               iTotalWidth, iTotalHeight,
               gdiTemp.GetDC(),
               0,0,
               SRCCOPY);

        i = GetCurrentFrame(dwCurTime, NUM_ANI_FRAMES, TOTAL_ANIMATION_TIME);

        if(ShouldAbort(m_hWnd))
            break;
    }
    ReleaseDC(m_hWnd, hDC);

    InvalidateRect(m_hWnd, NULL, FALSE);
}

void CDlgBackJack::AniDealerFlip()
{
    //draw the board without the face down dealer card

    m_bAniCardFlip = TRUE;
    m_aniCardFlip.ResetAnimation();
    
    RECT rcClip;
    HDC hDC = GetDC(m_hWnd);

    CIssGDIEx gdiTemp;

    gdiTemp.Create(hDC, m_gdiMem.GetWidth(), m_gdiMem.GetHeight(), FALSE, TRUE);

    GetClientRect(m_hWnd, &rcClip);

    while(m_bAniCardFlip)
    {
        if(m_aniCardFlip.IsDoneAnimating())
            m_bAniCardFlip = FALSE;

        OnPaint(m_hWnd, gdiTemp.GetDC(), rcClip);
            
        if(m_bAniCardFlip)
            m_aniCardFlip.Draw(gdiTemp, 
                m_ptDealer.x + CARD_SPACING,
                m_ptDealer.y - m_aniCardFlip.GetHeight() + m_gdiCardBack.GetHeight(),
                GetTickCount());

        BitBlt(hDC,
            rcClip.left, rcClip.top, 
            rcClip.right - rcClip.left,
            rcClip.bottom - rcClip.top,
            gdiTemp.GetDC(),
            rcClip.left,
            rcClip.top,
            SRCCOPY);

    }
    ReleaseDC(m_hWnd, hDC);
}

void CDlgBackJack::StartAnimation()
{
    KillTimer(m_hWnd, IDT_DRAW_TIMER);
    SetTimer(m_hWnd, IDT_DRAW_TIMER, DRAW_TIMER_DELAY, NULL);
}

void CDlgBackJack::EndAnimation()
{
    KillTimer(m_hWnd, IDT_DRAW_TIMER);
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
            RecalcCardLocations();
            m_oGame.Init(m_hWnd);
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

    TypeOptions sOptions;

    sOptions.bPlaySounds    = m_bPlaySounds;
    sOptions.eCardCount     = m_oGame.GetCountSystem();
    sOptions.eCardCountView = m_oGame.GetCardCountView();
    sOptions.eGameDifficulty= m_oGame.GetDifficulty();

    dlgMenu.Init(&m_oHighScores, m_oGame.HasGameStarted(), &sOptions);
    UINT uiResult = (UINT)dlgMenu.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);

    // they could have updated the options
    UpdateOptions(sOptions);

    switch(uiResult)
    {
    case IDMENU_NewGame:
        m_oGame.Init(m_hWnd);
        NewGame();
        break;
    case IDMENU_Resume:
        m_oGame.Init(m_hWnd);
        break;
    case IDMENU_Exit:
        PostQuitMessage(0);
        return TRUE;

    }

    return TRUE;
}

void CDlgBackJack::UpdateOptions(TypeOptions& sOptions)
{
    // check if nothing changed
    if(m_bPlaySounds == sOptions.bPlaySounds &&
       m_oGame.GetCountSystem() == sOptions.eCardCount &&
       m_oGame.GetCardCountView() == sOptions.eCardCountView &&
       m_oGame.GetDifficulty() == sOptions.eGameDifficulty &&
       g_cLocale.GetCurrentLanguage() == sOptions.dwLang)
       return;

    m_bPlaySounds       = sOptions.bPlaySounds;
    
    BOOL bNewGame = FALSE;

    if(/* m_oGame.GetCountSystem() != sOptions.eCardCount ||
        m_oGame.GetCardCountView() != sOptions.eCardCountView ||*/
        m_oGame.GetDifficulty() != sOptions.eGameDifficulty)
        bNewGame = TRUE;

    if(m_oGame.GetCardCountView() != sOptions.eCardCountView)
        m_gdiBackground.Destroy();//for the count square


    m_oGame.SetCountSystem(sOptions.eCardCount);
    m_oGame.SetCardCountView(sOptions.eCardCountView);
    m_oGame.SetDifficulty(sOptions.eGameDifficulty);
    //g_cLocale.SetCurrentLanguage(sOptions.dwLang);

    if(bNewGame)
        NewGame();

    SaveRegSettings();

}

void CDlgBackJack::NewGame(BOOL bCheckHiScore)
{
    if(m_oGame.GetHiBankAmount() > BANK_DEFAULT && bCheckHiScore)
    {
        TypeHighScore sHiScore;
        sHiScore.iBank = m_oGame.GetHiBankAmount();
        m_oHighScores.AddHighScore(m_oGame.GetHiBankAmount(), NULL, &sHiScore,  sizeof(TypeHighScore));
    }
    m_oGame.Reset();
    InvalidateRect(m_hWnd, NULL, FALSE);
    PlaySounds(_T("IDR_WAV_SHUFFLE"));
}

BOOL CDlgBackJack::ShowMenu()
{
	if(!g_wndMenu || g_wndMenu->IsWindowUp(TRUE))
		return TRUE;

	g_wndMenu->ResetContent();

	g_wndMenu->AddItem(ID(IDS_MENU_NewGame), m_hInst, IDMENU_NewGame);
	g_wndMenu->AddSeparator();
#ifndef NMA
	g_wndMenu->AddItem(ID(IDS_MENU_MoreApps), m_hInst, IDMENU_MoreApps);
#endif
	g_wndMenu->AddItem(ID(IDS_MENU_Options), m_hInst, IDMENU_Options);
	g_wndMenu->AddItem(ID(IDS_MENU_HiScores), m_hInst, IDMENU_HiScores);
	g_wndMenu->AddItem(ID(IDS_MENU_About), m_hInst, IDMENU_About);
	g_wndMenu->AddItem(ID(IDS_MENU_Help), m_hInst, IDMENU_Help);
	g_wndMenu->AddSeparator();
	g_wndMenu->AddItem(ID(IDS_MENU_Exit), m_hInst, IDMENU_Exit);

	g_wndMenu->SetSelectedItemIndex(0, TRUE);

	int iTotalHeight = g_wndMenu->GetTotalListHeight();

	RECT rc;
	GetWindowRect(m_hWnd, &rc);

	int iWidth = WIDTH(rc)*2/3;
	int iHeight= HEIGHT(rc) - HEIGHT(m_oMenu.GetMenuRect());

	DWORD dwAttributes = OPTION_AlwaysShowSelector|OPTION_CircularList|OPTION_Bounce;

	// if we need to scroll
	if(iTotalHeight > iHeight)
	{
		dwAttributes |= OPTION_DrawScrollArrows;
		iWidth += g_wndMenu->GetImageArrayGDI().GetHeight();
	}

	g_wndMenu->PopupMenu(m_hWnd, m_hInst,
		dwAttributes,
		rc.right - iWidth,rc.top,iWidth,iHeight,
		0, 0, 0, 0, ADJUST_Bottom);


    // don't show it twice
   /* if(m_oMenuHandler.IsMenuUp())
        return TRUE;

    //Create the menu.
    HMENU hMenu			= CreatePopupMenu();

    if(!hMenu)
    {
        return TRUE;
    }

    m_oMenuHandler.Initialize(m_hWnd, m_hInst, RGB(255,255,255), 0x8DEEA4, TRUE, 0);

 //   m_oMenuHandler.AppendMenu(hMenu, MF_SEPARATOR, IDMENU_Separator, 0);
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_NewGame, ID(IDS_MENU_NewGame));
	m_oMenuHandler.AppendMenu(hMenu, MF_SEPARATOR, IDMENU_Separator, 0);

#ifndef NMA
	m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_MoreApps, ID(IDS_MENU_MoreApps));
#endif
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_Options, ID(IDS_MENU_Options));
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_HiScores, ID(IDS_MENU_HiScores));
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_About, ID(IDS_MENU_About));
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_Help, ID(IDS_MENU_Help));
//    m_oMenuHandler.AppendMenu(hMenu, MF_SEPARATOR, IDMENU_Separator, 0);
    m_oMenuHandler.AppendMenu(hMenu, MF_STRING, IDMENU_Exit, ID(IDS_MENU_Exit));

    POINT pt;
    int iShadowSpace= 3*GetSystemMetrics(SM_CYSCREEN)/100;

    RECT rcTemp;
    GetClientRect(m_hWnd, &rcTemp);

    pt.x	= rcTemp.right;
    pt.y	= m_oMenu.GetMenuRect().top;

    //Display it.
    m_oMenuHandler.PopupMenu(	hMenu, 
        TPM_RIGHTALIGN|TPM_BOTTOMALIGN, 
        pt.x,pt.y);

    DestroyMenu(hMenu);*/

    return TRUE;
}

/* BOOL CDlgBackJack::OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
}*/



BOOL CDlgBackJack::AnimateBankIncrease(int iOldBank, int iNewBank)
{
/*    HDC hdc;
    hdc = GetDC(m_hWnd);

    //copy the little piece of background into the GDI
    CIssGDIEx gdiBackGround;
    CIssGDIEx gdiTemp;

    RECT rcTemp;
    SetRect(&rcTemp, 0, 0, WIDTH(m_rcTopText)/2, HEIGHT(m_rcTopText));

    gdiBackGround.Create(hdc, rcTemp, FALSE, FALSE);
    gdiTemp.Create(hdc, rcTemp, FALSE, FALSE);
    BitBlt(gdiBackGround.GetDC(), 0, 0, WIDTH(rcTemp), HEIGHT(rcTemp), m_gdiBackground.GetDC(), m_rcTopText.left, m_rcTopText.top, SRCCOPY);

    TCHAR szTemp[STRING_NORMAL];

    for(int i = iOldBank; i <= iNewBank; i++)
    {
        m_oStr->IntToString(szTemp, i);
        BitBlt(gdiTemp.GetDC(), 0, 0, WIDTH(rcTemp), HEIGHT(rcTemp), gdiBackGround.GetDC(), 0, 0, SRCCOPY);

        m_oStr->Format(szTemp, _T("Bank: $%i"), i);
        OffsetRect(&rcTemp, 1, 1);
        DrawText(gdiTemp.GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hfScreenText, TEXT_SHADOW);
        OffsetRect(&rcTemp, -1, -1);
        DrawText(gdiTemp.GetDC(), szTemp, rcTemp, DT_LEFT | DT_TOP, m_hfScreenText, TEXT_COLOR);


        BitBlt(hdc, m_rcTopText.left, m_rcTopText.top, WIDTH(rcTemp), HEIGHT(rcTemp), gdiTemp.GetDC(), 0, 0, SRCCOPY);

        if(ShouldAbort(m_hWnd))
            break;
    }

    ReleaseDC(m_hWnd, hdc);*/
    return TRUE;
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