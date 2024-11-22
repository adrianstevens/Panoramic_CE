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

namespace GuitarSuite.Controls
{
    /// <summary>
    /// Interaction logic for Tuner.xaml
    /// </summary>
    public partial class Tuner : UserControl, IScreenControl
    {
        ObjGuitarTuner _tuner = null;

        static string[] NoteNames 
        {
            get
            {
                return Globals.myGen.m_szRoots;
            }

        }
        static double ToneStep = Math.Pow(2, 1.0 / 12.0);
        const double AFrequency = 440.0;
        const int ToneIndexOffsetToPositives = 120;
        int _iCountNotes = 0;
        int _iStringIndex = 6;

        private const int BARS = 53;
        private double[] _arrSeparations = { 0.40, 0.43, 0.57, 0.60 };

        private bool _bBarsAniForward;
        private int _iBarsIndexCurrent;
        private int _iBarsIndexDestination;
        private DispatcherTimer _dtBars = null;

        //timer
        private DispatcherTimer _dtMicTimer = null;
        private int iBufferSize = 0;
        private int iTimerMS = 1000;

        long ticksLastGraph = -1;

        SolidColorBrush graphBrush = null;

        bool bShowSpectr
        {
            get
            {
                return Globals.Settings.bTuneGraphSpec;
            }
            set
            {
                Globals.Settings.bTuneGraphSpec = value;

                if (Globals.Settings.bTuneGraphSpec == true)
                {
                    imgSwitch.SetValue(Canvas.LeftProperty, 0 * -96d);
                    imgGraphPrev.SetValue(Canvas.LeftProperty, 0 * -99d);
                }
                else
                {
                    imgSwitch.SetValue(Canvas.LeftProperty, 1 * -96d);
                    imgGraphPrev.SetValue(Canvas.LeftProperty, 1 * -99d);
                }
            }
        }
        

        public Tuner()
        {
            InitializeComponent();

            _dtBars = new DispatcherTimer();
            _dtBars.Tick += new EventHandler(dtBars_Tick);

            bShowSpectr = Globals.Settings.bTuneGraphSpec;//redundant but gets it into the right state


            //timer to poll the mic
            _dtMicTimer = new DispatcherTimer();
            _dtMicTimer.Interval = new TimeSpan(0, 0, 0, 0, iTimerMS);//500
            _dtMicTimer.Tick += new EventHandler(MicTimer);

            graphBrush = new SolidColorBrush(Color.FromArgb(255, 0, 255, 0));
        }

        public void Refresh()
        {
            //update the tuner letters
            InitNoteGrid();
            UpdateTunerLimits();
        }

        public void UnloadScreen()
        {
            StopTuner();
        }

        public void StopTuner()
        {
            // make sure to stop the microphone
            if (_tuner != null)
                _tuner.StopTuner();
            _dtMicTimer.Stop();
            _dtBars.Stop();

        }

        private void InitNoteGrid()
        {
            gridNotes.Children.Clear();
            gridNotes.ColumnDefinitions.Clear();

            TextBlock text;

            for (int i = 0; i < Globals.myGen.GetNumStrings(); i++)
            {
                ColumnDefinition n = new ColumnDefinition();
                gridNotes.ColumnDefinitions.Add(n);

                text = new TextBlock();
                text.VerticalAlignment = VerticalAlignment.Center;
                text.HorizontalAlignment = HorizontalAlignment.Center;
                text.Foreground = new SolidColorBrush(Color.FromArgb(255, 0, 255, 0));
                text.Text = Globals.myGen.GetNote(i, 0);
                Grid.SetColumn(text, i);

                gridNotes.Children.Add(text);
            }

            //and one more for auto

            text = new TextBlock();
            text.VerticalAlignment = VerticalAlignment.Center;
            text.HorizontalAlignment = HorizontalAlignment.Center;
            text.Foreground = new SolidColorBrush(Color.FromArgb(255, 0, 255, 0));
            text.Text = "Auto ►";
            Grid.SetColumnSpan(text, 6);
            text.FontSize = 16;

            gridNotes.Children.Add(text);

            UpdateStringText();
        }

        private void UpdateStringText()
        {
            double dbOpacity = 0.2;

            if (_iStringIndex >= Globals.myGen.GetNumStrings())
            {
                dbOpacity = 0.0;
                gridNotes.Children[Globals.myGen.GetNumStrings()].Opacity = 1.0;
            }
            else
            {
                gridNotes.Children[Globals.myGen.GetNumStrings()].Opacity = 0.0;
            }

            for (int i = 0; i < Globals.myGen.GetNumStrings(); i++)
            {
                if (i == _iStringIndex)
                    gridNotes.Children[i].Opacity = 1.0;
                else
                    gridNotes.Children[i].Opacity = dbOpacity;
            }
        }

