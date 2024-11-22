/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: IssSound.cpp 
// Abstract: Class for recording, playing, loading, and saving wave sounds
// 
/***************************************************************************************************************/ 

#include "IssSound.h"
#include <mmreg.h>
#include <math.h>
#include <Msacm.h>

TypeSoundFormat g_sWaveFmtList[] = 
{
    { 1, 8000,  16, 40},
    { 1, 8000,  8,  40},
    { 1, 11025, 8,  40},
    { 1, 11025, 16, 40},
};

#define WaveFmtSize (int) (sizeof(g_sWaveFmtList) / sizeof(TypeSoundFormat))

/********************************
*	Memory Handling Macros		*
*********************************/
#define MemAlloc(dwSize)  VirtualAlloc(NULL, dwSize,MEM_COMMIT, PAGE_READWRITE);
#define MemFree(lp,dwSize) ((VirtualFree(lp,dwSize,MEM_DECOMMIT))? VirtualFree(lp,0,MEM_RELEASE):0)
#define MOD(a,b) (a - ((int) (a / b)) * b)

#define WF_OFFSET_FORMATTAG          20
#define WF_OFFSET_CHANNELS           22
#define WF_OFFSET_SAMPLESPERSEC      24
#define WF_OFFSET_AVGBYTESPERSEC     28
#define WF_OFFSET_BLOCKALIGN         32
#define WF_OFFSET_BITSPERSAMPLE      34
#define WF_OFFSET_DATASIZE           40
#define WF_OFFSET_DATA               44
#define WF_HEADER_SIZE               WF_OFFSET_DATA

CIssSound::CIssSound()
:m_dwWaveInMemSize(0)
,m_hWndMain(NULL)
,m_hWaveInHnd(NULL)
,m_lpWaveInFormat(NULL)
,m_bStopRecording(FALSE)
,m_dwWaveInCurrPos(0)
,m_lpWaveInBuffer(NULL)
,m_dwSilenceTime(0)
,m_dSilenceThreshold(0.0)
,m_iSilenceArraySize(0)
,m_iSilenceArrayPointer(0)
,m_bBlockSilenceFlag(NULL)
,m_dwWaveOutMemSize(0)
,m_hWaveOutHnd(NULL)
,m_lpWaveOutFormat(NULL)
,m_bStopPlaying(FALSE)
,m_dwWaveOutCurrPos(0)
,m_lpWaveOutBuffer(NULL)
,m_iExceptionCount(0)
{
    for(int i=0; i<WAVEINBLOCK_NUM; i++)
        m_lpWaveInHdr[i] = NULL;

    for(int i=0; i<WAVEOUTBLOCK_NUM; i++)
        m_lpWaveOutHdr[i] = NULL;
}

CIssSound::~CIssSound()
{
    // cleanup
    StopPlaySound();
    ClosePlayer();
    StopRecording();
    CloseRecorder();
}

/********************************************************************
Function        GetSoundDeviceName

Arguments:      bWaveInDevie    - grab the wave-in device name
uiDeviceID              - device ID to grab the name
szDeviceName    - return the name

Returns:        TRUE if success

Comments:       grab the desired sound device name
*********************************************************************/
HRESULT CIssSound::GetSoundDeviceName(BOOL bWaveInDevice, UINT uiDeviceID, TCHAR* szDeviceName)
{
    HRESULT hr = S_OK;

    szDeviceName[0] = _T('\0');

    if (bWaveInDevice) 
    {
        WAVEINCAPS win;
        CBARG(waveInGetDevCaps(uiDeviceID, &win, sizeof(win))==MMSYSERR_NOERROR, _T("waveInGetDevCaps failed")); 
        _tcscpy(szDeviceName, win.szPname);
    } 
    else 
    {
        WAVEOUTCAPS wout;
        CBARG(waveOutGetDevCaps(uiDeviceID, &wout, sizeof(wout))==MMSYSERR_NOERROR, _T("waveOutGetDevCaps failed")); 
        _tcscpy(szDeviceName, wout.szPname);
    }

Error:
    return hr;
}

