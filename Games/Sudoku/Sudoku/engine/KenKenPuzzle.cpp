#include "KenKenPuzzle.h"

CKenKenPuzzle::CKenKenPuzzle(void)
{
}

CKenKenPuzzle::~CKenKenPuzzle(void)
{
}

void CKenKenPuzzle::Reset()
{
    CKillerPuzzle::Reset();


}

void CKenKenPuzzle::GenerateKenKen(EnumDifficulty eDiff)
{
    //first generate the ol Samu
  //  GenerateSamu(eDiff);

    //now its time to get to work 
    //the samu engine now counts the number of values per section ... but we need to find the specific numbers per
    byte btArrValuesPerGroup[MAX_SAMUNAMPURES][10];
    byte btIndex[MAX_SAMUNAMPURES];

    ZeroMemory(&btArrValuesPerGroup, sizeof(byte)*MAX_SAMUNAMPURES*10);
    ZeroMemory(&btIndex, sizeof(byte)*MAX_SAMUNAMPURES);

    int iRow;

    for(int x = 0; x < 9; x++)
    {
        for(int y = 0; y < 9; y++)
        {
            iRow = m_iSamud[x][y];
            btArrValuesPerGroup[iRow][btIndex[iRow]] = m_iGeneratedPuzzle[x][y];
            btIndex[iRow]++;
        }
    }

    byte btTemp;
    int iSwap;

    //now sort each row in btArrValuesPerGroup
    for(int i = 0; i < MAX_SAMUNAMPURES; i++)
    {
        for(int n = 0; n < btIndex[i]-1; n++)
        {
            iSwap = n;

            while(iSwap > -1)
            {
                if(btArrValuesPerGroup[i][iSwap+1] > btArrValuesPerGroup[i][iSwap])
                {   //swap
                    btTemp = btArrValuesPerGroup[i][iSwap+1];
                    btArrValuesPerGroup[i][iSwap+1] = btArrValuesPerGroup[i][iSwap];
                    btArrValuesPerGroup[i][iSwap] = btTemp;
                }
                else 
                    iSwap = 0;//break out 
                iSwap--;
            }
        }
    }

    double dbTemp;
    int iTemp;
    BOOL bSet;

    for(int c = 0; c < m_iNumSamus; c++)
    {
        bSet = FALSE;
        dbTemp = (double)btArrValuesPerGroup[c][0];

        //check for divide
        for(int d = 0; d < btIndex[c]-1; d++)
        {
            if(c%5 == 0)
                break;//keep the mix better
            dbTemp = dbTemp/(double)btArrValuesPerGroup[c][d+1];
            if(dbTemp != (int)dbTemp || dbTemp == 1)
                break;//then onto the next check
            if(d == btIndex[c]-2)
            {   //we're good
                bSet = TRUE;
                m_eKenKenOps[c] = OP_Divide;
                m_iKenKens[c] = (int)dbTemp;
            }
        }

        if(bSet == TRUE)
            continue;



        iTemp = btArrValuesPerGroup[c][0];

        for(int d = 0; d < btIndex[c]-1;d++)
        {
            if(c%3 == 0 || c%5 == 0)
                break;//keep the mix better
            iTemp = iTemp - btArrValuesPerGroup[c][d+1];
            if(iTemp < 2)
                break;
            if(d == btIndex[c]-2)
            {   //we're good
                bSet = TRUE;
                m_eKenKenOps[c] = OP_Minus;
                m_iKenKens[c] = iTemp;
            }
        }

        if(bSet == TRUE)
            continue;


        iTemp = btArrValuesPerGroup[c][0];
        if(c%2 == 0 && btIndex[c] > 1)
        {//multiply
            for(int d = 1; d < btIndex[c]; d++)
                iTemp *= btArrValuesPerGroup[c][d];
            if(iTemp < 1000)
            {
                m_eKenKenOps[c] = OP_Times;
                m_iKenKens[c] = iTemp;
                continue;
            }//otherwise we'll just do plus
        }
        
        //else
        iTemp = btArrValuesPerGroup[c][0];
        {//plus
            for(int d = 1; d < btIndex[c]; d++)
                iTemp += btArrValuesPerGroup[c][d];
            m_eKenKenOps[c] = OP_Plus;
            m_iKenKens[c] = iTemp;
        }

        //otherwise minus

        //then alt between plus or times
    }
}

BOOL CKenKenPuzzle::GetNewPuzzle(EnumDifficulty eDifficulty, int iPuzzleArray[NUM_ROWS][NUM_COLUMNS], int iSolutionArray[NUM_ROWS][NUM_COLUMNS], HINSTANCE hInst /* = NULL */)
{
    GeneratePuzzle();

    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLUMNS; j++)
        {
            iSolutionArray[i][j]	= m_iGeneratedPuzzle[i][j];
            iPuzzleArray[i][j]		= 0;
        }
    }

    //first generate the ol Samu
    GenerateSamu(eDifficulty);

    //and finally ensure its solvable
    if(FixNonSolvable(iPuzzleArray, iSolutionArray) == FALSE)
    {
        Reset();
        return GetNewPuzzle(eDifficulty, iPuzzleArray, iSolutionArray);
    }

    GenerateKenKen(eDifficulty);

    // now we have a puzzle with the desired difficulty level, so end the function
    return TRUE;
}

int CKenKenPuzzle::GetKenKen(int iIndex)
{
    if(iIndex < 0 || iIndex >= MAX_SAMUNAMPURES)
        return -1;

    return m_iKenKens[iIndex];
}

EnumOperator CKenKenPuzzle::GetOp(int iIndex)
{
    if(iIndex < 0 || iIndex >= MAX_SAMUNAMPURES)
        return OP_None;

    return m_eKenKenOps[iIndex];
}

int CKenKenPuzzle::GetSamuSize(EnumDifficulty eDiff)
{
    int iSamuSize = 1;

    switch(eDiff)
    {
    case DIFF_VeryEasy:
        iSamuSize = rand()%2 + 1;
        if(iSamuSize == 1)//this just helps reduce the number of singles
            iSamuSize = rand()%2 + 1;
        break;
    case DIFF_Easy:
        iSamuSize = 2;
        break;
    case DIFF_Medium:
        iSamuSize = rand()%2 + 2;
        break;
    case DIFF_Hard:
        iSamuSize = rand()%3 + 2;
        break;
    case DIFF_VeryHard:
        iSamuSize = rand()%4 + 2;
    default:
        break;
    }

    return iSamuSize;
}

