//Open Metronome: Copyright 2004 David Johnston, 2009 Mark Billington.

//This file is part of "Open Metronome".
//
//"Open Metronome" is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//"Open Metronome" is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with "Open Metronome".  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////////////////////////
// BeatBox_WAV.cpp: implementation of the CBeatBox_WAV class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef UNDER_CE
#if UNDER_CE >= 400 //###Disable this for now in CE3 until I see if it works
#include <Pm.h> //SetPowerState
#include <PmPolicy.h> //PowerPolicyNotify
#endif
#endif
#ifndef USE_WEIRD_MIDI
#include "..\general_midi.h"
#include "BeatBox_WAV.h"

#ifdef _DEBUG
#ifdef _AFX
#define new DEBUG_NEW
#endif
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//--------------------------------------------------------------------------------------------------
//Note to implementers: I originally wrote this to handle lots of WAV formats, but:
//a) My PDA crashes sometimes if stuff is not 32-bit-word aligned (is why there is some oddness!).
//   After burning far too much time on it, I'm now sticking with what I've got rather than 
//   investigate further; googling around reveals ARM processors aren't so happy with non-32-bit
//   aligned stuff (memset'ing a vector of __int16's I think is particularly nasty)
//b) My PDA couldn't play very many formats (e.g., I tried to standardise on a 32-bit-sample format
//   but couldn't find one my PDA could play back in WMP, let alone from code.
//So: This file is a bit of a mess now but it works:/  I'm just hugely sick of poking at it now
//   and want to "get it out there".


//--------------------------------------------------------------------------------------------------
//                                                                                         Constants
//--------------------------------------------------------------------------------------------------
//static TCHAR const s_DefaultWAV[] = _T("ZZ_Default.WAV");
static TCHAR const s_DefaultWAV[] = _T("data1.dac");

//!!!Now a static so I don't realloc each time: It should be fixed-size anyway, as it is sized based
//on ZZ_Default.WAV parameters.
static std::vector<__int16> s_a1MinOfSamples; //This buffer is looped forever by the OS


//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//                                                                                   Local Utilities
//--------------------------------------------------------------------------------------------------
static std::basic_string<TCHAR> SamplesDirectory()
{
    std::basic_string<TCHAR> EXEFilepath;
    EXEFilepath.resize(MAX_PATH);
	GetModuleFileName(NULL, &*EXEFilepath.begin(), EXEFilepath.length());

    std::basic_string<TCHAR>::size_type const iBackslash = EXEFilepath.rfind(_T('\\'));
    if ((iBackslash != std::basic_string<TCHAR>::npos) &&
        (iBackslash < ((std::basic_string<TCHAR>::size_type)EXEFilepath.length())-1))
        EXEFilepath = EXEFilepath.substr(0, iBackslash+1);

    //return EXEFilepath.append(_T("Samples\\"));
    return EXEFilepath;
}
//--------------------------------------------------------------------------------------------------


static bool FileExists(std::basic_string<TCHAR> const & Filename)
{
    bool bRet = false;

    HANDLE hCheckFileExists = INVALID_HANDLE_VALUE;
    hCheckFileExists = CreateFile(Filename.c_str(), 0/*query access*/, 
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

    if (hCheckFileExists != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hCheckFileExists);
        bRet = true;
    }
    return bRet;
}


//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//                                                                                        Nastiness!
//--------------------------------------------------------------------------------------------------


template <class _Ty> void AddSamplesToSampleBuffer(_Ty * pDst,  _Ty const * pSrc, char Vol,
                                                   unsigned long const SampleSrcLen_bytes)
{
    unsigned long const iMax = SampleSrcLen_bytes/sizeof(_Ty);
    {for (unsigned long i = 0; i < iMax; ++i)
    {
        pDst[i] += (_Ty)(  (Vol/127.0) * pSrc[i]  );
    }} 
}
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//                                              (Audio Output is Handled by OS) Main BlinkMsg Thread
//--------------------------------------------------------------------------------------------------
DWORD WINAPI CBeatBox_WAV::BeatNotificationThread_stub(LPVOID pvThis)
{
    CBeatBox_WAV * pThis = reinterpret_cast<CBeatBox_WAV*>(pvThis);
    pThis->BeatNotificationThread();
    return 1;
}
//--------------------------------------------------------------------------------------------------


