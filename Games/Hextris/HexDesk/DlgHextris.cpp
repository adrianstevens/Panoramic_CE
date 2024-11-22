#include "StdAfx.h"
#include "DlgHextris.h"
#include "resource.h"
#include "IssRegistry.h"
#include "Commdlg.h"
#include "Winuser.h"
#include "IssCommon.h"
#include "IssGDIEffects.h"
#include "Mmsystem.h"


#define TXT_Title		_T("iSS Hexaminoes 1.0")

#define IDMENU_Restart			500
#define IDMENU_GameClassic		501
#define IDMENU_GameChallenge	502
#define IDMENU_GameUltra		503
#define IDMENU_Game40			504
#define IDMENU_Pause			600
#define IDMENU_HiScores			601

#define HEX_PREVIEW_SIZE 8

#define NUM_HEX_COLORS 10

COLORREF cores[NUM_HEX_COLORS][3] = { 

/*	{RGB(0,95,191), RGB(0,127,225), RGB(0,64,127)},			//cyan
	{ 49087,	65535,		32639 }, 						//yellow
	{RGB(191,0,0), RGB(255,0,0), RGB(127,0,0)},				//Red
	{RGB(191,191,191), RGB(255,255,255), RGB(127,127,127)},	//white
	{RGB(191,95,0), RGB(255,127,0), RGB(127,64,0)},			//orange
	{ 0xCE5000, 0xFF6400,	0x7B3200},						//new blue
	{RGB(48,0,191), RGB(64,0,255), RGB(32,0,127)},			//purple
	{RGB(191,0,191), RGB(240,0,255), RGB(127,0,127)},		//fuschia
	{RGB(0,191,48), RGB(0,255,64), RGB(0,127,32)},			//green
	{RGB(127,127,127), RGB(191,191,191), RGB(64,64,64)},	//grey*/

	{RGB(191,0,0),		RGB(255,0,0),		RGB(127,0,0)},			//Red
	{RGB(191,95,0),		RGB(255,127,0),		RGB(127,64,0)},			//orange
	{RGB(191,0,191),	RGB(240,0,255),		RGB(127,0,127)},		//fuschia
	{RGB(0,191,191),	RGB(0,255,255),		RGB(0,127,127)},		//cyan
	{RGB(0,95,191),		RGB(0,127,255),		RGB(0,64,127)},			//blue
	{RGB(127,0,191),	RGB(191,0,255),		RGB(92,0,127)},			//purple
	{RGB(0,191,0),		RGB(0,255,0),		RGB(0,127,0)},			//green
	{RGB(191,191,0),	RGB(255,255,0),		RGB(127,127,0)},		//yellow
	{RGB(191,191,191),	RGB(255,255,255),	RGB(127,127,127)},		//white
	{RGB(127,127,127), RGB(191,191,191),	RGB(64,64,64)},	//grey


};


//#define HEX_RATIO 866
//#define HEX_SIZE 10 //use even numbers only


#define HEX_RATIO 900
#define HEX_SIZE 10 //use even numbers only


#define BOTTOM_INDENT 3

#define IDT_IDLE_TIMER	1 //Timer to force game pieces down
#define IDT_CLOCK_TIMER 2
#define IDT_ANI_LEVEL   3 //this is a hack timer to correct the order of the game engine posting messages .. specfically it posts level change before line clear
#define IDT_LEVEL_UP	4 //timer so it plays the level up message a little later
#define IDT_TIMER_HISCORE 5

#define IDT_DRAW_TIMER 6
/* 
// Orange
#define BACKGROUND_HEXCOLOR			0
#define BACKGROUND_OUTLINECOLOR		0x2F3366
#define BACKGROUND_GRAD1			0x0387FD
#define BACKGROUND_GRAD2			0x00001D*/

//Aqua
#define BACKGROUND_HEXCOLOR			0x1A1A1A//0
#define BACKGROUND_OUTLINECOLOR		0x333333//0x4A5B32
#define BACKGROUND_GRAD1			0xFFF603
#define BACKGROUND_GRAD2			0x00001D

#define OUTLINE_COLOR				0x888888

CDlgHextris::CDlgHextris(void)
{
	m_iHexSize			= HEX_SIZE*GetSystemMetrics(SM_CXICON)/16;
	m_iHexPreviewSize	= HEX_PREVIEW_SIZE*GetSystemMetrics(SM_CXICON)/16;
	m_iHexSizeCos30		= m_iHexSize*HEX_RATIO/1000;

	m_sizeHex.cx		= m_iHexSize*2+1;
	m_sizeHex.cy		= m_iHexSizeCos30*2+1;

	m_sizePreview.cx	= m_iHexPreviewSize*2;
	m_sizePreview.cy	= m_sizePreview.cx*HEX_RATIO/1000;

	m_hFontText			= CIssGDI::CreateFont(GetSystemMetrics(SM_CXICON), FW_BOLD, TRUE);

	m_dwFrameCount		= 0;
	m_dwStartTime		= GetTickCount();
	m_bPlaySounds		= TRUE;
	m_bPause			= FALSE;
	m_bShowGhost		= TRUE;
}

CDlgHextris::~CDlgHextris(void)
{
	m_oStr->DeleteInstance();
	CIssGDI::DeleteFont(m_hFontText);

	
}

BOOL CDlgHextris::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	if(m_gdiMem.GetWidth() != (rcClient.right - rcClient.left) ||
	   m_gdiMem.GetHeight() != (rcClient.bottom - rcClient.top))
	{
		m_gdiMem.Destroy();
		m_gdiBackground.Destroy();
	}

	int iIndent = GetSystemMetrics(SM_CXSMICON)/2;

	//m_ptPreview1[0][3].x	= (GetSystemMetrics(SM_CXSCREEN) - m_iHexSize*11)/2 + m_iHexSize*11 - 2*m_iHexSize;
	
	int iXStart = rcClient.left + WIDTH(rcClient)/2 + m_iHexSize*4;
	int iYStart = iIndent/2;

