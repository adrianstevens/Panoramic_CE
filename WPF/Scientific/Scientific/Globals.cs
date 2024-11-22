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
using System.Reflection;
using System.Resources;
using System.Collections.Generic;

namespace CalcPro
{
    public class Globals
    {
        public static string szPublisherID = "8e7f8368-3fc7-df11-9eae-00237de2db9e";
        public static int NUM_GRAPHS = 8;
        public static double MARGIN = 2.0;


        //graphing values ... should probably move em into AppSettings
        public static string[] szGraphs = new string[Globals.NUM_GRAPHS];
        public static bool bGraphUsePoints = false;
        public static List<Point> arrGraphPoints = null;
        public static int iGraphIndex = 0; //I'm not proud if this

        public static double dbXmin = 0;
        public static double dbXMax = 0;
        public static double dbYMin = 0;
        public static double dbYMax = 0;
        

        public static ResourceManager rm = new ResourceManager("CalcPro.Strings.StringTable", Assembly.GetExecutingAssembly());

        public static IssCalculator Calc = new IssCalculator();

        public static BrushFactory Brush = new BrushFactory();

        public static CalcProDisplay Display = new CalcProDisplay();

        public static BtnManager btnMan = new BtnManager();

        public static ObjConstants objConstants = new ObjConstants();

        //this should always be called after the other classes so it can set the settings
        //perforance should be ok because the btn manager doesn't create anything until the screens are created (which is after all of this)
        public static AppSettings Settings = new AppSettings();




        //my dirty little window launcher 
        public delegate void PageLauncher(int iWindowType, int iWindowID);//we'll define these parameters properly later ... 0, 0 for normal graph for now

        public static PageLauncher LaunchPage
        {
            get { return _LaunchPage; }
            set { _LaunchPage = value; }
        }
        private static PageLauncher _LaunchPage;
    }

    
}
