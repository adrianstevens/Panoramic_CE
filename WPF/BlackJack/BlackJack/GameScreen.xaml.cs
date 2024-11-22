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
using System.Windows.Media.Animation;
using System.Globalization;
using BlackJack.Controls;
using BlackJack.Objects;
using System.Windows.Threading;
using System.Diagnostics;

namespace BlackJack
{
    /// <summary>
    /// Interaction logic for GameScreen.xaml
    /// </summary>
    public partial class GameScreen : Page
    {
        const double HAND_INDENT = 40;

        static readonly string CURRENCY_FORMAT = string.Format("{0}#,##0;({0}#,##0);{0}0", RegionInfo.CurrentRegion.CurrencySymbol);

        int _dealerCount; // cards visibile for the dealer

        bool _options; // have we shown the options screen?
        bool _gameover; //from game over screen

        private bool _blackJack; // have we gotten a blackjack?
        Storyboard _plrResultAni;
        Storyboard _splitResultAni;
        Storyboard _cardCountAni;


   //     PlayerHand playerHand = new PlayerHand();
   //     PlayerHand splitHand = new PlayerHand();

        int iCurrentBet = 0;//used to compare to the engine so we know when to change the chips on screen

        private Visual.Fireworks fire = null;


        public GameScreen()
        {
            InitializeComponent();

            fire = new Visual.Fireworks();
            fire.IsHitTestVisible = false;

            //            int iIndex = gameBoard.Children.IndexOf(lblBet);

            //gameBoard.Children.Insert(0, fire);
            gameBoard.Children.Add(fire);
        }

        private void SetBankLabel(decimal value)
        {
            lblBank.Text = value.ToString(CURRENCY_FORMAT);
        }

        private void SetCountLabel()
        {
            if (Globals.Settings.CardCountView != EnumCardCountView.SemiHidden)
            {
                lblCountValue.Text = Globals.Game.GetCardCount().ToString();
                lblCount.Text = Globals.Res.GetString("IDS_INFO_Count");
            }
            else
            {
                var s = Globals.Res.GetString("IDS_INFO_CardCount");
                var values = s.Split(new string[] { "\\r\\n" }, StringSplitOptions.RemoveEmptyEntries);
                if (values.Length == 2)
                {
                    lblCountValue.Text = values[0];
                    lblCount.Text = values[1];
                }
            }
        }

        private void SetBetLabel(decimal value)
        {
            lblBet.Text = value.ToString(CURRENCY_FORMAT);
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            //LayoutRoot.Background = Globals.Gui.GetImageBrush(ref Globals.Gui.Skin.imgGameBackground);
            settingsButton1.Source = Globals.Gui.Skin.imgSettings;

            btnSplit.Text = Globals.Res.GetString("IDS_BTNS_Split");
            btnBet.Text = Globals.Res.GetString("IDS_BTNS_Bet");
            btnDeal.Text = Globals.Res.GetString("IDS_BTNS_Deal");
            btnHit.Text = Globals.Res.GetString("IDS_BTNS_Hit");
            btnDouble.Text = Globals.Res.GetString("IDS_BTNS_Double");
            btnReset.Text = Globals.Res.GetString("IDS_BTNS_Reset");
            btnSurrender.Text = Globals.Res.GetString("IDS_RESULT_Surrender");
            btnStand.Text = Globals.Res.GetString("IDS_BTNS_Stand");
            var s = Globals.Res.GetString("IDS_INFO_CardCount");
            var values = s.Split(new string[] { "\\r\\n" }, StringSplitOptions.RemoveEmptyEntries);
            if (values.Length > 1)
                lblCount.Text = values[1];

            lblAchivementComplete.Text = Globals.Res.GetString("IDS_LBL_AchievementUnlock");

            btn100.Text = 100.ToString(CURRENCY_FORMAT);
            btn25.Text = 25.ToString(CURRENCY_FORMAT);
            btn50.Text = 50.ToString(CURRENCY_FORMAT);
            btn500.Text = 500.ToString(CURRENCY_FORMAT);

            if (gridHints.Opacity > 0)
                gridHints.Opacity = Globals.Game.GameDifficulty == EnumGameDifficulty.Learn ? 1 : 0;

            Globals.Game.DealerFlip = OnDealerFlip;

            Globals.Game.EndGame = OnEndGame;

            Globals.Game.Results = OnResults;

            Globals.Game.Shuffle = OnShuffle;

            Globals.Settings.Achievements.OnCompleted = OnAchievement;

            SetBankLabel(Globals.Game.Bank);

            SetBetLabel(Globals.Game.BetAmount);


            if (Globals.Settings.CardCountView == EnumCardCountView.Off)
            {
                gridCount.Opacity = 0;
            }
            else
            {
                gridCount.Opacity = 1;
            }

            if (_gameover == true)
            {
                ResetTable();
            }
            else if (_options == false) // we're not coming back from the options screen (or the game over screen) so start resuming
            {
                // add chips to screen
                for (var i = 0; i < Globals.Game.Num25Chips; i++)
                    playerHand.Add25Chip();

                for (var i = 0; i < Globals.Game.Num50Chips; i++)
                    playerHand.Add50Chip();

                for (var i = 0; i < Globals.Game.Num100Chips; i++)
                    playerHand.Add100Chip();

                for (var i = 0; i < Globals.Game.Num500Chips; i++)
                    playerHand.Add500Chip();

                int delay = 0;

                // add dealer cards
                while (_dealerCount < Globals.Game.DealerHand.Count)
                {
                    var img = canvasDealer.Children[_dealerCount] as CardImage;

                    if (delay == 0 || Globals.Game.GameState == EnumGameState.EndOfGame || Globals.Game.GameDifficulty == EnumGameDifficulty.Easy)
                        img.Flip(canvasDealer, Globals.Game.DealerHand[_dealerCount], delay);
                    else
                    {
                        img.SetReversed(delay);
                        break;
                    }

                    delay += 600;

                    _dealerCount++;
                }

                // add player cards
                if (playerHand.FlipNewCards(Globals.Game.PlayerHand) && Globals.Game.ShowPlayerScore())
                    playerHand.SetScore(Globals.Game.PlayerHand);

                if (Globals.Game.IsSplit)
                {
                    // add split cards
                    if (splitHand.FlipNewCards(Globals.Game.SplitHand) && Globals.Game.ShowSplitScore())
                        splitHand.SetScore(Globals.Game.SplitHand);
                    splitHand.CopyChips(playerHand);
                    ToggleSelector();
                }

                // set menu bars
                switch (Globals.Game.GameState)
                {
                    case EnumGameState.PlayerFirstMove:
                    case EnumGameState.PlayerMove:
                    case EnumGameState.SplitMove:
                        {
                            var story = new Storyboard();

                            HideMainMenu(story);
                            HideBetBar(story);
                            ShowPlayBar(story);

                            story.Begin();
                        }
                        break;
                    case EnumGameState.EndOfGame:
                        SetResults(Globals.Game.LastResult);
                        if (Globals.Game.ShowDealerScore())
                            SetDealerScore(Globals.Game.DealerHand.GetScore());
                        break;
                }
            }
            else
            {
                _options = false;
            }

            SetCountLabel();
            UpdateButtons();

            //probably not needed but just in case
            _gameover = false;
        }