//	m_ptPreview1[0][3].x	= GetSystemMetrics(SM_CXSCREEN)/2 + m_iHexSize*6;
//	m_ptPreview1[0][3].y	= iIndent;


	int j = iYStart + 6*HEX_RATIO/1000*m_iHexSize;

	for(int iRow = 0; iRow < 4; j++)//3 rows
	{
		for(int i = 0; i < 2; i++)//4 columns
		{
			m_ptPreview1[i*2][iRow].x = i*m_iHexSize*3 + m_iHexSize/2 + iXStart;
			m_ptPreview1[i*2][iRow].y = j;

			m_ptPreview1[i*2+1][iRow].x = i*m_iHexSize*3 + 2*m_iHexSize + iXStart;
			m_ptPreview1[i*2+1][iRow].y = m_iHexSizeCos30 + m_ptPreview1[i*2][iRow].y;
		}

		j -= 2*m_iHexSizeCos30;
		iRow++;
	}

	j = 2*iYStart + 12*HEX_RATIO/1000*m_iHexSize;

	for(int iRow = 0; iRow < 4; j++)//3 rows
	{
		for(int i = 0; i < 2; i++)//4 columns
		{
			m_ptPreview2[i*2][iRow].x = i*m_iHexSize*3 + m_iHexSize/2 + iXStart;
			m_ptPreview2[i*2][iRow].y = j;

			m_ptPreview2[i*2+1][iRow].x = i*m_iHexSize*3 + 2*m_iHexSize + iXStart;
			m_ptPreview2[i*2+1][iRow].y = m_iHexSizeCos30 + m_ptPreview2[i*2][iRow].y;
		}

		j -= 2*m_iHexSizeCos30;
		iRow++;
	}

	j = 2*iYStart + 20*HEX_RATIO/1000*m_iHexSize;

	for(int iRow = 0; iRow < 4; j++)//3 rows
	{
		for(int i = 0; i < 2; i++)//4 columns
		{
			m_ptSaved[i*2][iRow].x = i*m_iHexSize*3 + m_iHexSize/2 + iXStart;
			m_ptSaved[i*2][iRow].y = j;

			m_ptSaved[i*2+1][iRow].x = i*m_iHexSize*3 + 2*m_iHexSize + iXStart;
			m_ptSaved[i*2+1][iRow].y = m_iHexSizeCos30 + m_ptSaved[i*2][iRow].y;
		}

		j -= 2*m_iHexSizeCos30;
		iRow++;
	}




	
	return TRUE;
}


BOOL CDlgHextris::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef SHELL_AYGSHELL
	//Show as a Pocket PC full screen dialog.
	SHINITDLGINFO DlgInfo;

	DlgInfo.dwMask	= SHIDIM_FLAGS;
	DlgInfo.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
	DlgInfo.hDlg	= hWnd;
	SHInitDialog(&DlgInfo);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(mbi)); 
	mbi.cbSize		= sizeof(mbi);
	mbi.hwndParent	= hWnd;
	mbi.nToolBarId	= IDR_MENU_Hextris;
	mbi.hInstRes	= m_hInst;
	if(!SHCreateMenuBar(&mbi))
		ASSERT(0);
#endif

	//lets position the window
	RECT rcTemp;
	rcTemp.top		= GetSystemMetrics(SM_CYSCREEN)/8;
	rcTemp.bottom   = GetSystemMetrics(SM_CYSCREEN) - rcTemp.top;

	rcTemp.left		= GetSystemMetrics(SM_CXSCREEN)/2 - HEIGHT(rcTemp)/3;
	rcTemp.right	= rcTemp.left + HEIGHT(rcTemp)*2/3;

	MoveWindow(m_hWnd, rcTemp.left, rcTemp.top, WIDTH(rcTemp), HEIGHT(rcTemp), TRUE);

	//get the clock running
	SetTimer(m_hWnd, IDT_CLOCK_TIMER, 1000, NULL);

	m_oGame.Init(m_hWnd);
	NewGame();

//	SetTimer(m_hWnd, IDT_DRAW_TIMER, 15, NULL);

	return TRUE;
}

BOOL CDlgHextris::DrawBackground(HDC hdc, RECT rcClient)
{
	if(m_gdiBackground.GetDC() == NULL)
	{
		m_gdiBackground.Create(hdc, rcClient, FALSE, TRUE, FALSE, 24);

		CIssGDI::GradriantFillRect(m_gdiBackground.GetDC(), rcClient, GetBackgroundColor(BGCOLOR_Grad1), GetBackgroundColor(BGCOLOR_Grad2), TRUE);
		//m_gdiBackground.AddBlur(m_gdiBackground, 5, 5, FALSE);

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

		m_oGame.SetNumRows(iRow);
	}

	BitBlt(m_gdiMem.GetDC(), rcClient.left, 
		rcClient.top, 
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		m_gdiBackground.GetDC(), 
		rcClient.left, rcClient.top,
		SRCCOPY);

	return TRUE;
}

