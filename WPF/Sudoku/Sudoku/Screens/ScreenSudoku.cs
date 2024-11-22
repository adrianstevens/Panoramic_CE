using System;
using System.Net;
using System.Windows;
using System.Collections.Generic;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace Sudoku
{
    public class ScreenSudoku: IScreenGame
    {
        const int BOARD_SIZE = 9;

        public ScreenSudoku()
        {
        }

        public virtual void Initialize(ref Grid gridBoard, ref Grid gridOverlay)
        {
            // let's create our columns and rows
            double dbSpacer = 5.0;

            for (int i = 0; i < 9 + 2; i++)
            {
                ColumnDefinition c = new ColumnDefinition();
                RowDefinition r = new RowDefinition();

                if (i == 3 || i == 7)
                {
                    // add a spacer
                    c.Width = new GridLength(dbSpacer);
                    r.Height = new GridLength(dbSpacer);
                }

                gridBoard.ColumnDefinitions.Add(c);
                gridBoard.RowDefinitions.Add(r);
            }

            int iColumn;
            int iRow;

            for (int y = 0; y < BOARD_SIZE; y++)
            {
                for (int x = 0; x < BOARD_SIZE; x++)
                {
                    iColumn = x;
                    iRow = y;

                    if (x > 5)
                        iColumn += 2;
                    else if (x > 2)
                        iColumn++;

                    if (y > 5)
                        iRow += 2;
                    else if (y > 2)
                        iRow++;

                    BoardPiece b = new BoardPiece(x, y);

                    Grid.SetColumn(b, iColumn);
                    Grid.SetRow(b, iRow);

                    b.HorizontalAlignment = HorizontalAlignment.Stretch;
                    b.VerticalAlignment = VerticalAlignment.Stretch;

                    gridBoard.Children.Add(b);

                    SetPiece(ref b, x, y);
                 }
            }
        }

        public virtual void ResetBoard(ref Grid gridBoard, ref Grid gridOverlay)
        {
            BoardPiece b = null;

            for (int i = 0; i < gridBoard.Children.Count; i++)
            {
                b = (BoardPiece)gridBoard.Children[i];
                SetPiece(ref b, b.iX, b.iY);
            }
        }

        //only used for initializing and resetting the board
        void SetPiece(ref BoardPiece b, int x, int y)
        {
            b.UpdateGuess(0);
            b.UpdatePencilMarks(0);

            if (Globals.Game.IsGiven(x, y))
                b.UpdateGuess(Globals.Game.GetSolution(x, y));
            else if (Globals.Game.GetGuess(x, y) > 0)
                b.UpdateGuess(Globals.Game.GetGuess(x, y));
            else if (Globals.Game.GetPencilMarks(x, y) > 0)
                b.UpdatePencilMarks(Globals.Game.GetPencilMarks(x, y));
            b.UpdateBackground();
        }

        public void InputNumber(int iNum)
        {
            // number selected

        }

        public virtual void UpdateOverlay(ref Grid gridBoard, ref Grid gridOverlay)
        {

        }
    }
}
