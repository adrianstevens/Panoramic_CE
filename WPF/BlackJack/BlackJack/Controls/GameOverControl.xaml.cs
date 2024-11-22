using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace BlackJack.Controls
{
    public partial class GameOverControl : UserControl
    {
        private FrameworkElement _ctrlItem;
        private Grid _gridParentLayout;

        bool _bRet = false;

        public GameOverControl(FrameworkElement ctrlItem)
        {
            InitializeComponent();
            textBroke.Text = Globals.Res.GetString("IDS_MSG_BrokeShort");
            _ctrlItem = ctrlItem;
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            var dt = new DispatcherTimer();
            dt.Interval = TimeSpan.FromMilliseconds(2000);
            dt.Tick += delegate(object o, EventArgs args)
            {
                dt.Stop();

                var bHighScore = false;

                //Pop the end game message and handle
                if (Globals.Game.BankHi > BJEngine.HISCORE_DEFAULT)
                {
                    bHighScore = true;
                    var iBank = Globals.Game.BankHi;

                    //we'll leave that in the game screen
                    //Globals.Settings.HighScores.Add(null, Globals.Game.BankHi);
                }

                var msg = Globals.Res.GetString(bHighScore ? "IDS_MSG_HiEndGame" : "IDS_MSG_Broke");

                var res = MessageBox.Show(msg, "Panoramic BlackJack", MessageBoxButton.OKCancel);

                if (res == MessageBoxResult.OK)
                {
                    Globals.Game.NewGame(false);
                    Globals.Game.LastResult = null;
                    _bRet = true;
                    CloseScreen();
                }
                else
                {  //go to the Menu
                    _bRet = false;
                    CloseScreen();
                    
                              //     NavigationService.Navigate(new Uri("/Screens/MainScreen.xaml", UriKind.Relative));
                }
            };
            dt.Start();//seems backwards but needs to be started
        }

        public bool Launch(ref Grid layoutroot)
        {
            if (layoutroot == null)
                return false;

            _gridParentLayout = layoutroot;

            var dt = new DispatcherTimer();
            dt.Interval = TimeSpan.FromMilliseconds(750);
            dt.Tick += delegate(object o, EventArgs args)
            {
               // Grid.SetZIndex(this, 0);
                Grid.SetRowSpan(this, 2);
                _gridParentLayout.Children.Add(this);
                dt.Stop();

            };
            dt.Start();
            return true;
        }

        public bool Launch()
        {
            // get the parent page we need
            UIElement el = _ctrlItem as UIElement;

            if (!(el is Page))
            {
                el = VisualTreeHelper.GetParent(_ctrlItem) as UIElement;
                while (!(el is Page))
                {
                    el = VisualTreeHelper.GetParent(el) as UIElement;
                }
            }
           
           _gridParentLayout = VisualTreeHelper.GetChild(el, 0) as Grid;

           if (_gridParentLayout != null)
            _gridParentLayout.Children.Add(this);

            return _bRet;
        }

        public void CloseScreen()
        {
            if (_gridParentLayout == null)
                return;

          //  Dispatcher.BeginInvoke(() =>
            {
                _gridParentLayout.Children.Remove(this);

                GameScreen screen = (GameScreen)_ctrlItem;
                if(_bRet == false)
                    screen.NavBack();
            }
            //);
        }
    }
}
