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


    public class ButtonLabels
    {
        public static string[] szEqualsLabels = new string[]
        {("="), ("Enter"), (" ")};

        public static string[] szNumberLabels = new string[]
	        {("0"), ("1"), ("2"), ("3"), ("4"),
	         ("5"), ("6"), ("7"), ("8"), ("9"), 
	         ("A"), ("B"), ("C"), ("D"), ("E"),
             ("F"), ("."), ("e"), ("°\x2032\x2033")};

        public static string[] szOperatorLabels = new string[]
	        {("+"),	("-"),	("×"),	("÷"),
	        ("x"),	("Exp"),	("y\x221A\x203E"),	("nCr"),
	        ("nPr"),	("xy>r"),("r\x3B8>x"), ("xy>\x3B8"),
	        ("r\x3B8>y"),	("AND"),	("OR"),	("XOR"),
	        ("NOR"),	("MOD"), ("EEX"), ("XNOR"), ("NAND"),
            ("%T"), ("\x2206%")};

        public static string[] szOperatorExp = new string[]
	        {(""),	(""),	(""),	(""),
	        ("y"),	(""),	(""),	(""),
	        (""),	(""),	(""),	(""),
	        (""),	(""),	(""),	(""),
            (""),	(""),	(""),	(""),
	        (""),	(""),	("")};

        public static string[] szFunctionLabels = new string[]
        {
	        ("cos"),		("sin"),		("tan"),
	        ("acos"),		("asin"),		("atan"),
	        ("cosh"),		("sinh"),		("tanh"),
	        ("acsh"),		("asnh"),		("atnh"),
	        ("DRG>"),		("DRG"),			//DRG...handle as special cases
	        ("log"),		("ln"),		    ("log2"),
	        ("|x|"),		("+/-"),		("dec"),
	        ("d°m\x2032s"),	("1/x"),		("x\x00B2"),
	        ("x\x00B3"),	("\x221A\x203E"), ("\x00B3\x221A\x203E"),
	        ("e"),		    ("10"),		    ("x!"),
	        ("\x2211n"),    ("%"),	        ("rand"),
	        ("°\x2032\x2033"), ("CHS"),//pretty good
        };

        public static string[] szFunctionExp = new string[]
        {
	        (""),			(""),			(""),
	        (""),			(""),			(""),
	        (""),			(""),			(""),
	        (""),			(""),			(""),
	        (""),			(""),			//DRG...handle as special cases
	        (""),			(""),			(""),
	        (""),			(""),			(""),
	        (""),			(""),			(""),
	        (""),			(""),			(""),
	        ("x"),		    ("x"),		    (""),
	        (""),			(""),			(""),
	        (""),           ("")
        };

        public static string[] szConstantLabels = new string[]
        {
	        ("\x03C0"),/*pi*/	("e"),			("c"),
	        ("G"),			("Me"),			("Mp"),
	        ("Mn"),			("Mu"),			("Na"),
	        ("eV"),			("alpha"),		("sigma"),
	        ("h"),			("R"),			("k"),
	        ("u\x25E6"),		("F"),			("Au"),
	        ("g"),			("amu"),			("X"),
	        ("LastX"),        ("Ans")
        };

        public static string[] szBracketLabels = new string[]
        {
	        ("("),	(")")
        };

        public static string[] szMemoryLabels = new string[]
        {
	        ("MS"),			("MS2"),			("MS3"),
	        ("MS4"),			("MS5"),			("MS6"),
	        ("MS7"),			("MS8"),			("MR"),
	        ("MR2"),			("MR3"),			("MR4"),
	        ("MR5"),			("MR6"),			("MR7"),
	        ("MR8"),			("MC"),			("MC2"),
	        ("MC3"),			("MC4"),			("MC5"),
	        ("MC6"),			("MC7"),			("MC8"),
	        ("M+"),			("M1"),			("M2"),
	        ("M3"),			("M4"),			("M5"),
	        ("M6"),			("M7"),			("M8"),
	        ("M+"),			("M-")
        };

        public static string[] szClearLabels = new string[]
        {
	        ("C"),	("CA"),	("CE"),	("←"), ("Reset"), ("CLX")
        };

        public static string[] szBaseLabels = new string[]
        {
	        ("BIN"),	("B3"),	("B4"),	("B5"),
	        ("B6"),	("B7"),	("OCT"),	("B9"),
	        ("DEC"),	("B11"),	("B12"),	("B13"),
	        ("B14"),	("B15"),	("HEX"),  ("BCD") //not really a "base" per say but close enough ...
        };

        public static string[] szRPNLabels = new string[]
        {
	        ("X<>Y"), ("R Up"), ("Roll"), ("Drop")
        };


        public static string[] szCalcStateLabels = new string[]
        {
	        ("2ndF"),
	        ("Hyp"),
	        (""),
	        ("RCL"),
	        ("STO"),
	        (""),
	        ("RCL"),
	        ("CPT"),
            ("const"),
        };

        public static string[] szClipboardLabels = new string[]
        {
	        ("CPY"),
	        ("CpEq"),
	        ("PST")
        };

        public static string[] szWorksheetLabels = new string[]
        {
	        ("↑"),//draw arrows instead
	        ("↓"),
	        ("Reset"),
	        ("Calc"),
	        ("Graph"),
	        ("Load"),
	        ("Save"),
	        ("Opt.."),
	        ("Next"),
	        ("Back"),//9
        };

        public static string[] szLaunchLabels = new string[]
        {
	        ("TVM"),
	        ("Grph"),
	        ("Stat"),
	        ("Quit"),
	        ("Quad"),
	        ("Dep"),
	        ("Cash"),
	        ("ICom"),
            ("%\x2206"),
	        ("Date"),
	        ("Prft"),
	        ("BrkE"),
            ("Mort"),
            ("Lease"),
            ("Loan"),
            ("Tip"),//15
            ("LinR"),
            ("ExpR"),
            ("LogR"),
            ("PwrR"),
        };

        public static string[] szGraphLabels = new string[]
        {
	        ("graph"),
	        ("f(X)"),
	        ("X"),
        };

    }

