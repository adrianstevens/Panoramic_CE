using System;
using System.IO;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Media.Imaging;
using System.Windows.Resources;
using System.Media;

namespace GuitarSuite
{
    public enum EnumFinger
    {
        blue,
        red,
        dark,
        grey,
        blank,//used for the scales screen ... just set the opacity to 0
        blankRect,//for the nuts
        count,

    };

    public class GuitarFinger : UserControl
    {
        private MediaPlayer mPlayer;// = new MediaPlayer();

        public EnumFinger eFingerType
        {
            get;
            set;
        }

        int iTapTime = 0;
        bool bPlayOnLoad = false;

        public string szResource
        {
            get
            {
                return _szResource;
            }
            set
            {
                _szResource = value;
                Uri uriSound = new Uri(_szResource, UriKind.Relative);
                mPlayer.Volume = 0;
                mPlayer.Open(uriSound);
                bPlayOnLoad = false;
            }
        }
        string _szResource;

        void mPlayer_MediaOpened(object sender, EventArgs e)
        {
            if (bPlayOnLoad == true)
            {
                bPlayOnLoad = false;
                Play(false);
            }
        }
        

        public bool bEnableTapHold
        {
            get;
            set;
        }

        bool bPlayTapHold = false;

        public bool bEnableOpacity
        {
            get { return _bEnableOpacity; }
            set
            {
                _bEnableOpacity = value;
                if (_bEnableOpacity)
                    this.Opacity = 0.6;
                else
                {
                    StopFadeTimer();
                    this.Opacity = 1.0;
                }

                if (eFingerType == EnumFinger.blank || eFingerType == EnumFinger.blankRect)
                    this.Opacity = 0.0;

            }
        }
        bool _bEnableOpacity;

        System.Windows.Threading.DispatcherTimer myDispatcherTimer = null;
        System.Windows.Threading.DispatcherTimer myTapHoldTimer = null;
        System.Windows.Threading.DispatcherTimer myRepeatTimer = null;

        public Grid LayoutRoot;
        private TextBlock MainText;

        public void SetFingerStyle(EnumFinger eFinger, bool bForce)
        {
            //save a little work ...
            if (eFingerType == eFinger && bForce == false)
                return;

            ImageBrush brush = new ImageBrush();
            BitmapImage bitmap;

            switch (eFinger)
            {
                case EnumFinger.blank:
                    bitmap = new BitmapImage(new Uri("pack://application:,,/Assets/Note_Dark.png"));
                    this.Opacity = 0;
                    LayoutRoot.Width = 67;
                    LayoutRoot.Height = 67;
                    break;
                case EnumFinger.dark:
                    bitmap = new BitmapImage(new Uri("pack://application:,,/Assets/Note_Dark.png"));
                    LayoutRoot.Width = 67;
                    LayoutRoot.Height = 67;
                    break;
                case EnumFinger.blankRect:
                case EnumFinger.grey:
                    bitmap = new BitmapImage(new Uri("pack://application:,,/Assets/Note_Grey.png"));
                    MainText.Foreground = new SolidColorBrush(Colors.Black);
                    LayoutRoot.Width = 40;
                    LayoutRoot.Height = 36;
                    break;
                case EnumFinger.red:
                    bitmap = new BitmapImage(new Uri("pack://application:,,/Assets/Note_rd.png"));
                    LayoutRoot.Width = 67;
                    LayoutRoot.Height = 67;
                    break;
                case EnumFinger.blue:
                default:
                    bitmap = new BitmapImage(new Uri("pack://application:,,/Assets/Note.png"));
                    LayoutRoot.Width = 67;
                    LayoutRoot.Height = 67;
                    break;
            }

                        
            brush.ImageSource = bitmap;
            brush.Stretch = Stretch.Uniform;
            

            LayoutRoot.Background = brush;
        }

        public GuitarFinger(EnumFinger eFinger)
        {
            mPlayer = new MediaPlayer();
            mPlayer.MediaOpened += new EventHandler(mPlayer_MediaOpened);

            this.eFingerType = EnumFinger.count;
            bPlayTapHold = false;
            
            LayoutRoot = new Grid();

        //    LayoutRoot.MouseEnter += LayoutRoot_MouseDown;
         //   LayoutRoot.MouseLeave += LayoutRoot_MouseUp;

            LayoutRoot.MouseDown += LayoutRoot_MouseDown;
            LayoutRoot.MouseUp += LayoutRoot_MouseUp;
        
               
            MainText = new TextBlock();


            MainText.FontSize = 26;
            MainText.VerticalAlignment = VerticalAlignment.Center;
            MainText.HorizontalAlignment = HorizontalAlignment.Center;
            MainText.Foreground = new SolidColorBrush(Colors.White);
            MainText.Margin = new Thickness(-1, -2, 0, 0);

            SetFingerStyle(eFinger, true);

            this.eFingerType = eFinger;

            LayoutRoot.Children.Add(MainText);

            this.Content = LayoutRoot;
        }

        