BOOL CDlgHextris::DrawPiecePreview(HDC hDC, RECT rcClient)
{
	static POINT pt;

	POINT ptTemp;

	int iYOffset = 0;

	//a little brute force for the moment
	for(int i = 0; i < 4; i++)
	{
		if(i%2==0)
			iYOffset++;

	//	for(int k = 0; k < 5; k++)
	//		DrawHexagon(hDC, m_ptPreview1[i][k], BACKGROUND_OUTLINECOLOR, BACKGROUND_HEXCOLOR, m_iHexSize);	

		for(int j = 1; j < 5; j++)
		{
						

			if(m_oGame.GetNextPiece(1)->GetHex(i, j)->ePiece == HEX_GamePiece)
			{
				ptTemp.x = i;
				ptTemp.y = 4;
				ptTemp.y -= (j+iYOffset);
				if(ptTemp.y < 4 && ptTemp.y >= -1)
				{
					//DrawHexagon(hDC, m_ptPreview1[ptTemp.x][ptTemp.y+1], OUTLINE_COLOR, 0xFFFFFF, m_iHexSize);
					DrawHexagon(hDC, m_ptPreview1[ptTemp.x][ptTemp.y+1], m_oGame.GetNextPiece(1)->GetPieceType());
				}
				else
				{
					int asd = 0;
				}
			}


			if(m_oGame.GetNextPiece(2)->GetHex(i, j)->ePiece == HEX_GamePiece)
			{
				ptTemp.x = i;
				ptTemp.y = 4;
				ptTemp.y -= (j+iYOffset);
				if(ptTemp.y < 4 && ptTemp.y >= -1)
				{
					//DrawHexagon(hDC, m_ptPreview2[ptTemp.x][ptTemp.y+1], OUTLINE_COLOR, 0xFFFFFF, m_iHexSize);
					DrawHexagon(hDC, m_ptPreview2[ptTemp.x][ptTemp.y+1], m_oGame.GetNextPiece(2)->GetPieceType());
				}
				else
				{
					int asd = 0;
				}
			}

			if(m_oGame.GetSavedPiece()->GetHex(i, j)->ePiece == HEX_GamePiece)
			{
				ptTemp.x = i;
				ptTemp.y = 4;
				ptTemp.y -= (j+iYOffset);
				if(ptTemp.y < 4 && ptTemp.y >= -1)
				{
					//DrawHexagon(hDC, m_ptSaved[ptTemp.x][ptTemp.y+1], OUTLINE_COLOR, 0xFFFFFF, m_iHexSize);
					DrawHexagon(hDC, m_ptSaved[ptTemp.x][ptTemp.y+1], m_oGame.GetSavedPiece()->GetPieceType());
				}
				else
				{
					int asd = 0;
				}
			}


		}
	}

	return TRUE;
}

BOOL CDlgHextris::DrawCurrentPiece(HDC hdc, RECT rcClient)
{
	//its just easier to grab it and draw
	CurrentPiece* oPiece = m_oGame.GetCurrentPiece();
	static POINT ptTemp;

	/*if(m_gdiGamePieces.GetDC() == NULL)
	{
		CIssGDI gdiTemp;
		RECT rcTemp = {0, 0, 2*HEX_SIZE, HEX_SIZE*HEX_RATIO/500};
		gdiTemp.Create(hdc, rcTemp, FALSE, TRUE);
		m_gdiGamePieces.Create(hdc, rcTemp, FALSE, TRUE, TRUE);
		CIssGDI::DrawShinyRoundRectangle(gdiTemp, rcTemp, 0xD3BD84, 0, 0, 0xD3BD84, 0x9E6331, 0x7D3108, 0xDF425A);
		//CIssGDI::GradriantFillRect(gdiTemp, rcTemp, RGB(99,181,191), RGB(133,241,255));

		//CIssGDI::GradriantFillRect(gdiTemp, rcTemp, 0xFFFFFF, 0xBBBBBB);


		POINT pt;
		pt.x = HEX_SIZE/2;
		pt.y = 0;

		//Draw a white Hex
		CIssGDI::FillRect(m_gdiGamePieces.GetDC(), rcTemp, TRANSPARENT_COLOR);
		DrawHexagon(m_gdiGamePieces.GetDC(), pt, 0xFFFFFF, 0xFFFFFF, m_iHexSize);

		COLORREF crDest;

		//copy shiny bits to hex
		// loop through all the bits and change it to the proper color
		for (int y=0; y < HEIGHT(rcTemp); y++) 
		{
			for (int x=0; x < WIDTH(rcTemp); x++) 
			{
				crDest = GetPixel(m_gdiGamePieces.GetDC(), x, y);

				if(crDest != TRANSPARENT_COLOR)
				{
					SetPixel(m_gdiGamePieces.GetDC(), x, y, GetPixel(gdiTemp.GetDC(), x, y));
				}
			}
		}		



		//draw a new hex outline
		DrawHexagon(m_gdiGamePieces.GetDC(), pt, 0x777777, 0xFF00FF, m_iHexSize);


	}*/

	int iYOffset = 0;//I'm not happy about this either ....

	for(int i = 0; i < 5; i++)
	{
		if((i+oPiece->GetLocation()->x)%2==0)
			iYOffset++;

		for(int j = 0; j < 5; j++)
		{
			if(oPiece->GetHex(i, j)->ePiece == HEX_GamePiece)
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
				//	DrawHexagon(hdc, m_ptGameField[ptTemp.x][ptTemp.y], OUTLINE_COLOR, 0xFFFFFF, m_iHexSize);
					DrawHexagon(hdc, m_ptGameField[ptTemp.x][ptTemp.y], oPiece->GetPieceType());

				/*	TransparentBlt(hdc, m_ptGameField[ptTemp.x][ptTemp.y].x - m_iHexSize/2, m_ptGameField[ptTemp.x][ptTemp.y].y, 
						m_sizeHex.cx, m_sizeHex.cy,
						m_gdiGamePieces.GetDC(), 0, 0, m_sizeHex.cx, m_sizeHex.cy, TRANSPARENT_COLOR);*/
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
			if(oPiece->GetHex(i, j)->ePiece == HEX_GamePiece)
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
					DrawHexagon(hdc, m_ptGameField[ptTemp.x][ptTemp.y], cores[m_oGame.GetGhost()->GetPieceType()][1], 0x222222, m_iHexSize);
				}
			}
		}
	}
	return TRUE;
}