void CBeatBox_WAV::BeatNotificationThread()
{
    double const sampPerSec = m_fmtWAV.nSamplesPerSec;

    MMTIME myTime;
    memset (&myTime, 0, sizeof(myTime));

    int LastBeat12341234 = -1;
    while (!m_bQuitThread)
    {
        double const BPS = (((double)m_BeatsPerMinute)/60.0);
        double const BeatEveryThisMany_ms = 1000.0/BPS;

        myTime.wType = TIME_SAMPLES;
        MMRESULT mmRes = waveOutGetPosition(m_hwo, &myTime, sizeof(MMTIME));
        ErrorCheck(mmRes == MMSYSERR_NOERROR,_T("Unable to track WAV playback, Cannot blink"),true);

        if (myTime.wType == TIME_SAMPLES)
        {
            double const ms         = 1000.0* ((double)myTime.u.sample)/ sampPerSec;

            int Beat12341234;

            if(m_SequenceLength == 1)
            {
                Beat12341234 = ((int)(ms / BeatEveryThisMany_ms))%2;
            }
            else
            {
                Beat12341234 = ((int)(ms / BeatEveryThisMany_ms))%m_SequenceLength;
            }
            
            if (Beat12341234 != LastBeat12341234)
            {
            //    if(m_SequenceLength == 1)
            //        Beat12341234 = 0;
                
                LastBeat12341234 = Beat12341234;
                int MaxBlinkSize = 0;

                if(m_SequenceLength == 1)
                {
                    //might as well pass in the current beat 
                    ::PostMessage(m_hWnd, UWM_BeatBox_BEAT_OCCURRED_wpBlinkSize_lpNULL, MaxBlinkSize, 0);
                }
                else
                {
                /*    for (unsigned long i = 0; i < m_aInstrumentNums[Beat12341234].size(); ++i)
                    {
                        if (m_aInstrumentNums[Beat12341234][i] >= 0)
                        {
                            if (m_aBeatSizes[m_aInstrumentNums[Beat12341234][i]] > MaxBlinkSize)
                                MaxBlinkSize = m_aBeatSizes[m_aInstrumentNums[Beat12341234][i]];
                        }//else silence
                    }*/
                    //might as well pass in the current beat 
                    ::PostMessage(m_hWnd, UWM_BeatBox_BEAT_OCCURRED_wpBlinkSize_lpNULL, MaxBlinkSize, LastBeat12341234);
                }
            }
        }

#ifdef UNDER_CE
#if UNDER_CE < 400 //(#include <Winuser.h> for SystemIdleTimerReset())
        SystemIdleTimerReset(); //Bodge to always-on while ticking. Better way of doing this in CE3?
#endif
#endif

        ErrorCheck((WAIT_FAILED != WaitForSingleObject(m_hEvtPollPlayback, BeatEveryThisMany_ms/16)),
            _T("Failed waiting on blink event, Metronome will not blink"), true);
    }
}
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//                                                                          Construction/Destruction
//--------------------------------------------------------------------------------------------------
CBeatBox_WAV::CBeatBox_WAV(std::vector<std::vector<long> > const & aInstrumentNums,
                           std::vector<int               > const & aInstruments,   
                           std::vector<int               > const & aVolumes,       
                           std::vector<int               > const & aBeatSizes,     
                           unsigned long                   const   BeatsPerMinute,
                           HWND                            const   hWndToSendBlinksAndErrorsTo) : 
    m_hWnd(hWndToSendBlinksAndErrorsTo),
    m_hwo(NULL), m_bQuitThread(false), m_hThread(NULL), m_SequenceLength(aInstrumentNums.size()),
    m_hPowerReq(NULL),
    m_aInstrumentNums(aInstrumentNums),
    m_aInstruments(aInstruments),
    m_aVolumes(aVolumes),
    m_aBeatSizes(aBeatSizes),
    m_myWavDefault  ((SamplesDirectory().append(s_DefaultWAV)).c_str()),
    m_hEvtPollPlayback(CreateEvent(NULL, FALSE, FALSE, NULL)),
    m_BeatsPerMinute(BeatsPerMinute)
{
    memset(&m_whdr, 0, sizeof(m_whdr));
	memset(&m_fmtWAV, 0, sizeof(m_fmtWAV));
}
//--------------------------------------------------------------------------------------------------


