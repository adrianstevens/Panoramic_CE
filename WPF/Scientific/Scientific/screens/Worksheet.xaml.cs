using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;  
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.ComponentModel;
using System.Collections.Specialized;
using System.Resources;


public enum EntryType
{
    ENTRY_Double,
    ENTRY_Int,
    ENTRY_Percent,
    ENTRY_Currency,
};

namespace CalcPro.Screens
{
    public partial class Worksheet : Page
    {
        

        //this is linked to the current dataset
        public ObservableCollection<MyListboxItems> _arrItems;

        //public const int NUM_DATASETS = 4;
        //ObservableCollection<MyListboxItems>[] arrDataSets = new ObservableCollection<MyListboxItems>[NUM_DATASETS];
        //int iDataSet = 0;//current dataset for regression calcs

        IWorkSheet iCurrentWS;
        

        public Worksheet()
        {
            _arrItems = new ObservableCollection<MyListboxItems>();
            this.DataContext = _arrItems;

            InitializeComponent();

            //subscribe
            Globals.Calc.DisplayUpdate += UpdateDisplay;
            //Globals.btnMan.SkinUpdate += UpdateSkin;
        }

        ~Worksheet()
        {
            Globals.Calc.DisplayUpdate -= UpdateDisplay;
            Globals.btnMan.SkinUpdate -= UpdateSkin;
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            //let's set a background color
            LayoutRoot.Background = Globals.Brush.GetBrush(Globals.btnMan.GetBackgroundColor());

            // set the localized text
            // bugbug
            //((Microsoft.Phone.Shell.ApplicationBarIconButton)this.ApplicationBar.Buttons[0]).Text = Globals.rm.GetString("IDS_MENU_Back");

            if (gridMain.Children.Count != 0)
                return;

            if (gridMain.Children.Count == 0)
            {
                Globals.btnMan.SetCalcType(EnumCalcType.CALC_WorkSheet);
                Globals.btnMan.CreateMainButtons(ref  gridMain);
            }

            int iWS = 0;
            //bugbug
            //Int32.TryParse(NavigationContext.QueryString["LaunchType"], out iWS);

            Init((LaunchType)iWS);

            if(lbItems != null && lbItems.Items.Count > 0)
                lbItems.SelectedIndex = 0;

            this.DataContext = _arrItems;

            Globals.btnMan.CreateDisplay(ref DisplayGrid, "Display", EnumCalcDisplay.WorkSheet).SetDataContext();

            UpdateList(0);
        }

        public void AddlbItem(string szVariableName, string szTitle)
        {

        }

        public void OnDown()
        {
            if (lbItems == null || lbItems.Items.Count == 0)
                return;

            iCurrentWS.OnDown(lbItems.SelectedIndex);

            int iListBoxIndex = lbItems.SelectedIndex;
            iListBoxIndex++;

            if (iListBoxIndex >= lbItems.Items.Count)
                iListBoxIndex = 0;

            lbItems.SelectedIndex = iListBoxIndex;
            lbItems.UpdateLayout();
            lbItems.ScrollIntoView(lbItems.SelectedItem);

        }

        public void OnUp()
        {
            if (lbItems == null || lbItems.Items.Count == 0)
                return;

            int iListBoxIndex = lbItems.SelectedIndex;
            iListBoxIndex--;

            if (iListBoxIndex < 0)
                iListBoxIndex = lbItems.Items.Count - 1;

            lbItems.SelectedIndex = iListBoxIndex;
            lbItems.UpdateLayout();
            lbItems.ScrollIntoView(lbItems.SelectedItem);
        }

