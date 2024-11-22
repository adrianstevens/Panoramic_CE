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
using Sudoku.Engine;
using System.Media;
using System.IO;
using System.Windows.Resources;

namespace Sudoku
{
    /// <summary>
    /// Interaction logic for GameScreen.xaml
    /// </summary>
    public partial class GameScreen : Page
    {
        const string SOUND_FIREWORKS = "./Assets/Waves/fireworks-pop.wav";
        const string SOUND_HINT = "./Assets/Waves/hint.wav";
        const string SOUND_UNDO = "./Assets/Waves/undo.wav";
        const string SOUND_MENU = "./Assets/Waves/tick.wav";
        const string SOUND_SET = "./Assets/Waves/beep1.wav";
        const string SOUND_UNSET = "./Assets/Waves/beep2.wav";
        const string SOUND_GAMESTART = "./Assets/Waves/sudoku_start.wav";
        const string SOUND_SILENCE = "./Assets/Waves/silence.wav";

        private IScreenGame _objGameScreen;
        private List<NumberSelector> _arrSelectors;

        public delegate void MyHandler(object sender, BoardTileEventArgs e);


        private Visual.Fireworks fire = null;

        private bool bPen = true;
        private int _iSelectedNumber = -1;

        System.Windows.Threading.DispatcherTimer GameTimer = null;
        System.Windows.Threading.DispatcherTimer FireTimer = null;
        int iFireCount = 0;

        Random rand = new Random(System.DateTime.Today.Minute + System.Environment.TickCount);

        //SoundEffectInstance sfxSilence = null;

        void NewGame()
        {
            Globals.Game.NewGame();
            gridBoard.Opacity = 1.0;
            StartGameTimer();
            StopFireTimer();//should never be needed
            UpdateFreeCells();
            _objGameScreen.ResetBoard(ref gridBoard, ref gridBoardOverlay);
            _objGameScreen.UpdateOverlay(ref gridBoard, ref gridBoardOverlay);
            //OnUpdateAll(this, new EventArgs());

            PlaySound(SOUND_GAMESTART);
        }


        public GameScreen()
        {
            InitializeComponent();

            _arrSelectors = new List<NumberSelector>();
        }

        void UpdateFreeCells()
        {
            textFree.Text = Globals.rm.GetString("IDS_MENU_Free") + ": " + Globals.Game.GetCellsFree().ToString();
        }

        void StartGameTimer()
        {
            StopGameTimer();

            GameTimer = new System.Windows.Threading.DispatcherTimer();
            GameTimer.Interval = new TimeSpan(0, 0, 0, 0, 1000);
            GameTimer.Tick += new EventHandler(EverySecond);
            GameTimer.Start();
        }

        void StopGameTimer()
        {
            if (GameTimer != null)
            {
                GameTimer.Stop();
                GameTimer = null;
            }
        }

        void StartFireTimer()
        {
            StopFireTimer();
            iFireCount = 0;
            FireTimer = new System.Windows.Threading.DispatcherTimer();
            FireTimer.Interval = new TimeSpan(0, 0, 0, 0, 1100);
            FireTimer.Tick += new EventHandler(LaunchFirework);
            FireTimer.Start();
            System.Diagnostics.Debug.WriteLine("StartFireTimer");
        }

        void StopFireTimer()
        {
            if (FireTimer != null)
            {
                FireTimer.Stop();
                FireTimer = null;
            }

            System.Diagnostics.Debug.WriteLine("StopFireTimer");
        }

        public void EverySecond(object o, EventArgs sender)
        {
            Globals.Game.IncGameTimer();
            int iMin = Globals.Game.GetGameTimer() / 60;
            int iSec = Globals.Game.GetGameTimer() % 60;

            textTime.Text = String.Format("{0:00}", iMin) + ":" + String.Format("{0:00}", iSec);
        }

