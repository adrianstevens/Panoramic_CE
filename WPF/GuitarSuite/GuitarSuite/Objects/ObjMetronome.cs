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
using System.Windows.Threading;
using System.ComponentModel;
using System.Windows.Resources;
using System.IO;
using System.IO.IsolatedStorage;

namespace GuitarSuite
{
    public class ObjMetronome : INotifyPropertyChanged
    {
        private const int BPM_Max = 240;
        private const int BPM_Min = 30;
        private const int METER_Max = 12;
        private const int METER_Min = 1;
        private const string TEMPO = "szTempo";
        private const string METER = "szMeter";
        private const string ONOFF = "szStartStop";
        private const string TEMPOMARKING = "szTempoMarking";
        private const string SOUNDS = "szSounds";

        public string szSounds
        {
            get
            {
                switch (Globals.Settings.eMetronomeSounds)
                {
                    case EnumMetronome.Metal:
                        return Globals.rm.GetString("IDS_OPT_Metal");
                    case EnumMetronome.Wood:
                        return Globals.rm.GetString("IDS_OPT_Wood");
                    case EnumMetronome.Synth:
                        return Globals.rm.GetString("IDS_OPT_Synth");
                    case EnumMetronome.Old:
                        return Globals.rm.GetString("IDS_OPT_Retro");
                    case EnumMetronome.Silence:
                    default:
                        return Globals.rm.GetString("IDS_OPT_Silent");
                }
            }
        }
        public string szStartStop
        {
            get
            {
                if (bOn)
                    return Globals.rm.GetString("IDS_MENU_Stop");
                else
                    return Globals.rm.GetString("IDS_MENU_Start");
            }
        }
        public string szTempo
        {
            get
            {
                return iBPM.ToString();
            }
        }
        public string szMeter
        {
            get
            {
                return iMeter.ToString();
            }
        }
        public string szTempoMarking
        {
            get
            {
                if (Globals.Settings.iBpm > 199)
                    return Globals.rm.GetString("IDS_OPT_Prestissimo");// > 199
                else if (Globals.Settings.iBpm > 167)
                    return Globals.rm.GetString("IDS_OPT_Presto"); //> 167
                else if (Globals.Settings.iBpm > 119)
                    return Globals.rm.GetString("IDS_OPT_Allegro"); //> 119
                else if (Globals.Settings.iBpm > 107)
                    return Globals.rm.GetString("IDS_OPT_Moderato"); //> 107
                else if (Globals.Settings.iBpm > 75)
                    return Globals.rm.GetString("IDS_OPT_Andante"); //> 75
                else if (Globals.Settings.iBpm > 65)
                    return Globals.rm.GetString("IDS_OPT_Adagio"); //> 65
                else if (Globals.Settings.iBpm > 59)
                    return Globals.rm.GetString("IDS_OPT_Larghetto"); //> 59
                else if (Globals.Settings.iBpm > 39)
                    return Globals.rm.GetString("IDS_OPT_Largo"); //> 39
                else
                    return Globals.rm.GetString("IDS_INFO_Tempo");
            }
        }
        public int iBPM
        {
            get
            {
                return Globals.Settings.iBpm;
            }
            set
            {
                if (value >= BPM_Min && value <= BPM_Max)
                {
                    Globals.Settings.iBpm = value;
                    NotifyPropertyChanged(TEMPO);
                    NotifyPropertyChanged(TEMPOMARKING);
                }
                else if (value < BPM_Min)
                {
                    Globals.Settings.iBpm = BPM_Min;
                    NotifyPropertyChanged(TEMPO);
                    NotifyPropertyChanged(TEMPOMARKING);

                }
                else if (value > BPM_Max)
                {
                    Globals.Settings.iBpm = BPM_Max;
                    NotifyPropertyChanged(TEMPO);
                    NotifyPropertyChanged(TEMPOMARKING);
                }
            }
        }
        public int iMeter
        {
            get
            {
                return Globals.Settings.iMeter;
            }
            set
            {
                if (value >= METER_Min && value <= METER_Max)
                {
                    Globals.Settings.iMeter = value;
                    NotifyPropertyChanged(METER);

                   
                }
            }
        }
        public int iSoundIndex
        {
            get
            {
                return (int)Globals.Settings.eMetronomeSounds;
            }
            set
            {
                Globals.Settings.eMetronomeSounds = (EnumMetronome)value;
                NotifyPropertyChanged(SOUNDS);
            }
        }
        public bool bOn 
        {
            get
            {
                return _bOn;
            }
            set
            {
                _bOn = value;
                NotifyPropertyChanged(ONOFF);
            }
        }
        //public Sequencer sequencer;
        public event PropertyChangedEventHandler PropertyChanged;

        private bool _bOn;

        public ObjMetronome()
        {
            _bOn = false;

            //sequencer = new Sequencer();        
        }

        public bool TempoUp()
        {
            if (Globals.Settings.iBpm < BPM_Max)
            {
                Globals.Settings.iBpm++;
                NotifyPropertyChanged(TEMPO);
                NotifyPropertyChanged(TEMPOMARKING);

                //if (bOn)
                //    ResetSequencer();
                return true;
            }
            return false;
        }

        public bool TempoDown()
        {
            if (Globals.Settings.iBpm > BPM_Min)
            {
                Globals.Settings.iBpm--;
                NotifyPropertyChanged(TEMPO);
                NotifyPropertyChanged(TEMPOMARKING);

                //if (bOn)
                //    ResetSequencer();
                return true;
            }
            return false;
        }

        public bool MeterUp()
        {
            if (Globals.Settings.iMeter < METER_Max)
                Globals.Settings.iMeter++;
            else
                Globals.Settings.iMeter = METER_Min;
            NotifyPropertyChanged(METER);

            //if (bOn)
            //    ResetSequencer();
            return true;
        }

        public bool MeterDown()
        {
            if (Globals.Settings.iMeter > METER_Min)
                Globals.Settings.iMeter--;
            else
                Globals.Settings.iMeter = METER_Max;

            NotifyPropertyChanged(METER);

            //if (bOn)
            //    ResetSequencer();
            return true;
        }

        public int GetTimerTiming()
        {
            return 6000 / iBPM;
        }

        /*public void ResetSequencer()
        {
            sequencer.Tempo = Globals.Settings.iBpm;
            sequencer.StepCount = Globals.Settings.iMeter;
            sequencer.StepIndex = sequencer.StepCount - 1; 
            sequencer.Reset();

            for (int i = 0; i < Globals.Settings.iMeter; i++)
            {
                WavPlayer tick = new WavPlayer();
                if (i == Globals.Settings.iMeter-1)
                {
                    tick.WaveData = GetResource("Assets/Samples/Bass Drum 1.WAV");
                }
                else
                {
                    tick.WaveData = GetResource("Assets/Samples/Cabasa.WAV");
                }
                sequencer.AddNote(tick, i, 10);
            }

        }*/

        private void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this,
                    new PropertyChangedEventArgs(propertyName));
            }
        }

        private byte[] GetResource(string szFileName)
        {
            Uri uri = new Uri(szFileName, UriKind.RelativeOrAbsolute);

            byte[] buffer;
            StreamResourceInfo info = App.GetResourceStream(uri);
            using (Stream stream = info.Stream)
            {
                buffer = new byte[stream.Length];
                stream.Read(buffer, 0, (int)stream.Length);

            }
            return buffer;
          
        }

        

        

    }
}
