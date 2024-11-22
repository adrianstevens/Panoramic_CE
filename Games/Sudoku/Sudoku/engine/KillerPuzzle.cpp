#include "StdAfx.h"
#include "KillerPuzzle.h"
#include "IssDebug.h"

#define RANGE_MAX					9
#define RANGE_MIN					1
#define MIN_CELLS					32
#define MAX_CELLS					37
#define NUM_SECTIONS_WIDE			3
#define NUM_FIRST_REMOVE			40
#define NUM_CELLS_REMOVE_TARGET		NUM_FIRST_REMOVE + 14

/* Addressing board elements: linear array 0..80 */
#define ROW(idx)                ((idx)/9)
#define COLUMN(idx)             ((idx)%9)
#define BLOCK(idx)              (3*(ROW(idx)/3)+(COLUMN(idx)/3))
#define INDEX(row,col)          (9*(row)+(col))



CKillerPuzzle::CKillerPuzzle(void)
{
	m_oStr = CIssString::Instance();

	Reset();

}

CKillerPuzzle::~CKillerPuzzle(void)
{

}

void CKillerPuzzle::Reset()
{
    srand((unsigned)GetTickCount());
    // initialize array to all zeros
    int i;
    for (i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLUMNS; j++)
        {
            m_iGeneratedPuzzle[i][j] = 0;
            m_iSamud[i][j] = -1;
        }
    }

    // initialize array to all FALSE values
    for(i = 0; i < NUM_DIGITS; i++)
        m_bNumbers[i] = FALSE;

    for(int i = 0; i < MAX_SAMUNAMPURES; i++)
    {
        m_iSamunamupures[i] = 0;
        m_iSamuCounts[i] = 0;
    }

    m_iNumSamus = 0;
}

void CKillerPuzzle::MyQSort(int iSortArray[81], int iCompanionArray[81], int iFirst, int iLast)
{
	int i = iFirst;
	int j = iLast;
	int iMedian;

	// obtain the median element
	iMedian = iSortArray[(iFirst + iLast) / 2];
	do 
	{
		while (iSortArray[i] < iMedian)
		{
			i++;
		}
		while (iMedian < iSortArray[j])
		{
			j--;
		}
		if (i <= j)
		{
			// swap elements
			int iSwap1 = iSortArray[i];
			int iSwap2 = iCompanionArray[i];
			iSortArray[i] = iSortArray[j];
			iCompanionArray[i++] = iCompanionArray[j];
			iSortArray[j] = iSwap1;
			iCompanionArray[j--] = iSwap2;
		}
	} while(i <=j);

	if (iFirst < j)
	{
		MyQSort(iSortArray, iCompanionArray, iFirst, j);
	}

	if (i < iLast)
	{
		MyQSort(iSortArray, iCompanionArray, i, iLast);
	}
}

int CKillerPuzzle::Cardinality(int iPuzzleArray[NUM_ROWS][NUM_COLUMNS], int iRow, int iColumn)
{
	// add the number of given numbers in cells that are in the same row, column and block (9x9 section) as the
	// selected cell at iRow, iColumn
	int iCardinality = -3;	// will count itself 3 times
	int iBlockRowStart = (iRow / 3) * 3;
	int iBlockColumnStart = (iColumn / 3) * 3;
	
	// count number of other given cells in the column
	for (int i = 0; i < 9; i++)
	{
		if (iPuzzleArray[i][iColumn] != 0)
		{
			iCardinality++;
		}
	}
	// count number of other given cells in the row
	for (int j = 0; j < 9; j++)
	{
		if (iPuzzleArray[iRow][j] != 0)
		{
			iCardinality++;
		}
	}
	// count number of other given cells in the block
	for (int i = iBlockRowStart; i < iBlockRowStart + 3; i++)
	{
		for (int j = iBlockColumnStart; j < iBlockColumnStart + 3; j++)
		{
			if (iPuzzleArray[i][j] != 0)
			{
				iCardinality++;
			}
		}
	}
	return iCardinality;
}

BOOL CKillerPuzzle::GetNewPuzzle(EnumDifficulty eDifficulty, int iPuzzleArray[][9], int iSolutionArray[][9], HINSTANCE hInst)
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

	//And now generate the Samunamupure component	
	GenerateSamu(eDifficulty);

    //and finally ensure its solvable
    if(FixNonSolvable(iPuzzleArray, iSolutionArray) == FALSE)
    {
        Reset();
        return GetNewPuzzle(eDifficulty, iPuzzleArray, iSolutionArray);
    }

	// now we have a puzzle with the desired difficulty level, so end the function
	return TRUE;
}


