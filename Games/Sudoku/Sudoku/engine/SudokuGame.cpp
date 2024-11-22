#include "SudokuGame.h"
#include "IssRegistry.h"

#define POINTS_PER_CELL		500
#define POINTS_PER_ERROR	100
#define POINTS_PER_HINT		500
#define POINTS_LOCKSTATE	100
#define POINTS_MIN			25

struct TypeGameState	// used for saving a complete game set
{
	int iTiles[SUDOKU_Size][SUDOKU_Size];
	int iNumFreeCells;		// free cells in the game
	int iNumSeconds;
	int iScore;
};

CSudokuGame::CSudokuGame(void)
:m_iBoardSize(SUDOKU_Size)
,m_bIsGameInPlay(FALSE)
,m_iMistakeCounter(0)
,m_iNumFreeCells(0)
,m_iScore(0)
,m_iNumSeconds(0)
,m_iHighScore(0)
,m_bShowLockedState(TRUE)
,m_bShowErrors(TRUE)
,m_bAutoPencilMarks(FALSE)
,m_bAutoRemovePencilMarks(FALSE)
,m_eDifficulty(DIFF_Easy)
,m_hWnd(NULL)
,m_hInst(NULL)
{
    m_sLastMove.eLastMove = LASTMOVE_None;
	memset(m_iTile, 0, sizeof(int)*SUDOKU_Size*SUDOKU_Size);
	m_ptLastCorrectGuess.x	= -1;
	m_ptLastCorrectGuess.y	= -1;
	m_ptLastWrongGuess.x	= -1;
	m_ptLastWrongGuess.y	= -1;
	m_ptLastHint.x			= -1;
	m_ptLastHint.y			= -1;
	LoadRegistry();

#ifdef DEBUG
    m_bAutoRemovePencilMarks = TRUE;
#endif
}

CSudokuGame::~CSudokuGame(void)
{
	SaveRegistry();
}

void CSudokuGame::LoadRegistry()
{
	DWORD dwValue;

	if(S_OK == GetKey(REG_KEY,_T("SudokuGameInPlay"),dwValue))
		m_bIsGameInPlay	= (BOOL)dwValue;

	if(S_OK == GetKey(REG_KEY,_T("Level"),dwValue))
		m_eDifficulty		= (EnumDifficulty)dwValue;

/*	if(S_OK == GetKey(REG_KEY,_T("Locked"),dwValue))
		m_bShowLockedState	= (BOOL)dwValue;*/

/*	if(S_OK == GetKey(REG_KEY,_T("ShowErrors"),dwValue))
		m_bShowErrors	= (BOOL)dwValue;

	if(S_OK == GetKey(REG_KEY,_T("AutoPencil"),dwValue))
		m_bAutoPencilMarks	= (BOOL)dwValue;

	if(S_OK == GetKey(REG_KEY,_T("HiScore"),dwValue))
		m_iHighScore	= (BOOL)dwValue;*/

	// if there was a game then retrieve the game state
	if(m_bIsGameInPlay)
	{
		TypeGameState sGs;

		// get our single Game State values
		dwValue = sizeof(TypeGameState);
		if(S_OK == GetKey(REG_KEY, _T("SudokuGameState"), (LPBYTE)&sGs, dwValue))
		{
			// set our member variables
			m_iNumFreeCells		= sGs.iNumFreeCells;
			m_iNumSeconds		= sGs.iNumSeconds;
			m_iScore			= sGs.iScore;
			memcpy(m_iTile, sGs.iTiles, sizeof(int)*SUDOKU_Size*SUDOKU_Size);

			CalcFreeCells();
		}
	}
}

