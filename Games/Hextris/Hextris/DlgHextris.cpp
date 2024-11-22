#include "StdAfx.h"
#include "DlgHextris.h"
#include "resource.h"
#include "IssRegistry.h"
#include "Commdlg.h"
#include "Winuser.h"
#include "IssCommon.h"
#include "IssGDIEffects.h"
#include "DlgSplashScreen.h"
#include "DlgMainMenu.h"
#include "DlgGameOver.h"
#include "DlgOptions.h"
#include "DlgHighScores.h"
#include "DlgAbout.h"
#include "DlgExit.h"

#define TXT_Title		_T("iSS HexeGems 1.0")
#define TXT_Name		_T("HexeGems")

#define NUM_HEX_COLORS 10
#define NUM_ORB_Frames	40
#define DRAW_TIMER_DELAY 15
#define TIME_CompleteScoreAddition	1500
#define FRAME_ScoreAdditionCount	20


#define ANI_WAIT_TIME	50


#define ITEMS_SIZE		(m_sizeHex.cx*3/4)

#define FONT_TEXT_SIZE	(GetSystemMetrics(SM_CXSMICON)*32/ICON_RATIO)

COLORREF cores[NUM_HEX_COLORS][3] = { 

	{RGB(127,0,191),	RGB(78,18,82),		RGB(92,0,127)},			//purple
	{RGB(0,191,0),		RGB(67,154,10),		RGB(0,127,0)},			//green
	{RGB(191,0,0),		RGB(142,9,9),		RGB(127,0,0)},			//red
	{RGB(0,95,191),		RGB(0,64,128),		RGB(0,64,127)},			//blue
	{RGB(191,191,0),	RGB(144,152,3),		RGB(127,127,0)},		//yellow
	{RGB(191,95,0),		RGB(140,61,3),		RGB(127,64,0)},			//orange
	{RGB(191,95,0),		RGB(140,61,3),		RGB(127,64,0)},			//orange
	{RGB(127,0,191),	RGB(78,18,82),		RGB(92,0,127)},			//purple
	{RGB(0,191,0),		RGB(67,154,10),		RGB(0,127,0)},			//green
	{RGB(0,95,191),		RGB(0,64,128),		RGB(0,64,127)},			//blue
};

//#define HEX_RATIO 866
//#define HEX_SIZE 10 //use even numbers only


#ifdef WIN32_PLATFORM_WFSP
#define ICON_RATIO 44
#else
#define ICON_RATIO 32
#endif

#define HEX_RATIO		900
#define HEX_SIZE		10	//use even numbers only - even beccause of the background pattern
#define HEX_SIZE_MED	6	//odd or even numbers are ok here
#define HEX_SIZE_SMALL	7	//odd or even numbers are ok here

#define BOTTOM_INDENT	(GetSystemMetrics(SM_CXSMICON) - 2)//this will make room for our level indicator

#define IDT_IDLE_TIMER	1 //Timer to force game pieces down
#define IDT_CLOCK_TIMER 2
#define IDT_ANI_LEVEL   3 //this is a hack timer to correct the order of the game engine posting messages .. specfically it posts level change before line clear
#define IDT_LEVEL_UP	4 //timer so it plays the level up message a little later
#define IDT_TIMER_HISCORE 5

#define IDT_DRAW_TIMER 6

//Background
#define BACKGROUND_HEXCOLOR			0x1A1A1A
#define BACKGROUND_OUTLINECOLOR		0x333333
#define BACKGROUND_GRAD1			0x4A4A4A
#define BACKGROUND_GRAD2			0x4A4A4A

#define OUTLINE_COLOR				0x5C5C5C

CDlgHextris::CDlgHextris(void)
{
	m_iHexSize			= HEX_SIZE*GetSystemMetrics(SM_CXICON)/ICON_RATIO;
	m_iHexSizeMed		= HEX_SIZE_MED*GetSystemMetrics(SM_CXICON)/ICON_RATIO;
	m_iHexSizeSmall		= HEX_SIZE_SMALL*GetSystemMetrics(SM_CXICON)/ICON_RATIO;
	m_iHexSizeCos30		= m_iHexSize*HEX_RATIO/1000;

	m_sizeHex.cx		= m_iHexSize*2+1;
	m_sizeHex.cy		= m_iHexSizeCos30*2+1;

	m_iHexSizeMedCos30	= m_iHexSizeMed*HEX_RATIO/1000;

	m_sizeHexMed.cx		= m_iHexSizeMed*2+1;
	m_sizeHexMed.cy		= m_iHexSizeMedCos30*2+1;

	m_iHexSizeSmallCos30= m_iHexSizeSmall*HEX_RATIO/1000;

	m_sizeHexSmall.cx	= m_iHexSizeSmall*2+1;
	m_sizeHexSmall.cy	= m_iHexSizeSmallCos30*2+1;

	m_hFontText			= CIssGDI::CreateFont(GetSystemMetrics(SM_CXSMICON), FW_BOLD, TRUE);

	m_iNumExplosionFrames			= 0;
	m_iCurrentExplosionFrame		= 0;
	m_dwFrameCount					= 0;
	m_iNumFireballFrames			= 0;
	m_bDrawFloat					= FALSE;
	m_dwStartScoreAddition			= 0;

	m_bShowSplashScreen = TRUE;

	m_iNumPulseFrames = 11;

	m_oGame.LoadRegistry();
}

CDlgHextris::~CDlgHextris(void)
{
	m_oStr->DeleteInstance();
	CIssGDI::DeleteFont(m_hFontText);
	m_oGame.SaveRegistry();	
}

/*

This is where we set most of the sizes and positions

OnSize is called when the application starts up and whenever the screen size changes - so in the case of Windows Mobile that's only on an orientation change

So assume this code is run once

*/
BOOL CDlgHextris::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	if(m_gdiMem.GetWidth() == WIDTH(rcClient) && m_gdiMem.GetHeight() == HEIGHT(rcClient))
		return TRUE;

	m_gdiMem.Destroy();
	m_gdiBackground.Destroy();
	m_gdiTempBack.Destroy();
	m_gdiLevelBackground.Destroy();
	m_gdiLevel.Destroy();

	int iIndent = GetSystemMetrics(SM_CXSMICON)/2;
	int iYStart = iIndent/2;


	//this will of course be the edge of the game field (always)
	//we can use this value to center our piece previews
	int iRightGameEdge = GAME_WIDTH/2*(m_sizeHex.cy + m_iHexSize) + m_iHexSize;



#define TEXT_INDENT_AMNT (GetSystemMetrics(SM_CXSMICON)*8/ICON_RATIO)

	//now lets set up the text rects
	//aligned to top left
	m_rcLines.left	= TEXT_INDENT_AMNT;//just a little indent
	m_rcLines.right = GetSystemMetrics(SM_CXSCREEN);//who cares ... lots of room
	m_rcLines.top	= TEXT_INDENT_AMNT;
	m_rcLines.bottom= GetSystemMetrics(SM_CXICON)*32/ICON_RATIO;

	//the high score will be at the top of the right hand side
	//aligned to top left of this rect
	m_rcScore.top = TEXT_INDENT_AMNT;
	m_rcScore.bottom = m_rcScore.top + GetSystemMetrics(SM_CXICON)*32/ICON_RATIO;
	m_rcScore.left = TEXT_INDENT_AMNT;
	m_rcScore.right = iRightGameEdge - TEXT_INDENT_AMNT;


//lets set the background
	m_rcBackGround.left		= iRightGameEdge;
	m_rcBackGround.right	= GetSystemMetrics(SM_CXSCREEN);
	m_rcBackGround.top		= rcClient.top + GetSystemMetrics(SM_CXSMICON)*64/ICON_RATIO;
	m_rcBackGround.bottom	= rcClient.bottom;

	int iShadowSpace		= 2*rcClient.bottom/100;
	int iBtnHeight			= GetSystemMetrics(SM_CYICON)*48/ICON_RATIO;

	m_rcMenu				= m_rcBackGround;
	m_rcMenu.right			+= iShadowSpace;
	//m_rcMenu.left			+= GetSystemMetrics(SM_CXSMICON)/2;
	m_rcMenu.top			= m_rcMenu.bottom - iBtnHeight + iShadowSpace;
	m_rcMenu.bottom			= m_rcMenu.top + iBtnHeight;

	if(GetSystemMetrics(SM_CXSCREEN) >= GetSystemMetrics(SM_CYSCREEN))
	{
		m_rcPreview1			= m_rcBackGround;
		m_rcPreview1.top		= 0;
		m_rcPreview1.bottom		= m_rcPreview1.top + 5*m_sizeHexMed.cy;

		m_rcSaved				= m_rcBackGround;
		m_rcSaved.top			= m_rcPreview1.bottom;
		m_rcSaved.bottom		= m_rcPreview1.top + 10*m_sizeHexMed.cy;
	}
	else //portrait
	{
		m_rcPreview1			= m_rcBackGround;
		m_rcPreview1.top		= 0;
		m_rcPreview1.bottom		= m_rcPreview1.top + 7*m_sizeHexMed.cy;

		m_rcSaved				= m_rcBackGround;
		m_rcSaved.top			= m_rcPreview1.bottom;
		m_rcSaved.bottom		= m_rcPreview1.top + 14*m_sizeHexMed.cy;
	}

	//first piece preview
	int j;

	j = m_rcPreview1.bottom - (HEIGHT(m_rcPreview1) - m_sizeHexMed.cy/2)/2;

	int iXStart = (GetSystemMetrics(SM_CXSCREEN) - iRightGameEdge)/2 - m_iHexSizeMed*3 + iRightGameEdge;

	for(int iRow = 0; iRow < 4; iRow++)//3 rows
	{
		for(int i = 0; i < 2; i++)//4 columns
		{
			m_ptPreview1[i*2][iRow].x = i*m_iHexSizeMed*3 + m_iHexSizeMed/2 + iXStart;
			m_ptPreview1[i*2][iRow].y = j;

			m_ptPreview1[i*2+1][iRow].x = i*m_iHexSizeMed*3 + 2*m_iHexSizeMed + iXStart;
			m_ptPreview1[i*2+1][iRow].y = m_iHexSizeMedCos30 + m_ptPreview1[i*2][iRow].y;
		}

		j -= 2*m_iHexSizeMedCos30;
	}

	//banked or saved piece
	j = m_rcSaved.bottom - (HEIGHT(m_rcSaved) - m_sizeHexMed.cy/2)/2;

	iXStart = (GetSystemMetrics(SM_CXSCREEN) - iRightGameEdge)/2 - m_iHexSizeMed*3 + iRightGameEdge;

	for(int iRow = 0; iRow < 4; iRow++)//3 rows
	{
		for(int i = 0; i < 2; i++)//4 columns
		{
			m_ptSaved[i*2][iRow].x = i*m_iHexSizeMed*3 + m_iHexSizeMed/2 + iXStart;
			m_ptSaved[i*2][iRow].y = j;

			m_ptSaved[i*2+1][iRow].x = i*m_iHexSizeMed*3 + 2*m_iHexSizeMed + iXStart;
			m_ptSaved[i*2+1][iRow].y = m_iHexSizeMedCos30 + m_ptSaved[i*2][iRow].y;
		}

		j -= 2*m_iHexSizeMedCos30;
	}

	m_rcGS3					= m_rcBackGround;
	m_rcGS3.top				= m_rcSaved.bottom;

	//now the rest of the text will be at the bottom of the right side of the screen so we'll go to the bottom of the text now
	m_rcLevel2.left  = TEXT_INDENT_AMNT*2 + iRightGameEdge;
	m_rcLevel2.right = GetSystemMetrics(SM_CXSCREEN) - TEXT_INDENT_AMNT;
	m_rcLevel2.top   = m_rcSaved.bottom + TEXT_INDENT_AMNT;
	m_rcLevel2.bottom = m_rcLevel2.top + GetSystemMetrics(SM_CXSMICON)*32/ICON_RATIO;

	m_rcLevel		 = m_rcLevel2;
	m_rcLevel.top	 = m_rcLevel2.bottom;
	m_rcLevel.bottom = m_rcLevel.top + GetSystemMetrics(SM_CXSMICON)*32/ICON_RATIO;

	m_rcFPS			 = m_rcLevel2;
	m_rcFPS.top		 = m_rcLevel.bottom;
	m_rcFPS.bottom	 = m_rcFPS.top + GetSystemMetrics(SM_CXSMICON)*32/ICON_RATIO;

	if(GetSystemMetrics(SM_CXSCREEN) < GetSystemMetrics(SM_CYSCREEN))
	{
		m_rcBonus		 = m_rcLevel2;
		m_rcBonus.top	 = m_rcFPS.bottom+ GetSystemMetrics(SM_CXSMICON)*32/ICON_RATIO;
		m_rcBonus.bottom = m_rcBonus.top + GetSystemMetrics(SM_CXSMICON)*32/ICON_RATIO;
	}
	else
	{
		m_rcBonus		 = m_rcLevel2;
		m_rcBonus.top	 = m_rcFPS.bottom;
		m_rcBonus.bottom = m_rcBonus.top + GetSystemMetrics(SM_CXSMICON)*32/ICON_RATIO;
	}

	//bonus orb time
	int iSpacing	= (GetSystemMetrics(SM_CXSCREEN) - iRightGameEdge - ITEMS_SIZE*MAX_SAVED_GEMS - 2*TEXT_INDENT_AMNT)/7;

	if(iSpacing < 0)
		iSpacing = 0;

	m_rcOrbs[0].left = iSpacing + iRightGameEdge + 2*TEXT_INDENT_AMNT;
	m_rcOrbs[0].right= m_rcOrbs[0].left + ITEMS_SIZE;
	m_rcOrbs[0].top  = m_rcBonus.bottom;
	m_rcOrbs[0].bottom= m_rcOrbs[0].top + ITEMS_SIZE; //should be about right

	m_rcOrbs[1].left = m_rcOrbs[0].right + iSpacing;
	m_rcOrbs[1].right= m_rcOrbs[1].left + ITEMS_SIZE;
	m_rcOrbs[1].top  = m_rcOrbs[0].top;
	m_rcOrbs[1].bottom=m_rcOrbs[0].bottom;

	m_rcOrbs[2].left = m_rcOrbs[1].right + iSpacing;
	m_rcOrbs[2].right= m_rcOrbs[2].left + ITEMS_SIZE;
	m_rcOrbs[2].top  = m_rcOrbs[0].top;
	m_rcOrbs[2].bottom=m_rcOrbs[0].bottom;

	m_rcOrbs[3].left = m_rcOrbs[2].right + iSpacing;
	m_rcOrbs[3].right= m_rcOrbs[3].left + ITEMS_SIZE;
	m_rcOrbs[3].top  = m_rcOrbs[0].top;
	m_rcOrbs[3].bottom=m_rcOrbs[0].bottom;

	//set up the 4 rects for NEXT ... should probably base this off the piece preview rect
	m_rcN.left		= iRightGameEdge+GetSystemMetrics(SM_CXSMICON)*4/ICON_RATIO;
	m_rcN.right		= GetSystemMetrics(SM_CXSCREEN);
	m_rcN.top		= FONT_TEXT_SIZE*4/5;
	m_rcN.bottom	= m_rcN.top + FONT_TEXT_SIZE*4/5;

	m_rcE.left		= m_rcN.left;
	m_rcE.right		= m_rcN.right;
	m_rcE.top		= m_rcN.bottom;
	m_rcE.bottom	= m_rcE.top + FONT_TEXT_SIZE*4/5;

	m_rcX.left		= m_rcN.left;
	m_rcX.right		= m_rcN.right;
	m_rcX.top		= m_rcE.bottom;
	m_rcX.bottom	=m_rcX.top + FONT_TEXT_SIZE*4/5;

	m_rcT.left		= m_rcN.left;
	m_rcT.right		= m_rcN.right;
	m_rcT.top		= m_rcX.bottom;
	m_rcT.bottom	=m_rcT.top + FONT_TEXT_SIZE*4/5;

	return TRUE;
}


