using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace CalcPro
{
    public class ButtonSkin : UserControl, ICalcProBtn
    {        
        public EventHandler<EventArgs> BtnSelected{get;set;}
        public CalcProBtn cBtn{get;set;}

        private Grid LayoutRoot;
        private TextBlock MainText;
        private TextBlock SubText;

        private TextBlock _2ndText; //can't lead with numbers ....
        private TextBlock _2ndExpText; //(maybe ... )


        private EnumCalcProSkinColor _eSkinColor;
        private EnumCalcProSkinStyle _eSkinStyle;
        //private EnumCalcProSkins _eSkin;
        private int _iSkinIndex;
        private bool bBtnDown = false;
        private IButtonSkin btnSkin = null;
        //private TiltItem tilt = null;

        public ButtonSkin(EnumCalcProSkinColor eColor, EnumCalcProSkinStyle eStyle, int iSkinIndex)
        {
            cBtn = new CalcProBtn();
            LayoutRoot = new Grid();
            MainText = new TextBlock();
            SubText = new TextBlock();
            _2ndText = new TextBlock();
            _2ndExpText = new TextBlock();

            _eSkinColor = eColor;
            _eSkinStyle = eStyle;
            _iSkinIndex = iSkinIndex;

            CreateButtonSkin();

            // events
            LayoutRoot.MouseLeave += LayoutRoot_MouseLeave;
            LayoutRoot.MouseLeftButtonDown += LayoutRoot_MouseLeftButtonDown;
            LayoutRoot.MouseLeftButtonUp += LayoutRoot_MouseLeftButtonUp;
            LayoutRoot.SizeChanged += LayoutRoot_SizeChanged;

            LayoutRoot.Children.Add(MainText);
            LayoutRoot.Children.Add(SubText);
            LayoutRoot.Children.Add(_2ndText);
            LayoutRoot.Children.Add(_2ndExpText);

            this.Content = LayoutRoot;
        }        
        public bool SetMainText(string szMain)
        {
            MainText.Text = szMain;
            return true;
        }
        public bool SetExpText(string szExp)
        {
            SubText.Text = szExp;
            return true;
        }

        public bool Set2ndText(string szMain)
        {
            _2ndText.Text = szMain;
            return true;
        }

        public bool Set2ndExpText(string szExp)
        {
            _2ndExpText.Text = szExp;
            return true;
        }


        private void CreateButtonSkin()
        {
            switch (Globals.Settings.eSkinStyle)
            {
                case EnumCalcProSkinStyle.Classic:
                    btnSkin = new ButtonSkinClassic();
                    break;
                case EnumCalcProSkinStyle.Shine:
                    btnSkin = new ButtonSkinShine();
                    break;
                case EnumCalcProSkinStyle.Type:
                    btnSkin = new ButtonSkinType();
                    break;
                case EnumCalcProSkinStyle.Flat:
                    btnSkin = new ButtonSkinMetro();
                    break;
                case EnumCalcProSkinStyle.Standard:
                    btnSkin = new ButtonSkinMetro2();
                    break;

            }

            btnSkin.CreateButtonSkin(ref LayoutRoot, ref MainText, ref SubText, ref _2ndText, ref _2ndExpText, _eSkinColor, _eSkinStyle, ref _iSkinIndex);

            _2ndText.Opacity = 0.7;
            _2ndExpText.Opacity = 0.7;
        }

        private void LayoutRoot_MouseLeave(object sender, MouseEventArgs e)
        {
            bBtnDown = false;
            btnSkin.DrawUpState(ref LayoutRoot, ref MainText, ref SubText, ref _2ndText, ref _2ndExpText, _eSkinColor, _eSkinStyle, ref _iSkinIndex);
        }

        private void LayoutRoot_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            bBtnDown = true;
            btnSkin.DrawDownState(ref LayoutRoot, ref MainText, ref SubText, ref _2ndText, ref _2ndExpText, _eSkinColor, _eSkinStyle, ref _iSkinIndex);
        }

        private void LayoutRoot_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (!bBtnDown)
                return;

            bBtnDown = false;
            btnSkin.DrawUpState(ref LayoutRoot, ref MainText, ref SubText, ref _2ndText, ref _2ndExpText, _eSkinColor, _eSkinStyle, ref _iSkinIndex);

            if (this.BtnSelected != null)
                this.BtnSelected(sender, new EventArgs());
        }

        private void LayoutRoot_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (LayoutRoot.ActualHeight == 0 || LayoutRoot.ActualWidth == 0)
                return;

            cBtn.AlignText(ref MainText, ref SubText, ref _2ndText, ref _2ndExpText, LayoutRoot.ActualHeight, LayoutRoot.ActualWidth);
            //cBtn.AlignText(ref MainText, ref SubText, LayoutRoot.ActualHeight, LayoutRoot.ActualWidth);
        }
    }
}
