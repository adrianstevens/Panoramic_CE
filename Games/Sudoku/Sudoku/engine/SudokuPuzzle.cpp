#include "StdAfx.h"
#include "SudokuPuzzle.h"
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



CSudokuPuzzle::CSudokuPuzzle(void)
{
	m_oStr = CIssString::Instance();

	srand((unsigned)GetTickCount());
	// initialize array to all zeros
	int i;
	for (i = 0; i < NUM_ROWS; i++)
	{
		for (int j = 0; j < NUM_COLUMNS; j++)
		{
			m_iGeneratedPuzzle[i][j] = 0;
		}
	}

	// initialize array to all FALSE values
	for(i = 0; i < NUM_DIGITS; i++)
		m_bNumbers[i] = FALSE;

	int iRand = rand();
//	if(iRand > RAND_MAX/2)
		m_bTruelyRandom = TRUE;
//	else 
//		m_bTruelyRandom = FALSE;

}

CSudokuPuzzle::~CSudokuPuzzle(void)
{

}

void CSudokuPuzzle::MyQSort(int iSortArray[81], int iCompanionArray[81], int iFirst, int iLast)
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

int CSudokuPuzzle::Cardinality(int iPuzzleArray[NUM_ROWS][NUM_COLUMNS], int iRow, int iColumn)
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

void CSudokuPuzzle::GetNewHardPuzzle(int iPuzzleArray[NUM_ROWS][NUM_COLUMNS], int iSolutionArray[NUM_ROWS][NUM_COLUMNS], HINSTANCE hInst)
{
    // try and load up the text from the resource
    HRSRC hres = FindResource(hInst, _T("DAT_HANSWERS"), _T("DAT"));
    HRSRC hpuzzle = FindResource(hInst, _T("DAT_HPUZZLES"), _T("DAT"));

    if (hres == 0 || hpuzzle == 0)
    { 
        // did not find it
        return;
    }

    //If resource is found a handle to the resource is returned
    //now just load the resource
    HGLOBAL hbytes = LoadResource(hInst, hres);
    LPVOID pdata	= LockResource(hbytes);
    UINT resLen		= SizeofResource(hInst, hres);

    HGLOBAL hbytesP = LoadResource(hInst, hpuzzle);
    LPVOID pdataP	= LockResource(hbytesP);
    UINT resLenP    = SizeofResource(hInst, hpuzzle);

  //  char* puzzles = new char[resLen];

    //assume the files are identical sizes ... because they need to be
    int iLineLength = 81+2;
    int iNumPuzzles = resLen/(iLineLength);

    int iPuzzleIndex = rand()%(iNumPuzzles-1);

    char* szAnswer = m_oStr->CreateAndCopyChar((char*)pdata, iPuzzleIndex*iLineLength, iLineLength-2);
    char* szPuzzle = m_oStr->CreateAndCopyChar((char*)pdataP, iPuzzleIndex*iLineLength, iLineLength-2);

    //copy the answer into the array
    for(int x = 0; x < 9; x++)
    {
        for(int y = 0; y < 9; y++)
        {
            iSolutionArray[y][x] = szAnswer[x+9*y] - '0';
            iPuzzleArray[y][x] = szPuzzle[x+9*y] - '0';
        }
    }

    delete szPuzzle;
    szPuzzle = NULL;

    delete szAnswer;
    szAnswer = NULL;

    SwapNumbers(iPuzzleArray, iSolutionArray);//mix em a bit
    ManipulatePuzzle(iPuzzleArray, iSolutionArray);

#ifdef DEBUG
    if(IsBoardValid())
        DebugOut(_T("Hard Puzzle Is Valid"));
    else
        DebugOut(_T("Warning - Invalid Puzzle"));
#endif
}

void CSudokuPuzzle::SwapNumbers(int iPuzzleArray[][9], int iSolutionArray[][9], int iNumSwaps /* =/* = 3 */)
{
    int iFrom;
    int iTo;

    for(int iSwaps = 0; iSwaps < iNumSwaps; iSwaps++)
    {
        iFrom = rand()%9+1;
        iTo = rand()%9+1;

        if(iFrom == iTo)
            continue;

        for(int x = 0; x < 9; x++)
        {
            for(int y = 0; y < 9; y++)
            {
                if(iPuzzleArray[x][y] == iFrom)
                    iPuzzleArray[x][y] = iTo;
                else if(iPuzzleArray[x][y] == iTo)
                    iPuzzleArray[x][y] = iFrom;

                if(iSolutionArray[x][y] == iFrom)
                    iSolutionArray[x][y] = iTo;
                else if(iSolutionArray[x][y] == iTo)
                    iSolutionArray[x][y] = iFrom;
            }
        }
    }
}

