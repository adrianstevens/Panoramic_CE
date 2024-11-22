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
using GuitarSuite.Controls;
//using com.intel.adp;

namespace GuitarSuite
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        IScreenControl curScreen = null;

        public MainWindow()
        {
            InitializeComponent();

#if !DEBUG
            CheckIntel();
#endif
        }

        private void Grid_Loaded(object sender, RoutedEventArgs e)
        {
        //    Chords chord = new Chords();
        //    LayoutRoot.Children.Add(chord);

        //    TunerEar tEar = new TunerEar();
        //    LayoutRoot.Children.Add(tEar);

       //     Scales scale = new Scales();
       //     LayoutRoot.Children.Add(scale);

            Globals.myGen.Init();

            //hookup the delegate
            mySideBar.SetScreen = SetSection;
            mySideMenu.ScreenUpdate = UpdateSection;

            SetSection(Globals.Settings.eCurrentScreen);
            mySideBar.eSelected = Globals.Settings.eCurrentScreen;
            
        }

        public void UpdateSection()
        {
            curScreen.Refresh();
        }
                
        public void SetSection(EnumScreens eScreen)
        {
            if(curScreen != null)
                curScreen.UnloadScreen();

            MainControl.Children.Clear();
            
            switch (eScreen)
            {
                case EnumScreens.GuitarChords:
                    Chords chord = new Chords();
                    curScreen = (IScreenControl)chord;
                    MainControl.Children.Add(chord);
                    mySideMenu.SetTitle(Globals.rm.GetString("IDS_MENU_Chords"));
                    break;
                case EnumScreens.GuitarTuner:
                    Tuner tuner = new Tuner();
                    curScreen = (IScreenControl)tuner;
                    MainControl.Children.Add(tuner);
                    mySideMenu.SetTitle(Globals.rm.GetString("IDS_MENU_Digital"));
                    break;
                case EnumScreens.Metronome:
                    MainControl.Children.Clear();
                    Metronome met = new Metronome();
                    curScreen = (IScreenControl)met;
                    MainControl.Children.Add(met);
                    mySideMenu.SetTitle(Globals.rm.GetString("IDS_MENU_Metronome"));
                    break;
                case EnumScreens.Play:
                    MainControl.Children.Clear();
                    Play play = new Play();
                    curScreen = (IScreenControl)play;
                    MainControl.Children.Add(play);
                    mySideMenu.SetTitle(Globals.rm.GetString("IDS_MENU_Play"));
                    break;
                case EnumScreens.Scales:
                    Scales scale = new Scales();
                    MainControl.Children.Add(scale);
                    mySideMenu.SetTitle(Globals.rm.GetString("IDS_MENU_PlayMode"));
                    break;
                case EnumScreens.Settings:
                    Settings set = new Settings();
                    curScreen = (IScreenControl)set;
                    MainControl.Children.Add(set);
                    mySideMenu.SetTitle(Globals.rm.GetString("IDS_MENU_Options"));
                    break;
                case EnumScreens.TunerEar:
                    TunerEar tEar = new TunerEar();
                    curScreen = (IScreenControl)tEar;
                    MainControl.Children.Add(tEar);
                    mySideMenu.SetTitle(Globals.rm.GetString("IDS_MENU_TuneByEar"));
                    break;

            }

            mySideMenu.SetSideMenu(eScreen);
        }

        private void btnChords_Click(object sender, RoutedEventArgs e)
        {
            MainControl.Children.Clear();
            Chords chord = new Chords();
            MainControl.Children.Add(chord);
        }

        private void btnScales_Click(object sender, RoutedEventArgs e)
        {
            MainControl.Children.Clear();
            Scales scale = new Scales();
            MainControl.Children.Add(scale);
        }

        private void btnPlay_Click(object sender, RoutedEventArgs e)
        {
            MainControl.Children.Clear();
            Play play = new Play();
            MainControl.Children.Add(play);
        }

        private void btnTuner_Click(object sender, RoutedEventArgs e)
        {
            MainControl.Children.Clear();
        }

        private void btnTunerEar_Click(object sender, RoutedEventArgs e)
        {
            MainControl.Children.Clear();
            TunerEar tEar = new TunerEar();
            MainControl.Children.Add(tEar);
        }

        private void btnMetronome_Click(object sender, RoutedEventArgs e)
        {
            MainControl.Children.Clear();
            Metronome met = new Metronome();
            MainControl.Children.Add(met);
        }

        void CheckIntel()
        {
     /*       try
            {
                //0x30328AB7,0xAA754CD6,0xAD1125E8,0x0F58377D
                AdpApplication app = new AdpApplication(new AdpApplicationId(0x04F2F29A, 0xE96A43F6, 0x9A678524, 0x7D6C9402));
                /// Note: replace "0x11111111, 0x11111111, 0x11111111, 0x11111111" with the actual application ID
                /// once you obtain it from Intel AppUp(SM) Developer Program web portal
            }
            catch (AdpException e)
            {
                if (e is AdpErrorException)
                {
                    // TO DO: add your logic to handle the errors during initialization
                    MessageBox.Show("Error - Guitar Suite is not authenticated.  You may need to connect to the Intel AppUp store.", e.Message);
                    System.Environment.Exit(1);
                }
                else if (e is AdpWarningException)
                {
                    // TO DO: add your logic to handle the warnings
                    MessageBox.Show("Error - Guitar Suite is not authenticated.  You may need to connect to the Intel AppUp store.", e.Message);
                }
            }*/
        }
    }
}
