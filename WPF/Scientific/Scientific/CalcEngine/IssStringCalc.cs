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
using System.IO;

public enum LastInputType
{
    LASTINPUT_Blank,
    LASTINPUT_Number,
    LASTINPUT_Operator,
    LASTINPUT_Minus,
    LASTINPUT_OpenBrac,
    LASTINPUT_CloseBrac,
    LASTINPUT_Period,
    LASTINPUT_DMS,
    LASTINPUT_Error,
};

namespace CalcPro
{
    public class IssStringCalc
    {
        struct StringState
        {
            public InputType eLastInputType;
            public string szAnswer;
            public string szEquation;
            public string szLastAnswer;
            public FormatAnswerType eFormatAns;
        };

        public IssStringCalc()
        {
            bPlusMinusAsMinus = false;
            eLastInputType = InputType.INPUT_NULL;
            oEqBuffer.ClearBuffer();
        }


        static protected CalcEngine oCalc = new CalcEngine();//same engine instance for all inherited classes
        static protected IssEquationBuffer oEqBuffer = new IssEquationBuffer();
        static protected CalcUtil oUtil = new CalcUtil();

        protected InputType eLastInputType;

        protected string szAnswer = "";
        protected string szEquation = "";
        protected string szLastAnswer = "";

        protected FormatAnswerType eFormatAns;
        protected bool bPlusMinusAsMinus; //will probably never use ....

        public DRGStateType	GetDRGState(){return oCalc.GetDRGState();}
        public void SetDRGState(DRGStateType eState){oCalc.SetDRGState(eState);}

        public int GetBaseBits() { return oCalc.GetBaseBits(); }

        public void SetLastInput(InputType input){eLastInputType=input;}


        virtual public bool GetAnswer(ref string szNewAnswer)
        {
            szNewAnswer = szAnswer;
            return true;
            
        }

        virtual public bool GetEquation(ref string szEq)
        {
	        szEq = szEquation;
            return true;
        }


        virtual public bool AddNumber(NumberType eNumber)
        {

            string szTemp = "";
	        
            GetLastNumber(szEquation, ref szTemp);

            if(eLastInputType == InputType.INPUT_Equals)
	        {
                if (eNumber == NumberType.NUM_DMS)
                {
                    oCalc.SetShowDMS(true);
                    return true;
                }
                szEquation = "";
		        
	        }

            if (szTemp.Length == 0 &&
                eNumber == NumberType.NUM_Period)
            {
                szEquation = szEquation + CalcGlobals.szNumbers[(int)NumberType.NUM_0];
            }

            if(CheckInput(eNumber)==false)
		        return false;

            int iLocation = szTemp.IndexOf("°");

            //some DMS work to do 
            if (eNumber == NumberType.NUM_DMS)
            {
                //so we've been allowed to enter it so we must be able to add the degrees or minutes symbol
                if(iLocation != -1)
                {   //we need to add the minutes symbol
                    int iLen = szTemp.Length;

                    //but first check if we need to add zeros
                    for (int i = 0; i < iLocation + 3 - iLen; i++)
                    {
                        szEquation = szEquation + CalcGlobals.szNumbers[(int)NumberType.NUM_0];
                    }

                    //then add the minutes symbol
                    szEquation = szEquation + "\x2032";
                }
                else //we need to add the degrees symbol
                {   //so just go ahead and do it
                    szEquation = szEquation + "°";
                }

                eLastInputType = InputType.INPUT_Number;
                return true;
            }
            else if(iLocation != -1)
            {   //so this means we have a DMS number started so handle input accordingly
                //only want 0-9
                if(eNumber > NumberType.NUM_9)
                    return false; 
                //now lets figure out where in the number we are
                int i2ndLoc = szTemp.IndexOf("\x2032");

                if(i2ndLoc != -1)
                {   //we've already added the minutes symbol 
                    //do we need to add a period?
                    if (i2ndLoc + 3 == szTemp.Length)
                        szEquation = szEquation + ".";
                        
                }

                else if(iLocation + 3 == szTemp.Length)
                {
                    szEquation = szEquation + ("\x2032");
                }

                szEquation = szEquation + CalcGlobals.szNumbers[(int)eNumber];
                //and we're done
                return true;
            }
	
            //don't add a bunch of 0s
	        if(szTemp.Length==1 &&
		        szTemp[0] == CalcGlobals.szNumbers[(int)NumberType.NUM_0][0] &&
		        eNumber != NumberType.NUM_Period)
            {
		        szEquation = szEquation.Substring(0, szEquation.Length - 1);
            }

	        eLastInputType = InputType.INPUT_Number;
	        szEquation = szEquation + CalcGlobals.szNumbers[(int)eNumber];
            return true;
        }


