#include "KillerGame.h"
#include "KillerPuzzle.h"
#include "IssRegistry.h"

struct TypeGameState	// used for saving a complete game set
{
    int iTiles[NUM_ROWS][NUM_COLUMNS];
    int	iSamuIndex[NUM_ROWS][NUM_COLUMNS];//index for the colors
    int	iSamuValue[NUM_ROWS][NUM_COLUMNS];//total within the Samu (yeah I'm making up words)
    int iNumFreeCells;		// free cells in the game
    int iNumSeconds;
    int iScore;
};

CKillerGame::CKillerGame(void)
{
    m_sLastMove.eLastMove = LASTMOVE_None;
    memset(m_iTile, 0, sizeof(int)*SUDOKU_Size*SUDOKU_Size);
    memset(m_iSamuIndex, 0, sizeof(int)*SUDOKU_Size*SUDOKU_Size);
    memset(m_iSamuValue, 0, sizeof(int)*SUDOKU_Size*SUDOKU_Size);
    memset(m_bDrawSamuValue, 0, sizeof(BOOL)*SUDOKU_Size*SUDOKU_Size);
    m_ptLastCorrectGuess.x	= -1;
    m_ptLastCorrectGuess.y	= -1;
    m_ptLastWrongGuess.x	= -1;
    m_ptLastWrongGuess.y	= -1;
    m_ptLastHint.x			= -1;
    m_ptLastHint.y			= -1;
    LoadRegistry();

    m_bSetSingleValues = TRUE;
}

CKillerGame::~CKillerGame(void)
{
    Destroy();
}

void CKillerGame::Destroy()
{
    SaveRegistry();
}


void CKillerGame::NewGame(EnumDifficulty eDifficulty)
{
    int iPuzzleArray[NUM_ROWS][NUM_COLUMNS];
    int iSolutionArray[NUM_ROWS][NUM_COLUMNS];
    CKillerPuzzle Puzzle;
    // create an array used to pass back some info here about the puzzle generating performance

    Puzzle.GetNewPuzzle(eDifficulty, iPuzzleArray, iSolutionArray);
    
    // reset our tile pieces
    memset(m_iTile, 0, sizeof(int)*SUDOKU_Size*SUDOKU_Size);

    // reset all the cells with the new puzzle
    for (int i = 0; i < NUM_ROWS; i++)				// y
    {
        for (int j = 0; j < NUM_COLUMNS; j++)		// x
        {
            SetSolution(j, i, iSolutionArray[i][j]);
            SetIsGiven(j, i, (iPuzzleArray[i][j]==0?FALSE:TRUE));
            //this doesn't matter as long as the orientation matches
            m_iSamuIndex[j][i] = Puzzle.GetSamuIndex(i,j);
            m_iSamuValue[j][i] = Puzzle.GetSamuValue(i,j);
        }
    }

    if(m_bSetSingleValues)
    {
        //check for singles and set the value
        for(int i = 0; i < 9; i++)
        {
            for(int j = 0; j < 9; j++)
            {
                if(GetTotal(i,j) == GetSolution(i,j))
                    SetGuess(i,j,GetSolution(i,j));
            }
        }
    }

    m_bIsGameInPlay = TRUE;
    m_iNumFreeCells = 0;
    m_iNumSeconds	= 0;
    m_iScore		= 0;
    m_ptLastCorrectGuess.x	= -1;
    m_ptLastCorrectGuess.y	= -1;
    m_sLastMove.eLastMove = LASTMOVE_None;

    // see how many free cells there are
    CalcFreeCells();
    AutoFillPencilMarks();
}

DWORD CKillerGame::GetOutline(int i, int j)
{
    DWORD dwRet = 0;

    //we'll check for horizontal and vertical 
    if(m_iSamuIndex[j][i] == m_iSamuIndex[j][i+1] &&
        i < NUM_COLUMNS - 1)//draw horz line
    {
        dwRet = dwRet | KOUTLINE_Horizontal;
    }

    if(m_iSamuIndex[j][i] == m_iSamuIndex[j+1][i] &&
        j < NUM_ROWS - 1)//draw vert line
    {
        dwRet = dwRet | KOUTLINE_Vertical;
    }

    //draw left line
    if((m_iSamuIndex[j][i] != m_iSamuIndex[j][i-1] && i > 0) || 
        i == 0)
    {
        dwRet = dwRet | KOUTLINE_Left;
    }

    //draw right line
    if((m_iSamuIndex[j][i] != m_iSamuIndex[j][i+1] && 	i < NUM_ROWS - 1) || 
        i == NUM_ROWS - 1) 
    {
        dwRet = dwRet | KOUTLINE_Right;
    }

    //draw top line
    if((m_iSamuIndex[j][i] != m_iSamuIndex[j-1][i] && j > 0) ||
        j == 0)
    {
        dwRet = dwRet | KOUTLINE_Top;
    }

    //draw bottom line
    if((m_iSamuIndex[j][i] != m_iSamuIndex[j+1][i] && j < NUM_COLUMNS - 1) ||
        j == NUM_COLUMNS - 1)
    {
        dwRet = dwRet | KOUTLINE_Bottom;
    }

    return dwRet;

}

int CKillerGame::GetTotal(int iX, int iY)
{
    return m_iSamuValue[iX][iY];//who knows ... fuck you Kev
}

