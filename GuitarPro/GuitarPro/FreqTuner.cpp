#include "FreqTuner.h"
#include "winuser.h"
#include "IssDebug.h"


CFreqTuner::CFreqTuner()
:m_hWnd(NULL)
,m_hInst(NULL)
,m_hThread(NULL)
,m_hWaveCloseWait(NULL)
,m_hWaveIn(NULL)
,m_iNumWaveBufActive(0)
,m_bMeasuring(FALSE)
{
    InitializeCriticalSection(&m_cr);

    //process data variables
    m_bResetTuningComment   = FALSE;
    m_lngDispXStart         = 0;
    m_lngHeartBeat          = 0;
    m_lngDispX2Cycl         = 0;
    m_lNulCnt               = 0;
    m_lBestMesLen           = 0;
    m_lngDispXstep          = 0;

    m_iTrigger              = 3;
    m_iNoteNum              = 0;
    m_iPitch                = 0;
    m_iFreqQ                = 0;
    m_iMaxDispLev           = 0;
    m_iMaxLev               = 0;
    m_iAvgMaxScope          = (int)FREQAVGMAXSCOPE;
    m_iValueIndex           = 0;

    m_dbMinFreq             = FREQTUNE_MINFREQ; 
    m_dbFreqSampleQ         = 0; 
    m_dbFreqIn              = 0;
    m_dbBaseFreq            = BASEFREQ;
    m_dbPitch               = 0;
    m_dbFMaxDev             = FREQTUNE_MAXFREQDEVIATION;
    m_dbLastAvgFreq         = 1.0;
    m_dbNMaxDev             = FREQTUNE_MAXNOTEDEVIATION; 
    m_dbFreqPitch           = 1.0;

    m_bSetTuningComment     = FALSE;
    m_bTemperamentEnabled   = FALSE;
    m_bIsTuned              = FALSE;
    m_bEraseTuned           = FALSE;
    m_bFirstFill            = TRUE;

    memset(&m_unchDisplData, 0x80, FQ_DISPLAY_BUFFER);//128 if you're wondering ...
}

CFreqTuner::~CFreqTuner()
{
    Destroy();
    DeleteCriticalSection(&m_cr);
}

void CFreqTuner::Destroy()
{
    StopTuner();

    if(m_hThread)
    {
        PostThreadMessage(m_dwThreadID,WM_QUIT,0,0);
        WaitForSingleObject(m_hThread, INFINITE);
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }

    if(m_hWaveCloseWait)
    {
        CloseHandle(m_hWaveCloseWait);
        m_hWaveCloseWait = NULL;
    }
}

HRESULT CFreqTuner::InitTuner(HWND hWnd, HINSTANCE hInst)
{
    HRESULT hr = S_OK;
    Destroy();

    m_hWnd = hWnd;
    m_hInst = hInst;

    m_hWaveCloseWait    = CreateEvent(NULL, TRUE, FALSE, _T("WaveCloseWait"));
    CPHR(m_hWaveCloseWait, _T("m_hWaveCloseWait    = CreateEvent(NULL, TRUE, FALSE, _T(\"WaveCloseWait\"));"));

    m_hThread           = CreateThread(NULL, 0, ThreadWaveProcess, this, 0, &m_dwThreadID);
    CPHR(m_hThread, _T("m_hThread           = CreateThread(NULL, 0, ThreadWaveProcess, this, 0, dwThreadID);"));


Error:
    return hr;
}