void CSudokuGame::SaveRegistry()
{
	DWORD dwValue = (DWORD)m_bIsGameInPlay;
	SetKey(REG_KEY, _T("SudokuGameInPlay"), dwValue);
	dwValue = (DWORD)m_eDifficulty;
	SetKey(REG_KEY, _T("Level"), dwValue);
/*	dwValue = (DWORD)m_bShowLockedState;
	SetKey(REG_KEY, _T("Locked"), dwValue);*/
/*	dwValue = (DWORD)m_bShowErrors;
	SetKey(REG_KEY, _T("ShowErrors"), dwValue);
	dwValue = (DWORD)m_bAutoPencilMarks;
	SetKey(REG_KEY, _T("AutoPencil"), dwValue);
	dwValue = (DWORD)m_iHighScore;
	SetKey(REG_KEY, _T("HiScore"), dwValue);*/

	if(m_bIsGameInPlay)
	{
		TypeGameState sGs;
		sGs.iNumSeconds			= m_iNumSeconds;
		sGs.iNumFreeCells		= m_iNumFreeCells;
		sGs.iScore				= m_iScore;
		memcpy(sGs.iTiles, m_iTile, sizeof(int)*SUDOKU_Size*SUDOKU_Size);

		dwValue = sizeof(TypeGameState);
		SetKey(REG_KEY, _T("SudokuGameState"), (LPBYTE)&sGs, dwValue);	
	}
}

void CSudokuGame::NewBoard()
{
	// reset our tile pieces
	memset(m_iTile, 0, sizeof(int)*SUDOKU_Size*SUDOKU_Size);
	m_iNumFreeCells = 0;
	m_iNumSeconds	= 0;
}

void CSudokuGame::SaveSolvableGame()
{
	m_bIsGameInPlay = TRUE;
	SaveRegistry();
}

BOOL CSudokuGame::NewGame()
{
	// start the wait cursor because this could take a few seconds
	SetCursor(LoadCursor(NULL, IDC_WAIT)); 
	ShowCursor(TRUE);

    // clear the board off, reset everything, then load a new puzzle
	// get the new puzzle
	int iPuzzleArray[NUM_ROWS][NUM_COLUMNS];
	int iSolutionArray[NUM_ROWS][NUM_COLUMNS];
	CSudokuPuzzle Puzzle;
	// create an array used to pass back some info here about the puzzle generating performance
	Puzzle.GetNewPuzzle(m_eDifficulty, iPuzzleArray, iSolutionArray, m_hInst);

	// reset our tile pieces
	memset(m_iTile, 0, sizeof(int)*SUDOKU_Size*SUDOKU_Size);

	// reset all the cells with the new puzzle
	for (int i = 0; i < NUM_ROWS; i++)				// y
	{
		for (int j = 0; j < NUM_COLUMNS; j++)		// x
		{
			SetSolution(j, i, iSolutionArray[i][j]);
			SetIsGiven(j, i, (iPuzzleArray[i][j]==0?FALSE:TRUE));
		}
	}

	m_bIsGameInPlay = TRUE;
	m_iNumFreeCells = 0;
	m_iNumSeconds	= 0;
	m_iScore		= 0;
	m_ptLastCorrectGuess.x	= -1;
	m_ptLastCorrectGuess.y	= -1;
    m_sLastMove.eLastMove = LASTMOVE_None;

	// check the full solution for any locked states
	CheckSolution();

	// see how many free cells there are
	CalcFreeCells();
	AutoFillPencilMarks();

	// End the Wait cursor
	ShowCursor(FALSE); 
	SetCursor(NULL);

	return TRUE;
}


POINT CSudokuGame::GetLastCorrectGuess()
{
	POINT pt = m_ptLastCorrectGuess;

	//erases it when you ask
	m_ptLastCorrectGuess.x = -1;
	m_ptLastCorrectGuess.y = -1;
	return pt;
}

POINT CSudokuGame::GetLastWrongGuess()
{
	POINT pt = m_ptLastWrongGuess;

	//erases it when you ask
	m_ptLastWrongGuess.x = -1;
	m_ptLastWrongGuess.y = -1;
	return pt;
}

POINT CSudokuGame::GetLastHint()
{
	POINT pt = m_ptLastHint;

	//erases it when you ask ... don't see why (Nov 09)
	//m_ptLastHint.x = -1;
	//m_ptLastHint.y = -1;
	return pt;
}

void CSudokuGame::StopGame()
{
	// let's save what's there just in case
	if(m_bIsGameInPlay)
		SaveRegistry();

	m_bIsGameInPlay	= FALSE;
}

