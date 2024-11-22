using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
using System.Globalization;
using System.Reflection;
using System.Threading;


namespace CalcPro
{
    public class WSBreakEven: IWorkSheet
    {
        IssBreakEven oBreakEven = new IssBreakEven();

        ObservableCollection<MyListboxItems> arrEntries;

        public WSBreakEven()
        {

        }

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex) { return false; }

        public bool Init(ref ObservableCollection<MyListboxItems> list, ref TextBlock tBlock)
        {
            arrEntries = list;

            for(int i = 0; i < 5; i++)
            {
                MyListboxItems newItem = new MyListboxItems();
                list.Add(newItem);
            }

            tBlock.Text = Globals.rm.GetString("IDS_TITLE_BreakEvenSales");

            list[0].szTitle = Globals.rm.GetString("IDS_INFO_Profit");
            list[1].szTitle = Globals.rm.GetString("IDS_INFO_Price");
            list[2].szTitle = Globals.rm.GetString("IDS_INFO_FixedCost");
            list[3].szTitle = Globals.rm.GetString("IDS_INFO_VarCost");
            list[4].szTitle = Globals.rm.GetString("IDS_INFO_Quantity");

            list[0].eEntry = EntryType.ENTRY_Currency;
            list[1].eEntry = EntryType.ENTRY_Currency;
            list[2].eEntry = EntryType.ENTRY_Currency;
            list[3].eEntry = EntryType.ENTRY_Currency;
            list[4].eEntry = EntryType.ENTRY_Double;

            list[0].bGreyed = false;
            list[1].bGreyed = false;
            list[2].bGreyed = false;
            list[3].bGreyed = false;
            list[4].bGreyed = false;

            OnReset();

            return true;
        }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            if(iIndex < 0 || iIndex >= arrEntries.Count)
                return false;
            
            //calc whatever value is selected
            double dbTemp = 0;

            string szTemp = null;

            switch(iIndex) 
            {
            case 0:
                szTemp = arrEntries[0].szValue;
                dbTemp = oBreakEven.CalcProfit();
                break;
            case 1:
                szTemp = arrEntries[1].szValue;
                dbTemp = oBreakEven.CalcUnitPrice();
                break;
            case 2:
                szTemp = arrEntries[2].szValue;
                dbTemp = oBreakEven.CalcFixedCost();
                break;
            case 3:
                szTemp = arrEntries[3].szValue;
                dbTemp = oBreakEven.CalcVariableCostPer();
                break;
            case 4:
                szTemp = arrEntries[4].szValue;
                dbTemp = oBreakEven.CalcQuantity();
                break;
            default:
                return false;
            }
    
            string szDouble;

            szDouble = dbTemp.ToString();
            Globals.Calc.AddString(szDouble);

            arrEntries[iIndex].szValue = szDouble;

            return true;
        }

        public void OnReset()
        {
            if (arrEntries == null || arrEntries.Count == 0)
                return;

            oBreakEven.Clear();

            arrEntries[0].szValue = oBreakEven.GetProfit().ToString();
            arrEntries[1].szValue = oBreakEven.GetUnitPrice().ToString();
            arrEntries[2].szValue = oBreakEven.GetFixedCost().ToString();
            arrEntries[3].szValue = oBreakEven.GetVariableCost().ToString();
            arrEntries[4].szValue = oBreakEven.GetQuantity().ToString();

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

        public bool SetScreenValue(int iIndex, ref string szValue)
        {
            if(iIndex < 0 || iIndex >= arrEntries.Count)
                return false;

            double dbTemp = 0;
            if (szValue != null && szValue.Length > 0)
                dbTemp = Convert.ToDouble(szValue);       
            int iTemp = (int)dbTemp;           //oStr.StringToInt(arrEntries[iSwitch].szEntryValue);

            switch (iIndex)
            {
                case 0:
                    oBreakEven.SetProfit(dbTemp);
                    break;
                case 1:
                    oBreakEven.SetUnitPrice(dbTemp);
                    break;
                case 2:
                    oBreakEven.SetFixedCost(dbTemp);
                    break;
                case 3:
                    oBreakEven.SetVariableCostPer(dbTemp);
                    break;
                case 4:
                    oBreakEven.SetQuantity(dbTemp);
                    break;
                default://Ooops if here
                    break;
            }
            return true;
        }

        public bool GetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex >= arrEntries.Count)
            {
                szValue = "";
                return false;
            }

            double dbTemp = 0;

            switch (iIndex)
            {
                case 0:
                    dbTemp = oBreakEven.GetProfit();
                    szValue = dbTemp.ToString();
                    break;
                case 1:
                    dbTemp = oBreakEven.GetUnitPrice();
                    szValue = dbTemp.ToString();
                    break;
                case 2:
                    dbTemp = oBreakEven.GetFixedCost();
                    szValue = dbTemp.ToString();
                    break;
                case 3:
                    dbTemp = oBreakEven.GetVariableCost();
                    szValue = dbTemp.ToString();
                    break;
                case 4:
                    dbTemp = oBreakEven.GetQuantity();
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
