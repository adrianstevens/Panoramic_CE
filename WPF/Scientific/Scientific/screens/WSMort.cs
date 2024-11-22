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

enum TOTAL_VALUE_TYPE
{
    GET_GRAND_TOTAL,
    GET_TOTAL_INTEREST,
    GET_TOTAL_TAXES,
    GET_TOTAL_OTHER_RECURRING,
    GET_TOTAL_MORT_INSURANCE,
    GET_TOTAL_PAYMENTS,
};

enum PAYMENT_SCHEDULE
{
    PAYMENT_MONTHLY = 0,
    PAYMENT_MONTHLY_ACCELERATED,
    PAYMENT_BIWEEKLY,
    PAYMENT_BIWEEKLY_ACCELERATED,
    PAYMENT_WEEKLY,
    PAYMENT_WEEKLY_ACCELERATED,
    PAYMENT_SEMI_MONTHLY,
    PAYMENT_SEMI_MONTHLY_ACCELERATED,
    PAYMENT_BIMONTHLY,
    PAYMENT_BIMONTHLY_ACCELERATED,
};

namespace CalcPro
{
    public class WSMort : IWorkSheet
    {
        ObservableCollection<MyListboxItems> arrEntries;

        IssMortgage oMort = new IssMortgage();

        public WSMort()
        {
            oMort.SetPaymentSchedule(Globals.Settings.ePaymentSched);
        }

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex) { return false; }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            CalcMortValues();
            switch (iIndex)
            {
                default:
                case 8:
                    Globals.Calc.AddString(arrEntries[8].szValue);
                    break;
                case 7:
                    Globals.Calc.AddString(arrEntries[7].szValue);
                    break;
                case 6:
                    Globals.Calc.AddString(arrEntries[6].szValue);
                    break;
                case 5:
                    Globals.Calc.AddString(arrEntries[5].szValue);
                    break;
            }


            //lets show the amort table ...


       //     PopupMenu pop = new PopupMenu(fwElem, Globals.rm.GetString("IDS_TITLE_Mortgage"));
            
