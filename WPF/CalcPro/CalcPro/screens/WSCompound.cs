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
    public class WSCompound : IWorkSheet
    {
        IssPcntCompound oPcnt = new IssPcntCompound();
        ObservableCollection<MyListboxItems> arrEntries;

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex) { return false; }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            switch((EnumPcntScreenEntries)iIndex) 
            {
                case EnumPcntScreenEntries.PCNT_N:
                    arrEntries[(int)EnumPcntScreenEntries.PCNT_N].szValue = oPcnt.CalcNumPeriods().ToString();
                    Globals.Calc.AddString(arrEntries[(int)EnumPcntScreenEntries.PCNT_N].szValue);
                    break;
                case EnumPcntScreenEntries.PCNT_IY:
                    arrEntries[(int)EnumPcntScreenEntries.PCNT_IY].szValue = oPcnt.CalcInterestRate().ToString();
                    Globals.Calc.AddString(arrEntries[(int)EnumPcntScreenEntries.PCNT_IY].szValue);
                    break;
                case EnumPcntScreenEntries.PCNT_PV:
                    arrEntries[(int)EnumPcntScreenEntries.PCNT_PV].szValue = oPcnt.CalcOriginalValue().ToString();
                    Globals.Calc.AddString(arrEntries[(int)EnumPcntScreenEntries.PCNT_PV].szValue);
                    break;
            default:
            case EnumPcntScreenEntries.PCNT_FV:
                    arrEntries[iIndex].szValue = oPcnt.CalcNewValue().ToString();
                    Globals.Calc.AddString(arrEntries[iIndex].szValue);
                    break;
            }

            return true;
        }

        public void OnReset()
        {
            if (arrEntries == null || arrEntries.Count == 0)
                return;

            oPcnt.Clear();
            
            arrEntries[0].szValue = oPcnt.GetNumPeriods().ToString();
            arrEntries[1].szValue = oPcnt.GetInterestRate().ToString();
            arrEntries[2].szValue = oPcnt.GetOriginalValue().ToString();
            arrEntries[3].szValue = oPcnt.GetNewValue().ToString();

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

            for (int i = 0; i < 4; i++)
            {
                MyListboxItems newItem = new MyListboxItems();
                list.Add(newItem);
            }

            tBlock.Text = Globals.rm.GetString("IDS_TITLE_CompoundInterest");

            list[0].szTitle = Globals.rm.GetString("IDS_INFO_Periods");
            list[1].szTitle = Globals.rm.GetString("IDS_INFO_InterestRate");
            list[2].szTitle = Globals.rm.GetString("IDS_INFO_StartValue");
            list[3].szTitle = Globals.rm.GetString("IDS_INFO_FinalValue");


            list[0].eEntry = EntryType.ENTRY_Int;
            list[1].eEntry = EntryType.ENTRY_Percent;
            list[2].eEntry = EntryType.ENTRY_Currency;
            list[3].eEntry = EntryType.ENTRY_Currency;

            list[0].bGreyed = false;
            list[1].bGreyed = false;
            list[2].bGreyed = false;
            list[3].bGreyed = false;

            OnReset();

            return true;
        }

        public bool SetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > (int)EnumPcntScreenEntries.PCNT_FV)
            {
                return false;
            }

            double dbTemp = 0;
            if (szValue != null && szValue.Length > 0)
                dbTemp = Convert.ToDouble(szValue);       
            int iTemp = (int)dbTemp;

            switch ((EnumPcntScreenEntries)iIndex)
            {
                case EnumPcntScreenEntries.PCNT_N:
                    oPcnt.SetNumPeriods(iTemp);
                    break;
                case EnumPcntScreenEntries.PCNT_IY:
                    oPcnt.SetInterestRate(dbTemp);
                    break;
                case EnumPcntScreenEntries.PCNT_PV:
                    oPcnt.SetOriginalValue(dbTemp);
                    break;
                case EnumPcntScreenEntries.PCNT_FV:
                    oPcnt.SetNewValue(dbTemp);
                    break;
                default://Ooops if here
                    return false;
                    
            }

            return true;
        }


        public bool GetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0 || iIndex > (int)EnumPcntScreenEntries.PCNT_FV)
            {
                szValue = "";
                return false;
            }

            double dbTemp = 0;
            int iTemp = 0;

            switch ((EnumPcntScreenEntries)iIndex)
            {
                case EnumPcntScreenEntries.PCNT_N:
                    iTemp = oPcnt.GetNumPeriods();
                    szValue = iTemp.ToString();
                    break;
                case EnumPcntScreenEntries.PCNT_IY:
                    dbTemp = oPcnt.GetInterestRate();
                    szValue = dbTemp.ToString();
                    break;
                case EnumPcntScreenEntries.PCNT_PV:
                    dbTemp = oPcnt.GetOriginalValue();
                    szValue = dbTemp.ToString();
                    break;
                case EnumPcntScreenEntries.PCNT_FV:
                    dbTemp = oPcnt.GetNewValue();
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
