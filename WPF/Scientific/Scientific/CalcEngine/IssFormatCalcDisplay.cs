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
    public class IssFormatCalcDisplay
    {
        public IssFormatCalcDisplay()
        {
            eDisplay				= DisplayType.DISPLAY_Float;
            bUseComma               = false;
            bUseRegional            = false;

	        iSigFigs				= 15;
	        iFixedDigits			= 2; //currency is the most likely one to be used

            bAlwaysShowDecimal	    = true;
        }

        static int VALUE_LENGTH_OFFSET = 2;

        static double MAX_DMS_VALUE = 999999.59;

        DisplayType eDisplay;

        bool bUseComma;
        bool bUseRegional;
        bool bAlwaysShowDecimal;

        int iSigFigs;
        int iFixedDigits;

        public DisplayType		GetDisplayType(){return eDisplay;}
        public void			    SetUseComma(bool bUse){bUseComma=bUse;}
	    public bool			    GetUseComma(){return bUseComma;}
        
        public bool			    SetFixedDecDigits(int iDigits){if(iDigits < 0 || iDigits > 9)return false; iFixedDigits = iDigits; return true;}
	    public int				GetFixedDecDigits(){return iFixedDigits;}
               

        bool FormatNumber(string szNumber, ref string szResult, DisplayType eDisplay, int iSigFigs)
        {
	        //check to make sure we have something in szNumber
	        if(szNumber==null)
		        return false;

	        else if(szNumber.Length<1)
	        {
                szResult = "";
		        return true;
	        }

	        //sometimes we don't get a period so add one on
	        if(szNumber.IndexOf(".")==-1 && szNumber.IndexOf("e") == -1)
		        szNumber += (("."));

	        switch(eDisplay) 
	        {
	        case DisplayType.DISPLAY_Float:
		        FormatNumberToFloat(szNumber, ref szResult, iSigFigs);
		        if(bUseRegional && szResult.IndexOf("e")==-1)
			        FormatRegional(szResult, szResult);
		        else 
			        //clean up trailing garbage .. this will also call RoundNumber to correctly get sig figs and round up on non exponent numbers
			        CleanUpNumber(szResult, ref szResult);
		        break;
	        case DisplayType.DISPLAY_Fixed:
		        {
			        bool bRet = FormatNumberToFixed(szNumber, ref szResult, iSigFigs);
			        if(bUseRegional && szResult.IndexOf("e")==-1)
				        FormatRegional(szResult, szResult);
			        if(bRet == false)
				        CleanUpNumber(szResult, ref szResult);
		        }
		        break;
	        case DisplayType.DISPLAY_Scientific:
		        FormatNumberToSci(szNumber, ref szResult);
		        CleanUpNumber(szResult, ref szResult);
		        break;
	        case DisplayType.DISPLAY_Engineering:
		        FormatNumberToEng(szNumber, ref szResult);
		        CleanUpNumber(szResult, ref szResult);
		        break;
	        default:
		        return false;
	        }

	        return true;
        }

        bool RemoveTrailingZeros(ref string szNum)
        {
            //clean up any trailing 0s
            if (szNum.IndexOf(".") != -1)
            {
                while (1 == 1)
                {
                    if (szNum.IndexOf("0", szNum.Length - 1) == szNum.Length - 1)
                        szNum = szNum.Substring(0, szNum.Length - 1);
                    else
                        break;
                }
            }
            return true;
        }

        bool CleanUpNumber(string szNumber, ref string szResult)
        {
            if (szNumber.Length < 3)
                return true;

            string szExp = "";
	        string szValue;

            szExp = "";
            szValue = "";

	        szResult = szNumber;

	        //quick check for the 0. case
	        if(szNumber[0] == ('0') &&
		        szNumber[1] == ('.') &&
		        szNumber.Length == 2 &&
		        bAlwaysShowDecimal)
		        return true;
		

	        //CleanUpTime////////////////////////////////////////////////////////////////
	        //slightly redundant but it makes for easy changes if something gets goofed
	        if(szResult.IndexOf("e")!=-1)
	        {
		        //we've got an exponent
		        szExp = szResult;
		        szValue = szResult;
		        szExp = szExp.Substring(szExp.Length - 5);
                szValue = szValue.Substring(0, szValue.Length - 5);

		        //clean up any trailing 0s
                RemoveTrailingZeros(ref szValue);

		        //lets get rid of any trailing periods
		        if(szValue.IndexOf(".")==(szValue.Length-1) && bAlwaysShowDecimal == false)
                    szValue = szValue.Substring(0, szValue.Length - 1);

 		        szResult = szValue;
		        //trim off the non exponents...makes it cleaner
                if (szExp != ("e+000") &&
                    szExp != ("e-000"))
                {
			        szResult += szExp;
		        }
	        }
	        else 
	        {
		        RoundNumber(szResult, szResult);		
		        if(bAlwaysShowDecimal == false)
		        {
			        //lets get rid of any trailing periods
			        int iLength = szResult.Length;
			        int iIndex	= szResult.IndexOf(".", iLength - 1);
			        if(iIndex==iLength-1)
			        {
				        szResult = szResult.Substring(0, szResult.Length - 1);
			        }
		        }
	        }

	        return true;
        }

        bool RoundNumber(string szNumber, string szResult)
        {
            //technically we should copy szNumber into szResult first but I know how its called...
	        if(szNumber == null)
		        return false;
            
            if (szNumber.IndexOf("e") != -1)
                //|| szNumber.IndexOf("e") != -1 ) ... I assume I was thinking about something but we don't need to check twice .... Adrian, May 2010
                return false;

            int iLen = szNumber.Length;
            if (iLen < iSigFigs)
                return true;

            char[] cNumber = new char[iLen];

            for (int i = 0; i < iLen; i++)
                cNumber[i] = szNumber[i];

	         //check for negative values
	        int iOffset = 0;
	        if(szResult.IndexOf("-") != -1)
		        iOffset = 1;

            //can't round whole numbers
    
            
	        //Round the number
	        //We'll work from ASCII values cause we can....and we're assuming everything is either a number or a decimal
            if (cNumber.Length > iSigFigs && cNumber[iSigFigs] > 53 && cNumber[iSigFigs] < 58) //its between 5 & 9 inclusive....so round up....technically 5.5 wouldn't go up but that's a preference
	        {
		        //delete all previous values 
		        szResult = szNumber;//just in case they are different
		        szResult = szResult.Substring(0, iLen - iSigFigs);
		        
                int iIndex = iSigFigs - 1;
		        while (iIndex > -1)
		        {
			        if(szNumber[iIndex]==46 ||//period
				        szNumber[iIndex]==45)//negative
			        {
			        }
			        else
			        {
                        cNumber[iIndex]++;
				        if(szNumber[iIndex]==58)//we're past 9
				        {
                            cNumber[iIndex] = '0'; //0
                            if (iIndex == iOffset)
                            {
                                //m_oStr->Insert(szNumber, ("1"), iOffset);//clever eh? :)...quick sudo hack for the negative

                                for (int i = cNumber.Length - 1; i > iIndex; i--)
                                {
                                    cNumber[i + 1] = cNumber[i];
                                }
                                cNumber[iIndex] = '1';
                            }
				        }
				        else
				        {
					        break;//we're outta here
				        }

			        }
			        iIndex--;
		        }

	        }
	        else
	        {
		        //delete all previous values 
		        szResult = szNumber;//just in case they are different
		//        m_oStr->Delete(iSigFigs+1+iOffset, iLen - iSigFigs -1 - iOffset, szResult);
                int iTemp = iLen - iSigFigs - 1 - iOffset;

                if(iTemp > 0)
                    szResult = szResult.Remove(iSigFigs + 1 + iOffset, iTemp);
		
	        }
	
	        //and clean up trailing zeros :p
	        //ugly hack....this class sucks
	        iLen = szResult.Length;
	        if(szResult.IndexOf(".") != -1)
                while(szResult[iLen - 1] == ('0'))
		        {
                    szResult = szResult.Substring(0, iLen - 1);
			        iLen--;
		        }
	        return true;
        }


        bool InsertRegionalSperator(string szFull, string szValue)
        {
	     /*   int iPeriod = szFull.IndexOf(".");
	        
            if (iPeriod != -1)
	        {
		        m_oStr->Delete(iPeriod, 1, szFull);
		        m_oStr->Insert(szFull, lpf.lpDecimalSep, iPeriod);
	        }

	        iPeriod = szValue.IndexOf(".");
	        if (iPeriod != -1)
	        {
		        m_oStr->Delete(iPeriod, 1, szValue);
		        m_oStr->Insert(szValue, lpf.lpDecimalSep, iPeriod);
	        }
	        return true;*/

            //bugbug
            return false;
        }

        bool InsertThousandsSeparator(string szFull, string szValue, string szExp, int iSigFigs)
        {
	        //no thousands separator if we have an exponent
	     /*   if(szExp.Length>0)
		        return false;

	        //now lets see if we have a number large enough
	        int iPeriod = m_oStr->Find(szFull, lpf.lpDecimalSep, 0);

	        int iLength = m_oStr->GetLength(szFull);
	        int iIndex = 0;
	
	        //if iPeriod is -1...we have a complete whole number
	        if(iPeriod == -1)
	        {
		        //now we need to know how many separators we can fit
		        int iNum = iLength/3;

		        if(iNum + iLength > iSigFigs)
			        return false;

		        iIndex = iLength-3;
	        }
	        else //means we have decimals too
	        {
		        int iNum = iPeriod/3;

		        if(iNum + iLength > iSigFigs)
			        return false;

		        iIndex = iPeriod - 3;
	        }

	        while(iIndex > 0 && szFull[iIndex-1] != ('-'))
	        {
		        m_oStr->Insert(szFull, lpf.lpThousandSep, iIndex);
		        m_oStr->Insert(szValue, lpf.lpThousandSep, iIndex);
		        iIndex-=3;
	        }

	        return true;*/

            return false;
        }

        //this function is for base 10 only
        public bool NumberFormat(string szNumber, 
								    ref string szResultFull /* = null */, 
								    ref string szResultValue /* = null */, 
								    ref string szResultExp /* = null */, 
								    bool bUseRegional /* = false */, 
								    int iSigFigs /* = 0 */)
        {
        /*    if(szNumber.IndexOf("e") != -1)
            {
                szResultFull = szNumber;
                return true;
            }*/

            if (IsANumber(szNumber) == false)
            {
                szResultFull = szNumber;
                szResultValue = szNumber;
                szResultExp = "";
                return false;
            }
            //bugbug ... 
            if (szNumber.IndexOf("e") != -1)
            {
                //szNumber = szNumber.Replace("E", "e");
                //now check if the exponent is formatted correctly
                int iIndex = szNumber.IndexOf("e");

                //we need 2 zeros
                if(iIndex > szNumber.Length - 4)
                {
                    szNumber = szNumber.Insert(iIndex + 2, "00");
                }
                //we need 1 zero
                else if (iIndex > szNumber.Length - 5)
                {
                    szNumber = szNumber.Insert(iIndex + 2, "0");
                }
                szResultFull = szNumber;
             //   return true;
            }


	        //We'll make this easy for fractions
	        if(eDisplay == DisplayType.DISPLAY_Fractions)
	        {
                szResultValue = "";
                szResultExp = "";
                szResultFull = "";

		        if(IssFormatFractions.GetFraction(szNumber, ref szResultFull, ref szResultValue, ref szResultExp) == false)
		        {	// if the fractions fail we're back to float
			        eDisplay = DisplayType.DISPLAY_Float;
			        NumberFormat(szNumber, ref szResultFull, ref szResultValue, ref szResultExp, bUseRegional, iSigFigs);
			        eDisplay = DisplayType.DISPLAY_Fractions;
		        }
		        return true;//return regardless
	        }
            //let's handle DMS here 
            else if(eDisplay == DisplayType.DISPLAY_DMS)
            {
                szResultValue = "";
                szResultExp = "";
                szResultFull = "";

                if(FormatNumberToDMS(szNumber, ref szResultFull) == false)
                {
                    eDisplay = DisplayType.DISPLAY_Float;
                    NumberFormat(szNumber, ref szResultFull, ref szResultValue, ref szResultExp, bUseRegional, iSigFigs);
                    eDisplay = DisplayType.DISPLAY_Fractions;
                }
                return true;
            }
            else if(eDisplay == DisplayType.DISPLAY_BCD)
            {
                szResultValue = "";
                szResultExp = "";
                szResultFull = "";

                if(FormatNumberToBCD(szNumber, ref szResultFull) == false)
                {
                    eDisplay = DisplayType.DISPLAY_Float;
                    NumberFormat(szNumber, ref szResultFull, ref szResultValue, ref szResultExp, bUseRegional, iSigFigs);
                    eDisplay = DisplayType.DISPLAY_BCD;
                }
                return true;
            }

	        string szResult;
            szResult = "";
	
	        if(szNumber==null)
		        return false;

	        //check for output errors
	        if(CheckStringForErrors(szNumber, ref szResultFull, ref szResultValue, ref szResultExp))
		        return true;

	        //this is where we set it to Scientific, Eng, Float, etc
	        //also takes care of our sig figs for us ... sort of ...
	        FormatNumber(szNumber, ref szResult, eDisplay, iSigFigs);

	        if(szResultValue!=null)
		        GetValue(szResult, ref szResultValue, iSigFigs);
	        if(szResultExp!=null)
		        GetExponent(szResult, ref szResultExp);

	        if(szResultFull!=null)
	        {
                string szValTemp = "";
                string szExpTemp = "";

		        GetValue(szResult, ref szValTemp, iSigFigs);
                if (szValTemp == "")
                    szValTemp = szResultValue;

		        GetExponent(szResult, ref szExpTemp);


		        szResultFull = szValTemp;
		        if(szExpTemp.Length>0)
		        {
			        szResultFull += ("e");
			        if(szExpTemp.IndexOf("-")==-1)
				        szResultFull += ("+");
			        szResultFull += szExpTemp;
		        }
	        }

	        //now lets do the regional - by hand of course
	        if(bUseRegional)
	        {
		        //now lets switch the period for our separator
		        InsertRegionalSperator(szResultFull, szResultValue);
		        InsertThousandsSeparator(szResultFull, szResultValue, szResultExp, iSigFigs);
		    }
            return true;
        }


        bool FormatNumberToEng(string szNumber, ref string szResult)
        {
            string szExp = "";
	        string szValue;

	        double dAnswer;

	        int	 iExp;
	        int  iCounter;
	        int  iPeriodLocation;
	        int	 iRemainder;
	        bool bNegative=false;

	        dAnswer = StringToDouble(szNumber);

            szExp = "";
            szValue = "";
	        szResult = szNumber;

	        //Format it into Scientific first
	        FormatNumberToSci(szResult, ref szResult);

		        //make sure we at least have an e for the exponent
            iExp = szResult.IndexOf("e");
            if (iExp == -1)
		        return false;

            int iTemp = iExp + 2;
            szExp = szResult.Substring(iTemp);
            szValue = szResult.Remove(iExp);

            //note .. iExp changes uses here
            iExp = Convert.ToInt32(szExp);
	        iRemainder	= iExp % 3;

	        if(iRemainder==0)
		        return true;


	        if(szResult.IndexOf("-") == 0)
	        {
		        bNegative=true;
                szResult = szResult.Remove(0, 1);
                szValue = szValue.Remove(0, 1);
	        }

	        //check for negative	
	        if(szResult.IndexOf("-") !=-1)
	        {
		        //move the period along for every remainder
		        for(int i=0; i<(3-iRemainder); i++)
		        {
			        iPeriodLocation= szValue.IndexOf(".");
                    szValue = szValue.Remove(iPeriodLocation, 1);
                    
			        //if we run out of room, add a zero on to the end...ie...increase the value by a factor of 10
                    if (iPeriodLocation == szValue.Length)
                        szValue = "0" + szValue;

                    szValue = szValue.Insert(iPeriodLocation + 1, ".");
                    iExp++;
		        }
		
		        //Now rebuild the string
		        szResult = szValue;
                if (szResult.Length > iSigFigs)
                    szResult = szResult.Remove(iSigFigs, szResult.Length - 16);

			        

		        //add the first characters for the exponent
		        szResult += ("e-");

                szExp = iExp.ToString();

		        //fill in the extra zeros for proper formatting
		        iCounter=3-szExp.Length;

		        for(int j=0;j<iCounter;j++)
		        {
			        szExp = "0" + szExp;
		        }

		        szResult += szExp;			

	        }
	        else 	//positive
	        {
		        for(int i=0;i<iRemainder; i++)
		        {
			        iPeriodLocation= szValue.IndexOf(".");
                    szValue = szValue.Remove(iPeriodLocation, 1);

                    if (iPeriodLocation == szValue.Length)
                        szValue = "0" + szValue;

                    szValue = szValue.Insert(iPeriodLocation + 1, ".");
			        iExp--;
		        }

		        //Now rebuild the string
                if (szValue.Length > iSigFigs)
                    szValue = szValue.Remove(iSigFigs);

			    //add the first characters of the exponent
		        szValue += ( ("e+"));

                szExp = iExp.ToString();

		        iCounter=3-szExp.Length;
		        //make sure we have 3 digits for the exponent
		        for(int j=0;j<(iCounter);j++)
		        {
			        szExp = "0" + szExp;
		        }
		        //and the exponent characters to the value to make it complete
		        szValue += szExp;
		        szResult = szValue;
	        }
	        //return the negative sign
	        if(bNegative)
		        szResult = "-" + szResult;

	        return true;
        }

        bool FormatNumberToFixed(string szNumber, ref string szResult, int iSigFigs)
        {
	        double  dbAnswer;
            dbAnswer = StringToDouble(szNumber);
            
            //if we're out of range you're out of luck
	        if((dbAnswer<(Math.Pow(10.0,(double)(iSigFigs-1))) && dbAnswer>(Math.Pow(10.0,(double)-1*(iSigFigs-4)))) ||
		         (dbAnswer>(-1*(Math.Pow(10.0,(double)(iSigFigs-1)))) && dbAnswer<(-1*(Math.Pow(10.0,(double)-1*(iSigFigs-4))))))
	        {
                string szTest;
                string szFormat;

                szFormat = "{0:0.";
                for(int i = 0; i < iFixedDigits; i++)
                    szFormat += "#";
                szFormat += "}";

                szTest = String.Format(szFormat, dbAnswer);
               

                
		        //if we're over length
		        if(szTest.Length>iSigFigs)
                {
                    FormatNumberToFloat(szNumber, ref szResult, iSigFigs);
                    return false; //since we couldn't fix it
                }

                szResult = szTest;
	        }
	        else
	        {
		        FormatNumberToFloat(szNumber, ref szResult, iSigFigs);
		        return false;
	        }
	        return true;
        }

        bool FormatNumberToFloat(string szNumber, ref string szResult, int iSigFigs)
        {
            string szExp = "";
	        string szValue;

            string szTemp;
            
	        double dAnswer;

	        //bool	bNegative=false;

            dAnswer = StringToDouble(szNumber);

	        if(dAnswer > Math.Pow(10.0, (double)(iSigFigs-2)) || -1.0*dAnswer > Math.Pow(10.0, (double)(iSigFigs-2)))//we use the two because of the extra - character
		        return FormatNumberToSci(szNumber, ref szResult);

            szExp = "";
            szValue = "";
	        szResult = szNumber;

	        if(dAnswer<1 && dAnswer > -1)
	        {
                //m_oStr->Format(szTemp, ("%g"), dAnswer);
                szTemp = String.Format("{0:0.0}", dAnswer);//not sure if this is any different than ConvertTo ...

		        //ok...lets get the value of the other style

		        //if there's going to be an exponent anyways...just use the other method
		        //otherwise...continue
		        if(szTemp.IndexOf("e") != 0)
			        return true;
		
		        szValue = szResult;
		        //look for an e in szValue...if its not there than gcvt is fine
		        if(szResult.IndexOf("e")!=-1)
		        {
			        
			        if(szResult.IndexOf("-")==0)
			        {
				    
                        szResult = szResult.Remove(0, 1);
			        }

			        int iExp;
			        //Get the int value of the exponent...few steps below
			        szExp = szResult;
			        //its gonna be negative so we don't worry about getting the sign
			        //m_oStr->Delete(0, szExp.Length-3,szExp);
                    szExp = szExp.Remove(0, szExp.Length - 3);
			        //and we have our exponent value
                    iExp = Convert.ToInt32(szExp);
			        
			        //remove the exponent part
                    szValue = szValue.Remove(szValue.Length-5,5);
								
			        //delete the period in Value
			        int j;
			        if((j=szValue.IndexOf("."))!=-1)
			        {
                        szValue = szValue.Remove(j,1);
			        }

			        //now add some zeros as nesessary
			        for(int k=0;k<iExp;k++)
			        {
                        szValue = "0" + szValue;
			        }
			        //put our decimal back in
			        szValue = szValue.Insert(1, ".");
			        //shorten the sucka
			        
                    if(szValue.Length>iSigFigs)
			        {
                        szValue = szValue.Remove(iSigFigs, szValue.Length - iSigFigs);
			        }
			        szResult = szValue;
                    szResult = "-" + szResult;
		        }
	        }	
	        return true;
        }


        //pretty limited for now 
        //no exponential numbers, only positive whole numbers for now
        //and a length of less than 4 for now ...
        public bool FormatNumberToBCD(string szNumber, ref string szResult)
        {
            if(szNumber == null || szResult == null)
                return false;

            int iLen = szNumber.Length;

            if(szNumber.IndexOf("e") != -1)
                return false;

            if(szNumber.IndexOf("-") != -1)
                return false;

            if(szNumber.IndexOf(".") != -1)
                return false;

            bool bSpaces = true;

            //6 digits fit nicely
            if(iLen > 6)
                bSpaces = false;
            if(iLen > 8)
                return false;

            int iValue = 0;

            string[] szBCD = new string[]
            {
                ("0000"),
                ("0001"),
                ("0010"),
                ("0011"),
                ("0100"),
                ("0101"),
                ("0110"),
                ("0111"),
                ("1000"),
                ("1001")
            };

            szResult = "";

            //now lets convert our number to BCD ... easy enough
            for(int i = 0; i < iLen; i++)
            {
                iValue = szNumber[i] - ('0');

                if(iValue < 0 || iValue > 9)
                    continue;

                if(i != 0 && bSpaces)
                    szResult += ( (" "));

                szResult += ( szBCD[iValue]);
            }
            return true;
        } 

        public bool FormatNumberToDMS(string szNumber, ref string szResult)
        {
            if(szNumber == null || szResult == null)
                return false;

            if(szNumber.IndexOf("e") != -1)
                return false;

        //    TCHAR szAppend[] = ("°00\x203200.00\x2033");

            string szAppend = ("°00") + ("\x2032") + ("00");
            //m_oStr->Concatenate(szAppend, ("\x2033"));

            double dbNumber = StringToDouble(szNumber);

            double dbDMS;
            bool bNeg = false;

            if(dbNumber < 0)
            {
                bNeg = true;
                dbNumber *= -1;
            }

            //its too large .... you get normal formatting
            if(Math.Abs(dbNumber) > MAX_DMS_VALUE)
                return false;

            //convert the double value to DMS
            double dDeg, dMin, dSec;
            dDeg = Math.Floor(dbNumber);
            dMin = 0.6*(dbNumber - dDeg);
            if(dMin != 0)
                dMin += 0.0000000000001;//fudge factor
            dSec = (dMin * 100.0 - Math.Floor(dMin * 100.0))*60.0;
            dMin = Math.Floor(dMin * 100.0);
            dbDMS = dDeg + dMin/100.0 + dSec/10000.0;

            bool bSmall = false;
            if(dbDMS < 1 && dbDMS > 0)
            {
                bSmall = true;
                dbDMS += 1;
            }

            //m_oStr->DoubleToString(szResult, dbDMS, 12);//seems like good number of sig figs
            szResult = dbDMS.ToString();
            if (szResult.Length > 13)
            {
                szResult = szResult.Substring(0, 13);
                RemoveTrailingZeros(ref szResult);

            }



            if(szResult.IndexOf("e") != -1)
                return false;

            if (bSmall == true)
            {
                //szResult[0] = ('0');//hacked
                szResult = "0" + szResult.Remove(0, 1);
            }

            if (bNeg)
            {
                szResult = "-" + szResult;
            }

            int iLocation = szResult.IndexOf(".");
            int iLen = szResult.Length;

            if(iLocation == -1)//perfect .. this is easy
            {
                szResult += ( szAppend);
                goto Error;//minimize return points
            }

            //now replace the decimal with a degree sign
            szResult = szResult.Replace(".", "°");
            
            //now see if we have enough decimal values
            if(iLen - iLocation < 3)
            {   //we don't append and return
                szAppend = szAppend.Remove(0, iLen - iLocation);
                
                szResult += (szAppend);
                goto Error;
            }

            //now insert the minute symbol
            szResult = szResult.Insert(iLocation + 3, "\x2032");
            
            //and check if have enough decimal values
            if(iLen - iLocation < 5)
            {   //we don't append and return
                szAppend = szAppend.Remove(0, iLen - iLocation + 1);
                szResult += ( szAppend);
                goto Error;
            }

            //insert a decimal if we need one
            if(iLen - iLocation > 5)
                szResult = szResult.Insert(iLocation + 6, ".");

            //and the seconds symbol
          //  szResult += ( ("\x2033"));

        Error:
            return true;
        }

        bool FormatNumberToSci(string szNumber, ref string szResult)
        {
            string szExp = "";


	        //check if its all zeros first...we'll just leave it then
	        if(IsAllZeros(szNumber))
	        {
		        szResult = szNumber;
		        return true;
	        }

            if(szNumber.IndexOf("°") != -1)
                return FormatNumberToFloat(szNumber, ref szResult, iSigFigs);

	        double dAnswer;

	        int	iPeriod;
	        bool	bNegative=false;

            dAnswer = StringToDouble(szNumber);

            szExp = "";
	        
	        szResult = szNumber;
	
	        if(szResult.IndexOf("e")!=-1)
	        {
		        //its already in scientific notation so no change nessesary
		        return true;
	        }

	        //handle the negative
	        if(szResult.IndexOf("-")==0)
	        {
		        bNegative=true;
                szResult = szResult.Remove(0, 1);
	        }

	        //there's always a period from the conversion
	        iPeriod=szResult.IndexOf(".");
	        //remove the period
            szResult = szResult.Remove(iPeriod, 1);

	        if(szResult.Length==1)
	        {//gotta add the minus before you head back out - STUPID
                if (bNegative)
                    szResult = "-" + szResult;
		        return true;
	        }
	        else if(dAnswer>1 ||
		        dAnswer< -1)
	        {
		        //positive exponent
                int iTemp = iPeriod - 1;
                szExp = iTemp.ToString();

		        while(szExp.Length<3)
		        {
			        szExp = "0" + szExp;
		        }
                szExp = "e+" + szExp;
                szResult = szResult.Insert(1, ".");
	        }
	        else
	        {
		        //negative exponent
		        int iExp=0;
		        while(szResult[0] == '0')
		        {
			        iExp++;
	                szResult = szResult.Remove(0, 1);
		        }
		        //check for small exponents
                if ((dAnswer < 10.0 && dAnswer >= 1.0) ||
                    (dAnswer > -10.0 && dAnswer <= -1.0))
                {
                    szExp = ("0");
                }
                //otherwise, count the decimal places
                else
                {
                    szExp = iExp.ToString();
                }
		        while(szExp.Length<3)
		        {
			        szExp = "0" + szExp;
		        }

                szExp = "e-" + szExp;
                szResult = szResult.Insert(1, ".");
	        }
	        szResult += szExp;
            if (bNegative)
                szResult = "-" + szResult;
	        return true;
        }

        public bool SetDisplayType(DisplayType eDisp)
        {
            if(eDisp < DisplayType.DISPLAY_Count && (int)eDisp > -1)
	            eDisplay=eDisp;	
	        return true;
        }

        public bool FormatRegional(string szNumber, string szResult)
        {
	     /*   GetNumberFormat(LOCALE_USER_DEFAULT,
					        0,
					        szNumber,
					        null,
					        szResult,
					        STRING_NORMAL);
	        return true;*/

            return false;
        }

        public bool CurrencyFormat(string szNumber,  ref string szCurrencySymbol, 
									 ref string szResultFull /* = null */, 
									 ref string szResultValue /* = null */, 
									 ref string szResultExp /* = null */, 
									bool bUseRegional/* =false */,
									int iSigFigs)
        {
            return false;

            /* string szTemp2;    
        	
             //copy the given symbol into the currency structure
             m_oStr->StringCopy(lpf.lpCurrencySymbol, szCurrencySymbol);
             if(bUseRegional)
             {
                 GetCurrencyFormat(LOCALE_USER_DEFAULT,
                                             0,
                                             szNumber,
                                             &lpf,
                                             szTemp2,
                                             STR_LARGE);

                         //if we're not over length or we don't care...fill and return
                 if(((m_oStr->GetLength(szTemp2)<= iSigFigs+2 && iSigFigs != 0) ||
                     iSigFigs==0) &&
                     m_oStr->GetLength(szTemp2)!=0)
                 {
                     if(szResultFull!=null)
                         m_oStr->StringCopy(szResultFull, szTemp2);
                     if(szResultValue!=null)
                         m_oStr->StringCopy(szResultValue, szTemp2);
                     if(szResultExp!=null)
                         szResultExp = "";
                     return true;
                 }
                 else
                 {
                     //we're either not using regional settings or we're too long
                     m_oStr->StringCopy(szTemp2, szNumber);
                     FormatNumber(szTemp2, szTemp2, DisplayType.DISPLAY_Scientific, iSigFigs);
                 }
             }
             else
             {
                 m_oStr->StringCopy(szTemp2, szNumber);
                 int iTemp = iFixedDigits;
                 iFixedDigits = 2;
                 FormatNumber(szTemp2, szTemp2, DisplayType.DISPLAY_Fixed,	iSigFigs-m_oStr->GetLength(lpf.lpCurrencySymbol));
                 iFixedDigits = iTemp;
                 CleanUpNumber(szTemp2, szTemp2);
             }


             if(szResultValue!=null)
             {
                 GetValue(szTemp2, szResultValue, iSigFigs-m_oStr->GetLength(lpf.lpCurrencySymbol));
                 m_oStr->Insert(szResultValue, szCurrencySymbol, 0);
             }
             if(szResultExp!=null)
             {
                 GetExponent(szTemp2, szResultExp);
             }
             if(szResultFull!=null)
             {
                 m_oStr->StringCopy(szResultFull, szResultValue);
                 if(m_oStr->GetLength(szResultExp)>0)
                 {
                     m_oStr->Concatenate(szResultFull, ("e"));
                     if(m_oStr->Find(szResultExp, ("-"), 0)==-1)
                         m_oStr->Concatenate(szResultFull, ("+"));
                     m_oStr->Concatenate(szResultFull, szResultExp);
                 }
             }

             return true;*/
        }

        string CurrencyFormatBasic(string szNumber)
        {
            if (szNumber.IndexOf("E") != -1 ||
                szNumber.IndexOf("e") != -1)
                return szNumber;

	        int iDec = szNumber.IndexOf(".");
	        int iLen = szNumber.Length;

            if (iDec == -1)
                return szNumber + ".00";
            else if (iDec == iLen - 1)
                 return szNumber + "00";
            else if (iDec == iLen - 2)
                return szNumber + "0";
            else if (iDec == iLen - 3)// (iLen - iDec < 4)//all reeady perfect
                return szNumber;
            else //remove some excess decimal places
                return szNumber.Remove(iDec + 3, iLen - iDec - 3);
        }

        public bool CurrencyFormat(string szNumber, 
							ref string szResultFull /* = null */, 
							ref string szResultValue /* = null */, 
							ref string szResultExp /* = null */, 
							ref bool bUseRegional/* =false */,
							int iSigFigs)
        {
	        //return CurrencyFormat(szNumber, lpf.lpCurrencySymbol, szResultFull, szResultValue,  szResultExp, bUseRegional, iSigFigs);
            return false;
        }

        bool InitializeCurrenyFormatting()
        {
            return false;
	       /* string szTemp;

	        //GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_RETURN_NUMBER|LOCALE_IDIGITS, myTChar, STRING_MAX);


	        // First fill in the CURRENCYFMT structure with user locale-specific
	        //    information.

	        //this one returned 9 to me instead of 2....so we're just gonna hard code it...
	        //too bad for those who don't like decimal places
	        //GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IDIGITS,
	        //	szTemp, STRING_MAX);


	        lpf.NumDigits = 2;

	        lpf.LeadingZero = _ttoi(szTemp);
 
	        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILZERO,	
		        szTemp, STRING_NORMAL);

	        lpf.LeadingZero = _ttoi(szTemp);
   
	        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING,
		        szTemp, STRING_NORMAL);

	        lpf.Grouping = _ttoi(szTemp);

	        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SCURRENCY,	
		        lpf.lpCurrencySymbol, STRING_NORMAL);
   
	        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL,  
		        lpf.lpDecimalSep, STRING_NORMAL);

   
	        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND,
		        lpf.lpThousandSep, STRING_NORMAL);

	        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_INEGCURR,
		        szTemp, STRING_MAX);

	        lpf.NegativeOrder = _ttoi(szTemp);

	        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ICURRENCY, 
	           szTemp, STRING_NORMAL);

	        lpf.PositiveOrder = _ttoi(szTemp);

	        return true;*/
        }


        public bool GetExponent(string szNumber, ref string szExp)
        {
            if(szNumber == null || szExp == null)
                return false;

	        if(szNumber.IndexOf("e") !=-1)
	        {
                int iRemoveLen = szNumber.IndexOf("+");
                bool bNeg = false;

                if (iRemoveLen == -1)
                {
                    iRemoveLen = szNumber.IndexOf("-", 1);//incase there's a leading -
                    if (iRemoveLen == -1)
                        return false;//should never happen
                    bNeg = true;
                }
                iRemoveLen++;
                
                szExp = szNumber.Substring(iRemoveLen);

                while (szExp.Length < 3)
                    szExp = "0" + szExp;
                if (bNeg)
                    szExp = "-" + szExp;

	        }
	        else
                szExp = "";

	        return true;
        }


        public bool GetValue(string szNumber, ref string szValue, int iSigFigs)
        {
            if(szNumber == null || szValue == null)
                return false;

	        int	iLength = 0;
	        iLength = szNumber.Length;

	        if(szNumber.IndexOf("e")!=-1)
            {//5 for the exponent....3 for the reduction in length due to the exponent display
                int iRemoveLen = szNumber.IndexOf("+");
                if(iRemoveLen == -1)
                    iRemoveLen = szNumber.IndexOf("-", 1);
                if (iRemoveLen == -1)
                    return false;//should never happen

                iRemoveLen--;

                if(iRemoveLen > 0)
                    szValue = szNumber.Substring(0, iRemoveLen);
		        
		        //check length
	        if(szValue.Length>iSigFigs-VALUE_LENGTH_OFFSET && iSigFigs!=0)
                szValue = szValue.Substring(0, iSigFigs-VALUE_LENGTH_OFFSET);
	        }
	        else 
	        {
		        szValue =szNumber;
		        //check length
		        if(szValue.Length>iSigFigs && iSigFigs!=0)
                    szValue = szValue.Substring(0, iSigFigs);
			        
                //remove excess 0s
                //not this function's job ....
            /*    int iLen = szValue.Length;
                if(szValue.IndexOf(".")!= -1)
                    while(szValue[iLen - 1] == ('0'))
                    {
                        m_oStr->Delete(iLen-1, 1, szValue);
                        iLen--;
                    }
        */
	        }
	
	        return true;
        }



        //returns true if we find a calc class error code
        bool CheckStringForErrors(string szNumber, ref string szResult, ref string szValue, ref string szExp)
        {
            return false;

            /*   if(m_oStr->Compare(ERROR_1, szNumber)==0 ||
                   m_oStr->Compare(ERROR_2, szNumber)==0 ||
                   m_oStr->Compare(ERROR_3, szNumber)==0)
               {
                   if(szExp!=null)
                       szExp = "";

                   string szFormattedError;

                   if(0 == m_oStr->Compare(ERROR_1, szNumber))
                       m_oStr->StringCopy(szFormattedError, ("Syntax Error"));
                   else if(0 == m_oStr->Compare(ERROR_2, szNumber))
                       m_oStr->StringCopy(szFormattedError, ("Infinity"));
                   else
                       m_oStr->StringCopy(szFormattedError, ("Imaginary"));

                   if(szValue!=null)
                       m_oStr->StringCopy(szValue, szFormattedError);
                   if(szResult!=null)
                   m_oStr->StringCopy(szResult, szFormattedError);
                   return true;
               }
               else 
                   return false;*/
        }


        public bool NumberFormatNonBase10(string szNumber, ref string szResultFull, ref string szResultValue, ref string szResultExp, int iSigFigs)
        {
	        int iLen = 0;

	        if(szNumber==null)
		        return false;


            szResultFull = "";
            szResultExp = "";
            szResultValue = "";

	        //check length
	        iLen = szNumber.Length;

	        if(iLen > iSigFigs &&
		        iSigFigs != 0)
	        {
		        szResultValue = "Display too large";
		        szResultFull = "Display too large";
	        }
	        else
        //	if(CheckStringForErrors(szNumber)==false)
	        {
		        //found an error code
		        //or normal
		        szResultValue = szNumber;
		        szResultFull = szNumber;
		
	        }
	        return true;
        }


        public bool NumberNoFormat(string szNumber, ref string szResultFull , ref string szResultValue , ref string szResultExp)
        {
            string szTemp = "";

	        if(szNumber==null)
		        return false;

	        CleanUpNumber(szNumber, ref szTemp);
	        szTemp = szNumber;

            if(szResultFull != null)
            {
                szResultFull = "";
                szResultFull = szTemp;
            }

            if(szResultValue != null)
            {
                szResultValue = "";
                GetValue(szTemp, ref szResultValue, iSigFigs);
            }

            if(szResultExp != null)
            {
                szResultExp = "";
	            GetExponent(szTemp, ref szResultExp);
            }
	

	        return true;
        }


        bool FormatNumberRegional(string szNumber, ref string szResult, int iSigFigs)
        {
	       /* string szTemp;
	        
            GetNumberFormat(LOCALE_USER_DEFAULT,
								        0,
								        szResult,
								        null,
								        szTemp,
								        STRING_LARGE);
	
	        if(m_oStr->GetLength(szTemp)>iSigFigs && iSigFigs!=0)
		        return false;
	        m_oStr->StringCopy(szResult, szTemp);
	        return true;*/

            return false;
        }

        bool IsAllZeros(string szIn)
        {
	        if(szIn == null)
		        return false;

	        for(int i = 0; i < szIn.Length; i++)
	        {
		        if(szIn[i] != ('0') &&
			        szIn[i] != ('.'))
			        return false;
	        }
	        return true;
        }

        bool PercentageFormat(ref string szNumber)
        {
	        if(szNumber==null)
		        return true;
	        if(szNumber.Length<1)
		        return false;
            if (szNumber[szNumber.Length - 1] == ('.'))
                szNumber = szNumber.Remove(szNumber.Length - 1, 1);
	        szNumber += ("%");
	        return true;


        }

        public double StringToDouble(string szNumber)
        {
            double dbRet = 0;

            if (Double.TryParse(szNumber, out dbRet))
                return dbRet;
                          
            //ok now start looking for problems ...
            if (szNumber.IndexOf('°') != -1)
            {   //I can't believe I got away with this in the previous version .....
                szNumber = szNumber.Remove(szNumber.IndexOf('°'), 1);

                if (szNumber.IndexOf("\x2032") != -1)
                {
                    szNumber = szNumber.Remove(szNumber.IndexOf("\x2032"), 1);
                }

                return Convert.ToDouble(szNumber);
            }
          
            
            //is there an exponent?
            if (szNumber.IndexOf("e") != -1)
            {
                //probably some rogue 0s in the exponent
                string szExp = szNumber.Substring(szNumber.Length - 3);

                while (szExp[0] == '0')
                    szExp = szExp.Substring(1);

                szNumber = szNumber.Substring(0, szNumber.Length - 3) + szExp;

                return Convert.ToDouble(szNumber);
            }

            // ... need to change to a tryparse eventually ... but for now I wanna catch em
            return 0;
        }

        public bool IsANumber(string szNumber)
        {
            if (szNumber == null || szNumber.Length < 1 || szNumber[0] == '-')
                return true;//represents 0 .. all good

            if (szNumber[0] < '0' || szNumber[0] > '9')
            {
                if (szNumber[0] < 'A' || szNumber[0] > 'F')
                    return false;
            }
            return true;
        }



    }

}
