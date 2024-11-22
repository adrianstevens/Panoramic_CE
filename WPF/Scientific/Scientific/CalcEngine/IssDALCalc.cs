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

namespace CalcPro
{
    public class IssDALCalc: IssStringCalc
    {
        struct DALState
        {
             public string szLastNumEntered;
             public string szPreviousNumber;
             public string[] szEquation;// = new string[CalcGlobals.MAX_Bracket_Levels];

             public string szOppFuncDisplay;
             public string szAns;

             public int[] iOpenBracketCount;// = new int[CalcGlobals.MAX_Bracket_Levels];
             public int iUserBrackets;
        };

        static public int MAX_INPUT_LENGTH = 64;//for the ol binary

        protected string szLastNumEntered = "";
        protected string szPreviousNumber = "";
        protected new string[] szEquation = new string[CalcGlobals.MAX_Bracket_Levels];

        protected string szOppFuncDisplay = "";
        protected string szAns = "";//Last Answer Variable

        protected int[] iOpenBracketCount = new int[CalcGlobals.MAX_Bracket_Levels];
        protected int iUserBrackets;


        public IssDALCalc()
        {
            Clear();
        }

        void Clear()
        {
            szPreviousNumber = "";
            szOppFuncDisplay = "";
            szLastNumEntered = "";

            for (int i = 0; i < CalcGlobals.MAX_Bracket_Levels; i++)
	        {
                szEquation[i] = "";
		        iOpenBracketCount[i] = 0;
	        }
	        iUserBrackets		= 0;
            eLastInputType = InputType.INPUT_NULL;
            oCalc.SetShowDMS(false);
        }



        public override bool GetEquation(ref string szEquation)
        {
            szEquation = szOppFuncDisplay;
	        return true;
	
        }

        void SetEquation(ref string szEquation)
        {
            szOppFuncDisplay = szEquation;
        }

        public override bool GetAnswer(ref string szAnswer)
        {
	        switch(eLastInputType) 
	        {
                case InputType.INPUT_Clear:
                    goto case InputType.INPUT_NULL;
                case InputType.INPUT_NULL:
		            szAnswer =("0");
		            break;
                case InputType.INPUT_Bracket:
                case InputType.INPUT_Equals:
                case InputType.INPUT_Constants:
                case InputType.INPUT_Function:
                case InputType.INPUT_Number:
                case InputType.INPUT_Operator:
		            szAnswer = szLastNumEntered;
		        break;
	        }
	        return true;
        }




