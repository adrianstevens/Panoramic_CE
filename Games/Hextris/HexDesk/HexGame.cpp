#include "HexGame.h"
#include "IssRegistry.h"

/*	Classic Tetris Scoring
	10 lines per level 
	40 points per line on level 0, and this increases by 40 per level
	100 points for 2 lines
	300 points for 3 lines
	1200 points for 4 lines
*/

/* ADD ACHEIVEMENTS
clear 1 line
clear 2 lines
clear 3 lines
clear 4 lines
clear 5 lines
clear 6 lines
clear 7 lines
clear 8 lines

Reach Level 5
Reach Level 10
Reach Level 15
Reach Level 20

*/

#define START_X_OFFSET 3


#define LEVEL_BONUS			1000
#define POINTS_PER_LINE		  50


#define CHALLENGE_TIME		180//in seconds
#define BASE_DROP_TIME		800	//level 0 timing
#define DROP_CHANGE			65		//decrease per level

#define REG_KEY		_T("SOFTWARE\\iSS\\Hextris")


CHexGame::CHexGame(void)
:m_iRows(MAX_GAME_HEIGHT)
,m_eState(GS_GameOver)
,m_iStartingLevel(0)
,m_hWndDlg(NULL)
{
	// seed the seed of randomness
	srand( GetTickCount() );

	m_eGameType = GT_Classic;

	m_oSavedPiece	= new CurrentPiece;
	m_oGhost		= new CurrentPiece;
	
	for(int i = 0; i < NUM_PIECE_PREVIEWS; i++)
	{
		m_oCurPiece[i] = new CurrentPiece;
		m_oCurPiece[i]->Reset();//get a new piece
		m_oCurPiece[i]->SetPosition(START_X_OFFSET, 20);//we'll start em a little off screen
	}	
	Reset();

	for(int i = 0; i < (int)GT_NumberOfTypes; i++)
		m_iHighScore[i] = 0;//clear em out

	LoadRegistry();

}

CHexGame::~CHexGame(void)
{
	for(int i = 0; i < NUM_PIECE_PREVIEWS; i++)
		delete m_oCurPiece[i];

	SaveRegistry();
}

void CHexGame::LoadRegistry()
{
	DWORD dwTemp;

	TCHAR szTemp[2];
	szTemp[1] = _T('\0');	

	for(int i = 0; i < GT_NumberOfTypes; i++)
	{
		szTemp[0] = _T('1')+ i;	
		dwTemp = m_iHighScore[i];
		if(CIssRegistry::GetKey(REG_KEY, szTemp, dwTemp))
			m_iHighScore[i] = dwTemp;
	}
}

void CHexGame::SaveRegistry()
{
	DWORD dwTemp;

	TCHAR szTemp[2];
	szTemp[1] = _T('\0');	
	for(int i = 0; i < GT_NumberOfTypes; i++)
	{
		szTemp[0] = _T('1')+ i;	
		dwTemp = m_iHighScore[i];
		CIssRegistry::SetKey(REG_KEY, szTemp, dwTemp);
	}
}


void CHexGame::Reset()
{
	m_iScore			= 0;
	m_iLevel			= m_iStartingLevel;
	m_iRowsCleared		= 0;
	m_iTime				= 0;
	m_bNewHighScore		= TRUE;

	m_oSavedPiece->Reset(0);//always the line

/*#ifdef DEBUG
	m_iLevel = 7;
	m_iRowsCleared = 80;
#endif*/

	for(int i = 0; i < NUMBER_OF_PIECES; i++)
		m_iStats[i] = 0;

	ClearGameBoard();

	for(int k = 0; k < NUM_PIECE_PREVIEWS; k++)
		m_oCurPiece[k]->Reset();

	NewPiece();

	CalcGhostLocation();

	//conditions here depending on game type
}

void CHexGame::SetNumRows(int iRows)
{
	m_iRows = iRows;
	
	for(int i = 1; i < NUM_PIECE_PREVIEWS; i++)
	{
		if(m_oCurPiece[i] != NULL)
			m_oCurPiece[i]->SetPosition(START_X_OFFSET, m_iRows+2);//we'll start em a little off screen
	}


};//from the interface

void CHexGame::SetPattern()
{
	//5 lines of checkered





}


