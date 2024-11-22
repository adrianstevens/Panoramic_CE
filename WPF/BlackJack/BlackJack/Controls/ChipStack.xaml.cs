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

namespace BlackJack.Controls
{
    public partial class ChipStack : UserControl
    {

        public ChipStack()
        {
            InitializeComponent();
        }

        private void canvasBackground_Loaded(object sender, RoutedEventArgs e)
        {
            
        }

        private int _count;

        public void AddChip()
        {
            _count++;

            imgBackground.Opacity = 1;

            imgBackground.SetValue(Canvas.LeftProperty, (_count - 1) * 75.0 * -1);
        }

        public void CopyChips(ChipStack stack)
        {
            Clear();

            while (_count < stack._count)
                AddChip();
        }

        //ie reset without an animation
        public void Clear()
        {
            _count = 0;
            imgBackground.SetValue(Canvas.LeftProperty, (_count - 1) * 75.0 * -1);
        }

        public void Reset()
        {
            _count = 0;

            imgBackground.Opacity = 0;

        /*    var story = new Storyboard();

            var ani = new DoubleAnimation
            {
                From = imgBackground.Opacity,
                To = 0,
                Duration = new Duration(new TimeSpan(0, 0, 0, 0, 400))
            };

            Storyboard.SetTarget(ani, imgBackground);
            Storyboard.SetTargetProperty(ani, new PropertyPath("imgBackground.Opacity"));

            story.Children.Add(ani);

            story.Begin();*/
            
        }

        public void SetBackground(BitmapImage img)
        {
          
            imgBackground.Source = img;
               
        }
    }
}

