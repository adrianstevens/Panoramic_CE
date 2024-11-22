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
using System.Windows.Threading;
using System.Diagnostics;
using System.Windows.Media.Animation;
using System.Media;
using System.Windows.Resources;

namespace GuitarSuite.Controls
{
    /// <summary>
    /// Interaction logic for Metronome.xaml
    /// </summary>
    public partial class Metronome : UserControl, IScreenControl
    {
        private enum EnumQuadrant
        {
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight,
            Undefined,
        }

        private enum EnumButtonPress
        {
            BMP_Up,
            BMP_Down,
            Meter_Up,
            Meter_Down,
        }

        private ObjMetronome _metronome;
        private Storyboard _storyGreenBlink;
        private Storyboard _storyRedBlink;
        private int _iNumSeconds;
        private bool _bBtnDown;
        private int _iCurrentIndex = 0;
        private bool _bAniKnob = false;

        private Stopwatch _swBMPMeasure = null;
        private DispatcherTimer _dtKnob = null;
        //private DispatcherTimer _dtBMP = null;
        private DispatcherTimer _dtTapNHold = null;
        private DispatcherTimer _dtTapRepeat = null;
        private DispatcherTimer _dtSecondCounter = null;
        private EnumButtonPress _eBtn;
        private int _iMSPerBeat;
        private long _lLastTimestamp;

        public int iKnobCurrentIndex
        {
            get
            {
                return _iKnobCurrentIndex;
            }
            set
            {
                _iKnobCurrentIndex = value;
            }
        }
        private int _iKnobCurrentIndex = 0;


        private int _iKnobDestIndex;
        private bool _bKnobForward;
        private const int _iKnobCount = 9;
        private EnumQuadrant _eQuadrantStart;
        private EnumQuadrant _eQuadrantForward;
        private EnumQuadrant _eQuadrantBackward;

        private List<SoundPlayer> _arrSounds = null;

        SoundPlayer sfxSilence = null;
                

        public Metronome()
        {
            InitializeComponent();

            //set the tap to set text
            string szBPM = Globals.rm.GetString("IDS_MSG_SetBPM");
            szBPM = szBPM.Replace("\\r\\n", "\r\n");
            txtTapSetBPM.Text = szBPM;

            _metronome = new ObjMetronome();
            _swBMPMeasure = new Stopwatch();
            //_metronome.sequencer.IndexChanged += SequencerIndexChanged;

            _arrSounds = new List<SoundPlayer>();

            this.DataContext = _metronome;

            _storyGreenBlink = new Storyboard();
            Storyboard.SetTarget(_storyGreenBlink, imgGreen);
            Storyboard.SetTargetProperty(_storyGreenBlink, new PropertyPath(Image.OpacityProperty));
            _storyGreenBlink.Children.Add(new DoubleAnimation
            {
                Duration = new Duration(TimeSpan.FromMilliseconds(300)),
                From = Convert.ToDouble(1.0),
                To = Convert.ToDouble(0),
            });

            _storyRedBlink = new Storyboard();
            Storyboard.SetTarget(_storyRedBlink, imgRed);
            Storyboard.SetTargetProperty(_storyRedBlink, new PropertyPath(Image.OpacityProperty));
            _storyRedBlink.Children.Add(new DoubleAnimation
            {
                Duration = new Duration(TimeSpan.FromMilliseconds(300)),
                From = Convert.ToDouble(1.0),
                To = Convert.ToDouble(0),
            });

            //myMediaPlayer.Source = null;
            //myMediaPlayer.Volume = 1.0;
            //myMediaPlayer.Stop();

            imgPlayPause.SetValue(Canvas.LeftProperty, 1 * -16d);

            // create a new source
            //SynthMediaStreamSource source = new SynthMediaStreamSource(44100, 1);
            //source.SampleMaker = _metronome.sequencer;
            //myMediaPlayer.SetSource(source);

            txtTimer.Text = "00:00";

            _dtKnob = new DispatcherTimer();
            _dtKnob.Interval = new TimeSpan(0, 0, 0, 0, 100); // 100 Milliseconds
            _dtKnob.Tick += new EventHandler(dtKnob_Tick);

            /*_dtBMP = new DispatcherTimer();
            _dtBMP.Interval = new TimeSpan(0, 0, 0, 0, 500); // this will be reset
            _dtBMP.Tick += new EventHandler(dtBMP_Tick);*/

            _dtTapNHold = new DispatcherTimer();
            _dtTapNHold.Interval = new TimeSpan(0, 0, 0, 0, 1000);
            _dtTapNHold.Tick += new EventHandler(dtTapNHold_Tick);

            _dtTapRepeat = new DispatcherTimer();
            _dtTapRepeat.Interval = new TimeSpan(0, 0, 0, 0, 50);
            _dtTapRepeat.Tick += new EventHandler(dtTapRepeat_Tick);

            _dtSecondCounter = new DispatcherTimer();
            _dtSecondCounter.Interval = new TimeSpan(0, 0, 1); // 1 second
            _dtSecondCounter.Tick += new EventHandler(SecondCounter);

            CompositionTarget.Rendering += new EventHandler(CompositionTarget_Rendering);

            iKnobCurrentIndex = _metronome.iSoundIndex * 2;

            SetCurrentKnobIndex();

        }

