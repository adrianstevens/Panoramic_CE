#include "KenKenGame.h"
#include "KenKenPuzzle.h"
#include "IssCommon.h"
#include "IssDebug.h"
#include "IssVector.h"
#include "IssRegistry.h"

struct TypeGameState	// used for saving a complete game set
{
    int iTiles[NUM_ROWS][NUM_COLUMNS];
    int	iSamuIndex[NUM_ROWS][NUM_COLUMNS];//index for the colors
    int	iSamuValue[NUM_ROWS][NUM_COLUMNS];//total within the Samu (yeah I'm making up words)

    int iKenKens[MAX_SAMUNAMPURES]; //the kenken values ... ie the math answers 
    int eKenKenOps[MAX_SAMUNAMPURES]; //value of each group of numbers

    int iNumFreeCells;		// free cells in the game
    int iNumSeconds;
    int iScore;
};

#ifdef DEBUG
BOOL Increment(TCHAR* szNum, int iSize)
{
    for(int i = iSize -1; i >= 0; i--)
    {
        if(szNum[i] < _T('9') - iSize + i + 1)//pretty sure this is good
        {
            szNum[i]++;
            //reset all values following it
            for(int j = i; j < iSize; j++)
                szNum[j] = szNum[i]+j-i;//also pretty sure we're good
            return TRUE;
        }
    }
    return FALSE;
}

int Multiply(TCHAR* szNum, int iSize)
{
    int iRet = 1;

    for(int i = 0; i < iSize; i++)
    {
        iRet *= (int)(szNum[i] - _T('0'));
    }

    return iRet;
}

int Binary(TCHAR* szNum, int iSize)
{
    int iRet = 0;

    for(int i = 0; i < iSize; i++)
    {
        iRet += (int)(1 << (9 -(szNum[i] - _T('0'))));
    }
    
    return iRet;
}

int Find(CIssVector<StructNumberList> &arrList, int iSize, int iMult)
{
    for(int i = 0; i < arrList.GetSize(); i++)
    {
        if(arrList[i]->btNumCells == iSize &&
            arrList[i]->btTotal == iMult)
            return i;
    }

    return -1;
}

void CalcMult()
{
    CIssVector<StructNumberList> arrList;
    TCHAR szNum[STRING_SMALL];
    
    int iMult, iBin, iIndex;

    for(int iGroup = 2; iGroup < 7; iGroup++)
    {
        int i;
        for(i = 0; i < iGroup; i++)
            szNum[i] = i+_T('1');
        szNum[i] = _T('\0');

        do
        {
            iMult = Multiply(szNum, iGroup);
            iBin = Binary(szNum, iGroup);
            DBG_OUT((_T("{%i, %i, %i},//%s"), iGroup, iMult, iBin, szNum));

            if(iMult == 288)
                Sleep(1);

            iIndex = Find(arrList, iGroup, iMult);

            if(iIndex == -1)
            {
                StructNumberList* sList = new StructNumberList;
                sList->btNumCells = iGroup;
                sList->btTotal = iMult;
                sList->wIncluded = iBin;
                arrList.AddElement(sList);
                sList = NULL;
            }
            else
            {
                arrList[iIndex]->wIncluded |= iBin;
            }
        }
        while(Increment(szNum, iGroup));
    }

    for(int i = 0; i < arrList.GetSize(); i++)
    {
        DBG_OUT((_T("{%i, %i, %i},"), arrList[i]->btNumCells, arrList[i]->btTotal, arrList[i]->wIncluded));
    }


    Sleep(1);
}
#endif




CKenKenGame::CKenKenGame(void)
{
  //CalcMult();
    LoadRegistry();
}

CKenKenGame::~CKenKenGame(void)
{
    Destroy();
}

void CKenKenGame::Destroy()
{
    SaveRegistry();
}



