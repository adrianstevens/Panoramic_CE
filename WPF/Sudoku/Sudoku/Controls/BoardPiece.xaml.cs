using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using Sudoku.Engine;

namespace Sudoku
{
    public enum EnumSelect
    {
        None,
        Selected,
        Highlighted,
        Count,
    };

    public partial class BoardPiece : UserControl
    {
        enum EnumDraw
        {
            Guess,
            Given,
            PencilMarks,
            Kakuro,
            Blank,
            None,
        };
        private const int SIZE_CELLARRAY = 9;

        Grid gridPencilMarks = new Grid();

        public int iX;
        public int iY;

        Image imgFont = null; //ref to the current font

        int iGuess = 0;
        double dbFontWidth = 0;
        double dbGridSize = 0;

        EnumSelect eSelectState = EnumSelect.Count;
        EnumSquares eCurrentSq = EnumSquares.None;
        
        public EventHandler<BoardTileEventArgs> SelectionChanged;

        public BoardPiece(int x, int y)
        {
            iX = x;
            iY = y;

            InitializeComponent();

            layoutTotals.Opacity = 0;

            imgBackground.Opacity = 0;
            imgBackground2.Opacity = 0;
            imgFont1.Opacity = 0;
            imgFont2.Opacity = 0;
            imgFont = imgFont1;

            
            //and don't forget the grid .. 2 rows 3 columns
            gridPencilMarks.Margin = new Thickness(4,0,4,6);

            for (int i = 0; i < 3; i++)
            {
                if (i != 0)
                {
                    RowDefinition r = new RowDefinition();
                    gridPencilMarks.RowDefinitions.Add(r);
                }
                ColumnDefinition c = new ColumnDefinition();
                gridPencilMarks.ColumnDefinitions.Add(c);
            }

            for (int c = 0; c < 6; c++)
            {
                TextBlock t = new TextBlock();
                // t.Text = c.ToString(); //to test em
                Grid.SetColumn(t, c % 3);
                Grid.SetRow(t, c / 3);

                //t.Foreground = Globals.brush.GetBrush(Globals.gui.sSkin.crPencil1);
                t.Foreground = Globals.brush.GetBrush(Globals.gui.GetPencilMark(GetBackground(), Globals.Game.GetGameType()));
                t.TextAlignment = TextAlignment.Center;

                gridPencilMarks.Children.Add(t);


                if (Globals.Game.GetGameType() == Engine.EnumSudokuType.STYPE_Kakuro)
                {
                    switch (Globals.Game.GetBoardSize())
                    {
                        case Engine.EnumSize.KSIZE_5:
                            t.FontSize = 24;
                            break;
                        case Engine.EnumSize.KSIZE_7:
                            t.FontSize = 19;
                            break;
                        case Engine.EnumSize.KSIZE_9:
                            t.FontSize = 16;
                            break;
                    }
                }
                else
                {
                    t.FontSize = 20;
                }
            }

            LayoutRoot.Children.Add(gridPencilMarks);
        }

        public void SetGridSize(double dbGrid)
        {
            dbGridSize = dbGrid;

            RectangleGeometry myGeo = new RectangleGeometry();
            myGeo.Rect = new Rect(0, 0, dbGrid, dbGrid);

            canvasBackground.Clip = myGeo;


        }


        public void UpdateGuess(int iValue, int iTotal)
        {
            UpdateGuess(iValue);
        }

        public void UpdateGuess(int iValue)
        {
            iGuess = iValue;

            if (imgFont1 != null)
            {
                imgFont1.SetValue(Canvas.LeftProperty, ((iValue - 1) * dbFontWidth) * -1d);
                imgFont2.SetValue(Canvas.LeftProperty, ((iValue - 1) * dbFontWidth) * -1d);

                if (iValue < 1)
                {
                    ShowPencilText();
                }
                else
                {
                    ShowGuessText();
                }
            }
        }

        public void UpdateGroupTotal(int iValue, bool bShow)
        {
            if (bShow == true)
            {
                layoutTotals.Opacity = 1.0;

                textTopLeft.Text = iValue.ToString();

                textTopLeft.Foreground = Globals.brush.GetBrush(Globals.gui.sSkin.crPencilKakuro);
            }
            else
            {
                layoutTotals.Opacity = 0;
            }
        }

