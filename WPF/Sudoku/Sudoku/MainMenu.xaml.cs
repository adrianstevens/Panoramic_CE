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
using System.Windows.Media.Animation;
using System.Collections.ObjectModel;
using System.Threading;

namespace Sudoku
{
    /// <summary>
    /// Interaction logic for MainMenu.xaml
    /// </summary>
    public partial class MainMenu : Page
    {
        protected enum EnumMenuState
        {
            SelectGame,
            NewOrCustom,
            Size,
            Difficulty,
            LoadGame,
            Count,
        };
        protected enum Directions
        {
            In,
            Out
        }

        private ObservableCollection<TitleItems> _arrItems;
        
        private EnumMenuState _eState;
        private FrameworkElement _visual;
        private bool _bHoldSelectedItem;
        private bool _bIsTrial;
        private Directions _direction;
        protected int _iDuration { get; set; }
        protected int _iAngle { get; set; }
        protected int _iFeatherDelay { get; set; }
        protected int _iInitialDelay { get; set; }

        public FrameworkElement RootElement { get; set; }

        public MainMenu()
        {
            InitializeComponent();

            _arrItems = new ObservableCollection<TitleItems>();

            
            _bHoldSelectedItem = false;
            _direction = Directions.In;
            RootElement = LayoutRoot;
            _iDuration = 350;
            _iAngle = -80;
            _iFeatherDelay = 50;
            _iInitialDelay = 0;



            listTitles.DataContext = _arrItems;
            listTitles.SelectedIndex = -1;


            txtTrial.Opacity = _bIsTrial ? 1.0 : 0.0;

            if (mediaMusic.Source == null)
                mediaMusic.Source = new Uri("./Assets/Waves/sudoku16-bar.mp3", UriKind.RelativeOrAbsolute);

            btnResume.Source = new BitmapImage(new Uri("../Assets/appbar_sudoku.png", UriKind.Relative));
        
        }

        void StartMusic()
        {
            StopMusic();

            if (Globals.Settings.bPlaySounds)
            {
                mediaMusic.Volume = 1.0;

               // Dispatcher.BeginInvoke(() =>
                {
                    if (mediaMusic.Source == null)
                        mediaMusic.Source = new Uri("./Assets/Waves/sudoku16-bar.mp3", UriKind.RelativeOrAbsolute);

                    mediaMusic.Position = TimeSpan.Zero;
                    mediaMusic.Play();

                }
                //);
            }


        }

        void StopMusic()
        {
            if(mediaMusic.HasAudio)
                mediaMusic.Stop();
        }

        string GetStats(EnumDifficulty eDiff, EnumSudokuType eType)
        {
            return "*" + Globals.Game.GetBestTimeString(eType, eDiff) +
                "* " + Globals.rm.GetString("IDS_MENU_Avg") + " " + Globals.Game.GetAvgTimeString(eType, eDiff)
                + " (" + Globals.Game.GetGameCount(eType, eDiff) + ")";

        }