void CKillerPuzzle::GenerateSamu(EnumDifficulty eDiff)
{
	BOOL bNumUsed[9]; //so we don't repeat numbers within the outline or "cages"

	int iSamuSize	= 0;
	int iTemp		= 0;
	int iRand		= 0;

	int iRow;	//more temps
	int iCol;

	iSamuSize = GetSamuSize(eDiff);

	//reset the bool array
	for(int i = 0; i < 9; i++)
		bNumUsed[i] = FALSE;

	bNumUsed[m_iGeneratedPuzzle[0][0]-1] = TRUE;

	for(int i = 0; i < NUM_ROWS; i++)
	{
		for(int j = 0; j < NUM_COLUMNS; j++)
		{
			//randomly decide the size		
			if(m_iSamud[i][j] != -1)
			{
				continue; //next position in the array
			}
			
			
			//set the value
			m_iSamud[i][j] = m_iNumSamus;
			iTemp = 1;
			iRow = i;
			iCol = j;

			m_iSamunamupures[m_iNumSamus] = m_iGeneratedPuzzle[iRow][iCol];
            m_iSamuCounts[m_iNumSamus] = 1;

			bNumUsed[m_iGeneratedPuzzle[iRow][iCol] - 1] = TRUE;

			while(iTemp < iSamuSize && 
				IsSamuExpansionPossible(iRow,iCol, bNumUsed)!=FALSE)
			{	//at least on direction is open...lets randomly find it
				iRand = rand()%4;
				
				switch(iRand)
				{
				case 0://above
					if(IsBelowAval(iRow, iCol, bNumUsed))
					{
						iRow++;
						m_iSamud[iRow][iCol] = m_iNumSamus;
						iTemp++;
						m_iSamunamupures[m_iNumSamus] += m_iGeneratedPuzzle[iRow][iCol];
                        m_iSamuCounts[m_iNumSamus]++;
						bNumUsed[m_iGeneratedPuzzle[iRow][iCol]-1] = TRUE;
					}
					break;
				case 1://below
					if(IsAboveAval(iRow, iCol, bNumUsed))
					{
						iRow--;
						m_iSamud[iRow][iCol] = m_iNumSamus;
						iTemp++;
						m_iSamunamupures[m_iNumSamus] += m_iGeneratedPuzzle[iRow][iCol];
                        m_iSamuCounts[m_iNumSamus]++;

						bNumUsed[m_iGeneratedPuzzle[iRow][iCol]-1] = TRUE;
					}
					break;
				case 2://left
					if(IsLeftAval(iRow, iCol, bNumUsed))
					{
						iCol--;
						m_iSamud[iRow][iCol] = m_iNumSamus;
						iTemp++;
						m_iSamunamupures[m_iNumSamus] += m_iGeneratedPuzzle[iRow][iCol];
                        m_iSamuCounts[m_iNumSamus]++;

						bNumUsed[m_iGeneratedPuzzle[iRow][iCol]-1] = TRUE;

					}
				    break;
				case 3://right
					if(IsRightAval(iRow, iCol, bNumUsed))
					{
						iCol++;
						m_iSamud[iRow][iCol] = m_iNumSamus;
						iTemp++;
						m_iSamunamupures[m_iNumSamus] += m_iGeneratedPuzzle[iRow][iCol];
                        m_iSamuCounts[m_iNumSamus]++;

						bNumUsed[m_iGeneratedPuzzle[iRow][iCol]-1] = TRUE;
					}
				    break;
				default:
					continue;
				    break;
				}
			}

            iSamuSize = GetSamuSize(eDiff);
	
			m_iNumSamus++;

			//reset the bool array
			for(int i = 0; i < 9; i++)
				bNumUsed[i] = FALSE;

		}

	}
}

int CKillerPuzzle::GetSamuSize(EnumDifficulty eDiff)
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
        iSamuSize = rand()%5 + 2;
    default:
        break;
    }

    return iSamuSize;
}

BOOL CKillerPuzzle::IsLeftAval(int iRow, int iColumn, BOOL *bNumArray)
{
	if(iColumn > 1)
		if(m_iSamud[iRow][iColumn-1]==-1)
		{	
			if(bNumArray[m_iGeneratedPuzzle[iRow][iColumn-1]-1]== TRUE)			
				return FALSE;
			return TRUE;
		}
	return FALSE;
}

BOOL CKillerPuzzle::IsRightAval(int iRow, int iColumn, BOOL *bNumArray)
{
	if(iColumn < NUM_COLUMNS - 1)
		if(m_iSamud[iRow][iColumn+1]==-1)
		{	
			if(bNumArray[m_iGeneratedPuzzle[iRow][iColumn+1]-1]== TRUE)			
				return FALSE;
			return TRUE;
		}
	return FALSE;

}

