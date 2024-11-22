using System;
using System.Net;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Threading;
using NAudio.Wave;

namespace GuitarSuite
{
    public enum TunerSensitivity
    {
        normal,
        low,
        high,
        count,
    };

    public enum FrequencyRange
    {
        normal,
        guitar,
        bass,
        full,
     //   piano,
        count,
    };


    public class ObjGuitarTuner
    {
#region Private vars

        private ObjMic _mic = null; ///50 - 800 as a max range
        public double _minHz = 50.0;//low bass is at 40hz
        public double _maxHz = 800.0;//top string on a guitar is 329 so this is plenty ...  500 gave crap results on the LG so I'm trying 800

        private DispatcherTimer _dtMicTimer = null;
        private int iBufferSize = 3200;
        private int iTimerMS = 200;


        TunerSensitivity sensitivity
        {
            get
            {
                return Globals.Settings.eSensitivity;
            }
        }
        FrequencyRange freqrange
        {
            get
            {
                return Globals.Settings.eTunerRage;
            }
        }

  //      bool bassboost = false; //boost the response of the lowend

        int iSensLimit
        {
            get
            {
                if (sensitivity == TunerSensitivity.high)
                    return 50;
                else if (sensitivity == TunerSensitivity.low)
                    return 500;
                return 150;


            }

        }

  
#endregion

#region Public vars

        public object visualLock;
        public byte[] btBuffer;
        public Int16[] i16Buffer;
        public double[] dbBuffer;
        public double[] spectr;
        public double dbMaxSpectValue = 0; //amplitude so we can scale the graph 
        public double dbFundamentalFrequency;
        public event EventHandler<EventArgs> FrequencyDetected;

        public int iMaxAmplitude = 0;

        public double db1Freq = 0;
        public double db2Freq = 0;
        public double db3Freq = 0;

#endregion

        public ObjGuitarTuner()
        {
            SetRangeDefaults();

            visualLock = new object();

            _mic = new ObjMic();  //Globals.mic;
            _mic.Stop();
            iBufferSize = _mic.GetSampleSizeInBytes(_mic.BufferDuration);
            _mic.BufferDuration = 500; // TimeSpan.FromMilliseconds(1000);//was 800
            btBuffer = new byte[iBufferSize];
            i16Buffer = new Int16[btBuffer.Length / 2];
            dbBuffer = new double[btBuffer.Length / 2];

            //hook up the delegate
            _mic.NewData += waveIn_DataAvailable;
        }

        ~ObjGuitarTuner()
        {
        }

        public bool StartTuner()
        {
            if (_mic != null)
            {
                return _mic.Start();
            }
            return false;
        }

        public void StopTuner()
        {
            if(_mic != null)
                _mic.Stop();
            //_dtMicTimer.Stop();
        }
        
        public void MicTimer(object o, EventArgs sender)
        {
            if (_mic == null)
                return;

            lock (visualLock)
            {
                iMaxAmplitude = 0;
                //_mic.GetData(btBuffer);
                //_mic.GetData(btBuffer, 0, iBufferSize);

                CopyAndCalcBuffer();
            }


            if (FrequencyDetected != null)
            {
                FrequencyDetected(this, new EventArgs());
            }

        }

        void CopyAndCalcBuffer()
        {
            // fill the int16 array
            if(btBuffer.Length != i16Buffer.Length)
                i16Buffer = new short[btBuffer.Length];

            Buffer.BlockCopy(btBuffer, 0, i16Buffer, 0, i16Buffer.Length);//btBuffer.Length);

            int iInterstingValues = 0;

            int iSilenceDetection = (short)((double)short.MaxValue * 0.2);//* 0.07);

            int iCheckValue;

            // fill the double array
            for (int i = 0; i < dbBuffer.Length; i++)
            {
                dbBuffer[i / 2] = i16Buffer[i] / ((double)short.MaxValue);

                iCheckValue = (int)i16Buffer[i];

                // silence detection
                if (Math.Abs(iCheckValue) > iSilenceDetection)
                {

                    iMaxAmplitude = Math.Max(iMaxAmplitude, Math.Abs(iCheckValue));//save the max interesting value
                    iInterstingValues++;
                }
            }

            spectr = FftAlgorithm.Calculate(dbBuffer);

            if (iInterstingValues > iSensLimit) //i16Buffer.Length * 0.1)
            {
                // find the fundamental freqency
                dbFundamentalFrequency = DetectPitch(dbBuffer);

                // if it's outside the range
                if (dbFundamentalFrequency < _minHz || dbFundamentalFrequency > _maxHz)
                    dbFundamentalFrequency = 0;
            }
            else
                dbFundamentalFrequency = 0;
        }


        void Default_BufferReady(object sender, EventArgs e)
        {
            if (_mic == null)
                return;

            lock (visualLock)
            {
                iMaxAmplitude = 0;
               // _mic.GetData(btBuffer);

                CopyAndCalcBuffer();
            }
            

            if (FrequencyDetected != null)
            {
                FrequencyDetected(this, new EventArgs());
            }
        }

