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
    public class WSCashFlow : IWorkSheet
    {
        IssCashFlow oCashFlow = new IssCashFlow();
        ObservableCollection<MyListboxItems> arrEntries;

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex) { return false; }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            return false;
        }

        public void OnReset()
        {
            if (arrEntries == null || arrEntries.Count == 0)
                return;
            
          //  oCashFlow.Clear();
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

            for (int i = 0; i < 1; i++)
            {
                MyListboxItems newItem = new MyListboxItems();
                list.Add(newItem);
            }

            tBlock.Text = Globals.rm.GetString("IDS_TITLE_BreakEvenSales");

            list[0].szTitle = Globals.rm.GetString("IDS_INFO_Profit");

            list[0].eEntry = EntryType.ENTRY_Currency;
    
            list[0].bGreyed = false;
     
            OnReset();

            return true;
        }

        public bool SetScreenValue(int iIndex, ref string szValue)
        {
            double dbTemp = 0;
            if (szValue != null && szValue.Length > 0)
                dbTemp = Convert.ToDouble(szValue);
            int iTemp = (int)dbTemp;

            return false;
        }

        public bool GetScreenValue(int iIndex, ref string szValue)
        {
            szValue = "";
            return false;
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
