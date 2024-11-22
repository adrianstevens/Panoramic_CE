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

namespace BlackJack.Controls
{
    public partial class DollarImage : UserControl
    {
        private double _startY;

        public DollarImage()
        {
            InitializeComponent();
        }

        public void Start()
        {
            var story = new Storyboard();

            var dur = new Duration(TimeSpan.FromMilliseconds(800));

            var ani = new DoubleAnimation
            {
                From = 1,
                To = 0,
                Duration = dur
            };

            var ani2 = new DoubleAnimation
            {
                From = Canvas.GetTop(this),
                To = _startY - 40,
                Duration = dur
            };

            Storyboard.SetTarget(ani, this);
            Storyboard.SetTargetProperty(ani, new PropertyPath(DollarImage.OpacityProperty));

            Storyboard.SetTarget(ani2, this);
            Storyboard.SetTargetProperty(ani2, new PropertyPath(Canvas.TopProperty));

            story.Children.Add(ani);
            story.Children.Add(ani2);

            story.Completed += new EventHandler(story_Completed);
            story.Begin();
        }

        void story_Completed(object sender, EventArgs e)
        {
            Canvas.SetTop(this, _startY);
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            _startY = Canvas.GetTop(this);
        }
    }
}
