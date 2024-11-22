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

/*

ORB Rules:
7 lines or more: save ORB
4 lines or more: insert 
level clear: insert
Every 5 levels: save an orb

*/

#define MIN_ROWS_BONUS_QUE	3
#define MIN_ROWS_BONUS_SIDE 6

#define START_X_OFFSET 3





#define CHALLENGE_TIME		180//in seconds
#define BASE_DROP_TIME		800	//level 0 timing
#define DROP_CHANGE			65		//decrease per level

#define FOUR_ORB_NUM_EXP	MAX_SAVED_GEMS

#define REG_KEY				_T("SOFTWARE\\Pano\\HexeGems")
#define REG_Music			_T("Music")
#define REG_SFX				_T("SFX")
#define REG_FPS				_T("FPS")
#define REG_GameType		_T("GT")
#define REG_Startlevel		_T("SL")
#define REG_PowerSaving		_T("PS")

CHexGame::CHexGame(void)
:m_iRows(MAX_GAME_HEIGHT)
,m_eState(GS_GameOver)
,m_iStartingLevel(0)
,m_iBonusQue(0)
,m_iSavedGems(0)
,m_hWndDlg(NULL)
,m_bShowFPS(FALSE)
,m_iMusicLevel(64)
,m_iSFXLevel(64)
,m_eGameType(GT_Classic)
,m_bPowerSaving(FALSE)
{
	// seed the seed of randomness
	srand( GetTickCount() );

	m_oSavedPiece	= new CurrentPiece;
	m_oGhost		= new CurrentPiece;
	m_oStr			= CIssString::Instance();
	
	for(int i = 0; i < NUM_PIECE_PREVIEWS; i++)
	{
		m_oCurPiece[i] = new CurrentPiece;
		m_oCurPiece[i]->Reset();//get a new piece
		m_oCurPiece[i]->SetPosition(START_X_OFFSET, 20);//we'll start em a little off screen
	}	
	Reset();
}

CHexGame::~CHexGame(void)
{
	for(int i = 0; i < NUM_PIECE_PREVIEWS; i++)
		delete m_oCurPiece[i];

	if(m_oSavedPiece)
		delete m_oSavedPiece;
	if(m_oGhost)
		delete m_oGhost;
	m_oSavedPiece = NULL;
	m_oGhost = NULL;
}

void CHexGame::Reset()
{
	m_iScore			= 0;
	m_iLevel			= m_iStartingLevel;
	m_dbLevelPercent	= 0.0;
	m_iRowsCleared		= 0;
	m_iTime				= 0;
	m_iBonusQue			= 0;
	m_eState			= GS_GameOver;
	m_iSavedGems		= 0;
	m_bNewHighScore		= FALSE;

	m_oSavedPiece->Reset(0);//always the line

/*#ifdef DEBUG
	m_iLevel = 7;
	m_iRowsCleared = 80;
#endif*/

	for(int i = 0; i < NUMBER_OF_PIECES; i++)
		m_iStats[i] = 0;

	ClearGameBoard();

	for(int k = 0; k < NUM_PIECE_PREVIEWS; k++)
		m_oCurPiece[k]->Reset(-1, FALSE);

	NewPiece();

	CalcGhostLocation();

	//conditions here depending on game type
}

void CHexGame::LoadRegistry()
{
	TCHAR szTemp[STRING_MAX];
	TCHAR szNum[STRING_SMALL];

	for(int i = 0; i < (int)GT_NumberOfTypes; i++)
	{
		m_oHighScores[i].Init(NUM_HIGHSCORES);
		//we'll just append a number to the reg key....doesn't need to readable
		m_oStr->StringCopy(szTemp, REG_KEY);
		m_oStr->IntToString(szNum, i);
		m_oStr->Concatenate(szTemp, _T("\\"));
		m_oStr->Concatenate(szTemp, szNum);
		
		m_oHighScores[i].LoadRegistry(szTemp);
	}

	DWORD dwValue;
	if(CIssRegistry::GetKey(REG_KEY, REG_Music, dwValue))
		m_iMusicLevel = (int)dwValue;

	if(CIssRegistry::GetKey(REG_KEY, REG_SFX, dwValue))
		m_iSFXLevel = (int)dwValue;

	if(CIssRegistry::GetKey(REG_KEY, REG_FPS, dwValue))
		m_bShowFPS = (BOOL)dwValue;

	if(CIssRegistry::GetKey(REG_KEY, REG_PowerSaving, dwValue))
		m_bPowerSaving = (BOOL)dwValue;

	if(CIssRegistry::GetKey(REG_KEY, REG_GameType, dwValue))
		m_eGameType = (EnumGameType)dwValue;

	if(CIssRegistry::GetKey(REG_KEY, REG_Startlevel, dwValue))
		m_iStartingLevel = (int)dwValue;
}