        public void LaunchFirework(object o, EventArgs sender)
        {
            if (iFireCount > 3)
            {
                if (fire.IsComplete() == true)
                {
                    iFireCount = 0;//so this code won't run twice
                    StopFireTimer();
                    LayoutRoot.Children.Remove(fire);
                    fire = null;

                    string szGame = null;

                    switch (Globals.Game.GetGameType())
                    {
                        case EnumSudokuType.STYPE_Sudoku:
                            szGame = Globals.rm.GetString("IDS_Sudoku");
                            break;
                        case EnumSudokuType.STYPE_Killer:
                            szGame = Globals.rm.GetString("IDS_KillerSudoku");
                            break;
                        case EnumSudokuType.STYPE_Kakuro:
                            szGame = Globals.rm.GetString("IDS_Kakuro");
                            break;
                        case EnumSudokuType.STYPE_KenKen:
                            szGame = Globals.rm.GetString("IDS_KenKen");
                            break;
                        case EnumSudokuType.STYPE_Greater:
                        default:
                            szGame = Globals.rm.GetString("IDS_Greater");
                            break;
                    }

                    string szDif = null;

                    switch (Globals.Game.GetDifficulty())
                    {
                        case EnumDifficulty.DIFF_VeryEasy:
                            szDif = Globals.rm.GetString("IDS_PREF_VeryEasy");
                            break;
                        case EnumDifficulty.DIFF_Easy:
                            szDif = Globals.rm.GetString("IDS_PREF_Easy");
                            break;
                        case EnumDifficulty.DIFF_Medium:
                            szDif = Globals.rm.GetString("IDS_PREF_Normal");
                            break;
                        case EnumDifficulty.DIFF_Hard:
                            szDif = Globals.rm.GetString("IDS_PREF_Hard");
                            break;
                        case EnumDifficulty.DIFF_VeryHard:
                            szDif = Globals.rm.GetString("IDS_PREF_Expert");
                            break;
                        case EnumDifficulty.DIFF_Hardcore:
                            szDif = Globals.rm.GetString("IDS_PREF_Genius");
                            break;
                    }

                    string szMsg = Globals.rm.GetString("IDS_MSG_Congrats");
                    szMsg = String.Format(szMsg, szDif, szGame, textTime.Text);

                    if (MessageBoxResult.OK == MessageBox.Show(szMsg, Globals.rm.GetString("IDS_NAME_Sudoku"), MessageBoxButton.OKCancel))
                    {
                        NewGame();
                    }
                    else
                    {
                        NavigationService.GoBack();
                    }
                }
            }
            else if (fire != null && iFireCount <= 3)
            {
                iFireCount++;
                PlaySound(SOUND_FIREWORKS);
                fire.addFirework(120.0 + rand.NextDouble() * 240.0, 100.0 + rand.NextDouble() * 240.0);
                System.Diagnostics.Debug.WriteLine("FireCount++");
            }
        }

        public static void PlaySound(string res)
        {
            if (Globals.Settings.bPlaySounds == false)
                return;

            Uri uriSound = new Uri(@res, UriKind.Relative);
          //  Uri uriSound = new Uri(@"./Assets/Waves/sudoku_start.wav", UriKind.Relative);

            StreamResourceInfo sri = App.GetResourceStream(uriSound);

            if (sri == null)
                return;

            var Player = new SoundPlayer(sri.Stream);

           // var player = new SoundPlayer(res);



            Player.Play();

          /*  SoundEffect sfx = null;
            SoundEffectInstance sfxInstance = null;

            sfx = SoundEffect.FromStream(TitleContainer.OpenStream(res));
            sfxInstance = sfx.CreateInstance();
            //  FrameworkDispatcher.Update();
            sfxInstance.Play();
            FrameworkDispatcher.Update();*/
        }

        private void LayoutRoot_KeyDown(object sender, KeyEventArgs e)
        {
            switch(e.ImeProcessedKey)
            {
                case Key.NumPad1:
                    
                    break;
                case Key.NumPad2:

                    break;
                case Key.NumPad3:
                    
                    break;
                case Key.NumPad4:
                    
                    break;
                case Key.NumPad5:
                    
                    break;
                case Key.NumPad6:
                    
                    break;
                case Key.NumPad7:
                    
                    break;
                case Key.NumPad8:
                    
                    break;
                case Key.NumPad9:
                    
                    break;
            }
        }

        private void gridNumbers_MouseEnter(object sender, MouseEventArgs e)
        {
            //System.Diagnostics.Debug.WriteLine("MouseEnter - ");

            
        }

        private void gridNumbers_MouseLeave(object sender, MouseEventArgs e)
        {
            //System.Diagnostics.Debug.WriteLine("MouseLeave - ");

            _iSelectedNumber = -1;

            for (int i = gridNumbers.Children.Count - 1; i >= 0; i--)
            {
                NumberSelector n = (NumberSelector)gridNumbers.Children[i];

                n.MouseLeave();
            }
        }

        private void gridNumbers_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            //System.Diagnostics.Debug.WriteLine("MouseLeftButtonUp - ");

            if (_iSelectedNumber == -1)
                return;