void CSudokuGame::RestartGame()
{
	m_ptLastCorrectGuess.x	= -1;
	m_ptLastCorrectGuess.y	= -1;
	m_iMistakeCounter		= 0;
	m_iScore				= 0;
    m_sLastMove.eLastMove = LASTMOVE_None;

	m_bIsGameInPlay = TRUE;
	m_iNumSeconds	= 0;

	for(int x = 0; x < m_iBoardSize; x++)
	{
		for(int y=0; y < m_iBoardSize; y++)
		{
			SetIsLocked(x,y,FALSE);
			SetGuess(x,y,0);
			SetPencilMarks(x,y,0);
		}
	}

    AutoFillPencilMarks();
}

BOOL CSudokuGame::IsGameComplete()
{
	if(m_bIsGameInPlay == FALSE)
		return TRUE;

	if(m_iNumFreeCells > 0)
		return FALSE;

	//lets actually check then
	for(int x = 0; x < m_iBoardSize; x++)
	{
		for(int y=0; y < m_iBoardSize; y++)
		{
			if(!IsGiven(x,y) && !IsLocked(x,y))
			{
				//make sure the guess equals the solution
				if(GetSolution(x,y) != GetGuess(x,y))
					return FALSE;
			}
		}
	}
	//we win!
	m_bIsGameInPlay = FALSE;
	return TRUE;	
}

void CSudokuGame::SolveGame()
{
	m_bIsGameInPlay = FALSE;
	m_iNumFreeCells	= 0;

	// set the whole board the solution and locked (if applicable)
	for(int x = 0; x < m_iBoardSize; x++)
	{
		for(int y=0; y < m_iBoardSize; y++)
		{
            if(IsGiven(x, y))
                continue;
            if(IsLocked(x, y))
                continue;
            if(GetGuess(x, y) == GetSolution(x, y))
                continue;
            //much better
			SetGuess(x,y, GetSolution(x,y));
			SetIsLocked(x,y, TRUE);
            PostMessage(m_hWnd, WM_UPDATECELL, x, y);
		}
	}
}


BOOL CSudokuGame::IsGiven(int iX, int iY)
{
	int iTile = GetTile(iX, iY);
	return (BOOL)(iTile&1);
}

BOOL CSudokuGame::IsLocked(int iX, int iY)
{
	int iTile = GetTile(iX, iY);
	return (BOOL)(iTile&2)>>1;
}

int CSudokuGame::GetSolution(int iX, int iY)
{
	int iTile = GetTile(iX, iY);
	return (iTile&60)>>2;
}

int CSudokuGame::GetGuess(int iX, int iY)
{
	int iTile = GetTile(iX, iY);
	return (iTile&960)>>6;
}

int CSudokuGame::GetPencilMarks(int iX, int iY)
{
	int iTile = GetTile(iX, iY);
	return (iTile&523264)>>10;
}

BOOL CSudokuGame::IsValid(int iX, int iY)
{
	if(iX > m_iBoardSize ||	iY > m_iBoardSize)
		return FALSE;

	if(iX < 0 || iY < 0)
		return FALSE;

	return TRUE;
}

int	CSudokuGame::GetTile(int iX, int iY)
{
	if(!IsValid(iX, iY)) 
		return 0;

	return m_iTile[iX][iY];
}

void CSudokuGame::SetIsGiven(int iX, int iY, BOOL bIsGiven)
{
	int iTile = GetTile(iX, iY);
	iTile	&= ~1;
	iTile	|= (bIsGiven&1);
	SetTile(iX, iY, iTile);
}

void CSudokuGame::SetIsLocked(int iX, int iY, BOOL bIsLocked)
{
	int iTile = GetTile(iX, iY);
	iTile	&= ~2;

	iTile	|= ((bIsLocked<<1)&2);
	SetTile(iX, iY, iTile);
}

void CSudokuGame::SetSolution(int iX, int iY, int iSolution)
{
	int iTile = GetTile(iX, iY);
	iTile	&= ~60;
	iTile	|= ((iSolution<<2)&60);
	SetTile(iX, iY, iTile);
}