BOOL CDlgHextris::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

/*	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= hWnd;
	mbi.nToolBarId	= IDR_MENU_Hextris;
	mbi.hInstRes	= m_hInst;
	if(!SHCreateMenuBar(&mbi))
		ASSERT(0);*/

	
#endif

	m_oGame.Init(m_hWnd);
	return TRUE;
}

BOOL CDlgHextris::ShowMenu()
{
	// don't show it twice
	if(m_oMenu.IsMenuUp())
		return TRUE;

	Pause();

	//Create the menu.
	HMENU hMenu			= CreatePopupMenu();

	if(!hMenu)
	{
		Resume();
		return TRUE;
	}

	m_oMenu.Initialize(m_hWnd, m_hInst, RGB(255,255,255), RGB(0,100,200), TRUE, IDR_BEVEL);

	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Resume, IDS_MENU_Resume);
	m_oMenu.AppendMenu(hMenu, MF_SEPARATOR, IDMENU_Separator, 0);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_NewGame, IDS_MENU_NewGame);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Options, IDS_MENU_Options);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_HiScores, IDS_MENU_HiScores);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_About, IDS_MENU_About);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Help, IDS_MENU_Help);
	m_oMenu.AppendMenu(hMenu, MF_SEPARATOR, IDMENU_Separator, 0);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Exit, IDS_MENU_Exit);

	POINT pt;
	int iShadowSpace= 3*GetSystemMetrics(SM_CYSCREEN)/100;
	RECT rc = m_rcMenu;
	pt.x	= rc.right - iShadowSpace;
	pt.y	= rc.top + iShadowSpace;

	//Display it.
	m_oMenu.PopupMenu(	hMenu, 
		TPM_RIGHTALIGN|TPM_BOTTOMALIGN, 
		pt.x,pt.y);

	DestroyMenu(hMenu);

	Resume();
	return TRUE;
}

BOOL CDlgHextris::OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnMeasureItem(hWnd, wParam, lParam);
}

BOOL CDlgHextris::OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnDrawItem(hWnd, wParam, lParam);
}

BOOL CDlgHextris::OnEnterMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return m_oMenu.OnEnterMenuLoop(hWnd, wParam, lParam);
}

BOOL CDlgHextris::ShowMainMenu()
{
	ReloadSounds();

	Pause();
	
	CDlgMainMenu	dlgMenu;
	dlgMenu.Init(&m_oSoundFX, &m_oGame, &m_oKeys);
	UINT uiResult = (UINT)dlgMenu.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
	switch(uiResult)
	{
	case IDMENU_Resume:
		if(m_oGame.IsGameInPlay())
		{
			ResetFPS();
			Resume();
			break;
		}
		// otherwise fall through
	case IDMENU_NewGame:
		NewGame();
		break;
	case IDMENU_Exit:
		PostQuitMessage(0);
		return TRUE;
	
	}

	return TRUE;
}

BOOL CDlgHextris::ShowSplashScreen()
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
			ReloadSounds();
			SendMessage(dlgSplashScreen.GetWnd(), WM_SET_Percent, 10, 0);
			break;
		case WM_LOAD_Fonts:
			ReloadFonts();
			SendMessage(dlgSplashScreen.GetWnd(), WM_SET_Percent, 15, 0);
			break;
		case WM_LOAD_Background:
			ReloadBackground(dlgSplashScreen.GetWnd(), 60);
			SendMessage(dlgSplashScreen.GetWnd(), WM_SET_Percent, 60, 0);
			break;
		case WM_lOAD_Hexagons:
			ReloadHexagons(dlgSplashScreen.GetWnd(), 100);
			SendMessage(dlgSplashScreen.GetWnd(), WM_SET_Percent, 100, 0);
			break;
		}

		if(msg.message == WM_lOAD_Hexagons)
			break;
	}

	ShowMainMenu();
	return TRUE;
}


BOOL CDlgHextris::DrawBackground(HDC hdc, RECT rcClient)
{
	// make sure the background is loaded
	ReloadBackground();

	if(m_bIsBackgroundDirty)
	{
		BitBlt(m_gdiTempBack.GetDC(), rcClient.left, 
			rcClient.top, 
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			m_gdiBackground.GetDC(), 
			rcClient.left, rcClient.top,
			SRCCOPY);

		DrawGameBoard(m_gdiTempBack, rcClient);
		DrawPiecePreview(m_gdiTempBack, rcClient);//this handles both previews and the stored piece
		m_bIsBackgroundDirty = FALSE;
	}

	BitBlt(m_gdiMem.GetDC(), rcClient.left, 
		rcClient.top, 
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		m_gdiTempBack.GetDC(), 
		rcClient.left, rcClient.top,
		SRCCOPY);

	return TRUE;
}

BOOL CDlgHextris::DrawPiecePreview(CIssGDI &gdiDest, RECT rcClient)
{
	static POINT pt;

	POINT ptTemp;

	int iYOffset = 0;

	//a little brute force for the moment
	for(int i = 0; i < 4; i++)
	{
		if(i%2==0)
			iYOffset++;

		for(int j = 1; j < 5; j++)
		{
			//first piece
			if(m_oGame.IsGamePiece(m_oGame.GetNextPiece(1)->GetHex(i, j)->ePiece))
			{
				ptTemp.x = i;
				ptTemp.y = 4;
				ptTemp.y -= (j+iYOffset);
				if(ptTemp.y < 4 && ptTemp.y >= -1)
				{
					DrawHexagon(gdiDest.GetDC(), m_ptPreview1[ptTemp.x][ptTemp.y+1], m_oGame.GetNextPiece(1)->GetPieceType(), HEXTYPE_Med);
				}
			}
			//second piece
		/*	if(m_oGame.IsGamePiece(m_oGame.GetNextPiece(2)->GetHex(i, j)->ePiece))
			{
				ptTemp.x = i;
				ptTemp.y = 4;
				ptTemp.y -= (j+iYOffset);
				if(ptTemp.y < 4 && ptTemp.y >= -1)
				{
					DrawHexagon(gdiDest.GetDC(), m_ptPreview2[ptTemp.x][ptTemp.y+1], m_oGame.GetNextPiece(2)->GetPieceType(), HEXTYPE_Small);
				}
			}*/
			//saved piece
			if(m_oGame.IsGamePiece(m_oGame.GetSavedPiece()->GetHex(i, j)->ePiece))
			{
				ptTemp.x = i;
				ptTemp.y = 4;
				ptTemp.y -= (j+iYOffset);
				if(ptTemp.y < 4 && ptTemp.y >= -1)
				{
					DrawHexagon(gdiDest.GetDC(), m_ptSaved[ptTemp.x][ptTemp.y+1], m_oGame.GetSavedPiece()->GetPieceType(), HEXTYPE_Med);
				}
			}
		}
	}
	return TRUE;
}

BOOL CDlgHextris::DrawCurrentPiece(CIssGDI &gdiDest, RECT rcClient)
{
	//its just easier to grab it and draw
	CurrentPiece* oPiece = m_oGame.GetCurrentPiece();
	static POINT ptTemp;

	int iYOffset = 0;//I'm not happy about this either ....

	for(int i = 0; i < 5; i++)
	{
		if((i+oPiece->GetLocation()->x)%2==0)
			iYOffset++;

		for(int j = 0; j < 5; j++)
		{
			if(m_oGame.IsGamePiece(oPiece->GetHex(i, j)->ePiece))
			{
				//so ... the current piece stores its position relative to the grid (bottom left being 0,0 ... this are grid points not screen points
				//m_ptGameField is an array of screen points the size of the current grid
				//so we get the piece position (which is top left

				ptTemp.x = oPiece->GetLocation()->x;
				ptTemp.y = oPiece->GetLocation()->y;
				ptTemp.x += i;
				ptTemp.y -= (j+iYOffset);
				if(ptTemp.y < m_oGame.GetNumRows())
				{
					if(oPiece->GetHex(i, j)->ePiece == HEX_GamePiece)
						DrawHexagon(gdiDest, m_ptGameField[ptTemp.x][ptTemp.y], oPiece->GetPieceType(), HEXTYPE_Normal);
					else
						DrawOrb(gdiDest, m_ptGameField[ptTemp.x][ptTemp.y], ptTemp.x, ptTemp.y);


				}
			}
		}
	}




	return TRUE;
}

BOOL CDlgHextris::DrawGhost(HDC hdc, RECT rcClient)
{
	static POINT ptTemp;
	CurrentPiece* oPiece = m_oGame.GetGhost();

	int iYOffset = 0;//I'm not happy about this either ....

	for(int i = 0; i < 5; i++)
	{
		if((i+oPiece->GetLocation()->x)%2==0)
			iYOffset++;

		for(int j = 0; j < 5; j++)
		{
			if(m_oGame.IsGamePiece(oPiece->GetHex(i, j)->ePiece))
			{
				//so ... the current piece stores its position relative to the grid (bottom left being 0,0 ... this are grid points not screen points
				//m_ptGameField is an array of screen points the size of the current grid
				//so we get the piece position (which is top left

				ptTemp.x = oPiece->GetLocation()->x;
				ptTemp.y = oPiece->GetLocation()->y;
				ptTemp.x += i;
				ptTemp.y -= (j+iYOffset);
				if(ptTemp.y < m_oGame.GetNumRows())
				{
					//DrawHexagon(hdc, m_ptGameField[ptTemp.x][ptTemp.y], cores[m_oGame.GetGhost()->GetPieceType()][1], 0x000000, m_iHexSize);
					DrawHexagon(hdc, m_ptGameField[ptTemp.x][ptTemp.y], m_oGame.GetGhost()->GetPieceType(), HEXTYPE_Ghost);
				}
			}
		}
	}
	return TRUE;
}


BOOL CDlgHextris::DrawGameBoard(CIssGDI &gdiDest, RECT rcClient)
{
	for(int i = 0; i < GAME_WIDTH; i++)
	{
		for(int j = 0; j < m_oGame.GetNumRows(); j++)
		{
			if(m_oGame.GetBoardHex(i,j)->ePiece == HEX_GamePiece)
			//	DrawHexagon(hDC, m_ptGameField[i][j], OUTLINE_COLOR, RGB(200,222,255), m_iHexSize);
				//DrawHexagon(hDC, m_ptGameField[i][j], m_oGame.GetBoardHex(i,j)->iColorIndex, HEXTYPE_Normal);
				DrawHexagon(gdiDest, m_ptGameField[i][j], m_oGame.GetBoardHex(i,j)->iColorIndex, HEXTYPE_Normal);
		}
	}
	return TRUE;
}

BOOL CDlgHextris::DrawOrbs(CIssGDI &gdiDest, RECT rcClient)
{
	for(int i = 0; i < GAME_WIDTH; i++)
	{
		for(int j = 0; j < m_oGame.GetNumRows(); j++)
		{
			if(m_oGame.GetBoardHex(i,j)->ePiece == HEX_BonusOrb)
			{
			//	DrawHexagon(gdiDest, m_ptGameField[i][j], m_oGame.GetBoardHex(i,j)->iColorIndex, HEXTYPE_Normal);
				DrawOrb(gdiDest, m_ptGameField[i][j], i, j);
			}
		}
	}
	return TRUE;
}

