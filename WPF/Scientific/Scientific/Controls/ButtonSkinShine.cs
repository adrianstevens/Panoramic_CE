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
    public class ButtonSkinShine : ButtonSkinClassic
    {
        private Path Shine;

        public ButtonSkinShine()
        {
        }

        public override void DrawDownState(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex)
        {
            base.DrawDownState(ref LayoutRoot, ref MainText, ref SubText, ref SecondText, ref SecExpText, eSkinColor, eStyle, ref iSkinIndex);

            Shine.Visibility = Visibility.Collapsed;
        }

        public override bool DrawUpState(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex)
        {
            if (base.DrawUpState(ref LayoutRoot, ref MainText, ref SubText, ref SecondText, ref SecExpText, eSkinColor, eStyle, ref iSkinIndex) == false)
                return false;

            Shine.Visibility = Visibility.Visible;

            return true;
        }

        public override bool CreateButtonSkin(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex)
        {
            //http://msdn.microsoft.com/en-us/library/system.windows.shapes.path.data.aspx 
            base.CreateButtonSkin(ref LayoutRoot, ref MainText, ref SubText, ref SecondText, ref SecExpText, eSkinColor, eStyle, ref iSkinIndex);

            //and add the shine yo
            Shine = new Path();

            PathGeometry pg = new PathGeometry();

            PathFigure pf = new PathFigure();
            pf.StartPoint = new Point(7.9375, 1.9375);
            pf.IsClosed = true;

            BezierSegment ps = new BezierSegment();
            ps.Point1 = new Point(4.6870422, 1.937);
            ps.Point2 = new Point(2.0463746,4.8120866);
            ps.Point3 = new Point(2.0151246,7.6870866);
            pf.Segments.Add(ps);

            ps = new BezierSegment();
            ps.Point1 = new Point(1.9838746, 10.562087);
            ps.Point2 = new Point(1.9994998, 18.666899);
            ps.Point3 = new Point(1.9994998, 18.666899);
            pf.Segments.Add(ps);

            ps = new BezierSegment();
            ps.Point1 = new Point(1.9994998,18.666899);
            ps.Point2 = new Point(8.3744717, 29.437414);
            ps.Point3 = new Point(34.582333, 29.333487);
            pf.Segments.Add(ps);

            ps = new BezierSegment();
            ps.Point1 = new Point(60.292145, 29.231533);
            ps.Point2 = new Point(65.998619, 18.60914);
            ps.Point3 = new Point(65.998619, 18.60914);
            pf.Segments.Add(ps);

            ps = new BezierSegment();
            ps.Point1 = new Point(65.998619, 18.60914);
            ps.Point2 = new Point(66.013802, 10.874636);
            ps.Point3 = new Point(65.998619, 7.9058404);
            pf.Segments.Add(ps);

            ps = new BezierSegment();
            ps.Point1 = new Point(65.983261, 4.9032311);
            ps.Point2 = new Point(63.498734, 1.9995011);
            ps.Point3 = new Point(60.045361, 1.9995009);
            pf.Segments.Add(ps);
            
            
            ps = new BezierSegment();
            ps.Point1 = new Point(57.701653, 1.9995006);
            ps.Point2 = new Point(7.9375, 1.9375);
            ps.Point3 = new Point(7.9375, 1.9375);
            pf.Segments.Add(ps);

            pg.Figures.Add(pf);

            Shine.Margin = new Thickness(Globals.MARGIN);
            Shine.Height = 30;// 29.334;
            Shine.Stretch = Stretch.Fill;
            
            //Shine.UseLayoutRounding = false;
            Shine.VerticalAlignment = VerticalAlignment.Top;
            Shine.Opacity = 0.275;

            Shine.Data = pg;

            //now we need a linear grad brush
            LinearGradientBrush gradBrush = Globals.Brush.GetGradientBrush(Colors.Transparent, Colors.White);
            Shine.Fill = gradBrush;


            LayoutRoot.Children.Add(Shine);
            return true;
        }
    }
}
