using System;
using System.Net;
using System.Windows;
using System.ComponentModel;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace CalcPro
{
    public class CalcProDisplay : INotifyPropertyChanged
    {
        private bool _bDisplayExponent = false;
        private bool _bDisplayFrac = false;
        private string _szResult = "";
        private string _szExp = "";
        private string _szValue = "";
        private string _szRPNY = "";
        private DisplayType _eDisplay = DisplayType.DISPLAY_Float;
        private DRGStateType _eDRG = DRGStateType.DRG_Degrees;
        private int _iBase = 10;

        public event PropertyChangedEventHandler PropertyChanged;
        public string szBase
        {
            get
            {
                int iBase = Globals.Calc.GetCalcBase();
                switch (iBase)
                {
                    case 10:
                        return "DEC";
                    case 2:
                        return "BIN";
                    case 16:
                        return "HEX";
                    case 8:
                        return "OCT";
                    default:
                        return "B" + iBase.ToString();
                }
            }
        }
        public Visibility M1
        {
            get
            {
                if (Globals.Calc.IsMemoryFilled(0))
                    return Visibility.Visible;
                return Visibility.Collapsed;
            }
        }
        public Visibility M2
        {
            get
            {
                if (Globals.Calc.IsMemoryFilled(1) && Globals.btnMan.GetUseAdvMem() == true)
                    return Visibility.Visible;
                return Visibility.Collapsed;
            }
        }
        public Visibility M3
        {
            get
            {
                if (Globals.Calc.IsMemoryFilled(2) && Globals.btnMan.GetUseAdvMem() == true)
                    return Visibility.Visible;
                return Visibility.Collapsed;
            }
        }
        public Visibility s2ndF
        {
            get
            {
                if (Globals.Calc.GetCalcState() == CalcStateType.CALCSTATE_2ndF || Globals.Calc.GetCalcState() == CalcStateType.CALCSTATE_2ndF_Hyp)
                    return Visibility.Visible;
                else
                    return Visibility.Collapsed;
            }
        }
        public Visibility Hyp
        {
            get
            {
                if (Globals.Calc.GetCalcState() == CalcStateType.CALCSTATE_Hyp || Globals.Calc.GetCalcState() == CalcStateType.CALCSTATE_2ndF_Hyp)
                    return Visibility.Visible;
                else
                    return Visibility.Collapsed;
            }
        }
        public Visibility ResultNormal
        {
            get
            {
                if (_bDisplayExponent || _bDisplayFrac || Globals.Settings.eCalcType == CalcOperationType.CALC_RPN)
                    return Visibility.Collapsed;
                else
                    return Visibility.Visible;
            }
        }

        public Visibility ResultExponent
        {
            get
            {
                if (_bDisplayExponent)
                    return Visibility.Visible;
                return Visibility.Collapsed;
            }
        }

        public Visibility ResultWhole
        {
            get
            {
                if (_bDisplayFrac)
                    return Visibility.Visible;
                return Visibility.Collapsed;
            }

        }

        public string szLastEntry
        {
            get
            {
                if (Globals.Calc.GetCalcMode() == CalcOperationType.CALC_String)
                    return "";

                string szTempEq = null;
                Globals.Calc.GetEquation(ref szTempEq);
                return szTempEq;
            }
        }
        public string szEq
        {
            get
            {
                string szTempEq = null;
                Globals.Calc.GetEquation(ref szTempEq);
                return szTempEq;
            }
        }
        public string szDispType
        {
            get
            {
                switch (Globals.Calc.GetCalcDisplay())
                {
                    case DisplayType.DISPLAY_BCD:
                        return "BCD";
                    case DisplayType.DISPLAY_Engineering:
                        return "ENG";
                    case DisplayType.DISPLAY_Fixed:
                        return "FIX";
                    case DisplayType.DISPLAY_Fractions:
                        return "FRAC";
                    case DisplayType.DISPLAY_Scientific:
                        return "SCI";
                    case DisplayType.DISPLAY_Float:
                    default:
                        return "NORM";
                }
            }
        }

        public string szDRG
        {
            //we'll also display the base bits here
            get
            {   //DRG
                if (Globals.Calc.GetCalcBase() == 10)
                    return CalcGlobals.szDRG[(int)Globals.Calc.GetDRGState()];
                //base bits
                return Globals.Calc.GetBaseBits().ToString() + "bit";
            }
        }
        
        public string szResult
        {
            get { return _szResult; }
        }

        public string szWhole
        {
            get { return _szResult; }
        }

        public string szDenominator
        {
            get { return _szExp; }
        }

        public string szNumerator
        {
            get { return _szValue; }
        }

        
        public string szValue
        {
            get { return _szValue; }
        }

        public string szExp
        {
            get { return _szExp; }
        }

        public string szRPNY
        {
            get { return _szRPNY; }
        }

        public CalcProDisplay()
        {
            Globals.Calc.DisplayUpdate += UpdateDisplay;
        }

        //called from UpdateDisplay
        void UpdateDisplayFrac()
        {
            if ((_szExp.Length > 0) != _bDisplayFrac)
            {
                _bDisplayFrac = (_szExp.Length > 0 ? true : false);

                NotifyPropertyChanged("ResultNormal");
                NotifyPropertyChanged("ResultWhole");
            }

            if (_bDisplayFrac)
            {
                NotifyPropertyChanged("szWhole");
                NotifyPropertyChanged("szNumerator");
                NotifyPropertyChanged("szDenominator");
            }

            else
            {
                NotifyPropertyChanged("szResult");
            }

        }

        void ResetDisplay()
        {
            _bDisplayFrac = false;
            _bDisplayExponent = false;

            NotifyPropertyChanged("ResultExponent");
            NotifyPropertyChanged("ResultWhole");
            NotifyPropertyChanged("ResultNormal");
        }

        public void UpdateDisplay(bool bUpdate, int iBtnIndex, int iBtnId)
        {
            if (iBtnId == -1 && iBtnIndex == -1)
            {
                ResetDisplay();
            }


            if((InputType)iBtnIndex == InputType.INPUT_Memory || Globals.Settings.bAdvMem == true)
            {
                    //just pump em all
                    NotifyPropertyChanged("M1");
                    NotifyPropertyChanged("M2");
                    NotifyPropertyChanged("M3");
            }

            NotifyPropertyChanged("s2ndF");
            NotifyPropertyChanged("Hyp");
            NotifyPropertyChanged("szBase");
                        
            Globals.Calc.GetAnswer(ref _szResult, ref _szValue, ref _szExp, true, false);

            //almost always
            if(Globals.Calc.GetCalcMode() == CalcOperationType.CALC_String)
                NotifyPropertyChanged("szEq");
            else
                NotifyPropertyChanged("szLastEntry");

            if (Globals.Calc.GetDRGState() != _eDRG)
            {
                _eDRG = Globals.Calc.GetDRGState();
                NotifyPropertyChanged("szDRG");
            }
            else if (Globals.Calc.GetCalcBase() != _iBase)
            {
                _iBase = Globals.Calc.GetCalcBase();
                NotifyPropertyChanged("szDRG");
            }

            if (Globals.Calc.GetCalcDisplay() != _eDisplay)
            {
                _eDisplay = Globals.Calc.GetCalcDisplay();
                NotifyPropertyChanged("szDispType");
            }

            if (_eDisplay == DisplayType.DISPLAY_Fractions)
            {
                if (_szResult.Contains(".") == false && _szValue.Length > 0)
                {
                    NotifyPropertyChanged("ResultExponent");
                    UpdateDisplayFrac();
                    return;
                }
                _bDisplayFrac = false;
                NotifyPropertyChanged("ResultWhole");
                NotifyPropertyChanged("ResultNormal");
            }

            // if we have to change displays
            if ((_szExp.Length > 0) != _bDisplayExponent &&
                Globals.Calc.GetCalcMode() != CalcOperationType.CALC_RPN)

            {
                _bDisplayExponent = (_szExp.Length > 0?true:false);

                NotifyPropertyChanged("ResultNormal");
                NotifyPropertyChanged("ResultExponent");
            }
           
            if (_bDisplayExponent)
            {
                NotifyPropertyChanged("szValue");
                NotifyPropertyChanged("szExp");
            }

            else
            {
                NotifyPropertyChanged("szResult");
            }

            if (Globals.Settings.eCalcType == CalcOperationType.CALC_RPN)
            {
                Globals.Calc.GetRPNStackEntry(0, ref _szRPNY);
                NotifyPropertyChanged("szRPNY");
            }
        }

        private void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this,
                    new PropertyChangedEventArgs(propertyName));
            }

        }
    }
}