        /********************************************************************
	        Function:	AddNumber

	        Arguments:	eNumber
				
	        Returns:	true if its good

	        Comments:	
	
	        Checks input for correct base
	        Checks for max length
	        Clears m_szOppFunc if last input was equals
	        Corrects input for period (special cases)
	        Adds given number to m_szLastNum
        *********************************************************************/
        public override bool AddNumber(NumberType eNumber)
        {
	        //check the input base
	        if((int)eNumber > GetCalcBase() -1 &&
                eNumber != NumberType.NUM_Period && eNumber != NumberType.NUM_DMS) 
		        return false;

            if(eLastInputType == InputType.INPUT_Equals)
            {
                if(eNumber == NumberType.NUM_DMS)
                {
                    oCalc.SetShowDMS(true);
                    return true;
                }
            }

            if (eLastInputType == InputType.INPUT_Operator || szLastNumEntered == null)
                szLastNumEntered = "";

	        //check max input length
            if(szLastNumEntered.Length > MAX_INPUT_LENGTH && eLastInputType == InputType.INPUT_Number)
	        //if(szLastNumEntered.Length>MAX_INPUT_LENGTH &&eLastInputType == InputType.INPUT_Number)
		        return false;

	        if(eNumber==NumberType.NUM_Period && oCalc.GetBase() != 10)
	            return false;
            if (eNumber == NumberType.NUM_DMS && oCalc.GetBase() != 10)
                return false;
	
	        switch(eLastInputType) 
	        {
                case InputType.INPUT_Bracket:
                    szEquation[iUserBrackets] += "*";
                    goto case InputType.INPUT_Equals;
                case InputType.INPUT_Equals:
                    szOppFuncDisplay = "";
                    goto case InputType.INPUT_Function;
                case InputType.INPUT_Constants:
                    goto case InputType.INPUT_Function;
                case InputType.INPUT_Operator:
                    goto case InputType.INPUT_Function;
                case InputType.INPUT_Clear:
                    goto case InputType.INPUT_Function;
                case InputType.INPUT_NULL:
                    goto case InputType.INPUT_Function;
                case InputType.INPUT_Function:
                    szLastNumEntered = "";
		            break;
                case InputType.INPUT_Number:
		            break;
	        }

            int iLocation = szLastNumEntered.IndexOf("°");

            //some DMS work to do 
            if(eNumber == NumberType.NUM_DMS)
            {
                if(szLastNumEntered.IndexOf(".") != -1)
                    return false;

                if(szLastNumEntered.Length == 0)
                    szLastNumEntered = "0";

                //so we've been allowed to enter it so we must be able to add the degrees or minutes symbol
                if(iLocation != -1)
                {   
                    if(szLastNumEntered.IndexOf("\x2032") != -1)
                        return false;

                    //we need to add the minutes symbol
                    int iLen = szLastNumEntered.Length;

                    //but first check if we need to add zeros
                    for(int i = 0; i < iLocation + 3 - iLen; i++)
                        szLastNumEntered += CalcGlobals.szNumbers[(int)NumberType.NUM_0];

                    //then add the minutes symbol
                    szLastNumEntered += ("\x2032");
                }
                else //we need to add the degrees symbol
                {   //so just go ahead and do it
                    szLastNumEntered += ("°");
                }

               eLastInputType = InputType.INPUT_Number;
                return true;
            }
            else if(iLocation != -1)
            {   //so this means we have a DMS number started so handle input accordingly
                //only want 0-9
                if (eNumber > NumberType.NUM_9)
                    return false; 
                //now lets figure out where in the number we are
                int i2ndLoc = szLastNumEntered.IndexOf("\x2032");

                if(i2ndLoc != -1)
                {   //we've already added the minutes symbol 
                    //do we need to add a period?
                    if(i2ndLoc + 3 == szLastNumEntered.Length)
                        szLastNumEntered += (".");
                }

                else if(iLocation + 3 == szLastNumEntered.Length)
                {
                    szLastNumEntered += ("\x2032");
                }

                szLastNumEntered += CalcGlobals.szNumbers[(int)eNumber];
                //and we're done
                return true;
            }

	
	
	        //error correction for the period
            if (eNumber == NumberType.NUM_Period)
	        {
		        //one period per number
		        if(szLastNumEntered.IndexOf(".")!=-1)
			        return false;
		        //add a zero if there's nothing on screen 
		        else if(szLastNumEntered.Length<1)
			        szLastNumEntered += ("0");
	        }
	
	        //error correction for extra zeros
            else if (eNumber == NumberType.NUM_0)
	        {
		        //only one leading zero
		        if(szLastNumEntered.Length==1 &&
			        szLastNumEntered[0]==('0'))
			        return false;
	        }
	        else //anything but a zero or a period
	        {
		        //clear off any leading zeros
		        if(szLastNumEntered.Length==1 &&
			        szLastNumEntered[0]==('0'))
                    szLastNumEntered = "";
	        }
	
           eLastInputType = InputType.INPUT_Number;
	        szLastNumEntered += CalcGlobals.szNumbers[(int)eNumber];
	        return true;
        }

