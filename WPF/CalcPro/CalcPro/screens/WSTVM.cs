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
    public enum ScreenEntries
    {
        TVM_N,
        TVM_IY,
        TVM_PV,
        TVM_PMT,
        TVM_FV,
        TVM_PY,
        TVM_CY,
        TVM_Count,
    };

    public class WSTVM : IWorkSheet
    {
        ObservableCollection<MyListboxItems> arrEntries;

        IssTVM oTVM = new IssTVM();

        public WSTVM()
        {
            oTVM.SetBegYearPayments(Globals.Settings.bEndPeriodPayments);
        }

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex) { return false; }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            switch (iIndex)
            {
                case (int)ScreenEntries.TVM_N:
                    oTVM.CalcNumberOfPeriods();
                    arrEntries[(int)ScreenEntries.TVM_N].szValue = oTVM.GetNumberOfPeriods().ToString();
                    Globals.Calc.AddString(arrEntries[(int)ScreenEntries.TVM_N].szValue);
                    break;
                case (int)ScreenEntries.TVM_PMT:
                    oTVM.CalcPayment();
                    arrEntries[(int)ScreenEntries.TVM_PMT].szValue = oTVM.GetPayment().ToString();
                    Globals.Calc.AddString(arrEntries[(int)ScreenEntries.TVM_PMT].szValue);
                    break;
                case (int)ScreenEntries.TVM_PV:
                    oTVM.CalcPresentValue();
                    arrEntries[(int)ScreenEntries.TVM_PV].szValue = oTVM.GetPresentValue().ToString();
                    Globals.Calc.AddString(arrEntries[(int)ScreenEntries.TVM_PV].szValue);
                    break;
                case (int)ScreenEntries.TVM_FV:
                    oTVM.CalcFutureValue();
                    arrEntries[(int)ScreenEntries.TVM_FV].szValue = oTVM.GetFutureValue().ToString();
                    Globals.Calc.AddString(arrEntries[(int)ScreenEntries.TVM_FV].szValue);
                    break;
                case (int)ScreenEntries.TVM_IY:
                    oTVM.CalcInterestRateYear();
                    arrEntries[(int)ScreenEntries.TVM_IY].szValue = oTVM.GetInterestRateYear().ToString();
                    Globals.Calc.AddString(arrEntries[(int)ScreenEntries.TVM_IY].szValue);
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

            oTVM.Reset();

            arrEntries[(int)ScreenEntries.TVM_N].szValue = oTVM.GetNumberOfPeriods().ToString();
            arrEntries[(int)ScreenEntries.TVM_IY].szValue = oTVM.GetInterestRateYear().ToString();
            arrEntries[(int)ScreenEntries.TVM_PV].szValue = oTVM.GetPresentValue().ToString();
            arrEntries[(int)ScreenEntries.TVM_PMT].szValue = oTVM.GetPayment().ToString();
            arrEntries[(int)ScreenEntries.TVM_FV].szValue = oTVM.GetFutureValue().ToString();
            arrEntries[(int)ScreenEntries.TVM_CY].szValue = oTVM.GetCompPerYear().ToString();
            arrEntries[(int)ScreenEntries.TVM_PY].szValue = oTVM.GetPaymentsPerYear().ToString();

            Globals.Calc.AddString(arrEntries[(int)ScreenEntries.TVM_N].szValue);
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

            for(int i = 0; i < (int)ScreenEntries.TVM_Count; i++)
            {
                MyListboxItems newItem = new MyListboxItems();
                list.Add(newItem);
            }

            tBlock.Text = Globals.rm.GetString("IDS_TITLE_TVM");

            list[0].szTitle = Globals.rm.GetString("IDS_INFO_NumTerms");
            list[1].szTitle = Globals.rm.GetString("IDS_INFO_InterestYear");
            list[2].szTitle = Globals.rm.GetString("IDS_INFO_PresentValue");
            list[3].szTitle = Globals.rm.GetString("IDS_INFO_Payment");
            list[4].szTitle = Globals.rm.GetString("IDS_INFO_FutureValue");
            list[5].szTitle = Globals.rm.GetString("IDS_INFO_PaymentsYear");
            list[6].szTitle = Globals.rm.GetString("IDS_INFO_CompYear");

            list[0].eEntry = EntryType.ENTRY_Int;
            list[1].eEntry = EntryType.ENTRY_Percent;
            list[2].eEntry = EntryType.ENTRY_Currency;
            list[3].eEntry = EntryType.ENTRY_Currency;
            list[4].eEntry = EntryType.ENTRY_Currency;
            list[5].eEntry = EntryType.ENTRY_Int;
            list[6].eEntry = EntryType.ENTRY_Int;

            list[0].bGreyed = false;
            list[1].bGreyed = false;
            list[2].bGreyed = false;
            list[3].bGreyed = false;
            list[4].bGreyed = false;
            list[5].bGreyed = false;
            list[6].bGreyed = false;

            OnReset();

            return true;
        }

        public bool SetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 6)
                return false;

            double dbTemp = 0;
            
            if(szValue != null && szValue.Length > 0)
                dbTemp = Convert.ToDouble(szValue);
            int iTemp = (int)dbTemp;

            switch (iIndex)
            {
                case (int)ScreenEntries.TVM_N:
                    oTVM.SetNumberOfPeriods(iTemp);
                    break;
                case (int)ScreenEntries.TVM_IY:
                    oTVM.SetInterestRateYear(dbTemp);
                    break;
                case (int)ScreenEntries.TVM_PV:
                    oTVM.SetPresentValue(dbTemp);
                    break;
                case (int)ScreenEntries.TVM_PMT:
                    oTVM.SetPayment(dbTemp);
                    break;
                case (int)ScreenEntries.TVM_FV:
                    oTVM.SetFutureValue(dbTemp);
                    break;
                case (int)ScreenEntries.TVM_PY:
                    oTVM.SetPaymentsPerYear(iTemp);
                    break;
                case (int)ScreenEntries.TVM_CY:
                    oTVM.SetCompPMTPerYear(iTemp);
                    break;
                default://Ooops if here
                    return false;
            }

            return true;
        }

        public bool GetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 6)
            {
                szValue = "";
                return false;
            }

            switch (iIndex)
            {
                case (int)ScreenEntries.TVM_N:
                    szValue = oTVM.GetNumberOfPeriods().ToString();
                    break;
                case (int)ScreenEntries.TVM_IY:
                    szValue = oTVM.GetInterestRateYear().ToString();
                    break;
                case (int)ScreenEntries.TVM_PV:
                    szValue = oTVM.GetPresentValue().ToString();
                    break;
                case (int)ScreenEntries.TVM_PMT:
                    szValue = oTVM.GetPayment().ToString();
                    break;
                case (int)ScreenEntries.TVM_FV:
                    szValue = oTVM.GetFutureValue().ToString();
                    break;
                case (int)ScreenEntries.TVM_PY:
                    szValue = oTVM.GetPaymentsPerYear().ToString();
                    break;
                case (int)ScreenEntries.TVM_CY:
                    szValue = oTVM.GetCompPerYear().ToString();
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
