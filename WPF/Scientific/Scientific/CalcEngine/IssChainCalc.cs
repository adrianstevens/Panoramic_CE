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
using System.Xml.Serialization;

namespace CalcPro
{
    public class IssChainCalc: IssDALCalc
    {
        struct ChainState
        {
            public string szInput;
            public string szOpp;
        };

        public IssChainCalc()
        {
            Clear();
            bRepeatOnEquals = true;
        }

        string szInput;
        string szOpp;

        bool bRepeatOnEquals;

        public bool	GetRepeatedEquals() { return bRepeatOnEquals;}
        public bool SetRepeatedEquals(bool bRepeat) { bRepeatOnEquals = bRepeat; return true; }

        public override bool AddNumber(NumberType eNumber)
        {
	        //DAL accepts numbers after functions....OOO doesn't...fail or move on
	        if(eLastInputType==InputType.INPUT_Function)
		        return false;
	        else 
		        return base.AddNumber(eNumber);
        }

        public override bool AddOperator(OperatorType eOperator)
        {
	        bool bCalc = false;

	        switch(eLastInputType) 
	        {
	        case InputType.INPUT_Clear:
	        case InputType.INPUT_NULL:
		        szLastNumEntered =  ("0");//so we can operate after a clear
		        bCalc = true;
		        break;
	        case InputType.INPUT_Operator:
		        //if the last in was an Opp, we clear the last before adding the new
		        szEquation[iUserBrackets] = szEquation[iUserBrackets].Substring(szEquation[iUserBrackets].Length - 1);
		        break;
	        case InputType.INPUT_Function:
	        case InputType.INPUT_Constants:	
	        case InputType.INPUT_Bracket:
	        case InputType.INPUT_Equals:
	        case InputType.INPUT_Number:
		        bCalc = true;
		        break;
	        }

	        if(bCalc)
	        {
		        //store Last
		        szPreviousNumber = szLastNumEntered;
		        //these all mean we have something in szLast...so add it first...then clear it
		        szEquation[iUserBrackets] += szLastNumEntered;
                szLastNumEntered = "";
		        for(int i=0; i<iOpenBracketCount[iUserBrackets]; i++)
			        szEquation[iUserBrackets] +=(")");
		        iOpenBracketCount[iUserBrackets]=0;

		        //gotta return intermediates
		        oCalc.Calculate(szEquation[iUserBrackets], ref szEquation[iUserBrackets]);
		        //test
		        szLastNumEntered = szEquation[iUserBrackets];
	        }

	        eLastInputType = InputType.INPUT_Operator;	
	
	        //add the opp
	        szEquation[iUserBrackets] +=CalcGlobals.szOperators[(int)eOperator];
	        //save the opp
	        szOpp = CalcGlobals.szOperators[(int)eOperator];
	        //set our display opp
	        //use the special symbols for times and divide
	        if(eOperator==OperatorType.OPP_Times)
		        szOppFuncDisplay = ("×");
	        else if(eOperator==OperatorType.OPP_Divide)
		        szOppFuncDisplay = ("÷");
	        else
		        szOppFuncDisplay =  CalcGlobals.szOperators[(int)eOperator];
	        return true;
        }



        public override bool AddEquals(EqualsType eEquals)
        {
	        switch(eLastInputType) 
	        {
	        case InputType.INPUT_Clear:
                    goto case InputType.INPUT_NULL;
	        case InputType.INPUT_NULL:
		        return true;
		        
	        case InputType.INPUT_Operator:
		        if(bRepeatOnEquals)
			        szInput = szPreviousNumber;
		        break;
	        case InputType.INPUT_Function:
                goto case InputType.INPUT_Number;
	        case InputType.INPUT_Constants:
                goto case InputType.INPUT_Number;
	        case InputType.INPUT_Bracket:
                goto case InputType.INPUT_Number;
	        case InputType.INPUT_Number:
		        if(bRepeatOnEquals)
		        {
			        szInput = szLastNumEntered;
		        }
	        break;
	        case InputType.INPUT_Equals:
		        if(bRepeatOnEquals &&
			        szOpp.Length>0)
		        {	
			        szEquation[iUserBrackets] = szLastNumEntered;
			        szEquation[iUserBrackets] +=szOpp;
			        szLastNumEntered = szInput;
			        eLastInputType=InputType.INPUT_Constants;
		        }
                break;
	        }
	        return base.AddEquals(eEquals);
            

        }

