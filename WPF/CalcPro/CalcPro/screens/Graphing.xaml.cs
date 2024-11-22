using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Resources;
using System.Reflection;

using System.Windows.Navigation;
using System.ComponentModel;

namespace CalcPro
{
    public partial class Graphing : UserControl, ICalcProScreen
	{
        ICalcProBtn btnGraph = null;
        ICalcProDisplay display = null;

        ContextMenu graphMenu = null;

    /*    int Globals.iGraphIndex //currently selected graph
        {
            get{return _iGraphIndex;}
            set{_iGraphIndex = value;}
        }
        int _iGraphIndex;*/


		public Graphing()
		{
			// Required to initialize variables
			InitializeComponent();

            gridTop.Visibility = Visibility.Visible;
            grid2nd.Visibility = Visibility.Collapsed;
            gridHyp.Visibility = Visibility.Collapsed;
            grid2ndHyp.Visibility = Visibility.Collapsed;
		}

        public void CloseSaveState()
        {
            if (!IsScreenLoaded())
                return;
        }
        public bool IsScreenLoaded()
        {
            return gridMain.Children.Count > 0;
        }

        public void LoadScreen(bool bOnScreen)
        {
            if (bOnScreen == true)
            {
                //subscribe
                Globals.Calc.DisplayUpdate += UpdateDisplay;
                Globals.Calc.StateUpdate += UpdateCalcState;
                //Globals.btnMan.SkinUpdate += UpdateSkin;

                if (display != null)
                {
                    display.SetDisplayType(EnumCalcDisplay.Expression, true);
                }

                if (Globals.szGraphs[Globals.iGraphIndex] != null)
                {
                    SetCurrentGraph(Globals.iGraphIndex);
                }
                else
                {
                    for (int i = 0; i < Globals.NUM_GRAPHS; i++)
                    {
                        if (Globals.szGraphs[i] != null &&
                            Globals.szGraphs[i].Length > 0)
                        {
                            Globals.Calc.AddString(Globals.szGraphs[i]);
                            //Globals.iGraphIndex = i;
                            SetCurrentGraph(i);
                            break;//only gotta set one ...
                        }
                    }
                }
            }
            else
            {
                //unsubscribe
                Globals.Calc.DisplayUpdate -= UpdateDisplay;
                Globals.Calc.StateUpdate -= UpdateCalcState;
                //Globals.btnMan.SkinUpdate += UpdateSkin;

                if (display != null)
                    display.ReleaseDataContext();
            }

            if (IsScreenLoaded())
            {
                return;
            }

            Globals.btnMan.SetCalcType(EnumCalcType.CALC_Graphing);
            Globals.btnMan.CreateMainButtons(ref  gridMain);
            Globals.btnMan.CreateAltButtons(ref  gridTop, EnumButtonLayout.BL_Alt);
            display = Globals.btnMan.CreateDisplay(ref gridDisplay, EnumCalcDisplay.Graphing);
            display.SetDisplayType(EnumCalcDisplay.Expression, true);

            InitButtons();
        }

        void InitButtons()
        {
            double dbMargin = Globals.MARGIN;

            UserControl uCon;//just a reference

            Globals.btnMan.CreateButton(ref btnGraph, 4);

            string szTemp = "Graph";

            //cast to a user control
            uCon = (UserControl)btnGraph;

            uCon.SetValue(FrameworkElement.NameProperty, szTemp);

            btnGraph.cBtn.iBtnIndex = 0;
            btnGraph.cBtn.iBtnType = 0;

            btnGraph.SetMainText("f1(x)");
            btnGraph.SetExpText("");

            btnGraph.BtnSelected += delegate(object MySender, EventArgs eArg)
            {
                OnGraphButton(btnGraph);
            };

            Grid.SetColumn(uCon, 3);
            Grid.SetRow(uCon, 1);
            Grid.SetColumnSpan(uCon, 2);

            gridDisplayMain.Children.Add(uCon);

        }