        virtual public bool AddFunction(FunctionType eFunction)
        {
	        if(eLastInputType==InputType.INPUT_Equals)
	        {
                szEquation = "";
	        }

	
	        InputType eLast = eLastInputType;
	        eLastInputType = InputType.INPUT_Function;
	        //Special Case Functions
	        switch(eFunction) 
	        {
	        case FunctionType.FUNCTION_DRG://change the value and the state
		        oCalc.ConvertDRG(ref szAnswer, ref szAnswer);
                    goto case FunctionType.FUNCTION_DRGN;
                    
	        case FunctionType.FUNCTION_DRGN:
		        return oCalc.SetDRGState(DRGStateType.DRG_Next);
		        
	        case FunctionType.FUNCTION_PlusMinus:
		        if(bPlusMinusAsMinus)
			        return AddOperator(OperatorType.OPP_Minus);
                string szTemp = "";
		        int iTemp;
		        
		        GetLastNumber(szEquation, ref szTemp);
		        iTemp=szTemp.Length;

		        szEquation = szEquation.Substring(0, szEquation.Length - iTemp);
		        
                if(szEquation.Length < 1 || szEquation[szEquation.Length - 1] != '-')
                    szEquation = szEquation + CalcGlobals.szOperators[(int)OperatorType.OPP_Minus];
		        else
		        {
			        //we have a negative symbol
			        
                    szEquation = szEquation.Substring(0, szEquation.Length - 1);
			        if(CheckInput(OperatorType.OPP_Plus)) //yes, this is correct 
			        {
				        szEquation = szEquation + CalcGlobals.szOperators[(int)OperatorType.OPP_Minus]; //and this
				        szEquation = szEquation + CalcGlobals.szOperators[(int)OperatorType.OPP_Minus]; //and this
			        }
		        }
		        szEquation = szEquation + szTemp;
		        return true;
		        
	        //inverse
	        //square
	        //cube
	        //special cases that are really numbers and operators
	        case FunctionType.FUNCTION_Squared:
		        if(eLast==InputType.INPUT_Equals)
		        {
			        szEquation = szLastAnswer;
		        }
		        if(CheckInput(OperatorType.OPP_EXP)==false)
			        return false;
		        szEquation = szEquation + ("²");
                return true;
		        
	        case FunctionType.FUNCTION_Cubed:
		        if(eLast==InputType.INPUT_Equals)
		        {
			        szEquation = szLastAnswer;
		        }
		        if(CheckInput(OperatorType.OPP_EXP)==false)
			        return false;
		        szEquation = szEquation + ("³");
		        return true;

	        case FunctionType.FUNCTION_Inverse:
		        if(eLast==InputType.INPUT_Equals)
		        {
			        szEquation = szLastAnswer;
		        }
		        if(CheckInput(OperatorType.OPP_EXP)==false)
			        return false;
		        szEquation = szEquation + ("-¹");
		        return true;
	        default:
		        if(CheckInput(eFunction)==false)
			        return false;
		        break;
	        }
	

	        szEquation = szEquation + CalcGlobals.szFunctionsFull[(int)eFunction];
            return true;
        }



        virtual public bool AddOperator(OperatorType eOperator)
        {
	        if(eLastInputType==InputType.INPUT_Equals)
	        {
		        szEquation = szAnswer;
	        }


	        if(CheckInput(eOperator)==false)
		        return false;
	        eLastInputType = InputType.INPUT_Operator;
	        szEquation = szEquation + CalcGlobals.szOperators[(int)eOperator];
            return true;
        }

        virtual public bool AddEquals(EqualsType eEquals)
        {
	        if(eEquals != EqualsType.EQUALS_Equals)
		        return false;
	        eLastInputType = InputType.INPUT_Equals;

            CalcUtil.FillOpenBrackets(ref szEquation);
	        oCalc.Calculate(szEquation, ref szAnswer);
	        
            szLastAnswer = szAnswer;

	        oEqBuffer.AddToEquation(szEquation, ref szAnswer);
	        return true;
        }

