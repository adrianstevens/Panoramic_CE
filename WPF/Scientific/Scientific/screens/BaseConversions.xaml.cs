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
    public partial class BaseConversions : UserControl, ICalcProScreen
	{
        ICalcProDisplay display = null; //so we can enable/disable

		public BaseConversions()
		{
			// Required to initialize variables
			InitializeComponent();
			
            gridTop.Visibility = Visibility.Visible;
            grid2nd.Visibility = Visibility.Collapsed;
		}

        public bool IsScreenLoaded()
        {
            return gridMain.Children.Count > 0;
        }
        public void CloseSaveState()
        {
            if (!IsScreenLoaded())
                return;
        }

        public void LoadScreen(bool bOnScreen)
        {
            if (bOnScreen == true)
            {
                //subscribe
                //Globals.Calc.DisplayUpdate += UpdateDisplay;
                Globals.Calc.StateUpdate += UpdateCalcState;
                if (display != null)
                {
                    display.SetDisplayType(Globals.btnMan.GetDisplayType(), true);
                }
            }
            else
            {
                //unsubscribe
                Globals.Calc.StateUpdate -= UpdateCalcState;
                if(display != null) 
                    display.ReleaseDataContext();
            }

            if (IsScreenLoaded())
            {
                return;
            }

            Globals.btnMan.SetCalcType(EnumCalcType.CALC_BaseConversions);
            Globals.btnMan.CreateAltButtons(ref gridTop, EnumButtonLayout.BL_Alt);
            Globals.btnMan.CreateMainButtons(ref  gridMain);
            display = Globals.btnMan.CreateDisplay(ref gridDisplay);
            display.SetDisplayType(Globals.btnMan.GetDisplayType(), true);
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void UserControl_LayoutUpdated(object sender, EventArgs e)
        {

        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            
        }

        public void UpdateDisplay(bool bUpdate, int iBtnIndex, int iBtnId)
        {

        }

        public void UpdateCalcState(int iCalcState)
        {
            Globals.btnMan.SetCalcType(EnumCalcType.CALC_BaseConversions);

            switch (iCalcState)
            {
                case (int)CalcStateType.CALCSTATE_2ndF:
                    if (grid2nd.Visibility == Visibility.Collapsed)
                    {
                        if (grid2nd.Children.Count == 0)
                        {
                            Globals.btnMan.CreateAltButtons(ref grid2nd, EnumButtonLayout.BL_Alt2ndF);
                        }

                        gridTop.Visibility = Visibility.Collapsed;
                        grid2nd.Visibility = Visibility.Visible;
                    }
                    break;
                default:
                    if (gridTop.Visibility == Visibility.Collapsed)
                    {
                        gridTop.Visibility = Visibility.Visible;
                        grid2nd.Visibility = Visibility.Collapsed;
                    }
                    break;
            }
        }

        public void OnBackButton(object sender, CancelEventArgs e)
        {

        }
	}
}