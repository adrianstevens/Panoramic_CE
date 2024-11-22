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


namespace Sudoku.Engine
{
    public class KillerPuzzle: SudokuPuzzle
    {
        static protected int RANGE_MAX	= 9;
        static protected int RANGE_MIN = 1;
        static protected int MIN_CELLS = 32;
        static protected int MAX_CELLS = 37;
        static protected int NUM_SECTIONS_WIDE = 3;
        static protected int NUM_FIRST_REMOVE = 40;
        static protected int NUM_CELLS_REMOVE_TARGET = NUM_FIRST_REMOVE + 14; 
        static protected int MAX_SAMUNAMPURES = 81;

        //in place of macros .....
        int INDEX(int row, int col) { return (9 * (row) + (col)); }
        int ROW(int idx) { return (idx) / 9; }




	    
        protected int[]	m_iSamuCounts = new int[MAX_SAMUNAMPURES]; //number of cells in each group
        protected int[] m_iSamunamupures = new int[MAX_SAMUNAMPURES]; //value of each group of numbers
        protected int   m_iNumSamus;//number of samunamupures
        protected int[,] m_iSamud = new int[Globals.NUM_ROWS, Globals.NUM_COLUMNS];//if the cells have been samud yet


        public int      GetSamuIndex(int iRow, int iColumn){return m_iSamud[iRow,iColumn];}
	    public int      GetSamuValue(int iRow, int iColumn){return m_iSamunamupures[m_iSamud[iRow,iColumn]];}

        
        protected void Reset()
        {
            // initialize array to all zeros
            int i;
            for (i = 0; i < Globals.NUM_ROWS; i++)
            {
                for (int j = 0; j < Globals.NUM_COLUMNS; j++)
                {
                    m_iGeneratedPuzzle[i,j] = 0;
                    m_iSamud[i,j] = -1;
                }
            }

            // initialize array to all false values
            for(i = 0; i < Globals.NUM_DIGITS; i++)
                m_bNumbers[i] = false;

            for(int k = 0; k < MAX_SAMUNAMPURES; k++)
            {
                m_iSamunamupures[k] = 0;
                m_iSamuCounts[k] = 0;
            }

            m_iNumSamus = 0;
        }

        public virtual bool GetNewPuzzle(EnumDifficulty eDifficulty, int[,] iPuzzleArray, int[,] iSolutionArray)
        {
	        GeneratePuzzle();

	        for (int i = 0; i < Globals.NUM_ROWS; i++)
	        {
		        for (int j = 0; j < Globals.NUM_COLUMNS; j++)
		        {
			        iSolutionArray[i,j]	= m_iGeneratedPuzzle[i,j];
			        iPuzzleArray[i,j]		= 0;
		        }
	        }

	        //And now generate the Samunamupure component	
	        GenerateSamu(eDifficulty);

            //and finally ensure its solvable
            if(FixNonSolvable(iPuzzleArray, iSolutionArray) == false)
            {
                Reset();
                return GetNewPuzzle(eDifficulty, iPuzzleArray, iSolutionArray);
            }

	        // now we have a puzzle with the desired difficulty level, so end the function
	        return true;
        }


