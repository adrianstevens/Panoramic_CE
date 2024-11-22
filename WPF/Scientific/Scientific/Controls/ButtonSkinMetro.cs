﻿using System;
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
    public class ButtonSkinMetro: ButtonSkinClassic
    {
        public override void DrawDownState(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex)
        {

        }

        public override bool DrawUpState(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex)
        {

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
            Border.Background = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Metro);
            /****************************************************/
            
            LayoutRoot.Children.Add(Border);

            MainText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);
            SubText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);

            SecondText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);
            SecExpText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);

            return true;
        }
    }
}