BOOL CKillerPuzzle::IsAboveAval(int iRow, int iColumn, BOOL *bNumArray)
{
	if(iRow > 0)
		if(m_iSamud[iRow-1][iColumn]==-1)
		{	//checking for a repeated number
			if(bNumArray[m_iGeneratedPuzzle[iRow-1][iColumn]-1]== TRUE)			
				return FALSE;
			return TRUE;
		}
	return FALSE;
}

BOOL CKillerPuzzle::IsBelowAval(int iRow, int iColumn, BOOL *bNumArray)
{
	if(iRow < NUM_ROWS - 1)
		if(m_iSamud[iRow+1][iColumn]==-1)
		{	
			if(bNumArray[m_iGeneratedPuzzle[iRow+1][iColumn]-1]== TRUE)			
				return FALSE;
			return TRUE;
		}

	return FALSE;

}

BOOL CKillerPuzzle::IsSamuExpansionPossible(int iRow, int iColumn, BOOL *bNumArray)
{
	//check left 
	if(IsLeftAval(iRow, iColumn, bNumArray))
		return TRUE;

	//check right
	if(IsRightAval(iRow, iColumn, bNumArray))
		return TRUE;

	//check below
	if(IsBelowAval(iRow, iColumn, bNumArray))
		return TRUE;
	
	//check above
	if(IsAboveAval(iRow, iColumn, bNumArray))
		return TRUE;

	return FALSE;
}




void CKillerPuzzle::GeneratePuzzle()
{
	// have already seeded the random number generator in the constructor
	
	// generate a puzzle and place it into the variable m_iGeneratedPuzzle
	PopulateCell(0);


}

BOOL CKillerPuzzle::PopulateCell(int iCellIndex)
{
	if (iCellIndex == 81)
		return TRUE; // the board is full!!

	int iRow			= iCellIndex / NUM_ROWS;
	int iColumn			= iCellIndex % NUM_COLUMNS;
	int iCellOrder[]	= {1, 2, 3, 4, 5, 6, 7, 8, 9};	// Note this list will contain 1..9 in a *random* order

	// try setting each possible value in cell
	RandomizeArray(iCellOrder, NUM_DIGITS);

	for (int i = 0; i < NUM_DIGITS; i++)
	{
		// set this test value
		m_iGeneratedPuzzle[iRow][iColumn] = iCellOrder[i];

		// is this board still valid?
		if(IsBoardValid())
		{
			// it is so try to populate next cell
			if (PopulateCell(iCellIndex+1))
			{
				// recursive to the last cell, if this call returned true, then the board is valid
				return TRUE;
			}
		}
	}
	// rollback this cell
	// very critical to do this, otherwise bad values will cause the board to be invalid
	m_iGeneratedPuzzle[iRow][iColumn]	= 0;	
	return FALSE;

}


BOOL CKillerPuzzle::IsBoardValid()
{
	// test constraints
	// every column, row and subsection must be valid
	BOOL bIsValid = TRUE;
	for (int i = 0; i < NUM_COLUMNS; i++)
	{
		if (IsColumnValid(i) &&
			IsRowValid(i) &&
			IsSectionValid(i))
		{
			// do nothing
		}
		else
		{
			bIsValid = FALSE;
		}
	}
	return bIsValid;

}

BOOL CKillerPuzzle::IsColumnValid(int iColumnIndex)
{
	int iTemp;

	// check to make sure only one of each digit 1-9 is used
	for(iTemp = 0; iTemp < NUM_DIGITS; iTemp++)
		m_bNumbers[iTemp] = FALSE;

	for (int k = 0; k < NUM_ROWS; k++)
	{
		iTemp = m_iGeneratedPuzzle[k][iColumnIndex];
		if (iTemp < 1 || iTemp > NUM_DIGITS)
		{
			// no need to check, just continue
			continue;
		}
		if (m_bNumbers[iTemp-1] == TRUE)
			return FALSE;
		else
			m_bNumbers[iTemp-1] = TRUE;
	}
	return TRUE;
}

BOOL CKillerPuzzle::IsRowValid(int iRowIndex)
{
	// check to make sure only one of each digit 1-9 is used
	int iTemp;

	for(iTemp = 0; iTemp < NUM_DIGITS; iTemp++)
		m_bNumbers[iTemp] = FALSE;

	for (int k = 0; k < NUM_COLUMNS; k++)
	{
		iTemp = m_iGeneratedPuzzle[iRowIndex][k];
		if (iTemp < 1 || iTemp > NUM_DIGITS)
		{
			// no need to check, just continue
			continue;
		}
		if (m_bNumbers[iTemp-1] == TRUE)
			return FALSE;
		else
			m_bNumbers[iTemp-1] = TRUE;
	}
	return TRUE;
}