void CSudokuGame::OnSetGuess(int iX, int iY, int iGuess)
{
	// don't change anything if we're a given or if we're locked
	if(IsGiven(iX,iY) || IsLocked(iX, iY))
		return;

    m_sLastMove.eLastMove = LASTMOVE_Guess;
    m_sLastMove.iTile = m_iTile[iX][iY];
    m_sLastMove.iX = iX;
    m_sLastMove.iY = iY;

	// this is if we want to erase the current guess
	if(iGuess == GetGuess(iX, iY))
		iGuess = 0;

	SetGuess(iX, iY, iGuess);

    PostMessage(m_hWnd, WM_UPDATECELL, iX, iY);

	// we only care if the guess is valid
	if(iGuess != 0)
	{
		if(GetSolution(iX, iY) == GetGuess(iX, iY) || m_bShowErrors == FALSE)
		{
			m_ptLastCorrectGuess.x = iX;
			m_ptLastCorrectGuess.y = iY;
		}
		else if(GetSolution(iX, iY) != GetGuess(iX, iY))
		{
			m_ptLastWrongGuess.x	= iX;
			m_ptLastWrongGuess.y	= iY;
			m_iMistakeCounter++;//might not use it but now its here
		}
	}

	if(GetSolution(iX, iY) == GetGuess(iX, iY))
	{
		int iPoints = POINTS_PER_CELL - m_iNumSeconds;
		if(m_bShowLockedState)
			iPoints -= 100;//100 points off for the lock state
		if(m_bAutoPencilMarks)
			iPoints /= 2;//half points for auto pencil
		if(iPoints < POINTS_MIN)
			iPoints = POINTS_MIN;//min points (and no negatives for being slow)
		m_iScore += iPoints;
	}
	else
	{	//you're wrong and you lose points
		m_iScore -= POINTS_PER_ERROR;
		if(m_iScore < 0)
			m_iScore = 0;
	}

	//update the highscore
	if(m_iScore > m_iHighScore)
		m_iHighScore = m_iScore;

	RecalcPencilMarks(iX, iY);
	CalcFreeCells();
}

void CSudokuGame::SetGuess(int iX, int iY, int iGuess)
{
	int iTile = GetTile(iX, iY);
	iTile	&= ~960;
	iTile	|= ((iGuess<<6)&960);
	SetTile(iX, iY, iTile);

    CheckSolution();
}


void CSudokuGame::SetPencilMarks(int iX, int iY, int iPencilMarks)
{
	int iTile = GetTile(iX, iY);
	iTile	&= ~523264;
	iTile	|= ((iPencilMarks<<10)&523264);
	SetTile(iX, iY, iTile);
}

void CSudokuGame::SetTile(int iX, int iY, int iTile)
{
	if(!IsValid(iX, iY))
		return;

	m_iTile[iX][iY] = iTile;
}

void CSudokuGame::IncrementTimer()
{
	if(m_bIsGameInPlay)
		m_iNumSeconds++;
}

void CSudokuGame::TogglePencilMarks(int iX, int iY, int iNumber)
{
	if(iNumber < 1 || iNumber > 9 || !IsValid(iX, iY))
		return;

/*    m_sLastMove.eLastMove = LASTMOVE_Pencil;
    m_sLastMove.iTile = m_iTile[iX][iY];
   
    m_sLastMove.iX = iX;
    m_sLastMove.iY = iY;*/

	int iTemp = (int)(pow(2, iNumber-1));

	// make sure to get rid of the guess just in case
    if(GetGuess(iX, iY))
    {
        SetGuess(iX, iY, 0);
        PostMessage(m_hWnd, WM_UPDATECELL, iX, iY);
    }
	//RecalcPencilMarks(iX, iY);
	
	int iPencil = GetPencilMarks(iX, iY);

	if((iPencil & iTemp) == iTemp)
		iPencil -= iTemp;
	else
		iPencil += iTemp;

	SetPencilMarks(iX, iY, iPencil);
#ifdef DEBUG
	int iTest = GetPencilMarks(iX, iY);
#endif

	CalcFreeCells();
}

void CSudokuGame::CalcFreeCells()
{
	m_iNumFreeCells = 0;
	for(int x = 0; x < m_iBoardSize; x++)
	{
		for(int y=0; y < m_iBoardSize; y++)
		{
			if(!IsGiven(x,y) && GetGuess(x,y) == 0)
			{
				m_iNumFreeCells++;
			}
		}
	}
}

void CSudokuGame::CheckSolution()
{
	for(int x = 0; x < m_iBoardSize; x++)
	{
		for(int y=0; y < m_iBoardSize; y++)
		{
			// it will return TRUE if we have a full solution
			if(CheckCellSolution(x,y))
				return;
		}
	}
}