void CHexGame::SaveRegistry()
{
	TCHAR szTemp[STRING_MAX];
	TCHAR szNum[STRING_SMALL];

	for(int i = 0; i < (int)GT_NumberOfTypes; i++)
	{
		m_oStr->StringCopy(szTemp, REG_KEY);
		m_oStr->Concatenate(szTemp, _T("\\"));
		m_oStr->IntToString(szNum, i);
		m_oStr->Concatenate(szTemp, szNum);
		m_oHighScores[i].SaveRegistry(szTemp);
	}

	DWORD dwValue;

	dwValue = (DWORD)m_iMusicLevel;
	CIssRegistry::SetKey(REG_KEY, REG_Music, dwValue);

	dwValue = (DWORD)m_iSFXLevel;
	CIssRegistry::SetKey(REG_KEY, REG_SFX, dwValue);

	dwValue = (DWORD)m_bShowFPS;
	CIssRegistry::SetKey(REG_KEY, REG_FPS, dwValue);

	dwValue = (DWORD)m_bPowerSaving;
	CIssRegistry::SetKey(REG_KEY, REG_PowerSaving, dwValue);

	dwValue = (DWORD)m_eGameType;
	CIssRegistry::SetKey(REG_KEY, REG_GameType, dwValue);

	dwValue = (DWORD)m_iStartingLevel;
	CIssRegistry::SetKey(REG_KEY, REG_Startlevel, dwValue);
}

void CHexGame::SetNumRows(int iRows)
{
	m_iRows = iRows;
	
	for(int i = 1; i < NUM_PIECE_PREVIEWS; i++)
	{
		if(m_oCurPiece[i] != NULL)
			m_oCurPiece[i]->SetPosition(START_X_OFFSET, m_iRows+3);//we'll start em a little off screen
	}
}//from the interface


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
	m_oSavedPiece->Reset(m_oSavedPiece->GetPieceType(), m_oSavedPiece->HasOrb());

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
				m_oGameField[i][j].ePiece = HEX_GamePiece;
		}
	}
}