void CSudokuPuzzle::GetNewPuzzle(EnumDifficulty eDifficulty, int iPuzzleArray[][9], int iSolutionArray[][9], HINSTANCE hInst)
{
//#ifndef DEBUG 
    if(eDifficulty == DIFF_Hardcore)
//#endif
    {
        GetNewHardPuzzle(iPuzzleArray, iSolutionArray, hInst);
        return;
    }



	// run through all this code in a while loop until a puzzle with very few given cells is found
	// very few meaning less than MAX_CELLS
	int iTempArray[NUM_ROWS][NUM_COLUMNS];
	int iCardinalityArray[81];
	int iIndexingArray[81];
	int iRemoveArray[81];
	int iRow					= 0;
	int iColumn					= 0;
	int iRemoveIndex			= 0;
	BOOL bSolvable				= TRUE;
	int iSolverResult			= -1;
	int iCellsHidden			= 0;
	int iNumLoops				= 0;
	int iRandomX				= 0;
	int iRandomY				= 0;
	int iHighCardinality		= 0;
	int iRowHighCardinality		= 0;
	int iColumnHighCardinality	= 0;

	int iRemoveTarget = NUM_CELLS_REMOVE_TARGET;


	// determine number of extra cells to add to make the puzzle easier, once we have a solvable puzzle
	switch(eDifficulty)
	{
	case DIFF_VeryHard:
		iRemoveTarget -= 0;
		break;
	case DIFF_Hard:
		iRemoveTarget -= 4;
		break;
	case DIFF_Medium:
		iRemoveTarget -= 7;
		break;
	case DIFF_Easy:
		iRemoveTarget -= 11;
		break;
	case DIFF_VeryEasy:
	default:
		iRemoveTarget -= 14;
		break;
	}

	while (iCellsHidden < iRemoveTarget)
	{
		bSolvable = TRUE;
		iNumLoops++;
		iRemoveIndex = 0;
		// generate a puzzle and copy m_iGeneratedPuzzle[NUM_ROWS][NUM_COLUMNS] to the arguement
		// blank out the puzzle array
		GeneratePuzzle();
		memcpy(iTempArray, m_iGeneratedPuzzle, sizeof(int)*NUM_COLUMNS*NUM_ROWS);
		for (int i = 0; i < NUM_ROWS; i++)
		{
			for (int j = 0; j < NUM_COLUMNS; j++)
			{
				iSolutionArray[i][j]	= m_iGeneratedPuzzle[i][j];
				//iTempArray[i][j]		= m_iGeneratedPuzzle[i][j];
				iPuzzleArray[i][j]		= m_iGeneratedPuzzle[i][j];
			}
		}

		// create a 1D array with values from 0 to 80, randomized, to show the order of cell elements that are removed
		for (int i = 0; i < 81; i++)
			iRemoveArray[i] = i;
		
		RandomizeArray(iRemoveArray, 81);

		// Now remove random cells from the solution array to the temp array
		for (int i = 0; i < NUM_FIRST_REMOVE; i++)
		{
			// remove the random cell
			iRow	= iRemoveArray[i] / NUM_COLUMNS;
			iColumn	= iRemoveArray[i] % NUM_COLUMNS;
			// add number to cell and increment
			iTempArray[iRow][iColumn]	= 0;
			iPuzzleArray[iRow][iColumn] = 0;
			iRemoveIndex++;
		}

		// now iterate (hide cells with highest cardinality) by checking if the puzzle can be solved, is unique, and has the desired difficulty
		// Solve the new puzzle to get the values for every cell
		iSolverResult = m_oSolver.sudoku_solve(iTempArray);

		while (bSolvable == TRUE)
		{
			// returns 0	- Puzzle unsolvable
			// returns 1	- Puzzle solved
			// returns >1	- Puzzle invalid
			if (iRemoveIndex > 64)
			{
				// removed too many cells to have a solution, ever
				iRemoveIndex = 0;
				bSolvable = FALSE;
			}
			if (iRemoveIndex >= (iRemoveTarget)
				&& iSolverResult == 1)
			{
				// solved, and unique.  We're finished so exit the while loop
				bSolvable = TRUE;
				iRemoveIndex++;
				break;
			}
			//Still have values to remove to meet our target
			else if (iRemoveIndex < (iRemoveTarget)
				&& iSolverResult == 1)
			{
				int iTempValue;
				int iTempIndex;

				if(m_bTruelyRandom == FALSE)
				{
					// go through the puzzle and compute the cardinality of each cell
					for (int i = 0; i < 81; i++)
					{
						iCardinalityArray[i] = 0;
						iIndexingArray[i] = 0;
					}
					for (int i = 0; i < NUM_ROWS; i++)
					{
						for (int j = 0; j < NUM_COLUMNS; j++)
						{
							// only check cells with a digit in it (other than zero)
							if (iPuzzleArray[i][j] != 0)
							{
								iCardinalityArray[ INDEX(i,j) ] = Cardinality(iPuzzleArray, i, j) ;
								iIndexingArray[INDEX(i,j)] = INDEX(i,j);
							}
						}
					}
					// sort the arrays
					MyQSort(iCardinalityArray, iIndexingArray, 0, 80);

					// remove number from cell, starting with the highest cardinality, then going lower
					// check for a solution, and if none found then quit
						iTempValue = 0;
						iTempIndex = 80;
				}
				else //truly random method...makes for more interesting puzzles
				{
					iTempIndex = iRemoveIndex;
					memcpy(iIndexingArray, iRemoveArray, 81*sizeof(int));//quick array copy
				}
				do 
				{
					iTempValue = iPuzzleArray[ROW(iIndexingArray[iTempIndex])][COLUMN(iIndexingArray[iTempIndex])];
					iPuzzleArray[ROW(iIndexingArray[iTempIndex])][COLUMN(iIndexingArray[iTempIndex])] = 0;
					// must copy the puzzle array into the temp variable because the array is modified by the solver
					ArrayCopy(iPuzzleArray, iTempArray);
					iSolverResult = m_oSolver.sudoku_solve(iTempArray);
					if (iSolverResult == 1)
					{
						// do nothing
						break;
					}
					else
					{
						// place value remove back into the puzzle
						iPuzzleArray[ROW(iIndexingArray[iTempIndex])][COLUMN(iIndexingArray[iTempIndex])] = iTempValue;
						if(m_bTruelyRandom)
							iTempIndex++;
						else
							iTempIndex--;
					}

				}while(iSolverResult != 1 && 
						iPuzzleArray[ROW(iTempIndex)][COLUMN(iTempIndex)] != 0 && 
						iTempIndex > -1);
				
				// check if we should stop trying this puzzle and generate a new one
				if (iSolverResult != 1)
				{
					bSolvable = FALSE;
				}
				else
				{
					ArrayCopy(iPuzzleArray, iTempArray);
					iRemoveIndex++;
				}
			}
			else
			{
				// we want to stop and go to the start of while(iNumGivenElements)
				iRemoveIndex = 0;	// make sure we run another loop through while(iNumGivenElements)
				bSolvable = FALSE;
			}

		} // end of while (bSolvable == TRUE)
		iCellsHidden = iRemoveIndex;

	} // end of while(iNumGivenElements), check value of iNumGivenElements


    //and finally ... check for unsolvable ... not ideal but it'll work
    FixNonSolvable(iPuzzleArray, iSolutionArray);

	// now we have a puzzle with the desired difficulty level, so end the function
	return;
}