BOOL CDlgHextris::OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(LOWORD(wParam) == WA_ACTIVE) 
	{ 
		SHFullScreen(m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON); 
		RECT rc; 
		SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); 
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE); 

		if(m_bShowSplashScreen == TRUE)
		{
			m_bShowSplashScreen = FALSE;			

			// put up the splash screen
			ShowSplashScreen();		
		}
		else if(m_oGame.IsGameInPlay())
		{
			ResetFPS();
			Resume();
		}

		m_oKeys.RegisterHotKeys(m_hWnd, m_hInst, KEY_Soft1|KEY_Soft2);
		
		return TRUE; 
	}
	else if(LOWORD(wParam) == WA_INACTIVE)
	{
		if(m_oGame.IsGameInPlay())
			Pause();
	}
	return UNHANDLED;
}

BOOL CDlgHextris::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
	{
		m_gdiMem.Create(hDC, rcClient, FALSE, TRUE );
	}
	
	DrawBackground(m_gdiMem.GetDC(), rcClient);	
	DrawText(m_gdiMem, rcClient, FALSE);//also includes the hi score star 

	DrawGhost(m_gdiMem.GetDC(), rcClient);
	DrawCurrentPiece(m_gdiMem, rcClient);
	DrawOrbs(m_gdiMem, rcClient);
	DrawLevelProgress(m_gdiMem, rcClient);
	DrawFloatingMsg(m_gdiMem, rcClient);

	//draw saved orbs
	for(int i = 0; i < m_oGame.GetSavedGems(); i++)
	{
		CIssGDI::MyAlphaImage(m_gdiMem,	
			m_rcOrbs[i].left, m_rcOrbs[i].top, WIDTH(m_rcOrbs[i]), HEIGHT(m_rcOrbs[i]), m_gdiImages, m_gdiImagesAlpha, 
			((int)II_Orb)*WIDTH(m_rcOrbs[i]), 0);		
	}

	m_dwTickCount = GetTickCount();
	m_dwFrameCount++;

	BitBlt(hDC, rcClient.left, 
		rcClient.top, 
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		m_gdiMem.GetDC(), 
		rcClient.left, rcClient.top,
		SRCCOPY);

	return FALSE;
}

BOOL CDlgHextris::DrawText(CIssGDI &gdiDest, RECT rcClient, BOOL bDrawStatic)
{
	static TCHAR szTemp[STRING_NORMAL];

	if(bDrawStatic)
	{	//text for the background
		m_oStr->Format(szTemp, _T("LEVEL"));
		m_imgFont.DrawText(gdiDest, szTemp, m_rcLevel, DT_CENTER | DT_TOP);

		m_oStr->Format(szTemp, _T("BONUS"));
		m_imgFont.DrawText(gdiDest, szTemp, m_rcBonus, DT_CENTER | DT_TOP);

		//NEXT
		m_imgFont.DrawText(gdiDest, _T("N"), m_rcN, DT_LEFT | DT_TOP);
		m_imgFont.DrawText(gdiDest, _T("E"), m_rcE, DT_LEFT | DT_TOP);
		m_imgFont.DrawText(gdiDest, _T("X"), m_rcX, DT_LEFT | DT_TOP);
		m_imgFont.DrawText(gdiDest, _T("T"), m_rcT, DT_LEFT | DT_TOP);

		TCHAR szText[STRING_MAX];
		m_oStr->StringCopy(szText, IDS_MENU_Menu, STRING_MAX, m_hInst);
		CIssRect rcZero(m_rcMenu);
		rcZero.Translate(1,1);
		CIssGDI::DrawText(gdiDest.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(0,0,0));
		rcZero.Translate(-1,-1);
		CIssGDI::DrawText(gdiDest.GetDC(), szText, rcZero.Get(), DT_CENTER|DT_VCENTER, m_hFontText, RGB(255,255,255));
		//m_imgFont.DrawText(gdiDest, _T("MENU"), m_rcMenu, DT_CENTER|DT_VCENTER);
	}
	else
	{
		RECT rcTempScore = m_rcScore;

		if(m_oGame.IsHighScore())
		{	//draw some sexy star action to the left of the score

			RECT rcTemp = rcTempScore;
						
			rcTempScore.right -= m_gdiImages.GetWidth()/II_Count;

			rcTemp.bottom = rcTemp.top + m_gdiImages.GetHeight();
			rcTemp.left = rcTempScore.right;
		
			CIssGDI::MyAlphaImage(gdiDest,	
				rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), m_gdiImages, m_gdiImagesAlpha, 
				((int)II_Star)*WIDTH(rcTemp), 0);
		}

		m_oStr->Format(szTemp, _T("%i"), m_oGame.GetScore());
		m_imgFont.DrawText(gdiDest, szTemp, rcTempScore, DT_RIGHT | DT_TOP);

		m_oStr->Format(szTemp, _T("%i"), m_oGame.GetLevel());
		m_imgFont.DrawText(gdiDest, szTemp, m_rcLevel2, DT_CENTER | DT_TOP);

		m_oStr->Format(szTemp, _T("%.2i:%.2i"), m_oGame.GetTime()/60, m_oGame.GetTime()%60);
		m_imgFont.DrawText(gdiDest, szTemp, m_rcScore, DT_LEFT | DT_TOP);

		if(m_oGame.GetShowFPS())
		{
			DWORD dwNow = GetTickCount();
			m_oStr->Format(szTemp, _T("FPS: %i"), 1000*m_dwFrameCount/(dwNow-m_dwStartTime));
			m_imgFont.DrawText(gdiDest, szTemp, m_rcFPS, DT_CENTER| DT_TOP);
		}

	}

	return TRUE;
}


BOOL CDlgHextris::OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(m_oGame.GetGameState() == GS_GameOver)
		return TRUE;

	switch(wParam)
	{
	case IDT_LEVEL_UP:
		KillTimer(m_hWnd, IDT_LEVEL_UP);
		PlaySounds(EFFECT_LevelUp);
		break;
	case IDT_TIMER_HISCORE:
		KillTimer(m_hWnd, IDT_TIMER_HISCORE);
		PlaySounds(EFFECT_NewHiScore);
		break;
	case IDT_IDLE_TIMER:
		m_oGame.OnTimer();
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case IDT_CLOCK_TIMER:
		m_oGame.OnClock();
		break;
	case IDT_ANI_LEVEL:
		KillTimer(m_hWnd, IDT_ANI_LEVEL);
		AnimateLevelChangeChallenge();
		SetTimer(m_hWnd, IDT_IDLE_TIMER, m_oGame.GetLevelTiming(), NULL);
	    break;
	case IDT_DRAW_TIMER:
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	default:
	    break;
	}
	return FALSE;
}
BOOL CDlgHextris::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return CIssWnd::OnDestroy(hWnd, wParam, lParam);
}

BOOL CDlgHextris::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
#ifdef DEBUG
	case _T('b'):
	case _T('B'):
		AnimateFourOrbs();
		break;
	case _T('e'):
	case _T('E'):
		AnimateEndGame();
		break;
	case _T('h'):
	case _T('H'):
		HackAnimation();
		break;
#endif
	case _T('n'):
	case _T('N'):
		NewGame();
		break;
	case _T('s'):
	case _T('S'):
		m_oGame.OnSwitchPiece();
		break;
	case _T('d'):
	case _T('D'):
		m_oGame.OnDrop();
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case _T('q'):
	case _T('Q'):
		PostQuitMessage(0);
		break; 
	case _T('m'):
	case _T('M'):
		OnMenuRight();
		break;
	default:
		return FALSE;
	    break;
	}

	return TRUE;

}


BOOL CDlgHextris::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDMENU_Resume:
		ResetFPS();
		Resume();
		break;
	case IDMENU_NewGame:
	{
		NewGame();
		break;
	}
	case IDMENU_Help:
#ifdef WIN32_PLATFORM_WFSP
        LaunchHelp(_T("HexeGems.htm"), m_hInst);
#else
        CreateProcess(_T("peghelp"), _T("HexeGems.htm"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
#endif
		break;
	case IDMENU_Options:
	{
		CDlgOptions dlgOptions;
		dlgOptions.Init(&m_oSoundFX, &m_oGame, &m_oKeys);
		dlgOptions.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
		if(m_oGame.IsGameInPlay())
		{
			ResetFPS();
			Resume();
			break;
		}
		else
			NewGame();
		break;
	}
	case IDMENU_HiScores:
	{
		CDlgHighScores dlgHighScores;
		dlgHighScores.Init(&m_oGame, &m_oKeys);
		dlgHighScores.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
		ResetFPS();
		Resume();
		break;
	}
	case IDMENU_About:
	{
		CDlgAbout dlgAbout;
		dlgAbout.Init(&m_oKeys);
		dlgAbout.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic);
		ResetFPS();
		Resume();
		break;
	}
	case IDMENU_Exit:
	{	
		CDlgExit dlgExit;
		dlgExit.Init(&m_oKeys);
		if(IDYES == dlgExit.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
		{
			m_oSoundFX.Destroy();
			PostQuitMessage(0);
		}
		break;
	}
	case IDMENU_Restart:
		if(IDYES == MessageBox(m_hWnd, _T("Are you sure you want to start a new game"), TXT_Name, MB_YESNO))
			NewGame();
		break;
	case IDMENU_GameClassic:
		if(IDYES == MessageBox(m_hWnd, _T("Are you sure you want to start a new game?"), TXT_Name, MB_YESNO))
		{
			m_oGame.SetGameType(GT_Classic);
			NewGame();
		}
		break;
	case IDMENU_GameChallenge:
		if(IDYES == MessageBox(m_hWnd, _T("Are you sure you want to start a new game?"), TXT_Name, MB_YESNO))
		{
			m_oGame.SetGameType(GT_Challenge);
			NewGame();
		}
		break;
	case IDMENU_GameUltra:	
		if(IDYES == MessageBox(m_hWnd, _T("Are you sure you want to start a new game"), TXT_Name, MB_YESNO))
		{
			m_oGame.SetGameType(GT_Ultra);
			NewGame();
		}
		break;
	case IDMENU_Game40:	
	/*	if(IDYES == MessageBox(m_hWnd, _T("Are you sure you want to start a new game"), TXT_Name, MB_YESNO))
		{
			m_oGame.SetGameType(GT_40);
			NewGame();
		}*/
		break;
	case IDMENU_Pause:
		Pause();
		break;
	case IDMENU_Menu:
		OnMenuLeft();
		break;
	case IDMENU_Game:
		OnMenuRight();
		break;
	}
	return TRUE;
}

void CDlgHextris::Pause(BOOL bPauseSound)
{
	KillTimer(m_hWnd, IDT_CLOCK_TIMER);
	KillTimer(m_hWnd, IDT_IDLE_TIMER);
	KillTimer(m_hWnd, IDT_DRAW_TIMER);

	if(bPauseSound)
		m_oSoundFX.PauseMOD();
}

void CDlgHextris::Resume()
{
	if(m_oGame.GetGameState() == GS_Ingame)
	{
		SetTimer(m_hWnd, IDT_CLOCK_TIMER, 1000, NULL);
		SetTimer(m_hWnd, IDT_IDLE_TIMER, m_oGame.GetLevelTiming(), NULL);
		//game loop baby
		if(m_oGame.GetPowerSaving() == FALSE)	
			SetTimer(m_hWnd, IDT_DRAW_TIMER, DRAW_TIMER_DELAY, NULL);

		if(m_oSoundFX.GetVolumeMOD() > 0)
			m_oSoundFX.ResumeMOD();
	}
}


void CDlgHextris::PlaySounds(EnumSoundEffects eSound)
{
	m_oSoundFX.PlaySFX((int)eSound);
}

