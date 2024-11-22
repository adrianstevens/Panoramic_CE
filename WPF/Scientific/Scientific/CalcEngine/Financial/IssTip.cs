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
    public class IssTip
    {
        public IssTip()
        {
           	int[] iTemp = {0,1,2, 3,4, 5,6,7, 8,9, 10,11,12,13,14,15,16,17,18,19,20,22,25,30,35,40,45,50};

	        for(int i=0;i<NUMBER_OF_PERCENTAGES; i++)
	        {
		        iPercentages[i]=iTemp[i];
	        }
        }

        static int MAX_PERCENT = 1000;
        static int NUMBER_OF_PERCENTAGES = 28;
        static int NUMBER_OF_PEOPLE	= 999;

       	double			dbAmount;
	    double			dbTipAmount;
	    double			dbTotal;
	    double			dbTotalSplit;

	    int[]			iPercentages = new int[NUMBER_OF_PERCENTAGES];

	    double		    dbTipPercentage;
	    int				iNumberOfPeople;

        public double   GetBillAmount(){return dbAmount;}
        public double   GetTipAmount(){return dbTipAmount;}
        public double   GetTotal(){return dbTotal;}
        public double   GetTotalPerPerson(){return dbTotalSplit;}

        public bool Reset()
        {
            dbAmount = 0;
            dbTipAmount = 0;
            dbTotal = 0;
            dbTotalSplit = 0;
            dbTipPercentage = 0;
            iNumberOfPeople = 1;
    
            return true;
        }

                /********************************************************************

	        Function:	SetPercentage

	
	        purpose:	Sets the percentage we're going to tip 15 = 15% (it's an int, so don't use 0.15)
        *********************************************************************/
        public bool SetPercentage(int iPercentage)
        {
	        if(iPercentage < 0 ||
		        iPercentage > NUMBER_OF_PERCENTAGES)
		        return false;
	        else
	        {
		        dbTipPercentage = (double)iPercentages[iPercentage];
	        }
	        CalculateResults();
	        return true;
        }

        public bool SetPercentage(double dbPercentage)
        {
            if(dbPercentage < 0 ||
                dbPercentage > MAX_PERCENT)
                return false;

            dbTipPercentage = dbPercentage;
            CalculateResults();
            return true;
        }


        /********************************************************************

	        Function:	SetNumberofPeople

	
	        purpose:	Sets the number of people splitting the bill
				        offset by 1, ie 16 should be 15, etc
        *********************************************************************/
        public bool SetNumberofPeople(int iNumPeople)
        {
	        if(iNumPeople < 1 ||
		        iNumPeople > NUMBER_OF_PEOPLE)
                iNumberOfPeople = 1;
	        else
	        {
		        iNumberOfPeople = iNumPeople+1;
	        }
	        CalculateResults();
	        return true;
        }

        /********************************************************************

	        Function:	CalculateResults

	
	        purpose:	Calculates all of the amounts based off the current inputs
				
        *********************************************************************/
        public bool CalculateResults()
        {
	        dbTipAmount	= dbAmount*dbTipPercentage/100;
            dbTotal		= dbAmount+dbTipAmount;
	        dbTotalSplit	= dbTotal/(double)(iNumberOfPeople);

	        return true;
        }

        /********************************************************************

	        Function:	GetPercentageIndex

	        purpose:	Gives the percentage for a certain index 
				        Percentages can be found in the int array iPercentages
					
        *********************************************************************/
        public bool GetPercentageIndex(int iIndex, ref string szPercent)
        {
	        if(iIndex < 0 ||
		        iIndex > NUMBER_OF_PERCENTAGES-1)
		        return false;

            szPercent = iPercentages[iIndex].ToString() + " % ";

	        return true;
        }

        /********************************************************************

	        Function:	GetPeopleIndex

	        purpose:	Gives the people for a certain index 
				        Its simply based off the given iIndex up to the max NUMBER_OF_PEOPLE
					
        *********************************************************************/
        /* public bool GetPeopleIndex(int iIndex,  szPeople)
         {
             if(iIndex < 0 ||
                 iIndex > NUMBER_OF_PEOPLE-1)
                 return false;
             else
             {
                 oStr->IntToString(szPeople, iIndex+1);
                 TCHAR szGroup[STRING_LARGE];
                 if(iIndex == 0)
                     oStr->StringCopy(szGroup, IDS_TIPCALC_Person, STRING_LARGE);
                 else
                     oStr->StringCopy(szGroup, IDS_TIPCALC_People, STRING_LARGE);

                 oStr->Concatenate(szPeople, _T(" "));	
                 oStr->Concatenate(szPeople, szGroup);
             }
             return true;
         }*/


        /********************************************************************

	        Function:	GetPeopleIndex

	        return:		The current percentage

	        purpose:	Sets the current percent formatted nicely in a TCHAR
					
        *********************************************************************/
        public double GetPercent()
        {
            return dbTipPercentage;
        }



        public int GetPercent(ref string szPercent)
        {
            szPercent = ((int)dbTipPercentage).ToString() + " % ";

            return (int)dbTipPercentage;
        }

        /********************************************************************

	        Function:	GetNumberOfPeople

	        return:		The current number of people

	        purpose:	Sets the current number of people formatted nicely in a TCHAR
					
        *********************************************************************/
        public int GetNumberOfPeople()
        {
            return iNumberOfPeople;
        }

        public int	GetNumberOfPeople(ref string szNumPeople)
        {
            szNumPeople = iNumberOfPeople.ToString();
            return iNumberOfPeople;
        }

        /********************************************************************

	        Function:	SetAmount

		
	        purpose:	Sets the amount we're calculating for the tip
				        ie - the bill
        *********************************************************************/
        public bool SetAmount(string szAmount)
        {
            dbAmount = Convert.ToDouble(szAmount);
	        CalculateResults();
	        return true;
        }
        
    }
}
