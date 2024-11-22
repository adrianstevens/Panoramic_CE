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
    public class IssOOOCalc: IssDALCalc
    {
        public override bool AddNumber(NumberType eNumber)
        {
	        //DAL accepts numbers after functions....OOO doesn't...fail or move on
	        if(eLastInputType==InputType.INPUT_Function)
		        return false;
	        else 
		        return base.AddNumber(eNumber);
        }


        public override bool AddFunction(FunctionType eFunction)
        {
	        switch(eFunction) 
	        {
	        case FunctionType.FUNCTION_Percentage:
	        {
                string szTemp = null;


                int iLen = szEquation[iUserBrackets].Length;
		        if(iLen>0 &&
			        (szEquation[iUserBrackets][iLen-1] == ('+') || szEquation[iUserBrackets][iLen-1] == ('-')) &&
			        eLastInputType == InputType.INPUT_Number)
		        {
                    szTemp = szPreviousNumber + CalcGlobals.szOperators[(int)OperatorType.OPP_Times] + szLastNumEntered + CalcGlobals.szOperators[(int)OperatorType.OPP_Times] + "0.01";
			        oCalc.Calculate(szTemp, ref szLastNumEntered);
		        }
		        else
		        {
                    szTemp = szLastNumEntered + CalcGlobals.szOperators[(int)OperatorType.OPP_Times] + "0.01";
			        oCalc.Calculate(szTemp, ref szLastNumEntered);
		        }
		        return true;
	        }
	        
	        case FunctionType.FUNCTION_DRG:
		        oCalc.ConvertDRG(ref szLastNumEntered, ref szLastNumEntered);
                return oCalc.SetDRGState(DRGStateType.DRG_Next);
                
	        case FunctionType.FUNCTION_DRGN:
		        return oCalc.SetDRGState(DRGStateType.DRG_Next);
		        
	        }
	
	        return PostFunction(eFunction);
        }

        public override bool AddOperator(OperatorType eOperator)
        {
	        //DAL fails operators after functions....but we don't want to 
	        //so we set last in as a number...and let er rip
	        if(eLastInputType==InputType.INPUT_Function)
		        eLastInputType=InputType.INPUT_Number;
	        return base.AddOperator(eOperator);
        }

    }
}
