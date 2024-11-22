#include "GreaterGame.h"

CGreaterGame::CGreaterGame(void)
{
    LoadRegistry();
}

CGreaterGame::~CGreaterGame(void)
{
    SaveRegistry();
}

EnumGreaterDir CGreaterGame::GetGVertical(int iX, int iY)
{
    if(iY < 1)
        return GDIR_Unknown;//same as blank
    if(iY%3 == 0)
        return GDIR_Unknown;//same as blank

    if(GetSolution(iX, iY-1) > GetSolution(iX, iY))
        return GDIR_Up;
    else 
        return GDIR_Down;

  /*  if(iY > 7)
        return GDIR_Unknown;//same as blank
    if((iY+1)%3 == 0)
        return GDIR_Unknown;//same as blank

    if(GetSolution(iX, iY) > GetSolution(iX, iY+1))
        return GDIR_Up;
    else 
        return GDIR_Down;*/
}

EnumGreaterDir CGreaterGame::GetGHorizontal(int iX, int iY)
{
    if(iX < 1)
        return GDIR_Unknown;//same as blank
    if(iX%3 == 0)
        return GDIR_Unknown;//same as blank

    if(GetSolution(iX-1, iY) > GetSolution(iX, iY))
        return GDIR_Left;
    else 
        return GDIR_Right;

  /*  if(iX > 7)
        return GDIR_Unknown;//same as blank
    if((iX+1)%3 == 0)
        return GDIR_Unknown;//same as blank

    if(GetSolution(iX, iY) > GetSolution(iX+1, iY))
        return GDIR_Left;
    else 
        return GDIR_Right;*/
}






BOOL CGreaterGame::NewGame()
{
    // start the wait cursor because this could take a few seconds
    SetCursor(LoadCursor(NULL, IDC_WAIT)); 
    ShowCursor(TRUE);

    // clear the board off, reset everything, then load a new puzzle
    // get the new puzzle
    int iPuzzleArray[NUM_ROWS][NUM_COLUMNS];
    int iSolutionArray[NUM_ROWS][NUM_COLUMNS];
    CGreaterPuzzle Puzzle;
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

    //now some givens
    int iGivenCount = 0;

    switch(m_eDifficulty)
    {
    default:
    case DIFF_VeryEasy:
        iGivenCount = 27;
        break;    
    case DIFF_Easy:
        iGivenCount = 18;
        break;
    case DIFF_Medium:
        iGivenCount = 9;
        break;
    case DIFF_Hard:
        iGivenCount = 3;
        break;
    case DIFF_VeryHard:
        iGivenCount = 0;
        break;
    }

    int iX, iY;

    while(iGivenCount > 0)
    {
        iX = rand()%9;
        iY = rand()%9;

        if(iPuzzleArray[iX][iY] == 0)
        {
            SetIsGiven(iY, iX, TRUE);
            //iPuzzleArray[iX][iY] = iSolutionArray[iX][iY];
            iGivenCount--;
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