        public void UpdateDisplay(bool bUpdate, int iBtnIndex, int iBtnId)
        {
            if (iBtnIndex == -1 || iBtnId == -1)
                return;//gonna try this

            int iListBoxIndex = lbItems.SelectedIndex;

            string szTemp = null;
            Globals.Calc.GetAnswer(ref szTemp, false, false);

            if (iBtnId == (int)ClearType.CLEAR_Reset &&
                iBtnIndex == (int)InputType.INPUT_Clear)
            {
                lbItems.SelectedIndex = iListBoxIndex = 0;
                lbItems.UpdateLayout();
                lbItems.ScrollIntoView(lbItems.SelectedItem);
                iCurrentWS.OnReset();
            }

            //get the index
            if (iBtnIndex == (int)InputType.INPUT_WS)
            {
                switch ((WorksheetType)iBtnId)
                {
                    case WorksheetType.WS_Clear:
                        lbItems.SelectedIndex = iListBoxIndex = 0;
                        lbItems.UpdateLayout();
                        lbItems.ScrollIntoView(lbItems.SelectedItem);
                        iCurrentWS.OnReset();
                        break;
                    case WorksheetType.WS_Comp:
                        iCurrentWS.OnCalc(iListBoxIndex, (FrameworkElement)gridMain);
                        Globals.Calc.GetAnswer(ref szTemp, false, false);
                        break;
                    case WorksheetType.WS_Graph:
                        iCurrentWS.OnGraph();
                        break;
                    case WorksheetType.WS_Down:
                        OnDown();
                        break;
                    case WorksheetType.WS_Up:
                        OnUp();
                        break;
                }
            }
            else
            {   //tell the worksheet to update the internal values
                iCurrentWS.SetScreenValue(iListBoxIndex, ref szTemp);
            }

            UpdateList(0);


       /*     if (iListBoxIndex < 0)
                iListBoxIndex = 0;

            MyListboxItems myItem = _arrItems[iListBoxIndex];

            FormatValue(myItem.eEntry, ref szTemp);

            if (myItem.bGreyed == true)
                return;

            myItem.szValue = szTemp;*/
            
        }

        void UpdateList(int iNewIndex)
        {
            string szTemp = null;

            //lets loop through everything and format on the way into the list
            for (int i = 0; i < _arrItems.Count; i++)
            {
                iCurrentWS.GetScreenValue(i, ref szTemp);

                MyListboxItems myItem = _arrItems[i];
                FormatValue(myItem.eEntry, ref szTemp);
                if (myItem != null)
                    myItem.szValue = szTemp;
            }
        }

        public void UpdateSkin(Color crBackground, EnumCalcProSkinColor eColor, EnumCalcProSkinStyle eStyle)
        {
            //let's set a background color
            LayoutRoot.Background = Globals.Brush.GetBrush(crBackground);
        }