/********************************************************************
Function        StartRecording

Arguments:      uiDeviceID      - sound device to record
hWndCallBack- window to send messages to
szSoundFile     - save to this file

Returns:        

Comments:       Start a new recording and save to file
*********************************************************************/
HRESULT CIssSound::StartRecording(UINT uiDeviceID, HWND hWndCallback, char *szSoundFile)
{
    int  i;
    long lReturn;
    HRESULT hr = S_OK;

    // Copy recording file name
    CBARG(szSoundFile, _T("szSoundFile is NULL"));

    strcpy(m_szWaveFileName,szSoundFile);

    // clear off anything that may be running
    StopRecording();
    CloseRecorder();

    m_hWndMain = hWndCallback;

    // Set supported wave format
    m_lpWaveInFormat = (LPWAVEFORMATEX)calloc(1,sizeof(WAVEFORMATEX));
    CPHR(m_lpWaveInFormat, _T("m_lpWaveInFormat")); 

    for(i=0; i<WaveFmtSize; i++) 
    {
        GetSoundFormat(i, m_lpWaveInFormat);
        if (OpenRecorder(uiDeviceID, hWndCallback, m_lpWaveInFormat))
            break;
    }

    // if we were not able to Open the recorder properly
    CBARG(i < WaveFmtSize, _T("i < WaveFmtSize"));
  
    // Allocate proper memory
    m_dwWaveInMemSize = g_sWaveFmtList[i].dwMaxRecordingSeconds * m_lpWaveInFormat->nSamplesPerSec * m_lpWaveInFormat->wBitsPerSample / 8L;

    m_dwWaveInCurrPos = 0;
    m_lpWaveInBuffer = (LPSTR)MemAlloc(m_dwWaveInMemSize);
    CPHR(m_lpWaveInBuffer, _T("m_lpWaveInBuffer")); 

    for(i=0; i<WAVEINBLOCK_NUM; i++) 
    {
        m_lpWaveInHdr[i] = (LPWAVEHDR)MemAlloc(sizeof(WAVEHDR));
        CPHR(m_lpWaveInHdr[i], _T("m_lpWaveInHdr[i]")); 

        m_lpWaveInHdr[i]->lpData = (LPSTR)MemAlloc(WAVEINBLOCK_SIZE);
        CPHR(m_lpWaveInHdr[i]->lpData, _T("m_lpWaveInHdr[i]->lpData")); 

        m_lpWaveInHdr[i]->dwBufferLength = WAVEINBLOCK_SIZE;
        m_lpWaveInHdr[i]->dwFlags = 0;
        m_lpWaveInHdr[i]->dwLoops = 0;

        lReturn = waveInPrepareHeader(m_hWaveInHnd, m_lpWaveInHdr[i], sizeof(WAVEHDR));
        CBARG(lReturn == MMSYSERR_NOERROR, _T("waveInPrepareHeader failed"));
 
        lReturn = waveInAddBuffer(m_hWaveInHnd, m_lpWaveInHdr[i], sizeof(WAVEHDR));
        if (lReturn != MMSYSERR_NOERROR) 
        {
            // if there is an error we have to clean up
            for(int j=0; j<i+1; j++)
                waveInUnprepareHeader(m_hWaveInHnd, m_lpWaveInHdr[j], sizeof(WAVEHDR));

            CBARG(lReturn == MMSYSERR_NOERROR, _T("waveInAddBuffer failed"));
        }
    }

    m_bStopRecording = FALSE;

    // Setup silence detection variables, these are defaults for now
    m_dwSilenceTime                 = 7;
    m_dSilenceThreshold             = 0.10;
    m_iSilenceArraySize             = ( m_dwSilenceTime * m_lpWaveInFormat->nSamplesPerSec ) /(WAVEINBLOCK_SIZE / m_lpWaveInFormat->nBlockAlign );
    m_iSilenceArrayPointer  = 0;
    m_bBlockSilenceFlag             = (BOOL*)calloc(1, m_iSilenceArraySize * sizeof(BOOL));
    if (m_bBlockSilenceFlag==NULL) 
    {
        for(i=0; i<WAVEINBLOCK_NUM; i++)
            waveInUnprepareHeader(m_hWaveInHnd, m_lpWaveInHdr[i], sizeof(WAVEHDR));
        CPHR(m_bBlockSilenceFlag, _T("m_bBlockSilenceFlag")); 
    }

    for(i=0; i<m_iSilenceArraySize; i++)
        m_bBlockSilenceFlag[i] = FALSE;

    // Start recording
    lReturn = waveInStart(m_hWaveInHnd);
    if (lReturn !=MMSYSERR_NOERROR) 
    {
        for(i=0; i<WAVEINBLOCK_NUM; i++)
            waveInUnprepareHeader(m_hWaveInHnd, m_lpWaveInHdr[i], sizeof(WAVEHDR));

        CBARG(lReturn == MMSYSERR_NOERROR, _T("waveInStart failed"));
    }

Error:

    if(hr != S_OK)
    {
        if (m_lpWaveInBuffer) 
        {
            //MemFree(m_lpWaveInBuffer, m_dwWaveInMemSize);
            VirtualFree(m_lpWaveInBuffer, 0, MEM_RELEASE);
            m_lpWaveInBuffer = NULL;
        }

        for(i=0; i<WAVEINBLOCK_NUM; i++) 
        {
            if (m_lpWaveInHdr[i]) 
            {
                if (m_lpWaveInHdr[i]->lpData)
                    //MemFree(m_lpWaveInHdr[i]->lpData, WAVEINBLOCK_SIZE);
                    VirtualFree(m_lpWaveInHdr[i]->lpData, 0, MEM_RELEASE);

                //MemFree(m_lpWaveInHdr[i], sizeof(WAVEHDR));
                VirtualFree(m_lpWaveInHdr[i], 0, MEM_RELEASE);
                m_lpWaveInHdr[i] = NULL;
            }
        }

        if (m_hWaveInHnd) 
        {
            waveInClose(m_hWaveInHnd);
            m_hWaveInHnd = NULL;
        }

        if (m_lpWaveInFormat) 
        {
            free(m_lpWaveInFormat);
            m_lpWaveInFormat = NULL;
        }

        if (m_bBlockSilenceFlag) 
        {
            free(m_bBlockSilenceFlag);
            m_bBlockSilenceFlag = NULL;
            m_iSilenceArraySize = 0;
        }
    }

    return hr;
}

/********************************************************************
Function        Start PlaySound

Arguments:      uiDeviceID      - sound device
szSoundFile     - wav file to play

Returns:        

Comments:       Start playing a wav file
*********************************************************************/
HRESULT CIssSound::StartPlaySound(UINT uiDeviceID, char *szSoundFile)
{
    HRESULT hr = S_OK;
    HWAVEOUT hwo = {0};
    WAVEHDR whdr = {0};
    MMRESULT mmres = {0};
    UINT devId;
    BOOL bRet;
    HANDLE hDoneEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("DONE_EVENT"));
    CPHR(hDoneEvent, _T("CreateEvent"));

    bRet = LoadFromWaveFile(szSoundFile);
    CBHR(bRet, _T("LoadFromWaveFile failed"));

    // Open audio device
    for (devId = 0; devId < waveOutGetNumDevs(); devId++) 
    {
        mmres = waveOutOpen(&hwo, devId, m_lpWaveOutFormat, (DWORD) hDoneEvent, 0, CALLBACK_EVENT);
        if (mmres == MMSYSERR_NOERROR) 
            break;
    }
    CBARG(mmres == MMSYSERR_NOERROR, _T("mmres == MMSYSERR_NOERROR")); 

    // Initialize wave header
    ZeroMemory(&whdr, sizeof(WAVEHDR));
    whdr.lpData         = new char[m_dwWaveOutMemSize];
    whdr.dwBufferLength = m_dwWaveOutMemSize;
    whdr.dwUser         = 0;
    whdr.dwFlags        = 0;
    whdr.dwLoops        = 0;
    whdr.dwBytesRecorded= 0;
    whdr.lpNext         = 0;
    whdr.reserved       = 0;
    CPHR(whdr.lpData, _T("whdr.lpData not created"));

    // Play buffer
    //waveFile.Read(whdr.lpData, whdr.dwBufferLength);
    CopyMemory(whdr.lpData, m_lpWaveOutBuffer, m_dwWaveOutMemSize);

    mmres = waveOutPrepareHeader(hwo, &whdr, sizeof(WAVEHDR));      
    CBARG(mmres == MMSYSERR_NOERROR, _T("waveOutPrepareHeader"));

    mmres = waveOutWrite(hwo, &whdr, sizeof(WAVEHDR));      
    CBARG(mmres == MMSYSERR_NOERROR, _T("waveOutWrite"));

    // Wait for audio to finish playing
    while (!(whdr.dwFlags & WHDR_DONE))
        WaitForSingleObject(hDoneEvent, INFINITE);

    // Clean up
    mmres = waveOutUnprepareHeader(hwo, &whdr, sizeof(WAVEHDR));    
    CBARG(mmres == MMSYSERR_NOERROR, _T("waveOutUnprepareHeader"));

    mmres = waveOutClose(hwo);
    CBARG(mmres == MMSYSERR_NOERROR, _T("waveOutClose"));

