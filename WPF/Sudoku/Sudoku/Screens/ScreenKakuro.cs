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

namespace Sudoku
{
    public class ScreenKakuro : IScreenGame
    {
        public ScreenKakuro()
        {
        }
        
        public void Initialize(ref Grid gridBoard, ref Grid gridOverlay)
        {

            for (int i = 0; i < Globals.Game.GetGridSize(); i++)
            {
                ColumnDefinition c = new ColumnDefinition();
                RowDefinition r = new RowDefinition();


                gridBoard.ColumnDefinitions.Add(c);
                gridBoard.RowDefinitions.Add(r);
            }

            int iColumn;
            int iRow;

            for (int y = 0; y < Globals.Game.GetGridSize(); y++)
            {
                for (int x = 0; x < Globals.Game.GetGridSize(); x++)
                {
                    iColumn = x;
                    iRow = y;

                    BoardPiece b = new BoardPiece(x, y);

                    Grid.SetColumn(b, iColumn);
                    Grid.SetRow(b, iRow);

                    b.HorizontalAlignment = HorizontalAlignment.Stretch;
                    b.VerticalAlignment = VerticalAlignment.Stretch;

                 /*   if (Globals.Game.GetHorzTotal(x, y) > 0)
                        b.UpdateHTotal(Globals.Game.GetHorzTotal(x, y));

                    if (Globals.Game.GetVertTotal(x, y) > 0)
                        b.UpdateVTotal(Globals.Game.GetVertTotal(x, y));*/

                    SetPiece(ref b, x, y);
                    

                    gridBoard.Children.Add(b);
                }
            }
        }

        public void ResetBoard(ref Grid gridBoard, ref Grid gridOverlay)
        {
            BoardPiece b = null;

            for (int i = 0; i < gridBoard.Children.Count; i++)
            {
                b = (BoardPiece)gridBoard.Children[i];
                SetPiece(ref b, b.iX, b.iY);
            }
        }

        void SetPiece(ref BoardPiece b, int x, int y)
        {
            b.UpdateGuess(0);
            b.UpdatePencilMarks(0);

            b.UpdateHTotal(Globals.Game.GetHorzTotal(x, y));
            b.UpdateVTotal(Globals.Game.GetVertTotal(x, y));

            if (Globals.Game.GetGuess(x, y) > 0)
                b.UpdateGuess(Globals.Game.GetGuess(x, y));
            else if (Globals.Game.GetPencilMarks(x, y) > 0)
                b.UpdatePencilMarks(Globals.Game.GetPencilMarks(x, y));
            b.UpdateBackground();
        }

        public void InputNumber(int iNum)
        {
            // number selected

        }

        public void UpdateOverlay(ref Grid gridBoard, ref Grid gridOverlay)
        {

        }

    } 
}
