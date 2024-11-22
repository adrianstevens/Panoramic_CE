#pragma once
#include "hss.h"
#include "..\..\..\HekkusSound\include\hssdata.h"

#include "IssVector.h"

#pragma comment( lib, "hss.lib" )

typedef struct _HSSCONFIG {
	DWORD        dwSoundFrequency;     // (Sound) Sound output frequency (11025 - 22050 - 44100)
	// dwSoundFrequency == 0 -> no sound!
	DWORD        dwSoundBPS;           // (Sound) Set sound bit per sample output (8 - 16)
	// dwSoundBPS == 0 -> try first 16bit then 8bit
	BOOL         bSoundStereo;         // (Sound) Set sound stereo or mono output
	DWORD        dwMusicChannels;      // (Sound) number of music channels
	DWORD        dwSoundChannels;      // (Sound) number of sound channels
} HSSCONFIG;

class CHSound
{
public:
	CHSound(void);
	~CHSound(void);

	BOOL	Initialize(HINSTANCE hInst);
	void	Destroy();
//	BOOL	LoadMOD(UINT uiMOD, BOOL bLoop = TRUE);
	int		LoadSFX(UINT uiSFX);
	int		LoadSFX(TCHAR* szFX);
	void	PlaySFX(int iIndex);
//	void	PlayMOD();
//	void	PauseMOD();
//	void	ResumeMOD();
//	int		GetVolumeMOD();
//	void	SetVolumeMOD(int iVolume);
	int		GetVolumeSFX();
	void	SetVolumeSFX(int iVolume);
	BOOL	IsLoaded(){return (m_hInst?TRUE:FALSE);};
	void    StopSFX(){m_sound.stopSounds();};



private:
	HINSTANCE	m_hInst;
	
	HSSCONFIG	m_hssConfig;

	hssSpeaker	m_sound;

	//hssMusic for music ....

	CIssVector<hssSound>		m_arrSFX;   // our soundfx objs

};