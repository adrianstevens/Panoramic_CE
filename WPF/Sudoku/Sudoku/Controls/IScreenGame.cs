using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;

namespace Sudoku
{
    interface IScreenGame
    {
        void Initialize(ref Grid gridBoard, ref Grid gridOverlay);
        void InputNumber(int iNum);
        void ResetBoard(ref Grid gridBoard, ref Grid gridOverlay);
        void UpdateOverlay(ref Grid gridBoard, ref Grid gridOverlay);
    }
}