BOOL CSudokuGame::CheckCellSolution(int iX, int iY)
{
	BOOL bSolvedVal = FALSE;
	int iSolveCount = 0;

	// check if column is solved
	if(IsColumnSolved(iX))
	{
		iSolveCount++;
		if(m_bShowLockedState)
		{
			for (int y = 0; y < GetBoardSize(); y++)
            {
                if(IsLocked(iX, y) == FALSE)
                {
				    SetIsLocked(iX, y, TRUE);
                    if(m_bIsGameInPlay)PostMessage(m_hWnd, WM_UPDATECELL, iX, y);
                }
            }
		}			
	}

	// check if row is solved
	if(IsRowSolved(iY))
	{
		iSolveCount++;
		if(m_bShowLockedState)
		{
			for (int x = 0; x < GetBoardSize(); x++)
            {
                if(IsLocked(x, iY) == FALSE)
                {
                    SetIsLocked(x, iY, TRUE);
                    if(m_bIsGameInPlay)PostMessage(m_hWnd, WM_UPDATECELL, x, iY);
                }
            }
		}
	}

	// check if subsection is solved
	if (IsSubsectionSolved(iY, iX))
	{
		iSolveCount++;
		if(m_bShowLockedState)
		{
			// set the lockstate on all cells in the subsection
			int iRowSection		= GetSubsection(iY);
			int iColumnSection	= GetSubsection(iX);
			for (int y = iRowSection*3; y < iRowSection*3+3; y++)
			{
				for (int x = iColumnSection*3; x < iColumnSection*3+3; x++)
				{
                    if(IsLocked(x, y) == FALSE)
                    {
					    SetIsLocked(x,y, TRUE);
                        if(m_bIsGameInPlay)PostMessage(m_hWnd, WM_UPDATECELL, x, y);
                    }
				}
			}
		}
	}

	// if all three are solved, check if the whole game is solved
	if (iSolveCount == 3)
	{
		if (IsGameSolved())
		{
			bSolvedVal = TRUE;
			if(m_bShowLockedState)
			{
				// make sure to set everything as locked right now
				for (int y = 0; y < m_iBoardSize; y++)
				{
					for (int x = 0; x < m_iBoardSize; x++)
					{
						SetIsLocked(x,y,TRUE);
					}
				}
			}
		}
	}
	return bSolvedVal;
}

BOOL CSudokuGame::IsGameSolved()
{
	// check the whole board and see if we're solved
	for (int y = 0; y < m_iBoardSize; y++)
	{
		for (int x = 0; x < m_iBoardSize; x++)
		{
			if(!IsSolved(x,y))
				return FALSE;
		}
	}
	return TRUE;
}

BOOL CSudokuGame::IsColumnSolved(int iColumnIndex)
{
	// go through each cell of the column and check if either the current guess is equal
	// to the solution value, or if the cell value is given
	for (int y = 0; y < m_iBoardSize; y++)
	{
		if (!IsSolved(iColumnIndex, y))
			return FALSE;
	}
	return TRUE;
}

BOOL CSudokuGame::IsRowSolved(int iRowIndex)
{
	// go through each cell of the row and check if either the current guess is equal
	// to the solution value, or if the cell value is given
	for (int x = 0; x < m_iBoardSize; x++)
	{
		if (!IsSolved(x, iRowIndex))
			return FALSE;
	}

	return TRUE;
}

BOOL CSudokuGame::IsSubsectionSolved(int iRowIndex, int iColumnIndex)
{
	// go through each cell of the subsection and check if either the current guess is equal
	// to the solution value, or if the cell value is given

	// consider the subsections to make up a 3x3 array in the puzzle, and find which subsection
	// the given cell is in
	int iSubRow		= GetSubsection(iRowIndex);
	int iSubColumn	= GetSubsection(iColumnIndex);

	// now iterate through all cells in a subsection to see if they are all solved
	for (int y = iSubRow*3; y < iSubRow*3 + 3; y++)
	{
		for (int x = iSubColumn*3; x < iSubColumn*3 + 3; x++)
		{
			if (!IsSolved(x,y))
				return FALSE;
		}
	}
	return TRUE;
}



