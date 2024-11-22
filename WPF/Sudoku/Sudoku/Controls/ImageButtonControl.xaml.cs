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
using System.Windows.Media.Imaging;

namespace Sudoku.Controls
{
    public partial class ImageButtonControl : UserControl
    {
        public event EventHandler<MouseButtonEventArgs> OnClick;

        public ImageButtonControl()
        {
            InitializeComponent();

        }

        /*public event EventHandler OnClick;

        private void TiltContentControl_ManipulationCompleted(object sender, ManipulationCompletedEventArgs e)
        {
            if (OnClick != null)
                OnClick(this, EventArgs.Empty);
        }*/

        public BitmapImage Source
        {
            get 
            { 
                return (BitmapImage) imgMain.Source; 
            }
            set
            {
                imgMain.Source = value;
                if (value.PixelHeight > 0)
                {
                    imgMain.Height = value.PixelHeight;
                    imgMain.Width = value.PixelWidth;
                }
            }
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {

            var img = new BitmapImage(new Uri(@"../Assets/appbar.baseCircle.rest.png", UriKind.Relative));//Globals.Gui.Skin.imgBaseCircle;
            baseCircle.Source = img;
            baseCircle.Height = img.PixelHeight;
            baseCircle.Width = img.PixelWidth;
         
        }

        private void LayoutMain_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (OnClick != null)
                OnClick(sender, e);
        }
    }
}