BOOL CKillerPuzzle::IsSectionValid(int iRowIndex, int iColumnIndex)
{
	// find out which section (3x3 matrix) the row and column indexes point to
	int iSectionI	= iRowIndex / 3;
	int iSectionJ	= iColumnIndex / 3;

	// go through each cell in that section and make sure only one of each digit 1-9 is used
	int iTemp;
	for(iTemp = 0; iTemp < NUM_DIGITS; iTemp++)
		m_bNumbers[iTemp] = FALSE;

	for (int i = iSectionI*3; i < iSectionI*3 + 3; i++)
	{
		for (int j = iSectionJ*3; j < iSectionJ*3 + 3; j++)
		{
			iTemp = m_iGeneratedPuzzle[i][j];
			if (iTemp < 1 || iTemp > NUM_DIGITS)
			{
				// no need to check, just continue
				continue;
			}
			if (m_bNumbers[iTemp-1] == TRUE)
				return FALSE;
			else
				m_bNumbers[iTemp-1] = TRUE;
		}
	}
	return TRUE;
}

BOOL CKillerPuzzle::IsSectionValid(int iSectionIndex)
{
	// use a single integer to call isSectionValid(int iRowIndex, int iColumnIndex)
	int iIndexes[]	= {1,4,7};
	int iRow		= iIndexes[iSectionIndex / 3];
	int iColumn		= iIndexes[iSectionIndex % 3];
	return IsSectionValid(iRow, iColumn);
}

void CKillerPuzzle::RandomizeArray(int iArray[], int iSize)
{
	// use the random number generator to make swaps between two array elements
	int iFirstIndex		= 0;
	int iSecondIndex	= 0;
	int iTemp			= 0;
	for ( int i = 0; i < iSize*2; i++ )
	{
		iFirstIndex = (int)((double)rand() / (RAND_MAX + 1) * (iSize));
		iSecondIndex = (int)((double)rand() / (RAND_MAX + 1) * (iSize));
		if (iFirstIndex > iSize - 1)
			iFirstIndex = iSize - 1;
		
		if (iSecondIndex > iSize - 1)
			iSecondIndex = iSize - 1;

		iTemp = iArray[iFirstIndex];
		iArray[iFirstIndex] = iArray[iSecondIndex];
		iArray[iSecondIndex] = iTemp;
	}
	


}

void CKillerPuzzle::ArrayCopy(int iFrom[NUM_ROWS][NUM_COLUMNS], int iTo[NUM_ROWS][NUM_COLUMNS])
{
	memcpy(iTo, iFrom, sizeof(int)*NUM_ROWS*NUM_COLUMNS);
}

void CKillerPuzzle::OutputGrid(TCHAR* szName, int iArray[][9])
{
#ifdef DEBUG
    DebugOut(szName);
    for(int y = 0; y < 9; y++)
        DebugOut(_T("%i  %i  %i  %i  %i  %i  %i  %i  %i"), 
        iArray[y][0], 
        iArray[y][1], 
        iArray[y][2], 
        iArray[y][3], 
        iArray[y][4], 
        iArray[y][5], 
        iArray[y][6], 
        iArray[y][7], 
        iArray[y][8]);

#endif
}

void CKillerPuzzle::OutputKiller()
{
#ifdef DEBUG
    DebugOut(_T("Killer"));
    for(int y = 0; y < 9; y++)
    {
        DebugOut(_T("%.2i %.2i %.2i %.2i %.2i %.2i %.2i %.2i %.2i"), 
            GetSamuValue(y,0),
            GetSamuValue(y,1),
            GetSamuValue(y,2),
            GetSamuValue(y,3),
            GetSamuValue(y,4),
            GetSamuValue(y,5),
            GetSamuValue(y,6),
            GetSamuValue(y,7),
            GetSamuValue(y,8));
    }
    
#endif
}

