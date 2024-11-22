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
    public class ScreenKenKen : ScreenKillerSudoku
    {
        public ScreenKenKen()
        {
        }

        public override void SetPiece(ref BoardPiece b, int x, int y)
        {
            b.UpdateOperator(Globals.Game.GetKenKenOp(x, y), Globals.Game.DrawTotal(x, y));

            base.SetPiece(ref b, x, y);
        }
    }
}
