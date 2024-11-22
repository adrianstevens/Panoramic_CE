using System;
using System.Text;

public class CalcEngine
{
  /*  struct Angle
    {
        public Angle(int hours)
        {
            _Hours = hours;
        }   
    }*/
    

    public CalcEngine()
	{
        eDRGState = DRGStateType.DRG_Degrees;
	    
	    iSigFigs		= MAX_SIG_FIGS;
	    iBase			= 10;

	    bUseTape		= false;
        bDMS          = false;

        bBCDPrev      = false;
        bBCD          = false;

	    //oTape.Init();

	    //Seed the randomness with the seed of randomness
        //don't think I need to do this ...
	    //   SYSTEMTIME time;
	    //   GetLocalTime(&time);
	    //   srand(time.wSecond);
	}

    enum TokenType
    {
        Equals = 1,
        OpenBracket = 2,
        Plus,
        Minus,
        Times,
        Divide,
        Exponent,
        E,
        nthRoot,
        nCr,
        nPr,
        CloseBracket = 12,  //same as TOKEN
    }

    int NFUN = functions_struct.Length;//can't make it static ... don't care

    public const int MAX_SIG_FIGS = 15;
    public const int MAX_SIG_FIGS_NON_10 = 50;
    const int STACK_SIZE = 127;
    const int TOKEN = 25;
    const int TOKEN_OFFSET = 4;
    const int TOKEN_MAX = 75;
    const double DBL_MAX = double.MaxValue;

    //  functions_struct struct - stores the string names for the functions - change to taste
    //  we could probably reference this from global.h
    static string[] functions_struct = 
    {
	    "abs", "acos", "asin", "atanh", "atan", "cosh", "cos", "exp",
	    "log2", "log", "ln", "sinh", "sin", "sqrt", "tanh", "tan", "pct", 
	    "!", "asnh", "acsh", "atnh", "sum", "dc" /*dec to deg*/, "dms" /*deg to dec*/,
	    //NEW Functions to extend for other calc modes
	    "rand", "tenx", "crt", "cbd", "sqrd", "inv", "pm", "ex", 
    };

    BaseConverter oBaseConv = new BaseConverter();

    DRGStateType        eDRGState;

    double[]            dbReg = new double[STACK_SIZE];

    string				szInputString;		//this class was originally designed for the desktop
    int                 iStringIndex;   //	char				*szStringIndex;

														//so it works in chars not TCHARs
    int[]               iToken = new int[STACK_SIZE+1];			//stack that holds numerical references to the operators and functions
    int[]               iStack = new int[STACK_SIZE];

	int				    iTop;							//count to the top of the db stack
	int				    iTokenIndex;					//index to the Token Stack iToken[]
	int				    iRegIndex;					//index to the double stack db_Reg[];
	int				    iLen;							//string length

    public int iSigFigs
    {
        get { return iSigFigs; }
        set { if(value > 0 && value < MAX_SIG_FIGS) iSigFigs = value; }
    }



//	int				    iError;						//error index, not really used much yet
	int				    iBase;						//Current Number Base		

	bool				bUseTape;
    bool                bDMS;                         //if any inputs are in DMS notation
    bool                bBCD;
    bool                bBCDPrev;



    public int GetSigFigs() { return iSigFigs; }

    public int GetBaseBits() { return oBaseConv.iBitAccuracy; }

    public int GetBase() { return iBase; }

    public bool GetUseTape() { return bUseTape; }


    public bool GetShowDMS() { return bDMS; }
    public void SetShowDMS(bool bNewDMS) { bDMS = bNewDMS; }

    public bool GetShowBCD() { return bBCD; }
    public void SetShowBCD(bool bNewBCD) { bBCD = bNewBCD; }



    public double Calculate(string szEquation, ref string szAnswer)
    {
        return Calculate(szEquation, ref szAnswer, iBase, iBase);
    }

    public double Calculate(string szEquation, ref string szAnswer, int iBaseIn, int iBaseOut)
    {
        string szTempEq;
        bDMS = false;

        ClearArrays();

        szTempEq = szEquation;

        //save the expression to the tape  BUGBUG
        if (bUseTape == true && szTempEq.Length > 0)
        {
        //   oTape.AddEntry(szTempEq);
        }

        szTempEq = szTempEq.Replace("÷", "/");
        szTempEq = szTempEq.Replace("×", "*");
        szTempEq = szTempEq.Replace("²", "^(2)");
        szTempEq = szTempEq.Replace("³", "^(3)");
        szTempEq = szTempEq.Replace("-¹", "^(-1)");

        //search and swap out DMS values
        ConvertFromDMS(ref szTempEq);

        if (iBaseIn != 10)
        {
            oBaseConv.ConvertStringToDecimal(iBaseIn, szTempEq, ref szTempEq);
            iLen = szTempEq.Length ;  //gets the length of temp string
            szInputString = szTempEq;
        }
        else if (bBCDPrev)
        {   //looks about right 
            oBaseConv.ConvertBCDToDecimal(ref szTempEq, ref szTempEq);
            iLen = szTempEq.Length;  //gets the length of temp string
            szInputString = szTempEq;
        }
        else
        {
            iLen = szTempEq.Length;  //gets the length of temp string
            szInputString = szTempEq;
        }

        iStringIndex = 0;
        iTokenIndex = iRegIndex = 0; //reset our stack indexes
        //puts the expressions into the array
        Expression();

        
        
        PutToken(1);//basically adds an equals to the end of the token stack for completeness
        InfixPostfix();

        Evaluate();	 //now that the operators, numbers and functions are organized, evaluate

        if (bBCD)
        {
            if (false == oBaseConv.ConvertDecimalToBCD(dbReg[0], ref szAnswer))
            {   //we'll change modes 
                bBCD = false;
                szAnswer = dbReg[0].ToString();
            }
        }
        else if (iBaseOut == 10)
        {
            szAnswer = dbReg[0].ToString();
            szAnswer = szAnswer.Replace("E", "e");
            
      //      DblToString(ref szAnswer, dbReg[0], iSigFigs);
        }
        else
            oBaseConv.ConvertFromDecimal(iBaseOut, dbReg[0], ref szAnswer);

        //comes up as NaN ... not worried about it
     /*   if (oStr->Compare(szAnswer, ("1.#INF")) == 0 ||
            oStr->Compare(szAnswer, ("-1.#INF")) == 0 ||
            oStr->Compare(szAnswer, ("-1.#QNAN")) == 0 ||
            oStr->Compare(szAnswer, ("1.#QNAN")) == 0)
            oStr->StringCopy(szAnswer, ("infinity"));
        else if (oStr->Compare(szAnswer, ("1.#SNAN")) == 0 ||
            oStr->Compare(szAnswer, ("-1.#SNAN")) == 0 ||
            oStr->Compare(szAnswer, ("--1.#IND")) == 0 ||
            oStr->Compare(szAnswer, ("-1.#IND")) == 0 ||
            oStr->Compare(szAnswer, ("1.#IND")) == 0)

            oStr->StringCopy(szAnswer, ("imaginary"));*/
        
        // ... v3.1?
       /* if (bUseTape && oStr->GetLength(szTempEq))
        {
            oTape.AddEquals();
            oTape.AddAnswer(szAnswer);
        }*/


        bBCDPrev = bBCD;

        return (dbReg[0]);
    }

