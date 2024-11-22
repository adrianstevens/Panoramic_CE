using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Reflection;
using System.Resources;
using Sudoku.Engine;

namespace Sudoku
{
    public enum EnumBackground
    {
        Default,
        Custom,
        PurpleWhite,
        BlackWhite,
        GreenWhite,
        BlueWhite,
        DBlueWhite,
        PinkWhite,
        OrangeWhite,
        GreyBlack,
        RedBlack,
        PurpleBlack,
        BlueBlack,
        GreenBlack,
        DBlueBlue,
        BlueGreen,
        DGReenGreen,
        White,
        Black,
        Grey,
        Green,
        LPink,
        Orange,
        Blue,
        DBlue,
        DGreen,
        Pink,
        Purple,
        Count,
    };


    public class Globals
    {
        public static ResourceManager rm = new ResourceManager("Sudoku.Strings.StringTable", Assembly.GetExecutingAssembly());

        public static SEngine Game = new SEngine();

        public static AppSettings Settings = new AppSettings();

        public static ObjGui gui = new ObjGui();

        public static BrushFactory brush = new BrushFactory();
    }
}