BOOL CDlgHextris::DrawGameBoard(HDC hDC, RECT rcClient)
{
	for(int i = 0; i < GAME_WIDTH; i++)
	{
		for(int j = 0; j < m_oGame.GetNumRows(); j++)
		{
			if(m_oGame.GetBoardHex(i,j)->ePiece == HEX_GamePiece)
			//	DrawHexagon(hDC, m_ptGameField[i][j], OUTLINE_COLOR, RGB(200,222,255), m_iHexSize);
				DrawHexagon(hDC, m_ptGameField[i][j], m_oGame.GetBoardHex(i,j)->iColorIndex);
		}
	}
	return TRUE;
}

BOOL CDlgHextris::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, FALSE);
	
	m_dwFrameCount++;


	DrawBackground(m_gdiMem.GetDC(), rcClient);	
	DrawGameBoard(m_gdiMem.GetDC(), rcClient);
	DrawText(m_gdiMem.GetDC(), rcClient);

	if(m_bShowGhost)
		DrawGhost(m_gdiMem.GetDC(), rcClient);
	DrawCurrentPiece(m_gdiMem.GetDC(), rcClient);
	DrawPiecePreview(m_gdiMem.GetDC(), rcClient);
	
	BitBlt(hDC, rcClient.left, 
		rcClient.top, 
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		m_gdiMem.GetDC(), 
		rcClient.left, rcClient.top,
		SRCCOPY);

	return FALSE;
}

BOOL CDlgHextris::DrawText(HDC hDC, RECT rcClient)
{
	static TCHAR szTemp[STRING_NORMAL];
	RECT rcTemp = rcClient;
	rcTemp.right -= GetSystemMetrics(SM_CXICON)/2;

	m_oStr->Format(szTemp, _T("HiScore: %i"), m_oGame.GetHighScore());
	rcTemp.top = rcTemp.bottom/2 + GetSystemMetrics(SM_CXICON)/2;
	CIssGDI::DrawText(hDC, szTemp, rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0);

	rcTemp.top += GetSystemMetrics(SM_CXICON);
	m_oStr->Format(szTemp, _T("Score: %i"), m_oGame.GetScore());
	CIssGDI::DrawText(hDC, szTemp, rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0);

	rcTemp.top += GetSystemMetrics(SM_CXICON);
	m_oStr->Format(szTemp, _T("Rows: %i"), m_oGame.GetRowsCleared());
	CIssGDI::DrawText(hDC, szTemp, rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0);

	rcTemp.top += GetSystemMetrics(SM_CXICON);
	m_oStr->Format(szTemp, _T("Level: %i"), m_oGame.GetLevel());
	CIssGDI::DrawText(hDC, szTemp, rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0);

	rcTemp.top += GetSystemMetrics(SM_CXICON);
	m_oStr->Format(szTemp, _T("%.2i:%.2i"), m_oGame.GetTime()/60, m_oGame.GetTime()%60);
	CIssGDI::DrawText(hDC, szTemp, rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0);

	if(m_bPause)
	{
		rcTemp.top += GetSystemMetrics(SM_CXICON);
		CIssGDI::DrawText(hDC, _T("Paused"), rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0);
	}
	


	rcTemp.right--;

	m_oStr->Format(szTemp, _T("HiScore: %i"), m_oGame.GetHighScore());
	rcTemp.top = rcTemp.bottom/2 + GetSystemMetrics(SM_CXICON)/2;
	CIssGDI::DrawText(hDC, szTemp, rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0xFFFFFF);

	rcTemp.top += GetSystemMetrics(SM_CXICON);
	m_oStr->Format(szTemp, _T("Score: %i"), m_oGame.GetScore());
	CIssGDI::DrawText(hDC, szTemp, rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0xFFFFFF);

	rcTemp.top += GetSystemMetrics(SM_CXICON);
	m_oStr->Format(szTemp, _T("Rows: %i"), m_oGame.GetRowsCleared());
	CIssGDI::DrawText(hDC, szTemp, rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0xFFFFFF);

	rcTemp.top += GetSystemMetrics(SM_CXICON);
	m_oStr->Format(szTemp, _T("Level: %i"), m_oGame.GetLevel());
	CIssGDI::DrawText(hDC, szTemp, rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0xFFFFFF);

	rcTemp.top += GetSystemMetrics(SM_CXICON);
	m_oStr->Format(szTemp, _T("%.2i:%.2i"), m_oGame.GetTime()/60, m_oGame.GetTime()%60);
	CIssGDI::DrawText(hDC, szTemp, rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0xFFFFFF);

	if(m_bPause)
	{
		rcTemp.top += GetSystemMetrics(SM_CXICON);
		CIssGDI::DrawText(hDC, _T("Paused"), rcTemp, DT_RIGHT | DT_TOP, m_hFontText, 0xFFFFFF);
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
		PlaySounds(_T("IDWAV_LevelUp"));
		m_gdiBackground.Destroy();
		break;
	case IDT_TIMER_HISCORE:
		KillTimer(m_hWnd, IDT_TIMER_HISCORE);
		PlaySounds(_T("IDWAV_NewHighScore"));
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
	case _T('a'):
	case _T('A'):
		Pause();
		MessageBox(m_hWnd, _T("Hextris 1.0.0\r\n\r\nHappy Mother's Day\r\n\r\nMay 10, 2008"), _T("Hextris"), IDOK);
		break;
	case _T('g'):
	case _T('G'):
		m_bShowGhost = !m_bShowGhost;
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
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
	case _T('h'):
	case _T('H'):
		Pause();
		MessageBox(m_hWnd, _T("Controls:\r\n\r\nUse the left, right and down cursor keys to move\r\nUse the up cursor to rotate\r\n\r\n'1' to start a classic game\r\n'2' to start a challenge game\r\n'3' to start a timed game\r\n'4' to start a game of '40 lines'\r\n\r\n'a' to open the about box\r\n'g' to show/hide the ghost\r\n'h' to open help (but you knew that)\r\n'p' to pause\r\n'q' to quit\r\n'r' to restart\r\n's' to switch to your saved peice\r\n\r\nClassic Game: normal game, progressively gets faster, ends when you run out of space\r\n\r\nChallenge Game: random peices are added to the board.  Levels change after clearing 10 lines.\r\n\r\nTimed Game: you have 3 minutes to score as many points as possible\r\n\r\n40 Lines: Try to clear 40 lines as quickly as possible"), _T("Hextris Help"), MB_OK);		
		break;
	case _T('q'):
	case _T('Q'):
		if(IDYES == MessageBox(m_hWnd, _T("Are you sure you want to quit?"), _T("Hextris"), MB_YESNO))
			PostQuitMessage(0);
		break;
	case _T('p'):
	case _T('P'):
		Pause();
		break;
	case _T('r'):
	case _T('R'):
		Pause();
		PostMessage(m_hWnd, WM_COMMAND, IDMENU_Restart, 0);
		break;
	case _T('1'):
		Pause();
		PostMessage(m_hWnd, WM_COMMAND, IDMENU_GameClassic, 0);
		break;
	case _T('2'):
		Pause();
		PostMessage(m_hWnd, WM_COMMAND, IDMENU_GameChallenge, 0);
		break;
	case _T('3'):
		Pause();
		PostMessage(m_hWnd, WM_COMMAND, IDMENU_GameUltra, 0);
		break;
	case _T('4'):
		Pause();
		PostMessage(m_hWnd, WM_COMMAND, IDMENU_Game40, 0);
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
	case IDMENU_Restart:
		if(IDYES == MessageBox(m_hWnd, _T("Are you sure you want to start a new game?"), _T("Hexaminoes"), MB_YESNO))
			NewGame();
		break;
	case IDMENU_GameClassic:
		if(IDYES == MessageBox(m_hWnd, _T("This will end your current game and start a new 'Classic' game.\r\nAre you sure you want to continue?"), _T("Hexaminoes"), MB_YESNO))
		{
			m_oGame.SetGameType(GT_Classic);
			NewGame();
		}
		break;
	case IDMENU_GameChallenge:
		if(IDYES == MessageBox(m_hWnd, _T("This will end your current game and start a new 'Challenge' game.\r\nAre you sure you want to continue?"), _T("Hexaminoes"), MB_YESNO))
		{
			m_oGame.SetGameType(GT_Challenge);
			NewGame();
		}
		break;
	case IDMENU_GameUltra:	
		if(IDYES == MessageBox(m_hWnd, _T("This will end your current game and start a new 'Timed' game.\r\nAre you sure you want to continue?"), _T("Hexaminoes"), MB_YESNO))
		{
			m_oGame.SetGameType(GT_Ultra);
			NewGame();
		}
		break;
	case IDMENU_Game40:	
		if(IDYES == MessageBox(m_hWnd, _T("This will end your current game and start a new '40 Lines' game.\r\nAre you sure you want to continue?"), _T("Hexaminoes"), MB_YESNO))
		{
			m_oGame.SetGameType(GT_40);
			NewGame();
		}
		break;
	case IDMENU_Pause:
		Pause();
		break;
	case IDMENU_HiScores:
		break;
//	case IDMENU_Menu:
//		OnMenuLeft();
//		break;
//	case IDMENU_Game:
//		OnMenuRight();
//		break;
	}
	return TRUE;
}