Error:
    if(whdr.lpData)
        delete [] whdr.lpData;

    return hr;
}

/********************************************************************
Function        StartPlaySound

Arguments:      uiDeviceID      - sound device
hWndCallback- window handle to recieve messages
szSoundFile     - wav file to play 

Returns:        

Comments:       Play a wav file to the given device
*********************************************************************/
HRESULT CIssSound::StartPlaySound(UINT uiDeviceID, HWND hWndCallback, char *szSoundFile)
{
    int             i;
    LONG            lReturn;
    double          vAdjustment;
    BOOL            bRet;
    LPWAVEFORMATEX  lpWaveDestFormat;
    HRESULT         hr = S_OK;

    StopPlaySound();
    ClosePlayer();

    m_hWndMain = hWndCallback;

    bRet = LoadFromWaveFile(szSoundFile);
    CBHR(bRet, _T("LoadFromWaveFile"));

    // Open player with proper wave format.
    lpWaveDestFormat = (LPWAVEFORMATEX)calloc(1,sizeof(WAVEFORMATEX));
    CPHR(lpWaveDestFormat, _T("lpWaveDestFormat"));

    memcpy(lpWaveDestFormat,m_lpWaveOutFormat, sizeof(WAVEFORMATEX));

    for(i=-1; i<WaveFmtSize; i++) 
    {
        if (OpenPlayer(uiDeviceID, hWndCallback, lpWaveDestFormat))
            break;
        else
            GetSoundFormat(i+1, lpWaveDestFormat);
    }

    // see if we got an error trying to open the player
    CBARG(i < WaveFmtSize, _T("i < WaveFmtSize"));

    // Volume adjustment
    SetMaxVolume(uiDeviceID, m_hWaveOutHnd);

    vAdjustment = 1.0;

    AdjustVolume(m_lpWaveOutBuffer, m_lpWaveOutFormat, m_dwWaveOutMemSize, vAdjustment, TRUE);

    // if there is Sound conversion needed
    if (i!=-1) 
    {
        char * FAR lpDestBuff;
        DWORD dwOrigBufferSize;

        dwOrigBufferSize = m_dwWaveOutMemSize;

        lpDestBuff = ConvertSoundStream(lpWaveDestFormat,
                                        m_lpWaveOutFormat,
                                        m_lpWaveOutBuffer, 
                                        &m_dwWaveOutMemSize);
        CPHR(lpDestBuff, _T("lpDestBuff"));
        m_lpWaveOutBuffer = lpDestBuff;
    }

    free(lpWaveDestFormat);
    lpWaveDestFormat = NULL;

    m_bStopPlaying          = FALSE;
    m_dwWaveOutCurrPos      = 0;

    // Prepare header
    for(i=0; i<WAVEOUTBLOCK_NUM; i++) 
    {
        DWORD dwBlock;

        m_lpWaveOutHdr[i] = (LPWAVEHDR)MemAlloc(sizeof(WAVEHDR));
        CPHR(m_lpWaveOutHdr[i], _T("m_lpWaveOutHdr[i]"));

        m_lpWaveOutHdr[i]->lpData = (LPSTR)MemAlloc(WAVEOUTBLOCK_SIZE);
        CPHR(m_lpWaveOutHdr[i]->lpData, _T("m_lpWaveOutHdr[i]->lpData"));

        if (m_lpWaveOutFormat->wBitsPerSample > 8)
            ZeroMemory(m_lpWaveOutHdr[i]->lpData, WAVEOUTBLOCK_SIZE);
        else 
            FillMemory(m_lpWaveOutHdr[i]->lpData, WAVEOUTBLOCK_SIZE, 128);

        m_lpWaveOutHdr[i]->dwBufferLength       = WAVEOUTBLOCK_SIZE;
        m_lpWaveOutHdr[i]->dwLoops                      = 0L;
        m_lpWaveOutHdr[i]->dwFlags                      = 0L;

        if (m_dwWaveOutCurrPos < m_dwWaveOutMemSize) 
        {
            LPSTR p;
            dwBlock = m_dwWaveOutMemSize - m_dwWaveOutCurrPos;
            dwBlock = ((dwBlock>WAVEOUTBLOCK_SIZE)? WAVEOUTBLOCK_SIZE: dwBlock);

            p = m_lpWaveOutBuffer + m_dwWaveOutCurrPos;

            CopyMemory(m_lpWaveOutHdr[i]->lpData, p, dwBlock);

            m_lpWaveOutHdr[i]->dwBufferLength = dwBlock;
            m_dwWaveOutCurrPos = m_dwWaveOutCurrPos + dwBlock;
        }

        lReturn = waveOutPrepareHeader(m_hWaveOutHnd, m_lpWaveOutHdr[i], sizeof(WAVEHDR));
        CBARG(lReturn == MMSYSERR_NOERROR, _T("waveOutPrepareHeader"));

        lReturn = waveOutWrite(m_hWaveOutHnd,m_lpWaveOutHdr[i],sizeof(WAVEHDR));
        if (lReturn != MMSYSERR_NOERROR) 
        {
            for(int j=0; j<i+1; j++)
                waveOutUnprepareHeader(m_hWaveOutHnd,m_lpWaveOutHdr[j], sizeof(WAVEHDR));
            CBARG(lReturn == MMSYSERR_NOERROR, _T("waveOutWrite"));
        }
    }


Error:

    if(hr != S_OK)
    {
        if (m_lpWaveOutBuffer) 
        {
            //MemFree(m_lpWaveOutBuffer, m_dwWaveOutMemSize);
            VirtualFree(m_lpWaveOutBuffer, 0, MEM_RELEASE);
            m_lpWaveOutBuffer = NULL;
        }

        for(i=0; i<WAVEOUTBLOCK_NUM; i++) 
        {
            if (m_lpWaveOutHdr[i]) 
            {
                if (m_lpWaveOutHdr[i]->lpData)
                    //MemFree(m_lpWaveOutHdr[i]->lpData, WAVEOUTBLOCK_SIZE);
                    VirtualFree(m_lpWaveOutHdr[i]->lpData, 0, MEM_RELEASE);

                //MemFree(m_lpWaveOutHdr[i], sizeof(WAVEHDR));
                VirtualFree(m_lpWaveOutHdr[i], 0, MEM_RELEASE);
                m_lpWaveOutHdr[i] = NULL;
            }
        }

        if (m_hWaveOutHnd) 
        {
            waveOutClose(m_hWaveOutHnd);
            m_hWaveOutHnd = NULL;
        }

        if (m_lpWaveOutFormat)
        {
            free(m_lpWaveOutFormat);
            m_lpWaveOutFormat = NULL;
        }
    }

    return hr;
}