        virtual public bool AddBrackets(BracketType eBracket)
        {
	        if(eLastInputType==InputType.INPUT_Equals)
	        {
                szEquation = "";
	        }

            if (eBracket != BracketType.BRACKET_Open &&
                eBracket != BracketType.BRACKET_Close)
		        return false;

	        eLastInputType = InputType.INPUT_Bracket;

            if (CheckInput(eBracket))
            {
                szEquation = szEquation + CalcGlobals.szBrackets[(int)eBracket];
                return true;
            }
	        return false;
        }

        virtual public bool AddConstants(ConstantType eConstant)
        {
	        if(eLastInputType==InputType.INPUT_Equals)
	        {
                szEquation = "";
	        }

            if (eConstant == ConstantType.CONSTANT_Last_Ans)
	        {
		        eLastInputType = InputType.INPUT_Constants;
		        szEquation = szEquation + szLastAnswer;
                return true;
	        }
	        else if((int)eConstant < 0 ||
		        (int)eConstant > CalcGlobals.NUMBER_OF_Constants-1)
		        return false;
	        else if(CheckInput(eConstant))
	        {
		        eLastInputType = InputType.INPUT_Constants;
		        szEquation = szEquation + CalcGlobals.szConstants[(int)eConstant];
                return true;
	        }
	        return false;
        }

        virtual public bool AddClear(ClearType eClear)
        {
	        switch(eClear) 
	        {
                case ClearType.CLEAR_CE://all we can really do with string
                    szEquation = "";
		        eLastInputType = InputType.INPUT_Clear;
		        break;
            case ClearType.CLEAR_ClearAll:
                goto case ClearType.CLEAR_Clear;
            case ClearType.CLEAR_Clear:
		        if(eLastInputType==InputType.INPUT_Clear)
			        oEqBuffer.ClearBuffer();
		        eLastInputType = InputType.INPUT_Clear;
		        Clear();
		        return true;
            case ClearType.CLEAR_BackSpace:
                if(szEquation != null && szEquation.Length > 0)
                    szEquation = szEquation.Remove(szEquation.Length - 1);
		        break;
	        default:
		        break;
	        }
	        return true;
        }

        bool CheckInput(NumberType eNumber)
        {
	        //check input for valid base
            if(eNumber != NumberType.NUM_Period && eNumber != NumberType.NUM_DMS && (int)eNumber > GetCalcBase() - 1)
        //	if((int)eNumber>GetCalcBase()-1 && (eNumber!=NUM_Period && e
		        return false;
	        else if(GetCalcBase()!=10 && eNumber==NumberType.NUM_Period)
		        return false;

	        //get the last complete number entered
            string szTemp = "";
	        GetLastNumber(szEquation, ref szTemp);

	        //check for the period
	        if(eNumber == NumberType.NUM_Period &&
                szTemp.IndexOf(".") != -1)
            {
		        return false;
            }
 
            //check for DMS stuff
            if(eNumber==NumberType.NUM_DMS &&
                szTemp.IndexOf(".") != -1)
                return false;

            //we'll let the input code handle the counts and logic etc ... this should work for now
            if(eNumber == NumberType.NUM_DMS && 
                szTemp.IndexOf("\x2032") != -1)
                return false;

	        //check for a bigass number
	        if(szTemp.Length > 16)
		        return false;
	        if(szEquation.Length > 128)
		        return false;
		
	        LastInputType eInput = GetLastInputType(szEquation);

	
	        switch(eInput) 
	        {
	        case LastInputType.LASTINPUT_Minus:
	        case LastInputType.LASTINPUT_Blank:
	        case LastInputType.LASTINPUT_OpenBrac:
	        case LastInputType.LASTINPUT_Operator:
                if (eNumber == NumberType.NUM_Period || eNumber == NumberType.NUM_DMS)
			        szEquation = szEquation + ("0");
                goto case LastInputType.LASTINPUT_Period;
	        case LastInputType.LASTINPUT_Period:
                if (eNumber == NumberType.NUM_Period)
			        return false;
                goto case LastInputType.LASTINPUT_Number;
            case LastInputType.LASTINPUT_DMS: //I think this is right ... 
                goto case LastInputType.LASTINPUT_Number;
	        case LastInputType.LASTINPUT_Number:
		        return true;
	        case LastInputType.LASTINPUT_CloseBrac:
                return false;
	        case LastInputType.LASTINPUT_Error:
                return false;
	        default:
		        return false;
	        }

        }