int CSudokuGame::GetSubsection(int iRowColumnIndex)
{
	int iRet = 0;
	switch(iRowColumnIndex)
	{	
	case 0:
	case 1:
	case 2:
		{
			iRet = 0;
			break;
		}
	case 3:
	case 4:
	case 5:
		{
			iRet = 1;
			break;
		}
	case 6:
	case 7:
	case 8:
		{
			iRet = 2;
			break;
		}
	}
	return iRet;
}

BOOL CSudokuGame::IsSolved(int iX, int iY)
{
	return (IsGiven(iX, iY) || IsLocked(iX, iY) || GetSolution(iX, iY) == GetGuess(iX, iY));
}

void CSudokuGame::ResetLockedItems()
{
	// if we show the locked states
	if(m_bShowLockedState)
		CheckSolution();
	else
	{
		// set all locked states to false
		for (int y = 0; y < m_iBoardSize; y++)
		{
			for (int x = 0; x < m_iBoardSize; x++)
			{
				SetIsLocked(x,y, FALSE);
			}
		}
	}
}

BOOL CSudokuGame::ClearAllPencilMarks()
{
	if(m_bAutoPencilMarks)
		return FALSE;

	for(int x = 0; x < SUDOKU_Size; x++)
	{
		for(int y = 0; y < SUDOKU_Size; y++)
		{
			SetPencilMarks(x,y,0);//just do em all .. I don't care
		}
	}

	return TRUE;
}

BOOL CSudokuGame::Hint()
{
	if(m_bIsGameInPlay == FALSE)
		return FALSE;

	BOOL	bFound	= FALSE;
	int		iLoop	= 0;
	int		iRandX	= -1;
	int		iRandY	= -1;

	iRandX = rand()%SUDOKU_Size;
	iRandY = rand()%SUDOKU_Size;	


	while(bFound == FALSE)
	{
		if(IsGiven(iRandX, iRandY) ||
			(GetGuess(iRandX, iRandY) == GetSolution(iRandX, iRandY)) )
		{
			iRandX++;
			if(iRandX == SUDOKU_Size)
			{
				iRandX = 0;
				iRandY++;
			}
			if(iRandY == SUDOKU_Size)
				iRandY = 0;
			continue;
		}
		//enjoy your freebee
		//so we get the animation ... update the free cells, etc
		SetGuess(iRandX, iRandY, GetSolution(iRandX, iRandY));
		RecalcPencilMarks(iRandX, iRandY);
        CalcFreeCells();
		CheckCellSolution(iRandX, iRandY);
		m_ptLastHint.x	= iRandX;
		m_ptLastHint.y	= iRandY;
		m_iScore -= POINTS_PER_HINT;
        PostMessage(m_hWnd, WM_UPDATECELL, iRandX, iRandY);
		if(m_iScore < 0)
			m_iScore = 0;
		break;
	}

	return TRUE;
}

void CSudokuGame::AutoFillPencilMarks()
{
	if(!m_bAutoPencilMarks)
		return;

	for(int x = 0; x < m_iBoardSize; x++)
	{
		for(int y = 0; y < m_iBoardSize; y++)
		{
			if(IsGiven(x,y))
				continue;
			if(GetGuess(x,y) != 0)
				continue;
			SetPencilMarks(x,y,0);//blank it out
			for(int i =0; i < SUDOKU_Size; i++)//size ... same as the number of pencil mark options
			{
				if(IsPencilMarkValid(x,y,i+1))
				{
					TogglePencilMarks(x,y,i+1);
				}
			}
		}
	}
}

BOOL CSudokuGame::IsPencilMarkValid(int iX, int iY, int iValue)
{
	int i;
	//Check row
	for(i = 0; i < SUDOKU_Size; i++)
	{
		if(i == iX)
			continue;
		if(IsGiven(i, iY))
		{
			if(GetSolution(i, iY) == iValue)
				return FALSE;
		}
		else if(GetGuess(i, iY) == iValue)
			return FALSE;
	}


	//check column
	for(i = 0; i < SUDOKU_Size; i++)
	{
		if(i == iY)
			continue;
		if(IsGiven(iX, i))
		{
			if(GetSolution(iX, i) == iValue)
				return FALSE;
		}
		else if(GetGuess(iX, i) == iValue)
			return FALSE;
	}

	//check nanet (3x3)
	for(int x = iX-iX%3; x < iX+(3-iX%3); x++)
	{
		for(int y = iY-iY%3; y < iY+(3-iY%3); y++)
		{
			if(IsGiven(x, y))
			{
				if(GetSolution(x, y) == iValue)
					return FALSE;
			}
			else if(GetGuess(x, y) == iValue)
				return FALSE;		

		}
	}
	return TRUE;
}