/********************************************************************
Function        SoundCallback

Arguments:      

Returns:        

Comments:       Callback function for wave file
*********************************************************************/
void CIssSound::SoundCallback(UINT uMsg, UINT wParam, UINT lParam)
{ 
    static  BOOL bReentry = FALSE;

    if (bReentry == TRUE) 
    {
        PostMessage(m_hWndMain, uMsg, wParam, lParam);
        return;
    }
    bReentry = TRUE;

    switch (uMsg) 
    {
    case MM_WIM_DATA: 
        {

            LPWAVEHDR lpWaveHdr = (LPWAVEHDR) lParam;
            if (m_bStopRecording) 
            {
                // Recording needs to be stopped
                if (m_hWaveInHnd!=NULL && lpWaveHdr!=NULL) 
                {
                    waveInUnprepareHeader(m_hWaveInHnd, lpWaveHdr, sizeof(WAVEHDR));
                    ReleaseWaveInHdr(lpWaveHdr);
                    break;
                }

            } 
            else 
            {
                // Recording is proceeding as normal
                if (m_hWaveInHnd!=NULL && lpWaveHdr!=NULL) 
                {
                    // Within the buffer size?
                    if (m_dwWaveInCurrPos+lpWaveHdr->dwBytesRecorded<m_dwWaveInMemSize)
                    {
                        LONG    lReturn;
                        LPSTR   p;

                        // Copy to the wavein buffer
                        p = m_lpWaveInBuffer+m_dwWaveInCurrPos;
                        CopyMemory(     p, lpWaveHdr->lpData, lpWaveHdr->dwBytesRecorded);

                        //      Before move the pointer, check the block silence status.
                        CheckCurrentBlockSilence(lpWaveHdr);

                        // Move the position pointer
                        m_dwWaveInCurrPos = m_dwWaveInCurrPos + lpWaveHdr->dwBytesRecorded;

                        // Preparing and adding the buffer
                        lReturn = waveInPrepareHeader(m_hWaveInHnd, lpWaveHdr, sizeof(WAVEHDR));
                        if (lReturn != MMSYSERR_NOERROR) 
                        {
                            ReleaseWaveInHdr(lpWaveHdr);
                            break;
                        }

                        lReturn = waveInAddBuffer(m_hWaveInHnd, lpWaveHdr, sizeof(WAVEHDR));
                        if (lReturn != MMSYSERR_NOERROR) 
                        {
                            waveInUnprepareHeader(m_hWaveInHnd, lpWaveHdr, sizeof(WAVEHDR));
                            ReleaseWaveInHdr(lpWaveHdr);;
                            break;
                        }

                    }
                    else 
                    {
                        // Exceed the buffer size, stop recording.
                        waveInUnprepareHeader(m_hWaveInHnd, lpWaveHdr, sizeof(WAVEHDR));
                        ReleaseWaveInHdr(lpWaveHdr);
                        StopRecording();
                    }
                }
            }
            break;
        }
    case MM_WOM_DONE: 
        {
            LPWAVEHDR lpWaveHdr = (LPWAVEHDR) lParam;
            if (m_bStopPlaying) 
            {
                // Playing needs to be stopped
                if (m_hWaveOutHnd!=NULL && lpWaveHdr!=NULL) 
                {
                    waveOutUnprepareHeader(m_hWaveOutHnd, lpWaveHdr, sizeof(WAVEHDR));
                    ReleaseWaveOutHdr(lpWaveHdr);
                    break;
                }
            } 
            else 
            {
                // Playing is proceeding as normal
                if (m_hWaveOutHnd!=NULL && lpWaveHdr!=NULL) 
                {
                    // Within the buffer size?      
                    if (m_dwWaveOutCurrPos < m_dwWaveOutMemSize)
                    {
                        LONG    lReturn;
                        DWORD   szBlock;
                        LPSTR   p;

                        // Copy to the wavein buffer
                        szBlock = m_dwWaveOutMemSize - m_dwWaveOutCurrPos;
                        szBlock = ((szBlock>WAVEOUTBLOCK_SIZE)? WAVEOUTBLOCK_SIZE: szBlock);

                        p = m_lpWaveOutBuffer + m_dwWaveOutCurrPos;

                        ZeroMemory(lpWaveHdr->lpData, WAVEOUTBLOCK_SIZE);
                        CopyMemory(lpWaveHdr->lpData, p, szBlock);

                        // Move the position pointer
                        m_dwWaveOutCurrPos =    m_dwWaveOutCurrPos + szBlock;

                        //      Preparing and adding the buffer
                        lReturn = waveOutPrepareHeader(m_hWaveOutHnd, lpWaveHdr, sizeof(WAVEHDR));
                        if (lReturn != MMSYSERR_NOERROR) 
                        {
                            ReleaseWaveOutHdr(lpWaveHdr);
                            break;
                        }

                        lReturn = waveOutWrite(m_hWaveOutHnd, lpWaveHdr, sizeof(WAVEHDR));
                        if (lReturn != MMSYSERR_NOERROR) 
                        {
                            waveOutUnprepareHeader(m_hWaveOutHnd, lpWaveHdr, sizeof(WAVEHDR));
                            ReleaseWaveOutHdr(lpWaveHdr);
                            break;
                        }

                    } 
                    else 
                    {
                        // Exceed the buffer size, stop playing
                        waveOutUnprepareHeader(m_hWaveOutHnd, lpWaveHdr, sizeof(WAVEHDR));
                        ReleaseWaveOutHdr(lpWaveHdr);
                        StopPlaySound();
                    }
                }
            }
            break;
        }

    }

    bReentry = FALSE;

}