        bool CheckInput(FunctionType eFunction)
        {
	        //check input for valid base
	        if(GetCalcBase()!=10)
		        return false;

	        LastInputType eInput = GetLastInputType(szEquation);

	        switch(eInput) 
	        {
	        case LastInputType.LASTINPUT_Operator:
                return true;
	        case LastInputType.LASTINPUT_Minus:
                return true;
	        case LastInputType.LASTINPUT_OpenBrac:
                return true;
	        case LastInputType.LASTINPUT_Blank:
		        return true;
            case LastInputType.LASTINPUT_DMS:
                return false;
	        case LastInputType.LASTINPUT_Period:
                return false;
	        case LastInputType.LASTINPUT_Number:
                return false;
	        case LastInputType.LASTINPUT_CloseBrac:
                return false;
	        case LastInputType.LASTINPUT_Error:
                return false;
	        default:
		        return false;
	        }
        }	

        bool CheckInput(OperatorType eOperator)
        {
	        LastInputType eInput = GetLastInputType(szEquation);

	        switch(eInput) 
	        {
	        case LastInputType.LASTINPUT_OpenBrac:
                if (eOperator != OperatorType.OPP_Minus) //so we can put negative numbers in brackets
	            {
			        return false;
    	        }
                return true;
	        case LastInputType.LASTINPUT_Minus://quick hack for the ol double minus
                    return true;
	        case LastInputType.LASTINPUT_Number:
                    return true;
	        case LastInputType.LASTINPUT_CloseBrac:
                    return true;
	        case LastInputType.LASTINPUT_Period:
                    return true;
            case LastInputType.LASTINPUT_DMS:
		        return true;
	        case LastInputType.LASTINPUT_Operator:
                if (eOperator == OperatorType.OPP_Minus)//... might be wrong ... but it looks like the c++ code has a bug
			        return true;
                return false;
	        case LastInputType.LASTINPUT_Blank:
                return false;
	        case LastInputType.LASTINPUT_Error:
                return false;
	        default:
		        return false;
	        }
        }

        bool CheckInput(BracketType eBracket)
        {
	        //check input for valid base
	        if(GetCalcBase()!=10)
		        return false;

	        LastInputType eInput = GetLastInputType(szEquation);

	        if(eBracket==BracketType.BRACKET_Open)
	        {
		        switch(eInput) 
		        {
		        case LastInputType.LASTINPUT_Operator:
                     return true;
		        case LastInputType.LASTINPUT_Minus:
                     return true;
		        case LastInputType.LASTINPUT_OpenBrac:
                     return true;
		        case LastInputType.LASTINPUT_Blank:
			        return true;

                case LastInputType.LASTINPUT_DMS:
                    return false;
		        case LastInputType.LASTINPUT_Period:
                    return false;
		        case LastInputType.LASTINPUT_Number:
                    return false;
		        case LastInputType.LASTINPUT_CloseBrac:
                    return false;
		        case LastInputType.LASTINPUT_Error:
                    return false;
		        default:
			        return false;			        
		        }
	        }
            else if (eBracket == BracketType.BRACKET_Close)
            {
                if (CalcUtil.GetOpenBracketCount(szEquation) <= CalcUtil.GetCloseBracketCount(szEquation))
                    return false;
                switch (eInput)
                {
                    case LastInputType.LASTINPUT_DMS:
                        return true;
                    case LastInputType.LASTINPUT_Period:
                        return true;
                    case LastInputType.LASTINPUT_Number:
                        return true;
                    case LastInputType.LASTINPUT_CloseBrac:
                        return true;

                    case LastInputType.LASTINPUT_Blank:
                        return false;
                    case LastInputType.LASTINPUT_OpenBrac:
                        return false;
                    case LastInputType.LASTINPUT_Operator:
                        return false;
                    case LastInputType.LASTINPUT_Minus:
                        return false;
                    case LastInputType.LASTINPUT_Error:
                        return false;
                    default:
                        return false;
                }
            }
            else
            {
                return false;
            }

        }