        public override bool AddFunction(FunctionType eFunction)
        {
	        switch(eFunction) 
	        {
	        case FunctionType.FUNCTION_Percentage:
	        {
                string szTemp = "";
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
		        return true;
	        }

	        case FunctionType.FUNCTION_DRG:
		        oCalc.ConvertDRG(ref szLastNumEntered, ref szLastNumEntered);
                goto case FunctionType.FUNCTION_DRGN;
	        case FunctionType.FUNCTION_DRGN:
                return oCalc.SetDRGState(DRGStateType.DRG_Next);

	        }
	        return PostFunction(eFunction);
        }

        void Clear()
        {
            szOppFuncDisplay = "";
            szInput = "";
            szOpp = ""; ;
            szLastNumEntered = "";

            for (int i = 0; i < CalcGlobals.MAX_Bracket_Levels; i++)
	        {
                szEquation[i] = "";
		        iOpenBracketCount[i]=0;
	        }
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
				        case InputType.INPUT_Equals:
                                goto case InputType.INPUT_NULL;
				        case InputType.INPUT_Clear:
                                goto case InputType.INPUT_NULL;
				        case InputType.INPUT_NULL:
					        return false;

				        case InputType.INPUT_Constants:
                            goto case InputType.INPUT_Number;
				        case InputType.INPUT_Bracket:
                            goto case InputType.INPUT_Number;
				        case InputType.INPUT_Function:
                            goto case InputType.INPUT_Number;
				        case InputType.INPUT_Number:
					        //add these too the equation
					        AddOperator(OperatorType.OPP_Times);
                            goto case InputType.INPUT_Operator;
				        case InputType.INPUT_Operator:
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
                            goto case InputType.INPUT_Number;
				        case InputType.INPUT_Constants:
                            goto case InputType.INPUT_Number;
				        case InputType.INPUT_Bracket:
                            goto case InputType.INPUT_Number;
				        case InputType.INPUT_Number:
					        //add these to the equation
					        szEquation[iUserBrackets] +=szLastNumEntered;
					        oCalc.Calculate(szEquation[iUserBrackets], ref szLastNumEntered);
                            szEquation[iUserBrackets] = "";
					        iUserBrackets--;
                            break;
			        }
		        }
                break;
	        }
	        szOppFuncDisplay = CalcGlobals.szBrackets[(int)eBracket];
	        return true;
        }

        public override void SaveState(string name)
        {
            ChainState sState = new ChainState();

            sState.szInput = szInput;
            sState.szOpp = szOpp;

            try
            {
                System.Xml.Serialization.XmlSerializer ser = new System.Xml.Serialization.XmlSerializer(typeof(ChainState));
                TextWriter writer = new StreamWriter(name);
                ser.Serialize(writer, sState);
            }
            catch (System.Exception ex)
            {

            }

            base.SaveState("String"+name);
        }

        public override void RestoreState(string name)
        {
            ChainState sState = new ChainState();

            try
            {
                System.Xml.Serialization.XmlSerializer ser = new System.Xml.Serialization.XmlSerializer(typeof(ChainState));
                TextReader reader = new StreamReader(name);

                //and de serialize data
                sState = (ChainState)ser.Deserialize(reader);

                szInput = sState.szInput;
                szOpp = sState.szOpp;
            }
            catch (System.Exception ex)
            {

            }
            
            base.RestoreState("String"+name);
        }

    }
}