        protected void GenerateSamu(EnumDifficulty eDiff)
        {
	        bool[] bNumUsed = new bool[9]; //so we don't repeat numbers within the outline or "cages"

	        int iSamuSize	= 0;
	        int iTemp		= 0;
	        int iRand		= 0;

	        int iRow;	//more temps
	        int iCol;

	        iSamuSize = GetSamuSize(eDiff);

	        //reset the bool array
	        for(int i = 0; i < 9; i++)
		        bNumUsed[i] = false;

	        bNumUsed[m_iGeneratedPuzzle[0,0]-1] = true;

	        for(int i = 0; i < Globals.NUM_ROWS; i++)
	        {
		        for(int j = 0; j < Globals.NUM_COLUMNS; j++)
		        {
			        //randomly decide the size		
			        if(m_iSamud[i,j] != -1)
			        {
				        continue; //next position in the array
			        }
			
			
			        //set the value
			        m_iSamud[i,j] = m_iNumSamus;
			        iTemp = 1;
			        iRow = i;
			        iCol = j;

			        m_iSamunamupures[m_iNumSamus] = m_iGeneratedPuzzle[iRow,iCol];
                    m_iSamuCounts[m_iNumSamus] = 1;

			        bNumUsed[m_iGeneratedPuzzle[iRow,iCol] - 1] = true;

			        while(iTemp < iSamuSize && 
				        IsSamuExpansionPossible(iRow,iCol, bNumUsed)!=false)
			        {	//at least on direction is open...lets randomly find it
				        iRand = rand.Next()%4;
				
				        switch(iRand)
				        {
				        case 0://above
					        if(IsBelowAval(iRow, iCol, bNumUsed))
					        {
						        iRow++;
						        m_iSamud[iRow,iCol] = m_iNumSamus;
						        iTemp++;
						        m_iSamunamupures[m_iNumSamus] += m_iGeneratedPuzzle[iRow,iCol];
                                m_iSamuCounts[m_iNumSamus]++;
						        bNumUsed[m_iGeneratedPuzzle[iRow,iCol]-1] = true;
					        }
					        break;
				        case 1://below
					        if(IsAboveAval(iRow, iCol, bNumUsed))
					        {
						        iRow--;
						        m_iSamud[iRow,iCol] = m_iNumSamus;
						        iTemp++;
						        m_iSamunamupures[m_iNumSamus] += m_iGeneratedPuzzle[iRow,iCol];
                                m_iSamuCounts[m_iNumSamus]++;

						        bNumUsed[m_iGeneratedPuzzle[iRow,iCol]-1] = true;
					        }
					        break;
				        case 2://left
					        if(IsLeftAval(iRow, iCol, bNumUsed))
					        {
						        iCol--;
						        m_iSamud[iRow,iCol] = m_iNumSamus;
						        iTemp++;
						        m_iSamunamupures[m_iNumSamus] += m_iGeneratedPuzzle[iRow,iCol];
                                m_iSamuCounts[m_iNumSamus]++;

						        bNumUsed[m_iGeneratedPuzzle[iRow,iCol]-1] = true;

					        }
				            break;
				        case 3://right
					        if(IsRightAval(iRow, iCol, bNumUsed))
					        {
						        iCol++;
						        m_iSamud[iRow,iCol] = m_iNumSamus;
						        iTemp++;
						        m_iSamunamupures[m_iNumSamus] += m_iGeneratedPuzzle[iRow,iCol];
                                m_iSamuCounts[m_iNumSamus]++;

						        bNumUsed[m_iGeneratedPuzzle[iRow,iCol]-1] = true;
					        }
				            break;
				        default:
					        continue;
				            
				        }
			        }

                    iSamuSize = GetSamuSize(eDiff);
	
			        m_iNumSamus++;

			        //reset the bool array
			        for(int k = 0; k < 9; k++)
				        bNumUsed[k] = false;

		        }

	        }
        }

        int GetSamuSize(EnumDifficulty eDiff)
        {
            int iSamuSize = 1;

            switch(eDiff)
            {
            case EnumDifficulty.DIFF_VeryEasy:
                iSamuSize = rand.Next()%2 + 1;
                if(iSamuSize == 1)//this just helps reduce the number of singles
                    iSamuSize = rand.Next()%2 + 1;
                break;
            case EnumDifficulty.DIFF_Easy:
                iSamuSize = 2;
                break;
            case EnumDifficulty.DIFF_Medium:
                iSamuSize = rand.Next()%2 + 2;
                break;
            case EnumDifficulty.DIFF_Hard:
                iSamuSize = rand.Next()%3 + 2;
                break;
            case EnumDifficulty.DIFF_VeryHard:
                iSamuSize = rand.Next()%5 + 2;
                break;
            default:
                break;
            }

            return iSamuSize;
        }

        bool IsLeftAval(int iRow, int iColumn, bool[] bNumArray)
        {
	        if(iColumn > 1)
		        if(m_iSamud[iRow,iColumn-1]==-1)
		        {	
			        if(bNumArray[m_iGeneratedPuzzle[iRow,iColumn-1]-1]== true)			
				        return false;
			        return true;
		        }
	        return false;
        }

        bool IsRightAval(int iRow, int iColumn, bool[] bNumArray)
        {
	        if(iColumn < Globals.NUM_COLUMNS - 1)
		        if(m_iSamud[iRow,iColumn+1]==-1)
		        {	
			        if(bNumArray[m_iGeneratedPuzzle[iRow,iColumn+1]-1]== true)			
				        return false;
			        return true;
		        }
	        return false;

        }

        bool IsAboveAval(int iRow, int iColumn, bool[] bNumArray)
        {
	        if(iRow > 0)
		        if(m_iSamud[iRow-1,iColumn]==-1)
		        {	//checking for a repeated number
			        if(bNumArray[m_iGeneratedPuzzle[iRow-1,iColumn]-1]== true)			
				        return false;
			        return true;
		        }
	        return false;
        }

