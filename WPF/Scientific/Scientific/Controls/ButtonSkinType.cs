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


    public class ButtonSkinType: ButtonSkinClassic
    {
        private Border Border3;

        Brush brBorder3;

        public override void DrawDownState(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex)
        {
            base.DrawDownState(ref LayoutRoot, ref MainText, ref SubText, ref SecondText, ref SecExpText, eSkinColor, eStyle, ref iSkinIndex);

            brBorder3 = Border3.Background;
            GradientBrush br3 = Border3.Background as GradientBrush;
            Border3.Background = Globals.Brush.GetGradientBrush(br3.GradientStops[1].Color, br3.GradientStops[0].Color);
        }

        public override bool DrawUpState(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex)
        {
            if (base.DrawUpState(ref LayoutRoot, ref MainText, ref SubText, ref SecondText, ref SecExpText, eSkinColor, eStyle, ref iSkinIndex) == false)
                return false;

            Border3.Background = brBorder3;
            return true;
        }

        public override bool CreateButtonSkin(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex)
        {
            if (eSkinColor != eCurrentColor)//only need to load in the colors 
            {
                if (LoadSkin(eSkinColor) == false)
                    return false;
            }


            /****************************************************/
            Border = new Border();
            Border.Margin = new Thickness(Globals.MARGIN);
            Border.CornerRadius = new CornerRadius(10);
            Border.Background = Globals.Brush.GetGradientBrush(sBtnColors[iSkinIndex].Grad1Top, sBtnColors[iSkinIndex].Grad1Bottom);
            /****************************************************/
            Border2 = new Border();
            Border2.Margin = new Thickness(Globals.MARGIN+1);
            Border2.CornerRadius = new CornerRadius(10);
            Border2.Background = Globals.Brush.GetGradientBrush(sBtnColors[iSkinIndex].Grad2Top, sBtnColors[iSkinIndex].Grad2Bottom);
            /*****************************************************/
            Border3 = new Border();
            Border3.Margin = new Thickness(3 + Globals.MARGIN, 3 + Globals.MARGIN, 3 + Globals.MARGIN, 9 + Globals.MARGIN);
            Border3.CornerRadius = new CornerRadius(10);
            Border3.Background = Globals.Brush.GetGradientBrush(sBtnColors[iSkinIndex].Grad2Bottom, sBtnColors[iSkinIndex].Grad2Top);
            /*****************************************************/

            LayoutRoot.Children.Add(Border);
            LayoutRoot.Children.Add(Border2);
            LayoutRoot.Children.Add(Border3);

            MainText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);
            SubText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);

            SecondText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);
            SecExpText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);

            return true;
        }
    }


}