        public bool SetMainText(string szMain)
        {
            MainText.Text = szMain;
            return true;
        }

        private void LayoutRoot_MouseDown(object sender, RoutedEventArgs e)
        {
            iTapTime = System.Environment.TickCount;

            StopFadeTimer();

            if (bPlayTapHold)
                SetTapHold(false);

            if (bEnableOpacity)
            {
                StartFadeTimer();
            }

            if (myTapHoldTimer != null)
            {
                myTapHoldTimer.Stop();
                myTapHoldTimer = null;
            }

            if (bEnableTapHold)
            {
                myTapHoldTimer = new System.Windows.Threading.DispatcherTimer();
                myTapHoldTimer.Interval = new TimeSpan(0, 0, 0, 0, 500); // 100 Milliseconds 
                myTapHoldTimer.Tick += new EventHandler(TapNHold);
                myTapHoldTimer.Start();
            }

            //and set the opacity to full blast
            this.Opacity = 1.0;
        }

        public void Each_Tick(object o, EventArgs sender)
        {
            if (this.Opacity > 0.6 ||
                (this.Opacity > 0 && (eFingerType == EnumFinger.blank || eFingerType == EnumFinger.blankRect) ))
                this.Opacity -= 0.05;
            else
            {
                StopFadeTimer();
            }
        }

        public void TapNHold(object o, EventArgs sender)
        {
            SetTapHold(true);

            myTapHoldTimer.Stop();
            myTapHoldTimer = null;

        }

        public void RepeatNote(object o, EventArgs sender)
        {
            StartFadeTimer();

            this.Opacity = 1.0;

            Play(false);
        }

        private void LayoutRoot_MouseUp(object sender, RoutedEventArgs e)
        {
            Play(false);

            if (myTapHoldTimer != null)
            {
                myTapHoldTimer.Stop();
                myTapHoldTimer = null;
            }
        }

        void KillTimer()
        {

        }

        void FadeAnimation()
        {

        }

        public void Play(bool bAnimate)
        {
            if (szResource == null || szResource.Length < 1)
                return;

            mPlayer.Stop();
            if (mPlayer.HasAudio == true)
            {
                mPlayer.Volume = 1f;
                mPlayer.Play();
            }
            else
            {
                bPlayOnLoad = true;
            }

            /*

            StreamResourceInfo sri = App.GetResourceStream(uriSound);

            if (sri == null)
                return;

            var Player = new SoundPlayer(sri.Stream);

            Player.Play();*/
            

            if (bAnimate)
            {
                this.Opacity = 1.0;

                StartFadeTimer();

            }
        }

        void SetTapHold(bool bEnable)
        {
            StopTapNHoldTimer();
            
            if (bEnable)
            {
                SetFingerStyle(EnumFinger.red, true);

                StartTapNHoldTimer();

                bPlayTapHold = true;
            }
            else
            {   
                SetFingerStyle(eFingerType, true);
                bPlayTapHold = false;
            }
        }

        void StopTapNHoldTimer()
        {
            if (myRepeatTimer != null)
            {
                myRepeatTimer.Stop();
                myRepeatTimer = null;
            }
        }

        void StartTapNHoldTimer()
        {
            StopTapNHoldTimer();

            myRepeatTimer = new System.Windows.Threading.DispatcherTimer();
            myRepeatTimer.Interval = new TimeSpan(0, 0, 0, 0, 1500); // 100 Milliseconds 
            myRepeatTimer.Tick += new EventHandler(RepeatNote);
            myRepeatTimer.Start();
        }

        void StopFadeTimer()
        {
            if (myDispatcherTimer != null)
            {
                myDispatcherTimer.Stop();
                myDispatcherTimer = null;
            }
        }

        //bit dirty but I can live with it ...
        public void StartFadeTimer()
        {
            StopFadeTimer();

            if (bEnableOpacity == false)
                return;

            //you may need to force opacity levels before calling this
            myDispatcherTimer = new System.Windows.Threading.DispatcherTimer();
            myDispatcherTimer.Interval = new TimeSpan(0, 0, 0, 0, 100); // 100 Milliseconds 
            myDispatcherTimer.Tick += new EventHandler(Each_Tick);
            myDispatcherTimer.Start();
        }

        public void StopAll()
        {
            StopFadeTimer();
            StopTapNHoldTimer();
            mPlayer.Stop();
        }
    }
}
