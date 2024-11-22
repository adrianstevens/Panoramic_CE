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
using PSAMControlLibrary;
using System.Windows.Media.Effects;
using System.Windows.Resources;
using System.Media;

namespace PitchPerfect
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Random rand = new Random();

        System.Windows.Threading.DispatcherTimer myTimer = new System.Windows.Threading.DispatcherTimer();

        int iNote = -1;
        int iOctave;

        int iGuesses = 0;
        int iCorrect = 0;




        public MainWindow()
        {
            InitializeComponent();
        }

        //Unknown, Clef, Note, Rest, Barline, Key, TimeSignature, Direction };
        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            Clef c = new Clef(ClefType.GClef, 2);
            viewer.AddMusicalSymbol(c);

            

            Note n;
            Barline b;

            n = new Note("E", 0, 5, MusicalSymbolDuration.Sixteenth,
                    NoteStemDirection.Down, NoteTieType.None,
                    new List<NoteBeamType>() { NoteBeamType.Start, NoteBeamType.Start });
            viewer.AddMusicalSymbol(n);

            n = new Note("D", 1, 5, MusicalSymbolDuration.Sixteenth,
                    NoteStemDirection.Down, NoteTieType.None,
                    new List<NoteBeamType>() { NoteBeamType.End, NoteBeamType.End });
            viewer.AddMusicalSymbol(n);


            viewer.AddMusicalSymbol(new Barline());

            n = new Note("E", 0, 5, MusicalSymbolDuration.Sixteenth,
                    NoteStemDirection.Down, NoteTieType.None,
                    new List<NoteBeamType>() { NoteBeamType.Start, NoteBeamType.Start });
            viewer.AddMusicalSymbol(n);

            n = new Note("D", 1, 5, MusicalSymbolDuration.Sixteenth,
                    NoteStemDirection.Down, NoteTieType.None,
                    new List<NoteBeamType>() { NoteBeamType.Continue, NoteBeamType.Continue });
            viewer.AddMusicalSymbol(n);

            n = new Note("E", 0, 5, MusicalSymbolDuration.Sixteenth,
                    NoteStemDirection.Down, NoteTieType.None,
                    new List<NoteBeamType>() { NoteBeamType.Continue, NoteBeamType.Continue });
            viewer.AddMusicalSymbol(n);

            n = new Note("B", 0, 4, MusicalSymbolDuration.Sixteenth,
                    NoteStemDirection.Down, NoteTieType.None,
                    new List<NoteBeamType>() { NoteBeamType.Continue, NoteBeamType.Continue });
            viewer.AddMusicalSymbol(n);

            n = new Note("D", 0, 5, MusicalSymbolDuration.Sixteenth,
                    NoteStemDirection.Down, NoteTieType.None,
                    new List<NoteBeamType>() { NoteBeamType.Continue, NoteBeamType.Continue });
            viewer.AddMusicalSymbol(n);

            n = new Note("C", 0, 5, MusicalSymbolDuration.Sixteenth,
                    NoteStemDirection.Down, NoteTieType.None,
                    new List<NoteBeamType>() { NoteBeamType.End, NoteBeamType.End });
            viewer.AddMusicalSymbol(n);

            viewer.AddMusicalSymbol(new Barline());

            n = new Note("A", 0, 4, MusicalSymbolDuration.Eighth,
                    NoteStemDirection.Up, NoteTieType.None,
                    new List<NoteBeamType>() { NoteBeamType.Single });
            viewer.AddMusicalSymbol(n);




           

            viewer.SnapsToDevicePixels = true;
            
         /*   DropShadowBitmapEffect effect = new DropShadowBitmapEffect();
            effect.Color = Colors.Black;
            effect.ShadowDepth = 10;
            effect.Opacity = 0.7;
            effect.Softness = 9;
            effect.Direction = 320;

            viewer.BitmapEffect = effect;*/


            ScaleTransform scale = new ScaleTransform(3, 3);
            viewer.RenderTransform = scale;
        }

        void SetRandomNote()
        {
            viewer.ClearMusicalIncipit();

            iNote = rand.Next(7);
            iOctave = rand.Next(4) + 2;

            if (iOctave >= 4)
            {
                Clef c = new Clef(ClefType.GClef, 2);
                viewer.AddMusicalSymbol(c);
            }
            else
            {
                Clef c = new Clef(ClefType.FClef, 4);
                viewer.AddMusicalSymbol(c);
            }


      //      PSAMControlLibrary.Key k = new PSAMControlLibrary.Key(-2);
      //      viewer.AddMusicalSymbol(k);
            
            TimeSignature t = new TimeSignature(TimeSignatureType.Common, 4, 4);
            viewer.AddMusicalSymbol(t);

            



            char[] cNote =  new char[1];
            
            cNote[0] = (char)('A' + iNote);

            string szNote = new String(cNote);

            Note n = new Note(szNote, 0, iOctave, MusicalSymbolDuration.Quarter,
                    NoteStemDirection.Up, NoteTieType.None,
                    new List<NoteBeamType>() { NoteBeamType.Single });

            n.MusicalCharacterColor = System.Drawing.Color.Red;
            viewer.AddMusicalSymbol(n);

            

            txtAnswer.Text = szNote;
        }

        private void btnNext_Click(object sender, RoutedEventArgs e)
        {
            SetRandomNote();

            PlayNote(iNote, iOctave);
            


        }

        private void CorrectGuess()
        {
            iGuesses++;
            iCorrect++;
            SetRandomNote();
            UpdateStats();

            PlayNote(iNote, iOctave);

            SetResult("Correct!");
        }

        private void WrongGuess()
        {
            iGuesses++;
            UpdateStats();

            PlayNote(iNote, iOctave);

            SetResult("try again");
        }

        private void btnA_Click(object sender, RoutedEventArgs e)
        {
            if (iNote == 0)
            {
                CorrectGuess();
            }
            else
            {
                WrongGuess();
            }
        }

        private void btnB_Click(object sender, RoutedEventArgs e)
        {
            if (iNote == 1)
            {
                CorrectGuess();
            }
            else
            {
                WrongGuess();
            }
        }

        private void btnC_Click(object sender, RoutedEventArgs e)
        {
            if (iNote == 2)
            {
                CorrectGuess();
            }
            else
            {
                WrongGuess();
            }
        }

        private void btnD_Click(object sender, RoutedEventArgs e)
        {
            if (iNote == 3)
            {
                CorrectGuess();
            }
            else
            {
                WrongGuess();
            }
        }

        private void btnE_Click(object sender, RoutedEventArgs e)
        {
            if (iNote == 4)
            {
                CorrectGuess();
            }
            else
            {
                WrongGuess();
            }
        }

        private void btnF_Click(object sender, RoutedEventArgs e)
        {
            if (iNote == 5)
            {
                CorrectGuess();
            }
            else
            {
                WrongGuess();
            }
        }

        private void btnG_Click(object sender, RoutedEventArgs e)
        {
            if (iNote == 6)
            {
                CorrectGuess();
            }
            else
            {
                WrongGuess();
            }
        }

        void UpdateStats()
        {
            txtGuessCount.Text = "Guesses: " + (iGuesses).ToString();
            txtCorrectCount.Text = "Correct: " + iCorrect.ToString();

            if(iGuesses == 0)
                txtPcnt.Text = String.Format("Percentage: {0:}%", 100); 
            else if(iCorrect == 0)
                String.Format("Percentage: {0:}%", 0); 
            else
                txtPcnt.Text = String.Format("Percentage: {0:}%", 100 * iCorrect / iGuesses); 
        }

        private void LayoutRoot_Loaded_1(object sender, RoutedEventArgs e)
        {
            UpdateStats();
        }

        private void PlayNote(int Note, int Octave)
        {
            int iRes = Globals.GetNoteRes(GetCorrectedNote(Note), Octave);
            string szWave = Globals.GetWave(iRes);

            Uri uriSound = new Uri(szWave, UriKind.Relative);

            StreamResourceInfo sri = App.GetResourceStream(uriSound);

            if (sri == null)
                return;

            var Player = new SoundPlayer(sri.Stream);

            Player.Play();
        }

        int GetCorrectedNote(int Note)
        {
            if (Note == 0)
            {
                return 0;

            }
            if (Note == 1)
            {
                return 2;

            }
            if (Note == 2)
            {
                return 3;

            }
            if (Note == 3)
            {
                return 5;

            }
            if (Note == 4)
            {
                return 7;

            }
            if (Note == 5)
            {
                return 8;

            }
            else
            {
                return 10;

            }
            


        }

        void SetResult(String szIn)
        {
            txtOutput.Text = szIn;

            //System.Windows.Threading.DispatcherTimer myTimer = new System.Windows.Threading.DispatcherTimer();
            myTimer.Interval = new TimeSpan(0, 0, 0, 0, 2000);
            myTimer.Tick += new EventHandler(ClearResult);
            myTimer.Start();
        }

        public void ClearResult(object o, EventArgs sender)
        {
            txtOutput.Text = "";
            myTimer.Stop();
        }
    }
}
