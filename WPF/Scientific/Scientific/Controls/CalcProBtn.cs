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
    public class CalcProBtn : UserControl
    {
        //basically just storage ... makes Calc Pro easy
        public int iBtnType
        {
            get;
            set;
        }

        public int iBtnIndex
        {
            get;
            set;
        }

        public CalcProBtn()
        {
                
        }

        double GetTextWidth(int iLength, double dbFontSize, bool bExp)
        {   //lower numbers to increase font size
            if (iLength == 1 && bExp == false)
                return dbFontSize * 1.3;
            if (iLength == 1)
                return dbFontSize * 1.7;
            if(iLength < 5)
                return 2.2*dbFontSize;
           // if(iLength == 3)
           //     return 2.7*dbFontSize;
         //   if (iLength == 4)
         //       return 3*dbFontSize;
            return iLength*dbFontSize*0.5;
        }

        public bool AlignText(ref TextBlock tbMain, ref TextBlock tbExp, ref TextBlock tb2ndF, ref TextBlock tb2ndFExp, double dbHeight, double dbWidth)
        {
            double dbFontSize = 15.5;

            tb2ndF.FontSize = dbFontSize;
            tb2ndF.Margin = new Thickness(8, 4, 0, 0);

            //tb2ndFExp.Text = "x";

            if(tb2ndFExp.Text != null && tb2ndFExp.Text.Length > 0 && tb2ndFExp.Text.Length > 0)
            {
                double dbEstWidth = tb2ndF.Text.Length * 15;//GetTextWidth(tb2ndF.Text.Length, dbFontSize, true);
                tb2ndFExp.FontSize = 12;
                tb2ndFExp.Margin = new Thickness(dbEstWidth/2 + 10, 1, 0, 0);
            }
            
            return AlignText(ref tbMain, ref tbExp, dbHeight, dbWidth);
        }


        public bool AlignText(ref TextBlock tbMain, ref TextBlock tbExp, double dbHeight, double dbWidth)
        {

            bool bExp = false;

            if (tbExp.Text.Length > 0)
                bExp = true;

            tbMain.FontSize = 64;//experimenting

            //sure ...
            double dbEstWidth = GetTextWidth(tbMain.Text.Length, tbMain.FontSize, bExp);
             
            //so we'll align the main text to the same height regardless of exponent
            {
                while (dbEstWidth > dbWidth * 0.7 || tbMain.FontSize > dbHeight * 0.5)
                {
                    tbMain.FontSize -= 2;//testing

                //    dbEstWidth = tbMain.ActualWidth;
                    dbEstWidth = GetTextWidth(tbMain.Text.Length, tbMain.FontSize, bExp);
                }
            }

            tbMain.HorizontalAlignment = HorizontalAlignment.Center;
            tbMain.VerticalAlignment = VerticalAlignment.Center;
            tbMain.Margin = new Thickness(0, 0, 0, tbMain.FontSize/8);//text tends to sit low 

            if (bExp)
            {
                tbExp.FontSize = tbMain.FontSize * 0.65;
                //tbExp.Margin = new Thickness((dbWidth + tbMain.ActualWidth) / 2, 0, 0, (dbHeight)/2);
                tbExp.Margin = new Thickness((dbWidth + tbMain.FontSize*tbMain.Text.Length/2) / 2, 0, 0, (dbHeight) / 2);
                tbExp.HorizontalAlignment = HorizontalAlignment.Left;
                tbExp.VerticalAlignment = VerticalAlignment.Bottom;
            }

            return true;
        }

        public virtual bool SetMainText(string szMain)
        { return false; }

        public virtual bool SetExpText(string szExp)
        { return false; }

        public virtual bool Set2ndText(string szMain)
        { return false; }

        public virtual bool Set2ndExpText(string szExp)
        { return false; }



    }
}