HRESULT CFreqTuner::StartTuner(EnumSampleFreqID eFreqSample)
{
    HRESULT hr = S_OK;
    MMRESULT rc;

    CBARG(m_hWaveCloseWait && m_hThread, _T(""));

    StopTuner();

    m_eFreqSample = eFreqSample;
    m_lFreqMinLen = (int)m_eFreqSample/16;

    //Start wave input
    memset(&m_waveformat,0x80,sizeof WAVEFORMATEX);
    m_waveformat.wFormatTag = WAVE_FORMAT_PCM;
    m_waveformat.nChannels = WF_CHANNELS;
    m_waveformat.wBitsPerSample = WF_BITS_PER_SAMPLE;
    m_waveformat.nSamplesPerSec = (int)m_eFreqSample;
    m_waveformat.nBlockAlign = WF_BLOCK_ALIGN;
    m_waveformat.nAvgBytesPerSec = ((int)m_eFreqSample * WF_BLOCK_ALIGN);
    m_waveformat.cbSize = WF_CBSIZE;

    for (int i = 0;i<WF_NUM_BUFFERS;i++) 
    {
        m_waveinHdr[i].lpData = (char *) malloc(WF_BUFFER_SIZE);
        m_waveinHdr[i].dwBufferLength = WF_BUFFER_SIZE;
        m_waveinHdr[i].dwBytesRecorded  = 0; 
        m_waveinHdr[i].dwUser           = 0;
        m_waveinHdr[i].dwFlags          = 0;
        m_waveinHdr[i].dwLoops          = 0; 
    }

    m_bMeasuring = TRUE;

    rc = waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_waveformat, (DWORD)m_dwThreadID, 0, WAVE_FORMAT_DIRECT | CALLBACK_THREAD);
    CBARG(rc == 0, _T("MMRESULT rc = waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_waveformat, (DWORD)m_dwThreadID, 0, WAVE_FORMAT_DIRECT | CALLBACK_THREAD);"));

    for (int i = 0;i<WF_NUM_BUFFERS;i++) 
    {
        rc = waveInPrepareHeader(m_hWaveIn, &m_waveinHdr[i], WF_BUFFER_SIZE);
        CBARG(rc == 0, _T("rc = waveInPrepareHeader(m_hWaveIn, &m_waveinHdr[i], WF_BUFFER_SIZE);"));

        m_iNumWaveBufActive++;
    }

    for (int i = 0;i<WF_NUM_BUFFERS;i++) 
    {
        rc = waveInAddBuffer(m_hWaveIn, &m_waveinHdr[i], sizeof WAVEHDR);
        CBARG(rc == 0, _T("rc = waveInAddBuffer(m_hWaveIn, &m_waveinHdr[i], sizeof WAVEHDR);"));
    }

    rc = waveInStart(m_hWaveIn);
    CBARG(rc == 0, _T("rc = waveInStart(m_hWaveIn);"));

Error:

    if(hr != S_OK)
        StopTuner();

    return hr;
}

HRESULT CFreqTuner::StopTuner()
{
    HRESULT hr = S_OK;

    MMRESULT	rc;

    m_bMeasuring = FALSE;

    if (!m_hWaveIn)
        goto Error;

    rc=waveInReset(m_hWaveIn);
    if (!rc) 
        rc = waveInStop(m_hWaveIn);
    CBARG(rc == 0, _T("rc = waveInStop(m_hWaveIn);"));

    //Wait for thread to finish
    WaitForSingleObject(m_hWaveCloseWait,FREQTUNE_THREAD_MAX_WAIT);
    rc=waveInClose(m_hWaveIn);
    CBARG(rc == 0, _T("rc=waveInClose(m_hWaveIn);"));

    m_hWaveIn = NULL;
    m_iNumWaveBufActive = 0;


Error:
    return hr;
}

DWORD CFreqTuner::ThreadWaveProcess(LPVOID lpParameter)
{
    CFreqTuner* pThis = (CFreqTuner*)lpParameter;
    if(!pThis)
        return 0;

    return pThis->fnThreadWaveProcess();
}

DWORD CFreqTuner::fnThreadWaveProcess()
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        LPWAVEHDR pwvHdr = (LPWAVEHDR)msg.lParam;
        HWAVEIN hwi = (HWAVEIN)msg.wParam;

        switch(msg.message)
        {
        case MM_WIM_OPEN:
            ResetEvent(m_hWaveCloseWait);
            break;
        case MM_WIM_DATA:
            if (m_bMeasuring && (*pwvHdr).dwBytesRecorded == (*pwvHdr).dwBufferLength) 
            {
                ProcessData(m_hWnd, *pwvHdr);
            }

            if(m_bMeasuring)
            {
                waveInAddBuffer(hwi, pwvHdr, sizeof(*pwvHdr));
            }
            else
            {
                waveInUnprepareHeader(hwi, pwvHdr, sizeof(*pwvHdr));
                free((*pwvHdr).lpData);
                m_iNumWaveBufActive--;
            }
            
            if (!m_iNumWaveBufActive)
                SetEvent(m_hWaveCloseWait);

            break;
        case MM_WIM_CLOSE:

            break;
        }
    }
    ExitThread(0);


    return 0;
}