CBeatBox_WAV::~CBeatBox_WAV()
{
    Stop();
    if (m_hEvtPollPlayback)
    {
        CloseHandle(m_hEvtPollPlayback);
        m_hEvtPollPlayback = 0;
    }
}
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//                                                                                    Public Members
//--------------------------------------------------------------------------------------------------
static std::map<unsigned long, std::vector<__int32> > s_mapVoices; //!!!so will grow forever (or at least for as many voices are as in use)! But saves on loading times (!)
void CBeatBox_WAV::Play()
{
    Stop();

    ErrorCheck(PopulateVoiceWAVBufs(), _T("Sample WAVs must all be the same format:\rSpecifically, only mono uncompressed PCM WAV files @ 44.1kHz\r(16-bits-unsigned-int-samples) produced by \"Audacity\"\r(an open-source WAV editor) are properly supported."), true);

    if (s_mapVoices.size() == 0)
    {
        m_strLastError = _T("Cannot find valid format Sample WAVs; should be in \"Samples\" folder,\rwhich itself should be in same folder as exe.\rNote: only mono uncompressed PCM WAV files @ 44.1kHz\r(16-bits-unsigned-int-samples) produced by \"Audacity\"\r(an open-source WAV editor) are properly supported.");
        ::PostMessage(m_hWnd, UWM_BeatBox_ERROR_OCCURRED_wpNULL_lpNULL, NULL, NULL);
        return;
    }

    InitialiseWaveOutDeviceUsingDefaultWAV();

    Create1MinOfSamples();

    PlayWAVAndSendInitialBeat();
}
//--------------------------------------------------------------------------------------------------


void CBeatBox_WAV::Stop()
{
    if (m_hThread)
    {
        m_bQuitThread = true;
        if (!SetEvent(m_hEvtPollPlayback)) return /*AfxThrowUserException()*/;
        if (WAIT_FAILED == WaitForSingleObject(m_hThread, INFINITE)) return /*AfxThrowUserException()*/;
        if (!CloseHandle(m_hThread)) return /*AfxThrowUserException()*/;
        m_hThread = 0;
        m_bQuitThread = false;
    }

    if (m_hwo)
    {
        ErrorCheck((MMSYSERR_NOERROR == waveOutPause(m_hwo)), _T("Failed to pause playback!"),true);
        ErrorCheck((MMSYSERR_NOERROR == waveOutReset(m_hwo)), _T("Failed to reset playback!"),true);
        ErrorCheck((MMSYSERR_NOERROR == waveOutUnprepareHeader(m_hwo, &m_whdr, sizeof(WAVEHDR))), 
            _T("Failed to unprepare WAV header!"), true);	
        ErrorCheck((MMSYSERR_NOERROR == waveOutClose(m_hwo)), _T("Failed to close playback!"),true);
#ifdef UNDER_CE
#if UNDER_CE >= 400 //###Disable this for now in CE3 until I see if it works
		if (m_hPowerReq)
			{ErrorCheck((ERROR_SUCCESS    == ReleasePowerRequirement(m_hPowerReq)), _T("Failed to release power requirement"), true);}
		PowerPolicyNotify(PPN_UNATTENDEDMODE, 0);
#endif
#endif
        m_hwo = 0;
    }
}
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//                                                                                           Helpers
//--------------------------------------------------------------------------------------------------


bool CBeatBox_WAV::PopulateVoiceWAVBufs()
{
    bool bAllWavsAreTheSameFormat = true;
    for (unsigned long i = 0; i < m_aInstruments.size(); ++i)
    {
        int const instrument_num = m_aInstruments[i];

        if (instrument_num >= 0)
        {   
            if (s_mapVoices[instrument_num].size() == 0)
            {
                //!!!Confirmed: this is only getting called when required
                std::basic_string<TCHAR>       sVoice = (instrument_num >= 0)?
                    AlphaInstrumentStrings[MidiToAlpha[instrument_num]]:_T("");

                std::basic_string<TCHAR> const sVoiceFullPathFilename = 
                //    SamplesDirectory().append(sVoice.append(_T(".WAV")));
                    SamplesDirectory().append(sVoice.append(_T(".dac")));

                CWaveFile myWav(sVoiceFullPathFilename.c_str());

                CWaveFile & WavToUse = (myWav.GetLength() == 0)?m_myWavDefault:myWav;

                if (WavToUse.GetLength() == 0)
                    return false;

                std::vector<__int32> & rVoiceBuf = s_mapVoices[instrument_num];

                rVoiceBuf.resize(WavToUse.GetLength()/(WavToUse.GetWaveFormat()->wBitsPerSample/8)); //GetLength is in bytes..
	            WavToUse.ReadSamples(&*rVoiceBuf.begin(), WavToUse.GetLength());

                WAVEFORMATEX const * pWAVFmtDefault   = m_myWavDefault.GetWaveFormat();
                WAVEFORMATEX const * pWAVFmtToExamine = WavToUse.GetWaveFormat();

                if (memcmp(pWAVFmtDefault, pWAVFmtToExamine, sizeof(WAVEFORMATEX)) != 0)
                    bAllWavsAreTheSameFormat = false;
            }
            //else we already have cached this WAV
        }
        //else it is silence
    }
    return bAllWavsAreTheSameFormat;
}
//--------------------------------------------------------------------------------------------------


