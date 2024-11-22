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
    //#define USE_FADE
    //#define USE_FLIP
    public partial class CardImage : UserControl
    {

        public CardImage()
        {
            InitializeComponent();
            imgBackground.Opacity = 0;
        }

        private void canvasBackground_Loaded(object sender, RoutedEventArgs e)
        {
            imgBackground.Source = Globals.Gui.Skin.imgCards;
        }

        private void SetVisibleStart(int millis)
        {
            var dt = new DispatcherTimer();
            dt.Interval = new TimeSpan(0, 0, 0, 0, millis);
            dt.Tick += SetVisibleStop;

            dt.Start();
        }

        public void SetReversed(int delay)
        {
            _x = 0;

            _y = 4 * Card.HEIGHT;

            imgBackground.SetValue(Canvas.LeftProperty, _x * -1);
            imgBackground.SetValue(Canvas.TopProperty, _y * -1);

            if (delay == 0)
            {
                imgBackground.Opacity = 1;
                return;
            }
#if USE_FADE
            FadeIn(delay);
#else
            SetVisibleStart(delay);
#endif
        }
        private double _x, _y;

        private void GetIndexXY(Card card)
        {
            _x = 0;

            if (card.Rank != EnumCardRank.Ace)
            {
                _x = 1274.0 - ((card.GetCardRank() - 1) * Card.WIDTH);
            }

            _y = card.GetCardSuit() * Card.HEIGHT;

        }

        private void PlayFlip(object o, EventArgs e)
        {
            BJEngine.PlaySound(AppSettings.SOUND_FLIP);
        }

        public void SetBackground(Card card, int millis)
        {
            GetIndexXY(card);

            imgBackground.SetValue(Canvas.LeftProperty, _x * -1);
            imgBackground.SetValue(Canvas.TopProperty, _y * -1);

            if (millis == 0)
            {
                imgBackground.Opacity = 1;
                BJEngine.PlaySound(AppSettings.SOUND_FLIP);
                return;
            }
#if USE_FADE
            FadeIn(millis);
#else
            SetVisibleStart(millis);
#endif
        }

        private void SetVisibleStop(object o, EventArgs args)
        {
            var dt = o as DispatcherTimer;
            dt.Stop();

            BJEngine.PlaySound(AppSettings.SOUND_FLIP);

            imgBackground.Opacity = 1;

        }

        private void FadeIn(int millis)
        {
            Storyboard story = new Storyboard();

            var dur = new Duration(new TimeSpan(0, 0, 0, 0, 300));
            var ani = new DoubleAnimation
            {
                From = 0,
                To = 1,
                Duration = dur
            };

            Storyboard.SetTarget(ani, imgBackground);
            Storyboard.SetTargetProperty(story, new PropertyPath("imgBackground.Opacity"));

            story.Children.Add(ani);

            story.BeginTime = new TimeSpan(0, 0, 0, 0, millis);
            story.Completed += PlayFlip;

            story.Begin();
        }

        public void Flip(Canvas canvas, Card card)
        {
            Flip(canvas, card, 0);
        }

        public void Flip(Canvas canvas, Card card, int delay)
        {
            FlipImage flipper = new FlipImage(canvas);
            flipper.Opacity = 0;
            flipper.Image = this;
            flipper.Card = card;

            canvas.Children.Add(flipper);

            Canvas.SetZIndex(flipper, 99);
            Canvas.SetTop(flipper, Canvas.GetTop(this) - 63);
            Canvas.SetLeft(flipper, Canvas.GetLeft(this));

            flipper.Flip(delay);
        }

        public void Reset()
        {
            imgBackground.Opacity = 0;
        }
    }
}
