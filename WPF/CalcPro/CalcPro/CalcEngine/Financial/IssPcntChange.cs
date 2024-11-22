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
    public class IssPcntChange
    {
        const double CONST_NUM_e = 2.718281828459045235;

        double dbNominalRate;	//enter/compute
        double dbEffectiveRate;	//enter/compute
        int iPeriodsPerYr;	//enter only

        	
	    public double		GetNominalRate(){return dbNominalRate;}
	    public double		GetEffectiveRate(){return dbEffectiveRate;}
	    public int			GetPeriodsPerYr(){return iPeriodsPerYr;}

        public void			SetNominalRate(double dbIn){dbNominalRate = dbIn;}
	    public void			SetEffectiveRate(double dbIn){dbEffectiveRate = dbIn;}
	    public void			SetPeriodsPerYr(int iPeriods){iPeriodsPerYr = iPeriods;}
		
        public IssPcntChange()
        {
            Clear();
        }

        public void  Clear()
        {
	        iPeriodsPerYr		= 12;
	        dbNominalRate		= 0;
	        dbEffectiveRate	= 0;
        }

        public double CalcEffectiveRate()
        {
	        double dbX			= 0.01 * dbNominalRate / (double)iPeriodsPerYr;
	        double dbTemp		= iPeriodsPerYr * Math.Log(dbX + 1);

	        dbEffectiveRate	= 100.0 * (Math.Pow(CONST_NUM_e, dbTemp) - 1.0);

	        return dbEffectiveRate;
        }

        public double CalcNominalRate()
        {
	        double dbX			= 0.01 * dbEffectiveRate;
	        double dbTemp		= 1.0 / (double)iPeriodsPerYr * Math.Log(dbX + 1);

	        dbNominalRate		= 100.0 * (double)iPeriodsPerYr * (Math.Pow(CONST_NUM_e, dbTemp) - 1);

	        return dbNominalRate;
        }
    }
}
