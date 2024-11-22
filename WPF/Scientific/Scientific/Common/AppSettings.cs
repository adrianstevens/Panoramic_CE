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
using System.ComponentModel;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.IO;

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
            try
            {
                List<int> arrValues = null;

                XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                TextReader reader = new StreamReader("CalcOptions.xml");

                int iIndex = 0;

                arrValues = (List<int>)ser.Deserialize(reader);
                
                iCurrentScreen = arrValues[iIndex++];
                iPreviousScreen = arrValues[iIndex++];

                eSkinColor = (EnumCalcProSkinColor)arrValues[iIndex++];
                eSkinStyle = (EnumCalcProSkinStyle)arrValues[iIndex++];
                eCalcType = (CalcOperationType)arrValues[iIndex++];

                eDisplayType = (DisplayType)arrValues[iIndex++];
                iFixedDigits = arrValues[iIndex++];
                iConversionBuffer = arrValues[iIndex++];
                eDRG = (DRGStateType)arrValues[iIndex++];

                iRPNStackSize = arrValues[iIndex++];
                iPlaySounds = arrValues[iIndex++];
                iLanguageID = arrValues[iIndex++];
                bAdvMem = Convert.ToBoolean(arrValues[iIndex++]);
                bUseRegional = Convert.ToBoolean(arrValues[iIndex++]);

                eGraphStyle = (EnumGraphStyle)arrValues[iIndex++];

                reader.Close();
            }
            catch
            {

            }

            try
            {
                int iIndex = 0;

                List<String> arrStrings = null;

                XmlSerializer ser = new XmlSerializer(typeof(List<String>));
                TextReader reader = new StreamReader("CalcStrings.xml");

                arrStrings = (List<String>)ser.Deserialize(reader);

                string szTemp;

                for (int i = 0; i < Globals.NUM_GRAPHS; i++)
                {
                    szTemp = arrStrings[iIndex++];

                    Globals.szGraphs[i] = szTemp;
                }

                for (int i = 0; i < Globals.Calc.GetNumberOfMemory(); i++)
                {
                    szTemp = arrStrings[iIndex++];
                    
                    if(szTemp != null && szTemp.Length > 0)
                        Globals.Calc.SetMemory(szTemp, i);
                }

                reader.Close();
            }
            catch
            {

            }



     

             
        }

        public void Save()
        {
            List<int> arrValues = new List<int>();

            arrValues.Add(iCurrentScreen);
            arrValues.Add(iPreviousScreen);

            arrValues.Add((int)eSkinColor);
            arrValues.Add((int)eSkinStyle);
            arrValues.Add((int)eCalcType);

            arrValues.Add((int)eDisplayType);
            arrValues.Add(iFixedDigits);
            arrValues.Add(iConversionBuffer);
            arrValues.Add((int)eDRG);

            arrValues.Add(iRPNStackSize);
            arrValues.Add(iPlaySounds);
            arrValues.Add(iLanguageID);
            arrValues.Add(Convert.ToInt32(bAdvMem));

            arrValues.Add(Convert.ToInt32(bUseRegional));

            arrValues.Add((int)eGraphStyle);

            try
            {
                XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                TextWriter writer = new StreamWriter("CalcOptions.xml");
                ser.Serialize(writer, arrValues);
                writer.Close();
            }
            catch (System.Exception ex)
            {
            }

            List<String> arrStrings = new List<String>();

            //graphs
            for (int i = 0; i < Globals.NUM_GRAPHS; i++)
            {
                arrStrings.Add(Globals.szGraphs[i]);
            }

            for (int i = 0; i < Globals.Calc.GetNumberOfMemory(); i++)
            {
                if (Globals.Calc.GetMemory(i) != null)
                    arrStrings.Add(Globals.Calc.GetMemory(i));
                else
                    arrStrings.Add("");
            }

            try
            {
                XmlSerializer ser = new XmlSerializer(typeof(List<String>));
                TextWriter writer = new StreamWriter("CalcStrings.xml");
                ser.Serialize(writer, arrStrings);
                writer.Close();
            }
            catch (System.Exception ex)
            {
            }

        }
    }
}

