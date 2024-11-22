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
using System.Resources;
using System.Reflection;
using System.Threading;
using System.Diagnostics;

enum EnumGraphMode
{
//    MODE_Zoom,
    MODE_Translate,
    MODE_GetLocation,
};

enum EnumZoomMode
{
    ZOOM_Normal,
    ZOOM_X,
    ZOOM_Y,
};

public enum EnumGraphStyle
{
    Normal,
    Thick,
    Points,
    //shadeup,
    //shadedown,
};

namespace CalcPro.Screens
{
    public partial class Graph : UserControl
    {
        public Graph()
        {
            InitializeComponent();
            oCalc.SetDRGState(Globals.Calc.GetDRGState());
            swBMPMeasure = new Stopwatch();
            textBlockMode.Text = Globals.rm.GetString("IDS_MENU_Translate");
        }

        static double ZOOM_IN_FACTOR = 2.0;
        static double MIN_SCALE	   = 0.00001;
        static double ZOOM_OUT_FACTOR = 2;
        static double MAX_SCALE = 1000000;

        Line lnPointX = null;
        Line lnPointY = null;

        TextBox tbPointX = null;
        TextBox tbPointY = null;

        EnumGraphStyle eGStyle
        {
            get { return Globals.Settings.eGraphStyle; }
            set { Globals.Settings.eGraphStyle = value; }
        }
        

        CalcEngine oCalc = new CalcEngine();//just easier to have my own

        Line[] lineHorz = new Line[10];
        Line[] lineVert = new Line[10];

        EnumGraphMode eGraphMode = EnumGraphMode.MODE_Translate;
        EnumZoomMode eZoomMode;

        private Stopwatch swBMPMeasure = null;
        long lMouseDown = 0;

    
	    int				iAccuracy; //0 is max....value defines how many points are skipped

	    double			dbXScale;		//the number of X values on screen as a double...so 10 goes from -5 to +5
	    double			dbYScale;
	    double			dbXTranslation;//positive is positive etc...ie + 1 means we're centered +1 to the right of the center
	    double			dbYTranslation;//translations are exact to graph, not scaled 

	    //Graph Points
	    long[][]        iYGraph;

	    Point			ptStartSelect;
	    Point			ptEndSelect;

	    //int             iXMoveTo;
        //int             iYMoveTo;
    

	    int	iGraphSelected = -1;
        
        int iScreenWidth;
        int iScreenHeight;

        bool bDrawLines = true;
        bool bDrawGrid = true;
        bool bLButtonDown = false;


        public void ReDraw()
        {
            if (myCanvas.Children.Count > 0)
                myCanvas.Children.Clear();

            if (bLButtonDown)
                iAccuracy = 20;
            else
                iAccuracy = 1;

            CalcGraphPoints();
            DrawGrid();
            DrawAxis();
            DrawGraphs();
            DrawPoints();//control in the function
            SetDisplayText();
        }

        void DrawPoints()
        {
            if (Globals.bGraphUsePoints == false || Globals.arrGraphPoints == null ||
                Globals.arrGraphPoints.Count == 0)
                return;

            int iXtemp;
            int iYtemp;

            Line line = null;
            Ellipse circle = null;

            int iSize = 5;
            int iCirSize = 8;

            bool bDrawSphere = true;

            for (int i = 0; i < Globals.arrGraphPoints.Count; i++)
            {
                iXtemp = XToScreen(Globals.arrGraphPoints[i].X);
                iYtemp = YToScreen(Globals.arrGraphPoints[i].Y);
                
                if (bDrawSphere == true)
                {
                    circle = new Ellipse();

                    circle.Margin = new Thickness(iXtemp - iCirSize, iYtemp - iCirSize, 0, 0);
                    circle.Width = iCirSize * 2;
                    circle.Height = iCirSize * 2;
                    circle.Stroke = Globals.Brush.GetBrush(GetGraphColor(0));
                    circle.Fill = Globals.Brush.GetGradientBrush(GetGraphColor(0), Color.FromArgb(127,0,0,0));

                    myCanvas.Children.Add(circle);

                }
                else
                {
                    line = new Line();

                    line.X1 = iXtemp + iSize;
                    line.Y1 = iYtemp - iSize;
                    line.X2 = iXtemp - iSize;
                    line.Y2 = iYtemp + iSize;
                    line.StrokeThickness = 1;
                    line.Stroke = Globals.Brush.GetBrush(GetGraphColor(0));
                    line.IsHitTestVisible = false;

                    myCanvas.Children.Add(line);

                    line = new Line();

                    line.X1 = iXtemp - iSize;
                    line.Y1 = iYtemp - iSize;
                    line.X2 = iXtemp + iSize;
                    line.Y2 = iYtemp + iSize;
                    line.StrokeThickness = 1;
                    line.Stroke = Globals.Brush.GetBrush(GetGraphColor(0));
                    line.IsHitTestVisible = false;

                    myCanvas.Children.Add(line);
                }

            }
        }

