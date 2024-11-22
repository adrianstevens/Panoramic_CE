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
//using com.intel.adp;


namespace Sudoku
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : NavigationWindow
    {
        public Window1()
        {
            InitializeComponent();

#if !DEBUG
         //   CheckIntel();
#endif


            this.NavigationService.Navigate(new Uri("/MainMenu.xaml", UriKind.Relative));
        }

        void CheckIntel()
        {
            

    /*        try
            {
                //0x5ABE19D9,0xF4B840C2,0xA0974C5A,0xEE208DAB
                AdpApplication app = new AdpApplication(new AdpApplicationId(0x5ABE19D9, 0xF4B840C2, 0xA0974C5A, 0xEE208DAB));
                /// Note: replace "0x11111111, 0x11111111, 0x11111111, 0x11111111" with the actual application ID
                /// once you obtain it from Intel AppUp(SM) Developer Program web portal
            }
            catch (AdpException e)
            {
                if (e is AdpErrorException)
                {
                    // TO DO: add your logic to handle the errors during initialization
                    MessageBox.Show(e.Message, "Error - Ultimate Sudoku is not authenticated.");
                    System.Environment.Exit(1);
                }
                else if (e is AdpWarningException)
                {
                    // TO DO: add your logic to handle the warnings
                    MessageBox.Show(e.Message, "Error - Ultimate Sudoku  is not authenticated.");
                    System.Environment.Exit(1);
                }
                System.Environment.Exit(1);
            }*/
        }
    }
}