BOOL CDlgHextris::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uiMessage)
	{
	case WM_LINE_CLEARED:
		if(wParam > 1)
			PlaySounds(EFFECT_LineClearMulti);
		else
			PlaySounds(EFFECT_LineClear);
		AnimateLineCleared();
		InvalidateRect(m_hWnd, NULL, FALSE);
		m_bIsBackgroundDirty = TRUE;
		StartAddScore(m_oGame.GetLastScoreInc());
		break;
	case WM_LEVEL_CHANGE:
		SetTimer(m_hWnd, IDT_LEVEL_UP, 300, NULL);
		m_bIsBackgroundDirty = TRUE;
		StartAddMessage(_T("LEVEL UP"));
		break;
	case WM_LINE_TOBE_CLEARED:
		break;//might be needed ... we'll see
	case WM_TIME_UP:
	{
		InvalidateRect(m_hWnd, NULL, FALSE);
		AnimateEndGame();
		PlaySounds(EFFECT_GameOver);
		/*if(IDYES ==MessageBox(m_hWnd, _T("Time is up!"), _T("Play Again?"), MB_YESNO))
			NewGame();
		else
			PostQuitMessage(0);*/
		CDlgGameOver dlgGameOver;
		dlgGameOver.Init(&m_imgFont, m_oGame.GetScore(), m_oGame.GetHighScoreTable(), &m_oKeys);
		if(IDYES == dlgGameOver.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
			NewGame();
		else
			ShowMainMenu();
		break;
	}
	case WM_NO_MOVES:
	{
		Pause();
		AnimateEndGame();
		PlaySounds(EFFECT_GameOver);
		CDlgGameOver dlgGameOver;
		dlgGameOver.Init(&m_imgFont, m_oGame.GetScore(), m_oGame.GetHighScoreTable(), &m_oKeys);
		if(IDYES == dlgGameOver.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
			NewGame();
		else
			ShowMainMenu();
		break;
	}
	case WM_40LINES_CLEARED:
	{
		PlaySounds(EFFECT_LevelUp);
		/*if(IDYES ==MessageBox(m_hWnd, _T("40 Lines Cleared"), _T("Play Again?"), MB_YESNO))
			NewGame();
		else
			PostQuitMessage(0);*/
		CDlgGameOver dlgGameOver;
		dlgGameOver.Init(&m_imgFont, m_oGame.GetScore(), m_oGame.GetHighScoreTable(), &m_oKeys);
		if(IDYES == dlgGameOver.DoModal(m_hWnd, m_hInst, IDD_DLG_Basic))
			NewGame();
		else
			ShowMainMenu();
		break;
	}
	case WM_PIECE_PLACED:
		PlaySounds(EFFECT_Down);
		m_bIsBackgroundDirty = TRUE;
		break;
	case WM_NEW_HISCORE:
		//PlaySounds(_T("IDWAV_NewHighScore"));
		SetTimer(m_hWnd, IDT_TIMER_HISCORE, 1000, NULL);
		StartAddMessage(_T("HISCORE"));
		break;
	case WM_PIECE_SWITCHED:
		PlaySounds(EFFECT_Swtich);
		m_bIsBackgroundDirty = TRUE;
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case WM_BONUS_SAVED:
		StartAddMessage(_T("BONUS"));
		break;
	case WM_ALL_ORBS:
		PlaySounds(EFFECT_LineClearMulti);
		AnimateFourOrbs();
		m_bIsBackgroundDirty = TRUE;
		m_bDrawFloat = FALSE;//this takes priority
		StartAddScore(FOUR_ORB_BONUS);
		break;
	case WM_DOUBLE_ORB://good enough for now
		PlaySounds(EFFECT_LineClearMulti);
		//AnimateLineCleared();
		AnimateDoubleOrbs();
		m_bIsBackgroundDirty = TRUE;
		StartAddScore(m_oGame.GetLastScoreInc());
		break;
	default:
		return UNHANDLED;
	    break;
	}

	return TRUE;


	return UNHANDLED;

}


BOOL CDlgHextris::OnMenuLeft()
{
/*	//Create the menu.
	HMENU hMenu			= CreatePopupMenu();	
	if(!hMenu)
		return FALSE;

	TCHAR szText[STRING_MAX];

	//show title here
	m_oStr->StringCopy(szText, _T("Show Title Bar"));
	AppendMenu(hMenu, MF_STRING|(m_sOptions.bShowTitles?MF_CHECKED:NULL), IDMENU_ShowTitle, szText);
	AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);

	switch(m_eView)
	{
	case VIEW_Processes:
	{
		int iIndex = m_wndProcesses.GetItemIndex();
		int iCount = m_wndProcesses.GetItemCount();
		m_oStr->StringCopy(szText, IDS_MENU_EndProcess, STRING_MAX, m_hInst);
		AppendMenu(hMenu, MF_STRING|(iIndex==-1?MF_GRAYED:NULL), IDMENU_EndProcess, szText);
		AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
		break;
	}
	case VIEW_Performance:
		break;
	case VIEW_Tasks:
	{
		int iIndex = m_wndTasks.GetItemIndex();
		int iCount = m_wndTasks.GetItemCount();
		m_oStr->StringCopy(szText, IDS_MENU_SwitchTo, STRING_MAX, m_hInst);
		AppendMenu(hMenu, MF_STRING|(iIndex==-1?MF_GRAYED:NULL), IDMENU_SwitchTo, szText);
		m_oStr->StringCopy(szText, IDS_MENU_EndTask, STRING_MAX, m_hInst);
		AppendMenu(hMenu, MF_STRING|(iIndex==-1?MF_GRAYED:NULL), IDMENU_EndTask, szText);
		m_oStr->StringCopy(szText, IDS_MENU_CloseAll, STRING_MAX, m_hInst);
		AppendMenu(hMenu, MF_STRING|(iCount==0?MF_GRAYED:NULL), IDMENU_CloseAll, szText);
		AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
	    break;
	}
	case VIEW_Info:
	    break;
	}
//	m_oStr->StringCopy(szText, IDS_MENU_Help, STRING_MAX, m_hInst);
//	AppendMenu(hMenu, MF_STRING, IDMENU_Help, szText);
	m_oStr->StringCopy(szText, IDS_MENU_About, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_About, szText);
	m_oStr->StringCopy(szText, IDS_MENU_Exit, STRING_MAX, m_hInst);
	AppendMenu(hMenu, MF_STRING, IDMENU_Exit, szText);

	POINT pt;
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	pt.x	= rc.left + GetSystemMetrics(SM_CXVSCROLL);
	pt.y	= GetSystemMetrics(SM_CYSCREEN) - rc.top;


	//Display it.
	TrackPopupMenu(	hMenu, 
		TPM_LEFTALIGN|TPM_BOTTOMALIGN, 
		pt.x,pt.y, 
		0, 
		m_hWnd, NULL);

	DestroyMenu(hMenu);*/
	if(IDYES == MessageBox(m_hWnd, _T("Are you sure you want to quit?"), TXT_Name, MB_YESNO))
		PostQuitMessage(0);
	return TRUE;
}

BOOL CDlgHextris::OnMenuRight()
{
	//Create the menu.
	HMENU hMenu			= CreatePopupMenu();

	if(!hMenu)
		return FALSE;

	// pause the game
	Pause();

	m_oMenu.Initialize(m_hWnd, m_hInst, RGB(255,255,255), RGB(0,100,200), TRUE, IDR_BEVEL);

	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Restart, IDS_MENU_Restart);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_GameClassic, IDS_MENU_GameClassic);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_GameChallenge, IDS_MENU_GameChallenge);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Game40, IDS_MENU_Game40);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_GameUltra, IDS_MENU_GameTimed);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_Pause, IDS_MENU_Pause);
	m_oMenu.AppendMenu(hMenu, MF_STRING, IDMENU_HiScores, IDS_MENU_HiScores);
	


	POINT pt;
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	pt.x	= rc.right - GetSystemMetrics(SM_CXVSCROLL);
	pt.y	= GetSystemMetrics(SM_CYSCREEN) - rc.top;


	//Display it.
	m_oMenu.PopupMenu(	hMenu, 
		TPM_RIGHTALIGN|TPM_BOTTOMALIGN, 
		pt.x,pt.y);

	DestroyMenu(hMenu);
	return TRUE;
}

BOOL CDlgHextris::OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	
	return TRUE;
}

BOOL CDlgHextris::OnGetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	return TRUE;
}

BOOL CDlgHextris::OnLButtonUp(HWND hWnd, POINT& pt)
{
	if(PtInRect(&m_rcMenu, pt))
	{
		ShowMenu();
	}	
	return TRUE;
}

BOOL CDlgHextris::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(PtInRect(&m_rcMenu, pt))
	{}
	else if(PtInRect(&m_rcSaved, pt))
		m_oGame.OnSwitchPiece();
	else if(pt.x < GetSystemMetrics(SM_CXICON))
		m_oGame.OnLeft();
	/*else if(pt.x > 11*m_iHexSize)
		m_oGame.OnSwitchPiece();*/
	else if(pt.x > 9*m_iHexSize)
		m_oGame.OnRight();
	else
		m_oGame.OnRotate();

	return TRUE;
}


BOOL CDlgHextris::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_LEFT:
		m_oGame.OnLeft();
		break;
	case VK_RIGHT:
		m_oGame.OnRight();
		break;
	case VK_DOWN:
		m_oGame.OnDown();
	    break;
	case VK_UP:
		//m_oGame.OnUp();
		if(m_oGame.OnRotate())
			PlaySounds(EFFECT_Move);
		break;
	case VK_RETURN:
		m_oGame.OnDrop();
		break;
	default:
		return FALSE;
	    break;
	}

	KillTimer(m_hWnd, IDT_IDLE_TIMER);
	SetTimer(m_hWnd, IDT_IDLE_TIMER, m_oGame.GetLevelTiming(), NULL);

	InvalidateRect(m_hWnd, NULL, FALSE);
	return TRUE;
}

BOOL CDlgHextris::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case VK_TSOFT1:
        if(m_oGame.IsGameInPlay())
            m_oGame.OnSwitchPiece();
        break;
    case VK_TSOFT2:
        ShowMenu();
        break;
    case VK_ESCAPE:
        PostQuitMessage(0);
        break;
    default:
        return UNHANDLED;
        break;
    }

    return TRUE;
}

BOOL CDlgHextris::CreateGDIHex(HDC hdc, CIssGDI& gdiHex, int iWidth, int iHeight, int iHexSize, UINT res, CIssGDI* gdiGiven)
{
	//11 colors for now
	gdiHex.Create(hdc, NUM_HEX_COLORS*iWidth, iHeight, FALSE, TRUE, TRUE);
	//							Grad Top	/	Grad Bottom		/	Grad Outline		

	CIssGDI gdiTemp;
	CIssGDI gdiTempHex;
	CIssGDI gdiTempImage;
	POINT	pt;
	pt.x = iHexSize/2;
	pt.y = 0;
	RECT rcTemp = {0, 0, iWidth, iHeight};

	gdiTempHex.Create(hdc, rcTemp, FALSE, TRUE, TRUE);

	rcTemp.right *= NUM_HEX_COLORS;
	gdiTemp.Create(hdc, rcTemp, FALSE, TRUE);
	rcTemp.right /= NUM_HEX_COLORS;//back to a single

	if(gdiGiven && gdiGiven->GetWidth() && gdiGiven->GetHeight())
	{
		gdiTempImage.Create(gdiGiven->GetDC(), gdiGiven->GetWidth(), gdiGiven->GetHeight(), TRUE, TRUE, FALSE);
	}
	else
	{
		if(!LoadImage(gdiTempImage, res, m_hWnd, m_hInst))
			ASSERT(0);
	}
	

	SIZE sizeTemp = {iWidth, iHeight};
	sizeTemp.cx *= NUM_HEX_COLORS;

	// scale image to proper board size
	CIssGDIEffects::ScaleImage(gdiTempImage, gdiTemp, sizeTemp, FALSE);

	COLORREF crTempColor;
	COLORREF crOutline;

	for(int i = 0 ; i < NUM_HEX_COLORS; i++)
	{
		crOutline = gdiTemp.GetPixelColor(i*iWidth, 0);
		CIssGDI::FillRect(gdiTempHex.GetDC(), rcTemp, TRANSPARENT_COLOR);
		DrawHexagon(gdiTempHex.GetDC(), pt, 0, 0, iHexSize);

		// loop through all the bits and change it to the proper color
		for (int y=0; y < HEIGHT(rcTemp); y++) 
		{
			for (int x=0; x < iWidth; x++) 
			{
				crTempColor = gdiTempHex.GetPixelColor(x, y);
				if(crTempColor == 0)
				{
					gdiTempHex.SetPixelColor(x, y, gdiTemp.GetPixelColor(x+i*iWidth,y));
				}
			}
		}		

		//draw a new hex outline
		//DrawHexagon(gdiTempHex.GetDC(), pt, crOutline, 0xFF00FF, iHexSize);

		BitBlt(gdiHex.GetDC(), 
			i*iWidth, 0, 
			iWidth,iHeight, 
			gdiTempHex.GetDC(), 
			0, 0, SRCCOPY);


	}
	return TRUE;
}

BOOL CDlgHextris::DrawHexagon(HDC hdc, POINT ptLocation, int iColorIndex, EnumHexType eHexType)
{
	// make sure everything is loaded
	ReloadHexagons();

	switch(eHexType)
	{
	case HEXTYPE_Ghost:
		return	TransparentBlt(hdc, ptLocation.x - m_iHexSize/2, ptLocation.y, 
			m_sizeHex.cx, m_sizeHex.cy,
			m_gdiHexGhost.GetDC(), iColorIndex*m_sizeHex.cx, 0, m_sizeHex.cx, m_sizeHex.cy, TRANSPARENT_COLOR);
		break;
	case HEXTYPE_Small:
		return	TransparentBlt(hdc, ptLocation.x - m_iHexSizeSmall/2, ptLocation.y, 
			m_sizeHexSmall.cx, m_sizeHexSmall.cy,
			m_gdiHexSmall.GetDC(), iColorIndex*m_sizeHexSmall.cx, 0, m_sizeHexSmall.cx, m_sizeHexSmall.cy, TRANSPARENT_COLOR);
		break;
	case HEXTYPE_Med:
		return	TransparentBlt(hdc, ptLocation.x - m_iHexSizeMed/2, ptLocation.y, 
			m_sizeHexMed.cx, m_sizeHexMed.cy,
			m_gdiHexMed.GetDC(), iColorIndex*m_sizeHexMed.cx, 0, m_sizeHexMed.cx, m_sizeHexMed.cy, TRANSPARENT_COLOR);	
		break;
	case HEXTYPE_Normal:
	default:
		return	TransparentBlt(hdc, ptLocation.x - m_iHexSize/2, ptLocation.y, 
			m_sizeHex.cx, m_sizeHex.cy,
			m_gdiHex.GetDC(), iColorIndex*m_sizeHex.cx, 0, m_sizeHex.cx, m_sizeHex.cy, TRANSPARENT_COLOR);
	    break;
	}



}

