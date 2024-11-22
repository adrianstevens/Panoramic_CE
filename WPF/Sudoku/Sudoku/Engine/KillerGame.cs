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
using System.IO;
using System.IO.IsolatedStorage;
using System.Xml.Serialization;
using System.Collections.Generic;

namespace Sudoku.Engine
{
    public class KillerGame: SudokuGame
    {

        private const string FILE_SamuIndex = "samuindex.txt";
        private const string FILE_SamuValue = "samuvalue.txt";

        protected int[,] m_iSamuIndex = new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];//index for the colors
        protected int[,] m_iSamuValue = new int[Globals.NUM_ROWS, Globals.NUM_COLUMNS];//total within the Samu (yeah I'm making up words)
        protected bool[] m_bDrawSamuValue = new bool[Globals.NUM_ROWS * Globals.NUM_COLUMNS];	//bool to control if we've drawn the Samu value yet or not

        protected bool   m_bSetSingleValues;//set the values for any number in a group of 1


        public KillerGame()
        {
            m_sLastMove.eLastMove = EnumLastMove.LASTMOVE_None;
            m_ptLastCorrectGuess.x	= -1;
            m_ptLastCorrectGuess.y	= -1;
            m_ptLastWrongGuess.x	= -1;
            m_ptLastWrongGuess.y	= -1;
            m_ptLastHint.x			= -1;
            m_ptLastHint.y			= -1;
            
            m_bSetSingleValues = true;
        }

        public int GetSamuIndex(int iX, int iY){return m_iSamuIndex[iX,iY];}
        public int GetSamuValue(int iX, int iY) { return m_iSamuValue[iX, iY]; }