//switches the current piece with the saved piece
void CHexGame::OnSwitchPiece()
{
	CurrentPiece* oPiece;
	oPiece = m_oCurPiece[0];
	m_oCurPiece[0] = m_oSavedPiece;
	m_oSavedPiece = oPiece;

	m_oCurPiece[0]->SetPosition(m_oSavedPiece->GetLocation()->x, m_oSavedPiece->GetLocation()->y);
	if(CheckCollision(m_oCurPiece[0]))
	{
		m_oCurPiece[0]->MovePieceRight();
		if(CheckCollision(m_oCurPiece[0]))
		{
			m_oCurPiece[0]->MovePieceLeft();
			m_oCurPiece[0]->MovePieceLeft();
			if(CheckCollision(m_oCurPiece[0]))
			{	//switch it back
				oPiece = m_oCurPiece[0];
				m_oCurPiece[0] = m_oSavedPiece;
				m_oSavedPiece = oPiece;
				return;
			}
		}
	}

	PostMessage(m_hWndDlg, WM_PIECE_SWITCHED, 0, 0);

	//restores the alignment
	m_oSavedPiece->Reset(m_oSavedPiece->GetPieceType());

	CalcGhostLocation();

}


void CHexGame::ClearGameBoard()
{
	for(int i = 0; i < GAME_WIDTH; i++)
	{
		for(int j = 0; j < MAX_GAME_HEIGHT; j++)
		{
			m_oGameField[i][j].ePiece = HEX_Blank;
		}
	}
}

void CHexGame::AddGarageBlocks()
{
#define min(a,b) (((a) < (b)) ? (a) : (b)) 

	for(int i = 0; i < GAME_WIDTH; i++)
	{
		for(int j = 0; j < min(m_iLevel+1, m_iRows - 2); j++)
		{
			if(rand()%3 == 0)
			{
				m_oGameField[i][j].ePiece = HEX_GamePiece;
				m_oGameField[i][j].iColorIndex = 0;
			}
		}
	}
}

void CHexGame::NewPiece()
{
	static CurrentPiece* oCur = NULL;
	oCur = m_oCurPiece[0];
	oCur->Reset();//get a new piece
	oCur->SetPosition(START_X_OFFSET, m_iRows+2);//we'll start em a little off screen

	for(int i = 0; i < NUM_PIECE_PREVIEWS - 1; i++)
		m_oCurPiece[i] = m_oCurPiece[i+1];

	m_oCurPiece[NUM_PIECE_PREVIEWS - 1] = oCur;

//	oCur = m_oCurPiece[0];
//	oCur->Reset();//get a new piece
//	oCur->SetPosition(3, m_iRows+2);//we'll start em a little off screen

	m_iStats[oCur->GetPieceType()]++;
}

void CHexGame::NewGame()
{
	Reset();

	switch(m_eGameType)
	{
	case GT_Classic:
		break;
	case GT_Challenge:
		AddGarageBlocks();//based on current level
		break;
	case GT_Patterns:
		SetPattern();
		break;
	case GT_Ultra:
		m_iTime = CHALLENGE_TIME;//we'll count backwards for this gamemode
		break;
	case GT_40:		
		break;
	default:
		ASSERT(0);
	    break;
	}
	m_eState = GS_Ingame;

}




void CHexGame::OnTimer()
{
	if(m_eState == GS_Ingame)
	{
		m_oCurPiece[0]->MovePieceDown();
		if(CheckCollision(m_oCurPiece[0]))
		{
			//move it back
			m_oCurPiece[0]->MovePieceUp();
			//lock it in place
			SetCurPieceToBoard();
			//get a new piece
			NewPiece();
			//check for complete lines
			CheckForCompleteLines();
			//new ghost
			CalcGhostLocation();

		}
	}
}

BOOL CHexGame::SetGameType(EnumGameType eGameType)
{
	if(m_eGameType != eGameType)
	{
		m_eGameType = eGameType;
		NewGame();
	}
	return TRUE;
}

void CHexGame::SetCurPieceToBoard()
{
	PostMessage(m_hWndDlg, WM_PIECE_PLACED, 0, 0);//for the sounds

	int iYOffset = 0;

	for(int i = 0; i < 5; i++)
	{
		if((i+m_oCurPiece[0]->GetLocation()->x)%2==0)
			iYOffset++;

		for(int j = 0; j < 5; j++)
		{
			if(m_oCurPiece[0]->GetHex(i,j)->ePiece==HEX_GamePiece)
			{
				int x2 = m_oCurPiece[0]->GetLocation()->x + i;
				int y2 = m_oCurPiece[0]->GetLocation()->y - j - iYOffset;			

				m_oGameField[x2][y2].ePiece = HEX_GamePiece;
				m_oGameField[x2][y2].iColorIndex = m_oCurPiece[0]->GetPieceType();
			}
		}
	}
	CheckForEndGameState();
}

