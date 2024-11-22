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

namespace Sudoku
{
    public partial class OptionItemCategory : UserControl
    {
        public OptionItemCategory()
        {
            InitializeComponent();
        }

        public void SetTitle(string szTitle)
        {
            txtTitle.Text = szTitle;
        }
    }
}