        private void Layout_Root_Loaded(object sender, RoutedEventArgs e)
        {
            _tuner = new ObjGuitarTuner();
            _tuner.FrequencyDetected += new EventHandler<EventArgs>(OnFrequencyDetected);

            if (_tuner.StartTuner() == true)
                txtHz.Text = "";

            // create the column definitions
            for (int i = 0; i <= BARS; i++)
            {
                ColumnDefinition n = new ColumnDefinition();
                gridBars.ColumnDefinitions.Add(n);
            }

            InitNoteGrid();

            _dtMicTimer.Start();
        }

        void OnFrequencyDetected(object sender, EventArgs e)
        {
            
          //  Dispatcher.BeginInvoke(delegate()
            {
                lock (_tuner.visualLock)
                {
                    double dbTargetValue;
                    string szNoteName;


                    if (_tuner.dbFundamentalFrequency == 0)
                    {
                        if (_iCountNotes > 20)
                        {
                            txtOctave.Text = "";
                            txtHz.Text = "";
                            txtNote.Text = "";
                            imgArrowLeft.Opacity = 0.0;
                            imgArrowRight.Opacity = 0.0;
                            imgArrowGreen.Opacity = 0.0;
                            canvasBars.Opacity = 0.0;
                            _dtBars.Stop();
                            //         _graph.Clear();
                            //         _graph.Invalidate();

                        }

                        _iCountNotes++;
                    }
                    else
                    {
                        int toneIndex = (int)Math.Round(Math.Log(_tuner.dbFundamentalFrequency / AFrequency, ToneStep));
                        int iOctave = 0;

                        szNoteName = NoteNames[(int)Math.Max(0, (ToneIndexOffsetToPositives + toneIndex) % NoteNames.Length)];
                        double close = Math.Pow(ToneStep, toneIndex) * AFrequency;
                        double flat = Math.Pow(ToneStep, toneIndex - 1) * AFrequency;
                        double sharp = Math.Pow(ToneStep, toneIndex + 1) * AFrequency;
                        if (_tuner.dbFundamentalFrequency - close > 0)
                        {
                            dbTargetValue = 0.5 + 0.5 * (_tuner.dbFundamentalFrequency - close) / (sharp - close);

                            //     imgArrowLeft.Opacity = 0.0;
                            //     imgArrowRight.Opacity = 1.0;
                        }
                        else
                        {
                            dbTargetValue = 0.5 * (1.0 - (close - _tuner.dbFundamentalFrequency) / (close - flat));

                            //    imgArrowLeft.Opacity = 1.0;
                            //    imgArrowRight.Opacity = 0.0;
                        }

                        // show the bars
                        canvasBars.Opacity = 1.0;

                        int iIndex = 0;
                        bool bInTune = false;

                        // set the bar image to the proper index
                        for (int i = 0; i < _arrSeparations.Length; i++)
                        {
                            if (dbTargetValue < _arrSeparations[i])
                            {
                                iIndex = i;

                                // if it falls within this range we know we are in tune
                                if (i == 2)
                                    bInTune = true;
                                break;
                            }
                        }
                        //imgBars.SetValue(Canvas.LeftProperty, iIndex * -56d);

                        if (bInTune)
                        {
                            imgArrowLeft.Opacity = 0.0;
                            imgArrowRight.Opacity = 0.0;
                        }

                        double dbTemp = 0;

                        //and the octave
                        if (3951.07 + dbTemp < _tuner.dbFundamentalFrequency)
                            iOctave = 8;
                        else if (1975.53 + dbTemp < _tuner.dbFundamentalFrequency)
                            iOctave = 7;
                        else if (987.77 + dbTemp < _tuner.dbFundamentalFrequency)
                            iOctave = 6;
                        else if (493.88 + dbTemp < _tuner.dbFundamentalFrequency)
                            iOctave = 5;
                        else if (246.94 + dbTemp < _tuner.dbFundamentalFrequency)
                            iOctave = 4;
                        else if (123.47 + dbTemp < _tuner.dbFundamentalFrequency)
                            iOctave = 3;
                        else if (61.74 + dbTemp < _tuner.dbFundamentalFrequency)
                            iOctave = 2;
                        else if (30.87 + dbTemp < _tuner.dbFundamentalFrequency)
                            iOctave = 1;



                        // now let's set the image in the proper column
                        iIndex = (int)(dbTargetValue * BARS) - 3;   // we take the 3 off because the bars are shown 7 at a time so we need to work on the center one

                        AnimateBars(iIndex);


                        txtNote.Text = szNoteName;
                        txtHz.Text = String.Format("{0:0.00}", _tuner.dbFundamentalFrequency) + "Hz";
                        txtOctave.Text = iOctave.ToString();

                        _iCountNotes = 0;

                        //System.Diagnostics.Debug.WriteLine("OnFrequencyDetected - Frequency: " + _tuner.dbFundamentalFrequency.ToString());
                        //System.Diagnostics.Debug.WriteLine("OnFrequencyDetected - Note: " + szNoteName);
                        //System.Diagnostics.Debug.WriteLine("OnFrequencyDetected - TargetValue: " + dbTargetValue.ToString());
                    }


                    if (bShowSpectr)
                        GraphSpec();
                    else
                        Graph();

                }
            }
            //);
        }

