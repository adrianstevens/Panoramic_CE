using System;
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
using System.ComponentModel;

enum EnumConstButtons
{
    CSB_1,
    CSB_2,
    CSB_3,
    CSB_4,
    CSB_AddFav,
    CSB_Count,
};

enum EnumConstDisplay
{
    CD_Normal,
    CD_LastValue,
};

namespace CalcPro
{
    public partial class Constants : UserControl, ICalcProScreen
	{
        //ICalcProBtn[(int)(EnumConstButtons.CSB_Count)] oConstButtons;
        ICalcProBtn[] btnConsts = new ICalcProBtn[4];
        ICalcProBtn btnAddFav = null;
        ICalcProDisplay display = null;
        TextBlock txtDisplay = new TextBlock();

        int _iIndex;//lazyness
        
		public Constants()
		{
			// Required to initialize variables
			InitializeComponent();

            txtDisplay.FontSize = 24;
			
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

        void SetFontColors()
        {
            if (txtDisplay == null)
                return;

            txtDisplay.Foreground = Globals.btnMan.brAltTextColor;

            Globals.btnMan.SetFontItem(ref txtDisplay);
        }

        public void LoadScreen(bool bOnScreen)
        {
            if (bOnScreen == true)
            {
                //subscribe
                //Globals.Calc.DisplayUpdate += UpdateDisplay;
                //Globals.Calc.StateUpdate += UpdateCalcState;
                if (display != null)
                {
                    display.SetDisplayType(Globals.btnMan.GetDisplayType(), true);
                }
            }
            else
            {
                if (display != null)
                    display.ReleaseDataContext();
                //unsubscribe
            }

            if (IsScreenLoaded())
            {
                return;
            }

            Globals.btnMan.SetCalcType(EnumCalcType.CALC_Constants);

            Globals.btnMan.CreateMainButtons(ref  gridMain);

            display = Globals.btnMan.CreateDisplay(ref gridDisplay);
            display.SetDisplayType(Globals.btnMan.GetDisplayType(), true);

            string szName = "dispConst";
            Globals.btnMan.CreateDisplay(ref gridDisplayConst, szName, EnumCalcDisplay.Constants);

            InitButtons();
            InitDisplays();
            SetFontColors();
        }

        void InitDisplays()
        {
            txtDisplay.TextAlignment = TextAlignment.Center;
            txtDisplay.Foreground = Globals.Brush.GetBrush(Color.FromArgb(255, 00, 00, 00));
            txtDisplay.Text = "";
            txtDisplay.Margin = new Thickness(10);

            Grid.SetColumn(txtDisplay, 0);
            Grid.SetRow(txtDisplay, 1);
            Grid.SetColumnSpan(txtDisplay, 3);

            gridTop.Children.Add(txtDisplay);
        }

        void InitButtons()
        {
            double dbMargin = Globals.MARGIN;

            UserControl uCon;//just a reference

            string szTemp;

            for (int i = 0; i < 4; i++)
            {
                Globals.btnMan.CreateButton(ref btnConsts[i], 0);

                szTemp = "Const" + i.ToString();

                //cast to a user control
                uCon = (UserControl)btnConsts[i];

                uCon.SetValue(FrameworkElement.NameProperty, szTemp);

                btnConsts[i].cBtn.iBtnIndex = 0;
                btnConsts[i].cBtn.iBtnType = 0;

                btnConsts[i].SetMainText("");
                btnConsts[i].SetExpText("");

                int iTemp = i;

                btnConsts[i].BtnSelected += delegate(object MySender, EventArgs eArg)
                {
                    ClickButton(btnConsts[iTemp], iTemp);
                };
                
                Grid.SetColumn(uCon, i);
                Grid.SetRow(uCon, 2);
  
                gridTop.Children.Add(uCon);
            }

            //favorites buttons
            Globals.btnMan.CreateButton(ref btnAddFav, 2);

            szTemp = "AddFav";//" + Fav ";

            //cast to a user control
            uCon = (UserControl)btnAddFav;

            uCon.SetValue(FrameworkElement.NameProperty, szTemp);

            btnAddFav.cBtn.iBtnIndex = 0;
            btnAddFav.cBtn.iBtnType = 0;

            btnAddFav.SetMainText(szTemp);
            btnAddFav.SetExpText("");

            btnAddFav.BtnSelected += delegate(object MySender, EventArgs eArg)
            {
                Globals.objConstants.AddToFavorites();
            };

            Grid.SetColumn(uCon, 3);
            Grid.SetRow(uCon, 1);
            
            gridTop.Children.Add(uCon);

            ShowHideButtons(0);
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
           
        }

        public void UpdateDisplay(bool bUpdate, int iBtnIndex, int iBtnId)
        {
		}

        public void UpdateSkin(Color crBackground, EnumCalcProSkinColor eColor, EnumCalcProSkinStyle eStyle)
        {
            SetFontColors();
        }

        void const_Click(object sender, RoutedEventArgs e)
        {
            int iSel = Convert.ToInt32(((MenuItem)sender).Uid);

            int iIndex = _iIndex;

            if (Globals.objConstants.IsGroupAConstant(iIndex, iSel))
            {
                Globals.Calc.AddString(Globals.objConstants.GetGroupConstant(Globals.objConstants.iLastLevel, iSel));

                txtDisplay.Text = "\r\n" + Globals.objConstants.GetGroupName(iIndex, iSel);
                return;
            }
            else
            {
                string szGroups = "";
                int iStart = (iIndex == 3 ? 1 : 0);
                for (int i = iStart; i < iIndex; i++)
                    szGroups = szGroups + Globals.objConstants.GetSelectedSection(i) + "\r\n";

                szGroups += Globals.objConstants.GetGroupName(iIndex, iSel);

                txtDisplay.Text = szGroups;
                ShowHideButtons(iIndex + 1);
            }

            Globals.objConstants.SetGroup(iIndex, iSel);

            if (_iIndex + 1 >= 4)
                return;
            

        }

        private void ClickButton(ICalcProBtn sender, int iIndex)
        {
            if (sender.ToString() == "")
                return;

            if (Globals.objConstants.GetGroupCount(iIndex) == 0)
                return;

            _iIndex = iIndex;

            UserControl uCon = (UserControl)sender;

            ContextMenu menu = new ContextMenu();
            
            
            for (int i = 0; i < Globals.objConstants.GetGroupCount(iIndex); i++)
            {
                MenuItem item = new MenuItem();
                item.Header = Globals.objConstants.GetGroupName(iIndex, i);
                item.FontSize = 20;
                item.Click += new RoutedEventHandler(const_Click);
                item.Uid = i.ToString();
                menu.Items.Add(item);
            }

            menu.PlacementTarget = uCon;
            menu.IsOpen = true;
        }

        void ShowHideButtons(int iIndex)
        {
            UserControl uCon1 = (UserControl)btnConsts[1];
            UserControl uCon2 = (UserControl)btnConsts[2];
            UserControl uCon3 = (UserControl)btnConsts[3];

            if (iIndex == 0)
            {
                uCon1.Visibility = Visibility.Collapsed;
                uCon2.Visibility = Visibility.Collapsed;
                uCon3.Visibility = Visibility.Collapsed;
                btnConsts[0].SetMainText(Globals.rm.GetString("IDS_MENU_Constants"));
            }
            else if (iIndex == 1)
            {
                uCon1.Visibility = Visibility.Visible;
                uCon2.Visibility = Visibility.Collapsed;
                uCon3.Visibility = Visibility.Collapsed;
                btnConsts[0].SetMainText(Globals.rm.GetString("IDS_MENU_Group"));
                btnConsts[1].SetMainText(Globals.rm.GetString("IDS_MENU_Constants"));
            }
            else if(iIndex == 2)
            {
                uCon1.Visibility = Visibility.Visible;
                uCon2.Visibility = Visibility.Visible;
                uCon3.Visibility = Visibility.Collapsed;
                btnConsts[0].SetMainText(Globals.rm.GetString("IDS_MENU_Group"));
                btnConsts[1].SetMainText(Globals.rm.GetString("IDS_MENU_Group"));
                btnConsts[2].SetMainText(Globals.rm.GetString("IDS_MENU_Constants"));
            }
            else if (iIndex == 3)
            {
                uCon1.Visibility = Visibility.Visible;
                uCon2.Visibility = Visibility.Visible;
                uCon3.Visibility = Visibility.Visible;
                btnConsts[0].SetMainText(Globals.rm.GetString("IDS_MENU_Group"));
                btnConsts[1].SetMainText(Globals.rm.GetString("IDS_MENU_Group"));
                btnConsts[2].SetMainText(Globals.rm.GetString("IDS_MENU_Group"));
                btnConsts[3].SetMainText(Globals.rm.GetString("IDS_MENU_Constants"));
            }
        }

        public void OnBackButton(object sender, CancelEventArgs e)
        {
        /*    if (PopupMenu.bPopupActivated && PopupMenu.popupCurrent != null)
            {
                e.Cancel = true;
                PopupMenu.popupCurrent.CloseWindow();
            } */
        }
	}
}