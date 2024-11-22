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
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

using System.Resources;
using System.Globalization;
using System.Reflection;
using System.Threading;

namespace CalcPro.Screens
{
    public class WSTip: IWorkSheet
    {
        ObservableCollection<MyListboxItems> arrEntries;
        IssTip oTip = new IssTip();

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex) { return false; }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            switch (iIndex)
            {
                default://move the screen to the bottom
                 case 5:
                    oTip.CalculateResults();
                    arrEntries[3].szValue = oTip.GetTipAmount().ToString();
                    arrEntries[4].szValue = oTip.GetTotalPerPerson().ToString();
                    arrEntries[5].szValue = oTip.GetTotal().ToString();
                    Globals.Calc.AddString(arrEntries[5].szValue);
                    break;
                case 3:
                    //calc the results
                    oTip.CalculateResults();
                    arrEntries[3].szValue = oTip.GetTipAmount().ToString();
                    arrEntries[4].szValue = oTip.GetTotalPerPerson().ToString();
                    arrEntries[5].szValue = oTip.GetTotal().ToString();
                    Globals.Calc.AddString(arrEntries[3].szValue);
                    break;
                case 4:
                    oTip.CalculateResults();
                    arrEntries[3].szValue = oTip.GetTipAmount().ToString();
                    arrEntries[4].szValue = oTip.GetTotalPerPerson().ToString();
                    arrEntries[5].szValue = oTip.GetTotal().ToString();
                    Globals.Calc.AddString(arrEntries[4].szValue);
                    break;
            }
            return true;
        }

        public void OnReset()
        {
            if (arrEntries == null || arrEntries.Count == 0)
                return;

            oTip.Reset();

            arrEntries[0].szValue = oTip.GetBillAmount().ToString();
            arrEntries[1].szValue = oTip.GetPercent().ToString();
            arrEntries[2].szValue = oTip.GetNumberOfPeople().ToString();
            arrEntries[3].szValue = oTip.GetTipAmount().ToString();
            arrEntries[4].szValue = oTip.GetTotalPerPerson().ToString();
            arrEntries[5].szValue = oTip.GetTotal().ToString();

            Globals.Calc.AddString(arrEntries[0].szValue);
        }


        public void OnGraph()
        {
        }

        public void OnLoad()
        {
        }

        public void OnSave()
        {
        }

        public bool Init(ref ObservableCollection<MyListboxItems> list, ref System.Windows.Controls.TextBlock tBlock)
        {
            arrEntries = list;

            for (int i = 0; i < 6; i++)
            {
                MyListboxItems newItem = new MyListboxItems();
                list.Add(newItem);
                list[i].bGreyed = false;
            }

            arrEntries[0].szTitle = Globals.rm.GetString("IDS_INFO_BillAmount");
            arrEntries[1].szTitle = Globals.rm.GetString("IDS_INFO_TipPercent");
            arrEntries[2].szTitle = Globals.rm.GetString("IDS_INFO_NumPeople");
            arrEntries[3].szTitle = Globals.rm.GetString("IDS_INFO_TipAmount");
            arrEntries[4].szTitle = Globals.rm.GetString("IDS_INFO_TotalPer");
            arrEntries[5].szTitle = Globals.rm.GetString("IDS_INFO_BillTotal");

            arrEntries[0].eEntry = EntryType.ENTRY_Currency;
            arrEntries[1].eEntry = EntryType.ENTRY_Percent;
            arrEntries[2].eEntry = EntryType.ENTRY_Int;
            arrEntries[3].eEntry = EntryType.ENTRY_Currency;
            arrEntries[4].eEntry = EntryType.ENTRY_Currency;
            arrEntries[5].eEntry = EntryType.ENTRY_Currency;

            arrEntries[3].bGreyed = true;
            arrEntries[4].bGreyed = true;
            arrEntries[5].bGreyed = true;

            tBlock.Text = Globals.rm.GetString("IDS_TITLE_TipCalculator");

            OnReset();

            return true;
        }

        public bool SetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 5)
                return false;

            double dbTemp = 0;
            if (szValue != null && szValue.Length > 0)
                dbTemp = Convert.ToDouble(szValue);       

            int iTemp = (int)dbTemp;

            switch (iIndex)
            {
                case 0:
                    oTip.SetAmount(szValue);
                    break;
                case 1:
                    oTip.SetPercentage(dbTemp);
                    break;
                case 2:
                    oTip.SetNumberofPeople(iTemp - 1);
                    break;
                case 3:
                case 4:
                case 5:
                    break;
                default://Ooops if here
                    return false;
                    //MessageBox(hWnd, _T("Index out of range"), _T("Contact iSS"), MB_ICONERROR);
            }
            return true;
        }

        public bool GetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 5)
            {
                szValue = "";
                return false;
            }

            double dbTemp = 0;
            int iTemp = 0;

            switch (iIndex)
            {
                case 0:
                    dbTemp = oTip.GetBillAmount();
                    szValue = dbTemp.ToString();
                    break;
                case 1:
                    dbTemp = oTip.GetPercent();
                    szValue = dbTemp.ToString();
                    break;
                case 2:
                    iTemp = oTip.GetNumberOfPeople();
                    szValue = iTemp.ToString();
                    break;
                case 3:
                    dbTemp = oTip.GetTipAmount();
                    szValue = dbTemp.ToString();
                    break;
                case 4:
                    dbTemp = oTip.GetTotalPerPerson();
                    szValue = dbTemp.ToString();
                    break;
                case 5:
                    dbTemp = oTip.GetTotal();
                    szValue = dbTemp.ToString();
                    break;
                default://Ooops if here
                    return false;
            }
            return true;
        }

        public void OnUp(int iCurrentIndex)
        {
        }

        public void OnDown(int iCurrentIndex)
        {
        }

        public void OnClose()
        {
        }
    }
}
