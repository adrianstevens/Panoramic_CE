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

namespace CalcPro
{
    public class IssRPNCalc: IssDALCalc
    {
        public IssRPNCalc()
        {
            bShowOtherDisplay = false;
            bHP = true;
            iStackSize = 4;

            for (int i = 0; i < MAX_STACK_SIZE; i++)
                szStack[i] = "0";
        }

        static int MAX_STACK_SIZE = 50;
        static int MIN_STACK_SIZE=	 2;

        string[]		szStack = new string[MAX_STACK_SIZE];
	    string			szOperator;
	    string			szTemp;
	    string			szLastX;

	    int	iStackSize
        {
            get {return _iStackSize;}
            set { _iStackSize = value; }
        }
        int _iStackSize;

	
	    bool			bShowOtherDisplay;

        bool            bHP; //experimental

        public bool     GetIsHPMode(){return bHP;}
        public void     SetIsHPMode(bool bSetHP){bHP = bSetHP;}
        public int		GetStackSize(){return iStackSize+1;}


        public override bool AddFunction(FunctionType eFunction)
        {
            switch(eFunction) 
	        {
                case FunctionType.FUNCTION_PlusMinus:
                    {
                        if (szLastNumEntered.Length == 0)
                            return false;

                        int iE = szLastNumEntered.IndexOf("E");
                        
                        int iee = szLastNumEntered.IndexOf("e");

                        //do we have an exponent?
                        if (iE > -1)
                        {
                            //check for a neg exponent
                            if (szLastNumEntered[iE+1] == '-')
                            {
                                szLastNumEntered = szLastNumEntered.Remove(iE+1, 1);
                            }
                            else
                            {
                                szLastNumEntered = szLastNumEntered.Insert(iE+1, "-");
                            }
                        }
                    /*    else if (iee > -1)
                        {
                            //check for a neg exponent
                            if (szLastNumEntered[iee + 1] == '-')
                            {
                                szLastNumEntered = szLastNumEntered.Remove(iE + 1, 1);
                                szLastNumEntered = szLastNumEntered.Insert(iE + 1, "+");
                            }
                            else
                            {
                                szLastNumEntered = szLastNumEntered.Remove(iE + 1, 1);
                                szLastNumEntered = szLastNumEntered.Insert(iE + 1, "-");
                            }
                        }*/
                        else
                        {
                            if (szLastNumEntered[0] != '-')
                                szLastNumEntered = "-" + szLastNumEntered;
                            else
                                szLastNumEntered = szLastNumEntered.Remove(0, 1);

                        }
                 
                        return true;
                   }
	            case FunctionType.FUNCTION_DRG:
		            oCalc.ConvertDRG(ref szLastNumEntered, ref szLastNumEntered);
                    goto case FunctionType.FUNCTION_DRGN;
	            case FunctionType.FUNCTION_DRGN:
		            eLastInputType = InputType.INPUT_Constants;
		            return oCalc.SetDRGState(DRGStateType.DRG_Next);
		        
             /*   case FunctionType.FUNCTION_PercentageT:

                    break;
                case FunctionType.FUNCTION_DPercentage:
            
                    break;*/
	            case FunctionType.FUNCTION_Percentage:
		            {
                        int iLen = szLastNumEntered.Length;
                        if(iLen>0)
                        {
                            if(bHP)
                            {
                                szTemp = szLastNumEntered + CalcGlobals.szOperators[(int)OperatorType.OPP_Times] + "0.01" + CalcGlobals.szOperators[(int)OperatorType.OPP_Times] + szStack[0];
                                oCalc.Calculate(szTemp, ref szLastNumEntered);
                            }
                            else
                            {
                                szTemp = szLastNumEntered + CalcGlobals.szOperators[(int)OperatorType.OPP_Times] + "0.01";
                                oCalc.Calculate(szTemp, ref szLastNumEntered);
                            }
                        }
                        return true;
		            }
	        }

	        eLastInputType = InputType.INPUT_Function;

	        if(szLastNumEntered == null || szLastNumEntered.Length<1)
		        szLastNumEntered = ("0");

	        szTemp =  CalcGlobals.szFunctionsFull[(int)eFunction];

	        szTemp = szTemp + szLastNumEntered + (")");

	        oCalc.Calculate(szTemp, ref szLastNumEntered);
	        return true;
        }


        bool HPEEX()
        {
            if(eLastInputType != InputType.INPUT_Number)
                return false;
            if(szLastNumEntered.Length == 0)
                return false;
            if(szLastNumEntered.IndexOf("e") != -1)
                return false;
            szLastNumEntered += ("E");
            return true;
        }