void CDlgHextris::Pause()
{
	if(m_bPause == FALSE)
	{
		KillTimer(m_hWnd, IDT_CLOCK_TIMER);
		KillTimer(m_hWnd, IDT_IDLE_TIMER);
		InvalidateRect(m_hWnd, NULL, FALSE);
	}
	else
	{
		Resume();
	}

	m_bPause = !m_bPause;
	InvalidateRect(m_hWnd, NULL,FALSE);
	
}

void CDlgHextris::Resume()
{
	if(m_oGame.GetGameState() == GS_Ingame)
	{
		SetTimer(m_hWnd, IDT_CLOCK_TIMER, 1000, NULL);
		SetTimer(m_hWnd, IDT_IDLE_TIMER, m_oGame.GetLevelTiming(), NULL);
	}
}

void CDlgHextris::SaveRegistry()
{

}

void CDlgHextris::PlaySounds(TCHAR* szWave)
{
	if(m_bPlaySounds)
		PlaySound(szWave, m_hInst, SND_RESOURCE | SND_ASYNC);
}

BOOL CDlgHextris::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uiMessage)
	{
	case WM_LINE_CLEARED:
		if(wParam > 1)
			PlaySounds(_T("IDWAV_LineClearMulti"));
		else
			PlaySounds(_T("IDWAV_LineClear"));
		AnimateLineCleared();
		InvalidateRect(m_hWnd, NULL, FALSE);
		break;
	case WM_LEVEL_CHANGE:
		KillTimer(m_hWnd, IDT_IDLE_TIMER);
		if(m_oGame.GetGameType() == GT_Challenge)
			SetTimer(m_hWnd, IDT_ANI_LEVEL, 500, NULL);		
		else
		{
			SetTimer(m_hWnd, IDT_IDLE_TIMER, m_oGame.GetLevelTiming(), NULL);
			SetTimer(m_hWnd, IDT_LEVEL_UP, 300, NULL);
		}
		break;
	case WM_LINE_TOBE_CLEARED:
		break;//might be needed ... we'll see
	case WM_TIME_UP:
		InvalidateRect(m_hWnd, NULL, FALSE);
		AnimateEndGame();
		PlaySounds(_T("IDWAV_GameOver"));
		if(IDYES ==MessageBox(m_hWnd, _T("Time is up!"), _T("Play Again?"), MB_YESNO))
			NewGame();
		else
			PostQuitMessage(0);
		break;
	case WM_NO_MOVES:
		AnimateEndGame();
		PlaySounds(_T("IDWAV_GameOver"));
		if(IDYES ==MessageBox(m_hWnd, _T("Game Over"), _T("Play Again?"), MB_YESNO))
			NewGame();
		else
			PostQuitMessage(0);
		break;
	case WM_40LINES_CLEARED:
		PlaySounds(_T("IDWAV_LevelUp"));
		if(IDYES ==MessageBox(m_hWnd, _T("40 Lines Cleared"), _T("Play Again?"), MB_YESNO))
			NewGame();
		else
			PostQuitMessage(0);
		break;
	case WM_PIECE_PLACED:
		PlaySounds(_T("IDWAV_Down"));
		break;
	case WM_NEW_HISCORE:
		//PlaySounds(_T("IDWAV_NewHighScore"));
		SetTimer(m_hWnd, IDT_TIMER_HISCORE, 1000, NULL);
		break;
	case WM_PIECE_SWITCHED:
		PlaySounds(_T("IDWAV_Switch"));
		InvalidateRect(m_hWnd, NULL, FALSE);
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
	if(IDYES == MessageBox(m_hWnd, _T("Are you sure you want to quit?"), _T("Exit Hexaminoes"), MB_YESNO))
		PostQuitMessage(0);
	return TRUE;
}

