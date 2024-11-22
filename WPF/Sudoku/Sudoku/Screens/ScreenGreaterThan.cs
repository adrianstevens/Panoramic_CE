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
    public class ScreenGreaterThan : ScreenSudoku
    {
        protected const int BOARD_SIZE = 9;

        public ScreenGreaterThan()
        {
        }

        public override void Initialize(ref Grid gridBoard, ref Grid gridOverlay)
        {
            base.Initialize(ref gridBoard, ref gridOverlay);

            WriteableBitmap outline = null;// = new WriteableBitmap((int)imgBoard.Width, (int)imgBoard.Height, 96, 96, PixelFormats.Bgr32, null);
            
//            imgBoard.Source = outline;

            DrawSymbols(ref gridOverlay, ref gridBoard);
            
            
        }
        void DrawSymbols(ref Grid gridOverlay, ref Grid gridBoard)
        {
            gridOverlay.Children.Clear();

            Line myLine;

            //aim for about 1/6th of the square
            int iSize = 9;

            Engine.POINT[] pt = new Engine.POINT[3];
            
            int left, top;

            double dbWidth = gridBoard.ActualWidth / BOARD_SIZE;
            double dbHeight = gridBoard.ActualHeight / BOARD_SIZE;

            Color crOutline = Globals.gui.GetArrow();

            for (int iY = 0; iY < BOARD_SIZE; iY++)
            {
                for (int iX = 0; iX < BOARD_SIZE; iX++)
                {
                    left = (int)(dbWidth * iX);
                    top = (int)(dbHeight * iY);

                    if (Globals.Game.GetGHorizontal(iX, iY) == Engine.EnumGreaterDir.GDIR_Left)
                    {
                        pt[0].x = left - iSize / 2 - 1;
                        pt[0].y = top + (int)dbHeight / 2 - iSize;
                        pt[1].x = pt[0].x + iSize;
                        pt[1].y = pt[0].y + iSize;
                        pt[2].x = pt[0].x - 1;//correction 
                        pt[2].y = pt[0].y + 2 * iSize + 1;

                        myLine = new Line();
                        myLine.StrokeThickness = 2;
                        myLine.Stroke = Globals.brush.GetBrush(crOutline);

                        myLine.X1 = pt[0].x;
                        myLine.X2 = pt[1].x;
                        myLine.Y1 = pt[0].y;
                        myLine.Y2 = pt[1].y;

                        gridOverlay.Children.Add(myLine);

                        myLine = new Line();
                        myLine.StrokeThickness = 2;
                        myLine.Stroke = Globals.brush.GetBrush(crOutline);

                        myLine.X1 = pt[2].x;
                        myLine.X2 = pt[1].x;
                        myLine.Y1 = pt[2].y;
                        myLine.Y2 = pt[1].y;

                        gridOverlay.Children.Add(myLine);
                    }
                    else if (Globals.Game.GetGHorizontal(iX, iY) == Engine.EnumGreaterDir.GDIR_Right)
                    {
                       pt[0].x = left - 1 + iSize / 2;
                       pt[0].y = top + (int)dbHeight / 2 - iSize;
                       pt[1].x = pt[0].x - iSize;
                       pt[1].y = pt[0].y + iSize;
                       pt[2].x = pt[0].x + 1;
                       pt[2].y = pt[0].y + 2 * iSize + 1;

                       myLine = new Line();
                       myLine.StrokeThickness = 2;
                       myLine.Stroke = Globals.brush.GetBrush(crOutline);

                       myLine.X1 = pt[0].x;
                       myLine.X2 = pt[1].x;
                       myLine.Y1 = pt[0].y;
                       myLine.Y2 = pt[1].y;

                       gridOverlay.Children.Add(myLine);

                       myLine = new Line();
                       myLine.StrokeThickness = 2;
                       myLine.Stroke = Globals.brush.GetBrush(crOutline);

                       myLine.X1 = pt[2].x;
                       myLine.X2 = pt[1].x;
                       myLine.Y1 = pt[2].y;
                       myLine.Y2 = pt[1].y;

                       gridOverlay.Children.Add(myLine);
                    }
                    
                    
                    if (Globals.Game.GetGVertical(iX, iY) == Engine.EnumGreaterDir.GDIR_Down)
                    {
                        pt[0].x = left + (int)dbWidth / 2 - iSize;
                        pt[0].y = top + iSize / 2;
                        pt[1].x = pt[0].x + iSize;
                        pt[1].y = pt[0].y - iSize;
                        pt[2].x = pt[0].x + 2 * iSize + 1;
                        pt[2].y = pt[0].y + 1;

                        myLine = new Line();
                        myLine.StrokeThickness = 2;
                        myLine.Stroke = Globals.brush.GetBrush(crOutline);

                        myLine.X1 = pt[0].x;
                        myLine.X2 = pt[1].x;
                        myLine.Y1 = pt[0].y;
                        myLine.Y2 = pt[1].y;

                        Grid.SetZIndex(myLine, 0);
                        gridOverlay.Children.Add(myLine);
                        
                        myLine = new Line();
                        myLine.StrokeThickness = 2;
                        myLine.Stroke = Globals.brush.GetBrush(crOutline);

                        myLine.X1 = pt[2].x;
                        myLine.X2 = pt[1].x;
                        myLine.Y1 = pt[2].y;
                        myLine.Y2 = pt[1].y;

                        Grid.SetZIndex(myLine, 0);
                        gridOverlay.Children.Add(myLine);
                    }
                    else if (Globals.Game.GetGVertical(iX, iY) == Engine.EnumGreaterDir.GDIR_Up)
                    {
                         pt[0].x = left + (int)dbWidth / 2 - iSize;
                         pt[0].y = top - iSize / 2;
                         pt[1].x = pt[0].x + iSize;
                         pt[1].y = pt[0].y + iSize;
                         pt[2].x = pt[0].x + 2 * iSize + 1;
                         pt[2].y = pt[0].y - 1;

                         myLine = new Line();
                         myLine.StrokeThickness = 2;
                         myLine.Stroke = Globals.brush.GetBrush(crOutline);

                         myLine.X1 = pt[0].x;
                         myLine.X2 = pt[1].x;
                         myLine.Y1 = pt[0].y;
                         myLine.Y2 = pt[1].y;

                         gridOverlay.Children.Add(myLine);

                         myLine = new Line();
                         myLine.StrokeThickness = 2;
                         myLine.Stroke = Globals.brush.GetBrush(crOutline);

                         myLine.X1 = pt[2].x;
                         myLine.X2 = pt[1].x;
                         myLine.Y1 = pt[2].y;
                         myLine.Y2 = pt[1].y;

                         gridOverlay.Children.Add(myLine);
                    }
                }
            }
        }

        public override void UpdateOverlay(ref Grid gridBoard, ref Grid gridOverlay)
        {
         //   WriteableBitmap outline = (WriteableBitmap)imgBoard.Source;//new WriteableBitmap((int)imgBoard.Width, (int)imgBoard.Height);
            DrawSymbols(ref gridOverlay, ref gridBoard);
        }
    }
}