void CDlgHextris::ReloadBackground(HWND hWndSplash, int iMaxPercent)
{
	if(m_gdiBackground.GetDC() == NULL)
	{
		int iCurPercent = 0;
		if(hWndSplash)
			iCurPercent = SendMessage(hWndSplash, WM_GET_Percent, 0, 0);

		int iStep = (iMaxPercent-iCurPercent)/7;	// 7 steps

		HDC hdc = GetDC(m_hWnd);
		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);
		m_gdiBackground.Create(hdc, rcClient);
		ReleaseDC(m_hWnd, hdc);

		//CIssGDI::GradriantFillRect(m_gdiBackground.GetDC(), rcClient, GetBackgroundColor(BGCOLOR_Grad1), GetBackgroundColor(BGCOLOR_Grad2));
		CIssGDI::FillRect(m_gdiBackground.GetDC(), rcClient, 0);

		iCurPercent += iStep;
		if(hWndSplash)
			SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

		//create the game field starting from the bottom
		//create space for our level indicator
		int j = rcClient.bottom - BOTTOM_INDENT - 3*m_iHexSizeCos30;//a little padding

		int iRow = 0;
		while(j > rcClient.top)
		{
			for(int i = 0; i < GAME_WIDTH/2; i++)
			{
				m_ptGameField[i*2][iRow].x = i*m_iHexSize*3 + m_iHexSize/2;
				m_ptGameField[i*2][iRow].y = j;

				DrawHexagon(m_gdiBackground.GetDC(), m_ptGameField[i*2][iRow], GetBackgroundColor(BGCOLOR_Grid), GetBackgroundColor(BGCOLOR_Hex), m_iHexSize, TRUE);


				m_ptGameField[i*2+1][iRow].x = i*m_iHexSize*3 + 2*m_iHexSize;
				m_ptGameField[i*2+1][iRow].y = m_iHexSizeCos30 + m_ptGameField[i*2][iRow].y;

				DrawHexagon(m_gdiBackground.GetDC(), m_ptGameField[i*2+1][iRow], GetBackgroundColor(BGCOLOR_Grid), GetBackgroundColor(BGCOLOR_Hex), m_iHexSize, TRUE);
			}

			j -= 2*m_iHexSizeCos30;
			iRow++;
		}

		//draw level indicator using gradients
		RECT rcIndicator = rcClient;
		rcIndicator.right = m_ptGameField[GAME_WIDTH-1][0].x + 3*m_iHexSize/2;
		rcIndicator.top = rcIndicator.bottom - BOTTOM_INDENT + 2;
		rcIndicator.left += 2;

		RECT rcTemp = rcIndicator;
		rcTemp.bottom = rcTemp.top + HEIGHT(rcIndicator)/2;
		CIssGDI::GradriantFillRect(m_gdiBackground.GetDC(), rcTemp, 0, 0xBBBBBB);
		rcTemp.top = rcTemp.bottom;
		rcTemp.bottom = rcIndicator.bottom;
		CIssGDI::GradriantFillRect(m_gdiBackground.GetDC(), rcTemp, 0xBBBBBB, 0);

		rcTemp = rcIndicator;
		int iIndent = HEIGHT(rcIndicator)/4;

		rcTemp.left		+= (iIndent - 1);
		rcTemp.right	-= (iIndent - 1);
		rcTemp.top		+= (iIndent);
		rcTemp.bottom	-= (iIndent);

		CIssGDI::FillRect(m_gdiBackground.GetDC(), rcTemp, 0x303030);

		rcTemp.left += 1;
		rcTemp.right -= 1;
		rcTemp.top += 1;
		rcTemp.bottom -= 1;

		CIssGDI::FillRect(m_gdiBackground.GetDC(), rcTemp, 0);

		// increment the percent
		iCurPercent += iStep;
		if(hWndSplash)
			SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

		m_rcLevelProg = rcTemp;//set the rect

		CIssBeveledEdge		oBevel;

		//next piece background
		oBevel.Initialize(m_hWnd, m_hInst, IDR_GS1_TL, IDR_GS1_T, IDR_GS1_TR, IDR_GS1_R, IDR_GS1_BR, IDR_GS1_B, IDR_GS1_BL, IDR_GS1_L, IDR_GS_M);
		oBevel.SetLocation(m_rcPreview1);
		oBevel.Draw(m_gdiBackground.GetDC());

		// increment the percent
		iCurPercent += iStep;
		if(hWndSplash)
			SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

		//saved piece background
		oBevel.Initialize(m_hWnd, m_hInst, IDR_GS2_TL, IDR_GS2_T, IDR_GS2_TR, IDR_GS2_R, IDR_GS2_BR, IDR_GS2_B, IDR_GS2_BL, IDR_GS2_L, IDR_GS_M);
		oBevel.SetLocation(m_rcSaved);
		oBevel.Draw(m_gdiBackground.GetDC());

		// increment the percent
		iCurPercent += iStep;
		if(hWndSplash)
			SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

		//lower right background
		oBevel.Initialize(m_hWnd, m_hInst, IDR_GS3_TL, IDR_GS3_T, IDR_GS3_TR, IDR_GS3_R, IDR_GS3_BR, IDR_GS3_B, IDR_GS3_BL, IDR_GS3_L, IDR_GS3_M);
		oBevel.SetLocation(m_rcGS3);
		oBevel.Draw(m_gdiBackground.GetDC());

		// increment the percent
		iCurPercent += iStep;
		if(hWndSplash)
			SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

		// Menu Button
		oBevel.Initialize(m_hWnd, m_hInst, 
							IDR_MN_TL, IDR_MN_T, IDR_MN_TR, IDR_MN_R, IDR_MN_BR, IDR_MN_B, IDR_MN_BL, IDR_MN_L, IDR_MN_M);
		oBevel.InitializeAlpha(IDR_MNA_TL, IDR_MNA_T, IDR_MNA_TR, IDR_MNA_R, IDR_MNA_BR, IDR_MNA_B, IDR_MNA_BL, IDR_MNA_L, IDR_MNA_M);
		oBevel.SetLocation(m_rcMenu);
		oBevel.Draw(m_gdiBackground);

		// increment the percent
		iCurPercent += iStep;
		if(hWndSplash)
			SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

		//Draw the menu button
		/*rcTemp = m_rcMenu;
		rcTemp.top += HEIGHT(m_rcMenu)/2;
		CIssGDI::FillRect(m_gdiBackground.GetDC(), rcTemp, 0);
		rcTemp = m_rcMenu;
		rcTemp.bottom -= HEIGHT(m_rcMenu)/2;
		CIssGDI::GradriantFillRect(m_gdiBackground.GetDC(), rcTemp, 0x424242, 0x161616);*/

		//draw the top line


		//draw the text that doesn't change
		DrawText(m_gdiBackground, rcClient, TRUE);

		//since this is the first place these are used
		//icon images
		int iFrames = 0;

		if(m_gdiImages.GetDC() == NULL)
			LoadImgArray(m_gdiImages, IDR_PNG_Items, iFrames, ITEMS_SIZE, ITEMS_SIZE);
		if(m_gdiImagesAlpha.GetDC() == NULL)
			LoadImgArrayAlpha(m_gdiImagesAlpha, IDR_PNG_ItemsAlpha, iFrames, ITEMS_SIZE, ITEMS_SIZE);

		//draw switch
		rcTemp.left		= m_rcSaved.left + GetSystemMetrics(SM_CXSMICON)/4;//try this for now
		rcTemp.right	= rcTemp.left + m_gdiImages.GetWidth()/((int)II_Count);
		rcTemp.top		= m_rcSaved.top + HEIGHT(m_rcSaved)/2 - m_gdiImages.GetHeight()/2;
		rcTemp.bottom	= rcTemp.top + m_gdiImages.GetHeight();

		CIssGDI::MyAlphaImage(m_gdiBackground,	
			rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), m_gdiImages, m_gdiImagesAlpha, 
			((int)II_Switch)*WIDTH(rcTemp), 0);

		//draw blank orbs
		for(int i = 0; i < MAX_SAVED_GEMS; i++)
		{
			CIssGDI::MyAlphaImage(m_gdiBackground,	
				m_rcOrbs[i].left, m_rcOrbs[i].top, WIDTH(m_rcOrbs[i]), HEIGHT(m_rcOrbs[i]), m_gdiImages, m_gdiImagesAlpha, 
				((int)II_Blank)*WIDTH(m_rcOrbs[i]), 0);		
		}

		m_oGame.SetNumRows(iRow);
		
		ResetFPS();

		// increment the percent
		iCurPercent += iStep;
		if(hWndSplash)
			SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

	}

	if(m_gdiTempBack.GetDC() == NULL)
	{
		m_gdiTempBack.Create(m_gdiBackground.GetDC(), m_gdiBackground.GetWidth(), m_gdiBackground.GetHeight(), FALSE, TRUE);
		m_bIsBackgroundDirty = TRUE;
	}
}

void CDlgHextris::ResetFPS()
{

	m_dwStartTime		= GetTickCount();
	m_dwFrameCount		= 0;
}

void CDlgHextris::ReloadHexagons(HWND hWndSplash, int iMaxPercent)
{
	int iCurPercent = 0;
	if(hWndSplash)
		iCurPercent = SendMessage(hWndSplash, WM_GET_Percent, 0, 0);

	int iStep = (iMaxPercent-iCurPercent)/10;	// 10 steps

	if(m_gdiHex.GetDC() == NULL)
	{
		CIssGDI gdiTempImage;

		if(!LoadImage(gdiTempImage, IDR_PNG_Gems, m_hWnd, m_hInst))
			ASSERT(0);

		CreateGDIHex(m_gdiMem.GetDC(), m_gdiHex, m_sizeHex.cx, m_sizeHex.cy, m_iHexSize, IDR_PNG_Gems, &gdiTempImage);

		// increment the percent
		iCurPercent += iStep;
		if(hWndSplash)
			SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

		if(m_gdiHexMed.GetDC() == NULL)
			CreateGDIHex(m_gdiMem.GetDC(), m_gdiHexMed, m_sizeHexMed.cx, m_sizeHexMed.cy, m_iHexSizeMed, IDR_PNG_Gems, &gdiTempImage);

		// increment the percent
		iCurPercent += iStep;
		if(hWndSplash)
			SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

		if(m_gdiHexSmall.GetDC() == NULL)
			CreateGDIHex(m_gdiMem.GetDC(), m_gdiHexSmall, m_sizeHexSmall.cx, m_sizeHexSmall.cy, m_iHexSizeSmall, IDR_PNG_Gems, &gdiTempImage);

		// increment the percent
		iCurPercent += iStep;
		if(hWndSplash)
			SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);
	}

	if(m_gdiHexGhost.GetDC() == NULL)
		CreateGDIHex(m_gdiMem.GetDC(), m_gdiHexGhost, m_sizeHex.cx, m_sizeHex.cy, m_iHexSize, IDR_PNG_Ghost);
	// increment the percent
	iCurPercent += iStep;
	if(hWndSplash)
		SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);


	if(m_gdiAnimation.GetDC() == NULL)
		CreateGDIHex(m_gdiMem.GetDC(), m_gdiAnimation, m_sizeHex.cx, m_sizeHex.cy, m_iHexSize, IDR_PNG_ANI);

	// increment the percent
	iCurPercent += iStep;
	if(hWndSplash)
		SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

	

	if(!m_sprtOrb.IsLoaded())
	{
		m_sprtOrb.Initialize(IDR_PNG_Orb, IDR_PNG_OrbAlpha, m_hWnd, m_hInst, NUM_ORB_Frames, 5000, TRUE);
		m_sprtOrb.SetSize(2*m_sizeHex.cx, 2*m_sizeHex.cy);

	}
	// increment the percent
	iCurPercent += iStep;
	if(hWndSplash)
		SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);


	//explosion
	if(m_gdiExplosion.GetDC() == NULL)
		LoadImgArray(m_gdiExplosion, IDR_PNG_Explosion, m_iNumExplosionFrames, 2*m_sizeHex.cx, 2*m_sizeHex.cy);

	// increment the percent
	iCurPercent += iStep;
	if(hWndSplash)
		SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

	if(m_gdiExplosionAlpha.GetDC() == NULL)
		LoadImgArrayAlpha(m_gdiExplosionAlpha, IDR_PNG_ExpAlpha, m_iNumExplosionFrames, 2*m_sizeHex.cx, 2*m_sizeHex.cy);

	// increment the percent
	iCurPercent += iStep;
	if(hWndSplash)
		SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

	//orb fireball
	if(m_gdiFireball.GetDC() == NULL)
		LoadImgArray(m_gdiFireball, IDR_PNG_Fireball, m_iNumFireballFrames, 2*m_sizeHex.cx, 2*m_sizeHex.cy);

	// increment the percent
	iCurPercent += iStep;
	if(hWndSplash)
		SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);
	if(m_gdiFireballAlpha.GetDC() == NULL)
		LoadImgArrayAlpha(m_gdiFireballAlpha, IDR_PNG_FireballAlpha, m_iNumFireballFrames, 2*m_sizeHex.cx, 2*m_sizeHex.cy);

	//pulse (double explosion)
	int iFrames = 11;
	if(m_gdiPulse.GetDC() == NULL)
		LoadImgArray(m_gdiPulse, IDR_PNG_Pulse, m_iNumPulseFrames, 4*m_sizeHex.cx, 4*m_sizeHex.cy);
	if(m_gdiPulseAlpha.GetDC() == NULL)
		LoadImgArrayAlpha(m_gdiPulseAlpha, IDR_PNG_PulseAlpha, m_iNumPulseFrames, 4*m_sizeHex.cx, 4*m_sizeHex.cy);

	// increment the percent
	iCurPercent += iStep;
	if(hWndSplash)
		SendMessage(hWndSplash, WM_SET_Percent, iCurPercent, 0);

}