BOOL CDlgHextris::OnMenuRight()
{
	//Create the menu.
	HMENU hMenu			= CreatePopupMenu();

	if(!hMenu)
		return FALSE;

	TCHAR szText[STRING_MAX];

	m_oStr->StringCopy(szText, _T("Restart"));
	AppendMenu(hMenu, MF_STRING, IDMENU_Restart, szText);
	m_oStr->StringCopy(szText, _T("New Game (Classic)"));
	AppendMenu(hMenu, MF_STRING, IDMENU_GameClassic, szText);
	m_oStr->StringCopy(szText, _T("New Game (Challenge)"));
	AppendMenu(hMenu, MF_STRING, IDMENU_GameChallenge, szText);
	m_oStr->StringCopy(szText, _T("New Game (40 Lines)"));
	AppendMenu(hMenu, MF_STRING, IDMENU_Game40, szText);
	m_oStr->StringCopy(szText, _T("New Game (Timed)"));
	AppendMenu(hMenu, MF_STRING, IDMENU_GameUltra, szText);
	m_oStr->StringCopy(szText, _T("Pause"));
	AppendMenu(hMenu, MF_STRING, IDMENU_Pause, szText);
	m_oStr->StringCopy(szText, _T("HiScores"));
	AppendMenu(hMenu, MF_STRING, IDMENU_HiScores, szText);


	POINT pt;
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	pt.x	= rc.right - GetSystemMetrics(SM_CXVSCROLL);
	pt.y	= GetSystemMetrics(SM_CYSCREEN) - rc.top;


	//Display it.
	TrackPopupMenu(	hMenu, 
		TPM_RIGHTALIGN|TPM_BOTTOMALIGN, 
		pt.x,pt.y, 
		0, 
		m_hWnd, NULL);

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


BOOL CDlgHextris::OnLButtonDown(HWND hWnd, POINT& pt)
{
	if(pt.x < GetSystemMetrics(SM_CXICON))
		m_oGame.OnLeft();
	else if(pt.x > 11*m_iHexSize)
		m_oGame.OnSwitchPiece();
	else if(pt.x > 9*m_iHexSize)
		m_oGame.OnRight();
	else
		m_oGame.OnRotate();

	return TRUE;
}


BOOL CDlgHextris::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(m_bPause)
		return FALSE;

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
			PlaySounds(_T("IDWAV_Move"));
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
/*
void CDlgHextris::CreateGDIHex(HDC hdc)
{
	if(!m_gdiHex.GetDC())
	{
		//11 colors for now
		m_gdiHex.Create(hdc, NUM_HEX_COLORS*m_sizeHex.cx, m_sizeHex.cy, FALSE, TRUE, TRUE);

		//							Grad Top	/	Grad Bottom		/	Grad Outline							  


		CIssGDI gdiTemp;
		CIssGDI gdiTempHex;
		POINT	pt;
		pt.x = m_iHexSize/2;
		pt.y = 0;
		RECT rcTemp = {0, 0, m_sizeHex.cx, m_sizeHex.cy};
		gdiTemp.Create(hdc, rcTemp, FALSE, TRUE);
		gdiTempHex.Create(hdc, rcTemp, FALSE, TRUE, TRUE);

		for(int i = 0 ; i < NUM_HEX_COLORS; i++)
		{
			CIssGDI::GradriantFillRect(gdiTemp, rcTemp, cores[i][0], cores[i][1]);

			CIssGDI::FillRect(gdiTempHex.GetDC(), rcTemp, TRANSPARENT_COLOR);
			DrawHexagon(gdiTempHex.GetDC(), pt, 0, 0, m_iHexSize);

			COLORREF crDest;

			//copy shiny bits to hex
			// loop through all the bits and change it to the proper color
			for (int y=0; y < HEIGHT(rcTemp); y++) 
			{
				for (int x=0; x < m_sizeHex.cx; x++) 
				{
					crDest = GetPixel(gdiTempHex.GetDC(), x, y);

					if(crDest == 0buck
						)
					{
						SetPixel(gdiTempHex.GetDC(), x, y, GetPixel(gdiTemp.GetDC(), x, y));
					}
				}
			}		

			//draw a new hex outline
			DrawHexagon(gdiTempHex.GetDC(), pt, cores[i][2], 0xFF00FF, m_iHexSize);

		//	DrawHexagon(gdiTempHex.GetDC(), pt, 0x00CC00, 0x00FF00, m_iHexSize);

			BitBlt(m_gdiHex.GetDC(), i*m_sizeHex.cx, 0, m_sizeHex.cx, m_sizeHex.cy, 
				gdiTempHex.GetDC(), 0, 0, SRCCOPY);


		}
	}
}*/

void CDlgHextris::CreateGDIHex(HDC hdc)
{
	if(!m_gdiHex.GetDC())
	{
		CIssGDI gdiTempHex;
		CIssGDI gdiTemp;
		RECT rcTemp = {0, 0, m_sizeHex.cx, m_sizeHex.cy};
		gdiTempHex.Create(hdc, rcTemp, FALSE, TRUE, TRUE, 24);
		gdiTemp.Create(hdc, rcTemp, FALSE, TRUE, TRUE, 24);
		CIssGDI::FillRect(gdiTempHex.GetDC(), rcTemp, 0xF800F8);

		rcTemp.right *= NUM_HEX_COLORS;
		m_gdiHex.Create(hdc, NUM_HEX_COLORS*m_sizeHex.cx, m_sizeHex.cy, FALSE, TRUE, TRUE, 24);

		CIssGDI::FillRect(m_gdiHex.GetDC(), rcTemp, 0xF800F8);

		POINT	pt;
		pt.x = m_iHexSize/2;
		pt.y = 0;

		rcTemp.right /= NUM_HEX_COLORS;

		COLORREF crTempColor;

		for(int i = 0 ; i < NUM_HEX_COLORS; i++)
		{
			DrawHexagon(gdiTempHex.GetDC(), pt, 0, 0, m_iHexSize);
			CIssGDI::GradriantFillRect(gdiTemp.GetDC(), rcTemp, cores[i][0], cores[i][1], TRUE);
		//	CIssGDI::FillRect(gdiTemp, rcTemp, cores[i][0]);

			// loop through all the bits and change it to the proper color
			for (int y=0; y < HEIGHT(rcTemp); y++) 
			{
				for (int x=0; x < m_sizeHex.cx; x++) 
				{
					crTempColor = gdiTempHex.GetPixelColor(x, y);

					if(crTempColor == 0)
					{
						gdiTempHex.SetPixelColor(x, y, gdiTemp.GetPixelColor(x,y));
					}
				}
			}		

			//draw a new hex outline
			DrawHexagon(gdiTempHex.GetDC(), pt, cores[i][2], 0xFF00FF, m_iHexSize);

			BitBlt(m_gdiHex.GetDC(), i*m_sizeHex.cx, 0, m_sizeHex.cx, m_sizeHex.cy, 
				gdiTempHex.GetDC(), 0, 0, SRCCOPY);


		}




	}



}
/*

void CDlgHextris::CreateGDIHex(HDC hdc)
{
	if(!m_gdiHex.GetDC())
	{
		//11 colors for now
		m_gdiHex.Create(hdc, NUM_HEX_COLORS*m_sizeHex.cx, m_sizeHex.cy, FALSE, TRUE, TRUE);
		
		CIssGDI gdiTemp;
		CIssGDI gdiTempHex;
		CIssGDI gdiTempImage;
		POINT	pt;
		pt.x = m_iHexSize/2;
		pt.y = 0;
		RECT rcTemp = {0, 0, m_sizeHex.cx, m_sizeHex.cy};
		
		gdiTempHex.Create(hdc, rcTemp, FALSE, TRUE, TRUE);

		rcTemp.right *= NUM_HEX_COLORS;
		gdiTemp.Create(hdc, rcTemp, FALSE, TRUE);
		rcTemp.right /= NUM_HEX_COLORS;//back to a single

		SIZE sizeTemp = m_sizeHex;
		sizeTemp.cx *= NUM_HEX_COLORS;

		gdiTempImage.Create(hdc, sizeTemp.cx, sizeTemp.cy, FALSE, TRUE, FALSE, 16);
		CIssGDI::FillRect(gdiTempImage.GetDC(), rcTemp, 0xFFFFFF);


		COLORREF crTempColor;
		COLORREF crOutline;

		for(int i = 0 ; i < NUM_HEX_COLORS; i++)
		{
			crOutline = gdiTemp.GetPixelColor(i*m_sizeHex.cx, 0);
			CIssGDI::FillRect(gdiTempHex.GetDC(), rcTemp, TRANSPARENT_COLOR);
			DrawHexagon(gdiTempHex.GetDC(), pt, 0, 0, m_iHexSize);

			// loop through all the bits and change it to the proper color
			for (int y=0; y < HEIGHT(rcTemp); y++) 
			{
				for (int x=0; x < m_sizeHex.cx; x++) 
				{
					//crTempColor = GetPixel(gdiTempHex.GetDC(), x, y);
					crTempColor = gdiTempHex.GetPixelColor(x, y);

					if(crTempColor == 0)
					{
						gdiTempHex.SetPixelColor(x, y, gdiTemp.GetPixelColor(x+i*m_sizeHex.cx,y));
						//SetPixel(gdiTempHex.GetDC(), x, y, GetPixel(gdiTemp.GetDC(), x, y));
					}
				}
			}		

			//draw a new hex outline
			DrawHexagon(gdiTempHex.GetDC(), pt, cores[i][2], 0xFF00FF, m_iHexSize);
			//DrawHexagon(gdiTempHex.GetDC(), pt, crOutline, 0xFF00FF, m_iHexSize);


			BitBlt(m_gdiHex.GetDC(), i*m_sizeHex.cx, 0, m_sizeHex.cx, m_sizeHex.cy, 
				gdiTempHex.GetDC(), 0, 0, SRCCOPY);


		}
	}
}*/

BOOL CDlgHextris::DrawHexagon(HDC hdc, POINT ptLocation, int iColorIndex)
{
	if(m_gdiHex.GetDC() == NULL)
		CreateGDIHex(hdc);


	return	TransparentBlt(hdc, ptLocation.x - m_iHexSize/2, ptLocation.y, 
		m_sizeHex.cx, m_sizeHex.cy,
		m_gdiHex.GetDC(), iColorIndex*m_sizeHex.cx, 0, m_sizeHex.cx, m_sizeHex.cy, TRANSPARENT_COLOR);

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

		DeleteObject(hPen);
		DeleteObject(hPen);
	}

	return TRUE;
}

