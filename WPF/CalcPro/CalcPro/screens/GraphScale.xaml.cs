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

namespace CalcPro.Screens
{
    public partial class GraphScale : Page
    {
        public GraphScale()
        {
            InitializeComponent();

            textBoxXMin.Text = Globals.dbXmin.ToString();
            textBoxXMax.Text = Globals.dbXMax.ToString();
            textBoxYMin.Text = Globals.dbYMin.ToString();
            textBoxYMax.Text = Globals.dbYMax.ToString();

            textBlockTitle.Text = Globals.rm.GetString("IDS_MENU_Range");
            buttonReset.Content = Globals.rm.GetString("IDS_MENU_Reset"); 
        }

        private void ApplicationBarAccept_Click(object sender, EventArgs e)
        {
            //save the current values ... we'll let the graph screen load em back in

            try
            {
                Globals.dbXmin = Convert.ToDouble(textBoxXMin.Text);
                Globals.dbXMax = Convert.ToDouble(textBoxXMax.Text);
                Globals.dbYMin = Convert.ToDouble(textBoxYMin.Text);
                Globals.dbYMax = Convert.ToDouble(textBoxYMax.Text);
            }
            catch
            {
                buttonReset_Click(sender, new RoutedEventArgs());
            }
            NavigationService.GoBack();
        }

        private void ApplicationBarCancel_Click(object sender, EventArgs e)
        {
            NavigationService.GoBack();
        }

        private void buttonReset_Click(object sender, RoutedEventArgs e)
        {
            Globals.dbXMax = 0;
            Globals.dbXmin = 0;

            textBoxXMin.Text = (-5.0).ToString();
            textBoxXMax.Text = (5.0).ToString();

            textBoxYMin.Text = (-5.0 * 728 / 480).ToString();
            textBoxYMax.Text = (5.0 * 728 / 480).ToString();
        }
    }
}