            return true;
        }

        public void OnReset()
        {
            if (arrEntries == null || arrEntries.Count == 0)
                return;

            oMort.ResetValues(true);

            arrEntries[0].szValue = oMort.GetPrinciple().ToString();
            arrEntries[1].szValue = oMort.GetTermInYears().ToString();
            arrEntries[2].szValue = oMort.GetInterest().ToString();
            arrEntries[3].szValue = oMort.GetDownPayment().ToString();
            arrEntries[4].szValue = oMort.GetOtherRecurringCosts().ToString();

            CalcMortValues();

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

            for (int i = 0; i < 9; i++)
            {
                MyListboxItems newItem = new MyListboxItems();
                list.Add(newItem);
            }

            //properly set the title
            switch (Globals.Settings.ePaymentSched)
            {
                default:
                case PAYMENT_SCHEDULE.PAYMENT_MONTHLY:
                    tBlock.Text = Globals.rm.GetString("IDS_OPT_Monthly");
                    break;
                case PAYMENT_SCHEDULE.PAYMENT_MONTHLY_ACCELERATED:
                    tBlock.Text = Globals.rm.GetString("IDS_OPT_MonthlyAccel");
                    break;
                case PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY:
                    tBlock.Text = Globals.rm.GetString("IDS_OPT_BiWeekly");
                    break;
                case PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY_ACCELERATED:
                    tBlock.Text = Globals.rm.GetString("IDS_OPT_BiWeeklyAccel");
                    break;
                case PAYMENT_SCHEDULE.PAYMENT_WEEKLY:
                    tBlock.Text = Globals.rm.GetString("IDS_OPT_Weekly");
                    break;
                case PAYMENT_SCHEDULE.PAYMENT_WEEKLY_ACCELERATED:
                    tBlock.Text = Globals.rm.GetString("IDS_OPT_WeeklyAccel");
                    break;
                case PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY:
                    tBlock.Text = Globals.rm.GetString("IDS_OPT_SemiMonthly");
                    break;
                case PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY_ACCELERATED:
                    tBlock.Text = Globals.rm.GetString("IDS_OPT_SemiMonthlyAccel");
                    break;
                case PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY:
                    tBlock.Text = Globals.rm.GetString("IDS_OPT_BiMonthly");
                    break;
                case PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY_ACCELERATED:
                    tBlock.Text = Globals.rm.GetString("IDS_OPT_BiMonthlyAccel");
                    break;





            }

            tBlock.Text = Globals.rm.GetString("IDS_TITLE_Mortgage") + tBlock.Text;

            arrEntries[0].szTitle = Globals.rm.GetString("IDS_INFO_Principal");
            arrEntries[1].szTitle = Globals.rm.GetString("IDS_INFO_Terms");
            arrEntries[2].szTitle = Globals.rm.GetString("IDS_INFO_InterestRate");
            arrEntries[3].szTitle = Globals.rm.GetString("IDS_INFO_DownPayment");
            arrEntries[4].szTitle = Globals.rm.GetString("IDS_INFO_MonthlyFees");

            arrEntries[5].szTitle = Globals.rm.GetString("IDS_INFO_TotalPayments");
            arrEntries[6].szTitle = Globals.rm.GetString("IDS_INFO_TotalInterest");
            arrEntries[7].szTitle = Globals.rm.GetString("IDS_INFO_MonthlyPayment");
            arrEntries[8].szTitle = Globals.rm.GetString("IDS_INFO_TotalRepaid");

            arrEntries[0].eEntry = EntryType.ENTRY_Currency;
            arrEntries[1].eEntry = EntryType.ENTRY_Int;
            arrEntries[2].eEntry = EntryType.ENTRY_Percent;
            arrEntries[3].eEntry = EntryType.ENTRY_Currency;
            arrEntries[4].eEntry = EntryType.ENTRY_Currency;
            arrEntries[5].eEntry = EntryType.ENTRY_Int;
            arrEntries[6].eEntry = EntryType.ENTRY_Currency;
            arrEntries[7].eEntry = EntryType.ENTRY_Currency;
            arrEntries[8].eEntry = EntryType.ENTRY_Currency;

            arrEntries[0].bGreyed = false;
            arrEntries[1].bGreyed = false;
            arrEntries[2].bGreyed = false;
            arrEntries[3].bGreyed = false;
            arrEntries[4].bGreyed = false;
            arrEntries[5].bGreyed = true;
            arrEntries[6].bGreyed = true;
            arrEntries[7].bGreyed = true;
            arrEntries[8].bGreyed = true;

            OnReset();

            return true;
        }

        public bool SetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 8)
                return false;

            double dbTemp = 0;
            if (szValue != null && szValue.Length > 0)
                dbTemp = Convert.ToDouble(szValue);       
            int iTemp = (int)dbTemp;

            switch (iIndex)
            {
                case 0:
                    oMort.SetPrinciple(dbTemp);
                    CalcMortValues();
                    break;
                case 1:
                    oMort.SetTermInYears(iTemp);
                    CalcMortValues();
                    break;
                case 2:
                    oMort.SetInterest(dbTemp);
                    CalcMortValues();
                    break;
                case 3:
                    oMort.SetDownPayment(dbTemp);
                    CalcMortValues();
                    break;
                case 4:
                    oMort.SetOtherRecurringCosts(dbTemp);
                    CalcMortValues();
                    break;
                case 5:
                case 6:
                case 7:
                case 8:
                    break;
                default:
                    break;
            }

            return true;
        }

        public bool GetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > 8)
            {
                szValue = "";
                return false;
            }

            double dbTemp = 0;
            int iTemp = 0;

            switch (iIndex)
            {
                case 0:
                    dbTemp = oMort.GetPrinciple();
                    szValue = dbTemp.ToString();
                    break;
                case 1:
                    iTemp = oMort.GetTermInYears();
                    szValue = iTemp.ToString();
                    break;
                case 2:
                    dbTemp = oMort.GetInterest();
                    szValue = dbTemp.ToString();
                    break;
                case 3:
                    dbTemp = oMort.GetDownPayment();
                    szValue = dbTemp.ToString();
                    break;
                case 4:
                    dbTemp = oMort.GetOtherRecurringCosts();
                    szValue = dbTemp.ToString();
                    break;
                case 5:
                    dbTemp = oMort.GetNumberOfPayments();
                    szValue = dbTemp.ToString();
                    break;
                case 6:
                    dbTemp = oMort.GetTotal(TOTAL_VALUE_TYPE.GET_TOTAL_INTEREST);
                    szValue = dbTemp.ToString();
                    break;
                case 7:
                    dbTemp = oMort.GetMonthlyPaymentAmt();
                    szValue = dbTemp.ToString();
                    break;
                case 8:
                    dbTemp = oMort.GetTotal(TOTAL_VALUE_TYPE.GET_GRAND_TOTAL);
                    szValue = dbTemp.ToString();
                    break;
                default:
                    break;
            }
            return true;
        }

        void CalcMortValues()
        {
            //oMort.SetPaymentSchedule(g_ePaymentSched);
            bool bRet = oMort.FillTables();
            
            arrEntries[5].szValue = oMort.GetNumberOfPayments().ToString();
            arrEntries[6].szValue = oMort.GetTotal(TOTAL_VALUE_TYPE.GET_TOTAL_INTEREST).ToString();
            arrEntries[7].szValue = oMort.GetMonthlyPaymentAmt().ToString();
            arrEntries[8].szValue = oMort.GetTotal(TOTAL_VALUE_TYPE.GET_GRAND_TOTAL).ToString();
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
