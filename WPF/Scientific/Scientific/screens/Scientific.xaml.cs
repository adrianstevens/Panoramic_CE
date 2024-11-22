using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.ComponentModel;

namespace CalcPro
{
    public partial class Scientific : UserControl, ICalcProScreen
	{
        ICalcProDisplay display = null;

		public Scientific()
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
                //Globals.Calc.DisplayUpdate += UpdateDisplay;
                Globals.Calc.StateUpdate += UpdateCalcState;
                //Globals.btnMan.SkinUpdate += UpdateSkin;

                if (display != null)
                {
                    display.SetDisplayType(Globals.btnMan.GetDisplayType(), true);
                }
            }
            else
            {
                //unsubscribe
                //Globals.Calc.DisplayUpdate -= UpdateDisplay;
                Globals.Calc.StateUpdate -= UpdateCalcState;
                //Globals.btnMan.SkinUpdate += UpdateSkin;

                if (display != null)
                    display.ReleaseDataContext();
            }
            if (IsScreenLoaded())
                return;

            if(Globals.Settings.eCalcType == CalcOperationType.CALC_RPN)
                Globals.btnMan.SetCalcType(EnumCalcType.CALC_SciRPN);
            else
                Globals.btnMan.SetCalcType(EnumCalcType.CALC_Scientific);
            Globals.btnMan.CreateAltButtons(ref gridTop, EnumButtonLayout.BL_Alt);
            Globals.btnMan.CreateMainButtons(ref  gridMain);
            display = Globals.btnMan.CreateDisplay(ref gridDisplay);
            display.SetDisplayType(Globals.btnMan.GetDisplayType(), true);
        }



        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            
        }

        public void UpdateSkin(Color crBackground, EnumCalcProSkinColor eColor, EnumCalcProSkinStyle eStyle)
        {
      //      gridMain.Children.Clear();
      //      gridDisplay.Children.Clear();
      //      gridTop.Children.Clear();
        }

        public void UpdateDisplay(bool bUpdate, int iBtnIndex, int iBtnId)
        {

        }

        public void UpdateCalcState(int iCalcState)
        {
            Globals.btnMan.SetCalcType(EnumCalcType.CALC_Scientific);

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

        void ShowLayout(EnumButtonLayout eLayout)
        {


        }

        public void OnBackButton(object sender, CancelEventArgs e)
        {

        }
	}
}