/********************************************************************
Function        ReleaseWaveInHdr

Arguments:      

Returns:        

Comments:       Clean up all memory for the wave in header
*********************************************************************/
void CIssSound::ReleaseWaveInHdr(LPWAVEHDR lpWaveHdr)
{
    int i;                                                                                                  
    BOOL bAllReleased = TRUE;                                                               

    // Release this wavein buffer
    if (lpWaveHdr->lpData!=NULL)                                                    
    {                                                                                                               
        //MemFree(lpWaveHdr->lpData, WAVEINBLOCK_SIZE);    
        VirtualFree(lpWaveHdr->lpData, 0, MEM_RELEASE);
        lpWaveHdr->lpData = NULL;                                                       
    }                                                                                                               

    // Check the all allocated                                                      
    // buffers are returned.
    for(i=0; i<WAVEINBLOCK_NUM; i++)                                                
    {                                                                                                               
        if (m_lpWaveInHdr[i]->lpData!=NULL)                                     
        {                                                                                                       
            bAllReleased = FALSE;                                                   
            break;                                                                                  
        }                                                                                                       
    }                                                                                                               

    // if all allocated buffers are                         
    // returned, then close recorder.                               
    if (bAllReleased)                                                                               
        CloseRecorder();                                                                                                                                                                                                                                        
}

/********************************************************************
Function        ReleaseWaveOutHdr

Arguments:      

Returns:        

Comments:       Clean up all memory for the wave out header
*********************************************************************/
void CIssSound::ReleaseWaveOutHdr(LPWAVEHDR lpWaveHdr)
{
    int i;                                                                                                  
    BOOL bAllReleased = TRUE;                                                               

    // Release this wavein buffer
    if (lpWaveHdr->lpData!=NULL)                                                    
    {                                                                                                               
        //MemFree(lpWaveHdr->lpData, WAVEOUTBLOCK_SIZE); 
        VirtualFree(lpWaveHdr->lpData, 0, MEM_RELEASE);
        lpWaveHdr->lpData = NULL;                                                       
    }                                                                                                               

    // Check the all allocated
    // buffers are returned.                                                
    for(i=0; i<WAVEOUTBLOCK_NUM; i++)                                               
    {                                                                                                               
        if (m_lpWaveOutHdr[i]->lpData!=NULL)                            
        {                                                                                                       
            bAllReleased = FALSE;                                                   
            break;                                                                                  
        }                                                                                                       
    }                                                                                                               

    // if all allocated buffers are 
    // returned, then close recorder.                                               
    if (bAllReleased)                                                                               
        ClosePlayer();                                                                                                                                                                                                  
}

/********************************************************************
Function        IsWaveIOInUse

Arguments:      

Returns:        

Comments:       Are either the wave-in or wave-out devices in use?
*********************************************************************/
BOOL CIssSound::IsWaveIOInUse()
{
    if (m_hWaveOutHnd!=NULL)        return TRUE;
    if (m_hWaveInHnd!=NULL)         return TRUE;

    return FALSE;
}

/********************************************************************
Function        AccessMonoSample

Arguments:      

Returns:        

Comments:       Get sample value from 0.0 to 1.0 both 8 or 16 bits
*********************************************************************/
double CIssSound::AccessMonoSample(int iSpec, LPSTR lpWaveBuffer, LPWAVEFORMATEX lpWaveFormat, DWORD dwPos, double dbSmp)
{
    double dbSample = 0.0;

    // make sure the buffer's are not NULL
    if (lpWaveFormat!=NULL && lpWaveBuffer!=NULL) 
    {
        double dbOrgSmp;
        unsigned char *cSample8;
        short int *iSample16;

        if (lpWaveFormat->wBitsPerSample <= 8) 
        {
            //Get 8 bits dbSample
            cSample8 = (unsigned char *) (lpWaveBuffer + dwPos * lpWaveFormat->nBlockAlign);
            if (iSpec == READMONO_SAMPLE) 
            {
                dbOrgSmp = ((double) *cSample8) - 128.0;
                dbSample = (dbOrgSmp / ((dbOrgSmp<0)? 128.0: 127.0));
            } 
            else 
                *cSample8 = (unsigned char) (dbSmp * ((dbSmp<0)? 128.0:127.0) + 128.0);

        } 
        else 
        {
            // Get 16 bits sample
            iSample16 = (short int *) (lpWaveBuffer + dwPos * lpWaveFormat->nBlockAlign);
            if (iSpec == READMONO_SAMPLE) 
            {
                dbOrgSmp = (double) *iSample16;
                dbSample = (dbOrgSmp / ((dbOrgSmp<0)? 32768.0:32767.0));
            } 
            else
                *iSample16 = (short int) (dbSmp * ((dbSmp<0)? 32768.0:32767.0));
        }
    }
    return dbSample;
}

/********************************************************************
Function        AdjustVolume

Arguments:      

Returns:        

Comments:       Adjust the volume of a wave sample
*********************************************************************/
BOOL CIssSound::AdjustVolume(LPSTR lpWaveBuffer, LPWAVEFORMATEX lpWaveFormat, DWORD dwTotBytes, double dbAdjRatio, BOOL bAutomaticVol)
{
    DWORD i;

    // sanity check
    if (lpWaveFormat == NULL || lpWaveBuffer == NULL)
        return FALSE;

    DWORD dwSamples;
    double dbMin, dbMax, dbSmp, dbOrgSmp;

    dwSamples = dwTotBytes / lpWaveFormat->nBlockAlign;

    // Get min and max samples      
    dbMin = 1.0;
    dbMax = -1.0;
    for(i=0; i<dwSamples; i++) 
    {
        dbSmp = ReadMonoSample(lpWaveBuffer, lpWaveFormat, i);
        if (dbSmp>dbMax) dbMax = dbSmp;
        if (dbSmp<dbMin) dbMin = dbSmp;
    }

    for(i=0; i<dwSamples; i++) 
    {
        dbOrgSmp = ReadMonoSample(lpWaveBuffer, lpWaveFormat, i);

        // if we're automatically adjusting the Volume
        if (bAutomaticVol) 
            dbSmp = ((dbOrgSmp - dbMin) * 2.0 / (dbMax - dbMin) - 1.0) * dbAdjRatio;
        else
            dbSmp = dbOrgSmp * dbAdjRatio;

        WriteMonoSample(lpWaveBuffer, lpWaveFormat, i, dbSmp);
    }

    return TRUE;
}