BOOL CDlgHextris::LoadImgArray(CIssGDI &oGDIDest, UINT res, int &iNumFrames, int iX, int iY)
{
	CIssGDI gdiTempImage;

	if(!LoadImage(gdiTempImage, res, m_hWnd, m_hInst))
		return FALSE;

	gdiTempImage.ConvertToDIB();

	iNumFrames	= gdiTempImage.GetWidth()/gdiTempImage.GetHeight();

	SIZE sizeFire = {iNumFrames*iX, iY};

	// scale image to proper board size
	CIssGDIEffects::ScaleImage(gdiTempImage, oGDIDest, sizeFire, FALSE);
	return oGDIDest.ConvertToDIB();
}

BOOL CDlgHextris::LoadImgArrayAlpha(CIssGDI &oGDIDest, UINT res, int iNumFrames, int iX, int iY)
{
	CIssGDI gdiTempImage;

	if(!LoadImage(gdiTempImage, res, m_hWnd, m_hInst))
		return FALSE;

	gdiTempImage.ConvertToDIB();

	SIZE sizeFire = {iNumFrames*iX, iY};

	// scale image to proper board size
	CIssGDIEffects::ScaleImage(gdiTempImage, oGDIDest, sizeFire, FALSE);
	return oGDIDest.ConvertToDIB();
}

BOOL CDlgHextris::DrawOrb(CIssGDI &gdiDest, POINT ptLocation, int x, int y)
{
	// make sure everything is loaded
	ReloadHexagons();

	m_sprtOrb.Draw(gdiDest, ptLocation.x - m_iHexSize*3/2, ptLocation.y - m_iHexSize, m_dwTickCount, (DWORD)((BYTE)y)|((BYTE)x<<16));
	return TRUE;
}



//for testing - we'll handle the bits when drawing
//all drawing done to m_gdiMem
BOOL CDlgHextris::DrawHexagon(CIssGDI &gdiDest, POINT ptLocation, int iColorIndex, EnumHexType eHexType)
{
	// make sure everything is loaded
	ReloadHexagons();

	switch(eHexType)
	{
	case HEXTYPE_Ghost:
		return	CIssGDI::MyTransparentImage(gdiDest, ptLocation.x - m_iHexSize/2, ptLocation.y, 
			m_sizeHex.cx, m_sizeHex.cy,
			m_gdiHexGhost, iColorIndex*m_sizeHex.cx, 0, m_sizeHex.cx, m_sizeHex.cy, TRANSPARENT_COLOR);
		break;
	case HEXTYPE_Small:
		return	CIssGDI::MyTransparentImage(gdiDest, ptLocation.x - m_iHexSizeSmall/2, ptLocation.y, 
			m_sizeHexSmall.cx, m_sizeHexSmall.cy,
			m_gdiHexSmall, iColorIndex*m_sizeHexSmall.cx, 0, m_sizeHexSmall.cx, m_sizeHexSmall.cy, TRANSPARENT_COLOR);
		break;
	case HEXTYPE_Med:
		return	CIssGDI::MyTransparentImage(gdiDest, ptLocation.x - m_iHexSizeMed/2, ptLocation.y, 
			m_sizeHexMed.cx, m_sizeHexMed.cy,
			m_gdiHexMed, iColorIndex*m_sizeHexMed.cx, 0, m_sizeHexMed.cx, m_sizeHexMed.cy, TRANSPARENT_COLOR);	
		break;
	case HEXTYPE_Normal:
	default:
		return	CIssGDI::MyTransparentImage(gdiDest, ptLocation.x - m_iHexSize/2, ptLocation.y, 
			m_sizeHex.cx, m_sizeHex.cy,
			m_gdiHex, iColorIndex*m_sizeHex.cx, 0, m_sizeHex.cx, m_sizeHex.cy, TRANSPARENT_COLOR);
		break;
	}

	return TRUE;
}




BOOL CDlgHextris::DrawHexagon(HDC hdc, POINT ptGridLocation, COLORREF rgbOutline, COLORREF rgbColor, int iSize, BOOL bDrawHiLite)
{
	HPEN hPen = CIssGDI::CreatePen(rgbOutline, PS_SOLID, 1);
	HBRUSH hBrush = CIssGDI::CreateBrush(rgbColor);

	POINT ptHex[6];

//0.866025 - square root of 3/4
//0.5 of course
//slay uses size 12 



	ptHex[0].x	= ptGridLocation.x;
	ptHex[0].y	= ptGridLocation.y;
	ptHex[1].x	= ptHex[0].x + iSize;
	ptHex[1].y	= ptHex[0].y;
	ptHex[2].x	= ptHex[1].x + iSize/2;
	ptHex[2].y	= ptHex[1].y + iSize*HEX_RATIO/1000;
	ptHex[3].x	= ptHex[1].x;
	ptHex[3].y	= ptHex[2].y + iSize*HEX_RATIO/1000;
	ptHex[4].x	= ptHex[0].x;
	ptHex[4].y	= ptHex[3].y;
	ptHex[5].x	= ptHex[0].x - iSize/2;
	ptHex[5].y	= ptHex[2].y;

	HPEN hOldPen;
	if(rgbOutline == 0xFF00FF)
		hOldPen = (HPEN)SelectObject(hdc, (HPEN)GetStockObject(NULL_PEN));
	else
		hOldPen = (HPEN)SelectObject(hdc, hPen);


	HBRUSH hOldBrush;
	
	if(rgbColor == 0xFF00FF)
		hOldBrush = (HBRUSH)SelectObject(hdc, (HBRUSH)GetStockObject(NULL_BRUSH));
	else
		hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	Polygon(hdc, ptHex, 6);

	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);

	DeleteObject(hPen);
	DeleteObject(hBrush);

	if(bDrawHiLite)
	{
		ptHex[0].x = ptHex[2].x - 1;
		ptHex[0].y = ptHex[2].y;
		ptHex[1].x = ptHex[3].x;
		ptHex[1].y = ptHex[3].y - 1;
		ptHex[2].x = ptHex[4].x;
		ptHex[2].y = ptHex[4].y - 1;

		hPen = CIssGDI::CreatePen(0x5C5C5C, PS_SOLID, 1);
		hOldPen = (HPEN)SelectObject(hdc, hPen);

		Polyline(hdc, ptHex, 3);

		SelectObject(hdc, hOldPen);

		DeleteObject(hPen);
	}

	return TRUE;
}

void CDlgHextris::NewGame()
{
	m_oGame.NewGame();
	Pause();
	Resume();
	if(m_oSoundFX.GetVolumeMOD() > 0)
		m_oSoundFX.PlayMOD();

	m_bIsBackgroundDirty = TRUE;
	InvalidateRect(m_hWnd, NULL, FALSE);
	ResetFPS();
}

void CDlgHextris::AnimateDoubleOrbs()
{
	Pause();

	int iTickCount;
	int iPrevCount;
	int iTemp;

	HDC dc = GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	BOOL bAniOrb = FALSE;

	//copy the current screen into our temp
	BitBlt(m_gdiTempBack, rcClient.left, rcClient.top, WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiMem, rcClient.left, rcClient.top, SRCCOPY);

	//draw the first frame onto the erased hex
	for(int x = 0; x < GAME_WIDTH; x++)
	{
		for(int y = 0; y < m_oGame.GetNumRows(); y++)
		{
			if(m_oGame.GetPreDropHex(x,y)->ePiece == HEX_Erased)
			{
				if(m_oGame.GetPrevHex(x,y)->ePiece == HEX_GamePiece || m_oGame.GetPrevHex(x,y)->ePiece == HEX_BonusOrb)
					DrawHexagon(m_gdiTempBack.GetDC(), m_ptGameField[x][y], GetBackgroundColor(BGCOLOR_Grid), GetBackgroundColor(BGCOLOR_Hex), m_iHexSize, TRUE);	
				if(m_oGame.GetPrevHex(x,y)->ePiece == HEX_BonusOrb)
					bAniOrb = TRUE; //need to correct the animation for the orb
			}
		}
	}

	POINT ptPulse;

	POINT pt1;
	POINT pt2;

	pt1 = m_ptGameField[m_oGame.GetDoubleExOrb1().x][m_oGame.GetDoubleExOrb1().y];
	pt2 = m_ptGameField[m_oGame.GetDoubleExOrb2().x][m_oGame.GetDoubleExOrb2().y];

	//I think this works
	ptPulse.x = abs((pt1.x - pt2.x)/2) + min(pt1.x, pt2.x) - m_sizeHex.cx*2;
	ptPulse.y = abs((pt1.y - pt2.y)/2) + min(pt1.y, pt2.y) - m_gdiPulse.GetHeight()/2;

	ResetAniArray();//clear out the animation timing array
	int iCurrentFrame = 0;

	int iTotalFrames;
	if(bAniOrb)
		iTotalFrames = m_iNumFireballFrames + SetAniArrayOrbLineClear();
	else
		iTotalFrames = m_iNumExplosionFrames + SetAniArrayLineClear();//for now ... we'll check for orbs later

	iPrevCount = GetTickCount();

	//play the explode effect
	PlaySounds(EFFECT_Explode);

	for(int i = 0; i < iTotalFrames; i++)
	{
		//redraws the "blank" background 
		BitBlt(m_gdiMem.GetDC(), rcClient.left, 
			rcClient.top, 
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			m_gdiTempBack.GetDC(), 
			rcClient.left, rcClient.top,
			SRCCOPY);

		for(int x = 0; x < GAME_WIDTH; x++)
		{
			for(int y = 0; y < m_oGame.GetNumRows(); y++)
			{
				if(m_iAniArray[x][y] == ANI_NO_RENDER)
					continue;

				//check if we should be animating ... well ... anything
				if(m_iAniArray[x][y] >= 0)
				{	//we need to render something
					if(m_oGame.GetPrevHex(x,y)->ePiece == HEX_GamePiece)
					{
						//no more animations yo
						if(m_iAniArray[x][y] >= m_iNumExplosionFrames)
						{
							m_iAniArray[x][y] = ANI_NO_RENDER;
							continue;
						}

						CIssGDI::MyAlphaImage(m_gdiMem,
							m_ptGameField[x][y].x + m_iHexSize/2 - m_sizeHex.cx,
							m_ptGameField[x][y].y - m_sizeHex.cy/2,
							m_sizeHex.cx*2, m_gdiExplosion.GetHeight(),
							m_gdiExplosion, 
							m_gdiExplosionAlpha,
							m_iAniArray[x][y]*m_sizeHex.cx*2, 0);
					}
					//check if we're past our animation and set it to ANI_NO_RENDER
				}
				else //not going to bother checking the orbs .. only the gem
				{
					if(m_oGame.GetPrevHex(x,y)->ePiece == HEX_GamePiece)
					{
						CIssGDI::MyAlphaImage(m_gdiMem,
							m_ptGameField[x][y].x + m_iHexSize/2 - m_sizeHex.cx,
							m_ptGameField[x][y].y - m_sizeHex.cy/2,
							m_sizeHex.cx*2, m_gdiExplosion.GetHeight(),
							m_gdiExplosion, 
							m_gdiExplosionAlpha,
							0, 0);
					}
				}
				//m_iAniArray[x][y]++;//move the animation index along for next pass
				m_iAniArray[x][y]++;//move the animation index along for next pass
			}
		}

		//render pulse
		if(i < m_iNumPulseFrames)
		{
			CIssGDI::MyAlphaImage(m_gdiMem,
				ptPulse.x,
				ptPulse.y,
				m_sizeHex.cx*4, m_gdiPulse.GetHeight(),
				m_gdiPulse, 
				m_gdiPulseAlpha,
				i*m_sizeHex.cx*4, 0);
		}

		BitBlt(dc, rcClient.left, 
			rcClient.top, 
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			m_gdiMem.GetDC(), 
			rcClient.left, rcClient.top,
			SRCCOPY);

		iTickCount = GetTickCount();

		iTemp = iPrevCount + ANI_WAIT_TIME - iTickCount;

		if(iTemp > 0)
			Sleep(iTemp);

		iPrevCount = iTickCount;

		m_dwFrameCount++;
	}
	ReleaseDC(m_hWnd, dc);

	Resume();



/*	POINT ptPulse;

	POINT pt1;
	POINT pt2;

	pt1 = m_ptGameField[m_oGame.GetDoubleExOrb1().x][m_oGame.GetDoubleExOrb1().y];
	pt2 = m_ptGameField[m_oGame.GetDoubleExOrb2().x][m_oGame.GetDoubleExOrb2().y];

	//I think this works
	ptPulse.x = abs((pt1.x - pt2.x)/2) + min(pt1.x, pt2.x) - m_sizeHex.cx*2;
	ptPulse.y = abs((pt1.y - pt2.y)/2) + min(pt1.y, pt2.y) - m_gdiPulse.GetHeight()/2;

	CIssGDI gdiBack;
	gdiBack.Create(m_gdiMem.GetDC(), rcClient, TRUE);

	for(int i = 0; i < m_iNumPulseFrames; i++)
	{
		BitBlt(m_gdiMem.GetDC(), rcClient.left, rcClient.top, WIDTH(rcClient), HEIGHT(rcClient), gdiBack.GetDC(), rcClient.left, rcClient.top, SRCCOPY);

		CIssGDI::MyAlphaImage(m_gdiMem,
			ptPulse.x,
			ptPulse.y,
			m_sizeHex.cx*4, m_gdiPulse.GetHeight(),
			m_gdiPulse, 
			m_gdiPulseAlpha,
			i*m_sizeHex.cx*4, 0);		

		BitBlt(dc, rcClient.left, rcClient.top, WIDTH(rcClient), HEIGHT(rcClient), m_gdiMem.GetDC(), rcClient.left, rcClient.top, SRCCOPY);

		Sleep(50);
	}*/

}


