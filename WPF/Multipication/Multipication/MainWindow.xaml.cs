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

namespace Multipication
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 

    public enum OperatorType
    {
        plus,
        minus,
        times,
        divide,
    }

    public partial class MainWindow : Window
    {
        Random rand = new Random();

        System.Windows.Threading.DispatcherTimer myTimer = new System.Windows.Threading.DispatcherTimer();

        OperatorType eType;

        int iValue1;
        int iValue2;

        int iLevel = 1;

        int iSolved;
        int iGuesses;

        double dbAns;

        public MainWindow()
        {
            InitializeComponent();
            UpdateStatus();
            CreateQuestion();

            PlaySound("./Assets/math-intro.wav");


        }

        void Reset()
        {
            iSolved = 0;
            iGuesses = 0;
            CreateQuestion();
            UpdateStatus();

        }

        void CreateQuestion()
        {
            eType = OperatorType.times;// (OperatorType)rand.Next(3);

            if(iLevel < 2)
                iValue1 = 1+rand.Next(5);
            else if(iLevel == 2)
                iValue1 = 4 + rand.Next(4);
            else
                iValue1 = 7 + rand.Next(4);

            iValue2 = 1+rand.Next(iValue1);

            dbAns = GetAnswer();

            textQ1.Text = iValue1.ToString();
            textQ2.Text = iValue2.ToString();

            txtOperator.Text = GetOperatorString();

            textAnswer.Text = "";

        }

        string GetOperatorString()
        {
            switch (eType)
            {
                case OperatorType.plus:
                    return "+";
                case OperatorType.minus:
                    return "-";
                case OperatorType.times:
                    return "×";
                case OperatorType.divide:
                    return "÷";
            }
            return "oops";
        }

        double GetAnswer()
        {
            switch (eType)
            {
                case OperatorType.plus:
                    return iValue1 + iValue2;
                case OperatorType.minus:
                    return iValue1 - iValue2;
                case OperatorType.times:
                    return iValue1 * iValue2;
                case OperatorType.divide:
                    return iValue1 / iValue2;
            }

            return 0;
        }

        private void btnNewQuestion_Click(object sender, RoutedEventArgs e)
        {
            CreateQuestion();
        }

        void UpdateStatus()
        {
            textSolved.Text = "Solved: " + iSolved;
            textGuesses.Text = "Guesses: " + iGuesses;
            if (iSolved == 0 && iGuesses == 0)
                textPcnt.Text = "Percentage: --";
            else if (iSolved == 0)
                textPcnt.Text = "Percentage: 0%";
            else
                textPcnt.Text = "Percentage: " + (100 * iSolved / iGuesses).ToString() + "%";
        }

        private void btnReset_Click(object sender, RoutedEventArgs e)
        {
            Reset();
        }

        private void btn9_Click(object sender, RoutedEventArgs e)
        {
            AddNumber(9);
        }

        private void btn8_Click(object sender, RoutedEventArgs e)
        {
            AddNumber(8);            
        }

        private void btn7_Click(object sender, RoutedEventArgs e)
        {
            AddNumber(7);
        }

        private void btn6_Click(object sender, RoutedEventArgs e)
        {
            AddNumber(6);
        }

        private void btn5_Click(object sender, RoutedEventArgs e)
        {
            AddNumber(5);
        }

        private void btn4_Click(object sender, RoutedEventArgs e)
        {
            AddNumber(4);
        }

        private void btn3_Click(object sender, RoutedEventArgs e)
        {
            AddNumber(3);
        }

        private void btn2_Click(object sender, RoutedEventArgs e)
        {
            AddNumber(2);
        }

        private void btn1_Click(object sender, RoutedEventArgs e)
        {
            AddNumber(1);
        }

        private void btn0_Click(object sender, RoutedEventArgs e)
        {
            AddNumber(0);
        }

        private void Button_Click_Clear(object sender, RoutedEventArgs e)
        {
            textAnswer.Text = "";
        }

        private void Button_Click_Enter(object sender, RoutedEventArgs e)
        {

            OnEnter();
        }
        private void OnEnter()
        {
            if (textAnswer.Text == "")
                return;

            double dbGuess;

            try
            {
                dbGuess = Convert.ToDouble(textAnswer.Text);
            }
            catch
            {
                return;
            }

            if (dbGuess == dbAns)
            {
                PlaySound("./Assets/correct.wav");

                SetResult("Correct!");
                CreateQuestion();
                textAnswer.Text = "";
                iGuesses++;
                iSolved++;
                UpdateStatus();
                CreateQuestion();
            }
            else
            {
                PlaySound("./Assets/error.wav");

                SetResult("Try again!");
                textAnswer.Text = "";
                iGuesses++;
                UpdateStatus();
            }
        }

        void SetResult(String szIn)
        {
            textResults.Text = szIn;

            //System.Windows.Threading.DispatcherTimer myTimer = new System.Windows.Threading.DispatcherTimer();
            myTimer.Interval = new TimeSpan(0, 0, 0, 0, 2000);
            myTimer.Tick += new EventHandler(ClearResult);
            myTimer.Start();
        }

        public void ClearResult(object o, EventArgs sender)
        {
            textResults.Text = "";
            myTimer.Stop();
        }

        private void btnAbout_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Copyright 2011\r\nAll Rights Reserved\r\nPanoramic Software Inc.\r\nwww.panoramicsoft.com\r\n\r\nTest program - Not for Sale",
                "Math Adventure Alpla", MessageBoxButton.OK);

        }

        public static void PlaySound(string res)
        {
         //   if (Globals.Settings.bPlaySounds == false)
         //       return;

            Uri uriSound = new Uri(@res, UriKind.Relative);
            //  Uri uriSound = new Uri(@"./Assets/Waves/sudoku_start.wav", UriKind.Relative);

            StreamResourceInfo sri = App.GetResourceStream(uriSound);

            if (sri == null)
                return;

            var Player = new SoundPlayer(sri.Stream);

            Player.Play();
        }

        private void btnLev1_Click(object sender, RoutedEventArgs e)
        {
            SetLevel(1);
        }

        private void btnLev3_Click(object sender, RoutedEventArgs e)
        {
            SetLevel(3);
        }

        private void btnLev2_Click(object sender, RoutedEventArgs e)
        {
            SetLevel(2);
        }

        void SetLevel(int iNewLevel)
        {
            ImageBrush brush = new ImageBrush();
            BitmapImage bitmap;

            switch (iNewLevel)
            {
                case 1:
                    iLevel = 1;
                    bitmap =new BitmapImage(new Uri("pack://application:,,/Assets/bg_pigfarm.png"));
                   // new BitmapImage(new Uri("../Assets/appbar_sudoku.png", UriKind.Relative));
                    break;
                case 2:
                    iLevel = 2;
                    bitmap = new BitmapImage(new Uri("pack://application:,,/Assets/bg_forest.png"));
                    break;
                case 3:
                default:
                    iLevel = 3;
                    bitmap = new BitmapImage(new Uri("pack://application:,,/Assets/bg_farm.png"));
                    break;
            }

            brush.Stretch = Stretch.UniformToFill;
            brush.AlignmentY = AlignmentY.Top;

            brush.ImageSource = bitmap;

            Layout_Root.Background = brush;

            textLevel.Text = "Level: " + iLevel;

            CreateQuestion();
        }

        private void Layout_Root_KeyUp(object sender, KeyEventArgs e)
        {
            //not triggered
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            
        }

        private void Window_KeyUp(object sender, KeyEventArgs e)
        {
            switch (e.Key)
            {
                case Key.NumPad0:
                case Key.D0:
                    AddNumber(0);
                    break;
                case Key.NumPad1:
                case Key.D1:
                    AddNumber(1);
                    break;
                case Key.NumPad2:
                case Key.D2:
                    AddNumber(2);
                    break;
                case Key.NumPad3:
                case Key.D3:
                    AddNumber(3);
                    break;
                case Key.NumPad4:
                case Key.D4:
                    AddNumber(4);
                    break;
                case Key.NumPad5:
                case Key.D5:
                    AddNumber(5);
                    break;
                case Key.NumPad6:
                case Key.D6:
                    AddNumber(6);
                    break;
                case Key.NumPad7:
                case Key.D7:
                    AddNumber(7);
                    break;
                case Key.NumPad8:
                case Key.D8:
                    AddNumber(8);
                    break;
                case Key.NumPad9:
                case Key.D9:
                    AddNumber(9);
                    break;
                case Key.Delete:
                    textAnswer.Text = "";
                    break;
                case Key.Enter:
                    OnEnter();
                    break;
            }
        }

        private void AddNumber(int iNum)
        {
            if (textAnswer.Text.Length < 5)
                textAnswer.Text += iNum.ToString();
        }



    }
}