        bool CheckInput(EqualsType eEquals)
        {
	        return true;
        }

        bool CheckInput(ConstantType eConstant)
        {
	        return CheckInput(BracketType.BRACKET_Open);
        }	

        bool CheckInput(MemoryType eMemory)
        {
	        return true;
        }

        bool CheckInput(CalcStateType eCalcState)
        {
	        return true;
        }

        bool CheckInput(ClearType eClear)
        {
	        return true;
        }

        bool CheckInput(ClipboardType eClipBoard)
        {
	        return CheckInput(BracketType.BRACKET_Open);
        }


        public int GetCalcBase()
        {
	        return oCalc.GetBase();
        }

        public bool SetCalcBase(int iBase)
        {
	        oCalc.SetBase(iBase);
	        if(szAnswer != null && szAnswer.Length>0)
		        AddEquals(EqualsType.EQUALS_Equals);
	        return true;
        }


        public LastInputType GetLastInputType(string szSource)
        {
            string szTemp = "";
            return GetLastInputType(szSource, ref szTemp);
        }

        LastInputType GetLastInputType(string szSource, ref string szResult)
        {
	        char szLastInput;
	        int iLength;

            iLength = szSource.Length;
	        if(iLength==0)
		        return LastInputType.LASTINPUT_Blank;

	        //set our TCHAR
	        szLastInput = szSource[iLength-1];
	
	        //copy the last character into szResult
	        if(szResult!=null)
	        {
                szResult = "" + szLastInput;
	        }
	
	        //Check for numbers (and captital letters)
	        if((szLastInput > 47 && szLastInput < 58) ||//numbers
		        (szLastInput > 64 && szLastInput < 91))
		        return LastInputType.LASTINPUT_Number;

	        //Check for open and close brackets
	        else if(szLastInput==('('))
		        return LastInputType.LASTINPUT_OpenBrac;
	        else if(szLastInput==(')') ||
			        szLastInput==('¹') ||
			        szLastInput==('²') ||
			        szLastInput==('³'))
		        return LastInputType.LASTINPUT_CloseBrac;

	        //Check for period
	        else if(szLastInput==('.'))
		        return LastInputType.LASTINPUT_Period;

            //check for Degrees and minutes
            else if(szLastInput == ('\x2032') || szLastInput == ('°'))
                return LastInputType.LASTINPUT_DMS;

	        else if(szLastInput==('-'))
		        return LastInputType.LASTINPUT_Minus;

	        //Finally....check for operators
	        for(int i=0;i < CalcGlobals.MAX_ERROR_CONV; i++)
	        {
		        if(szLastInput == CalcGlobals.szErrorConvert[i][0])
			        return LastInputType.LASTINPUT_Operator;
	        }
	        return LastInputType.LASTINPUT_Error;
        }

        public virtual bool AddString(string szString)
        {
	        if(eLastInputType==InputType.INPUT_Equals)
	        {
                szEquation = "";
	        }

            if (CheckInput(ConstantType.CONSTANT_Pi) ||
                szEquation.Length == 0)
            {
                szEquation = szEquation + szString;
                return true;
            }
            else
            {
                return false;
            }
        }

        public bool ConvertString(string szString, ref string szResult, int iFrom, int iTo/* =10 */)
        {
	        oCalc.Calculate(szString, ref szResult, iFrom, iTo);
	        return true;
        }

        void Clear()
        {
            szAnswer = "";
            szEquation = "";
        }


        public bool IsScrollUpAvailable()
        {
	        return oEqBuffer.IsUpAvailable();
        }

        public bool IsScrollDownAvailable()
        {
	        return oEqBuffer.IsDownAvailable();
        }

        public bool OnScrollUp()
        {
            if(oEqBuffer.IsUpAvailable())
            {
                return oEqBuffer.GetLastEquation(ref szEquation, ref szAnswer);
            }
            return false;
        }

        public bool OnScrollDown()
        {
            if(oEqBuffer.IsDownAvailable())
            {
                return oEqBuffer.GetNextEquation(szEquation, ref szAnswer);
            }
            return false;
        }


