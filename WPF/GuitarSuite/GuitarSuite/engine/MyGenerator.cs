using System;
using System.Collections.Generic;
using System.Text;


namespace GuitarSuite {

//we'll add to it as we find more chords ...
public enum EnumChordType
{
    CHORD_maj,
    CHORD_5,
    CHORD_6,
    CHORD_7,
    CHORD_maj7,
    CHORD_9,
    CHORD_maj9,
    CHORD_11,
    CHORD_13,
    CHORD_maj13,
    CHORD_m,
    CHORD_m6,
    CHORD_m7,
    CHORD_m9,
    CHORD_m11,
    CHORD_m13,
    CHORD_sus2,
    CHORD_sus4,
    CHORD_dim,
    CHORD_aug,
    //	CHORD_6over9,
    CHORD_7sus4,
    CHORD_7b5,
    CHORD_7b9,
	CHORD_7s5,//7#5
	CHORD_9s5,//9#5
    CHORD_9sus4,
    CHORD_add9,
    CHORD_aug9,
    CHORD_Count,
};

public enum EnumChordRoot
{
    ROOT_A,
    ROOT_As,
    ROOT_B,
    ROOT_C,
    ROOT_Cs,
    ROOT_D,
    ROOT_Ds,
    ROOT_E,
    ROOT_F,
    ROOT_Fs,
    ROOT_G,
    ROOT_Gs,
    ROOT_Count, //12 of course
};

public enum EnumInstrument
{
    INST_Guitar,
    //    INST_5StringBass,
    INST_4StringBass,
    INST_Banjo,
    INST_Lute,
    INST_Ukulele,
    INST_UkuleleConcert,
    INST_count,
};
//doesn't really need to be here but doesn't do any harm ...
public enum EnumInstrumentSound
{
    Accoustic,
    Overdrive,
	Electric,
	Banjo,
    Count,
};

public enum EnumTuningTextStyle
{
	Normal,
	Title,
	Notes,
}



public enum EnumScales
{
	SCALE_Major,
	SCALE_HarmonicMinor,
	SCALE_MelodicMinorAscending,
	SCALE_MelodicMinorDescending,
	SCALE_PentatonicMajor,
	SCALE_PentatonicMinor,
	SCALE_PentatonicBlues,
	SCALE_PentatonicNeutral,
	//SCALE_Ionian, - same as major
	SCALE_Dorian,
	SCALE_Phrygian,
	SCALE_Lydian,
	SCALE_Mixolydian,
	SCALE_Aeolian,
	SCALE_Locrian,
	SCALE_HalfDiminished,
	SCALE_WholeDiminished,
	SCALE_Whole,
	SCALE_Augmented,
	SCALE_Chromatic,
	SCALE_RoumanianMinor,
	SCALE_SpanishGypsy,
	SCALE_Blues,
	SCALE_DoubleHarmonic,
	SCALE_EightToneSpanish,
	SCALE_Enigmatic,
	SCALE_LeadingWholeTone,
	SCALE_LydianAugmented,
	SCALE_NeopolitanMajor,
	SCALE_NeopolitanMinor,
	SCALE_Pelog,
	SCALE_Prometheus,
	SCALE_PrometheusNeopolitan,
	SCALE_SixToneSymmetrical,
	SCALE_SuperLocrian,
	SCALE_LydianMinor,
	SCALE_LydianDiminished,
	SCALE_NineToneScale,
	SCALE_AuxiliaryDiminished,
	SCALE_AuxiliaryAugmented,
	SCALE_AuxiliaryDiminishedBlues,
	SCALE_MajorLocrian,
	SCALE_Overtone,
	SCALE_DiminishedWholeTone,
	SCALE_Scale,
	SCALE_Count,
};

public enum EnumTuning
{
	TUNING_Standard,
	TUNING_Drop,
	TUNING_DropD,
	TUNING_DropDAlt,
	TUNING_OpenD,
	TUNING_OpenDm,
	TUNING_DModal,
	TUNING_Fourths,
	TUNING_OpenE,
	TUNING_ShiftedE,
	TUNING_G6,
	TUNING_OpenG,
	TUNING_Gm,
	TUNING_OpenA,
	TUNING_Am,
	TUNING_DropC,
	TUNING_C6,
	TUNING_OpenC,
	TUNING_LowC,
	TUNING_Count,
};
//thinking one tuning enum per instrument
public enum EnumTuningBass5
{
	TUNINGB5_Standard,
	TUNINGB5_DropD,
	TUNINGB5_HiC,
	TUNINGB5_HalfDown,
	TUNINGB5_FullDown,
	TUNINGB5_HalfDownHi,
	TUNINGB5_FullDownHi,
	TUNINGB5_Count,
};

public enum EnumTuningBass4
{
	TUNINGB4_Standard,  //E A D G
	TUNINGB4_DropD,     //G#D#C#F#
	TUNINGB4_HalfDown,  //EbAbDbGb
	TUNINGB4_LowB,      //B E A D
	TUNINGB4_DropC,     //C A D G
	TUNINGB4_Count,
};

public enum EnumTuningBanjo
{
	TUNINGBANJO_G,//GDGBD        
	TUNINGBANJO_GModal,//GDGCD        
	TUNINGBANJO_DoubleC,//GCGCD        
	TUNINGBANJO_C,//GDGDB        
	TUNINGBANJO_D,//DAF#AD
	TUNINGBANJO_DAlt,//F#DF#AD
	TUNINGBANJO_Guitar,//ADGBE        
	TUNINGBANJO_Count,
};

public enum EnumTuningLute
{
	TUNINGLUTE_Mandolin,    //G D A E
	TUNINGLUTE_Mandola,     //C G D A
	TUNINGLUTE_Pipa,        //D A D A
	TUNINGLUTE_Bouzouki,    //C F A D
	TUNINGLUTE_Bouzouki2,   //G D A D
	TUNINGLUTE_Bouzouki3,   //A D E A
	TUNINGLUTE_Count,
};

public enum EnumTuningUkulele
{
	TUNINGUKU_RentrantC,//gCEA
	TUNINGUKU_C,//GCEA
	TUNINGUKU_G,//dGBA
	TUNINGUKU_D,//ADF#B
	TUNINGUKU_B,
	TUNINGUKU_Cs,
	TUNINGUKU_Guitar,//DGBE
	TUNINGUKU_SlackKey,//gCEG
	TUNINGUKU_Count,
};


    public class MyGenerator
    {
        public MyGenerator()
		{	//constructor
			m_eTuning       = EnumTuning.TUNING_Standard;
			m_eTuningB4     = EnumTuningBass4.TUNINGB4_Standard;
			m_eTuningB5     = EnumTuningBass5.TUNINGB5_Standard;
			m_eTuningBanjo  = EnumTuningBanjo.TUNINGBANJO_G;
			m_eTuningLute   = EnumTuningLute.TUNINGLUTE_Mandolin;
			m_eTuningUkulele= EnumTuningUkulele.TUNINGUKU_RentrantC;

			m_eType = EnumChordType.CHORD_maj;
			m_eRoot = EnumChordRoot.ROOT_C;
			m_iChordIndex = 0;
			m_bIsLeftHanded = false;
			m_bNumberFrets = false;

            for (int i = 0; i < (int)EnumInstrument.INST_count; i++)
            {
                arrCustomTuning[i, 0] = 7;
                arrCustomTuning[i, 1] = 0;
                arrCustomTuning[i, 2] = 5;
                arrCustomTuning[i, 3] = 10;
                arrCustomTuning[i, 4] = 2;
                arrCustomTuning[i, 5] = 7;
            }
		}

        //I don't mind hiding theses down here
        string[] szTuningRes = new string[]
        {
            "IDS_OPT_Major",            
            "IDS_OPT_HarmonicMinor",  
            "IDS_OPT_MelodicMinorAscending",
            "IDS_OPT_MelodicMinorDescending",
            "IDS_OPT_PentatonicMajor",
            "IDS_OPT_PentatonicMinor",      
            "IDS_OPT_PentatonicBlues",      
            "IDS_OPT_PentatonicNeutral",    
            "IDS_OPT_Dorian",  
            "IDS_OPT_Phrygian",             
            "IDS_OPT_Lydian",           
            "IDS_OPT_Mixolydian",           
            "IDS_OPT_Aeolian",              
            "IDS_OPT_Locrian",              
            "IDS_OPT_HalfDiminished",       
            "IDS_OPT_WholeDiminished",      
            "IDS_OPT_Whole",                
            "IDS_OPT_Augmented",            
            "IDS_OPT_Chromatic",            
            "IDS_OPT_RomanianMinor",        
            "IDS_OPT_SpanishGypsy",         
            "IDS_OPT_Blues",                
            "IDS_OPT_DoubleHarmonic",       
            "IDS_OPT_EightToneSpanish",     
            "IDS_OPT_Enigmatic",            
            "IDS_OPT_LeadingWholeTone",     
            "IDS_OPT_LydianAugmented",      
            "IDS_OPT_NeopolitanMajor",      
            "IDS_OPT_NeopolitanMinor",      
            "IDS_OPT_Pelog",                
            "IDS_OPT_Prometheus",           
            "IDS_OPT_PrometheusNeopolitan", 
            "IDS_OPT_SixToneSymmetrical",   
            "IDS_OPT_SuperLocrian",         
            "IDS_OPT_LydianMinor",          
            "IDS_OPT_LydianDiminished",     
            "IDS_OPT_NineToneScale",        
            "IDS_OPT_AuxiliaryDiminished",  
            "IDS_OPT_AuxiliaryAugmented",   
            "IDS_OPT_AuxiliaryDiminishedBlues",
            "IDS_OPT_MajorLocrian",         
            "IDS_OPT_Overtone",             
            "IDS_OPT_DiminishedWholeTone",  
            "IDS_OPT_Scale",
            "IDS_OPT_ScalesOff"
        };
		