        void graph_Click(object sender, RoutedEventArgs e)
        {
            int i = Convert.ToInt32(((MenuItem)sender).Uid);
            SetCurrentGraph(i);
        }

        void CreateGraphMenu()
        {
            graphMenu = new ContextMenu();

            for (int i = 0; i < Globals.NUM_GRAPHS; i++)
            {
                string szTemp = "f" + (i + 1).ToString() + "(x) = ";

                if (Globals.szGraphs[i] != null)
                    szTemp += Globals.szGraphs[i];

                MenuItem item = new MenuItem();
                item.FontSize = 20;
                item.Header = szTemp;
                item.Click += new RoutedEventHandler(graph_Click);
                item.Uid = i.ToString();
                graphMenu.Items.Add(item);
            }
        }

        void OnGraphButton(ICalcProBtn sender)
        {
            //save the current graph
            SaveCurrentGraph();

            //needs to be done every time because the graph strings change
            CreateGraphMenu();

            graphMenu.PlacementTarget = (UIElement)btnGraph;
            graphMenu.VerticalAlignment = VerticalAlignment.Bottom;
            graphMenu.IsOpen = true;

            //we'll simply drop in a menu



            /*
            PopupMenu pop = new PopupMenu((FrameworkElement)this, Globals.rm.GetString("IDS_MENU_Graph"));

            string szTemp;

            for (int i = 0; i < Globals.NUM_GRAPHS; i++)
            {
                szTemp = "f" + (i+1).ToString() + "(x) = ";

                if (Globals.szGraphs[i] != null)
                    szTemp += Globals.szGraphs[i];

                pop.AddItem(szTemp);
            }

            pop.Launch();

            
            pop.SelectionChanged += delegate(object MySender, PopupMenuEventArgs eArg)
            {
                SetCurrentGraph(eArg.iSelected);
            };
             */ 
        }

        void SetCurrentGraph(int iIndex)
        {
            if (iIndex < 0 || iIndex >= Globals.NUM_GRAPHS || iIndex == Globals.iGraphIndex)
                return;

            string szTemp;
            szTemp = "f" + (iIndex+1).ToString() + "(x)";

            if (btnGraph != null)
            {
                btnGraph.SetMainText(szTemp);
            }
            ChangeGraph(iIndex);
        }

        private void LayoutRoot_LayoutUpdated(object sender, EventArgs e)
        {

        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            
        }

        public void UpdateDisplay(bool bUpdate, int iBtnIndex, int iBtnId)
        {
            if (iBtnIndex == (int)InputType.INPUT_Graph)
            {
                SaveCurrentGraph();
                Globals.bGraphUsePoints = false;
                Globals.LaunchPage((int)InputType.INPUT_Graph, 0);
            }
		}

        public void UpdateSkin(Color crBackground, EnumCalcProSkinColor eColor, EnumCalcProSkinStyle eStyle)
        {
        }

