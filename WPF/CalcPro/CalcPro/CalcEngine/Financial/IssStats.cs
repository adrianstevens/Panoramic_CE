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
    public enum EnumStatistics
    {
        Stats_Linear_Reg,
        Stats_Ln_Reg,
        Stats_Log_Reg,
        Stats_Exponential_Reg,
        Stats_Power_Reg,
        Stats_1_Variable,
        Stats_Quad_Reg,//new
        Stats_Cubic_Reg,//new
    };

    public class IssStats
    {
        static int MAX_DATA_SETS = 100;
   //     static int NUMBER_OF_REG = 6;
        
        public IssStats()
        {
            eStatsType = EnumStatistics.Stats_Linear_Reg;
            ResetWorkSheet();
        }



	    //Settings
	    EnumStatistics	eStatsType;		//Sets what type of Statistics we'll be doing - see enum

	    //Auto Compute
        //we'll use these as actual values

	    double		dbMeanX;			//Average of X Vals		-	Auto Compute
	    double		dbSX;				//Sample Sd Dev of X	-	Auto Compute
	    double		dbPopStrdX;		//Population Std Dev X	-	Auto Compute
	    double		dbMeanY;			//Average of Y Vals		-	Auto Compute
	    double		dbSY;				//Sample Sd Dev of Y	-	Auto Compute
	    double		dbPopStrdY;		//Population Std Dev Y	-	Auto Compute
	    double		dbLinA;			//Linear Reg Y Intcept	-	Auto Compute
	    double		dbLinB;			//Linear Reg Slope		-	Auto Compute
	    double		dbRegR;			//Correlation Co-eff	-	Auto Compute -1 < r < 1 ... 0 means bad fit
	    double		dbPredX;			//Predicted Val of X	-	Enter / Auto Compute
	    double		dbPredY;			//Predicted Val of Y	-	Enter / Auto Compute
	    double		dbSumX;			//Sum of X Values		-	Auto Compute
	    double		dbSumX2;			//Sum of X Squared Vals	-	Auto Compute
	    double		dbSumY;			//Sum of Y Values		-	Auto Compute
	    double		dbSumY2;			//Sum of Y Squared Vals	-	Auto Compute
	    double		dbSumXY;			//Sum of XY products	-	Auto Compute

        //and we'll use this for calculating regression
        //since they'll be converted for linear purposes
        double		dbConPopStrdX;		//Population Std Dev X	-	Auto Compute
        double		dbConPopStrdY;		//Population Std Dev Y	-	Auto Compute
        double		dbConSumX;			//Sum of X Values		-	Auto Compute
        double		dbConSumX2;			//Sum of X Squared Vals	-	Auto Compute
        double		dbConSumY;			//Sum of Y Values		-	Auto Compute
        double		dbConSumY2;			//Sum of Y Squared Vals	-	Auto Compute
        double		dbConSumXY;			//Sum of XY products	-	Auto Compute


        
	    int		    iNumberOfXData;	//Data Set Counter
	    int		    iNumberOfYData;	//Data Set Counter
	    double[]	dbXValues = new double[MAX_DATA_SETS];	//Pointer to X value array
	    double[]  	dbYValues = new double[MAX_DATA_SETS];	//Pointer to Y value array

        public EnumStatistics GetStatsType() { return eStatsType; }
        public int RecallXValueCount() { return iNumberOfXData; }
        public int RecallYValueCount() { return iNumberOfYData; }



        public bool ResetWorkSheet()
        {
	        dbMeanX				= 0.0;			//Average of X Vals		-	Auto Compute
	        dbSX				= 0.0;			//Sample Sd Dev of X	-	Auto Compute
	        dbPopStrdX			= 0.0;			//Population Std Dev X	-	Auto Compute
	        dbMeanY				= 0.0;			//Average of Y Vals		-	Auto Compute
	        dbSY				= 0.0;			//Sample Sd Dev of Y	-	Auto Compute
	        dbPopStrdY			= 0.0;			//Population Std Dev Y	-	Auto Compute
	        dbLinA				= 0.0;			//Linear Reg Y Intcept	-	Auto Compute
	        dbLinB				= 0.0;			//Linear Reg Slope		-	Auto Compute
	        dbRegR				= 0.0;			//Correlation Co-eff	-	Auto Compute
	        dbPredX				= 0.0;			//Predicted Val of X	-	Enter / Auto Compute
	        dbPredY				= 0.0;			//Predicted Val of Y	-	Enter / Auto Compute
	        dbSumX				= 0.0;			//Sum of X Values		-	Auto Compute
	        dbSumX2				= 0.0;			//Sum of X Squared Vals	-	Auto Compute
	        dbSumY				= 0.0;			//Sum of Y Values		-	Auto Compute
	        dbSumY2				= 0.0;			//Sum of Y Squared Vals	-	Auto Compute
	        dbSumXY				= 0.0;			//Sum of XY products	-	Auto Compute

            dbConPopStrdX		= 0.0;			//Population Std Dev X	-	Auto Compute
            dbConPopStrdY		= 0.0;			//Population Std Dev Y	-	Auto Compute
            dbConSumX			= 0.0;			//Sum of X Values		-	Auto Compute
            dbConSumX2			= 0.0;			//Sum of X Squared Vals	-	Auto Compute
            dbConSumY			= 0.0;			//Sum of Y Values		-	Auto Compute
            dbConSumY2			= 0.0;			//Sum of Y Squared Vals	-	Auto Compute
            dbConSumXY			= 0.0;			//Sum of XY products	-	Auto Compute

	        iNumberOfXData		= 0;
	        iNumberOfYData		= 0;

            for(int i =0; i < MAX_DATA_SETS; i++)
	        {
		        dbXValues[i]=0;
		        dbYValues[i]=0;
	        }
	
	        return true;
        }

        public bool SetStatsType(EnumStatistics eType)
        {
	        switch(eType) 
	        {
	        case EnumStatistics.Stats_Linear_Reg:
	        case EnumStatistics.Stats_Log_Reg:
	        case EnumStatistics.Stats_Ln_Reg:
	        case EnumStatistics.Stats_Exponential_Reg:
	        case EnumStatistics.Stats_Power_Reg:
	        case EnumStatistics.Stats_1_Variable:
		        eStatsType = eType;
		        return true;
		        
	        default:
		        return false;
		        
	        }
        }

        public bool AddXValue(double dbX)
        {
	        if(iNumberOfXData > MAX_DATA_SETS)
		        return false;
	
	        dbXValues[iNumberOfXData] = dbX;

	        iNumberOfXData++;

	        return true;
        }

        public bool AddXValue(double dbX, int iLocation)
        {
	        if(iLocation < 0)
		        return false;
	        else if(iLocation > iNumberOfXData)
		        return false;
	        else if(iLocation == iNumberOfXData) //gotta make one
	        {
		        iNumberOfXData++;
	        }

	        dbXValues[iLocation] = dbX;
	
	        return true;
        }

        public bool InsertXValue(double dbX, int iLocation)
        {
	        if(iLocation < 0 ||
		        iLocation > iNumberOfXData - 1)
		        return false;

	        if(iNumberOfXData >= MAX_DATA_SETS)
		        return false;

	        //shift everything up
	        for(int i = iNumberOfXData ; i > iLocation; i--)
	        {
		        dbXValues[i] = dbXValues[i-1];
	        }
	
	        iNumberOfXData++;

	        //and finally insert
	        dbXValues[iLocation] = dbX;


	        return true;	
        }

        public bool DeleteXValue(int iLocation)
        {
	        if(iLocation < 0 ||
		        iLocation > iNumberOfXData)
		        return false;

	        //shift down
	        for(int i = iLocation; i < iNumberOfXData; i++)
	        {
		        dbXValues[i] = dbXValues[i+1];
	        }
	        //decrement the counter
	        iNumberOfXData--;

	        return true;
        }


        public bool AddYValue(double dbY)
        {
	        if(iNumberOfXData > MAX_DATA_SETS)
		        return false;
	
	        dbYValues[iNumberOfYData] = dbY;

	        iNumberOfYData++;

	        return true;
        }

        public bool AddYValue(double dbY, int iLocation)
        {
	        if(iLocation < 0)
		        return false;
	        else if(iLocation > iNumberOfYData)
		        return false;
	        else if(iLocation == iNumberOfYData) //gotta make one
	        {
		        iNumberOfYData++;
	        }

	        dbYValues[iLocation] = dbY;
	
	        return true;
        }

        public bool InsertYValue(double dbY, int iLocation)
        {
	        if(iLocation < 0 ||
		        iLocation > iNumberOfYData - 1)
		        return false;

	        if(iNumberOfYData >= MAX_DATA_SETS)
		        return false;

	        //shift everything up
	        for(int i = iNumberOfYData ; i > iLocation; i--)
	        {
		        dbYValues[i] = dbYValues[i-1];
	        }
	
	        iNumberOfYData++;

	        //and finally insert
	        dbYValues[iLocation] = dbY;


	        return true;	
        }

        public bool DeleteYValue(int iLocation)
        {
	        if(iLocation < 0 ||
		        iLocation > iNumberOfYData)
		        return false;

	        //shift down
	        for(int i = iLocation; i < iNumberOfYData; i++)
	        {
		        dbYValues[i] = dbYValues[i+1];
	        }
	        //decrement the counter
	        iNumberOfYData--;

	        return true;
        }


        public void ClearXYArrays(bool bClearAll)
        {
	        int iNum = 0;
	        if(bClearAll)
		        iNum = MAX_DATA_SETS;
	        else
	        {
		        iNum = iNumberOfXData;
		        if(iNumberOfXData < iNumberOfYData)
		        iNum = iNumberOfYData;
	        }

        }

        public bool Calculate()
        {
            CalcMeanSumSum2();
            CalcStdDeviation();
            CalcSumXY();

            return CalcLinearReg();
        }


        void CalcMeanSumSum2()
        {
	        int i = 0; //counter variable

	        dbSumX = 0;//of course
	        dbSumX2 = 0;
	        dbSumY = 0;//of course
	        dbSumY2 = 0;

            dbConSumX = 0;//of course
            dbConSumX2 = 0;
            dbConSumY = 0;//of course
            dbConSumY2 = 0;

	        //X mean & SumX & SumX^2
	        for(i=0; i < iNumberOfXData; i++)
	        {
		        switch(eStatsType) 
		        {
		        case EnumStatistics.Stats_Exponential_Reg:
                case EnumStatistics.Stats_Linear_Reg:
                case EnumStatistics.Stats_1_Variable:		
                    dbConSumX += dbXValues[i];
                    dbConSumX2 += dbXValues[i]*dbXValues[i];
                    break;
		        case EnumStatistics.Stats_Ln_Reg:
			        dbConSumX += Math.Log(dbXValues[i]);
                    dbConSumX2 += Math.Log(dbXValues[i])*Math.Log(dbXValues[i]);
			        break;
		        case EnumStatistics.Stats_Log_Reg:
			        dbConSumX +=Math.Log10(dbXValues[i]);
                    dbConSumX2 += Math.Log10(dbXValues[i])*Math.Log10(dbXValues[i]);
			        break;
                case EnumStatistics.Stats_Power_Reg:
                    dbConSumX +=Math.Log(dbXValues[i]);
                    dbConSumX2 += Math.Log(dbXValues[i])*Math.Log(dbXValues[i]);
                    break;
		        default:
			        return;

		        }
                dbSumX += dbXValues[i];
		        dbSumX2 += (dbXValues[i]*dbXValues[i]);
        
	        }
	        dbMeanX = dbSumX / (double)iNumberOfXData;
    
	        //Y mean & Sum Y & SumX^2
	        for(i = 0; i < iNumberOfYData; i++)
	        {
		        switch(eStatsType) 
		        {
		        case EnumStatistics.Stats_Linear_Reg:
		        case EnumStatistics.Stats_Ln_Reg:
		        case EnumStatistics.Stats_Log_Reg:
			        dbConSumY += dbYValues[i];
                    dbConSumY2 += dbYValues[i] * dbYValues[i];
			        break;
		        case EnumStatistics.Stats_Exponential_Reg:
			        dbConSumY += Math.Log10(dbYValues[i]);
                    dbConSumY2 += Math.Log10(dbYValues[i])*Math.Log10(dbYValues[i]);
			        break;
		        case EnumStatistics.Stats_Power_Reg:
			        dbConSumY += Math.Log(dbYValues[i]);
                    dbConSumY2 += Math.Log(dbYValues[i])*Math.Log(dbYValues[i]);
			        break;
		        case EnumStatistics.Stats_1_Variable:
		        default:
			        return;

		        }
                dbSumY += dbYValues[i];
		        dbSumY2 += dbYValues[i] * dbYValues[i];

	        }
	        dbMeanY = dbSumY / (double)iNumberOfYData;
        }

        void CalcStdDeviation()
        {   //use the "real" calculations for the standard deviation ... not useful for nonlinear I don't think
	        double dbTemp;

	        //Standard Deviation with n-1 weighting - Sample Standard Deviation
	        dbTemp = (dbSumX2 - (Math.Pow(dbSumX, 2))/iNumberOfXData)/iNumberOfXData;
	        dbSX = Math.Pow(dbTemp, 0.5);

	        dbTemp = (dbSumY2 - (Math.Pow(dbSumY, 2))/iNumberOfYData)/iNumberOfYData;
	        dbSY = Math.Pow(dbTemp, 0.5);

	        //Standard Deviation with n weighting - Population Standard Deviation
	        dbTemp = (dbSumX2 - (Math.Pow(dbSumX, 2))/iNumberOfXData)/(iNumberOfXData-1);
	        dbPopStrdX = Math.Pow(dbTemp, 0.5);
            dbTemp = (dbConSumX2 - (Math.Pow(dbConSumX, 2))/iNumberOfXData)/(iNumberOfXData-1);
            dbConPopStrdX = Math.Pow(dbTemp, 0.5);

	        dbTemp = (dbSumY2 - (Math.Pow(dbSumY, 2))/iNumberOfYData)/(iNumberOfYData-1);
	        dbPopStrdY = Math.Pow(dbTemp, 0.5);
            dbTemp = (dbConSumY2 - (Math.Pow(dbConSumY, 2))/iNumberOfYData)/(iNumberOfYData-1);
            dbConPopStrdY = Math.Pow(dbTemp, 0.5);
        }

        void CalcSumXY()
        {
	        dbSumXY = 0;
            dbConSumXY = 0;

	        if(iNumberOfXData == iNumberOfYData)
	        {
		        //Sum of XY products
		        for(int i = 0; i < iNumberOfXData; i++)
		        {
			        switch(eStatsType) 
			        {
			        case EnumStatistics.Stats_Linear_Reg:
				        dbConSumXY += dbXValues[i] * dbYValues[i];
				        break;
			        case EnumStatistics.Stats_Ln_Reg:
				        dbConSumXY += Math.Log(dbXValues[i]) * dbYValues[i];
				        break;
			        case EnumStatistics.Stats_Log_Reg:
				        dbConSumXY += Math.Log10(dbXValues[i]) * dbYValues[i];
				        break;
			        case EnumStatistics.Stats_Exponential_Reg:
                        dbConSumXY += dbXValues[i] * Math.Log10(dbYValues[i]);
				        break;
			        case EnumStatistics.Stats_Power_Reg:
				        dbConSumXY += Math.Log(dbXValues[i]) * Math.Log(dbYValues[i]);
				        break;
			        case EnumStatistics.Stats_1_Variable:
			        default:
				        return;
			        }
                    dbSumXY += dbXValues[i] * dbYValues[i];
		        }
	        }
        }


        bool CalcLinearReg()
        {
	        //Assume Auto Compute has been hit
	        if(iNumberOfXData != iNumberOfYData)
		        return false;
            //slope
	        dbLinB = (iNumberOfXData*dbConSumXY - dbConSumY * dbConSumX) / (iNumberOfXData*dbConSumX2 - dbConSumX*dbConSumX);
	        //intercept
            dbLinA = (dbConSumY - dbLinB * dbConSumX)/iNumberOfXData;
            //regression
	        dbRegR = dbLinB * dbConPopStrdX / dbConPopStrdY;

	        return true;
        }

        public bool GetMeanX(ref double dbX)
        {
	        if(iNumberOfXData == 0)					return false;
	        if(!Calculate())			{}//			return false;

	        dbX = dbMeanX;	
	        return true;
        }

        public bool GetMeanY(ref double dbY)
        {
	        if(iNumberOfYData == 0)					return false;
	        if(iNumberOfXData != iNumberOfYData)	return false;
	        if(eStatsType == EnumStatistics.Stats_1_Variable)		return false;
	        if(!Calculate())							return false;

	        dbY = dbMeanY;	
	        return true;
        }

        public bool GetPopSX(ref double dbPopSX)
        {
	        if(iNumberOfXData == 0)					return false;
	        if(!Calculate())							return false;

	        dbPopSX = dbPopStrdX;
	        return true;
        }

        public bool GetPopSY(ref double dbPopSY)
        {
	        if(iNumberOfYData == 0)					return false;
	        if(iNumberOfXData != iNumberOfYData)	return false;
	        if(eStatsType == EnumStatistics.Stats_1_Variable)		return false;
	        if(!Calculate())							return false;

	        dbPopSY = dbPopStrdY;
	        return true;
        }

        public bool GetSumX(ref double dbSumXOut)
        {
	        if(iNumberOfXData == 0)					return false;
	        if(!Calculate())							return false;

            dbSumXOut = dbSumX;
	        return true;
        }

        public bool GetSumY(ref double dbSumYOut)
        {
	        if(iNumberOfYData == 0)					return false;
	        if(iNumberOfXData != iNumberOfYData)	return false;
	        if(eStatsType == EnumStatistics.Stats_1_Variable)		return false;
	        if(!Calculate())							return false;

            dbSumY = dbSumYOut;
	        return true;
        }

        public bool GetSumX2(ref double dbSumX2Out)
        {
	        if(iNumberOfXData == 0)					return false;
	        if(!Calculate())							return false;

            dbSumX2 = dbSumX2Out;
	        return true;
        }

        public bool GetSumY2(ref double dbSumY2Out)
        {
	        if(iNumberOfYData == 0)					return false;
	        if(iNumberOfXData != iNumberOfYData)	return false;
	        if(eStatsType == EnumStatistics.Stats_1_Variable)		return false;
	        if(!Calculate())							return false;

            dbSumY2 = dbSumY2Out;
	        return true;
        }

        public bool GetSumXY(ref double dbSumXYOut)
        {
	        if(iNumberOfYData == 0)					return false;
	        if(iNumberOfXData != iNumberOfYData)	return false;
	        if(eStatsType == EnumStatistics.Stats_1_Variable)		return false;
	        if(!Calculate())							return false;

            dbSumXY = dbSumXYOut;
	        return true;

        }

        public bool GetSX(ref double dbSXOut)
        {
	        if(iNumberOfXData == 0)					return false;
	        if(!Calculate())							return false;

            dbSX = dbSXOut;
	        return true;
        }

        public bool GetSY(ref double dbSYOut)
        {
	        if(iNumberOfYData == 0)					return false;
	        if(iNumberOfXData != iNumberOfYData)	return false;
	        if(eStatsType == EnumStatistics.Stats_1_Variable)		return false;
	        if(!Calculate())							return false;

            dbSY = dbSYOut;
	        return true;
        }

        public bool GetA(ref double dbA)
        {
	        dbA = dbLinA;
	        return true;
        }

        public bool GetB(ref double dbB)
        {
	        dbB = dbLinB;
	        return true;
        }

        public bool GetR(ref double dbR)
        {
	        dbR = dbRegR;
	        return true;	
        }


        public void SetPredictedX(double dbX)
        {
	        dbPredX = dbX;
	        CalcPredY();
        }

        public void SetPredictedY(double dbY)
        {
	        dbPredY = dbY;
	        CalcPredX();
        }

        public void CalcPredY()
        {
	        Calculate();
	        switch(eStatsType) 
	        {
	        case EnumStatistics.Stats_Linear_Reg:
		        dbPredY = dbLinA + dbLinB*dbPredX;
		        break;
	        case EnumStatistics.Stats_Ln_Reg:
		        dbPredY = dbLinA + dbLinB*Math.Log(dbPredX);
		        break;
	        case EnumStatistics.Stats_Log_Reg:
		        dbPredY = dbLinA + dbLinB*Math.Log10(dbPredX);
		        break;
	        case EnumStatistics.Stats_Exponential_Reg:
		        dbPredY = dbLinA + Math.Pow(dbLinB,dbPredX);
		        break;
	        case EnumStatistics.Stats_Power_Reg:
		        dbPredY = dbLinA + Math.Pow(dbPredX,dbLinB);
		        break;
	        case EnumStatistics.Stats_1_Variable:
	        default:
		        return;
	        }
			
        }

        public void CalcPredX()
        {
	        Calculate();
	        switch(eStatsType) 
	        {
	        case EnumStatistics.Stats_Linear_Reg:
		        dbPredX = (dbPredY - dbLinA)/dbLinB;
		        break;
	        case EnumStatistics.Stats_Ln_Reg:
		        dbPredX = Math.Pow(CalcGlobals.CONST_NUM_e, (dbPredY - dbLinA)/dbLinB);
		        break;
	        case EnumStatistics.Stats_Log_Reg:
		        dbPredX = Math.Pow(10, (dbPredY - dbLinA)/dbLinB);
		        break;
	        case EnumStatistics.Stats_Exponential_Reg:
		        dbPredX = Math.Log10(dbLinA - dbPredY)/Math.Log10(dbLinB);
		        break;
	        case EnumStatistics.Stats_Power_Reg:
		        dbPredX = Math.Pow(CalcGlobals.CONST_NUM_e, Math.Log(dbPredY - dbLinA)/dbLinB);
		        break;
	        case EnumStatistics.Stats_1_Variable:
	        default:
		        return;
	        }
        }


        public double RecallXValue(int iLocation)
        {
		
	        return dbXValues[iLocation]; 
	
        }

        public double RecallYValue(int iLocation)
        {
	        return dbYValues[iLocation]; 
	
        }

        public int GetNumberOfXYPairs()
        {
            return iNumberOfYData;//I think ...
        }

        void InsertDebugData()
        {
            return;

     /*       ResetWorkSheet();

            //for testing exponential
            iNumberOfXData		= 14;
            iNumberOfYData		= 14;

            dbXValues[0] = 0; 
            dbYValues[0] = 179.5; 
            dbXValues[1] = 5; 
            dbYValues[1] = 168.7; 
            dbXValues[2] = 8; 
            dbYValues[2] = 158.1; 
            dbXValues[3] = 11; 
            dbYValues[3] = 149.2; 
            dbXValues[4] = 15; 
            dbYValues[4] = 141.7; 
            dbXValues[5] = 18; 
            dbYValues[5] = 134.6; 
            dbXValues[6] = 22; 
            dbYValues[6] = 125.4; 
            dbXValues[7] = 25; 
            dbYValues[7] = 123.5; 
            dbXValues[8] = 30; 
            dbYValues[8] = 116.3; 
            dbXValues[9] = 34; 
            dbYValues[9] = 113.2; 
            dbXValues[10] = 38; 
            dbYValues[10] = 109.1; 
            dbXValues[11] = 42; 
            dbYValues[11] = 105.7; 
            dbXValues[12] = 45; 
            dbYValues[12] = 102.2; 
            dbXValues[13] = 50; 
            dbYValues[13] = 100.5; 

            */
        }
            

    }
}