        void AnimateBars(int iDestinationIndex)
        {
            _dtBars.Stop();

            if (_iBarsIndexCurrent == iDestinationIndex)
                return;

            // so we don't go off any edges
            iDestinationIndex = Math.Max(0, iDestinationIndex);
            iDestinationIndex = Math.Min(BARS - 8, iDestinationIndex);

            _bBarsAniForward = iDestinationIndex > _iBarsIndexCurrent ? true : false;

            // i want to do it in 400 ms ... gonna try 100 (Adrian)
            int iMS = 200 / Math.Abs(iDestinationIndex - _iBarsIndexCurrent);

            _iBarsIndexDestination = iDestinationIndex;

            _dtBars.Interval = new TimeSpan(0, 0, 0, 0, iMS);
            _dtBars.Start();

        }

        private void dtBars_Tick(object sender, EventArgs e)
        {
            _iBarsIndexCurrent += (_bBarsAniForward ? 1 : -1);


            Grid.SetColumn(canvasBars, _iBarsIndexCurrent);

            double dbTargetValue = ((double)(_iBarsIndexCurrent + 3) / (double)BARS);

            int iIndex = 0;
            // set the bar image to the proper index
            for (int i = 0; i < _arrSeparations.Length; i++)
            {
                if (dbTargetValue < _arrSeparations[i])
                {
                    iIndex = i;

                    break;
                }
            }
            imgBars.SetValue(Canvas.LeftProperty, iIndex * -56d);

            if (_iBarsIndexCurrent == _iBarsIndexDestination)
            {
                _dtBars.Stop();
            }

            //and set the arrow opacity
            if (_iBarsIndexCurrent + 3 == 26)
            {
                imgArrowLeft.Opacity = 0.0;
                imgArrowRight.Opacity = 0.0;

            }
            else if (_iBarsIndexCurrent + 3 > 26)
            {
                imgArrowLeft.Opacity = 0.0;
                imgArrowRight.Opacity = (_iBarsIndexCurrent + 3.0 - 26.0) / 17.0; //17 so they light up at about 75% out ... change to 26 for even scaling

            }
            else
            {
                imgArrowLeft.Opacity = 1.0 - (_iBarsIndexCurrent + 3.0) / 17.0;
                imgArrowRight.Opacity = 0.0;

            }

            //and fade the green arrow when its close
            if (Math.Abs(23 - _iBarsIndexCurrent) < 4)
            {
                imgArrowGreen.Opacity = (4.0 - (double)Math.Abs(23 - _iBarsIndexCurrent)) / 4.0;
            }
            else
            {
                imgArrowGreen.Opacity = 0;
            }

        }

        private void imgSwitch_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            bShowSpectr = !bShowSpectr;
        }

        private void txtNote_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _iStringIndex++;

            if (_iStringIndex > Globals.myGen.GetNumStrings())
                _iStringIndex = 0;

            UpdateStringText();

