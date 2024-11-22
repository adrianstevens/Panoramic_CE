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

using System.Xml.Serialization;
using System.IO.IsolatedStorage;
using System.Collections.Generic;

namespace CalcPro
{
    public class WSStats : IWorkSheet
    {
        public const int NUM_DATASETS = 5;
        int iDataIndex = 0;
        ObservableCollection<MyListboxItems>[] arrDataSets = new ObservableCollection<MyListboxItems>[NUM_DATASETS];

        TextBlock textTitle;//just a ref so we can update it when we change datasets


        //reference passed in from the actual listbox
        ObservableCollection<MyListboxItems> arrEntries;


        IssStats oStats = new IssStats();

        EnumStatistics eStatType = EnumStatistics.Stats_Linear_Reg;

        string szEquation;

        public WSStats(EnumStatistics eType)
        {
            eStatType = eType;
        }

        public bool OnCalc(int iIndex, FrameworkElement fwElem)
        {
            int iTemp = 0;
            double dbTemp = 0;
            string szTemp = "";

            oStats.ResetWorkSheet();

         /*   PopupMenu pop = new PopupMenu(fwElem, Globals.rm.GetString("IDS_MENU_Stats"));

            //enter all of the values
            for(int i = 0; i < arrEntries.Count; i+=2)
            {
                oStats.AddXValue(Convert.ToDouble(arrEntries[i].szValue), i/2);
                oStats.AddYValue(Convert.ToDouble(arrEntries[i+1].szValue), i/2);
            }

            oStats.SetStatsType(eStatType);

            //solve
            if(!oStats.Calculate())
            {	
                return false;
            }

            BuildLineEq();

            //lets open an answer DLG
            //now lets add some terms and stuff
            iTemp = oStats.GetNumberOfXYPairs();
            szTemp = iTemp.ToString();
            

            oStats.GetMeanX(ref dbTemp);
            szTemp = dbTemp.ToString();

            pop.AddItem(Globals.rm.GetString("IDS_CALC_MeanOfX"), szTemp);
            

            //oStats.GetMedianX(ref dbTemp);
            //szTemp = dbTemp.ToString();
            //pop.AddItem("Median of X", szTemp);

            oStats.GetSX(ref dbTemp);
            szTemp = dbTemp.ToString();
            pop.AddItem("SX", szTemp);

            oStats.GetPopSX(ref dbTemp);
            szTemp = dbTemp.ToString();
            pop.AddItem(Globals.rm.GetString("IDS_CALC_DevOfX"), szTemp);
            
            oStats.GetMeanY(ref dbTemp);
            szTemp = dbTemp.ToString();
            pop.AddItem(Globals.rm.GetString("IDS_CALC_MeanOfY"), szTemp);
            
           // oStats.GetMedianY(ref dbTemp);
           // szTemp = dbTemp.ToString();
           // dlgList.AddItem(_T("Median of Y"), szTemp);	

            oStats.GetSY(ref dbTemp);
            szTemp = dbTemp.ToString();
            pop.AddItem("SY", szTemp);
            
            oStats.GetPopSY(ref dbTemp);
            szTemp = dbTemp.ToString();
            pop.AddItem(Globals.rm.GetString("IDS_CALC_DevOfY"), szTemp);
                   
            oStats.GetSumX(ref dbTemp);
            szTemp = dbTemp.ToString();
            pop.AddItem("\x2211X", szTemp);

            oStats.GetSumX2(ref dbTemp);
            szTemp = dbTemp.ToString();
            pop.AddItem("\x2211X²", szTemp);
            
            oStats.GetSumY(ref dbTemp);
            szTemp = dbTemp.ToString();
            pop.AddItem("\x2211Y", szTemp);

            oStats.GetSumY2(ref dbTemp);
            szTemp = dbTemp.ToString();
            pop.AddItem("\x2211Y²", szTemp);

            oStats.GetSumXY(ref dbTemp);
            szTemp = dbTemp.ToString();
            pop.AddItem("\x2211X×Y", szTemp);


            switch(eStatType)
            {
            case EnumStatistics.Stats_Linear_Reg:
                    pop.AddItem(Globals.rm.GetString("IDS_CALC_Equation"), "f(x)=a+b×x");
                
                oStats.GetA(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_YIntercept"), szTemp);
                
                oStats.GetB(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Slope"), szTemp);
                
                oStats.GetR(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Regression"), szTemp);
                
                dbTemp = dbTemp*dbTemp;
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Determination"), szTemp);
    	        break;

            case EnumStatistics.Stats_Ln_Reg:
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Equation"), "f(x)=a+b×ln(x)");

                oStats.GetA(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem("a", szTemp);

                oStats.GetB(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem("b", szTemp);

                oStats.GetR(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Regression"), szTemp);

                dbTemp = dbTemp * dbTemp;
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Determination"), szTemp);
                break;

            case EnumStatistics.Stats_Log_Reg:
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Equation"), "f(x)=a+b×log(x)");

                oStats.GetA(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_YIntercept"), szTemp);

                oStats.GetB(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Slope"), szTemp);

                oStats.GetR(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Regression"), szTemp);

                dbTemp = dbTemp * dbTemp;
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Determination"), szTemp);
                break;

            case EnumStatistics.Stats_Exponential_Reg:
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Equation"), "f(x)=a×b^x");
                
                oStats.GetA(ref dbTemp);
                dbTemp = Math.Pow(10, dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem("a", szTemp);

                oStats.GetB(ref dbTemp);
                dbTemp = Math.Pow(10, dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem("b", szTemp);
                
                oStats.GetR(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Regression"), szTemp);
                
                dbTemp = dbTemp*dbTemp;
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Determination"), szTemp);
                break;
            case EnumStatistics.Stats_Power_Reg:
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Equation"), "f(x)=a×x^b");

                oStats.GetA(ref dbTemp);
                dbTemp = Math.Pow(Math.E, dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem("a", szTemp);

                oStats.GetB(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem("b", szTemp);

                oStats.GetR(ref dbTemp);
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Regression"), szTemp);

                dbTemp = dbTemp * dbTemp;
                szTemp = dbTemp.ToString();
                pop.AddItem(Globals.rm.GetString("IDS_CALC_Determination"), szTemp);
                break;

            default:
                break;
            }

            pop.Launch();
            */
          
/*            pop.SelectionChanged += delegate(object MySender, PopupMenuEventArgs eArg)
            {
                Globals.Calc.AddString(Globals.objConstants.GetFavoriteConstant(eArg.iSelected));
            };*/

            return true;
        }

