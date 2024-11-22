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
    public enum EnumOption
    {
        OPTION_UseRegionalSettings,
        OPTION_PlaySounds,
        OPTION_RecordTape,
        OPTION_AdvancedMemory,
        OPTION_CalculatorMode,
        OPTION_Notation,
        OPTION_ConversionBuffer,
        OPTION_RPNStackSize,
        OPTION_SkinColor,
        OPTION_SkinStyle,
        OPTION_FixedDigits,
        OPTION_GraphAccuracy,
        OPTION_GraphUsePoints,
        OPTION_EndPeriodPayments, //TVM
        OPTION_PaymentSchedule, // Mortgage
        OPTION_Size,			//size on PC
        OPTION_Keyboard,
        OPTION_KeyboardOffset, //gotta figure out how to label this bad boy
        OPTION_KeyboardLetters, //do we draw the letters on the keyboard?
        OPTION_Language,
        OPTION_FullScreen,
    };

    /// <summary>
    /// Interaction logic for Settings.xaml
    /// </summary>
    public partial class Settings : UserControl, IScreenControl
    {
        public Settings()
        {
            InitializeComponent();

            // skins
            optSkin.txtTitle = Globals.rm.GetString("IDS_OPT_Skin");
            optSkin.AddItem(Globals.rm.GetString("IDS_OPT_Rosewood"));
            optSkin.AddItem(Globals.rm.GetString("IDS_OPT_Alder"));
            optSkin.AddItem(Globals.rm.GetString("IDS_OPT_Rebel"));
            optSkin.AddItem(Globals.rm.GetString("IDS_OPT_Fire"));//, "/Assets/Options/Skin4.png");
            optSkin.AddItem(Globals.rm.GetString("IDS_OPT_SonicBlue"));//, "/Assets/Options/Skin5.png");
            optSkin.AddItem(Globals.rm.GetString("IDS_OPT_Carbon"));//, "/Assets/Options/Skin6.png");
            optSkin.AddItem("Lava");//, "/Assets/Options/Skin7.png");
            optSkin.AddItem(Globals.rm.GetString("IDS_OPT_PinkBubbles"));//, "/Assets/Options/Skin8.png");
            optSkin.AddItem("Leopard");//, "/Assets/Options/Skin9.png");
            optSkin.AddItem("Zebra");//, "/Assets/Options/Skin10.png");


            // optSkin.AddItem("Windows", "/Assets/Options/Skin11.png");
            optSkin.SetCurrentIndex((int)Globals.Settings.eSkin);
            optSkin.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.eSkin = (EnumSkin)iSelIndex;
            };

            // Fret Indicators
            optFretIndicators.txtTitle = Globals.rm.GetString("IDS_OPT_FretIndicators");
            optFretIndicators.AddItem(Globals.rm.GetString("IDS_OPT_RomanNumerals"));
            optFretIndicators.AddItem(Globals.rm.GetString("IDS_OPT_Numbers"));
            optFretIndicators.SetCurrentIndex((int)Globals.Settings.eFretIndicators);
            optFretIndicators.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.eFretIndicators = (EnumFretIndicators)iSelIndex;
            };

            // Left Handed
            optLeftHanded.txtTitle = Globals.rm.GetString("IDS_OPT_LeftHanded");
            optLeftHanded.AddItem(Globals.rm.GetString("IDS_OPT_Off"));
            optLeftHanded.AddItem(Globals.rm.GetString("IDS_OPT_On"));
            optLeftHanded.SetCurrentIndex(Globals.Settings.bLeftHanded ? 1 : 0);
            optLeftHanded.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.bLeftHanded = (iSelIndex > 0) ? true : false;
            };

            // instrument
            optInstrument.txtTitle = Globals.rm.GetString("IDS_OPT_Instrument");
            optInstrument.AddItem(Globals.rm.GetString("IDS_OPT_Guitar"));
            optInstrument.AddItem(Globals.rm.GetString("IDS_OPT_BassGuitar"));
            optInstrument.AddItem(Globals.rm.GetString("IDS_OPT_Banjo"));
            optInstrument.AddItem(Globals.rm.GetString("IDS_OPT_Lute"));
            optInstrument.AddItem(Globals.rm.GetString("IDS_OPT_Ukulele") + " (soprano)");
            optInstrument.AddItem(Globals.rm.GetString("IDS_OPT_Ukulele") + " (concert)");
            optInstrument.SetCurrentIndex((int)Globals.Settings.eInstrument);
            optInstrument.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.eInstrument = (EnumInstrument)iSelIndex;
                UpdateCustom();
            };


            // sound type
            optSound.txtTitle = Globals.rm.GetString("IDS_OPT_SoundType");
            optSound.AddItem(Globals.rm.GetString("IDS_OPT_Accoustic"));
            optSound.AddItem(Globals.rm.GetString("IDS_OPT_Overdrive"));

            optSound.SetCurrentIndex((int)Globals.Settings.eSound);
            optSound.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.eSound = (EnumInstrumentSound)iSelIndex;
            };

            optNotation.txtTitle = Globals.rm.GetString("IDS_OPT_Notation");
            optNotation.AddItem(Globals.rm.GetString("IDS_OPT_Sharp"));
            optNotation.AddItem(Globals.rm.GetString("IDS_OPT_Flat"));
            optNotation.SetCurrentIndex(Globals.Settings.bShowFlats ? 1 : 0);
            optNotation.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.bShowFlats = (iSelIndex > 0) ? true : false;
            };

          //  optCustomTuning.txtTitle = Globals.rm.GetString("IDS_OPT_CustomTuning");
          //  string szTemp = Globals.myGen.GetCustomTuningText(Globals.myGen.m_eInst);
          //  optCustomTuning.AddItem(szTemp);
           /* optCustomTuning.MouseUpUpdate += delegate()
            {
                NavigationService.Navigate(new Uri("/Screens/CustomTuning.xaml", UriKind.Relative));
            }; */



            //sensitivity
            optTuningSensitivity.txtTitle = Globals.rm.GetString("IDS_OPT_Sensitivity");
            optTuningSensitivity.AddItem(Globals.rm.GetString("IDS_OPT_Normal"));
            optTuningSensitivity.AddItem(Globals.rm.GetString("IDS_TUN_Low"));
            optTuningSensitivity.AddItem(Globals.rm.GetString("IDS_TUN_Full"));
            optTuningSensitivity.SetCurrentIndex((int)Globals.Settings.eSensitivity);
            optTuningSensitivity.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.eSensitivity = (TunerSensitivity)iSelIndex;
            };


            //range
            optTuningRange.txtTitle = Globals.rm.GetString("IDS_OPT_TuningRange");
            optTuningRange.AddItem(Globals.rm.GetString("IDS_OPT_Normal") + " (50Hz-800Hz)");
            optTuningRange.AddItem(Globals.rm.GetString("IDS_OPT_Guitar") + " (65Hz-400Hz)");
            optTuningRange.AddItem(Globals.rm.GetString("IDS_OPT_BassGuitar") + " (30Hz-200Hz)");
            optTuningRange.AddItem(Globals.rm.GetString("IDS_TUN_Full") + " (50Hz-10kHz)");
            optTuningRange.SetCurrentIndex((int)Globals.Settings.eTunerRage);
            optTuningRange.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.eTunerRage = (FrequencyRange)iSelIndex;
            };
        }

        private void UpdateCustom()
        {
            //update the custom label
      //      optCustomTuning.ClearItems();
      //      string szTemp = Globals.myGen.GetCustomTuningText(Globals.myGen.m_eInst);
      //      optCustomTuning.AddItem(szTemp);
      //      optCustomTuning.txtSubItem.Text = szTemp;
        }

        public void Refresh()
        {

        }

        public void UnloadScreen()
        {
            //save settings here
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            //first things first ... load the background
            ImageBrush brush = new ImageBrush();
            brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/OptionsBG.jpg"));
            LayoutRoot.Background = brush;
        }


    }
}
