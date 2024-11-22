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
    public class IssFDateCalc
    {
        public IssFDateCalc()
        {
            Clear();
        }

        int iStartDay;
        int iStartMonth;
        int iStartYear;

        int iEndDay;
        int iEndMonth;
        int iEndYear;

        int iDaysBetweenDates;

        public int GetStartDay(){return iStartDay;}
        public int GetStartMonth() { return iStartMonth; }
        public int GetStartYear() { return iStartYear; }

        public int GetEndDay() { return iEndDay; }
        public int GetEndMonth() { return iEndMonth; }
        public int GetEndYear() { return iEndYear; }

        public int GetDaysNormal() { CalcNormal(); return iDaysBetweenDates; }
        public int GetDays30360() { Calc30360(); return iDaysBetweenDates; }
    

        public void Clear()
        {
            iStartDay = DateTime.Now.Day;
            iStartMonth = DateTime.Now.Month;
            iStartYear = DateTime.Now.Year;

            iEndDay = DateTime.Now.Day;
            iEndMonth = DateTime.Now.Month;
            iEndYear = DateTime.Now.Year;
        }

        /***********************************************************************************
	        30/360 day-count method
	        (assumes 30 days per month and 360 days per year):

	        DBD=(Y2-Y1)´360+(M2-M1)´30+(DT2-DT1)
	
	        where: M1 = month of first date
	        DT1 = day of first date
	        Y1 = year of first date
	        M2 = month of second date
	        DT2 = day of second date
	        Y2 = year of second date
	
	        Note: If DT1 is 31, change DT1 to 30. If DT2 is 31 and DT1 is 30 or 31,
	
	        change DT2 to 30; otherwise, leave it at 31.
	        Source for 30/360 day-count method formula: Lynch, John J.,
	        Jr., and Jan H. Mayle. Standard Securities Calculation
	        Methods. New York: Securities Industry Association, 1986.
         **********************************************************************************/
        public int Calc30360()
        {
	        int iNewStartDay	= iStartDay;
	        int iNewEndDay		= iEndDay;

            if (iNewEndDay > 30)
                iNewEndDay = 30;

            if (iNewStartDay > 30)
                iNewStartDay = 30;

	        iDaysBetweenDates = (iEndYear - iStartYear)*360+(iEndMonth - iStartMonth)*30+ iNewEndDay- iNewStartDay;
	
        //	if(iDaysBetweenDates < 0)
        //		iDaysBetweenDates *= -1;

	        return iDaysBetweenDates;
        }



        /***********************************************************************************
	        Actual/actual day-count method
	        (assumes actual number of days per month and actual number

	        of days per year):
	        DBD (days between dates)
	        = number of days II - number of days I

	        Number of Days I = (Y1 - YB) * 365
	        + (number of days MB to M1)
	        + DT1 +(Y1 - YB)/4

	        Number of Days II = (Y2 - YB) * 365
	        + (number of days MB to M2)
	        + DT2
	        + (Y2 - YB)/4
	
	        where: 
	        M1	= month of first date
	        DT1 = day of first date
	        Y1	= year of first date
	        M2	= month of second date
	        DT2 = day of second date
	        Y2	= year of second date
	        MB	= base month (January)
	        DB	= base day (1)
	        YB	= base year (first year after leap year)
        ***********************************************************************************/
        public int CalcNormal()
        {
	        int iBaseYear;
	        int iNumDays1;
	        int iNumDays2;

	        iBaseYear = FindLastLeapYear(iStartYear - 1) + 1;

	        if(iEndYear < iStartYear)
		        iBaseYear = FindLastLeapYear(iEndYear - 1) + 1;

        //	int iLeap1 = ((double)iStartYear - (double)iBaseYear)/4.0;
        //	int iLeap2 = ((double)iEndYear - (double)iBaseYear)/4.0;

	        int iLeap1 = GetNumberOfLeaps(iBaseYear,iStartYear, 1, iStartMonth, 1, iStartDay);
	        int iLeap2 = GetNumberOfLeaps(iBaseYear,iEndYear, 1, iEndMonth, 1, iEndDay);

	        iNumDays1 = (iStartYear -	iBaseYear)*365 + DaysBetweenMonths(1, iStartMonth)	+ iStartDay	+ iLeap1;

	        iNumDays2 = (iEndYear -	iBaseYear)*365 + DaysBetweenMonths(1, iEndMonth)		+ iEndDay		+ iLeap2;

	        iDaysBetweenDates = iNumDays2 - iNumDays1;

	        if(iDaysBetweenDates < 0)
		        iDaysBetweenDates *= -1;
				
	        return iDaysBetweenDates;
        }





        public bool IsLeapYear(int iYear)
        {
	        if(iYear < 0)
		        return false;

	        //Divisable by 400....yup
	        if(iYear%400 == 0)
		        return true;
	
	        //Divisable by 4....might be...carry on
	        if(iYear%4 != 0)
		        return	false;

	        //Divisable by 100...not a leapyear (we already handled 400)
	        if(iYear%100 == 0)
		        return false;

	        //if it got this far its good
	        return true;
        }

        int FindLastLeapYear(int iYear)
        {
	        if(iYear < 0)//we're not doing BC
		        return 0;

	        int iReturn = iYear - iYear%4; 

	        if(iReturn%400 == 0)
		        return iReturn;

	        if(iReturn%100 == 0)
		        return iReturn - 4;

	        return iReturn;
		
	
	
        }


        int DaysBetweenMonths(int iMonth1, int iMonth2)
        {
	        int iRet = 0;
	
	        if(iMonth1 == iMonth2)
		        return 0;

	        else if(iMonth1 > iMonth2)
	        {
		        for(int i = iMonth1; i < iMonth2 + 12; i++)
		        {	
			        if(i > 12)
				        iRet += GetDaysInMonth(i-12);
			        else
				        iRet += GetDaysInMonth(i);
		        }
		        iRet -= 365;
	        }
	        else //(iMonth1 < iMonth2)
	        {
		        for(int i = iMonth1; i < iMonth2; i++)
			        iRet += GetDaysInMonth(i);
	        }
	
	        return iRet;
        }

        int GetDaysInMonth(int iMonth)
        {
	        switch(iMonth) 
	        {
	        case 1:	//Jan
	        case 3: //Mar
	        case 5:	//May
	        case 7:	//July
	        case 8:	//Aug
	        case 10:	//Oct
	        case 12:	//Dec
		        return 31;
                		        
	        case 2: //Feb
		        return 28;	
		        
	        case 4: //April
	        case 6:	//June
	        case 9:	//Sept
	        case 11:	//Nov
		        return 30;
		        
	        default:
		        return 0;
	        }
        }


        public bool SetEndDay(int iDay)
        {
	        if(iDay < 0)
		        iEndDay = 1;

	        else if(iDay > GetDaysInMonth(iEndMonth))
		        iEndDay = GetDaysInMonth(iEndMonth);

	        else iEndDay = iDay;

	        return true;
        }

        public bool SetEndMonth(int iMonth)
        {
	        if(iMonth < 0)
		        iEndMonth = 1;
	        if(iMonth > 12)
		        iEndMonth = 12;
	        else
		        iEndMonth = iMonth;
	        return true;
        }

        public bool SetEndYear(int iYear)
        {
	        if(iYear < 0)
		        iEndYear = 0;
	        else if(iYear > 3000)
		        iEndYear = 3000;
	        else
		        iEndYear = iYear;
	        return true;
        }


        public bool SetStartDay(int iDay)
        {
	        if(iDay < 0)
		        iStartDay = 1;

	        else if(iDay > GetDaysInMonth(iStartMonth))
		        iStartDay = GetDaysInMonth(iStartMonth);

	        else iStartDay = iDay;

	        return true;
        }

        public bool SetStartMonth(int iMonth)
        {
	        if(iMonth < 0)
		        iStartMonth = 1;
	        if(iMonth > 12)
		        iStartMonth = 12;
	        else
		        iStartMonth = iMonth;
	        return true;
        }

        public bool SetStartYear(int iYear)
        {
	        if(iYear < 0)
		        iStartYear = 0;
	        else if(iYear > 3000)
		        iStartYear = 3000;
	        else
		        iStartYear = iYear;
	        return true;
        }


        int GetNumberOfLeaps(int iYearS, int iYearE, int iMonthS, int iMonthE, int iDayS, int iDayE)
        {
	        if(iYearS > iYearE) //one way street buddy
		        return 0;

	        if(iYearS == iYearE)
	        {
		        if(iMonthS >= iMonthE)
			        return 0;
		        else if(IsLeapYear(iYearS))//special case ... same year, is a leap...gotta check the months then
		        {
			        if(iMonthS < 3 && iMonthE > 2)
				        return 1;
		        }
		        return 0;
	        }

	        int iLeapCount = 0;

	        if(IsLeapYear(iYearS) && iMonthS < 3)
		        iLeapCount++;

	        for(int i = iYearS + 1; i < iYearE; i++)
	        {
		        if(IsLeapYear(i))
			        iLeapCount++;
	        }

	        if(IsLeapYear(iYearE))
	        {
		        if((iMonthE == 2 && iDayE == 29) || iMonthE > 2)
			        iLeapCount++;
	        }

	        return iLeapCount;
        }
    }
}
