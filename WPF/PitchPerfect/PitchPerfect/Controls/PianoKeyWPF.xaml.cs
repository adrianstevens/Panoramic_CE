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
using System.Windows.Resources;
using System.Media;

namespace PitchPerfect.Controls
{
    /// <summary>
    /// Interaction logic for PianoKeyWPF.xaml
    /// </summary>
    public partial class PianoKeyWPF : UserControl
    {
        private MediaPlayer mPlayer = new MediaPlayer();
        //private MediaElement mElement= new MediaElement();

        private PianoControlWPF.KeyType keyType = PianoControlWPF.KeyType.White;

        private bool on = false;

        private LinearGradientBrush whiteKeyOnBrush;
        private LinearGradientBrush blackKeyOnBrush;

        private SolidColorBrush whiteKeyOffBrush = new SolidColorBrush(Colors.White);

        private int noteID = 60;

        public PianoKeyWPF(PianoControlWPF.KeyType keyType)
        {
            this.keyType = keyType;
            whiteKeyOnBrush = new LinearGradientBrush();
            whiteKeyOnBrush.GradientStops.Add(new GradientStop(Colors.White, 0.0));
            whiteKeyOnBrush.GradientStops.Add(new GradientStop(Color.FromArgb(0xFF, 0x20, 0x20, 0x20), 1.0));

            blackKeyOnBrush = new LinearGradientBrush();
            blackKeyOnBrush.GradientStops.Add(new GradientStop(Colors.LightGray, 0.0));
            blackKeyOnBrush.GradientStops.Add(new GradientStop(Colors.Black, 1.0));

            InitializeComponent();

           // LayoutRoot.Children.Add(mPlayer);

            
        }

        public void PressPianoKey()
        {
            brdInner.Dispatcher.Invoke(
            System.Windows.Threading.DispatcherPriority.Normal,
            new Action(
                delegate()
                {
                    if (keyType == PianoControlWPF.KeyType.White)
                        brdInner.Background = whiteKeyOnBrush;
                    else
                        brdInner.Background = blackKeyOnBrush;
                }
            ));

            PlayNote();
            on = true;
        }

        public void ReleasePianoKey()
        {
            brdInner.Dispatcher.Invoke(
            System.Windows.Threading.DispatcherPriority.Normal,
            new Action(
                delegate()
                {
                    brdInner.Background = whiteKeyOffBrush;
                }
            ));

            on = false;
        }

        public int NoteID
        {
            get
            {
                return noteID;
            }
            set
            {
                #region Require

            /*    if (value < 0 || value > ShortMessage.DataMaxValue)
                {
                    throw new ArgumentOutOfRangeException("NoteID", noteID,
                        "Note ID out of range.");
                }*/

                #endregion

                noteID = value;

                LoadWave();
            }
        }

        public bool IsPianoKeyPressed
        {
            get
            {
                return on;
            }
        }

        public Color NoteOnColor
        {
            set
            {
                Brush brush = null;
                if (keyType == PianoControlWPF.KeyType.White)
                    brush = whiteKeyOnBrush;
                else
                    brush = blackKeyOnBrush;
                brdInner.Background = brush;
            }
        }

        public Color NoteOffColor
        {
            get
            {
                return whiteKeyOffBrush.Color;
            }
            set
            {
                whiteKeyOffBrush.Color = value;
                brdInner.Background = whiteKeyOffBrush;
            }
        }

        public PianoControlWPF.KeyType KeyType { get; set; }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            LayoutRoot.MouseLeftButtonDown += LayoutRoot_MouseDown;
            LayoutRoot.MouseEnter += LayoutRoot_MouseDown;
            LayoutRoot.MouseLeave += LayoutRoot_MouseUp;


            //LayoutRoot.MouseLeftButtonDown += LayoutRoot_MouseDown;
            //LayoutRoot.MouseLeftButtonUp += LayoutRoot_MouseUp;
        }

        private void LayoutRoot_MouseDown(object sender, RoutedEventArgs e)
        {
            MouseEventArgs m = (MouseEventArgs)e;

            if (m.LeftButton == MouseButtonState.Pressed)
            {
                PressPianoKey();
            }
        }

        private void LayoutRoot_MouseUp(object sender, RoutedEventArgs e)
        {
            ReleasePianoKey();
        }

        private void LoadWave()
        {
            string szResource = GetNoteRes();

            if (szResource == null || szResource.Length < 1)
                return;

            Uri uriFile = new Uri(szResource, UriKind.Relative);
            mPlayer.Open(uriFile);

        }

        private void PlayNote()
        {
            

            //Uri uriSound = new Uri(@szResource, UriKind.Relative);

           // Uri uriSound = new Uri("pack://application:,,,/Assets/Piano/c-mid.wav");

            

            /*StreamResourceInfo sri = App.GetResourceStream(uriSound);

            if (sri == null)
                return;*/

           // mElement.LoadedBehavior = MediaState.Manual;
           // mElement.Source = uriSound;
           // mElement.Play();

            if (mPlayer.HasAudio == false)
            {
                LoadWave();
                mPlayer.Volume = 100;
            }
            
            mPlayer.Stop();

            if(mPlayer.HasAudio == true)
                mPlayer.Play();

            /*
            var Player = new SoundPlayer(sri.Stream);

            Player.Play();*/
        }

        //from the ID
        private string GetNoteRes()
        {
            int iTemp = noteID;

            while (iTemp < 24)
                iTemp++;

            while (iTemp > 72)
                iTemp--;

            return Globals.GetWave(iTemp - 24);
        }
    }
}