        bool Init(LaunchType eWS)
        {
            Globals.Calc.AddClear(ClearType.CLEAR_ClearAll);

            CalcOperationType eOldCalcOp = Globals.Calc.GetCalcMode();
            EnumCalcType eOldCalcType = Globals.btnMan.GetCalcType();

            Globals.Calc.SetCalcMode(CalcOperationType.CALC_OrderOfOpps);
            Globals.Calc.SetCalcDisplay(DisplayType.DISPLAY_Float);

            Globals.btnMan.SetCalcType(EnumCalcType.CALC_WorkSheet);

            

            switch (eWS)
            {
                case LaunchType.LAUNCH_BreakEven:
                    iCurrentWS = (IWorkSheet)new WSBreakEven();
                    break;
                case LaunchType.LAUNCH_CashFlow:
                    iCurrentWS = (IWorkSheet)new WSCashFlow();
                    break;
                case LaunchType.LAUNCH_CompoundInt:
                    iCurrentWS = (IWorkSheet)new WSCompound();
                    break;
                case LaunchType.LAUNCH_Date:
                    iCurrentWS = (IWorkSheet)new WSDate();
                    break;
                case LaunchType.LAUNCH_Depreciation:
                    iCurrentWS = (IWorkSheet)new WSDepreciation();
                    break;
                case LaunchType.LAUNCH_Lease:
                    iCurrentWS = (IWorkSheet)new WSLease();
                    break;
                case LaunchType.LAUNCH_Loan:
                    iCurrentWS = (IWorkSheet)new WSLoan();
                    break;
                case LaunchType.LAUNCH_Mort:
                    iCurrentWS = (IWorkSheet)new WSMort();
                    break;
                case LaunchType.LAUNCH_PercentChange:
                    iCurrentWS = (IWorkSheet)new WSPcntChange();
                    break;
                case LaunchType.LAUNCH_ProfitMargin:
                    iCurrentWS = (IWorkSheet)new WSProfitMargin();
                    break;
                case LaunchType.LAUNCH_TVM:
                    iCurrentWS = (IWorkSheet)new WSTVM();
                    break;
                case LaunchType.LAUNCH_Tip:
                    iCurrentWS = (IWorkSheet)new WSTip();
                    break;
                case LaunchType.LAUNCH_LinearReg:
                    iCurrentWS = (IWorkSheet)new WSStats(EnumStatistics.Stats_Linear_Reg);
                    Globals.btnMan.SetCalcType(EnumCalcType.CALC_WorkSheet);
                    AddRegressionMenus();
                    break;
                case LaunchType.LAUNCH_ExpReg:
                    iCurrentWS = (IWorkSheet)new WSStats(EnumStatistics.Stats_Exponential_Reg);
                    Globals.btnMan.SetCalcType(EnumCalcType.CALC_WorkSheet);
                    AddRegressionMenus();
                    break;
                case LaunchType.LAUNCH_LogReg:
                    iCurrentWS = (IWorkSheet)new WSStats(EnumStatistics.Stats_Ln_Reg);//natural log
                    Globals.btnMan.SetCalcType(EnumCalcType.CALC_WorkSheet);
                    AddRegressionMenus();
                    break;
                case LaunchType.LAUNCH_Power:
                    iCurrentWS = (IWorkSheet)new WSStats(EnumStatistics.Stats_Power_Reg);
                    Globals.btnMan.SetCalcType(EnumCalcType.CALC_WorkSheet);
                    AddRegressionMenus();
                    break;
                default:
                    return false;
            }

            if (_arrItems.Count() > 0)
            {
                _arrItems.Clear();
            }

            iCurrentWS.Init(ref _arrItems, ref txtTitle);

            txtTitle.Foreground = Globals.Brush.GetBrush(0xFF111111); //Globals.Brush.GetBrush(Globals.btnMan.GetMainTextColor());
            
            return true;
        }

        void AddRegressionMenus()
        {
            /*
            //add some menu entries
          //  ApplicationBar appBar = (ApplicationBar)this.ApplicationBar;
            ApplicationBarMenuItem appBarMenu;

            for (int i = 0; i < WSStats.NUM_DATASETS; i++)
            {
                appBarMenu = new ApplicationBarMenuItem();
                appBarMenu.Text = Globals.rm.GetString("IDS_MENU_Stats") + " " + (i+1).ToString(); //Globals.rm.GetString("IDS_MENU_Purchase");
                int iTemp = i;//must have its own instance of the variable ... don't use i
                appBarMenu.Click += delegate(object sender, EventArgs e)
                {
                    iCurrentWS.SetDataIndex(ref _arrItems, iTemp);
                    this.DataContext = _arrItems;
                };

                this.ApplicationBar.MenuItems.Add(appBarMenu);
            }

            //and finally ... add the graph menu button
            ApplicationBarIconButton appIcon = new ApplicationBarIconButton();
            appIcon.Text = Globals.rm.GetString("IDS_MENU_Graph");
            appIcon.IconUri = new Uri(@"/Assets/appbar_graph.png", UriKind.Relative);
            appIcon.Click += delegate(object sender, EventArgs e)
            {
                iCurrentWS.OnGraph();
            };
            this.ApplicationBar.Buttons.Add(appIcon);
            */
        }

        //for regression data sets
      /*  private void menuItem_Click(object sender, EventArgs e)
        {
            ApplicationBarIconButton appMenu = (ApplicationBarIconButton)sender;
          
        }*/