        //note - x^y button actually operates as y^x in RPN
        public override bool AddOperator(OperatorType eOperator)
        {
	        if(((int)eOperator<0) || 
		        ((int)eOperator > CalcGlobals.NUMBER_OF_Operators))
		        return false;
	
	        szOperator = CalcGlobals.szOperators[(int)eOperator];

            if(bHP)
            {
                if(eOperator == OperatorType.OPP_EXP || eOperator == OperatorType.OPP_EEX) //eex should never happen ... converted before it gets here
                {
                    return HPEEX();
                }


                if((eOperator == OperatorType.OPP_XY || eOperator == OperatorType.OPP_XrY) 
                    && bHP == false)
                {
                    Calculate(szLastNumEntered, szStack[0], szOperator, ref szLastNumEntered);
                }
                else if(eOperator == OperatorType.OPP_DPcnt || eOperator == OperatorType.OPP_PcntT)
                {
                    Calculate(szLastNumEntered, szStack[0], szOperator, ref szLastNumEntered);
                    Calculate(szLastNumEntered, ("100"), CalcGlobals.szOperators[(int)OperatorType.OPP_Times], ref szLastNumEntered);
                }
                else
                {
                    Calculate(szStack[0], szLastNumEntered, szOperator, ref szLastNumEntered);
                } 
            }
            else
            {
                Calculate(szStack[0], szLastNumEntered, szOperator, ref szLastNumEntered);
            }
		
	        //save last answer
	        szAns = szLastNumEntered;
	        eLastInputType=InputType.INPUT_Operator;
	        return DropStack();
        }

        public override bool AddNumber(NumberType eNumber)
        {
	        switch(eLastInputType) 
	        {
	        case InputType.INPUT_Operator:
                goto case InputType.INPUT_Constants;
	        case InputType.INPUT_Function:
                goto case InputType.INPUT_Constants;
	        case InputType.INPUT_Constants:
                if(eNumber == NumberType.NUM_DMS)
                { 
                    oCalc.SetShowDMS(true);
                    return true;
                }
		        AddToStack();
                szLastNumEntered = "";
                break;
	        default:
		        break;
	        }
	        return base.AddNumber(eNumber);
        }

        public override bool AddEquals(EqualsType eEquals)
        {
	        if(eEquals!= EqualsType.EQUALS_Equals && eEquals != EqualsType.EQUALS_Enter)
		        return false;

            //This is the enter button
	        if(szLastNumEntered == null || szLastNumEntered.Length<1)
		        return false;

	        eLastInputType=InputType.INPUT_Equals;

            oCalc.SetShowDMS(false);
	
	        return AddToStack();

        }

        //adds m_szLastAns to the RPN number stack
        bool AddToStack()
        {
	        for(int i=iStackSize-1; i>0;i--)
	        {
		        szStack[i] = szStack[i-1];
            }
	        szStack[0]= szLastNumEntered;
	        
	        szLastX = szStack[0];
	        return true;
        }

        public bool DropStack()
        {
	        szLastX = szStack[0];
	        for(int i=0; i<iStackSize-1; i++)
		        szStack[i] = szStack[i+1];
		        
	        return true;
        }

        void Clear()
        {
            szOppFuncDisplay = "";
            szLastNumEntered = "";
            szTemp = "";
            szAns = "";

	        eLastInputType=InputType.INPUT_NULL;

	        for(int i=0; i<MAX_STACK_SIZE; i++)
		        szStack[i] = ("0");
        }

        public override bool GetAnswer(ref string szAnswer)
        {
            szAnswer = "";
	        if(bShowOtherDisplay==true)
	        {
		        szAnswer = ("X: ");
	        }
            if (szLastNumEntered == null || szLastNumEntered.Length < 1)
            {
                szAnswer += "0";
		        return true;
            }
	        else
            {
		        szAnswer += szLastNumEntered;
                return true;
            }
        }

        public override bool GetEquation(ref string szEquation)
        {
	        if(bShowOtherDisplay==false)
	        {
                szEquation = "";
		        return true;
	        }
	        if(iStackSize==MIN_STACK_SIZE)
	        {
                szEquation = "T: " + szStack[2] + " Z: " + szStack[1] + " Y: " + szStack[0];
	        }
	        else
	        {
		        szEquation = szStack[0];
	        }

	        return true;
        }

        public bool RotateStackDown()
        {
	        if(szLastNumEntered == null || szLastNumEntered.Length<1)
		        szTemp = ("0");
	        else
		        szTemp = szLastNumEntered;

	        szLastNumEntered = (szStack[0]);

	        for(int i=0; i<iStackSize; i++)
	        {
		        szStack[i] = szStack[i+1];
	        }

	        szStack[iStackSize-1] = szTemp;
	        szLastX = szStack[0];

            return true;
        }


        bool RotateStackUp()
        {
	        if(szLastNumEntered == null || szLastNumEntered.Length<1)
		        szTemp = ("0");
	        else
		        szTemp = szLastNumEntered;

	        szLastNumEntered = (szStack[iStackSize-1]);

	        for(int i=iStackSize; i>0; i--)
	        {
		        szStack[i] = szStack[i-1];
	        }

	        szStack[0] = szTemp;
	        szLastX = szStack[0];

	        return true;
        }