void CBeatBox_WAV::InitialiseWaveOutDeviceUsingDefaultWAV()
{
    m_fmtWAV = *m_myWavDefault.GetWaveFormat();

    bool bOk = false;
    switch (m_fmtWAV.wBitsPerSample/8)
    {
    case 1:
    case 2:
    case 4:
    case 8:{bOk = true; break;} //See also AddSamplesToSampleBuffer, where this matters!
    }
    ErrorCheck(bOk, _T("WAV Format Invalid:\rSpecifically, only mono uncompressed PCM WAV files @ 44.1kHz\r(16-bits-unsigned-int-samples) produced by \"Audacity\"\r(an open-source WAV editor) are properly supported."), true);

    //See also AddSamplesToSampleBuffer, where this matters!
    ErrorCheck(m_fmtWAV.wFormatTag == WAVE_FORMAT_PCM, 
        _T("WAV Format Invalid:\rSpecifically, only mono uncompressed PCM WAV files @ 44.1kHz\r(16-bits-unsigned-int-samples) produced by \"Audacity\"\r(an open-source WAV editor) are properly supported."), true);

    unsigned long const BufSize = m_fmtWAV.nSamplesPerSec * (m_fmtWAV.wBitsPerSample/8) * 60;
    s_a1MinOfSamples.resize(BufSize/2); //2 is cos of 16 bit samples (it is always 16 bit)

	memset(&m_whdr, 0, sizeof(m_whdr));
	m_whdr.lpData = (char*)&*s_a1MinOfSamples.begin();
	m_whdr.dwBufferLength = BufSize;

    unsigned long const NumWAVDevices = waveOutGetNumDevs();
    ErrorCheck((NumWAVDevices>0), _T("No WAV Ouput Device Found! Cannot Play Beats"), true);

    MMRESULT mmres = MMSYSERR_NODRIVER; //Initialise to something other than MMSYSERR_NOERROR
    {for (unsigned long devId = 0; ((devId < NumWAVDevices)&&(mmres != MMSYSERR_NOERROR)); ++devId)
		mmres = waveOutOpen(&m_hwo, devId, &m_fmtWAV, NULL, NULL, NULL);
#ifdef UNDER_CE
#if UNDER_CE >= 400 //###Disable this for now in CE3 until I see if it works
        if (mmres == MMSYSERR_NOERROR)
        {
            TCHAR bufID[34];
            _ultot(devId, bufID, 10);
            TCHAR bufWAVID[42] = _T("WAV");
            _tcscat(bufWAVID, bufID);
            _tcscat(bufWAVID, _T(":"));

			PowerPolicyNotify(PPN_UNATTENDEDMODE, 1);
			m_hPowerReq = SetPowerRequirement(bufWAVID, D0/*Fully On*/, POWER_NAME|POWER_FORCE, NULL, 0);
            ErrorCheck((m_hPowerReq != 0), _T("Unable to set WaveOut to always-on"), true); //###Probably don't want to ErrorCheck this (but for now I need to see if it works!)
        }
#endif
#endif
	}

    ErrorCheck(mmres == MMSYSERR_NOERROR, _T("Unable to open any WAV Ouput Devices!"), true);
}
//--------------------------------------------------------------------------------------------------


