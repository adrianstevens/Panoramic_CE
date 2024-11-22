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
    public class IssDrepreciation
    {
        enum EnumDepreciationType
        {
            Dep_StraightLine,
            Dep_SumYearDigits,
            Dep_DecliningBal,
            Dep_DB_co_SL,
        };

        public IssDrepreciation()
        {
            ClearWorksheet();
            eDepType = EnumDepreciationType.Dep_StraightLine;
        }

        //Settings
        EnumDepreciationType eDepType;

        int iLife;	//Life of the asset in Years		- Enter Only
        int iStartMnth;	//Starting Month				- Enter Only
        int iStartYr;	//Starting Date						- Enter Only

        double dbCost;	//Cost of the asset					- Enter Only
        double dbSAL;	//Salvage value of Asset			- Enter Only
        int iYR;		//Year to Compute					- Enter Only

        double dbDEP;	//Depreciation for the year			- Auto Calc			//I think the depreciation rate is constant...so all years are the same
        double dbRBV;	//Remaining Book value at EOY		- Auto Calc
        double dbRDV;	//Remaining Depreciable Value		- Auto Calc


        public int GetAssetLife() { return iLife; }
        public int GetStartMonth() { return iStartMnth; }
        public int GetStartYear() { return iStartYr; }
        public double GetCostOfAsset() { return dbCost; }
        public double GetSalvageValue() { return dbSAL; }
        public int GetYearToCompute() { return iYR; }

        public bool ClearWorksheet()
        {
            iLife		= 1;
	        iStartMnth  = 1;
            iStartYr    = DateTime.Now.Year;
            iYR         = DateTime.Now.Year;

            dbCost	    = 0.0;
	        dbSAL		= 0.0;

	        dbDEP		= 0.0;
	        dbRBV		= 0.0;
	        dbRDV		= 0.0;
	

	        return true;
        }


        public bool SetStartMonth(int iMonth)
        {
	        if(iMonth < 0 || iMonth > 12)
		        return false;

	        iStartMnth = iMonth;
	        return true;
        }

        public bool SetStartYear(int iYear)
        {
	        if(iYear < 0 || iYear > 9999)
		        return false;

	        iStartYr = iYear;
	        return true;
        }

        public bool SetCostofAsset(double dbNewCost)
        {
            dbCost = dbNewCost;

	        return true;
        }

        public bool SetSalvageValue(double dbSal)
        {
	        dbSAL = dbSal;
	        return true;
        }

        public bool SetYearToCompute(int iYear)
        {
	        if(iYear < iStartYr)
		        return false;
	        if(iYear < 0)
		        return false;
	
	        iYR = iYear;
	        return true;
        }

        public bool SetAssetLife(int iNewLife)
        {
	        if(iLife < 0)
		        return false;
            iLife = iNewLife;
	        return true;
        }





        //solve for:
        //dep for the year
        //remaining book value
        //remaining depreciable value
        //
        //and we have 3 types of depreciation
        public bool Calc()
        {
	        switch(eDepType) 
	        {
                case EnumDepreciationType.Dep_StraightLine:
		            CalcStraightLineDep();
		            break;
                case EnumDepreciationType.Dep_SumYearDigits:
		            break;
                case EnumDepreciationType.Dep_DecliningBal:
		            break;
                case EnumDepreciationType.Dep_DB_co_SL:
		            break;
	        default:
		            break;
	        }


	
	        return true;
        }

        void CalcStraightLineDep()
        {
	        int iYr = iYR - iStartYr + 1;


	        if(iYr == 1)
	        {
		        dbDEP = GetTotalStraightDep(1);
		        dbRDV = (dbCost - dbSAL - dbDEP);
		        dbRBV = dbCost - dbDEP;		
	        }
	        else if(iYr < iLife) //in the middle
	        {
		        dbDEP = (dbCost - dbSAL)/(double)iLife;	//DEP for the current year
		        dbRDV = (dbCost - dbSAL - GetTotalStraightDep(iYr));		//Remaining Dep Value
		        dbRBV = dbCost - GetTotalStraightDep(iYr);		
	        }
	        else if(iYr == iLife) //the last year ... ie a partial
	        {
		        dbDEP = (dbCost - dbSAL)/(double)iLife*((double)iStartMnth-1.0)/12.0;
		        dbRDV = (dbCost - dbSAL - GetTotalStraightDep(iYr));		//Remaining Dep Value
		        dbRBV = dbCost - GetTotalStraightDep(iYr);
	        }
	        else //past the life
	        {
		        dbDEP = 0;
		        dbRDV = 0;
		        dbRBV = dbSAL;
	        }
        }

        void CalcDecliningBalDep()
        {


        }

        void CalcSumOfYearsDep()
        {



        }

        public double GetTotalStraightDep(int iYear)
        {
	        double dbYear1 = (dbCost - dbSAL)/(double)iLife*(13.0 - (double)iStartMnth)/12.0;
	
	        if(iYear < 1)
		        return 0;
	        else if(iYear > iLife)//that's all of it
		        return dbCost - dbSAL;
	        else if(iYear == 1)
		        return dbYear1;

	        double dbCompleteYear = (dbCost - dbSAL)/(double)iLife;

	        return dbYear1 + (iYear-1)*dbCompleteYear;
        }


        public double GetTotalSumOfYearsDep(int iYear)
        {
	        double dbYear1 = (dbCost - dbSAL)/(double)iLife*(13.0 - (double)iStartMnth)/12.0;
	
	        if(iYear < 1)
		        return 0;
	        else if(iYear > iLife)//that's all of it
		        return dbCost - dbSAL;
	        else if(iYear == 1)
	        {
		        return ((double)iLife*(dbCost - dbSAL))	* ((13.0 - (double)iStartMnth)/12.0)	/		(((double)iLife*((double)iLife + 1.0)) / 2.0);
	        }
	
	        return ((double)iLife + 2.0 - (double)iYear - ((13.0 - (double)iStartMnth)/12.0))		*		(dbCost - dbSAL)	/	(((double)iLife*((double)iLife+1.0))/2.0);
        }


        public double GetTotalDeclineDep(int iYear)
        {
	        double dbYear1 = (dbCost - dbSAL)/(double)iLife*(13.0 - (double)iStartMnth)/12.0;
	
	        if(iYear < 1)
		        return 0;
	        else if(iYear > iLife)//that's all of it
		        return dbCost - dbSAL;
	        else if(iYear == 1)
		        return dbYear1;

	        double dbCompleteYear = (dbCost - dbSAL)/(double)iLife;

	        return dbYear1 + (iYear-1)*dbCompleteYear;
        }

        public double GetDep()
        {
	        return dbDEP;
        }

        public double GetRemainingBookValue()
        {
	        return dbRBV;
        }

        public double GetRemainingDepValue()
        {
	        return dbRDV;
        }
    }
}