        public void UpdateCalcState(int iCalcState)
        {
            Globals.btnMan.SetCalcType(EnumCalcType.CALC_Graphing);

            switch (iCalcState)
            {
                case (int)CalcStateType.CALCSTATE_2ndF:
                    if(grid2nd.Visibility == Visibility.Collapsed)
                    {
                        if (grid2nd.Children.Count == 0)
                        {
                            Globals.btnMan.CreateAltButtons(ref grid2nd, EnumButtonLayout.BL_Alt2ndF);
                        }

                        gridTop.Visibility = Visibility.Collapsed;
                        grid2nd.Visibility = Visibility.Visible;
                        gridHyp.Visibility = Visibility.Collapsed;
                        grid2ndHyp.Visibility = Visibility.Collapsed;

                    }
                    break;
                case (int)CalcStateType.CALCSTATE_2ndF_Hyp:
                    if(grid2ndHyp.Visibility == Visibility.Collapsed)
                    {
                        if (grid2ndHyp.Children.Count == 0)
                        {
                            Globals.btnMan.CreateAltButtons(ref grid2ndHyp, EnumButtonLayout.BL_Alt2ndFHype);
                        }

                        gridTop.Visibility = Visibility.Collapsed;
                        grid2nd.Visibility = Visibility.Collapsed;
                        gridHyp.Visibility = Visibility.Collapsed;
                        grid2ndHyp.Visibility = Visibility.Visible;
                    }
                    break;
                case (int)CalcStateType.CALCSTATE_Hyp:
                    if(gridHyp.Visibility == Visibility.Collapsed)
                    {
                        if (gridHyp.Children.Count == 0)
                        {
                            Globals.btnMan.CreateAltButtons(ref gridHyp, EnumButtonLayout.BL_AltHyp);
                        }

                        gridTop.Visibility = Visibility.Collapsed;
                        grid2nd.Visibility = Visibility.Collapsed;
                        gridHyp.Visibility = Visibility.Visible;
                        grid2ndHyp.Visibility = Visibility.Collapsed;
                    }
                    break;
                default:
                    if(gridTop.Visibility == Visibility.Collapsed)
                    {
                        gridTop.Visibility = Visibility.Visible;
                        grid2nd.Visibility = Visibility.Collapsed;
                        gridHyp.Visibility = Visibility.Collapsed;
                        grid2ndHyp.Visibility = Visibility.Collapsed;
                    }
                    break;
            }
        }

        bool SaveCurrentGraph()
        {
            string szTemp = null;
            Globals.Calc.GetEquation(ref szTemp);

            CheckEquation(ref szTemp);

            Globals.szGraphs[Globals.iGraphIndex] = szTemp;
            return true;
        }

        bool CheckEquation(ref string szEquation)
        {
            if (szEquation == null)
                return false;

            int iIndex = 0; //brute force
            bool bOpp = false;
            while (iIndex < szEquation.Length - 1)
            {
                if (szEquation[iIndex] == 'X')
                {   //make sure there's an operator (OR a bracket dumbass) following it ... otherwise insert a *
                    //we're always in range
                    bOpp = false;
                    if (szEquation[iIndex + 1] == CalcGlobals.szBrackets[1][0])
                        break;

                    for (int i = 0; i < CalcGlobals.NUMBER_OF_Operators; i++)
                    {
                        if (szEquation[iIndex + 1] == CalcGlobals.szOperators[i][0])
                        {
                            bOpp = true;
                            break;
                        }
                    }
                    //also check for squared and cubed
                    if (szEquation[iIndex + 1] == ('\x00B2') || szEquation[iIndex + 1] == ('\x00B3'))
                        bOpp = true;


                    if (bOpp == false)//insert the multi symbol
                        szEquation = szEquation.Insert(iIndex + 1, CalcGlobals.szOperators[(int)OperatorType.OPP_Times]);
                }
                iIndex++;
            }

            //add closing brackets
            CalcUtil.FillOpenBrackets(ref szEquation);


            return true;
        }

        bool ChangeGraph(int iGraph)
        {
            if (iGraph < 0 || iGraph >= Globals.NUM_GRAPHS)
                return false;

            //we need to save the current graph
            //SaveCurrentGraph();

            Globals.iGraphIndex = iGraph;

            Globals.Calc.AddClear(ClearType.CLEAR_ClearAll);//gotta clear the equation

            if (Globals.szGraphs[Globals.iGraphIndex] != null)
            {
                Globals.Calc.UpdateDisplays("", Globals.szGraphs[Globals.iGraphIndex]);
            }
            else
            {
                Globals.szGraphs[Globals.iGraphIndex] = "";
                Globals.Calc.UpdateDisplays("", "");
            }


            return true;
        }

        public void OnBackButton(object sender, CancelEventArgs e)
        {

        }


	}
}