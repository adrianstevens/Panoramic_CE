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
using System.Diagnostics;

namespace BlackJack.Controls
{
    public partial class PlayerHand : UserControl
    {
        private int _playedCount;

    //    BetStack betStack = new BetStack();
    //    BetStack doubleStack = new BetStack();

        public PlayerHand()
        {
            InitializeComponent();
        }

        public CardImage this[int index]
        {
            get
            {
                if (index >= canvasPlayer.Children.Count)
                    return null;

                return canvasPlayer.Children[index] as CardImage;
            }
        }

        public bool FlipNewCards(BJHand hand)
        {
            int delay = 0;

            while (_playedCount < hand.Count)
            {
                Globals.Game.DealAnotherCard();

                var img = canvasPlayer.Children[_playedCount] as CardImage;
                
                
                img.SetBackground(hand[_playedCount], delay);

                delay += 600;

                _playedCount++;
            }

            return delay > 0;
        }


        public void HideScore()
        {
            var story = new Storyboard();

            var ani = new DoubleAnimation
            {
                From = borScore.Opacity,
                To = 0,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            var ani2 = new DoubleAnimation
            {
                From = lblScore.Opacity,
                To = 0,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

         //   Storyboard.SetTarget(ani, borScore.Background);
         //   Storyboard.SetTargetProperty(ani, new PropertyPath(Brush.OpacityProperty));
         //     Storyboard.SetTarget(ani, borScore);
         //     Storyboard.SetTargetProperty(ani, new PropertyPath(Border.OpacityProperty));
         //     Storyboard.SetTarget(ani, borScore.Background);
         //     Storyboard.SetTargetProperty(ani, new PropertyPath(Background.Opacity));

            Storyboard.SetTarget(ani, borScore);
            Storyboard.SetTargetProperty(ani, new PropertyPath(Border.OpacityProperty));



            Storyboard.SetTarget(ani2, lblScore);
            Storyboard.SetTargetProperty(ani2, new PropertyPath(TextBlock.OpacityProperty));

            story.Children.Add(ani);
         //   story.Children.Add(ani2);

            story.Begin();
        }


        public void SetScore(BJHand hand)
        {
            int iHi, iLow;
            hand.GetScore(out iLow, out iHi);

            if (iHi < 22 &&
                iLow != iHi)
            {	//show both scores
                lblScore.Text = iLow + " (" + iHi + ")";
            }
            else
            {
                lblScore.Text = iLow.ToString();
            }

            if (iLow == 0)
                return;

        //    if (lblScore.Opacity != 0)
        //        return;

            var story = new Storyboard();

            var ani = new DoubleAnimation
            {
                From = 0,
                To = 1,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

      //      Storyboard.SetTarget(ani, borScore.Background);
      //      Storyboard.SetTargetProperty(ani, new PropertyPath(Background.Opacity));

            Storyboard.SetTarget(ani, borScore);
            Storyboard.SetTargetProperty(ani, new PropertyPath(Border.OpacityProperty));


            var ani2 = new DoubleAnimation
            {
                From = 0,
                To = 1,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            Storyboard.SetTarget(ani2, lblScore);
            Storyboard.SetTargetProperty(ani2, new PropertyPath(TextBlock.OpacityProperty));

         

            story.Children.Add(ani);
        //    story.Children.Add(ani2);

            story.Begin();
        }

        public void Add25Chip()
        {
            betStack.chipsBlue.AddChip();
        }

        public void Add50Chip()
        {
            betStack.chipsRed.AddChip();
        }

        public void Add100Chip()
        {
            betStack.chipsGreen.AddChip();
        }

        public void Reset()
        {
            foreach (CardImage c in canvasPlayer.Children)
                c.Reset();

            if (doubleStack.Opacity > 0)
            {
                var story = new Storyboard();

                var ani = new DoubleAnimation
                {
                    From = Canvas.GetLeft(betStack),
                    To = Canvas.GetLeft(betStack) + 40,
                    Duration = new Duration(TimeSpan.FromMilliseconds(400))
                };

                Storyboard.SetTarget(ani, betStack);
                Storyboard.SetTargetProperty(ani, new PropertyPath(Canvas.LeftProperty));

                story.Children.Add(ani);

                var ani2 = new DoubleAnimation
                {
                    From = doubleStack.Opacity,
                    To = 0,
                    Duration = new Duration(TimeSpan.FromMilliseconds(400))
                };

                Storyboard.SetTarget(ani2, doubleStack);
                Storyboard.SetTargetProperty(ani2, new PropertyPath(BetStack.OpacityProperty));

                story.Children.Add(ani2);

                story.Completed += delegate(object o, EventArgs args)
                {
                    doubleStack.ResetChips();
                };
                story.Begin();
            }
        }
        public void Add500Chip()
        {
            betStack.chipsBlack.AddChip();

        }


        public void DoubleDown()
        {
            var story = new Storyboard();

            var ani = new DoubleAnimation
            {
                From = Canvas.GetLeft(betStack),
                To = Canvas.GetLeft(betStack) - 40,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };
            Storyboard.SetTarget(ani, betStack);
            Storyboard.SetTargetProperty(ani, new PropertyPath(Canvas.LeftProperty));

            story.Children.Add(ani);

            doubleStack.CopyChips(betStack);

            var ani2 = new DoubleAnimation
            {
                From = doubleStack.Opacity,
                To = 1,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            Storyboard.SetTarget(ani2, doubleStack);
            Storyboard.SetTargetProperty(ani2, new PropertyPath(BetStack.OpacityProperty));

            story.Children.Add(ani2);

            story.Begin();
        }

        public void CopyChips(PlayerHand hand)
        {
            betStack.CopyChips(hand.betStack);
        }
        public int DealHand(BJHand hand, int startDelay, int delay)
        {
            int d = startDelay;

            _playedCount = 0;

            foreach (var card in hand)
            {
                CardImage img = canvasPlayer.Children[_playedCount++] as CardImage;

                img.SetBackground(card, d);

                Globals.Game.DealAnotherCard();

                d += delay;

                Debug.WriteLine("Dealing player " + _playedCount + " " + card);

                if (_playedCount >= canvasPlayer.Children.Count)
                    break;
            }
            return d;
        }


        public void ClearChips()
        {
            betStack.ClearChips();

        }

        public void ResetChips()
        {
            betStack.ResetChips();
        }

        public void Hide()
        {
            var story = new Storyboard();

            var ani = new DoubleAnimation
            {
                From = Opacity,
                To = 0,
                Duration = new Duration(new TimeSpan(0, 0, 0, 0, 400))
            };

            Storyboard.SetTarget(ani, this);
            Storyboard.SetTargetProperty(ani, new PropertyPath(PlayerHand.OpacityProperty));

            story.Children.Add(ani);

            story.Begin();
        }

        public void Show()
        {
            var story = new Storyboard();

            var ani = new DoubleAnimation
            {
                From = Opacity,
                To = 1,
                Duration = new Duration(TimeSpan.FromMilliseconds(400))
            };

            Storyboard.SetTarget(ani, this);
            Storyboard.SetTargetProperty(ani, new PropertyPath(PlayerHand.OpacityProperty));

            story.Children.Add(ani);

            story.Begin();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void UserControl_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            int p = 0;
        }
        
    }
}