        bool IsBelowAval(int iRow, int iColumn, bool[] bNumArray)
        {
	        if(iRow < Globals.NUM_ROWS - 1)
		        if(m_iSamud[iRow+1,iColumn]==-1)
		        {	
			        if(bNumArray[m_iGeneratedPuzzle[iRow+1,iColumn]-1]== true)			
				        return false;
			        return true;
		        }

	        return false;

        }

        bool IsSamuExpansionPossible(int iRow, int iColumn, bool[] bNumArray)
        {
	        //check left 
	        if(IsLeftAval(iRow, iColumn, bNumArray))
		        return true;

	        //check right
	        if(IsRightAval(iRow, iColumn, bNumArray))
		        return true;

	        //check below
	        if(IsBelowAval(iRow, iColumn, bNumArray))
		        return true;
	
	        //check above
	        if(IsAboveAval(iRow, iColumn, bNumArray))
		        return true;

	        return false;
        }

        bool PopulateCell(int iCellIndex)
        {
	        if (iCellIndex == 81)
		        return true; // the board is full!!

	        int iRow			= iCellIndex / Globals.NUM_ROWS;
	        int iColumn			= iCellIndex % Globals.NUM_COLUMNS;
	        int[] iCellOrder	= {1, 2, 3, 4, 5, 6, 7, 8, 9};	// Note this list will contain 1..9 in a *random* order

	        // try setting each possible value in cell
	        RandomizeArray(ref iCellOrder, Globals.NUM_DIGITS);

	        for (int i = 0; i < Globals.NUM_DIGITS; i++)
	        {
		        // set this test value
		        m_iGeneratedPuzzle[iRow,iColumn] = iCellOrder[i];

		        // is this board still valid?
		        if(IsBoardValid())
		        {
			        // it is so try to populate next cell
			        if (PopulateCell(iCellIndex+1))
			        {
				        // recursive to the last cell, if this call returned true, then the board is valid
				        return true;
			        }
		        }
	        }
	        // rollback this cell
	        // very critical to do this, otherwise bad values will cause the board to be invalid
	        m_iGeneratedPuzzle[iRow,iColumn]	= 0;	
	        return false;

        }


    	


        void OutputGrid(string szName, int[,] iArray)
        {
        
        }

        void OutputKiller()
        {
        
        }