        /********************************************************************
	        Function:	AddFunction

	        Arguments:	eFunction
				
	        Returns:	true if its good

	        Comments:	
	
	        Function special cases - ie DRG
	        Checks if m_szOpp is blank and m_szLast 0, szEq is not...then Concat times onto m_szEq
	        Clears m_szLast num cause this is DAL
	        Copy the operator into m_szOppFunc
	        Concat the function onto the equation
	        Incrase openbracket count
        *********************************************************************/
        public override bool AddFunction(FunctionType eFunction)
        {
	        switch(eFunction) 
	        {
	            //DRG doesn't really count as a function	
	            case FunctionType.FUNCTION_Percentage:
	            {
		            string szTemp;
		            int iLen = szEquation[iUserBrackets].Length;
		            if(iLen>0 &&
			            (szEquation[iUserBrackets][iLen-1] == ('+') || szEquation[iUserBrackets][iLen-1] == ('-')) &&
			           eLastInputType == InputType.INPUT_Number)
		            {
                        szTemp = szPreviousNumber + CalcGlobals.szOperators[(int)OperatorType.OPP_Times] + szLastNumEntered + CalcGlobals.szOperators[(int)OperatorType.OPP_Times] + ("0.01");
                        oCalc.Calculate(szTemp, ref szLastNumEntered);
		            }
		            else
		            {
                        szTemp = szLastNumEntered + CalcGlobals.szOperators[(int)OperatorType.OPP_Times] + ("0.01");
			            oCalc.Calculate(szTemp, ref szLastNumEntered);
		            }
		           eLastInputType = InputType.INPUT_Constants;
		            return true;
	            }

	        case FunctionType.FUNCTION_DRG:
		        oCalc.ConvertDRG(ref szLastNumEntered, ref szLastNumEntered);
                goto case FunctionType.FUNCTION_DRGN;
	        case FunctionType.FUNCTION_DRGN:
		       eLastInputType = InputType.INPUT_Constants;
               return oCalc.SetDRGState(DRGStateType.DRG_Next);
	        default:
		        break;
	        }

	        //for non DAL functions..ie x^2
	        if(CheckForPostFunction(eFunction))
		        return PostFunction(eFunction);

	        switch(eLastInputType) 
	        {
	        case InputType.INPUT_Operator:
                    break;
	        case InputType.INPUT_Clear:
                    break;
	        case InputType.INPUT_NULL:
                    break;
	        case InputType.INPUT_Function:
		            break;//ideal
	        case InputType.INPUT_Constants:
                    goto case InputType.INPUT_Number;
	        case InputType.INPUT_Bracket:
                    goto case InputType.INPUT_Number;
	        case InputType.INPUT_Number:
		        //add the number to the equation and add a times just cause 
		        szEquation[iUserBrackets] += szLastNumEntered;
                szLastNumEntered = "";
                AddOperator(OperatorType.OPP_Times);
		        break;
	        case InputType.INPUT_Equals:
		        szLastNumEntered = "";//clear it
		        break;
	        }

	        eLastInputType=InputType.INPUT_Function;

	        iOpenBracketCount[iUserBrackets]++;
	        //set up our opp/func display
	        szOppFuncDisplay = CalcGlobals.szFunctionsFull[(int)eFunction];
            szOppFuncDisplay = szOppFuncDisplay.Substring(0, szOppFuncDisplay.Length - 1);
            	        
	
	        szEquation[iUserBrackets] += CalcGlobals.szFunctionsFull[(int)eFunction];	
	        return true;
        }