    void Expression()
    {
        Term();

        if (iStringIndex >= szInputString.Length)
            return;

        switch(szInputString[iStringIndex]) 
	    {
		    case('+'):
			    PutToken(3);
			    Expression();
                break;
		    case('-'):
			    PutToken(4);
			    Expression();
                break;
        }
    }

    void Term()
    {
        Factor();

        if (iStringIndex >= szInputString.Length)
            return;

        switch (szInputString[iStringIndex]) 
	    {
		    case('*'):
		    case('×'):
			    PutToken(5);
			    Term();
			    break;
		    case('/'):
		    case('÷'):
			    PutToken(6);
			    Term();
			    break;
        }
    }

    void Factor()
    {
        Operand();

        if (iStringIndex >= szInputString.Length)
            return;

        if (IsSubstringInString(szInputString,"^",iStringIndex))
	    {
            PutToken(7);
            Factor();
        }
	    //attempted addon for E -> exponent function since the default 'e' won't do fractional exponents
        else if (IsSubstringInString(szInputString, "E", iStringIndex))
	    {
            //iStringIndex++;
		    PutToken(8);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "r", iStringIndex))
	    {
		    //adding roots
            //iStringIndex++;
		    PutToken(9);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "C", iStringIndex))
	    {
            //iStringIndex++;
		    PutToken(10);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "P", iStringIndex))
	    {
            //iStringIndex++;
		    PutToken(11);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "X", iStringIndex))//r,theta to X
	    {
            //iStringIndex++;
		    PutToken(12);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "Y", iStringIndex))//r, theta to Y
	    {
            //iStringIndex++;
		    PutToken(13);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "R", iStringIndex))//x,y to R
	    {
            //iStringIndex++;
		    PutToken(14);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "<", iStringIndex))//x,y to Theta
	    {
            //iStringIndex++;
		    PutToken(15);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "&", iStringIndex))//AND
	    {
            //iStringIndex++;
		    PutToken(16);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "|", iStringIndex))//OR
	    {
            //iStringIndex++;
		    PutToken(17);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "x", iStringIndex))//XOR
	    {
            //iStringIndex++;
		    PutToken(18);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "n", iStringIndex))//NOR
	    {
            //iStringIndex++;
		    PutToken(19);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "%", iStringIndex))//MOD
	    {
            //iStringIndex++;
		    PutToken(20);
		    Factor();
	    }
        else if (IsSubstringInString(szInputString, "o", iStringIndex))//XNOR
        {
            //iStringIndex++;
            PutToken(21);
            Factor();
        }
        else if (IsSubstringInString(szInputString, "d", iStringIndex))//NAND
        {
            //iStringIndex++;
            PutToken(22);
            Factor();
        }
        else if (IsSubstringInString(szInputString, "T", iStringIndex))
        {
            //iStringIndex++;
            PutToken(23);
            Factor();
        }
        else if (IsSubstringInString(szInputString, "D", iStringIndex))
        {
            //iStringIndex++;
            PutToken(24);
            Factor();
        }
    }

    void Operand()
    {
        if (iStringIndex >= szInputString.Length)
            return;

        string c;
        int k;
	
        //c = *szStringIndex;

       // c = szInputString.Substring(iStringIndex, szInputString.Length - iStringIndex);
        c = szInputString.Substring(iStringIndex);

        if (c == null || c.Length == 0)
            return;

        if (c[0] =='-') 
	    {
            PutToken(TOKEN + 3);
            Operand();
        } 
	    else if (c[0] =='(') 
	    {
            PutToken(2);
            Expression();

            c = szInputString.Substring(iStringIndex);

            if (c.Length == 0)
                return;

            if (c[0] == ')')
                //  puttoken(10);
                PutToken(TOKEN);
            else
            {
                SyntaxError();
            }
        } 
	    //Add new "operators and functions here:
	    //ah yeah
        else if ((c[0] >= 'a' && c[0] <= 'z' || c[0] == '!' || c[0] == 'E') &&
                    (c[0] != 'r' || c[0] != 'C' || c[0] != 'P' || c[0] != 'x' || c[0] != 'n')) //the r and E are redundant...we're looking for words basically
	    {
            for (k = 0; k < NFUN; ++k)
            {
                if (IsSubstringInString(szInputString, functions_struct[k], iStringIndex)) //functions_struct is a list of all functions....looking for a match with string i
                {
                    PutToken(k + TOKEN + 4);//I increased this to 14 when adding r, "E"..we'll see what happens
                    //ok...we loop through the functions_struct looking for a match...then we put the token
                    //of the match which corresponds to our function...gotta rename these

                    //recalc c
                    c = szInputString.Substring(iStringIndex, szInputString.Length - iStringIndex);

                    if (c[0] == '(')
                        Operand();
                    else
                    {
                        SyntaxError();
                    }
                    return;
                }
            }

            string szTemp;

            //doing the ol constants by hand since there's only two
            if(IsSubstringInString(szInputString, "pi", iStringIndex))
            {
                szTemp = "3.141592653589793238";

                PutNumberInStack(ref szTemp, szTemp.Length);
				iStringIndex += 2;//length of pi
                return;
            }
            if(IsSubstringInString(szInputString, "e", iStringIndex))
            {
                szTemp = "2.718281828459045235";

                PutNumberInStack(ref szTemp, szTemp.Length);
				iStringIndex += 1;//length of e
                return;
            }
            
            SyntaxError();
        } 
	    else
        {
            UnsignedNumFound();//must be a number then
        }
    }	


    /********************************************************************
	Function:	UnsignedNumFound

	Comments:	This is called when a number is detected in the string,
				This function then finds the full number and calls
				PutNumberInStack

				This can probably be cleaned a bit more...
				...ie - remove the formatting char table
    *********************************************************************/
    void UnsignedNumFound()
    {
	    bool bNumber	= true;		//if we're here we assume we have a number
	    bool bDecimal	= false;	//we only get one decimal....chances of two are SLIM
	    bool bExp		= false;
	    bool bOperator	= false;
	    int iLength		= 0;		//length of the number to add to stack
	    char charTmp;

	    while(bNumber)
	    {
            if(iStringIndex + iLength >= szInputString.Length)
                break;

            charTmp = szInputString[iStringIndex + iLength];

		    //normal number 0-9
		    if(charTmp>='0' && charTmp<='9')
		    {
			    iLength++;
		    }
		    //the decimal point ... can only have 1
		    else if(charTmp == '.' && bDecimal == false)
		    {
			    iLength++;
			    bDecimal = true;
		    }
		    //we have to complain ... won't ever happen
		    else if(charTmp=='.')
		    {	//error...2nd decimal
			    bNumber = false;
			    SyntaxError();
		    }
		    //do we have an exponent?
		    else if(charTmp=='e')
		    {
			    if(bExp)
			    {
				    SyntaxError();
				    bNumber = false;
			    }
			    else
				    bExp = true;
			    iLength++;
		    } //set this to true if we find an operator ... only for exponents and only 1!!
		    else if(charTmp=='-' && bOperator == false || charTmp=='+' && bOperator == false)
		    {
			    bOperator = true;
			    if(bExp)// && bOperator == false)
				    iLength++;
			    else
				    bNumber=false;
		    }
		    else
		    {
			    bNumber = false;
		    }
	    }

        string szTemp = szInputString.Substring(iStringIndex, iLength);

        PutNumberInStack(ref szTemp, iLength);
	    iStringIndex += iLength;


    }


    /********************************************************************
	Function:	InfixPostFix

	Comments:	This function converts the stacks from Infix orders to 
				PostFix aka RPN

				This is called once all of the numbers, operators and 
				functions have been parsed out of the string
    *********************************************************************/
    void InfixPostfix()	
    {
        int iIndexP,iIndexQ;
	    int  iTempToken;
	
        iStack[0]		= 1;
        iTop			= 0;
        iTempToken		= iToken[0];
        iIndexP			= 1;
        iIndexQ			= 0;

        while (iTempToken!=1) 
	    {
		    if (iTempToken>TOKEN_MAX)//token is out of range
			    iToken[iIndexQ++] = iTempToken;
		    else if (iTempToken>TOKEN+1 || iTempToken==2)
			    iStack[++iTop] = iTempToken;
		    else if (iTempToken>2 && iTempToken<TOKEN) 
		    {
			    while (iTempToken<=iStack[iTop])
			    {
				    iToken[iIndexQ++] = iStack[iTop--];	
			    }
			    iStack[++iTop] = iTempToken;
		    } 
		    else 
		    {
			    while (iStack[iTop]!=2)
			    {
				    iToken[iIndexQ++] = iStack[iTop--];
			    }
			    --iTop;
		    }
		    iTempToken = iToken[iIndexP++];
        }
        while (iStack[iTop]!=1)
		    iToken[iIndexQ++] = iStack[iTop--];
        iToken[iIndexQ]	= 1;
    }



    /********************************************************************
	Function:	Evaluate

	Comments:	This actually does the calculations for the 
				operators and the functions based off the current
				Token iToken[0] and the numbers currently on the stack
				dbReg[]
    *********************************************************************/   
    void Evaluate()
    {
        int		iNumDecPlaces=0;
	    int		iTokenIndex;
	    int		iCurrentToken;

        double      dbOpp1,dbOpp2,dbOpp3;	//temp doubles for calculations

        iTop = -1;
        iTokenIndex = 0;
        iCurrentToken = iToken[0];

        while (iCurrentToken!=1) 
	    {
		    if (iCurrentToken>=TOKEN_MAX)
		        iStack[++iTop] = iCurrentToken-TOKEN_MAX;

		    else if (iCurrentToken<TOKEN+3) 
		    {
                if (iTop < 1)
                    break;

			    dbOpp2 = dbReg[iStack[iTop--]];
			    dbOpp1 = dbReg[iStack[iTop]];
			
			    switch(iCurrentToken) 
			    {
			    case(3):
				    dbOpp1 = dbOpp1+dbOpp2;				//add
				    break;
			    case(4):
				    iNumDecPlaces=GetDecimalPlaces(dbOpp1, dbOpp2);
				    dbOpp1 = dbOpp1-dbOpp2;				//subtract
				    dbOpp1 = RoundDouble(dbOpp1, iNumDecPlaces);
				    break;
			    case(5):
				    dbOpp1 = dbOpp1*dbOpp2;				//multiply
				    break;
			    case(6):
				    dbOpp1 = dbOpp1/dbOpp2;				//divide
				    break;
			    case(7):
				    {
					     dbOpp1 = Math.Pow(dbOpp1, dbOpp2);
					    //exponents - x to the y
				    }
				    break;
			    case(8):						//E - exp
				    dbOpp1=dbOpp1*(Math.Pow(10,dbOpp2));
				    break;
			    case(9):						//nth root
				    dbOpp1 = Math.Pow(dbOpp1, 1.0/dbOpp2);
				    break;
			    case(10):						//nCr
				    {
					    double iN=Math.Floor(dbOpp1);
                        double iR = Math.Floor(dbOpp2);
    /*
										    if(iN==iR)
										    {
											    dbOpp1=1;
											    break;
										    }//quick hack*/
					

					    double iNR=iN-iR;

					    int i;
					    for(i=(int)(iN-1);i>0;i--)
					    {
						    iN*=i;
					    }
					    for(i=(int)(iR-1);i>0;i--)
					    {
						    iR*=i;
					    }

                   	    if(iNR>0)
						    for(i=(int)(iNR-1);i>0;i--)
						    {
							    iNR*=i;
						    }
					    else
						    iNR=1;

                        //otherwise it returns imaginary
                        if(iR == 0)
                            dbOpp1 = 1;
                        else
					        dbOpp1=iN/(iR*iNR);
				    }
			    break;
			    case(11):					//nPr
				    {
					    double iN=Math.Floor(dbOpp1);
					    double iNR=iN-Math.Floor(dbOpp2);
					    int i;
					    for(i=(int)Math.Floor(iN-1);i>0;i--)
					    {
						    iN*=i;
					    }
					    if(iNR>0)
						    for(i=(int)Math.Floor(iNR-1);i>0;i--)
						    {
							    iNR*=i;
						    }
					    else
						    iNR=1;
					    dbOpp1=iN/iNR;
				    }
				    break;
			    case(12)://r,theta to X
                    if (eDRGState == DRGStateType.DRG_Degrees)
				    {
                        dbOpp2 = dbOpp2 * CalcGlobals.CONST_NUM_PI / 180;
				    }
				    else if(eDRGState==DRGStateType.DRG_Gradians)
				    {
                        dbOpp2 = dbOpp2 * CalcGlobals.CONST_NUM_PI / 200;
				    }
				    dbOpp1 = dbOpp1*Math.Cos(dbOpp2);
				    break;
			    case(13)://r, theta to Y
				    if(eDRGState==DRGStateType.DRG_Degrees)
				    {
                        dbOpp2 = dbOpp2 * CalcGlobals.CONST_NUM_PI / 180;
				    }
				    else if(eDRGState==DRGStateType.DRG_Gradians)
				    {
                        dbOpp2 = dbOpp2 * CalcGlobals.CONST_NUM_PI / 200;
				    }
				    dbOpp1 = dbOpp1*Math.Sin(dbOpp2);
				    break;
			    case(14)://x,y to R
				    dbOpp1 = Math.Pow(dbOpp1, 2) + Math.Pow(dbOpp2, 2);
				    dbOpp1 = Math.Pow(dbOpp1, 0.5);
				    break;
			    case(15)://x,y to Theta
				    {
					    double dbadj=0;
					    //adjustment factor
					    if(dbOpp1<0 && dbOpp2 <0)
                            dbadj = CalcGlobals.CONST_NUM_PI;
					    else if(dbOpp2 <0)
                            dbadj = 2 * CalcGlobals.CONST_NUM_PI;
					    else if(dbOpp1 <0)
                            dbadj = CalcGlobals.CONST_NUM_PI;

					    dbOpp1 = dbOpp2/dbOpp1;
					    dbOpp1 = Math.Atan(dbOpp1);
					    dbOpp1 += dbadj;
					
					    if(eDRGState==DRGStateType.DRG_Degrees)
					    {
                            dbOpp1 = dbOpp1 * 180 / CalcGlobals.CONST_NUM_PI;
					    }
					    else if(eDRGState==DRGStateType.DRG_Gradians)
					    {
						    dbOpp1=dbOpp1*200/CalcGlobals.CONST_NUM_PI;
					    }
				    }
				    break;
			    case(16)://AND &
				    {
				       dbOpp1=(double)((Int64)dbOpp1&(Int64)dbOpp2);
				    }
				    break;
			    case(17)://OR |
				    {
					    dbOpp1=(double)((Int64)dbOpp1|(Int64)dbOpp2);
				    }
				    break;
			    case(18)://XOR x
				    {
					    dbOpp1=(double)((Int64)dbOpp1^(Int64)dbOpp2);
				    }
				    break;
			    case(19)://NOR n
				    {
					    dbOpp1=(double)~((Int64)dbOpp1|(Int64)dbOpp2);
				    }
				    break;
			    case(20)://MOD %
				    {
                        dbOpp1 = dbOpp1 % dbOpp2;
				    }
				    break;
                case(21)://"o"))//XNOR
                    {
                        dbOpp1=(double)~((Int64)dbOpp1^(Int64)dbOpp2);
                    }
                    break;
                case(22)://"d"))//NAND
                    {
                        dbOpp1=(double)~((Int64)dbOpp1&(Int64)dbOpp2);
                    }
                    break;
                case(23)://%T
                    {
                        dbOpp1 = dbOpp2/dbOpp1;
                    }
                    break;
                case(24)://D%
                    {
                        dbOpp1 = (dbOpp2-dbOpp1)/dbOpp1;
                    }
                    break;
			    }
			    dbReg[iStack[iTop]] = dbOpp1;
		    } 
		    else
		    {
                if(iTop < 0)
                {
                    return;
                }

			    dbOpp1 = dbReg[iStack[iTop]];
			    switch(iCurrentToken) 
			    {
    			    case(TOKEN+3):
					    dbOpp1 = -dbOpp1;
					    break;
				    case(TOKEN+4):
					    if (dbOpp1<0)
						    dbOpp1 = -dbOpp1;
					    break;
				    case(TOKEN+5):
					    if(eDRGState==DRGStateType.DRG_Degrees)
                            dbOpp1 = Math.Acos(dbOpp1) * 180 / CalcGlobals.CONST_NUM_PI;
					    else if (eDRGState==DRGStateType.DRG_Radians)
                            dbOpp1 = Math.Acos(dbOpp1);
					    else if(eDRGState==DRGStateType.DRG_Gradians)
                            dbOpp1 = Math.Acos(dbOpp1) * 200 / CalcGlobals.CONST_NUM_PI;
					    if((dbOpp1 < 0.00000000001 && dbOpp1 > 0) || 
						    (dbOpp1 > -0.0000000001 && dbOpp1 < 0))
						    dbOpp1=0;

					    break;
				    case(TOKEN+6):
					    if(eDRGState==DRGStateType.DRG_Degrees)
                            dbOpp1 = Math.Asin(dbOpp1) * 180 / CalcGlobals.CONST_NUM_PI;
					    else if(eDRGState==DRGStateType.DRG_Radians)
                            dbOpp1 = Math.Asin(dbOpp1);
					    else
                            dbOpp1 = Math.Asin(dbOpp1) * 200 / CalcGlobals.CONST_NUM_PI;
					    if((dbOpp1 < 0.00000000001 && dbOpp1 > 0) || 
						    (dbOpp1 > -0.0000000001 && dbOpp1 < 0))
						    dbOpp1=0;
					    break;
				    case(TOKEN+7):
					    dbOpp1 = Math.Log((1+dbOpp1)/(1-dbOpp1))/2;
					    break;
				    case(TOKEN+8):
					    if(eDRGState==DRGStateType.DRG_Degrees)
                            dbOpp1 = Math.Atan(dbOpp1) * 180 / CalcGlobals.CONST_NUM_PI;
					    else if(eDRGState==DRGStateType.DRG_Radians)
                            dbOpp1 = Math.Atan(dbOpp1);
					    else
                            dbOpp1 = Math.Atan(dbOpp1) * 200 / CalcGlobals.CONST_NUM_PI;
					    if((dbOpp1 < 0.00000000001 && dbOpp1 > 0) || 
						    (dbOpp1 > -0.0000000001 && dbOpp1 < 0))
						    dbOpp1=0;
					    break;
				    case(TOKEN+9):
						    dbOpp1 = Math.Cosh(dbOpp1);
					    break;
				    case(TOKEN+10):
					    if(eDRGState==DRGStateType.DRG_Degrees)
                            dbOpp1 = Math.Cos(dbOpp1 * CalcGlobals.CONST_NUM_PI / 180);
					    else if(eDRGState==DRGStateType.DRG_Radians)
                            dbOpp1 = Math.Cos(dbOpp1);
					    else
                            dbOpp1 = Math.Cos(dbOpp1 * CalcGlobals.CONST_NUM_PI / 200);
					    if((dbOpp1 < 0.00000000001 && dbOpp1 > 0) || 
						    (dbOpp1 > -0.0000000001 && dbOpp1 < 0))
						    dbOpp1=0;
					    break;
				    case(TOKEN+11):
					    dbOpp1 = Math.Exp(dbOpp1);
					    break;
				    case(TOKEN+12):
					    dbOpp1 = Math.Log(dbOpp1)/Math.Log(2.0);
					    break;
				    case(TOKEN+13):
					    dbOpp1 = Math.Log10(dbOpp1);
					    break;
				    case(TOKEN+14):
					    dbOpp1 = Math.Log(dbOpp1);
					    break;
				    case(TOKEN+15):					//hyp sin - not dependant on Deg or Rad
						    dbOpp1 = Math.Sinh(dbOpp1);
					    break;
				    case(TOKEN+16):
					    if(eDRGState==DRGStateType.DRG_Degrees)
                            dbOpp1 = Math.Sin(dbOpp1 * CalcGlobals.CONST_NUM_PI / 180);
					    else if(eDRGState==DRGStateType.DRG_Radians)
						    dbOpp1 = Math.Sin(dbOpp1);
					    else
                            dbOpp1 = Math.Sin(dbOpp1 * CalcGlobals.CONST_NUM_PI / 200);
					    if((dbOpp1 < 0.00000000001 && dbOpp1 > 0) || 
						    (dbOpp1 > -0.0000000001 && dbOpp1 < 0))
						    dbOpp1 = 0;
					    break;
				    case(TOKEN+17):
					    dbOpp1 = Math.Sqrt(dbOpp1);
					    break;
				    case(TOKEN+18):
						    dbOpp1 = Math.Tanh(dbOpp1);
					    break;
				    case(TOKEN+19):
					    if(eDRGState==DRGStateType.DRG_Degrees)
                            dbOpp1 = Math.Tan(dbOpp1 * CalcGlobals.CONST_NUM_PI / 180);
					    else if(eDRGState==DRGStateType.DRG_Radians)
						    dbOpp1 = Math.Tan(dbOpp1);
					    else
                            dbOpp1 = Math.Tan(dbOpp1 * CalcGlobals.CONST_NUM_PI / 200);
					    if((dbOpp1 < 0.00000000001 && dbOpp1 > 0) || 
						    (dbOpp1 > -0.0000000001 && dbOpp1 < 0))
						    dbOpp1 = 0;
					    break;
				    case(TOKEN+20):					// Percentage
					    // to watch this ... keep an eye on the reg stack...shouldn't be too tough to fix if this is wrong
					    if(dbReg[1]==0)
						    dbOpp1=dbOpp1*0.01;
					    else
						    dbOpp1 = 0.01*dbOpp1*dbReg[0];
					    break;
				    case(TOKEN+21):
					    {
						    //Gamma Function approx
						    bool bNeg=false;
						    if(dbOpp1<0)
						    {
							    bNeg=true;
							    dbOpp1*=-1;
						    }
						    if(dbOpp1==0)
						    {
							    dbOpp1=1;
						    }
						    else if(dbOpp1==(Int64)dbOpp1 &&
							    dbOpp1>0)
						    {
							    dbOpp1=Math.Floor(dbOpp1);
							    for(Int64 iCounter = (Int64)dbOpp1-1; iCounter > 0; iCounter--)	//do some error checking later.....
							    {
								    dbOpp1=dbOpp1 * (iCounter);
								    if(dbOpp1 >= DBL_MAX)
								    {
									    break;
								    }

							    }
						    }
						    else if(dbOpp1>0)
						    {
					 		    //Interesting approximation of the gamma function, factorial results are off by 1
                                dbOpp1 = (Math.Pow(((dbOpp1 + 1) / CalcGlobals.CONST_NUM_e) * Math.Sqrt((dbOpp1 + 1) * Math.Sinh(1 / (dbOpp1 + 1)) + 1 / (810 * Math.Pow((dbOpp1 + 1), 6))), (dbOpp1 + 1))) * (Math.Sqrt(2 * CalcGlobals.CONST_NUM_PI / (dbOpp1 + 1)));
						    }
						    if(bNeg)
						    {
							    dbOpp1*=-1;
						    }
					    }
					    break;
				    case(TOKEN+22):	//Inv Hyp Sin
                        dbOpp3 = dbOpp1 + Math.Sqrt(1 + dbOpp1 * dbOpp1);
						    if(dbOpp3 < 0)
							    dbOpp3=(dbOpp3)*(-1);
						    dbOpp1 = Math.Log(dbOpp3);						 //ABSOLUTE VALUE!!!
					    break;
				    case(TOKEN+23):	//Inv Hyp Cos
					    if (dbOpp1 >= 1)
					    {
                            dbOpp3 = dbOpp1 + Math.Sqrt(dbOpp1 * dbOpp1 - 1);
						    if(dbOpp3 < 0)
							    dbOpp3=dbOpp3*(-1);
						    dbOpp1 = Math.Log(dbOpp3);						 //ABSOLUTE VALUE!!!
					    }
					    else
						    dbOpp1=0;
					    break;
				    case(TOKEN+24):   //Inv Hyp Tan anth
					    if(dbOpp1 == 1 || dbOpp1 > 1 || dbOpp1 == -1 || dbOpp1 < -1)
						    dbOpp1=0;
					    else
                            dbOpp1 = 0.5 * Math.Log((1 + dbOpp1) / (1 - dbOpp1));
					    break;
				    case(TOKEN+25)://Sum
					    {
						    dbOpp1 = Math.Floor(dbOpp1);
						    Int64 j=(Int64)dbOpp1;
						    if(j>0)
							    for (Int64 i=0; i<j; i++)
							    {
								    dbOpp1+=i;
							    }
						    else
							    for (Int64 i=0; i>j; i--)
							    {
								    dbOpp1+=i;
							    }
					    }
					    break;
				    case(TOKEN+27)://Dec to Deg
					    {
						    double dDeg, dMin, dSec;
						    dDeg = Math.Floor(dbOpp1);
						    dMin = 0.6*(dbOpp1 - dDeg);
						    dSec = (dMin * 100.0 - Math.Floor(dMin * 100.0))*60.0;
						    dMin = Math.Floor(dMin * 100.0);
						    dbOpp1 = dDeg + dMin/100.0 + dSec/10000.0;
					    }
					    break;
				    case (TOKEN+26):
					    {
						    double dDeg, dMin, dSec;
						    double dTemp = dbOpp1;
						    dTemp *= 100000000000000000000000000.0;
						    dTemp /= 1000000000000000000.0;
						    Int64 iTemp = (Int64)dTemp;

						    iTemp /= 1000000;
			
						    dDeg = Math.Floor(dbOpp1);
						    dSec = 100.0*dbOpp1-iTemp;
						    dSec = dSec/100.0;
						    dMin = dbOpp1 - dDeg - dSec;
						    dSec = dSec/0.6;
						    dMin = dMin + dSec;
						    dMin = dMin/0.6;
						    dbOpp1 = dDeg+dMin;	
					    }
					    break;
				    case (TOKEN+28)://RAND
					    {
                            Random rnd1 = new Random();
                            dbOpp1 = rnd1.NextDouble()*dbOpp1;
                        }
					    break;
				    case (TOKEN+29):// tenx -> ten to the X
					    {
						    dbOpp1 = Math.Pow(10, dbOpp1);
					    }
					    break;
				    case (TOKEN +30)://crt -> cube root
					    {
						    bool bNeg = false;
						    //quick hack for negative numbers....math.h sucks ass
						    if(dbOpp1 < 0)
						    {
							    dbOpp1 *=-1;
							    bNeg = true;
						    }
						    dbOpp1 = Math.Pow(dbOpp1, 1.0/3.0);
						    if(bNeg)
							    dbOpp1 *=-1;
					    }
					    break;
				    case (TOKEN + 31)://cbd -> cubed
					    {
						    dbOpp1 = Math.Pow(dbOpp1, 3.0);
					    }
					    break;
				    case (TOKEN + 32)://sqrd -> squares
					    {
						    dbOpp1 = Math.Pow(dbOpp1, 2.0);
					    }
					    break;
				    case (TOKEN + 33)://inv -> inverse
					    {
						    dbOpp1 = 1.0/dbOpp1;
					    }
					    break;
				    case (TOKEN + 34)://pm -> +/-
					    {
						    dbOpp1 = -1.0 * dbOpp1;
					    }
					    break;
				    case (TOKEN + 35): //ex ... e to the x
					    {
						    dbOpp1 = Math.Pow(CalcGlobals.CONST_NUM_e, dbOpp1);
					    }
                        break;
			    }
	        dbReg[iStack[iTop]] = dbOpp1;
		    }
            iTokenIndex++;

            iCurrentToken = iToken[iTokenIndex];
	    }
    }


    void PutToken(int iNewToken)
    {
        iToken[iTokenIndex++] = iNewToken;
        if (iNewToken == 7 || iNewToken == 8 /*|| iToken==(TOKEN+20)*/ || iNewToken == 9 || iNewToken == 10 || iNewToken == 11)				//t for exponents and percentage
            //TOKEN+20 is %  //7 is ^
            //8 is E    //9 is r (root)
            //10 is nCr	//11 is nPr

            iStringIndex += 1;							//string length of symbol for exponents  -  now 1 for   ^
        else if (iNewToken < (TOKEN + TOKEN_OFFSET))
            ++iStringIndex;
        else
            iStringIndex += functions_struct[iNewToken - TOKEN - TOKEN_OFFSET].Length;
    }

    /********************************************************************
	Function:	PutNumberInStack

	Arguments:	szEquation: pointer to input equation
				iLengthOfNumber: length of the number we're adding 

	Comments:	This Function puts a number into the stack db_Reg
    *********************************************************************/
    void PutNumberInStack(ref string szEquation,int iLengthOfNumber)
    {
        if (szEquation.Length == 0 || iLengthOfNumber == 0)
            return;

        char[] buf = new char[iLengthOfNumber + 1];
        int k;

        for (k = 0; k<iLengthOfNumber; k++)
		    buf[k] = szEquation[k];

        string szTemp = szEquation.Substring(0, iLengthOfNumber);

        dbReg[iRegIndex] = Convert.ToDouble(szTemp);

        iToken[iTokenIndex++]	= (iRegIndex++)+TOKEN_MAX;
    }

    void SyntaxError(ref string szAnswer)
    {
    /*		Save for better error analysis
            if (!iError) {
        //	printf("-----");
    	    while (i--!=szInputString)
    	        printf("-");
        //	printf("*\nUnexpected character.\n");
    	    iError = 1;
    	    i = "";
            }*/
        SyntaxError();
        szAnswer = "syntax error";
    }

    void SyntaxError()
    {
        
    }

    /********************************************************************
	Function:	IsSubstringInString

	Arguments:	2 strings			

	Returns:	true if sz_IsInString is contained completely in sz_Expression

	Comments:	Compares individual characters until characters don't match
				or the \0 in szIsInString is found
	
				*note the ++ in the while statement (not important but interesting...
				the increment happens after the comparision
    *********************************************************************/
    bool /*int*/ IsSubstringInString(string sz_IsInString, string sz_Expression)
    {
        return sz_Expression.Contains(sz_IsInString);
           
    }

    //should be at the current position too of course (who cares if its in there somewhere ....)
    bool IsSubstringInString(string sz_Expression, string sz_IsInString, int iPos)
    {
        if (iPos >= sz_Expression.Length)
            return false;

        int iIndex = sz_Expression.IndexOf(sz_IsInString, iPos);

        if (iIndex != iPos)
            return false;
        return true;
        
    }

    /********************************************************************
	Function:	GetDecimalPlaces

	Arguments:				

	Returns:	the number of decimal places

	Comments:	find the total number of decimal places from two doubles
    *********************************************************************/
    int GetDecimalPlaces(double dbArg1, double dbArg2)
    {
        string szArg1, szArg2, szDec1, szDec2;

        int iDecimalNumbers = 0;
        int iDecimalNumbers2 = 0;
        int iDecNum = 0;

        szArg1 = dbArg1.ToString();
        szArg2 = dbArg2.ToString();
        szDec1 = szArg1;
        szDec2 = szArg2;


        if (szArg1.Contains("e") == true ||
            szArg2.Contains("e") == true)
        {
            return iSigFigs;
        }

	    iDecimalNumbers=szDec1.Length-1;
	    iDecimalNumbers2=szDec2.Length-1;

	    if(iDecimalNumbers <= iDecimalNumbers2) 
		    iDecNum=iDecimalNumbers2;
	    else
		    iDecNum=iDecimalNumbers;

        iDecNum = Math.Max(iDecimalNumbers, iDecimalNumbers2);

	    //return answer
	    return iDecNum;

        //this seemed like a lot of work to do very little ... I won't question ...
    }

    double RoundDouble(double doValue, int nPrecision)
    {
	    const double doBase = 10.0f;
	    double doComplete5, doComplete5i;
	
	    doComplete5 = doValue * Math.Pow(doBase, (double) (nPrecision + 1));
	
	    if(doValue < 0.0f)
		    doComplete5 -= 5.0f;
	    else
		    doComplete5 += 5.0f;
	
	    doComplete5 /= doBase;

	    //modf(doComplete5, &doComplete5i);
        doComplete5i = (double)(Int64)doComplete5;

	
	    return doComplete5i / Math.Pow(doBase, (double) nPrecision);
    }

    public DRGStateType GetDRGState()
    {
	    return eDRGState;
    }

    public bool SetDRGState(DRGStateType eNewDRGState)
    {
        if (eNewDRGState == DRGStateType.DRG_Next)
	    {
		    if(eDRGState==DRGStateType.DRG_Degrees)
			    eDRGState=DRGStateType.DRG_Radians;
		    else if(eDRGState==DRGStateType.DRG_Radians)
			    eDRGState=DRGStateType.DRG_Gradians;
		    else 
			    eDRGState=DRGStateType.DRG_Degrees;
	    }
	    else
            eDRGState = eNewDRGState;
	    return true;
    }

    public bool SetBase(int iNewBase)
    {
        if (iNewBase < BaseConverter.MIN_BASE)
		    return false;
        else if (iNewBase > BaseConverter.MAX_BASE)
		    return false;
        else if (iNewBase == iBase)
		    return true;
	    else
            iBase = iNewBase;
	    return true;
    }

    public bool SetBaseBits(int iBaseBits)
    {
        if (iBaseBits < 0)
            return false;

        oBaseConv.iBitAccuracy = iBaseBits;
        return true;

    }

    public bool ConvertDRG(ref string szNumber, ref string szResult)
    {
        string szTemp;

	    //little error checking
        if (szNumber.Length < 1)
            szNumber = "0";

        szTemp = szNumber;    
	
	    switch(eDRGState) 
	    {
	    case DRGStateType.DRG_Degrees:
            szTemp = szTemp + "*" + "pi" + "/180.0";
		    Calculate(szTemp, ref szResult);
            return true;
		    
		
	    case DRGStateType.DRG_Radians:
            szTemp = szTemp + "*200.0/" +"pi";
		    Calculate(szTemp, ref szResult);
		    return true;

	    case DRGStateType.DRG_Gradians:
            szTemp = szTemp + "*9.0/10.0";
		    Calculate(szTemp, ref szResult);
		    return true;
            
	    default:
		    return false;
        }
    }

    void SetUseTape(bool bNewUseTape)
    {
        if (bNewUseTape == bUseTape)
		    return;
        bUseTape = bNewUseTape;

        if (bUseTape == true)
        {
         //   oTape.Init();
        }
    //  don't do this because we may turn the tape on and off ... won't clear it if we've already started
    //  ie graphing, memory saving, etc
    //	else
    //		oTape.ClearTape();
    }

    public bool GetFullTapePath(ref string szPath)
    {
        if(szPath == null)
            return false;

        szPath = "";

        return false;

        /*if(bUseTape)
        {
            //hack but that's perfect     
            oTape.SaveTape(true);
        }

        return oTape.GetFullTapePath(szPath);*/

    }

    void ConvertFromDMS(ref string szEq)
    {
        int iLoc = szEq.IndexOf("°");

        if(iLoc == -1)
            return;
    
        bDMS = true;

        int iStart;
        int iEnd;
        int iLen;
        int iEqLen;

        string szChar;

        string szDMS;

        while(iLoc != -1)
        {
            iEqLen = szEq.Length;

            //find the start of the number
            iStart = iLoc - 1;

            while(true)
            {
                if(iStart < 0)
                    break;

                szChar = szEq.Substring(iStart);
            
                if(IsNum(szChar) == false)
                    break;

                iStart--;
            }

            iStart++;

            iEnd = iLoc + 1;

            while(true)
            {
                if(iEnd >= iEqLen)
                    break;

                szChar = szEq.Substring(iEnd);

                if(IsNum(szChar) == false)
                    break;

                iEnd++;
            }

            iEnd--;

            iLen = iEnd - iStart + 1;

            if(iLen <= 0)
                continue;//no good but we'll play safe

            
            szDMS = szEq.Substring(iStart, iLen);

            DMSToDeg(ref szDMS);

            //now the fun part ... rebuild the string
            szEq = szEq.Substring(0, iStart) + szDMS + szEq.Substring(iStart + iLen);
            //left the lines below just in case it doesn't work ... 
            //oStr->Delete(iStart, iLen, szEq);
            //oStr->Insert(szEq, szDMS, iStart);

            //and try it again ...
            //start index doesn't really matter since its already been swapped out ...
            iLoc = szEq.IndexOf("°");

            

        }
    }

    //quick check for DMS functions
    bool IsNum(string szNum)
    {
        if (szNum[0] == ('.') || szNum[0] == ('°') || szNum[0] == ('\x2032'))
            return true;
        if (szNum[0] >= ('0') && szNum[0] <= ('9'))
            return true;

        return false;
    }

    void DMSToDeg(ref string szValue)
    {   //assume a number only
        //assume it is formatted for DMS already

        int iDegLoc = szValue.IndexOf("°");
        int iMinLoc = szValue.IndexOf("\x2032");

        double dbValue = 0;
        double dbTemp;
        string szTemp;

        szTemp = szValue.Substring(0, iDegLoc);

        dbValue = Convert.ToDouble(szTemp);
        
        if(iMinLoc == -1)
        {

            szTemp = szValue.Substring(iDegLoc + 1, szValue.Length - iDegLoc - 1);

            if (szTemp.Length > 0)
            {
                dbTemp = Convert.ToDouble(szTemp);
                dbValue += (dbTemp / 60.0);
            }
            goto Error;
        }

        //minutes
        szTemp = szValue.Substring(iDegLoc + 1,  iMinLoc - iDegLoc - 1);
        dbTemp = Convert.ToDouble(szTemp);
        dbValue += (dbTemp / 60.0); 

        //seconds
        szTemp = szValue.Substring(iMinLoc + 1, szValue.Length - iMinLoc - 1);
        dbTemp = Convert.ToDouble(szTemp);
        dbValue += (dbTemp / 3600.0); 

    Error:
        DblToString(ref szValue, dbValue, 15);
    }

    void ClearArrays()
    {
        for (int i = 0; i < dbReg.Length; i++)
            dbReg[i] = 0;
        for (int i = 0; i < iStack.Length; i++)
            iStack[i] = 0;
        for (int i = 0; i < iToken.Length; i++)
            iToken[i] = 0;

    }

    //DoubleToString(szAnswer, dbReg[0], iSigFigs);
    void DblToString(ref string szAnswer, double dbNum, int iSigFigs)
    {
        //bugbug ... we can do this via format but we'll have to build the string ... later
        szAnswer = dbNum.ToString();
    }

}

