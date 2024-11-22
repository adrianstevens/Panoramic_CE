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
using System.Collections.Generic;

namespace CalcPro
{
    public class TypeGradientItem
    {
        public Color cr {get;set;}
        public double dbOffset {get;set;}
    }
    public class BrushFactory
    {
        private Dictionary<string, SolidColorBrush> _arrBrush;
        private Dictionary<string, LinearGradientBrush> _arrGradientBrush;
        
        public BrushFactory()
        {
            _arrBrush = new Dictionary<string, SolidColorBrush>();
            _arrGradientBrush = new Dictionary<string, LinearGradientBrush>();
            
        }
        public void ClearBrushes()
        {
            _arrBrush.Clear();
            _arrGradientBrush.Clear();
        }
        
        public SolidColorBrush GetBrush(Color cr)
        {
            SolidColorBrush br = null;

            if (_arrBrush.TryGetValue(cr.ToString(), out br))
                return br;

            br = new SolidColorBrush();
            br.Color = cr;

            _arrBrush.Add(cr.ToString(), br);

            return br;
        }
        public SolidColorBrush GetBrush(uint uiColor)
        {
            return GetBrush(ToColor(uiColor));
        }
        public LinearGradientBrush GetGradientBrush(Color cr1, Color cr2)
        {
            LinearGradientBrush br = null;

            if (_arrGradientBrush.TryGetValue(cr1.ToString() + cr2.ToString(), out br))
                return br;

            br = new LinearGradientBrush();

            br.StartPoint = new Point(0.5, 0);
            br.EndPoint = new Point(0.5, 1);

            GradientStop stop = new GradientStop();
            stop.Offset = 0;
            stop.Color = cr1;
            br.GradientStops.Add(stop);

            stop = new GradientStop();
            stop.Offset = 1;
            stop.Color = cr2;
            br.GradientStops.Add(stop);

            _arrGradientBrush.Add(cr1.ToString() + cr2.ToString(), br);

            return br;
        }
        public LinearGradientBrush GetGradientBrush(uint uiColor1, uint uiColor2)
        {
            return GetGradientBrush(ToColor(uiColor1), ToColor(uiColor2));
        }
        public LinearGradientBrush GetGradientBrush(List<TypeGradientItem> items)
        {
            if (items.Count == 0)
                return null;

            string szKey = "";

            LinearGradientBrush br = null;

            foreach (TypeGradientItem i in items)
            {
                szKey += i.cr.ToString();
            }

            if (_arrGradientBrush.TryGetValue(szKey, out br))
                return br;

            br = new LinearGradientBrush();

            br.StartPoint = new Point(0.5, 0);
            br.EndPoint = new Point(0.5, 1);

            GradientStop stop;
            foreach (TypeGradientItem i in items)
            {
                stop = new GradientStop();
                stop.Offset = i.dbOffset;
                stop.Color = i.cr;
                br.GradientStops.Add(stop);
            }

            _arrGradientBrush.Add(szKey, br);

            return br;

        }

        /*public LinearGradientBrush GetGradientBrush(Color cr1, Color cr2, Color cr3)
        {
            if (_arrGradientBrush.ContainsKey(cr1.ToString() + cr2.ToString()))
                return _arrGradientBrush[cr1.ToString() + cr2.ToString() + cr3.ToString()];

            LinearGradientBrush br = new LinearGradientBrush();

            br.StartPoint = new Point(0.5, 0);
            br.EndPoint = new Point(0.5, 1);

            GradientStop stop = new GradientStop();
            stop.Offset = 0;
            stop.Color = cr1;
            br.GradientStops.Add(stop);

            stop = new GradientStop();
            stop.Offset = 1;
            stop.Color = cr2;
            br.GradientStops.Add(stop);

            _arrGradientBrush.Add(cr1.ToString() + cr2.ToString(), br);

            return br;
        }*/

        static public Color ToColor(uint argb)
        {
            return Color.FromArgb((byte)((argb & -16777216) >> 0x18),(byte)((argb & 0xff0000) >> 0x10),(byte)((argb & 0xff00) >> 8),(byte)(argb & 0xff));
        }

    }
}