/********************************************************************
Function        SetMaxVolume

Arguments:      uiDeviceID      - device to change the volume
hWaveDev        - change this device's volume

Returns:        

Comments:       Set the volume to maximum
*********************************************************************/
BOOL CIssSound::SetMaxVolume(UINT uiDeviceID, HWAVEOUT hWaveDev)
{
    WAVEOUTCAPS wc;
    LONG lMmr, lReturn;

    lReturn = FALSE;
    lMmr    = waveOutGetDevCaps(uiDeviceID, &wc, sizeof(wc));
    if (lMmr == MMSYSERR_NOERROR) 
    {
        if (wc.dwSupport & WAVECAPS_VOLUME)
        {
            DWORD dwVolume  = 0x0ffffffff;
            lReturn                 = waveOutSetVolume(hWaveDev, dwVolume);
            lReturn                 = ((lReturn == MMSYSERR_NOERROR)? TRUE: FALSE);
        }
    }

    return lReturn;
}

/********************************************************************
Function        CheckCurrentBlockSilence

Arguments:      lpWaveHdr                       - wave header to check the volume
dbSilenceThreshold      - defined silence levels

Returns:        TRUE if the block is silent

Comments:       Clean up all memory and close all threads
*********************************************************************/
BOOL CIssSound::CheckCurrentBlockSilence(LPWAVEHDR lpWaveHdr, double dbSilenceThreshold)
{
    double  dbS;
    BOOL    bSilence = TRUE;
    DWORD   dwStart, dwEnd, dwSamples;

    if(NULL == m_lpWaveInFormat || lpWaveHdr == NULL || m_lpWaveInBuffer)
        return TRUE;

    // Get sample number
    dwSamples       = lpWaveHdr->dwBytesRecorded / m_lpWaveInFormat->nBlockAlign;
    dwStart         = m_dwWaveInCurrPos / m_lpWaveInFormat->nBlockAlign;
    dwEnd           = dwStart + dwSamples;

    for(DWORD i=dwStart; i<dwEnd; i++) 
    {
        dbS = ReadMonoSample(m_lpWaveInBuffer, m_lpWaveInFormat, i);
        if (fabs(dbS) > dbSilenceThreshold) 
        {
            bSilence = FALSE;
            break;
        }
    }

    if (m_bBlockSilenceFlag != NULL) 
    {
        m_bBlockSilenceFlag[m_iSilenceArrayPointer] = bSilence;

        m_iSilenceArrayPointer++;
        m_iSilenceArrayPointer = MOD(m_iSilenceArrayPointer, m_iSilenceArraySize);
    }

    return bSilence;
}

/********************************************************************
Function        IsSilence

Arguments:      

Returns:        True if the silence flags are all true

Comments:       Check the silence flags to see if the whole thing is silent
*********************************************************************/
BOOL CIssSound::IsSilence()
{
    int i;
    LONG lReturn = TRUE;

    if(m_hWaveInHnd == NULL || m_bBlockSilenceFlag == NULL || m_iSilenceArraySize <= 0)
        return TRUE;

    for(i=0; i<m_iSilenceArraySize; i++) 
    {
        if (m_bBlockSilenceFlag[i] == FALSE) 
        {
            lReturn = FALSE;
            break;
        }
    }

    return lReturn;
}

/********************************************************************
Function        GetSoundFormat

Arguments:      iIndex          - which sound format we're using
lpWaveFormat- retrieve the sound format and place here

Returns:        TRUE if successful

Comments:       Get the desired sound format
*********************************************************************/
BOOL CIssSound::GetSoundFormat(int iIndex, LPWAVEFORMATEX lpWaveFormat)
{
    if (iIndex < 0 || iIndex >= WaveFmtSize)
        return FALSE;

    lpWaveFormat->wFormatTag                = WAVE_FORMAT_PCM;
    lpWaveFormat->nChannels                 = g_sWaveFmtList[iIndex].wChannels;
    lpWaveFormat->nSamplesPerSec    = g_sWaveFmtList[iIndex].dwSamplingRate;
    lpWaveFormat->nBlockAlign               = g_sWaveFmtList[iIndex].wSamplingBits / 8;
    lpWaveFormat->nAvgBytesPerSec   = lpWaveFormat->nBlockAlign * lpWaveFormat->nSamplesPerSec;
    lpWaveFormat->wBitsPerSample    = g_sWaveFmtList[iIndex].wSamplingBits;
    lpWaveFormat->cbSize                    = sizeof(WAVEFORMATEX);
    return TRUE;
}

