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

namespace Sudoku
{
    public partial class NumberSelector : UserControl
    {
        private Storyboard _storyAni = null;
        private Storyboard _storyAniDown = null;
        private const int CONST_VerticalOffset = 30;//47;
        private bool _bBtnDown = false;
        private bool _bAniUpDown = false;

        public int iNumber;

        public EventHandler<EventArgs> Selected;

        public NumberSelector()
        {
            InitializeComponent();

            _storyAni = new Storyboard();
            Storyboard.SetTarget(_storyAni, gridSelector);
            Storyboard.SetTargetProperty(_storyAni, new PropertyPath(Canvas.TopProperty));

            imgNumbersGrey.Opacity = 0.0;
            imgNumbersWhite.Opacity = 1.0;

            _storyAni.Completed += AniUp_Completed;
            
        }

        public void SetNumber(int iNum)
        {
            iNumber = iNum + 1;
            imgNumbersWhite.SetValue(Canvas.LeftProperty, iNum * -26d);
            imgNumbersGrey.SetValue(Canvas.LeftProperty, iNum * -26d);
        }

        public void UpdateNumber()
        {
            bool bDrawWhite = Globals.Game.IsPencilMarkValid(iNumber, Globals.Settings.iSelectorX, Globals.Settings.iSelectorY);

            // if we are already the same
            if (bDrawWhite && imgNumbersWhite.Opacity == 1.0 || !bDrawWhite && imgNumbersGrey.Opacity == 1.0)
                return;

            imgNumbersGrey.Opacity = bDrawWhite ? 0.0 : 1.0;
            imgNumbersWhite.Opacity = bDrawWhite ? 1.0 : 0.0;
        }

        public void AnimateUpDown(int iMSDelay)
        {
            _bAniUpDown = true;
            AnimateUp(iMSDelay);
        }

        public void AnimateUp(int iMSDelay)
        {         
            double dbYFrom = Canvas.GetTop(gridSelector);

            if (_storyAni.Children.Count > 0)
            {
                _storyAni.Stop();
                _storyAni.Children.Clear();
            }

            _storyAni.Children.Add(new DoubleAnimation
            {
                //Duration = new Duration(TimeSpan.FromMilliseconds(300 + iMSDelay)),
                Duration = new Duration(TimeSpan.FromMilliseconds(50)),
                From = dbYFrom,
                To = 0,
            });

            //_storyAni.BeginTime = new TimeSpan(0, 0, 0, 0, iMSDelay);
            _storyAni.Begin();

        }

        private void AniUp_Completed(object sender, EventArgs e)
        {
            if (!_bAniUpDown)
                return;

            _bAniUpDown = false;

            double dbYFrom = Canvas.GetTop(gridSelector);
          //Adrian bugbug  if(dbYFrom == 0)
                AnimateDown();
        }

        private void AnimateDown()
        {       

            double dbYFrom = gridSelector.Margin.Top;

            if (_storyAni.Children.Count > 0)
            {
                _storyAni.Stop();
                _storyAni.Children.Clear();
            }

            _storyAni.Children.Add(new DoubleAnimation
            {
                Duration = new Duration(TimeSpan.FromMilliseconds(400)),
                From = dbYFrom,
                To = CONST_VerticalOffset,
            });

            _storyAni.Begin();
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            
        }

        new public void MouseEnter()
        {
        /*    if (_bBtnDown == true)
                return;

            _bBtnDown = true;
            AnimateUp(0);*/

        }

        new public void MouseLeave()
        {
        /*    if (_bBtnDown == false) 
                return;

            _bBtnDown = false;
            AnimateDown();*/

        }

        new public void MouseLeftButtonUp()
        {
            //_bBtnDown = false;
            //AnimateDown();

            // if we're disabled don't select ... to restrictive
          /*  if (imgNumbersGrey.Opacity == 1.0)
                return;*/

            if (Selected != null)
                Selected(this, new EventArgs());

            //AnimateUpDown(0);
        }

        private void LayoutRoot_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            gridSelector.Width = LayoutRoot.ActualWidth;
            gridSelector.Height = LayoutRoot.ActualHeight - CONST_VerticalOffset;

        }


    }
}