        private void menuItemStats_Click(object sender, EventArgs e)
        {
            PopupMenu pop = new PopupMenu((FrameworkElement)this, Globals.rm.GetString("IDS_Menu_Stats"));

            string szTemp;

            szTemp = "*" + Globals.Game.GetBestTimeString(Engine.EnumSudokuType.STYPE_Sudoku, Engine.EnumDifficulty.DIFF_VeryEasy) +
                "* " + Globals.rm.GetString("IDS_MENU_Avg") + " " + Globals.Game.GetAvgTimeString(Engine.EnumSudokuType.STYPE_Sudoku, Engine.EnumDifficulty.DIFF_VeryEasy)
                + " (" + Globals.Game.GetGameCount(Engine.EnumSudokuType.STYPE_Sudoku, Engine.EnumDifficulty.DIFF_VeryEasy) + ")";

            pop.AddItem(Globals.rm.GetString("IDS_Sudoku") + ": " + Globals.rm.GetString("IDS_PREF_VeryEasy"), GetStats(EnumDifficulty.DIFF_VeryEasy, EnumSudokuType.STYPE_Sudoku));
            pop.AddItem(Globals.rm.GetString("IDS_Sudoku") + ": " + Globals.rm.GetString("IDS_PREF_Easy"), GetStats(EnumDifficulty.DIFF_Easy, EnumSudokuType.STYPE_Sudoku));
            pop.AddItem(Globals.rm.GetString("IDS_Sudoku") + ": " + Globals.rm.GetString("IDS_PREF_Normal"), GetStats(EnumDifficulty.DIFF_Medium, EnumSudokuType.STYPE_Sudoku));
            pop.AddItem(Globals.rm.GetString("IDS_Sudoku") + ": " + Globals.rm.GetString("IDS_PREF_Hard"), GetStats(EnumDifficulty.DIFF_Hard, EnumSudokuType.STYPE_Sudoku));
            pop.AddItem(Globals.rm.GetString("IDS_Sudoku") + ": " + Globals.rm.GetString("IDS_PREF_Expert"), GetStats(EnumDifficulty.DIFF_VeryHard, EnumSudokuType.STYPE_Sudoku));
            pop.AddItem(Globals.rm.GetString("IDS_Sudoku") + ": " + Globals.rm.GetString("IDS_PREF_Genius"), GetStats(EnumDifficulty.DIFF_Hardcore, EnumSudokuType.STYPE_Sudoku));

            pop.AddSeparator();

            pop.AddItem(Globals.rm.GetString("IDS_KillerSudoku") + ": " + Globals.rm.GetString("IDS_PREF_VeryEasy"), GetStats(EnumDifficulty.DIFF_VeryEasy, EnumSudokuType.STYPE_Killer));
            pop.AddItem(Globals.rm.GetString("IDS_KillerSudoku") + ": " + Globals.rm.GetString("IDS_PREF_Easy"), GetStats(EnumDifficulty.DIFF_Easy, EnumSudokuType.STYPE_Killer));
            pop.AddItem(Globals.rm.GetString("IDS_KillerSudoku") + ": " + Globals.rm.GetString("IDS_PREF_Normal"), GetStats(EnumDifficulty.DIFF_Medium, EnumSudokuType.STYPE_Killer));
            pop.AddItem(Globals.rm.GetString("IDS_KillerSudoku") + ": " + Globals.rm.GetString("IDS_PREF_Hard"), GetStats(EnumDifficulty.DIFF_Hard, EnumSudokuType.STYPE_Killer));
            pop.AddItem(Globals.rm.GetString("IDS_KillerSudoku") + ": " + Globals.rm.GetString("IDS_PREF_Expert"), GetStats(EnumDifficulty.DIFF_VeryHard, EnumSudokuType.STYPE_Killer));

            pop.AddSeparator();

            pop.AddItem(Globals.rm.GetString("IDS_Kakuro") + ": " + Globals.rm.GetString("IDS_PREF_VeryEasy"), GetStats(EnumDifficulty.DIFF_VeryEasy, EnumSudokuType.STYPE_Kakuro));
            pop.AddItem(Globals.rm.GetString("IDS_Kakuro") + ": " + Globals.rm.GetString("IDS_PREF_Easy"), GetStats(EnumDifficulty.DIFF_Easy, EnumSudokuType.STYPE_Kakuro));
            pop.AddItem(Globals.rm.GetString("IDS_Kakuro") + ": " + Globals.rm.GetString("IDS_PREF_Normal"), GetStats(EnumDifficulty.DIFF_Medium, EnumSudokuType.STYPE_Kakuro));
            pop.AddItem(Globals.rm.GetString("IDS_Kakuro") + ": " + Globals.rm.GetString("IDS_PREF_Hard"), GetStats(EnumDifficulty.DIFF_Hard, EnumSudokuType.STYPE_Kakuro));
            pop.AddItem(Globals.rm.GetString("IDS_Kakuro") + ": " + Globals.rm.GetString("IDS_PREF_Expert"), GetStats(EnumDifficulty.DIFF_VeryHard, EnumSudokuType.STYPE_Kakuro));

            pop.AddSeparator();

            pop.AddItem(Globals.rm.GetString("IDS_KenKen") + ": " + Globals.rm.GetString("IDS_PREF_VeryEasy"), GetStats(EnumDifficulty.DIFF_VeryEasy, EnumSudokuType.STYPE_KenKen));
            pop.AddItem(Globals.rm.GetString("IDS_KenKen") + ": " + Globals.rm.GetString("IDS_PREF_Easy"), GetStats(EnumDifficulty.DIFF_Easy, EnumSudokuType.STYPE_KenKen));
            pop.AddItem(Globals.rm.GetString("IDS_KenKen") + ": " + Globals.rm.GetString("IDS_PREF_Normal"), GetStats(EnumDifficulty.DIFF_Medium, EnumSudokuType.STYPE_KenKen));
            pop.AddItem(Globals.rm.GetString("IDS_KenKen") + ": " + Globals.rm.GetString("IDS_PREF_Hard"), GetStats(EnumDifficulty.DIFF_Hard, EnumSudokuType.STYPE_KenKen));
            pop.AddItem(Globals.rm.GetString("IDS_KenKen") + ": " + Globals.rm.GetString("IDS_PREF_Expert"), GetStats(EnumDifficulty.DIFF_VeryHard, EnumSudokuType.STYPE_KenKen));

            pop.AddSeparator();

            pop.AddItem(Globals.rm.GetString("IDS_Greater") + ": " + Globals.rm.GetString("IDS_PREF_VeryEasy"), GetStats(EnumDifficulty.DIFF_VeryEasy, EnumSudokuType.STYPE_Greater));
            pop.AddItem(Globals.rm.GetString("IDS_Greater") + ": " + Globals.rm.GetString("IDS_PREF_Easy"), GetStats(EnumDifficulty.DIFF_Easy, EnumSudokuType.STYPE_Greater));
            pop.AddItem(Globals.rm.GetString("IDS_Greater") + ": " + Globals.rm.GetString("IDS_PREF_Normal"), GetStats(EnumDifficulty.DIFF_Medium, EnumSudokuType.STYPE_Greater));
            pop.AddItem(Globals.rm.GetString("IDS_Greater") + ": " + Globals.rm.GetString("IDS_PREF_Hard"), GetStats(EnumDifficulty.DIFF_Hard, EnumSudokuType.STYPE_Greater));
            pop.AddItem(Globals.rm.GetString("IDS_Greater") + ": " + Globals.rm.GetString("IDS_PREF_Expert"), GetStats(EnumDifficulty.DIFF_VeryHard, EnumSudokuType.STYPE_Greater));

            pop.SetCurrentIndex(-1);
            pop.Launch();
        }