char FAR * CIssSound::ConvertSoundStream(LPWAVEFORMATEX lpWaveDestFormat, LPWAVEFORMATEX lpWaveSrcFormat,char FAR *     lpSrcBuffer, DWORD* dwSrcBufferSize)
{
    MMRESULT                        lReturn;
    HACMSTREAM                      hStream = NULL;
    ACMSTREAMHEADER         hdrStream;
    static char FAR *       lpDestBuffer;

    DWORD dwDestBufferSize, dwSrcTotalSamples, dwSrcBytesPerSample;

    // Calculate buffer size
    dwSrcBytesPerSample = lpWaveSrcFormat->wBitsPerSample / 8;
    dwSrcTotalSamples = (*dwSrcBufferSize) / dwSrcBytesPerSample ;

    dwDestBufferSize = dwSrcTotalSamples * lpWaveDestFormat->nSamplesPerSec / lpWaveSrcFormat->nSamplesPerSec * (lpWaveDestFormat->wBitsPerSample / 8);

    // Allocate memory
    lpDestBuffer = (char FAR *) MemAlloc(dwDestBufferSize);
    if (lpDestBuffer==NULL)
        goto cleanup;

    //      Stream Conversion
    hStream = NULL;
    lReturn = acmStreamOpen(&hStream, NULL, 
        lpWaveSrcFormat, lpWaveDestFormat,
        (DWORD) 0, (DWORD) 0, 0, 
        ACM_STREAMOPENF_NONREALTIME);

    if (lReturn) 
        goto cleanup;

    memset(&hdrStream, 0, sizeof(ACMSTREAMHEADER)); // Must clean the buffer

    hdrStream.cbStruct              = sizeof(ACMSTREAMHEADER);
    hdrStream.pbSrc                 = (LPBYTE)lpSrcBuffer;
    hdrStream.cbSrcLength   = (* dwSrcBufferSize);
    hdrStream.pbDst                 = (LPBYTE)lpDestBuffer;
    hdrStream.cbDstLength   = dwDestBufferSize;

    lReturn = acmStreamPrepareHeader(hStream, &hdrStream, 0);
    if (lReturn) 
        goto cleanup;

    lReturn = acmStreamConvert(hStream, &hdrStream, 0);
    if (lReturn) 
    {
        acmStreamUnprepareHeader(hStream, &hdrStream, 0);
        goto cleanup;
    }

    lReturn = acmStreamUnprepareHeader(hStream, &hdrStream, 0);
    if (lReturn) 
        goto cleanup;

    acmStreamClose(hStream, 0);
    *dwSrcBufferSize = dwDestBufferSize;

    return lpDestBuffer;

cleanup:

    if (hStream)
        acmStreamClose(hStream, 0);

    if (lpDestBuffer)
        //MemFree(lpDestBuffer, dwDestBufferSize);
        VirtualFree(lpDestBuffer, 0, MEM_RELEASE);

    return NULL;
}

/********************************************************************
Function        OpenRecorder

Arguments:      uiDeviceID              - the wave - in device
hWndCalback             - window handle to recieve messages
lpWaveFormat    - Format needed for recording

Returns:        TRUE if successful

Comments:       Open the recording
*********************************************************************/
BOOL CIssSound::OpenRecorder(UINT uiDeviceID, HWND hWndCallback, LPWAVEFORMATEX lpWaveFormat)
{
    LONG                    lReturn;
    DWORD                   dwOpenFlag;

    // see if we're already open
    if (m_hWaveInHnd)
        return TRUE;

    DWORD dwCallBK;

    lReturn = waveInOpen(NULL, 
        uiDeviceID, 
        lpWaveFormat, 
        0, 0, 
        WAVE_FORMAT_QUERY);

    dwCallBK        = (DWORD) hWndCallback;
    dwOpenFlag      = CALLBACK_WINDOW;

    // if we returned an error we have to add another flag
    if (lReturn != MMSYSERR_NOERROR)
        dwOpenFlag = dwOpenFlag  | WAVE_MAPPED;

    lReturn = waveInOpen(&m_hWaveInHnd, 
        uiDeviceID, lpWaveFormat,
        dwCallBK, 0, 
        dwOpenFlag);
    //lReturn = waveInOpen(&m_hWaveInHnd, uiDeviceID, lpWaveFormat, 0,0,0);
    if (lReturn == MMSYSERR_NOERROR)
        return TRUE;

    m_hWaveInHnd = NULL;
    return FALSE;
}

/********************************************************************
Function        OpenPlayer

Arguments:      uiDeviceID              - the wave - in device
hWndCalback             - window handle to recieve messages
lpWaveFormat    - Format needed for playing

Returns:        TRUE if successful

Comments:       Open the Player
*********************************************************************/
BOOL CIssSound::OpenPlayer(UINT uiDeviceID, HWND hWndCallback, LPWAVEFORMATEX lpWaveFormat)
{
    LONG                    lReturn;
    DWORD                   dwOpenFlag;

    // see if it's already open
    if(m_hWaveOutHnd)
        return TRUE;

    DWORD dwCallBK;

    // let's open our Wave out
    lReturn = waveOutOpen(NULL, 
        uiDeviceID, lpWaveFormat, 
        0,0, 
        WAVE_FORMAT_QUERY);

    dwOpenFlag      = CALLBACK_WINDOW;
    dwCallBK        = (DWORD) hWndCallback;

    // if there is an error we try again
    if (lReturn != MMSYSERR_NOERROR)
        dwOpenFlag = dwOpenFlag | WAVE_MAPPED;

    lReturn = waveOutOpen(&m_hWaveOutHnd, 
        uiDeviceID, lpWaveFormat,
        dwCallBK, 0, 
        dwOpenFlag);
    if (lReturn == MMSYSERR_NOERROR) 
        return TRUE;

    // we couldn't open anything
    m_hWaveOutHnd = NULL;
    return FALSE;
}

HRESULT CIssSound::StopRecording()
{
    HRESULT hr = S_OK;
    CBARG(m_hWaveInHnd && !m_bStopRecording, _T("m_hWaveInHnd && !m_bStopRecording"));

    BOOL bRet = waveInReset(m_hWaveInHnd);
    bRet = ((bRet==MMSYSERR_NOERROR)? (m_bStopRecording=TRUE):FALSE);
    CBHR(bRet, _T("waveInReset"));
Error:
    return hr;
}

HRESULT CIssSound::StopPlaySound()
{
    HRESULT hr = S_OK;
    CBARG(m_hWaveOutHnd && !m_bStopPlaying, _T("m_hWaveOutHnd && !m_bStopPlaying"));

    BOOL bRet = waveOutReset(m_hWaveOutHnd);
    bRet = ((bRet == MMSYSERR_NOERROR)? (m_bStopPlaying=TRUE):FALSE);
    CBHR(bRet, _T("waveOutReset"));
Error:
    return hr;
}

