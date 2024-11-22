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
    public class IssLeaseCalc
    {
        public IssLeaseCalc()
        {
            ResetValues();
        }

        static double MAX_VALUE = 100000000.0;

        public bool bPayTaxOnCapCost;     

        int			 iMonths;				//Length of Lease in Months
				
	    double		 dbPrice;				//Price
	    double		 dbTaxPercent;			//Tax %
	    double		 dbTotalTax;			//Total Tax
	    double		 dbFees;				//Additional Fees
	    double		 dbResidual;			//Residual Amount after Lease
	    double		 dbDownPayment;		//Down Payment
	    double		 dbTrade;				//Trade In Value
	    double		 dbInterest;			//Interest Rate
	    double		 dbMoneyFactor;		//Money Factor based off Interest
	    double		 dbMonthlyPayment;		//Monthly Payment
	    double		 dbMonthlyDep;			//Depreciation Per Month
	    double		 dbMonthlyInterest;	//Interest per Month
	    double		 dbMonthlyTax;			//Ave Monthly Tax
	    double		 dbTotalPayments;		//Lease Total
	    double		 dbTotalDepreciation;	//Total Depreciation
	    double		 dbTotalInterest;		//Total Interest

        public bool ResetValues()
        {
	        iMonths				= 36;
	        dbPrice				= 0;
	        dbTaxPercent		= 0;
	        dbTotalTax			= 0;
	        dbFees				= 0;
	        dbResidual			= 0;
	        dbDownPayment		= 0;
	        dbTrade				= 0;
	        dbInterest			= 0;
	        dbMoneyFactor		= 0;
	        dbMonthlyPayment	= 0;
	        dbMonthlyDep		= 0;
	        dbMonthlyInterest	= 0;
	        dbMonthlyTax		= 0;
	        dbTotalPayments		= 0;
	        dbTotalDepreciation	= 0;
	        dbTotalInterest		= 0;

            bPayTaxOnCapCost = false;

	        return true;
        }

       

        /********************************************************************
	        Function:	DoubleToCurrency(double dbNum, ref string szNum)

	        Arguments:	dbNum	- The double to convert to a currency.
				        szNum	- a ref string capable of storing the double number
							        formatted into  a 2 decimal currency value.

	        Returns:	true if dbNum was valid, false otherwise.

	        Comments:	
        *********************************************************************/
        bool DoubleToCurrency(double dbNum, ref string szNum)
        {
	        if(!IsDoubleInRange(dbNum))
		        return false;

	        string szTemp;
        
	        szTemp = dbNum.ToString();

	        
	        //This will remove the exponent
	        //Assumes TWO things
	        //the number isn't negative
	        //the number is LESS than 0
	        //you want it better...IssCalcDisplay
            if(szTemp.IndexOf("e") != -1)
	        {
                string szExp = "";
                string szValue = "";
		        int iExp = 0;

		        
		        //Period Location is at 1
		        //Separate the exponent
                szExp = szTemp.Substring(szTemp.Length-3, 3);
                szValue = szTemp.Substring(0, szTemp.Length-5);

                iExp = Convert.ToInt32(szExp);

		        for(int i=0; i < iExp; i++)
		        {
                    szValue = szValue.Remove(1, 1);
                    szValue = szValue.Insert(0, "0.");
		        }

		        szTemp = szValue;
	        }

	        szNum = szTemp;

	        return true;
        }


        /********************************************************************
	        Function:	IsDoubleInRange(double dbNum)

	        Arguments:	dbNum	- The double to check for validity

	        Returns:	true if dbNum was above 0 and below the max, false otherwise.

	        Comments:	
        *********************************************************************/
        bool IsDoubleInRange(double dbNum)
        {
	        if(dbNum < 0 ||
		        dbNum > MAX_VALUE)
	        {
		        return false;
	        }
	        return true;

        }

        /********************************************************************
	        Function:	SetPrice

	        Arguments:	dbPrice - The amount of $$$ borrowed.

	        Returns:	true if a good price was entered, false otherwise.

	        Comments:	Assigns the amount of money borrowed for the loan.
        *********************************************************************/
        public bool SetPrice(double dbNewPrice)
        {
            if (!IsDoubleInRange(dbNewPrice))
		        return false;

            dbPrice = dbNewPrice;
	        return true;
        }

        /********************************************************************
	        Function:	GetPrice()

	        Arguments:	szPrice - ref string to a position in memory that is able to 
				        accept the amount of money borrowed.

	        Returns:	Double format of the price.

	        Comments:	Returns the price of the loan in TCHAR and a double.
				
        *********************************************************************/
        public double GetPrice(ref string szPrice)
        {
	        DoubleToCurrency(dbPrice, ref szPrice);

	        return dbPrice;
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

	        dbTaxPercent = dbSalesTax;
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
	        DoubleToCurrency( dbTaxPercent, ref szSaleTaxPercent);

	        return dbTaxPercent;
        }


        /********************************************************************
	        Function:	GetSalesTax(ref string szSalesTax)

	        Arguments:	szSalesTax - A ref string of a formatted
				        string of the current sales tax.

	        Returns:	The entered sales tax in double format.

	        Comments:	
        *********************************************************************/
        public double GetSalesTax(ref string szSalesTax)
        {
	        DoubleToCurrency( dbTotalTax, ref szSalesTax);

	        return dbTotalTax;
        }


        /********************************************************************
	        Function:	SetAdditionalFees

	        Arguments:	dbAdditionalFees - Assign the total of all other fees associated
									        with a loan.

	        Returns:	true if a valid number entered, false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetAdditionalFees(double dbAdditionalFees)
        {
	        if(!IsDoubleInRange(dbAdditionalFees))
		        return false;

	        dbFees = dbAdditionalFees;
	        return true;
        }


        /********************************************************************
	        Function:	GetAdditionalFees(ref string szAddFees)

	        Arguments:	szAddFees - A ref string of a formatted
				        string able to accept the total of extra fees on the loan.

	        Returns:	The total of all extras in double format.

	        Comments:	
        *********************************************************************/
        public double GetAdditionalFees(ref string szAddFees)
        {
	        DoubleToCurrency( dbFees, ref szAddFees);

	        return dbFees;
        }


        /********************************************************************
	        Function:	SetResidualPrice(double dbResidualPrice)

	        Arguments:	dbResidualPrice - The value of the item after the lease.

	        Returns:	true if a valid , false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetResidualPrice(double dbResidualPrice)
        {
	        if(!IsDoubleInRange(dbResidualPrice))
		        return false;

	        dbResidual = dbResidualPrice;
	        return true;
        }





        /********************************************************************
	        Function:	GetResidualPrice(ref string szResPrice)

	        Arguments:	szResPrice - A ref string of a formatted
				        string able to accept the residual value of the lease.

	        Returns:	The residual value in double format.

	        Comments:	
        *********************************************************************/
        public double GetResidualPrice(ref string szResPrice)
        {
	        DoubleToCurrency( dbResidual, ref szResPrice);

	        return dbResidual;
        }


        /********************************************************************
	        Function:	SetDownPayment

	        Arguments:	dbDownPayment - Assign the down payment made on the lease

	        Returns:	true if a valid , false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetDownPayment(double dbNewDownPayment)
        {
            if (!IsDoubleInRange(dbNewDownPayment))
		        return false;

            if (dbNewDownPayment > dbPrice)
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
	        DoubleToCurrency( dbDownPayment, ref szDownPayment);

	        return dbDownPayment;
        }


        /********************************************************************
	        Function:	SetTradeIn

	        Arguments:	dbTradeIn - Assign the trade in value of your current car.

	        Returns:	true if a valid , false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetTradeIn(double dbTradeIn)
        {
	        if(!IsDoubleInRange(dbTradeIn))
		        return false;

	        dbTrade = dbTradeIn;
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
	        DoubleToCurrency( dbTrade, ref szTradeIn);

	        return dbDownPayment;
        }


        /********************************************************************
	        Function:	SetNumberOfMonths

	        Arguments:	iNumMonths - Assign the number of months in the loan

	        Returns:	true if a valid , false otherwise.
  
	        Comments:	
        *********************************************************************/
        public bool SetNumberOfMonths(int iNumMonths)
        {
	        if(!IsDoubleInRange((double)iNumMonths))
		        return false;

	        iMonths = iNumMonths;
	        return true;
        }


        /********************************************************************
	        Function:	GetNumberOfMonths(ref string szNumMonths)

	        Arguments:	szNumMonths - A ref string of a formatted
				        string able to accept the total number of months in the loan.

	        Returns:	The number of months in the loan in integer format.

	        Comments:	
        *********************************************************************/
        public int GetNumberOfMonths(ref string szNumMonths)
        {
	        szNumMonths = iMonths.ToString();

	        return iMonths;
        }


        public double GetMonthlyTax(ref string szMonthlyTax)
        {
	        DoubleToCurrency( dbMonthlyTax, ref szMonthlyTax);

	        return dbMonthlyTax;
        }


        public double GetMonthlyInterest(ref string szMonthlyInterest)
        {
	        DoubleToCurrency( dbMonthlyInterest,ref szMonthlyInterest);
	        return dbMonthlyInterest;
	
        }


        public bool SetMoneyFactorPcnt(double dbMoneyFactorPcnt)
        {
	        if(!IsDoubleInRange(dbMoneyFactorPcnt))
		        return false;

	        dbMoneyFactor = dbMoneyFactorPcnt;
	        return true;
        }

        public double GetMoneyFactorPcnt(ref string szMoneyFactor)
        {
	        DoubleToCurrency( dbMoneyFactor, ref szMoneyFactor);

	        return dbMoneyFactor;
        }

        public bool SetMoneyFactor(double dbNewMoneyFactor)
        {
            if (!IsDoubleInRange(dbNewMoneyFactor))
		        return false;

            dbMoneyFactor = dbNewMoneyFactor;
	        return true;
        }


        public bool SetInterestRate(double dbNewInterest)
        {
            if (!IsDoubleInRange(dbNewInterest))
		        return false;

            dbInterest = dbNewInterest;
	        return true;
        }

        public double GetInterestRate(ref string szInterest)
        {
	        DoubleToCurrency( dbInterest, ref szInterest);

	        return dbInterest;
        }

        public double GetMoneyFactor(ref string szMoneyFactor)
        {
	        DoubleToCurrency( dbMoneyFactor, ref szMoneyFactor);

	        return dbMoneyFactor;
        }

        public double GetTotalPayments(ref string szPayments)
        {
	        DoubleToCurrency( dbTotalPayments, ref szPayments);

	        return dbTotalPayments;
        }

        public double GetMonthlyPayments(ref string szMonthlyPayments)
        {
	        DoubleToCurrency( dbMonthlyPayment, ref szMonthlyPayments);

	        return dbMonthlyPayment;
        }

        public double GetDepreciation(ref string szDepreciation)
        {
	        DoubleToCurrency( dbMonthlyDep, ref szDepreciation);

	        return dbMonthlyDep;
        }


        public double GetTotalDepreciation(ref string szTotalDepreciation)
        {
	        DoubleToCurrency( dbTotalDepreciation, ref szTotalDepreciation);

	        return dbTotalDepreciation;
        }

        public double GetTotalTax(ref string szTotalTax)
        {
	        DoubleToCurrency( dbTotalTax, ref szTotalTax);

	        return dbTotalTax;
        }


        public double GetInterest(ref string szInterest)
        {
	        DoubleToCurrency( dbTotalInterest, ref szInterest);

	        return dbTotalInterest;
        }


        /********************************************************************
	        Function:	CalculateValues()

	        Arguments:	N/A

	        Returns:	true.

	        Comments:	Call this function to calculate all values entered.
        *********************************************************************/
        void CalculateValues()
        {
	        dbTotalDepreciation	= dbPrice - dbResidual - dbTrade - dbDownPayment + dbFees;
	        dbMonthlyDep			= dbTotalDepreciation / iMonths;

	        dbMoneyFactor			= dbInterest/2400;

	        dbMonthlyInterest		= ( dbPrice + dbFees - dbTrade - dbDownPayment + dbResidual)* dbMoneyFactor;

	        dbTotalInterest		= dbMonthlyInterest* iMonths;

	        dbMonthlyTax			= ( dbMonthlyInterest + dbMonthlyDep)* dbTaxPercent/100;
	        if( bPayTaxOnCapCost)
		        dbMonthlyTax		+= ( dbResidual/ iMonths)* dbTaxPercent/100.0;

	
	        dbTotalTax			= dbMonthlyTax* iMonths;

	        dbMonthlyPayment		= dbMonthlyTax + dbMonthlyInterest + dbMonthlyDep;

	        dbTotalPayments		= dbMonthlyPayment* iMonths;



	        //Hack for 0 months on first screen
	        if( iMonths == 0)
		        dbTotalTax		= dbTotalDepreciation * dbTaxPercent/100;
        }

        //double
        double RoundtoTwoDecimals(double dbIn)
        {

	        // Not checked...just converted without testing from your version in mfc version.....

	        string  szTempString;
	        double dbReturn;
	
	        //place the double into a string and specify that we want only 2 decimal places
	        szTempString = String.Format("{0:0.00}", dbIn);

	
	        //get the amount
            dbReturn = Convert.ToDouble(szTempString);

            int iPeriod = szTempString.IndexOf(".");
	        int iStringLen = szTempString.Length;
	
	        //if there is a decimal in the string then we want to extract it from the string
	        if(-1 != iPeriod && iStringLen > iPeriod)
	        {
		        szTempString = szTempString.Remove(iPeriod, iStringLen - iPeriod);
		        //now get the decimal values and add it to the fAmount
		        //double fDecimal = _ttol(szTempString) / 100.0;
                double fDecimal = (double)Convert.ToInt32(szTempString) / 100.0;
		        dbReturn += fDecimal;
	        }
	
	        //return dat bitch
	        return dbReturn;
        }

        void CalculateInterestRate()
        {
	        dbInterest = dbMoneyFactor*2400;
        }


        
    }
}
