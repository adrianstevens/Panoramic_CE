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

namespace CalcPro
{
    public class WSProfitMargin : IWorkSheet
    {
        ObservableCollection<MyListboxItems> arrEntries;

        IssProfitMargin oProfit = new IssProfitMargin();

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex) { return false; }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            //calc whatever value is selected
            switch (iIndex)
            {
                case 0:
                    arrEntries[0].szValue = oProfit.CalcCost().ToString();
                    Globals.Calc.AddString(arrEntries[0].szValue);
                    break;
                case 1:
                    arrEntries[1].szValue = oProfit.CalcSellPrice().ToString();
                    Globals.Calc.AddString(arrEntries[1].szValue);
                    break;
                case 2:
                    arrEntries[2].szValue = oProfit.CalcProfit().ToString();
                    Globals.Calc.AddString(arrEntries[2].szValue);
                    break;
                default:
                    return false;
            }

            return true;
        }

        public void OnReset()
        {
            if (arrEntries == null || arrEntries.Count == 0)
                return;

            oProfit.Clear();

            arrEntries[0].szValue = oProfit.GetCost().ToString();
            arrEntries[1].szValue = oProfit.GetSellPrice().ToString();
            arrEntries[2].szValue = oProfit.GetProfitMargin().ToString();

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

            for (int i = 0; i < 3; i++)
            {
                MyListboxItems newItem = new MyListboxItems();
                list.Add(newItem);
                list[i].bGreyed = false;
            }

            arrEntries[0].szTitle = Globals.rm.GetString("IDS_INFO_Cost");
            arrEntries[1].szTitle = Globals.rm.GetString("IDS_INFO_Price");
            arrEntries[2].szTitle = Globals.rm.GetString("IDS_INFO_Profit");

            arrEntries[0].eEntry = EntryType.ENTRY_Currency;
            arrEntries[1].eEntry = EntryType.ENTRY_Currency;
            arrEntries[2].eEntry = EntryType.ENTRY_Percent;

            tBlock.Text = Globals.rm.GetString("IDS_TITLE_ProfitMargin");

            OnReset();

            return true;
        }

        public bool SetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 2)
                return false;

            double dbTemp = 0;
            if (szValue != null && szValue.Length > 0)
                dbTemp = Convert.ToDouble(szValue);       

            int iTemp = (int)dbTemp;

            switch (iIndex)
            {
                case 0:
                    oProfit.SetCost(dbTemp);
                    break;
                case 1:
                    oProfit.SetSellPrice(dbTemp);
                    break;
                case 2:
                    oProfit.SetProfitMargin(dbTemp);
                    break;

                default://Ooops if here
                    return false;
                    //MessageBox(hWnd, _T("Index out of range"), _T("Contact iSS"), MB_ICONERROR);
            }

            return true;
        }

        public bool GetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 2)
            {
                szValue = "";
                return false;
            }

            switch (iIndex)
            {
                case 0:
                    szValue = oProfit.GetCost().ToString();
                    break;
                case 1:
                    szValue = oProfit.GetSellPrice().ToString();
                    break;
                case 2:
                    szValue = oProfit.GetProfitMargin().ToString();
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
