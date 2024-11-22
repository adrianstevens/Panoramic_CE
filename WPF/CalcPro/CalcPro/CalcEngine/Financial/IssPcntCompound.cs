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

public enum EnumPcntScreenEntries
{
    PCNT_N,
    PCNT_IY,
    PCNT_PV,
    PCNT_FV,

};

namespace CalcPro
{
    public class IssPcntCompound
    {
        private double dbOriginalValue;
        private double dbNewValue;
        private double dbPercentage;
        private int iNumPeriods;
        public IssPcntCompound()
        {
            Clear();
        }


        public double GetOriginalValue() {return dbOriginalValue; }
        public double GetNewValue() {return dbNewValue; }
        public double GetInterestRate() {return dbPercentage; }
	    public int	  GetNumPeriods(){return iNumPeriods;}


        public bool Clear()
        {
	        dbOriginalValue	= 0.0;
	        dbNewValue		= 0.0;
	        dbPercentage	= 0.0;
	        iNumPeriods		= 1;

	        return true;
        }

        public bool SetNewValue(double dbValue)
        {
	        dbNewValue = dbValue;
	        return true;
        }

        public bool SetOriginalValue(double dbValue)
        {
	        dbOriginalValue = dbValue;
	        return true;
        }

        public bool SetInterestRate(double dbInterestRate)
        {
	        dbPercentage = dbInterestRate;
	        return true;
        }

        public bool SetNumPeriods(int iValueNumPeriods)
        {
            if (iValueNumPeriods < 1)
                iNumPeriods = 1;
            else
	            iNumPeriods = iValueNumPeriods;
	        return true;
        }



        public double CalcNewValue()
        {
	        dbNewValue = dbOriginalValue*Math.Pow((1+dbPercentage/100.0),iNumPeriods);
	        return dbNewValue;
        }
    
        public double CalcInterestRate()
        {
	        double dbTemp = Math.Log10(dbNewValue/dbOriginalValue)/iNumPeriods;
	        dbPercentage = (Math.Pow((double)10.0, dbTemp));
            dbPercentage = - 100.0 * (1.0 - dbPercentage);
            return dbPercentage;

        }

        public double CalcOriginalValue()
        {
	        dbOriginalValue = dbNewValue / Math.Pow((1+dbPercentage/100.0),iNumPeriods);
	        return dbOriginalValue;
        }

        public int CalcNumPeriods()
        {
	        iNumPeriods = (int)(Math.Log(dbNewValue/dbOriginalValue)/Math.Log(1.0+dbPercentage/100.0));
	        return iNumPeriods;
        }

    }
}
