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
using com.intel.adp;


namespace BlackJack
{
    /// <summary>
    /// Interaction logic for Main.xaml
    /// </summary>
    public partial class Main : Page
    {
        public Main()
        {
            InitializeComponent();
#if !DEBUG
        //    CheckIntel();
#endif
        }

        void CheckIntel()
        {
            try
            {
                //0x30328AB7,0xAA754CD6,0xAD1125E8,0x0F58377D
                AdpApplication app = new AdpApplication(new AdpApplicationId(0x30328AB7, 0xAA754CD6, 0xAD1125E8, 0x0F58377D));
                /// Note: replace "0x11111111, 0x11111111, 0x11111111, 0x11111111" with the actual application ID
                /// once you obtain it from Intel AppUp(SM) Developer Program web portal
            }
            catch (AdpException e)
            {
                if (e is AdpErrorException)
                {
                    // TO DO: add your logic to handle the errors during initialization
                    MessageBox.Show(e.Message, "Error - BlackJack is not authenticated.  You may need to connect to the Intel AppUp store.");
                    System.Environment.Exit(1);
                }
                else if (e is AdpWarningException)
                {
                    // TO DO: add your logic to handle the warnings
                    MessageBox.Show(e.Message, "Error - BlackJack is not authenticated.  You may need to connect to the Intel AppUp store.");
                }
            }
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            //LayoutRoot.Background = Globals.Gui.GetImageBrush(ref Globals.Gui.Skin.imgMainBackground);

            newGameControl1.Text = Globals.Res.GetString("IDS_MENU_NewGame");
            settingsButtonControl1.Source = Globals.Gui.Skin.imgSettings;
            resumeButtonControl1.Source = Globals.Gui.Skin.imgResume;
        }
        

        private void resumeButtonControl1_OnClick(object sender, MouseButtonEventArgs e)
        {
            this.NavigationService.Navigate(new Uri("/GameScreen.xaml", UriKind.Relative));
        }

        private void settingsButtonControl1_OnClick(object sender, MouseButtonEventArgs e)
        {
            this.NavigationService.Navigate(new Uri("/Options.xaml", UriKind.Relative));
        }

        private void newGameControl1_OnClick(object sender, MouseButtonEventArgs e)
        {
            Globals.Game.NewGame();

            this.NavigationService.Navigate(new Uri("/GameScreen.xaml", UriKind.Relative));

            //   NavigationService.Navigate(new Uri("/Screens/GameScreen.xaml", UriKind.Relative));

            BJEngine.PlaySound(AppSettings.SOUND_SHUFFLE);
        }
    }
}
