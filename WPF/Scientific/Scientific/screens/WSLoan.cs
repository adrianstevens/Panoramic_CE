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
    public class WSLoan : IWorkSheet
    {
        ObservableCollection<MyListboxItems> arrEntries;

        IssLoan oLoan = new IssLoan();

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex) { return false; }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            string szTemp = " ";
            oLoan.CalculateValues();
            arrEntries[7].szValue = oLoan.GetMonthlyPayments(ref szTemp).ToString();
            arrEntries[8].szValue = oLoan.GetTotalInterest(ref szTemp).ToString();
            arrEntries[9].szValue = oLoan.GetTotalCost(ref szTemp).ToString();

            return true;
        }

        public void OnReset()
        {
            if (arrEntries == null || arrEntries.Count == 0)
                return;

            oLoan.ResetValues();

            string szTemp = null;

            oLoan.GetCarPrice(ref szTemp);
            arrEntries[0].szValue = szTemp;
            oLoan.GetSalesTaxPcnt(ref szTemp);
            arrEntries[1].szValue = szTemp;
            oLoan.GetOtherFees(ref szTemp);
            arrEntries[2].szValue = szTemp;
            oLoan.GetDownPayment(ref szTemp);
            arrEntries[3].szValue = szTemp;
            oLoan.GetTradeIn(ref szTemp);
            arrEntries[4].szValue = szTemp;
            oLoan.GetNumberOfMonths(ref szTemp);
            arrEntries[5].szValue = szTemp;
            oLoan.GetInterestRatePcnt(ref szTemp);
            arrEntries[6].szValue = szTemp; 

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

            for (int i = 0; i < 10; i++)
            {
                MyListboxItems newItem = new MyListboxItems();
                list.Add(newItem);
            }

            tBlock.Text = Globals.rm.GetString("IDS_TITLE_LoanCalculator");


            list[0].szTitle = Globals.rm.GetString("IDS_INFO_Price");
            list[1].szTitle = Globals.rm.GetString("IDS_INFO_SalesTax");
            list[2].szTitle = Globals.rm.GetString("IDS_INFO_OtherFees");
            list[3].szTitle = Globals.rm.GetString("IDS_INFO_DownPayment");
            list[4].szTitle = Globals.rm.GetString("IDS_INFO_TradeIn");
            list[5].szTitle = Globals.rm.GetString("IDS_INFO_NumberOfMonths");
            list[6].szTitle = Globals.rm.GetString("IDS_INFO_InterestRate");

            list[7].szTitle = Globals.rm.GetString("IDS_INFO_MonthlyPayment");
            list[8].szTitle = Globals.rm.GetString("IDS_INFO_TotalInterest");
            list[9].szTitle = Globals.rm.GetString("IDS_INFO_TotalRepaid");

            list[0].eEntry = EntryType.ENTRY_Currency;
            list[1].eEntry = EntryType.ENTRY_Percent;
            list[2].eEntry = EntryType.ENTRY_Currency;
            list[3].eEntry = EntryType.ENTRY_Currency;
            list[4].eEntry = EntryType.ENTRY_Currency;
            list[5].eEntry = EntryType.ENTRY_Int;
            list[6].eEntry = EntryType.ENTRY_Percent;

            list[7].eEntry = EntryType.ENTRY_Currency;
            list[8].eEntry = EntryType.ENTRY_Currency;
            list[9].eEntry = EntryType.ENTRY_Currency;

            list[0].bGreyed = false;
            list[1].bGreyed = false;
            list[2].bGreyed = false;
            list[3].bGreyed = false;
            list[4].bGreyed = false;
            list[5].bGreyed = false;
            list[6].bGreyed = false;
            list[7].bGreyed = true;
            list[8].bGreyed = true;
            list[9].bGreyed = true;

            OnReset();

            return true;
        }

        public bool SetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 9)
            {
                return false;
            }

            double dbTemp2 = 0;
            if (szValue != null && szValue.Length > 0)
                dbTemp2 = Convert.ToDouble(szValue);      
            int iTemp2 = (int)dbTemp2;

            switch (iIndex)
            {
                case 0:
                    oLoan.SetCarPrice(dbTemp2);
                    break;
                case 1:
                    oLoan.SetSalesTaxPcnt(dbTemp2);
                    break;
                case 2:
                    oLoan.SetOtherFees(dbTemp2);
                    break;
                case 3:
                    oLoan.SetDownPayment(dbTemp2);
                    break;
                case 4:
                    oLoan.SetTradeIn(dbTemp2);
                    break;
                case 5:
                    oLoan.SetNumberOfMonths(iTemp2);
                    break;
                case 6:
                    oLoan.SetInterestRatePcnt(dbTemp2);
                    break;
                case 7:
                case 8:
                case 9:
                    break;
                default://Ooops if here
                    //MessageBox(hWnd, _T("Index out of range"), _T("Error"), MB_ICONERROR);
                    return false;
            }

            return true;
        }

        public bool GetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 9)
            {
                szValue = "";
                return false;
            }

            double dbTemp = 0;
            int iTemp = 0;

            switch (iIndex)
            {
                case 0:
                    dbTemp = oLoan.GetCarPrice(ref szValue);
                    break;
                case 1:
                    dbTemp = oLoan.GetSalesTaxPcnt(ref szValue);
                    break;
                case 2:
                    dbTemp = oLoan.GetOtherFees(ref szValue);
                    break;
                case 3:
                    dbTemp = oLoan.GetDownPayment(ref szValue);
                    break;
                case 4:
                    dbTemp = oLoan.GetTradeIn(ref szValue);
                    break;
                case 5:
                    iTemp = oLoan.GetNumberOfMonths(ref szValue);
                    szValue = iTemp.ToString();
                    return true;//its the only int ... sorta bad
                case 6:
                    dbTemp = oLoan.GetInterestRatePcnt(ref szValue);
                    break;
                case 7://GetMonthlyPayments
                    dbTemp = oLoan.GetMonthlyPayments(ref szValue);
                    break;
                case 8://GetTotalInterest
                    dbTemp = oLoan.GetTotalInterest(ref szValue);
                    break;
                case 9://GetTotalCost
                    dbTemp = oLoan.GetTotalCost(ref szValue);
                    break;
                default:
                    return false;
            }
            szValue = dbTemp.ToString();

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
