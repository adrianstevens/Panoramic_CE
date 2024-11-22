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
    public enum TOTAL_VALUE_TYPE
    {	
	    GET_GRAND_TOTAL,
	    GET_TOTAL_INTEREST,
	    GET_TOTAL_TAXES,
	    GET_TOTAL_OTHER_RECURRING,
	    GET_TOTAL_MORT_INSURANCE,
	    GET_TOTAL_PAYMENTS,
    };

    public enum PAYMENT_SCHEDULE
    {
	    PAYMENT_MONTHLY	= 0,
	    PAYMENT_MONTHLY_ACCELERATED,
	    PAYMENT_BIWEEKLY,
	    PAYMENT_BIWEEKLY_ACCELERATED,
	    PAYMENT_WEEKLY,
	    PAYMENT_WEEKLY_ACCELERATED,
	    PAYMENT_SEMI_MONTHLY,
	    PAYMENT_SEMI_MONTHLY_ACCELERATED,
	    PAYMENT_BIMONTHLY,
	    PAYMENT_BIMONTHLY_ACCELERATED,
    };

    public class IssMortgage
    {
        static int    DEFAULT_TERYEARS = 25;
        static double DEFAULT_PRINCIPLE = 250000.00;
        static double DEFAULT_DOWN_PAYMENT = 0.00;
        static double DEFAULT_INTEREST_NO_PNT = 0.05;
        static double DEFAULT_INTEREST_PNT = 0.0;
        static int    DEFAULT_POINTS_PURCH = 0;
        static double DEFAULT_PROPERTY_TAX = 0.0;
        static double DEFAULT_MORT_INSURANCE = 0.0;
        static double DEFAULT_OTHER_RECURING = 0.0;
        static double DEFAULT_POINT_COST_PERCENTAGE = 1.0;

    //    static int NUM_PAYMENT_SCHEDULES = 10;
        static int NUM_POINT_COSTS = 31;

        public IssMortgage()
        {
            ResetValues(true);
        }

        double		dbPrinciple;					// Total amount being borrowed
	    double		dbInterestRate;				// Amount of interest being charged yearly
	    int			iTerm;						// Total number of years being financed.
	    double		dbDownPayment;				


	    double		dbMonthlyPropertyTaxes;		// Estimation of monthly property tax
	    double		dbMortgageInsurancePerMonth;	// Estimation of the mortgage insurance premiums per month
	    double		dbOtherRecurringCostsPerMonth;// House insurance + Earthquake insur + phone + other stuff 
	    int			iPointsPurchased;				// For more info on points
												    // http://www.getsmart.com/grd/articles/yourmortgage/discount-points.asp?esourceid=25681&source=knowl
	    double		dbInterestRateWithPoints;		// Reduced rate of interest in exchange for buying points.
	    double		dbCostForsPoint;				// Generally 1% of the Principle by industry standards for every point

	    PAYMENT_SCHEDULE	ePaymentType;		

        // Calculated values
	    int			iPayments;					// Number of monthly payments		
	    double		dbPaymentAmount;				// payment amount + interest    
	    double		dbFractionalInterest;			// Interest divided by the number 12 (APR)
	    double		dbCostForsPointPercent;		// Standard is 1.0%, user has option to change.

        // Operational values
        bool        bUsePoints;						// True if points are worked into the calculation..see below for a points link	
        bool        bCanadianMortgage;	

	
	    double		dbAdjustedFractionalInterest;	// When points are used this will store the fractional rate of interest based on the point discount

	    double[]	dbArrPrinciplePerMonth; //= new double[];			// An array to store the amount of Principle paid each month
	    double[]	dbArrPrincipleRemaining; //= new double[];		// An array to store the amount of Principle left to be paid at everymonth


        double  SumUpMortgageInsurancePremiums(){return (double)iPayments * dbMortgageInsurancePerMonth;}
	    double  SumUpTaxes(){return (double)iTerm * 12.0 * dbMonthlyPropertyTaxes;}
	    double	SumUpOtherRecurringCosts(){return (double)iPayments * dbOtherRecurringCostsPerMonth;}




	    public double	GetPointCostInterest(){return dbCostForsPointPercent;}
        public double	GetDownPayment(){return dbDownPayment;}
        public double	GetPrinciple(){return dbPrinciple;}
        public double	GetInterest(){return 100.0*dbInterestRate;}
        public double	GetPointInterest(){return 100.0*dbInterestRateWithPoints;}
        public int		GetTermInYears(){return iTerm;}
        public int		GetNumberOfPayments(){return iPayments;}
        public double	GetPropertyTax(){return dbMonthlyPropertyTaxes;}
        public double   GetMonthlyPaymentAmt(){return CalculateMonthlyPaymentAmt();}
        public double	GetMortgageInsurancePremiums(){return dbMortgageInsurancePerMonth;}
        public double	GetOtherRecurringCosts(){return dbOtherRecurringCostsPerMonth;}
        public int 	    GetPointsPurchased(){return iPointsPurchased;}
        public double	GetCostOfPoints(){return dbCostForsPoint;}


        /********************************************************************

	        Function:	SetDownPayment(double dbDownPayment)

	        Inputs:		dbDownPayment	- This is the total down payment being made.
	        Outputs:	N/A

	        Returns:	true if a valid value was entered( inclusive between 0 and 100%), false otherwise.

	        Comments:	Public

        *********************************************************************/
        public bool SetDownPayment(double dbNewDownPayment)
        {
	        if(dbNewDownPayment < dbPrinciple && dbNewDownPayment >= 0.0)
	        {
		        dbDownPayment = dbNewDownPayment;
		        return true;
	        }
            return false;
        }


        /********************************************************************

	        Function:	SetInterest(double dbInterestRate)

	        Inputs:		dbInterestRate	- This is the interest rate of the mortgage, without points being considered

	        Outputs:	N/A

	        Returns:	true if a valid value was entered( inclusive between 0 and 100%), false otherwise.

	        Comments:	Public

        *********************************************************************/
        public bool SetInterest(double dbNewInterestRate)
        {
	        double dbDivisor;
	        if(dbInterestRate <= 100.0 && dbInterestRate >= 0.0)
	        {
                dbInterestRate = dbNewInterestRate / 100.0;

		        dbDivisor = 12.0;
		
		        dbFractionalInterest	= dbInterestRate / dbDivisor;

		        return true;
	        }

	        return false;
        }


        /********************************************************************

	        Function:	SetPointInterest(double dbInterestRate)

	        Inputs:		dbInterestRate	- This is the interest rate of the mortgage offered by use of points.

	        Outputs:	N/A

	        Returns:	true if a valid value was entered( inclusive between 0 and 100%), false otherwise.

	        Comments:	Public

        *********************************************************************/
        public bool SetPointInterest(double dbInterestRate)
        {
	        double dbDivisor;
	        if(dbInterestRate <= 100.0 && dbInterestRate >= 0.0)
	        {
		        dbInterestRateWithPoints		= dbInterestRate / 100.0;

		        dbDivisor = 12.0;

		        dbAdjustedFractionalInterest	= dbInterestRateWithPoints / dbDivisor;
		        return true;
	        }

	        return false;
	
        }


        /********************************************************************

	        Function:	SetMortgageInsurancePremiums( double dbMortPremiums)

	        Inputs:		dbMortPremiums	- The estimated monthly premiums on your mortgage insurance.
								          this value is not added to principle, just tagged on at the end 
								          for convenience.

	        Outputs:	N/A

	        Returns:	true if a number greater then 0 inclusive is added.

	        Comments:	Public

        *********************************************************************/
        public bool SetMortgageInsurancePremiums(double dbMortPremiums)
        {
	        if (dbMortPremiums >= 0.0)
	        {
		        dbMortgageInsurancePerMonth = dbMortPremiums;
		        return true;
	        }

	        return false;

        }


        /********************************************************************

	        Function:	SetOtherRecurringCosts( double dbOtherRecurringCosts)

	        Inputs:		dbOtherRecurringCosts	- The estimated monthly premiums for other costs you want to budget monthly for 
										          such as electric bill, phone, and misc insurance premiums. Not taken into account for
										          mortgage calculation - just convenience.

	        Outputs:	N/A

	        Returns:	true if a number greater then 0 inclusive is added. false otherwise.
	        Comments:	Public

        *********************************************************************/
        public bool SetOtherRecurringCosts(double dbOtherRecurringCosts)
        {
	        if (dbOtherRecurringCosts >= 0.0)
	        {
		        dbOtherRecurringCostsPerMonth = dbOtherRecurringCosts;
		        return true;
	        }

	        return false;

        }

        /********************************************************************

	        Function:	SetPrinciple( double dbPrinciple)

	        Inputs:		dbPrinciple	- This is the amount of money you are borrowing for the mortgage - do NOT include
							          extras such as insurance and other recurring costs unless this is borrowed money as well (ie part of mortgage at mortgage rate)
							  
	        Outputs:	N/A

	        Returns:	true if a number greater then 0 inclusive is added. false otherwise.

	        Comments:	Public

        *********************************************************************/
        public bool SetPrinciple(double dbNewPrinciple)
        {
            if (dbNewPrinciple >= 0.0)
	        {
                dbPrinciple = dbNewPrinciple;
		        return true;
	        }

	        return false;
	
        }

        /********************************************************************

	        Function:	SetPointsPurchased(int iPoints)

	        Inputs:		byPoints	- This is the number of points purchased to help reduce the overall cost of your mortgage, beneficial
							          for longer term mortgages, not necessarily for short term ones.
							  
	        Outputs:	N/A

	        Returns:	true if a number greater then 0 inclusive is added. false otherwise.

	        Comments:	Public

        *********************************************************************/
        public bool SetPointsPurchased(int byPoints)
        {
	        if (byPoints > 0)
	        {
		        iPointsPurchased = byPoints;
		        bUsePoints		= true;

		        // 1% of Principle by industry standards for every point
		        dbCostForsPoint	= iPointsPurchased * (dbPrinciple * (dbCostForsPointPercent/100.0));
                return true;
	        }

	        return false;
        }


        /********************************************************************

	        Function:	SetTermInYears(int iTerm)

	        Inputs:		iTerm - This is the length of the loan in years
							  
	        Outputs:	N/A

	        Returns:	true if a number greater then 0 inclusive is added. false otherwise.

	        Comments:	Public

        *********************************************************************/
        public bool SetTermInYears(int iNewTerm)
        {


            if (iNewTerm >= 0 && iNewTerm <= 201)
	        {
                iTerm = iNewTerm;

		        return true;
	        }

	        return false;
        }


        /********************************************************************

	        Function:	CalculateMonthlyPaymentAmt(bool bCompoundedSemiAnnualy)

	        Inputs:		N/A
							  
	        Outputs:	N/A

	        Returns:	Error number based on which value was entered incorrectly.

				        1	- Need to enter a valid principle.
				        2	- Need to enter a valid interest rate (both with and without points)
				        3	- Need to enter a valid fractional interest rate - this SHOULD be handled internally, but just in case you have the error.
				        4	- Your not making any payments, ask user how many years payments need to be made for through set and get functions.

	        Comments:	PRIVATE

				        Calculates for either a Canadian (compound semi annually) or American(compound monthly) mortgage

				        Source for formula : http://www.hughchou.org/calc/formula.html
				        Output matches calculations of
				        http://www.mortgage101.com/Calculators/Amortization.asp?p=mtg101
				        http://www.lendingmax.ca/calculator.php - info on Canadian vs American morts

        *********************************************************************/
        double CalculateMonthlyPaymentAmt()
        {
	        // Make sure all needed variables are filled with good values, if not
	        // return a number that can be used to decode which variable needs to be filled
	        // ie maybe pop up a dialog asking for that variable;
	        if(dbPrinciple < 0.0)
		        return 1;
	
	        if (GetAdjustedInterest() < 0.0)
	        {
		        return 2;
	        }
	
	        if(GetAdjustedFractionalInterest() < 0.0)
	        {
		        return 3;
	        }
	
	        if(iPayments < 0)
	        {
		        return 4;
	        }

	        // everything ok.
	        double dbAdjFracInterest	= GetAdjustedFractionalInterest();

	        double dbFinancingAmt		=  GetAdjustedPrinciple();




	        if(bCanadianMortgage)
	        {
		        // switching to canadian interest rates.
		        dbAdjFracInterest = 1200 * (Math.Pow( 1+ (dbAdjFracInterest / 200),(double)(1.0/6.0)) -1.0    );

		        // American formula - works, but we moved it below 
		
		        //dbPaymentAmount = dbFinancingAmt * ( (dbAdjFracInterest) / (1.0 - Math.Pow(( 1.0 + dbAdjFracInterest),-(12.0 * (double)iTerm))) );
	        }
	        // Old Canadian formula that seemed to work below....
	        /*
	        else
	        {
		
		
		        // Canadian formula compounded semi annually - ?? Formula from same source as above
		        double dbAdjustedInterest = GetAdjustedInterest() * 100.0;
		        double dbTop = Math.Pow(1.0 + (dbAdjustedInterest/200.0),(double)(1.0/6.0)) - 1.0;
		        double dbTerm = (double) iTerm;
		        double dbBottom = 1.0 - ( Math.Pow(Math.Pow(1.0 + (dbAdjustedInterest/200.0), (double)(1.0/6.0)),(-12.0 * dbTerm)));
		        double dbAnswer = dbFinancingAmt * (dbTop / dbBottom);
		        dbPaymentAmount = dbAnswer;
		
		
	        }*/

	
	        dbPaymentAmount = dbFinancingAmt * ( (dbAdjFracInterest) / (1.0 - Math.Pow(( 1.0 + dbAdjFracInterest),-(12.0 * (double)iTerm))) );

	        switch(ePaymentType)
	        {
		        case PAYMENT_SCHEDULE.PAYMENT_MONTHLY:
			        return dbPaymentAmount;
			        
		        case PAYMENT_SCHEDULE.PAYMENT_MONTHLY_ACCELERATED:
			        dbPaymentAmount *= 13;
			        dbPaymentAmount /= 12;
			        return dbPaymentAmount;

		        case PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY:
			        dbPaymentAmount *= 12;
			        dbPaymentAmount /= 26;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY_ACCELERATED:
			        dbPaymentAmount *= 13;
			        dbPaymentAmount /= 26;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_WEEKLY:
			        dbPaymentAmount *= 12;
			        dbPaymentAmount /= 52;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_WEEKLY_ACCELERATED:
			        dbPaymentAmount *= 13;
			        dbPaymentAmount /= 26;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY:
			        dbPaymentAmount *= 12;
			        dbPaymentAmount /= 24;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY_ACCELERATED:
			        dbPaymentAmount *= 13;
			        dbPaymentAmount /= 24;
		        break;
		        case PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY:
			        dbPaymentAmount *= 12;
			        dbPaymentAmount /= 6;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY_ACCELERATED:
			        dbPaymentAmount *= 12;
			        dbPaymentAmount /= 6;
		        break;
	        }

	        return dbPaymentAmount;
        }




        /********************************************************************

	        Function:	GetAdjustedFractionalInterest()

	        Inputs:		N/A
							  
	        Outputs:	N/A

	        Returns:	Returns the appropriate value of fractional interest depending on the state of bUsePoints.

	        Comments:	PRIVATE

        *********************************************************************/
        public double GetAdjustedFractionalInterest()
        {
	        if (bUsePoints)
	        {
		        return dbAdjustedFractionalInterest;
	        }
	        else
	        {
		        return dbFractionalInterest;
	        }
        }


        /********************************************************************

	        Function:	GetAdjustedPrinciple()

	        Inputs:		N/A
							  
	        Outputs:	N/A

	        Returns:	Returns the principle, I used this function name to match up with the other
				        "Adjusted*" just so we could see that all the functions are related.

	        Comments:	PRIVATE

        *********************************************************************/
        public double GetAdjustedPrinciple()
        {
	        return dbPrinciple - dbDownPayment;
        }




        /********************************************************************

	        Function:	FillBalanceRemainingTable()

	        Inputs:		N/A
							  
	        Outputs:	N/A

	        Returns:	true if all calculations went ok, false if you haven't set the number of years payments need to be made.

	        Comments:	PRIVATE	-	This function calculates the balance of the loan you have for each month and fills the array
							        dbArrPrincipleRemaining with these values.

        *********************************************************************/
        bool FillBalanceRemainingTable()
        {
	        if (iPayments <= 0)
	        {
		        return false;
	        }

	        dbArrPrincipleRemaining = new double[iPayments];
	
	        double dbMonthInterest = 0.0;

	        dbArrPrincipleRemaining[0] = GetAdjustedPrinciple();


	        for(int i = 1; i < iPayments; i++)
	        {

		        dbMonthInterest = GetMonthInterest(dbArrPrincipleRemaining[i-1]);

		        if( (dbArrPrincipleRemaining[i-1] - dbPaymentAmount + dbMonthInterest) < 0.0)
		        {
			        // No more balance to pay...get out.
			        break;
		        }
		        else
		        {
			        dbArrPrincipleRemaining[i] = dbArrPrincipleRemaining[i-1] - dbPaymentAmount + dbMonthInterest;
		        }

		        /*
		        Old formula .. worked fine for Paid_Monthly, had to change to above for others as payments 
		        were still calculated after "last" payment should have been made.

		        dbArrPrincipleRemaining[i] = dbArrPrincipleRemaining[i-1] - dbPaymentAmount + dbMonthInterest;
		        if(dbArrPrincipleRemaining[i] < 0.0)
		        {
			        dbArrPrincipleRemaining[i] = dbArrPrincipleRemaining[i-1];
			        break;
		        }*/

	        }

	        return true;
        }



        /********************************************************************

	        Function:	FillPrinciplePerMonth()

	        Inputs:		N/A
							  
	        Outputs:	N/A

	        Returns:	true if all calculations went ok, false if you haven't set the number of years payments need to be made.

	        Comments:	PRIVATE	- This value fills up the amount of principle you pay for each month and fills up the array dbArrPrinciplePerMonth with these values.
						          FillBalanceRemainingTable() must be called prior to this, or the values will be incorrect/crash the app.

        *********************************************************************/
        public bool FillPrinciplePerMonth()
        {
	        if (iPayments <= 0)
	        {
		        return false;
	        }

	        dbArrPrinciplePerMonth = new double[iPayments];
	        double dbMonthInterest = 0.0;

	        for(int i = 0; i < iPayments; i++)
	        {
		        dbMonthInterest = GetMonthInterest(dbArrPrincipleRemaining[i]);
		        dbArrPrinciplePerMonth[i] = dbPaymentAmount - dbMonthInterest;
	
		        // Don't process any more after this, we have no payments left...
		        if(dbMonthInterest == 0.0)
		        {
			        dbArrPrinciplePerMonth[i] = 0.0;
			        break;
		        }
	        }

	        return true;
        }


        /********************************************************************

	        Function:	GetAdjustedInterest()

	        Inputs:		N/A
							  
	        Outputs:	N/A

	        Returns:	Returns the appropriate value of interest depending on the state of bUsePoints.

	        Comments:	PRIVATE

        *********************************************************************/
        public double GetAdjustedInterest()
        {
	        if (bUsePoints)
	        {
		        return dbInterestRateWithPoints;
	        }
	        else
	        {
		        return dbInterestRate;
	        }
        }



        /********************************************************************

	        Function:	SumUpGrandTotal()

	        Inputs:		N/A
							  
	        Outputs:	N/A

	        Returns:	Returns the grand total of the cost of ownership of the home over the set number of years in the mortgage with
				        optional elements included if they are supposed to be visible.

	        Comments:	PRIVATE

        *********************************************************************/
        double SumUpGrandTotal()
        {
	        double dbInterestPaid;

	        dbInterestPaid = SumUpInterest();

	        double dbGrandTotal;

	        dbGrandTotal = 	dbInterestPaid + dbPrinciple;
	
	        dbGrandTotal += SumUpMortgageInsurancePremiums() + 
					        SumUpOtherRecurringCosts() + 
					        SumUpTaxes();

	        return ( dbGrandTotal);
        }


        double SumUpTotalPayments()
        {
	        if(dbDownPayment == 0)
		        return SumUpGrandTotal();


	        double dbInterestPaid;

	        dbInterestPaid = SumUpInterest();

	        double dbGrandTotal;

	        dbGrandTotal = 	dbInterestPaid + GetAdjustedPrinciple();
	
	        //dbGrandTotal += SumUpMortgageInsurancePremiums() + 
	        //				SumUpOtherRecurringCosts() + 
	        //				SumUpTaxes();

	        return ( dbGrandTotal);
        }

        /********************************************************************

	        Function:	SumUpInterest()

	        Inputs:		N/A
							  
	        Outputs:	N/A

	        Returns:	Returns the grand total of all the interest paid on the mortgage component of the numbers entered.

	        Comments:	PRIVATE

        *********************************************************************/
        double SumUpInterest()
        {
	        double dbTotal = 0.0;

	        for(int i = 0; i < iPayments; i++)
	        {
		        dbTotal	+= GetMonthInterest(dbArrPrincipleRemaining[i]);
	        }

	        return dbTotal;
        }


        /********************************************************************

	        Function:	GetMontthInterest(double dbBalance)

	        Inputs:		dbBalance -	Pass in the balance of the mortgage remaining.
							  
	        Outputs:	N/A

	        Returns:	Returns the total of interest paid for the month.

	        Comments:	PRIVATE

        *********************************************************************/
        public double GetMonthInterest(double dbBalance)
        {
	        double dbInterestCharged;
	        double dbAdjustedRate = GetAdjustedInterest();
	        double dbDivisor = 0.0;

	        switch(ePaymentType)
	        {
		        case PAYMENT_SCHEDULE.PAYMENT_MONTHLY:
		        case PAYMENT_SCHEDULE.PAYMENT_MONTHLY_ACCELERATED:
			        dbDivisor = 12.0;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY:
		        case PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY_ACCELERATED:
			        dbDivisor = 26.0;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_WEEKLY:
		        case PAYMENT_SCHEDULE.PAYMENT_WEEKLY_ACCELERATED:
			        dbDivisor = 52.0;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY:
		        case PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY_ACCELERATED:
			        dbDivisor = 24.0;
		        break;
		        case PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY:
		        case PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY_ACCELERATED:
			        dbDivisor = 6.0;
		        break;
	        }

	        dbAdjustedRate = dbAdjustedRate / dbDivisor;
	
	        dbInterestCharged = dbBalance * dbAdjustedRate;

	        return  (dbInterestCharged);
        }



        /********************************************************************

	        Function:	FillTables()

	        Inputs:		N/A
							  
	        Outputs:	N/A

	        Returns:	true if the application was able to fill the values in each of the tables.

	        Comments:	PUBLIC

        *********************************************************************/
        public bool FillTables()
        {
	        SetPaymentSchedule(ePaymentType);
	        CalculateMonthlyPaymentAmt();

	        if	(	FillBalanceRemainingTable() &&
			        FillPrinciplePerMonth()
		        )
	        {
		        return true;
	        }

	        return false;
        }



        /********************************************************************

	        Function:	GetBalanceRemainingForMonth(int iZeroBasedIndex)

	        Inputs:		iZeroBasedIndex - Index within the array dbArrPrincipleRemaining
								          for which you wish to return.
							  
	        Outputs:	N/A

	        Returns:	-911.0 if value could not be found/array not initialized (pointer still NULL)
				        Otherwise, the balance for the month in double format is returned.

	        Comments:	PUBLIC

        *********************************************************************/
        public double GetBalanceRemainingForMonth(int iZeroBasedIndex)
        {
	        if(dbArrPrincipleRemaining != null && iZeroBasedIndex <= iPayments && iZeroBasedIndex >= 0) 
	        {
		        return dbArrPrincipleRemaining[iZeroBasedIndex];
	        }
	
	        return -911.0;
        }




        /********************************************************************

	        Function:	GetPrincipleForMonth(int iZeroBasedIndex)

	        Inputs:		iZeroBasedIndex - Index within the array dbArrPrinciplePerMonth
								          for which you wish to return.
							  
	        Outputs:	N/A

	        Returns:	-911.0 if value could not be found/array not initialized (pointer still NULL)
				        Otherwise, the balance for the month in double format is returned.

	        Comments:	PUBLIC

        *********************************************************************/
        public double GetPrincipleForMonth(int iZeroBasedIndex)
        {
	        if(dbArrPrinciplePerMonth.Length > 0 && iZeroBasedIndex <= iPayments  && iZeroBasedIndex >= 0) 
	        {
		        return dbArrPrinciplePerMonth[iZeroBasedIndex];
	        }
	
	        return -911.0;
        }




        /********************************************************************

	        Function:	SetPropertyTax(double fTax)

	        Inputs:		fTax - true to enable Full Amortization Reports
							  
	        Outputs:	N/A

	        Returns:	true if valid property tax assigned

	        Comments:	PUBLIC

        *********************************************************************/
        public bool SetPropertyTax(double fTax)
        {
	        if(fTax >= 0.0)
	        {
		        dbMonthlyPropertyTaxes = fTax;
		        return true;
	        }
	
	        return false;
        }




        /********************************************************************

	        Function:	GetTotal(TOTAL_VALUE_TYPE eValueType)

	        Inputs:		eValueType - Type of total you want to get back
							         ie. TOTAL_VALUE_TYPE.GET_GRAND_TOTAL to get back the total
								         of all money paid.
							  
	        Outputs:	N/A

	        Returns:	The desired sum in double format, or -1 if a invalid 
				        total type is requested.

	        Comments:	PUBLIC

        *********************************************************************/
        public double GetTotal(TOTAL_VALUE_TYPE eValueType)
        {
	        switch(eValueType)
	        {
	        case TOTAL_VALUE_TYPE.GET_GRAND_TOTAL:
		        return SumUpGrandTotal();
	        case TOTAL_VALUE_TYPE.GET_TOTAL_PAYMENTS:
		        return SumUpTotalPayments();
	        case TOTAL_VALUE_TYPE.GET_TOTAL_INTEREST:
		        return SumUpInterest();
	        case TOTAL_VALUE_TYPE.GET_TOTAL_TAXES:
		        return SumUpTaxes();
	        case TOTAL_VALUE_TYPE.GET_TOTAL_OTHER_RECURRING:
		        return SumUpOtherRecurringCosts();
	        case TOTAL_VALUE_TYPE.GET_TOTAL_MORT_INSURANCE:
		        return SumUpMortgageInsurancePremiums();
	        default:
		        return -1;
	        }
        }




        /********************************************************************

	        Function:	CostRecoveryTimeForPoints()

	        Inputs:		dbSavings - The amount of savings you are getting by
				        paying for points to reduce interest cost. (monthly savings)
							  
	        Outputs:	N/A

	        Returns:	The amount of time you need to keep your house for
				        it to be worth buying points.

				

	        Comments:	PUBLIC

        *********************************************************************/
        public double CostRecoveryTimeForPoints(double dbSavings)
        {
	        if(bUsePoints && dbCostForsPoint > 0.0)
	        {
		        return dbCostForsPoint / dbSavings;
	        }

	        return 0.0;

        }

        /********************************************************************

	        Function:	Restart()

	        Purpose:	Call this function to reset everything to a known state.
				        Registry reads are necessary - if values can't be found in 
				        registry, defaults are assigned.
				

	        Inputs:		N/A
							  
	        Outputs:	N/A

	        Returns:	Nothing.

				

	        Comments:	PUBLIC

        *********************************************************************/
        public void Restart()
        {
            Array.Clear(dbArrPrinciplePerMonth, 0, dbArrPrinciplePerMonth.Length);
            Array.Clear(dbArrPrincipleRemaining, 0, dbArrPrincipleRemaining.Length);
            
	        ResetValues(false);
        }

        /********************************************************************

	        Function:	GetComparisonTerm()

	        Purpose:	Given the $$ value of your payments, your payment schedule
				        and the interest percentage charged on a loan...this function
				        will figure out how long it will take to make all payments for your
				        mortgage.
				

	        Inputs:		N/A
							  
	        Outputs:	N/A

	        Returns:	The number of years needed to pay off your entire mortgage.

				

	        Comments:	PUBLIC

        *********************************************************************/
        public double GetComparisonTerm()
        {
	        double dbDivisor = 1.0;
	        switch(ePaymentType)
	        {
		        case PAYMENT_SCHEDULE.PAYMENT_MONTHLY:
			        dbDivisor = 12.0;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY:
		        case PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY_ACCELERATED:
			        dbDivisor = 26.0;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_WEEKLY:
		        case PAYMENT_SCHEDULE.PAYMENT_WEEKLY_ACCELERATED:
			        dbDivisor = 52.0;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY:
		        case PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY_ACCELERATED:
			        dbDivisor = 24.0;
		        break;
		        case PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY:
		        case PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY_ACCELERATED:
			        dbDivisor = 6.0;
		        break;
	        }

	        double dbResult;
	        double dbFractionInterest = GetAdjustedInterest() / dbDivisor;
	        dbResult = Math.Log( dbPaymentAmount / dbPaymentAmount - (dbPrinciple * dbFractionInterest) / Math.Log(1 + dbFractionInterest));
	        dbResult /= 12.0;
	
	        return dbResult;
        }

        /********************************************************************

	        Function:	SetPointCostInterest()

	        Purpose:	Some companies charge a rate that is different from the industry
				        norm of 1% of mortgage to purchase a point.  Use this function 
				        to change that amount.
				

	        Inputs:		dbPointCostPercent - Percentage of loan required to pay to purchase a point.
							  
	        Outputs:	N/A

	        Returns:	true if a valid value was entered, false otherwise.

				

	        Comments:	PUBLIC

        *********************************************************************/
        public bool SetPointCostInterest(double dbPointCostPercent)
        {
	        if(dbPointCostPercent <= 100.0 && dbPointCostPercent >= 0.0)
	        {
		        dbCostForsPointPercent = dbPointCostPercent;
		        return true;
	        }

	        return false;
        }


        /********************************************************************

	        Function:	SetPaymentSchedule(PAYMENT_SCHEDULE eSchedule)

	        Purpose:	Payment schedule assignment.  Can be one of the following
					        PAYMENT_SCHEDULE.PAYMENT_MONTHLY,
					        PAYMENT_SCHEDULE.PAYMENT_MONTHLY_ACCELERATED,
					        PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY,
					        PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY_ACCELERATED,
					        PAYMENT_SCHEDULE.PAYMENT_WEEKLY,
					        PAYMENT_SCHEDULE.PAYMENT_WEEKLY_ACCELERATED,
					        PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY,
					        PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY_ACCELERATED,
					        PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY,
					        PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY_ACCELERATED.

				        The class defaults to PAYMENT_SCHEDULE.PAYMENT_MONTHLY if a value can't be read from
				        the registry, or assigned here.

	        Inputs:		eSchedule - The desired payment schedule.
							  
	        Outputs:	N/A

	        Returns:	true if a valid value was entered, false otherwise.

				

	        Comments:	PUBLIC

        *********************************************************************/
        public void SetPaymentSchedule(PAYMENT_SCHEDULE eSchedule)
        {
	        int iPaymentsPerYear = 12;

	        ePaymentType = eSchedule;

	        switch(ePaymentType)
	        {
		        case PAYMENT_SCHEDULE.PAYMENT_MONTHLY:
			        // Already initialized
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY:
		        case PAYMENT_SCHEDULE.PAYMENT_BIWEEKLY_ACCELERATED:
			        iPaymentsPerYear = 26;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_WEEKLY:
		        case PAYMENT_SCHEDULE.PAYMENT_WEEKLY_ACCELERATED:
			        iPaymentsPerYear = 52;
			        break;
		        case PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY:
		        case PAYMENT_SCHEDULE.PAYMENT_SEMI_MONTHLY_ACCELERATED:
			        iPaymentsPerYear = 24;
		        break;
		        case PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY:
		        case PAYMENT_SCHEDULE.PAYMENT_BIMONTHLY_ACCELERATED:
			        iPaymentsPerYear = 6;
		        break;
	        }
	
	        iPayments = iTerm * iPaymentsPerYear;
        }

        public bool GetPointCostsLabel(int iPoint, ref string szLabel)
        {
	        if(iPoint < 0 || iPoint > NUM_POINT_COSTS)
		        return false;
	
	        double dbTemp = 0.0;
		
	        dbTemp = (double)iPoint/10.0;

            szLabel = String.Format("{0.00}", dbTemp);

            return true;
            //return oStr->DoubleToString(szLabel, dbTemp, 2);
        }

        public void ResetValues(bool bClearAll)
        {
            if (dbArrPrinciplePerMonth != null)
                Array.Clear(dbArrPrinciplePerMonth, 0, dbArrPrinciplePerMonth.Length);
            //Array.Clear(dbArrPrinciplePerMonth, 0, dbArrPrinciplePerMonth.Length);
	
	        bUsePoints					    = false;

	        dbPrinciple					    = DEFAULT_PRINCIPLE;
	        dbInterestRate				    = DEFAULT_INTEREST_NO_PNT;

	        iTerm							= DEFAULT_TERYEARS;
            dbFractionalInterest            = DEFAULT_INTEREST_PNT;
	        dbPaymentAmount				    = 0.0;
	        iPayments						= 0;

	        dbMonthlyPropertyTaxes		    = DEFAULT_PROPERTY_TAX;		
	        dbMortgageInsurancePerMonth	    = DEFAULT_MORT_INSURANCE;	
	        dbOtherRecurringCostsPerMonth	= DEFAULT_OTHER_RECURING;
	        iPointsPurchased				= DEFAULT_POINTS_PURCH;

	        dbInterestRateWithPoints		= 0.0;
	        dbCostForsPoint				    = 0;

	        dbAdjustedFractionalInterest	= 0.0;

            dbDownPayment                   = DEFAULT_DOWN_PAYMENT;


	        if(bClearAll)
	        {
		        dbCostForsPointPercent		= DEFAULT_POINT_COST_PERCENTAGE;
		        bCanadianMortgage				= false;
		        ePaymentType					= PAYMENT_SCHEDULE.PAYMENT_MONTHLY;
	        }

            SetInterest(dbInterestRate*100.0);
        }
    }
}
