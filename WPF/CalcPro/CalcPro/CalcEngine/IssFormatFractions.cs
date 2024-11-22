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
    public class IssFormatFractions
    {
        public IssFormatFractions()
        {
        }

        public const int MAX_LENGTH = 14; //aribtrary
        public const int MAX_DECIMAL = 10; //might be too large
        public const double FRACTION_MAX_PRECISION = 10.0e-15;  // Significant digits
        public const int MAX_DENOMINATOR = 999;

        public static bool GetFraction(string szValue, ref string szWholeNum, ref string szNumerator, ref string szDenominator)
        {
	        if(szValue.Length == 0)
		        return false;

	        double dbVal = Convert.ToDouble(szValue);

	        bool bNeg = false;
	        if(dbVal < 0)
		        bNeg = true;

	        int iDecimalLoc = szValue.IndexOf(".");
	        int iLen = szValue.Length;

            if(iDecimalLoc == -1)
                return false;


	        double dbError = FRACTION_MAX_PRECISION;

	        int iMaxDen = MAX_DENOMINATOR;
	        if(iDecimalLoc != -1)
	        {
		        iMaxDen = (int)Math.Pow(10.0, (double)(iLen - iDecimalLoc - 1));
		        if(iMaxDen < 0)
			        iMaxDen = MAX_DENOMINATOR; //int overflow ... ooops
		        else
			        dbError = 1.0/(double)iMaxDen;
	        }
	        Int64 iWhole = 0;
	        Int64 iNumerator = 0;
	        Int64 iDenominator = 0;

	        //we'll try both methods
	        double dbRet1 = GetFraction(dbVal, ref iWhole, ref iNumerator, ref iDenominator, iMaxDen);
	        if(dbRet1 > dbError)
	        {
		        double dbRet2 = GetFraction(dbVal, ref iWhole, ref iNumerator, ref iDenominator, dbError);
		        if(dbRet2 > dbError)
			        return false;
	        }

	        if(iDenominator > MAX_DENOMINATOR || iDenominator == 0)//non fraction yo
		        return false;

	        //fill our TCHARs and call it a day
	        if(iWhole == 0)
	        {
                if (bNeg)
                {
                    szWholeNum = "-";
                }
                else
                {
                    szWholeNum = "";
                }
	        }
	        else
	        {
                szWholeNum = iWhole.ToString();
	        }

	        if(iNumerator == 0 || iDenominator == 0)
	        {
                szNumerator = "";
                szDenominator = "";
	        }
	        else
	        {
                szNumerator = iNumerator.ToString();
                szDenominator = iDenominator.ToString();
		        
	        }


	

	        return true;
        }

        public static double GetFraction(double f, ref Int64 Whole, ref Int64 Num, ref Int64 Den, int MaxDen)
        {
	        Int64 i;
	        Int64 A = 0;
	        Int64 B = 0;
	        double AllowedError = FRACTION_MAX_PRECISION; 
	        double Precision = AllowedError;
	        double d = f;
	        Whole = (Int64)(d+Precision);
	        d -= Whole;
	        d = Math.Abs(d);
	        double Frac = d;
	        double Diff = Frac;
	        Num = 1;
	        Den = 0;
	        if (Frac > Precision)
		        while (true) 
		        {
			        d = 1.0/d;
			        i = (Int64)(d+Precision);
			        d -= i;
			        if (A != 0)
				        Num = i * Num + B;
			        Den = (Int64)(Num/Frac+0.5);
                    Diff = Math.Abs((double)Num / Den - Frac);
			        if (Den > MaxDen)
			        {
				        if (A != 0)
				        { 
					        Num = A;
					        Den = (Int64)(Num/Frac+0.5);
                            Diff = Math.Abs((double)Num / Den - Frac);
				        }
				        else
				        {
					        Den = MaxDen;
					        Num = 1;
					        Diff = Math.Abs((double)Num/Den-Frac);
					        if (Diff > Frac)
					        {
						        Num = 0;
						        Den = 1; // Keeps final check below from adding 1 and keeps Den from being 0
						        Diff = Frac;
					        }
				        }
				        break;
			        }
			        if ((Diff <= AllowedError) || (d < Precision))
				        break;
			        Precision = AllowedError/Diff;
			        // This calcualtion of Precision does not always provide results within
			        // Allowed Error. It compensates for loss of significant digits that occurs.
			        // It helps to round the inprecise reciprocal values to i. 
			        B = A;
			        A = Num;
		        }
		        if (Num == Den)
		        {
			        Whole++;
			        Num = 0;
			        Den = 0;
		        }
		        else
			        if (Den == 0) 
				        Num = 0;
		        return Diff;
        }



        public static double GetFraction(double f, ref Int64 Whole, ref Int64 Num, ref Int64 Den, double AllowedError)
        {
	        Int64 i;
	        Int64 A = 0;
	        Int64 B = 0;

	        double Precision = AllowedError; 
	        double d = f;
	
	        Whole = (Int64)(d+Precision);
	        d -= Whole;
	        
            if (d < 0)
                d *= -1.0;

	        double Frac = d;
	        double Diff = Frac;
	        Num = 1;
	        Den = 0;
	        if (Frac > Precision)
	        {
		        while (true) 
		        {
			        d = 1.0/d;
			        i = (Int64)(d+Precision);
			        d -= i;
			        if (A != 0)
				        Num = i * Num + B;
			        Den = (Int64)(Num/Frac+0.5);
			        Diff = Math.Abs((double)Num/Den-Frac);
			        if ((Diff <= AllowedError) || (d < Precision))
				        break;
			        Precision = AllowedError/Diff; 
			        // This calculation of Precision does not always provide results within
			        // Allowed Error. It compensates for loss of significant digits that occurs.
			        // It helps to round the imprecise reciprocal values to i. 
			        B = A;
			        A = Num;
		        }
	        }

	
	        if (Num == Den)
	        {
		        Whole++;
		        Num = 0;
		        Den = 0;
	        }
	        else
	        {
		        if (Den == 0) 
			        Num = 0;
	        }

	        return Diff;
        }


    }
}
