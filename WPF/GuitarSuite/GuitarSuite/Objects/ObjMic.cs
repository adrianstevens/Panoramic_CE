using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NAudio.Wave;
using System.Windows;

namespace GuitarSuite
{
    public class ObjMic
    {
        public int BufferDuration = 0;//ms

        public WaveIn waveIn = new WaveIn();


        public int deviceIndex
        {
            get
            {
                return _deviceIndex;
            }

            set
            {
                _deviceIndex = value;
                waveIn.DeviceNumber = deviceIndex;
            }
        }
        int _deviceIndex = 0;

        bool bBStarted = false;

        int sampleRate;
        
        public delegate void DataAvail(object sender, WaveInEventArgs e);

        public DataAvail NewData = null;

        public ObjMic()
        {
            return;//bugbug

            int waveInDevices = WaveIn.DeviceCount;

#if DEBUG
            string message = "";

            for (int waveInDevice = 0; waveInDevice < waveInDevices; waveInDevice++)
            {
                WaveInCapabilities deviceInfo = WaveIn.GetCapabilities(waveInDevice);

                message += String.Format("Device {0}: {1}, {2} channels",
                    waveInDevice, deviceInfo.ProductName, deviceInfo.Channels);
                // Console.WriteLine("Device {0}: {1}, {2} channels",
                // waveInDevice, deviceInfo.ProductName, deviceInfo.Channels);
            }

            MessageBox.Show(message);
#endif
            
            if (waveInDevices > 0)
            {
                waveIn.DeviceNumber = deviceIndex;
                waveIn.DataAvailable += waveIn_DataAvailable;
                sampleRate = 16000; // 16 kHz
                int channels = 1; // mono
                waveIn.WaveFormat = new WaveFormat(sampleRate, channels);
            }
        }

        public bool Start()
        {
            if (bBStarted == false)
            {
                try
                {
                    if (WaveIn.DeviceCount > 0 && waveIn.NumberOfBuffers > 0)
                    {
                        waveIn.StartRecording();
                        bBStarted = true;
                    }
                }
                catch
                {
                }
            }
            return bBStarted;
        }

        public void Stop()
        {
            if (bBStarted == true)
            {
                bBStarted = false;
                waveIn.StopRecording();
            }
        }

        public int GetSampleRate()
        {
            return sampleRate;//bugbug
        }


        public int GetSampleSizeInBytes(int iBufDur)
        {
            return 3200;//to start ... will be reloaded dynamically 
        }

        public bool GetData(byte[] btArray)
        {
            return false;
        }

        //GetData(btBuffer, 0, iBufferSize);
        public bool GetData(byte[] btArray, int iVal, int iBuffSize)
        {
            

            return false;
        }

        void waveIn_DataAvailable(object sender, WaveInEventArgs e)
        {
            NewData(sender, e);

            /*
            byte[] buffer = e.Buffer;
            int bytesRecorded = e.BytesRecorded;
            //WriteToFile(buffer, bytesRecorded);

            for (int index = 0; index < e.BytesRecorded; index += 2)
            {
                short sample = (short)((buffer[index + 1] << 8) |
                                        buffer[index + 0]);
                
                
                
                //float sample32 = sample / 32768f;
                //sampleAggregator.Add(sample32);
            }*/
        }

    }

 



}