		public string[] m_szScalesCode = new string[] 
        {
            ("1,2,3,4,5,6,7"),
	        ("1,2,b3,4,5,b6,7"),
	        ("1,2,b3,4,5,6,7"),
	        ("1,2,3,4,5,b6,b7"),
	        ("1,2,3,5,6"),
	        ("1,b3,4,5,b7"),
	        ("1,b3,4,b5,5,b7"),//pent blues
	        ("1,2,4,5,b7"),
	        ("1,2,b3,4,5,6,b7"),//dorian
	        ("1,b2,b3,4,5,b6,b7"),
	        ("1,2,3,#4,5,6,7"),
	        ("1,2,3,4,5,6,b7"),
	        ("1,2,b3,4,5,b6,b7"),
	        ("1,b2,b3,4,b5,b6,b7"),
	        ("1,b2,b3,3,b5,5,6,b7"),//half diminished
	        ("1,2,b3,4,b5,#5,6,7"),
	        ("1,2,3,#4,#5,#6"),
	        ("1,#2,3,#4,#5,7"),
	        ("1,b2,2,b3,3,4,b5,5,#5,6,b7,7"),
	        ("1,2,b3,b5,5,6,b7"),
	        ("1,b2,3,4,5,#5,b7"),
	        ("1,b2,3,4,5,#5,7"), //blues 
	        ("1,b2,3,4,b5,#5,7"),
	        ("1,b2,b3,3,4,b5,#5,b7"),
	        ("1,b2,3,b5,#5,b7,7"),
	        ("1,2,3,b5,#5,6,b7"),
	        ("1,2,3,#4,#5,6,7"),
	        ("1,b2,b3,4,5,6,7"),
	        ("1,b2,b3,4,5,b6,b7"),
	        ("1,b2,b3,b5,b7,7"),//pelog
	        ("1,2,3,b5,6,b7"),
	        ("1,b2,3,b5,6,b7"),
	        ("1,b2,3,4,#5,6"),
	        ("1,b2,b3,3,b5,#5,b7"),
	        ("1,2,3,b5,5,b6,b7"),
	        ("1,2,b3,b5,5,b6,b7"),
	        ("1,2,b3,3,b5,5,#5,6,7"),//nine tone scale 
	        ("1,2,b3,4,b5,#5,6,7"),
	        ("1,2,3,b5,#5,b7"),
	        ("1,b2,b3,3,b5,6,b7"),
	        ("1,2,3,4,b5,#5,b7"),
	        ("1,2,3,b5,5,6,b7"),
	        ("1,b2,b3,3,b5,b6,b7"),
	        ("1,2,4,5,6,b7"),//scale
            (""),//off
        };

        public string[] m_szFrets = new string[]
        {
	        (""),    (""), ("III"), (""), ("V"),    (""),
	        ("VII"), (""), ("IX"),  (""), (""),     ("XII"),
	        (""),    (""), ("XV"),  (""), ("XVII"), ("")
        };

        string[] szRootsFlats = new string[]
        {
	        ("A"), ("B♭"), ("B"), ("C"), ("D♭"), ("D"),
	        ("E♭"), ("E"), ("F"), ("G♭"), ("G"), ("A♭")
        };

		string[] szRootsFlatsPlain = new string[]
        {
	        ("A"), ("Bb"), ("B"), ("C"), ("Db"), ("D"),
	        ("Eb"), ("E"), ("F"), ("Gb"), ("G"), ("Ab")
        };

		string[] szRoots = new string[]
        {
	        ("A"), ("A♯"), ("B"), ("C"), ("C♯"), ("D"),
	        ("D♯"), ("E"), ("F"), ("F♯"), ("G"), ("G♯")
        };

		string[] szRootsPlain = new string[]
        {
	        ("A"), ("A#"), ("B"), ("C"), ("C#"), ("D"),
	        ("D#"), ("E"), ("F"), ("F#"), ("G"), ("G#")
        };

		string [] szRootsSolfege = new string[] 
		{
			("La"), ("La♯"), ("Si"), ("Do"), ("Do♯"), ("Re"),
			("Re♯"), ("Mi"), ("Fa"), ("Fa♯"), ("Sol"), ("Sol♯")
		};

		string [] szRootsFlatSolfege = new string[] 
		{
			("La"), ("Si♭"), ("Si"), ("Do"), ("Re♭"), ("Re"),
			("Mi♭"), ("Mi"), ("Fa"), ("Sol♭"), ("Sol"), ("La♭")
		};


		public string[] m_szRootsPlain
		{
			get
            {
                if (Globals.Settings != null && Globals.Settings.bShowFlats == true)
                    return szRootsFlatsPlain;
                else
                    return szRootsPlain;
            }
        }

        public string[] m_szRoots
        {
            get
            {
				if(Globals.Settings != null && Globals.Settings.bUseSolfege == true)
					return m_szRootsSolfege;
                if (Globals.Settings != null && Globals.Settings.bShowFlats == true)
                    return szRootsFlats;
                return szRoots;
            }
        }

		public string[] m_szRootsSolfege
		{
			get
			{
				if (Globals.Settings != null && Globals.Settings.bShowFlats == true)
					return szRootsSolfege;
				else
					return szRootsFlatSolfege;
			}
		}

        public string[] m_szChords = new string[]
        {
	        ("maj"), ("5"), ("6"), ("7"), ("maj7"), 
	        ("9"), ("maj9"), ("11"), ("13"), ("maj13"), 
	        ("m"), ("m6"), ("m7"), ("m9"), ("m11"), 
	        ("m13"), ("sus2"), ("sus4"), ("dim"), ("aug"), 
        //	("6-9"), 
			("7sus4"), ("7♭5"), ("7♭9"),("7♯5"), ("9♯5"), ("9sus4"), 
	        ("add9"), ("aug9")
        };

        public static int SCALE_SIZE = 16;
        public static int NOTE_NOT_DEF = 255;
        public static int NUM_CHORDS = 12;

        public Fretboard m_oFretBoard = new Fretboard();
        public ChordGenerator m_oChord = new ChordGenerator();

        public EnumTuning m_eTuning;
        public EnumTuningBass4 m_eTuningB4;
        public EnumTuningBass5 m_eTuningB5;        //not used yet
        public EnumTuningBanjo m_eTuningBanjo;     //not used yet
        public EnumTuningLute m_eTuningLute
		{
			set { _eTuningLute = value; }
			get { return _eTuningLute; }
		}
		EnumTuningLute _eTuningLute;//reflection for testing

        public EnumTuningUkulele m_eTuningUkulele;
        public EnumTuningUkulele m_eTuningUkuleleConcert;


        public EnumChordRoot m_eRoot;
        public EnumChordType m_eType;

        public EnumInstrument m_eInst;
        public EnumInstrumentSound m_eSound;

        public EnumScales   m_eScale;//doesn't effect the other parts but I might as well have it in the generator

	    public int			m_iChordIndex;

	    public bool			m_bIsLeftHanded;
	    public bool			m_bNumberFrets; 

        public byte[]       m_btCurrentScale = new byte[SCALE_SIZE];//WAY bigger than needed 
        public byte[]       m_btSharpFlatScale = new byte[SCALE_SIZE];//sharp 2, flat 1, other 0
        public byte[]       m_btShapFlatToNoteIndex = new byte[SCALE_SIZE];

        EnumInstrument      GetInstrument(){return m_eInst;}

        //public so I can save/restore ... yeah I'm lazy
        public int[,]       arrCustomTuning = new int[(int)EnumInstrument.INST_count,6];//6 for the number of strings
		
		public string GetFretString(int iFret)
		{
			if(iFret < 0 || iFret > 17)
				return " ";
			return m_szFrets[iFret];
		}

        public string GetScaleString()
        {
            return GetScaleString((int)m_eScale);
        }

        public string GetScaleString(int iScale)
        {
            if (iScale < 0)
                return "";

            return Globals.rm.GetString(szTuningRes[iScale]);
            //return Globals.rm.GetString("IDS_TUN_Modal");
        }
		
		public string GetRootString()
		{
			return GetRootString((int)m_eRoot);
		}

		public string GetIntervalString(int iRoot, int iNote)
		{
			int interval = iNote - iRoot;
			while(interval < 0) 
				interval += 12;

			switch(interval)
			{
			case 0:
				return "Root";
			case 1:
				return Globals.rm.GetString("IDS_CHORDL_Minor") + " 2nd";
			case 2:
				return  "2nd";
			case 3:
				return Globals.rm.GetString("IDS_CHORDL_Minor") + " 3rd";
			case 4:
				return "3rd";
			case 5:
				return "4th";
			case 6:
				return Globals.rm.GetString("IDS_CHORDL_Dim")  + " 5th";
			case 7:
				return "5th";
			case 8:
				return Globals.rm.GetString("IDS_CHORDL_Minor") + " 6th";
			case 9:
				return "6th";
			case 10:
				return Globals.rm.GetString("IDS_CHORDL_Minor") + " 7th";
			case 11:
				return "7th";
			default:
				return " ";
			}
		}
		
		public string GetRootString(int iRoot)
		{
			if(iRoot < 0 || iRoot >= m_szRoots.Length)
				return " ";
			
			return m_szRoots[iRoot];
		}

