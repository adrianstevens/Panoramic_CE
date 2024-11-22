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
using System.IO.IsolatedStorage;
using System.ComponentModel;
using System.Collections.Generic;
using System.Xml.Serialization;
using System.IO;

namespace GuitarSuite
{
    public enum EnumScreens
    {
        Scales,
        Play,
        TunerEar,
        GuitarChords,
        GuitarTuner,
        Metronome,
        Settings,
        Music,
    };

    public enum EnumSkin
    {
        Rosewood,
        Alder,
        Rebel,
        Fire,
        SonicBlue,
        Carbon,
        Lava,
        PinkBubbles,
        Leopard,
        Zebra,
        Eleven,
        Count,
    };

    public enum EnumFretIndicators
    {
        Roman,
        Numbered,
        Count,
    };

    public enum EnumStrumMode
    {
        Fast,
        Slow,
        Off, //so you can strum yourself
        Count,
    };

    public enum EnumMetronome
    {
        Wood,
        Metal,
        Synth,
        Old,
        Silence,
        Count,
    };

    public class AppSettings
    {
        public EnumScreens eCurrentScreen;
        public EnumSkin eSkin;
        public EnumFretIndicators eFretIndicators;
        public bool bLeftHanded = false;
        public bool bShowFlats = false;
        
        public EnumStrumMode eStrumMode;
        public EnumMetronome eMetronomeSounds;
        public int iBpm;
        public int iMeter;

        public bool bTuneGraphSpec = false;

        public bool bUseSolfege = false;



        public TunerSensitivity eSensitivity = TunerSensitivity.normal;
        public FrequencyRange eTunerRage = FrequencyRange.normal;
        
        public EnumInstrument eInstrument
        {
            get
            {
                return Globals.myGen.m_eInst;
            }
            set
            {
                Globals.myGen.SetInstrument((EnumInstrument)value, true);
            }
        }

        public EnumInstrumentSound eSound
        {
            get
            {
                return Globals.myGen.m_eSound;
            }
            set
            {
                Globals.myGen.m_eSound = value;
            }

        }

        public EnumChordRoot eRoot
        {
            get { return Globals.myGen.GetRoot(); }
            set { Globals.myGen.SetRoot((EnumChordRoot)value, true); }
        }

        public EnumChordType     eChord
        {
            get { return Globals.myGen.GetChord(); }
            set { Globals.myGen.SetChord((EnumChordType)value, true); }
        }

        public EnumScales        eScale
        {
            get { return Globals.myGen.GetScale(); }
            set { Globals.myGen.SetScale((EnumScales)value);}
        }

        public EnumTuning        eTuning
        {
            get { return (EnumTuning)Globals.myGen.GetTuning(EnumInstrument.INST_Guitar); }
            set { Globals.myGen.SetTuning(EnumInstrument.INST_Guitar, (int)value, false); }
        }

        public EnumTuningBanjo   eTuningBanjo
        {
            get { return (EnumTuningBanjo)Globals.myGen.GetTuning(EnumInstrument.INST_Banjo); }
            set { Globals.myGen.SetTuning(EnumInstrument.INST_Banjo, (int)value, false); }
        }

        public EnumTuningBass4   eTuningBass
        {
            get { return (EnumTuningBass4)Globals.myGen.GetTuning(EnumInstrument.INST_4StringBass); }
            set { Globals.myGen.SetTuning(EnumInstrument.INST_4StringBass, (int)value, false); }
        }
        
        public EnumTuningBass5   eTuningBass5
        {
            get { return (EnumTuningBass5)0;}// Globals.myGen.GetTuning(EnumInstrument.INST_Guitar); }
            set { return; } //Globals.myGen.SetTuning(EnumInstrument.INST_Guitar, (int)value, false); }
        }

