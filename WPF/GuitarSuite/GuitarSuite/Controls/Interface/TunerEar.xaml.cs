using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace GuitarSuite.Controls
{
    /// <summary>
    /// Interaction logic for TunerEar.xaml
    /// </summary>
    public partial class TunerEar : UserControl, IScreenControl
    {
        GuitarString[] myStrings = new GuitarString[6];
        GuitarFinger[] myFingers = new GuitarFinger[6];

        public TunerEar()
        {
            InitializeComponent();
        }

        public void Refresh()
        {

        }

        public void UnloadScreen()
        {
            for (int i = 0; i < 6; i++)
            {
                if (myFingers[i] != null)
                    myFingers[i].StopAll();
            }
        }

        private void Layout_Root_Loaded(object sender, RoutedEventArgs e)
        {
            Layout_Root.Background = Globals.gui.GetBackground();

            if (Globals.Settings.bLeftHanded)
            {
                ScaleTransform scale = new ScaleTransform();
                scale.CenterX = 240;
                scale.ScaleX = -1;
                nut.RenderTransform = scale;
            }

            UpdateStrings();
            UpdateNeck();


        }

        void UpdateStrings()
        {
            LayoutStrings.Children.Clear();

            for (int x = 0; x < Globals.myGen.GetNumStrings(); x++)
            {
                int iString = Globals.GetString(x);

                //this will need to be improved for bass/left/etc
                myStrings[x] = new GuitarString(iString);
                //myStrings[x].Width = 50;

                Grid.SetColumn(myStrings[x], x);
                Grid.SetRow(myStrings[x], 0);

                LayoutStrings.Children.Add(myStrings[x]);

                //and don't foget the set the string resource
                int iTemp = Globals.myGen.GetNoteRes(iString, 0);

                if (iTemp > -1)
                {
                    myStrings[x].szResource = Globals.GetWave(iTemp);
                    myStrings[x].SetInUse(true);
                }
                else
                {
                    myStrings[x].szResource = "";
                    myStrings[x].SetInUse(false);
                }
            }
        }

        void UpdateNeck()
        {
            layoutNeck.Children.Clear();

            int iString;

            for (int x = 0; x < Globals.myGen.GetNumStrings(); x++)
            {
                iString = Globals.GetString(x);

                {   //draw the finger somewhere on the neck please

                    //get the relative fret ...
                    int iFret = Globals.myGen.GetFret(x) - Globals.myGen.GetStartingFret();

                    myFingers[x] = new GuitarFinger(EnumFinger.blue);


                    //not worried about blank strings because of the loop restraints
                    myFingers[x].SetMainText(Globals.myGen.GetOpenNoteString(iString));
                    myFingers[x].bEnableTapHold = true;
                    myFingers[x].bEnableOpacity = true;
                    myFingers[x].Opacity = 0.5;

                    Grid.SetColumn(myFingers[x], x);
                    Grid.SetRow(myFingers[x], 2);

                    //and set the resource
                    int iTemp = Globals.myGen.GetNoteRes(iString, 0);

                    if (iTemp > -1)
                    {
                        layoutNeck.Children.Add(myFingers[x]);
                        myFingers[x].szResource = Globals.GetWave(iTemp);
                    }
                }
            }
        }
    }
}
