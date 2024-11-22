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
    public enum EnumGreaterDir
    {
        GDIR_Left,
        GDIR_Right,
        GDIR_Up,
        GDIR_Down,
        GDIR_Unknown,
    };

    public enum EnumOperator
    {
        OP_Plus,
        OP_Minus,
        OP_Times,
        OP_Divide,
        OP_None,
    };

    
    public class KenKenPuzzle : KillerPuzzle
    {
        int[] m_iKenKens = new int[MAX_SAMUNAMPURES]; //the kenken values ... ie the math answers 
        EnumOperator[] m_eKenKenOps = new EnumOperator[MAX_SAMUNAMPURES]; //value of each group of numbers

        public KenKenPuzzle()
        {

        }

        void GenerateKenKen(EnumDifficulty eDiff)
        {
            //first generate the ol Samu
          //  GenerateSamu(eDiff);

            //now its time to get to work 
            //the samu engine now counts the number of values per section ... but we need to find the specific numbers per
            byte[,] btArrValuesPerGroup = new byte[MAX_SAMUNAMPURES,10];
            byte[] btIndex = new byte[MAX_SAMUNAMPURES];

            int iRow;

            for(int x = 0; x < 9; x++)
            {
                for(int y = 0; y < 9; y++)
                {
                    iRow = m_iSamud[x,y];
                    btArrValuesPerGroup[iRow,btIndex[iRow]] = (byte)m_iGeneratedPuzzle[x,y];
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
                        if(btArrValuesPerGroup[i,iSwap+1] > btArrValuesPerGroup[i,iSwap])
                        {   //swap
                            btTemp = btArrValuesPerGroup[i,iSwap+1];
                            btArrValuesPerGroup[i,iSwap+1] = btArrValuesPerGroup[i,iSwap];
                            btArrValuesPerGroup[i,iSwap] = btTemp;
                        }
                        else 
                            iSwap = 0;//break out 
                        iSwap--;
                    }
                }
            }

            double dbTemp;
            int iTemp;
            bool bSet;

            for(int c = 0; c < m_iNumSamus; c++)
            {
                bSet = false;
                dbTemp = (double)btArrValuesPerGroup[c,0];

                //check for divide
                for(int d = 0; d < btIndex[c]-1; d++)
                {
                    if(c%5 == 0)
                        break;//keep the mix better
                    dbTemp = dbTemp/(double)btArrValuesPerGroup[c,d+1];
                    if(dbTemp != (int)dbTemp || dbTemp == 1)
                        break;//then onto the next check
                    if(d == btIndex[c]-2)
                    {   //we're good
                        bSet = true;
                        m_eKenKenOps[c] = EnumOperator.OP_Divide;
                        m_iKenKens[c] = (int)dbTemp;
                    }
                }

                if(bSet == true)
                    continue;



                iTemp = btArrValuesPerGroup[c,0];

                for(int d = 0; d < btIndex[c]-1;d++)
                {
                    if(c%3 == 0 || c%5 == 0)
                        break;//keep the mix better
                    iTemp = iTemp - btArrValuesPerGroup[c,d+1];
                    if(iTemp < 2)
                        break;
                    if(d == btIndex[c]-2)
                    {   //we're good
                        bSet = true;
                        m_eKenKenOps[c] = EnumOperator.OP_Minus;
                        m_iKenKens[c] = iTemp;
                    }
                }

                if(bSet == true)
                    continue;


                iTemp = btArrValuesPerGroup[c,0];
                if(c%2 == 0 && btIndex[c] > 1)
                {//multiply
                    for(int d = 1; d < btIndex[c]; d++)
                        iTemp *= btArrValuesPerGroup[c,d];
                    if(iTemp < 1000)
                    {
                        m_eKenKenOps[c] = EnumOperator.OP_Times;
                        m_iKenKens[c] = iTemp;
                        continue;
                    }//otherwise we'll just do plus
                }
        
                //else
                iTemp = btArrValuesPerGroup[c,0];
                {//plus
                    for(int d = 1; d < btIndex[c]; d++)
                        iTemp += btArrValuesPerGroup[c,d];
                    m_eKenKenOps[c] = EnumOperator.OP_Plus;
                    m_iKenKens[c] = iTemp;
                }

                //otherwise minus

                //then alt between plus or times
            }
        }

        public override bool GetNewPuzzle(EnumDifficulty eDifficulty, int[,] iPuzzleArray, int[,] iSolutionArray)
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

            //first generate the ol Samu
            GenerateSamu(eDifficulty);

            //and finally ensure its solvable
            if(FixNonSolvable(iPuzzleArray, iSolutionArray) == false)
            {
                Reset();
                return GetNewPuzzle(eDifficulty, iPuzzleArray, iSolutionArray);
            }

            GenerateKenKen(eDifficulty);

            // now we have a puzzle with the desired difficulty level, so end the function
            return true;
        }

        public int GetKenKen(int iIndex)
        {
            if(iIndex < 0 || iIndex >= MAX_SAMUNAMPURES)
                return -1;

            return m_iKenKens[iIndex];
        }

        public EnumOperator GetOp(int iIndex)
        {
            if(iIndex < 0 || iIndex >= MAX_SAMUNAMPURES)
                return EnumOperator.OP_None;

            return m_eKenKenOps[iIndex];
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
                iSamuSize = rand.Next()%4 + 2;
                break;
            default:
                break;
            }

            return iSamuSize;
        }

 

    }
}