        public void Refresh()
        {

        }

        public void UnloadScreen()
        {
            _dtKnob.Stop();
            //_dtBMP.Stop();
            _swBMPMeasure.Stop();
            _swBMPMeasure.Reset();
            _dtTapNHold.Stop();
            _dtTapRepeat.Stop();
            _dtSecondCounter.Stop();
        }

        #region privatefunctions
        private void btnTempDown_Click(object sender, RoutedEventArgs e)
        {
            if (_metronome.bOn)
                ResetSequencer();

            _metronome.TempoDown();
        }

        private void btnMeterUp_Click(object sender, RoutedEventArgs e)
        {
            if (_metronome.bOn)
                ResetSequencer();

            _metronome.MeterUp();
        }

        private void btnMeterDown_Click(object sender, RoutedEventArgs e)
        {
            if (_metronome.bOn)
                ResetSequencer();

            _metronome.MeterDown();
        }

        private void ResetSequencer()
        {
          //  Dispatcher.BeginInvoke(() =>
            {
                _iCurrentIndex = -1;
                _swBMPMeasure.Stop();
                _swBMPMeasure.Reset();
                _dtSecondCounter.Stop();

                // hide all the ticks
                for (int i = 0; i < gridTicks.Children.Count; i++)
                {
                    Image img = (Image)gridTicks.Children[i];
                    img.Visibility = Visibility.Collapsed;
                }
                txtMeterIndex.Text = "";
                _iNumSeconds = 0;

                //_metronome.bOn = !_metronome.bOn;

                // turn on / off indicator
                if (_metronome.bOn)
                    imgPlayPause.SetValue(Canvas.LeftProperty, 0 * -16d);
                else
                    imgPlayPause.SetValue(Canvas.LeftProperty, 1 * -16d);

                if (_metronome.bOn == false)
                    return;

                // load up our new beat
                //_metronome.ResetSequencer();

                ResetTickIndicators();
                ResetTickSounds(false);

                //myMediaPlayer.Position = TimeSpan.Zero;
                //myMediaPlayer.Play();

                txtTimer.Text = "00:00";


                _dtSecondCounter.Start();

                _iMSPerBeat = 60000 / _metronome.iBPM;
                _swBMPMeasure.Start();
                _lLastTimestamp = _swBMPMeasure.ElapsedMilliseconds;

            }  //);
        }

        void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            if (!_swBMPMeasure.IsRunning)
                return;

            long lNew = _swBMPMeasure.ElapsedMilliseconds;
            if (_iMSPerBeat < lNew - _lLastTimestamp)
            {
                // play a beat
                dtBMP_Tick();
                _lLastTimestamp += _iMSPerBeat;//_swBMPMeasure.ElapsedMilliseconds;
            }
        }