        void DrawAxis()
        {
	        //ok...translations are exact...not relative so
	        //we need to convert the translations and see if the axis are on screen
	        //and of course shift in the opposite direction

	        double dX = (dbXTranslation*iScreenWidth/dbXScale);
            double dY = 1 + (iScreenHeight - dbYTranslation * (double)iScreenHeight / dbYScale);//try and figure out why the correction is needed

            Line line = null;

	        if(dX > -1 && dX < iScreenWidth)
	        {
                line = new Line();

                line.X1 = (int)dX+0.5;
                line.Y1 = 0;
                line.X2 = (int)dX + 0.5;
                line.Y2 = iScreenHeight;
                line.StrokeThickness = 1;
                line.Stroke = Globals.Brush.GetBrush(Color.FromArgb(255, 255, 255, 255));
                line.IsHitTestVisible = false;

                myCanvas.Children.Add(line);
            }

	        if(dY > -1 && dY < iScreenHeight)
	        {
                line = new Line();

                line.X1 = 0;
                line.Y1 = (int)dY + 0.5;
                line.X2 = iScreenWidth;
                line.Y2 = (int)dY + 0.5;
                line.StrokeThickness = 1;
                line.Stroke = Globals.Brush.GetBrush(Color.FromArgb(255, 255, 255, 255));
                line.IsHitTestVisible = false;

                myCanvas.Children.Add(line);
		    }
        }

        public void DrawGrid()
        {
            //we gotta rmove all of the line elements... hmm
            int NUM_X_DIVIDERS = 9;

            if(bDrawGrid == false)
		        return;

            System.Diagnostics.Debug.WriteLine("DrawGrid()");


	        //we'll just draw 4 lines per....making 5 sections per side...10 total
	        //relative to the center
	        double dXSpacing	= iScreenWidth/(NUM_X_DIVIDERS+1);	
	        double dYSpacing	= dXSpacing; //iScreenHeight/(NUM_DIVIDERS+1);

	        //Now translation & scale? correction
            double dXAdjust = (dbXTranslation * iScreenWidth / dbXScale);  //translation is converted to a fraction of the screen
            double dYAdjust = (dbYTranslation * iScreenHeight / dbYScale);  //translation is converted to a fraction of the screen

	        //now we need it in range ... should be a while loop but whatever
	        for (; dXAdjust > -1 * dXSpacing; dXAdjust -= dXSpacing)
	        {}
	        for(; dXAdjust < -1*dXSpacing; dXAdjust += dXSpacing)
	        {}

	        //now we need it in range
	        for(; dYAdjust > 0; dYAdjust -= dYSpacing)
	        {}
	        for(; dYAdjust < 0; dYAdjust += dYSpacing)
	        {}

	        dYAdjust = dYSpacing - dYAdjust;

	        double dX = 0;
	        double dY = 0;
	        int i;

	        //And lets draw	
        
	        i = 0;
	        while(dY < iScreenHeight)
	        {	//Y
		        dY = (dYSpacing*(1+i)-dYAdjust);

                Line line = new Line();

                line.X1 = 0;
                line.Y1 = iScreenHeight - (int)dY + 0.5;
                line.X2 = iScreenWidth;
                line.Y2 = iScreenHeight - (int)dY + 0.5;
                line.StrokeThickness = 1;
                line.Stroke = Globals.Brush.GetBrush(Color.FromArgb(255, 38, 48, 52));
                line.IsHitTestVisible = false;

                myCanvas.Children.Add(line);
                i++;
	        }

        //	for(int i =0; i < NUM_X_DIVIDERS+1; i++)
	        i = 0;
	        while(dX < iScreenWidth)
	        {	//X
		        dX = dXAdjust+dXSpacing*(1+i);

                Line line = new Line();

                line.X1 = (int)dX + 0.5;
                line.Y1 = 0;
                line.X2 = (int)dX + 0.5;
                line.Y2 = iScreenHeight;
                line.StrokeThickness = 1;
                line.Stroke = Globals.Brush.GetBrush(Color.FromArgb(255, 38, 48, 52));
                line.IsHitTestVisible = false;

                myCanvas.Children.Add(line);

		        i++;
	        }
        }

        private void LayoutRoot_MouseEnter(object sender, MouseEventArgs e)
        {

        }

        private void LayoutRoot_MouseLeave(object sender, MouseEventArgs e)
        {

        }

        private void LayoutRoot_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            bLButtonDown = true;
            lMouseDown = swBMPMeasure.ElapsedMilliseconds;