        private double DetectPitch(double[] samples)
        {
            int index = 0;
            int index2 = 0;
            int index3 = 0;
            double max = 0;// spectr[0];
            dbMaxSpectValue = 0;
            
            int usefullMaxSpectr = Math.Min(spectr.Length, (int)(_maxHz * spectr.Length /  _mic.GetSampleRate()) + 1);
            int usefullMinSpectr = Math.Min(spectr.Length, (int)(_minHz * spectr.Length / _mic.GetSampleRate()) + 1);

            for (int i = usefullMinSpectr; i < usefullMaxSpectr; i++)
            {
                if (max < spectr[i])
                {
                    max = spectr[i];
                   /* if (Math.Abs(i - index) > 10)
                    {
                        index2 = index;
                        index3 = index2;
                        
                    }*/
                    index2 = index;
                    index3 = index2;
                    index = i;
                }
            }

            db1Freq = (double)_mic.GetSampleRate() * index / spectr.Length;
            db2Freq = (double)_mic.GetSampleRate() * index2 / spectr.Length;
            db3Freq = (double)_mic.GetSampleRate() * index3 / spectr.Length;

            dbMaxSpectValue = spectr[index];//save the max amplitude 

            if (db1Freq < _minHz)
            {
                db1Freq = 0;
                db2Freq = 0;
                db3Freq = 0;
            }

            /*
            int p = 0;

            if(spectr[index2] > 1000)
                p = 12;*/

            System.Diagnostics.Debug.WriteLine("Amp: "+ spectr[index].ToString() + " " + spectr[index2].ToString() + " " + spectr[index3].ToString());
            System.Diagnostics.Debug.WriteLine("Hz: " + db1Freq.ToString() + " " + db2Freq.ToString() + " " + db3Freq.ToString());

            return db1Freq;
        }

     
        //this method isn't currenty being used
        private double DetectPitchCalculation(double[] samples)
        {
            // note that higher frequency means lower period
            int nLowPeriodInSamples = (int)hzToPeriodInSamples(_maxHz, _mic.GetSampleRate());
            int nHiPeriodInSamples = (int)hzToPeriodInSamples(_minHz, _mic.GetSampleRate());

            double dbLowPeriodInSamples = hzToPeriodInSamples(_maxHz, _mic.GetSampleRate());
            double dbHiPeriodInSamples = hzToPeriodInSamples(_minHz, _mic.GetSampleRate());

            if (nHiPeriodInSamples <= nLowPeriodInSamples) 
                throw new Exception("Bad range for pitch detection.");

            if (samples.Length < nHiPeriodInSamples) 
                throw new Exception("Not enough samples.");

            // both algorithms work in a similar way
            // they yield an array of data, and then we find the index at which the value is highest.
            double[] results = new double[nHiPeriodInSamples - nLowPeriodInSamples];

            for (int period = nLowPeriodInSamples; period < nHiPeriodInSamples; period += 1)
            {
                double sum = 0;
                // for each sample, find correlation. (If they are far apart, small)
                for (int i = 0; i < samples.Length - period; i++)
                    sum += samples[i] * samples[i + period];

                double mean = sum / (double)samples.Length;
                results[period - nLowPeriodInSamples] = mean;
            }

            // find the best indices
            int bestIndice = FindBestCandidates(ref results); //note findBestCandidates modifies parameter

            // convert back to Hz
            return periodInSamplesToHz(bestIndice + nLowPeriodInSamples, _mic.GetSampleRate());
        }
        /// <summary>
        /// Finds n "best" values from an array. Returns the indices of the best parts.
        /// (One way to do this would be to sort the array, but that could take too long.
        /// Warning: Changes the contents of the array!!! Do not use result array afterwards.
        /// </summary>
        private int FindBestCandidates(ref double[] inputs)
        {            
            int res;

            // find the highest.
            double fBestValue = double.MinValue;
            int nBestIndex = -1;
            for (int i = 0; i < inputs.Length; i++)
            {
                if (inputs[i] > fBestValue)
                {
                    nBestIndex = i;
                    fBestValue = inputs[i];
                }
            }

            // record this highest value
            res = nBestIndex;

            // now blank out that index.
            inputs[nBestIndex] = double.MinValue;

            return res;
        }

        private double hzToPeriodInSamples(double hz, int sampleRate)
        {
            return 1.0 / (hz / (double)sampleRate);
        }

        private double periodInSamplesToHz(int period, int sampleRate)
        {
            return 1.0 / ((double)period / (double)sampleRate);
        }

        public void SetRangeDefaults()
        {
            switch (freqrange)
            {
                case FrequencyRange.normal:
                    _maxHz = 800.0;
                    _minHz = 50.0;
                    break;
                case FrequencyRange.guitar:
                    _maxHz = 400.0;
                    _minHz = 65.0;
                    break;
                case FrequencyRange.bass:
                    _maxHz = 200.0;
                    _minHz = 30.0; 
                    break;
                case FrequencyRange.full:
                    _maxHz = 10000.0;
                    _minHz = 50;
                    break;
            }
        }

        void waveIn_DataAvailable(object sender, WaveInEventArgs e)
        {
            //Console.WriteLine("Data!");
            lock (visualLock)
            {
                btBuffer = e.Buffer;
                iBufferSize = btBuffer.Length;

                iMaxAmplitude = 0;
                
                CopyAndCalcBuffer();
            }


            if (FrequencyDetected != null)
            {
                FrequencyDetected(this, new EventArgs());
            }

        }
    }
}
