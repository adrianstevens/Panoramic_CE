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
    public class KenKenGame: KillerGame
    {
     /*   struct TypeGameState	// used for saving a complete game set
        {
            int[,] iTiles;//[NUM_ROWS][NUM_COLUMNS];
            int[,] iSamuIndex;//[NUM_ROWS][NUM_COLUMNS];//index for the colors
            int[,] iSamuValue;//[NUM_ROWS][NUM_COLUMNS];//total within the Samu (yeah I'm making up words)

            int[] iKenKens;//[MAX_SAMUNAMPURES]; //the kenken values ... ie the math answers 
            int[] eKenKenOps;//[MAX_SAMUNAMPURES]; //value of each group of numbers

            int iNumFreeCells;		// free cells in the game
            int iNumSeconds;
            int iScore;
        };*/


        static int MAX_SAMUNAMPURES = 81;
        private const string FILE_KenKen = "kenken.txt";
        private const string FILE_KenKenOps = "kenkenops.txt";

        int[]				  m_iKenKens = new int[MAX_SAMUNAMPURES]; //the kenken values ... ie the math answers 
        EnumOperator[]        m_eKenKenOps = new EnumOperator[MAX_SAMUNAMPURES]; //value of each group of numbers

        public override bool NewGame(EnumDifficulty eDifficulty)
        {
            int[,] iPuzzleArray = new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
            int[,] iSolutionArray = new int[Globals.NUM_ROWS, Globals.NUM_COLUMNS];
            KenKenPuzzle Puzzle = new KenKenPuzzle();
            // create an array used to pass back some info here about the puzzle generating performance

            Puzzle.GetNewPuzzle(eDifficulty, iPuzzleArray, iSolutionArray);

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

            for(int i = 0; i < MAX_SAMUNAMPURES; i++)
            {
                m_iKenKens[i]      = Puzzle.GetKenKen(i);
                m_eKenKenOps[i]    = Puzzle.GetOp(i);
            }

            if(m_bSetSingleValues == true)
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

            List<int> arrKenKens = new List<int>();
            List<int> arrKenKenOps = new List<int>();

            for (int x = 0; x < MAX_SAMUNAMPURES; x++)
            {
                arrKenKens.Add(m_iKenKens[x]);
                arrKenKenOps.Add((int)m_eKenKenOps[x]);
            }

            try
            {
                XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                TextWriter writer = new StreamWriter(FILE_KenKen);
                ser.Serialize(writer, arrKenKens);
                writer.Close();

                writer = new StreamWriter(FILE_KenKenOps);
                ser.Serialize(writer, arrKenKenOps);
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
                TextReader reader = new StreamReader(FILE_KenKen);

                //and serialize data
                List<int> arrTiles = (List<int>)ser.Deserialize(reader);

                if (arrTiles.Count == MAX_SAMUNAMPURES)
                {
                    for (int x = 0; x < MAX_SAMUNAMPURES; x++)
                    {
                        m_iKenKens[x] = arrTiles[x];
                    }
                }

                reader.Close();

                reader = new StreamReader(FILE_KenKenOps);
                //and serialize data
                arrTiles = (List<int>)ser.Deserialize(reader);

                if (arrTiles.Count == MAX_SAMUNAMPURES)
                {
                    for (int x = 0; x < MAX_SAMUNAMPURES; x++)
                    {
                        m_eKenKenOps[x] = (EnumOperator)arrTiles[x];
                    }
                }
                reader.Close();
            }
            catch (System.Exception ex)
            {
            }

            m_bIsGameInPlay = true;
        }

        public int GetMathAnswer(int iX, int iY)
        {
            return m_iKenKens[m_iSamuIndex[iX,iY]];
        }

        public EnumOperator GetOp(int iX, int iY)
        {
            return m_eKenKenOps[m_iSamuIndex[iX,iY]];
         //   return EnumOperator.OP_None;
        }

        public new bool IsPencilMarkValid(int iX, int iY, int iValue)
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
            case EnumOperator.OP_Plus:
                iGuess = GetAddPossibilities(iSize, iTotal);
                break;
            case EnumOperator.OP_Minus:
                iGuess = GetSubtractPossibilities(iSize, iTotal);
                break;
            case EnumOperator.OP_Times:
                iGuess = GetMultiplyPossibilities(iSize, iTotal);
                break;
            case EnumOperator.OP_Divide:
                iGuess = GetDividePossibilities(iSize, iTotal);
                break;
            }
            if(IsGuessValid(iGuess, iValue) == false)//plain sudoku rules check
                return false;

            //return base.IsPencilMarkValid(iX, iY, iValue);
            return IsPencilValid(iX, iY, iValue);

        }

        int GetSubtractPossibilities(int iSize, int iTotal)
        {
            int iGuess = 511; // default to all possibilities

            for(int i=0; i< Globals.sSubNumList.GetLength(0); i++)
            {
                if (iSize == Globals.sSubNumList[i,0] && iTotal == Globals.sSubNumList[i,1])
                {
                    iGuess = Globals.sSubNumList[i,2];
                    break;
                }
            }
            return iGuess;
        }

        int GetMultiplyPossibilities(int iSize, int iTotal)
        {
            int iGuess = 511; // default to all possibilities

            for(int i=0; i< Globals.sMultiNumList.GetLength(0); i++)
            {
                if(iSize == Globals.sMultiNumList[i,0] && iTotal == Globals.sMultiNumList[i,1])
                {
                    iGuess = Globals.sMultiNumList[i,2];
                    break;
                }
                else if(iSize < Globals.sMultiNumList[i,0])
                {
                    break;
                }

            }
            return iGuess;
        }

        int GetDividePossibilities(int iSize, int iTotal)
        {
            int iGuess = 511; // default to all possibilities

            for(int i=0; i< Globals.sDivNumList.GetLength(0); i++)
            {
                if(iSize == Globals.sDivNumList[i,0] && iTotal == Globals.sDivNumList[i,1])
                {
                    iGuess = Globals.sDivNumList[i,2];
                    break;
                }
            }
            return iGuess;
        }

        int GetAddPossibilities(int iSize, int iTotal)
        {
            return GetGuessPossibilities(iSize, iTotal);
        }


    }
}