        public EnumTuningLute    eTuningLute
        {
            get { return (EnumTuningLute)Globals.myGen.GetTuning(EnumInstrument.INST_Lute); }
            set { Globals.myGen.SetTuning(EnumInstrument.INST_Lute, (int)value, false); }
        }

        public EnumTuningUkulele eTuningUkulele
        {
            get { return (EnumTuningUkulele)Globals.myGen.GetTuning(EnumInstrument.INST_Ukulele); }
            set { Globals.myGen.SetTuning(EnumInstrument.INST_Ukulele, (int)value, false); }
        }

        public EnumTuningUkulele eTuningUkuleleConcert
        {
            get { return (EnumTuningUkulele)Globals.myGen.GetTuning(EnumInstrument.INST_UkuleleConcert); }
            set { Globals.myGen.SetTuning(EnumInstrument.INST_UkuleleConcert, (int)value, false); }
        }


        
        private const string REG_CurrentScreen = "current";
        
        private const string REG_Skin = "skin";
        private const string REG_FretIndicators= "fretindicators";
        private const string REG_LeftHanded = "LeftHanded";
        private const string REG_ShowFlats = "ShowFlats";
        private const string REG_Instrument = "instrument";
        private const string REG_Sound = "sound";
        private const string REG_ChordStrumMode = "chordstrummode";
        private const string REG_MetronomeSound = "metronomesound";
        private const string REG_Bpm = "bpm";
        private const string REG_Meter = "meter";

        private const string REG_TuneGraphSpec = "tuneGraph";

        private const string REG_Root = "root";
        private const string REG_Chord = "chord";
        private const string REG_Scale = "scale";
        
        private const string REG_Tuning = "tuning";
        private const string REG_TuningBass = "tuningBass";
        private const string REG_TuningBass5 = "tuningBass5";
        private const string REG_TuningBanjo = "tuningBanjo";
        private const string REG_TuningLute = "tuningLute";
        private const string REG_TuningUkulele = "tuningUkulele";
        private const string REG_TuningUkuleleConcert = "tuningUkuleleConcert";

        private const string REG_Custom = "custom";

        private const string REG_TunerSensitivity = "tuneSense";
        private const string REG_TunerRange = "tuneRange";

        public AppSettings()
        {
            // set defaults
            eCurrentScreen = EnumScreens.GuitarChords;
            eSkin = EnumSkin.Rosewood;
            eFretIndicators = EnumFretIndicators.Roman;
            bLeftHanded = false;
            bShowFlats = false;
            eInstrument = EnumInstrument.INST_Guitar;
            eSound = EnumInstrumentSound.Accoustic;
            eStrumMode = EnumStrumMode.Slow;
            eMetronomeSounds = EnumMetronome.Wood;
            iBpm = 120;
            iMeter = 4;

            //eSensitivity = TunerSensitivity.normal;
            //eTunerRage = FrequencyRange.normal;

            
            eRoot = EnumChordRoot.ROOT_C;
            eChord = EnumChordType.CHORD_maj;
            eScale = EnumScales.SCALE_Major;

            eTuning = EnumTuning.TUNING_Standard;
            eTuningBanjo = EnumTuningBanjo.TUNINGBANJO_D;
            eTuningBass = EnumTuningBass4.TUNINGB4_Standard;
            eTuningBass5 = EnumTuningBass5.TUNINGB5_Standard;
            eTuningLute = EnumTuningLute.TUNINGLUTE_Mandolin;
            eTuningUkulele = EnumTuningUkulele.TUNINGUKU_C;
            eTuningUkuleleConcert = EnumTuningUkulele.TUNINGUKU_C;
        
            bTuneGraphSpec = false;

            Read();
        }

