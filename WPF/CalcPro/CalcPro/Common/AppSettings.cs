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
using System.IO.IsolatedStorage;
using System.ComponentModel;

namespace CalcPro
{
    public class AppSettings
    {
        public int iCurrentScreen;
        public int iPreviousScreen;

        private const string REG_CurrentScreen = "current";
        private const string REG_PreviousScreen = "previous";
        
        private const string REG_CalcMode = "calcmode";
        private const string REG_Notation = "notation";
        private const string REG_FixedDigits = "fixeddigits";
        private const string REG_ConversionBuf = "basebits";
        private const string REG_DRG = "drg";

        private const string REG_RPNStack = "rpnstack";
        private const string REG_PlaySounds = "playsounds";
        private const string REG_EndPeriodPay = "endperiodpay";
        private const string REG_PaymentSched = "paymentsched";
        private const string REG_AdvancedMem = "advancedmem";
        private const string REG_CalcSkinColor = "skincolor";
        private const string REG_CalcSkinStyle = "skinstyle";
        private const string REG_Language = "language";
        private const string REG_Regional = "regional";
        private const string REG_GraphStyle = "graphstyle";

        private const string REG_CurFrom = "curfromindex";
        private const string REG_CurTo = "curtoindex";

        //so this class will be responsible for all of the gets and sets

        //skin
        public EnumCalcProSkinColor eSkinColor
        {
            get {  return Globals.btnMan.GetSkinColor();  }
            set {  if(Globals.btnMan != null)Globals.btnMan.SetSkinColor(value, true); }
        }

        public EnumCalcProSkinStyle eSkinStyle
        {
            get { return Globals.btnMan.GetSkinStyle(); }
            set { if (Globals.btnMan != null)Globals.btnMan.SetSkinStyle(value, true); }
        }

        //calc mode
        public CalcOperationType eCalcType
        {
            get { return _eCalcType; }
            set
            {
                if (Globals.Calc != null) Globals.Calc.SetCalcMode(value); _eCalcType = value;

                if(_eCalcType == CalcOperationType.CALC_RPN)  Globals.btnMan.SetUseRPN(true); 
                else Globals.btnMan.SetUseRPN(false);
            }
        }
        CalcOperationType _eCalcType = CalcOperationType.CALC_OrderOfOpps;//this is here so we can change the calc mode independant of the settings (ie force for graph, worksheet, etc)

        //calc notation (fixed, sci, etc)
        public DisplayType eDisplayType
        {
            get { return Globals.Calc.GetCalcDisplay(); }
            set { Globals.Calc.SetCalcDisplay(value); }
        }

        public int iFixedDigits
        {
            get { return Globals.Calc.GetFixedDecDigits(); }
            set { Globals.Calc.SetFixedDecDigits(value); }
        }

        public int iConversionBuffer
        {
            get { return Globals.Calc.GetBaseBits(); }
            set { Globals.Calc.SetBaseBits(value); }
        }
        
        public int iRPNStackSize
        {
            get { return Globals.Calc.GetRPNStackSize(); }
            set { Globals.Calc.SetRPNStackSize(value); }
        }

        public DRGStateType eDRG
        {
            get { return Globals.Calc.GetDRGState(); }
            set { Globals.Calc.SetDRGState(value); }
        }

        //memory
        public bool bAdvMem
        {
            get { return Globals.btnMan.GetUseAdvMem(); }
            set { Globals.btnMan.SetUseAdvMem(value); }
        }

        //sounds
        public int iPlaySounds
        {
            get { return _iPlaySounds; }
            set { _iPlaySounds = value;}
        }
        int _iPlaySounds = 2;//this one can get stored here

        //language
        public int iLanguageID
        {
            get { return _iLanguageID; }
            set { _iLanguageID = value; }

        }
        int _iLanguageID;

        //record tape
        public bool bRecordTape
        {
            get { return _bRecordTape; }
            set { _bRecordTape = value; }
        }
        bool _bRecordTape;

        public bool bUseRegional
        {
            get { return Globals.Calc.GetUseComma(); }
            set { Globals.Calc.SetUseComma(value); }
        }

        public PAYMENT_SCHEDULE ePaymentSched
        {
            get { return _ePaymentSched; }
            set { _ePaymentSched = value; }
        }
        PAYMENT_SCHEDULE _ePaymentSched;

        public bool bEndPeriodPayments
        {
            get { return _bEndPeriodPayments; }
            set { _bEndPeriodPayments = value; }
        }
        bool _bEndPeriodPayments;
        
        public EnumGraphStyle eGraphStyle
        {
            get;
            set;
        }

        public AppSettings()
        {
            iCurrentScreen = (int)EnumCalcType.CALC_Standard;
            iPreviousScreen = (int)EnumCalcType.CALC_Scientific;

            Read();
        }