BOOL CSudokuGame::ReCalcCell(int iX, int iY, int iXCell, int iYCell)
{
	if(iX/3 != iXCell/3 && iY != iYCell)
		return FALSE;
	if(iY/3 != iYCell/3 && iX != iXCell)
		return FALSE;

	return TRUE;
}


void CSudokuGame::RecalcPencilMarks(int iX, int iY)
{
	if(!m_bAutoPencilMarks && !m_bAutoRemovePencilMarks)
		return;

    //don't clear out the pencil marks if you guessed wrong
    if(!m_bAutoPencilMarks && m_bAutoRemovePencilMarks && m_bShowErrors &&
        GetGuess(iX, iY) != GetSolution(iX, iY))
        return;


    if(m_bAutoPencilMarks)
    {
	    //only need to calc the row, column and nanet
	    //brute force for now
	    for(int x = 0; x < m_iBoardSize; x++)
	    {
		    for(int y = 0; y < m_iBoardSize; y++)
		    {
			    if(!ReCalcCell(x,y,iX,iY))
				    continue;//save a little work
			    if(IsGiven(x,y))
				    continue;
			    if(GetGuess(x,y) != 0)
				    continue;
			    SetPencilMarks(x,y,0);//blank it out
			    for(int i =0; i < SUDOKU_Size; i++)//size ... same as the number of pencil mark options
			    {
				    if(IsPencilMarkValid(x,y,i+1))
				    {
					    TogglePencilMarks(x,y,i+1);
				    }
			    }
		    }
	    }
    }

    //not needed twice
    else if(m_bAutoRemovePencilMarks)
    {
        POINT pt = {iX,iY};

        int iValue = GetGuess(iX, iY);
        if(iValue < 0)
            return;//no guess

        //if(GetLastCorrectGuess != pt && GetLastWrongGuess() != pt)
        //    return;//wasn't a guess so I don't care ...

        for(int x = 0; x < m_iBoardSize; x++)
        {
            for(int y = 0; y < m_iBoardSize; y++)
            {
                if(!ReCalcCell(x,y,iX,iY))
                    continue;//save a little work
                if(IsGiven(x,y))
                    continue;
                if(GetGuess(x,y) != 0)
                    continue;

                int iTemp = (int)(pow(2, iValue-1));

                int iPencil = GetPencilMarks(x, y);

                if((iPencil & iTemp) == iTemp)
                {
                    iPencil -= iTemp;
                    SetPencilMarks(x, y, iPencil);
                }
            }
        }
    }
}

BOOL CSudokuGame::Undo()
{
 //   OnSetGuess(m_sLastMove.iX,m_sLastMove.iY, 5);
 //   return TRUE;

    if(m_sLastMove.eLastMove == LASTMOVE_None)
        return FALSE;
    if(IsLocked(m_sLastMove.iX, m_sLastMove.iY))
        return FALSE;//don't get to unlock ... to bad

    int iCell = GetTile(m_sLastMove.iX,m_sLastMove.iY);
    
    SetTile(m_sLastMove.iX,m_sLastMove.iY,m_sLastMove.iTile);
 //   SetGuess(m_sLastMove.iX,m_sLastMove.iY,1);
    m_sLastMove.iTile = iCell;
    //lets just always post
    RecalcPencilMarks(m_sLastMove.iX, m_sLastMove.iY);
    CalcFreeCells();
    
    PostMessage(m_hWnd, WM_UNDOMOVE, m_sLastMove.iX, m_sLastMove.iY);
    return TRUE;
}

void CSudokuGame::SetLocked(BOOL bLocked)
{
    m_bShowLockedState = bLocked;
    ResetLockedItems();
}