        private void listBox1_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (listTitles.SelectedIndex == -1)
                return;

            // don't allow selection
            TitleItems title = _arrItems[listTitles.SelectedIndex];
            if (title.bItemEnabled == false)
            {
                listTitles.SelectedIndex = -1;
                return;
            }

            switch (_eState)
            {
                case EnumMenuState.SelectGame:
                    // if there is a game in play then make sure to delete it
                    Globals.Game.SetIsGameInPlay(false);
                    Globals.Game.SetGameType((Engine.EnumSudokuType)listTitles.SelectedIndex);
                    switch (Globals.Game.GetGameType())
                    {
                        case Engine.EnumSudokuType.STYPE_Kakuro:
                            _eState = EnumMenuState.Size;
                            break;
                        default:
                            _eState = EnumMenuState.Difficulty;
                            break;
                    }
                    UpdateList();
                    break;
                case EnumMenuState.Size:
                    switch (listTitles.SelectedIndex)
                    {
                        case 0: //5x5
                            Globals.Game.SetBoardSize(Engine.EnumSize.KSIZE_5);
                            break;
                        case 1: //7x7
                            Globals.Game.SetBoardSize(Engine.EnumSize.KSIZE_7);
                            break;
                        case 2: //9x9
                        default:
                            Globals.Game.SetBoardSize(Engine.EnumSize.KSIZE_9);
                            break;
                    }
                    _eState = EnumMenuState.Difficulty;
                    UpdateList();
                    break;
                case EnumMenuState.Difficulty:
                    switch (listTitles.SelectedIndex)
                    {
                        case 0: // IDS_VeryEasy:
                            Globals.Game.SetDifficulty(Engine.EnumDifficulty.DIFF_VeryEasy);
                            break;
                        case 1: // IDS_Easy:
                            Globals.Game.SetDifficulty(Engine.EnumDifficulty.DIFF_Easy);
                            break;
                        case 2: //IDS_Medium:
                            Globals.Game.SetDifficulty(Engine.EnumDifficulty.DIFF_Medium);
                            break;
                        case 3: //IDS_Hard:
                            Globals.Game.SetDifficulty(Engine.EnumDifficulty.DIFF_Hard);
                            break;
                        case 4: //IDS_VeryHard:
                            Globals.Game.SetDifficulty(Engine.EnumDifficulty.DIFF_VeryHard);
                            break;
                        case 5: //IDS_Genius:
                        default:
                            Globals.Game.SetDifficulty(Engine.EnumDifficulty.DIFF_Hardcore);
                            break;
                    }
                    _eState = EnumMenuState.LoadGame;
                    UpdateList();
                    break;
            }
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            ctrlProgress.Opacity = 0.0;
            ctrlProgress.Visibility = Visibility.Collapsed;
            ctrlProgress.IsEnabled = false;
            txtLoading.Opacity = 0.0;

