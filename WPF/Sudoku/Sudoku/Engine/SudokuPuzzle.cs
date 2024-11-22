using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Resources;
using System.IO;

namespace Sudoku.Engine
{
    public enum EnumPuzzleType
    {
        normal,
        cross,
        hyper,
    };

    public enum EnumGridType
    {
        balanced,
        random,
        symetrical,
    };

    public class SudokuPuzzle
    {
        //in place of macros .....
        int INDEX(int row, int col) { return (9 * (row) + (col)); }
        int ROW(int idx) { return (idx) / 9; }
        int COLUMN(int idx) { return (idx) % 9; }
        int BLOCK(int idx) { return 3 * (ROW(idx) / 3) + (COLUMN(idx) / 3); }

        protected Random rand = new Random(System.DateTime.Today.Minute + System.Environment.TickCount);
        
		static int NUM_FIRST_REMOVE = 40;
        static int NUM_CELLS_REMOVE_TARGET = NUM_FIRST_REMOVE + 13;

        protected int[,] m_iGeneratedPuzzle = new int[Globals.NUM_ROWS, Globals.NUM_COLUMNS];

        protected bool[] m_bNumbers = new bool[Globals.NUM_DIGITS];

        EnumPuzzleType ePuzType = EnumPuzzleType.normal;
        EnumGridType eGridType  = EnumGridType.random;
        
    
        public SudokuPuzzle()
        {

        }
        