void CDlgHextris::AnimateLineCleared()
{
	Pause(FALSE);

	int iTickCount;
	int iPrevCount;
	int iTemp;

	HDC dc = GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	BOOL bAniOrb = FALSE;

	//copy the current screen into our temp
	BitBlt(m_gdiTempBack, rcClient.left, rcClient.top, WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiMem, rcClient.left, rcClient.top, SRCCOPY);

	//draw the first frame onto the erased hex
	for(int x = 0; x < GAME_WIDTH; x++)
	{
		for(int y = 0; y < m_oGame.GetNumRows(); y++)
		{
			if(m_oGame.GetPreDropHex(x,y)->ePiece == HEX_Erased)
			{
				if(m_oGame.GetPrevHex(x,y)->ePiece == HEX_GamePiece || m_oGame.GetPrevHex(x,y)->ePiece == HEX_BonusOrb)
					DrawHexagon(m_gdiTempBack.GetDC(), m_ptGameField[x][y], GetBackgroundColor(BGCOLOR_Grid), GetBackgroundColor(BGCOLOR_Hex), m_iHexSize, TRUE);	
				if(m_oGame.GetPrevHex(x,y)->ePiece == HEX_BonusOrb)
					bAniOrb = TRUE; //need to correct the animation for the orb
			}
		}
	}

	ResetAniArray();//clear out the animation timing array
	int iCurrentFrame = 0;

	int iTotalFrames;
	if(bAniOrb)
	{
		iTotalFrames = m_iNumFireballFrames + SetAniArrayOrbLineClear();
		PlaySounds(EFFECT_Explode);
	}
	else
	{
		iTotalFrames = m_iNumExplosionFrames + SetAniArrayLineClear();//for now ... we'll check for orbs later
	}

	iPrevCount = GetTickCount();

	for(int i = 0; i < iTotalFrames; i++)
	{
		//redraws the "blank" background 
		BitBlt(m_gdiMem.GetDC(), rcClient.left, 
			rcClient.top, 
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			m_gdiTempBack.GetDC(), 
			rcClient.left, rcClient.top,
			SRCCOPY);

		for(int x = 0; x < GAME_WIDTH; x++)
		{
			for(int y = 0; y < m_oGame.GetNumRows(); y++)
			{
				if(m_iAniArray[x][y] == ANI_NO_RENDER)
					continue;

				//check if we should be animating ... well ... anything
				if(m_iAniArray[x][y] >= 0)
				{	//we need to render something
					if(m_oGame.GetPrevHex(x,y)->ePiece == HEX_GamePiece)
					{
						//no more animations yo
						if(m_iAniArray[x][y] >= m_iNumExplosionFrames)
						{
							m_iAniArray[x][y] = ANI_NO_RENDER;
							continue;
						}
					
						CIssGDI::MyAlphaImage(m_gdiMem,
							m_ptGameField[x][y].x + m_iHexSize/2 - m_sizeHex.cx,
							m_ptGameField[x][y].y - m_sizeHex.cy/2,
							m_sizeHex.cx*2, m_gdiExplosion.GetHeight(),
							m_gdiExplosion, 
							m_gdiExplosionAlpha,
							m_iAniArray[x][y]*m_sizeHex.cx*2, 0);
					}
					//check if we're past our animation and set it to ANI_NO_RENDER
				}
				else //not going to bother checking the orbs .. only the gem
				{
					if(m_oGame.GetPrevHex(x,y)->ePiece == HEX_GamePiece)
					{
						CIssGDI::MyAlphaImage(m_gdiMem,
							m_ptGameField[x][y].x + m_iHexSize/2 - m_sizeHex.cx,
							m_ptGameField[x][y].y - m_sizeHex.cy/2,
							m_sizeHex.cx*2, m_gdiExplosion.GetHeight(),
							m_gdiExplosion, 
							m_gdiExplosionAlpha,
							0, 0);
					}
				}
				//m_iAniArray[x][y]++;//move the animation index along for next pass
			}
		}

		for(int x = 0; x < GAME_WIDTH; x++)
		{
			for(int y = 0; y < m_oGame.GetNumRows(); y++)
			{
				if(m_iAniArray[x][y] == ANI_NO_RENDER)
					continue;

				//check if we should be animating ... well ... anything
				if(m_iAniArray[x][y] >= 0)
				{	//we need to render something
					if(m_oGame.GetPrevHex(x,y)->ePiece != HEX_GamePiece)
					{
						//no more animations yo
						if(m_iAniArray[x][y] >= m_iNumFireballFrames)
						{
							m_iAniArray[x][y] = ANI_NO_RENDER;
							continue;
						}

						CIssGDI::MyAlphaImage(m_gdiMem,
							m_ptGameField[x][y].x + m_iHexSize/2 - m_sizeHex.cx,
							m_ptGameField[x][y].y - m_sizeHex.cy/2,
							m_sizeHex.cx*2, m_gdiExplosion.GetHeight(),
							m_gdiFireball, 
							m_gdiFireballAlpha,
							m_iAniArray[x][y]*m_sizeHex.cx*2, 0);
					}
					//check if we're past our animation and set it to ANI_NO_RENDER
				}
				m_iAniArray[x][y]++;//move the animation index along for next pass
			}
		}

		BitBlt(dc, rcClient.left, 
			rcClient.top, 
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			m_gdiMem.GetDC(), 
			rcClient.left, rcClient.top,
			SRCCOPY);

		iTickCount = GetTickCount();

		iTemp = iPrevCount + ANI_WAIT_TIME - iTickCount;

		if(iTemp > 0)
			Sleep(iTemp);

		iPrevCount = iTickCount;

		m_dwFrameCount++;
	}
	ReleaseDC(m_hWnd, dc);

	Resume();
}

void CDlgHextris::StartAddScore(int iScore)
{
	if(m_bDrawFloat && m_szFloatMsg[0] != _T('+'))
	{
		return;
	}
	
	m_oStr->Format(m_szFloatMsg, _T("+%d"), iScore);
	m_dwStartFloatMsg	= 0;
	m_bDrawFloat = TRUE;
}

void CDlgHextris::StartAddMessage(TCHAR* szText)
{
	if(szText == NULL)
		return;

	m_dwStartFloatMsg = 0;
	m_oStr->StringCopy(m_szFloatMsg, szText);
	m_bDrawFloat = TRUE;
}

BOOL CDlgHextris::DrawFloatingMsg(CIssGDI &gdiDest, RECT rcClient)
{
	if(m_bDrawFloat == FALSE)
	{
		return FALSE;
	}

	if(m_dwStartFloatMsg == 0)
		m_dwStartFloatMsg = GetTickCount();

	int iTimeDiff		= m_dwTickCount - m_dwStartFloatMsg;
	int iCurrentFrame	= 0;

	if(iTimeDiff == 0)
	{}
	else if(iTimeDiff > TIME_CompleteScoreAddition)
	{
		// we're done animating
		m_bDrawFloat = FALSE;
		return FALSE;
	}
	else
	{
		iCurrentFrame = iTimeDiff*FRAME_ScoreAdditionCount/TIME_CompleteScoreAddition;
	}

	if(m_oGame.GetPowerSaving() == TRUE)
	{
		iCurrentFrame = 0;
	}

	RECT rcScore	= rcClient;
	rcScore.right	= m_rcBackGround.left;
	rcScore.top		= rcClient.bottom/4 - iCurrentFrame;
	rcScore.bottom	= rcScore.top + m_imgFont.GetHeight();

	m_imgFont.DrawText(gdiDest, m_szFloatMsg, rcScore, DT_CENTER, 100 - iCurrentFrame*100/FRAME_ScoreAdditionCount);

	return TRUE;
}

void CDlgHextris::AnimateFourOrbs()
{
	//get the game engine in the correct state
	Pause(FALSE);
	
	HDC dc = GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	OnPaint(m_hWnd, dc, rcClient);

	m_oGame.FourOrbClear();

	//flash the orbs
	for(int i = 0; i < 6; i++)
	{
		DrawBackground(m_gdiMem.GetDC(), rcClient);
		DrawText(m_gdiMem, rcClient, FALSE);
		DrawLevelProgress(m_gdiMem, rcClient);
		DrawPiecePreview(m_gdiMem, rcClient);

		if(i%2 == 1)
		{	//draw saved orbs
			for(int i = 0; i < MAX_SAVED_GEMS; i++)
			{
				CIssGDI::MyAlphaImage(m_gdiMem,	
					m_rcOrbs[i].left, m_rcOrbs[i].top, WIDTH(m_rcOrbs[i]), HEIGHT(m_rcOrbs[i]), m_gdiImages, m_gdiImagesAlpha, 
					((int)II_Orb)*WIDTH(m_rcOrbs[i]), 0);		
			}
		}

		BitBlt(dc, rcClient.left, rcClient.top, WIDTH(rcClient), HEIGHT(rcClient), m_gdiMem.GetDC(), rcClient.left, rcClient.top, SRCCOPY);
		Sleep(150);
	}

	ReleaseDC(m_hWnd, dc);
	

	Resume();
	AnimateLineCleared();
	
	m_bIsBackgroundDirty = TRUE;
	InvalidateRect(m_hWnd, NULL, FALSE);
}

void CDlgHextris::AnimateEndGame()
{
	HDC dc = GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	DrawBackground(m_gdiMem.GetDC(), rcClient);
	DrawText(m_gdiMem, rcClient, FALSE);
	DrawLevelProgress(m_gdiMem, rcClient);
	DrawPiecePreview(m_gdiMem, rcClient);

	//draw saved orbs
	for(int i = 0; i < m_oGame.GetSavedGems(); i++)
	{
		CIssGDI::MyAlphaImage(m_gdiMem,	
			m_rcOrbs[i].left, m_rcOrbs[i].top, WIDTH(m_rcOrbs[i]), HEIGHT(m_rcOrbs[i]), m_gdiImages, m_gdiImagesAlpha, 
			((int)II_Orb)*WIDTH(m_rcOrbs[i]), 0);		
	}
	
	//	for(int i = 0; i <= iHeight; i++)
	for(int i = m_oGame.GetNumRows() - 1; i > -1; i--)
	{
		//animate up from the bottom
		for(int x = 0; x < GAME_WIDTH; x++)
		{
			if(m_oGame.IsGamePiece(m_oGame.GetBoardHex(x,i)->ePiece))
			{
				CIssGDI::MyAlphaImage(m_gdiMem,
					m_ptGameField[x][i].x + m_iHexSize/2 - m_sizeHex.cx,
					m_ptGameField[x][i].y - m_sizeHex.cy/2,
					m_sizeHex.cx*2, m_gdiExplosion.GetHeight(),
					m_gdiExplosion, 
					m_gdiExplosionAlpha,
					0, 0);
			}
		}



//		Sleep(200);
	}

	CIssGDIEffects::FadeIn(dc, m_gdiMem);

	BitBlt(dc, rcClient.left, 
		rcClient.top, 
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		m_gdiMem.GetDC(), 
		rcClient.left, rcClient.top,
		SRCCOPY);

	ReleaseDC(m_hWnd, dc);

	Sleep(500);

}

void CDlgHextris::AnimateLevelChangeChallenge()
{
	HDC dc = GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int iHeight = 1;

	for(int y = m_oGame.GetNumRows() - 1; y > 0; y--)//start at the top
	{
		for(int x = 0; x < GAME_WIDTH; x++)
		{
			if(m_oGame.GetPrevHex(x, y)->ePiece != HEX_Blank)
			{
				iHeight = y;
				break;
			}
		}
		if(iHeight != 1)
			break;//just a quick hack to get out of the for loop
	}

//	for(int i = 0; i <= iHeight; i++)
	for(int i = m_oGame.GetNumRows() - 1; i > iHeight; i--)
	{
		//animate up from the bottom
		for(int x = 0; x < GAME_WIDTH; x++)
		{
			DrawHexagon(m_gdiMem.GetDC(), m_ptGameField[x][i], OUTLINE_COLOR, RGB(100,100,100), m_iHexSize);
		}

		BitBlt(dc, rcClient.left, 
			rcClient.top, 
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			m_gdiMem.GetDC(), 
			rcClient.left, rcClient.top,
			SRCCOPY);

		Sleep(350);
	}

	ReleaseDC(m_hWnd, dc);
}


COLORREF CDlgHextris::GetBackgroundColor(EnumBGColor eColor)
{
	if(eColor == BGCOLOR_Hex)
		return 0x1A1A1A;

	if(eColor == BGCOLOR_Grid)
	{
		return BACKGROUND_OUTLINECOLOR;
	}

	else if(eColor == BGCOLOR_Grad1)
	{
		return 0x4A4A4A;
		}

	else if(eColor == BGCOLOR_Grad2)
	{
		return 0x4A4A4A;
	}
	return 0xFFFFFF;//should never get here
}



BOOL CDlgHextris::ShouldAbort()
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

BOOL CDlgHextris::HackAnimation()
{
	HDC hdc;

	hdc = GetDC(m_hWnd);

	int iFrames = 10;

	int iWidth = m_gdiAnimation.GetWidth()/iFrames;

	//change the multiplication factor in the for loop to change the number of loops
	//ie ... 5*
	for(int i = 0; i < 5*iFrames; i++)
	{
		TransparentBlt(hdc, m_ptGameField[0][0].x, m_ptGameField[0][0].y, 
			iWidth, m_gdiAnimation.GetHeight(), 
			m_gdiAnimation.GetDC(), 
			iWidth*(i%iFrames), 0, 
			m_gdiAnimation.GetHeight(), m_gdiAnimation.GetHeight(), 
			TRANSPARENT_COLOR);

		//delay between frames
		Sleep(150); // in milliseconds
	}

	ReleaseDC(m_hWnd, hdc);

	return TRUE;
}