        public bool SetStackSize(int iNewStackSize)
        {
            if (iNewStackSize < MIN_STACK_SIZE ||
                iNewStackSize > MAX_STACK_SIZE)
		        return false;

            iStackSize = iNewStackSize - 1;
	        return true;
        }

        void Calculate(string szOpp1, string szOpp2, string szOpp, ref string szResult)
        {
	        bool bOpp2Empty = false;
	        bool bOpp1Empty = false;
//	        bool bOppEmpty	= false;

	        if(szOpp1 == null || szOpp1.Length<1)
		        bOpp1Empty = true;
            if (szOpp2 == null || szOpp2.Length < 1)
		        bOpp2Empty = true;
//	        if(szOpp.Length<1)
//		        bOppEmpty = true;

	        if(bOpp1Empty && bOpp2Empty)
		        szResult = ("");
	        else if(bOpp1Empty)
		        szResult = szOpp2;
	        else if(bOpp2Empty)
		        szResult = szOpp1;
	        else
	        {
		        szTemp = szOpp1;
		        szTemp += szOpp;
		        szTemp += szOpp2;
		        oCalc.Calculate(szTemp, ref szResult);
	        }
        }

        //A few RPN Specific buttons
        public bool AddRPN(RPNManipulationType eRPN)
        {
	        if((int)eRPN < 0 || (int)eRPN > CalcGlobals.NUMBER_OF_RPN)
		        return false;

	        switch(eRPN) 
	        {
	        case RPNManipulationType.RPN_XY:
		        return XYSwitch();
		        
	        case RPNManipulationType.RPN_RotateUp:
		        return RotateStackUp();
		        
	        case RPNManipulationType.RPN_RotateDown:
		        return RotateStackDown();
		        
	        case RPNManipulationType.RPN_StackDown:
		        return DropStack();
		        
	        }
	        return false;
        }

        bool XYSwitch()
        {
	        szTemp = szLastNumEntered;
	        szLastNumEntered = (szStack[0]);
	        szStack[0] = szTemp;
            if (szLastNumEntered == null || szLastNumEntered.Length < 1)
		        szLastNumEntered = ("0");
	        if(szStack[0] == null || szStack[0].Length < 1)
		        szStack[0] = ("0");
	        szLastX = szStack[0];
	        return true;
        }

        public void OnScreenTap()
        {
	        string szName;
	        string szMessage;

            szName = "";
            szMessage = "";

	        //NAME
            int iTemp = iStackSize + 1;
            szName = iTemp.ToString();
	        
	        szTemp = ("R.P.N. stack size: ");

	        szName = szTemp + szName;


	        //Body
	        int i=iStackSize;
	
	        while(i>3)
	        {
                iTemp = i+1;

                szMessage += (iTemp.ToString() + ": " + szStack[i-1] + "\r\n");
                i--;
	        }
	
	        if(iStackSize>2)
	        {
                szMessage += ("T: " + szStack[2] + "\r\n");
	        }

            szMessage += ("Z: " + szStack[1] + "\r\n");

            szMessage += ("Y: " + szStack[0] + "\r\n");

            szMessage += ("X: " + szLastNumEntered + "\r\n");


            // ... pretty sure we don't use this anymore but since I already ported it I'll leave it
	      /*  MessageBox(NULL,
			           szMessage, 
			           szName,
			           MB_OK);*/
        }

        public override bool AddBrackets(BracketType eBracket)
        {
	        if((int)eBracket<0 ||
		        (int)eBracket>CalcGlobals.NUMBER_OF_Brackets-1)
		        return false;

	        else if(eBracket == BracketType.BRACKET_Open) //This will be X<>Y
	        {
		        return XYSwitch();
	        }
	        else //Close Bracket will be a Rotate Stack Down
	        {
		        return RotateStackDown();
	        }
        }

        public bool GetLastX(ref string szLastXout)
        {
	        szLastXout =  szLastX;
            return true;
            
        }

        public override bool AddClear(ClearType eClear)
        {
	        if(eClear == ClearType.CLEAR_CLX)
	        {
                oCalc.SetShowDMS(false);
		        szLastNumEntered = ("0");
		        return true;
	        }
	        else
	        {
		        return base.AddClear(eClear);
	        }
        }

        public override bool AddConstants(ConstantType eConstants)
        {
        //    if(eLastInputType == InputType.INPUT_Number)
        //        AddEquals(EQUALS_Enter);
            return base.AddConstants(eConstants);
        }

        public override bool AddString(string szString)
        {
            if(szString.Length == 0 && eLastInputType != InputType.INPUT_Equals)
                AddEquals(EqualsType.EQUALS_Enter);
            return base.AddString(szString);
        }

        public bool GetStackEntry(int iEntry, ref string szValue)
        {
            if(iEntry < 0 || iEntry >= MAX_STACK_SIZE)
                return false;

            szValue = szStack[iEntry];
            return true;
        }

    }
}
