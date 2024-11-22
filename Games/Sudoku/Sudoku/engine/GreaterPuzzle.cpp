#include "GreaterPuzzle.h"

CGreaterPuzzle::CGreaterPuzzle(void)
{

}

CGreaterPuzzle::~CGreaterPuzzle(void)
{

}

void CGreaterPuzzle::GetNewPuzzle(EnumDifficulty eDifficulty, int iPuzzleArray[][9], int iSolutionArray[][9], HINSTANCE hInst)
{
    // generate a puzzle and copy m_iGeneratedPuzzle[NUM_ROWS][NUM_COLUMNS] to the argument
    // blank out the puzzle array
    GeneratePuzzle();
   
    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLUMNS; j++)
        {
            iSolutionArray[i][j]	= m_iGeneratedPuzzle[i][j];
            iPuzzleArray[i][j]		= 0;
        }
    }

    int iHintCount = 0;

    //we'll add some givens to make it easier later

    // determine number of extra cells to add to make the puzzle easier, once we have a solvable puzzle
    switch(eDifficulty)
    {
    case DIFF_VeryHard:
     
        break;
    case DIFF_Hard:
     
        break;
    case DIFF_Medium:
     
        break;
    case DIFF_Easy:
     
        break;
    case DIFF_VeryEasy:
    default:
     
        break;
    }




}