            //NumberSelector n = (NumberSelector)gridNumbers.Children[_iSelectedNumber];

            //n.MouseLeftButtonUp();

            System.Windows.Point offset = e.GetPosition(this);

            double dbNumberSize = gridNumbers.ActualWidth / 9;

            bool bFound = false;
            for (int i = gridNumbers.Children.Count - 1; i >= 0; i--)
            {
                NumberSelector n = (NumberSelector)gridNumbers.Children[i];

                if (!bFound && offset.X > i * dbNumberSize)
                {
                    _iSelectedNumber = i;
                    bFound = true;
                    n.MouseLeftButtonUp();
                }
                else
                    n.MouseLeave();
            }

        }

        private void gridNumbers_MouseMove(object sender, MouseEventArgs e)
        {
            //System.Diagnostics.Debug.WriteLine("MouseMove - ");

            System.Windows.Point offset = e.GetPosition(this);

            double dbNumberSize = gridNumbers.ActualWidth / 9;

            bool bFound = false;
            for (int i = gridNumbers.Children.Count - 1; i >= 0; i--)
            {
                NumberSelector n = (NumberSelector)gridNumbers.Children[i];

                if (!bFound && offset.X > i * dbNumberSize)
                {
                    bFound = true;
                    if (_iSelectedNumber != i)
                    {
                        _iSelectedNumber = i;
                        n.MouseEnter();
                    }
                }
                else
                    n.MouseLeave();
            }
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            // this should only happen after a tombstone
            if (Globals.Game.GetIsBoardSet() == false)
            {
                if (Globals.Settings.bIsGameInPlay)
                    Globals.Game.LoadGameState();
                else
                    Globals.Game.NewGame();
            }

            //menu button images
            btnUndo.Source = new BitmapImage(new Uri(@"../Assets/appbar.undo.png", UriKind.Relative));
            btnHint.Source = new BitmapImage(new Uri(@"../Assets/appbar.hint.png", UriKind.Relative));
            btnPenPencil.Source = new BitmapImage(new Uri(@"../Assets/appbar.pen.png", UriKind.Relative));
            btnMenu.Source = new BitmapImage(new Uri(@"../Assets/appbar_listmenu.png", UriKind.Relative));


            imgLogo.Source = new BitmapImage(new Uri(@"../Assets/Sudoku_logo.png", UriKind.Relative));

   /*         ImageBrush i = new ImageBrush();
            i.Stretch = Stretch.UniformToFill;
            i.ImageSource = new BitmapImage(new Uri("Assets/OptionsBGsm.png", UriKind.Relative));

            LayoutRoot.Background = i;

            */

            InitializeBoard();
        }

        void AnimateTiles()
        {


        }

        void OnUpdateTile(object sender, BoardTileEventArgs e)
        {
            //lets update the selected tile 
            int i = e.x + e.y * Globals.Game.GetGridSize();
            BoardPiece b = (BoardPiece)gridBoard.Children[i];

            if (bPen == true)
            {
                int iGuess = Globals.Game.GetGuess(e.x, e.y);
                if (iGuess > -1)
                {
                    //bigger update as needed
                    if (Globals.Settings.bAutoPencil || Globals.Settings.bAutoRemovePencil)
                        _objGameScreen.ResetBoard(ref gridBoard, ref gridBoardOverlay);
                    else
                        b.UpdateGuess(iGuess);

                    if (iGuess != 0)
                        PlaySound(SOUND_SET);
                    else
                        PlaySound(SOUND_UNSET);

                }
                else
                {

                }
            }
            else
            {
                int iPencil = Globals.Game.GetPencilMarks(e.x, e.y);
                b.UpdatePencilMarks(iPencil);
            }
            UpdateTotals();//for kenken and killer
            UpdateFreeCells();
        }

        void UpdateTotals()
        {
            if (Globals.Game.GetGameType() != EnumSudokuType.STYPE_KenKen &&
                Globals.Game.GetGameType() != EnumSudokuType.STYPE_Killer)
                return;

            for (int i = 0; i < gridBoard.Children.Count; i++)
            {
                BoardPiece b = (BoardPiece)gridBoard.Children[i];

                b.UpdateGroupTotal(Globals.Game.GetTotal(b.iX, b.iY), Globals.Game.DrawTotal(b.iX, b.iY));
                if (Globals.Game.GetGameType() == EnumSudokuType.STYPE_KenKen)
                {

                    b.UpdateOperator(Globals.Game.GetKenKenOp(b.iX, b.iY), Globals.Game.DrawTotal(b.iX, b.iY));
                }
            }
        }

