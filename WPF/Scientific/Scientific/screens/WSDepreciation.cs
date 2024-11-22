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
    enum EnumDepScreenEntries
    {
        DEP_Life,
        DEP_StartMonth,
        DEP_StartYear,
        DEP_Cost,
        DEP_Salvage,
        DEP_Year,
        DEP_Dep,
        DEP_RBV,
        DEP_RDV,
    };

    
    public class WSDepreciation : IWorkSheet
    {
        ObservableCollection<MyListboxItems> arrEntries;
        IssDrepreciation oDep = new IssDrepreciation();

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex) { return false; }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            oDep.Calc();
            arrEntries[(int)EnumDepScreenEntries.DEP_Dep].szValue = oDep.GetDep().ToString();
            arrEntries[(int)EnumDepScreenEntries.DEP_RBV].szValue = oDep.GetRemainingBookValue().ToString();
            arrEntries[(int)EnumDepScreenEntries.DEP_RDV].szValue = oDep.GetRemainingDepValue().ToString();

            return true;
        }

        public void OnReset()
        {
            if (arrEntries == null || arrEntries.Count == 0)
                return;

            oDep.ClearWorksheet();

            arrEntries[(int)EnumDepScreenEntries.DEP_Life].szValue = oDep.GetAssetLife().ToString();
            arrEntries[(int)EnumDepScreenEntries.DEP_StartMonth].szValue = oDep.GetStartMonth().ToString();
            arrEntries[(int)EnumDepScreenEntries.DEP_StartYear].szValue = oDep.GetStartYear().ToString();
            arrEntries[(int)EnumDepScreenEntries.DEP_Cost].szValue = oDep.GetCostOfAsset().ToString();
            arrEntries[(int)EnumDepScreenEntries.DEP_Salvage].szValue = oDep.GetSalvageValue().ToString();
            arrEntries[(int)EnumDepScreenEntries.DEP_Year].szValue = oDep.GetYearToCompute().ToString();
            arrEntries[(int)EnumDepScreenEntries.DEP_Dep].szValue = oDep.GetDep().ToString();
            arrEntries[(int)EnumDepScreenEntries.DEP_RBV].szValue = oDep.GetRemainingBookValue().ToString();
            arrEntries[(int)EnumDepScreenEntries.DEP_RDV].szValue = oDep.GetRemainingDepValue().ToString();

            
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
                list[i].bGreyed = false;
            }

            tBlock.Text = Globals.rm.GetString("IDS_TITLE_Depreciation");

            arrEntries[(int)EnumDepScreenEntries.DEP_Life].szTitle = Globals.rm.GetString("IDS_INFO_Life");
            arrEntries[(int)EnumDepScreenEntries.DEP_StartMonth].szTitle = Globals.rm.GetString("IDS_INFO_StartMonth");
            arrEntries[(int)EnumDepScreenEntries.DEP_StartYear].szTitle = Globals.rm.GetString("IDS_INFO_StartYear");
            arrEntries[(int)EnumDepScreenEntries.DEP_Cost].szTitle = Globals.rm.GetString("IDS_INFO_Cost");
            arrEntries[(int)EnumDepScreenEntries.DEP_Salvage].szTitle = Globals.rm.GetString("IDS_INFO_Salary");
            arrEntries[(int)EnumDepScreenEntries.DEP_Year].szTitle = Globals.rm.GetString("IDS_INFO_Year");
            arrEntries[(int)EnumDepScreenEntries.DEP_Dep].szTitle = Globals.rm.GetString("IDS_INFO_Depreciation");
            arrEntries[(int)EnumDepScreenEntries.DEP_RBV].szTitle = Globals.rm.GetString("IDS_INFO_RBVal");
            arrEntries[(int)EnumDepScreenEntries.DEP_RDV].szTitle = Globals.rm.GetString("IDS_INFO_RDVal");

            arrEntries[(int)EnumDepScreenEntries.DEP_Life].eEntry = EntryType.ENTRY_Int;
            arrEntries[(int)EnumDepScreenEntries.DEP_StartMonth].eEntry = EntryType.ENTRY_Int;
            arrEntries[(int)EnumDepScreenEntries.DEP_StartYear].eEntry = EntryType.ENTRY_Int;
            arrEntries[(int)EnumDepScreenEntries.DEP_Cost].eEntry = EntryType.ENTRY_Currency;
            arrEntries[(int)EnumDepScreenEntries.DEP_Salvage].eEntry = EntryType.ENTRY_Currency;
            arrEntries[(int)EnumDepScreenEntries.DEP_Year].eEntry = EntryType.ENTRY_Int;
            arrEntries[(int)EnumDepScreenEntries.DEP_Dep].eEntry = EntryType.ENTRY_Currency;
            arrEntries[(int)EnumDepScreenEntries.DEP_RBV].eEntry = EntryType.ENTRY_Currency;
            arrEntries[(int)EnumDepScreenEntries.DEP_RDV].eEntry = EntryType.ENTRY_Currency;

            arrEntries[(int)EnumDepScreenEntries.DEP_Dep].bGreyed = true;
            arrEntries[(int)EnumDepScreenEntries.DEP_RBV].bGreyed = true;
            arrEntries[(int)EnumDepScreenEntries.DEP_RDV].bGreyed = true;
            
            OnReset();

            return true;
        }

        public bool SetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0)
                return false;

            double dbTemp = 0;
            if (szValue != null && szValue.Length > 0)
                dbTemp = Convert.ToDouble(szValue);       
            int iTemp = (int)dbTemp;

            switch (iIndex)
            {
                case (int)EnumDepScreenEntries.DEP_Life:
                    oDep.SetAssetLife(iTemp);
                    break;
                case (int)EnumDepScreenEntries.DEP_StartMonth:
                    oDep.SetStartMonth(iTemp);
                    break;
                case (int)EnumDepScreenEntries.DEP_StartYear:
                    oDep.SetStartYear(iTemp);
                    break;
                case (int)EnumDepScreenEntries.DEP_Cost:
                    oDep.SetCostofAsset(dbTemp);
                    break;
                case (int)EnumDepScreenEntries.DEP_Salvage:
                    oDep.SetSalvageValue(dbTemp);
                    break;
                case (int)EnumDepScreenEntries.DEP_Year:
                    oDep.SetYearToCompute(iTemp);
                    break;
                default://Ooops if here
                //MessageBox(hWnd, _T("Index out of range"), _T("Contact Panoramic Software Inc"), MB_ICONERROR);
                case (int)EnumDepScreenEntries.DEP_Dep:
                case (int)EnumDepScreenEntries.DEP_RBV:
                case (int)EnumDepScreenEntries.DEP_RDV:
                    break;
            }

            return true;
        }

        public bool GetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0)
            {
                szValue = "";
                return false;
            }

            double dbTemp = 0;
            int iTemp = 0;

            switch (iIndex)
            {
                case (int)EnumDepScreenEntries.DEP_Life:
                    iTemp = oDep.GetAssetLife();
                    szValue = iTemp.ToString();
                    break;
                case (int)EnumDepScreenEntries.DEP_StartMonth:
                    iTemp = oDep.GetStartMonth();
                    szValue = iTemp.ToString();
                    break;
                case (int)EnumDepScreenEntries.DEP_StartYear:
                    iTemp = oDep.GetStartYear();
                    szValue = iTemp.ToString();
                    break;
                case (int)EnumDepScreenEntries.DEP_Cost:
                    dbTemp = oDep.GetCostOfAsset();
                    szValue = dbTemp.ToString();
                    break;
                case (int)EnumDepScreenEntries.DEP_Salvage:
                    dbTemp = oDep.GetSalvageValue();
                    szValue = dbTemp.ToString();
                    break;
                case (int)EnumDepScreenEntries.DEP_Year:
                    iTemp = oDep.GetYearToCompute();
                    szValue = iTemp.ToString();
                    break;
                case (int)EnumDepScreenEntries.DEP_Dep:
                    dbTemp = oDep.GetDep();
                    szValue = dbTemp.ToString();
                    break;
                case (int)EnumDepScreenEntries.DEP_RBV:
                    dbTemp = oDep.GetRemainingBookValue();
                    szValue = dbTemp.ToString();
                    break;
                case (int)EnumDepScreenEntries.DEP_RDV:
                    dbTemp = oDep.GetRemainingDepValue();
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