        private void ToggleOnOff()
        {
          //  Dispatcher.BeginInvoke(() =>
            {
                /*if (_metronome.bOn)
                    myMediaPlayer.Pause();*/

                _iCurrentIndex = -1;
                //_dtBMP.Stop();
                _swBMPMeasure.Stop();
                _swBMPMeasure.Reset();
                _dtSecondCounter.Stop();

                // hide all the ticks
                for (int i = 0; i < gridTicks.Children.Count; i++)
                {
                    Image img = (Image)gridTicks.Children[i];
                    img.Visibility = Visibility.Collapsed;
                }
                txtMeterIndex.Text = "";
                _iNumSeconds = 0;

                _metronome.bOn = !_metronome.bOn;

                // turn on / off indicator
                if (_metronome.bOn)
                {
                    imgPlayPause.SetValue(Canvas.LeftProperty, 0 * -16d);
                    txtTapSetBPM.Opacity = 0;
                    txtMeterIndex.Opacity = 1;
                }
                else
                {
                    imgPlayPause.SetValue(Canvas.LeftProperty, 1 * -16d);
                    txtTapSetBPM.Opacity = 1;
                    txtMeterIndex.Opacity = 0;
                }


                if (_metronome.bOn == true)
                {
                    //bugbug
                    /*SoundEffect mySoundEffect = SoundEffect.FromStream(TitleContainer.OpenStream("Assets/Metronome/silence.wav"));

                    sfxSilence = mySoundEffect.CreateInstance();

                    sfxSilence.IsLooped = true;
                    sfxSilence.Play();*/


                }
                else
                {
                    //sfxSilence.Stop();
                    return;
                }

                // load up our new beat
                //_metronome.ResetSequencer();

                ResetTickIndicators();
                ResetTickSounds(false);

                //myMediaPlayer.Position = TimeSpan.Zero;
                //myMediaPlayer.Play();

                txtTimer.Text = "00:00";


                _dtSecondCounter.Start();

                _iMSPerBeat = 60000 / _metronome.iBPM;
                _swBMPMeasure.Start();
                _lLastTimestamp = _swBMPMeasure.ElapsedMilliseconds;
            }//);
        }

        private void SecondCounter(object o, EventArgs sender)
        {
            DispatcherTimer d = (DispatcherTimer)o;
            d.Stop(); // kill the timer

            if (!_metronome.bOn)
            {
                return;
            }

            _iNumSeconds++;

            txtTimer.Text = string.Format("{0:00}:{1:00}", (int)(_iNumSeconds / 60), (int)(_iNumSeconds % 60));

            //d = new DispatcherTimer();
            //d.Interval = new TimeSpan(0, 0, 1); // 1 second
            //d.Tick += new EventHandler(SecondCounter);
            d.Start();

        }

        string GetSoundLocation(int iSound)//0 for the 1st one ... anything else for the second
        {
        /*    if(iSound == 0)
                // return "Assets/Waves/a-hi.wav"; 
                return "Assets/Metronome/synth1.wav"; 
            else
                return "Assets/Metronome/synth2.wav"; */
               

            if (iSound == 0)
            {
                switch ((int)Globals.Settings.eMetronomeSounds)
                {   //swapped wood and metal so internally they're labelled wrong ... all good
                    case 0: return "Assets/Metronome/metal1.wav";
                    case 1: return "Assets/Metronome/wood1.WAV";
                    case 2: return "Assets/Metronome/synth1.wav";
                    case 3: return "Assets/Metronome/old1.wav";
                    default: return "Assets/Metronome/silence.wav";
                }
            }
            else
            {
                switch ((int)Globals.Settings.eMetronomeSounds)
                {
                    case 0: return "Assets/Metronome/metal2.wav";
                    case 1: return "Assets/Metronome/wood2.wav";
                    case 2: return "Assets/Metronome/synth2.wav";
                    case 3: return "Assets/Metronome/old2.wav";
                    default: return "Assets/Metronome/silence.wav";
                }
            }
        }