        void OnGameComplete(object sender, EventArgs e)
        {
            //let's try some fireworks
            if (fire == null)
            {
                fire = new Visual.Fireworks();
                Grid.SetRowSpan(fire, 4);
                LayoutRoot.Children.Add(fire);

                fire.Start();
            }

            gridBoard.Opacity = 0;
            StopGameTimer();
            StartFireTimer();
        }

        void OnUpdateAll(object sender, EventArgs e)
        {
            for (int i = 0; i < gridBoard.Children.Count; i++)
            {
                if (gridBoard.Children[i].GetType() != typeof(BoardPiece))
                    continue;

                BoardPiece b = (BoardPiece)gridBoard.Children[i];
                b.UpdateBackground();
            }
            UpdateFreeCells();
        }

        void OnSelectionChanged(object sender, BoardTileEventArgs e)
        {
            int iSize = (int)Math.Sqrt(gridBoard.Children.Count);

            Globals.Settings.iSelectorX = e.x;
            Globals.Settings.iSelectorY = e.y;

            // update the number selectors
            for (int i = 0; i < gridNumbers.Children.Count; i++)
            {
                NumberSelector s = (NumberSelector)gridNumbers.Children[i];
                s.UpdateNumber();
            }

            // update the board with the new selection states
            for (int i = 0; i < gridBoard.Children.Count; i++)
            {
                if (gridBoard.Children[i].GetType() != typeof(BoardPiece))
                    continue;

                BoardPiece b = (BoardPiece)gridBoard.Children[i];

                if ((e.x == i % iSize) &&
                    (e.y == i / iSize))
                {
                    b.UpdateSelector(EnumSelect.Selected);
                }
                else
                {
                    b.UpdateBackground();
                    //b.UpdateSelector(EnumSelect.None);
                }
            }

            // update the totals display
            if (Globals.Game.GetGameType() == EnumSudokuType.STYPE_Killer)
                textTotals.Text = Globals.rm.GetString("IDS_MENU_Total") + ": " + Globals.Game.GetTotal(e.x, e.y).ToString();
            else if (Globals.Game.GetGameType() == EnumSudokuType.STYPE_KenKen)
            {
                string szTotal = null;
                //if (Globals.Game.GetGameType() == EnumSudokuType.STYPE_KenKen)
                {
                    szTotal = Globals.Game.GetTotal(e.x, e.y).ToString();
                    switch (Globals.Game.GetKenKenOp(e.x, e.y))
                    {
                        case Engine.EnumOperator.OP_Divide:
                            szTotal += "÷";
                            break;
                        case Engine.EnumOperator.OP_Minus:
                            szTotal += "-";
                            break;
                        case Engine.EnumOperator.OP_Plus:
                            szTotal += "+";
                            break;
                        case Engine.EnumOperator.OP_Times:
                            szTotal += "×";
                            break;
                        case Engine.EnumOperator.OP_None:
                        default:
                            break;
                    }
                }
                textTotals.Text = szTotal;
            }
        }

