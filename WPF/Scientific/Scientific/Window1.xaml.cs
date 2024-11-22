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
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Globalization;
using CalcPro.screens;
using System.Media;
using com.intel.adp;

namespace CalcPro
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        private ObservableCollection<PageItem> _arrScreens;

        OptionsWindow options = null;

        //0x22A023F6,0xC56442A6,0x8666AB78,0xE09066A0

        public Window1()
        {

#if !DEBUG
            CheckIntel();
#endif
            
            
            InitializeComponent();

            System.Threading.Thread.CurrentThread.CurrentCulture = new CultureInfo("en-US");

            //subscribe
            Globals.LaunchPage += PageLauncher;
            Globals.btnMan.SkinUpdate += UpdateSkin;
            Globals.Calc.DisplayUpdate += UpdateDisplay;//for play sounds

            _arrScreens = new ObservableCollection<PageItem>();

            // menu needs data to work with
            //menu.lbMenu.SelectionChanged += Menu_SelectionChanged;

            LayoutRoot.Background = Globals.Brush.GetBrush(Globals.btnMan.GetBackgroundColor());

        }


        void CheckIntel()
        {
            try
            {
                //0x30328AB7,0xAA754CD6,0xAD1125E8,0x0F58377D
                AdpApplication app = new AdpApplication(new AdpApplicationId(0x22A023F6, 0xC56442A6, 0x8666AB78, 0xE09066A0));
                /// Note: replace "0x11111111, 0x11111111, 0x11111111, 0x11111111" with the actual application ID
                /// once you obtain it from Intel AppUp(SM) Developer Program web portal
            }
            catch (AdpException e)
            {
                if (e is AdpErrorException)
                {
                    // TO DO: add your logic to handle the errors during initialization
                    MessageBox.Show("Error - Scientific Calc is not authenticated.  You may need to connect to the Intel AppUp store.", e.Message);
                    System.Environment.Exit(1);
                }
                else if (e is AdpWarningException)
                {
                    // TO DO: add your logic to handle the warnings
                    MessageBox.Show("Error - Scientific Calc is not authenticated.  You may need to connect to the Intel AppUp store.", e.Message);
                }
            }
        }


        public void UpdateSkin(Color crBackground, EnumCalcProSkinColor eColor, EnumCalcProSkinStyle eStyle)
        {
            //let's set a background color
            LayoutRoot.Background = Globals.Brush.GetBrush(Globals.btnMan.GetBackgroundColor());
        }
        public void ClosingApp()
        {
            for (int i = 0; i < _arrScreens.Count; i++)
            {
                ICalcProScreen screen = (ICalcProScreen)_arrScreens[i].ctrlPage;
                screen.CloseSaveState();
            }
        }
        public void PageLauncher(int iWindowType, int iWindowID)
        {
            if (iWindowType == (int)InputType.INPUT_Graph)
            {
             //   NavigationService.Navigate(new Uri("/Screens/GraphPage.xaml", UriKind.Relative));
                GraphWindow graph = new GraphWindow();
                graph.ShowDialog();
            }
            if (iWindowType == (int)InputType.INPUT_Launch)
            {
                //NavigationService.Navigate(new Uri("/Screens/WorkSheet.xaml?LaunchType=" + iWindowID.ToString(), UriKind.Relative));
            }
        }

        public void UpdateDisplay(bool bUpdate, int iBtnIndex, int iBtnId)
        {
            string szWave = null;

            switch (Globals.Settings.iPlaySounds)
            {
                case 0://off
                    break;
                case 1://on
                    szWave = "Assets/click.wav";
                    break;
                case 2://quiet ... synced to options.xaml.cs 
                default:
                    szWave = "Assets/clicksoft.wav";
                    break;
            }

            if (szWave != null)
            {
            //    var player = new SoundPlayer(szWave);
            //    player.Play();
            }

            // specific for constants menu press
            if (iBtnIndex == (int)InputType.INPUT_CalcState && iBtnId == (int)CalcStateType.CALCSTATE_Constants)
            {
                ContextMenu menu = new ContextMenu();

                for (int i = 0; i < Globals.objConstants.GetFavoritesCount(); i++)
                {
                    MenuItem item = new MenuItem();
                    item.Header = Globals.objConstants.GetFavoriteName(i) + " " + Globals.objConstants.GetFavoriteGroup(i);
                    item.Uid = i.ToString();
                    item.FontSize = 20;
                    item.Click += new RoutedEventHandler(const_Click);
                    menu.Items.Add(item);
                }

                menu.PlacementTarget = this;
                menu.IsOpen = true;

                               
            }
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            if (_arrScreens.Count == 0)
            {
                // add the pages
                ResetScreenList();
                PopulateMenu();

                GoToScreen(Globals.Settings.iCurrentScreen, false);

                SetScreen(Globals.Settings.iCurrentScreen, false);
            }
            else
            {
                // load up this screen
                ICalcProScreen screen = (ICalcProScreen)_arrScreens[Globals.Settings.iCurrentScreen].ctrlPage;
                screen.LoadScreen(true);
                Globals.Calc.DisplayUpdate(true, -1, -1);
            }

        }
        private void ResetScreenList()
        {
            if (_arrScreens.Count > 0)
            {
                _arrScreens.Clear();

                // clean out the brush memory too
                Globals.Brush.ClearBrushes();
            }

            //     AddCalcScreen(EnumCalcType.CALC_Standard);
            AddCalcScreen(EnumCalcType.CALC_Scientific);
            AddCalcScreen(EnumCalcType.CALC_BaseConversions);
            AddCalcScreen(EnumCalcType.CALC_Graphing);
            //    AddCalcScreen(EnumCalcType.CALC_UnitConv);
            //   AddCalcScreen(EnumCalcType.CALC_Currency);
            AddCalcScreen(EnumCalcType.CALC_Constants);
            //    AddCalcScreen(EnumCalcType.CALC_Financial);
            //    AddCalcScreen(EnumCalcType.CALC_Statistics);

            this.UpdateLayout();

        }
        private void AddCalcScreen(EnumCalcType eCalc)
        {
            UserControl newCtrl = null;
            string szTitle = null;
            string szImagePath = null;

            switch (eCalc)
            {
                case EnumCalcType.CALC_Standard:
                    newCtrl = new Standard();
                    szTitle = Globals.rm.GetString("IDS_MENU_Standard");
                    szImagePath = "/Assets/Standard_icon.jpg";
                    break;
                case EnumCalcType.CALC_Scientific:
                case EnumCalcType.CALC_SciRPN:
                    newCtrl = new Scientific();
                    szTitle = Globals.rm.GetString("IDS_OPT_Scientific");
                    szImagePath = "/Assets/Scientific_icon.jpg";
                    break;
                case EnumCalcType.CALC_BaseConversions:
                    newCtrl = new BaseConversions();
                    szTitle = Globals.rm.GetString("IDS_MENU_BaseConv");
                    szImagePath = "/Assets/Base_icon.jpg";
                    break;
                case EnumCalcType.CALC_Graphing:
                    newCtrl = new Graphing();
                    szTitle = Globals.rm.GetString("IDS_MENU_Graphing");
                    szImagePath = "/Assets/Graphing_icon.jpg";
                    break;
                case EnumCalcType.CALC_UnitConv:
                //    newCtrl = new UnitConverter();
                    szTitle = Globals.rm.GetString("IDS_MENU_Unit");
                    szImagePath = "/Assets/Unit_icon.jpg";
                    break;
                case EnumCalcType.CALC_Currency:
                    newCtrl = new CurrencyConverter();
                    szTitle = Globals.rm.GetString("IDS_MENU_Currency");
                    szImagePath = "/Assets/World_icon.jpg";
                    break;
                case EnumCalcType.CALC_DateTime:
                    newCtrl = new CurrencyConverter();//DateAndTime();
                    szTitle = Globals.rm.GetString("IDS_MENU_DateTime");
                    szImagePath = "/Assets/DateTime_icon.png";
                    break;
                case EnumCalcType.CALC_Constants:
                    newCtrl = new Constants();
                    szTitle = Globals.rm.GetString("IDS_MENU_Constants");
                    szImagePath = "/Assets/Constants_icon.jpg";
                    break;
                case EnumCalcType.CALC_Financial:
                    newCtrl = new Financial();
                    szTitle = Globals.rm.GetString("IDS_MENU_Financial");
                    szImagePath = "/Assets/Financial_icon.jpg";
                    break;
                case EnumCalcType.CALC_Statistics:
                    newCtrl = new Statistics();
                    szTitle = Globals.rm.GetString("IDS_MENU_Stats");
                    szImagePath = "/Assets/Statistics_icon.jpg";
                    break;
                default:
                    return; // don't add anything
            }

            //currency.Name = "ctrlCurrency";
            newCtrl.HorizontalAlignment = HorizontalAlignment.Stretch;
            newCtrl.VerticalAlignment = VerticalAlignment.Stretch;

            // add to our list control... aka... menu
            _arrScreens.Add(new PageItem(eCalc, newCtrl, szTitle, szImagePath));
        }

        private void SetScreen(int iScreenIndex, bool bSetPrevious)
        {
            if (iScreenIndex < 0 || iScreenIndex >= _arrScreens.Count)
                return;

            // if this is the same screen
            if (bSetPrevious && iScreenIndex == Globals.Settings.iCurrentScreen)
                return;

            if (bSetPrevious)
                Globals.Settings.iPreviousScreen = Globals.Settings.iCurrentScreen;

            Globals.Settings.iCurrentScreen = iScreenIndex;

            //set the calc state to normal (ie no Hyp, etc)
            Globals.Calc.AddStates(CalcStateType.CALCSTATE_Normal);

            //set the calc base
            // if ((EnumCalcType)iScreenIndex != EnumCalcType.CALC_BaseConversions)
            //     Globals.Calc.SetCalcBase(10);
            Globals.Calc.AddBase(10);

            //if we're on graphing and expression press clear
            if (_arrScreens[Globals.Settings.iPreviousScreen].eCalc == EnumCalcType.CALC_Graphing &&
                Globals.Settings.eCalcType == CalcOperationType.CALC_String)
            {
                Globals.Calc.AddClear(ClearType.CLEAR_Clear);
            }

            // load the buttons if not loaded
            ICalcProScreen screen = (ICalcProScreen)_arrScreens[iScreenIndex].ctrlPage;

            switch (_arrScreens[iScreenIndex].eCalc)
            {
                default:
                case EnumCalcType.CALC_Standard:
                case EnumCalcType.CALC_Scientific:
                case EnumCalcType.CALC_BaseConversions:
                case EnumCalcType.CALC_Statistics:
                case EnumCalcType.CALC_Financial:
                case EnumCalcType.CALC_Constants:
                case EnumCalcType.CALC_DateTime:
                    Globals.Calc.SetCalcMode(Globals.Settings.eCalcType);
                    break;
                case EnumCalcType.CALC_Graphing:
                    Globals.Calc.SetCalcMode(CalcOperationType.CALC_String);
                    break;
                case EnumCalcType.CALC_UnitConv:
                case EnumCalcType.CALC_Currency:
                    if (Globals.Settings.eCalcType == CalcOperationType.CALC_String)
                        Globals.Calc.SetCalcMode(CalcOperationType.CALC_OrderOfOpps);
                    else
                        Globals.Calc.SetCalcMode(Globals.Settings.eCalcType);
                    break;
            }

            //force us back to base 10
            if (Globals.Calc.GetCalcBase() != 10)
            {
                Globals.Calc.SetCalcBase(10);
            }

            // load up this screen
            screen.LoadScreen(true);


            /*
         //   Dispatcher.BeginInvoke(() =>
            {
                // next screen
                screen = (ICalcProScreen)_arrScreens[NextScreen(iScreenIndex)].ctrlPage;
                screen.LoadScreen(false);

                // previous screen
                screen = (ICalcProScreen)_arrScreens[PreviousScreen(iScreenIndex)].ctrlPage;
                screen.LoadScreen(false);
            }
          //  );
            */


            Globals.Calc.DisplayUpdate(true, -1, -1);

        }
        private int NextScreen(int iIndex)
        {
            if (_arrScreens.Count == 0)
                return 0;

            if (iIndex == _arrScreens.Count - 1)
                return 0;
            else
                return iIndex + 1;
        }
        private int PreviousScreen(int iIndex)
        {
            if (_arrScreens.Count == 0)
                return 0;

            if (iIndex == 0)
                return _arrScreens.Count - 1;
            else
                return iIndex - 1;
        }
        private void GoToScreen(int iScreenIndex, bool bSetPrevious)
        {
            if (iScreenIndex == -1 || _arrScreens.Count == 0)
                return;

            gridCalc.Children.Clear();
            
            Grid.SetRow(_arrScreens[iScreenIndex].ctrlPage, 1);
            gridCalc.Children.Add(_arrScreens[iScreenIndex].ctrlPage);
        }

        private void PopulateMenu()
        {
            MenuItem item1 = new MenuItem();

            //item1.Width = 50;
            item1.Header = "Calculator";


            for(int i = 0; i < _arrScreens.Count; i++)
            {
                MenuItem item = new MenuItem();
                item.Header = _arrScreens[i].szPageName;
                item.Uid = i.ToString();//((int)_arrScreens[i].eCalc).ToString();
                item.Click += new RoutedEventHandler(item_Click);
                item1.Items.Add(item);
            }
            menuFile.Items.Add(item1);


            MenuItem item2 = new MenuItem();
            //item2.Width = 50;
            item2.Header = Globals.rm.GetString("IDS_MENU_Options2");
            item2.Click += new RoutedEventHandler(option_Click);
            menuFile.Items.Add(item2);
            
            MenuItem item3 = new MenuItem();
            //item3.Width = 50;
            item3.Header = Globals.rm.GetString("IDS_MENU_About2");
            item3.Click += new RoutedEventHandler(option_Click);
            menuFile.Items.Add(item3); 
        }

        void item_Click(object sender, RoutedEventArgs e)
        {
            int i = Convert.ToInt32(((MenuItem)sender).Uid);

            SetScreen(i, false);
            GoToScreen(i, false);
        }

        void option_Click(object sender, RoutedEventArgs e)
        {
            EnumCalcProSkinColor eColor = Globals.Settings.eSkinColor;
            EnumCalcProSkinStyle eSkin = Globals.Settings.eSkinStyle;
            bool bMem = Globals.Settings.bAdvMem;
            CalcOperationType eType = Globals.Settings.eCalcType;

            
            options = new OptionsWindow();

            options.InitializeComponent();
            options.Owner = this;
            options.ShowDialog();

            //we can check here to see if we need to reload the buttons
            if (eColor != Globals.Settings.eSkinColor ||
                eSkin != Globals.Settings.eSkinStyle ||
                bMem != Globals.Settings.bAdvMem ||
                eType != Globals.Settings.eCalcType)
            {
                //force a reload
                ResetScreenList();


                SetScreen(Globals.Settings.iCurrentScreen, false);
                GoToScreen(Globals.Settings.iCurrentScreen, false);

                ICalcProScreen screen = (ICalcProScreen)_arrScreens[Globals.Settings.iCurrentScreen].ctrlPage;
                screen.LoadScreen(true);

            }
        }

        void const_Click(object sender, RoutedEventArgs e)
        {
            int i = Convert.ToInt32(((MenuItem)sender).Uid); 
            Globals.Calc.AddString(Globals.objConstants.GetFavoriteConstant(i));
        }
    }

    public class PageItem : INotifyPropertyChanged
    {
        public UserControl ctrlPage { get; set; }
      //  public PanoramaItem panoramaItem { get; set; }
        public EnumCalcType eCalc { get; set; }
        public string szPageName { get; set; }
        public string szImagePath { get; set; }

        public event PropertyChangedEventHandler PropertyChanged;

        public PageItem(EnumCalcType calc, UserControl Page, string PageName, string ImagePath)
        //public PageItem(EnumCalcType calc, PanoramaItem panorama, UserControl Page, string PageName, string ImagePath)
        {
            eCalc = calc;
        //    panoramaItem = panorama;
            ctrlPage = Page;
            szPageName = PageName;
            szImagePath = ImagePath;
        }
    }
}
