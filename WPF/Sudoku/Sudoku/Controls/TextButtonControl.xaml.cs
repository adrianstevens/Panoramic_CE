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
using System.Windows.Navigation;

namespace BlackJack.Controls
{
    public partial class TextButtonControl : UserControl
    {
        public event EventHandler<MouseButtonEventArgs> OnClick;

        public TextButtonControl()
        {
            InitializeComponent();
        }

        /*public event EventHandler OnClick;

        private void TiltContentControl_ManipulationCompleted(object sender, ManipulationCompletedEventArgs e)
        {
            if(OnClick != null)
                OnClick(this, EventArgs.Empty);
        }*/

        public string Text
        {
            get { return txtMain.Text; }
            set { txtMain.Text = value; }
        }

        public bool Disabled
        {
            get { return !IsEnabled; }
            set {
                if(!value != IsEnabled)
                    txtMain.Foreground = !value ? Foreground : new SolidColorBrush(Colors.Gray);
                IsEnabled = !value;
            }
        }

        public void OnSelect()
        {
            var story = new Storyboard();

        //    Storyboard.SetTarget(story, txtMain);
            
            Storyboard.SetTargetProperty(story, new PropertyPath("(TextBlock.Foreground).(SolidColorBrush.Color)"));
            story.Children.Add(new ColorAnimation
            {

                From = Colors.Yellow,//ToColor(0xFF42FF00),
                To = Colors.White,
                Duration = TimeSpan.FromMilliseconds(400)
            });

            story.Begin(txtMain);
        }

        static public Color ToColor(uint argb)
        {
            return Color.FromArgb((byte)((argb & -16777216) >> 0x18), (byte)((argb & 0xff0000) >> 0x10), (byte)((argb & 0xff00) >> 8), (byte)(argb & 0xff));
        }

        private void UserControl_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            OnSelect();

            if (OnClick != null)
                OnClick(sender, e);
        }

    }
}