        public void Read()
        {
      /*      int iTemp = 0;
            bool bTemp = false;
            string szTemp = "";

            IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_CurrentScreen, out iCurrentScreen);
            IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_PreviousScreen, out iPreviousScreen);

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_CalcSkinColor, out iTemp))
                eSkinColor = (EnumCalcProSkinColor)iTemp;

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_CalcSkinStyle, out iTemp))
                eSkinStyle = (EnumCalcProSkinStyle)iTemp;

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_CalcMode, out iTemp))
                eCalcType = (CalcOperationType)iTemp;

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_Notation, out iTemp))
                eDisplayType = (DisplayType)iTemp;

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_FixedDigits, out iTemp))
                iFixedDigits = iTemp;

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_DRG, out iTemp))
                eDRG = (DRGStateType)iTemp;

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>( REG_ConversionBuf, out iTemp))
                iConversionBuffer = iTemp;
            
            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>( REG_RPNStack, out iTemp))
                iRPNStackSize = iTemp;

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_PlaySounds, out iTemp))
                iPlaySounds = iTemp;

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_Language, out iTemp))
                iLanguageID = iTemp;

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_EndPeriodPay, out iTemp))
            {   }//bugbug

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_PaymentSched, out iTemp))
                {   }//bugubg
            
            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<bool>(REG_AdvancedMem, out bTemp))
                bAdvMem = bTemp;

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<bool>(REG_Regional, out bTemp))
                bUseRegional = bTemp;

            if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<int>(REG_GraphStyle, out iTemp))
                eGraphStyle = (EnumGraphStyle)iTemp;

            //graphs

            for (int i = 0; i < Globals.NUM_GRAPHS; i++)
            {
                if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<string>("graph" + i.ToString(), out szTemp))
                    Globals.szGraphs[i] = szTemp;
                else
                    Globals.szGraphs[i] = null;
            }

            for (int i = 0; i < Globals.Calc.GetNumberOfMemory(); i++)
            {
                if (IsolatedStorageSettings.ApplicationSettings.TryGetValue<string>("memory" + i.ToString(), out szTemp))
                    Globals.Calc.SetMemory(szTemp, i);
            }  */
        }

        public void Save()
        {
         /* IsolatedStorageSettings.ApplicationSettings.Clear();

            IsolatedStorageSettings.ApplicationSettings.Add(REG_CurrentScreen, iCurrentScreen);
            IsolatedStorageSettings.ApplicationSettings.Add(REG_PreviousScreen, iPreviousScreen);

            IsolatedStorageSettings.ApplicationSettings.Add(REG_CalcSkinColor, (int)eSkinColor);
            IsolatedStorageSettings.ApplicationSettings.Add(REG_CalcSkinStyle, (int)eSkinStyle);
            IsolatedStorageSettings.ApplicationSettings.Add(REG_CalcMode, (int)eCalcType);

            IsolatedStorageSettings.ApplicationSettings.Add(REG_Notation, (int)eDisplayType);
            IsolatedStorageSettings.ApplicationSettings.Add(REG_FixedDigits, iFixedDigits);
            IsolatedStorageSettings.ApplicationSettings.Add(REG_ConversionBuf, iConversionBuffer);
            IsolatedStorageSettings.ApplicationSettings.Add(REG_DRG, (int)eDRG);

            IsolatedStorageSettings.ApplicationSettings.Add(REG_RPNStack, iRPNStackSize);
            IsolatedStorageSettings.ApplicationSettings.Add(REG_PlaySounds, iPlaySounds);
            IsolatedStorageSettings.ApplicationSettings.Add(REG_Language, iLanguageID);
            //IsolatedStorageSettings.ApplicationSettings.Add(REG_EndPeriodPay,
            //IsolatedStorageSettings.ApplicationSettings.Add(REG_PaymentSched, 0);
            IsolatedStorageSettings.ApplicationSettings.Add(REG_AdvancedMem, bAdvMem);

            IsolatedStorageSettings.ApplicationSettings.Add(REG_Regional, bUseRegional);

            IsolatedStorageSettings.ApplicationSettings.Add(REG_GraphStyle, (int)eGraphStyle);

            //graphs
            for (int i = 0; i < Globals.NUM_GRAPHS; i++)
            {
                IsolatedStorageSettings.ApplicationSettings.Add("graph" + i.ToString(), Globals.szGraphs[i]);
            }

            for (int i = 0; i < Globals.Calc.GetNumberOfMemory(); i++)
            {
                if(Globals.Calc.GetMemory(i) != null)
                    IsolatedStorageSettings.ApplicationSettings.Add("memory" + i.ToString(), Globals.Calc.GetMemory(i));
            }
            
            IsolatedStorageSettings.ApplicationSettings.Save();
          * 
          */ 
        }
    }
}

