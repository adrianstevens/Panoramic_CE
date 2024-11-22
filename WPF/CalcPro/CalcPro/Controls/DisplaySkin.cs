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
using System.Windows.Data;
using System.Collections.Generic;

namespace CalcPro
{
    public class DisplaySkin : UserControl, ICalcProDisplay
    {
        private Grid LayoutRoot;
        private TextBlock txtStyle;
        private TextBlock txtDRG;
        private TextBlock txtBASE;
        private TextBlock txt2ndF;
        private TextBlock txtHyp;
        private TextBlock txtOperator;
        private TextBlock txtM;
        private TextBlock txtM2;
        private TextBlock txtM3;
        private TextBlock txtResult;
        private TextBlock txtValue;
        private TextBlock txtExp;
        private TextBlock txtEquation;
        private TextBlock txtRPNX;
        private TextBlock txtRPNY;
        private TextBlock txtRPNXValue;
        private TextBlock txtRPNYValue;

        private TextBlock txtNumerator;  
        private TextBlock txtDenominator; 
        private TextBlock txtWhole;

        private Line lineFrac;

        public DisplaySkin(EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eSkinStyle)
        {
            LayoutRoot = new Grid();
            txtStyle = new TextBlock();
            txtDRG = new TextBlock();
            txtBASE = new TextBlock();
            txt2ndF = new TextBlock();
            txtHyp = new TextBlock();
            txtOperator = new TextBlock();
            txtM = new TextBlock();
            txtM2 = new TextBlock();
            txtM3 = new TextBlock();
            txtResult = new TextBlock();
            txtValue = new TextBlock();
            txtExp = new TextBlock();
            txtEquation = new TextBlock();
            txtRPNX = new TextBlock();
            txtRPNY = new TextBlock();
            txtRPNXValue = new TextBlock();
            txtRPNYValue = new TextBlock();

            txtWhole = new TextBlock();
            txtNumerator = new TextBlock();
            txtDenominator = new TextBlock();
            lineFrac = new Line();

            // if we need a custom font
            Globals.btnMan.SetFontItem(ref txtStyle);
            Globals.btnMan.SetFontItem(ref txtDRG);
            Globals.btnMan.SetFontItem(ref txtBASE);
            Globals.btnMan.SetFontItem(ref txt2ndF);
            Globals.btnMan.SetFontItem(ref txtHyp);
            Globals.btnMan.SetFontItem(ref txtOperator);
            Globals.btnMan.SetFontItem(ref txtM);
            Globals.btnMan.SetFontItem(ref txtM2);
            Globals.btnMan.SetFontItem(ref txtM3);
            Globals.btnMan.SetFontItem(ref txtResult);
            Globals.btnMan.SetFontItem(ref txtValue);
            Globals.btnMan.SetFontItem(ref txtExp);
            Globals.btnMan.SetFontItem(ref txtEquation);
            Globals.btnMan.SetFontItem(ref txtRPNX);
            Globals.btnMan.SetFontItem(ref txtRPNY);
            Globals.btnMan.SetFontItem(ref txtRPNXValue);
            Globals.btnMan.SetFontItem(ref txtRPNYValue);
            Globals.btnMan.SetFontItem(ref txtWhole);//for fractions
            Globals.btnMan.SetFontItem(ref txtDenominator);
            Globals.btnMan.SetFontItem(ref txtNumerator);

            txtStyle.Height = 27;
            txtStyle.VerticalAlignment = VerticalAlignment.Top;
            txtStyle.HorizontalAlignment = HorizontalAlignment.Left;
            txtStyle.Margin = new Thickness(280, 2, 0, 0);
            txtStyle.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetAltTextColor());
            txtStyle.FontSize = 22;
            txtStyle.Visibility = Visibility.Collapsed;
            //txtStyle.SetBinding(TextBlock.TextProperty, new Binding("szDispType"));

            txtDRG.Height = 27;
            txtDRG.VerticalAlignment = VerticalAlignment.Top;
            txtDRG.HorizontalAlignment = HorizontalAlignment.Left;
            txtDRG.Margin = new Thickness(209, 2, 0, 0);
            txtDRG.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetAltTextColor());
            txtDRG.FontSize = 22;
            txtDRG.Visibility = Visibility.Collapsed;
            //txtDRG.SetBinding(TextBlock.TextProperty, new Binding("szDRG"));