//we're assuming the y is the first value in the array but it shouldn't matter
BOOL CKillerPuzzle::FixNonSolvable(int iPuzzleArray[][9], int iSolutionArray[][9])
{
    BOOL bRet = TRUE;

#ifdef DEBUG
    //doesn't seem to quite work
    OutputGrid(_T("Solution"), iSolutionArray);
    OutputKiller();
#endif

    POINT ptNum1;
    POINT ptNum2;
    POINT ptNum3;
    POINT ptNum4;
    POINT ptNum5;
    POINT ptNum6;

    //so we basically need to go through and make one comparison below and one to the right 
    //and we don't need to check the bottom or the right 3 rows/columns

    
    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 8; y++)
        {
            //now find the value ptNum2 below ptNum1.x
            //ie ... Horizontals
            for(int j = y; j <9; j++)
            {
                if(iSolutionArray[y][x+1] == iSolutionArray[j][x] &&
                    iSolutionArray[y][x] == iSolutionArray[j][x+1])
                {
                    //we found a potential problem
                    //check if its unsolvable
                    if(GetSamuIndex(y,x) == GetSamuIndex(y, x+1) &&
                        GetSamuIndex(j,x) == GetSamuIndex(j, x+1))
                    {
                        DebugOut(_T("(%i, %i)  (%i, %i)  [%i][%i] Non-unique"), x,y,x+1,j,iSolutionArray[y][x], iSolutionArray[y][x+1]);
                        ptNum1.y = x;
                        ptNum1.x = y;
                        ptNum2.y = x+1;
                        ptNum2.x = y;


                        ptNum3.y = x;
                        ptNum3.x = j;
                        ptNum4.y = x+1;
                        ptNum4.x = j;

                        if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, TRUE))
                        { }
                        else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, TRUE))
                        { }
                        else
                        {
                            bRet = FALSE;
                            goto error;
                        }
                        

                    }
                    else
                        DebugOut(_T("(%i, %i)  (%i, %i)  [%i][%i]"), x,y,x+1,j,iSolutionArray[y][x], iSolutionArray[y][x+1]);
                }
                else if(iSolutionArray[y][x+1] == iSolutionArray[j][x])
                {
                    for(int j2 = j; j2 < 9; j2++)
                    {
                        if(iSolutionArray[y][x] == iSolutionArray[j2][x+1] &&
                            iSolutionArray[j2][x] == iSolutionArray[j][x+1])   
                        {
                            if(GetSamuIndex(y,x) == GetSamuIndex(y, x+1) &&
                                GetSamuIndex(j,x) == GetSamuIndex(j, x+1) &&
                                GetSamuIndex(j2,x) == GetSamuIndex(j2, x+1))
                            {
                                ptNum1.y = x;
                                ptNum1.x = y;
                                ptNum2.y = x+1;
                                ptNum2.x = y;

                                ptNum3.y = x;
                                ptNum3.x = j;
                                ptNum4.y = x+1;
                                ptNum4.x = j;
                                
                                ptNum5.y = x;
                                ptNum5.x = j2;
                                ptNum6.y = x+1;
                                ptNum6.x = j2;

                                DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i][%i][%i] h1 Non-unique"), x,y,x,j,x,j2,iSolutionArray[y][x], iSolutionArray[y][x+1], iSolutionArray[j][x+1]);

                                if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, TRUE))
                                { }
                                else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, TRUE))
                                { }
                                else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum5, ptNum6, TRUE))
                                { }
                                else
                                {
                                    bRet = FALSE;
                                    goto error;
                                }
                            }
                            else 
                                DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i][%i][%i] h1"), x,y,x,j,x,j2,iSolutionArray[y][x], iSolutionArray[y][x+1], iSolutionArray[j][x+1]);
                        }
                    }
                }
                else if(iSolutionArray[y][x] == iSolutionArray[j][x+1])
                {
                    for(int j2 = j; j2 < 9; j2++)
                    {
                        if(iSolutionArray[y][x+1] == iSolutionArray[j2][x] &&
                            iSolutionArray[j2][x+1] == iSolutionArray[j][x])   
                        {
                            if(GetSamuIndex(y,x) == GetSamuIndex(y, x+1) &&
                                GetSamuIndex(j,x) == GetSamuIndex(j, x+1) &&
                                GetSamuIndex(j2,x) == GetSamuIndex(j2, x+1))
                            {
                                ptNum1.y = x;
                                ptNum1.x = y;
                                ptNum2.y = x+1;
                                ptNum2.x = y;

                                ptNum3.y = x;
                                ptNum3.x = j;
                                ptNum4.y = x+1;
                                ptNum4.x = j;
                                
                                ptNum5.y = x;
                                ptNum5.x = j2;
                                ptNum6.y = x+1;
                                ptNum6.x = j2;

                                DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i][%i][%i] h2 Non-unique"), x,y,x,j,x,j2,iSolutionArray[y][x], iSolutionArray[y][x+1], iSolutionArray[j][x]);

                                if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, TRUE))
                                { }
                                else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, TRUE))
                                { }
                                else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum5, ptNum6, TRUE))
                                { }
                                else
                                {
                                    bRet = FALSE;
                                    goto error;
                                }
                            }

                            else 
                                DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i][%i][%i] h2"), x,y,x,j,x,j2,iSolutionArray[y][x], iSolutionArray[y][x+1], iSolutionArray[j][x]);
                        }
                    }
                }
            }

            //now lets check the verticals
            for(int i = x; i <9; i++)
            {
                if(iSolutionArray[y+1][x] == iSolutionArray[y][i] &&
                    iSolutionArray[y][x] == iSolutionArray[y+1][i])
                {
                    //we found a potential problem
                    //check if its unsolvable
                    if(GetSamuIndex(y,x) == GetSamuIndex(y+1, x) &&
                        GetSamuIndex(y,i) == GetSamuIndex(y+1, i))
                    {
                        DebugOut(_T("(%i, %i)  (%i, %i)  [%i][%i] Non-unique"), x,y,i,y+1,iSolutionArray[y][x], iSolutionArray[y+1][x]);
                        ptNum1.y = x;
                        ptNum1.x = y;
                        ptNum2.y = x;
                        ptNum2.x = y+1;

                        ptNum3.y = i;
                        ptNum3.x = y;
                        ptNum4.y = i;
                        ptNum4.x = y+1;

                        if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, FALSE))
                        {}
                        else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, FALSE))
                        {}
                        else
                        {
                            bRet = FALSE;
                            goto error;
                        }
                    }
                    else
                        DebugOut(_T("(%i, %i)  (%i, %i)  [%i][%i]"), x,y,i,y+1,iSolutionArray[y][x], iSolutionArray[y+1][x]);
                }
                else if(iSolutionArray[y][x] == iSolutionArray[y+1][i])
                {
                    for(int i2 = i; i2 < 9; i2++)
                    {
                        if(iSolutionArray[y][i] == iSolutionArray[y+1][i2] &&
                            iSolutionArray[y][i2] == iSolutionArray[y+1][x])   
                        {
                            if(GetSamuIndex(y,x) == GetSamuIndex(y+1, x) &&
                                GetSamuIndex(y,i) == GetSamuIndex(y+1, i) &&
                                GetSamuIndex(y,i2) == GetSamuIndex(y+1, i2))
                            {
                                DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i][%i][%i] v1 Non-unique"), x,y,i,y,i2,y,iSolutionArray[y][x], iSolutionArray[y+1][x], iSolutionArray[y][i]);
                                ptNum1.y = x;
                                ptNum1.x = y;
                                ptNum2.y = x;
                                ptNum2.x = y+1;

                                ptNum3.y = i;
                                ptNum3.x = y;
                                ptNum4.y = i;
                                ptNum4.x = y+1;

                                ptNum5.y = i2;
                                ptNum5.x = y;
                                ptNum6.y = i2;
                                ptNum6.x = y+1;

                                if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, FALSE))
                                {}
                                else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, FALSE))
                                {}
                                else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum5, ptNum6, FALSE))
                                {}
                                else
                                {
                                    bRet = FALSE;
                                    goto error;
                                }
                                

                            }
                            else
                                DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i][%i][%i] v1"), x,y,i,y,i2,y,iSolutionArray[y][x], iSolutionArray[y+1][x], iSolutionArray[y][i]);
                        }
                    }
                }

                else if(iSolutionArray[y+1][x] == iSolutionArray[y][i])
                {
                    for(int i2 = i; i2 < 9; i2++)
                    {
                        if(iSolutionArray[y+1][i] == iSolutionArray[y][i2] &&
                            iSolutionArray[y+1][i2] == iSolutionArray[y][x])   
                        {
                            if(GetSamuIndex(y,x) == GetSamuIndex(y+1, x) &&
                                GetSamuIndex(y,i) == GetSamuIndex(y+1, i) &&
                                GetSamuIndex(y,i2) == GetSamuIndex(y+1, i2))
                            {
                                DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i][%i][%i] v2 Non-unique"), x,y,i,y,i2,y,iSolutionArray[y][x], iSolutionArray[y+1][x], iSolutionArray[y+1][i]);
                             
                                ptNum1.y = x;
                                ptNum1.x = y;
                                ptNum2.y = x;
                                ptNum2.x = y+1;

                                ptNum3.y = i;
                                ptNum3.x = y;
                                ptNum4.y = i;
                                ptNum4.x = y+1;

                                ptNum5.y = i2;
                                ptNum5.x = y;
                                ptNum6.y = i2;
                                ptNum6.x = y+1;

                                if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, FALSE))
                                {}
                                else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, FALSE))
                                {}
                                else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum5, ptNum6, FALSE))
                                {}
                                else
                                {
                                    bRet = FALSE;
                                    goto error;
                                }


                            }
                            else
                                DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i][%i][%i] v2"), x,y,i,y,i2,y,iSolutionArray[y][x], iSolutionArray[y+1][x], iSolutionArray[y+1][i]);
                        }
                    }
                }
            }




        }
    }