void CHexGame::CheckForEndGameState()
{
	for(int i = 0; i < GAME_WIDTH; i++)
	{
		if(m_oGameField[i][m_iRows-1].ePiece == HEX_GamePiece)
		{	//game over
			m_eState = GS_GameOver;
			PostMessage(m_hWndDlg, WM_NO_MOVES, 0, 0);
			return;
		}
	}
}

BOOL CHexGame::CheckCollision(CurrentPiece* oPiece)
{
	if(oPiece == NULL)
		return FALSE;

	int iYOffset = 0;//I'm not happy about this either ....
	for(int x=0;x<5; x++)
	{
		if((x+oPiece->GetLocation()->x)%2==0)
			iYOffset++;

		for(int y=0;y<5; y++)
		{
			if(oPiece->GetHex(x,y)->ePiece == HEX_GamePiece)
			{
				int x2 = oPiece->GetLocation()->x + x;
				int y2 = oPiece->GetLocation()->y - y - iYOffset;

				if(y2 >= MAX_GAME_HEIGHT)
					continue;
				if(x2 < 0 || x2 >= GAME_WIDTH || y2 < 0)
					return TRUE;
				if(m_oGameField[x2][y2].ePiece != HEX_Blank)
					return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CHexGame::OnRight()
{
	m_oCurPiece[0]->MovePieceRight();
	if(CheckCollision(m_oCurPiece[0]))
	{
		m_oCurPiece[0]->MovePieceLeft();
		return FALSE;
	}

	CalcGhostLocation();
	return TRUE;
}

BOOL CHexGame::OnLeft()
{
	m_oCurPiece[0]->MovePieceLeft();
	if(CheckCollision(m_oCurPiece[0]))
	{
		m_oCurPiece[0]->MovePieceRight();
		return FALSE;
	}

	CalcGhostLocation();
	return TRUE;
}

void CHexGame::OnUp()
{
	m_oCurPiece[0]->MovePieceUp();
}

BOOL CHexGame::OnDown()
{
	m_oCurPiece[0]->MovePieceDown();
	if(CheckCollision(m_oCurPiece[0]))
	{
		m_oCurPiece[0]->MovePieceUp();
		return FALSE;
	}
	return TRUE;
}

BOOL CHexGame::OnRotate()
{
	//save the state
	CurrentPiece oCur = *m_oCurPiece[0];

	m_oCurPiece[0]->Rotate();

	if(CheckCollision(m_oCurPiece[0]))
	{
		m_oCurPiece[0]->MovePieceLeft();
		if(CheckCollision(m_oCurPiece[0]))
		{
			m_oCurPiece[0]->MovePieceRight();
			m_oCurPiece[0]->MovePieceRight();
			if(CheckCollision(m_oCurPiece[0]))
			{	//give it up ...
				*m_oCurPiece[0] = oCur;
				return FALSE;
			}
		}
	}
	CalcGhostLocation();
	return TRUE;
}

void CHexGame::OnDrop()
{
	m_oCurPiece[0]->CopyPiece(m_oGhost);
}


//we have to check for 2 types

void CHexGame::CheckForCompleteLines()
{
	m_iClearedLines = 0;

	BOOL bCompLine;
	BOOL bCompAlt;

	int iOffSet = 0;

	for(int y = 0; y < m_iRows; y++)
	{
		bCompLine = TRUE;

		for(int x = 0; x < GAME_WIDTH; x++)
		{
			if(m_oGameField[x][y].ePiece != HEX_GamePiece)
			{
				bCompLine = FALSE;
			}
		}

		if(bCompLine)
		{
			m_sClearedLines[m_iClearedLines].bAlt = false;
			m_sClearedLines[m_iClearedLines].btRow = y;
			m_iClearedLines++;
		}

		bCompAlt = TRUE;

		for(int x = 0; x < GAME_WIDTH; x++)
		{
			if(m_oGameField[x][y+x%2].ePiece != HEX_GamePiece)
			{
				bCompAlt = FALSE;
			}
		}

		if(bCompAlt)
		{
			m_sClearedLines[m_iClearedLines].bAlt = true;
			m_sClearedLines[m_iClearedLines].btRow = y;
			m_iClearedLines++;
		}
	}

	if(m_iClearedLines > 0)
	{
		//now we set our alt structures
		memcpy(m_oPrevField, m_oGameField, sizeof(GameHexagon)*GAME_WIDTH*MAX_GAME_HEIGHT);

		int iOffset = 0;

		for(int i = 0; i < m_iClearedLines; i++)
		{
			
			//now we'll set the hexes to cleared
			SetLineToCleared(m_sClearedLines[i].btRow, m_sClearedLines[i].bAlt);
			memcpy(m_oPreDropField, m_oGameField, sizeof(GameHexagon)*GAME_WIDTH*MAX_GAME_HEIGHT);
		}
		//and finally, copy and move everything down
		DropPieces();

		PostMessage(m_hWndDlg, WM_LINE_CLEARED, m_iClearedLines, 0);
	}
}

void CHexGame::DropPieces()
{
	//to make this really easy we're going to do this one column at a time
	for(int i = 0; i < GAME_WIDTH; i++)
	{
		//for(int j = 0; j < m_iRows-1; j++)
		for(int j = m_iRows-1; j > -1; j--)
		{
			if(m_oGameField[i][j].ePiece == HEX_Erased)
			{	
				for(int k = j; k < m_iRows-2; k++)
				{
					m_oGameField[i][k] = m_oGameField[i][k+1];
				}
				//and no matter what, we clear the top
				m_oGameField[i][m_iRows-1].ePiece = HEX_Blank;
			}
		}
	}
}


void CHexGame::SetLineToCleared(int iY, BOOL bAlt)
{
	int iYLow = iY;

	//to make this really easy we're going to do this one column at a time
	for(int i = 0; i < GAME_WIDTH; i++)
	{
		if(bAlt)
			iYLow = iY + i%2;

		m_oGameField[i][iYLow].ePiece = HEX_Erased;
	}
	ScoreClearedLine();
}

//add extra points for back to back clears??? yeah I think so .... 
void CHexGame::ScoreClearedLine()
{
	m_iRowsCleared++;

	int iNewLevel = m_iRowsCleared/10;

	if(iNewLevel!=m_iLevel)
	{
		m_iLevel = iNewLevel;
		m_iScore += m_iLevel*LEVEL_BONUS;

		if(m_eGameType == GT_Challenge)
		{
			memcpy(m_oPrevField, m_oGameField, sizeof(GameHexagon)*GAME_WIDTH*MAX_GAME_HEIGHT);//save the field
			ClearGameBoard();
			AddGarageBlocks();
		}
		PostMessage(m_hWndDlg, WM_LEVEL_CHANGE, m_iLevel, GetLevelTiming());
	}

	m_iScore += POINTS_PER_LINE + m_iLevel*POINTS_PER_LINE;

	if(m_iScore > m_iHighScore[m_eGameType])//save the correct high score
	{
		m_iHighScore[m_eGameType] = m_iScore;
		if(m_bNewHighScore)
		{
			PostMessage(m_hWndDlg, WM_NEW_HISCORE, 0, 0);
			m_bNewHighScore = FALSE;//only play the sound once yo
		}
	}

	//for Clear 40 Lines GameType
	if(m_eGameType == GT_40 && m_iRowsCleared >= 40)
	{
		PostMessage(m_hWndDlg, WM_40LINES_CLEARED, 0, 0);
		m_eState = GS_GameOver;
	}

}

void CHexGame::OnClock()
{
	if(m_eGameType == GT_Ultra)
	{
		m_iTime--; 
		if(m_iTime <= 0)
		{	//time's up!
			PostMessage(m_hWndDlg, WM_TIME_UP, 0, 0);
			m_eState = GS_GameOver;
		}
	}
	else
		m_iTime++;



}

int CHexGame::GetLevelTiming()
{
	int iRet = BASE_DROP_TIME - m_iLevel*DROP_CHANGE;

	if(iRet < DROP_CHANGE)
		iRet = (20-m_iLevel)*5;

	if(iRet < 0)
		iRet = 0;

	return iRet;
}

void CHexGame::CalcGhostLocation()
{
	m_oGhost->CopyPiece(m_oCurPiece[0]);

	if(CheckCollision(m_oGhost))
		return;

	m_oGhost->MovePieceDown();

	while(CheckCollision(m_oGhost) == FALSE)
		m_oGhost->MovePieceDown();

	m_oGhost->MovePieceUp();

}