void CDlgHextris::NewGame()
{
	SetTimer(m_hWnd, IDT_IDLE_TIMER, m_oGame.GetLevelTiming(), NULL);//start the timer
	m_oGame.NewGame();
	m_bPause = FALSE;
	InvalidateRect(m_hWnd, NULL, FALSE);
}

void CDlgHextris::AnimateLineCleared()
{
	//lets just flash between the two states for now	
	HDC dc = GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	for(int i = 0; i < 8; i++)
	{
		for(int x = 0; x < GAME_WIDTH; x++)
		{
			for(int y = 0; y < m_oGame.GetNumRows(); y++)
			{
				if(m_oGame.GetPreDropHex(x,y)->ePiece == HEX_Erased)
				{
					if(i%2)
						DrawHexagon(m_gdiMem.GetDC(), m_ptGameField[x][y], OUTLINE_COLOR, RGB(200,222,255), m_iHexSize);
					else
						DrawHexagon(m_gdiMem.GetDC(), m_ptGameField[x][y], OUTLINE_COLOR, RGB(255,255,255), m_iHexSize);
				}
			}
		}
		BitBlt(dc, rcClient.left, 
			rcClient.top, 
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			m_gdiMem.GetDC(), 
			rcClient.left, rcClient.top,
			SRCCOPY);

		Sleep(50);
	}
	ReleaseDC(m_hWnd, dc);
}

