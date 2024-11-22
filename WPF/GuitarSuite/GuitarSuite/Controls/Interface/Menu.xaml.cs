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
using NAudio.Wave;

namespace GuitarSuite.Controls
{
    /// <summary>
    /// Interaction logic for Menu.xaml
    /// </summary>
    public partial class SideMenu : UserControl
    {
        public delegate void UpdateScreen();

        public UpdateScreen ScreenUpdate = null;//called from UpdateAll so I can check for null

        public SideMenu()
        {
            InitializeComponent();
        }

        public void SetTitle(string titleText)
        {
            txtTitle.Text = titleText;
        }

        private void Layout_Root_Loaded(object sender, RoutedEventArgs e)
        {
            ImageBrush brush = new ImageBrush();
            brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Interface/Menu_right.png"));

            Layout_Root.Background = brush;
        }

        public void SetSideMenu(EnumScreens eScreen)
        {
            stackOptions.Children.Clear();

            //tuning
            switch (eScreen)
            {
                case EnumScreens.GuitarChords:
                case EnumScreens.GuitarTuner:
                //case EnumScreens.Metronome:
                case EnumScreens.Play:
                case EnumScreens.Scales:
                //case EnumScreens.Settings:
                case EnumScreens.TunerEar:
                    //start with 
                    SideBarOption optTuning = new SideBarOption();
                    //optTuning.txtTitle = Globals.rm.GetString("REG_Tuning");
                    optTuning.txtTitle = "tuning";

                    //and add the options
                    for (int i = 0; i < Globals.myGen.GetNumTuning(); i++)
                    {
                        optTuning.AddItem(Globals.myGen.GetTuningString(i));
                    }
                    optTuning.SetCurrentIndex(Globals.myGen.GetTuning());
                    optTuning.IndexUpdate += UpdateTuning;
                    stackOptions.Children.Add(optTuning);
                    break;
            }

            //root
            switch (eScreen)
            {
                case EnumScreens.GuitarChords:
                case EnumScreens.Scales:
                    //start with 
                    SideBarOption optRoot = new SideBarOption();
                    //optTuning.txtTitle = Globals.rm.GetString("REG_Tuning");
                    optRoot.txtTitle = "root";

                    //and add the options
                    for (int i = 0; i < 12; i++)
                    {
                        optRoot.AddItem(Globals.myGen.m_szRoots[i]);
                    }
                    optRoot.SetCurrentIndex((int)Globals.myGen.GetRoot());
                    optRoot.IndexUpdate += UpdateRoot;
                    stackOptions.Children.Add(optRoot);
                    break;
            }

            //chord
            if (eScreen == EnumScreens.GuitarChords)
            {
                SideBarOption optChord = new SideBarOption();
                //optTuning.txtTitle = Globals.rm.GetString("REG_Tuning");
                optChord.txtTitle = "chord type";

                //and add the options
                for (int i = 0; i < (int)EnumChordType.CHORD_Count; i++)
                {
                    optChord.AddItem(Globals.myGen.m_szChords[i]);
                }
                optChord.SetCurrentIndex((int)Globals.myGen.GetChord());
                optChord.IndexUpdate += UpdateChord;
                stackOptions.Children.Add(optChord);
            }

            if (eScreen == EnumScreens.Scales)
            {
                SideBarOption optScale = new SideBarOption();
                //optTuning.txtTitle = Globals.rm.GetString("REG_Tuning");
                optScale.txtTitle = "scale";

                //and add the options
                for (int i = 0; i < Globals.myGen.GetNumScales(); i++)
                {
                    optScale.AddItem(Globals.myGen.GetScaleString(i));
                }

                optScale.AddItem(Globals.rm.GetString("IDS_OPT_ScalesOff"));
                optScale.SetCurrentIndex((int)Globals.myGen.GetScale());
                optScale.IndexUpdate += UpdateScale;
                stackOptions.Children.Add(optScale);

            }

            //variation
            if (eScreen == EnumScreens.GuitarChords)
            {
                SideBarOption optVar = new SideBarOption();
                optVar.txtTitle = "variation";

                for (int i = 0; i < 12; i++)
                {
                    optVar.AddItem((i+1).ToString());
                }
                
                optVar.SetCurrentIndex(Globals.myGen.GetIndex());
                optVar.IndexUpdate += UpdateVariation;
                stackOptions.Children.Add(optVar);
            }
            
            //mic input ... we'll set on the tuning screen
         /*   if (eScreen == EnumScreens.GuitarTuner)
            {
                //poll the inputs 
                SideBarOption optInput = new SideBarOption();
                optInput.txtTitle = "input";

                int waveInDevices = WaveIn.DeviceCount;

                for (int waveInDevice = 0; waveInDevice < waveInDevices; waveInDevice++)
                {
                    WaveInCapabilities deviceInfo = WaveIn.GetCapabilities(waveInDevice);
                    optInput.AddItem(deviceInfo.ProductName);
                }

                optInput.SetCurrentIndex(0);
                optInput.IndexUpdate += UpdateInput;
                stackOptions.Children.Add(optInput);
            }*/
        }

        void UpdateInput(int iID, int iSelIndex)
        {
            //waveIn.DeviceNumber = iSelIndex;
        }

        void UpdateTuning(int iID, int iSelIndex)
        {
            if (iSelIndex == (int)Globals.myGen.GetTuning())
                return;

            Globals.myGen.SetTuning(iSelIndex, true);
            UpdateAll();
        }

        void UpdateChord(int iID, int iSelIndex)
        {
            if (iSelIndex == (int)Globals.myGen.GetChord())
                return;

            Globals.myGen.SetChord((EnumChordType)iSelIndex, true);
            UpdateAll();
        }

        void UpdateScale(int iID, int iSelIndex)
        {
            if (iSelIndex == (int)Globals.myGen.GetScale())
                return;

            Globals.myGen.SetScale((EnumScales)iSelIndex);
            UpdateAll();
        }

        void UpdateRoot(int iID, int iSelIndex)
        {
            if (iSelIndex == (int)Globals.myGen.GetRoot())
                return;

            Globals.myGen.SetRoot((EnumChordRoot)iSelIndex, true);
            UpdateAll();
        }

        void UpdateVariation(int iID, int iSelIndex)
        {
            Globals.myGen.SetChordIndex (iSelIndex);
            UpdateAll();
        }

        //delegate again??
        void UpdateAll()
        {
            if (ScreenUpdate != null)
                ScreenUpdate();

        }

    }
}
