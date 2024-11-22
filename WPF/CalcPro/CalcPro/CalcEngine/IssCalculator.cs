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

using System.Xml.Serialization;
using System.IO.IsolatedStorage;

namespace CalcPro
{
    public class IssCalculator
    {
        struct IssCalcState
        {
            public CalcStateType eCalcState;
            public string szLastAnswer;
            public int iDisplayCharacters;
            public int iErrorCount;
        };

        public IssCalculator()
        {
            eCalcType					= CalcOperationType.CALC_OrderOfOpps;
	        eCalcState				    = CalcStateType.CALCSTATE_Normal;

            iDisplayCharacters          = 20;
            iErrorCount                 = 0;
        }

        public delegate void UpdateDisplayHandler(bool bUpdate, int iBtnType, int iBtnId);

        public UpdateDisplayHandler DisplayUpdate
        {
            get{return _DisplayUpdate;}
            set{ _DisplayUpdate = value;}
        }
        private UpdateDisplayHandler _DisplayUpdate;

        
        public delegate void UpdateCalcState(int iState);

        public UpdateCalcState StateUpdate
        {
            get{return _StateUpdate;}
            set
            {
                _StateUpdate = value;
            }
        }

        private UpdateCalcState _StateUpdate;

        	
	    //CIssSciCalcUtil*			m_oUtil;
	    public IssFormatCalcDisplay	oDisplay = new IssFormatCalcDisplay();

	    IssStringCalc				oStrCalc = new IssStringCalc();
	    IssOOOCalc					oOOOCalc = new IssOOOCalc();
	    IssDALCalc					oDALCalc = new IssDALCalc();
	    IssChainCalc				oChainCalc = new IssChainCalc();
	    IssRPNCalc					oRPNCalc = new IssRPNCalc();

	    CalcOperationType			eCalcType;

        public CalcStateType eCalcState
        {
            get { return _eCalcState; }
            set 
            {
                if (value != _eCalcState)
                {
                    _eCalcState = value;


                    if (StateUpdate != null)
                        StateUpdate((int)value);
                }
            
            }
        }
        private CalcStateType _eCalcState;

	    string[]					szMemory = new string[(int)CalcGlobals.NUMBER_OF_MemoryBanks];
	    string						szLastAnswer;
	    
	    int						    iDisplayCharacters;
	    int						    iErrorCount;

        public int				    GetDisplayCharacters(){return iDisplayCharacters;}
        public void				    SetDisplayCharacters(int iNumChar){iDisplayCharacters = iNumChar;}

        public int				    GetNumberOfMemory(){return CalcGlobals.NUMBER_OF_MemoryBanks;}

        public bool                 SetMemory(string szMem, int iIndex) { szMemory[iIndex] = szMem; return true; }

        public CalcOperationType GetCalcMode() { return eCalcType; }//Calc operation mode..ie, OOO, DAL, etc...stored in this class

        public bool GetRepeatedEquals() { return oChainCalc.GetRepeatedEquals(); }
        public bool SetRepeatedEquals(bool bRepeatedEquals) { return oChainCalc.SetRepeatedEquals(bRepeatedEquals); }

        public int GetRPNStackSize() { return oRPNCalc.GetStackSize(); }
        public bool SetRPNStackSize(int iStackSize) { return oRPNCalc.SetStackSize(iStackSize); }

        public int GetFixedDecDigits() { return oDisplay.GetFixedDecDigits(); }
        public bool SetFixedDecDigits(int iDigits) { return oDisplay.SetFixedDecDigits(iDigits); }


        public CalcStateType GetCalcState() { return eCalcState; }

        public bool AddNumber(NumberType eNumberType)
        {
            if((int)eNumberType > CalcGlobals.NUMBER_OF_Numbers -1 ||
                (int)eNumberType < 0)
                return false;

            if(eCalcState == CalcStateType.CALCSTATE_MemoryRecall)
            {
                string szTemp = "";
                int iIndex=(int)eNumberType;
                if(iIndex >= CalcGlobals.NUMBER_OF_MemoryBanks)
                    return false;

                GetMemory(ref szTemp, iIndex);
                eCalcState =CalcStateType.CALCSTATE_Normal;
                return AddString(szTemp);
            }
            else if(eCalcState ==CalcStateType.CALCSTATE_MemoryStore)
            {
                string szTemp = "";
                int iIndex=(int)eNumberType;
                if(iIndex >= CalcGlobals.NUMBER_OF_MemoryBanks)
                    return false;

                GetAnswer(ref szTemp, false, false);
                eCalcState =CalcStateType.CALCSTATE_Normal;
                return StoreMemory(szTemp, iIndex);
            }
    
            //good ol Binary Converted Decimal
            if(GetShowBCD() == true)
            {
                if(eNumberType > NumberType.NUM_1)
                    return false;
            }

            eCalcState =CalcStateType.CALCSTATE_Normal;

	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		         return oStrCalc.AddNumber(eNumberType);

	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.AddNumber(eNumberType);
	
	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.AddNumber(eNumberType);
	
	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.AddNumber(eNumberType);
	
	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.AddNumber(eNumberType);
	
	        default:
		        break;
	        }
	
	        return false;
        }

        public bool GetCalcBaseName(string szBase)
        {
            szBase = CalcGlobals.szBaseNames[GetCalcBase() - 2];
                return true;
          
        }

