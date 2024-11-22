using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace Sudoku
{
    public class ScreenKillerSudoku : IScreenGame
    {
        protected const int BOARD_SIZE = 9;
    
        public ScreenKillerSudoku()
        {
        }

        public virtual void Initialize(ref Grid gridBoard, ref Grid gridOverlay)
        {
            for (int i = 0; i < BOARD_SIZE; i++)
            {
                ColumnDefinition c = new ColumnDefinition();
                RowDefinition r = new RowDefinition();

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

                    BoardPiece b = new BoardPiece(x, y);

                    Grid.SetColumn(b, iColumn);
                    Grid.SetRow(b, iRow);

                    b.HorizontalAlignment = HorizontalAlignment.Stretch;
                    b.VerticalAlignment = VerticalAlignment.Stretch;

                    gridBoard.Children.Add(b);

                    SetPiece(ref b, x, y);

                }
            }

          //  WriteableBitmap outline = new WriteableBitmap((int)imgBoard.Width, (int)imgBoard.Height, 96, 96, PixelFormats.Bgr32, null);
          //  imgBoard.Source = outline;

            DrawOutlines(ref gridOverlay, ref gridBoard);
            
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

        public virtual void UpdateOverlay(ref Grid gridBoard, ref Grid gridOverlay)
        {
            DrawOutlines(ref gridOverlay, ref gridBoard);
        }

        public virtual void SetPiece(ref BoardPiece b, int x, int y)
        {

            b.UpdateGuess(0);
            b.UpdatePencilMarks(0);
            b.UpdateGroupTotal(Globals.Game.GetTotal(x, y), Globals.Game.DrawTotal(x, y));
            
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

        void DrawLine(int x1, int y1, int x2, int y2, Color crOutline, ref Grid gridOver)
        {
            Line myLine = new Line();

            myLine.StrokeThickness = 2;
            myLine.Stroke = Globals.brush.GetBrush(crOutline);

            myLine.X1 = x1;
            myLine.X2 = x2;
            myLine.Y1 = y1;
            myLine.Y2 = y2;

            gridOver.Children.Add(myLine);
        }

        protected void DrawOutlines(ref Grid gridOverlay, ref Grid gridBoard)
        {
            gridOverlay.Children.Clear();

            int iLineIndent = 4;
            int x1, y1, x2, y2;
            int left, top, right, bottom;


            double dbWidth = gridBoard.ActualWidth/BOARD_SIZE;
            double dbHeight = gridBoard.ActualHeight / BOARD_SIZE;

            for (int y = 0; y < BOARD_SIZE; y++)
            {
                for (int x = 0; x < BOARD_SIZE; x++)
                {
                    int dwType = Globals.Game.GetOutline(x, y);

                    Color crOutline = Globals.gui.GetOutlineColor(Globals.Game.GetSamuIndex(x, y), false);

                    left = (int)(dbWidth * x);
                    top = (int)(dbHeight * y);
                    right = (int)(dbWidth * (x + 1));
                    bottom = (int)(dbHeight * (y + 1));                  


                    //so the 4 overdraw functions draw outside of the cell if needed ... so we are drawing parts of the outline twice ... don't care

                    //over draw left
                    if (x > 0 &&
                        Globals.Game.GetSamuIndex(x, y) == Globals.Game.GetSamuIndex(x - 1, y))
                    {
                        x1 = left + iLineIndent;
                        y1 = top + iLineIndent;
                        x2 = left - iLineIndent;
                        y2 = y1;

                        DrawLine(x1, y1, x2,y2, crOutline, ref gridOverlay);
           
                        y1 = bottom - iLineIndent - 1;
                        y2 = y1;

                        DrawLine(x1, y1, x2, y2, crOutline, ref gridOverlay);
                    }

                    //over draw right
                    if (x < BOARD_SIZE - 1 &&
                        Globals.Game.GetSamuIndex(x, y) == Globals.Game.GetSamuIndex(x + 1, y))
                    {
                        x1 = right - iLineIndent;
                        y1 = top + iLineIndent;
                        x2 = right + iLineIndent;
                        y2 = y1;

                        DrawLine(x1, y1, x2, y2, crOutline, ref gridOverlay);

                        y1 = bottom - iLineIndent - 1;
                        y2 = y1;

                        DrawLine(x1, y1, x2, y2, crOutline, ref gridOverlay);
                    }

                    //overdraw top
                    if (y > 0 &&
                        Globals.Game.GetSamuIndex(x, y) == Globals.Game.GetSamuIndex(x, y - 1))
                    {
                        x1 = left + iLineIndent;
                        y1 = top + iLineIndent;
                        x2 = x1;
                        y2 = top - iLineIndent;

                        DrawLine(x1, y1, x2, y2, crOutline, ref gridOverlay);

                        x1 = right - iLineIndent - 1;
                        x2 = x1;

                        DrawLine(x1, y1, x2, y2, crOutline, ref gridOverlay);
                    }

                    //overdraw bottom
                    if (y < BOARD_SIZE - 1 &&
                        Globals.Game.GetSamuIndex(x, y) == Globals.Game.GetSamuIndex(x, y + 1))
                    {
                        x1 = left + iLineIndent;
                        y1 = bottom - iLineIndent;
                        x2 = x1;
                        y2 = bottom + iLineIndent;
                        //y2 = m_rcGrid[x][y+1].top;

                        DrawLine(x1, y1, x2, y2, crOutline, ref gridOverlay);

                        x1 = right - iLineIndent - 1;
                        x2 = x1;

                        DrawLine(x1, y1, x2, y2, crOutline, ref gridOverlay);
                    }

                    //draw left line
                    if ((x > 0 &&
                        (Globals.Game.GetSamuIndex(x, y) != Globals.Game.GetSamuIndex(x - 1, y))) ||
                        x == 0)
                    {
                        x1 = left + iLineIndent;
                        y1 = top + iLineIndent;
                        x2 = x1;
                        y2 = bottom - iLineIndent;
                        DrawLine(x1, y1, x2, y2, crOutline, ref gridOverlay);
                    }

                    //draw right line
                    if ((x < 8 &&
                        (Globals.Game.GetSamuIndex(x, y) != Globals.Game.GetSamuIndex(x + 1, y))) ||
                        x == 8)
                    {
                        x1 = right - iLineIndent - 1;
                        y1 = top + iLineIndent;
                        x2 = x1;
                        y2 = bottom - iLineIndent;

                        DrawLine(x1, y1, x2, y2, crOutline, ref gridOverlay);
                    }

                    //draw top line
                    if ((y > 0 &&
                        (Globals.Game.GetSamuIndex(x, y) != Globals.Game.GetSamuIndex(x, y - 1))) ||
                        y == 0)
                    {
                        x1 = left + iLineIndent;
                        y1 = top + iLineIndent;
                        x2 = right - iLineIndent;
                        y2 = y1;

                        DrawLine(x1, y1, x2, y2, crOutline, ref gridOverlay);
                    }


                    //draw bottom line
                    if ((y < 8 &&
                        (Globals.Game.GetSamuIndex(x, y) != Globals.Game.GetSamuIndex(x, y + 1))) ||
                        y == 8)
                    {
                        x1 = left + iLineIndent;
                        y1 = bottom - iLineIndent - 1;
                        x2 = right - iLineIndent;
                        y2 = y1;

                        DrawLine(x1, y1, x2, y2, crOutline, ref gridOverlay);
                    }
                }
            }
        }
    }
}