            txtTrial.Opacity = _bIsTrial ? 1.0 : 0.0;

            // localized menu
          /*  ((ApplicationBarIconButton)this.ApplicationBar.Buttons[0]).Text = Globals.rm.GetString("IDS_MENU_Resume");

            ((ApplicationBarMenuItem)this.ApplicationBar.MenuItems[0]).Text = Globals.rm.GetString("IDS_MENU_Options");
            ((ApplicationBarMenuItem)this.ApplicationBar.MenuItems[1]).Text = Globals.rm.GetString("IDS_MENU_Stats");
            ((ApplicationBarMenuItem)this.ApplicationBar.MenuItems[2]).Text = Globals.rm.GetString("IDS_MENU_About");
            ((ApplicationBarMenuItem)this.ApplicationBar.MenuItems[3]).Text = Globals.rm.GetString("IDS_MENU_MoreApps"); */

            _eState = EnumMenuState.SelectGame;

            UpdateList();

            StartMusic();
        }

        private void UpdateList()
        {
            if (_arrItems.Count > 0)
            {
                // animate out

                _arrItems.Clear();
            }

            switch (_eState)
            {
                case EnumMenuState.SelectGame:
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_Sudoku"), true));
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_KillerSudoku"), _bIsTrial ? false : true));
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_Kakuro"), true));
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_KenKen"), _bIsTrial ? false : true));
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_Greater"), _bIsTrial ? false : true));


                    break;
                case EnumMenuState.Difficulty:
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_PREF_VeryEasy"), true));
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_PREF_Easy"), _bIsTrial ? false : true));
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_PREF_Normal"), _bIsTrial ? false : true));
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_PREF_Hard"), _bIsTrial ? false : true));
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_PREF_Expert"), _bIsTrial ? false : true));

                    if (Globals.Game.GetGameType() == Engine.EnumSudokuType.STYPE_Sudoku)
                        _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_PREF_Genius"), _bIsTrial ? false : true));
                    break;
                case EnumMenuState.Size:
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_PREF_5x5"), true));
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_PREF_7x7"), _bIsTrial ? false : true));
                    _arrItems.Add(new TitleItems(Globals.rm.GetString("IDS_PREF_9x9"), _bIsTrial ? false : true));
                    break;
                case EnumMenuState.LoadGame:
                    ctrlProgress.IsEnabled = true;
                    ctrlProgress.Visibility = Visibility.Visible;
                    ctrlProgress.Opacity = 1.0;
                    txtLoading.Opacity = 1.0;

                    ThreadPool.QueueUserWorkItem(o =>
                    {
                        Thread.Sleep(500);
                        Globals.Game.NewGame();

                      //  Dispatcher.BeginInvoke(() =>

                        Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Normal,
                            new Action(delegate()
                        {
                            ctrlProgress.Opacity = 0.0;
                            ctrlProgress.Visibility = Visibility.Collapsed;
                            ctrlProgress.IsEnabled = false;
                            txtLoading.Opacity = 0.0;

                            NavigationService.Navigate(new Uri("/GameScreen.xaml", UriKind.Relative));
                        }));
                        

                        // NOTICE THIS LINE!!!  
                        Thread.Sleep(0);
                    });

                    //Dispatcher.BeginInvoke(() =>
                    //{
                    //Thread thread = new Thread(LoadNewGame);
                    //thread.Start();
                    //thread.Join();
                    //NavigationService.Navigate(new Uri("/Screens/GameScreen.xaml", UriKind.Relative));
                    //});
                    break;
            }