        public bool AddFunction(FunctionType eFunction)
        {
	        eCalcState =CalcStateType.CALCSTATE_Normal;
    	
	        if((int)eFunction > CalcGlobals.NUMBER_OF_Functions -1 ||
		        (int)eFunction < 0)
		        return false;

	        if(eFunction == FunctionType.FUNCTION_CHS)
		        eFunction = FunctionType.FUNCTION_PlusMinus;//RPN style

            if(eFunction != FunctionType.FUNCTION_PlusMinus)
            {
                if(GetCalcBase() != 10)
                    return false;
            }

            if(eFunction == FunctionType.FUNCTION_DecToDeg)
            {
                AddEquals(EqualsType.EQUALS_Equals);
                SetShowDMS(true);
                return true;
            }
            else if(eFunction == FunctionType.FUNCTION_DegToDec)
            {
                AddEquals(EqualsType.EQUALS_Equals);
                SetShowDMS(false);
                return true;
            }

	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		         return oStrCalc.AddFunction(eFunction);
		
	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.AddFunction(eFunction);
		       
	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.AddFunction(eFunction);
		     
	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.AddFunction(eFunction);
		        
	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.AddFunction(eFunction);
		   
	        default:
		        break;
	        }
	        return false;
        }

        public bool AddOperator(OperatorType eOperator)
        {
	        eCalcState =CalcStateType.CALCSTATE_Normal;

	        if((int)eOperator > CalcGlobals.NUMBER_OF_Operators -1 ||
		        (int)eOperator < 0)
		        return false;

	        if(eOperator == OperatorType.OPP_EEX)
		        eOperator = OperatorType.OPP_EXP;//save the hassles
	
	        //check for valid operators based on the calc base
	        switch(eOperator)
	        {
		        case OperatorType.OPP_Plus:
                case OperatorType.OPP_Minus:
                case OperatorType.OPP_Times:
                case OperatorType.OPP_Divide:
                case OperatorType.OPP_OR:
                case OperatorType.OPP_AND:
                case OperatorType.OPP_NOR:
                case OperatorType.OPP_MOD:
                case OperatorType.OPP_XOR:
                case OperatorType.OPP_NAND:
                case OperatorType.OPP_XNOR:
			        break;
		        default:
			        if(GetCalcBase()!=10)
				        return false;
			        break;
	        }

	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        return oStrCalc.AddOperator(eOperator);
		        
	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.AddOperator(eOperator);
		        
	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.AddOperator(eOperator);
		        
	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.AddOperator(eOperator);
		        
	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.AddOperator(eOperator);
		        
	        default:
		        break;
	        }
	        return false;
        }

        public bool AddEquals(EqualsType eEquals)
        {
	        eCalcState =CalcStateType.CALCSTATE_Normal;

	        if(eEquals == EqualsType.EQUALS_Enter)
		        eEquals = EqualsType.EQUALS_Equals;//saves changing the classes

	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        oStrCalc.AddEquals(eEquals);
		        break;
	        case CalcOperationType.CALC_OrderOfOpps:
		        oOOOCalc.AddEquals(eEquals);
		        break;
	        case CalcOperationType.CALC_DAL:
		        oDALCalc.AddEquals(eEquals);
		        break;
	        case CalcOperationType.CALC_Chain:
		        oChainCalc.AddEquals(eEquals);
		        break;
	        case CalcOperationType.CALC_RPN:
		        oRPNCalc.AddEquals(eEquals);
		        break;
	        default:
		        return false;
		    
	        }
	        GetAnswer(ref szLastAnswer, false, false);

	        return true;
        }

        public bool AddBrackets(BracketType eBracket)
        {
	        if((int)eBracket < 0 ||
		        (int)eBracket > CalcGlobals.NUMBER_OF_Brackets -1)
		        return false;

	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		         return oStrCalc.AddBrackets(eBracket);

	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.AddBrackets(eBracket);

	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.AddBrackets(eBracket);

	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.AddBrackets(eBracket);

	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.AddBrackets(eBracket);

	        default:
		        break;
	        }
	        return false;
        }

        public bool AddConstants(ConstantType eConstant)
        {
	        eCalcState =CalcStateType.CALCSTATE_Normal;

	        if(eConstant==ConstantType.CONSTANT_Last_Ans)
		        return AddString(szLastAnswer);
	        if(eConstant==ConstantType.CONSTANT_LastX)//Quick Addon for the RPN last X
	        {
		        if(eCalcType!=CalcOperationType.CALC_RPN)
			        return false;
                string szTemp = "";
		        oRPNCalc.GetLastX(ref szTemp);
		        return AddString(szTemp);
	        }
	
	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        return oStrCalc.AddConstants(eConstant);

	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.AddConstants(eConstant);

	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.AddConstants(eConstant);

	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.AddConstants(eConstant);

	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.AddConstants(eConstant);

	        default:
		        break;
	        }
	        return false;
        }

        public bool AddStates(CalcStateType eState)
        {
	        switch(eState) 
	        {
	        case CalcStateType.CALCSTATE_Normal:
                    goto case CalcStateType.CALCSTATE_Compute;
	        case CalcStateType.CALCSTATE_MemoryStore:
                    goto case CalcStateType.CALCSTATE_Compute;
	        case CalcStateType.CALCSTATE_MemoryRecall:
                    goto case CalcStateType.CALCSTATE_Compute;
	        case CalcStateType.CALCSTATE_Recall:
                    goto case CalcStateType.CALCSTATE_Compute;
	        case CalcStateType.CALCSTATE_Compute:
	        {
		        if(eCalcState==eState)
			        eCalcState=CalcStateType.CALCSTATE_Normal;
		        else
			        eCalcState=eState;
		        break;
	        }
	        case CalcStateType.CALCSTATE_2ndF:
	        {
		        if(eCalcState==CalcStateType.CALCSTATE_2ndF)
			        eCalcState=CalcStateType.CALCSTATE_Normal;
		        else if(eCalcState==CalcStateType.CALCSTATE_Hyp)
			        eCalcState=CalcStateType.CALCSTATE_2ndF_Hyp;
		        else if(eCalcState==CalcStateType.CALCSTATE_2ndF_Hyp)
			        eCalcState=CalcStateType.CALCSTATE_Hyp;
		        else
			        eCalcState=CalcStateType.CALCSTATE_2ndF;
		        break;
	        }
	        case CalcStateType.CALCSTATE_Hyp:
	        {
		        if(eCalcState==CalcStateType.CALCSTATE_2ndF)
			        eCalcState=CalcStateType.CALCSTATE_2ndF_Hyp;
		        else if(eCalcState==CalcStateType.CALCSTATE_Hyp)
			        eCalcState=CalcStateType.CALCSTATE_Normal;
		        else if(eCalcState==CalcStateType.CALCSTATE_2ndF_Hyp)
			        eCalcState=CalcStateType.CALCSTATE_2ndF;
		        else
			        eCalcState=CalcStateType.CALCSTATE_Hyp;
		        break;
	        }
	        default:
		        eCalcState=CalcStateType.CALCSTATE_Normal;
		        break;
	        }
	        return true;
        }

        public bool Reset()
        {
	        //let get everything back to normal

	        //base
	        SetCalcBase(10);
	        SetBaseBits(32);

	        //2nd and Hyp
	        eCalcState =CalcStateType.CALCSTATE_Normal;

            SetShowDMS(false);
            SetShowBCD(false);

	        //Clear everything (so do it twice)
	        AddClear(ClearType.CLEAR_ClearAll);
	        AddClear(ClearType.CLEAR_ClearAll);
	
	        //set calc mode to Algebraic
	        SetCalcMode(CalcOperationType.CALC_OrderOfOpps);

	        //set notation to normal
	        SetCalcDisplay(DisplayType.DISPLAY_Float);

	        //clear memory
	        AddMemory(MemoryType.MEMORY_ClearMem1);
	        AddMemory(MemoryType.MEMORY_ClearMem2);
	        AddMemory(MemoryType.MEMORY_ClearMem3);
	        AddMemory(MemoryType.MEMORY_ClearMem4);
	        AddMemory(MemoryType.MEMORY_ClearMem5);
	        AddMemory(MemoryType.MEMORY_ClearMem6);
	        AddMemory(MemoryType.MEMORY_ClearMem7);
	        AddMemory(MemoryType.MEMORY_ClearMem8);

	        //DRG
	        return true;
        }

        public bool AddClear(ClearType eClear)
        {
	        if(eClear!=ClearType.CLEAR_BackSpace)
		        eCalcState =CalcStateType.CALCSTATE_Normal;

	        if(eClear == ClearType.CLEAR_Reset)
		        return Reset();

	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        return oStrCalc.AddClear(eClear);

	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.AddClear(eClear);

	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.AddClear(eClear);

	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.AddClear(eClear);

	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.AddClear(eClear);

	        default:
		        break;
	        }
	        return false;
        }

        public DRGStateType GetDRGState()
        {
	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        return oStrCalc.GetDRGState();
		        
	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.GetDRGState();
		        
	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.GetDRGState();
		        
	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.GetDRGState();

	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.GetDRGState();
		        
	        default:
		        break;
	        }
	        return (DRGStateType)0;
        }

        public void SetDRGState(DRGStateType eState)
        {
            switch(eCalcType) 
            {
            case CalcOperationType.CALC_String:
                oStrCalc.SetDRGState(eState);
                return;
            case CalcOperationType.CALC_OrderOfOpps:
                oOOOCalc.SetDRGState(eState);
                return;
            case CalcOperationType.CALC_DAL:
                oDALCalc.SetDRGState(eState);
                return;
            case CalcOperationType.CALC_Chain:
                oChainCalc.SetDRGState(eState);
                return;
            case CalcOperationType.CALC_RPN:
                oRPNCalc.SetDRGState(eState);
                return;
            default:
                break;
            }
        }

        public bool GetAnswer(ref string szFull, bool bFormat, bool bUseRegional)
        {
            string szValue = "";
            string szExp = "";

            return GetAnswer(ref szFull, ref szValue, ref szExp, bFormat, bUseRegional);

        }

        public bool GetAnswer(ref string szFull, ref string szValue, ref string szExp, bool bFormat, bool bUseRegional)
        {
	        //Temp Variable
	        string szTemp;
            szTemp = "";

	        //Get the answer back from the appropriate calc class
	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		         oStrCalc.GetAnswer(ref szTemp);
		        break;
	        case CalcOperationType.CALC_OrderOfOpps:
		        oOOOCalc.GetAnswer(ref szTemp);
		        break;
	        case CalcOperationType.CALC_DAL:
		        oDALCalc.GetAnswer(ref szTemp);
		        break;
	        case CalcOperationType.CALC_Chain:
		        oChainCalc.GetAnswer(ref szTemp);
		        break;
	        case CalcOperationType.CALC_RPN:
		        oRPNCalc.GetAnswer(ref szTemp);
		        break;
	        default:
		        break;
	        }

            DisplayType eTemp = oDisplay.GetDisplayType();

            if(GetShowDMS())
                oDisplay.SetDisplayType(DisplayType.DISPLAY_DMS);
        //    else if(GetShowBCD())
        //        oDisplay.SetDisplayType(DisplayType.DISPLAY_BCD);

            if(GetShowBCD() == false)
                FormatValue(ref szTemp, ref szFull, ref szValue, ref szExp, bFormat, bUseRegional);
            else
                FormatValue(ref szTemp, ref szFull, ref szValue, ref szExp, false, false);

            oDisplay.SetDisplayType(eTemp);

            if (szFull == null)
                szFull = szValue;//this should never happen ...

            return true;
        }

        bool FormatValue(ref string szNumber, ref string szFull, ref string szValue, ref string szExp, bool bFormat, bool bUseRegional)
        {
            if(szNumber == null)
                return false;

        //////////////////////////////////////////////////////////////////////////////////
	        //Format non base 10 numbers
	        if(GetCalcBase()!=10 &&
		        bFormat)
	        {
		        oDisplay.NumberFormatNonBase10(szNumber, ref szFull, ref szValue, ref szExp, CalcEngine.MAX_SIG_FIGS_NON_10); // iDisplayCharacters);
	        }

        //////////////////////////////////////////////////////////////////////////////////
	        //Check if we're supposed to format the answer
	        else if(bFormat)
	        {
		        oDisplay.NumberFormat(szNumber, ref szFull, ref szValue, ref szExp, bUseRegional, iDisplayCharacters);
		        //mild hack...I'll get this all nice and tidy into the format display class later
		        if(GetUseComma())
		        {
                    szFull.Replace(("."), (","));
                    szValue.Replace(("."), (","));
    	        }
	        }
	
        //////////////////////////////////////////////////////////////////////////////////////	
	        //No Formatting
	        else
	        {
		        oDisplay.NumberNoFormat(szNumber, ref szFull, ref szValue, ref szExp);
	        }
	        return true;

        }

        public bool GetEquation(ref string szEquation)
        {
	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        oStrCalc.GetEquation(ref szEquation);
		        break;
	        case CalcOperationType.CALC_OrderOfOpps:
		        oOOOCalc.GetEquation(ref szEquation);
		        break;
	        case CalcOperationType.CALC_DAL:
		        oDALCalc.GetEquation(ref szEquation);
		        break;
	        case CalcOperationType.CALC_Chain:
		        oChainCalc.GetEquation(ref szEquation);
		        break;
	        case CalcOperationType.CALC_RPN:
		        oRPNCalc.GetEquation(ref szEquation);
		        break;
	        default:
		        break;
	        }
	        //mild hack...I'll get this all nice and tidy into the format display class later
	        if(GetUseComma())
		        szEquation.Replace(".", ",");

	        return true;
        }

        public bool SetCalcBase(int iBase)
        {
            bool bRet = false;

	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        bRet = oStrCalc.SetCalcBase(iBase);
                break;
	        case CalcOperationType.CALC_OrderOfOpps:
		        bRet = oOOOCalc.SetCalcBase(iBase);
                break;
	        case CalcOperationType.CALC_DAL:
		        bRet = oDALCalc.SetCalcBase(iBase);
                break;
	        case CalcOperationType.CALC_Chain:
		        bRet = oChainCalc.SetCalcBase(iBase);
                break;
	        case CalcOperationType.CALC_RPN:
		        bRet = oRPNCalc.SetCalcBase(iBase);
                break;
	        default:
		        break;
	        }

            //if(bRet)
            //    DisplayUpdate(bRet, -1, 0);

	        return bRet;
        }

        public int GetBaseBits()
        {
	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        return oStrCalc.GetBaseBits();
		        
	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.GetBaseBits();
		        
	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.GetBaseBits();
		        
	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.GetBaseBits();
		        
	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.GetBaseBits();
		        
	        default:
		        break;
	        }
	        return 0;
        }


        public bool SetBaseBits(int iBits)
        {
	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        return oStrCalc.SetBaseBits(iBits);
	
	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.SetBaseBits(iBits);
		
	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.SetBaseBits(iBits);
		  
	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.SetBaseBits(iBits);
	
	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.SetBaseBits(iBits);
	
	        default:
		        break;
	        }
	        return false;
        }

        public int GetCalcBase()
        {
	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        return oStrCalc.GetCalcBase();
		        
	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.GetCalcBase();
		        
	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.GetCalcBase();
		        
	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.GetCalcBase();
		       
	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.GetCalcBase();
		        
	        default:
		        break;
	        }
	        return 10;
        }

        //sets the calc mode, ie DAL, Chain, etc
        public bool SetCalcMode(CalcOperationType eCalcOpp)
        {
	        eCalcType=eCalcOpp;
	        return true;
        }

        bool AddClipBoard(ClipboardType eClipBoard)
        {
	    /*    eCalcState =CalcStateType.CALCSTATE_Normal;

	        string szTemp;
	        szTemp = null;
	
	        switch(eClipBoard) 
	        {
	        case CLIPBOARD_CopyEq:
		        if(eCalcType==CalcOperationType.CALC_String)
		        {
			        GetEquation(szTemp);
			        return CalcUtil.SendTextToClipboard(szTemp, m_hWnd);
		        }
		        break;
	        case CLIPBOARD_Copy:
		        GetAnswer(szTemp, null, null, false, false);
		        return CalcUtil.SendTextToClipboard(szTemp, m_hWnd);
		        break;
	        case CLIPBOARD_Paste:
		        CalcUtil.GetTextFromClipboard(szTemp);
		        return AddString(szTemp);
		        break;
	        default:
		        break;
	        }*/
	        return false;
        }

        public bool AddString(string szString)
        {
	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        oStrCalc.AddString(szString);
                break;        
	        case CalcOperationType.CALC_Chain:
		        oChainCalc.AddString(szString);
                break;        
	        case CalcOperationType.CALC_DAL:
		        oDALCalc.AddString(szString);
                break;        
	        case CalcOperationType.CALC_OrderOfOpps:
		        oOOOCalc.AddString(szString);
                break;	        
	        case CalcOperationType.CALC_RPN:
		        oRPNCalc.AddString(szString);
                break;
	        default:
                return false;
	        }
            DisplayUpdate(true, -1, -1);
	        return true;
        }

        bool AddMemory(MemoryType eMemory)
        {
        //	eCalcState =CalcStateType.CALCSTATE_Normal;

	        if((int)eMemory<0 ||
		        (int)eMemory>= CalcGlobals.NUMBER_OF_Memory)
		        return false;

            string szTemp = "";
	        int iIndex;

	        switch(eMemory) 
	        {
	        case MemoryType.MEMORY_ClearMem1:
                    szMemory[0] = "";
		        break;
	        case MemoryType.MEMORY_Plus1:
		        if(GetCalcBase()==10)
			        GetAnswer(ref szTemp, false, false);
		        else
			        GetAnswer(ref szTemp, true, false);
		        return PlusMemory(ref szTemp, 0);
			        
	        case MemoryType.MEMORY_RecallMem1:
	        case MemoryType.MEMORY_RecallMem2:
	        case MemoryType.MEMORY_RecallMem3:
	        case MemoryType.MEMORY_RecallMem4:
	        case MemoryType.MEMORY_RecallMem5:
	        case MemoryType.MEMORY_RecallMem6:
	        case MemoryType.MEMORY_RecallMem7:
	        case MemoryType.MEMORY_RecallMem8:
                iIndex = (int)eMemory - (int)MemoryType.MEMORY_RecallMem1;
		        GetMemory(ref szTemp, iIndex);
		        return AddString(szTemp);
		        
	        case MemoryType.MEMORY_StoreMem1:
	        case MemoryType.MEMORY_StoreMem2:
	        case MemoryType.MEMORY_StoreMem3:
	        case MemoryType.MEMORY_StoreMem4:
	        case MemoryType.MEMORY_StoreMem5:
	        case MemoryType.MEMORY_StoreMem6:
	        case MemoryType.MEMORY_StoreMem7:
	        case MemoryType.MEMORY_StoreMem8:
                iIndex = (int)eMemory - (int)MemoryType.MEMORY_StoreMem1;
		        GetAnswer(ref szTemp, false, false);
		        return StoreMemory(szTemp, iIndex);
		        
	        case MemoryType.MEMORY_1:
	        case MemoryType.MEMORY_2:
	        case MemoryType.MEMORY_3:
	        case MemoryType.MEMORY_4:
	        case MemoryType.MEMORY_5:
	        case MemoryType.MEMORY_6:
	        case MemoryType.MEMORY_7:
	        case MemoryType.MEMORY_8:
                iIndex = (int)eMemory - (int)MemoryType.MEMORY_1;
		        switch(eCalcState) 
		        {
		        case CalcStateType.CALCSTATE_Normal:
			        GetAnswer(ref szTemp, false, false);
			        //mem+
			        return PlusMemory(ref szTemp, iIndex);
			        
		        case CalcStateType.CALCSTATE_2ndF:
			        GetAnswer(ref szTemp, false, false);
			        //mem-
			        return MinusMemory(ref szTemp, iIndex);
			        
		        case CalcStateType.CALCSTATE_MemoryStore:
			        GetAnswer(ref szTemp, false, false);
			        return StoreMemory(szTemp, iIndex);
			        
		        case CalcStateType.CALCSTATE_MemoryRecall:
			        GetMemory(ref szTemp, iIndex);
			        return AddString(szTemp);
			        
		        default:
			        break;
		        }
                break;
	        case MemoryType.MEMORY_PLUS:
		        GetAnswer(ref szTemp, false, false);
		        return PlusMemory(ref szTemp, 0);
		        
	        case MemoryType.MEMORY_MINUS:
		        GetAnswer(ref szTemp, false, false);
		        return MinusMemory(ref szTemp, 0);
	        
	        }
            return false;
        }

        bool ConvertString(ref string szString, ref string szResult, int iFrom, int iTo)
        {
	        if(szString.Length<1)
		        return false;
		        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        return oStrCalc.ConvertString(szString, ref szResult, iFrom, iTo);
		        
	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.ConvertString(szString, ref szResult, iFrom, iTo);
		        
	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.ConvertString(szString, ref szResult, iFrom, iTo);
		        
	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.ConvertString(szString, ref szResult, iFrom, iTo);
		        
	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.ConvertString(szString, ref szResult, iFrom, iTo);
		        
	        default:
		        break;
	        }
	        return false;
        }

        bool StoreMemory(string szStore, int iIndex)
        {
	        eCalcState=CalcStateType.CALCSTATE_Normal;
	        string szTemp;
            szTemp = "";
	        //store blank as 0
	        if(szStore.Length==0)
		        szStore = CalcGlobals.szNumbers[(int)NumberType.NUM_0];

	        if(!ConvertString(ref szStore, ref szTemp, GetCalcBase(), 10))
		        return false;
	        switch(eCalcType) 
		        {
		        case CalcOperationType.CALC_String:
			        oStrCalc.SetLastInput(InputType.INPUT_Constants);
			        break;
		        case CalcOperationType.CALC_Chain:
			        oChainCalc.SetLastInput(InputType.INPUT_Constants);
			        break;
		        case CalcOperationType.CALC_DAL:
			        oDALCalc.SetLastInput(InputType.INPUT_Constants);
			        break;
		        case CalcOperationType.CALC_OrderOfOpps:
			        oOOOCalc.SetLastInput(InputType.INPUT_Constants);
			        break;
		        case CalcOperationType.CALC_RPN:
			        oRPNCalc.SetLastInput(InputType.INPUT_Constants);
                    break;
		        default:
			        break;
		        }
	        szMemory[iIndex] = szTemp;
            return true;
            
        }

        //Memory Index Starts at 0 not 1
        bool GetMemory(ref string szRecall, int iIndex)
        {
	        eCalcState=CalcStateType.CALCSTATE_Normal;
	        string szTemp;
            szRecall = "";
	        szTemp = (szMemory[iIndex]);
	        if(szTemp == null || szTemp.Length<1)
            {
                szRecall = ("0");
                return true;
                
            }
	        return ConvertString(ref szTemp, ref szRecall, 10, GetCalcBase());
        }

        bool PlusMemory(ref string szPlus, int iIndex)
        {
	        eCalcState=CalcStateType.CALCSTATE_Normal;
            string szTemp = "";
	        GetMemory(ref szTemp, iIndex);
	        if(szTemp.Length<1)
		        szTemp =  ("0");
	        if(szPlus.Length>0)
		        szTemp +=  ("+");
	        szTemp += szPlus;
	        ConvertString(ref szTemp, ref szTemp, GetCalcBase(), 10);

	        switch(eCalcType) 
		        {
		        case CalcOperationType.CALC_String:
			        oStrCalc.SetLastInput(InputType.INPUT_Constants);
			        break;
		        case CalcOperationType.CALC_Chain:
			        oChainCalc.SetLastInput(InputType.INPUT_Constants);
			        break;
		        case CalcOperationType.CALC_DAL:
			        oDALCalc.SetLastInput(InputType.INPUT_Constants);
			        break;
		        case CalcOperationType.CALC_OrderOfOpps:
			        oOOOCalc.SetLastInput(InputType.INPUT_Constants);
			        break;
		        case CalcOperationType.CALC_RPN:
			        oRPNCalc.SetLastInput(InputType.INPUT_Constants);
                    break;
		        default:
			        break;
		        }
            szMemory[iIndex] = szTemp;
            return true;
            
        }

        bool MinusMemory(ref string szMinus, int iIndex)
        {
	        eCalcState=CalcStateType.CALCSTATE_Normal;
            string szTemp = "";

	        GetMemory(ref szTemp, iIndex);
	        if(szTemp.Length<1)
		        szTemp = ("0");
	        szTemp +=  ("-");
	        szTemp +=  szMinus;
	        ConvertString(ref szTemp, ref szTemp, 10, 10);
	        szMemory[iIndex] = szTemp;
            return true;
            
        }

        public bool IsMemoryFilled(int iMem)
        {
            if (iMem < 0 || iMem >= (CalcGlobals.NUMBER_OF_MemoryBanks))
                return false;
            else if (szMemory[iMem] == null || szMemory[iMem].Length == 0)
                return false;
            else if (szMemory[iMem][0] == ('0') &&
                szMemory[iMem].Length == 1)
                return false;
            else if (szMemory[iMem].Length > 0)
                return true;
            else
                return false;
        }



        public bool AddBase(int iBase)
        {
	        eCalcState =CalcStateType.CALCSTATE_Normal;

            if(iBase == GetCalcBase() && GetShowBCD() == false)
                return true;

            if(iBase == CalcGlobals.BASE_BCD) 
            {   //hack
                SetShowBCD(true);
                iBase = 10;//force to decimal
            }
            else
            {   //otherwise no BCD yo
                SetShowBCD(false);
            }
    
	        //Convert Onscreen....Clear....move on
            string szTemp = "";
	        GetAnswer(ref szTemp, false, false);
	        AddClear(ClearType.CLEAR_Clear);
	        ConvertString(ref szTemp, ref szTemp, GetCalcBase(), iBase);
	        
            //no need to set the base
            if (GetShowBCD())
            {
                SetCalcBase(10);
                return AddString(szTemp);
            }


	        SetCalcBase(iBase);
            return AddString(szTemp);
        }

        public DisplayType GetCalcDisplay()
        {
	        return oDisplay.GetDisplayType();
        }

        public bool SetCalcDisplay(DisplayType eDisplay)
        {
	        return oDisplay.SetDisplayType(eDisplay);
        }

        bool IsScrollUpAvalible()
        {
	        if(eCalcType==CalcOperationType.CALC_String)
		        return oStrCalc.IsScrollUpAvailable();
	        else 
		        return false;
        }

        bool IsScrollDownAvalible()
        {
	        if(eCalcType==CalcOperationType.CALC_String)
		        return oStrCalc.IsScrollDownAvailable();
	        else 
		        return false;
        }

        bool OnScrollUp()
        {
            if(eCalcType==CalcOperationType.CALC_String)
                return oStrCalc.OnScrollUp();
            else 
                return false;
        }

        bool OnScrollDown()
        {
            if(eCalcType==CalcOperationType.CALC_String)
                return oStrCalc.OnScrollDown();
            else 
                return false;
        }

        bool AddRPN(RPNManipulationType eRPN)
        {
	        if(eCalcType!=CalcOperationType.CALC_RPN)
		        return false;
	        else
		        return oRPNCalc.AddRPN(eRPN);
        }

        public bool CalcButtonPress(int iButtonType, int iButtonId)
        {
	        bool bReturn=false;

            switch((InputType)iButtonType)
	        {
	        case InputType.INPUT_Number:
		        bReturn=AddNumber((NumberType)iButtonId);
		        break;
	        case InputType.INPUT_Operator:
		        bReturn=AddOperator((OperatorType)iButtonId);
		        break;
	        case InputType.INPUT_Function:
		        bReturn=AddFunction((FunctionType)iButtonId);
		        break;
	        case InputType.INPUT_Memory:
		        bReturn=AddMemory((MemoryType)iButtonId);
		        break;
	        case InputType.INPUT_Constants:
		        bReturn=AddConstants((ConstantType)iButtonId);
		        break;
	        case InputType.INPUT_Bracket:
		        bReturn=AddBrackets((BracketType)iButtonId);
		        break;
	        case InputType.INPUT_Equals:
		        bReturn=AddEquals((EqualsType)iButtonId);
		        break;
	        case InputType.INPUT_CalcState:
		        bReturn=AddStates((CalcStateType)iButtonId);
		        break;
	        case InputType.INPUT_Clear:
		        bReturn=AddClear((ClearType)iButtonId);
		        break;
	        case InputType.INPUT_Clipboard:
		        bReturn=AddClipBoard((ClipboardType)iButtonId);
		        break;
	        case InputType.INPUT_BaseConversion:
		        bReturn=AddBase((int)iButtonId);
		        break;
	        case InputType.INPUT_RPN:
		        bReturn=AddRPN((RPNManipulationType)iButtonId);
		        break;
	        }

	        if(!bReturn)
	        {
		       iErrorCount++;
		        if(iErrorCount>3)
		        {
                    //bugbug
                    //count fire a message back to the interface ...                    
			        iErrorCount=0;
		        }
	        }
	        else
		       iErrorCount=0;

            //publish
             DisplayUpdate(bReturn, iButtonType, iButtonId);

	        return bReturn;
	
        }

        void OnScreenTap()
        {
	        if(eCalcType!=CalcOperationType.CALC_RPN)
		        return;
	        else
		        oRPNCalc.OnScreenTap();
        }



   /*     void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
        {
	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		         oStrCalc.OnKeyUp(nChar, nRepCnt, nFlags);
		        break;

	        }
        }*/

        public bool GetUseComma()
        {   
           return oDisplay.GetUseComma();
           
        }

        public void SetUseComma(bool bUseComma)
        {
            oDisplay.SetUseComma(bUseComma);
        }


        public void UpdateDisplays(string szConstantAdd, string szEquationChange)
        {
	        if(szConstantAdd == null || szEquationChange == null)
		        return;

	        if(szConstantAdd.Length > 0)
		        AddString(szConstantAdd);

	        if(szEquationChange.Length > 0)
	        {
		        switch(eCalcType) 
		        {
		        case CalcOperationType.CALC_String:
			         oStrCalc.SetEquation(szEquationChange);
			        break;
		        case CalcOperationType.CALC_OrderOfOpps:
			        oOOOCalc.SetEquation(szEquationChange);
			        break;
		        case CalcOperationType.CALC_DAL:
			        oDALCalc.SetEquation(szEquationChange);
			        break;
		        case CalcOperationType.CALC_Chain:
			        oChainCalc.SetEquation(szEquationChange);
			        break;
		        case CalcOperationType.CALC_RPN:
			        oRPNCalc.SetEquation(szEquationChange);
			        break;
		        }	
	        }

            DisplayUpdate(false, -1, -1);
        }

        bool GetUseTape()
        {
	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        return oStrCalc.GetUseTape();

	        case CalcOperationType.CALC_OrderOfOpps:
		        return oOOOCalc.GetUseTape();

	        case CalcOperationType.CALC_DAL:
		        return oDALCalc.GetUseTape();

	        case CalcOperationType.CALC_Chain:
		        return oChainCalc.GetUseTape();

	        case CalcOperationType.CALC_RPN:
		        return oRPNCalc.GetUseTape();

	        default:
		        break;
	        }
	        return false;
        }

        void SetUseTape(bool bUseTape)
        {
	        switch(eCalcType) 
	        {
	        case CalcOperationType.CALC_String:
		        oStrCalc.SetUseTape(bUseTape);
		        break;
	        case CalcOperationType.CALC_OrderOfOpps:
		        oOOOCalc.SetUseTape(bUseTape);
		        break;
	        case CalcOperationType.CALC_DAL:
		        oDALCalc.SetUseTape(bUseTape);
		        break;
	        case CalcOperationType.CALC_Chain:
		        oChainCalc.SetUseTape(bUseTape);
		        break;
	        case CalcOperationType.CALC_RPN:
		        oRPNCalc.SetUseTape(bUseTape);
		        break;
	        default:
		        break;
	        }
        }

        InputType GetInputType()
        {
            switch(eCalcType) 
            {
            case CalcOperationType.CALC_String:
                return oStrCalc.GetInputType();

            case CalcOperationType.CALC_OrderOfOpps:
                return oOOOCalc.GetInputType();
 
            case CalcOperationType.CALC_DAL:
                return oDALCalc.GetInputType();
  
            case CalcOperationType.CALC_Chain:
                return oChainCalc.GetInputType();

            case CalcOperationType.CALC_RPN:
                return oRPNCalc.GetInputType();

            default:
                return InputType.INPUT_Other;

            }
        }

        bool GetFullTapePath(ref string szPath)
        {
            return false;

            /* 

            switch(eCalcType) 
            {
            case CalcOperationType.CALC_String:
                return oStrCalc.GetTapeFullPath(szPath);

            case CalcOperationType.CALC_OrderOfOpps:
                return oOOOCalc.GetTapeFullPath(szPath);

            case CalcOperationType.CALC_DAL:
                return oDALCalc.GetTapeFullPath(szPath);

            case CalcOperationType.CALC_Chain:
                return oChainCalc.GetTapeFullPath(szPath);

            case CalcOperationType.CALC_RPN:
                return oRPNCalc.GetTapeFullPath(szPath);

            default:
                return false;
                break;
            }   */
        }

        public bool GetRPNStackEntry(int iEntry, ref string szFull)
        {
            string szValue = "";
            string szExp = "";

            return GetRPNStackEntry(iEntry, ref szFull, ref szValue, ref szExp, true, false);
        }

        public bool GetRPNStackEntry(int iEntry, ref string szFull, ref string szValue, ref string szExp, bool bFormat, bool bUseRegional)
        {
            if(iEntry < 0 || iEntry >= oRPNCalc.GetStackSize())
                return false;

            string szNumber = "";

            oRPNCalc.GetStackEntry(iEntry, ref szNumber);

            return FormatValue(ref szNumber, ref szFull, ref szValue, ref szExp, bFormat, bUseRegional);
        }

        bool GetShowDMS()
        {
            switch(eCalcType) 
            {
            case CalcOperationType.CALC_String:
                return oStrCalc.GetShowDMS();
            
            case CalcOperationType.CALC_OrderOfOpps:
                return oOOOCalc.GetShowDMS();
                
            case CalcOperationType.CALC_DAL:
                return oDALCalc.GetShowDMS();
                
            case CalcOperationType.CALC_Chain:
                return oChainCalc.GetShowDMS();
                
            case CalcOperationType.CALC_RPN:
                return oRPNCalc.GetShowDMS();
                
            default:
                return false;
                  
            }
        }

        void SetShowDMS(bool bDMS)
        {
            switch(eCalcType) 
            {
            case CalcOperationType.CALC_String:
                oStrCalc.SetShowDMS(bDMS);
                break;
            case CalcOperationType.CALC_OrderOfOpps:
                oOOOCalc.SetShowDMS(bDMS);
                break;
            case CalcOperationType.CALC_DAL:
                oDALCalc.SetShowDMS(bDMS);
                break;
            case CalcOperationType.CALC_Chain:
                oChainCalc.SetShowDMS(bDMS);
                break;
            case CalcOperationType.CALC_RPN:
                oRPNCalc.SetShowDMS(bDMS);
                break;
            default:
                break; 
            }
        }

        bool GetShowBCD()
        {
            switch(eCalcType) 
            {
            case CalcOperationType.CALC_String:
                return oStrCalc.GetShowBCD();

            case CalcOperationType.CALC_OrderOfOpps:
                return oOOOCalc.GetShowBCD();

            case CalcOperationType.CALC_DAL:
                return oDALCalc.GetShowBCD();
  
            case CalcOperationType.CALC_Chain:
                return oChainCalc.GetShowBCD();

            case CalcOperationType.CALC_RPN:
                return oRPNCalc.GetShowBCD();

            default:
                return false;

            }
        }

        void SetShowBCD(bool bDMS)
        {
            switch(eCalcType) 
            {
            case CalcOperationType.CALC_String:
                oStrCalc.SetShowBCD(bDMS);
                break;
            case CalcOperationType.CALC_OrderOfOpps:
                oOOOCalc.SetShowBCD(bDMS);
                break;
            case CalcOperationType.CALC_DAL:
                oDALCalc.SetShowBCD(bDMS);
                break;
            case CalcOperationType.CALC_Chain:
                oChainCalc.SetShowBCD(bDMS);
                break;
            case CalcOperationType.CALC_RPN:
                oRPNCalc.SetShowBCD(bDMS);
                break;
            default:
                break; 
            }
        }

 //       public double Calculate(string szEq, ref string szAnswer)
 //       {
 //           return 0.0;
            /*
            //I should really just tunnel down to the core class ....
            SetCalcBase(10);
            oStrCalc.SetEquation(szEq);
            oStrCalc.AddEquals(EqualsType.EQUALS_Equals);
            oStrCalc.GetAnswer(ref szAnswer);
            return Convert.ToDouble(szAnswer);*/
 //       }

        public string GetMemory(int iIndex)
        {
            if (iIndex < 0 || iIndex >= GetNumberOfMemory())
                return null;

            return szMemory[iIndex];
        }


        public void SaveState()
        {
            IssCalcState sState = new IssCalcState();
            sState.eCalcState = eCalcState;
            sState.iDisplayCharacters = iDisplayCharacters;
            sState.iErrorCount = iErrorCount;
            sState.szLastAnswer = szLastAnswer;

            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
                //If user choose to save, create a new file
          /*      using (IsolatedStorageFileStream fs = isf.CreateFile("IssCalc"))
                {
                    //and serialize data
                    XmlSerializer ser = new XmlSerializer(typeof(IssCalcState));
                    ser.Serialize(fs, sState);
                }  */
            }

            oStrCalc.SaveState("String");
            oChainCalc.SaveState("Chain");
            oDALCalc.SaveState("DAL");
            oRPNCalc.SaveState("RPN");
            oOOOCalc.SaveState("OOO");
        }

        public void RestoreState()
        {
            IssCalcState sState = new IssCalcState();

            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
            /*    if (isf.FileExists("IssCalc"))
                {
                    //If user choose to save, create a new file
                    using (IsolatedStorageFileStream fs = isf.OpenFile(("IssCalc"), System.IO.FileMode.Open))
                    {
                        //and serialize data
                        XmlSerializer ser = new XmlSerializer(typeof(IssCalcState));
                        sState = (IssCalcState)ser.Deserialize(fs);
                    }

                    eCalcState = sState.eCalcState;
                    iDisplayCharacters = sState.iDisplayCharacters;
                    iErrorCount = sState.iErrorCount;
                    szLastAnswer = sState.szLastAnswer;
                }  */
            }
                                  
            oStrCalc.RestoreState("String");
            oChainCalc.RestoreState("Chain");
            oDALCalc.RestoreState("DAL");
            oRPNCalc.RestoreState("RPN");
            oOOOCalc.RestoreState("OOO");
        }
    }
}
