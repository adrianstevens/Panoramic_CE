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
using System.Windows.Threading;

namespace BlackJack.Controls
{
    public partial class FlipImage : UserControl
    {
        public Card Card { get; set; }
        public CardImage Image { get; set; }

        private Canvas Canvas { get; set; }

        public FlipImage(Canvas canvas)
        {
            InitializeComponent();
            Canvas = canvas;
        }

        private void canvasBackground_Loaded(object sender, RoutedEventArgs e)
        {
            imgBackground.Source = Globals.Gui.Skin.imgCardFlip;

        }

        private double _flipX;

        public void Flip(int delay)
        {
            _flipX = 0;

            Image.Opacity = 0;
            imgBackground.SetValue(Canvas.LeftProperty, 0.0);
            imgBackground.SetValue(Canvas.TopProperty, 0.0);

            var dt = new DispatcherTimer();

            dt.Interval = new TimeSpan(0, 0, 0, 0, delay);
            dt.Tick += FlipStop;

            dt.Start();
        }

        private void FlipStop(object o, EventArgs e)
        {
             var dt = o as DispatcherTimer;
  
             dt.Interval = TimeSpan.FromMilliseconds(10);

            _flipX += Card.WIDTH;

            if(_flipX >= (12 * Card.WIDTH))
            {
                dt.Stop();

                Canvas.Children.Remove(this);

                Image.Opacity = 1;

                Image.SetBackground(Card, 0);

                return;
            }

            Opacity = 1;

            imgBackground.SetValue(Canvas.LeftProperty, _flipX * -1);
        }
        public void Reset()
        {
            imgBackground.Opacity = 0;
        }
    }
}