        public void OnKeyUp(uint nChar, uint nRepCnt, uint nFlags)
        {   
	     /*   switch(nChar)
	        {
	        case VK_LEFT:
		        break;
	        case VK_RIGHT:
		        break;
	        case VK_DOWN:
		        if(m_oEqBuffer.GetNextEquation(m_szEquation, m_szAnswer))
			        eLastInputType=InputType.INPUT_Equals;
		        break;
	        case VK_UP:
		        if(m_oEqBuffer.GetLastEquation(m_szEquation, m_szAnswer))
			        eLastInputType=InputType.INPUT_Equals;
		        break;	
	        }*/

        }
        public void GetLastNumber(string szEquation, ref string szNumber)
        {
	        int iCount	=	0;
	        int iLen	=	0;


            szNumber = "";
            
            //now track back to the last input
            if (szEquation == null)
                iLen = 0;
            else
                iLen = szEquation.Length;

	        //if the equation is empty
	        if(iLen==0)
		        return;
            	        
	        int iBase = oCalc.GetBase();

	        bool bNum=true;

	        for(int i=iLen;i>0; i--)
	        {
		        bNum=false; 
		        for(int j=0; j<CalcGlobals.NUMBER_OF_Numbers; j++)
		        {	
			        //E isn't a number if its not HEX
			        if(CalcGlobals.szNumbers[j][0] == ('E') && iBase != 16)
			        //if(szEquation[i-1] == ('E') && iBase != 16)
			        {
				        //just need to not look for it
				        continue;
			        }
			        else if(szEquation.IndexOf(CalcGlobals.szNumbers[j], i-1) == i -1)
                        //(m_oStr->Find(szEquation, CalcGlobals.szNumbers[j], i-1)==i-1)
			        {
				        bNum=true;
				        break;
			        }
		        }
		        if(!bNum)
			        break;
		        else
			        iCount++;
		
	        }
	        
            szNumber = szEquation.Substring(szEquation.Length - iCount, iCount);
        }

        public void SetEquation(string szEq)
        {
            if (szEquation != null)
                szEquation = szEq;
        }

        public bool GetTapeFullPath(ref string szPath)
        {
            if(oCalc.GetUseTape() == false)
                return false;

            return oCalc.GetFullTapePath(ref szPath);
        }

        public bool GetShowDMS()
        {
            if(eLastInputType == InputType.INPUT_Equals)
                return oCalc.GetShowDMS();
            return false;
        }

        public bool GetShowBCD()
        {
            return oCalc.GetShowBCD();
        }

        public bool GetUseTape()
        {
            return oCalc.GetUseTape();
        }

        public void SetUseTape(bool bUseTape)
        {
            
        }

        public InputType GetInputType()
        {
            return eLastInputType;
        }

        public void SetShowDMS(bool bShow)
        {
            oCalc.SetShowDMS(bShow);
        }

        public void SetShowBCD(bool bShow)
        {
            oCalc.SetShowBCD(bShow);
        }

        public bool SetBaseBits(int iBits)
        {
            oCalc.SetBaseBits(iBits);
            return true;
        }

        virtual public void SaveState(string name)
        {
            StringState sState = new StringState();
            
            sState.eLastInputType = eLastInputType;
            sState.szAnswer = szAnswer;
            sState.szEquation = szEquation;
            sState.szLastAnswer = szLastAnswer;
            sState.eFormatAns = eFormatAns;

            try
            {
                TextWriter writer = new StreamWriter(name);
                //and serialize data
                System.Xml.Serialization.XmlSerializer ser = new System.Xml.Serialization.XmlSerializer(typeof(StringState));
                ser.Serialize(writer, sState);
            }
            catch (System.Exception ex)
            {

            }

   
        }

        virtual public void RestoreState(string name)
        {
            StringState sState = new StringState();

            try
            {
                TextReader reader = new StreamReader(name);

                //and serialize data
                System.Xml.Serialization.XmlSerializer ser = new System.Xml.Serialization.XmlSerializer(typeof(StringState));
                sState = (StringState)ser.Deserialize(reader);

                eLastInputType = sState.eLastInputType;
                szAnswer = sState.szAnswer;
                szEquation = sState.szEquation;
                szLastAnswer = sState.szLastAnswer;
                eFormatAns = sState.eFormatAns;
            }
            catch (System.Exception ex)
            {

            }
           
        }
    }
}