void CBeatBox_WAV::Create1MinOfSamples() //I might consider threading this, and allowing the thread to be interrupted, to improve UI responsiveness; or I might not care:D
{   //!!!This is the "slow" function (well d'uh!); would be *a lot* faster if I just standardized on 32bit samples:/
    unsigned long const SeqLength = m_aInstrumentNums.size();
    unsigned long const BitsPerSample = m_fmtWAV.wBitsPerSample;
             long const BytesPerSample = BitsPerSample/8; //long, not unsigned long, cos that is what div() expects

    std::vector<long> InstrumentNumsForLastBeat;
    std::vector<__int32> strMultiVoiceBuf;

    memset(&*s_a1MinOfSamples.begin(), 0, s_a1MinOfSamples.size()*2);

    {for (unsigned long iBeatInSeq = 0; iBeatInSeq < SeqLength; ++iBeatInSeq)
    {
        //A nasty form of optimisation: the else means we can leave the MultiVoiceBuf at 0;
        //Could build a map if I were really serious about it, but doesn't really seem worth the effort
        //(I'm mainly interested in a Dum-chik-chik-chik type beat anyway, so this will do just fine).
        if (m_aInstrumentNums[iBeatInSeq] != InstrumentNumsForLastBeat)
        {
            unsigned long const BufSize = s_a1MinOfSamples.size();
            //!!!Move downwards!{for (unsigned long jBeatInTime=0; jBeatInTime < m_BeatsPerMinute; jBeatInTime += SeqLength)
            //!!!Move downwards!{
            unsigned long lMaxSampleSize = 0;
            {for (unsigned long kVoice = 0; kVoice < m_aInstrumentNums[iBeatInSeq].size(); ++kVoice)
            {
                long const nIndex = m_aInstrumentNums[iBeatInSeq][kVoice];
                if (nIndex >= 0)
                {
                    int nMidi = m_aInstruments[nIndex];
                    if (s_mapVoices[nMidi].size() == 0)
                        PopulateVoiceWAVBufs(); //Must have introduced a new voice since we last did this bit...
                    lMaxSampleSize = __max(s_mapVoices[nMidi].size(), lMaxSampleSize);
                }
                //else it is silence
            }}

            strMultiVoiceBuf.clear();
            strMultiVoiceBuf.resize(lMaxSampleSize, 0);
            {for (unsigned long kVoice = 0; kVoice < m_aInstrumentNums[iBeatInSeq].size(); ++kVoice)
            {
                long const nIndex = m_aInstrumentNums[iBeatInSeq][kVoice];
                if (nIndex >= 0)
                {
                    int nMidi = m_aInstruments[nIndex];
                    std::vector<__int32> & VoiceWAVBuf = s_mapVoices[nMidi];
                    char const Vol = m_aVolumes[nIndex];

                    __int32 * pDst = &*strMultiVoiceBuf.begin();
                    __int32 const * pSrc = &*VoiceWAVBuf.begin();
                    unsigned long const Len = VoiceWAVBuf.size();

                    {for (unsigned long i = 0; i < Len; ++i)
                    {
                        pDst[i] += (__int32)( ((((unsigned long const)Vol) * ((unsigned long const)pSrc[i])))>>7 ); //Use >>7 instead of /127, hopefully it is faster
                    }} 
                }
                //else it is silence
            }}
            InstrumentNumsForLastBeat = m_aInstrumentNums[iBeatInSeq];
        }

        {for (unsigned long jBeatInTime=0; jBeatInTime < m_BeatsPerMinute; jBeatInTime += SeqLength)
        {
            //if (( (jBeatInTime*2) % SeqLength) == 0) //!!!this if is kindof spurious, can do it with for loop, but leave alone for now..
            //{
                double const SecondsIntoSample = ((jBeatInTime+iBeatInSeq) * 60.0 / m_BeatsPerMinute);
                long iSample = (unsigned long)((((double)(m_fmtWAV.nSamplesPerSec*(BytesPerSample)))* SecondsIntoSample) + 0.5);

                iSample = (div((int)(iSample/2),(int)BytesPerSample).quot) * BytesPerSample;//Round to boundary; //!!!it is isample/2 since we're using isample as an index into 16 bit array now, whereas before was 8 bit array

                for (unsigned long i = 0; (i < strMultiVoiceBuf.size()) && (iSample+i < s_a1MinOfSamples.size()); ++i)
                    s_a1MinOfSamples[iSample+i]= (__int16)strMultiVoiceBuf[i];
            //}
        }}
    }}
}
//--------------------------------------------------------------------------------------------------


void CBeatBox_WAV::PlayWAVAndSendInitialBeat()
{
	MMRESULT mmres = waveOutPrepareHeader(m_hwo, &m_whdr, sizeof(WAVEHDR));	
    ErrorCheck((mmres == MMSYSERR_NOERROR), _T("Failed to prepare WAV, cannot play beats"), true);

	m_whdr.dwFlags |= WHDR_BEGINLOOP|WHDR_ENDLOOP;
	m_whdr.dwLoops = 0xffffffff; //effectively infinite (runs for several years!)
	mmres = waveOutWrite(m_hwo, &m_whdr, sizeof(WAVEHDR));	
    ErrorCheck((mmres == MMSYSERR_NOERROR), _T("Failed to play composite WAV"), true);

    ::PostMessage(m_hWnd, UWM_BeatBox_BEAT_OCCURRED_wpBlinkSize_lpNULL, 0, 0);

    DWORD dwThreadID = 0;
    m_hThread = CreateThread(NULL, NULL, BeatNotificationThread_stub, this, NULL, &dwThreadID);
    ErrorCheck(m_hThread, _T("Failed to start blink thread, cannot blink"), true);
}
//--------------------------------------------------------------------------------------------------


#endif //#ifndef USE_WEIRD_MIDI