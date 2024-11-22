using System;

public enum InputType		//14 button Types total
{
    INPUT_Number,
    INPUT_Operator,
    INPUT_Function,
    INPUT_Bracket,
    INPUT_Memory,
    INPUT_Equals,
    INPUT_Constants,
    INPUT_CalcState,//7
    INPUT_Clear,
    INPUT_Clipboard,
    INPUT_BaseConversion,
    INPUT_RPN,
    INPUT_WS,
    INPUT_Launch,	//13
    INPUT_NULL,		//Used for buttons we're going to draw over
    INPUT_Custom,	//Used for buttons we're going to pass in text as opposed to taking out of the labels.h file
    INPUT_Graph,	//16
    INPUT_Other,
};


//Numerate all possible single digit and constant calc inputs
//Also includes adding the last Answer
public enum NumberType
{
    NUM_0 = 0x00000,
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,

    NUM_A,			//10
    NUM_B,
    NUM_C,
    NUM_D,
    NUM_E,
    NUM_F,

    NUM_Period,		//16

    NUM_e,
    NUM_DMS,
    NUM_Min,
    NUM_Count,
};

//Numerate all Calculator Operators
public enum OperatorType
{
    // Basic Calculator Opps
    OPP_Plus,
    OPP_Minus,
    OPP_Times,
    OPP_Divide,

    // Scientific Calculator Opps
    //exponents
    OPP_XY,//4
    OPP_EXP,//10^x
    OPP_XrY,//root x or nth root

    OPP_NCR,
    OPP_NPR,

    OPP_XYToR,//9
    OPP_RThetaToX,
    OPP_XYtoTheta,
    OPP_RThetaToY,

    //Bitwise Operators
    OPP_AND,//13
    OPP_OR,
    OPP_XOR,
    OPP_NOR,
    OPP_MOD,

    OPP_EEX, //same as EXP - RPN style
    OPP_XNOR,
    OPP_NAND,

    OPP_PcntT,//%T
    OPP_DPcnt,//D%
};

public enum FunctionType//32
{
    // Scientific Calculator Functs - Trig
    FUNCTION_Cos = 0x0000,
    FUNCTION_Sin,
    FUNCTION_Tan,
    FUNCTION_ACos,		// Arc
    FUNCTION_ASin,
    FUNCTION_ATan,
    FUNCTION_HCos,		//Hyp
    FUNCTION_HSin,
    FUNCTION_HTan,
    FUNCTION_AHCos,		//ArcHyp
    FUNCTION_AHSin,
    FUNCTION_AHTan,

    FUNCTION_DRG,//12		//change value (degrees, radians, gradians)   12
    FUNCTION_DRGN,		//change calc state

    FUNCTION_Log,	//14	
    FUNCTION_LN,
    FUNCTION_Log2,

    FUNCTION_ABS,//17		
    FUNCTION_PlusMinus,
    FUNCTION_DegToDec,
    FUNCTION_DecToDeg,

    // Scientific Calculator Functs - Algebra
    FUNCTION_Inverse,//21	
    FUNCTION_Squared,
    FUNCTION_Cubed,
    FUNCTION_SquareRoot,
    FUNCTION_CubeRoot,
    FUNCTION_EX,
    FUNCTION_10X,
    FUNCTION_Factorial,
    FUNCTION_Sum,

    FUNCTION_Percentage,//30

    FUNCTION_Random,

    FUNCTION_ShowDeg,//another one

    FUNCTION_CHS, //same as +/- ... RPN speak
};

public enum ConstantType	//21
{
    CONSTANT_Pi,
    CONSTANT_e,

    CONSTANT_c,		//2
    CONSTANT_G,
    CONSTANT_Me,
    CONSTANT_Mp,
    CONSTANT_Mn,
    CONSTANT_Mu,
    CONSTANT_Na,
    CONSTANT_Ev,
    CONSTANT_Alpha,//10
    CONSTANT_Sigma,
    CONSTANT_H,
    CONSTANT_R,
    CONSTANT_K,
    CONSTANT_Unot,
    CONSTANT_F,
    CONSTANT_Au,
    CONSTANT_Grav,
    CONSTANT_amu,
    CONSTANT_X,
    CONSTANT_LastX = 98,
    CONSTANT_Last_Ans = 99,//19
};

public enum BracketType	//2
{
    BRACKET_Open,
    BRACKET_Close,
};

public enum EqualsType
{
    EQUALS_Equals,
    EQUALS_Enter,
};

public enum CalcStateType	//8
{
    CALCSTATE_2ndF,
    CALCSTATE_Hyp,
    CALCSTATE_2ndF_Hyp,
    CALCSTATE_MemoryRecall,
    CALCSTATE_MemoryStore,
    CALCSTATE_Normal,
    CALCSTATE_Recall,	//for financial ... no onscreen for the moment
    CALCSTATE_Compute,	//for financial ... no onscreen for the moment
    CALCSTATE_Constants, //gotta put it somwhere
};