            UpdateTunerLimits();
        }

        private void gridNotes_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            txtNote_MouseLeftButtonUp(sender, e);
        }

        private void UpdateTunerLimits()
        {
            if (_iStringIndex >= Globals.myGen.GetNumStrings())
            {
                _tuner.SetRangeDefaults();
                //_tuner._minHz = 50.0;
                //_tuner._maxHz = 800.0;
            }
            else
            {
                double dbTemp = Globals.myGen.GetStringFreq(0);
                dbTemp = Globals.myGen.GetStringFreq(1);
                dbTemp = Globals.myGen.GetStringFreq(2);
                dbTemp = Globals.myGen.GetStringFreq(3);
                dbTemp = Globals.myGen.GetStringFreq(4);
                dbTemp = Globals.myGen.GetStringFreq(5);


                _tuner._maxHz = Globals.myGen.GetStringFreq(_iStringIndex) * 0.8;
                _tuner._maxHz = Globals.myGen.GetStringFreq(_iStringIndex) * 2.05;


                //      _tuner._minHz = Globals.myGen.GetMinStringFreq(_iStringIndex);
                //      _tuner._maxHz = Globals.myGen.GetMaxStringFreq(_iStringIndex);
            }
        }

        private void MicTimer(object o, EventArgs sender)
        {
    //        Dispatcher.BeginInvoke(() =>
            {
                _tuner.MicTimer(o, sender);
            }
            //bugubg);

        }

        void GraphSpec()
        {
            if (_tuner.spectr == null)
                return;

            if (DateTime.Now.Ticks - ticksLastGraph < 6000000 &&
                ticksLastGraph != -1)
                return;

            ticksLastGraph = DateTime.Now.Ticks;
            ClearGraph();

            int iWidth = (int)graphCanvas.Width;
            int iCenter = (int)graphCanvas.Height / 2;
            int iHeight = (int)graphCanvas.Height;

            int iLeft = 0;
            int iTop = 0;
            int iRight = iWidth;
            int iBottom = iHeight - 1;


            int xPrev = iLeft, yPrev = iBottom;

            int y = 0;
            int iIndex;

            double dbScale = (double)iHeight / _tuner.dbMaxSpectValue;

            if (dbScale > 3)
                dbScale = 3;

            for (int x = iLeft; x < iRight; x++)
            {
                //iIndex = x*1300 / iWidth;
                iIndex = x * (int)_tuner._maxHz / iWidth + (int)_tuner._minHz;

                y = iBottom - (int)(_tuner.spectr[iIndex] * dbScale);//scale factor goes here .....

                if (y < iTop)
                    y = iTop;

                //_graph.DrawLine(xPrev, yPrev, x, y, Colors.Green);
                DrawLine(xPrev, yPrev, x, y, Color.FromArgb(255, 0, 255, 0));

                xPrev = x;
                yPrev = y;
            }

            InvalidateGraph();
        }

        void ClearGraph()
        {
            graphCanvas.Children.Clear();
        }

        void InvalidateGraph()
        {

        }

        void DrawLine(int xStart, int yStart, int xEnd, int yEnd, Color clr)
        {
            Line line = new Line();
            line.X1 = xStart;
            line.Y1 = yStart;
            line.X2 = xEnd;
            line.Y2 = yEnd;
            line.StrokeThickness = 1;
            line.Stroke = graphBrush;
            line.IsHitTestVisible = false;

            graphCanvas.Children.Add(line);
        }

        void Graph()
        {
            if ((DateTime.Now.Ticks - ticksLastGraph) < 6000000 &&
                ticksLastGraph != -1)
                return;

            ticksLastGraph = DateTime.Now.Ticks;
            ClearGraph();

            int iWidth = (int)graphCanvas.Width;
            int iHeight = (int)graphCanvas.Height;

            int iLeft = 0;
            int iTop = 0;
            int iRight = iWidth;
            int iBottom = iHeight - 1;

            int iCenter = (iBottom - iTop) / 2;//+5 offset to align it to the background image
            double yScale = 0.5 * (iBottom - iTop) / 32768;


            //I'd do more scaling but the alignment seems to be inconsistant between the phone and the emulator ... baseline noise??
            if (_tuner.iMaxAmplitude < 8000)
            {
                yScale *= 2;
            }
            else
            {

            }

            //double yScale = (double)iHeight / (double)iMax;

            // a 16 bit sample has values from -32768 to 32767
            //double dbXScale = 0.5 * _tuner.i16Buffer.Length / (iRight - iLeft); //... old setting before I changed the mic sampling style 
            double dbXScale = (double)(_tuner.i16Buffer.Length) / (double)(iRight - iLeft) / 32.0;

            int xPrevLeft = 0, yPrevLeft = 0;
            for (int x = iLeft; x < iRight; x++)
            {
                int iArrayIndex = (int)(dbXScale * x);
                int y = iCenter + (int)(_tuner.i16Buffer[iArrayIndex] * yScale);
                if (x == 0)
                {
                    xPrevLeft = 0;
                    yPrevLeft = y;
                }
                else
                {
                    //_graph.DrawLine(xPrevLeft, yPrevLeft, xAxis, yAxis, Colors.Green);
                    DrawLine(xPrevLeft, yPrevLeft, x, y, Color.FromArgb(255, 0, 255, 0));
                    xPrevLeft = x;
                    yPrevLeft = y;
                }
            }

            InvalidateGraph();

        }
    }
}