void CSudokuPuzzle::GeneratePuzzle()
{
	// have already seeded the random number generator in the constructor
	// generate a puzzle and place it into the variable m_iGeneratedPuzzle
	PopulateCell(0);
}

BOOL CSudokuPuzzle::PopulateCell(int iCellIndex)
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
			{	// recursive to the last cell, if this call returned true, then the board is valid
				return TRUE;
			}
		}
	}
	// rollback this cell
	// very critical to do this, otherwise bad values will cause the board to be invalid
	m_iGeneratedPuzzle[iRow][iColumn]	= 0;	
	return FALSE;

}


BOOL CSudokuPuzzle::IsBoardValid()
{
	// test constraints
	// every column, row and subsection must be valid
	for (int i = 0; i < NUM_COLUMNS; i++)
	{
		if(!IsColumnValid(i))
			return FALSE;
		if(!IsRowValid(i))
			return FALSE;
		if(!IsSectionValid(i))
			return FALSE;
	}
	return TRUE;

}

BOOL CSudokuPuzzle::IsColumnValid(int iColumnIndex)
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

BOOL CSudokuPuzzle::IsRowValid(int iRowIndex)
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

BOOL CSudokuPuzzle::IsSectionValid(int iRowIndex, int iColumnIndex)
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

BOOL CSudokuPuzzle::IsSectionValid(int iSectionIndex)
{
	// use a single integer to call isSectionValid(int iRowIndex, int iColumnIndex)
	int iIndexes[]	= {1,4,7};
	int iRow		= iIndexes[iSectionIndex / 3];
	int iColumn		= iIndexes[iSectionIndex % 3];
	return IsSectionValid(iRow, iColumn);
}