        private void ResetTickSounds(bool bForce)
        {   //bugbug
            
            // we don't need to reset
            if (_arrSounds.Count == _metronome.iMeter && bForce == false)
                return;

            _arrSounds.Clear();

            SoundPlayer mySoundEffect;

            for (int i = 0; i < _metronome.iMeter; i++)
            {
                Uri uriSound;
                
                if(i == _metronome.iMeter -1)
                    uriSound = new Uri(@"./" + GetSoundLocation(0), UriKind.Relative);
                else
                    uriSound = new Uri(@"./" + GetSoundLocation(1), UriKind.Relative);

                StreamResourceInfo sri = App.GetResourceStream(uriSound);

                if (sri == null)
                    break;

                mySoundEffect = new SoundPlayer(sri.Stream);

                _arrSounds.Add(mySoundEffect);
            }
            
        }
        private void ResetTickIndicators()
        {
            // it's the same
            if (gridTicks.Children.Count == _metronome.iMeter)
                return;

            // delete all the old tick images
            gridTicks.Children.Clear();
            //_arrTicks.Clear();

            gridTicks.ColumnDefinitions.Clear();

            for (int i = 0; i <= _metronome.iMeter ; i++)
            {
                ColumnDefinition n = new ColumnDefinition();
                gridTicks.ColumnDefinitions.Add(n);
            }

            gridTicks.UpdateLayout();

            for(int i=0; i<_metronome.iMeter; i++)
            {
                Image img = new Image();

                //BitmapImage bmi = new BitmapImage(new Uri("/Assets/Metronome/meter.png", UriKind.RelativeOrAbsolute));
                img.Source = new System.Windows.Media.Imaging.BitmapImage(new Uri("/Assets/Metronome/meter.png", UriKind.Relative)); 
                img.HorizontalAlignment = HorizontalAlignment.Left;
                img.VerticalAlignment = VerticalAlignment.Top;
                img.Visibility = Visibility.Collapsed;                

                gridTicks.Children.Add(img);

                Grid.SetColumn(img, i + 1);
            }
        }

        /*private void myMediaPlayer_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("myMediaPlayer_MediaFailed");
        }

        private void myMediaPlayer_MediaOpened(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("myMediaPlayer_MediaOpened");
        }

        private void myMediaPlayer_DownloadProgressChanged(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("myMediaPlayer_DownloadProgressChanged");
        }

        private void myMediaPlayer_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("myMediaPlayer_CurrentStateChanged");
        }

        private void myMediaPlayer_BufferingProgressChanged(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("myMediaPlayer_BufferingProgressChanged");
        }*/

        private void dtBMP_Tick()
        {
            _iCurrentIndex++;

            if (_iCurrentIndex >= _metronome.iMeter)
                _iCurrentIndex = 0;

            SoundPlayer sfx = _arrSounds[_iCurrentIndex];
            sfx.Play();

            System.Threading.Thread thread = new System.Threading.Thread(
                   new System.Threading.ThreadStart(
                     delegate()
                     {
                         _storyGreenBlink.Dispatcher.Invoke(
                         System.Windows.Threading.DispatcherPriority.Normal,
                       new Action(
                         delegate()
                         {
                             _storyGreenBlink.Begin();
                         }));

                         if (_iCurrentIndex == _metronome.iMeter - 1)
                         {
                             _storyRedBlink.Dispatcher.Invoke(
                             System.Windows.Threading.DispatcherPriority.Normal,
                                new Action(
                             delegate()
                             {

                                 _storyRedBlink.Begin();
                             }));
                         }
                     }
            
                ));
            thread.Start();

           // Dispatcher.BeginInvoke(() =>
            {
                int iIndex = _iCurrentIndex + 1;
                txtMeterIndex.Text = iIndex.ToString();

                if (gridTicks.Children.Count == 1)
                    return;

                for (int i = 0; i < gridTicks.Children.Count; i++)
                {
                    Image img = (Image)gridTicks.Children[i];

                    if (i <= _iCurrentIndex)
                        img.Visibility = Visibility.Visible;
                    else
                        img.Visibility = Visibility.Collapsed;
                }
            }//);
        }

        /*private void SequencerIndexChanged(Object sender, EventArgs e)
        {
            if (!_metronome.bOn)
                return; // don't do anything

            System.Diagnostics.Debug.WriteLine("SequencerIndexChanged: " + _metronome.sequencer.StepIndex.ToString());
            Dispatcher.BeginInvoke(() =>
            {
                _storyGreenBlink.Begin();

                if (_metronome.sequencer.StepIndex == _metronome.iMeter-1)
                    _storyRedBlink.Begin();

                int iIndex = _metronome.sequencer.StepIndex + 1;
                txtMeterIndex.Text = iIndex.ToString();

                for (int i = 0; i < gridTicks.Children.Count; i++)
                {
                    Image img = (Image)gridTicks.Children[i];

                    if (i <= _metronome.sequencer.StepIndex)
                        img.Visibility = Visibility.Visible;
                    else
                        img.Visibility = Visibility.Collapsed;
                }
            });
        }*/
        #endregion

