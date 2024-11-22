/***************************************************************************************************************/ 
// © 2003-2008 Implicit Software Solutions Inc. All Rights Reserved 
// 
// This source code is licensed under Implicit Software Solutions Inc.   
//  
// Any use of this software is limited to those who have agreed to the License. 
// 
// FileName: IssSound.h 
// Abstract: Class for recording, playing, loading, and saving wave sounds
// 
/***************************************************************************************************************/ 
#pragma once

#include "windows.h"
#include "mmsystem.h"
#include "IssDebug.h"

struct TypeSoundFormat
{
    WORD    wChannels;
    DWORD   dwSamplingRate;
    WORD    wSamplingBits;
    WORD    dwMaxRecordingSeconds;
};

#define         WAVEINBLOCK_SIZE                        4096
#define         WAVEINBLOCK_NUM                         2
#define         WAVEOUTBLOCK_SIZE                       4096
#define         WAVEOUTBLOCK_NUM                        2
#define         MAX_PATH_LEN                            1000
#define         WRITEMONO_SAMPLE                        2
#define         READMONO_SAMPLE                         1

class CIssSound
{
public:
    CIssSound(void);
    ~CIssSound(void);

    HRESULT StartRecording(UINT uiDeviceID, HWND hWndCallback, char* szFileName);
    HRESULT StopRecording();
    HRESULT StartPlaySound(UINT uiDeviceID, char *szSoundFile);
    HRESULT StartPlaySound(UINT uiDeviceID, HWND hWndCallback, char* szFileName);
    HRESULT StopPlaySound();
    BOOL    IsRecording(){return !m_bStopRecording;};
    BOOL    IsPlaying(){return !m_bStopPlaying;};
    void    SoundCallback(UINT uMsg, UINT wParam, UINT lParam);
    BOOL    IsWaveIOInUse();
    BOOL    IsSilence();
    int     GetNumWaveInDevices(){return waveInGetNumDevs();};
    int     GetNumWaveOutDevices(){return waveOutGetNumDevs();};
    HRESULT GetWaveInDeviceName(UINT uiDeviceID, TCHAR* szDeviceName){return GetSoundDeviceName(TRUE, uiDeviceID, szDeviceName);};
    HRESULT GetWaveOutDeviceName(UINT uiDeviceID, TCHAR* szDeviceName){return GetSoundDeviceName(FALSE, uiDeviceID, szDeviceName);};


private:        // Functions
    HRESULT GetSoundDeviceName(BOOL bWaveInDevice, UINT uiDeviceID, TCHAR* szDeviceName);
    BOOL    SaveToWaveFile(char* szFileName);
    BOOL    LoadFromWaveFile(char* szFileName);
    BOOL    OpenPlayer(UINT uiDeviceID, HWND hWndCallback, LPWAVEFORMATEX lpWaveFormat);
    BOOL    OpenRecorder(UINT uiDeviceID, HWND hWndCallback, LPWAVEFORMATEX lpWaveFormat);
    void    CloseRecorder();
    void    ClosePlayer();
    BOOL    CheckCurrentBlockSilence(LPWAVEHDR lpWaveHdr, double dbSilenceThreshold = 0.10);
    BOOL    SetMaxVolume(UINT uiDeviceID, HWAVEOUT hWaveDev);
    BOOL    AdjustVolume(LPSTR lpWaveBuffer, LPWAVEFORMATEX lpWaveFormat, DWORD dwTotBytes, double dbAdjRatio, int iAdjMode);
    double  AccessMonoSample(int iSpec, LPSTR lpWaveBuffer, LPWAVEFORMATEX lpWaveFormat, DWORD dwPos, double dbSmp);
    double  ReadMonoSample(LPSTR lpWaveBuffer, LPWAVEFORMATEX lpWaveFormat, DWORD dwPos){return AccessMonoSample(READMONO_SAMPLE, lpWaveBuffer, lpWaveFormat, dwPos, 0.0);};
    double  WriteMonoSample(LPSTR lpWaveBuffer, LPWAVEFORMATEX lpWaveFormat, DWORD dwPos, double dbSmp){return AccessMonoSample(WRITEMONO_SAMPLE, lpWaveBuffer, lpWaveFormat, dwPos, dbSmp);};
    BOOL    GetSoundFormat(int iIndex, LPWAVEFORMATEX lpWaveFormat);
    char FAR * ConvertSoundStream(LPWAVEFORMATEX    lpWaveDestFormat, LPWAVEFORMATEX lpWaveSrcFormat,char FAR *     lpSrcBuffer, DWORD* dwSrcBufferSize);
    void    ReleaseWaveInHdr(LPWAVEHDR lpWaveHdr);
    void    ReleaseWaveOutHdr(LPWAVEHDR lpWaveHdr);


private:        // Variables
    HWND                            m_hWndMain;
    DWORD                           m_dwWaveInMemSize;
    HWAVEIN                         m_hWaveInHnd;
    LPWAVEHDR                       m_lpWaveInHdr[WAVEINBLOCK_NUM];
    LPWAVEFORMATEX                  m_lpWaveInFormat;
    BOOL                            m_bStopRecording;

    DWORD                           m_dwWaveInCurrPos;
    LPSTR                           m_lpWaveInBuffer;

	/************************************
	*	Silence detection parameters	*
	*************************************/
    DWORD                           m_dwSilenceTime;
    double                          m_dSilenceThreshold;
    int                             m_iSilenceArraySize;
    int                             m_iSilenceArrayPointer;
    BOOL*                           m_bBlockSilenceFlag;

	/************************************
	*	Internal variables for player	*
	*************************************/
    DWORD                           m_dwWaveOutMemSize;
    HWAVEOUT                        m_hWaveOutHnd;
    LPWAVEHDR                       m_lpWaveOutHdr[WAVEOUTBLOCK_NUM];
    LPWAVEFORMATEX                  m_lpWaveOutFormat;
    BOOL                            m_bStopPlaying;
    DWORD                           m_dwWaveOutCurrPos;
    LPSTR                           m_lpWaveOutBuffer;

	/************************
	*	Exception handle	*
	*************************/
    int                             m_iExceptionCount;

	/********************
	*	Wave File Name	*
	*********************/
    char                            m_szWaveFileName[MAX_PATH_LEN];

};
