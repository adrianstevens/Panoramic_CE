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
    public class WSDate : IWorkSheet
    {
        ObservableCollection<MyListboxItems> arrEntries;

        IssFDateCalc oDateCalc = new IssFDateCalc();

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex) { return false; }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            arrEntries[6].szValue = oDateCalc.CalcNormal().ToString();
            arrEntries[7].szValue = oDateCalc.Calc30360().ToString();
            
            if(iIndex == 7)
                Globals.Calc.AddString(arrEntries[7].szValue);
            else
                Globals.Calc.AddString(arrEntries[6].szValue);

            return true;
        }

        public void OnReset()
        {
            if (arrEntries == null || arrEntries.Count == 0)
                return;

            oDateCalc.Clear();

            arrEntries[0].szValue = oDateCalc.GetStartYear().ToString();
            arrEntries[1].szValue = oDateCalc.GetStartMonth().ToString();
            arrEntries[2].szValue = oDateCalc.GetStartDay().ToString();
            arrEntries[3].szValue = oDateCalc.GetEndYear().ToString();
            arrEntries[4].szValue = oDateCalc.GetEndMonth().ToString();
            arrEntries[5].szValue = oDateCalc.GetEndDay().ToString();
            arrEntries[6].szValue = "0";
            arrEntries[7].szValue = "0";

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

            for (int i = 0; i < 8; i++)
            {
                MyListboxItems newItem = new MyListboxItems();
                list.Add(newItem);
                list[i].bGreyed = false;
                list[i].eEntry = EntryType.ENTRY_Int;
            }

            tBlock.Text = Globals.rm.GetString("IDS_TITLE_DateCalculator");


            list[0].szTitle = Globals.rm.GetString("IDS_INFO_StartYear");
            list[1].szTitle = Globals.rm.GetString("IDS_INFO_StartMonth");
            list[2].szTitle = Globals.rm.GetString("IDS_INFO_StartDay");
            list[3].szTitle = Globals.rm.GetString("IDS_INFO_EndYear");
            list[4].szTitle = Globals.rm.GetString("IDS_INFO_EndMonth");
            list[5].szTitle = Globals.rm.GetString("IDS_INFO_EndDay");
            list[6].szTitle = Globals.rm.GetString("IDS_INFO_Difference");
            list[7].szTitle = Globals.rm.GetString("IDS_INFO_Diff360");

            list[6].bGreyed = true;
            list[7].bGreyed = true;

            OnReset();

            return true;
        }

        public bool SetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 7)
            {
                return false;
            }

            double dbTemp = 0;
            if (szValue != null && szValue.Length > 0)
                dbTemp = Convert.ToDouble(szValue);       
            int iTemp = (int)dbTemp;

            switch (iIndex)
            {
                case 0:
                    oDateCalc.SetStartYear(iTemp);
                    break;
                case 1:
                    oDateCalc.SetStartMonth(iTemp);
                    break;
                case 2:
                    oDateCalc.SetStartDay(iTemp);
                    break;
                case 3:
                    oDateCalc.SetEndYear(iTemp);
                    break;
                case 4:
                    oDateCalc.SetEndMonth(iTemp);
                    break;
                case 6:
                case 7:
                    break;
                case 5:
                    oDateCalc.SetEndDay(iTemp);
                    break;
                default://Ooops if here
                    //MessageBox(hWnd, _T("Index out of range"), _T("Error"), MB_ICONERROR);
                    break;
            }

            return false;
        }


        public bool GetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 7)
            {
                szValue = "";
                return false;
            }

            int iTemp = 0;

            switch (iIndex)
            {
                case 0:
                    iTemp = oDateCalc.GetStartYear();
                    break;
                case 1:
                    iTemp = oDateCalc.GetStartMonth();
                    break;
                case 2:
                    iTemp = oDateCalc.GetStartDay();
                    break;
                case 3:
                    iTemp = oDateCalc.GetEndYear();
                    break;
                case 4:
                    iTemp = oDateCalc.GetEndMonth();
                    break;
                case 6:
                    iTemp = oDateCalc.GetDays30360();
                    break;
                case 7:
                    iTemp = oDateCalc.GetDaysNormal();
                    break;
                case 5:
                    iTemp = oDateCalc.GetEndDay();
                    break;
                default://Ooops if here
                    return false;
                    
            }

            szValue = iTemp.ToString();

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