void CHexGame::NewPiece()
{
	static CurrentPiece* oCur = NULL;
	oCur = m_oCurPiece[0];

	BOOL bBonus = FALSE;

	//if we have a bonus gem in the que to go out
	if(m_iBonusQue > 0)
	{	
		bBonus = TRUE;
		m_iBonusQue--;
		
	}

	oCur->Reset(-1, bBonus);//get a new piece
	oCur->SetPosition(START_X_OFFSET, m_iRows+3);//we'll start em a little off screen

	for(int i = 0; i < NUM_PIECE_PREVIEWS - 1; i++)
		m_oCurPiece[i] = m_oCurPiece[i+1];

	m_oCurPiece[NUM_PIECE_PREVIEWS - 1] = oCur;

//	oCur = m_oCurPiece[0];
//	oCur->Reset();//get a new piece
//	oCur->SetPosition(3, m_iRows+3);//we'll start em a little off screen

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
	case GT_Ultra:
		m_iTime = CHALLENGE_TIME;//we'll count backwards for this gamemode
		break;
//	case GT_40:		
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
			
			//check for double explosion
			if(CheckForDoubleOrb(m_ptOrb1, m_ptOrb2))
			{
				//now we set our alt structures
				SetCurPieceToBoard();
				memcpy(m_oPrevField, m_oGameField, sizeof(GameHexagon)*GAME_WIDTH*MAX_GAME_HEIGHT);
				DoubleExplosion(m_ptOrb1);
				DoubleExplosion(m_ptOrb2);
				memcpy(m_oPreDropField, m_oGameField, sizeof(GameHexagon)*GAME_WIDTH*MAX_GAME_HEIGHT);
				DropPieces();
				PostMessage(m_hWndDlg, WM_DOUBLE_ORB, 0, 0);
				AddGemToSaved();//you deserve it big boy
			}
			else
			{   //it needs to be laid out like this
				SetCurPieceToBoard();
			}

			//get a new piece
			NewPiece();
			//check for complete lines
			CheckForCompleteLines();
			//new ghost
			CalcGhostLocation();

			//check for new high score if it isn't already
			//so we can draw our sexy little star - DON'T REMOVE
			if(m_bNewHighScore == FALSE)
			//	&&	m_eGameType != GT_40)
			{
				if(m_iScore > m_oHighScores[m_eGameType].GetHiScore())
				{
					PostMessage(m_hWndDlg, WM_NEW_HISCORE, 0, 0);
					m_bNewHighScore = TRUE;
				}
			}
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
			if(IsGamePiece(m_oCurPiece[0]->GetHex(i,j)->ePiece))
			{
				int x2 = m_oCurPiece[0]->GetLocation()->x + i;
				int y2 = m_oCurPiece[0]->GetLocation()->y - j - iYOffset;			

				m_oGameField[x2][y2].ePiece = m_oCurPiece[0]->GetHex(i,j)->ePiece;//set the piece
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
		if(IsGamePiece(m_oGameField[i][m_iRows-1].ePiece))
		{	//game over
			m_eState = GS_GameOver;
			PostMessage(m_hWndDlg, WM_NO_MOVES, 0, 0);
	
			TypeHighScore sHiScore;
			sHiScore.iLevel = GetLevel();
			sHiScore.eGameType = m_eGameType;//for the high score class

			//and now we have to submit our high score for consideration into the ol high score table
		/*	if(m_eGameType == GT_40)//we're interested in time
			{
				sHiScore.iScore = m_iTime;			
				m_oHighScores[m_eGameType].AddHighScore(m_iTime, NULL, &sHiScore, sizeof(TypeHighScore));
			}
			else*/
			if(m_iScore > 0)
			{
				sHiScore.iScore = m_iScore;
				m_oHighScores[m_eGameType].AddHighScore(m_iScore, NULL, &sHiScore, sizeof(TypeHighScore));
			}
			
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
			if(IsGamePiece(oPiece->GetHex(x,y)->ePiece))
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
		m_oCurPiece[0]->MovePieceDown();
		if(CheckCollision(m_oCurPiece[0]))
		{
			m_oCurPiece[0]->MovePieceLeft();
			m_oCurPiece[0]->MovePieceUp();
			return FALSE;
		}
	}

	CalcGhostLocation();
	return TRUE;
}

BOOL CHexGame::OnLeft()
{
	m_oCurPiece[0]->MovePieceLeft();
	if(CheckCollision(m_oCurPiece[0]))
	{
		m_oCurPiece[0]->MovePieceDown();
		if(CheckCollision(m_oCurPiece[0]))
		{
			m_oCurPiece[0]->MovePieceRight();
			m_oCurPiece[0]->MovePieceUp();
			return FALSE;
		}		
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
	m_iLastScoreIncrease = 0;

	BOOL bCompLine;
	BOOL bCompAlt;
	
#define MAX_ORBS 20 //for now ... max orbs in Play
	POINT ptExplodedOrbs[MAX_ORBS];
	POINT ptOrbTemp[GAME_WIDTH];//temp orb location saving
	int iExplodedOrbs = 0; //our nuked orb count 
	int iTempCount = 0; //count orbs in a line before we know if its a complete line
	
	int iOffSet = 0;

	for(int y = 0; y < m_iRows; y++)
	{
		bCompLine	= TRUE;
		iTempCount = 0;//reset the temp count

		for(int x = 0; x < GAME_WIDTH; x++)
		{
			if(IsGamePiece(m_oGameField[x][y].ePiece) == FALSE)
				bCompLine = FALSE;
			if(bCompLine == FALSE)
				break;//save a little processing power
			if(m_oGameField[x][y].ePiece == HEX_BonusOrb)
			{	//we found an orb but we don't know if its a complete line yet ... so save it to the temp array for now
				ptOrbTemp[iTempCount].x = x;
				ptOrbTemp[iTempCount].y = y;
				iTempCount++;
			}
		}

		//we have a complete line
		if(bCompLine)
		{
			//increase the count
			m_sClearedLines[m_iClearedLines].bAlt = false;
			m_sClearedLines[m_iClearedLines].btRow = y;
			m_iClearedLines++;

			//and set our explosions
			while(iTempCount > 0)
			{	//we're doing the in the reverse order but it doesn't matter
				ptExplodedOrbs[iExplodedOrbs].x = ptOrbTemp[iTempCount-1].x;
				ptExplodedOrbs[iExplodedOrbs].y = ptOrbTemp[iTempCount-1].y;
				iExplodedOrbs++;
				iTempCount--;
			}


		}

		bCompAlt = TRUE;

		for(int x = 0; x < GAME_WIDTH; x++)
		{
			if(IsGamePiece(m_oGameField[x][y+x%2].ePiece) == FALSE)
				bCompAlt = FALSE;
			if(bCompAlt == FALSE)
				break;//break out if we don't have a complete line
			if(m_oGameField[x][y+x%2].ePiece == HEX_BonusOrb)
			{
				//the other consideration .... have we found this orb already? ... honestly? doesn't matter ... we'll take care of that in the explosion code
				ptOrbTemp[iTempCount].x = x;
				ptOrbTemp[iTempCount].y = y;
				iTempCount++;
			}
		}

		if(bCompAlt)
		{	//alt complete lines 
			//increase the count
			m_sClearedLines[m_iClearedLines].bAlt = true;
			m_sClearedLines[m_iClearedLines].btRow = y;
			m_iClearedLines++;

			//and set our explosions
			while(iTempCount > 0)
			{	//we're doing the in the reverse order but it doesn't matter
				ptExplodedOrbs[iExplodedOrbs].x = ptOrbTemp[iTempCount-1].x;
				ptExplodedOrbs[iExplodedOrbs].y = ptOrbTemp[iTempCount-1].y;
				iExplodedOrbs++;
				iTempCount--;
			}
		}
	}

	if(m_iClearedLines > 0)
	{
		//now we set our alt structures
		memcpy(m_oPrevField, m_oGameField, sizeof(GameHexagon)*GAME_WIDTH*MAX_GAME_HEIGHT);

		int iOffset = 0;

		for(int i = 0; i < iExplodedOrbs; i++)
		{	//set our explosions
			SingleExplosion(ptExplodedOrbs[i]);
		}

		for(int i = 0; i < m_iClearedLines; i++)
		{
			//now we'll set the hexes to cleared
			SetLineToCleared(m_sClearedLines[i].btRow, m_sClearedLines[i].bAlt);
		}
		//save the pre drop field
		memcpy(m_oPreDropField, m_oGameField, sizeof(GameHexagon)*GAME_WIDTH*MAX_GAME_HEIGHT);

		//and finally, copy and move everything down
		DropPieces();

		PostMessage(m_hWndDlg, WM_LINE_CLEARED, m_iClearedLines, 0);

		//check count for bonuses
		//we do this here because we want the bonus animation happening after the line drop
		if(m_iClearedLines > MIN_ROWS_BONUS_SIDE)
		{
			AddGemToSaved();
			AddGemToQue();
		}
		else if(m_iClearedLines > MIN_ROWS_BONUS_QUE)
		{
			AddGemToQue();
		}
	}
}

void CHexGame::DropPieces()
{
	//to make this really easy we're going to do this one column at a time
	for(int i = 0; i < GAME_WIDTH; i++)
	{
		
		for(int j = m_iRows-1; j > -1; j--)
        //for(int j = m_iRows; j > -1; j--)
		{
			if(m_oGameField[i][j].ePiece == HEX_Erased)
			{	
				for(int k = j; k < m_iRows-1; k++)
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

	int iPoints = 0;

	int iNewLevel = m_iRowsCleared/LINES_PER_LEVEL;

	if(iNewLevel > m_iLevel)
	{
		m_iLevel		= iNewLevel;
		m_iScore		+= m_iLevel*LEVEL_BONUS;
		m_dbLevelPercent = 0.0;

		if((m_iLevel-1)%5 == 0)
		{
			AddGemToSaved();
			AddGemToQue();
		}
		else
		{
			AddGemToQue();
		}

		if(m_eGameType == GT_Challenge)
		{
			memcpy(m_oPrevField, m_oGameField, sizeof(GameHexagon)*GAME_WIDTH*MAX_GAME_HEIGHT);//save the field
			ClearGameBoard();
			AddGarageBlocks();
		}
		PostMessage(m_hWndDlg, WM_LEVEL_CHANGE, m_iLevel, GetLevelTiming());
	}
	else
	{
		m_dbLevelPercent = (double)m_iRowsCleared/(double)((m_iLevel+1)*LINES_PER_LEVEL);
	}

	int iIncrease = POINTS_PER_LINE + m_iLevel*POINTS_PER_LINE;

	m_iLastScoreIncrease += iIncrease;

	m_iScore += iIncrease;


	//for Clear 40 Lines GameType
/*	if(m_eGameType == GT_40 && m_iRowsCleared >= 40)
	{
		PostMessage(m_hWndDlg, WM_40LINES_CLEARED, 0, 0);
		m_eState = GS_GameOver;
	}*/

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
	return BASE_DROP_TIME - (m_iLevel)*BASE_DROP_TIME/20;


/*	int iRet = BASE_DROP_TIME - m_iLevel*DROP_CHANGE;

	if(iRet < DROP_CHANGE)
		iRet = (20-m_iLevel)*5;

	if(iRet < 0)
		iRet = 0;

	return iRet;*/
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

void CHexGame::AddGemToQue()
{
	//do a post message
	m_iBonusQue++;
	PostMessage(m_hWndDlg, WM_BONUS_IN_QUE, m_iBonusQue, m_iSavedGems);
}

void CHexGame::AddGemToSaved()
{
	m_iSavedGems++;
	PostMessage(m_hWndDlg, WM_BONUS_SAVED, m_iBonusQue, m_iSavedGems);

	if(m_iSavedGems >= MAX_SAVED_GEMS)
	{
	//	FourOrbClear();

		//its bonus time

		//add huge score
		m_iLastScoreIncrease = FOUR_ORB_BONUS;
		m_iScore += m_iLastScoreIncrease;

		//send score message?
		//don't have to check hi score - its checked every OnTimer
		
		//clear gems
		m_iSavedGems = 0;

		//send bonus message
		PostMessage(m_hWndDlg, WM_ALL_ORBS, 0, 0);//this should probably do an animation of sorts
	}
}

void CHexGame::FourOrbClear()
{
	//save the previous state
	memcpy(m_oPrevField, m_oGameField, sizeof(GameHexagon)*GAME_WIDTH*MAX_GAME_HEIGHT);

	//see how many gems we have ... make sure we have enough to blow up
	int iCount = 0;

	for(int x = 0; x < GAME_WIDTH; x++)
	{
		for(int y = 0; y < GetGameHeight(); y++)
		{
			if(IsGamePiece(m_oGameField[x][y].ePiece))
				iCount++;
		}
	}

	if(iCount <= FOUR_ORB_NUM_EXP)
	{
		//we set them all to orbs and thats it

		for(int x = 0; x < GAME_WIDTH; x++)
		{
			for(int y = 0; y < GAME_WIDTH; y++)
			{
				if(IsGamePiece(m_oGameField[x][y].ePiece))
				{
					//so we set the previous game field to all orbs
					//and set the current game field to all erased yo
					m_oGameField[x][y].ePiece = HEX_Erased;
					m_oPrevField[x][y].ePiece = HEX_BonusOrb;
					POINT pt;
					pt.x = x;
					pt.y = y;
					SingleExplosion(pt, FALSE);
				}
			}
		}
	}

	//randomly select gems on the game board
	else
	{
		int iLoopCount = 0;
		int iSet = 0;
		POINT ptLoc;
		while(iSet < FOUR_ORB_NUM_EXP)
		{
			ptLoc.x = rand()%GAME_WIDTH;
			ptLoc.y = rand()%GetGameHeight();

			while(IsGamePiece(m_oGameField[ptLoc.x][ptLoc.y].ePiece) == FALSE)
			{
				iLoopCount++;
				if(iLoopCount > 10000)
					break;
				ptLoc.x++;
				if(ptLoc.x >= GAME_WIDTH)
				{
					ptLoc.x = 0;
					ptLoc.y++;
					if(ptLoc.y >= GetGameHeight())
						ptLoc.y = 0;
				}
			}

			if(iLoopCount > 10000)
				break;

			//probably don't need to set the field to erased but that's cool
			m_oGameField[ptLoc.x][ptLoc.y].ePiece = HEX_Erased;
			m_oPrevField[ptLoc.x][ptLoc.y].ePiece = HEX_BonusOrb;
			SingleExplosion(ptLoc, FALSE);

			iSet++;
		}
	}

	//	convert them to orbs
	//  set the game board for orb explosions
	memcpy(m_oPreDropField, m_oGameField, sizeof(GameHexagon)*GAME_WIDTH*MAX_GAME_HEIGHT);//not sure if we need this

	DropPieces();
}

BOOL CHexGame::OnUseGem()
{
	if(m_iSavedGems < 1)
		return FALSE;
	m_iSavedGems--;
	AddGemToQue();//this will increment m_iBonusQue
	return TRUE;
}

BOOL CHexGame::IsGamePiece(EnumHexPeice ePiece)
{
	if(ePiece == HEX_GamePiece || ePiece == HEX_BonusOrb)
		return TRUE;

	return FALSE;
}


/*
	So we always check left, right, up and down
	and if X%2 == 0 then we do upper corners
	otherwise ... lower corners

	This function will set our game field arrays for gameplay and animation when an orb explodes

	We'll set:
	m_oPreDropField to Erased and
	m_oPrevField to Explode




*/
void CHexGame::SingleExplosion(int iX, int iY)
{
	static POINT pt;
	pt.x = iX;
	pt.y = iY;
	SingleExplosion(pt);
}

void CHexGame::SingleExplosion(POINT ptLocation, BOOL bScore)
{
	//left
	if(ptLocation.x > 0)
	{
		if(m_oGameField[ptLocation.x-1][ptLocation.y].ePiece == HEX_GamePiece)
		{
			m_oGameField[ptLocation.x-1][ptLocation.y].ePiece = HEX_Erased;
		}
	}

	//right
	if(ptLocation.x < GAME_WIDTH - 1)
	{
		if(m_oGameField[ptLocation.x+1][ptLocation.y].ePiece == HEX_GamePiece)
		{
			m_oGameField[ptLocation.x+1][ptLocation.y].ePiece = HEX_Erased;
		}
	}

	//down
	if(ptLocation.y > 0)
	{
		if(m_oGameField[ptLocation.x][ptLocation.y-1].ePiece == HEX_GamePiece)
		{
			m_oGameField[ptLocation.x][ptLocation.y-1].ePiece = HEX_Erased;
		}
	}

	//up
	if(ptLocation.y < MAX_GAME_HEIGHT)
	{
		if(m_oGameField[ptLocation.x][ptLocation.y+1].ePiece == HEX_GamePiece)
		{
			m_oGameField[ptLocation.x][ptLocation.y+1].ePiece = HEX_Erased;
		}
	}

	//left corner
	//depending on location we do upper or lower corners (its the way the game board is stored in code)
	if(ptLocation.x%2==0)
	{
		if(ptLocation.x > 0 && ptLocation.y < MAX_GAME_HEIGHT)
		{
			if(m_oGameField[ptLocation.x-1][ptLocation.y+1].ePiece == HEX_GamePiece)
			{
				m_oGameField[ptLocation.x-1][ptLocation.y+1].ePiece = HEX_Erased;
			}
		}
		if(ptLocation.x < GAME_WIDTH - 1 && ptLocation.y < MAX_GAME_HEIGHT)
		{
			if(m_oGameField[ptLocation.x+1][ptLocation.y+1].ePiece == HEX_GamePiece)
			{
				m_oGameField[ptLocation.x+1][ptLocation.y+1].ePiece = HEX_Erased;
			}
		}
	}
	//right corner
	//depending on location we do upper or lower corners (its the way the game board is stored in code)
	else
	{
		if(ptLocation.x > 0 && ptLocation.y > 0)
		{
			if(m_oGameField[ptLocation.x-1][ptLocation.y-1].ePiece == HEX_GamePiece)
			{
				m_oGameField[ptLocation.x-1][ptLocation.y-1].ePiece = HEX_Erased;
			}
		}
		if(ptLocation.x < GAME_WIDTH - 1 && ptLocation.y > 0)
		{
			if(m_oGameField[ptLocation.x+1][ptLocation.y-1].ePiece == HEX_GamePiece)
			{
				m_oGameField[ptLocation.x+1][ptLocation.y-1].ePiece = HEX_Erased;
			}
		}
	}

	if(bScore)
		ScoreClearedLine();
}

int CHexGame::GetLevelComp()
{
	return m_iRowsCleared%LINES_PER_LEVEL;	
}

void CHexGame::DoubleExplosion(POINT ptLocation)
{
	//we're going to be really lazy and just call a bunch of single explosions
	//left
	if(m_oGameField[ptLocation.x - 1][ptLocation.y].ePiece == HEX_BonusOrb)
		m_oGameField[ptLocation.x - 1][ptLocation.y].ePiece = HEX_GamePiece;
	if(m_oGameField[ptLocation.x + 1][ptLocation.y].ePiece == HEX_BonusOrb)
		m_oGameField[ptLocation.x + 1][ptLocation.y].ePiece = HEX_GamePiece;
	if(m_oGameField[ptLocation.x][ptLocation.y-1].ePiece == HEX_BonusOrb)
		m_oGameField[ptLocation.x][ptLocation.y-1].ePiece = HEX_GamePiece;
	if(m_oGameField[ptLocation.x][ptLocation.y+1].ePiece == HEX_BonusOrb)
		m_oGameField[ptLocation.x][ptLocation.y+1].ePiece = HEX_GamePiece;

	if(ptLocation.x%2!=0)
	{
		if(m_oGameField[ptLocation.x - 1][ptLocation.y-1].ePiece == HEX_BonusOrb)
			m_oGameField[ptLocation.x - 1][ptLocation.y-1].ePiece = HEX_GamePiece;
		if(m_oGameField[ptLocation.x + 1][ptLocation.y-1].ePiece == HEX_BonusOrb)
			m_oGameField[ptLocation.x + 1][ptLocation.y-1].ePiece = HEX_GamePiece;
	}
	else
	{
		if(m_oGameField[ptLocation.x - 1][ptLocation.y+1].ePiece == HEX_BonusOrb)
			m_oGameField[ptLocation.x - 1][ptLocation.y+1].ePiece = HEX_GamePiece;
		if(m_oGameField[ptLocation.x + 1][ptLocation.y+1].ePiece == HEX_BonusOrb)
			m_oGameField[ptLocation.x + 1][ptLocation.y+1].ePiece = HEX_GamePiece;
	}

	

	SingleExplosion(ptLocation.x - 1, ptLocation.y);
	//right
	SingleExplosion(ptLocation.x + 1, ptLocation.y);
	//down
	SingleExplosion(ptLocation.x, ptLocation.y - 1);
	//top
	SingleExplosion(ptLocation.x, ptLocation.y + 1);
	//left corner
	if(ptLocation.x%2!=0)
		SingleExplosion(ptLocation.x - 1, ptLocation.y - 1);
	else
		SingleExplosion(ptLocation.x - 1, ptLocation.y + 1);
		//right corner
	if(ptLocation.x%2!=0)
		SingleExplosion(ptLocation.x + 1, ptLocation.y - 1);
	else
		SingleExplosion(ptLocation.x + 1, ptLocation.y + 1);

	//and clear the center
	m_oGameField[ptLocation.x][ptLocation.y].ePiece = HEX_Erased;
}

BOOL CHexGame::CheckForDoubleOrb(POINT &pt1, POINT &pt2)
{
	if(m_oCurPiece[0]->HasOrb() == FALSE)
		return FALSE;//make it easy
	
	//otherwise we find the orb location and then check if its touching another orb
	int x,y;
	
	BOOL bBreak = FALSE;

	for(x = 0; x < 5; x++)
	{
		for(y = 0; y < 5; y++)
		{
			if(m_oCurPiece[0]->GetHex(x,y)->ePiece == HEX_BonusOrb)
			{
				goto exit;
			}
		}
	}
exit:

	if(x==5 || y == 5)
		return FALSE;

	//now we have to translate the orb position in the piece to the board ... and then check the board
	pt1.x = m_oCurPiece[0]->GetLocation()->x + x;
	pt1.y = m_oCurPiece[0]->GetLocation()->y - y;

	for(int i = 0; i < x + 1; i++)
	{
		if(((i+m_oCurPiece[0]->GetLocation()->x)%2)   ==0)
			pt1.y--;
	}

	//now we search for the other orb
	//I'm not looking for triples ... yet
	if(GetHexAtLocation(pt1.x - 1, pt1.y) == HEX_BonusOrb)
	{
		pt2.x = pt1.x -1;
		pt2.y = pt1.y;
		return TRUE;
	}

	if(GetHexAtLocation(pt1.x + 1, pt1.y) == HEX_BonusOrb)
	{
		pt2.x = pt1.x + 1;
		pt2.y = pt1.y;
		return TRUE;
	}

	if(GetHexAtLocation(pt1.x, pt1.y - 1) == HEX_BonusOrb)
	{
		pt2.x = pt1.x;
		pt2.y = pt1.y - 1;
		return TRUE;
	}

	if(GetHexAtLocation(pt1.x, pt1.y + 1) == HEX_BonusOrb)
	{
		pt2.x = pt1.x;
		pt2.y = pt1.y + 1;
		return TRUE;
	}

	//and of course the corners
	if(pt1.x%2!=0)
	{
		if(GetHexAtLocation(pt1.x - 1, pt1.y - 1) == HEX_BonusOrb)
		{
			pt2.x = pt1.x - 1;
			pt2.y = pt1.y - 1;
			return TRUE;
		}
		if(GetHexAtLocation(pt1.x + 1, pt1.y - 1) == HEX_BonusOrb)
		{
			pt2.x = pt1.x + 1;
			pt2.y = pt1.y - 1;
			return TRUE;
		}
	}
	else
	{
		if(GetHexAtLocation(pt1.x - 1, pt1.y + 1) == HEX_BonusOrb)
		{
			pt2.x = pt1.x - 1;
			pt2.y = pt1.y + 1;
			return TRUE;
		}
		if(GetHexAtLocation(pt1.x + 1, pt1.y + 1) == HEX_BonusOrb)
		{
			pt2.x = pt1.x + 1;
			pt2.y = pt1.y + 1;
			return TRUE;
		}
	}
			


	return FALSE;
}

EnumHexPeice CHexGame::GetHexAtLocation(int iX, int iY)
{
	if(iX < 0 || iX >= GAME_WIDTH || iY < 0 || iY > MAX_GAME_HEIGHT)
		return HEX_Null;
	else
//		return m_oGameField[iX, iY]->ePiece;
		return m_oGameField[iX][iY].ePiece;
}
