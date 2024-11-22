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
    public partial class SelectorImage : UserControl
    {
        private Storyboard story;

        public SelectorImage()
        {
            InitializeComponent();
        }

        public void Start()
        {
            if(story != null)
            {
                story.Stop();
            }

            Opacity = 1;

            story = new Storyboard();

            var ani = new DoubleAnimation
            {
                From = Canvas.GetTop(this),
                To = Canvas.GetTop(this) + 10,
                Duration = new Duration(new TimeSpan(0, 0, 0, 0, 400))
            };

            Storyboard.SetTarget(ani, this);
            Storyboard.SetTargetProperty(ani, new PropertyPath(Canvas.TopProperty));

            story.Children.Add(ani);
            story.AutoReverse = true;
            story.RepeatBehavior = RepeatBehavior.Forever;

            story.Begin();
        }

        public void Stop()
        {
            if (story != null)
            {
                story.Stop();
                story = null;
            }
            Opacity = 0;
        }
    }
}
