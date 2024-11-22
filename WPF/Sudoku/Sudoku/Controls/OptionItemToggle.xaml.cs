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
using System.Resources;
using System.Reflection;
using System.ComponentModel;

namespace Sudoku
{
    public partial class OptionItemToggle : UserControl, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public string szOnOff
        {
            get
            {
                return btnToggle.IsChecked == true ? Globals.rm.GetString("IDS_MENU_On") : Globals.rm.GetString("IDS_MENU_Off");
            }
        }

        public OptionItemToggle()
        {
            InitializeComponent();
            btnToggle.DataContext = this;
        }

        public void SetValue(bool bValue)
        {
            btnToggle.IsChecked = bValue;
        }
        public bool GetValue()
        {
            return (bool)btnToggle.IsChecked;
        }

        public int iRefID
        {
            get { return _iRefID; }
            set { _iRefID = value; }
        }
        private int _iRefID = 0;

        public delegate void UpdateIndexHandler(int iID, int iSelIndex);

        public UpdateIndexHandler IndexUpdate
        {
            get { return _IndexUpdate; }
            set { _IndexUpdate = value; }
        }
        private UpdateIndexHandler _IndexUpdate;

        private void btnToggle_Click(object sender, RoutedEventArgs e)
        {
            if(IndexUpdate != null)
                IndexUpdate(iRefID, Convert.ToInt32(btnToggle.IsChecked));
        }

        private void btnToggle_Checked(object sender, RoutedEventArgs e)
        {
            NotifyPropertyChanged("szOnOff");
        }

        private void btnToggle_Unchecked(object sender, RoutedEventArgs e)
        {
            NotifyPropertyChanged("szOnOff");
        }

        private void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this,
                    new PropertyChangedEventArgs(propertyName));
            }

        }
    }
}