        /********************************************************************
	        Function:	AddOperator

	        Arguments:	eOperator
				
	        Returns:	true if its good

	        Comments:	
	

        *********************************************************************/
        public override bool AddOperator(OperatorType eOperator)
        {
	        bool bCalc = false;

	        switch(eLastInputType) 
	        {
	        case InputType.INPUT_Function://no operators after functions
		        return false;
	        case InputType.INPUT_Clear:
                goto case InputType.INPUT_NULL;
	        case InputType.INPUT_NULL:
		        //this should mean there's nothing onscreen ... we'll make it a 0 to behave nicely
		        szLastNumEntered = ("0");
		        bCalc = true;
		        break;
	        case InputType.INPUT_Operator:
		        //if the last in was an Opp, we clear the last before adding the new
		        szEquation[iUserBrackets] = szEquation[iUserBrackets].Substring(szEquation[iUserBrackets].Length - 1);
                goto case InputType.INPUT_Number;
	        case InputType.INPUT_Constants:
                goto case InputType.INPUT_Number;
	        case InputType.INPUT_Bracket:
                goto case InputType.INPUT_Number;
	        case InputType.INPUT_Equals:
                goto case InputType.INPUT_Number;
	        case InputType.INPUT_Number:
		        bCalc = true;
		        break;
	        }

	        //was just a little to complex to cram it into the switch
	        if(bCalc)
	        {
		        //store the lastNum into previous so we can use it for that damn percentage button
		        szPreviousNumber = szLastNumEntered;
		        //these all mean we have something in szLast...so add it first...then clear it

		        szEquation[iUserBrackets] += szLastNumEntered;
                szLastNumEntered = "";
		        
                for(int i=0; i<iOpenBracketCount[iUserBrackets]; i++)
			        szEquation[iUserBrackets] += (")");
		        iOpenBracketCount[iUserBrackets]=0;
	        }

	       eLastInputType = InputType.INPUT_Operator;	
	        //add the opp
	        szEquation[iUserBrackets] += CalcGlobals.szOperators[(int)eOperator];
	        //set our display opp
            if (eOperator == OperatorType.OPP_Times)
                szOppFuncDisplay = ("×");
            else if (eOperator == OperatorType.OPP_Divide)
                szOppFuncDisplay = ("÷");
            else
                szOppFuncDisplay = CalcGlobals.szOperators[(int)eOperator];
	        return true;
        }

        public override bool AddEquals(EqualsType eEquals)
        {
	        switch(eLastInputType) 
	        {
	        case InputType.INPUT_Operator://remove the stray opp
                    szEquation[iUserBrackets] = szEquation[iUserBrackets].Substring(0, szEquation[iUserBrackets].Length - 1);
		        
		        break;
	        case InputType.INPUT_Clear:
                return false;//nothing to do
	        case InputType.INPUT_NULL:
                return false;//nothing to do
	        case InputType.INPUT_Equals:
                return false;//nothing to do
	        case InputType.INPUT_Function:
		        return false;//nothing to do
	        case InputType.INPUT_Constants:
                goto case InputType.INPUT_Number;
	        case InputType.INPUT_Bracket:
                goto case InputType.INPUT_Number;
	        case InputType.INPUT_Number:
		        int iTemp = iUserBrackets;
		        for(int j=0; j<iTemp+1; j++)//we do one extra because we want to check
		        {
			        szEquation[iUserBrackets] +=  szLastNumEntered;
                    szLastNumEntered = "";
			        //add these to the equation
			        for(int i=0; i<iOpenBracketCount[iUserBrackets]; i++)
				        szEquation[iUserBrackets] +=  (")");
			        iOpenBracketCount[iUserBrackets]=0;
			        AddBrackets(BracketType.BRACKET_Close);//won't do anything when j=0;
		        }
		        break;//close em off
	        }
	       eLastInputType = InputType.INPUT_Equals;

	        //set our OppFunc
	        szOppFuncDisplay =  ("=");

	        oCalc.Calculate(szEquation[iUserBrackets], ref szLastNumEntered);
            szEquation[iUserBrackets] = "";

	        //save last answer
	        szAns = szLastNumEntered;
	        return true;
        }