        private void OnDealerFlip(object o, EventArgs args)
        {
            SetCountLabel();

            FlipDealerCards();
        }

        private void ShowAchievement(Achievement a)
        {
            lblAchivementTitle.Text = a.Title;
            imgAchievementComplete.Source = a.Image;
            imgAchievementPoints.Source = a.PointImage;

            var story = new Storyboard();

            var ani = new DoubleAnimation
            {
                From = gridAchievementComplete.Opacity,
                To = 0.9,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            Storyboard.SetTarget(ani, gridAchievementComplete);
            Storyboard.SetTargetProperty(ani, new PropertyPath(Grid.OpacityProperty));

            story.Children.Add(ani);

            story.Completed += delegate(object o2, EventArgs args2)
            {
                HideAchievement(3);
                BJEngine.PlaySound(AppSettings.SOUND_ACHIEVE);
            };

            story.Begin();
        }

        private void HideAchievement(int delay)
        {
            var story2 = new Storyboard();

            var ani2 = new DoubleAnimation
            {
                From = gridAchievementComplete.Opacity,
                To = 0,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            Storyboard.SetTarget(ani2, gridAchievementComplete);
            Storyboard.SetTargetProperty(ani2, new PropertyPath(Grid.OpacityProperty));

            story2.Children.Add(ani2);

            story2.BeginTime = TimeSpan.FromSeconds(delay);

            story2.Begin();
        }

        private void OnAchievement(object o, EventArgs args)
        {
            Achievement a = o as Achievement;

            if (a == null) return;

            ShowAchievement(a);
        }

        
        private void ResetTable()
        {
            Globals.Game.NewGame(false);
            Globals.Game.LastResult = null;
            HideDealerScore();
            HideHints();
            foreach (CardImage c in canvasDealer.Children)
                c.Reset();
            playerHand.Reset();
            playerHand.ResetChips();
            playerHand.HideScore();
            splitHand.Hide();
            betBar.IsHitTestVisible = true;
            HideDealerScore();
            SetBetLabel(Globals.Game.BetAmount);
            SetBankLabel(Globals.Game.Bank);

            var story = new Storyboard();
            HidePlayBar(story);
            ShowMainMenu(story);
            ShowBetBar(story);
            story.Begin();
            HideResults();

            SetCountLabel();
            UpdateButtons();

            _gameover = false;

        }

        private void OnEndGame(object o, EventArgs args)
        {
            if (Globals.Game.GameState == EnumGameState.Bet ||
            Globals.Game.GameState == EnumGameState.EndOfGame)
            {
                if (Globals.Game.Bank /*+ Globals.Game.BetAmount*/ < 25)
                {
                    var bHighScore = false;

                    //Pop the end game message and handle
                    if (Globals.Game.BankHi > BJEngine.HISCORE_DEFAULT)
                    {
                        bHighScore = true;
                        var iBank = Globals.Game.BankHi;
                        Globals.Settings.HighScores.Add(null, Globals.Game.BankHi);
                    }

                    _gameover = true;

                    GameOverControl gameOver = new GameOverControl(this);

                    gameOver.Launch(ref LayoutRoot);
                    ResetTable();

                }
            }
        }

        private void ShowMainMenu(Storyboard story)
        {
            var dur = new Duration(TimeSpan.FromMilliseconds(400));

            var aniOpacity = new DoubleAnimation
            {
                From = 0.0,
                To = 0.7,
                Duration = dur
            };

            Storyboard.SetTarget(aniOpacity, mainMenu);
            Storyboard.SetTargetProperty(aniOpacity, new PropertyPath(Grid.OpacityProperty));

            story.Children.Add(aniOpacity);

            mainMenu.IsHitTestVisible = true;
        }

        private void ShowHint()
        {
            if (Globals.Game.GetDifficulty() == EnumGameDifficulty.Learn)
            {
                switch (Globals.Game.GetNextMove())
                {
                    case EnumNextMove.Hit:
                        lblHints.Text = Globals.Res.GetString("IDS_BTNS_Hit");
                        break;
                    case EnumNextMove.Stand:
                        lblHints.Text = Globals.Res.GetString("IDS_BTNS_Stand");
                        break;
                    case EnumNextMove.Split:
                        lblHints.Text = Globals.Res.GetString("IDS_BTNS_Split");
                        break;
                    case EnumNextMove.Surrender:
                        lblHints.Text = Globals.Res.GetString("IDS_BTNS_Late"); //surrender
                        break;
                    case EnumNextMove.Double:
                        lblHints.Text = Globals.Res.GetString("IDS_BTNS_Double");
                        break;
                    default:
                        return;
                }

                if (gridHints.Opacity == 0)
                {
                    var story = new Storyboard();

                    var ani = new DoubleAnimation
                    {
                        From = 0,
                        To = 1,
                        Duration = new Duration(TimeSpan.FromMilliseconds(400))
                    };

                    Storyboard.SetTarget(ani, gridHints);
                    Storyboard.SetTargetProperty(ani, new PropertyPath(Grid.OpacityProperty));

                    story.Children.Add(ani);

                    story.Begin();
                }
            }
        }

        private void HideHints()
        {

            if (gridHints.Opacity == 0)
                return;


            var story = new Storyboard();

            var ani = new DoubleAnimation
            {
                From = gridHints.Opacity,
                To = 0,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            Storyboard.SetTarget(ani, gridHints);
            Storyboard.SetTargetProperty(ani, new PropertyPath(Grid.OpacityProperty));

            story.Children.Add(ani);

            story.Begin();


        }

        private void SetResults(ResultEventArgs args)
        {
            if (args == null) return;

            SetPlayerResult(args.Player);

            if (Globals.Game.IsSplit)
            {
                SetSplitResult(args.Split);
            }
        }

        public void HideResults()
        {
            if (_plrResultAni != null)
            {
                _plrResultAni.Stop();
                _plrResultAni = null;
            }
            if (_splitResultAni != null)
            {
                _splitResultAni.Stop();
                _splitResultAni = null;
            }
            gridPlrResult.Opacity = 0;
            gridSplitResult.Opacity = 0;
        }


        private void OnResults(object o, ResultEventArgs args)
        {
            Globals.Settings.Achievements.Check(args);

            if ((EnumGameResult)Math.Min((int)args.Player, (int)args.Split) == EnumGameResult.BlackJack)
            {
                Globals.Game.LastResult = null;
                _blackJack = true;
                return; // handle elsewhere (when dealing)
            }

            Globals.Game.LastResult = args;
            //SetResults(args.Player, args.Split);
            Storyboard story = new Storyboard();
            HidePlayBar(story);

            HideHints();

            //the bet has changed so update it
            if (Globals.Game.BetAmount != iCurrentBet)
            {
                iCurrentBet = Globals.Game.BetAmount;
                SetChips();
                SetBetLabel(Globals.Game.BetAmount);
            }

            ShowMainMenu(story);
            splitSelector.Stop();
            playerSelector.Stop();
            story.Begin();
        }

        //automatically populate the onscreen chips (for bet reductions)
        private void SetChips()
        {
            //ie ... no animation
            playerHand.ClearChips();

            int iTemp = Globals.Game.BetAmount;

            while (iTemp >= 500)
            {
                playerHand.Add500Chip();
                iTemp -= 500;
            }

            while (iTemp >= 100)
            {
                playerHand.Add100Chip();
                iTemp -= 100;
            }

            while (iTemp >= 50)
            {
                playerHand.Add50Chip();
                iTemp -= 50;
            }

            while (iTemp >= 25)
            {
                playerHand.Add25Chip();
                iTemp -= 25;
            }



        }

        private void OnShuffle(object o, EventArgs args)
        {
           BJEngine.PlaySound(AppSettings.SOUND_SHUFFLE);

        }

        private int DrawDealerHand(int startDelay, int delay)
        {
            _dealerCount = 0;

            int d = startDelay;

            foreach (var card in Globals.Game.DealerHand)
            {
                CardImage img = canvasDealer.Children[_dealerCount] as CardImage;

                if (_dealerCount == 0 || Globals.Game.ShowDealerCard)
                {
                    img.SetBackground(card, d);

                    _dealerCount++;
                }
                else
                {
                    img.SetReversed(d);
                }

                Globals.Game.DealAnotherCard();

                d += delay;

                Debug.WriteLine("Drawing dealer " + _dealerCount + " " + card);

                if (_dealerCount >= canvasDealer.Children.Count)
                    break;
            }

            if (Globals.Game.ShowDealerScore() && Globals.Game.ShowDealerCard)
            {
                var dt = new DispatcherTimer();
                dt.Interval = new TimeSpan(0, 0, 0, 0, d);
                dt.Tick += delegate(object obj, EventArgs e)
                {
                    dt.Stop();

                    SetDealerScore(Globals.Game.DealerHand.GetScore());
                };
                dt.Start();
            }
            return d;
        }


        private void HideMainMenu(Storyboard story)
        {
            var dur = new Duration(TimeSpan.FromMilliseconds(400));

            var opAni = new DoubleAnimation
            {
                From = mainMenu.Opacity,
                To = 0.0,
                Duration = dur
            };

            mainMenu.IsHitTestVisible = false;

            Storyboard.SetTarget(opAni, mainMenu);
            Storyboard.SetTargetProperty(opAni, new PropertyPath(Grid.OpacityProperty));

            story.Children.Add(opAni);
        }

        private void HideBetBar(Storyboard story)
        {

            var dur = new Duration(TimeSpan.FromMilliseconds(400));

            var opAni = new DoubleAnimation
            {
                From = betBar.Opacity,
                To = 0.0,
                Duration = dur
            };

            Storyboard.SetTarget(opAni, betBar);
            Storyboard.SetTargetProperty(opAni, new PropertyPath(Grid.OpacityProperty));

            story.Children.Add(opAni);

        }

        private void HidePlayBar(Storyboard story)
        {
            var dur = new Duration(TimeSpan.FromMilliseconds(400));

            var opAni = new DoubleAnimation
            {
                From = playBar.Opacity,
                To = 0.0,
                Duration = dur
            };

            Storyboard.SetTarget(opAni, playBar);
            Storyboard.SetTargetProperty(opAni, new PropertyPath(Grid.OpacityProperty));

            playBar.IsHitTestVisible = false;

            story.Children.Add(opAni);

        }

        private void Deal_Click(object sender, MouseButtonEventArgs args)
        {
            //save the current bet
            iCurrentBet = Globals.Game.BetAmount;

            Debug.WriteLine("Deal clicked");

            SetBetLabel(Globals.Game.BetAmount);

            playerHand.Reset();
            splitHand.Reset();

            foreach (CardImage c in canvasDealer.Children)
                c.Reset();

            playerHand.HideScore();
            splitHand.HideScore();
            HideDealerScore();

            if (Globals.Game.IsSplit)
            {
                playerSelector.Stop();
                splitSelector.Stop();
                splitHand.Hide();
            }
            HideResults();

            var story = new Storyboard();

            HideMainMenu(story);

            HideBetBar(story);

            HideAchievement(0);

            story.Completed += Deal_Completed;

            story.Begin();

            betBar.IsHitTestVisible = false;

        }

        private void Deal_Completed(object o, EventArgs args)
        {
            btnBet.Foreground = new SolidColorBrush(Colors.White);

            Globals.Game.Deal();

            UpdateButtons();

            var d = playerHand.DealHand(Globals.Game.PlayerHand, 0, 250);

            d = DrawDealerHand(d, 250);

            if (!_blackJack)
            {
                var story = new Storyboard();

                ShowPlayBar(story);

                story.Begin();

                playBar.IsHitTestVisible = true;
            }

            var dt = new DispatcherTimer();
            dt.Interval = TimeSpan.FromMilliseconds(d);
            dt.Tick += delegate(object obj, EventArgs e)
            {
                dt.Stop();
                if (Globals.Game.ShowPlayerScore())
                    this.playerHand.SetScore(Globals.Game.PlayerHand);
                if (_blackJack)
                {
                    fire.addFirework(400, 240.0);
                    SetPlayerResult(EnumGameResult.BlackJack);
                    _blackJack = false;
                    FlipDealerCards();
                    var story = new Storyboard();
                    ShowMainMenu(story);
                    story.Begin();
                }
                //both player and dealer have blackjack
                else if (Globals.Game.IsBJTie())
                {
                    FlipDealerCards();

                    var story = new Storyboard();

                    HidePlayBar(story);
                    ShowMainMenu(story);

                    story.Begin();
                }
                else
                {
                    ShowHint();
                }
                SetBankLabel(Globals.Game.Bank);
            };
            dt.Start();

        }

        private void ShowPlayBar(Storyboard story)
        {
            var dur = new Duration(TimeSpan.FromMilliseconds(400));

            var aniOpacity = new DoubleAnimation
            {
                From = 0.0,
                To = 0.7,
                Duration = dur
            };

            Storyboard.SetTarget(aniOpacity, playBar);
            Storyboard.SetTargetProperty(aniOpacity, new PropertyPath(Grid.OpacityProperty));

            playBar.IsHitTestVisible = true;

            story.Children.Add(aniOpacity);

        }



        private void SetDealerScore(int score)
        {
            lblDealerCount.Text = score.ToString();

            //if (lblDealerCount.Opacity != 0)
            //    return;

            var story = new Storyboard();

            var ani = new DoubleAnimation
            {
                From = 0,
                To = 1.0,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            Storyboard.SetTarget(ani, borDealerCount);
            Storyboard.SetTargetProperty(ani, new PropertyPath(Border.OpacityProperty));

            
            story.Children.Add(ani);
       //     story.Children.Add(ani2);

            story.Begin();
        }

        private void HideDealerScore()
        {
            var story = new Storyboard();

            var ani = new DoubleAnimation
            {
                From = borDealerCount.Opacity,
                To = 0,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            Storyboard.SetTarget(ani, borDealerCount);
            Storyboard.SetTargetProperty(ani, new PropertyPath(Border.OpacityProperty));

            story.Children.Add(ani);

            story.Begin();
        }

        private void UpdateButtons()
        {
            //btnBet.Disabled = Globals.Game.GameState != EnumGameState.Bet && Globals.Game.GameState != EnumGameState.PlayerFirstMove;
            btnReset.Disabled = !Globals.Game.ShowDealButton();
            btnDeal.Disabled = !Globals.Game.ShowDealButton();
            btnSplit.Disabled = !Globals.Game.ShowSplitButton();
            if (Globals.Game.ShowInsuranceButton())
            {
                btnSurrender.Text = Globals.Res.GetString("IDS_BTNS_Insurance");
                btnSurrender.Disabled = false;
            }
            else if (Globals.Game.ShowSurrenderButton())
            {

                btnSurrender.Disabled = false;
                btnSurrender.Text = Globals.Res.GetString("IDS_RESULT_Surrender");
            }
            else
            {
                btnSurrender.Text = string.Empty;
                btnSurrender.Disabled = true;
            }

            btn100.Disabled = !Globals.Game.Show100();
            btn25.Disabled = !Globals.Game.Show25();
            btn50.Disabled = !Globals.Game.Show50();
            btn500.Disabled = !Globals.Game.Show500();

            btnDouble.Disabled = !Globals.Game.ShowDoubleButton();

            btnReset.Disabled = Globals.Game.BetAmount == 0;

        }

        private void btn25_OnClick(object sender, MouseButtonEventArgs e)
        {
            Debug.WriteLine("$25 bet clicked");
            if (Globals.Game.Add25ChipBet())
            {
                SetBetLabel(Globals.Game.BetAmount);
                if (Globals.Game.GameState == EnumGameState.SplitMove)
                    splitHand.Add25Chip();
                else
                    playerHand.Add25Chip();

                BJEngine.PlaySound(AppSettings.SOUND_BET);

                dollarRed.Start();
                SetBankLabel(Globals.Game.Bank);

                UpdateButtons();
            }
        }

        private void btn50_OnClick(object sender, MouseButtonEventArgs e)
        {
            Debug.WriteLine("50 bet clicked");
            if (Globals.Game.Add50ChipBet())
            {
                SetBetLabel(Globals.Game.BetAmount);
                if (Globals.Game.GameState == EnumGameState.SplitMove)
                    splitHand.Add50Chip();
                else
                    playerHand.Add50Chip();
                BJEngine.PlaySound(AppSettings.SOUND_BET);

                dollarBlue.Start();
                SetBankLabel(Globals.Game.Bank);

                UpdateButtons();
            }
        }

        private void btn100_OnClick(object sender, MouseButtonEventArgs e)
        {
            Debug.WriteLine("100 bet clicked");
            if (Globals.Game.Add100ChipBet())
            {
                SetBetLabel(Globals.Game.BetAmount);
                if (Globals.Game.GameState == EnumGameState.SplitMove)
                    splitHand.Add100Chip();
                else
                    playerHand.Add100Chip();

                BJEngine.PlaySound(AppSettings.SOUND_BET);

                dollarGreen.Start();
                SetBankLabel(Globals.Game.Bank);

                UpdateButtons();
            }
        }

        private void btn500_OnClick(object sender, MouseButtonEventArgs e)
        {
            Debug.WriteLine("500 bet clicked");
            if (Globals.Game.Add500ChipBet())
            {
                SetBetLabel(Globals.Game.BetAmount);
                if (Globals.Game.GameState == EnumGameState.SplitMove)
                    splitHand.Add500Chip();
                else
                    playerHand.Add500Chip();

                BJEngine.PlaySound(AppSettings.SOUND_BET);

                dollarBlack.Start();
                SetBankLabel(Globals.Game.Bank);

                UpdateButtons();
            }
        }

        private void btnReset_OnClick(object sender, MouseButtonEventArgs e)
        {
            Debug.WriteLine("Reset clicked");

            if (Globals.Game.ResetBetAmount())
            {
                SetBetLabel(Globals.Game.BetAmount);
                playerHand.ResetChips();
                UpdateButtons();
            }
        }


        private void btnHit_OnClick(object sender, MouseButtonEventArgs e)
        {
            Debug.WriteLine("hit clicked");

            Globals.Game.Hit();

            if (splitHand.FlipNewCards(Globals.Game.SplitHand) && Globals.Game.ShowSplitScore())
                splitHand.SetScore(Globals.Game.SplitHand);

            if (playerHand.FlipNewCards(Globals.Game.PlayerHand) && Globals.Game.ShowPlayerScore())
                playerHand.SetScore(Globals.Game.PlayerHand);

            Globals.Settings.Achievements.Check(EnumAchievement.TheGambler, ResultEventArgs.Empty);

            ToggleSelector();

            ShowHint();

            UpdateButtons();
        }

        private void ToggleSelector()
        {
            if (!Globals.Game.IsSplit)
            {
                return;
            }
            if (Globals.Game.GameState == EnumGameState.EndOfGame)
                return;

            if (Globals.Game.GameState == EnumGameState.PlayerMove)
            {
                playerSelector.Start();
                splitSelector.Stop();
            }
            else
            {
                splitSelector.Start();
                playerSelector.Stop();
            }

        }
        private void btStand_OnClick(object sender, MouseButtonEventArgs e)
        {
            Debug.WriteLine("stand clicked");

            if (Globals.Game.Stand())
            {
                playerHand.FlipNewCards(Globals.Game.PlayerHand);

                ToggleSelector();

                ShowHint();
            }

        }

        private void FlipDealerCards()
        {
            int delay = 0;

            while (_dealerCount < Globals.Game.DealerHand.Count)
            {
                var img = canvasDealer.Children[_dealerCount] as CardImage;

                Globals.Game.DealAnotherCard();

                if (delay == 0)
                {
                    img.Flip(canvasDealer, Globals.Game.DealerHand[_dealerCount], delay);
                    delay += 600;
                }
                else
                    img.SetBackground(Globals.Game.DealerHand[_dealerCount], delay);

                _dealerCount++;
            }

            var dt = new DispatcherTimer();

            dt.Interval = new TimeSpan(0, 0, 0, 0, delay);

            dt.Tick += delegate(object o, EventArgs args)
            {

                dt.Stop();

                if (Globals.Game.ShowDealerScore())
                    SetDealerScore(Globals.Game.DealerHand.GetScore());

                if (Globals.Game.ShowPlayerScore())
                    playerHand.SetScore(Globals.Game.PlayerHand);

                if (Globals.Game.ShowSplitScore())
                    splitHand.SetScore(Globals.Game.SplitHand);

                SetBankLabel(Globals.Game.Bank);

                SetResults(Globals.Game.LastResult);

            };

            dt.Start();
        }

        private void btnSplit_OnClick(object sender, MouseButtonEventArgs e)
        {
            Debug.WriteLine("split clicked");

            if (Globals.Game.Split())
            {
                playerHand.Reset();

                playerHand.HideScore();

                splitHand.Reset();

                splitHand.CopyChips(playerHand);

                splitHand.Show();

                var d = splitHand.DealHand(Globals.Game.SplitHand, 400, 250);

                if (Globals.Game.ShowSplitScore())
                {
                    var dt = new DispatcherTimer();
                    dt.Interval = TimeSpan.FromMilliseconds(d);
                    dt.Tick += delegate(object o, EventArgs args)
                    {
                        dt.Stop();

                        splitHand.SetScore(Globals.Game.SplitHand);
                        BJEngine.PlaySound(AppSettings.SOUND_BET);

                    };
                    dt.Start();
                }

                d = playerHand.DealHand(Globals.Game.PlayerHand, d, 250);


                var dt2 = new DispatcherTimer();
                dt2.Interval = TimeSpan.FromMilliseconds(d);
                dt2.Tick += delegate(object o, EventArgs args)
                {
                    dt2.Stop();

                    if (Globals.Game.ShowPlayerScore())
                        this.playerHand.SetScore(Globals.Game.PlayerHand);

                    ToggleSelector();
                    ShowHint();
                };

                dt2.Start();

                UpdateButtons();
            }

        }

        private void btDouble_OnClick(object sender, MouseButtonEventArgs e)
        {
            Debug.WriteLine("double clicked");

            if (Globals.Game.DoubleDown())
            {
                if (Globals.Game.GameState == EnumGameState.SplitMove)
                {
                    splitHand.FlipNewCards(Globals.Game.SplitHand);
                    splitHand.DoubleDown();
                }
                else
                {
                    playerHand.FlipNewCards(Globals.Game.PlayerHand);
                    playerHand.DoubleDown();
                }

                BJEngine.PlaySound(AppSettings.SOUND_BET);

                Globals.Settings.Achievements.Check(EnumAchievement.DoubleTrouble,
                    ResultEventArgs.Empty);

                FlipDealerCards();

                SetBetLabel(Globals.Game.BetAmount);
                SetBankLabel(Globals.Game.Bank);

                UpdateButtons();
            }

        }

        private static string GetResultMsg(EnumGameResult res)
        {
            switch (res)
            {
                case EnumGameResult.BlackJack:
                    return Globals.Res.GetString("IDS_RESULT_Blackjack");
                case EnumGameResult.Draw:
                    return Globals.Res.GetString("IDS_RESULT_Draw");
                case EnumGameResult.Lose:
                    return Globals.Res.GetString("IDS_RESULT_Lose");
                case EnumGameResult.Surrender:
                    return Globals.Res.GetString("IDS_RESULT_Surrender");
                case EnumGameResult.Win:
                    return Globals.Res.GetString("IDS_RESULT_Win");
                default:
                    return null;
            }
        }

        public void SetResultImage(EnumGameResult res, Image img)
        {
            switch (res)
            {
                case EnumGameResult.BlackJack:
                    BJEngine.PlaySound(AppSettings.SOUND_BLACKJACK);
                    BJEngine.PlaySound(AppSettings.SOUND_CHA_CHING);
                    img.Source = Globals.Gui.Skin.imgTabWin;
                    break;
                case EnumGameResult.Win:
                    img.Source = Globals.Gui.Skin.imgTabWin;
                    BJEngine.PlaySound(AppSettings.SOUND_CHA_CHING);
                    break;
                case EnumGameResult.Lose:
                    img.Source = Globals.Gui.Skin.imgTabLose;
                    BJEngine.PlaySound(AppSettings.SOUND_LOSE);
                    break;
                case EnumGameResult.Surrender:
                    img.Source = Globals.Gui.Skin.imgTabLose;
                    break;
                default:
                    BJEngine.PlaySound(AppSettings.SOUND_CLAP);
                    img.Source = Globals.Gui.Skin.imgTabPush;
                    break;

            }
        }

        public void SetSplitResult(EnumGameResult res)
        {
            SetResultImage(res, imgSplitResult);

            lblSplitResult.Text = GetResultMsg(res);

            var ani = new DoubleAnimation
            {
                From = 0,
                To = 1,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            _splitResultAni = new Storyboard();

            Storyboard.SetTarget(ani, gridSplitResult);
            Storyboard.SetTargetProperty(ani, new PropertyPath(Grid.OpacityProperty));

            _splitResultAni.Children.Add(ani);

            _splitResultAni.Begin();
        }


        public void SetPlayerResult(EnumGameResult res)
        {
            SetResultImage(res, imgPlrResult);

            lblPlrResult.Text = GetResultMsg(res);

            var ani = new DoubleAnimation
            {
                From = 0,
                To = 1,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            _plrResultAni = new Storyboard();

            Storyboard.SetTarget(ani, gridPlrResult);
            Storyboard.SetTargetProperty(ani, new PropertyPath(Grid.OpacityProperty));

            _plrResultAni.Children.Add(ani);

            _plrResultAni.Begin();
        }

        private void btnSurrender_OnClick(object sender, MouseButtonEventArgs e)
        {
            Debug.WriteLine("surrender clicked");

            if (Globals.Game.ShowInsuranceButton())
            {
                Globals.Game.Insurance();

            }
            else if (Globals.Game.Surrender())
            {
                BJEngine.PlaySound(AppSettings.SOUND_BET);
                BJEngine.PlaySound(AppSettings.SOUND_COUGH);
            }

            SetBankLabel(Globals.Game.Bank);
            SetBetLabel(Globals.Game.BetAmount);
            UpdateButtons();
        }

        private void settingsButton1_OnClick(object sender, MouseButtonEventArgs e)
        {
            _options = true;
            NavigationService.Navigate(new Uri("/Options.xaml?OptionType=0", UriKind.Relative));
        }

        private void PhoneApplicationPage_LayoutUpdated(object sender, EventArgs e)
        {
        }

        private void ShowBetBar(Storyboard story)
        {
            var dur = new Duration(TimeSpan.FromMilliseconds(400));

            var opAni = new DoubleAnimation
            {
                From = 0.0,
                To = 0.7,
                Duration = dur
            };

            Storyboard.SetTarget(opAni, betBar);
            Storyboard.SetTargetProperty(opAni, new PropertyPath(Grid.OpacityProperty));

            story.Children.Add(opAni);

        }

        private void btnBet_OnClick(object sender, MouseButtonEventArgs e)
        {
            Storyboard story = new Storyboard();

            if (betBar.Opacity == 0)
            {
                ShowBetBar(story);

                betBar.IsHitTestVisible = true;
            }
            else
            {
                HideBetBar(story);

                betBar.IsHitTestVisible = false;
            }

            story.Begin();

            UpdateButtons();

        }

        int clickCount = 0;
        long lastClickTime = 0;
        private void gridHints_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            bool bDouble = false;
            if (lastClickTime == 0 || DateTime.Now.Ticks - lastClickTime > 5000000)//500 ms
            {
                lastClickTime = DateTime.Now.Ticks;
                bDouble = false;
                return;
            }
            else
            {
                lastClickTime = DateTime.Now.Ticks;
                bDouble = true;
            }

            switch (Globals.Game.GetNextMove())
            {
                case EnumNextMove.Double:
                    btDouble_OnClick(btnDouble, e);
                    break;
                case EnumNextMove.Hit:
                    btnHit_OnClick(btnHit, e);
                    break;
                case EnumNextMove.Insurance:
                case EnumNextMove.Surrender:
                    btnSurrender_OnClick(btnSurrender, e);
                    break;
                case EnumNextMove.Split:
                    btnSplit_OnClick(btnSplit, e);
                    break;
                case EnumNextMove.Stand:
                    btStand_OnClick(btnStand, e);
                    break;
            }

            bDouble = false;
        }

        
        private void gridHints_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
           
        }

        private void OnCountClick(object sender, MouseButtonEventArgs e)
        {
            if (Globals.Settings.CardCountView != EnumCardCountView.SemiHidden || _cardCountAni != null)
                return;


            lblCountValue.Text = Globals.Game.GetCardCount().ToString();
            lblCount.Text = Globals.Res.GetString("IDS_INFO_Count");

            var ani = new DoubleAnimation
            {
                From = lblCount.Opacity,
                To = 0,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            Storyboard.SetTarget(ani, lblCount);
            Storyboard.SetTargetProperty(ani, new PropertyPath(TextBlock.OpacityProperty));

            _cardCountAni = new Storyboard();

            _cardCountAni.BeginTime = TimeSpan.FromSeconds(3);

            _cardCountAni.Children.Add(ani);

            ani = new DoubleAnimation
            {
                From = lblCountValue.Opacity,
                To = 0,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            Storyboard.SetTarget(ani, lblCountValue);
            Storyboard.SetTargetProperty(ani, new PropertyPath(TextBlock.OpacityProperty));

            _cardCountAni.Children.Add(ani);

            _cardCountAni.Completed += delegate(object o, EventArgs args)
            {
                var ani2 = new DoubleAnimation
                {
                    From = 0,
                    To = 1,
                    Duration = new Duration(TimeSpan.FromMilliseconds(400))
                };

                var s = Globals.Res.GetString("IDS_INFO_CardCount");
                var values = s.Split(new string[] { "\\r\\n" }, StringSplitOptions.RemoveEmptyEntries);
                if (values.Length == 2)
                {
                    lblCountValue.Text = values[0];
                    lblCount.Text = values[1];
                }

                Storyboard.SetTarget(ani2, lblCount);
                Storyboard.SetTargetProperty(ani2, new PropertyPath(TextBlock.OpacityProperty));
                Storyboard s2 = new Storyboard();
                s2.Children.Add(ani2);

                ani2 = new DoubleAnimation
                {
                    From = 0,
                    To = 1,
                    Duration = new Duration(TimeSpan.FromMilliseconds(400))
                };

                Storyboard.SetTarget(ani2, lblCountValue);
                Storyboard.SetTargetProperty(ani2, new PropertyPath(TextBlock.OpacityProperty));

                s2.Children.Add(ani2);

                s2.Begin();

                _cardCountAni = null;
            };

            _cardCountAni.Begin();
        }

        public void NavBack()
        {
            NavigationService.GoBack();
        }

       
    }
}
