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
using System.Windows.Media.Imaging;

namespace GuitarSuite
{
    public class Globals
    {
        public static string szPublisherID = "3bd167a5-d7db-df11-a844-00237de2db9e";

        public static MyGenerator myGen = new MyGenerator();

        public static ResourceManager rm = new ResourceManager("GuitarSuite.Strings.StringTable", Assembly.GetExecutingAssembly());

        public static AppSettings Settings = new AppSettings();

        public static ObjGui gui = new ObjGui();

        //public static TrialManager Trial = new TrialManager();


        public static string GetWave(int iIndex)
        {
            if (Globals.Settings.eSound == EnumInstrumentSound.Overdrive)
            {
                return "./Assets/Overdrive/" + waves[iIndex];
            }
            else //default  
            {
                return "./Assets/Waves/" + waves[iIndex];
            }
        }

        public static ImageBrush GetNutBrush()
        {
            ImageBrush brush = new ImageBrush();

            brush.ImageSource = new BitmapImage(new Uri(@"/Assets/whitebar.png", UriKind.Relative));

            if (Settings.bLeftHanded)
            {
                ScaleTransform scale = new ScaleTransform();
                scale.ScaleX = -1.0;
                scale.CenterX = 0.5;
                brush.RelativeTransform = scale;
            }
            return brush;
        }

        public static int GetString(int s)
        {
            if (Settings.bLeftHanded == true)
                return Globals.myGen.GetNumStrings() - s - 1;
            return s;
        }
        
        public static string[] waves = {"c-vlow.wav", 
                                        "cs-vlow.wav", 
                                        "d-vlow.wav", 
                                        "ds-vlow.wav", 
                                        "e-vlow.wav", 
                                        "f-vlow.wav", 
                                        "fs-vlow.wav", 
                                        "g-vlow.wav", 
                                        "gs-vlow.wav", 

                                        "a-low.wav", 
                                        "as-low.wav", 
                                        "b-low.wav", 
                                        "c-low.wav", 
                                        "cs-low.wav", 
                                        "d-low.wav", 
                                        "ds-low.wav", 
                                        "e-low.wav", 
                                        "f-low.wav", 
                                        "fs-low.wav", 
                                        "g-low.wav", 
                                        "gs-low.wav", 

                                        "a-mid.wav", 
                                        "as-mid.wav", 
                                        "b-mid.wav", 
                                        "c-mid.wav", 
                                        "cs-mid.wav", 
                                        "d-mid.wav", 
                                        "ds-mid.wav", 
                                        "e-mid.wav", 
                                        "f-mid.wav", 
                                        "fs-mid.wav", 
                                        "g-mid.wav", 
                                        "gs-mid.wav",

                                        "a-hi.wav", 
                                        "as-hi.wav", 
                                        "b-hi.wav", 
                                        "c-hi.wav", 
                                        "cs-hi.wav", 
                                        "d-hi.wav", 
                                        "ds-hi.wav", 
                                        "e-hi.wav", 
                                        "f-hi.wav", 
                                        "fs-hi.wav", 
                                        "g-hi.wav", 
                                        "gs-hi.wav",

                                        "a-vhi.wav", 
                                        "as-vhi.wav", 
                                        "b-vhi.wav", 
                                        "c-vhi.wav", 
                                        "cs-vhi.wav", 
                                        "d-vhi.wav", 
                                        "ds-vhi.wav", 
                                        "e-vhi.wav", 
                                        "f-vhi.wav", 
                                        "fs-vhi.wav", 
                                        "g-vhi.wav", 
                                        "gs-vhi.wav",
                                   };


    }
}
