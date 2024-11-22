using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;

namespace CalcPro
{
    interface IButtonSkin
    {
        bool CreateButtonSkin(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex);
        void DrawDownState(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex);
        bool DrawUpState(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex);
    }
}
