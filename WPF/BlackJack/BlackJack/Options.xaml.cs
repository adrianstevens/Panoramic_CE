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

namespace BlackJack
{
    /// <summary>
    /// Interaction logic for Options.xaml
    /// </summary>
    public partial class Options : Page
    {
        static readonly string CURRENCY_FORMAT = string.Format("{0}#,##0;({0}#,##0);{0}0", RegionInfo.CurrentRegion.CurrencySymbol);
        
        public enum EnumTab
        {
            Settings,
            HighScores,
            About,
            MoreApps,
        }

        private void AddComboItem(ref ComboBox cBox, String szItem)
        {
            ComboBoxItem cItem = new ComboBoxItem();
            cItem.Content = szItem;
            cItem.Foreground = new SolidColorBrush(Colors.White);
            cItem.Background = new SolidColorBrush(Colors.Black);
            cItem.FontSize = 16;
            cBox.Items.Add(cItem);
        }

        public Options()
        {
            InitializeComponent();


            string szAbout = Globals.Res.GetString("IDS_ABOUT_1");
            txtAboutCopyright.Text = szAbout.Replace("\\r\\n", "\r\n");

            SetTabColor(EnumTab.Settings);

          //  AddComboItem(ref optGameDifficulty, Globals.Res.GetString("IDS_CMB_CCV_On"));
          //  AddComboItem(ref optGameDifficulty, Globals.Res.GetString("IDS_CMB_CCV_Off"));
            
            optGameDifficulty.txtTitle = Globals.Res.GetString("IDS_CMB_GD_Learn");
            optGameDifficulty.AddItem(Globals.Res.GetString("IDS_CMB_CCV_On"));
            optGameDifficulty.AddItem(Globals.Res.GetString("IDS_CMB_CCV_Off"));
            optGameDifficulty.SetCurrentIndex(Globals.Game.GameDifficulty == EnumGameDifficulty.Learn ? 0 : 1);
            optGameDifficulty.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Game.GameDifficulty = iSelIndex == 0 ? EnumGameDifficulty.Learn : EnumGameDifficulty.Expert;
            };

            optPlaySounds.txtTitle = Globals.Res.GetString("IDS_LBL_PlaySounds");
            optPlaySounds.AddItem(Globals.Res.GetString("IDS_MENU_Yes"));
            optPlaySounds.AddItem(Globals.Res.GetString("IDS_MENU_No"));
            optPlaySounds.SetCurrentIndex(Globals.Settings.bPlaySounds ? 0 : 1);
            optPlaySounds.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.bPlaySounds = iSelIndex == 1 ? false : true;
            };

