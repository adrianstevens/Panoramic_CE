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
    /// Interaction logic for SideBar.xaml
    /// </summary>
    public partial class SideBar : UserControl
    {
        public delegate void ScreenSet(EnumScreens eScreen);

        public ScreenSet SetScreen = null;

        public EnumScreens eSelected
        {
            get
            {
                return _eSel;
            }
            set
            {
                _eSel = value;
                UpdateSelector();
            }
        }
        EnumScreens _eSel;

        Image imgSelector = null;

        public SideBar()
        {
            InitializeComponent();

            //load the selector
            imgSelector = new Image();
            imgSelector.Source = new BitmapImage(new Uri("pack://application:,,/Assets/Interface/Selector.png"));
            imgSelector.Width = 70;
            imgSelector.Height = 62;
            imgSelector.Stretch = Stretch.Uniform;

            Grid.SetRow(imgSelector, 0);
            Layout_Menu.Children.Add(imgSelector);
        }

        private void Layout_Root_Loaded(object sender, RoutedEventArgs e)
        {
            ImageBrush brush = new ImageBrush();
            brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Interface/Menu_left.png"));

            Layout_Root.Background = brush;

            for (int i = 0; i < 5; i++)
            {
                int iTemp = i;
                if (i == 4)
                    iTemp++;

                MenuIcon myIcon = new MenuIcon((EnumScreens)iTemp);
                Grid.SetRow(myIcon, i);

                Layout_Menu.Children.Add(myIcon);

                myIcon.MouseLeftButtonUp += delegate(object o, MouseButtonEventArgs args)
                {
                    if (SetScreen != null)
                    {
                        SetScreen((EnumScreens)iTemp);
                        eSelected = (EnumScreens)iTemp;
                        UpdateSelector();
                    }
                };
            }

          /*  for (int i = 0; i < 6; i++)
            {
                MenuIcon myIcon = new MenuIcon((EnumScreens)i);
                Grid.SetRow(myIcon, i);

                Layout_Menu.Children.Add(myIcon);

                int iTemp = i;

                myIcon.MouseLeftButtonUp += delegate(object o, MouseButtonEventArgs args)
                {
                    if (SetScreen != null)
                    {
                        SetScreen((EnumScreens)iTemp);
                        eSelected = (EnumScreens)iTemp;
                        UpdateSelector();
                    }
                };
            }  */

            MenuIcon myIco = new MenuIcon(EnumScreens.Settings);
            Grid.SetRow(myIco, 1);
            myIco.MouseLeftButtonUp += delegate(object o, MouseButtonEventArgs args)
            {
                if (SetScreen != null)
                {
                    SetScreen(EnumScreens.Settings);
                    eSelected = EnumScreens.Settings;
                    UpdateSelector();
                }
            };

            Layout_Root.Children.Add(myIco);
            
        }

        public void UpdateSelector()
        {
            //remove
            Layout_Menu.Children.Remove(imgSelector);
            Layout_Root.Children.Remove(imgSelector);

            if (eSelected == EnumScreens.Metronome)
            {
                Grid.SetRow(imgSelector, 4);
                Layout_Menu.Children.Insert(0, imgSelector);
            }

            else if (eSelected != EnumScreens.Settings)
            {
                Grid.SetRow(imgSelector, (int)eSelected);
                Layout_Menu.Children.Insert(0, imgSelector);
                
            }
            else
            {
                Grid.SetRow(imgSelector, 1);
                Layout_Root.Children.Insert(0, imgSelector);
            }


        }
    }
}