        #region BTN_PlayPause
        private void rectBtnPlayPause_MouseLeave(object sender, MouseEventArgs e)
        {
            if (!_bBtnDown)
                return;

            _bBtnDown = false;

            imgBtnPlayPause.Visibility = Visibility.Collapsed;
            txtBtnPlayPause.Foreground = new SolidColorBrush(Colors.White);
        }

        private void rectBtnPlayPause_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            _bBtnDown = true;

            imgBtnPlayPause.Visibility = Visibility.Visible;
            txtBtnPlayPause.Foreground = new SolidColorBrush(Colors.Yellow);
        }

        private void rectBtnPlayPause_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (!_bBtnDown)
                return;

            ToggleOnOff();

            _bBtnDown = false;

            imgBtnPlayPause.Visibility = Visibility.Collapsed;
            txtBtnPlayPause.Foreground = new SolidColorBrush(Colors.White);
        }
        #endregion // BTN_PlayPause

        #region BTN_TempoAndMeter
        private void rectBtn_MouseLeave(object sender, MouseEventArgs e)
        {
            _dtTapNHold.Stop();
            _dtTapRepeat.Stop();

            if (!_bBtnDown)
                return;

            _bBtnDown = false;

            System.Windows.Shapes.Rectangle r = (System.Windows.Shapes.Rectangle)sender;
            if(r == rectBtnTempUp)
            {
                imgBtnTempoUp.Visibility = Visibility.Collapsed;
            }
            else if(r == rectBtnTempDown)
            {
                imgBtnTempoDown.Visibility = Visibility.Collapsed;
            }
            else if (r == rectBtnMeterUp)
            {
                imgBtnMeterUp.Visibility = Visibility.Collapsed;
            }
            else if (r == rectBtnMeterDown)
            {
                imgBtnMeterDown.Visibility = Visibility.Collapsed;
            }
        }

        private void rectBtn_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            _dtTapNHold.Stop();
            _dtTapRepeat.Stop();

            _bBtnDown = true;

            System.Windows.Shapes.Rectangle r = (System.Windows.Shapes.Rectangle)sender;
            if (r == rectBtnTempUp)
            {
                _eBtn = EnumButtonPress.BMP_Up;
                imgBtnTempoUp.Visibility = Visibility.Visible;
            }
            else if (r == rectBtnTempDown)
            {
                _eBtn = EnumButtonPress.BMP_Down;
                imgBtnTempoDown.Visibility = Visibility.Visible;
            }
            else if (r == rectBtnMeterUp)
            {
                _eBtn = EnumButtonPress.Meter_Up;
                imgBtnMeterUp.Visibility = Visibility.Visible;
            }
            else if (r == rectBtnMeterDown)
            {
                _eBtn = EnumButtonPress.Meter_Down;
                imgBtnMeterDown.Visibility = Visibility.Visible;
            }

            _dtTapNHold.Start();
        }

        private void rectBtn_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _dtTapNHold.Stop();
            _dtTapRepeat.Stop();

            if (!_bBtnDown)
                return;

            _bBtnDown = false;

            if (_metronome.bOn)
            {
                ResetSequencer();
            }

            System.Windows.Shapes.Rectangle r = (System.Windows.Shapes.Rectangle)sender;
            if (r == rectBtnTempUp)
            {
                _metronome.TempoUp();
                imgBtnTempoUp.Visibility = Visibility.Collapsed;
            }
            else if (r == rectBtnTempDown)
            {
                _metronome.TempoDown();
                imgBtnTempoDown.Visibility = Visibility.Collapsed;
            }
            else if (r == rectBtnMeterUp)
            {
                _metronome.MeterUp();
                imgBtnMeterUp.Visibility = Visibility.Collapsed;
            }
            else if (r == rectBtnMeterDown)
            {
                _metronome.MeterDown();
                imgBtnMeterDown.Visibility = Visibility.Collapsed;
            }

            if (_metronome.bOn)
            {
                ResetSequencer();
            }
        }

        private void dtTapNHold_Tick(object sender, EventArgs e)
        {
            _dtTapNHold.Stop();

            if (_bBtnDown)
                _dtTapRepeat.Start();
        }

        private void dtTapRepeat_Tick(object sender, EventArgs e)
        {
            if (_metronome.bOn)
                ToggleOnOff();

            switch (_eBtn)
            {
                case EnumButtonPress.BMP_Down:
                    _metronome.TempoDown();
                    break;
                case EnumButtonPress.BMP_Up:
                    _metronome.TempoUp();
                    break;
                case EnumButtonPress.Meter_Down:
                    _metronome.MeterDown();
                    break;
                case EnumButtonPress.Meter_Up:
                    _metronome.MeterUp();
                    break;
            }
        }
        #endregion

        #region Knob
        private void canvasKnob_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (_bAniKnob)
                return;

            System.Windows.Point ptStart = e.GetPosition(this);
            _eQuadrantStart = GetQuadrant(ptStart);

            GetForwardBackwardQuadrants(iKnobCurrentIndex);

            if (_metronome.bOn)
                ResetSequencer();

            _bBtnDown = true;
        }

        private void canvasKnob_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _bBtnDown = false;
        }

        private void canvasKnob_MouseMove(object sender, MouseEventArgs e)
        {
            if (Mouse.LeftButton != MouseButtonState.Pressed)
                return;

            if (!_bBtnDown || _dtKnob.IsEnabled || _bAniKnob)
                return;

            System.Windows.Point ptCheck = e.GetPosition(this);
            EnumQuadrant eCheck = GetQuadrant(ptCheck);

            //System.Diagnostics.Debug.WriteLine(eCheck.ToString());

            if (eCheck == _eQuadrantStart)
            {
            }
            else if(eCheck == EnumQuadrant.Undefined)
            {
                // some error
                _bBtnDown = false;
            }
            else if (eCheck == _eQuadrantForward)
            {
                _eQuadrantStart = _eQuadrantForward;
                _iKnobDestIndex = iKnobCurrentIndex + 2;
                GetForwardBackwardQuadrants(_iKnobDestIndex);
                _bKnobForward = true;
                _bAniKnob = true;
                _dtKnob.Start();
            }
            else if (eCheck == _eQuadrantBackward)
            {
                _eQuadrantStart = _eQuadrantBackward;
                _iKnobDestIndex = iKnobCurrentIndex - 2;
                GetForwardBackwardQuadrants(_iKnobDestIndex);
                _bKnobForward = false;
                _bAniKnob = true;
                _dtKnob.Start();
            }

        }

        private void rectKnobMiddle_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (_dtKnob.IsEnabled || _bAniKnob)
                return;

            if (_metronome.bOn)
                ResetSequencer();

            _iKnobDestIndex = iKnobCurrentIndex + 2;

            if (_iKnobDestIndex >= _iKnobCount)
                _iKnobDestIndex = 0;

            _bKnobForward = (_iKnobDestIndex > iKnobCurrentIndex ? true : false);

            _dtKnob.Start();
            _bAniKnob = true;
        }

        private void dtKnob_Tick(object sender, EventArgs e)
        {
            if (!_bAniKnob)
            {
                _dtKnob.Stop();
                return;
            }

            iKnobCurrentIndex += (_bKnobForward ? 1 : -1);

            // if it goes off the end then we have a problem
            if (iKnobCurrentIndex < 0 || iKnobCurrentIndex >= _iKnobCount)
            {
                iKnobCurrentIndex = (iKnobCurrentIndex < 0 ? 0 : (_iKnobCount-1));
                imgKnob.SetValue(Canvas.LeftProperty, iKnobCurrentIndex * -173d);
                _metronome.iSoundIndex = iKnobCurrentIndex / 2;
                _dtKnob.Stop();
                _bAniKnob = false;                
                ResetTickSounds(true);
                return;
            }

            imgKnob.SetValue(Canvas.LeftProperty, iKnobCurrentIndex * -173d);
         //   imgKnob.SetValue(Canvas.LeftProperty, -100d);

            if (iKnobCurrentIndex == _iKnobDestIndex)
            {
                _metronome.iSoundIndex = iKnobCurrentIndex / 2;
                _dtKnob.Stop();
                _bAniKnob = false;                
                ResetTickSounds(true);
            }
        }

        private void GetForwardBackwardQuadrants(int iCurrentIndex)
        {
            switch (_eQuadrantStart)
            {
                case EnumQuadrant.TopLeft:
                    _eQuadrantForward = EnumQuadrant.TopRight;
                    _eQuadrantBackward = EnumQuadrant.BottomLeft;
                    break;
                case EnumQuadrant.TopRight:
                    _eQuadrantForward = EnumQuadrant.BottomRight;
                    _eQuadrantBackward = EnumQuadrant.TopLeft;
                    break;
                case EnumQuadrant.BottomRight:
                    _eQuadrantForward = EnumQuadrant.BottomLeft;
                    _eQuadrantBackward = EnumQuadrant.TopRight;
                    break;
                case EnumQuadrant.BottomLeft:
                    _eQuadrantForward = EnumQuadrant.TopLeft;
                    _eQuadrantBackward = EnumQuadrant.BottomRight;
                    break;
                default:
                    // some error
                    return;
            }

            // we don't allow it to go off the end
            if (iCurrentIndex == 0)
                _eQuadrantBackward = EnumQuadrant.Undefined;

            if (iCurrentIndex == _iKnobCount - 1)
                _eQuadrantForward = EnumQuadrant.Undefined;
        }

        private void SetCurrentKnobIndex()
        {
            imgKnob.SetValue(Canvas.LeftProperty, iKnobCurrentIndex * -173d);
        }

        private EnumQuadrant GetQuadrant(System.Windows.Point pt)
        {
            GeneralTransform gt = canvasKnob.TransformToVisual(this);

            System.Windows.Point ptTopLeft = gt.Transform(new System.Windows.Point(0, 0));

            double dbWidth = canvasKnob.ActualWidth;
            double dbHeight = canvasKnob.ActualHeight;

            Rect rc;

            // top left
            rc = new Rect(ptTopLeft.X, ptTopLeft.Y, dbWidth / 2, dbHeight / 2);
            if (rc.Contains(pt))
                return EnumQuadrant.TopLeft;

            // top right
            rc = new Rect(ptTopLeft.X + dbWidth / 2, ptTopLeft.Y , dbWidth / 2, dbHeight / 2);
            if (rc.Contains(pt))
                return EnumQuadrant.TopRight;

            // bottom right
            rc = new Rect(ptTopLeft.X + dbWidth / 2, ptTopLeft.Y + dbHeight/2 , dbWidth / 2, dbHeight / 2);
            if (rc.Contains(pt))
                return EnumQuadrant.BottomRight;

            // bottom left
            rc = new Rect(ptTopLeft.X, ptTopLeft.Y + dbHeight/2, dbWidth / 2, dbHeight / 2);
            if (rc.Contains(pt))
                return EnumQuadrant.BottomLeft;

            return EnumQuadrant.Undefined;
        }

        #endregion


        //variables for the TapSet function so just leave em here 
        static int NUM_TAPS = 8;
        int iNextTick = 0;//just makes it easier
        int iCurrentTick = 0;
        bool bSetBPM = false;
        //long dwLastTick = 0;
        int[] iTaps = new int[NUM_TAPS];
        //
        private void txtTapSetBPM_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if(_metronome.bOn)
                return;

            //if its been too long reset
            iCurrentTick = iNextTick;
            iTaps[iCurrentTick] = Environment.TickCount;

            iNextTick++;
            if(iNextTick >= NUM_TAPS)
            {
                bSetBPM = true;
                iNextTick = 0;
            }

            if(bSetBPM && (iTaps[iCurrentTick] - iTaps[iNextTick] > 10000))
            {   //reset everything
                iTaps[0] = iTaps[iCurrentTick];
                iCurrentTick = 0;
                iNextTick = 0;
                bSetBPM = false;
            }

            if(bSetBPM)
            {   //calc the bpm
                _metronome.iBPM = (NUM_TAPS - 1)*60000/(iTaps[iCurrentTick] - iTaps[iNextTick]);
                
  
            }

            _storyGreenBlink.Begin();
      
        }
    }
}