        public override bool AddBrackets(BracketType eBracket)
        {
	        switch(eBracket) 
	        {
		        case BracketType.BRACKET_Open:
			        {
				        if(iUserBrackets + 2 > CalcGlobals.MAX_Bracket_Levels)
					        return false;
				        switch(eLastInputType) 
				        {
				        case InputType.INPUT_Constants:	
                                goto case InputType.INPUT_Number;
				        case InputType.INPUT_Bracket:
                                goto case InputType.INPUT_Number;
				        case InputType.INPUT_Number:
				        //add these too the equation
				        //	szEquation[iUserBrackets] +=  szLastNumEntered);
                                AddOperator(OperatorType.OPP_Times);
                            goto case InputType.INPUT_Equals;
				        case InputType.INPUT_Equals:
                            szLastNumEntered = "";
                            goto case InputType.INPUT_Function;
				        case InputType.INPUT_Operator:
                            goto case InputType.INPUT_Function;
				        case InputType.INPUT_Clear:
                            goto case InputType.INPUT_Function;
				        case InputType.INPUT_NULL:
                            goto case InputType.INPUT_Function;
				        case InputType.INPUT_Function:
				        //ideal
					        iUserBrackets++;
					       eLastInputType=InputType.INPUT_Clear;//fresh equation
					        break;
				        }			
			        }
			        break;
		        case BracketType.BRACKET_Close:
		        {
			        if(iUserBrackets==0)
				        return false;

			        switch(eLastInputType) 
				        {
				        case InputType.INPUT_Equals:	//will never happen....
                            return false;
				        case InputType.INPUT_Operator:
                            return false;
				        case InputType.INPUT_Clear:
                            return false;
				        case InputType.INPUT_NULL:
                            return false;
				        case InputType.INPUT_Function:
					        return false;
				        case InputType.INPUT_Constants:	
                            goto case InputType.INPUT_Number;
				        case InputType.INPUT_Bracket:
                            goto case InputType.INPUT_Number;
				        case InputType.INPUT_Number:
					        //add these too the equation
					        szEquation[iUserBrackets] +=  szLastNumEntered;
					        oCalc.Calculate(szEquation[iUserBrackets], ref szLastNumEntered);
                            szEquation[iUserBrackets] = "";
					        iUserBrackets--;
                            break;
			            }
		        }
                break;
	        }
	        szOppFuncDisplay =  CalcGlobals.szBrackets[(int)eBracket];
	        return true;
        }

        public override bool AddClear(ClearType eClear)
        {
	        switch(eClear) 
	        {
	        case ClearType.CLEAR_ClearAll:
                goto case ClearType.CLEAR_Clear;
	        case ClearType.CLEAR_Clear:
		        eLastInputType=InputType.INPUT_Clear;
		        Clear();
		        break;
	        case ClearType.CLEAR_CE:
		        AddString((""));
		        eLastInputType=InputType.INPUT_Clear;
		        break;
	        
            case ClearType.CLEAR_BackSpace:
	            {
		            switch(eLastInputType)
		            {
		            case InputType.INPUT_Number:
                         goto case InputType.INPUT_Bracket;
		            case InputType.INPUT_Bracket:
			            if(szLastNumEntered.Length>0)
				            szLastNumEntered = szLastNumEntered.Substring(0, szLastNumEntered.Length-1);
			            break;
		            default:
			            break;
		            }
	            }
                break;
	
	        default:
		        break;
	        }
	        return true;
        }

        public override bool AddString(string szString)
        {
	        switch(eLastInputType) 
	        {
	        case InputType.INPUT_Bracket:
                    AddOperator(OperatorType.OPP_Times);
                goto case InputType.INPUT_Function;
            case InputType.INPUT_Clear:
                goto case InputType.INPUT_Function;
	        case InputType.INPUT_NULL:
                goto case InputType.INPUT_Function;
	        case InputType.INPUT_Constants:
                goto case InputType.INPUT_Function;
	        case InputType.INPUT_Equals:
                goto case InputType.INPUT_Function;
	        case InputType.INPUT_Number:
                goto case InputType.INPUT_Function;
	        case InputType.INPUT_Operator:
                goto case InputType.INPUT_Function;
	        case InputType.INPUT_Function:
                szLastNumEntered = szString;
                szOppFuncDisplay = "";
		        break;
	        }

	        eLastInputType=InputType.INPUT_Constants;
	        return true;
        }