        //we're assuming the y is the first value in the array but it shouldn't matter
        protected bool FixNonSolvable(int[,] iPuzzleArray, int[,] iSolutionArray)
        {
            bool bRet = true;

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
                        if(iSolutionArray[y,x+1] == iSolutionArray[j,x] &&
                            iSolutionArray[y,x] == iSolutionArray[j,x+1])
                        {
                            //we found a potential problem
                            //check if its unsolvable
                            if(GetSamuIndex(y,x) == GetSamuIndex(y, x+1) &&
                                GetSamuIndex(j,x) == GetSamuIndex(j, x+1))
                            {
                                //DebugOut(_T("(%i, %i)  (%i, %i)  [%i,%i] Non-unique"), x,y,x+1,j,iSolutionArray[y,x], iSolutionArray[y,x+1]);
                                ptNum1.y = x;
                                ptNum1.x = y;
                                ptNum2.y = x+1;
                                ptNum2.x = y;


                                ptNum3.y = x;
                                ptNum3.x = j;
                                ptNum4.y = x+1;
                                ptNum4.x = j;

                                if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, true))
                                { }
                                else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, true))
                                { }
                                else
                                {
                                    bRet = false;
                                    goto error;
                                }
                        

                            }
                            //else
                                //DebugOut(_T("(%i, %i)  (%i, %i)  [%i,%i]"), x,y,x+1,j,iSolutionArray[y,x], iSolutionArray[y,x+1]);
                        }
                        else if(iSolutionArray[y,x+1] == iSolutionArray[j,x])
                        {
                            for(int j2 = j; j2 < 9; j2++)
                            {
                                if(iSolutionArray[y,x] == iSolutionArray[j2,x+1] &&
                                    iSolutionArray[j2,x] == iSolutionArray[j,x+1])   
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

                                        //DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i,%i,%i] h1 Non-unique"), x,y,x,j,x,j2,iSolutionArray[y,x], iSolutionArray[y,x+1], iSolutionArray[j,x+1]);

                                        if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, true))
                                        { }
                                        else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, true))
                                        { }
                                        else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum5, ptNum6, true))
                                        { }
                                        else
                                        {
                                            bRet = false;
                                            goto error;
                                        }
                                    }
                                    //else 
                                        //DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i,%i,%i] h1"), x,y,x,j,x,j2,iSolutionArray[y,x], iSolutionArray[y,x+1], iSolutionArray[j,x+1]);
                                }
                            }
                        }
                        else if(iSolutionArray[y,x] == iSolutionArray[j,x+1])
                        {
                            for(int j2 = j; j2 < 9; j2++)
                            {
                                if(iSolutionArray[y,x+1] == iSolutionArray[j2,x] &&
                                    iSolutionArray[j2,x+1] == iSolutionArray[j,x])   
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

                                        //DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i,%i,%i] h2 Non-unique"), x,y,x,j,x,j2,iSolutionArray[y,x], iSolutionArray[y,x+1], iSolutionArray[j,x]);

                                        if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, true))
                                        { }
                                        else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, true))
                                        { }
                                        else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum5, ptNum6, true))
                                        { }
                                        else
                                        {
                                            bRet = false;
                                            goto error;
                                        }
                                    }

                                //    else 
                                        //DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i,%i,%i] h2"), x,y,x,j,x,j2,iSolutionArray[y,x], iSolutionArray[y,x+1], iSolutionArray[j,x]);
                                }
                            }
                        }
                    }

                    //now lets check the verticals
                    for(int i = x; i <9; i++)
                    {
                        if(iSolutionArray[y+1,x] == iSolutionArray[y,i] &&
                            iSolutionArray[y,x] == iSolutionArray[y+1,i])
                        {
                            //we found a potential problem
                            //check if its unsolvable
                            if(GetSamuIndex(y,x) == GetSamuIndex(y+1, x) &&
                                GetSamuIndex(y,i) == GetSamuIndex(y+1, i))
                            {
                                //DebugOut(_T("(%i, %i)  (%i, %i)  [%i,%i] Non-unique"), x,y,i,y+1,iSolutionArray[y,x], iSolutionArray[y+1,x]);
                                ptNum1.y = x;
                                ptNum1.x = y;
                                ptNum2.y = x;
                                ptNum2.x = y+1;

                                ptNum3.y = i;
                                ptNum3.x = y;
                                ptNum4.y = i;
                                ptNum4.x = y+1;

                                if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, false))
                                {}
                                else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, false))
                                {}
                                else
                                {
                                    bRet = false;
                                    goto error;
                                }
                            }
                            ///else
                                //DebugOut(_T("(%i, %i)  (%i, %i)  [%i,%i]"), x,y,i,y+1,iSolutionArray[y,x], iSolutionArray[y+1,x]);
                        }
                        else if(iSolutionArray[y,x] == iSolutionArray[y+1,i])
                        {
                            for(int i2 = i; i2 < 9; i2++)
                            {
                                if(iSolutionArray[y,i] == iSolutionArray[y+1,i2] &&
                                    iSolutionArray[y,i2] == iSolutionArray[y+1,x])   
                                {
                                    if(GetSamuIndex(y,x) == GetSamuIndex(y+1, x) &&
                                        GetSamuIndex(y,i) == GetSamuIndex(y+1, i) &&
                                        GetSamuIndex(y,i2) == GetSamuIndex(y+1, i2))
                                    {
                                        //DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i,%i,%i] v1 Non-unique"), x,y,i,y,i2,y,iSolutionArray[y,x], iSolutionArray[y+1,x], iSolutionArray[y,i]);
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

                                        if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, false))
                                        {}
                                        else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, false))
                                        {}
                                        else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum5, ptNum6, false))
                                        {}
                                        else
                                        {
                                            bRet = false;
                                            goto error;
                                        }
                                

                                    }
                                    //else
                                        //DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i,%i,%i] v1"), x,y,i,y,i2,y,iSolutionArray[y,x], iSolutionArray[y+1,x], iSolutionArray[y,i]);
                                }
                            }
                        }

                        else if(iSolutionArray[y+1,x] == iSolutionArray[y,i])
                        {
                            for(int i2 = i; i2 < 9; i2++)
                            {
                                if(iSolutionArray[y+1,i] == iSolutionArray[y,i2] &&
                                    iSolutionArray[y+1,i2] == iSolutionArray[y,x])   
                                {
                                    if(GetSamuIndex(y,x) == GetSamuIndex(y+1, x) &&
                                        GetSamuIndex(y,i) == GetSamuIndex(y+1, i) &&
                                        GetSamuIndex(y,i2) == GetSamuIndex(y+1, i2))
                                    {
  //                                      DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i,%i,%i] v2 Non-unique"), x,y,i,y,i2,y,iSolutionArray[y,x], iSolutionArray[y+1,x], iSolutionArray[y+1,i]);
                             
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

                                        if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum1, ptNum2, false))
                                        {}
                                        else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum3, ptNum4, false))
                                        {}
                                        else if(SplitKiller(iPuzzleArray, iSolutionArray, ptNum5, ptNum6, false))
                                        {}
                                        else
                                        {
                                            bRet = false;
                                            goto error;
                                        }

                                    }