        public void UpdateVTotal(int iValue)
        {
            if (iValue < 1)
            {
                textBottomLeft.Text="";
                return;
            }

            layoutTotals.Opacity = 1.0;

            textBottomLeft.Text = iValue.ToString();

            textBottomLeft.Foreground = Globals.brush.GetBrush(Colors.Black);

            switch (Globals.Game.GetBoardSize())
            {
                case Engine.EnumSize.KSIZE_5:
                    textBottomLeft.FontSize = 27;
                    break;
                case Engine.EnumSize.KSIZE_7:
                    textBottomLeft.FontSize = 22;
                    break;
                case Engine.EnumSize.KSIZE_9:
                default:
                    textBottomLeft.FontSize = 20;
                    break;
            }
        }

        public void UpdateHTotal(int iValue)
        {
            if (iValue < 1)
            {
                textTopRight.Text = "";
                return;
            }

            layoutTotals.Opacity = 1.0;

            textTopRight.Text = iValue.ToString();

            textTopRight.Foreground = Globals.brush.GetBrush(Colors.Black);

            switch(Globals.Game.GetBoardSize())
            {
                case Engine.EnumSize.KSIZE_5:
                    textTopRight.FontSize = 27;
                    break;
                case Engine.EnumSize.KSIZE_7:
                    textTopRight.FontSize = 22;
                    break;
                case Engine.EnumSize.KSIZE_9:
                default:
                    textTopRight.FontSize = 20;
                    break;
            }
            
        }

        public void UpdateOperator(Engine.EnumOperator eOp, bool bShow)
        {
            if (bShow == true)
            {
                layoutTotals.Opacity = 1.0;

                switch (eOp)
                {
                    case Engine.EnumOperator.OP_Divide:
                        textBottomLeft.Text = "÷";
                        break;
                    case Engine.EnumOperator.OP_Minus:
                        textBottomLeft.Text = "-";
                        break;
                    case Engine.EnumOperator.OP_Plus:
                        textBottomLeft.Text = "+";
                        break;
                    case Engine.EnumOperator.OP_Times:
                        textBottomLeft.Text = "×";
                        break;
                    case Engine.EnumOperator.OP_None:
                    default:
                        textBottomLeft.Text = "";
                        break;
                }

                textBottomLeft.Foreground = Globals.brush.GetBrush(Globals.gui.sSkin.crPencilKakuro);
            }
            else
            {
                layoutTotals.Opacity = 0.0;
            }
        }

        void ShowGuessText()
        {
            gridPencilMarks.Opacity = 0;

            if(imgFont != null)
                imgFont.Opacity = 1;
            gridPencilMarks.Opacity = 0;

            if (Globals.Game.GetShowErrors() == true &&
                Globals.Game.GetGuess(iX, iY) > 0 &&
                Globals.Game.GetGuess(iX, iY) != Globals.Game.GetSolution(iX, iY))
                txtError.Opacity = 1;
            else
                txtError.Opacity = 0;

        }

        void ShowPencilText()
        {
            gridPencilMarks.Opacity = 1;

            txtError.Opacity = 0;
            if(imgFont != null)
                imgFont.Opacity = 0;
        }

        public void UpdatePencilMarks(int iMarks)
        {
            ShowPencilText();

            //now figure out the pencil marks ...
            TextBlock textCur = null;
            bool bDone = false;
            int iTemp, iTemp2;
            int i = 0;
            int j = 0;

            for (i = 0; i < 7; i++) //6 for the numbers...one for the extra indicator (...)
            {
                if ((bDone == true || iMarks == 0) && i < 6)
                {
                   // textCur = (TextBlock)gridPencilMarks.Children.ElementAt(i);
                    textCur = (TextBlock)gridPencilMarks.Children[i];
                    textCur.Text = "";
                }

                //j = j is intentional ... c# is bitchy 
                for (j = j; j < 9; j++)
                {
                    iTemp = (int)(Math.Pow(2, j));
                    iTemp2 = iMarks & iTemp;
                    if (iTemp2 == iTemp)
                    {
                        if (i < 6)
                        {
                            textCur = (TextBlock)gridPencilMarks.Children[i];
                            textCur.Text = (j + 1).ToString();
                        }
                        else
                        {
                            //don't need to get the child again ...
                            textCur.Text = "...";
                        }
                        j++;
                        bDone = true;
                        break;
                    }
                }
            }
        }

        public void UpdateSelector(EnumSelect eSelect)
        {
            eSelectState = eSelect;

            EnumSquares eSquare = GetBackground();

            if(eSquare != eCurrentSq)
                UpdateBackground(eSquare);
        }

       //call this so it'll look up the background type itself
        public void UpdateBackground()
        {
            EnumSquares eSquare = GetBackground();

            if (eSquare != eCurrentSq)
                UpdateBackground(eSquare);
        }

