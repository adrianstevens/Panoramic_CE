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
using System.Collections.Generic;
using System.Linq;

namespace CalcPro
{
/*    class TypeCashFlowItem
    {
        public double dbAmout { get; set; }
        public int iRepetition { get; set; }
    }*/

    class IssCashFlow
    {
     /*   private const int MAX_CASH_FLOWS = 30;
        private const int MAX_REPETITIONS = 20000;

        private double dbCF0;		//Initial Cash Flow				- Enter Only -
        private double dbCnn;		//Amount of the nth Cash flow	- Enter Only - 
        private double dbFnn;			//Freq of nth cash flow			- Enter Only - 
        private double dbI;			//Discount Rate					- Enter Only -
        private double dbNPV;		//Net Present Value				- Compute Only -
        private double dbIRR;		//Internal rate of return		- Compute Olny - 
      //  private double dbSj;

        List<TypeCashFlowItem> arrCashFlows;
        private int iCashFlowIndex;

        public IssCashFlow()
        {
            Clear();
        }

        public void Clear()
        {
            arrCashFlows.Clear();

	        iCashFlowIndex = 0;
	        dbCF0		= 0.0;
	        dbCnn		= 0.0;
	        dbFnn		= 0;
	        dbI		= 0.0;
	        dbNPV		= 0.0;
	        dbIRR		= 0.0;
        }


        public bool SetCashFlow(double dbCashFlow, int iReps )
        {
	        if(iReps < 0 ||
		        iReps > MAX_REPETITIONS)
		        return false;
        
	        TypeCashFlowItem newItem = new TypeCashFlowItem();

            newItem.dbAmout = dbCashFlow;
            newItem.iRepetition = iReps;

            arrCashFlows.Add(newItem);

            iCashFlowIndex = arrCashFlows.Count - 1;

	        return true;
        }


        public bool CalcNPV()
        {
	        return true;
        }*/
    }
}