void CSudokuPuzzle::RandomizeArray(int iArray[], int iSize)
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

void CSudokuPuzzle::ArrayCopy(int iFrom[NUM_ROWS][NUM_COLUMNS], int iTo[NUM_ROWS][NUM_COLUMNS])
{
	memcpy(iTo, iFrom, sizeof(int)*NUM_ROWS*NUM_COLUMNS);
}

//1 check per number per subset pair
//slight optimization ... only check 1-8 ... 9 will be redundant
//we don't really have to worry about which is x or y as long as we're consistent
//m_iGeneratedPuzzle[iRow][iColumn]
void CSudokuPuzzle::FixNonSolvable(int iPuzzleArray[][9], int iSolutionArray[][9])
{
    POINT ptNum1;
    POINT ptNum2;
    POINT ptCheck1;
    POINT ptCheck2;

    POINT ptSubSet1;
    POINT ptSubSet2;

    int iXTemp, iYTemp;

    //so although this looks gross we can control everything right here
    //******************************
   
    //we need to do every comparison between 1,1 & 3,3 without overlap 
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

            for(int iNum = 1; iNum < 9; iNum++)//1-8 .. 9 will be found indirectly ...
            {    
                //to start we'll compare 1s between the first subset and the 2nd
         
                for(int x = 0; x < 3; x++)
                {
                    for(int y = 0; y < 3; y++)
                    {   
                        iXTemp = x + ptSubSet1.x * 3;      
                        iYTemp = y + ptSubSet1.y * 3;

                        if(iNum == iSolutionArray[iYTemp][iXTemp])
                        {
                            ptNum1.x = iXTemp;
                            ptNum1.y = iYTemp;
                        }

                        iXTemp = x + ptSubSet2.x * 3;      
                        iYTemp = y + ptSubSet2.y * 3;

                        if(iNum == iSolutionArray[iYTemp][iXTemp])
                        {
                            ptNum2.x = iXTemp;
                            ptNum2.y = iYTemp;
                        }
                    }
                }

                //ok ... we have our two points ... now set the check points and see if they're equal
                ptCheck1.x = ptNum1.x;
                ptCheck1.y = ptNum2.y;
                ptCheck2.x = ptNum2.x;
                ptCheck2.y = ptNum1.y;

                if(iSolutionArray[ptCheck2.y][ptCheck2.x] == iSolutionArray[ptCheck1.y][ptCheck1.x])
                {
                    //we have a possible issue .... 
                    if(iPuzzleArray[ptCheck2.y][ptCheck2.x] == 0 &&
                        iPuzzleArray[ptCheck1.y][ptCheck1.x] == 0 &&
                        iPuzzleArray[ptNum1.y][ptNum1.x] == 0 &&
                        iPuzzleArray[ptNum2.y][ptNum2.x] == 0) //only if all 4 are blank do we care
                        iPuzzleArray[ptCheck1.y][ptCheck1.x] = iSolutionArray[ptCheck1.y][ptCheck1.x];//this "should" just work but never been able to repro
                }

            }
        }

    }
  
}