void CFreqTuner::ProcessData(HWND hWnd, WAVEHDR wvHdr)
{
    long	        lngPOffset=0;
    static	int		iLastNoteNr = -1;
    double	        dblFreq = 0.0;
    long	        lngN=0;
    long	        lngNulCnt=0;
    int		        iNewRawSamp=0;
    int		        iOldRawSamp=0;
    int		        iNewSamp=0;
    bool	        bTop=FALSE;
    bool	        bBottom=FALSE;
    int		        iMaxLev=0;
    int		        iMinLev=0;
    long	        lngStableX=0,lngStableXOffset=0,lngFirstX=0,lngLastX=0,lngNewX=0;
    long	        lngBestMesLen=0;
    double	        dblNoteNr=0;
    double	        dblNoteNrFmod=0;
    int		        intNoteNrFmod=0;
    int		        intPitch=0;
    double	        dblPitch=0.0;

    lngNulCnt = 0;
    iMinLev = 0;
    iMaxLev = 0;
    lngStableX = 0;
    lngStableXOffset = 0;
    lngFirstX = 0;
    lngLastX = 0;
    lngBestMesLen = 0;
    dblFreq = 0.0;
    m_lngDispXStart = 0;
    //Reset heartbeat counter to ensure foreground thread that a measurement is active
    m_lngHeartBeat = 0;

    for (lngN = 0;(unsigned long) lngN < wvHdr.dwBytesRecorded;lngN++) 
    {
        iNewSamp = ((unsigned char) *(wvHdr.lpData + lngN)) - 128;

        if (iNewSamp > iMaxLev) iMaxLev = iNewSamp;
        if (iNewSamp < iMinLev) iMinLev = iNewSamp;
        if ((iNewSamp > m_iTrigger)) 
        { 
            if (bBottom && !bTop)
            {
                bBottom = FALSE;
                //XOffset instable
                if ((abs((lngN - lngNewX) - lngStableXOffset) > (long) (lngStableXOffset / FQ_MAX_OFFSET)) && (lngNewX != 0) && (lngStableXOffset != 0)) 
                {
                    lngNewX = 0;
                    m_lngDispXStart = 0;
                }
                //First point?
                if ((lngNewX == 0) ) 
                { 
                    //Save Last Mes.
                    //Calculate Frequency
                    if (((lngLastX - lngFirstX) > m_lFreqMinLen) && (lngNulCnt >= MIN_PERIODS)) 
                    {
                        if ((lngLastX - lngFirstX) > lngBestMesLen) 
                        {
                            dblFreq = (((double) m_eFreqSample * (double) (lngNulCnt - 1)) / (double (lngLastX - lngFirstX  + MEASURE_PITCH))) * m_dbFreqPitch;
                            lngBestMesLen = lngLastX - lngFirstX;
                        }
                    }

                    //Reset counters
                    lngNewX = lngN;
                    lngStableXOffset = 0;
                    lngFirstX = 0;
                    lngLastX = 0;
                    lngNulCnt = 0;
                } 
                else 
                {
                    //Update Offset
                    lngNulCnt++;
                    lngStableXOffset = lngN - lngNewX;
                    lngNewX = lngN;
                    if (lngFirstX == 0) 
                    {
                        lngFirstX = lngN; //Start point
                        if (m_lngDispXStart == 0)
                            m_lngDispXStart = lngN;
                    }
                    lngLastX = lngN;
                    if ((m_lngDispXStart > 0) && (lngNulCnt == 3)) m_lngDispX2Cycl = lngN;
                }
            }
            bTop = TRUE;
        }
        if (iNewSamp < (0 - (int) m_iTrigger)) 
        {
            if (bTop && !bBottom) 
            {
                bTop = FALSE;
            }
            bBottom = TRUE;
        }
    }
    if (((lngLastX - lngFirstX) > m_lFreqMinLen) && (lngNulCnt >= MIN_PERIODS)) 
    {
        if ((lngLastX - lngFirstX) > lngBestMesLen) 
        {
            dblFreq = (((double) m_eFreqSample * (double) (lngNulCnt - 1)) / (double (lngLastX - lngFirstX  + MEASURE_PITCH))) * m_dbFreqPitch; // - 1
            lngBestMesLen = lngLastX - lngFirstX;
        }
    }
    m_lNulCnt = lngNulCnt;
    m_lBestMesLen = lngBestMesLen;
    if (((0-iMinLev) > iMaxLev) && ((0 - iMinLev) != 0x80)) iMaxLev= (0 - iMinLev);
    //update status label to show test results

    EnterCriticalSection(&m_cr);

    if (dblFreq > m_dbMinFreq) 
    {
        double dNewFreq;
        m_bSetTuningComment = TRUE;

        m_dbFreqSampleQ = (double) lngBestMesLen / (double) wvHdr.dwBytesRecorded * 100.0;

        if (m_dbFreqIn == 0.0)
            dNewFreq = AvgFreq(dblFreq, TRUE);
        else
            dNewFreq = AvgFreq(dblFreq, FALSE);
        m_dbFreqIn = dNewFreq;

        if (m_bTemperamentEnabled)
            dblNoteNr = (log(m_dbFreqIn / m_dbBaseFreq) / log(2.0) * 12.0) + 60.0;
        else
            dblNoteNr = (log(m_dbFreqIn / BASEFREQ) / log(2.0) * 12.0) + 60.0;
        m_iNoteNum = (int) (dblNoteNr + 0.5);
        dblNoteNrFmod = fmod(dblNoteNr, 12.0);
        intNoteNrFmod = (int) floor(dblNoteNrFmod + 0.5);
        if ((intNoteNrFmod == 0) && (dblNoteNrFmod > 11.0) ) intNoteNrFmod+=12;
        if (m_bTemperamentEnabled)
            //Use modified Base Frequency
            dblPitch = ((dblNoteNrFmod - m_dbPitchArray[m_iNoteNum % 12] - (double) intNoteNrFmod) * 100.0);
        else
            //Use 440.0 Hz as Base Frequency
            dblPitch = ((dblNoteNrFmod - (double) intNoteNrFmod) * 100.0);
        m_dbPitch = dblPitch;
        intPitch = (int) dblPitch;
        m_iPitch = intPitch;
        m_bIsTuned = IsTuned(dblPitch);
        if ((iLastNoteNr != m_iNoteNum) && m_bEraseTuned) {
            m_bIsTuned = FALSE;
            iLastNoteNr = m_iNoteNum;
        } else {
            if (m_iFreqQ == 0) {
                m_bIsTuned = FALSE;
                m_bSetTuningComment = FALSE;
            }
        }
        //Keep alive the PDA when there is a stable signal
        if (m_iFreqQ > 1)
            SystemIdleTimerReset();
    } 
    else 
    {
        m_dbFreqIn = 0;

        m_dbFreqSampleQ -= 10.0;
        if (m_dbFreqSampleQ <= 0.0) {
            m_dbFreqSampleQ = 0.0;
            m_iFreqQ = 0;
        }
        if ((m_bEraseTuned) && (m_dbFreqSampleQ < 20)) {
            m_bResetTuningComment = TRUE;
            m_bIsTuned = FALSE;
        }

    }
    if (iMaxLev > m_iTrigger) { //(!(nCnt %2) && (iMaxLev > m_iTrigger))
        //if (m_lngDispX2Cycl < FQ_DISPLAY_BUFFER) m_lngDispXStart =0;
        if (m_lngDispXStart == 0) {
            m_lngDispXStart = FQ_STATIC_DISPLAY_OFFSET;
            m_lngDispX2Cycl = FQ_DISPLAY_BUFFER;
        } else {
            if (m_lngDispX2Cycl - m_lngDispXStart < FQ_DISPLAY_BUFFER) {
                m_lngDispX2Cycl = m_lngDispXStart + FQ_DISPLAY_BUFFER;
            }
        }

        m_lngDispXstep = long (floor(double(m_lngDispX2Cycl - m_lngDispXStart) / 100.0));
        if (m_lngDispXstep == 0) m_lngDispXstep = 1;
        if (m_lngDispXStart + (m_lngDispXstep * FQ_DISPLAY_BUFFER) > lngN) {
            m_lngDispXStart = 0;
            m_lngDispXstep = 1;
        }

        memcpy(m_unchDisplData,wvHdr.lpData, (long) wvHdr.dwBytesRecorded);
        m_iMaxDispLev=(iMaxLev - 1 & 0xF0) + 0x10;

    }
    m_iMaxLev = iMaxLev;
    LeaveCriticalSection(&m_cr);

    PostMessage(m_hWnd, WM_FREQ_UPDATE, 0, 0);
    return; 
}