        public override bool NewGame(EnumDifficulty eDifficulty)
        {
            int[,] iPuzzleArray = new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
            int[,] iSolutionArray = new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
            KillerPuzzle Puzzle = new KillerPuzzle();
            // create an array used to pass back some info here about the puzzle generating performance

            Puzzle.GetNewPuzzle(eDifficulty, iPuzzleArray, iSolutionArray);
    
            // reset our tile pieces
            NewBoard();
            
            // reset all the cells with the new puzzle
            for (int i = 0; i < Globals.NUM_ROWS; i++)				// y
            {
                for (int j = 0; j < Globals.NUM_COLUMNS; j++)		// x
                {
                    SetSolution(j, i, iSolutionArray[i,j]);
                    SetIsGiven(j, i, false); //SetIsGiven(j, i, (iPuzzleArray[i, j] == 0 ? false : true));
                    //this doesn't matter as long as the orientation matches
                    m_iSamuIndex[j,i] = Puzzle.GetSamuIndex(i,j);
                    m_iSamuValue[j,i] = Puzzle.GetSamuValue(i,j);
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

            m_bIsGameInPlay = true;
            m_iNumFreeCells = 0;
            m_iNumSeconds	= 0;
            m_iScore		= 0;
            m_ptLastCorrectGuess.x	= -1;
            m_ptLastCorrectGuess.y	= -1;
            m_sLastMove.eLastMove = EnumLastMove.LASTMOVE_None;

            // see how many free cells there are
            CalcFreeCells();
            AutoFillPencilMarks();

            return true;
        }

        public override void SaveGameState()
        {
            base.SaveGameState();

            List<int> arrSamuIndex = new List<int>();
            List<int> arrSamuValues = new List<int>();

            for (int y = 0; y < Globals.NUM_ROWS; y++)
            {
                for (int x = 0; x < Globals.NUM_COLUMNS; x++)
                {
                    arrSamuIndex.Add(m_iSamuIndex[x, y]);
                    arrSamuValues.Add(m_iSamuValue[x, y]);
                }
            }

            try
            {
                XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                TextWriter writer = new StreamWriter(FILE_SamuIndex);
                ser.Serialize(writer, arrSamuIndex);
                writer.Close();

                writer = new StreamWriter(FILE_SamuValue);
                ser.Serialize(writer, arrSamuValues);
                writer.Close();
            }
            catch (System.Exception ex)
            {
            }
        }

        public override void LoadGameState()
        {
            base.LoadGameState();

            XmlSerializer ser = new XmlSerializer(typeof(List<int>));

            try
            {
                TextReader reader = new StreamReader(FILE_SamuIndex);

                //and serialize data
                List<int> arrTiles = (List<int>)ser.Deserialize(reader);

                if (arrTiles.Count == Globals.NUM_COLUMNS * Globals.NUM_ROWS)
                {
                    for (int y = 0; y < Globals.NUM_ROWS; y++)
                    {
                        for (int x = 0; x < Globals.NUM_COLUMNS; x++)
                        {
                            m_iSamuIndex[x, y] = arrTiles[y * Globals.NUM_ROWS + x];
                        }
                    }
                }

                reader.Close();

                reader = new StreamReader(FILE_SamuValue);

                //and serialize data
                arrTiles = (List<int>)ser.Deserialize(reader);

                if (arrTiles.Count == Globals.NUM_COLUMNS * Globals.NUM_ROWS)
                {
                    for (int y = 0; y < Globals.NUM_ROWS; y++)
                    {
                        for (int x = 0; x < Globals.NUM_COLUMNS; x++)
                        {
                            m_iSamuValue[x, y] = arrTiles[y * Globals.NUM_ROWS + x];
                        }
                    }
                }
                reader.Close();
            }
            catch (System.Exception ex)
            {
            }
            m_bIsGameInPlay = true;
        }

        public int GetOutline(int i, int j)
        {
            int dwRet = 0;

            //we'll check for horizontal and vertical 
            if(i < Globals.NUM_COLUMNS - 1 && m_iSamuIndex[j,i] == m_iSamuIndex[j,i+1])//draw horz line
            {
                dwRet = dwRet | Globals.KOUTLINE_Horizontal;
            }

            if(j < Globals.NUM_ROWS - 1 && m_iSamuIndex[j,i] == m_iSamuIndex[j+1,i])//draw vert line
            {
                dwRet = dwRet | Globals.KOUTLINE_Vertical;
            }

            //draw left line
            if ((i > 0 && m_iSamuIndex[j, i] != m_iSamuIndex[j, i - 1]) || i == 0)
            {
                dwRet = dwRet | Globals.KOUTLINE_Left;
            }

            //draw right line
            if((i < Globals.NUM_ROWS - 1 && m_iSamuIndex[j,i] != m_iSamuIndex[j,i+1]) || i == Globals.NUM_ROWS - 1) 
            {
                dwRet = dwRet | Globals.KOUTLINE_Right;
            }

            //draw top line
            if ((j > 0 && m_iSamuIndex[j, i] != m_iSamuIndex[j - 1, i]) ||
                j == 0)
            {
                dwRet = dwRet | Globals.KOUTLINE_Top;
            }

            //draw bottom line
            if((j < Globals.NUM_COLUMNS - 1 && m_iSamuIndex[j,i] != m_iSamuIndex[j+1,i]) ||
                j == Globals.NUM_COLUMNS - 1)
            {
                dwRet = dwRet | Globals.KOUTLINE_Bottom;
            }

            return dwRet;

        }

        public int GetTotal(int iX, int iY)
        {
            return m_iSamuValue[iX,iY];//who knows ... fuck you Kev
        }

        public override bool IsPencilMarkValid(int iX, int iY, int iValue)
        {
            //check the grouping and then pass it off to Sudoku
            int iSize = GetCellsInGroup(iX, iY);
            int iTotal = GetTotal(iX, iY);
        
            //get encoded guess value
            int iGuess = GetGuessPossibilities(iSize , iTotal);

             if(IsGuessValid(iGuess, iValue) == false)
                return false;

            return base.IsPencilMarkValid(iX, iY, iValue);

        }

        protected int GetGuessPossibilities(int iNumCells, int iTotal)
        {
            int iGuess = 511; // default to all possibilities

            for(int i=0; i< Globals.sNumList.GetLength(0); i++)
            {
                if (iNumCells == Globals.sNumList[i, (int)EnumNumList.Num] && iTotal == Globals.sNumList[i, (int)EnumNumList.Total])
                {
                    iGuess = Globals.sNumList[i, (int)EnumNumList.Included];
                    break;
                }
            }
            return iGuess;
        }

        //iGuess is the encoded possible values ... kind of clever actually
        protected bool IsGuessValid(int iGuess, int iNumberToCheck)
        {
            if((iGuess & (int)Math.Pow(2, 9-iNumberToCheck)) > 0)
                return true;
            return false;
        }

        protected int GetCellsInGroup(int iX, int iY)
        {
            int iGroup = m_iSamuIndex[iX,iY];

            int iCount = 0;

            for(int i = 0; i < 9; i++)
            {
                for(int j = 0; j < 9; j++)
                {
                    if(m_iSamuIndex[i,j] == iGroup)
                        iCount++;
                }
            }
            return iCount;
        }

        public bool DrawTotal(int iX, int iY)
        {
            //if there's a guess 
            if(GetGuess(iX, iY) > 0)
                return false;

            //if there's a pencil mark
            if(GetPencilMarks(iX, iY) > 0)
                return false;

            //otherwise we loop and see if its the top left of its group
            int iGroup = m_iSamuIndex[iX,iY];

            for(int y = 0; y < iY + 1; y++)
            {
                for(int x = 0; x < 9; x++)
                {
                    if(x == iX && y == iY)
                        return true;

                    //if we hit one bail ....
                    if(m_iSamuIndex[x,y] == iGroup)
                    {
                        if(GetGuess(x, y) == 0 &&
                            GetPencilMarks(x, y) == 0)
                            return false;//we can draw here
                    }
                }
            }



          /*  for(int i = 0; i < 9; i++)
            {
                for(int j = 0; j < iY+1; j++)
                {
                    if(i == iX && j == iY)
                        return true;

                    //if we hit one bail ....
                    if(m_iSamuIndex[i,j] == iGroup)
                        return false;
                }
            }*/
  
            return false;
        }
    }
}
