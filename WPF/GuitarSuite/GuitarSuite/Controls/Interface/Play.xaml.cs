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
    /// Interaction logic for Play.xaml
    /// </summary>
    /// 


    public partial class Play : UserControl, IScreenControl
    {
        ReverseChord oRevChord = new ReverseChord();//chord lookup engine ...
        //SoundEffectInstance sfxSilence;

        GuitarString[] myStrings = new GuitarString[6];

        GuitarFinger upArrow = null;
        GuitarFinger downArrow = null;

        int iBottomFret = 0;

        int[] iNotes = new int[6];//so I can keep track of the note values

        const int NUM_FRETS = 5;//number of frets we're going to be using for chord formation 

        bool bLock
        {
            get
            {
                return _bLock;
            }
            set
            {
                _bLock = value;
            }
        }
        bool _bLock = true;

        public Play()
        {
            InitializeComponent();

            UpdateFingers();

            for (int i = 0; i < 6; i++)
                iNotes[i] = -1;


            upArrow = new GuitarFinger(EnumFinger.grey);
            downArrow = new GuitarFinger(EnumFinger.grey);

            upArrow.SetMainText("↑");
            upArrow.LayoutRoot.MouseUp += OnNeckUp;

            downArrow.SetMainText("↓");
            downArrow.LayoutRoot.MouseUp += OnNeckDown;

            Grid.SetColumn(upArrow, 0);
            Grid.SetRow(upArrow, 0);

            Grid.SetColumn(downArrow, 0);
            Grid.SetRow(downArrow, 3);

            layoutControls.Children.Add(upArrow);
            layoutControls.Children.Add(downArrow);
        }

        public void Refresh()
        {

        }

        public void UnloadScreen()
        {
        }

        void UpdateFingers()
        {
            layoutNeck.Children.Clear();

            int iTemp;
            // int iNote;

            for (int s = 0; s < Globals.myGen.GetNumStrings(); s++)
            {
                iTemp = Globals.GetString(s);

                GuitarFinger finger = null;

                for (int f = 0; f < NUM_FRETS; f++)
                {
                    finger = new GuitarFinger(EnumFinger.blank);

                    Grid.SetColumn(finger, s);
                    Grid.SetRow(finger, f);
                    finger.bEnableOpacity = true;
                    finger.Opacity = 0;
                    //finger.LayoutRoot.MouseEnter += FingerMouseEnter;
                    finger.LayoutRoot.MouseLeftButtonUp += FingerMouseEnter;
                    finger.SetMainText(Globals.myGen.GetNote(iTemp, f + 1 + iBottomFret));

                    layoutNeck.Children.Add(finger);
                }
            }

        }

        //LButton Up for PC
        private void FingerMouseEnter(object sender, RoutedEventArgs e)
        {
            //need to figure out which string ....
            Grid grid = (Grid)sender;
            UserControl uiControl = (UserControl)grid.Parent;
            GuitarFinger finger = (GuitarFinger)uiControl;

            int iColumn = Grid.GetColumn(uiControl);
            int iRow = Grid.GetRow(uiControl);

            int iString = Globals.GetString(iColumn);

            bool bStringActive = true;

            Image myImage = null;

            switch (iColumn)
            {
                case 0: myImage = imageNo1; break;
                case 1: myImage = imageNo2; break;
                case 2: myImage = imageNo3; break;
                case 3: myImage = imageNo4; break;
                case 4: myImage = imageNo5; break;
                case 5: myImage = imageNo6; break;
            }

            if (myImage.Opacity == 1)
                bStringActive = false;

            if (bLock)//always locked ... leave in place incase multitouch gets better ...
            {
                if (bStringActive == false)
                {
                    finger.bEnableOpacity = true;
                    finger.Opacity = 0;//leave these here as they override internal opacity settings
                }
                //resetting back to open string
                else if (finger.bEnableOpacity == false)
                {
                    finger.bEnableOpacity = true;
                    finger.Opacity = 1.0;//leave these here as they override internal opacity settings
                    finger.StartFadeTimer();

                    myStrings[iColumn].szResource = Globals.GetWave(Globals.myGen.GetNoteRes(iString, 0));
                    iNotes[iColumn] = Globals.myGen.GetNoteValue(iString, 0);
                }
                //set the finger
                else
                {
                    finger.SetFingerStyle(EnumFinger.blue, false);
                    finger.bEnableOpacity = false;
                    finger.Opacity = 1.0;//leave these here as they override internal opacity settings

                    myStrings[iColumn].szResource = Globals.GetWave(Globals.myGen.GetNoteRes(iString, iRow + 1 + iBottomFret));

                    iNotes[iColumn] = Globals.myGen.GetNoteValue(iString, iRow + 1 + iBottomFret);
                }

            }
            else
            {   //not currently used
                finger.SetFingerStyle(EnumFinger.blank, false);
                finger.bEnableOpacity = true;
                finger.Opacity = 1.0;
            }

            ClearOtherNotes(iColumn, iRow);

            CalcChord();
        }

        void CalcChord()
        {
            oRevChord.Reset();

            for (int i = 0; i < Globals.myGen.GetNumStrings(); i++)
            {
                oRevChord.AddNote(iNotes[i]);
            }

            string szTemp = null;
            oRevChord.GetChord(ref szTemp);
            textChord.Text = szTemp;
        }

        //not used
        void Update()
        {
            UpdateNut();
            UpdateFingers();
            UpdateStrings();
            UpdateNeck();

            UpdateFretMarkers();
        }

        void UpdateNut()
        {
            LayoutNut.Children.Clear();

            //add the nut
            if (iBottomFret == 0)
            {
                nut.Opacity = 1;
            }
            else
            {
                nut.Opacity = 0;
                return;
            }
        }

        void UpdateFretMarkers()
        {
            //we'll do the up/down arrows here too
            if (iBottomFret == 0) //-1)
            {
                upArrow.Opacity = 0;
                upArrow.IsHitTestVisible = false;
            }
            else
            {
                upArrow.Opacity = 1;
                upArrow.IsHitTestVisible = true;
            }

            if (iBottomFret == 5)
            {
                downArrow.Opacity = 0;
                downArrow.IsHitTestVisible = false;
            }
            else
            {
                downArrow.Opacity = 1;
                downArrow.IsHitTestVisible = true;
            }


            layoutMarkers.Children.Clear();

            int iTemp = iBottomFret;// Globals.myGen.GetStartingFret() - 1;

            for (int y = 0; y < 5; y++)
            {
                if (Globals.Settings.eFretIndicators == EnumFretIndicators.Numbered)
                {
                    GuitarFinger finger = new GuitarFinger(EnumFinger.grey);
                    finger.SetMainText((iTemp + y + 1).ToString());

                    Grid.SetColumn(finger, 0);
                    Grid.SetRow(finger, y);

                    layoutMarkers.Children.Add(finger);
                }
                else
                {
                    if (iTemp + y >= 17)
                        break;

                    if (iTemp + y < 0)
                        continue;

                    if (Globals.myGen.m_szFrets[y + iTemp].Length < 1)
                        continue;

                    GuitarFinger finger = new GuitarFinger(EnumFinger.grey);
                    finger.SetMainText(Globals.myGen.m_szFrets[y + iTemp]);

                    Grid.SetColumn(finger, 0);
                    Grid.SetRow(finger, y);

                    layoutMarkers.Children.Add(finger);

                }
            }
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
                iNotes[x] = Globals.myGen.GetNoteValue(iString, 0);

                if (iTemp > -1)
                {
                    myStrings[x].SetInUse(true);
                    myStrings[x].szResource = Globals.GetWave(iTemp);
                }
                else
                {
                    myStrings[x].SetInUse(false);
                    myStrings[x].szResource = "";
                }
            }
        }

        void UpdateNeck()
        {

        }

        private void Layout_Root_Loaded(object sender, RoutedEventArgs e)
        {
            //first things first ... load the background
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
            UpdateFretMarkers();

            CalcChord();
        }

        void ClearOtherNotes(int iString, int iFret)
        {
            if (iString >= Globals.myGen.GetNumStrings())
                return;

            for (int i = 0; i < NUM_FRETS; i++)
            {
                if (i == iFret)
                    continue;

                int iTemp = i + iString * NUM_FRETS;//5 being the number of frets ... 

                GuitarFinger finger = (GuitarFinger)layoutNeck.Children[iTemp]; //layoutNeck.Children.ElementAt(iTemp);
                

                int iRow = Grid.GetRow(finger);
                int iColumn = Grid.GetColumn(finger);

                finger.bEnableOpacity = true;
                finger.Opacity = 0;
            }
        }

        void ClearAll()
        {
            for (int i = 0; i < layoutNeck.Children.Count; i++)
            {
                GuitarFinger finger = (GuitarFinger)layoutNeck.Children[i]; //layoutNeck.Children.ElementAt(i);
                finger.bEnableOpacity = true;
                finger.Opacity = 0;
            }

            int iString;

            //and set all of the string resources to open
            for (int x = 0; x < Globals.myGen.GetNumStrings(); x++)
            {
                iString = Globals.GetString(x);

                int iTemp = Globals.myGen.GetNoteRes(iString, 0);

                if (iTemp > -1)
                    myStrings[x].szResource = Globals.GetWave(iTemp);
                else
                    myStrings[x].szResource = "";

                iNotes[x] = Globals.myGen.GetNoteValue(iString, 0);
            }

            //Clear the chord text
            textChord.Text = "";
        }

        //all 6 use the same function ... good enough
        private void imageNo1_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Image img = (Image)sender;
            int iColumn = Grid.GetColumn(img);
            int iRow = Grid.GetRow(img);

            int iString = Globals.GetString(iColumn);

            if (iColumn >= Globals.myGen.GetNumStrings())
                return;

            if (img.Opacity == 0)
            {
                ClearOtherNotes(iColumn, -1);//so we clear the whole string
                img.Opacity = 1;
                myStrings[iColumn].Opacity = 0.5;
                myStrings[iColumn].szResource = "";
                iNotes[iColumn] = -1;
            }
            else
            {
                img.Opacity = 0;
                myStrings[iColumn].Opacity = 1.0;
                iNotes[iColumn] = Globals.myGen.GetNoteValue(iString, 0);

                int iTemp = Globals.myGen.GetNoteRes(iString, 0);

                if (iTemp > -1)
                    myStrings[iColumn].szResource = Globals.GetWave(iTemp);
            }

            CalcChord();
        }

        private void OnNeckUp(object sender, RoutedEventArgs e)
        {
            if (iBottomFret > 0) //-1)
            {
                iBottomFret--;
                ClearAll();
                Update();
            }
        }

        private void OnNeckDown(object sender, RoutedEventArgs e)
        {
            if (iBottomFret < 12 - 7)
            {
                iBottomFret++;
                ClearAll();
                Update();
            }
        }

    }
}