        private void InitializeBoard()
        {
            //PlaySilence();

            double dbGridsize = gridBoard.Height / Globals.Game.GetGridSize();

            if (Globals.Game.GetGameType() == EnumSudokuType.STYPE_Sudoku)
                dbGridsize -= 3;//don't forget the extra gaps
            else
                dbGridsize -= 1;

            Globals.gui.LoadSkin(Globals.Settings.eSkin, Globals.Settings.eBackground, false, dbGridsize);

            // set the background here
            Globals.gui.LoadBackground(ref LayoutRoot, Globals.Game.GetGameType());

            
            // set up our callback

            //he.SampleEvent += new EventHandler<MyEventArgs>(SampleEventHandler);

            Globals.Game.UpdateTile += new EventHandler<BoardTileEventArgs>(OnUpdateTile);
//            Globals.Game.UpdateTile = (new EventHandler<BoardTileEventArgs>(OnUpdateTile));
      //      Globals.Game.UpdateTile += OnUpdateTile;
            Globals.Game.UpdateAll += new EventHandler<EventArgs>(OnUpdateAll);
      //      Globals.Game.UpdateAll += OnUpdateAll;
      //      Globals.Game.GameComplete += OnGameComplete;
            Globals.Game.GameComplete += new EventHandler<EventArgs>(OnGameComplete);

            switch (Globals.Game.GetGameType())
            {
                case EnumSudokuType.STYPE_Sudoku:
                    _objGameScreen = new ScreenSudoku();
                    break;
                case EnumSudokuType.STYPE_Killer:
                    _objGameScreen = new ScreenKillerSudoku();
                    break;
                case EnumSudokuType.STYPE_KenKen:
                    _objGameScreen = new ScreenKenKen();
                    break;
                case EnumSudokuType.STYPE_Kakuro:
                    _objGameScreen = new ScreenKakuro();
                    break;
                case EnumSudokuType.STYPE_Greater:
                    _objGameScreen = new ScreenGreaterThan();
                    break;
                default:
                    _objGameScreen = new ScreenSudoku();//should never get here of course
                    break;
            }

            // clear the board
            gridBoard.Children.Clear();
            gridBoard.ColumnDefinitions.Clear();
            gridBoard.RowDefinitions.Clear();

            // initialize our board
            _objGameScreen.Initialize(ref gridBoard, ref gridBoardOverlay);

            // update the board and also set up the callbacks
            for (int i = 0; i < gridBoard.Children.Count; i++)
            {
                if (gridBoard.Children[i].GetType() != typeof(BoardPiece))
                    continue;

                BoardPiece b = (BoardPiece)gridBoard.Children[i];



                b.SelectionChanged += OnSelectionChanged;

                b.SetGridSize(dbGridsize);
                b.imgBackground.Source = Globals.gui.sSkin.imgCellArray;
                b.imgBackground.Width = (int)(Globals.gui.sSkin.imgCellArray.PixelWidth * dbGridsize / Globals.gui.sSkin.imgCellArray.PixelHeight);
                b.imgBackground.Height = dbGridsize;
                b.imgBackground2.Source = Globals.gui.sSkin.imgCellArray2;
                b.imgBackground2.Width = (int)(Globals.gui.sSkin.imgCellArray2.PixelWidth * dbGridsize / Globals.gui.sSkin.imgCellArray2.PixelHeight);
                b.imgBackground2.Height = dbGridsize;
                b.imgFont1.Source = Globals.gui.sSkin.imgFont1;
                b.imgFont2.Source = Globals.gui.sSkin.imgFont2;
                b.UpdateItem();
            }

            //// Bottom number creation to save time and memory
            BitmapImage imgWhite = new BitmapImage(new Uri(@"/Assets/ButtonFont_White.png", UriKind.Relative));
            BitmapImage imgGrey = new BitmapImage(new Uri(@"/Assets/ButtonFont_Grey.png", UriKind.Relative));
            BitmapImage imgNumberBackground = new BitmapImage(new Uri(@"/Assets/button.png", UriKind.Relative));

            gridNumbers.ColumnDefinitions.Clear();
            gridNumbers.Children.Clear();

            // first set our columns
            for (int i = 0; i < 9; i++)
            {
                ColumnDefinition n = new ColumnDefinition();
                gridNumbers.ColumnDefinitions.Add(n);
            }

            // animate the numbers
            for (int i = 0; i < 9; i++)
            {
                NumberSelector num = new NumberSelector();

                num.imgNumbersWhite.Source = imgWhite;
                num.imgNumbersGrey.Source = imgGrey;
                num.imgButton.Source = imgNumberBackground;

                num.HorizontalAlignment = HorizontalAlignment.Stretch;
                num.VerticalAlignment = VerticalAlignment.Bottom;
                num.Height = 175;

                Grid.SetColumn(num, i);
                num.Margin = new Thickness(0, 0, 0, -77);
                gridNumbers.Children.Add(num);

                num.SetNumber(i);
                num.AnimateUpDown(i * 50); // delay 50 ms

                int iTemp = i;//needs its own int memory per control ... I think .. anyways its needed

                num.Selected += delegate(Object o, EventArgs ee)
                {
                    // a number has been selected
                    if (bPen)
                        Globals.Game.OnSetGuess(Globals.Settings.iSelectorX, Globals.Settings.iSelectorY, iTemp + 1);
                    else
                        Globals.Game.OnSetPencilMark(Globals.Settings.iSelectorX, Globals.Settings.iSelectorY, iTemp + 1);
                };

            }

            string szGame = null;

            switch (Globals.Game.GetGameType())
            {
                case EnumSudokuType.STYPE_Sudoku:
                    szGame = Globals.rm.GetString("IDS_Sudoku");
                    break;
                case EnumSudokuType.STYPE_Killer:
                    szGame = Globals.rm.GetString("IDS_KillerSudoku");
                    break;
                case EnumSudokuType.STYPE_Kakuro:
                    szGame = Globals.rm.GetString("IDS_Kakuro");
                    break;
                case EnumSudokuType.STYPE_KenKen:
                    szGame = Globals.rm.GetString("IDS_KenKen");
                    break;
                case EnumSudokuType.STYPE_Greater:
                default:
                    szGame = Globals.rm.GetString("IDS_Greater");
                    break;
            }

            string szDif = null;

            switch (Globals.Game.GetDifficulty())
            {
                case EnumDifficulty.DIFF_VeryEasy:
                    szDif = Globals.rm.GetString("IDS_PREF_VeryEasy");
                    break;
                case EnumDifficulty.DIFF_Easy:
                    szDif = Globals.rm.GetString("IDS_PREF_Easy");
                    break;
                case EnumDifficulty.DIFF_Medium:
                    szDif = Globals.rm.GetString("IDS_PREF_Normal");
                    break;
                case EnumDifficulty.DIFF_Hard:
                    szDif = Globals.rm.GetString("IDS_PREF_Hard");
                    break;
                case EnumDifficulty.DIFF_VeryHard:
                    szDif = Globals.rm.GetString("IDS_PREF_Expert");
                    break;
                case EnumDifficulty.DIFF_Hardcore:
                    szDif = Globals.rm.GetString("IDS_PREF_Genius");
                    break;
            }

            //set the game difficulty
            textLevel.Text = szGame + ": " + szDif;

            // totals
            textTotals.Text = "";


            StartGameTimer();
            UpdateFreeCells();

            PlaySound(SOUND_GAMESTART);
        }