		public string GetRootStringPlain(int iRoot)
		{
			if(iRoot < 0 || iRoot >= m_szRoots.Length)
				return " ";
			
			return m_szRootsPlain[iRoot];
		}
		
		public void NextRoot()
		{
			int iTemp = (int)m_eRoot + 1;
			if(iTemp >= m_szRoots.Length)
				iTemp = 0;
			
			SetRoot((EnumChordRoot)iTemp, true);
		}

        public string GetChordString()
        {
            return GetChordString((int)m_eType);
            
        }

        public string GetChordString(int iIndex)
        {
            if (iIndex < 0 || iIndex >= m_szChords.Length)
                return "";
            return m_szChords[iIndex];
        }
		
		public string GetChordStringLong(int iIndex)
		{
			if(iIndex < 0 || iIndex >= m_szChords.Length)
				return "";
			
			switch((EnumChordType)iIndex)
			{
			case EnumChordType.CHORD_11:
				return Globals.rm.GetString("IDS_CHORDL_11");
			case EnumChordType.CHORD_13:
				return Globals.rm.GetString("IDS_CHORDL_13");
			case EnumChordType.CHORD_5:
				return Globals.rm.GetString("IDS_CHORDL_5");
			case EnumChordType.CHORD_6:
				return Globals.rm.GetString("IDS_CHORDL_6");
			case EnumChordType.CHORD_7:
				return Globals.rm.GetString("IDS_CHORDL_7");
			case EnumChordType.CHORD_7b5:
				return Globals.rm.GetString("IDS_CHORDL_7b5");
			case EnumChordType.CHORD_7b9:
				return Globals.rm.GetString("IDS_CHORDL_7b9");
			case EnumChordType.CHORD_7sus4:
				return Globals.rm.GetString("IDS_CHORDL_7sus4");
			case EnumChordType.CHORD_9:
				return Globals.rm.GetString("IDS_CHORDL_9");
			case EnumChordType.CHORD_9sus4:
				return Globals.rm.GetString("IDS_CHORDL_9sus4");
			case EnumChordType.CHORD_add9:
				return Globals.rm.GetString("IDS_CHORDL_Add9");
			case EnumChordType.CHORD_aug:
				return Globals.rm.GetString("IDS_CHORDL_Aug");
			case EnumChordType.CHORD_aug9:
				return Globals.rm.GetString("IDS_CHORDL_Aug9");
			case EnumChordType.CHORD_dim:
				return Globals.rm.GetString("IDS_CHORDL_Dim");
			case EnumChordType.CHORD_m:
				return Globals.rm.GetString("IDS_CHORDL_Minor");
			case EnumChordType.CHORD_m11:
				return Globals.rm.GetString("IDS_CHORDL_Min11");
			case EnumChordType.CHORD_m13:
				return Globals.rm.GetString("IDS_CHORDL_Min13");
			case EnumChordType.CHORD_m6:
				return Globals.rm.GetString("IDS_CHORDL_Min6");
			case EnumChordType.CHORD_m7:
				return Globals.rm.GetString("IDS_CHORDL_Min7");
			case EnumChordType.CHORD_m9:
				return Globals.rm.GetString("IDS_CHORDL_Min9");
			case EnumChordType.CHORD_maj:
				return Globals.rm.GetString("IDS_CHORDL_Major");
			case EnumChordType.CHORD_maj13:
				return Globals.rm.GetString("IDS_CHORDL_Maj13");
			case EnumChordType.CHORD_maj7:
				return Globals.rm.GetString("IDS_CHORDL_Maj7");
			case EnumChordType.CHORD_maj9:
				return Globals.rm.GetString("IDS_CHORDL_Maj9");
			case EnumChordType.CHORD_sus2:
				return Globals.rm.GetString("IDS_CHORDL_Sus2");
			case EnumChordType.CHORD_sus4:
				return Globals.rm.GetString("IDS_CHORDL_Sus4");
			case EnumChordType.CHORD_7s5:
				return "Dominant 7th ♯5";
			case EnumChordType.CHORD_9s5:
				return "Dominant 9th ♯5";
			default:
				return "Unknown";
			}
		}

		//push to the next chord
        public void NextChord()
        {
            m_iChordIndex++; 
            if(m_iChordIndex >= NUM_CHORDS) 
                m_iChordIndex = 0;
        }

		//so we can manually set the chord index/variation 
		public void SetChordIndex (int index)
		{
			if(index > -1 && index < NUM_CHORDS)
				m_iChordIndex = index;
		}

		public string GetTuningNotesText(int iIndex)
		{
			return GetTuningNotesText(iIndex, m_eInst);
		}

		public string GetTuningNotesText(int iIndex, EnumInstrument eInst)
		{
			string szTemp = "";

            int iTuning = GetTuning();
            SetTuning(iIndex, false);

            for(int i = 0; i < GetNumStrings(); i++)
            {
                switch(GetOpenNote(i))
                {
                case (int)EnumChordRoot.ROOT_A:
                case (int)EnumChordRoot.ROOT_B:
                case (int)EnumChordRoot.ROOT_C:
                case (int)EnumChordRoot.ROOT_D:
                case (int)EnumChordRoot.ROOT_E:
                case (int)EnumChordRoot.ROOT_F:
                case (int)EnumChordRoot.ROOT_G:
                    szTemp += (m_szRootsPlain[GetOpenNote(i)]);
                //    szTemp += " ";
                    break;
                case (int)EnumChordRoot.ROOT_As:
                case (int)EnumChordRoot.ROOT_Cs:
                case (int)EnumChordRoot.ROOT_Ds:
                case (int)EnumChordRoot.ROOT_Fs:
                case (int)EnumChordRoot.ROOT_Gs:
                    szTemp += (m_szRootsPlain[GetOpenNote(i)]);
                    break;
                default:
                    return null;
                }
            }

			return szTemp;

		}

        
        string GetTuningLongText(int iIndex, EnumInstrument eInst)
        {
            string szTemp = null;
    
            szTemp = null;

            if(iIndex < 0 || iIndex > GetNumTuning(eInst))
                return null;

            if (iIndex == GetNumTuning(eInst))
                return GetCustomTuningText(eInst);

            szTemp = GetTuningText(iIndex, eInst) + " (" + GetTuningNotesText(iIndex, eInst) + ")";
			return szTemp;
        }

        string GetTuningTextBanjo(int iIndex)
        {
            string szTemp = null;

            switch(iIndex)
            {
            case (int)EnumTuningBanjo.TUNINGBANJO_G:
                szTemp = m_szRoots[(int)EnumChordRoot.ROOT_G];
                return szTemp;

            case (int)EnumTuningBanjo.TUNINGBANJO_GModal:
                szTemp = m_szRoots[(int)EnumChordRoot.ROOT_G] + Globals.rm.GetString("IDS_TUN_Modal");

                return szTemp;

            case (int)EnumTuningBanjo.TUNINGBANJO_DoubleC:
                szTemp = Globals.rm.GetString("IDS_TUN_Double");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_C]);
                return szTemp;
                
            case (int)EnumTuningBanjo.TUNINGBANJO_C:
                szTemp = m_szRoots[(int)EnumChordRoot.ROOT_C];
                return szTemp;
                
            case (int)EnumTuningBanjo.TUNINGBANJO_D:
                szTemp = m_szRoots[(int)EnumChordRoot.ROOT_D];
                return szTemp;
                
            case (int)EnumTuningBanjo.TUNINGBANJO_DAlt:
                szTemp = m_szRoots[(int)EnumChordRoot.ROOT_D];
                szTemp += (("2"));
                return szTemp;
              