void CIssSound::CloseRecorder(void)
{
    static int bReentry = FALSE;

    // see if we're closed already
    if(m_hWaveInHnd == NULL)
        return;

    if (!bReentry) 
    {
        bReentry = TRUE;

        waveInClose(m_hWaveInHnd);
        SaveToWaveFile(m_szWaveFileName);
        //MemFree(m_lpWaveInBuffer, m_dwWaveInMemSize);
        VirtualFree(m_lpWaveInBuffer, 0, MEM_RELEASE);
        m_lpWaveInBuffer = NULL;

        for(int i=0; i<WAVEINBLOCK_NUM; i++) 
        {
            //MemFree(m_lpWaveInHdr[i], sizeof(WAVEHDR));
            VirtualFree(m_lpWaveInHdr[i], 0, MEM_RELEASE);
            m_lpWaveInHdr[i] = NULL;
        }

        free(m_lpWaveInFormat);
        m_lpWaveInFormat        = NULL;
        m_hWaveInHnd            = NULL;

        free(m_bBlockSilenceFlag);
        m_bBlockSilenceFlag = NULL;
        m_iSilenceArraySize = 0;

        m_bStopRecording        = FALSE;
        bReentry                        = FALSE;
    }
}

void CIssSound::ClosePlayer(void)
{
    static int bReentry = FALSE;

    if(m_hWaveOutHnd == NULL)
        return;

    if (!bReentry) 
    {
        bReentry = TRUE;

        waveOutClose(m_hWaveOutHnd);
        //MemFree(m_lpWaveOutBuffer, m_dwWaveOutMemSize);
        VirtualFree(m_lpWaveOutBuffer, 0, MEM_RELEASE);
        m_lpWaveOutBuffer = NULL;

        for(int i=0; i<WAVEOUTBLOCK_NUM; i++) 
        {
            //MemFree(m_lpWaveOutHdr[i], sizeof(WAVEHDR));
            VirtualFree(m_lpWaveOutHdr[i], 0, MEM_RELEASE);
            m_lpWaveOutHdr[i] = NULL;
        }

        free(m_lpWaveOutFormat);
        m_lpWaveOutFormat = NULL;
        m_hWaveOutHnd = NULL;

        m_bStopPlaying = FALSE;
        bReentry = FALSE;
    }
}

/********************************************************************
Function        SaveToWaveFile

Arguments:      szSoundFile

Returns:        TRUE if successful

Comments:       Save the wave-in to a sound file.  NOTE: the calling function
is responsible for allocating and freeing the memory space
*********************************************************************/
BOOL CIssSound::SaveToWaveFile(char     *szSoundFile)
{
    // check if we valid variables to save
    if(m_lpWaveInFormat == NULL || m_lpWaveInBuffer == NULL)
        return FALSE;

    int iRes;
    FILE* fp = fopen(szSoundFile, "wb");
    if(!fp)
        return FALSE;

    // write the riff section
    fprintf(fp, "RIFF****WAVE");

    // write the format section
    fprintf(fp, "fmt ");
    DWORD dwSize = sizeof(WAVEFORMATEX);
    fwrite(&dwSize, 4, 1, fp);
    fwrite(m_lpWaveInFormat, dwSize, 1, fp);

    // now write the data section
    fprintf(fp, "data****");
    fwrite(m_lpWaveInBuffer, m_dwWaveInCurrPos, 1, fp);

    // fill in the RIFF info size
    iRes = fseek(fp, 4, SEEK_SET);
    dwSize = 20 + sizeof(WAVEFORMATEX) + 4 + m_dwWaveInCurrPos;
    fwrite(&dwSize, 4, 1, fp);

    // fill in the data info size
    iRes = fseek(fp, 20 + sizeof(WAVEFORMATEX) + 4, SEEK_SET);
    fwrite(&m_dwWaveInCurrPos, 4, 1, fp);

    // successful writing
    fclose(fp);

    return TRUE;
}

/********************************************************************
Function        LoadFromWaveFile

Arguments:      szSoundFile - file to load

Returns:        TRUE if successful

Comments:       Load a wave from file.  NOTE: the calling function is 
responsible for freeing the memory space only.
*********************************************************************/
BOOL CIssSound::LoadFromWaveFile(char *szSoundFile)
{

    BYTE aHeader[WF_HEADER_SIZE];

    // open the file
    FILE* fp = fopen(szSoundFile, "rb");
    if(!fp)
        return FALSE;

    // get the length of the data portion
    int iRes;
    iRes = fseek(fp, 0, SEEK_END);
    m_dwWaveOutMemSize = ftell(fp) - WF_HEADER_SIZE;

    // create the memory and fill it with what we have read in
    m_lpWaveOutFormat = (LPWAVEFORMATEX) calloc(1,sizeof(WAVEFORMATEX));
    if (m_lpWaveOutFormat == NULL) 
        goto cleanup;

    // set the format attribute members
    iRes = fseek(fp, 0, SEEK_SET);
    fread(aHeader, 1, WF_HEADER_SIZE, fp);
    m_lpWaveOutFormat->wFormatTag           = *((WORD*) (aHeader + WF_OFFSET_FORMATTAG));
    m_lpWaveOutFormat->nChannels            = *((WORD*) (aHeader + WF_OFFSET_CHANNELS));
    m_lpWaveOutFormat->nSamplesPerSec       = *((DWORD*)(aHeader + WF_OFFSET_SAMPLESPERSEC));
    m_lpWaveOutFormat->nAvgBytesPerSec      = *((DWORD*)(aHeader + WF_OFFSET_AVGBYTESPERSEC));
    m_lpWaveOutFormat->nBlockAlign          = *((WORD*) (aHeader + WF_OFFSET_BLOCKALIGN));
    m_lpWaveOutFormat->wBitsPerSample       = *((WORD*) (aHeader + WF_OFFSET_BITSPERSAMPLE));

    // declare our data buffer
    m_lpWaveOutBuffer = (LPSTR)MemAlloc(m_dwWaveOutMemSize);
    if (m_lpWaveOutBuffer == NULL) 
        goto cleanup;

    // read the data in
    fread(m_lpWaveOutBuffer, 1, m_dwWaveOutMemSize, fp);

    fclose(fp);

    // success!
    return m_dwWaveOutMemSize;


cleanup:

    if (m_lpWaveOutBuffer)  
    {
        //MemFree(m_lpWaveOutBuffer, m_dwWaveOutMemSize);
        VirtualFree(m_lpWaveOutBuffer, 0, MEM_RELEASE);
        m_lpWaveOutBuffer = NULL;
    }

    if (m_lpWaveOutFormat) 
    {
        free(m_lpWaveOutFormat);
        m_lpWaveOutFormat = NULL;
    }

    fclose (fp);
    return FALSE;
}