void CDlgHextris::AnimateEndGame()
{
	HDC dc = GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	//	for(int i = 0; i <= iHeight; i++)
	for(int i = m_oGame.GetNumRows() - 1; i > -1; i--)
	{
		//animate up from the bottom
		for(int x = 0; x < GAME_WIDTH; x++)
		{
			DrawHexagon(m_gdiMem.GetDC(), m_ptGameField[x][i], GetBackgroundColor(BGCOLOR_Grad1), GetBackgroundColor(BGCOLOR_Grid), m_iHexSize);
		}

		BitBlt(dc, rcClient.left, 
			rcClient.top, 
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			m_gdiMem.GetDC(), 
			rcClient.left, rcClient.top,
			SRCCOPY);

		Sleep(200);
	}

	ReleaseDC(m_hWnd, dc);

}

void CDlgHextris::AnimateLevelChangeChallenge()
{
	return;


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
		return 0;

	if(eColor == BGCOLOR_Grid)
	{
		return BACKGROUND_OUTLINECOLOR;

		switch(m_oGame.GetLevel()%20)
		{
		case 0:
		case 1:
			return 0x4A5B32;
			break;
		case 2:
		case 3:
			return 0x484F04;
			break;
		case 4:
		case 5://red
			return 0x190751;
			break;
		case 6:
		case 7://orange
			return 0x2F3366;
			break;
		case 8:
		case 9://yellow
			return 0x41536F;
			break;
		case 10:
		case 11://green
			return 0x185737;
			break;
		case 12:
		case 13://gray
			return 0x4E4E4E;
			break;
		case 14:
		case 15://purple
			return 0x4D2038;
			break;
		case 16:
		case 17://dark blue
			return 0x5E4423;
			break;
		case 18:
		case 19://light grey
			return 0xAFAFAF;
		default:
		    break;
		}
	}

	else if(eColor == BGCOLOR_Grad1)
	{
		switch(m_oGame.GetLevel()%20)
		{
		case 0://aqua
		case 1:
			return 0xFFF603;
			break;
		case 2://blue
		case 3:
			return 0xFF8900;
			break;
		case 4://red
		case 5:
			return 0x0422FE;
			break;
		case 6:
		case 7://orange
			return 0x0387FD;
			break;
		case 8:
		case 9:
			return 0x04E5FD;
			break;
		case 10://green
		case 11:
			return 0x04FF35;
			break;
		case 12:
		case 13://gray
			return 0x7F7F7F;
			break;
		case 14:
		case 15://purple
			return 0xFF099D;
			break;
		case 16:
		case 17://dark blue
			return 0xFC3203;
			break;
		case 18:
		case 19://light grey
		default:
			return 0xD6D6D6;
			break;
		}
	}

	else if(eColor == BGCOLOR_Grad2)
	{
		switch(m_oGame.GetLevel()%20)
		{
		case 0:
		case 1:
			return 0x00001D;
			break;
		case 2:
		case 3:
			return 0x001213;
			break;
		case 4:
		case 5://red
			return 0x0B0017;
			break;
		case 6:
		case 7://orange
			return 0x00001D;
			break;
		case 8:
		case 9://yellow
			return 0x000918;
			break;
		case 10:
		case 11://green
			return 0x010F0B;
			break;
		case 12:
		case 13://grey
			return 0x141414;
			break;
		case 14:
		case 15://purple
			return 0x1B0003;
			break;
		case 16:
		case 17://dark blue
			return 0x241703;
			break;
		case 18:
		case 19:
		default:
			return 0x666666;
			break;
		}
	}

	return 0xFFFFFF;//should never get here


}