void CDlgHextris::ReloadFonts()
{
	// we really don't need to reload the whole thing
	if(m_imgFont.IsLoaded())
		return;

	m_imgFont.Initialize(IDR_GFont, IDR_GFont_Alpha, m_hWnd, m_hInst, FONT_TEXT_SIZE);
}

void CDlgHextris::ReloadSounds()
{
	if(m_oSoundFX.IsLoaded())
		return;

	m_oSoundFX.Initialize(m_hInst);

	m_oSoundFX.LoadSFX(IDWAV_DOWN);
	m_oSoundFX.LoadSFX(IDWAV_GAMEOVER);
	m_oSoundFX.LoadSFX(IDWAV_LEVELUP);
	m_oSoundFX.LoadSFX(IDWAV_LINECLEAR);
	m_oSoundFX.LoadSFX(IDWAV_LINECLEARMULTI);
	m_oSoundFX.LoadSFX(IDWAV_MOVE);
	m_oSoundFX.LoadSFX(IDWAV_NEWHIGHSCORE);
	m_oSoundFX.LoadSFX(IDWAV_SWITCH);
	m_oSoundFX.LoadSFX(IDWAV_EXPLODE);

	m_oSoundFX.LoadMOD(IDR_PTMOD2, TRUE);
	
	m_oSoundFX.SetVolumeMOD(m_oGame.GetMusicLevel());
	m_oSoundFX.SetVolumeSFX(m_oGame.GetSFXLevel());

	m_oSoundFX.PlayMOD();
	m_oSoundFX.PauseMOD();

}

BOOL CDlgHextris::DrawLevelProgress(CIssGDI &gdiDest, RECT rcClient)
{
	int iPercentWidth = (int)((double)WIDTH(m_rcLevelProg)*m_oGame.GetLevelPercent());

	if(m_gdiLevel.GetDC() == NULL || m_gdiLevelBackground.GetDC() == NULL)
	{
		TypeColorInfo sInfo;
		memset(&sInfo, 0, sizeof(TypeColorInfo));

		sInfo.crBackground	= 0x555555;
		sInfo.crAlpha1		= 0x999999;
		sInfo.crAlpha2		= 0xFFFFFF;
		sInfo.iPcnt1		= 33;
		//		sInfo.iPcnt2		= 0;
		sInfo.crColor1		= RGB(89,102,89);//RGB(255, 0, 0);
		sInfo.crColor2		= RGB(58,222,195);//RGB(255,127,0);
		sInfo.crColor3		= RGB(255,255,255);//RGB(255,255,0);
		//		sInfo.crColor4		= RGB(0,0,0);

		CreateLevelGradients(gdiDest, sInfo);
	}

	BitBlt(gdiDest.GetDC(), 
		m_rcLevelProg.left, m_rcLevelProg.top, 
		iPercentWidth, HEIGHT(m_rcLevelProg), 
		m_gdiLevel.GetDC(), 
		0,0, 
		SRCCOPY);

	BitBlt(gdiDest.GetDC(), 
		m_rcLevelProg.left + iPercentWidth, m_rcLevelProg.top, 
		WIDTH(m_rcLevelProg) - iPercentWidth, HEIGHT(m_rcLevelProg), 
		m_gdiLevelBackground.GetDC(), 
		iPercentWidth,0,
		SRCCOPY);

	return TRUE;
}

void CDlgHextris::DrawAlphaImages(CIssGDI& gdiDest1, CIssGDI& gdiDest2, CIssGDI& gdiAlphaChannel, RECT rcDraw)
{
	LPVOID lpBuf1		= gdiDest1.GetBits();
	LPVOID lpBuf2		= gdiDest2.GetBits();
	LPVOID lpBufAlpha	= gdiAlphaChannel.GetBits();

	if(!lpBuf1 || 
		!lpBuf2 || 
		!lpBufAlpha)
		return;

	WORD*	lprgbDest1;
	WORD*	lprgbDest2;
	WORD*	lprgbAlpha;
	WORD		wR,wG,wB,wR2,wG2,wB2,wColor;
	DWORD		dwWeight1;//		= 255 - uiAlpha;
	DWORD		dwWeight2;//		= uiAlpha;
	DWORD		dwWidthBytes	= (DWORD)gdiDest1.GetBitmapInfo().bmWidthBytes;

	// Initialize the surface pointers.
	lprgbDest1  = (WORD*)((LPBYTE)lpBuf1  + dwWidthBytes*(gdiDest1.GetHeight() - rcDraw.bottom));
	lprgbDest2  = (WORD*)((LPBYTE)lpBuf2  + dwWidthBytes*(gdiDest2.GetHeight() - rcDraw.bottom));
	lprgbAlpha  = (WORD*)((LPBYTE)lpBufAlpha  + dwWidthBytes*(gdiAlphaChannel.GetHeight() - rcDraw.bottom));

	// loop through all the bits and change it to the proper color
	for (int y=0; y<rcDraw.bottom-rcDraw.top; y++) 
	{
		for (int x=rcDraw.left; x<rcDraw.right; x++) 
		{
			wColor		= lprgbAlpha[x];
			dwWeight1	= (DWORD)((wColor >> 11) & 0x1f);
			dwWeight2	= 32 - dwWeight1;

			wColor	= lprgbDest1[x];
			wR = (WORD)((wColor >> 11) & 0x1f);
			wG = (WORD)((wColor >> 5) & 0x3f);
			wB = (WORD)(wColor & 0x1f);
			wColor	= lprgbDest2[x];
			wR2 = (WORD)((wColor >> 11) & 0x1f);
			wG2 = (WORD)((wColor >> 5) & 0x3f);
			wB2 = (WORD)(wColor & 0x1f);

			wR   = (BYTE)((((DWORD)wR * dwWeight1) + (0)) >> 5);
			wG   = (BYTE)((((DWORD)wG * 2 * dwWeight1) + (0)) >> 6);
			wB   = (BYTE)((((DWORD)wB * dwWeight1) + (0)) >> 5);
			lprgbDest1[x] = (WORD)(wR<<11|wG<<5|wB);

			wR2  = (BYTE)((((DWORD)wR2 * dwWeight1) + (0)) >> 5);
			wG2  = (BYTE)((((DWORD)wG2 * 2 * dwWeight1) + (0)) >> 6);
			wB2  = (BYTE)((((DWORD)wB2 * dwWeight1) + (0)) >> 5);
			lprgbDest2[x] = (WORD)(wR2<<11|wG2<<5|wB2);
		}

		// Move to next scan line.
		lprgbDest1 = (WORD *)((LPBYTE)lprgbDest1 + dwWidthBytes);
		lprgbDest2 = (WORD *)((LPBYTE)lprgbDest2 + dwWidthBytes);
		lprgbAlpha = (WORD *)((LPBYTE)lprgbAlpha + dwWidthBytes);
	}
}

BOOL CDlgHextris::CreateLevelGradients(CIssGDI &gdiDest, TypeColorInfo& sColorInfo)
{
	RECT rcClient;
	SetRect(&rcClient, 0, 0, WIDTH(m_rcLevelProg), HEIGHT(m_rcLevelProg));
	m_gdiLevel.Create(gdiDest.GetDC(), m_rcLevelProg, FALSE, TRUE);
	m_gdiLevelBackground.Create(gdiDest.GetDC(), m_rcLevelProg, FALSE, TRUE);

	CIssGDI gdiOverLay;
	gdiOverLay.Create(gdiDest.GetDC(), rcClient, FALSE);

	//fill the background with grey
	RECT rcTemp;
	rcTemp = rcClient;
	rcTemp.bottom -= rcTemp.top;
	rcTemp.top = 0;

	CIssGDI::FillRect(m_gdiLevelBackground.GetDC(), rcTemp, sColorInfo.crBackground);

	//create the overlay gradients
	rcTemp.bottom /= 2; //top half please
	CIssGDI::GradriantFillRect(gdiOverLay, rcTemp, sColorInfo.crAlpha1, sColorInfo.crAlpha2);
	rcTemp.top = rcTemp.bottom;
	rcTemp.bottom = rcClient.bottom - rcClient.top;
	CIssGDI::GradriantFillRect(gdiOverLay, rcTemp, sColorInfo.crAlpha2, sColorInfo.crAlpha1);


	rcTemp = rcClient;
	rcTemp.bottom -= rcTemp.top;
	rcTemp.top = 0;

	//ok gonna be a little lazy and use the code I already have
	if(sColorInfo.iPcnt1)	//3 or 4
	{
		rcTemp.right = (rcClient.right - rcClient.left)*sColorInfo.iPcnt1/100;
		CIssGDI::GradriantAngleFillRect(m_gdiLevel, rcTemp, sColorInfo.crColor1, sColorInfo.crColor2);

		if(sColorInfo.iPcnt2) //4
		{
			rcTemp.left = rcTemp.right;
			rcTemp.right = (rcClient.right - rcClient.left)*sColorInfo.iPcnt2/100;
			CIssGDI::GradriantAngleFillRect(m_gdiLevel, rcTemp, sColorInfo.crColor2, sColorInfo.crColor3);

			rcTemp.left = rcTemp.right;
			rcTemp.right = (rcClient.right - rcClient.left);
			CIssGDI::GradriantAngleFillRect(m_gdiLevel, rcTemp, sColorInfo.crColor3, sColorInfo.crColor4);
		}
		else //3
		{
			rcTemp.left = rcTemp.right;
			rcTemp.right = (rcClient.right - rcClient.left);
			CIssGDI::GradriantAngleFillRect(m_gdiLevel, rcTemp, sColorInfo.crColor2, sColorInfo.crColor3);
		}
	}
	else	//2 Color
	{
		rcTemp.right = (rcClient.right - rcClient.left);
		CIssGDI::GradriantAngleFillRect(m_gdiLevel, rcTemp, sColorInfo.crColor1, sColorInfo.crColor2);
	}

	//now we're going to use the overlay as an alpha mask ala iSS Wifi Scanner
	rcTemp = rcClient;
	rcTemp.bottom -= rcTemp.top;
	rcTemp.top = 0;

	DrawAlphaImages(m_gdiLevel, m_gdiLevelBackground, gdiOverLay, rcTemp);

	return TRUE;
}

void CDlgHextris::ResetAniArray()
{
	for(int i = 0; i < GAME_WIDTH; i++)
	{	
		for(int j = 0; j < m_oGame.GetNumRows(); j++)
		{
			m_iAniArray[i][j] = ANI_NO_RENDER;
		}
	}
}

//powned
int CDlgHextris::SetAniArrayLineClear()
{
	for(int i = 0; i < GAME_WIDTH; i++)
	{
		for(int j = 0; j < m_oGame.GetNumRows(); j++)
		{
			//we check with the game engine as to which gems are going to explode and set the animation 
			if(m_oGame.GetPreDropHex(i,j)->ePiece == HEX_Erased)
			{
				//set the animation timing -4,-3,-2,-1,0,0,-1,-2,-3,-4
				if(i < GAME_WIDTH/2)
					m_iAniArray[i][j] = i + 1 - GAME_WIDTH/2;
				else
					m_iAniArray[i][j] = GAME_WIDTH/2 - i;
			}
		}
	}

	return GAME_WIDTH/2 - 1;
}

int CDlgHextris::SetAniArrayOrbLineClear()
{
	int iReturn = 0;
	int iOrbCount = 0;
	POINT ptOrbs[10];

	// populate our orb array
	for(int i = 0; i < GAME_WIDTH; i++)
	{
		for(int j = 0; j < m_oGame.GetNumRows(); j++)
		{
			if(m_oGame.GetPreDropHex(i,j)->ePiece == HEX_Erased && m_oGame.GetPrevHex(i,j)->ePiece == HEX_BonusOrb)
			{
				ptOrbs[iOrbCount].x	= i;
				ptOrbs[iOrbCount].y = j;
				iOrbCount++;

				// also set the animation offset for this value
				m_iAniArray[i][j] = 0;
			}
		}
	}

	for(int i = 0; i < GAME_WIDTH; i++)
	{
		for(int j = 0; j < m_oGame.GetNumRows(); j++)
		{
			//we check with the game engine as to which gems are going to explode and set the animation 
			if(m_oGame.GetPreDropHex(i,j)->ePiece == HEX_Erased && m_oGame.GetPrevHex(i,j)->ePiece != HEX_BonusOrb)
			{
				int iDistance = MAX_GAME_HEIGHT;
				POINT ptHex;
				ptHex.x = i;
				ptHex.y = j;
				// find out the least distance from the orb
				for(int orb=0; orb<iOrbCount; orb++)
				{
					iDistance = min(GetDistance(ptHex, ptOrbs[orb]), iDistance);
					iReturn = max(iDistance, iReturn);

					m_iAniArray[i][j] = 0 - ANI_ORB_WAIT - iDistance;
				}
			}
		}
	}

	return iReturn;
}


//points are locations in the game array ... ie game co-ordinates
int CDlgHextris::GetDistance(POINT pt1, POINT pt2)
{
	RECT rc;

	for(int i = 1; i < MAX_GAME_HEIGHT; i++)//we should never be this far apart but just in case
	{
		rc.left		= pt2.x - i;
		rc.right	= pt2.x + i;
		rc.top		= pt2.y - i;
		rc.bottom	= pt2.y + i;

		if(PtInRect(&rc, pt1))
			return i;

	}
	return MAX_GAME_HEIGHT;
}