        public void OnReset()
        {
            oStats.ResetWorkSheet();
            arrEntries.Clear();
            AddNextSet();
            AddNextSet();
        }


        public void OnGraph()
        {
            //gotta add the data before you can do anything
            oStats.ResetWorkSheet();

            //enter all of the values
            for (int i = 0; i < arrEntries.Count(); i += 2)
            {
                oStats.AddXValue(Convert.ToDouble(arrEntries[i].szValue), i / 2);
                oStats.AddYValue(Convert.ToDouble(arrEntries[i + 1].szValue), i / 2);
            }
            oStats.SetStatsType(eStatType);

            oStats.Calculate();

            BuildLineEq();

            for (int i = 1; i < Globals.NUM_GRAPHS; i++)
                Globals.szGraphs[i] = null;
            Globals.szGraphs[0] = szEquation;
            Globals.bGraphUsePoints = true;

            if (Globals.arrGraphPoints == null)
                Globals.arrGraphPoints = new List<Point>();
            else
                Globals.arrGraphPoints.Clear();

            for (int i = 0; i < oStats.GetNumberOfXYPairs(); i++)
                Globals.arrGraphPoints.Add(new Point(oStats.RecallXValue(i), oStats.RecallYValue(i)));
            
            Globals.LaunchPage((int)InputType.INPUT_Graph, 0);

            //and graph the sucker
           
           /* dlgGraph.UsePoints(g_bGraphPoints);


            dlgGraph.SetGraph(szEquation, 0);

            for (int i = 0; i < oStats.GetNumberOfXYPairs(); i++)
            {
                dlgGraph.AddPoint(oStats.RecallXValue(i), oStats.RecallYValue(i));
            }*/
        }