        public virtual void GetNewPuzzle(EnumDifficulty eDifficulty, ref int[,] iPuzzleArray, ref int[,] iSolutionArray)//[Globals.NUM_ROWS,Globals.NUM_COLUMNS], int iSolutionArray[Globals.NUM_ROWS,Globals.NUM_COLUMNS])
        {
            if (eDifficulty == EnumDifficulty.DIFF_Hardcore)
            {
                GetNewGeniusPuzzle(ref iPuzzleArray, ref iSolutionArray);
                return;
            }

            // run through all this code in a while loop until a puzzle with very few given cells is found
            // very few meaning less than MAX_CELLS
            int[,] iTempArray = new int[Globals.NUM_ROWS, Globals.NUM_COLUMNS];
            int[] iCardinalityArray = new int[81];
            int[] iIndexingArray = new int[81];
            int[] iRemoveArray = new int[81];
            int iRow = 0;
            int iColumn = 0;
            int iRemoveIndex = 0;
            bool bSolvable = true;
            int iSolverResult = -1;
            int iCellsHidden = 0;
            int iNumLoops = 0;

            int iRemoveTarget = NUM_CELLS_REMOVE_TARGET;

            // determine number of extra cells to add to make the puzzle easier, once we have a solvable puzzle
            switch (eDifficulty)
            {
                case EnumDifficulty.DIFF_VeryHard:
                    iRemoveTarget -= 0;
                    break;
                case EnumDifficulty.DIFF_Hard:
                    iRemoveTarget -= 4;
                    break;
                case EnumDifficulty.DIFF_Medium:
                    iRemoveTarget -= 7;
                    break;
                case EnumDifficulty.DIFF_Easy:
                    iRemoveTarget -= 11;
                    break;
                case EnumDifficulty.DIFF_VeryEasy:
                default:
                    iRemoveTarget -= 14;
                    break;
            }

            while (iCellsHidden < iRemoveTarget)
            {
                bSolvable = true;
                iNumLoops++;
                iRemoveIndex = 0;
                // generate a puzzle and copy m_iGeneratedPuzzle[NUM_ROWS][NUM_COLUMNS] to the arguement
                // blank out the puzzle array
                GeneratePuzzle();

                Buffer.BlockCopy(m_iGeneratedPuzzle, 0, iTempArray, 0, 4 * Globals.NUM_COLUMNS * Globals.NUM_ROWS);//2 per int dimension ... 
                //these were in a for loop ... but there's no reason for it ....
                Buffer.BlockCopy(m_iGeneratedPuzzle, 0, iSolutionArray, 0, 4 * Globals.NUM_COLUMNS * Globals.NUM_ROWS);
                Buffer.BlockCopy(m_iGeneratedPuzzle, 0, iPuzzleArray, 0, 4 * Globals.NUM_COLUMNS * Globals.NUM_ROWS);

                // create a 1D array with values from 0 to 80, randomized, to show the order of cell elements that are removed
                for (int i = 0; i < 81; i++)
                    iRemoveArray[i] = i;


                RandomizeArray(ref iRemoveArray, 81, (eGridType==EnumGridType.symetrical?true:false));

                // Now remove random cells from the solution array to the temp array
                for (int i = 0; i < 10; i++)//NUM_FIRST_REMOVE; i++)
                {
                    // remove the random cell
                    iRow = iRemoveArray[i] / Globals.NUM_COLUMNS;
                    iColumn = iRemoveArray[i] % Globals.NUM_COLUMNS;
                    // add number to cell and increment
                    iTempArray[iRow, iColumn] = 0;
                    iPuzzleArray[iRow, iColumn] = 0;
                    iRemoveIndex++;
                }

                // now iterate (hide cells with highest cardinality) by checking if the puzzle can be solved, is unique, and has the desired difficulty
                // Solve the new puzzle to get the values for every cell

                Solver oSol = new Solver(iTempArray);
                iSolverResult = oSol.Solve(iTempArray, false);

 

                while (bSolvable == true)
                {
                    if (iRemoveIndex > 64)
                    {
                        // removed too many cells to have a solution, ever
                        iRemoveIndex = 0;
                        bSolvable = false;
                    }
                    if (iRemoveIndex >= (iRemoveTarget)
                        && iSolverResult == 1)
                    {
                        // solved, and unique.  We're finished so exit the while loop
                        bSolvable = true;
                        iRemoveIndex++;
                        break;
                    }
                    //Still have values to remove to meet our target
                    else if (iRemoveIndex < iRemoveTarget
                        && iSolverResult == 1)
                    {
                        int iTempValue;
                        int iTempIndex;

                        //if (bTruelyRandom == false)
                        if(eGridType == EnumGridType.balanced)
                        {
                            // go through the puzzle and compute the cardinality of each cell
                            for (int i = 0; i < 81; i++)
                            {
                                iCardinalityArray[i] = 0;
                                iIndexingArray[i] = 0;
                            }
                            for (int i = 0; i < Globals.NUM_ROWS; i++)
                            {
                                for (int j = 0; j < Globals.NUM_COLUMNS; j++)
                                {
                                    // only check cells with a digit in it (other than zero)
                                    if (iPuzzleArray[i, j] != 0)
                                    {
                                        iCardinalityArray[INDEX(i, j)] = Cardinality(ref iPuzzleArray, i, j);
                                        iIndexingArray[INDEX(i, j)] = INDEX(i, j);
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
                            Buffer.BlockCopy(iRemoveArray, 0, iIndexingArray, 0, 4 * 81);
                            //memcpy(iIndexingArray, iRemoveArray, 81 * sizeof(int));//quick array copy
                        }

                        do
                        {
                            iTempValue = iPuzzleArray[ROW(iIndexingArray[iTempIndex]), COLUMN(iIndexingArray[iTempIndex])];
                            iPuzzleArray[ROW(iIndexingArray[iTempIndex]), COLUMN(iIndexingArray[iTempIndex])] = 0;
                            // must copy the puzzle array into the temp variable because the array is modified by the solver
                            Buffer.BlockCopy(iPuzzleArray, 0, iTempArray, 0, 4 * 9 * 9);//4*9*9

                            
                            oSol = new Solver(iTempArray);
                            iSolverResult = oSol.Solve(iTempArray, false);


                            if (iSolverResult == 1)
                            {
                                // do nothing
                                break;
                            }
                            else
                            {
                                // place value remove back into the puzzle
                                iPuzzleArray[ROW(iIndexingArray[iTempIndex]), COLUMN(iIndexingArray[iTempIndex])] = iTempValue;
                                if (eGridType == EnumGridType.random)
                                    iTempIndex++;
                                else
                                    iTempIndex++;//iTempIndex--;
                            }

                        } while (iSolverResult != 1 &&
                                iPuzzleArray[ROW(iTempIndex), COLUMN(iTempIndex)] != 0 &&
                                iTempIndex > -1);

                        // check if we should stop trying this puzzle and generate a new one
                        if (iSolverResult != 1)
                        {
                            bSolvable = false;
                        }
                        else
                        {
                            Buffer.BlockCopy(iPuzzleArray, 0, iTempArray, 0, 4 * 9 * 9);//2*9*9
                            iRemoveIndex++;
                        }
                    }
                    else
                    {
                        // we want to stop and go to the start of while(iNumGivenElements)
                        iRemoveIndex = 0;	// make sure we run another loop through while(iNumGivenElements)
                        bSolvable = false;
                    }
                }// end of while (bSolvable == TRUE)
                iCellsHidden = iRemoveIndex;
            }

            //and finally ... check for unsolvable ... not ideal but it'll work
            FixNonSolvable(ref iPuzzleArray, ref iSolutionArray);

            // now we have a puzzle with the desired difficulty level, so end the function
            return;

        }

        protected void GeneratePuzzle()
        {
            // have already seeded the random number generator in the constructor
            // generate a puzzle and place it into the variable m_iGeneratedPuzzle
            PopulateCell(0);
        }

        private void GetNewGeniusPuzzle(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
            StreamResourceInfo sri = App.GetResourceStream(new Uri("Assets/Sudoku/hanswer.txt", UriKind.RelativeOrAbsolute));
            StreamReader sr = new StreamReader(sri.Stream);
            string szAnswer = sr.ReadToEnd();
            sr.Close();

            sri = App.GetResourceStream(new Uri("Assets/Sudoku/hpuzzle.txt", UriKind.RelativeOrAbsolute));
            sr = new StreamReader(sri.Stream);
            string szPuzzle = sr.ReadToEnd();
            sr.Close();

            //assume the files are identical sizes ... because they need to be
            int iLineLength = 81 + 2;
            int iNumPuzzles = szPuzzle.Length / (iLineLength);
            int iPuzzleIndex = rand.Next(0, iNumPuzzles - 1);

            //copy the answer into the array
            for (int x = 0; x < 9; x++)
            {
                for (int y = 0; y < 9; y++)
                {
                    iSolutionArray[y, x] = szAnswer[iPuzzleIndex*iLineLength + x + 9 * y] - 48;
                    iPuzzleArray[y, x] = szPuzzle[iPuzzleIndex * iLineLength + x + 9 * y] - 48;
                }
            }

            SwapNumbers(ref iPuzzleArray, ref iSolutionArray, 3);//mix em a bit
            ManipulatePuzzle(ref iPuzzleArray, ref iSolutionArray);

        }

        private void SwapNumbers(ref int[,] iPuzzleArray, ref int[,] iSolutionArray, int iNumSwaps /* =/* = 3 */)
        {
            int iFrom;
            int iTo;

            for(int iSwaps = 0; iSwaps < iNumSwaps; iSwaps++)
            {
                iFrom = rand.Next(1, 9);
                iTo = rand.Next(1, 9);

                if(iFrom == iTo)
                    continue;

                for(int x = 0; x < 9; x++)
                {
                    for(int y = 0; y < 9; y++)
                    {
                        if(iPuzzleArray[x,y] == iFrom)
                            iPuzzleArray[x,y] = iTo;
                        else if(iPuzzleArray[x,y] == iTo)
                            iPuzzleArray[x,y] = iFrom;

                        if(iSolutionArray[x,y] == iFrom)
                            iSolutionArray[x,y] = iTo;
                        else if(iSolutionArray[x,y] == iTo)
                            iSolutionArray[x,y] = iFrom;
                    }
                }
            }
        }

        private void ManipulatePuzzle(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {		
	        // randomly perform a matrix manipulation on the current puzzle
            int iPick;

            for(int i=0; i < 2; i++)
	        {
		        iPick = rand.Next(0,9);

		        switch(iPick)
		        {
		        case 0:
			        Rotate90(ref iPuzzleArray, ref iSolutionArray);
			        break;
		        case 1:
			        Rotate180(ref iPuzzleArray, ref iSolutionArray);
			        break;
		        case 3:
			        Rotate270(ref iPuzzleArray, ref iSolutionArray);
			        break;
		        case 4:
			        MirrorXY(ref iPuzzleArray, ref iSolutionArray);
			        break;
		        case 5:
			        MirrorHorizontal(ref iPuzzleArray, ref iSolutionArray);
			        break;
		        case 6:
			        MirrorVertical(ref iPuzzleArray, ref iSolutionArray);
			        break;
		        case 7:
			        SwapColumnSections(ref iPuzzleArray, ref iSolutionArray);
			        break;
		        case 8:
			        SwapRowSections(ref iPuzzleArray, ref iSolutionArray);
			        break;
		        case 9:
			        SwapColumnSingle(ref iPuzzleArray, ref iSolutionArray);
			        break;
		        default:
			        SwapRowSingle(ref iPuzzleArray, ref iSolutionArray);
			        break;
		        }
	        }
        }

        private void Rotate90(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
            int[,] arrTempSol= new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
            int[,] arrTempPuz = new int[Globals.NUM_ROWS, Globals.NUM_COLUMNS];

	        // set the temp matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        arrTempSol[x,y]	= iSolutionArray[8-y,x];
			        arrTempPuz[x,y]	= iPuzzleArray[8-y,x];
		        }
	        }
	
	        // now reset the original matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        iSolutionArray[x,y] = arrTempSol[x,y];
			        iPuzzleArray[x,y]   = arrTempPuz[x,y];
		        }
	        }		
        }