void CSudokuPuzzle::ManipulatePuzzle(int iPuzzleArray[][9], int iSolutionArray[][9])
{		
	// randomly perform a matrix manipulation on the current puzzle
    int iPick;

    for(int i=0; i < 2; i++)
	{
		iPick = rand()%10;

		switch(iPick)
		{
		case 0:
			Rotate90(iPuzzleArray, iSolutionArray);
			break;
		case 1:
			Rotate180(iPuzzleArray, iSolutionArray);
			break;
		case 3:
			Rotate270(iPuzzleArray, iSolutionArray);
			break;
		case 4:
			MirrorXY(iPuzzleArray, iSolutionArray);
			break;
		case 5:
			MirrorHorizontal(iPuzzleArray, iSolutionArray);
			break;
		case 6:
			MirrorVertical(iPuzzleArray, iSolutionArray);
			break;
		case 7:
			SwapColumnSections(iPuzzleArray, iSolutionArray);
			break;
		case 8:
			SwapRowSections(iPuzzleArray, iSolutionArray);
			break;
		case 9:
			SwapColumnSingle(iPuzzleArray, iSolutionArray);
			break;
		default:
			SwapRowSingle(iPuzzleArray, iSolutionArray);
			break;
		}
	}
}

void CSudokuPuzzle::Rotate90(int iPuzzleArray[][9], int iSolutionArray[][9])
{
    int	arrTempSol[NUM_ROWS][NUM_COLUMNS];
    int	arrTempPuz[NUM_ROWS][NUM_COLUMNS];

	// set the temp matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			arrTempSol[x][y]	= iSolutionArray[8-y][x];
			arrTempPuz[x][y]	= iPuzzleArray[8-y][x];
		}
	}
	
	// now reset the original matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			iSolutionArray[x][y] = arrTempSol[x][y];
			iPuzzleArray[x][y]   = arrTempPuz[x][y];
		}
	}		
}

void CSudokuPuzzle::Rotate270(int iPuzzleArray[][9], int iSolutionArray[][9])
{
    int	arrTempSol[NUM_ROWS][NUM_COLUMNS];
    int	arrTempPuz[NUM_ROWS][NUM_COLUMNS];

	// set the temp matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			arrTempSol[8-y][x]	= iSolutionArray[x][y];
			arrTempPuz[8-y][x]	= iPuzzleArray[x][y];
		}
	}
	
	// now reset the original matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			iSolutionArray[x][y] = arrTempSol[x][y];
			iPuzzleArray[x][y]   = arrTempPuz[x][y];
		}
	}
}

void CSudokuPuzzle::Rotate180(int iPuzzleArray[][9], int iSolutionArray[][9])
{
    int	arrTempSol[NUM_ROWS][NUM_COLUMNS];
    int	arrTempPuz[NUM_ROWS][NUM_COLUMNS];

	// set the temp matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			arrTempSol[x][y]	= iSolutionArray[8-x][8-y];
			arrTempPuz[x][y]	= iPuzzleArray[8-x][8-y];
		}
	}

	// now reset the original matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			iSolutionArray[x][y] = arrTempSol[x][y];
			iPuzzleArray[x][y]   = arrTempPuz[x][y];
		}
	}
}

void CSudokuPuzzle::MirrorXY(int iPuzzleArray[][9], int iSolutionArray[][9])
{
    int	arrTempSol[NUM_ROWS][NUM_COLUMNS];
    int	arrTempPuz[NUM_ROWS][NUM_COLUMNS];

	// set the temp matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			arrTempSol[x][y]	= iSolutionArray[y][x];
			arrTempPuz[x][y]	= iPuzzleArray[y][x];
		}
	}

	// now reset the original matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			iSolutionArray[x][y] = arrTempSol[x][y];
			iPuzzleArray[x][y]   = arrTempPuz[x][y];
		}
	}
}

void CSudokuPuzzle::MirrorHorizontal(int iPuzzleArray[][9], int iSolutionArray[][9])
{
    int	arrTempSol[NUM_ROWS][NUM_COLUMNS];
    int	arrTempPuz[NUM_ROWS][NUM_COLUMNS];

	// set the temp matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			arrTempSol[x][y]	= iSolutionArray[x][8-y];
			arrTempPuz[x][y]	= iPuzzleArray[x][8-y];
		}
	}
	
	// now reset the original matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			iSolutionArray[x][y] = arrTempSol[x][y];
			iPuzzleArray[x][y]   = arrTempPuz[x][y];
		}
	}
}

