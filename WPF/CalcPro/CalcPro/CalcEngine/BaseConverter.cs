using System;

public class BaseConverter
{
    public BaseConverter()
	{
        iBitAccuracy = 32;
	}

    public const int MAX_BASE = 36;
    public const int MIN_BASE = 2;

    public const int MAX_BITS = 64;
    public const int MIN_BITS = 4;

    public int iBitAccuracy
    {
        get { return _iBitAccuracy; }
        set { _iBitAccuracy = value; }
    }
    int _iBitAccuracy;

    
    public bool ConvertStringToDecimal(int iFrom, string szEquation, ref string szResult)
    {
       	int iAmountLength	= 0;
	    int iNumStart;								//location of the start of the number in the string
	    int iNumLength;								//length of temp number
	
	    bool bNegative			= false;
	    bool bNumStart			= false;						//just a check to see if a number has started

	    string szTemp;
	    string szParse;
	    string szTempNumber;
        char szAt;//single should be fine

        szTemp = szEquation;

        if(szTemp[0] == '-')
	    {
		    bNegative=true;
            szTemp.Remove(0, 1);
	    }

        iAmountLength = szTemp.Length;

        szTempNumber = "";
        szAt = ' ';
        szParse = "";


        for (int k = 0; k < iAmountLength; k++)
        {
            //get each character in order
            szAt = (char)szTemp[k];


            if ((szAt > 47 && szAt < 58) ||//numbers
                (szAt > 64 && szAt < 91))//letters for base 11 - 36
            {
                if (bNumStart == false)
                {
                    bNumStart = true;
                    iNumStart = k;
                }
                szTempNumber = szTempNumber + szAt;
            }
            else if(szTempNumber.Length > 0)//we have a number to convert
		    {
			    iNumLength = szTempNumber.Length;
			    //Convert Hex to Decimal
			    ConvertToDecimal(iFrom, szTempNumber, ref szTempNumber);
                
			    szParse = szParse + szTempNumber + szAt;
                szTempNumber = "";
			    		    
			    bNumStart=false;
		    }
		    else
            {
			    szParse = szParse + szAt;
            }
	    }
	    //only really applies to the first number because its an operator otherwise
	    if(bNegative)
	    {
		    bNegative=false;
		    szParse = "-" + szParse;
	    }
	    if(szTempNumber.Length > 0)
	    {
            iNumLength = szTempNumber.Length;
		    ConvertToDecimal(iFrom, szTempNumber, ref szTempNumber);
		    
            szParse = szParse + szTempNumber;
            szTempNumber = "";
		    bNumStart=false;
	    }
	    
        szResult = szParse;
	    return true;

    }

    bool ConvertToDecimal(int iFrom, string szSource, ref string szResult)
    {
        char szAt;

	    long	i64Decimal	= 0;
	    long	i64Temp	= 0;
	    int     iLength	= 0;

	    
	    iLength = szSource.Length;

	    for(int i=0; i<iLength; i++)
	    {
		    szAt = szSource[i];
		    
		    i64Temp = CHARtoINT(szAt);
		    if(i64Temp==-1)
			    return false;
		    i64Temp = i64Temp * (Int64)Math.Pow((double)iFrom, (double)(iLength - 1 - i));
		    i64Decimal += i64Temp;
	    }
	
	    return I64toSTRING(i64Decimal, ref szResult);
    }


    public bool ConvertFromDecimal(int iTo, double dbNum, ref string szResult)
    {
	    string szTemp;
	    Int64 i64Decimal;
	    Int64 i64Max;
	    int   iTemp	= 0;
	    int   iCnt = 0;

        szResult = "";
        szTemp = "";

	    
	    //Too big
	    if(dbNum > Math.Pow(2.0,60.0))
		    dbNum=0;

	    i64Decimal = (Int64)dbNum;

     //   i64Max = (Int64)Math.Pow(2.0, (double)iBitAccuracy);
        double dbTemp = Math.Pow(2.0, (double)iBitAccuracy);

        i64Max = Convert.ToInt64(dbTemp);

	    //reduce the double until its in range
	    while(i64Decimal > (i64Max-1))
	    {
		    i64Decimal -= i64Max;
		    iCnt++;
		    if(iCnt> 10000)
		    {
			    i64Decimal=0;
			    break;
		    }
	    }

	    //increase it till its in range
	    while(i64Decimal < 0)
	    {
		    i64Decimal+=i64Max;
		    iCnt++;
		    if(iCnt> 10000)
		    {
			    i64Decimal=0;
			    break;
		    }
	    }

	    while(1==1)

	    {
		    iTemp = (int)(i64Decimal%iTo);
		    INTtoSTRING(iTemp, ref szTemp);
            if (iTemp != -1)
            {
                szResult = szTemp + szResult;
            }
		
		    if((Int64)iTemp == i64Decimal)
				    break;
		    else
		    {
                i64Decimal -= (Int64)(iTemp);
                i64Decimal /= (Int64)iTo;
		    }
	    }
	    return true;
    }


