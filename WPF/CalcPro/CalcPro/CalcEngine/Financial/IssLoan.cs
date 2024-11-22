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
    public class IssLoan
    {
        static double MAX_VALUE	= 100000000.0;

        int iMonthsLeft;

        public bool bFullInterest = false;

        double dbCarPrice;
        double dbSalesTaxRate;
        double dbSalesTax;
        double dbRegistrationFees;
        double dbOtherFees;
        double dbDownPayment;
        double dbRebate;
        double dbTradeIn;
        double dbNoOfMonths;
        double dbInterestRate;
        double dbAmountBorrowed;
        double dbTotalInterest;
        double dbTotalCostOfCar;
        double dbMonthlyPayment;
        //double dbAmountOwing;


        /********************************************************************
	        Function:	ResetValues

	        Arguments:	N/A	

	        Returns:	true

	        Comments:	Returns the class to a known initial state.
        *********************************************************************/
        public bool ResetValues()
        {
	         iMonthsLeft		=0;
	         dbCarPrice			=0;
	         dbSalesTaxRate		=0;
	         dbSalesTax			=0;
	         dbRegistrationFees	=0;
	         dbOtherFees		=0;
	         dbDownPayment		=0;
	         dbRebate			=0;
	         dbTradeIn			=0;
	         dbNoOfMonths		=0;
	         dbInterestRate		=0;
	         dbAmountBorrowed	=0;
	         dbTotalInterest	=0;
	         dbTotalCostOfCar	=0;
	         dbMonthlyPayment	=0;
	         //dbAmountOwing		=0;

	         return true;
        }

        /********************************************************************
	        Function:	SetCarPrice

	        Arguments:				

	        Returns:	true if a good car price was entered, false otherwise.

	        Comments:	Assigns the price of the car if a good value was entered.
        *********************************************************************/
        public bool SetCarPrice(double dbNewCarPrice)
        {
            if (!IsDoubleInRange(dbNewCarPrice))
		        return false;

            dbCarPrice = dbNewCarPrice;
	        return true;
        }

        public bool SetMonthlyPayments(double dbPayment)
        {  
            if(!IsDoubleInRange(dbPayment))
                return false;

            dbMonthlyPayment = dbPayment;
            return true;
        }

        /********************************************************************
	        Function:	GetCarPrice()

	        Arguments:	szCarPrice - ref string to a position in memory that is able to 
				        accept the price of the car.

	        Returns:	Double format of the car price.

	        Comments:	Returns the price of the car in TCHAR and a double.
				
        *********************************************************************/
        public double GetCarPrice(ref string szCarPrice)
        {
            if(szCarPrice != null)
	            DoubleToCurrency(dbCarPrice, ref szCarPrice);

	        return dbCarPrice;
        }


        /********************************************************************
	        Function:	SetSalesTaxPcnt

	        Arguments:	dbSalesTax - The desired sales tax percentage

	        Returns:	true if a valid sales percentage was entered, false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetSalesTaxPcnt(double dbSalesTax)
        {
	        if(!IsDoubleInRange(dbSalesTax))
		        return false;	

	        dbSalesTaxRate = dbSalesTax;
	        //dbSalesTax = dbSalesTax;

	        return true;
        }


        /********************************************************************
	        Function:	GetSalesTaxPcnt(ref string szSalesTaxPercent)

	        Arguments:	szSalesTaxPercent - A ref string able to accept a formatted
				        string of the current sales tax percentage.

	        Returns:	The entered sales tax in double format.

	        Comments:	
        *********************************************************************/
        public double GetSalesTaxPcnt(ref string szSaleTaxPercent)
        {
            if(szSaleTaxPercent != null)
	            DoubleToCurrency(dbSalesTaxRate, ref szSaleTaxPercent);

	        return dbSalesTaxRate;
        }

        /********************************************************************
	        Function:	GetSalesTax(ref string szSalesTax)

	        Arguments:	szSalesTax - A ref string of a formatted
				        string of the current sales tax.

	        Returns:	The entered sales tax in double format.

	        Comments:	
        *********************************************************************/
        double GetSalesTax(ref string szSalesTax)
        {
	        DoubleToCurrency(dbSalesTax, ref szSalesTax);

	        return dbSalesTax;
        }

        /********************************************************************
	        Function:	GetMonthlyPayments(ref string szMonthlyPayments)

	        Arguments:	szMonthlyPayments - A ref string of a formatted
				        string able to accept the current monthly payments.

	        Returns:	The monthly payment in double format.

	        Comments:	
        *********************************************************************/
        public double GetMonthlyPayments(ref string szMonthlyPayments)
        {
            if(szMonthlyPayments != null)
	            DoubleToCurrency(dbMonthlyPayment, ref szMonthlyPayments);

	        return dbMonthlyPayment;
        }

        /********************************************************************
	        Function:	GetTotalCost(ref string szTotalCost)

	        Arguments:	szTotalCost - A ref string of a formatted
				        string able to accept the total of all monthly payments.

	        Returns:	The total of all payments in double format.

	        Comments:	
        *********************************************************************/
        public double GetTotalCost(ref string szTotalCost)
        {
            if(szTotalCost != null)
	            DoubleToCurrency(dbTotalCostOfCar, ref szTotalCost);
	
	        return dbTotalCostOfCar;
        }

        /********************************************************************
	        Function:	GetTotalInterest(ref string szTotalInterest)

	        Arguments:	szTotalCost - A ref string of a formatted
				        string able to accept the total of all interest payments.

	        Returns:	The total of all interest payments in double format.

	        Comments:	
        *********************************************************************/
        public double GetTotalInterest(ref string szTotalInterest)
        {
            if(szTotalInterest != null)
	            DoubleToCurrency(dbTotalInterest, ref szTotalInterest);
	
	        return dbTotalInterest;
        }

        /********************************************************************
	        Function:	GetAmountBorrowed(ref string szAmountBorrowed)

	        Arguments:	szAmountBorrowed - A ref string of a formatted
				        string able to accept the total of all money borrowed.

	        Returns:	The total of all money borrowed in double format.

	        Comments:	
        *********************************************************************/
        double GetAmountBorrowed(ref string szAmountBorrowed)
        {
	        DoubleToCurrency(dbAmountBorrowed, ref szAmountBorrowed);
	
	        return dbAmountBorrowed;
        }

        /********************************************************************
	        Function:	Instance

	        Arguments:				

	        Returns:	CIssCarLoanCalc* - used to pass only one instance of the class 
							           so everyone will share the same instance

	        Comments:	Function used to pass back one global instance of this class
        *********************************************************************/
        public bool SetRegFees(double dbRegFees)
        {
	        if(!IsDoubleInRange(dbRegFees))
		        return false;	

	        dbRegistrationFees = dbRegFees;

	        return true;
        }

        /********************************************************************
	        Function:	GetRegFees(ref string szRegFees)

	        Arguments:	szRegFees - A ref string of a formatted
				        string able to accept the total of all monthly payments.

	        Returns:	The total of all payments in double format.

	        Comments:	
        *********************************************************************/
        double GetRegFees(ref string szRegFees)
        {
	        DoubleToCurrency(dbRegistrationFees, ref szRegFees);

	        return dbRegistrationFees;
        }


        /********************************************************************
	        Function:	SetOtherFees

	        Arguments:	dbOtherFees - Assign the total of all other fees associated
							          with a car loan.

	        Returns:	true if a valid sales percentage was entered, false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetOtherFees(double dbNewOtherFees)
        {
            if (!IsDoubleInRange(dbNewOtherFees))
		        return false;

            dbOtherFees = dbNewOtherFees;

	        return true;
        }


        /********************************************************************
	        Function:	GetOtherFees(ref string szOtherFees)

	        Arguments:	szOtherFees - A ref string of a formatted
				        string able to accept the total of all monthly payments.

	        Returns:	The total of all payments in double format.

	        Comments:	
        *********************************************************************/
        public double GetOtherFees(ref string szOtherFees)
        {
            if(szOtherFees != null)
	            DoubleToCurrency(dbOtherFees, ref szOtherFees);

	        return dbOtherFees;
        }


        /********************************************************************
	        Function:	SetDownPayment

	        Arguments:	dbDownPayment - Assign the down payment made on the car

	        Returns:	true if a valid , false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetDownPayment(double dbNewDownPayment)
        {
            if (!IsDoubleInRange(dbNewDownPayment))
		        return false;

            if (dbNewDownPayment > dbCarPrice)
		        dbDownPayment = 0;

	        else
                dbDownPayment = dbNewDownPayment;

	        return true;
        }

        /********************************************************************
	        Function:	GetDownPayment(ref string szDownPayment)

	        Arguments:	szDownPayment - A ref string of a formatted
				        string able to accept the down payment

	        Returns:	The down payment in double format.

	        Comments:	
        *********************************************************************/
        public double GetDownPayment(ref string szDownPayment)
        {
            if(szDownPayment != null)
	            DoubleToCurrency(dbDownPayment, ref szDownPayment);

	        return dbDownPayment;

        }


        /********************************************************************
	        Function:	SetRebate

	        Arguments:	dbRebate - Assign the rebate offered by the dealer.

	        Returns:	true if a valid , false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetRebate(double dbNewRebate)
        {
            if (!IsDoubleInRange(dbNewRebate))
		        return false;

            dbRebate = dbNewRebate;

	        return true;
        }


        /********************************************************************
	        Function:	GetRebate(ref string szRebate)

	        Arguments:	szRebate - A ref string of a formatted
				        string able to accept the rebate.

	        Returns:	The total of rebate received on the car in double format.

	        Comments:	
        *********************************************************************/
        double GetRebate(ref string szRebate)
        {
	        DoubleToCurrency(dbRebate, ref szRebate);

	        return dbRebate;
        }


        /********************************************************************
	        Function:	SetTradeIn

	        Arguments:	dbTradeIn - Assign the trade in value of your current car.

	        Returns:	true if a valid , false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetTradeIn(double dbNewTradeIn)
        {
            if (!IsDoubleInRange(dbNewTradeIn))
		        return false;

            dbTradeIn = dbNewTradeIn;

	        return true;
        }


        /********************************************************************
	        Function:	GetTradeIn(ref string szTradeIn)

	        Arguments:	szTradeIn - A ref string of a formatted
				        string able to accept the total of all monthly payments.

	        Returns:	The total of all payments in double format.

	        Comments:	
        *********************************************************************/
        public double GetTradeIn(ref string szTradeIn)
        {
            if(szTradeIn != null)
	            DoubleToCurrency(dbTradeIn, ref szTradeIn);

	        return dbTradeIn;
        }



        /********************************************************************
	        Function:	SetNumberOfMonths

	        Arguments:	iNumMonths - Assign the number of months in the loan

	        Returns:	true if a valid , false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetNumberOfMonths(int iNumMonths)
        {
	        if(iNumMonths <= 0)
		        return false;	

	        // ??? iMonthsLeft doesn't seem to be used???
	        iMonthsLeft = iNumMonths;
	        dbNoOfMonths = (double) iNumMonths;

	        return true;
        }


        /********************************************************************
	        Function:	GetNumberOfMonths(ref string szNumMonths)

	        Arguments:	szNumMonths - A ref string of a formatted
				        string able to accept the total number of months in the loan.

	        Returns:	The number of months in the loan in double format.

	        Comments:	
        *********************************************************************/
        public int	GetNumberOfMonths(ref string szNumMonths)
        {
            if(szNumMonths != null)
	            szNumMonths = iMonthsLeft.ToString();

	        return iMonthsLeft;
        }


        /********************************************************************
	        Function:	SetInterestRatePcnt

	        Arguments:	dbIntRate - Assign the interest rate on the loan.
	        Returns:	true if a valid , false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetInterestRatePcnt(double dbIntRate)
        {
	        if(!IsDoubleInRange(dbIntRate))
		        return false;	

	        dbInterestRate = dbIntRate;


	        return true;
        }


        /********************************************************************
	        Function:	GetInterestRatePcnt(ref string szIntRate)

	        Arguments:	szIntRate - A ref string of a formatted
				        string able to accept the interest rate of the loan in a percent format.

	        Returns:	The interest rate percentage in double format.

	        Comments:	
        *********************************************************************/
        public double GetInterestRatePcnt(ref string szIntRate)
        {
            if(szIntRate != null)
	            DoubleToCurrency(dbInterestRate, ref szIntRate);

	        return dbInterestRate;
        }


        /********************************************************************
	        Function:	DoubleToCurrency(double dbNum, ref string szNum)

	        Arguments:	dbNum	- The double to convert to a currency.
				        szNum	- a ref string capable of storing the double number
							        formatted into  a 2 decimal currency value.

	        Returns:	true if dbNum was valid, false otherwise.

	        Comments:	
        *********************************************************************/
        public bool DoubleToCurrency(double dbNum, ref string szNum)
        {
	        if(!IsDoubleInRange(dbNum))
		        return false;

	        string szTemp;
        //	int iDecimalPlace;
	
	        szTemp = dbNum.ToString();

	        //add more formatting later

	        szNum = szTemp;

	        return true;
        }


        /********************************************************************
	        Function:	IsDoubleInRange(double dbNum)

	        Arguments:	dbNum	- The double to check for validity

	        Returns:	true if dbNum was above 0 and below the max, false otherwise.

	        Comments:	
        *********************************************************************/
        public bool IsDoubleInRange(double dbNum)
        {
	        if(dbNum < 0 ||
		        dbNum > MAX_VALUE)
	        {
		        return false;
	        }
	        return true;

        }

        /********************************************************************
	        Function:	CalculateValues()

	        Arguments:	N/A

	        Returns:	true.

	        Comments:	Call this function to calculate all values entered.
        *********************************************************************/
        public bool CalculateValues()
        {
            if(dbCarPrice == 0 && dbNoOfMonths != 0 && dbMonthlyPayment != 0)
            {

            }



	        //Calculate the Tax
	        dbSalesTax = (dbCarPrice - dbTradeIn)*dbSalesTaxRate/100.0;


	        //calculate the Amount Borrowed first
	        dbAmountBorrowed =  dbCarPrice;
	        dbAmountBorrowed += dbSalesTax;
	        dbAmountBorrowed += dbRegistrationFees;
	        dbAmountBorrowed += dbOtherFees;
	        dbAmountBorrowed -= dbDownPayment;
	        dbAmountBorrowed -= dbRebate;
	        dbAmountBorrowed -= dbTradeIn;

	        //if the Number of Months is Greater then Zero
	        if(dbNoOfMonths > 0 && 
		        dbInterestRate > 0 && 
		        dbAmountBorrowed > 0 && 
		        dbCarPrice > 0)
	        {
                if(bFullInterest)
                {
                    dbTotalCostOfCar = dbNoOfMonths*dbAmountBorrowed*dbInterestRate/1200.0+dbAmountBorrowed;
                }
                else
                {
		            dbTotalCostOfCar = dbNoOfMonths*dbAmountBorrowed*(dbInterestRate/1200.0)/
			            (1-Math.Pow(1.0+dbInterestRate/1200.0,-dbNoOfMonths));
                }
	

		        //Update all the values, and don't forget to check for out of bounds stuff
		        if (dbAmountBorrowed < 0 ||
			        dbAmountBorrowed > 999999999999)
			        dbAmountBorrowed = 0.0;

		        dbMonthlyPayment =  dbTotalCostOfCar / dbNoOfMonths;
		        if (dbMonthlyPayment < 0 ||
                    dbMonthlyPayment > 999999999999)
			        dbMonthlyPayment = 0.0;
		
		        if (dbTotalCostOfCar < 0 ||
                    dbTotalCostOfCar > 999999999999)
			        dbTotalCostOfCar = 0.0;
		
		        dbTotalInterest = dbTotalCostOfCar - dbAmountBorrowed;
		        if (dbTotalInterest < 0 ||
                    dbTotalInterest > 999999999999)
			        dbTotalInterest = 0.0;

		        //now we want to round off the doubles to 2 decimal places
		        /*dbAmountBorrowed = RoundtoTwoDecimals(dbAmountBorrowed);
		        dbMonthlyPayment = RoundtoTwoDecimals(dbMonthlyPayment);
		        dbTotalCostOfCar = RoundtoTwoDecimals(dbTotalCostOfCar);
		        dbTotalInterest = RoundtoTwoDecimals(dbTotalInterest);*/

	        }
	        else //some required values were not entered properly or at all
	        {
		        if(dbNoOfMonths!=0)
			        dbMonthlyPayment = dbAmountBorrowed/dbNoOfMonths;
		        else
			        dbMonthlyPayment = 0;
		        dbTotalCostOfCar = dbAmountBorrowed;
		        dbTotalInterest = 0;
	        }

	        return true;
        }



    }
}