BOOL CKillerGame::IsPencilMarkValid(int iX, int iY, int iValue)
{
    //check the grouping and then pass it off to Sudoku
    int iSize = GetCellsInGroup(iX, iY);
    int iTotal = GetTotal(iX, iY);
        
    //get encoded guess value
    int iGuess = GetGuessPossibilities(iSize , iTotal);

     if(IsGuessValid(iGuess, iValue) == FALSE)
        return FALSE;

    return CSudokuGame::IsPencilMarkValid(iX, iY, iValue);

}

int CKillerGame::GetGuessPossibilities(int iNumCells, int iTotal)
{
    if(iTotal == 1)
        Sleep(1);

    int iGuess = 511; // default to all possibilities

    for(int i=0; i< sizeof(g_sNumList)/sizeof(StructNumberList); i++)
    {
        if(iNumCells == g_sNumList[i].btNumCells && iTotal == g_sNumList[i].btTotal)
        {
            iGuess = g_sNumList[i].wIncluded;
            break;
        }
    }
    return iGuess;
}

//iGuess is the encoded possible values ... kind of clever actually
BOOL CKillerGame::IsGuessValid(int iGuess, int iNumberToCheck)
{
    return (BOOL)(iGuess & (int)pow(2, 9-iNumberToCheck));
}

int CKillerGame::GetCellsInGroup(int iX, int iY)
{
    int iGroup = m_iSamuIndex[iX][iY];

    int iCount = 0;

    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            if(m_iSamuIndex[i][j] == iGroup)
                iCount++;
        }
    }
    return iCount;
}

BOOL CKillerGame::DrawTotal(int iX, int iY)
{
    //if there's a guess 
    if(GetGuess(iX, iY))
        return FALSE;

    //if there's a pencil mark
    if(GetPencilMarks(iX, iY))
        return FALSE;

    //otherwise we loop and see if its the top left of its group
    int iGroup = m_iSamuIndex[iX][iY];

    for(int y = 0; y < iY + 1; y++)
    {
        for(int x = 0; x < 9; x++)
        {
            if(x == iX && y == iY)
                return TRUE;

            //if we hit one bail ....
            if(m_iSamuIndex[x][y] == iGroup)
            {
                if(GetGuess(x, y) == 0 &&
                    GetPencilMarks(x, y) == 0)
                    return FALSE;//we can draw here
            }
        }
    }



  /*  for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < iY+1; j++)
        {
            if(i == iX && j == iY)
                return TRUE;

            //if we hit one bail ....
            if(m_iSamuIndex[i][j] == iGroup)
                return FALSE;
        }
    }*/
  
    return FALSE;
}

void CKillerGame::LoadRegistry()
{
    DWORD dwValue;

    if(S_OK == GetKey(REG_KEY,_T("KillerGameInPlay"),dwValue))
        m_bIsGameInPlay	= (BOOL)dwValue;

    if(S_OK == GetKey(REG_KEY,_T("Level"),dwValue))
        m_eDifficulty		= (EnumDifficulty)dwValue;

 /*   if(S_OK == GetKey(REG_KEY,_T("Locked"),dwValue))
        m_bShowLockedState	= (BOOL)dwValue;*/

/*    if(S_OK == GetKey(REG_KEY,_T("ShowErrors"),dwValue))
        m_bShowErrors	= (BOOL)dwValue;

    if(S_OK == GetKey(REG_KEY,_T("AutoPencil"),dwValue))
        m_bAutoPencilMarks	= (BOOL)dwValue;*/

    if(S_OK == GetKey(REG_KEY,_T("HiScore"),dwValue))
        m_iHighScore	= (BOOL)dwValue;

    // if there was a game then retrieve the game state
    if(m_bIsGameInPlay)
    {
        TypeGameState sGs;

        // get our single Game State values
        dwValue = sizeof(TypeGameState);
        if(S_OK == GetKey(REG_KEY, _T("KillerGameState"), (LPBYTE)&sGs, dwValue))
        {
            // set our member variables
            m_iNumFreeCells		= sGs.iNumFreeCells;
            m_iNumSeconds		= sGs.iNumSeconds;
            m_iScore			= sGs.iScore;
            memcpy(m_iTile, sGs.iTiles, sizeof(int)*SUDOKU_Size*SUDOKU_Size);
            memcpy(m_iSamuIndex, sGs.iSamuIndex, sizeof(int)*SUDOKU_Size*SUDOKU_Size);
            memcpy(m_iSamuValue, sGs.iSamuValue, sizeof(int)*SUDOKU_Size*SUDOKU_Size);

            CalcFreeCells();
        }
    }
}

void CKillerGame::SaveRegistry()
{
    DWORD dwValue = (DWORD)m_bIsGameInPlay;
    SetKey(REG_KEY, _T("KillerGameInPlay"), dwValue);
    dwValue = (DWORD)m_eDifficulty;
    SetKey(REG_KEY, _T("Level"), dwValue);
 /*   dwValue = (DWORD)m_bShowLockedState;
    SetKey(REG_KEY, _T("Locked"), dwValue);*/
 /*   dwValue = (DWORD)m_bShowErrors;
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
        memcpy(sGs.iSamuIndex, m_iSamuIndex, sizeof(int)*SUDOKU_Size*SUDOKU_Size);
        memcpy(sGs.iSamuValue, m_iSamuValue, sizeof(int)*SUDOKU_Size*SUDOKU_Size);

        dwValue = sizeof(TypeGameState);
        SetKey(REG_KEY, _T("KillerGameState"), (LPBYTE)&sGs, dwValue);	
    }
}