    bool I64toSTRING(Int64 i64, ref string szResult)
    {
	    szResult = i64.ToString();
	    return true;
    }

    bool INTtoSTRING(int iNum, ref string szResult)
    {
	    if(iNum < 10)
	    {
		    
            szResult = iNum.ToString();
            return true;
	    }
	    else
	    {
		    char szChar;  
            szChar = (char)(iNum+55);
            szResult = "" + szChar;
            return true;
	    }
    }

    /// <summary>
    /// single character only ....
    /// </summary>
    /// <param name="szNumber"></param>
    /// <returns></returns>
    int STRINGtoINT(ref string szNumber)
    {
	    if (szNumber.Length != 1)
            return -1;

        char szChar = szNumber[0];

        return CHARtoINT(szChar); 
    }

    int CHARtoINT(char szNumber)
    {
        if (szNumber > 47 &&
            szNumber < 58)
            return (int)(szNumber - 48);

        else if (szNumber > 64 &&
                szNumber < 91)
            return (int)(szNumber - 65 + 10);

        else
            return -1;
    }


    public bool ConvertBCDToDecimal(ref string szEquation, ref string szResult)
    {
        //this should be easy enough ...
        //we'll skip format checking 
        //we'll code to jump over spaces
        //we'll assume blocks of 4
        int iAmountLength	= 0;
        int iNumStart;								//location of the start of the number in the string
        int iNumLength;								//length of temp number

        string szTemp;
        string szParse;
        string szTempNumber;
        char szAt;
        string szPortion; //per BCD

        bool bNumStart			= false;

        szTemp = szEquation;

        iAmountLength   = szTemp.Length;
        szTempNumber = "";
        szParse = "";
        

        for(int k = 0; k < iAmountLength; k++)
        {
            //get each character in order
            szAt = szTemp[k];

            if(szAt == (' '))
            {
                continue;//skip spaces
            }
            else if(szAt == ('0') || szAt == ('1')) //only two values
            {
                if(bNumStart==false)
                {
                    bNumStart=true;
                    iNumStart=k;
                }

                szTempNumber = szTempNumber + szAt;

            }
            else if(szTempNumber.Length > 0)//we have a number to convert
            {
                iNumLength = szTempNumber.Length;

                //in case it doesn't have leading 0s ...
                int iSkip = iNumLength%4;
                if(iSkip == 0)
                    iSkip = 4;
            
                for(int p = 0; p < iNumLength; p += iSkip)
                {
                    char[] charTemp = new char[256];

                    szTempNumber.CopyTo(p, charTemp, 0, iSkip);
                    szPortion = new String(charTemp);
                    
                    ConvertToDecimal(2, szPortion, ref szPortion);//to a single character
                    
                    szParse = szParse + szPortion;
                    szPortion = "";
                    iSkip = 4;
                }
                szTempNumber = "";
                szParse = szParse + szAt;
                 
                bNumStart=false;
                
            }
            else
            {   //whatever it is can go into our equations
                szParse = szParse + szAt;
            }
        }

        if (szTempNumber.Length > 0)
        {
            iNumLength = szTempNumber.Length;

            //in case it doesn't have leading 0s ...
            int iSkip = iNumLength%4;
            if(iSkip == 0)
                iSkip = 4;

            for(int p = 0; p < iNumLength; p += iSkip)
            {
                char[] charTemp = new char[256];

                szTempNumber.CopyTo(p, charTemp, 0, iSkip);
                //m_oStr->StringCopy(szPortion, szTempNumber, p, iSkip);
                szPortion = new String(charTemp);
                
                ConvertToDecimal(2, szPortion, ref szPortion);//to a single character
                
                szParse = szParse + szPortion;
                szPortion = "";
                iSkip = 4;
            }

            szTempNumber = "";
            bNumStart=false;
            
        }
        szResult = szParse;
        return true;
    }


    
    public bool ConvertDecimalToBCD(double dbNum, ref string szResult)
    {
        if(dbNum < 0 || dbNum > 999999)
            return false;


        
        string szTemp;
        int iValue = 0;

        szTemp = "";
        szResult = "";
        

        //no more decimal places ... not my problem
        iValue = (int)dbNum;

        szTemp = iValue.ToString();

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

        int iLen = szTemp.Length;

        bool bSpaces = true; 

        //now lets convert our number to BCD ... easy enough
        for(int i = 0; i < iLen; i++)
        {
            iValue = szTemp[i] - '0';

            if(iValue < 0 || iValue > 9)
                continue;

            if (i != 0 && bSpaces == true)
                szResult = szResult + " ";

            szResult = szResult + szBCD[iValue];
            
        }

        return true;
    }


}