public enum ClearType
{
    CLEAR_Clear,
    CLEAR_ClearAll,
    CLEAR_CE,
    CLEAR_BackSpace,
    CLEAR_Reset,
    CLEAR_CLX,//clear X for RPN
};

public enum ClipboardType
{
    CLIPBOARD_Copy,
    CLIPBOARD_CopyEq,
    CLIPBOARD_Paste,
};

public enum MemoryType
{
    MEMORY_StoreMem1,
    MEMORY_StoreMem2,
    MEMORY_StoreMem3,
    MEMORY_StoreMem4,
    MEMORY_StoreMem5,
    MEMORY_StoreMem6,
    MEMORY_StoreMem7,
    MEMORY_StoreMem8,
    MEMORY_RecallMem1,//8
    MEMORY_RecallMem2,
    MEMORY_RecallMem3,
    MEMORY_RecallMem4,
    MEMORY_RecallMem5,
    MEMORY_RecallMem6,
    MEMORY_RecallMem7,
    MEMORY_RecallMem8,
    MEMORY_ClearMem1,//16
    MEMORY_ClearMem2,
    MEMORY_ClearMem3,
    MEMORY_ClearMem4,
    MEMORY_ClearMem5,
    MEMORY_ClearMem6,
    MEMORY_ClearMem7,
    MEMORY_ClearMem8,
    MEMORY_Plus1,		//24
    MEMORY_1,
    MEMORY_2,
    MEMORY_3,
    MEMORY_4,
    MEMORY_5,
    MEMORY_6,
    MEMORY_7,
    MEMORY_8,
    MEMORY_PLUS,
    MEMORY_MINUS,
    MEMORY_LastAns = 99,
};

public enum CalcOperationType
{
    CALC_String,
    CALC_OrderOfOpps,
    CALC_DAL,
    CALC_Chain,				//Chain input method
    CALC_RPN,
};

public enum FormatAnswerType
{
    ANS_ANS,
    ANS_EQUATION,
    ANS_NUMBER,
};

public enum DisplayType
{
    DISPLAY_Float,
    DISPLAY_Fixed,
    DISPLAY_Scientific,
    DISPLAY_Engineering,
    DISPLAY_Fractions,
    DISPLAY_DMS,
    DISPLAY_BCD,
    DISPLAY_Count,
};

public enum ResultType
{
    RESULT_Full,
    RESULT_Value,
    RESULT_Exp,
};

public enum DRGStateType
{
    DRG_Degrees,
    DRG_Radians,
    DRG_Gradians,
    DRG_Next,
};

public enum BASEBITSType
{
    BASEBITS_BYTE = 8,
    BASEBITS_WORD = 18,
    BASEBITS_DWORD = 32,
    BASEBITS_48 = 48,
    BASEBITS_QWORD = 64,
};

public enum RPNManipulationType
{
    RPN_XY,
    RPN_RotateUp,
    RPN_RotateDown,
    RPN_StackDown,
};

public enum WorksheetType
{
    WS_Up,
    WS_Down,
    WS_Clear,
    WS_Comp,
    WS_Graph,
    WS_Load,
    WS_Save,
    WS_Options,
    WS_Next,
    WS_Back,
};

public enum LaunchType
{//remember .. this order should correspond to the string arrays
    LAUNCH_TVM,
    LAUNCH_Graph,
    LAUNCH_Stats,
    LAUNCH_Close,
    LAUNCH_Quad,
    LAUNCH_Depreciation,
    LAUNCH_CashFlow,
    LAUNCH_CompoundInt,
    LAUNCH_PercentChange,
    LAUNCH_Date,
    LAUNCH_ProfitMargin,
    LAUNCH_BreakEven,
    LAUNCH_Mort,
    LAUNCH_Lease,
    LAUNCH_Loan,
    LAUNCH_Tip,
    LAUNCH_LinearReg,
    LAUNCH_ExpReg,
    LAUNCH_LogReg,
    LAUNCH_Power,
    LAUNCH_Count,
};

public enum GraphType
{
    GRAPH_Graph,
    GRAPH_X,
    GRAPH_fX,
};

public class CalcGlobals
{
    public CalcGlobals()
	{
	}

    public const double CONST_NUM_PI = 3.1415926535897932384626;
    public const double CONST_NUM_e = 2.718281828459045235;


    public const int MAX_Bracket_Levels=		20;

    public const int NUMBER_OF_Numbers = (int)NumberType.NUM_Count;
    public const int NUMBER_OF_Operators=		23;
    public const int NUMBER_OF_Functions=		34;
    public const int NUMBER_OF_Constants=		23;
    public const int NUMBER_OF_Brackets=		2;
    public const int NUMBER_OF_Equals=		    1;
    public const int NUMBER_OF_CalcStates=	    9;
    public const int NUMBER_OF_Clear=			6;
    public const int NUMBER_OF_Clipboard=		3;
    public const int NUMBER_OF_Memory=		    35;
    public const int NUMBER_OF_Bases=			16;
    public const int NUMBER_OF_RPN=			    4;
    public const int NUMBER_OF_Worksheet=		10;
    public const int NUMBER_OF_Launch = ((int)LaunchType.LAUNCH_Count); //new financial screens
    public const int NUMBER_OF_MemoryBanks=	    10;
    public const int NUMBER_OF_Graphing=		3;

