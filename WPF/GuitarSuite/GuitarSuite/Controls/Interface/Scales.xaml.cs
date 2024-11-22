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
    public enum EnumScaleDraw
    {
        SDRAW_Note,
        SDRAW_Root,
        SDRAW_None,
    };

    /// <summary>
    /// Interaction logic for Scales.xaml
    /// </summary>
    public partial class Scales : UserControl, IScreenControl
    {
        GuitarString[] myStrings = new GuitarString[6];

        GuitarFinger upArrow = null;
        GuitarFinger downArrow = null;

        int iBottomFret = 0;

        int iNumFrets = 5;

        public Scales()
        {
            InitializeComponent();

            //we'll add the fingers for the up and down controls here
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

                myStrings[x].szResource = "";

                //and don't foget the set the string resource
               /* int iTemp = Globals.myGen.GetNoteRes(iString);

                if (iTemp > -1)
                    myStrings[x].szResource = Globals.GetWave(iTemp);
                else
                    myStrings[x].szResource = "";*/

                if (Globals.myGen.GetFret(iString) == -1)
                    myStrings[x].SetInUse(false);
                else
                    myStrings[x].SetInUse(true);
            }
        }

        void UpdateAll()
        {
            UpdateNeck();
            UpdateNut();
            UpdateStrings();
            UpdateFretMarkers();
        }

        void UpdateFretMarkers()
        {
            layoutMarkers.Children.Clear();

            int iTemp = Globals.myGen.GetStartingFret() - 1;

            for (int y = 0; y < iNumFrets; y++)
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

            int iTemp;
            int iNote;

            for (int s = 0; s < Globals.myGen.GetNumStrings(); s++)
            {
                iTemp = Globals.GetString(s);

                GuitarFinger finger = null;

                switch (GetScaleDrawType(iTemp, iBottomFret, Globals.myGen.GetRoot()))
                {
                    case EnumScaleDraw.SDRAW_None:
                        {
                            finger = new GuitarFinger(EnumFinger.blankRect);
                        }
                        break;
                    case EnumScaleDraw.SDRAW_Note:
                    case EnumScaleDraw.SDRAW_Root:
                        {
                            finger = new GuitarFinger(EnumFinger.grey);
                        }
                        break;
                    default:
                        break;
                }

                finger.SetMainText(Globals.myGen.GetNote(iTemp, iBottomFret));
                Grid.SetColumn(finger, s);
                Grid.SetRow(finger, 0);
                finger.bEnableOpacity = true;

                LayoutNut.Children.Add(finger);

                iNote = Globals.myGen.GetNoteRes(iTemp, iBottomFret);

                if (iTemp > -1)
                    finger.szResource = Globals.GetWave(iNote);
                else
                    finger.szResource = "";
            }
        }

        void UpdateNeck()
        {
            layoutNeck.Children.Clear();

            int iTemp;
            int iNote;

            for (int s = 0; s < Globals.myGen.GetNumStrings(); s++)
            {
                iTemp = Globals.GetString(s);

                GuitarFinger finger = null;

                for (int f = 0; f < iNumFrets; f++)
                {
                    switch (GetScaleDrawType(iTemp, f + iBottomFret + 1, Globals.myGen.GetRoot()))
                    {
                        case EnumScaleDraw.SDRAW_None:
                            {
                                finger = new GuitarFinger(EnumFinger.blank);
                            }
                            break;
                        case EnumScaleDraw.SDRAW_Note:
                            {
                                finger = new GuitarFinger(EnumFinger.dark);
                            }
                            break;
                        case EnumScaleDraw.SDRAW_Root:
                            {
                                finger = new GuitarFinger(EnumFinger.blue);
                            }
                            break;
                        default:
                            break;
                    }
                    finger.SetMainText(Globals.myGen.GetNote(iTemp, f + iBottomFret + 1));
                    Grid.SetColumn(finger, s);
                    Grid.SetRow(finger, f);
                    finger.bEnableOpacity = true;

                    layoutNeck.Children.Add(finger);

                    iNote = Globals.myGen.GetNoteRes(iTemp, f + iBottomFret + 1);

                    if (iTemp > -1)
                        finger.szResource = Globals.GetWave(iNote);
                    else
                        finger.szResource = "";
                }
            }
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

            UpdateAll();
        }

        EnumScaleDraw GetScaleDrawType(int iString, int iFret, EnumChordRoot eRoot)
        {
            //first find the note
            int iNote = Globals.myGen.GetNoteValue(iString, iFret);//the call can correct for any offsets not here
            byte btTemp;

            //we need to correct for the root now ... easiest is to adjust iNote 
            iNote -= (int)eRoot;
            if (iNote < 0)
                iNote += 12;

            //so the engine has the scale in the byte array
            for (int i = 0; i < 16; i++)//16 is the size of the scale ....
            {
                btTemp = Globals.myGen.GetScaleValue(i);
                if (btTemp == iNote && i == 0)
                    return EnumScaleDraw.SDRAW_Root;
                else if (btTemp == iNote)
                    return EnumScaleDraw.SDRAW_Note;
                else if (btTemp == 255)//no note
                    return EnumScaleDraw.SDRAW_None;
                //continue        
            }
            return EnumScaleDraw.SDRAW_None;
        }

        private void OnNeckUp(object sender, RoutedEventArgs e)
        {
            if (iBottomFret > -1)
            {
                iBottomFret--;
                UpdateAll();
            }

        }

        private void OnNeckDown(object sender, RoutedEventArgs e)
        {
            if (iBottomFret < 12 - iNumFrets)
            {
                iBottomFret++;
                UpdateAll();
            }
        }

        



    }
}
