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
    /// Interaction logic for GuitarNeck.xaml
    /// </summary>
    public partial class GuitarNeck : UserControl
    {
        public GuitarNeck()
        {
            InitializeComponent();

            Layout_Root.Background = Globals.gui.GetBackground();
        }
    }
}