            case (int)EnumTuningBanjo.TUNINGBANJO_Guitar:
                szTemp = Globals.rm.GetString("IDS_OPT_Guitar");
                return szTemp;
                
            }
            return null;
        }

        string GetTuningTextUkulele(int iIndex)
        {
	        string szTemp = null;
            szTemp = null;

	        switch(iIndex)
	        {
	        case (int)EnumTuningUkulele.TUNINGUKU_RentrantC:
		        szTemp = m_szRoots[(int)EnumChordRoot.ROOT_C];
                szTemp += ((" 'RE'"));
		        return szTemp;
		        
	        case (int)EnumTuningUkulele.TUNINGUKU_C:
                szTemp = m_szRoots[(int)EnumChordRoot.ROOT_C];		
                return szTemp;
		        
	        case (int)EnumTuningUkulele.TUNINGUKU_G:
                szTemp = m_szRoots[(int)EnumChordRoot.ROOT_G];
                szTemp += ((" 'RE'"));
		        return szTemp;
		        
	        case (int)EnumTuningUkulele.TUNINGUKU_D:
		        szTemp = m_szRoots[(int)EnumChordRoot.ROOT_D];
		        return szTemp;
		        
	        case (int)EnumTuningUkulele.TUNINGUKU_B:
                szTemp = m_szRoots[(int)EnumChordRoot.ROOT_B];
                szTemp += ((" 'RE'"));
		        return szTemp;
		        
	        case (int)EnumTuningUkulele.TUNINGUKU_Cs:
                szTemp = m_szRoots[(int)EnumChordRoot.ROOT_Cs];
                szTemp += ((" 'RE'"));
		        return szTemp;
		        
	        case (int)EnumTuningUkulele.TUNINGUKU_Guitar:
		        szTemp = Globals.rm.GetString("IDS_OPT_Guitar");
		        return szTemp;
		        
	        case (int)EnumTuningUkulele.TUNINGUKU_SlackKey:
		        szTemp = Globals.rm.GetString("IDS_TUN_SlackKey");
		        return szTemp;
		        
	        }
	        return null;
        }

        string GetTuningTextLute(int iIndex)
        {
            string szTemp = null;

            switch(iIndex)
            {
            case (int)EnumTuningLute.TUNINGLUTE_Mandolin:   //G D A E
                szTemp = Globals.rm.GetString("IDS_TUNE_Mandolin");
                return szTemp;
                
            case (int)EnumTuningLute.TUNINGLUTE_Mandola:     //C G D A
                szTemp = Globals.rm.GetString("IDS_TUNE_Mandola");
                return szTemp;
                
            case (int)EnumTuningLute.TUNINGLUTE_Pipa:        //D A D A
                szTemp = Globals.rm.GetString("IDS_TUNE_Pipa");
                return szTemp;
                
            case (int)EnumTuningLute.TUNINGLUTE_Bouzouki:    //C F A D
                szTemp = Globals.rm.GetString("IDS_TUNE_Bouzouki");
                return szTemp;
                
            case (int)EnumTuningLute.TUNINGLUTE_Bouzouki2:   //G D A D
                szTemp = Globals.rm.GetString("IDS_TUNE_Bouzouki2");
                return szTemp;
                
            case (int)EnumTuningLute.TUNINGLUTE_Bouzouki3:   //A D E A
                szTemp = Globals.rm.GetString("IDS_TUNE_Bouzouki3");
                return szTemp;
                
            }
            return null;
        }



        string GetTuningTextB4(int iIndex)
        {
            string szTemp = null;
            string szTemp2 = null;

            switch(iIndex)
            {
            case (int)EnumTuningBass4.TUNINGB4_Standard:
                return Globals.rm.GetString("IDS_TUN_Standard");
                
            case (int)EnumTuningBass4.TUNINGB4_DropD:
                szTemp = Globals.rm.GetString("IDS_TUN_Drop");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_D]);
                return szTemp;
                
            case (int)EnumTuningBass4.TUNINGB4_HalfDown:
                szTemp = Globals.rm.GetString("IDS_TUN_Half");
                szTemp2 = Globals.rm.GetString("IDS_TUN_Down");
        
                szTemp += " ";
                szTemp += (szTemp2);
                return szTemp;
                
            case (int)EnumTuningBass4.TUNINGB4_LowB:
                szTemp = Globals.rm.GetString("IDS_TUN_Low");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_B]);
                return szTemp;
                
            case (int)EnumTuningBass4.TUNINGB4_DropC:
                szTemp = Globals.rm.GetString("IDS_TUN_Drop");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_C]);
                return szTemp;
                
            default:
                return null;
                
            }
        }

        string GetTuningTextB5(int iIndex)
        {/* switch(iIndex)
            {
            case (int)EnumTuningBass5.TUNINGB5_Standard:
                return Globals.rm.GetString("IDS_TUN_Standard));
                break;
            case (int)EnumTuningBass5.TUNINGB5_DropD:
                return Globals.rm.GetString("IDS_TUN_DropD));
                break;
            case (int)EnumTuningBass5.TUNINGB5_HiC:
                szTemp = Globals.rm.GetString("IDS_TUNE_HighC");
                return szTemp;
                break;
            case (int)EnumTuningBass5.TUNINGB5_HalfDown:
                szTemp = Globals.rm.GetString("IDS_TUNE_HalfDown");
                return szTemp;
                break;
            case (int)EnumTuningBass5.TUNINGB5_FullDown:
                szTemp = Globals.rm.GetString("IDS_TUNE_FullDown");
                return szTemp;
                break;
            case (int)EnumTuningBass5.TUNINGB5_HalfDownHi:
                szTemp = Globals.rm.GetString("IDS_TUNE_HalfDHigh");
                return szTemp;
                break;
            case (int)EnumTuningBass5.TUNINGB5_FullDownHi:
                szTemp = Globals.rm.GetString("IDS_TUNE_FullDHigh");
                return szTemp;
                break;
            default:
                return null;
                break;
            }*/

            return null;
        }

        string GetTuningTextGuitar(int iIndex)
        {
            string szTemp = null;

            switch(iIndex)
            {
            case (int)EnumTuning.TUNING_Standard:
                return Globals.rm.GetString("IDS_TUN_Standard");
                
            case (int)EnumTuning.TUNING_Drop:
                return Globals.rm.GetString("IDS_TUN_Drop");
                
            case (int)EnumTuning.TUNING_DropD:
                szTemp = Globals.rm.GetString("IDS_TUN_Drop");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_D]);
                break;
            case (int)EnumTuning.TUNING_DropDAlt:
                szTemp = Globals.rm.GetString("IDS_TUN_Drop");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_D]);
                szTemp += ("2");
                break;
            case (int)EnumTuning.TUNING_OpenD:
                szTemp = Globals.rm.GetString("IDS_TUN_Open");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_D]);
                break;
            case (int)EnumTuning.TUNING_OpenDm:
                szTemp = Globals.rm.GetString("IDS_TUN_Open");
                szTemp += (" Dm");//could also pull it from the chords ....
                break;
            case (int)EnumTuning.TUNING_DModal:
                szTemp = Globals.rm.GetString("IDS_TUN_Modal") + m_szRoots[(int)EnumChordRoot.ROOT_D] + " " + szTemp;
                break;
            case (int)EnumTuning.TUNING_Fourths:
                return Globals.rm.GetString("IDS_TUN_Fourths");
                
            case (int)EnumTuning.TUNING_OpenE:
                szTemp = Globals.rm.GetString("IDS_TUN_Open");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_E]);
                break;
            case (int)EnumTuning.TUNING_ShiftedE:
                szTemp = Globals.rm.GetString("IDS_TUN_Shifted");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_E]);
                break;
            case (int)EnumTuning.TUNING_G6:
                szTemp = ("G6");
                break;
            case (int)EnumTuning.TUNING_OpenG:
                szTemp = Globals.rm.GetString("IDS_TUN_Open");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_G]);
                break;
            case (int)EnumTuning.TUNING_Gm:
                szTemp = ("Gm");
                break;
            case (int)EnumTuning.TUNING_OpenA:
                szTemp = Globals.rm.GetString("IDS_TUN_Open");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_A]);
                break;
            case (int)EnumTuning.TUNING_Am:
                szTemp = ("Am");
                break;
			case (int)EnumTuning.TUNING_DropC:
				szTemp = Globals.rm.GetString("IDS_TUN_Drop");
				szTemp += " ";
				szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_C]);
				break;
            case (int)EnumTuning.TUNING_C6:
                szTemp = ("C6");
                break;
            case (int)EnumTuning.TUNING_OpenC:
                szTemp = Globals.rm.GetString("IDS_TUN_Open");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_C]);
                break;
            case (int)EnumTuning.TUNING_LowC:
                szTemp = Globals.rm.GetString("IDS_TUN_Low");
                szTemp += " ";
                szTemp += (m_szRoots[(int)EnumChordRoot.ROOT_C]);
                break;
            default:
                return null;
            }
            return szTemp;
        }

        string GetTuningText(int iIndex, EnumInstrument eInst)
        {
	        if(iIndex < 0 || iIndex > GetNumTuning(eInst))
		        return null;

            if (iIndex == GetNumTuning(eInst))
                return Globals.rm.GetString("IDS_Custom");
	        
            switch(eInst)
	        {
                case EnumInstrument.INST_4StringBass:
		        return GetTuningTextB4(iIndex);
		        
                case EnumInstrument.INST_Lute:
                return GetTuningTextLute(iIndex);
                
                case EnumInstrument.INST_Ukulele:
		        return GetTuningTextUkulele(iIndex);

                case EnumInstrument.INST_UkuleleConcert:
                return GetTuningTextUkulele(iIndex);
		        
        //	case INST_5StringBass:
        //        return GetTuningTextB5(iIndex);
        //		break;
                case EnumInstrument.INST_Banjo:
                return GetTuningTextBanjo(iIndex);
                
                case EnumInstrument.INST_Guitar:
	        default:
                return GetTuningTextGuitar(iIndex);
		    }
        }

		public string GetCustomTuningText()
		{
			return GetCustomTuningText (GetInstrument());	
		}

		public string GetCustomTuningNotesText()
		{
			return GetCustomTuningNotesText(m_eInst);
		}

		public string GetCustomTuningNotesText(EnumInstrument eInst)
		{
			string szTemp = "";

            for (int i = 0; i < GetNumStrings(); i++)
            {
                switch (GetCustomTuning(i))
                {
                    case (int)EnumChordRoot.ROOT_A:
                    case (int)EnumChordRoot.ROOT_B:
                    case (int)EnumChordRoot.ROOT_C:
                    case (int)EnumChordRoot.ROOT_D:
                    case (int)EnumChordRoot.ROOT_E:
                    case (int)EnumChordRoot.ROOT_F:
                    case (int)EnumChordRoot.ROOT_G:
                        szTemp += (m_szRootsPlain[GetCustomTuning(i)]);
                    //    szTemp += " ";
                        break;
                    case (int)EnumChordRoot.ROOT_As:
                    case (int)EnumChordRoot.ROOT_Cs:
                    case (int)EnumChordRoot.ROOT_Ds:
                    case (int)EnumChordRoot.ROOT_Fs:
                    case (int)EnumChordRoot.ROOT_Gs:
                        szTemp += (m_szRootsPlain[GetCustomTuning(i)]);
                        break;
                    default:
                        return null;
                }
            }
			return szTemp;
		}
		
        public string GetCustomTuningText(EnumInstrument eInst)
        {
            string szTemp = Globals.rm.GetString("IDS_Custom") + " (" + GetCustomTuningNotesText(eInst) + ")";
			            
            return szTemp;
        }

        public string GetTuningString()
        {
            return GetTuningText(GetTuning(), m_eInst);
        }

        public string GetTuningString(int iIndex)
        {
            return GetTuningLongText(iIndex, m_eInst);
        }

		public string GetTuningText(int iIndex)
		{
			return GetTuningText(iIndex, m_eInst);
		}

        public void NextTuning()
        {
            //bugbug
        }
        
        public void Init()
        {
            SetScale(EnumScales.SCALE_Major);
            SetTuning((int)m_eTuning);
            GenerateChords(m_eRoot, m_eType);
        }

        //refresh
        public bool GenerateChords()
        {
            return GenerateChords(m_eRoot, m_eType);
        }
        
        public bool GenerateChords(EnumChordRoot eRoot, EnumChordType eType)
        {
            //string szTemp = null;
		//	Debug.Log("GenerateChords("IDS
            
			int iRoot = (int)eRoot;
			int iChord = (int)eType;

            string chord = m_szRootsPlain[iRoot] + m_szChords[iChord];
                
            //    iRoot.ToString() + iChord.ToString();
			
			return GenerateChords(chord);

            
            //oStr->Format(szTemp, ("%s%s"), m_szRoots[eRoot], m_szChords[eType]);
        /*    oStr->Format(szTemp, ("%s%s"), m_szRoots[eRoot], GetChordText((int)eType, m_hInst));

	        return GenerateChords(szTemp);*/
        } 

        
        bool GenerateChords(string szName)
        {
	        m_iChordIndex = 0;

   /*         string buf, buf2;
            buf = "A";
            buf2 = "A";*/

	        
	        // Allocate it for DOS/WINDOWS, to avoid stack overflow (weak)
        //	Fretboard fb;
        //	Chord chord;
        //	chord.findChord(buf);
        //	chord.print(buf2);
        //	fb.iterate(chord);
        //	fb.printStack();
        //	printf("Enter Chord: ("IDS

	        //convert to char
            string szTemp = FixChordString(szName);
  
	        m_oChord.findChord(ref szTemp);
            m_oFretBoard.Reset();
	        m_oFretBoard.iterate(ref m_oChord);
        
	        return true;
        }

		string FixChordString (string chord)
		{
			int index = chord.IndexOf("7sus4");
			if(index != -1)
			{
				chord = chord.Substring(0, index) + "sus47";
			}

			index = chord.IndexOf("9sus4");
			if(index != -1)
			{
				chord = chord.Substring(0, index) + "sus49";
			}
			return chord;

		}

        public void SetChord(EnumChordType eType, bool bReload)
        {
	        if(m_eType == eType && bReload == false)
		        return;

	        m_eType = eType;

	        if(bReload)
		        GenerateChords(m_eRoot, m_eType);
        }

        public EnumScales GetScale()
        {
            return m_eScale;
        }

        public EnumChordRoot GetRoot()
        {
            return m_eRoot;
        }

        public EnumChordType GetChord()
        {
            return m_eType;
        }


        public void SetRoot(EnumChordRoot eRoot, bool bReload)
        {
	        if(m_eRoot == eRoot)
		        return;

	        m_eRoot = eRoot;

            SetScale(m_eScale);//reload cause of root ???

	        if(bReload)
		        GenerateChords(m_eRoot, m_eType);
        }

        public void SetInstrument(EnumInstrument eInst, bool bReload)
        {
            m_eInst = eInst;

            //scale won't change until we set different tunings for diff instruments
            if(bReload)
            {
                SetTuning(GetTuning(), true);//forces the strings to be reset
                GenerateChords(m_eRoot, m_eType);
            }

        }

        public int GetFret(int iString)
        {
            int iRet = m_oFretBoard.bestFrets[m_iChordIndex,iString];
            return iRet;
        }

        public string GetNote(int iString)
        {
            int iRet = m_oFretBoard.bestNotes[m_iChordIndex, iString];
            if (iRet < 0)
                return "";

	        return m_szRoots[m_oFretBoard.bestNotes[m_iChordIndex,iString]];
        }
		
		public string GetNoteFromValue(int iNoteValue)
		{
			while(iNoteValue > 11)
				iNoteValue -= 12;
			
			if(iNoteValue < 0 || iNoteValue > 11)
				return "null";
			
			return szRoots[iNoteValue];
		}

        public int GetNoteValue(int iString, int iFret)
        {
            int iIndex = m_oFretBoard.GetOpenString(iString);
            iIndex += iFret;

            while(iIndex > 11)
                iIndex -= 12;

            return iIndex;
        }

        public int GetStartingFret()
        {
	        int iRet = 100;
	        int iMax = 0;
	
	        for(int i = 0; i < GetNumStrings(); i++)
	        {
                if (m_oFretBoard.bestFrets[m_iChordIndex,i] > 0)
			        iRet = Math.Min(iRet, m_oFretBoard.bestFrets[m_iChordIndex,i]);
                iMax = Math.Max(iMax, m_oFretBoard.bestFrets[m_iChordIndex,i]);
	        }

	        if(iRet == 2)
		        iRet = 1;

	        if(iRet == 3 && iMax <= 5)
		        iRet = 1;

            if(iRet == 100)
                iRet = 1;//good ol Banjo ....

	        return iRet;

        }

        public string GetNoteScale(int iString, int iFret)
        {
            int iIndex = m_oFretBoard.GetOpenString(iString);
            iIndex += iFret;

            while (iIndex > 11)
                iIndex -= 12;

            //now ... if its a sharp or flat index ...
            if(IsWholeNote (iIndex) == false)
            {
                //could be sharp or flat depending on the scale so check btSharpFlat
                if(Globals.Settings.bUseSolfege == true)
				{
					if (m_btShapFlatToNoteIndex[iIndex] == 1) //flat
						return szRootsFlatSolfege[iIndex];
					else if (m_btShapFlatToNoteIndex[iIndex] == 2)
						return szRootsSolfege[iIndex];
				}
				else
				{
					if (m_btShapFlatToNoteIndex[iIndex] == 1) //flat
	                    return szRootsFlats[iIndex];
	                else if (m_btShapFlatToNoteIndex[iIndex] == 2)
	                    return szRoots[iIndex];
				}
            }
            //otherwise by settings   
            return m_szRoots[iIndex];
        }

        public string GetNote(int iString, int iFret)
        {
            int iIndex = m_oFretBoard.GetOpenString(iString);
            iIndex += iFret;

            while(iIndex > 11)
                iIndex -= 12;

            return m_szRoots[iIndex];
        }

        public int GetNoteValue(int iString)
        {
	        if(GetFret(iString) == -1)
		        return -1;//not played

	        return m_oFretBoard.bestNotes[m_iChordIndex,iString];//otherwise return the note
        }


        public void SetLeftHanded(bool bLeftHanded)
        {
	        m_bIsLeftHanded = bLeftHanded;
        }

        void SetTuningB4(int iTuning, bool bReload)
        {
	        m_oFretBoard.SetScoreVars(true);

            switch((EnumTuningBass4)iTuning)
            {
            case EnumTuningBass4.TUNINGB4_Standard:
                m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7, GetNumStrings());
                break;
            case EnumTuningBass4.TUNINGB4_DropD:
                m_oFretBoard.SetOpenStrings( 5, 0, 5, 10, 2, 7, GetNumStrings());
                break;
            case EnumTuningBass4.TUNINGB4_HalfDown:
                m_oFretBoard.SetOpenStrings( 6, 11, 4, 9, 1, 6, GetNumStrings());
                break;
            case EnumTuningBass4.TUNINGB4_LowB:
                m_oFretBoard.SetOpenStrings( 2, 7, 0, 5, 2, 7, GetNumStrings());
                break;
            case EnumTuningBass4.TUNINGB4_DropC:
                m_oFretBoard.SetOpenStrings( 3, 0, 5, 10, 2, 7, GetNumStrings());
                break;
            }
        }

        //incomplete ... CHECK if you want to use it
        //5 string bass typically has a lower B string below the low E ... as of writing I don't have the samples that low
        void SetTuningB5(int iTuning, bool bReload)
        {
	        m_oFretBoard.SetScoreVars(true);

            switch((EnumTuningBass5)iTuning)
            {
            case EnumTuningBass5.TUNINGB5_Standard:
                m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7, GetNumStrings());
                break;
            case EnumTuningBass5.TUNINGB5_DropD:
                m_oFretBoard.SetOpenStrings( 5, 0, 5, 10, 2, 7, GetNumStrings());
                break;
            case EnumTuningBass5.TUNINGB5_HiC:
                m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 3, 7, GetNumStrings());
                break;
            case EnumTuningBass5.TUNINGB5_HalfDown:
                m_oFretBoard.SetOpenStrings( 1, 6, 11, 4, 9, 7, GetNumStrings());
                break;
            case EnumTuningBass5.TUNINGB5_FullDown:
                m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7, GetNumStrings());
                break;
            case EnumTuningBass5.TUNINGB5_HalfDownHi:
                m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7, GetNumStrings());
                break;
            case EnumTuningBass5.TUNINGB5_FullDownHi:
                m_oFretBoard.SetOpenStrings( 7, 0, 5, 10, 2, 7, GetNumStrings());
                break;
            }
        }

        void SetTuningBanjo(int iIndex, bool bReload /* = true */)
        {
            m_oFretBoard.SetScoreVars(false);

            switch(iIndex)
            {
            case (int)EnumTuningBanjo.TUNINGBANJO_G:
                m_oFretBoard.SetOpenStrings( 10, 5, 10, 2, 5, 0, GetNumStrings());
                break;
            case (int)EnumTuningBanjo.TUNINGBANJO_GModal:
                m_oFretBoard.SetOpenStrings( 10, 5, 10, 3, 5, 0, GetNumStrings());
                break;
            case (int)EnumTuningBanjo.TUNINGBANJO_DoubleC:
                m_oFretBoard.SetOpenStrings( 10, 3, 10, 3, 5, 0, GetNumStrings());
                break;
            case (int)EnumTuningBanjo.TUNINGBANJO_C:
                m_oFretBoard.SetOpenStrings( 10, 5, 10, 5, 2, 0, GetNumStrings());
                break;
            case (int)EnumTuningBanjo.TUNINGBANJO_D:
                m_oFretBoard.SetOpenStrings( 5, 0, 9, 0, 5, 0, GetNumStrings());
                break;
            case (int)EnumTuningBanjo.TUNINGBANJO_DAlt:
                m_oFretBoard.SetOpenStrings( 9, 5, 9, 0, 5, 0, GetNumStrings());
                break;
            case (int)EnumTuningBanjo.TUNINGBANJO_Guitar:
                m_oFretBoard.SetOpenStrings( 0, 5, 10, 2, 7, 0, GetNumStrings());
                break;
            }
        }

        void SetTuningUkuleleConcert(int iIndex, bool bReload)
        {
            SetTuningUkulele(iIndex, bReload);
        }

        void SetTuningUkulele(int iIndex, bool bReload /* = true */)
        {
	        m_oFretBoard.SetScoreVars(false);

            switch ((EnumTuningUkulele)iIndex)
	        {
	        case EnumTuningUkulele.TUNINGUKU_RentrantC:
		        m_oFretBoard.SetOpenStrings(10, 3, 7, 0, 0, 0, GetNumStrings());
		        break;
	        case EnumTuningUkulele.TUNINGUKU_C:
		        m_oFretBoard.SetOpenStrings(10, 3, 7, 0, 0, 0, GetNumStrings());
		        break;
	        case EnumTuningUkulele.TUNINGUKU_G:
		        m_oFretBoard.SetOpenStrings(5, 10, 2, 7, 0, 0, GetNumStrings());
		        break;
	        case EnumTuningUkulele.TUNINGUKU_D:
		        m_oFretBoard.SetOpenStrings(0, 5, 9, 2, 0, 0, GetNumStrings());
		        break;
	        case EnumTuningUkulele.TUNINGUKU_B:
		        m_oFretBoard.SetOpenStrings(9, 2, 6, 11, 0, 0, GetNumStrings());
		        break;
	        case EnumTuningUkulele.TUNINGUKU_Cs:
		        m_oFretBoard.SetOpenStrings(11, 4, 8, 1, 0, 0, GetNumStrings());
		        break;
	        case EnumTuningUkulele.TUNINGUKU_Guitar:
		        m_oFretBoard.SetOpenStrings(5, 10, 2, 7, 0, 0, GetNumStrings());
		        break;
	        case EnumTuningUkulele.TUNINGUKU_SlackKey:
		        m_oFretBoard.SetOpenStrings(10, 3, 7, 10, 0, 0, GetNumStrings());
		        break;
	        }
        }

        void SetTuningLute(int iIndex, bool bReload /* = true */)
        {
	        m_oFretBoard.SetScoreVars(false);

            switch((EnumTuningLute)iIndex)
            {
            case EnumTuningLute.TUNINGLUTE_Mandolin:   //G D A E
                m_oFretBoard.SetOpenStrings(10, 5, 0, 7, 2, 7, GetNumStrings());
                break;
            case EnumTuningLute.TUNINGLUTE_Mandola:     //C G D A
                m_oFretBoard.SetOpenStrings( 3, 10, 5, 0, 2, 7, GetNumStrings());
                break;
            case EnumTuningLute.TUNINGLUTE_Pipa:        //D A D A
                m_oFretBoard.SetOpenStrings( 5, 0, 5, 0, 2, 7, GetNumStrings());
                break;
            case EnumTuningLute.TUNINGLUTE_Bouzouki:    //C F A D
                m_oFretBoard.SetOpenStrings( 3, 8, 0, 5, 2, 7, GetNumStrings());
                break;
            case EnumTuningLute.TUNINGLUTE_Bouzouki2:   //G D A D
                m_oFretBoard.SetOpenStrings(10, 5, 0, 5, 2, 7, GetNumStrings());
                break;
            case EnumTuningLute.TUNINGLUTE_Bouzouki3:   //A D E A
                m_oFretBoard.SetOpenStrings( 0, 5, 7, 0, 2, 7, GetNumStrings());
                break;
            }
        }

        void SetCustomTuning(EnumInstrument eInst, bool bReload)
        {
            m_oFretBoard.SetOpenStrings(arrCustomTuning[(int)eInst, 0],
                                                  arrCustomTuning[(int)eInst, 1],
                                                  arrCustomTuning[(int)eInst, 2],
                                                  arrCustomTuning[(int)eInst, 3],
                                                  arrCustomTuning[(int)eInst, 4],
                                                  arrCustomTuning[(int)eInst, 5],
                                                  GetNumStrings());
            
            if (bReload)
            {
                GenerateChords(m_eRoot, m_eType);
            }

            switch (eInst)
            {
                case EnumInstrument.INST_Banjo:
                    m_eTuningBanjo = EnumTuningBanjo.TUNINGBANJO_Count;
                    break;
                case EnumInstrument.INST_4StringBass:
                    m_eTuningB4 = EnumTuningBass4.TUNINGB4_Count;
                    break;
                case EnumInstrument.INST_Guitar:
                    m_eTuning = EnumTuning.TUNING_Count;
                    break;
                case EnumInstrument.INST_Lute:
                    m_eTuningLute = EnumTuningLute.TUNINGLUTE_Count;
                    break;
                case EnumInstrument.INST_Ukulele:
                    m_eTuningUkulele = EnumTuningUkulele.TUNINGUKU_Count;
                    break;
                case EnumInstrument.INST_UkuleleConcert:
                    m_eTuningUkuleleConcert = EnumTuningUkulele.TUNINGUKU_Count;
                    break;
            }
        }

        public void SetTuning(EnumInstrument eInst, int iTuning, bool bReload)
        {   //custom tuning
            if(iTuning == GetNumTuning(eInst))
            {
                SetCustomTuning(eInst, bReload);
                return;
            }

            switch (eInst)
            {
                case EnumInstrument.INST_4StringBass:
                    SetTuningB4(iTuning, bReload);
                    m_eTuningB4 = (EnumTuningBass4)iTuning;
                    if (bReload)
                        GenerateChords(m_eRoot, m_eType);
                    return;

                //    case INST_5StringBass:
                //        SetTuningB5(iTuning, bReload);
                //        return;
                //        break;
                case EnumInstrument.INST_Banjo:
                    SetTuningBanjo(iTuning, bReload);
                    m_eTuningBanjo = (EnumTuningBanjo)iTuning;
                    if (bReload)
                        GenerateChords(m_eRoot, m_eType);
                    return;
                case EnumInstrument.INST_Lute:
                    SetTuningLute(iTuning, bReload);
                    m_eTuningLute = (EnumTuningLute)iTuning;
                    if (bReload)
                        GenerateChords(m_eRoot, m_eType);
                    return;
                case EnumInstrument.INST_Ukulele:
                    SetTuningUkulele(iTuning, bReload);
                    m_eTuningUkulele = (EnumTuningUkulele)iTuning;
                    if (bReload)
                        GenerateChords(m_eRoot, m_eType);
                    return;
                case EnumInstrument.INST_UkuleleConcert:
                    SetTuningUkuleleConcert(iTuning, bReload);
                    m_eTuningUkuleleConcert = (EnumTuningUkulele)iTuning;
                    if (bReload)
                        GenerateChords(m_eRoot, m_eType);
                    return;
            }

            m_oFretBoard.SetScoreVars(false);

            m_eTuning = (EnumTuning)iTuning;

            switch (m_eTuning)
            {
                case EnumTuning.TUNING_Standard:
                    m_oFretBoard.SetOpenStrings(7, 0, 5, 10, 2, 7, GetNumStrings());
                    break;
                case EnumTuning.TUNING_Drop:
                    m_oFretBoard.SetOpenStrings(6, 11, 4, 9, 1, 6, GetNumStrings());
                    break;
                case EnumTuning.TUNING_DropD:
                    m_oFretBoard.SetOpenStrings(5, 0, 5, 10, 2, 7, GetNumStrings());
                    break;
                case EnumTuning.TUNING_DropDAlt:
                    m_oFretBoard.SetOpenStrings(5, 0, 5, 10, 2, 5, GetNumStrings());
                    break;
                case EnumTuning.TUNING_OpenD:
                    m_oFretBoard.SetOpenStrings(5, 0, 5, 9, 0, 5, GetNumStrings());
                    break;

                case EnumTuning.TUNING_OpenDm://D A D F A D
                    m_oFretBoard.SetOpenStrings(5, 0, 5, 8, 0, 5, GetNumStrings());
                    break;

				case EnumTuning.TUNING_DModal:
                    m_oFretBoard.SetOpenStrings(5, 0, 5, 10, 0, 5, GetNumStrings());
                    break;
                case EnumTuning.TUNING_Fourths:
                    m_oFretBoard.SetOpenStrings(7, 0, 5, 10, 3, 8, GetNumStrings());
                    break;
                case EnumTuning.TUNING_OpenE:
                    m_oFretBoard.SetOpenStrings(7, 2, 7, 11, 2, 7, GetNumStrings());
                    break;
                case EnumTuning.TUNING_ShiftedE:
                    m_oFretBoard.SetOpenStrings(7, 11, 4, 9, 2, 7, GetNumStrings());
                    break;


                case EnumTuning.TUNING_G6:
                    m_oFretBoard.SetOpenStrings(5, 10, 5, 10, 2, 7, GetNumStrings());
                    break;
                case EnumTuning.TUNING_OpenG:
                    m_oFretBoard.SetOpenStrings(5, 10, 5, 10, 2, 5, GetNumStrings());
                    break;
                case EnumTuning.TUNING_Gm:
                    m_oFretBoard.SetOpenStrings(5, 10, 5, 10, 1, 5, GetNumStrings());
                    break;

                case EnumTuning.TUNING_OpenA:
                    m_oFretBoard.SetOpenStrings(7, 0, 7, 0, 4, 7, GetNumStrings());
                    break;
                case EnumTuning.TUNING_Am:
                    m_oFretBoard.SetOpenStrings(7, 0, 7, 0, 3, 7, GetNumStrings());
                    break;

				case EnumTuning.TUNING_DropC:
					m_oFretBoard.SetOpenStrings(3, 10, 3, 8, 0, 5, GetNumStrings());
					break;
                case EnumTuning.TUNING_C6:
                    m_oFretBoard.SetOpenStrings(3, 10, 3, 10, 0, 7, GetNumStrings());
                    break;
                case EnumTuning.TUNING_OpenC:
                    m_oFretBoard.SetOpenStrings(3, 10, 3, 10, 3, 7, GetNumStrings());
                    break;
                case EnumTuning.TUNING_LowC:
                    m_oFretBoard.SetOpenStrings(3, 10, 5, 10, 0, 5, GetNumStrings());
                    break;

                //	case (int)EnumTuning.TUNING_Lute:
                //		m_oFretBoard.SetOpenStrings( 7, 11, 4, 9, 2, 7, GetNumStrings());
                //		break;
                default:
                    return;
            }

            if (bReload)
                GenerateChords(m_eRoot, m_eType);
        }

        
        //void SetTuning(EnumTuning eTuning, bool bReload)
        public void SetTuning(int iTuning, bool bReload = true)
        {
            SetTuning(GetInstrument(), iTuning, bReload);
        }

        public int GetOpenNote(int iString)
        {
	        if(iString < 0 || iString > GetNumStrings())
		        return -1;

	        return m_oFretBoard.GetOpenString(iString);
        }

        public string GetOpenNoteString(int iString)
        {
            if (iString < 0 || iString > GetNumStrings())
                return "";

            int iRet = m_oFretBoard.GetOpenString(iString);
            if (iRet < 0)
                return "";

            return m_szRoots[iRet];
        }
        

        public void SetScale(EnumScales eScale)
        {
            m_eScale = eScale;
    
            for (int i = 0; i < SCALE_SIZE; i++)
            {
                m_btCurrentScale[i] = (byte)NOTE_NOT_DEF;//max value 
                m_btSharpFlatScale[i] = (byte)NOTE_NOT_DEF;
                m_btShapFlatToNoteIndex[i] = (byte)NOTE_NOT_DEF;
            }

            //("1,2,3,4,5,6,7")
            //("1,2,b3,4,5,b6,7")
            string szCode = m_szScalesCode[(int)m_eScale];
        
            int iIndex = 0;//index for string
            int iNote = 0;//note from array
            int iLen = 0;//index in bt array

            bool bFlat;
            bool bSharp;
  
            //I'm not babysitting ... better be formatted correctly
            while(iIndex < szCode.Length)
            {
                if(szCode[iIndex] != ',')
                {
                    bFlat = false;
                    bSharp = false;

                    if(szCode[iIndex] == 'b')
                    {
                        bFlat = true;
                        iIndex++;
                    }
                    else if(szCode[iIndex] == '#')
                    {
                        bSharp = true;
                        iIndex++;
                    }
                    iNote = szCode[iIndex] - '0';

                    //now note to step
                    m_btCurrentScale[iLen] = ScaleToStep(iNote, bSharp, bFlat);

                    if (bSharp == true) m_btSharpFlatScale[iLen] = 2;
                    else if (bFlat == true) m_btSharpFlatScale[iLen] = 1;
                    else m_btSharpFlatScale[iLen] = 0;

                    iLen++;
                }
                iIndex++;
            }
            //now set m_btSharpFlatToIndex ... for the notes A = 0, etc
            for (int i = 0; i < 12; i++)
            {
                int temp = i + (int)m_eRoot;
                if (temp >= 12)
                    temp -= 12;

                if (IsWholeNote(i) == true)
                {
                    m_btShapFlatToNoteIndex[i] = 0;//whole note
                }
                else if (GetScaleIndexFromBTArrays(i - (int)m_eRoot) == -1)
                {   //its not in the scale and its not a whole note just set it to sharp
                     m_btShapFlatToNoteIndex[i] = 2;
                }
                else //its in the scale and not a whole note ... calculation time
                {
                    //are we sharp or flat??
                    m_btShapFlatToNoteIndex[i] = m_btSharpFlatScale[GetScaleIndexFromBTArrays(i - (int)m_eRoot)];



                    //is it sharp or flat?
                    /*  for (int j = 0; j < SCALE_SIZE; j++)
                      {
                          if (m_btSharpFlatScale[j] != 0 && m_btSharpFlatScale[j] != NOTE_NOT_DEF)
                          {
                              m_btShapFlatToNoteIndex[i] = m_btSharpFlatScale[j];
                              break;
                          }
                      }*/
                }
            }

            //int k = 9;//for a break point
        }

        //gets the location in btCurScale (if its defined)
        int GetScaleIndexFromBTArrays(int iStep) //0 to 12
        {
            if (iStep >= 12)
                iStep -= 12;
            if (iStep < 0)
                iStep += 12;

            for (int i = 0; i < SCALE_SIZE; i++)
            {
                if (m_btCurrentScale[i] == iStep)
                    return i;

            }
            return -1;
        }

        bool IsWholeNote(int iNote)
        {
            if (iNote >= 12)
                iNote -= 12;

             switch (iNote)
            {
                case 1:
                case 4:
                case 6:
                case 9:
                case 11:
                    return false;
            }
             return true;

        }


        byte ScaleToStep(int iScaleNote, bool bSharp, bool bFlat)
        {
            //most are whole steps so double then correct by two zero base
            byte btRet = (byte)(iScaleNote * 2 - 2);

            //now correct for the half step
            if(iScaleNote > 3)
                btRet--;
            if(bSharp)
                btRet++;
            else if(bFlat)
                btRet--;
 
            return btRet;
        }

        public bool OnFavorites()
        {
         /*   if(m_oFav.AddFavorite(m_eRoot, m_eType, m_eTuning, m_iChordIndex, 
                GetNoteValue(0), GetNoteRes(1), GetNoteRes(2), GetNoteRes(3), GetNoteRes(4), GetNoteRes(5)) == false)
            {   //then we delete it
                m_oFav.DeleteLastFav();
            }*/
            return false;
        }

        public bool IsStarLit()
        {
            return false; // m_oFav.IsFavorite(m_eRoot, m_eType, m_eTuning, m_iChordIndex);
        }

        public int GetNoteRes(int iString)
        {
            int iNote = GetNoteValue(iString);
            if(iNote == -1)
                return -1;

            int iRet = GetNoteRes(iString, GetFret(iString));
            return iRet;
        }

        //we'll also use this function to correct the uku standard tuning
        public int RaiseNote(int iString)
        {
            switch(m_eInst)
            {
            case EnumInstrument.INST_Banjo:
                if(iString == 0)
                    return 2;
                if(iString == 2)
                    return 1;
                break;
            case EnumInstrument.INST_UkuleleConcert:
                    switch(m_eTuningUkulele)
                {
                case EnumTuningUkulele.TUNINGUKU_C:
                    return 0;
                case EnumTuningUkulele.TUNINGUKU_D:
                    return 0;
                case EnumTuningUkulele.TUNINGUKU_Guitar:
                    return 0;
                    
                }
                if(iString == 0)
                    return 1;
                return 0;
            case EnumInstrument.INST_Ukulele:
                switch(m_eTuningUkulele)
                {
                case EnumTuningUkulele.TUNINGUKU_C:
                    return 1;
                case EnumTuningUkulele.TUNINGUKU_D:
                    return 1;
                case EnumTuningUkulele.TUNINGUKU_Guitar:
                    return 1;
                    
                }
                if(iString == 0)
                    return 2;
                return 1;
            default:
                return 0;
            }

            return 0;
        }



        public int GetNoteRes(int iString, int iFret)
        {
            //lowest note is a C (perfect)
            int AMid = 21; //from Unity Array
            int AHi = 33;
            int HIGHEST_SOUND_RES = 56;
        

            int uiRet = -1;

            int iNote = GetOpenNote(iString);

            switch(iString)
            {
            case 0:
                uiRet = iNote + iFret + AMid - 12; //A Mid
                break;
            case 1:
            case 2:
                uiRet = iNote + iFret + AMid;//A Md
                if(iNote > 9)
                    uiRet -= 12;
				if(iString == 2 && m_eInst == EnumInstrument.INST_Lute && m_eTuningLute == EnumTuningLute.TUNINGLUTE_Mandolin)
					uiRet += 12;//hack 
                break;
            case 3:
            case 4:	//should be good for both the G & B strings 
                uiRet = iNote + iFret + AMid;
                if(iNote < 8)//assume its been tuned up
                    uiRet += 12;
                break;
            case 5:
                uiRet = iNote + iFret + AHi;
                break;
            }

            //drop an octave for bass guitars
            if(m_eInst == EnumInstrument.INST_4StringBass)// || m_eInst == INST_5StringBass)
                uiRet -= 12;
            else
                uiRet += RaiseNote(iString)*12;

            //now check ranges
            while(uiRet < 0)
                uiRet += 12;
            while(uiRet > HIGHEST_SOUND_RES)
                uiRet -= 12;

                 return uiRet;
        }


        public int GetScaleLength()
        {
            for(int i = 0; i < SCALE_SIZE; i++)
            {
                if(m_btCurrentScale[i] == NOTE_NOT_DEF)
                    return i;
            }
            return SCALE_SIZE;
        }

        public int GetNumStrings()
        {
            switch(m_eInst)
            {
            case EnumInstrument.INST_Guitar:
                return 6;
            case EnumInstrument.INST_4StringBass:
                return 4;
            case EnumInstrument.INST_Lute:
                return 4;
            case EnumInstrument.INST_UkuleleConcert:
            case EnumInstrument.INST_Ukulele:
                return 4;
        //    case INST_5StringBass:
            case EnumInstrument.INST_Banjo:
                return 5;
            default: 
                break;
            }
            return 6;
        }


        public int GetTuning(EnumInstrument eInst)
        {
            switch (m_eInst)
            {
                case EnumInstrument.INST_4StringBass:
                    if ((int)m_eTuningB4 > GetNumTuning(m_eInst))
                        SetTuning(0, true);
                    return (int)m_eTuningB4;
                case EnumInstrument.INST_Lute:
                    if ((int)m_eTuningLute > GetNumTuning(m_eInst))
                        SetTuning(0, true);
                    return (int)m_eTuningLute;
                case EnumInstrument.INST_Banjo:
                    if ((int)m_eTuningB4 > GetNumTuning(m_eInst))
                        SetTuning(0, true);
                    return (int)m_eTuningBanjo;
                case EnumInstrument.INST_Ukulele:
                    if ((int)m_eTuningUkulele > GetNumTuning(m_eInst))
                        SetTuning(0, true);
                    return (int)m_eTuningUkulele;
                case EnumInstrument.INST_UkuleleConcert:
                    if ((int)m_eTuningUkuleleConcert > GetNumTuning(m_eInst))
                        SetTuning(0, true);
                    return (int)m_eTuningUkuleleConcert;

                //	case INST_5StringBass:
                //		break;
                case EnumInstrument.INST_Guitar:
                    return (int)m_eTuning;
            }

            return 0;
        }

        public int GetTuning()
        {
            return GetTuning(m_eInst);
        }

        public int GetIndex()
        {
            return m_iChordIndex;
        }
		
		public int GetNumChordTypes()
		{
			return (int)EnumChordType.CHORD_Count;	
		}

        public int GetNumTuning()
        {
            return GetNumTuning(m_eInst);
        }

        public int GetNumScales()
        {
            return (int)EnumScales.SCALE_Count;
        }

        public int GetNumTuning(EnumInstrument eInst)
        {
            switch (eInst)
            {
                case EnumInstrument.INST_4StringBass:
                    return (int)EnumTuningBass4.TUNINGB4_Count;
                case EnumInstrument.INST_Lute:
                    return (int)EnumTuningLute.TUNINGLUTE_Count;
                case EnumInstrument.INST_Banjo:
                    return (int)EnumTuningBanjo.TUNINGBANJO_Count;
                case EnumInstrument.INST_Ukulele:
                case EnumInstrument.INST_UkuleleConcert:
                    return (int)EnumTuningUkulele.TUNINGUKU_Count;
                //	case INST_5StringBass:
                //		return (int)TUNINGB5_Count;
                //		break;
                case EnumInstrument.INST_Guitar:
                default:
                    return (int)EnumTuning.TUNING_Count;

            }
        }

        public byte GetScaleValue(int iIndex)
        {
            return m_btCurrentScale[iIndex];
        }

        //for scales
        public bool GetScaleIsFlat(int iIndex)
        {
            if (m_btSharpFlatScale[iIndex] == 1)
                return true;
            return false;
        }

        public double GetNoteFreq(int iNote, int iOctave)
        {
          //  double n = iOctave - 4;
          //  double freq = Math.Pow(2.0, n / 12.0) * 440.0;

            //offset to c  +  octave offset + note   to find the relative key on the piano ... good enough 
            //offset is negative because pianos start on a (+2) but at the sub octave (-12)
            double n = -11.0 + iOctave * 12 + iNote;
            if (iNote < 5) n += 12;
            double n12 = Math.Pow(2.0, 1.0/12.0);
            double freq = 440.0*Math.Pow(n12, n - 49);

            return freq;
        }

        //C4 261.63 (Middle C) which is c-hi.wav ... at the moment ...
        //c-vlow.wav is then is octave 1 ....
        public double GetFretFreq(int iString, int iFret)
        {
            int iRes = GetNoteRes(iString, iFret);

            int iNote = GetNoteValue(iString, iFret);

            //lowest note is a C (perfect)
            int iOctave = (iRes + 12) / 12;

            return GetNoteFreq(iNote, iOctave);
        }

        public double GetStringFreq(int iString)
        {
            return GetFretFreq(iString, 0);
        }

        public double GetMinStringFreq(int iString)
        {
            int iRes = GetNoteRes(iString, 0);
            int iNote = GetNoteValue(iString, 0);

            if (iRes > 0)
            {
                iRes--;
                iNote--;
            }
            
            if (iNote < 0)
                iNote += 12;

            //lowest note is a C (perfect)
            int iOctave = (iRes + 12) / 12;

            return GetNoteFreq(iNote, iOctave);

        }

        public double GetMaxStringFreq(int iString)
        {
            int iRes = GetNoteRes(iString, 0);
            int iNote = GetNoteValue(iString, 0);

            iRes++;
            iNote++;
            
            if (iNote >= 12)
                iNote -= 12;

            //lowest note is a C (perfect)
            int iOctave = (iRes + 12) / 12;

            return GetNoteFreq(iNote, iOctave);
        }

        public int GetCustomTuning(int iString)
        {
            if (iString < 0)
                return -1;
            if (iString >= GetNumStrings())
                return -1;

            return arrCustomTuning[(int)m_eInst,iString];
        }

        public bool SetCustomTuning(int iString, int iNote)
        {
            if (iString < 0)
                return false;
            if (iString >= GetNumStrings())
                return false;

            arrCustomTuning[(int)m_eInst, iString] = iNote;

            return true;

        }

        public bool UpdateCustomTuning()
        {
            if (GetTuning() != GetNumTuning())
                return false;

            SetCustomTuning(m_eInst, true);
            return true;
        }
    }

	public class RootLabels
	{
		public enum AccidentalsType
		{
			Sharp,
			Flat, 
			Both,
		}

		public RootLabels()
		{


		}

		static string[] szRootsPlain = new string[]
		{
			("A"), ("A#"), ("B"), ("C"), ("C#"), ("D"),
			("D#"), ("E"), ("F"), ("F#"), ("G"), ("G#")
		};

		static string[] szRootsPlainFlats = new string[]
		{
			("A"), ("Bb"), ("B"), ("C"), ("Db"), ("D"),
			("Eb"), ("E"), ("F"), ("Gb"), ("G"), ("Ab")
		};

		static string [] szRootsSolfege = new string[] 
		{
			("La"), ("La#"), ("Si"), ("Do"), ("Do#"), ("Re"),
			("Re#"), ("Mi"), ("Fa"), ("Fa#"), ("Sol"), ("Sol#")
		};

		static string [] szRootsSolfegeFlats = new string[] 
		{
			("La"), ("Lab"), ("Si"), ("Do"), ("Dob"), ("Re"),
			("Reb"), ("Mi"), ("Fa"), ("Fab"), ("Sol"), ("Solb")
		};

		public static string GetRoot(int iIndex, bool bUseSolfage, bool bSimpleChars, AccidentalsType eAcc)
		{
			//get in range
			while (iIndex < 0)
				iIndex += 12;
			while (iIndex > 11)
				iIndex -= 12;

			//first let's get the simple version
			string root;
			if (bUseSolfage == true)
				root = szRootsSolfege [iIndex];
			else 
				root = szRootsPlain[iIndex];

			//now let's change or add flats if needed
			if(eAcc == AccidentalsType.Flat)
			{
				if (IsSharpOrFlat (iIndex) == true) 
				{
					if (bUseSolfage == true)
						root = szRootsSolfege [iIndex];
					else
						root = szRootsPlainFlats [iIndex];
				}
			}
			else if (eAcc == AccidentalsType.Both && IsSharpOrFlat(iIndex) == true)
			{
				if(bUseSolfage == true)
					root += " / " + szRootsSolfegeFlats [iIndex];
				else
					root += " / " + szRootsPlainFlats [iIndex];
			}

			//now let's swap simple chars for good ones
			if (bSimpleChars == false) 
			{
				root = root.Replace ("#", "#");
				root = root.Replace ("b", "♭");
			}

			return root;
		}

		public static bool IsSharpOrFlat(int iIndex)
		{
			switch (iIndex) 
			{
			case 1:
			case 4:
			case 9:
			case 11:
				return true;
			}
			return false;
		}
	}
}