        public void OnLoad()
        {
           
        }

        public void OnSave()
        {
        }

        void SetTitle(ref System.Windows.Controls.TextBlock tBlock)
        {
            switch (eStatType)
            {
                case EnumStatistics.Stats_Power_Reg:
                    tBlock.Text = Globals.rm.GetString("IDS_INFO_PowerRegression");
                    break;
                case EnumStatistics.Stats_Log_Reg:
                case EnumStatistics.Stats_Ln_Reg:
                    tBlock.Text = Globals.rm.GetString("IDS_INFO_LogRegression");
                    break;
                case EnumStatistics.Stats_Exponential_Reg:
                    tBlock.Text = Globals.rm.GetString("IDS_INFO_ExpRegression");
                    break;
                case EnumStatistics.Stats_Linear_Reg:
                default:
                    tBlock.Text = Globals.rm.GetString("IDS_INFO_LinearRegression");
                    break;
            }

            tBlock.Text += " (" + (iDataIndex + 1) + ")";
        }

        public bool Init(ref ObservableCollection<MyListboxItems> list, ref System.Windows.Controls.TextBlock tBlock)
        {
            textTitle = tBlock;
            SetTitle(ref tBlock);

            oStats.ResetWorkSheet();
            if (LoadValues(ref list) == false)
            {
                arrEntries = list;
                OnReset();
            }

            arrEntries = list;

            return true;
        }

        public bool SetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0)
                return false;

            arrEntries[iIndex].szValue = szValue;