        EnumSquares GetBackground()
        {
            EnumSquares eSquareOrig;
            EnumSquares eSquare = EnumSquares.None;
            EnumDraw eDraw = EnumDraw.None;

            if (Globals.Game.IsLocked(iX, iY))
            {
                eSquare = EnumSquares.Locked;
                eDraw = EnumDraw.Given;
            }
            else if (Globals.Game.IsGiven(iX, iY))
            {
                eSquare = EnumSquares.Given;
                eDraw = EnumDraw.Given;
            }
            else if (Globals.Game.GetGuess(iX, iY) != 0)
            {
                if (Globals.Game.GetGameType() == Engine.EnumSudokuType.STYPE_Kakuro)
                    eSquare = EnumSquares.Locked;
                else if (IsAltColor(iX, iY))
                    eSquare = EnumSquares.Guess2;
                else
                    eSquare = EnumSquares.Guess1;
                eDraw = EnumDraw.Guess;
            }
            else if (Globals.Game.IsBlank(iX, iY))
            {
                eDraw = EnumDraw.Blank;
            }
            else if (Globals.Game.IsKakuro(iX, iY))
            {
                eSquare = EnumSquares.Kakuro;
                eDraw = EnumDraw.Kakuro;
            }
            else // pencil marks
            {
                if (Globals.Game.GetGameType() == Engine.EnumSudokuType.STYPE_Kakuro)
                    eSquare = EnumSquares.Locked;
                else if (IsAltColor(iX, iY))
                    eSquare = EnumSquares.Board2;
                else
                    eSquare = EnumSquares.Board1;
                eDraw = EnumDraw.PencilMarks;
            }

            //draw some selector action here too ... notepad ...
            if (Globals.Settings.iSelectorX == iX && Globals.Settings.iSelectorY == iY)
            {
                eSquare = EnumSquares.Selector;
                eDraw = EnumDraw.Guess;
            }
            else if (DrawHighlight(iX, iY) == true)
            {
                eSquare = EnumSquares.Highlighted;
                eDraw = EnumDraw.Guess;
            }
            //to correct for alternate games 
            eSquareOrig = eSquare;
            eSquare = Globals.gui.GetSquareEnum(ref eSquare, Globals.Game.GetGameType(), (iX == Globals.Settings.iSelectorX) ? true : false);

            switch (eDraw)
            {
                case EnumDraw.Guess:
                    break;
                case EnumDraw.Blank:
                    if (Globals.Settings.bDrawBlanks == true)
                    {
                        if (Globals.Settings.eSkin == EnumSudokuSkins.Notepad)
                            eSquare = EnumSquares.Kakuro;//(EnumSquares.Board1);
                        else
                            eSquare = (EnumSquares.Given);
                    }
                    else if (Globals.Settings.eSkin == EnumSudokuSkins.Notepad)
                    {
                        eSquare = EnumSquares.None;
                    }
                    break;
                case EnumDraw.None:
                default:
                 //   eSquare = (EnumSquares.None);
                    break;
            }
                      
            return eSquare;

        }

        //this function is basically just used to intialize the background
        public void UpdateItem() 
        {
            EnumSquares eSquare = GetBackground();

            if (Globals.gui.UseFont1(eSquare))
            {
                if (imgFont != null)
                    imgFont1.Opacity = imgFont.Opacity;
                imgFont = imgFont1;
                imgFont2.Opacity = 0;
            }
            else
            {
                if (imgFont != null)
                    imgFont2.Opacity = imgFont.Opacity;
                imgFont = imgFont2;
                imgFont1.Opacity = 0;
            }

            UpdateBackground(eSquare);
            eCurrentSq = eSquare;

        }

