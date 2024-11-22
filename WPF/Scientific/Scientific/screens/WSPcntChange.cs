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
    public class WSPcntChange : IWorkSheet
    {
        ObservableCollection<MyListboxItems> arrEntries;
        IssPcntChange oPcnt = new IssPcntChange();

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex) { return false; }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            switch (iIndex)
            {
                case 1:
                    arrEntries[1].szValue = oPcnt.CalcNominalRate().ToString();
                    Globals.Calc.AddString(arrEntries[1].szValue);
                    break;
                case 2:
                    arrEntries[2].szValue = oPcnt.CalcEffectiveRate().ToString();
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

            oPcnt.Clear();

            arrEntries[0].szValue = oPcnt.GetPeriodsPerYr().ToString();
            arrEntries[1].szValue = oPcnt.GetNominalRate().ToString();
            arrEntries[2].szValue = oPcnt.GetEffectiveRate().ToString();

            Globals.Calc.AddString(arrEntries[0].szValue); //for N
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

            arrEntries[0].szTitle = Globals.rm.GetString("IDS_INFO_PeriodsYear");
            arrEntries[1].szTitle = Globals.rm.GetString("IDS_INFO_NominalRate");
            arrEntries[2].szTitle = Globals.rm.GetString("IDS_INFO_EffectiveRate");

            arrEntries[0].eEntry = EntryType.ENTRY_Int;
            arrEntries[1].eEntry = EntryType.ENTRY_Percent;
            arrEntries[2].eEntry = EntryType.ENTRY_Percent;


            tBlock.Text = Globals.rm.GetString("IDS_TITLE_PercentChange");

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
                case (int)EnumPcntScreenEntries.PCNT_N:
                    oPcnt.SetPeriodsPerYr(iTemp);
                    break;
                case (int)EnumPcntScreenEntries.PCNT_IY:
                    oPcnt.SetNominalRate(dbTemp);
                    break;
                case (int)EnumPcntScreenEntries.PCNT_PV:
                    oPcnt.SetEffectiveRate(dbTemp);
                    break;

                default://Ooops if here
                    //MessageBox(hWnd, _T("Index out of range"), _T("Contact iSS"), MB_ICONERROR);
                    return false;
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
                case (int)EnumPcntScreenEntries.PCNT_N:
                    szValue = oPcnt.GetPeriodsPerYr().ToString();
                    break;
                case (int)EnumPcntScreenEntries.PCNT_IY:
                    szValue = oPcnt.GetNominalRate().ToString();
                    break;
                case (int)EnumPcntScreenEntries.PCNT_PV:
                    szValue = oPcnt.GetEffectiveRate().ToString();
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