            listTitles.SelectedIndex = -1; // no selection

            // animate in
            /*Dispatcher.BeginInvoke(() =>
            {
                _bHoldSelectedItem = false;
                _direction = Directions.In;
                RootElement = LayoutRoot;
                _iDuration = 350;
                _iAngle = -80;
                _iFeatherDelay = 50;
                _iInitialDelay = 0;

                //Animation();
            });*/


            if (_eState == EnumMenuState.SelectGame)
            {
            //    ((ApplicationBarIconButton)this.ApplicationBar.Buttons[0]).IsEnabled = Globals.Game.GetIsGameInPlay(); // resume 
            }
            else if (_eState == EnumMenuState.LoadGame)
            {
            //    ((ApplicationBarIconButton)this.ApplicationBar.Buttons[0]).IsEnabled = false; // resume disabled
            }
            else
            {
            //    ((ApplicationBarIconButton)this.ApplicationBar.Buttons[0]).IsEnabled = false; // resume disabled
            }
        }

        private void Animation()
        {
        
        }

        bool IsOnCurrentPage(ListBoxItem item)
        {
            return true;
            /*    var itemsHostRect = Rect.Empty;
                var listBoxItemRect = Rect.Empty;

                if (_visual == null)
                {
                    ItemsControlHelper ich = new ItemsControlHelper(listTitles);
                    ScrollContentPresenter scp = ich.ScrollHost == null ? null : ich.ScrollHost.GetVisualDescendants().OfType<ScrollContentPresenter>().FirstOrDefault();
                    _visual = (ich.ScrollHost == null) ? null : ((scp == null) ? ((FrameworkElement)ich.ScrollHost) : ((FrameworkElement)scp));
                }

                if (_visual == null)
                    return true;

                itemsHostRect = new Rect(0.0, 0.0, _visual.ActualWidth, _visual.ActualHeight);
                //ListBoxItem item = ListBox.ItemContainerGenerator.ContainerFromIndex(index) as ListBoxItem;
                if (item == null)
                {
                    listBoxItemRect = Rect.Empty;
                    return false;
                }

                GeneralTransform transform = item.TransformToVisual(_visual);
                listBoxItemRect = new Rect(transform.Transform(new Point()), transform.Transform(new Point(item.ActualWidth, item.ActualHeight)));

                return ((listBoxItemRect.Bottom + 100 >= itemsHostRect.Top) && (listBoxItemRect.Top - 100 <= itemsHostRect.Bottom));
                //return ((itemsHostRect.Top <= listBoxItemRect.Bottom) && (listBoxItemRect.Top <= itemsHostRect.Bottom));
             */
        }

        private void btnResume_OnClick(object sender, MouseButtonEventArgs e)
        {
            if (Globals.Settings.bIsGameInPlay)
            {
                _arrItems.Clear();
                ctrlProgress.IsEnabled = true;
                ctrlProgress.Visibility = Visibility.Visible;
                ctrlProgress.Opacity = 1.0;
                txtLoading.Opacity = 1.0;

                ThreadPool.QueueUserWorkItem(o =>
                {
                    Thread.Sleep(500);
                    Globals.Game.LoadGameState();

                    ctrlProgress.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new Action(
                        delegate()
                        {
                            ctrlProgress.Opacity = 0.0;
                            ctrlProgress.Visibility = Visibility.Collapsed;
                            ctrlProgress.IsEnabled = false;
                            txtLoading.Opacity = 0.0;
                        }
                    ));
                    // NOTICE THIS LINE!!!  
                    Thread.Sleep(0);
                });
            }
            NavigationService.Navigate(new Uri("GameScreen.xaml", UriKind.Relative));
        }
    }


    public class TitleItems
    {
        public string szTitle { get; set; }
        public Brush brColor { get; set; }
        public bool bItemEnabled { get; set; }

        public TitleItems(string szMyTitle, bool bEnabled)
        {
            szTitle = szMyTitle;
            bItemEnabled = bEnabled;

            if (bEnabled)
                brColor = Globals.brush.GetBrush(Colors.Black);
            else
                brColor = Globals.brush.GetBrush(Colors.Gray);
        }
    }
}
