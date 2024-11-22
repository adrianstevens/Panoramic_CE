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
    /// Interaction logic for Chords.xaml
    /// </summary>
    public partial class Chords : UserControl, IScreenControl
    {
        GuitarString[] myStrings = new GuitarString[6];

        public Chords()
        {
            InitializeComponent();
        }

        public void Refresh()
        {
            UpdateAll();//easy
        }

        void UpdateAll()
        {
            UpdateStrings();
            UpdateNeck();
            UpdateNut(); 
            UpdateFretMarkers();
        }

        public void UnloadScreen()
        {
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
                int iTemp = Globals.myGen.GetNoteRes(iString);

                if (iTemp > -1)
                    myStrings[x].szResource = Globals.GetWave(iTemp);
                else
                    myStrings[x].szResource = "";

                if (Globals.myGen.GetFret(iString) == -1)
                    myStrings[x].SetInUse(false);
                else
                    myStrings[x].SetInUse(true);
            }
        }


        void UpdateNut()
        {
            LayoutNut.Children.Clear();

            //add the nut
            if (Globals.myGen.GetStartingFret() == 1)
                nut.Opacity = 1;
            else
                nut.Opacity = 0;

            int iString;

            for (int x = 0; x < Globals.myGen.GetNumStrings(); x++)
            {
                iString = Globals.GetString(x);

                if (Globals.myGen.GetFret(iString) < 1)
                {
                    if (Globals.myGen.GetFret(iString) == -1)
                    {
                        //draw the X at the top
                        Image myImage = new Image();
                        ImageBrush brush = new ImageBrush();
                        brush.ImageSource = new BitmapImage(new Uri(@"/Assets/Note_No.png", UriKind.Relative));
                        brush.Stretch = Stretch.None;
                        myImage.Source = brush.ImageSource;
                        myImage.Stretch = Stretch.Uniform;
                        myImage.Width = 32;
                        myImage.Height = 32;

                        Grid.SetColumn(myImage, x);
                        Grid.SetRow(myImage, 0);

                        LayoutNut.Children.Add(myImage);

                    }
                    else
                    {
                        //draw the open string finger 
                        GuitarFinger finger = new GuitarFinger(EnumFinger.grey);
                        finger.SetMainText(Globals.myGen.GetNote(iString));

                        Grid.SetColumn(finger, x);
                        Grid.SetRow(finger, 0);

                        LayoutNut.Children.Add(finger);

                        //and set the resource
                        int iTemp = Globals.myGen.GetNoteRes(iString);
                        finger.szResource = Globals.GetWave(iTemp);
                    }

                }
            }
        }

        void UpdateFretMarkers()
        {
            layoutMarkers.Children.Clear();

            int iTemp = Globals.myGen.GetStartingFret() - 1;

            for (int y = 0; y < 7; y++)
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

        void UpdateNeck()
        {
            layoutNeck.Children.Clear();

            int iString;

            for (int x = 0; x < Globals.myGen.GetNumStrings(); x++)
            {
                iString = Globals.GetString(x);

                if (Globals.myGen.GetFret(iString) < 1)
                {

                }
                else
                {   //draw the finger somewhere on the next please

                    //get the relative fret ...
                    int iFret = Globals.myGen.GetFret(iString) - Globals.myGen.GetStartingFret();

                    GuitarFinger finger = new GuitarFinger(EnumFinger.blue);
                    finger.SetMainText(Globals.myGen.GetNote(iString));


                    Grid.SetColumn(finger, x);
                    Grid.SetRow(finger, iFret);


                    layoutNeck.Children.Add(finger);

                    //and set the resource
                    int iTemp = Globals.myGen.GetNoteRes(iString);

                    if (iTemp > -1)
                        finger.szResource = Globals.GetWave(iTemp);
                    else
                        finger.szResource = "";
                }
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

            


            UpdateAll();

            //set top menu text
            /*
            textBoxChord.Text = Globals.myGen.GetChordString();
            textBoxRoot.Text = Globals.myGen.GetRootString();
            textBoxVar.Text = (Globals.myGen.GetIndex() + 1).ToString() + "/12";
            textBoxTuning.Text = Globals.myGen.GetTuningString();*/

                
        }
    }
}
