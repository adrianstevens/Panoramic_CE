#pragma once
#include "stdafx.h"
#include <Mmsystem.h>

enum EnumSampleFreqID
{
    FQ_SAMP_Low = 11025,
    FQ_SAMP_Med = 22050,
    FQ_SAMP_High= 44100,
};

#define WAVE_FORMAT_PCM 1
#define WF_CHANNELS 1
#define WF_BITS_PER_SAMPLE 8
#define WF_BLOCK_ALIGN (WF_CHANNELS * WF_BITS_PER_SAMPLE / 8)
#define FQ_SAMP_FREQ_LOW 1
#define FQ_SAMP_FREQ_MEDIUM 2
#define FQ_SAMP_FREQ_HIGH 3
#define WF_CBSIZE 0
#define WF_NUM_BUFFERS 3
#define WF_BUFFER_SIZE 8192
#define WF_DEVICEID 0
#define MSGBUFLEN 256
#define MM_HARTBEAT_MAX 4
#define FQ_TRIGGER 0x03
#define FQ_MAX_OFFSET 2
#define PPC_FQ_DR_XBASE 30
#define PPC_FQ_DR_YBASE 190
#define HPC_FQ_DR_XBASE 80
#define HPC_FQ_DR_YBASE 100
//#define FQ_GRAPH_REGION 1
//#define FQ_FRET_REGION 2
#define FQ_MIN 3  // Frequency Quality
#define FQ_DISPLAY_BUFFER 100
#define FQ_STATIC_DISPLAY_OFFSET 100
#define MEASURE_PITCH 0 // -1 in (version 1.3)
//#define NOTEVIEW_PIANO 1
//#define NOTEVIEW_GUITAR 2
#define FREQTUNE_THREAD_MAX_WAIT 4000 //Max. time to wait for waveIn thread to stop
#define FREQAVGMAXSCOPE 5 //Settable from version 1.5: Min 1, Max 20, def 10
#define	FREQAVGMAXSCOPE_MIN 3.0
#define	FREQAVGMAXSCOPE_MAX 20.0
#define FREQTUNE_MAXNOTEDEVIATION 1.5 //Settable from version 1.5: Min 0.5%/Max 5% def 1.5%
#define FREQTUNE_MAXNOTEDEVIATION_MIN 0.1
#define FREQTUNE_MAXNOTEDEVIATION_MAX 10.0
#define FREQTUNE_MAXFREQDEVIATION 0.005 //Settable from version 1.5: Min 0.1%/Max 3% def 0.3%
#define FREQTUNE_MAXFREQDEVIATION_MIN 0.1
#define FREQTUNE_MAXFREQDEVIATION_MAX 5.0
#define FREQTUNE_MINFREQ 1.0 //Settable from version 1.5: Min 16.0 Max 10000.0 Hz
//#define	FREQTUNE_MINFREQMAX 10000.0
//#define FREQTUNE_MINPITCH 0.9
//#define FREQTUNE_MAXPITCH 1.1
#define MIN_PERIODS 5 //Minimum amount of measured periods
#define BASEFREQ 440.0
#define NOTE_E2 31
#define NOTE_B3 50
#define NOTE_GS4 59
#define	MIN_CENT 50.0
#define MAX_CENT 200.0
#define	MIN_FREQ 100.0
#define MAX_FREQ 1000.0
#define TEMPR_EXT L".fqt"
//#define MAX_TEMPERAMENTFILES 256
//#define EM_SIZELIMIT 6
#define MAX_CENT_DEV 0.01
#define resNoteName	L"A      \0B flat \0B      \0C      \0C sharp\0D      \0E flat \0E      \0F      \0F sharp\0G      \0G sharp\0A      \0"

#define WM_FREQ_UPDATE      WM_USER + 200
#define WM_FREQ_TUNED       WM_USER + 201

class CFreqTuner
{
public:
    CFreqTuner(void);
    ~CFreqTuner(void);

    HRESULT             InitTuner(HWND hWnd, HINSTANCE hInst);
    void                SetTrigger(int iTrigger){m_iTrigger = iTrigger;};//0 to 32 ... keep it in range pls
    HRESULT             StartTuner(EnumSampleFreqID eFreqSample);
    HRESULT             StopTuner();

    BOOL                GetIsTuned();
    double              GetFrequency();
    int                 GetSampleQuality();
    int                 GetSignalStrength();
    int                 GetPcntFromTuned(); //negative for flat?
    int                 GetNote();

    unsigned char*      GetGraphData(){return m_unchDisplData;};
    int                 GetMaxDispLev(){return m_iMaxDispLev;};//so we can scale

private:
    void                Destroy();
    static  DWORD WINAPI ThreadWaveProcess(LPVOID lpParameter);
    DWORD               fnThreadWaveProcess();
    void                ProcessData(HWND hWnd, WAVEHDR wvHdr);
    double              AvgFreq(double dFreqIn, bool bFirstValue);
    BOOL                IsTuned(double dPitch);

    

private:
    HWND                m_hWnd;
    HINSTANCE           m_hInst;

    CRITICAL_SECTION    m_cr;
    HANDLE              m_hWaveCloseWait;       // wveIn synchronisation
    HANDLE              m_hThread;              // processing thread
    DWORD               m_dwThreadID;

    EnumSampleFreqID    m_eFreqSample;
    long                m_lFreqMinLen;//g_lFQminmeslen
    WAVEFORMATEX        m_waveformat; 
    WAVEHDR             m_waveinHdr[WF_NUM_BUFFERS]; 
    HWAVEIN             m_hWaveIn; 
    int                 m_iNumWaveBufActive;
    BOOL                m_bMeasuring;           // start stop checkbox

    //process data variables 
    BOOL                m_bResetTuningComment;
    long                m_lngDispXStart;
    long                m_lngHeartBeat;
    long                m_lngDispX2Cycl;
    long                m_lNulCnt;
    long                m_lBestMesLen;
    long                m_lngDispXstep;

    int                 m_iTrigger;     //trigger level or sensitivity 0-31
    int                 m_iNoteNum;     //Note and octave ... might be ofset by 3 ...
    int                 m_iPitch;       //how in tune are we? -50 to 50
    int                 m_iFreqQ;       //0 - 100 ....  quality
    int                 m_iMaxDispLev;  //to scale graph
    int                 m_iMaxLev;      //signal strength ... 0 - 127
    int                 m_iAvgMaxScope;
    int                 m_iValueIndex;

    double              m_dbPitchArray[13];
    double              m_dbAvgFreqArray[(int)FREQAVGMAXSCOPE_MAX];
    double              m_dbMinFreq; 
    double              m_dbFreqSampleQ;
    double              m_dbFreqIn;     //frequency ...  wsprintf(msgFreq,L"%#.2f Hz",g_dblFreqIn);
    double              m_dbBaseFreq;
    double              m_dbPitch;
    double              m_dbLastAvgFreq;
    double              m_dbFMaxDev;
    double              m_dbNMaxDev;
    double              m_dbFreqPitch;

    BOOL                m_bSetTuningComment; 
    BOOL                m_bTemperamentEnabled;
    BOOL                m_bIsTuned;
    BOOL                m_bEraseTuned;
    BOOL                m_bFirstFill;

    unsigned char       m_unchDisplData[WF_BUFFER_SIZE]; 
};
