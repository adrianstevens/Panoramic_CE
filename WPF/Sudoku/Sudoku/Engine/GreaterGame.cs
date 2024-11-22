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
    public class GreaterGame : SudokuGame
    {

        public EnumGreaterDir GetGVertical(int iX, int iY)
        {
            if (iY < 1)
                return EnumGreaterDir.GDIR_Unknown;//same as blank
            if (iY % 3 == 0)
                return EnumGreaterDir.GDIR_Unknown;//same as blank

            if (GetSolution(iX, iY - 1) > GetSolution(iX, iY))
                return EnumGreaterDir.GDIR_Up;
            else
                return EnumGreaterDir.GDIR_Down;
        }

        public EnumGreaterDir GetGHorizontal(int iX, int iY)
        {
            if (iX < 1)
                return EnumGreaterDir.GDIR_Unknown;//same as blank
            if (iX % 3 == 0)
                return EnumGreaterDir.GDIR_Unknown;//same as blank

            if (GetSolution(iX - 1, iY) > GetSolution(iX, iY))
                return EnumGreaterDir.GDIR_Left;
            else
                return EnumGreaterDir.GDIR_Right;
        }

        //new means it replaces
        public override bool NewGame(EnumDifficulty eDifficulty)
        {
             // clear the board off, reset everything, then load a new puzzle
            // get the new puzzle
            int[,] iPuzzleArray = new int[Globals.NUM_ROWS, Globals.NUM_COLUMNS];
            int [,]iSolutionArray = new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
            GreaterPuzzle Puzzle = new GreaterPuzzle();
            // create an array used to pass back some info here about the puzzle generating performance
            Puzzle.GetNewPuzzle(m_eDifficulty, ref iPuzzleArray, ref iSolutionArray);

            // reset our tile pieces
            NewBoard();

            
            // reset all the cells with the new puzzle
            for (int i = 0; i < Globals.NUM_ROWS; i++)				// y
            {
                for (int j = 0; j < Globals.NUM_COLUMNS; j++)		// x
                {
                    SetSolution(j, i, iSolutionArray[i,j]);
                    SetIsGiven(j, i, (iPuzzleArray[i,j]==0?false:true));
                }
            }

            //now some givens
            int iGivenCount = 0;

            m_eDifficulty = eDifficulty;

            switch(m_eDifficulty)
            {
            default:
            case EnumDifficulty.DIFF_VeryEasy:
                iGivenCount = 27;
                break;
            case EnumDifficulty.DIFF_Easy:
                iGivenCount = 18;
                break;
            case EnumDifficulty.DIFF_Medium:
                iGivenCount = 9;
                break;
            case EnumDifficulty.DIFF_Hard:
                iGivenCount = 3;
                break;
            case EnumDifficulty.DIFF_VeryHard:
                iGivenCount = 0;
                break;
            }

            int iX, iY;

            Random rand = new Random(System.DateTime.Today.Minute + System.Environment.TickCount);

            while(iGivenCount > 0)
            {
                iX = rand.Next()%9;
                iY = rand.Next() % 9;

                if(iPuzzleArray[iX,iY] == 0)
                {
                    SetIsGiven(iY, iX, true);
                    //iPuzzleArray[iX,iY] = iSolutionArray[iX,iY];
                    iGivenCount--;
                }      
            }

            m_bIsGameInPlay = true;
            m_iNumFreeCells = 0;
            m_iNumSeconds	= 0;
            m_iScore		= 0;
            m_ptLastCorrectGuess.x	= -1;
            m_ptLastCorrectGuess.y	= -1;
            m_sLastMove.eLastMove = EnumLastMove.LASTMOVE_None;

            // check the full solution for any locked states
            CheckSolution();

            // see how many free cells there are
            CalcFreeCells();
            AutoFillPencilMarks();

            return true;
        }

        public override bool IsPencilMarkValid(int iX, int iY, int iValue)
        {
            //check all four directions 
            int iMin = 1;
            int iMax = 9;

            //check symobol and value  ... symbols are assigned above and left 
            if (GetGHorizontal(iX, iY) == EnumGreaterDir.GDIR_Left)
                iMax--;
            else if (GetGHorizontal(iX, iY) == EnumGreaterDir.GDIR_Right)
                iMin++;

            if (GetGHorizontal(iX+1, iY) == EnumGreaterDir.GDIR_Left)
                iMin++;
            else if (GetGHorizontal(iX+1, iY) == EnumGreaterDir.GDIR_Right)
                iMax--;

            if (GetGVertical(iX, iY) == EnumGreaterDir.GDIR_Up)
                iMax--;
            else if (GetGVertical(iX, iY) == EnumGreaterDir.GDIR_Down)
                iMin++;

            if (GetGVertical(iX, iY+1) == EnumGreaterDir.GDIR_Up)
                iMin++;
            else if (GetGVertical(iX, iY+1) == EnumGreaterDir.GDIR_Down)
                iMax--;

            int iLeft = 0; if(iX > 0) iLeft = (IsGiven(iX - 1, iY) ? GetSolution(iX - 1, iY) : GetGuess(iX - 1, iY));
            int iRight = 0;  if(iX < 8) iRight = (IsGiven(iX + 1, iY) ? GetSolution(iX + 1, iY) : GetGuess(iX + 1, iY));
            int iTop = 0;  if(iY > 0) iTop = (IsGiven(iX, iY - 1) ? GetSolution(iX, iY - 1) : GetGuess(iX, iY - 1));
            int iBottom = 0; if(iY < 8) iBottom = (IsGiven(iX, iY + 1) ? GetSolution(iX, iY + 1) : GetGuess(iX, iY + 1));



            //now check guessed adjacent values
            if (GetGHorizontal(iX, iY) == EnumGreaterDir.GDIR_Left && iLeft != 0)
                iMax = Math.Min(iMax, iLeft);
            else if (GetGHorizontal(iX, iY) == EnumGreaterDir.GDIR_Right && iLeft != 0)
                iMin = Math.Max(iMin, iLeft + 1);

            if (GetGHorizontal(iX + 1, iY) == EnumGreaterDir.GDIR_Left && iRight != 0)
                iMin = Math.Max(iMin, iRight + 1);
            else if (GetGHorizontal(iX + 1, iY) == EnumGreaterDir.GDIR_Right && iRight != 0)
                iMax = Math.Min(iMax, iRight - 1);

            if (GetGVertical(iX, iY) == EnumGreaterDir.GDIR_Up && iTop != 0)
                iMax = Math.Min(iMax, iTop - 1);
            else if (GetGVertical(iX, iY) == EnumGreaterDir.GDIR_Down && iTop != 0)
                iMin = Math.Max(iMin, iTop + 1);

            if (GetGVertical(iX, iY + 1) == EnumGreaterDir.GDIR_Up && iBottom != 0)
                iMin = Math.Max(iMin, iBottom + 1);
            else if (GetGVertical(iX, iY + 1) == EnumGreaterDir.GDIR_Down && iBottom != 0)
                iMax = Math.Min(iMax, iBottom - 1);





            if (iValue > iMax)
                return false;
            if (iValue < iMin)
                return false;

            return base.IsPencilMarkValid(iX, iY, iValue);
        }
    }
}