            ptStartSelect = e.GetPosition(null);

            if(eGraphMode == EnumGraphMode.MODE_GetLocation)
                DrawPointLocation();

            LayoutRoot.CaptureMouse();
        }

        private void LayoutRoot_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
         //   bMouseMoved = true;
            ptEndSelect = e.GetPosition(null);

            if (bLButtonDown && eGraphMode == EnumGraphMode.MODE_Translate)
            {
                bLButtonDown = false;
                double dX = ptEndSelect.X - ptStartSelect.X;
                double dY = ptEndSelect.Y - ptStartSelect.Y;

                double dbXMove = dX / myCanvas.Width * dbXScale;
                double dbYMove = dY / myCanvas.Height * dbYScale;

                dbXTranslation += dbXMove;
                dbYTranslation -= dbYMove;

                ptStartSelect = e.GetPosition(null);
                
                SaveRangeValues();
                
                //lets see what happens
                ReDraw();
            }
            else
            {
                bLButtonDown = false;
                DrawPointLocation();
            }

            LayoutRoot.ReleaseMouseCapture();
        }

        private void LayoutRoot_MouseMove(object sender, MouseEventArgs e)
        {
            ptEndSelect = e.GetPosition(null);

            if (eGraphMode == EnumGraphMode.MODE_Translate && bLButtonDown == true)
            {
                double dX = ptEndSelect.X - ptStartSelect.X;
                double dY = ptEndSelect.Y - ptStartSelect.Y;

                double dbXMove = dX / myCanvas.Width * dbXScale;
                double dbYMove = dY / myCanvas.Height * dbYScale;

                dbXTranslation += dbXMove;
                dbYTranslation -= dbYMove;

                ptStartSelect = e.GetPosition(null);

                //lets see what happens
                if (swBMPMeasure.ElapsedMilliseconds >= lMouseDown)
                {
                    ReDraw();
                    lMouseDown = swBMPMeasure.ElapsedMilliseconds + 33;//20fps?
                }
                else
                {
                    System.Diagnostics.Debug.WriteLine("DrawSkipped()");

                }
            }
            else
            {
                DrawPointLocation();
            }
        }

        bool CalcGraphPoints()
        {
            if (iAccuracy == -1 || iAccuracy > 1)
                return false;

            //System.Diagnostics.Debug.WriteLine("CalcGraphPoints()");

	        int i			= 0;

            string szEq = null;
            string szDoubleTemp; //convert our doubles to string for the ol calc engine
                    
	        
	        //first thing is to figure out the start point and step size
	
	        double dbStart	= -1.0*dbXTranslation;
	        double dbStep	= dbXScale/((double)iScreenWidth/(1+iAccuracy));
	        double dbAns	= 0;

	        //should be based on the accuracy but it really doesn't matter since we're always going full strength
            int iSize = iScreenWidth;

            if (iYGraph == null)
            {
                iYGraph = new long[Globals.NUM_GRAPHS][];                    
            }


	        for(i = 0; i < Globals.NUM_GRAPHS; i++)
	        {
                if (Globals.szGraphs[i] != null && Globals.szGraphs[i].Length > 0)
		        {
			        if(iYGraph[i] == null)
				        iYGraph[i] = new long[iScreenWidth];
					
			        for(int j=0; j < iScreenWidth/(1+iAccuracy); j++)
			        {
				        //now screen points are relative to say...the SCREEN..so we don't need an x
				        //don't forget the damn Y is upside down...we'll do that later
				        //lets build our string to parse
                        szDoubleTemp = "(" + (dbStart + (j * dbStep)).ToString() + ")";

                        szEq = Globals.szGraphs[i].Replace("X", szDoubleTemp);

				        if(szEq[0] == ('-'))
                            szEq = "0" + szEq;

                        dbAns = oCalc.Calculate(szEq, ref szEq);
				        

				        //And finally we need to store it as an int into our array....leet math scilz
				        //quick translation from real Y to pocket PC y screen location
				        iYGraph[i][j] = YToScreen(dbAns);
			        }
		        }
	        }

	        return true;
        }

            
        public void SetGraphAccuracy(int i)
        {
	        if(i < 1)
		        iAccuracy = 0;
	        else if(i == 1)
		        iAccuracy = 3;
	        else
		        iAccuracy = 6;
        }


        int XToScreen(double dbX)
        {
	        return (int)((dbX+dbXTranslation)*iScreenWidth/dbXScale);//the cast should fix any range issues;
        }

        int YToScreen(double dbY)
        {
	        //we start from the height and subtract to invert for Y...note we don't for X
        //	int iRet = iScreenHeight - (int)((dbY+dbYTranslation)*(double)iScreenHeight/dbYScale);//the cast should fix any range issues
        //	double dbret = 0.5 + (double)iScreenHeight - (dbY+dbYTranslation)*(double)iScreenHeight/dbYScale; 

	        double dbret = 0.5  - (dbY+dbYTranslation)*(double)iScreenHeight/dbYScale; 

	        int iCast = (int)dbret;

	        return iScreenHeight + iCast;
        }

        void ResetValues()
        {
	        //lets set a proper scale based on the width
	        dbXScale = 10.0;

            dbYScale = dbXScale*iScreenHeight/iScreenWidth;

	        dbXTranslation = 5.0;
        //	dbYTranslation = 5.0; //to center, this should be half of the scale
	        dbYTranslation = dbYScale/2.0;

            SaveRangeValues();
	        CalcGraphPoints();
        }

        private void AddMenu()
        {
            MenuItem item = new MenuItem();
            item.Header = Globals.rm.GetString("IDS_MENU_Graph");

            for (int i = 0; i < Globals.NUM_GRAPHS; i++)
            {
                if (Globals.szGraphs[i] != null && Globals.szGraphs[i].Length > 0)
                {
                    MenuItem subItem = new MenuItem();
                    subItem.Header = Globals.szGraphs[i];
                    subItem.Click += new RoutedEventHandler(subItem_Graph);
                    subItem.Uid = i.ToString();
                    item.Items.Add(subItem);
                }
            }
            menuFile.Items.Add(item);


            item = new MenuItem();
            item.Header = Globals.rm.GetString("IDS_MENU_ZoomIn");
            item.Click += new RoutedEventHandler(item_ZoomIn);
            menuFile.Items.Add(item);

            item = new MenuItem();
            item.Header = Globals.rm.GetString("IDS_MENU_ZoomOut");
            item.Click += new RoutedEventHandler(item_ZoomOut);
            menuFile.Items.Add(item);



        }

        void subItem_Graph(object sender, RoutedEventArgs e)
        {
            int iGraph = Convert.ToInt32(((MenuItem)sender).Uid);
            iGraphSelected = iGraph;
            ReDraw();
           
        }

        void item_ZoomOut(object sender, RoutedEventArgs e)
        {
            ZoomOut();
        }

        void item_ZoomIn(object sender, RoutedEventArgs e)
        {
            ZoomIn();
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            iScreenWidth = (int)myCanvas.Width;
            iScreenHeight = (int)myCanvas.Height;

            AddMenu();

            /*
            // set the localized text
            ((ApplicationBarIconButton)this.ApplicationBar.Buttons[0]).Text = Globals.rm.GetString("IDS_MENU_Mode");
            ((ApplicationBarIconButton)this.ApplicationBar.Buttons[1]).Text = Globals.rm.GetString("IDS_MENU_Graph");
            ((ApplicationBarIconButton)this.ApplicationBar.Buttons[2]).Text = Globals.rm.GetString("IDS_MENU_ZoomIn");
            ((ApplicationBarIconButton)this.ApplicationBar.Buttons[3]).Text = Globals.rm.GetString("IDS_MENU_ZoomOut");
            //((ApplicationBarMenuItem)this.ApplicationBar.MenuItems[0]).Text = Globals.rm.GetString("IDS_MENU_Reset");
            ((ApplicationBarMenuItem)this.ApplicationBar.MenuItems[0]).Text = Globals.rm.GetString("IDS_MENU_CenterGraph");
            //((ApplicationBarMenuItem)this.ApplicationBar.MenuItems[1]).Text = Globals.rm.GetString("IDS_MENU_ZoomNormal");
            //((ApplicationBarMenuItem)this.ApplicationBar.MenuItems[2]).Text = Globals.rm.GetString("IDS_MENU_ZoomX");
            //((ApplicationBarMenuItem)this.ApplicationBar.MenuItems[3]).Text = Globals.rm.GetString("IDS_MENU_ZoomY");
            ((ApplicationBarMenuItem)this.ApplicationBar.MenuItems[1]).Text = Globals.rm.GetString("IDS_MENU_ZoomMode");
            ((ApplicationBarMenuItem)this.ApplicationBar.MenuItems[2]).Text = Globals.rm.GetString("IDS_MENU_Range");

             */ 
            for (int i = 0; i < Globals.NUM_GRAPHS; i++)
            {
                if (Globals.szGraphs[i] != null && Globals.szGraphs[i].Length > 0)
                {
                    iGraphSelected = i;
                    break;
                }
            }

            if(LoadGraphRange() == false)
            {
                ResetValues();
                SaveRangeValues();
            }

            if (Globals.bGraphUsePoints == true)
                FitToData();

            swBMPMeasure.Reset();
            swBMPMeasure.Start();
            
            ReDraw();
        }

        private void SaveRangeValues()
        {
            Globals.dbYMax = (dbYScale - dbYTranslation);
            Globals.dbYMin = (-1 * dbYTranslation);
            Globals.dbXmin = (-1 * dbXTranslation);
            Globals.dbXMax = (dbXScale - dbXTranslation);
        }

        private bool LoadGraphRange()
        {
            if (Globals.dbXmin >= Globals.dbXMax)
                return false;
            if (Globals.dbYMin >= Globals.dbYMax)
                return false;

            //ok we're good
            dbYTranslation = -1 * Globals.dbYMin;
            dbYScale = Globals.dbYMax + dbYTranslation;

            dbXTranslation = -1 * Globals.dbXmin;
            dbXScale = Globals.dbXMax + dbXTranslation;

            CalcGraphPoints();

            return true;
        }

        void DrawGraphs()
        {
            if (eGStyle == EnumGraphStyle.Points && iAccuracy < 2)
            {
                //Create the Point array if it hasn't already been created AND the graph equation is defined
                for (int i = 0; i < iScreenWidth / (1 + iAccuracy); i++)
                {
                    for (int j = 0; j < Globals.NUM_GRAPHS; j++)
                    {
                        if (iYGraph == null || iYGraph[j] == null)
                            continue;

                        Line line = new Line();
                        line.X1 = i * (1 + iAccuracy);
                        line.Y1 = iYGraph[j][i];
                        line.X2 = line.X1 + 1;
                        line.Y2 = iYGraph[j][i];

                        line.StrokeThickness = 1;
                        line.Stroke = Globals.Brush.GetBrush(GetGraphColor(j));
                        line.IsHitTestVisible = false;

                        myCanvas.Children.Add(line);
                    }
                }
            }
	        else if(iAccuracy < 2)//Draw Lines
	        {
                int iThick = 1;
                if (eGStyle == EnumGraphStyle.Thick)
                    iThick = 2;

                double X, Y;
 
		        for(int j = 0; j < Globals.NUM_GRAPHS; j++)
		        {
			        if(iYGraph == null || iYGraph[j] == null)
				        continue;

                    Line line = new Line();
                    line.X1 = 0;
                    line.Y1 = iYGraph[j][0];
                    		        
			        for(int i = 1; i < iScreenWidth/(1+iAccuracy); i++)
			        {
                        line.X2 = i * (1 + iAccuracy);
                        line.Y2 = iYGraph[j][i];

                        line.StrokeThickness = iThick;
                        line.Stroke = Globals.Brush.GetBrush(GetGraphColor(j));
                        line.IsHitTestVisible = false;

                        myCanvas.Children.Add(line);

                        X = line.X2;
                        Y = line.Y2;

                        line = new Line();

                        line.X1 = X;
                        line.Y1 = Y;
			        }
		        }
	        }
        }

        Color GetGraphColor(int iIndex)
        {
            switch (iIndex)
            {
                case 0:
                    return Colors.Red;

                case 1:
                    return Color.FromArgb(255, 00, 127, 255);

                case 2:
                    return Color.FromArgb(255, 00, 255, 127);

                case 3:
                    return Colors.Yellow;

                case 4:
                    return Colors.Orange;

                case 5:
                    return Colors.Purple;

                case 6:
                    return Colors.Brown;

                case 7:
                    return Colors.Blue;


                default:
                    return Colors.Magenta;
            }
        }

        private void myCanvas_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {

        }

        private void MenuZoomY_Click(object sender, EventArgs e)
        {
            eZoomMode = EnumZoomMode.ZOOM_Y;
        }

        private void MenuZoomX_Click(object sender, EventArgs e)
        {
            eZoomMode = EnumZoomMode.ZOOM_X;
        }

        private void MenuZoomNormal_Click(object sender, EventArgs e)
        {
            eZoomMode = EnumZoomMode.ZOOM_Normal;
        }

        private void MenuZoomMode_Click(object sender, EventArgs e)
        {
         /*   PopupMenu pop = new PopupMenu((FrameworkElement)this, Globals.rm.GetString("IDS_MENU_ZoomMode"));

            pop.AddItem(Globals.rm.GetString("IDS_MENU_ZoomNormal"));
            pop.AddItem(Globals.rm.GetString("IDS_MENU_ZoomX"));
            pop.AddItem(Globals.rm.GetString("IDS_MENU_ZoomY"));

            pop.Launch();
                        
            pop.SetCurrentIndex((int)eZoomMode);
            
            pop.SelectionChanged += delegate(object MySender, PopupMenuEventArgs eArg)
            {
                //_oUnitConv.SetSubSectionTo(eArg.iSelected);
                eZoomMode = (EnumZoomMode)eArg.iSelected;
            };
          * 
          */ 
        }

        
        private void MenuMode_Click(object sender, EventArgs e)
        {
            if (eGraphMode == EnumGraphMode.MODE_Translate)
            {
                eGraphMode = EnumGraphMode.MODE_GetLocation;
                textBlockMode.Text = Globals.rm.GetString("IDS_MENU_PointOnGraph");
            }
            else
            {
                eGraphMode = EnumGraphMode.MODE_Translate;
                textBlockMode.Text = Globals.rm.GetString("IDS_MENU_Translate");
            }
            ReDraw();
        }

        private void MenuGraph_Click(object sender, EventArgs e)
        {
            int iTemp = iGraphSelected + 1;

            int iCount = 0;
            while (iTemp != iGraphSelected && iCount < Globals.NUM_GRAPHS + 1)
            {
                if (iTemp >= Globals.NUM_GRAPHS)
                    iTemp = 0;

                if (Globals.szGraphs[iTemp] != null && Globals.szGraphs[iTemp].Length > 0)
                    iGraphSelected = iTemp;
                else
                    iTemp++;
                iCount++;
            }

            ReDraw();//probably not nessesary
        }

        private void MenuZoomIn_Click(object sender, EventArgs e)
        {
            ZoomIn();
        }

        private void MenuZoomOut_Click(object sender, EventArgs e)
        {
            ZoomOut();
        }

        private void MenuReset_Click(object sender, EventArgs e)
        {
            ResetValues();
            ReDraw();
        }

        private void MenuCenter_Click(object sender, EventArgs e)
        {
            CenterGraph();
        }

        private void MenuRange_Click(object sender, EventArgs e)
        {
            //NavigationService.Navigate(new Uri("/Screens/GraphScale.xaml?OptionType=0", UriKind.Relative));

        }

        //we gotta set the Scale AND the translation
        void ZoomIn()
        {
	        if(dbXScale/ZOOM_IN_FACTOR < MIN_SCALE && eZoomMode != EnumZoomMode.ZOOM_Y)
		        return;
            if (dbYScale / ZOOM_IN_FACTOR < MIN_SCALE && eZoomMode != EnumZoomMode.ZOOM_X)
		        return;

            if (eZoomMode != EnumZoomMode.ZOOM_Y)
            {
                dbXScale /= ZOOM_IN_FACTOR;
                //Old Trans        New Trans		  Old Scale                        New Scale
                dbXTranslation = dbXTranslation - dbXScale*ZOOM_IN_FACTOR/2.0 + dbXScale/2.0;
            }
            if (eZoomMode != EnumZoomMode.ZOOM_X)
            {
	            dbYScale /= ZOOM_IN_FACTOR;
                //Old Trans        New Trans		  Old Scale                        New Scale
                dbYTranslation = dbYTranslation - dbYScale*ZOOM_IN_FACTOR/2.0 + dbYScale/2.0;
            }

            SaveRangeValues();
            ReDraw();
        }

        void ZoomOut()
        {
            if (dbXScale * ZOOM_IN_FACTOR > MAX_SCALE && eZoomMode != EnumZoomMode.ZOOM_Y)
		        return;
            if (dbYScale * ZOOM_IN_FACTOR > MAX_SCALE && eZoomMode != EnumZoomMode.ZOOM_X)
		        return;

            if (eZoomMode != EnumZoomMode.ZOOM_Y)
            {   //Old Trans        New Trans		  Old Scale                        New Scale
                dbXScale *= ZOOM_OUT_FACTOR;
                dbXTranslation = dbXTranslation - dbXScale/ZOOM_OUT_FACTOR/2.0 + dbXScale/2.0;
            }

            if (eZoomMode != EnumZoomMode.ZOOM_X)
            {
                dbYScale *= ZOOM_OUT_FACTOR;
                dbYTranslation = dbYTranslation - dbYScale/ZOOM_OUT_FACTOR/2.0 + dbYScale/2.0;
            }

            SaveRangeValues();
            ReDraw();
        }

        void CenterGraph()
        {
           // int NUMBER_OF_STEPS = 15;

	        //find y location at center of screen
	        double dbX = dbXScale/2 - dbXTranslation;

	        string szEq;	
	        string szDoubleTemp;			//convert our doubles to string for the ol calc engine

	        //now screen points are relative to say...the SCREEN..so we don't need an x
	        //don't forget the damn Y is upside down...we'll do that later
	        //lets build our string to parse
            szDoubleTemp = "(" + dbX.ToString() + ")";

            if (iGraphSelected < 0)
                return;

            if (iGraphSelected > -1 && Globals.szGraphs[iGraphSelected] == null)
                return;


            szEq = Globals.szGraphs[iGraphSelected].Replace("X", szDoubleTemp);


            double dbAns = oCalc.Calculate(szEq, ref szEq);
            
	        //gotta make sure dbAns isn't an error 
	        if(szEq[0] == ('N') || szEq[0] == ('I'))
		        return;

	        double dbTemp = dbYTranslation;
	        double dbDelta = dbAns + dbYTranslation - dbYScale/2;

	        if(dbDelta == 0)
	        {	//we're already centered
		        return;
	        }

	        //log translation?
	   /*     for(int i = 0; i < NUMBER_OF_STEPS; i++)
	        {
		        dbYTranslation = log10((i+1)*10/(double)NUMBER_OF_STEPS)*dbDelta*-1.0 + dbTemp;

		        DrawGrid(oGDI.GetDC());
		        DrawAxis(oGDI.GetDC());
		        DrawScale(oGDI.GetDC());

		        BitBlt(dc, rcClient.left, rcClient.top, WIDTH(rcClient), HEIGHT(rcClient),
			        oGDI.GetDC(), rcClient.left, rcClient.top, SRCCOPY);

		        Sleep(33);//make it look good
	        }*/

	        //lets just set the final location
	        dbYTranslation = dbYScale/2 - dbAns;

            ReDraw();

        }

        void SetDisplayText()
        {
            if (iGraphSelected > -1 && 
                Globals.szGraphs[iGraphSelected] != null && 
                Globals.szGraphs[iGraphSelected].Length > 0)
            {
                textBlockGraph.Text = "f(x) = " + Globals.szGraphs[iGraphSelected];
                textBlockGraph.Foreground  = Globals.Brush.GetBrush(GetGraphColor(iGraphSelected));
            }
            else
                textBlockGraph.Text = "";

            textBlockTop.Text = String.Format("{0:0.###}", (dbYScale - dbYTranslation));
            textBlockBottom.Text = String.Format("{0:0.###}", (-1*dbYTranslation));
            textBlockLeft.Text = String.Format("{0:0.###}", (-1*dbXTranslation));
            textBlockRight.Text = String.Format("{0:0.###}", (dbXScale - dbXTranslation));

        }

        void RemovePointLocation()
        {
            myCanvas.Children.Remove(lnPointX);
            myCanvas.Children.Remove(lnPointY);

            myCanvas.Children.Remove(tbPointX);
            myCanvas.Children.Remove(tbPointY);
        }

        void DrawPointLocation()
        {
	        if(iGraphSelected < 0 ||
		        iGraphSelected > Globals.NUM_GRAPHS)
		        return;

            RemovePointLocation();

            Color rgb = GetGraphColor(iGraphSelected);

            int iX = (int)ptEndSelect.X;

	        double dbX = iX/(double)iScreenWidth*dbXScale -dbXTranslation;

	        string szDouble;

	        szDouble = dbX.ToString();

	        string szTemp;
	        szTemp = Globals.szGraphs[iGraphSelected];

            szTemp = szTemp.Replace("X", szDouble);

	        //get the answer 
	        double dbY = oCalc.Calculate(szTemp, ref szTemp);

	        //now we have 2 doubles and 2 TCHARs...cool eh?..one 1 int though
	        int iY = (int)(iScreenHeight - (dbY+dbYTranslation)*(double)iScreenHeight/dbYScale);

	        //there we go....
            if ((iX > 0 && iX < iScreenWidth) &&
                iY > 0 && iY < iScreenHeight)
            {
                lnPointX = new Line();

                lnPointX.X1 = 0;
                lnPointX.Y1 = iY + 0.5;
                lnPointX.X2 = iScreenWidth;
                lnPointX.Y2 = iY + 0.5;
                lnPointX.StrokeThickness = 1;
                lnPointX.Stroke = Globals.Brush.GetBrush(GetGraphColor(iGraphSelected));
                lnPointX.IsHitTestVisible = false;

                myCanvas.Children.Add(lnPointX);


                lnPointY = new Line();

                lnPointY.X1 = iX + 0.5;
                lnPointY.Y1 = 0;
                lnPointY.X2 = iX + 0.5;
                lnPointY.Y2 = iScreenHeight;
                lnPointY.StrokeThickness = 1;
                lnPointY.Stroke = Globals.Brush.GetBrush(GetGraphColor(iGraphSelected));
                lnPointY.IsHitTestVisible = false;

                myCanvas.Children.Add(lnPointY);


                //and of course the text boxes
                tbPointX = new TextBox();
                tbPointY = new TextBox();

                Thickness thick = new Thickness();


                tbPointX.Width = iScreenWidth;
                tbPointY.Width = iScreenWidth;

                tbPointX.Height = 70;
                tbPointY.Height = 70;

                tbPointX.HorizontalAlignment = HorizontalAlignment.Left;
                tbPointX.VerticalAlignment = VerticalAlignment.Top;

                tbPointY.HorizontalAlignment = HorizontalAlignment.Left;
                tbPointY.VerticalAlignment = VerticalAlignment.Top;

                tbPointX.Foreground = Globals.Brush.GetBrush(GetGraphColor(iGraphSelected));
                tbPointY.Foreground = Globals.Brush.GetBrush(GetGraphColor(iGraphSelected));

                tbPointX.Background = Globals.Brush.GetBrush(Color.FromArgb(00, 00, 00, 00));
                tbPointY.Background = Globals.Brush.GetBrush(Color.FromArgb(00, 00, 00, 00));

                tbPointX.BorderBrush = Globals.Brush.GetBrush(Color.FromArgb(00, 00, 00, 00));
                tbPointY.BorderBrush = Globals.Brush.GetBrush(Color.FromArgb(00, 00, 00, 00));

                if (iX > iScreenWidth / 2)
                {
                 //   tbPointX.HorizontalAlignment = HorizontalAlignment.Right;
                 //   tbPointX.TextAlignment = TextAlignment.Right;

                 //   tbPointX.HorizontalContentAlignment = HorizontalAlignment.Right;
                 //   tbPointY.HorizontalContentAlignment = HorizontalAlignment.Right;

                    thick.Left = 0;// iX - 5;
                    thick.Right = iX + 5;
                    thick.Top =  iY - tbPointX.Height + 20;
                    thick.Bottom = 0;

                    tbPointX.Margin = thick;

                    thick = new Thickness();

                    thick.Left = 0;// iX - 5;
                    thick.Right = iX + 5;
                    thick.Top = iY - 20;
                    thick.Bottom = 0;

                    tbPointY.Margin = thick;
                }
                else
                {
                    thick.Left = iX - 5;
                    thick.Right = thick.Left + iScreenWidth/2;
                    thick.Top = iY - 5 - tbPointX.Height + 20;
                    thick.Bottom = thick.Top - iScreenHeight/2;

                    tbPointX.Margin = thick;

                    thick = new Thickness();

                    thick.Left = iX - 5;
                    thick.Right = 0;
                    thick.Top = iY - 20;
                    thick.Bottom = 0;

                    tbPointY.Margin = thick;
                }

                tbPointX.Text = szDouble;
                tbPointY.Text = szTemp;

                tbPointX.IsHitTestVisible = false;
                tbPointY.IsHitTestVisible = false;

                myCanvas.Children.Add(tbPointX);
                myCanvas.Children.Add(tbPointY);
            }
	        //oh hell...lets draw some co-ordinates too
       /*     if(iX > m_iScreenWidth/2)
            {
                DrawText(dc, szDouble, iX-5, iY-GetSystemMetrics(SM_CXICON)/2, DT_RIGHT, m_hFontText, rgb);
                DrawText(dc, szTemp,	iX-5, iY, DT_RIGHT, m_hFontText, rgb);
            }
            else
            { 
	            DrawText(dc, szDouble, 5+iX, iY-GetSystemMetrics(SM_CXICON)/2, DT_LEFT, m_hFontText, rgb);
	            DrawText(dc, szTemp,	5+iX, iY, DT_LEFT, m_hFontText, rgb);
            }*/
        }

        
        void FitToData()
        {
            if(Globals.arrGraphPoints == null || Globals.arrGraphPoints.Count < 2)
                return;

            double dbXMin = Globals.arrGraphPoints[0].X;
            double dbXMax = Globals.arrGraphPoints[0].X;
            double dbYMin = Globals.arrGraphPoints[0].Y;
            double dbYMax = Globals.arrGraphPoints[0].Y;

            for (int i = 1; i < Globals.arrGraphPoints.Count; i++)
            {
                dbXMin = Math.Min(dbXMin, Globals.arrGraphPoints[i].X);
                dbXMax = Math.Max(dbXMax, Globals.arrGraphPoints[i].X);
                dbYMin = Math.Min(dbYMin, Globals.arrGraphPoints[i].Y);
                dbYMax = Math.Max(dbYMax, Globals.arrGraphPoints[i].Y);
            }

            if(dbXMin == dbXMax ||
                dbYMin == dbYMax)
                return;

            double dbXPad = (dbXMax - dbXMin)/10;
            double dbYPad = (dbYMax - dbYMin)/10;

            //set scale and translation
            dbXTranslation = 0 - dbXMin + dbXPad;
            dbYTranslation = 0 - dbYMin + dbYPad;//gross but just the way it is

            //uhhh ... 
            dbXScale       = 12*dbXPad;
            dbYScale       = 12*dbYPad;

            //done
        }

        
    }
}