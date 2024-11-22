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
    public class GreaterPuzzle : SudokuPuzzle
    {
        public override void GetNewPuzzle(EnumDifficulty eDifficulty, ref int[,] iPuzzleArray, ref int[,] iSolutionArray)
        {
            // generate a puzzle and copy m_iGeneratedPuzzle[NUM_ROWS,NUM_COLUMNS] to the argument
            // blank out the puzzle array
            GeneratePuzzle();
   
            for (int i = 0; i < Globals.NUM_ROWS; i++)
            {
                for (int j = 0; j < Globals.NUM_COLUMNS; j++)
                {
                    iSolutionArray[i,j]	= m_iGeneratedPuzzle[i,j];
                    iPuzzleArray[i,j]		= 0;
                }
            }

            int iHintCount = 0;

            //we'll add some givens to make it easier later

            // determine number of extra cells to add to make the puzzle easier, once we have a solvable puzzle
            switch(eDifficulty)
            {
            case EnumDifficulty.DIFF_VeryHard:
                iHintCount = 0;
                break;
            case EnumDifficulty.DIFF_Hard:
                iHintCount = 2;
                break;
            case EnumDifficulty.DIFF_Medium:
                iHintCount = 5;
                break;
            case EnumDifficulty.DIFF_Easy:
                iHintCount = 12;
                break;
            case EnumDifficulty.DIFF_VeryEasy:
            default:
                iHintCount = 20;
                break;
            }

            for (int i = 0; i < iHintCount; i++)
            {
                //bugbug
                //set some givens
            }
        } 

        
    }
}
