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
    public enum SolveType
    {
        SOLVE_IY,
        SOLVE_PMT,
        SOLVE_N,
        SOLVE_FV,
        SOLVE_PV,
        SOLVE_Count,
    };

    public class IssTVM
    {
        public IssTVM()
        {
        }

        // Core TVM Variables
        int iNumPeriods;	//Number of Periods			- Enter/Compute
        double dbIY;			//Interest Rate per Year	- Enter/Compute		*Nominal
        double dbPV;			//Present Value				- Enter/Compute
        double dbPMT;		//Payment					- Enter/Compute
        double dbFV;			//Future Value				- Enter/Compute
        //////////////////////

        int iPymntsYear;	//Payments Per Year			- Enter Only
        int iCompYear;	//# Compounding pmt/year	- Enter Only


        // Settings
        bool bBeggingOfPeriodPMTS; //toggled end or beg- Setting
        ///////////

        // Amortization
        int iPayment1;	//Starting Payment			- Enter Only
        int iPayment2;	//Ending Payment			- Enter Only
        double dbBalance;	//Balance					- Compute
        double dbPRN;		//Principle Paid			- Compute
        double dbInterest;	//Interest Paid				- Compute
        ///////////////


        //Temp for TVM
        double dbi;			//Temp variable, i chosen from TI manual
        double dbGi;			//Temp variable
        SolveType eSolve;		//whatever we're solving for


        public void SetBegYearPayments(bool bSet) { bBeggingOfPeriodPMTS = bSet; }

        	//Amoritization
        public void Set1stPayment(int iIndex) { iPayment1 = iIndex; }
        public void SetlastPayment(int iIndex) { iPayment2 = iIndex; }

        public int GetNumberOfPeriods() { return iNumPeriods; }
        public double GetInterestRateYear() { return dbIY; }
        public double GetPresentValue() { return dbPV; }
        public double GetPayment() { return dbPMT; }
        public double GetFutureValue() { return dbFV; }
        public int GetPaymentsPerYear() { return iPymntsYear; }
        public int GetCompPerYear() { return iCompYear; }

        public bool ClearEndPayment() { bBeggingOfPeriodPMTS = false; return true; }//Clears the end payment	




        bool ClearTVMVariables()
        {
	        iNumPeriods	    = 1;
	        iPymntsYear	    = 12;
	        iCompYear		= 12;
	        dbIY			= 0.0;
	        dbPV			= 0.0;
	        dbPMT			= 0.0;
	        dbFV			= 0.0;

	        return true;
        }

        bool ClearWorkSheet()
        {
	        iPymntsYear	= 12;
	        iCompYear	= 12;

	        return true;
        }

        bool ClearAmortWorkSheet()
        {
	        iPayment1		= 1;
	        iPayment2		= 1;
	        dbBalance		= 0.0;
	        dbPRN			= 0.0;
	        dbInterest	= 0.0;

	        return true;
        }

        public bool Reset()
        {
	        if(!ClearTVMVariables())
		        return false;
	        if(!ClearWorkSheet())
		        return false;
	        if(!ClearAmortWorkSheet())
		        return false;
	        if(!ClearEndPayment())
		        return false;

	        dbGi	= 1;
	        dbi	= 0;
	
	        return true;
        }

        public bool SetFutureValue(double dbFValue)
        {
	        dbFV = dbFValue;
	        return true;
        }

        //Set Annual rate and we'll divide by the number of Compound periods
        public bool SetInterestRateYear(double dbNewIY)
        {
            dbIY = dbNewIY;
	        return true;
        }

        public bool SetNumberOfPeriods(int iNewNumPeriods)
        {
            if (iNewNumPeriods < 1)
		        return false;
            iNumPeriods = iNewNumPeriods;
	        return true;
        }



        public bool SetPayment(double dbPayment)
        {
        //	if(dbPayment < 0)
        //		return false;
	        dbPMT = dbPayment;
	        return true;
        }

        public bool SetPresentValue(double dbValue)
        {
	        dbPV = dbValue;
	        return true;
        }

        public bool SetPaymentsPerYear(int iPY)
        {
	        if(iPY < 1)
		        return false;
	        iPymntsYear	= iPY;
	        iCompYear		= iPY;
	        return true;
        }

        public bool SetxPY(int ixPY)
        {
	        if(ixPY < 1)
		        return false;
	        iNumPeriods = iPymntsYear * ixPY;

	        return true;
        }


        public bool SetCompPMTPerYear(int iCY)
        {
	        if(iCY < 0)
		        return false;
	        iCompYear		= iCY;
	        return true;
        }

        public bool CalcFutureValue()
        {
	        eSolve = SolveType.SOLVE_FV;
	        CalculateTemp();
	        if(dbi == 0.0)
	        {
		        dbFV = -1.0*(dbPV + dbPMT * iNumPeriods);
	        }
	        else
	        {
        //		double dbTemp1 = dbPMT * dbGi / dbi + dbPV;
        //		double dbTemp2 = Math.Pow(1.0+dbi, iNumPeriods);
        //		double	dbTemp3 = dbPMT * dbGi / dbi;

		        double dbTemp1 = dbPMT * dbGi / dbi + dbPV;
		        double dbTemp2 = Math.Pow(1.0+dbi, iNumPeriods);
		        double	dbTemp3 = dbPMT * dbGi / dbi;

                //changed to negative April 13, 2010
		        dbFV = -1*(dbTemp3 - (dbTemp1 * dbTemp2)); 
                dbFV = (dbTemp3 - (dbTemp1 * dbTemp2)); 
	        }

	        return true;
        }

        public bool CalcInterestRateYear()
        {
	        eSolve = SolveType.SOLVE_IY;

        /*    CalculateTemp();
            double dbY		= (double)iPymntsYear/(double)iCompYear;
            double dbTemp	= dbY * Math.Log(dbi + 1); 
            dbIY = 100 * iCompYear * (Math.Pow(CalcGlobals.CONST_NUM_e, dbTemp) - 1);*/


	        //I think TI is wrong.......
	        if(dbPMT==0.0 &&
		        dbi == 0.0)
	        {
                double dbTemp = Math.Log(-1.0 * dbFV / dbPV) / iNumPeriods;
		        dbIY = iCompYear*100*(Math.Pow((double)CalcGlobals.CONST_NUM_e, dbTemp) - 1);
	        }

	        else //this likely wrong....we'll take a look later
	        {
		        CalculateTemp();
		        double dbY		= (double)iPymntsYear/(double)iCompYear;
		        double dbTemp	= dbY * Math.Log(dbi + 1); 
		        dbIY				= 100.0 * iCompYear * ( Math.Pow((double)CalcGlobals.CONST_NUM_e, dbTemp) - 1);
	        }

	        return true;
        }

        public bool CalcNumberOfPeriods()
        {
	        eSolve = SolveType.SOLVE_N;
	        CalculateTemp();
	        if(dbi == 0.0)
	        {
		        iNumPeriods		= (int)(-1.0*(dbPV+dbFV)/dbPMT);
	        }
	        else
	        {
		        double dbTemp	= (dbPMT*dbGi - dbFV*dbi)/(dbPMT*dbGi + dbPV*dbi);
		        iNumPeriods		= (int)(Math.Log(dbTemp)/Math.Log(dbi + 1));
	        }
	        return true;
        }

        public bool CalcPayment()
        {
	        eSolve = SolveType.SOLVE_PMT;
	        CalculateTemp();
	        if(dbi == 0.0)
	        {
		        dbPMT = -1.0*(dbPV + dbFV)/(double)iNumPeriods;
	        }
	        else
	        {
		        double dbTemp = dbPV + (dbPV + dbFV)/(Math.Pow(1.0+dbi,iNumPeriods)-1);
		        dbPMT	= -1.0*dbi/dbGi*dbTemp;

	        }
	        return true;
        }

        public bool CalcPresentValue()
        {
	        eSolve = SolveType.SOLVE_PV;
	        CalculateTemp();
	        if(dbi == 0.0)
	        {
		        dbPV = -1.0*(dbFV + dbPMT * iNumPeriods);
	        }
	        else
	        {
		        double dbTemp1 = dbPMT * dbGi / dbi - dbFV;
		        double dbTemp2 = 1.0/Math.Pow(1.0+dbi, iNumPeriods);
		        double	dbTemp3 = dbPMT * dbGi / dbi;

		        dbPV = dbTemp1 * dbTemp2 - dbTemp3;

	        }
	        return true;
        }


        //Sets two intermediate variables for TMV calculations
        // i & G
        bool CalculateTemp()
        {
	        if(eSolve == SolveType.SOLVE_IY) //gotta iterate to solve for i from all other terms
							        //works even if payment is zero, just a big chunk of the eq goes to 0
		        dbi				= SolveForI(dbPMT, bBeggingOfPeriodPMTS, dbFV, dbPV, iNumPeriods);
	        else if(eSolve == SolveType.SOLVE_PMT ||
		        dbPMT != 0.0) //more complex, paying off debt
	        {
		        double dbY		= (double)iCompYear/(double)iPymntsYear;
		        double dbX		= 0.01*dbIY / (double)iCompYear;
		        double dbTemp	= dbY*Math.Log(dbX+1);

		        dbi				= Math.Pow(CalcGlobals.CONST_NUM_e, dbTemp) - 1;
	        }
	        else //simple ... ie compound interest
		        dbi				= 0.01*dbIY / iPymntsYear;
	
	        if(bBeggingOfPeriodPMTS)
		        dbGi	= 1.0 + dbi;
	        else
		        dbGi	= 1.0;
	


	        return true;
        }

        bool CalculateAmort()
        {
	        //let bal(0) = RND(PV) .... why round???
	        dbBalance = dbPV;

	        double dbIm=0;
	        double dbBalPay1=0.0;


	        for(int m = 1; m < iPayment2 + 1; m++)
	        {
		        dbIm = -1.0 * dbi * (m - 1);
		        dbBalance = dbBalance - dbIm + dbPMT;
		        if(m == iPayment1)
			        dbBalPay1 = dbBalance; //Save balance at P1
	        }

	        dbPRN = dbBalance - dbBalPay1;

	        dbInterest = (iPayment2 - iPayment1 + 1) * dbPMT - dbPRN;

	        return true;
        }


        double SolveForI(double dbPayment, bool payAtStart, double dbFV, double dbPV, int iPeriods)
        {
	        double dbInterestLeft = Math.Pow(10.0, -10);
            double dbInterestRight = Math.Pow(10.0, 6);
	        double dbEpsilon = 0.00000000005;
	
	        while ( dbInterestRight - dbInterestLeft > dbEpsilon)
	        {
		        double dbMidPoint = (dbInterestRight + dbInterestLeft) / 2;
		        double dbIWannaKnow = FofX(dbPayment, payAtStart, dbFV, dbPV, iPeriods, dbMidPoint);
		
		        if ( FofX(dbPayment, payAtStart, dbFV, dbPV, iPeriods, dbInterestRight) *
			        FofX(dbPayment, payAtStart, dbFV, dbPV, iPeriods, dbInterestLeft) > 0)
		        {
			        //THIS IS SHIT....TOTAL SHIT....and there's NO WAY TO RECOVER because......
			        return 0;
			        
		        }
		        else if ( FofX(dbPayment, payAtStart, dbFV, dbPV, iPeriods, dbInterestRight) *
			        FofX(dbPayment, payAtStart, dbFV, dbPV, iPeriods, dbMidPoint) > 0)
		        {
			        dbInterestRight = dbMidPoint;
		        }
		        else //we've surrounded the solution
		        {
			        dbInterestLeft = dbMidPoint;
		        }
	        }
           return (dbInterestRight + dbInterestLeft) / 2;
        }


        double FofX(double dbPayment, bool payAtStart, double dbFV, double dbPV, int iPeriods, double dbInterestRate)
        {
	        double dbG = 1;
	        double dbTemp = (1-Math.Pow((1+dbInterestRate), iPeriods*-1))/dbInterestRate;
	        if (payAtStart)
	        {
		        dbG = (1 + dbInterestRate);
	        }
	        return dbPV + dbG*dbPayment*dbTemp + dbFV*Math.Pow((1 + dbInterestRate),-1.0*(double)iPeriods);
        }

    }
}
