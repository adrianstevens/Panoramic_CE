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
    public partial class Standard : UserControl, ICalcProScreen
	{
        ICalcProDisplay display = null;

		public Standard()
		{
			// Required to initialize variables
			InitializeComponent();

            //subscribe
            //Globals.Calc.DisplayUpdate += UpdateDisplay;
            //Globals.btnMan.SkinUpdate += UpdateSkin;
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
 
        }

        public void UpdateDisplay(bool bUpdate, int iBtnIndex, int iBtnId)
        {
        }

        public void UpdateSkin(Color crBackground, EnumCalcProSkinColor eColor, EnumCalcProSkinStyle eStyle)
        {
            gridMain.Children.Clear();
            gridDisplay.Children.Clear();
            gridTop.Children.Clear();
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
                //Globals.Calc.StateUpdate += UpdateCalcState;
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
                //Globals.Calc.StateUpdate -= UpdateCalcState;
                //Globals.btnMan.SkinUpdate += UpdateSkin;

                if (display != null)
                    display.ReleaseDataContext();
            }

            if (IsScreenLoaded())
            {
                return;
            }

            Globals.btnMan.SetCalcType(EnumCalcType.CALC_Standard);
            Globals.btnMan.CreateAltButtons(ref gridTop, EnumButtonLayout.BL_Alt);
            Globals.btnMan.CreateMainButtons(ref  gridMain);
            display = Globals.btnMan.CreateDisplay(ref gridDisplay);
            display.SetDisplayType(Globals.btnMan.GetDisplayType(), true);

            /*SolidColorBrush stroke = new SolidColorBrush();
            stroke.Color = Globals.btnMan.GetBackgroundColor();
            LayoutRoot.Background = stroke;*/
        }

        public void OnBackButton(object sender, CancelEventArgs e)
        {

        }

	}
}