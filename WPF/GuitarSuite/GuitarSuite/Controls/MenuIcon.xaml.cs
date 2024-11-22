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

namespace GuitarSuite.Controls
{
    /// <summary>
    /// Interaction logic for MenuIcon.xaml
    /// </summary>
    public partial class MenuIcon : UserControl
    {
        EnumScreens eIcon = EnumScreens.GuitarChords;

        Image img = new Image();

        public MenuIcon(EnumScreens ico)
        {
            InitializeComponent();

            eIcon = ico;
       
            //set the image to the correct "frame" 
            myImage.SetValue(Canvas.LeftProperty, -1d * 50f * (int)ico);

        }
    }
}