//                                    else
//                                        DebugOut(_T("(%i, %i)  (%i, %i)  (%i, %i)  [%i,%i,%i] v2"), x,y,i,y,i2,y,iSolutionArray[y,x], iSolutionArray[y+1,x], iSolutionArray[y+1,i]);
                                }
                            }
                        }
                    }
                }
            }
        error:
            return bRet;
        }

        bool SplitKiller(int[,] iPuzzleArray, int[,] iSolutionArray, POINT pt1, POINT pt2, bool bHorizontal)
        {
            //so we need to add a new samu section and then check if we need to modify any other values/samus
            //lets just always change the second and work our way down
            m_iNumSamus++;

            int iSamuIndex1 = GetSamuIndex(pt1.x, pt1.y);
            int iSamuIndex2 = GetSamuIndex(pt2.x, pt2.y);

            int iSamuID = GetSamuIndex(pt2.x, pt2.y);

            if(m_iSamuCounts[iSamuID] != 2)
                return false;

            m_iSamuCounts[iSamuID]--; //reduce the count by one
            m_iSamunamupures[iSamuID] -= iSolutionArray[pt2.x,pt2.y];

    
            m_iSamud[pt2.x,pt2.y] = m_iNumSamus-1;;
            m_iSamuCounts[m_iNumSamus-1] = 1;
            m_iSamunamupures[m_iNumSamus-1] = iSolutionArray[pt2.x,pt2.y];

            iPuzzleArray[pt2.x,pt2.y] = m_iNumSamus-1;

            return true;
        }


        //1 check per number per subset pair
        //slight optimization ... only check 1-8 ... 9 will be redundant
        //we don't really have to worry about which is x or y as long as we're consistent
        //m_iGeneratedPuzzle[iRow,iColumn]
        bool FixNonSolvable2(int[,] iPuzzleArray, int[,] iSolutionArray)
        {
            POINT ptNum1;
            POINT ptNum2;
            POINT ptCheck1;
            POINT ptCheck2;

            POINT ptSubSet1;
            POINT ptSubSet2;

            int iXTemp, iYTemp;

            ptNum1.x = -1;
            ptNum1.y = -1;
            ptNum2.x = -1;
            ptNum2.y = -1;

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

                                if(iNum == iSolutionArray[iYTemp,iXTemp])
                                {   //found it in the first set
                                    ptNum1.x = iXTemp;
                                    ptNum1.y = iYTemp;
                                }

                                iXTemp = x + ptSubSet2.x * 3;      
                                iYTemp = y + ptSubSet2.y * 3;

                                if(iNum == iSolutionArray[iYTemp,iXTemp])
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
                        if(iSolutionArray[ptCheck2.y,ptCheck2.x] == iSolutionArray[ptCheck1.y,ptCheck1.x])
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
                                m_iSamud[ptCurCell.y,ptCurCell.x] = iNewID;
                                m_iSamunamupures[iNewID] += iSolutionArray[ptCurCell.y,ptCurCell.x];
                                m_iSamuCounts[iNewID]++;

                                m_iSamunamupures[iPrevID] -= iSolutionArray[ptCurCell.y,ptCurCell.x];
                                m_iSamuCounts[iPrevID]--;


                                if(m_iSamunamupures[iPrevID] < 0)
                                    return false;//never hits so we're good ...
                                if(m_iSamunamupures[iNewID] < 0)
                                    return false;

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
                                    m_iSamud[ptCurCell.y,ptCurCell.x] = iNewID;
                                    m_iSamunamupures[iNewID] += iSolutionArray[ptCurCell.y,ptCurCell.x];
                                    m_iSamuCounts[iNewID]++;

                                    m_iSamunamupures[iPrevID] -= iSolutionArray[ptCurCell.y,ptCurCell.x];
                                    m_iSamuCounts[iPrevID]--;

                                    OutputKiller();

                                    if(m_iSamunamupures[iPrevID] < 0)
                                        return false;
                                    if(m_iSamunamupures[iNewID] < 0)
                                        return false;
                                }
                            }
                        }
                    }
                }
            }
            return true;

        }

    }
}