        public void Read()
        {
            try
            {
                List<int> arrValues = null;

                XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                TextReader reader = new StreamReader("GuitarSettings.xml");

                int iIndex = 0;

                arrValues = (List<int>)ser.Deserialize(reader);

                eCurrentScreen = (EnumScreens)arrValues[iIndex++];
                eSkin = (EnumSkin)arrValues[iIndex++];
                eFretIndicators = (EnumFretIndicators)arrValues[iIndex++];
                bLeftHanded = (arrValues[iIndex++] == 1) ? true : false;
                bShowFlats = (arrValues[iIndex++] == 1) ? true : false;
                eInstrument = (EnumInstrument)arrValues[iIndex++];
                eSound = (EnumInstrumentSound)arrValues[iIndex++];
                eStrumMode = (EnumStrumMode)arrValues[iIndex++];
                eMetronomeSounds = (EnumMetronome)arrValues[iIndex++];
                iBpm = arrValues[iIndex++];
                iMeter = arrValues[iIndex++];

                bTuneGraphSpec = (arrValues[iIndex++] == 1) ? true : false;
                eTunerRage = (FrequencyRange)arrValues[iIndex++];
                eSensitivity = (TunerSensitivity)arrValues[iIndex++];

                //chord/root/scale/tunings
                eRoot = (EnumChordRoot)arrValues[iIndex++];
                eChord = (EnumChordType)arrValues[iIndex++];
                eScale = (EnumScales)arrValues[iIndex++];
                eTuning = (EnumTuning)arrValues[iIndex++];
                eTuningBanjo = (EnumTuningBanjo)arrValues[iIndex++];
                eTuningBass = (EnumTuningBass4)arrValues[iIndex++];
                eTuningBass5 = (EnumTuningBass5)arrValues[iIndex++];
                eTuningLute = (EnumTuningLute)arrValues[iIndex++];
                eTuningUkulele = (EnumTuningUkulele)arrValues[iIndex++];
                eTuningUkuleleConcert = (EnumTuningUkulele)arrValues[iIndex++];

                reader.Close();
            }
            catch
            {
            }
        }

        public void Save()
        {
            List<int> arrValues = new List<int>();

            arrValues.Add((int)eCurrentScreen);
            arrValues.Add((int)eSkin);
            arrValues.Add((int)eFretIndicators);
            arrValues.Add(bLeftHanded?1:0);
            arrValues.Add(bShowFlats?1:0);
            arrValues.Add((int)eInstrument);
            arrValues.Add((int)eSound);
            arrValues.Add((int)eStrumMode);
            arrValues.Add((int)eMetronomeSounds);
            arrValues.Add(iBpm);
            arrValues.Add(iMeter);

            arrValues.Add(bTuneGraphSpec?1:0);
            arrValues.Add((int)eTunerRage);
            arrValues.Add((int)eSensitivity);
            
            //chord/root/scale/tunings
            arrValues.Add((int)eRoot);
            arrValues.Add((int)eChord);
            arrValues.Add((int)eScale);
            arrValues.Add((int)eTuning);
            arrValues.Add((int)eTuningBanjo);
            arrValues.Add((int)eTuningBass);
            arrValues.Add((int)eTuningBass5);
            arrValues.Add((int)eTuningLute);
            arrValues.Add((int)eTuningUkulele);
            arrValues.Add((int)eTuningUkuleleConcert);

            try
            {
                XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                TextWriter writer = new StreamWriter("GuitarSettings.xml");
                ser.Serialize(writer, arrValues);
                writer.Close();
            }
            catch (System.Exception ex)
            {
            }


            //custom tuning 
            //IsolatedStorageSettings.ApplicationSettings.Add(REG_Custom, Globals.myGen.arrCustomTuning);
            /*
            for (int i = 0; i < (int)EnumInstrument.INST_count; i++)
            {
                for (int j = 0; j < 6; j++)
                {
                    IsolatedStorageSettings.ApplicationSettings.Add(REG_Custom+i+j, Globals.myGen.arrCustomTuning[i,j]);
                }
            }
            
            IsolatedStorageSettings.ApplicationSettings.Save();
             
             */ 
        }
    }
}