void CKenKenGame::NewGame(EnumDifficulty eDifficulty)
{
    int iPuzzleArray[NUM_ROWS][NUM_COLUMNS];
    int iSolutionArray[NUM_ROWS][NUM_COLUMNS];
    CKenKenPuzzle Puzzle;
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

    for(int i = 0; i < MAX_SAMUNAMPURES; i++)
    {
        m_iKenKens[i]      = Puzzle.GetKenKen(i);
        m_eKenKenOps[i]    = Puzzle.GetOp(i);
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

int CKenKenGame::GetMathAnswer(int iX, int iY)
{
    return m_iKenKens[m_iSamuIndex[iX][iY]];
}

EnumOperator CKenKenGame::GetOp(int iX, int iY)
{
    return m_eKenKenOps[m_iSamuIndex[iX][iY]];
    return OP_None;
}

BOOL CKenKenGame::IsPencilMarkValid(int iX, int iY, int iValue)
{
    //check the grouping and then pass it off to Sudoku
    int iSize = GetCellsInGroup(iX, iY);
    int iTotal = GetMathAnswer(iX, iY);

    int iGuess = 511; // default to all possibilities

    //get encoded guess value
    //encoded in binary with 9 values left to right
    /*
    1 = 10000000 or 256
    2 = 01000000 or 128
    3 = 00100000 or 64

    1 to 9 = 11111111 or 511
    etc
    */
    switch(GetOp(iX, iY))
    {
    case OP_Plus:
        iGuess = GetAddPossibilities(iSize, iTotal);
        break;
    case OP_Minus:
        iGuess = GetSubtractPossibilities(iSize, iTotal);
        break;
    case OP_Times:
        iGuess = GetMultiplyPossibilities(iSize, iTotal);
        break;
    case OP_Divide:
        iGuess = GetDividePossibilities(iSize, iTotal);
        break;
    }
    if(IsGuessValid(iGuess, iValue) == FALSE)//plain sudoku rules check
        return FALSE;

    return CSudokuGame::IsPencilMarkValid(iX, iY, iValue);

}

int CKenKenGame::GetSubtractPossibilities(int iSize, int iTotal)
{
    int iGuess = 511; // default to all possibilities

    for(int i=0; i< sizeof(g_sSubNumList)/sizeof(StructNumberList); i++)
    {
        if(iSize == g_sSubNumList[i].btNumCells && iTotal == g_sSubNumList[i].btTotal)
        {
            iGuess = g_sSubNumList[i].wIncluded;
            break;
        }
    }
    return iGuess;
}

int CKenKenGame::GetMultiplyPossibilities(int iSize, int iTotal)
{
    int iGuess = 511; // default to all possibilities

    for(int i=0; i< sizeof(g_sMultiNumList)/sizeof(StructNumberList); i++)
    {
        if(iSize == g_sMultiNumList[i].btNumCells && iTotal == g_sMultiNumList[i].btTotal)
        {
            iGuess = g_sMultiNumList[i].wIncluded;
            break;
        }
        else if(iSize < g_sMultiNumList[i].btNumCells)
        {
            break;
        }

    }
    return iGuess;
}

int CKenKenGame::GetDividePossibilities(int iSize, int iTotal)
{
    int iGuess = 511; // default to all possibilities

    for(int i=0; i< sizeof(g_sDivNumList)/sizeof(StructNumberList); i++)
    {
        if(iSize == g_sDivNumList[i].btNumCells && iTotal == g_sDivNumList[i].btTotal)
        {
            iGuess = g_sDivNumList[i].wIncluded;
            break;
        }
    }
    return iGuess;
}

int CKenKenGame::GetAddPossibilities(int iSize, int iTotal)
{
    return GetGuessPossibilities(iSize, iTotal);
}



void CKenKenGame::LoadRegistry()
{
    DWORD dwValue;

    if(S_OK == GetKey(REG_KEY,_T("KenKenGameInPlay"),dwValue))
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
        if(S_OK == GetKey(REG_KEY, _T("KenKenGameState"), (LPBYTE)&sGs, dwValue))
        {
            // set our member variables
            m_iNumFreeCells		= sGs.iNumFreeCells;
            m_iNumSeconds		= sGs.iNumSeconds;
            m_iScore			= sGs.iScore;
            memcpy(m_iTile, sGs.iTiles, sizeof(int)*SUDOKU_Size*SUDOKU_Size);
            memcpy(m_iSamuIndex, sGs.iSamuIndex, sizeof(int)*SUDOKU_Size*SUDOKU_Size);
            memcpy(m_iSamuValue, sGs.iSamuValue, sizeof(int)*SUDOKU_Size*SUDOKU_Size);


            memcpy(m_iKenKens, sGs.iKenKens, sizeof(int)*MAX_SAMUNAMPURES);
            memcpy(m_eKenKenOps, sGs.eKenKenOps, sizeof(EnumOperator)*MAX_SAMUNAMPURES);

            CalcFreeCells();
        }
    }
}

void CKenKenGame::SaveRegistry()
{
    DWORD dwValue = (DWORD)m_bIsGameInPlay;
    SetKey(REG_KEY, _T("KenKenGameInPlay"), dwValue);
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

        memcpy(sGs.iKenKens, m_iKenKens, sizeof(int)*MAX_SAMUNAMPURES);
        memcpy(sGs.eKenKenOps, m_eKenKenOps, sizeof(EnumOperator)*MAX_SAMUNAMPURES);

        dwValue = sizeof(TypeGameState);
        SetKey(REG_KEY, _T("KenKenGameState"), (LPBYTE)&sGs, dwValue);	
    }
}