void CSudokuPuzzle::MirrorVertical(int iPuzzleArray[][9], int iSolutionArray[][9])
{
    int	arrTempSol[NUM_ROWS][NUM_COLUMNS];
    int	arrTempPuz[NUM_ROWS][NUM_COLUMNS];

	// set the temp matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			arrTempSol[x][y]	= iSolutionArray[8-x][y];
			arrTempPuz[x][y]	= iPuzzleArray[8-x][y];
		}
	}
	
	// now reset the original matrices
	for(int y=0; y<9; y++)
	{
		for(int x=0; x<9; x++)
		{
			iSolutionArray[x][y] = arrTempSol[x][y];
			iPuzzleArray[x][y]   = arrTempPuz[x][y];
		}
	}
}

void CSudokuPuzzle::SwapColumns(int iPuzzleArray[][9], int iColumn1, int iColumn2)
{
	int iTemp;
	for(int i=0; i<9; i++)
	{
		iTemp 				      = iPuzzleArray[iColumn1][i];
		iPuzzleArray[iColumn1][i] = iPuzzleArray[iColumn2][i];
		iPuzzleArray[iColumn2][i] = iTemp;			
	}
}

void CSudokuPuzzle::SwapRows(int iPuzzleArray[][9], int iRow1, int iRow2)
{
	int iTemp;
	for(int i=0; i<9; i++)
	{
		iTemp 		  		    = iPuzzleArray[i][iRow1];
		iPuzzleArray[i][iRow1]  = iPuzzleArray[i][iRow2];
		iPuzzleArray[i][iRow2]  = iTemp;			
	}
}

void CSudokuPuzzle::SwapColumnSections(int iPuzzleArray[][9], int iSolutionArray[][9])
{
	// pick two rand sections
	int iSection1 = -1;
	int iSection2 = -1;
	
	while(iSection1 == iSection2)
	{
		iSection1 = rand()%3;
		iSection2 = rand()%3;
	}
	
	// switch all the columns
	SwapColumns(iSolutionArray, iSection1*3, iSection2*3);
	SwapColumns(iSolutionArray, iSection1*3+1, iSection2*3+1);
	SwapColumns(iSolutionArray, iSection1*3+2, iSection2*3+2);
	SwapColumns(iPuzzleArray, iSection1*3, iSection2*3);
	SwapColumns(iPuzzleArray, iSection1*3+1, iSection2*3+1);
	SwapColumns(iPuzzleArray, iSection1*3+2, iSection2*3+2);			 
}

void CSudokuPuzzle::SwapRowSections(int iPuzzleArray[][9], int iSolutionArray[][9])
{
	// pick two rand sections
	int iSection1 = -1;
	int iSection2 = -1;
	
	while(iSection1 == iSection2)
	{
		iSection1 = rand()%3;
		iSection2 = rand()%3;
	}
	
	// switch all the columns
	SwapRows(iSolutionArray, iSection1*3, iSection2*3);
	SwapRows(iSolutionArray, iSection1*3+1, iSection2*3+1);
	SwapRows(iSolutionArray, iSection1*3+2, iSection2*3+2);
	SwapRows(iPuzzleArray, iSection1*3, iSection2*3);
	SwapRows(iPuzzleArray, iSection1*3+1, iSection2*3+1);
	SwapRows(iPuzzleArray, iSection1*3+2, iSection2*3+2);			 
}

void CSudokuPuzzle::SwapColumnSingle(int iPuzzleArray[][9], int iSolutionArray[][9])
{
	// pick two rand sections
	int iColumn1 = -1;
	int iColumn2 = -1;
	int iSection = rand()%3;
	
	while(iColumn1 == iColumn2)
	{
		iColumn1 = rand()%3;
		iColumn2 = rand()%3;
	}
	
	// switch all the columns
	SwapColumns(iSolutionArray, iSection*3 + iColumn1, iSection*3+iColumn2);
	SwapColumns(iPuzzleArray, iSection*3 + iColumn1, iSection*3+iColumn2);
}

void CSudokuPuzzle::SwapRowSingle(int iPuzzleArray[][9], int iSolutionArray[][9])
{
	// pick two rand sections
	int iRow1 = -1;
	int iRow2 = -1;
    int iSection = rand()%3;
	
	while(iRow1 == iRow2)
	{
		iRow1 = rand()%3;
		iRow2 = rand()%3;
	}
	
	// switch all the columns
	SwapRows(iSolutionArray, iSection*3 + iRow1, iSection*3+iRow2);
	SwapRows(iPuzzleArray, iSection*3 + iRow1, iSection*3+iRow2);
}