        public override bool AddConstants(ConstantType eConstants)
        {
            string szTemp = "";

	        if(eConstants == ConstantType.CONSTANT_Last_Ans)
	        {
		        if(szAns.Length > -1)
			        oCalc.Calculate(szAns, ref szTemp);
		        else 
			        szTemp =("0");
	        }
	        else if(oCalc.GetBase() != 10)
		        return false;
	        else
		        oCalc.Calculate(CalcGlobals.szConstants[(int)eConstants], ref szTemp);
	        return AddString(szTemp);
        }

        protected bool PostFunction(FunctionType eFunction)
        {
	        switch(eLastInputType) 
	        {
	        case InputType.INPUT_Operator:
		        return false;
		        
	        case InputType.INPUT_Clear:
                szLastNumEntered = ("0");
                break;
	        case InputType.INPUT_NULL:
		        szLastNumEntered = ("0");
                break;

	        case InputType.INPUT_Function:
                break;
	        case InputType.INPUT_Constants:
                break;
	        case InputType.INPUT_Bracket:
                break;
	        case InputType.INPUT_Equals:
                break;
	        case InputType.INPUT_Number:
		        break;
	        }
	
	        string szTempNum;
	
	        //we have to insert and calculate the function answer
	        int iLastLength = (szLastNumEntered == null)?0:szLastNumEntered.Length;
	        if(iLastLength<1)
		        return false;

	        eLastInputType = InputType.INPUT_Constants;

            szTempNum = CalcGlobals.szFunctionsFull[(int)eFunction] + szLastNumEntered + (")");

	        //clear the Opp Func display
            szOppFuncDisplay = "";
	
	        oCalc.Calculate(szTempNum, ref szLastNumEntered);
	        return true;
	
        }

        bool CheckForPostFunction(FunctionType eFunction)
        {
	        switch(eFunction)
	        {
		        case FunctionType.FUNCTION_PlusMinus:
		        case FunctionType.FUNCTION_DegToDec:
		        case FunctionType.FUNCTION_DecToDeg:
		        case FunctionType.FUNCTION_Inverse:	
		        case FunctionType.FUNCTION_Squared:
		        case FunctionType.FUNCTION_Cubed:
		        case FunctionType.FUNCTION_SquareRoot:
		        case FunctionType.FUNCTION_CubeRoot:
		        case FunctionType.FUNCTION_EX:
		        case FunctionType.FUNCTION_10X:
		        case FunctionType.FUNCTION_Factorial:
		        case FunctionType.FUNCTION_Sum:
		        case FunctionType.FUNCTION_Random:
			        return true;
		        default:
			        return false;
	        }
        }


        public override void SaveState(string name)
        {
            DALState sState = new DALState();

            sState.szLastNumEntered = szLastNumEntered;
            sState.szPreviousNumber = szPreviousNumber;
            sState.szEquation = new string[CalcGlobals.MAX_Bracket_Levels];

            sState.szOppFuncDisplay = szOppFuncDisplay;
            sState.szAns = szAns;

            sState.iOpenBracketCount = new int[CalcGlobals.MAX_Bracket_Levels];

            sState.iUserBrackets = iUserBrackets;

            for (int i = 0; i < CalcGlobals.MAX_Bracket_Levels; i++)
            {
                sState.szEquation[i] = szEquation[i];
                sState.iOpenBracketCount[i] = iOpenBracketCount[i];
            }

            try
            {
                System.Xml.Serialization.XmlSerializer ser = new System.Xml.Serialization.XmlSerializer(typeof(DALState));
                TextWriter writer = new StreamWriter(name);
                                       
                //and serialize data
                ser.Serialize(writer, sState);
            }
            catch (System.Exception ex)
            {

            }

            base.SaveState("String" + name);
        }

        public override void RestoreState(string name)
        {
            DALState sState = new DALState();

            try
            {
                TextReader reader = new StreamReader(name);

                //and serialize data
                System.Xml.Serialization.XmlSerializer ser = new System.Xml.Serialization.XmlSerializer(typeof(DALState));
                sState = (DALState)ser.Deserialize(reader);
            }
            catch (System.Exception ex)
            {
                
            }
            base.RestoreState("String" + name);
        }



    }
}