    public const int MAX_ERROR_CONV=		    NUMBER_OF_Operators+5;

    public const int BASE_BCD = 17;



    public static string[] szNumbers = new string[]
	    {("0"), ("1"), ("2"), ("3"), ("4"),
	     ("5"), ("6"), ("7"), ("8"), ("9"), 
	     ("A"), ("B"), ("C"), ("D"), ("E"),
         ("F"), ("."), ("e"), ("°"), ("\x2032")};

    public static string[] szOperators = new string[]
	    {("+"),
	    ("-"),
	    ("×"),
	    ("÷"),
	    ("^"),
	    ("E"),
	    ("r"),
	    ("C"),
	    ("P"),
	    ("R"),
	    ("X"),
	    ("<"),
	    ("Y"),
	    ("&"),
	    ("|"),
	    ("x"),
	    ("n"),
	    ("%"),//17
        ("E"),
        ("o"),
        ("d"),//20
        ("T"),
        ("D"),
    };

    public static string[] szFunctionsFull = new string[]
    {
	    ("cos("),		("sin("),		("tan("),
	    ("acos("),	("asin("),	("atan("),
	    ("cosh("),	("sinh("),	("tanh("),
	    ("acsh("),	("asnh("),	("atnh("),
	    (""),			(""),			//DRG...handle as special cases
	    ("log("),		("ln("),		("log2("),
	    ("abs("),		("pm("),		("dc("),
	    ("dms("),		("inv("),		("sqrd("),
	    ("cbd("),		("sqrt("),	("crt("),
	    ("e^("),		("10^("),		("!("),
	    ("sum("),		("pct("),	    ("rand("),
	    (""),         (""),//dec to deg  & CHS (RPN +-)
    };

    public static string[] szBrackets = new string[]
	    {("("), (")")};

    public static string[] szConstants = new string[]
    {
	/*  CONST_PI,	CONST_e,	CONST_c,	CONST_G,	CONST_Me,		
	    CONST_Mp,	CONST_Mn,	CONST_Mu,	CONST_Na,	CONST_Ev,	
	    CONST_Alpha,CONST_Sigma,CONST_H,	CONST_R,	CONST_K,	
	    CONST_mu_0,	CONST_F,	CONST_Au,   CONST_Grav, CONST_Amu,*/

         ("(pi)")
        ,("(e)")
        ,("(299792458)")			//299 792 458 m s-1      (speed of light)
        ,("(6.6742e-011)")			//6.6742 x 10-11 m3 kg-1 s-2  (constant of gravitation)
        ,("(9.1093826e-031)")		//9.109 3826 x 10-31 kg  (electron mass)

        ,("(1.67262158e-027)")
        ,("(1.67492716e-027)")   
        ,("(1.88353109e-028)")		
        ,("(6.022141998e+023)")	//6.022 1415 x 1023 mol-1   (Avogadro constant)
        ,("(1.60217646e-019)")		//1.602 176 53 x 10-19 C (elementary charge)

        ,("(7.297352533e-003)")
        ,("(5.6705e-008)")
        ,("(6.6260693e-034)")		//6.626 0693 x 10-34 J s (Planck constant)
        ,("(8.314472)")			//8.314 472 J mol-1 K-1  (molar gas constant)
        ,("(1.3806505e-023)")		//1.380 6505 x 10-23 J K-1 (Bolzmann constant)

        ,("(4*pi*10^-7)")			//4pi x 10-7 N A-2   (magnetic field constant)
        ,("(96485.3415)")
        ,("(149597870691)")
        ,("(9.80665)")
        ,("1.66053886e-027")	

        ,("X")
    };

    public static string[] szErrorConvert = new string[]
    {
	    ("+"),
	    ("×"),
	    ("/"),
	    ("*"),
	    ("÷"),
	    ("E"),
	    ("e"),
	    ("²"),
	    ("r"),
	    ("^"),
	    ("C"),
	    ("P"),
	    ("R"),
	    ("X"),
	    ("<"),
	    ("Y"),
	    ("&"),
	    ("|"),
	    ("x"),
	    ("n"),
	    ("%"),
        (" "),
    };


    public static string[] szBaseNames = new string[]
    {
	    ("BIN"),
	    ("Base 3"),
	    ("Base 4"),
	    ("Base 5"),
	    ("Base 6"),
	    ("Base 7"),
	    ("OCT"),
	    ("Base 9"),
	    ("Decimal"),
	    ("Base 11"),
	    ("Base 12"),
	    ("Base 13"),
	    ("Base 14"),
	    ("Base 15"),
	    ("HEX"),
        ("BCD")//its fine here ... if we go beyond 16 is sort of corrects the index ...
    };


    public static string[] szDRG = new string[]
    {
	    ("DEG"),
	    ("RAD"),
	    ("GRAD"),
    };


    
}
