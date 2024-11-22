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

namespace CalcPro.Controls
{
    /// <summary>
    /// Interaction logic for OptionControl.xaml
    /// </summary>
    public partial class OptionControl : UserControl
    {
        public string txtTitle
        {
            get { return textBox.Text; }
            set { textBox.Text = value; }
        }

        public delegate void UpdateIndexHandler(int iID, int iSelIndex);

        public UpdateIndexHandler IndexUpdate
        {
            get { return _IndexUpdate; }
            set { _IndexUpdate = value; }
        }
        private UpdateIndexHandler _IndexUpdate;

        public int iRefID
        {
            get { return _iRefID; }
            set { _iRefID = value; }
        }
        private int _iRefID = 0;

        public OptionControl()
        {
            InitializeComponent();
        }

        public void AddItem(string szItem)
        {
            ComboBoxItem cItem = new ComboBoxItem();
            cItem.Content = szItem;
            cItem.Foreground = new SolidColorBrush(Colors.White);
            cItem.Background = new SolidColorBrush(Colors.Black);
            cItem.FontSize = 16;
            comboBox.Items.Add(cItem);
        }

        public void SetCurrentIndex(int index)
        {
            comboBox.SelectedIndex = index;
        }

        private void comboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (IndexUpdate != null)
                IndexUpdate(iRefID, ((ComboBox)sender).SelectedIndex);
        }
    }
}