            optCardCount.txtTitle = Globals.Res.GetString("IDS_LBL_CardCountView");
            optCardCount.AddItem(Globals.Res.GetString("IDS_CMB_CCV_On"));
            optCardCount.AddItem(Globals.Res.GetString("IDS_CMB_CCV_Off"));
            optCardCount.AddItem(Globals.Res.GetString("IDS_CMB_CCV_Semi"));
            optCardCount.SetCurrentIndex((int)Globals.Settings.CardCountView);
            optCardCount.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.CardCountView = (EnumCardCountView)iSelIndex;
            };

            optCardCountStyle.txtTitle = Globals.Res.GetString("IDS_LBL_CardCounting");
            optCardCountStyle.AddItem(Globals.Res.GetString("IDS_CMB_CC_HiLow"));
            optCardCountStyle.AddItem(Globals.Res.GetString("IDS_CMB_CC_HiOpt1"));
            optCardCountStyle.AddItem(Globals.Res.GetString("IDS_CMB_CC_HiOpt2"));
            optCardCountStyle.AddItem(Globals.Res.GetString("IDS_CMB_CC_KO"));
            optCardCountStyle.AddItem(Globals.Res.GetString("IDS_CMB_CC_OmegaII"));
            optCardCountStyle.AddItem(Globals.Res.GetString("IDS_CMB_CC_Wizard"));
            optCardCountStyle.AddItem(Globals.Res.GetString("IDS_CMB_CC_Zen"));
            optCardCountStyle.SetCurrentIndex((int)Globals.Settings.CountSystem);
            optCardCountStyle.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.CountSystem = (EnumCardCount)iSelIndex;
            };

            optShowDealerCard.txtTitle = Globals.Res.GetString("IDS_OPT_ShowDealerCard");
            optShowDealerCard.AddItem(Globals.Res.GetString("IDS_MENU_Yes"));
            optShowDealerCard.AddItem(Globals.Res.GetString("IDS_MENU_No"));
            optShowDealerCard.SetCurrentIndex(Globals.Game.ShowDealerCard ? 0 : 1);
            optShowDealerCard.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Game.ShowDealerCard = iSelIndex == 1 ? false : true;
            };

            optBurnFirst.txtTitle = Globals.Res.GetString("IDS_OPT_BurnFirst");
            optBurnFirst.AddItem(Globals.Res.GetString("IDS_MENU_Yes"));
            optBurnFirst.AddItem(Globals.Res.GetString("IDS_MENU_No"));
            optBurnFirst.SetCurrentIndex(Globals.Game.BurnFirstCard ? 0 : 1);
            optBurnFirst.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Game.BurnFirstCard = iSelIndex == 1 ? false : true;
            };

            optShowScores.txtTitle = Globals.Res.GetString("IDS_OPT_ShowScores");
            optShowScores.AddItem(Globals.Res.GetString("IDS_MENU_Yes"));
            optShowScores.AddItem(Globals.Res.GetString("IDS_MENU_No"));
            optShowScores.SetCurrentIndex(Globals.Game.ShowScores ? 0 : 1);
            optShowScores.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Game.ShowScores = iSelIndex == 0 ? true : false;
            };

            optDealerSoft17.txtTitle = Globals.Res.GetString("IDS_OPT_DealerHitSoft17");
            optDealerSoft17.AddItem(Globals.Res.GetString("IDS_MENU_Yes"));
            optDealerSoft17.AddItem(Globals.Res.GetString("IDS_MENU_No"));
            optDealerSoft17.SetCurrentIndex(Globals.Game.DealerHitsOnSoft17 ? 0 : 1);
            optDealerSoft17.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Game.DealerHitsOnSoft17 = iSelIndex == 1 ? false : true;
            };

            optNumDecks.txtTitle = Globals.Res.GetString("IDS_OPT_NumDecks");
            optNumDecks.AddItem(1.ToString());
            optNumDecks.AddItem(2.ToString());
            optNumDecks.AddItem(3.ToString());
            optNumDecks.AddItem(4.ToString());
            optNumDecks.SetCurrentIndex(Globals.Game.NumDecks - 1);
            optNumDecks.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Game.NumDecks = iSelIndex + 1;
            };

            optPayout.txtTitle = Globals.Res.GetString("IDS_OPT_BJPayout");
            optPayout.AddItem("2/1");
            optPayout.AddItem("2/3");
            optPayout.SetCurrentIndex(Globals.Game.BlackJack2to1 ? 0 : 1);
            optPayout.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Game.BlackJack2to1 = iSelIndex == 0;
            };

            optBankRoll.txtTitle = Globals.Res.GetString("IDS_OPT_InitialBank");
            optBankRoll.AddItem(10000.ToString(CURRENCY_FORMAT));
            optBankRoll.AddItem(5000.ToString(CURRENCY_FORMAT));
            optBankRoll.AddItem(1000.ToString(CURRENCY_FORMAT));
            optBankRoll.AddItem(500.ToString(CURRENCY_FORMAT)); ;
            optBankRoll.AddItem(250.ToString(CURRENCY_FORMAT));
            optBankRoll.AddItem(100.ToString(CURRENCY_FORMAT));
            optBankRoll.SetCurrentIndex((int)Globals.Game.BankRoll);
            optBankRoll.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Game.BankRoll = (StartingBank)iSelIndex;
            };
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            //LayoutRoot.Background = Globals.Gui.GetImageBrush(ref Globals.Gui.Skin.imgSettingsBackground);

            btnBack.Source = Globals.Gui.Skin.imgPrev;

            btnSettings.Text = Globals.Res.GetString("IDS_MENU_Options");
            btnHiScores.Text = lblAchievements.Text = Globals.Res.GetString("IDS_MENU_Achievements");
            lblHiScores.Text = Globals.Res.GetString("IDS_MENU_HiScores");
            btnAbout.Text = Globals.Res.GetString("IDS_MENU_About");
            txtAboutCopyright.Text = Globals.Res.GetString("IDS_ABOUT_1").Replace("\\r\\n", "\r\n");
            //btnMoreApps.Text = Globals.Res.GetString("IDS_MENU_MoreApps");

            lbAchievements.ItemsSource = Globals.Settings.Achievements;

            SetHiScores();
        }

        private void SetHiScores()
        {
            var index = 0;

            int i = 0;

            for (i = Globals.Settings.HighScores.Count - 1; i >= 0; i--)
            {
                if (index >= lstHiScores.Children.Count) break;

                var tb = lstHiScores.Children[index++] as TextBlock;

                tb.Text = index + ". " + Globals.Settings.HighScores[i].Score.ToString("$#,#");
            }

            for (int j = i; j < 5; j++)
            {
                if (index >= lstHiScores.Children.Count) break;

                var tb = lstHiScores.Children[index++] as TextBlock;

                tb.Text = index + ". " + BJEngine.HISCORE_DEFAULT.ToString("$#,#");
            }



            lblAchievePoints.Text = Globals.Settings.Achievements.TotalPoints().ToString();
        }

        private void Button_Click(object sender, MouseButtonEventArgs e)
        {
         /*   WebBrowserTask web = new WebBrowserTask();
            web.URL = "http://www.twitter.com/PanoramicSoft";
            web.Show();*/
        }

        private void Button2_Click(object sender, MouseButtonEventArgs e)
        {
          /*  WebBrowserTask web = new WebBrowserTask();
            web.URL = "http://www.facebook.com/panoramicsoft";
            web.Show();*/
        }

        private void btnSettings_OnClick(object sender, MouseButtonEventArgs e)
        {
            ShowTab(gridSettings, Globals.Gui.GetImageBrush(ref Globals.Gui.Skin.imgSettingsBackground));

            SetTabColor(EnumTab.Settings);
        }

        private void btnHiScores_OnClick(object sender, MouseButtonEventArgs e)
        {
            ShowTab(gridHiScores, Globals.Gui.GetImageBrush(ref Globals.Gui.Skin.imgSettingsBackground));

            SetTabColor(EnumTab.HighScores);
        }

        private void btAbout_OnClick(object sender, MouseButtonEventArgs e)
        {
            ShowTab(gridAbout, Globals.Gui.GetImageBrush(ref Globals.Gui.Skin.imgAboutBg));
            //ShowTab(gridAbout, Globals.Gui.GetImageBrush(new BitmapImage(new Uri(@"Assets/about.jpg", UriKind.Relative));

            SetTabColor(EnumTab.About);
        }

        private void btnMoreApps_OnClick(object sender, MouseButtonEventArgs e)
        {
           /* ShowTab(gridMoreApps, Globals.Gui.GetImageBrush(ref Globals.Gui.Skin.imgAboutBg));

            SetTabColor(EnumTab.MoreApps);*/
        }

        private void ShowTab(Grid tab, ImageBrush background)
        {
            Grid grid;

            // get the current grid
            if (gridAbout.Visibility != Visibility.Collapsed)
                grid = gridAbout;
            else if (gridHiScores.Visibility != Visibility.Collapsed)
                grid = gridHiScores;
         /*   else if (gridMoreApps.Visibility != Visibility.Collapsed)
                grid = gridMoreApps;*/
            else if (gridSettings.Visibility != Visibility.Collapsed)
                grid = gridSettings;
            else
                return;

            // short circuit on same grid
            if (grid == tab) return;

            var dt = new DoubleAnimation
            {
                From = LayoutRoot.Opacity,
                To = 0,
                Duration = new Duration(new TimeSpan(0, 0, 0, 0, 400))
            };


            // fade out/in to the selected grid
            var story = new Storyboard();

            Storyboard.SetTarget(dt, LayoutRoot);
            Storyboard.SetTargetProperty(dt, new PropertyPath(Grid.OpacityProperty));

            story.Children.Add(dt);

            story.Completed += delegate(object o, EventArgs args)
            {
                var ani = new DoubleAnimation
                {
                    From = LayoutRoot.Opacity,
                    To = 1,
                    Duration = new Duration(new TimeSpan(0, 0, 0, 0, 400))
                };

                var endStory = new Storyboard();
                grid.Visibility = Visibility.Collapsed;
                grid.IsHitTestVisible = false;
                tab.Visibility = Visibility.Visible;
                tab.IsHitTestVisible = true;
         //       LayoutRoot.Background = background;

                Storyboard.SetTarget(ani, LayoutRoot);
                Storyboard.SetTargetProperty(ani, new PropertyPath(Grid.OpacityProperty));

                endStory.Children.Add(ani);

                endStory.Begin();
            };

            story.Begin();
        }

        private void SetTabColor(EnumTab tab)
        {
            btnSettings.txtMain.Foreground = tab == EnumTab.Settings ? new SolidColorBrush(Colors.Yellow) : new SolidColorBrush(Colors.White);
            btnHiScores.txtMain.Foreground = tab == EnumTab.HighScores ? new SolidColorBrush(Colors.Yellow) : new SolidColorBrush(Colors.White);
            btnAbout.txtMain.Foreground = tab == EnumTab.About ? new SolidColorBrush(Colors.Yellow) : new SolidColorBrush(Colors.White);
            //btnMoreApps.txtMain.Foreground = tab == EnumTab.MoreApps ? new SolidColorBrush(Colors.Yellow) : new SolidColorBrush(Colors.White);
        }

        private void btnBack_OnClick(object sender, MouseButtonEventArgs e)
        {
            NavigationService.GoBack();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {

        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {

        }

    }
}