error:

    return bRet;
}


BOOL CKillerPuzzle::SplitKiller(int iPuzzleArray[][9], int iSolutionArray[][9], POINT pt1, POINT pt2, BOOL bHorizontal)
{
    

    //so we need to add a new samu section and then check if we need to modify any other values/samus
    //lets just always change the second and work our way down
    m_iNumSamus++;

    int iSamuIndex1 = GetSamuIndex(pt1.x, pt1.y);
    int iSamuIndex2 = GetSamuIndex(pt2.x, pt2.y);

    int iSamuID = GetSamuIndex(pt2.x, pt2.y);

    if(m_iSamuCounts[iSamuID] != 2)
        return FALSE;

    m_iSamuCounts[iSamuID]--; //reduce the count by one
    m_iSamunamupures[iSamuID] -= iSolutionArray[pt2.x][pt2.y];

    
    m_iSamud[pt2.x][pt2.y] = m_iNumSamus-1;;
    m_iSamuCounts[m_iNumSamus-1] = 1;
    m_iSamunamupures[m_iNumSamus-1] = iSolutionArray[pt2.x][pt2.y];

    iPuzzleArray[pt2.x][pt2.y] = m_iNumSamus-1;


    return TRUE;
}


//1 check per number per subset pair
//slight optimization ... only check 1-8 ... 9 will be redundant
//we don't really have to worry about which is x or y as long as we're consistent
//m_iGeneratedPuzzle[iRow][iColumn]
BOOL CKillerPuzzle::FixNonSolvable2(int iPuzzleArray[][9], int iSolutionArray[][9])
{
    POINT ptNum1;
    POINT ptNum2;
    POINT ptCheck1;
    POINT ptCheck2;

    POINT ptSubSet1;
    POINT ptSubSet2;

    int iXTemp, iYTemp;

#ifdef DEBUG
    //doesn't seem to quite work
    OutputGrid(_T("Solution"), iSolutionArray);
    OutputKiller();
#endif

    //so although this looks gross we can control everything right here
    //******************************

    //we need to do every comparison between 1,1 & 3,3 without overlap 
    //this is to compare large sections ... ie the 3x3s or subsets to one another
    for(int iTemp1 = 0; iTemp1 < 9; iTemp1++)
    {
        for(int iTemp2 = 0; iTemp2 < 9; iTemp2++)
            //******************************
        {
            if(iTemp2 <= iTemp1)
                continue;

            ptSubSet1.x = iTemp1%3;
            ptSubSet1.y = iTemp1/3;

            ptSubSet2.x = iTemp2%3;
            ptSubSet2.y = iTemp2/3;

            //gonna cheat this
            if(ptSubSet1.x != ptSubSet2.x &&
                ptSubSet1.y != ptSubSet2.y)
                continue; //gotta be in the same row or column
            //check the values 1-8 .. since we need pairs the 9s will be found indirectly
            for(int iNum = 1; iNum < 9; iNum++)//changed to 10 from 8 ??
            {    
                //to start we'll compare 1s between the first subset and the 2nd
                //now loop through all comparisons between the two subsets
                //now here we can actually limit our checks as we need a proper "box"
                for(int x = 0; x < 3; x++)
                {
                    for(int y = 0; y < 3; y++)
                    {   
                        iXTemp = x + ptSubSet1.x * 3;      
                        iYTemp = y + ptSubSet1.y * 3;

                        if(iNum == iSolutionArray[iYTemp][iXTemp])
                        {   //found it in the first set
                            ptNum1.x = iXTemp;
                            ptNum1.y = iYTemp;
                        }

                        iXTemp = x + ptSubSet2.x * 3;      
                        iYTemp = y + ptSubSet2.y * 3;

                        if(iNum == iSolutionArray[iYTemp][iXTemp])
                        {   //found it in the 2nd
                            ptNum2.x = iXTemp;
                            ptNum2.y = iYTemp;
                        }
                    }
                }

                //ok ... we have our two points ... now set the check points and see if they're equal .. ie corners
                ptCheck1.x = ptNum1.x;
                ptCheck1.y = ptNum2.y;
                ptCheck2.x = ptNum2.x;
                ptCheck2.y = ptNum1.y;

                //so here we can check for non-useful pairs ... not to worried about performance
                //so first if the subsets are in the same column .... the ptCheck must be in the same row as the 
                //corresponding ptNum
                if(ptSubSet1.x == ptSubSet2.x)
                {
                    if(ptNum1.y != ptCheck1.y)
                        continue;
                }
                else
                {
                    if(ptNum1.x != ptCheck1.x)
                        continue;
                }



                //fingers crossed this is correct ... pretty sure it is
                if(iSolutionArray[ptCheck2.y][ptCheck2.x] == iSolutionArray[ptCheck1.y][ptCheck1.x])
                {
                    //now if the pairs are in the same samu we need to split it up ... easier said than done ....
                    //the x,y pairs would appear to be backwards ... need to check
                    if((GetSamuValue(ptCheck1.y, ptCheck1.x) == GetSamuValue(ptNum1.y, ptNum1.x) &&
                        GetSamuValue(ptCheck2.y, ptCheck2.x) == GetSamuValue(ptNum2.y, ptNum2.x)) ||
                        //gotta do vertical and horizontal
                        (GetSamuValue(ptCheck1.y, ptCheck1.x) == GetSamuValue(ptNum2.y, ptNum2.x) &&
                        GetSamuValue(ptCheck2.y, ptCheck2.x) == GetSamuValue(ptNum1.y, ptNum1.x)) )
                    {
                        //time to split the outline ... this isn't exactly easy
                        int iNewID = m_iNumSamus;//new sumu id
                        int iPrevID = GetSamuIndex(ptCheck2.y, ptCheck2.x);//just picked this one .. any of the 4 would work
                        
                        m_iNumSamus++;//increase our id count

                        //so lets change the first value
                        POINT ptRef;
                        //1s are together
                        if(GetSamuValue(ptCheck1.y, ptCheck1.x) == GetSamuValue(ptNum1.y, ptNum1.x))
                            ptRef = ptNum2;
                        else //1s not together
                            ptRef = ptNum1;

                        POINT ptCurCell = ptCheck2;//we chose that above
                        m_iSamud[ptCurCell.y][ptCurCell.x] = iNewID;
                        m_iSamunamupures[iNewID] += iSolutionArray[ptCurCell.y][ptCurCell.x];
                        m_iSamuCounts[iNewID]++;

                        m_iSamunamupures[iPrevID] -= iSolutionArray[ptCurCell.y][ptCurCell.x];
                        m_iSamuCounts[iPrevID]--;


                        if(m_iSamunamupures[iPrevID] < 0)
                            return FALSE;//never hits so we're good ...
                        if(m_iSamunamupures[iNewID] < 0)
                            return FALSE;

                            //so this will work but we have to be concerned with discontinuous samus now .. 
                        //ie .. did we cut one in half?

                        POINT ptNextCell;

                        while(0 == 0)
                        {   //check all 4 directions
                            ptNextCell = ptCurCell;
                            //right
                            if(ptNextCell.x + 1 < 9 && 
                                ((ptNextCell.x + 1 != ptRef.x) || (ptNextCell.y != ptRef.y)) &&//as long as one is true
                                (GetSamuValue(ptNextCell.y, ptNextCell.x+1) == GetSamuValue(ptRef.y, ptRef.x)) )
                            {
                                ptNextCell.x++;
                            }//left
                            else if(ptNextCell.x - 1 >= 0 && 
                                ((ptNextCell.x - 1 != ptRef.x) || (ptNextCell.y != ptRef.y)) &&//as long as one is true
                                (GetSamuValue(ptNextCell.y, ptNextCell.x-1) == GetSamuValue(ptRef.y, ptRef.x)) )
                            {
                                ptNextCell.x--;
                            }//up
                            else if(ptNextCell.y + 1 < 9 && 
                                ((ptNextCell.x != ptRef.x) || (ptNextCell.y +1 != ptRef.y)) &&//as long as one is true
                                (GetSamuValue(ptNextCell.y+1, ptNextCell.x) == GetSamuValue(ptRef.y, ptRef.x)) )
                            {
                                ptNextCell.y++;
                            }//down
                            else if(ptNextCell.y - 1 >= 0 && 
                                ((ptNextCell.x != ptRef.x) || (ptNextCell.y -1 != ptRef.y)) &&//as long as one is true
                                (GetSamuValue(ptNextCell.y+1, ptNextCell.x) == GetSamuValue(ptRef.y, ptRef.x)) )

                            {
                                ptNextCell.y--;
                            }  //done   
                            else
                            {
                                break; //we're done
                            }     

                            ptCurCell = ptNextCell;

                            //so now we need to update ....
                            m_iSamud[ptCurCell.y][ptCurCell.x] = iNewID;
                            m_iSamunamupures[iNewID] += iSolutionArray[ptCurCell.y][ptCurCell.x];
                            m_iSamuCounts[iNewID]++;

                            m_iSamunamupures[iPrevID] -= iSolutionArray[ptCurCell.y][ptCurCell.x];
                            m_iSamuCounts[iPrevID]--;

                            OutputKiller();

                            if(m_iSamunamupures[iPrevID] < 0)
                                return FALSE;
                            if(m_iSamunamupures[iNewID] < 0)
                                return FALSE;
                        }
                    }
                }
            }
        }
    }
    return TRUE;

}