        void FormatValue(EntryType eType, ref string szValue)
        {
            if (szValue == null || szValue.Length == 0)
                szValue = "0";
            else if (szValue[0] < '0' && szValue[0] > '9' && szValue[0] != '-')
                return;

            int iDec = 0;
            int iLen = 0;

            switch (eType)
            {
                case EntryType.ENTRY_Currency:
                    if(szValue.Contains("e") == true)
                        return;
                    iDec = szValue.IndexOf(".");
                    iLen = szValue.Length;

                    if(iDec == -1)
                        szValue += ".00";
                    else if(iDec == iLen - 1)
                        szValue += "00";
                    else if(iDec == iLen - 2)
                        szValue += "0";
                    else if(iDec == iLen - 3)
                        {}
                    else
                        szValue = szValue.Remove(iDec + 3, iLen - iDec - 3);


                    //add commas
                    int iTemp = szValue.Length - 3;
                    //while (iTemp + 3 > iDec)
                    while(iTemp -3 > ((szValue[0]!='-')?0:1) )
                    {
                        szValue = szValue.Insert(iTemp - 3, ",");
                        iTemp  -= 3;
                    }
                    return;
                        
                case EntryType.ENTRY_Int:
                    if(szValue.Contains("e") == true)
                        return;
                    iDec = szValue.IndexOf(".");
                    if(iDec == -1)//its already an int
                        return;

                    else
                        szValue = szValue.Remove(iDec, szValue.Length - iDec);
                    
                    return;
                case EntryType.ENTRY_Percent:
                    if(szValue[szValue.Length - 1] == '.')
                        szValue = szValue.Substring(0, szValue.Length - 1);
                    szValue += "%";
                    return;
                
                case EntryType.ENTRY_Double:
                default:
                    //all good 
                    break;
            }
        }

        private void lbItems_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            int iListBoxIndex = lbItems.SelectedIndex;

            if (iListBoxIndex == -1)
                return;

            MyListboxItems myItem = _arrItems[iListBoxIndex];

            if (myItem.szValue == null || myItem.szValue.Length == 0)
            {
                Globals.Calc.AddString("0");
                return;
            }

            string szTemp = myItem.szValue;

            if (szTemp.IndexOf("%") != -1)
            {
                szTemp = szTemp.Substring(0, szTemp.Length - 1);//lazy remove %
            }
            Globals.Calc.AddString(szTemp);
        }

      /*  protected override void OnBackKeyPress(System.ComponentModel.CancelEventArgs e)
        {
            //unregister 
            Globals.Calc.DisplayUpdate -= UpdateDisplay;
            iCurrentWS.OnClose();
        }*/

        private void Back_Click(object sender, EventArgs e)
        {
            //unregister 
            Globals.Calc.DisplayUpdate -= UpdateDisplay;
            NavigationService.GoBack();
            iCurrentWS.OnClose();
        }


    }
}

namespace CalcPro
{
    public class MyListboxItems : INotifyPropertyChanged
    {
        public MyListboxItems()
        {
            _bGreyed = false;

        }

        public event PropertyChangedEventHandler PropertyChanged;

        public Color MyTextColor
        {
            get
            {
                return Globals.btnMan.GetMainTextColor();
            }

        }
       
        public bool bGreyed
        {
            set
            {
                if (_bGreyed != value)
                {
                    _bGreyed = value;
                    NotifyPropertyChanged("brushText");
                }
            }
            get { return _bGreyed; }
        }
        bool _bGreyed;

        public EntryType eEntry { get; set; }

        public Brush brushText
        {
            get
            {
                if (_bGreyed)
                { return Globals.Brush.GetBrush(0xFF666666); }
                else
                { return Globals.Brush.GetBrush(0xFF111111); }
            }
        }

        public string szTitle
        {
            get { return _szTitle; }
            set
            {
                if (_szTitle != value)
                {
                    _szTitle = value;
                    NotifyPropertyChanged("szTitle");
                }
            }
        }
        string _szTitle;

        public string szValue
        {
            get { return _szValue; }
            set
            {
                if (_szValue != value)
                {
                    _szValue = value;
                    NotifyPropertyChanged("szValue");
                }
            }
        }
        string _szValue;

        private void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this,
                    new PropertyChangedEventArgs(propertyName));
            }

        }
    }
}