        private void UpdateBackground(EnumSquares eSquare)
        {
            int iTemp = 0;
            if (eSquare != EnumSquares.None)
            {
                iTemp = (int)eSquare;
            }
            else
            {
                imgBackground.Opacity = 0;
                imgBackground2.Opacity = 0;
                return;
            }

            if (iTemp >= SIZE_CELLARRAY)
            {
                imgBackground.Opacity = 0;
                imgBackground2.Opacity = 1.0;
                iTemp -= SIZE_CELLARRAY;
                imgBackground2.SetValue(Canvas.LeftProperty, iTemp * dbGridSize * -1d);
            }
            else
            {
                imgBackground2.Opacity = 0;
                imgBackground.Opacity = 1.0;
                imgBackground.SetValue(Canvas.LeftProperty, iTemp * dbGridSize * -1d);
            }


            //no I am not happy about this ....
            if (Globals.Settings.eSkin == EnumSudokuSkins.Notepad &&
                Globals.Game.GetGameType() == Engine.EnumSudokuType.STYPE_Sudoku &&
                Globals.Game.IsGiven(iX, iY) == true)
            {
                if (imgFont != null)
                    imgFont2.Opacity = imgFont.Opacity;
                imgFont = imgFont2;
                imgFont1.Opacity = 0;
            }
            else if (Globals.gui.UseFont1(eSquare))
            {
                if(imgFont != null)
                    imgFont1.Opacity = imgFont.Opacity;
                imgFont = imgFont1;
                imgFont2.Opacity = 0;
            }
            else
            {
                if (imgFont != null)
                    imgFont2.Opacity = imgFont.Opacity;
                imgFont = imgFont2;
                imgFont1.Opacity = 0;
            }
            
            eCurrentSq = eSquare;

        }

        private bool IsAltColor(int iX, int iY)
        {
            if(Globals.Game.GetGameType() == Engine.EnumSudokuType.STYPE_Kakuro)
                return false;

            if((iX/3 == 1) ^ (iY/3 == 1))
                return true;

            return false;
        }

        private bool DrawHighlight(int iX, int iY)
        {
            if(Globals.Game.GetGameType() == Engine.EnumSudokuType.STYPE_Sudoku && Globals.Settings.bShowHighlights)
            {
                if(Globals.Settings.iSelectorX == iX)
                    return true;
                if(Globals.Settings.iSelectorY == iY)
                    return true;
            }

            else if (Globals.Game.GetGameType() == Engine.EnumSudokuType.STYPE_Killer || Globals.Game.GetGameType() == Engine.EnumSudokuType.STYPE_KenKen)
            {
                if (Globals.Settings.eSkin == EnumSudokuSkins.Notepad)
                    return false;

                if (Globals.Game.GetSamuIndex(iX, iY) == Globals.Game.GetSamuIndex(Globals.Settings.iSelectorX, Globals.Settings.iSelectorY))
                {
                    if (Globals.Settings.eSkin == EnumSudokuSkins.Grey || Globals.Settings.eSkin == EnumSudokuSkins.Glass)
                        return true;
                    return true;
                }
            }

            return false;
        }

        private void LayoutRoot_MouseLeftButtonUp(object sender, MouseEventArgs e)
        {
            if (SelectionChanged != null)
            {
                SelectionChanged(sender, new BoardTileEventArgs
                {
                    x = iX,
                    y = iY
                });
            }
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            RectangleGeometry myGeo = new RectangleGeometry();
            myGeo.Rect = new Rect(0, 0, Globals.gui.sSkin.imgFont1.PixelWidth/9, Globals.gui.sSkin.imgFont1.PixelHeight);

            imgFont1.Width = Globals.gui.sSkin.imgFont1.PixelWidth;
            imgFont2.Width = Globals.gui.sSkin.imgFont2.PixelWidth;

            imgFont1.Height = Globals.gui.sSkin.imgFont1.PixelHeight;
            imgFont2.Height = Globals.gui.sSkin.imgFont2.PixelHeight;

            canvasFont.Clip = myGeo;
            dbFontWidth = myGeo.Rect.Right;

            canvasFont.Width = dbFontWidth;
            canvasFont.Height = Globals.gui.sSkin.imgFont1.PixelHeight;

            //HARD CODED CENTERING HACK
            int iIndent = (int)((48- imgFont1.Width/9)/2);

            canvasFont.Margin = new Thickness(iIndent, 0, 0, 0);

           /* imgFont1.Margin = new Thickness(iIndent, 0, 0, 0);
            imgFont2.Margin = new Thickness(iIndent, 0, 0, 0);*/

            if (iGuess != 0)
            {
                imgFont1.SetValue(Canvas.LeftProperty, ((iGuess - 1) * dbFontWidth) * -1d);
                imgFont2.SetValue(Canvas.LeftProperty, ((iGuess - 1) * dbFontWidth) * -1d);
            }
        }

        private void UserControl_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
       //     Globals.Game.OnSetPencilMark(iX, iY, 0);
            Globals.Game.ClearPencilMarks(iX, iY);

            UpdatePencilMarks(0);
        }
    }

  /*  public class BoardTileEventArgs : EventArgs
    {
        public int x { get; set; }
        public int y { get; set; }
    }*/
}