double CFreqTuner::AvgFreq(double dFreqIn, bool bFirstValue) 
{
    int				i, n;
    int				iLastValue;
    double			dTempTotal;
    bool			bIsFirstValue;


    if (dFreqIn == 0.0) {
        m_dbLastAvgFreq = 1.0;
        return (0.0);
    }
    if (m_dbLastAvgFreq < m_dbMinFreq)
        m_dbLastAvgFreq = 1.0;
    else
        m_dbLastAvgFreq = dFreqIn;
    if (((fabs(1.0 - (dFreqIn / m_dbLastAvgFreq)) > m_dbFMaxDev)) || bFirstValue) {
        //if (bFirstValue) {
        bIsFirstValue = TRUE;
        m_iFreqQ = 0;
    } else {
        bIsFirstValue = FALSE;
    }
    if (bIsFirstValue) {
        double dVal = 0.0;
        for (i = 0; i < m_iAvgMaxScope; i++)
            memcpy(&m_dbAvgFreqArray[i], &dVal, sizeof(double));
        m_iValueIndex = 0;
        m_bFirstFill = TRUE;
    }
    if (m_bFirstFill) {
        m_iFreqQ = m_iValueIndex;
        iLastValue = m_iValueIndex;
    } else {
        m_iFreqQ = m_iAvgMaxScope;
        iLastValue = m_iAvgMaxScope - 1;
    }
    //Store last value in array
    m_dbAvgFreqArray[m_iValueIndex] = dFreqIn;
    if (++m_iValueIndex == m_iAvgMaxScope) {
        m_iValueIndex = 0;
        m_bFirstFill = FALSE;
    }

    //Calculate average
    dTempTotal = 0.0;
    for (n = 0; n <= iLastValue; n++) {
        dTempTotal += m_dbAvgFreqArray[n];
    }
    m_dbLastAvgFreq = dTempTotal / (iLastValue + 1);	

    return (m_dbLastAvgFreq);
}