            txtBASE.Height = 27;
            txtBASE.VerticalAlignment = VerticalAlignment.Top;
            txtBASE.HorizontalAlignment = HorizontalAlignment.Left;
            txtBASE.Margin = new Thickness(157, 2, 0, 0);
            txtBASE.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetAltTextColor());
            txtBASE.FontSize = 22;
            txtBASE.Visibility = Visibility.Collapsed;
            //txtBASE.SetBinding(TextBlock.TextProperty, new Binding("szBase"));

            txt2ndF.Height = 27;
            txt2ndF.VerticalAlignment = VerticalAlignment.Top;
            txt2ndF.HorizontalAlignment = HorizontalAlignment.Left;
            txt2ndF.Margin = new Thickness(42, 2, 0, 0);
            txt2ndF.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetAltTextColor());
            txt2ndF.FontSize = 22;
            txt2ndF.Text = "2ndF";
            txt2ndF.Visibility = Visibility.Collapsed;
            //txt2ndF.SetBinding(TextBlock.VisibilityProperty, new Binding("s2ndF"));

            txtHyp.Height = 34;
            txtHyp.VerticalAlignment = VerticalAlignment.Top;
            txtHyp.HorizontalAlignment = HorizontalAlignment.Left;
            txtHyp.Margin = new Thickness(103, 2, 0, 0);
            txtHyp.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetAltTextColor());
            txtHyp.FontSize = 22;
            txtHyp.Text = "Hyp";
            txtHyp.Visibility = Visibility.Collapsed;
            //txtHyp.SetBinding(TextBlock.VisibilityProperty, new Binding("Hyp"));

            txtOperator.Height = 27;
            txtOperator.VerticalAlignment = VerticalAlignment.Top;
            txtOperator.HorizontalAlignment = HorizontalAlignment.Left;
            txtOperator.Margin = new Thickness(14, 2, 0, 0);
            txtOperator.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetAltTextColor());
            txtOperator.FontSize = 22;
            txtOperator.Visibility = Visibility.Collapsed;
            //txtOperator.SetBinding(TextBlock.TextProperty, new Binding("szLastEntry"));

            txtM.Height = 27;
            txtM.VerticalAlignment = VerticalAlignment.Top;
            txtM.HorizontalAlignment = HorizontalAlignment.Left;
            txtM.Margin = new Thickness(368, 2, 0, 0);
            txtM.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetAltTextColor());
            txtM.FontSize = 22;
            txtM.Text = "M";
            txtM.Visibility = Visibility.Collapsed;
            //txtM.SetBinding(TextBlock.VisibilityProperty, new Binding("M1"));

            txtM2.Height = 27;
            txtM2.VerticalAlignment = VerticalAlignment.Top;
            txtM2.HorizontalAlignment = HorizontalAlignment.Left;
            txtM2.Margin = new Thickness(396, 2, 0, 0);
            txtM2.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetAltTextColor());
            txtM2.FontSize = 22;
            txtM2.Text = "M2";
            txtM2.Width = 35;
            txtM2.Visibility = Visibility.Collapsed;
            //txtM2.SetBinding(TextBlock.VisibilityProperty, new Binding("M2"));

            txtM3.Height = 27;
            txtM3.VerticalAlignment = VerticalAlignment.Top;
            txtM3.HorizontalAlignment = HorizontalAlignment.Left;
            txtM3.Margin = new Thickness(436, 2, 0, 0);
            txtM3.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetAltTextColor());
            txtM3.FontSize = 22;
            txtM3.Text = "M3";
            txtM3.Width = 35;
            txtM3.Visibility = Visibility.Collapsed;
            //txtM3.SetBinding(TextBlock.VisibilityProperty, new Binding("M3"));

            txtResult.Height = 53;
            txtResult.HorizontalAlignment = HorizontalAlignment.Right;
            txtResult.TextAlignment = TextAlignment.Right;
            txtResult.VerticalAlignment = VerticalAlignment.Bottom;
            txtResult.Margin = new Thickness(6, 0, 6, 0);
            //txtResult.SetBinding(TextBlock.TextProperty, new Binding("szResult"));
            txtResult.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            txtResult.FontSize = 40;
            txtResult.Visibility = Visibility.Collapsed;
            //txtResult.SetBinding(TextBlock.VisibilityProperty, new Binding("ResultNormal"));

            txtValue.Height = 53;
            txtValue.HorizontalAlignment = HorizontalAlignment.Right;
            txtValue.TextAlignment = TextAlignment.Right;
            txtValue.VerticalAlignment = VerticalAlignment.Bottom;
            txtValue.Margin = new Thickness(0, 0, 80, 0);
            //txtValue.SetBinding(TextBlock.TextProperty, new Binding("szValue"));
            txtValue.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            txtValue.FontSize = 40;
            txtValue.Visibility = Visibility.Collapsed;
            //txtValue.SetBinding(TextBlock.VisibilityProperty, new Binding("ResultExponent"));

            txtExp.Height = 53;
            txtExp.HorizontalAlignment = HorizontalAlignment.Right;
            txtExp.TextAlignment = TextAlignment.Right;
            txtExp.VerticalAlignment = VerticalAlignment.Bottom;
            txtExp.Margin = new Thickness(6, 0, 6, 0);
            //txtExp.SetBinding(TextBlock.TextProperty, new Binding("szExp"));
            txtExp.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            txtExp.FontSize = 32;
            txtExp.Width = 65;
            txtExp.Visibility = Visibility.Collapsed;
            //txtExp.SetBinding(TextBlock.VisibilityProperty, new Binding("ResultExponent"));

            txtEquation.Height = 53;
            txtEquation.Margin = new Thickness(12, 0, 12, 32);
            txtEquation.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            txtEquation.FontSize = 32;
            txtEquation.HorizontalAlignment = HorizontalAlignment.Right;
            txtEquation.TextAlignment = TextAlignment.Left;
            txtEquation.VerticalAlignment = VerticalAlignment.Bottom;
            txtEquation.TextWrapping = TextWrapping.NoWrap;
            txtEquation.Visibility = Visibility.Collapsed;
            //txtEquation.SetBinding(TextBlock.TextProperty, new Binding("szEq"));


            txtRPNX.Height = 44;
            txtRPNX.Margin = new Thickness(12, 0, 0, 0);
            txtRPNX.Text = "X:";
            txtRPNX.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            txtRPNX.FontSize = 32;
            txtRPNX.VerticalAlignment = VerticalAlignment.Bottom;
            txtRPNX.Visibility = Visibility.Collapsed;

            txtRPNY.Height = 72;
            txtRPNY.Margin = new Thickness(12, 0, 0, 0);
            txtRPNY.Text = "Y:";
            txtRPNY.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            txtRPNY.FontSize = 32;
            txtRPNY.VerticalAlignment = VerticalAlignment.Bottom;
            txtRPNY.Visibility = Visibility.Collapsed;

            txtRPNXValue.Height = 44;
            txtRPNXValue.Margin = new Thickness(6, 0, 6, 0);
            txtRPNXValue.HorizontalAlignment = HorizontalAlignment.Right;
            txtRPNXValue.TextAlignment = TextAlignment.Right;
            txtRPNXValue.VerticalAlignment = VerticalAlignment.Bottom;
            txtRPNXValue.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            txtRPNXValue.FontSize = 32;
            txtRPNXValue.Visibility = Visibility.Collapsed;
            //txtRPNXValue.SetBinding(TextBlock.TextProperty, new Binding("szResult"));

            txtRPNYValue.Height = 72;
            txtRPNYValue.Margin = new Thickness(6, 0, 6, 0);
            txtRPNYValue.HorizontalAlignment = HorizontalAlignment.Right;
            txtRPNYValue.TextAlignment = TextAlignment.Right;
            txtRPNYValue.VerticalAlignment = VerticalAlignment.Bottom;
            txtRPNYValue.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            txtRPNYValue.FontSize = 32;
            txtRPNYValue.Visibility = Visibility.Collapsed;
            //txtRPNYValue.SetBinding(TextBlock.TextProperty, new Binding("szRPNY"));


            txtWhole.Height = 53;
            txtWhole.Margin = new Thickness(6, 0, 60, 0);
            txtWhole.HorizontalAlignment = HorizontalAlignment.Right;
            txtWhole.TextAlignment = TextAlignment.Right;
            txtWhole.VerticalAlignment = VerticalAlignment.Bottom;
            txtWhole.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            txtWhole.FontSize = 40;
            //txtWhole.SetBinding(TextBlock.TextProperty, new Binding("szWhole"));
            txtWhole.Visibility = Visibility.Collapsed;


            txtNumerator.Height = 72;
            txtNumerator.Margin = new Thickness(12, 0, 6, 0);
            txtNumerator.HorizontalAlignment = HorizontalAlignment.Right;
            txtNumerator.TextAlignment = TextAlignment.Right;
            txtNumerator.Text = "Num";
            txtNumerator.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            txtNumerator.FontSize = 32;
            txtNumerator.VerticalAlignment = VerticalAlignment.Bottom;
            //txtNumerator.SetBinding(TextBlock.TextProperty, new Binding("szNumerator"));
            txtNumerator.Visibility = Visibility.Collapsed;

            txtDenominator.Height = 44;
            txtDenominator.Margin = new Thickness(12, 0, 6, 0);
            txtDenominator.HorizontalAlignment = HorizontalAlignment.Right;
            txtDenominator.TextAlignment = TextAlignment.Right;
            txtDenominator.Text = "Den";
            txtDenominator.Foreground = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            txtDenominator.FontSize = 32;
            txtDenominator.VerticalAlignment = VerticalAlignment.Bottom;
           // txtDenominator.SetBinding(TextBlock.TextProperty, new Binding("szDenominator"));
            txtDenominator.Visibility = Visibility.Collapsed;

            lineFrac.Stroke = Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
      //      lineFrac.Height = 100;
            lineFrac.Margin = new Thickness(12, 0, 6, 0);
            lineFrac.HorizontalAlignment = HorizontalAlignment.Right;
            lineFrac.VerticalAlignment = VerticalAlignment.Top;
            lineFrac.Visibility = Visibility.Collapsed;
            lineFrac.X1 = 416;
            lineFrac.X2 = 466;
            lineFrac.Y1 = 70;
            lineFrac.Y2 = 70;
            
            // create the background first
            CreateBackgroundDisplay(eSkinColor, eSkinStyle);

            LayoutRoot.Children.Add(txtStyle);
            LayoutRoot.Children.Add(txtDRG);
            LayoutRoot.Children.Add(txtBASE);
            LayoutRoot.Children.Add(txt2ndF);
            LayoutRoot.Children.Add(txtHyp);
            LayoutRoot.Children.Add(txtOperator);
            LayoutRoot.Children.Add(txtM);
            LayoutRoot.Children.Add(txtM2);
            LayoutRoot.Children.Add(txtM3);
            LayoutRoot.Children.Add(txtResult);
            LayoutRoot.Children.Add(txtValue);
            LayoutRoot.Children.Add(txtExp);
            LayoutRoot.Children.Add(txtEquation);
            LayoutRoot.Children.Add(txtRPNX);
            LayoutRoot.Children.Add(txtRPNY);
            LayoutRoot.Children.Add(txtRPNXValue);
            LayoutRoot.Children.Add(txtRPNYValue);

            LayoutRoot.Children.Add(txtWhole);
            LayoutRoot.Children.Add(txtDenominator);
            LayoutRoot.Children.Add(txtNumerator);
            LayoutRoot.Children.Add(lineFrac);

            this.Content = LayoutRoot;

        }

        //public because of the interface
        public void SetDataContext()
        {   
            //binding order shouldn't matter ....
            this.DataContext = Globals.Display;

            //rebind because we're forcing em
          /*  txt2ndF.SetBinding(TextBlock.VisibilityProperty, new Binding("s2ndF"));
            txtHyp.SetBinding(TextBlock.VisibilityProperty, new Binding("Hyp"));
            txtM.SetBinding(TextBlock.VisibilityProperty, new Binding("M1"));
            txtM2.SetBinding(TextBlock.VisibilityProperty, new Binding("M2"));
            txtM3.SetBinding(TextBlock.VisibilityProperty, new Binding("M3"));*/
        }

        public void ReleaseDataContext()
        {
            this.DataContext = null;
            txt2ndF.Visibility = Visibility.Collapsed;
            txtHyp.Visibility = Visibility.Collapsed;
            txtM.Visibility = Visibility.Collapsed;
            txtM2.Visibility = Visibility.Collapsed;
            txtM3.Visibility = Visibility.Collapsed;
            lineFrac.Visibility = Visibility.Collapsed;
        }

        public void SetBindings()
        {
            txtStyle.SetBinding(TextBlock.TextProperty, new Binding("szDispType"));
            txtDRG.SetBinding(TextBlock.TextProperty, new Binding("szDRG"));
            txtBASE.SetBinding(TextBlock.TextProperty, new Binding("szBase"));
            txt2ndF.SetBinding(TextBlock.VisibilityProperty, new Binding("s2ndF"));
            txtHyp.SetBinding(TextBlock.VisibilityProperty, new Binding("Hyp"));
            txtOperator.SetBinding(TextBlock.TextProperty, new Binding("szLastEntry"));
            txtM.SetBinding(TextBlock.VisibilityProperty, new Binding("M1"));
            txtM2.SetBinding(TextBlock.VisibilityProperty, new Binding("M2"));
            txtM3.SetBinding(TextBlock.VisibilityProperty, new Binding("M3"));
            txtResult.SetBinding(TextBlock.VisibilityProperty, new Binding("ResultNormal"));
            txtValue.SetBinding(TextBlock.VisibilityProperty, new Binding("ResultExponent"));
            txtExp.SetBinding(TextBlock.VisibilityProperty, new Binding("ResultExponent"));
            txtEquation.SetBinding(TextBlock.TextProperty, new Binding("szEq"));
            txtRPNXValue.SetBinding(TextBlock.TextProperty, new Binding("szResult"));
            txtRPNYValue.SetBinding(TextBlock.TextProperty, new Binding("szRPNY"));

            txtResult.SetBinding(TextBlock.TextProperty, new Binding("szResult"));
            txtValue.SetBinding(TextBlock.TextProperty, new Binding("szValue"));
            txtExp.SetBinding(TextBlock.TextProperty, new Binding("szExp"));

            txtWhole.SetBinding(TextBlock.VisibilityProperty, new Binding("ResultWhole"));
            txtDenominator.SetBinding(TextBlock.VisibilityProperty, new Binding("ResultWhole"));
            txtNumerator.SetBinding(TextBlock.VisibilityProperty, new Binding("ResultWhole"));
            lineFrac.SetBinding(Line.VisibilityProperty, new Binding("ResultWhole"));

            txtWhole.SetBinding(TextBlock.TextProperty, new Binding("szWhole"));
            txtDenominator.SetBinding(TextBlock.TextProperty, new Binding("szDenominator"));
            txtNumerator.SetBinding(TextBlock.TextProperty, new Binding("szNumerator"));
        }

        public void ReleaseBindings()
        {
            DependencyProperty dp = null;
            
            txtStyle.GetValue(dp);
            if(dp != null)
                txtStyle.ClearValue(dp);

            txtDRG.GetValue(dp);
            if(dp != null)
                txtDRG.ClearValue(dp);

            txtBASE.GetValue(dp);
            if(dp != null)
                txtBASE.ClearValue(dp);

            txt2ndF.GetValue(dp);
            if(dp != null)
                txt2ndF.ClearValue(dp);

            txtHyp.GetValue(dp);
            if(dp != null)
                txtHyp.ClearValue(dp);

            txtOperator.GetValue(dp);
            if(dp != null)
                txtOperator.ClearValue(dp);

            txtM.GetValue(dp);
            if(dp != null)
                txtM.ClearValue(dp);

            txtM2.GetValue(dp);
            if(dp != null)
                txtM2.ClearValue(dp);

            txtM3.GetValue(dp);
            if(dp != null)
                txtM3.ClearValue(dp);

            txtResult.GetValue(dp);
            if(dp != null)
                txtResult.ClearValue(dp);

            txtValue.GetValue(dp);
            if(dp != null)
                txtValue.ClearValue(dp);

            txtExp.GetValue(dp);
            if(dp != null)
                txtExp.ClearValue(dp);

            txtEquation.GetValue(dp);
            if(dp != null)
                txtEquation.ClearValue(dp);

            txtRPNXValue.GetValue(dp);
            if(dp != null)
                txtRPNXValue.ClearValue(dp);

            txtRPNYValue.GetValue(dp);
            if(dp != null)
                txtRPNYValue.ClearValue(dp);

            txtWhole.GetValue(dp);
            if (dp != null)
                txtWhole.ClearValue(dp);

            txtNumerator.GetValue(dp);
            if (dp != null)
                txtNumerator.ClearValue(dp);

            txtDenominator.GetValue(dp);
            if (dp != null)
                txtDenominator.ClearValue(dp);
        }

        public void SetDisplayType(EnumCalcDisplay eDisplay, bool bDataBind)
        {
    //        Dispatcher.BeginInvoke(() =>
            {
                // Do your stuff here, in the “future”            
                txtStyle.Visibility = Visibility.Visible;
                txtDRG.Visibility = Visibility.Visible;
                txtBASE.Visibility = Visibility.Visible;
                txtOperator.Visibility = Visibility.Visible;

                switch (eDisplay)
                {
                    case EnumCalcDisplay.Graphing:
                        txtDenominator.Visibility = Visibility.Collapsed;
                        txtNumerator.Visibility = Visibility.Collapsed;
                        txtWhole.Visibility = Visibility.Collapsed;
                        lineFrac.Visibility = Visibility.Collapsed;
                        txtRPNX.Visibility = Visibility.Collapsed;
                        txtRPNY.Visibility = Visibility.Collapsed;
                        txtRPNXValue.Visibility = Visibility.Collapsed;
                        txtRPNYValue.Visibility = Visibility.Collapsed;
                        txtM.Visibility = Visibility.Collapsed;
                        txtM2.Visibility = Visibility.Collapsed;
                        txtM3.Visibility = Visibility.Collapsed;
                        txtOperator.Visibility = Visibility.Collapsed;
                        txtValue.Visibility = Visibility.Collapsed;
                        txtResult.Visibility = Visibility.Collapsed;
                        txtExp.Visibility = Visibility.Collapsed;
                        txtEquation.Visibility = Visibility.Visible;
                        break;
                    case EnumCalcDisplay.Expression:
                    case EnumCalcDisplay.ExpressionExp:
                        txtDenominator.Visibility = Visibility.Collapsed;
                        txtNumerator.Visibility = Visibility.Collapsed;
                        txtWhole.Visibility = Visibility.Collapsed;
                        lineFrac.Visibility = Visibility.Collapsed;
                        txtOperator.Visibility = Visibility.Collapsed;
                        txtEquation.Visibility = Visibility.Visible;
                        txtRPNX.Visibility = Visibility.Collapsed;
                        txtRPNY.Visibility = Visibility.Collapsed;
                        txtRPNXValue.Visibility = Visibility.Collapsed;
                        txtRPNYValue.Visibility = Visibility.Collapsed;
                        break;
                    case EnumCalcDisplay.RPN:
                        txtDenominator.Visibility = Visibility.Collapsed;
                        txtNumerator.Visibility = Visibility.Collapsed;
                        txtWhole.Visibility = Visibility.Collapsed;
                        lineFrac.Visibility = Visibility.Collapsed;
                        txtRPNX.Visibility = Visibility.Visible;
                        txtRPNY.Visibility = Visibility.Visible;
                        txtRPNXValue.Visibility = Visibility.Visible;
                        txtRPNYValue.Visibility = Visibility.Visible;
                        txtEquation.Visibility = Visibility.Collapsed;
                        txtResult.Visibility = Visibility.Collapsed;
                        txtValue.Visibility = Visibility.Collapsed;
                        txtExp.Visibility = Visibility.Collapsed;
                        txtEquation.Visibility = Visibility.Collapsed;
                        break;
                    case EnumCalcDisplay.Fractions:
                        txtDenominator.Visibility = Visibility.Visible;
                        txtNumerator.Visibility = Visibility.Visible;
                        txtWhole.Visibility = Visibility.Visible;
                        lineFrac.Visibility = Visibility.Collapsed;//yes default it to off
                        txtRPNX.Visibility = Visibility.Collapsed;
                        txtRPNY.Visibility = Visibility.Collapsed;
                        txtRPNXValue.Visibility = Visibility.Collapsed;
                        txtRPNYValue.Visibility = Visibility.Collapsed;
                        txtEquation.Visibility = Visibility.Collapsed;
                        break;
                    case EnumCalcDisplay.Normal:
                    case EnumCalcDisplay.NormalExp:
                        txtDenominator.Visibility = Visibility.Collapsed;
                        txtNumerator.Visibility = Visibility.Collapsed;
                        txtWhole.Visibility = Visibility.Collapsed;
                        lineFrac.Visibility = Visibility.Collapsed;//yes default it to off
                        txtRPNX.Visibility = Visibility.Collapsed;
                        txtRPNY.Visibility = Visibility.Collapsed;
                        txtRPNXValue.Visibility = Visibility.Collapsed;
                        txtRPNYValue.Visibility = Visibility.Collapsed;
                        txtEquation.Visibility = Visibility.Collapsed;
                        break;
                    case EnumCalcDisplay.Currency:
                    case EnumCalcDisplay.CurrencyUpdate:
                    case EnumCalcDisplay.Unit:
                    case EnumCalcDisplay.Constants:
                    case EnumCalcDisplay.WorkSheet:
                    case EnumCalcDisplay.UnitCategory:
                        HideTextBlocks();
                        break;
                }

                //because changing the setting wipes out the binding
                if (bDataBind)
                {
                    SetDataContext();
                    SetBindings();
                }
            }
     //       );
        }

        private void HideTextBlocks()
        {
            lineFrac.Visibility = Visibility.Collapsed;
            txtDenominator.Visibility = Visibility.Collapsed;
            txtNumerator.Visibility = Visibility.Collapsed;
            txtWhole.Visibility = Visibility.Collapsed;
            txtStyle.Visibility = Visibility.Collapsed;
            txtDRG.Visibility = Visibility.Collapsed;
            txtBASE.Visibility = Visibility.Collapsed;
            txt2ndF.Visibility = Visibility.Collapsed;
            txtHyp.Visibility = Visibility.Collapsed;
            txtOperator.Visibility = Visibility.Collapsed;
            txtM.Visibility = Visibility.Collapsed;
            txtM2.Visibility = Visibility.Collapsed;
            txtM3.Visibility = Visibility.Collapsed;
            txtResult.Visibility = Visibility.Collapsed;
            txtValue.Visibility = Visibility.Collapsed;
            txtExp.Visibility = Visibility.Collapsed;
            txtEquation.Visibility = Visibility.Collapsed;
            txtRPNX.Visibility = Visibility.Collapsed;
            txtRPNY.Visibility = Visibility.Collapsed;
            txtRPNXValue.Visibility = Visibility.Collapsed;
            txtRPNYValue.Visibility = Visibility.Collapsed;
        }

        private void CreateMetroDisplay(EnumCalcProSkinColor eSkinColor)
        {
            Rectangle r1;

            r1 = new Rectangle();
            r1.StrokeThickness = 2;

            r1.Fill = Globals.Brush.GetBrush(GetMetroBackground(eSkinColor));
            r1.Stroke = Globals.Brush.GetBrush(GetMetroStroke(eSkinColor));
            LayoutRoot.Children.Add(r1);
        }

        UInt32 GetMetroBackground(EnumCalcProSkinColor eSkinColor)
        {
            switch (eSkinColor)
            {
                default:
                case EnumCalcProSkinColor.Black:
                    return 0xFF000000;
                case EnumCalcProSkinColor.Blue:
                    return 0xFFA0B0CB;
                case EnumCalcProSkinColor.BlueSteel:
                    return 0xFFA3ADB8;
                case EnumCalcProSkinColor.Brown:
                    return 0xFFB6B7B2;
                case EnumCalcProSkinColor.Green:
                    return 0xFFB8D19E;
                case EnumCalcProSkinColor.Grey:
                    return 0xFFB9B9B9;
                case EnumCalcProSkinColor.Orange:
                    return 0xFF000000;
                case EnumCalcProSkinColor.Pink:
                    return 0xFFF5E1E8;
            }
        }

        UInt32 GetMetroStroke(EnumCalcProSkinColor eSkinColor)
        {
            switch (eSkinColor)
            {
                default:
                case EnumCalcProSkinColor.Black:
                    return 0xFF333333;
                case EnumCalcProSkinColor.Blue:
                    return 0;
                case EnumCalcProSkinColor.BlueSteel:
                    return 0xFF343E4B;
                case EnumCalcProSkinColor.Brown:
                    return 0xFF29251B;
                case EnumCalcProSkinColor.Green:
                    return 0xFF65883A;
                case EnumCalcProSkinColor.Grey:
                    return 0xFF333333;
                case EnumCalcProSkinColor.Orange:
                    return 0xFF202020;
                case EnumCalcProSkinColor.Pink:
                    return 0xFFB34C6F;
            }
        }

        private void CreateBackgroundDisplay(EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eSkinStyle)
        {
            if (eSkinStyle == EnumCalcProSkinStyle.Standard || eSkinStyle == EnumCalcProSkinStyle.Flat)
            {
                CreateMetroDisplay(eSkinColor);
                    return;
            }

            Rectangle r1;
            Rectangle r2;
            Rectangle r3;
            List<TypeGradientItem> items;
            TypeGradientItem i;
            Rectangle r = new Rectangle();

            switch (eSkinColor)
            {
           /* case EnumCalcProSkinColor.Black:
                r1 = new Rectangle();
                r1.StrokeThickness = 3;
                r1.Stroke = Globals.Brush.GetGradientBrush(0xFFFFFFFF, 0xFF406095);
                r1.Fill = Globals.Brush.GetGradientBrush(0xFF7C7C7C, 0xFFE5E5E5);
                LayoutRoot.Children.Add(r1);

                r2 = new Rectangle();
                r2.StrokeThickness = 2;
                r2.Margin = new Thickness(6, 6, 6, 12);
                r2.Opacity = 0.565;
                r2.Fill = Globals.Brush.GetGradientBrush(Colors.White, Colors.Transparent);
                r2.Opacity = 0.37;
                LayoutRoot.Children.Add(r2);

                r.StrokeThickness = 2;
                r.Margin = new Thickness(3, 0, 3, 3);
                r.Height = 5;
                r.VerticalAlignment = VerticalAlignment.Bottom;
                r.Fill = Globals.Brush.GetGradientBrush(0xB7858585, 0xFFFFFFFF);
                LayoutRoot.Children.Add(r);
                break;
                case EnumCalcProSkinColor.BlueSteel:
                r1 = new Rectangle();
                r1.StrokeThickness = 0;
                r1.Margin = new Thickness(2);
                r1.RadiusX = 8;
                r1.RadiusY = 8;
                r1.Fill = Globals.Brush.GetGradientBrush(0xFF668891, 0xFFAFCBD7);
                LayoutRoot.Children.Add(r1);

                r2 = new Rectangle();
                r2.StrokeThickness = 4;
                r2.Margin = new Thickness(4,4,2,2);
                r2.RadiusX = 6;
                r2.RadiusY = 6;
                r2.Stroke = Globals.Brush.GetBrush(Colors.Black);
                r2.Opacity = 0.37;
                LayoutRoot.Children.Add(r2);

                r.StrokeThickness = 4;
                r.Margin = new Thickness(1);
                r.RadiusX = 6;
                r.RadiusY = 6;
                r.Fill = Globals.Brush.GetGradientBrush(0xFF8C9AA4, 0xFF4A5963);
                LayoutRoot.Children.Add(r);
                break;*/
            case EnumCalcProSkinColor.Green:
            case EnumCalcProSkinColor.Brown:

                r1 = new Rectangle();
                r1.StrokeThickness = 0;
                r1.Margin = new Thickness(1,2,1,2);
                r1.RadiusX = 8;
                r1.RadiusY = 8;
                r1.Fill = Globals.Brush.GetGradientBrush(0xFF7A8475, 0xFFB5C0AF);
                LayoutRoot.Children.Add(r1);

                r2 = new Rectangle();
                r2.StrokeThickness = 6;
                r2.Margin = new Thickness(1,2,1,1);
                r2.RadiusX = 6;
                r2.RadiusY = 6;
                r2.Stroke = Globals.Brush.GetBrush(Colors.Black);
                r2.Opacity = 0.37;
                LayoutRoot.Children.Add(r2);

                r3 = new Rectangle();
                r3.StrokeThickness = 4;
                r3.Margin = new Thickness(0,1,0,1);
                r3.RadiusY = 6;
                items = new List<TypeGradientItem>();
                i = new TypeGradientItem();
                i.dbOffset = 0;
                i.cr = BrushFactory.ToColor(0xFFB5C0AF);
                items.Add(i);
                i = new TypeGradientItem();
                i.dbOffset = 0.284;
                i.cr = BrushFactory.ToColor(0xFF9CA796);
                items.Add(i);
                i = new TypeGradientItem();
                i.dbOffset =1;
                i.cr = BrushFactory.ToColor(0xFF5D6658);
                items.Add(i);
                r3.Stroke = Globals.Brush.GetGradientBrush(items);
                LayoutRoot.Children.Add(r3);

                r.StrokeThickness = 0;
                r.Margin = new Thickness(14, 12, 11, 0);
                r.RadiusX = 8;
                r.RadiusY = 8;
                r.Opacity = 0.355;
                r.Height = 29;
                r.VerticalAlignment = VerticalAlignment.Top;
                r.Fill = Globals.Brush.GetGradientBrush(Colors.White, Colors.Transparent);
                LayoutRoot.Children.Add(r);
                break;
            case EnumCalcProSkinColor.Orange:
                r.StrokeThickness = 3;
                r.Margin = new Thickness(1);
                r.Stroke = Globals.Brush.GetGradientBrush(0xFF333333, 0xFF979797);
                r.Fill = Globals.Brush.GetGradientBrush(0xFF494949, 0xFF1D1D1D);
                LayoutRoot.Children.Add(r);
                break;
            case EnumCalcProSkinColor.Pink:
                r.StrokeThickness = 3;
                r.Margin = new Thickness(1);
                r.Stroke = Globals.Brush.GetGradientBrush(0xFF39222E, 0xFFBC7B9D);
                r.Fill = Globals.Brush.GetGradientBrush(Color.FromArgb(255, 255,196, 222), Colors.White);
                LayoutRoot.Children.Add(r);
                break;
            case EnumCalcProSkinColor.Black:
                r1 = new Rectangle();
                r1.StrokeThickness = 2;
                r1.Margin = new Thickness(1);
                r1.Fill = Globals.Brush.GetGradientBrush(0xFF7A8475, 0xFFB5C0AF);
                LayoutRoot.Children.Add(r1);

                r2 = new Rectangle();
                items = new List<TypeGradientItem>();
                i = new TypeGradientItem();
                i.dbOffset = 0;
                i.cr = BrushFactory.ToColor(0xFFCECECE);
                items.Add(i);
                i = new TypeGradientItem();
                i.dbOffset = 0.491;
                i.cr = BrushFactory.ToColor(0xFFFFFFFF);
                items.Add(i);
                i = new TypeGradientItem();
                i.dbOffset =1;
                i.cr = BrushFactory.ToColor(0xFF959595);
                items.Add(i);
                r2.Stroke = Globals.Brush.GetGradientBrush(items);
                LayoutRoot.Children.Add(r2);

                r.StrokeThickness = 2;
                r.Margin = new Thickness(3);
                r.Fill = Globals.Brush.GetGradientBrush(0xFF2B2B2B, 0xFF000000);
                LayoutRoot.Children.Add(r);

                break;

            case EnumCalcProSkinColor.BlueSteel:
                r1 = new Rectangle();
                r1.StrokeThickness = 0;
                r1.Margin = new Thickness(1,2,1,1);
                r1.RadiusX = 8;
                r1.RadiusY = 8;
                r1.Fill = Globals.Brush.GetGradientBrush(0xFF668891, 0xFFAFCBD7);
                LayoutRoot.Children.Add(r1);

                r2 = new Rectangle();
                r2.StrokeThickness = 4;
                r2.Margin = new Thickness(3,4,3,1);
                r2.RadiusX = 6;
                r2.RadiusY = 6;
                r2.Stroke = Globals.Brush.GetBrush(Colors.Black);
                r2.Opacity = 0.37;
                LayoutRoot.Children.Add(r2);

                r3 = new Rectangle();
                r3.StrokeThickness = 4;
                r3.Margin = new Thickness(0,1,0,0);
                r3.RadiusX = 6;
                r3.RadiusY = 6;
                r3.Stroke = Globals.Brush.GetGradientBrush(0xFF8C9AA4, 0xFF4A5963);
                LayoutRoot.Children.Add(r3);

                r.StrokeThickness = 0;
                r.Margin = new Thickness(14, 12, 11, 0);
                r.RadiusX = 8;
                r.RadiusY = 8;
                r.Opacity = 0.155;
                r.Height = 29;
                r.VerticalAlignment = VerticalAlignment.Top;
                r.Fill = Globals.Brush.GetGradientBrush(0xFFFFFFFF, 0x40FFFFFF);
                LayoutRoot.Children.Add(r);
                break;



            
            case EnumCalcProSkinColor.Blue:
            case EnumCalcProSkinColor.Grey:

            default:                
                r.Stroke = Globals.Brush.GetBrush(0xFF3D3D3D);
                r.StrokeThickness = 2;
             //   r.Margin = new Thickness(1);
                r.Fill = Globals.Brush.GetGradientBrush(Color.FromArgb(255, 149,149, 149), Colors.White);
                LayoutRoot.Children.Add(r);
                break;
            }
        }
    }
}