        private void Rotate270(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
            int[,]	arrTempSol= new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
            int[,]	arrTempPuz= new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];

	        // set the temp matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        arrTempSol[8-y,x]	= iSolutionArray[x,y];
			        arrTempPuz[8-y,x]	= iPuzzleArray[x,y];
		        }
	        }
	
	        // now reset the original matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        iSolutionArray[x,y] = arrTempSol[x,y];
			        iPuzzleArray[x,y]   = arrTempPuz[x,y];
		        }
	        }
        }

        private void Rotate180(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
            int[,]	arrTempSol= new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
            int[,]	arrTempPuz= new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];

	        // set the temp matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        arrTempSol[x,y]	= iSolutionArray[8-x,8-y];
			        arrTempPuz[x,y]	= iPuzzleArray[8-x,8-y];
		        }
	        }

	        // now reset the original matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        iSolutionArray[x,y] = arrTempSol[x,y];
			        iPuzzleArray[x,y]   = arrTempPuz[x,y];
		        }
	        }
        }

        private void MirrorXY(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
            int[,]	arrTempSol= new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
            int[,]	arrTempPuz= new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];

	        // set the temp matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        arrTempSol[x,y]	= iSolutionArray[y,x];
			        arrTempPuz[x,y]	= iPuzzleArray[y,x];
		        }
	        }

	        // now reset the original matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        iSolutionArray[x,y] = arrTempSol[x,y];
			        iPuzzleArray[x,y]   = arrTempPuz[x,y];
		        }
	        }
        }

        private void MirrorHorizontal(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
            int[,]	arrTempSol= new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
            int[,]	arrTempPuz= new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];

	        // set the temp matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        arrTempSol[x,y]	= iSolutionArray[x,8-y];
			        arrTempPuz[x,y]	= iPuzzleArray[x,8-y];
		        }
	        }
	
	        // now reset the original matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        iSolutionArray[x,y] = arrTempSol[x,y];
			        iPuzzleArray[x,y]   = arrTempPuz[x,y];
		        }
	        }
        }

        private void MirrorVertical(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
            int[,]	arrTempSol= new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
            int[,]	arrTempPuz= new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];

	        // set the temp matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        arrTempSol[x,y]	= iSolutionArray[8-x,y];
			        arrTempPuz[x,y]	= iPuzzleArray[8-x,y];
		        }
	        }
	
	        // now reset the original matrices
	        for(int y=0; y<9; y++)
	        {
		        for(int x=0; x<9; x++)
		        {
			        iSolutionArray[x,y] = arrTempSol[x,y];
			        iPuzzleArray[x,y]   = arrTempPuz[x,y];
		        }
	        }
        }

        private void SwapColumns(ref int[,] iPuzzleArray, int iColumn1, int iColumn2)
        {
	        int iTemp;
	        for(int i=0; i<9; i++)
	        {
		        iTemp 				      = iPuzzleArray[iColumn1,i];
		        iPuzzleArray[iColumn1,i] = iPuzzleArray[iColumn2,i];
		        iPuzzleArray[iColumn2,i] = iTemp;			
	        }
        }

        private void SwapRows(ref int[,] iPuzzleArray, int iRow1, int iRow2)
        {
	        int iTemp;
	        for(int i=0; i<9; i++)
	        {
		        iTemp 		  		    = iPuzzleArray[i,iRow1];
		        iPuzzleArray[i,iRow1]  = iPuzzleArray[i,iRow2];
		        iPuzzleArray[i,iRow2]  = iTemp;			
	        }
        }

        private void SwapColumnSections(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
	        // pick two rand sections
	        int iSection1 = -1;
	        int iSection2 = -1;
	
	        while(iSection1 == iSection2)
	        {
		        iSection1 = rand.Next(0,2);
		        iSection2 = rand.Next(0,2);
	        }
	
	        // switch all the columns
	        SwapColumns(ref iSolutionArray, iSection1*3, iSection2*3);
	        SwapColumns(ref iSolutionArray, iSection1*3+1, iSection2*3+1);
	        SwapColumns(ref iSolutionArray, iSection1*3+2, iSection2*3+2);
	        SwapColumns(ref iPuzzleArray, iSection1*3, iSection2*3);
	        SwapColumns(ref iPuzzleArray, iSection1*3+1, iSection2*3+1);
	        SwapColumns(ref iPuzzleArray, iSection1*3+2, iSection2*3+2);			 
        }

        private void SwapRowSections(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
	        // pick two rand sections
	        int iSection1 = -1;
	        int iSection2 = -1;
	
	        while(iSection1 == iSection2)
	        {
                iSection1 = rand.Next(0, 2);
                iSection2 = rand.Next(0, 2);
	        }
	
	        // switch all the columns
	        SwapRows(ref iSolutionArray, iSection1*3, iSection2*3);
	        SwapRows(ref iSolutionArray, iSection1*3+1, iSection2*3+1);
	        SwapRows(ref iSolutionArray, iSection1*3+2, iSection2*3+2);
	        SwapRows(ref iPuzzleArray, iSection1*3, iSection2*3);
	        SwapRows(ref iPuzzleArray, iSection1*3+1, iSection2*3+1);
	        SwapRows(ref iPuzzleArray, iSection1*3+2, iSection2*3+2);			 
        }

        private void SwapColumnSingle(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
	        // pick two rand sections
	        int iColumn1 = -1;
	        int iColumn2 = -1;
            int iSection = rand.Next(0, 2);
	
	        while(iColumn1 == iColumn2)
	        {
                iColumn1 = rand.Next(0, 2);
                iColumn2 = rand.Next(0, 2);
	        }
	
	        // switch all the columns
	        SwapColumns(ref iSolutionArray, iSection*3 + iColumn1, iSection*3+iColumn2);
	        SwapColumns(ref iPuzzleArray, iSection*3 + iColumn1, iSection*3+iColumn2);
        }

        private void SwapRowSingle(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
	        // pick two rand sections
	        int iRow1 = -1;
	        int iRow2 = -1;
            int iSection = rand.Next(0,2);
	
	        while(iRow1 == iRow2)
	        {
		        iRow1 = rand.Next(0,2);
		        iRow2 = rand.Next(0,2);
	        }
	
	        // switch all the columns
	        SwapRows(ref iSolutionArray, iSection*3 + iRow1, iSection*3+iRow2);
	        SwapRows(ref iPuzzleArray, iSection*3 + iRow1, iSection*3+iRow2);
        }

        bool PopulateCell(int iCellIndex)
        {

            if (iCellIndex == 81)
                return true; // the board is full!!

            int iRow = iCellIndex / Globals.NUM_ROWS;
            int iColumn = iCellIndex % Globals.NUM_COLUMNS;
            int[] iCellOrder = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };	// Note this list will contain 1..9 in a *random* order

            // try setting each possible value in cell
            RandomizeArray(ref iCellOrder, Globals.NUM_DIGITS);

            for (int i = 0; i < Globals.NUM_DIGITS; i++)
            {
                // set this test value
                m_iGeneratedPuzzle[iRow, iColumn] = iCellOrder[i];

                // is this board still valid?
                if (IsBoardValid())
                {
                    // it is so try to populate next cell
                    if (PopulateCell(iCellIndex + 1))
                    {	// recursive to the last cell, if this call returned true, then the board is valid
                        return true;
                    }
                }
            }
            // rollback this cell
            // very critical to do this, otherwise bad values will cause the board to be invalid
            m_iGeneratedPuzzle[iRow, iColumn] = 0;
            return false;

        }

        protected bool IsBoardValid()
        {
            // test constraints
            // every column, row and subsection must be valid
            for (int i = 0; i < Globals.NUM_COLUMNS; i++)
            {
                if (!IsColumnValid(i))
                    return false;
                if (!IsRowValid(i))
                    return false;
                if (!IsSectionValid(i))
                    return false;
            }

            if (ePuzType == EnumPuzzleType.cross)
            {
                if (!IsDiagonalValid())
                    return false;
            }

            if (ePuzType == EnumPuzzleType.hyper)
            {
                for (int i = 0; i < 4; i++)
                    if (!IsHyperSecValid(i))
                        return false;
            }
            return true;

        }

        bool IsDiagonalValid()
        {
            int iTemp;

            // check to make sure only one of each digit 1-9 is used
            for (iTemp = 0; iTemp < Globals.NUM_DIGITS; iTemp++)
                m_bNumbers[iTemp] = false;

            for (int k = 0; k < Globals.NUM_ROWS; k++)
            {
                iTemp = m_iGeneratedPuzzle[k, k];
                if (iTemp < 1 || iTemp > Globals.NUM_DIGITS)
                {
                    // no need to check, just continue
                    continue;
                }
                if (m_bNumbers[iTemp - 1] == true)
                    return false;
                else
                    m_bNumbers[iTemp - 1] = true;
            }

            for (iTemp = 0; iTemp < Globals.NUM_DIGITS; iTemp++)
                m_bNumbers[iTemp] = false;

            for (int k = 0; k < Globals.NUM_ROWS; k++)
            {
                iTemp = m_iGeneratedPuzzle[k, 8-k];
                if (iTemp < 1 || iTemp > Globals.NUM_DIGITS)
                {   // no need to check, just continue
                    continue;
                }
                if (m_bNumbers[iTemp - 1] == true)
                    return false;
                else
                    m_bNumbers[iTemp - 1] = true;
            }
            return true;
        }

        bool IsHyperSecValid(int iSec)
        {
            //we'll start with top left
            int iOffSet = 1 + (iSec%2)*4;//1 or 3
            int jOffSet = 1 + ((iSec > 1) ? 4 : 0);//2 or 3

            // go through each cell in that section and make sure only one of each digit 1-9 is used
            int iTemp;
            for (iTemp = 0; iTemp < Globals.NUM_DIGITS; iTemp++)
                m_bNumbers[iTemp] = false;

            for (int i = iOffSet; i < iOffSet + 3; i++)
            {
                for (int j = jOffSet; j < jOffSet + 3; j++)
                {
                    iTemp = m_iGeneratedPuzzle[i, j];
                    if (iTemp < 1 || iTemp > Globals.NUM_DIGITS)
                    {
                        // no need to check, just continue
                        continue;
                    }
                    if (m_bNumbers[iTemp - 1] == true)
                        return false;
                    else
                        m_bNumbers[iTemp - 1] = true;
                }
            }
            return true;
        }

        bool IsColumnValid(int iColumnIndex)
        {
            int iTemp;

            // check to make sure only one of each digit 1-9 is used
            for (iTemp = 0; iTemp < Globals.NUM_DIGITS; iTemp++)
                m_bNumbers[iTemp] = false;

            for (int k = 0; k < Globals.NUM_ROWS; k++)
            {
                iTemp = m_iGeneratedPuzzle[k, iColumnIndex];
                if (iTemp < 1 || iTemp > Globals.NUM_DIGITS)
                {
                    // no need to check, just continue
                    continue;
                }
                if (m_bNumbers[iTemp - 1] == true)
                    return false;
                else
                    m_bNumbers[iTemp - 1] = true;
            }
            return true;
        }

        bool IsRowValid(int iRowIndex)
        {
            // check to make sure only one of each digit 1-9 is used
            int iTemp;

            for (iTemp = 0; iTemp < Globals.NUM_DIGITS; iTemp++)
                m_bNumbers[iTemp] = false;

            for (int k = 0; k < Globals.NUM_COLUMNS; k++)
            {
                iTemp = m_iGeneratedPuzzle[iRowIndex, k];
                if (iTemp < 1 || iTemp > Globals.NUM_DIGITS)
                {
                    // no need to check, just continue
                    continue;
                }
                if (m_bNumbers[iTemp - 1] == true)
                    return false;
                else
                    m_bNumbers[iTemp - 1] = true;
            }
            return true;

        }

        bool IsSectionValid(int iRowIndex, int iColumnIndex)
        {
            // find out which section (3x3 matrix) the row and column indexes point to
            int iSectionI = iRowIndex / 3;
            int iSectionJ = iColumnIndex / 3;

            // go through each cell in that section and make sure only one of each digit 1-9 is used
            int iTemp;
            for (iTemp = 0; iTemp < Globals.NUM_DIGITS; iTemp++)
                m_bNumbers[iTemp] = false;

            for (int i = iSectionI * 3; i < iSectionI * 3 + 3; i++)
            {
                for (int j = iSectionJ * 3; j < iSectionJ * 3 + 3; j++)
                {
                    iTemp = m_iGeneratedPuzzle[i, j];
                    if (iTemp < 1 || iTemp > Globals.NUM_DIGITS)
                    {
                        // no need to check, just continue
                        continue;
                    }
                    if (m_bNumbers[iTemp - 1] == true)
                        return false;
                    else
                        m_bNumbers[iTemp - 1] = true;
                }
            }
            return true;
        }

        bool IsSectionValid(int iSectionIndex)
        {
            // use a single integer to call isSectionValid(int iRowIndex, int iColumnIndex)
            int[] iIndexes = { 1, 4, 7 };
            int iRow = iIndexes[iSectionIndex / 3];
            int iColumn = iIndexes[iSectionIndex % 3];
            return IsSectionValid(iRow, iColumn);
        }


        protected void RandomizeArray(ref int[] iArray, int iSize)
        {
            RandomizeArray(ref iArray, iSize, false);
        }

        protected void RandomizeArray(ref int[] iArray, int iSize, bool bUseSymetry)
        {
            // use the random number generator to make swaps between two array elements
            int iFirstIndex = 0;
            int iSecondIndex = 0;
            int iTemp = 0;
            
            if (bUseSymetry)
            {
                int iTempSize = iSize / 2;

                for (int i = 0; i < iSize * 2; i++)
                {
                    //mix half
                    iFirstIndex = rand.Next(iTempSize);
                    iSecondIndex = rand.Next(iTempSize);
                    if (iFirstIndex > iTempSize - 1)
                        iFirstIndex = iTempSize - 1;

                    if (iSecondIndex > iTempSize - 1)
                        iSecondIndex = iTempSize - 1;

                    iTemp = iArray[iFirstIndex];
                    iArray[iFirstIndex] = iArray[iSecondIndex];
                    iArray[iSecondIndex] = iTemp;
                }

                //now set to every other position and add in the inverse piece ....
                for (int i = iTempSize; i > 0; i--)
                {
                    iArray[i * 2] = iArray[i];
                    iArray[i * 2 - 1] = iSize - 1 - iArray[i];
                }
            }
            else 
            {
                for (int i = 0; i < iSize * 2; i++)
                {
                    iFirstIndex = rand.Next(iSize); //(int)((double)rand() / (RAND_MAX + 1) * (iSize));
                    iSecondIndex = rand.Next(iSize);// (int)((double)rand() / (RAND_MAX + 1) * (iSize));
                    if (iFirstIndex > iSize - 1)
                        iFirstIndex = iSize - 1;

                    if (iSecondIndex > iSize - 1)
                        iSecondIndex = iSize - 1;

                    iTemp = iArray[iFirstIndex];
                    iArray[iFirstIndex] = iArray[iSecondIndex];
                    iArray[iSecondIndex] = iTemp;
                }
            }
        }


        void MyQSort(int[] iSortArray, int[] iCompanionArray, int iFirst, int iLast)
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
            } while (i <= j);

            if (iFirst < j)
            {
                MyQSort(iSortArray, iCompanionArray, iFirst, j);
            }

            if (i < iLast)
            {
                MyQSort(iSortArray, iCompanionArray, i, iLast);
            }
        }

        //ref so we don't copy the whole damn array
        int Cardinality(ref int[,] iPuzzleArray, int iRow, int iColumn)
        {
            // add the number of given numbers in cells that are in the same row, column and block (9x9 section) as the
            // selected cell at iRow, iColumn
            int iCardinality = -3;	// will count itself 3 times
            int iBlockRowStart = (iRow / 3) * 3;
            int iBlockColumnStart = (iColumn / 3) * 3;

            // count number of other given cells in the column
            for (int i = 0; i < 9; i++)
            {
                if (iPuzzleArray[i, iColumn] != 0)
                {
                    iCardinality++;
                }
            }
            // count number of other given cells in the row
            for (int j = 0; j < 9; j++)
            {
                if (iPuzzleArray[iRow, j] != 0)
                {
                    iCardinality++;
                }
            }
            // count number of other given cells in the block
            for (int i = iBlockRowStart; i < iBlockRowStart + 3; i++)
            {
                for (int j = iBlockColumnStart; j < iBlockColumnStart + 3; j++)
                {
                    if (iPuzzleArray[i, j] != 0)
                    {
                        iCardinality++;
                    }
                }
            }
            
            return iCardinality;
        }



        //1 check per number per subset pair
        //slight optimization ... only check 1-8 ... 9 will be redundant
        //we don't really have to worry about which is x or y as long as we're consistent
        //m_iGeneratedPuzzle[iRow][iColumn]
        void FixNonSolvable(ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
            Point ptNum1 = new Point();
            Point ptNum2 = new Point();
            Point ptCheck1 = new Point();
            Point ptCheck2 = new Point();

            Point ptSubSet1 = new Point();
            Point ptSubSet2 = new Point();

            int iXTemp, iYTemp;

            //so although this looks gross we can control everything right here
            //******************************

            //we need to do every comparison between 1,1 & 3,3 without overlap 
            for (int iTemp1 = 0; iTemp1 < 9; iTemp1++)
            {
                for (int iTemp2 = 0; iTemp2 < 9; iTemp2++)
                //******************************
                {
                    if (iTemp2 <= iTemp1)
                        continue;

                    ptSubSet1.X = iTemp1 % 3;
                    ptSubSet1.Y = iTemp1 / 3;

                    ptSubSet2.X = iTemp2 % 3;
                    ptSubSet2.Y = iTemp2 / 3;

                    //gonna cheat this
                    if (ptSubSet1.X != ptSubSet2.X &&
                        ptSubSet1.Y != ptSubSet2.Y)
                        continue; //gotta be in the same row or column

                    for (int iNum = 1; iNum < 9; iNum++)//1-8 .. 9 will be found indirectly ...
                    {
                        //to start we'll compare 1s between the first subset and the 2nd

                        for (int x = 0; x < 3; x++)
                        {
                            for (int y = 0; y < 3; y++)
                            {
                                iXTemp = x + (int)ptSubSet1.X * 3;
                                iYTemp = y + (int)ptSubSet1.Y * 3;

                                if (iNum == iSolutionArray[iYTemp,iXTemp])
                                {
                                    ptNum1.X = iXTemp;
                                    ptNum1.Y = iYTemp;
                                }

                                iXTemp = x + (int)ptSubSet2.X * 3;
                                iYTemp = y + (int)ptSubSet2.Y * 3;

                                if (iNum == iSolutionArray[iYTemp,iXTemp])
                                {
                                    ptNum2.X = iXTemp;
                                    ptNum2.Y = iYTemp;
                                }
                            }
                        }

                        //ok ... we have our two points ... now set the check points and see if they're equal
                        ptCheck1.X = ptNum1.X;
                        ptCheck1.Y = ptNum2.Y;
                        ptCheck2.X = ptNum2.X;
                        ptCheck2.Y = ptNum1.Y;

                        if (iSolutionArray[(int)ptCheck2.Y, (int)ptCheck2.X] == iSolutionArray[(int)ptCheck1.Y, (int)ptCheck1.X])
                        {
                            //we have a possible issue .... 
                            if (iPuzzleArray[(int)ptCheck2.Y, (int)ptCheck2.X] == 0 &&
                                iPuzzleArray[(int)ptCheck1.Y, (int)ptCheck1.X] == 0 &&
                                iPuzzleArray[(int)ptNum1.Y, (int)ptNum1.X] == 0 &&
                                iPuzzleArray[(int)ptNum2.Y, (int)ptNum2.X] == 0) //only if all 4 are blank do we care
                                iPuzzleArray[(int)ptCheck1.Y, (int)ptCheck1.X] = iSolutionArray[(int)ptCheck1.Y, (int)ptCheck1.X];//this "should" just work but never been able to repro
                        }

                    }
                }

            }

        }
    }
}