        private void btnUndo_OnClick(object sender, MouseButtonEventArgs e)
        {
            if (Globals.Game.Undo())
                PlaySound(SOUND_UNDO);
            UpdateFreeCells();
        }

        private void btnHint_OnClick(object sender, MouseButtonEventArgs e)
        {
            if (bPen == false)
                btnPenPencil_OnClick(sender, e);

            if (Globals.Game.Hint())
            {
                OnSelectionChanged(sender, new BoardTileEventArgs() { x = Globals.Game.GetLastHint().x, y = Globals.Game.GetLastHint().y });
                PlaySound(SOUND_HINT);
            }
            UpdateFreeCells();
        }

        private void btnPenPencil_OnClick(object sender, MouseButtonEventArgs e)
        {
            bPen = !bPen;

            //and toggle the appbar icons ....
            if (bPen)
            {
                btnPenPencil.Source = new BitmapImage(new Uri(@"../Assets/appbar.pen.png", UriKind.Relative));
            }
            else
            {
                btnPenPencil.Source = new BitmapImage(new Uri(@"../Assets/appbar.pencil.png", UriKind.Relative));

            }
            PlaySound(SOUND_MENU);
        }

        private void btnMenu_OnClick(object sender, MouseButtonEventArgs e)
        {
            
        }

        private void MenuItem_ClickNewGame(object sender, RoutedEventArgs e)
        {
            if (MessageBoxResult.OK == MessageBox.Show(Globals.rm.GetString("IDS_MSG_NewGame"), Globals.rm.GetString("IDS_NAME_Sudoku"), MessageBoxButton.OKCancel))
            {
                NewGame();
            }
        }

        private void MenuItem_ClickRestart(object sender, RoutedEventArgs e)
        {
            if (MessageBoxResult.OK == MessageBox.Show(Globals.rm.GetString("IDS_MSG_RestartGame"), Globals.rm.GetString("IDS_NAME_Sudoku"), MessageBoxButton.OKCancel))
            {
                Globals.Game.RestartGame();

                _objGameScreen.ResetBoard(ref gridBoard, ref gridBoardOverlay);

                PlaySound(SOUND_GAMESTART);
            }
        }

        private void MenuItem_ClickSolve(object sender, RoutedEventArgs e)
        {
            Globals.Game.Solve();
            StopGameTimer();
            _objGameScreen.ResetBoard(ref gridBoard, ref gridBoardOverlay);
        }

        private void MenuItem_ClickClearPencil(object sender, RoutedEventArgs e)
        {
            Globals.Game.ClearPencilMarks();
            _objGameScreen.ResetBoard(ref gridBoard, ref gridBoardOverlay);
        }

        private void MenuItem_ClickOptions(object sender, RoutedEventArgs e)
        {
            NavigationService.Navigate(new Uri("/Options.xaml", UriKind.Relative));
        }


    }
}