BOOL CFreqTuner::IsTuned(double dPitch) 
{
    BOOL bOK;

    bOK = ((m_iFreqQ >= FQ_MIN) && (fabs(dPitch) < m_dbNMaxDev));

    return (bOK);
}

BOOL CFreqTuner::GetIsTuned()
{
    BOOL bRet;
    EnterCriticalSection(&m_cr);
    bRet = m_bIsTuned;
    LeaveCriticalSection(&m_cr);
    return bRet;
}

double CFreqTuner::GetFrequency()
{
    double dbRet;
    EnterCriticalSection(&m_cr);
    dbRet = m_dbFreqIn;
    LeaveCriticalSection(&m_cr);
    return dbRet;
}

int CFreqTuner::GetSampleQuality()
{
    int iRet;
    EnterCriticalSection(&m_cr);
    iRet = m_iFreqQ;
    LeaveCriticalSection(&m_cr);
    return iRet;
}

int CFreqTuner::GetSignalStrength()
{
    int iRet;
    EnterCriticalSection(&m_cr);
    iRet = m_iMaxLev;
    LeaveCriticalSection(&m_cr);
    return iRet;
}

//returns a value beteen -50 & 50
int CFreqTuner::GetPcntFromTuned()
{
    int iRet;
    EnterCriticalSection(&m_cr);
    iRet = m_iPitch;
    LeaveCriticalSection(&m_cr);
    return iRet;
}

int CFreqTuner::GetNote()
{
    int iRet;
    EnterCriticalSection(&m_cr);
    iRet = m_iNoteNum;
    LeaveCriticalSection(&m_cr);
    return iRet;
}