            return false;
        }

        public void SetStatsType(LaunchType eWS)
        {
            switch(eWS)
            {
            case LaunchType.LAUNCH_Power:
                eStatType = EnumStatistics.Stats_Power_Reg;
                break;
            case LaunchType.LAUNCH_LogReg:
                eStatType = EnumStatistics.Stats_Log_Reg;
                break;
            case LaunchType.LAUNCH_ExpReg:
                eStatType = EnumStatistics.Stats_Exponential_Reg;
                break;
            case LaunchType.LAUNCH_LinearReg:
            default:
                eStatType = EnumStatistics.Stats_Linear_Reg;
                break;
            }
        }

        void AddNextSet()
        {
            string szTemp;
                        

            //we "could" handle 1 variable stats here too ...
            int iEntry = 0;
            
            if(arrEntries != null)
                iEntry = arrEntries.Count / 2 + 1;


            MyListboxItems sEntry = new MyListboxItems();

            //now we need both an X ref  a Y
            szTemp = String.Format("X{0:0}:",iEntry);

            sEntry.szTitle = szTemp;
            sEntry.szValue = ("0");
            sEntry.bGreyed = false;
            sEntry.eEntry = EntryType.ENTRY_Double;

            arrEntries.Add(sEntry);
            
            
            sEntry = new MyListboxItems();

            //now we need both an X ref  a Y
            szTemp = String.Format("Y{0:0}:",iEntry);

            sEntry.szTitle = szTemp;
            sEntry.szValue = ("0");
            sEntry.bGreyed = false;
            sEntry.eEntry = EntryType.ENTRY_Double;

            arrEntries.Add(sEntry);
        }

        bool BuildLineEq()
        {
            double dbA = 0;
            double dbB = 0;
            
            //Create the equation in terms of f(x)
            szEquation = "";

            switch(eStatType)
            {
                case EnumStatistics.Stats_Ln_Reg:
                    oStats.GetA(ref dbA);
                    //szEquation = dbA.ToString();
                    oStats.GetB(ref dbB);
                    //szEquation += "+" + dbB.ToString() + "*ln(X)";

                    szEquation = String.Format("{0:0.#####}+{1:0.#####}*ln(X)", dbA, dbB);

                    break;

                case EnumStatistics.Stats_Power_Reg:
                    oStats.GetA(ref dbA);
                    dbA = Math.Pow(Math.E, dbA);
                  //  szEquation = dbA.ToString();
                    oStats.GetB(ref dbB);
                   // szEquation += "*X^" + dbB.ToString();
                    szEquation = String.Format("{0:0.#####}*X^{1:0.#####}", dbA, dbB);
                    break;



                case EnumStatistics.Stats_Exponential_Reg:
                    oStats.GetA(ref dbA);
                    oStats.GetB(ref dbB);

                    dbA = Math.Pow(10, dbA);
                    dbB = Math.Pow(10, dbB);

                  //  szEquation = dbA.ToString() + "*" + dbB.ToString() + "^(X)";
                    szEquation = String.Format("{0:0.#####}*{1:0.#####}^X", dbA, dbB);
                    break;
                case EnumStatistics.Stats_Linear_Reg:
                default:
                    oStats.GetB(ref dbB);
                    oStats.GetA(ref dbA);
                    szEquation = String.Format("{0:0.#####}+{1:0.#####}*X", dbA, dbB);
                    break;
            }

            //equation built y = ax+b
            return true;
        }

        public bool GetScreenValue(int iIndex, ref string szValue)
        {
            if (iIndex < 0)
            {
                szValue = "";
                return false;
            }

            szValue = arrEntries[iIndex].szValue;

            return true;

        }

        public void OnUp(int iCurrentIndex)
        {
        }

        public void OnDown(int iCurrentIndex)
        {
            if (iCurrentIndex + 1 >= arrEntries.Count)
            {
                AddNextSet();
            }
        }

        public void OnClose()
        {
         /*   ObservableCollection<MyListboxItems> list = null;

            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
                for (int i = 0; i < NUM_DATASETS; i++)
                {
                    if (i == iDataIndex)
                        list = arrEntries;
                    else
                        list = arrDataSets[i];

                    //If user choose to save, create a new file
                    using (IsolatedStorageFileStream fs = isf.CreateFile("Stats" + i.ToString()))
                    {
                        //and serialize data
                        XmlSerializer ser = new XmlSerializer(typeof(ObservableCollection<MyListboxItems>));
                        ser.Serialize(fs, list);
                    }
                }
            } */
        }

        bool LoadValues(ref ObservableCollection<MyListboxItems> arrList)
        {
            ObservableCollection<MyListboxItems> list = null;
            bool bRet = false;

            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
             /*   for (int i = 0; i < NUM_DATASETS; i++)
                {
                    if (isf.FileExists("Stats" + i.ToString()))
                    {
                        //If user choose to save, create a new file
                        using (IsolatedStorageFileStream fs = isf.OpenFile("Stats" + i.ToString(), System.IO.FileMode.Open))
                        {
                            //and serialize data
                            XmlSerializer ser = new XmlSerializer(typeof(ObservableCollection<MyListboxItems>));
                            list = (ObservableCollection<MyListboxItems>)ser.Deserialize(fs);
                        }

                        if (i == iDataIndex)
                            arrList = list;
                        else
                            arrDataSets[i] = list;

                        bRet = true;
                    } 
                }*/
            }
            return bRet;
        }

        public bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex)
        {
            if (iIndex < 0 || iIndex >= NUM_DATASETS)
                return false; //or we could default to 0

            //save the current set
            arrDataSets[iDataIndex] = arrEntries;

            //update the active set
            if(arrDataSets[iIndex] == null)
                arrDataSets[iIndex] = new ObservableCollection<MyListboxItems>();
            list = arrDataSets[iIndex];
            arrEntries = list;

            //update the index
            iDataIndex = iIndex;

            //update the title
            SetTitle(ref textTitle);

            //and finally, reset it if its empty
            if (list.Count == 0)
                OnReset();

            return true;
        }
    }
}
