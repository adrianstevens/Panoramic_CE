#include "HSound.h"

CHSound::CHSound(void)
:m_hInst(NULL)
{
	::ZeroMemory(&m_hssConfig, sizeof(HSSCONFIG));

	// set config.dwSoundFrequency = 0 to disable sound
	m_hssConfig.dwSoundFrequency= 22050;

	// if you set config.dwSoundBPS to 0 then the apps try to open Hekkus with 
	// 16bit output, if it fails then it will use 8bit
	m_hssConfig.dwSoundBPS		= 0;
	m_hssConfig.bSoundStereo	= true;
	m_hssConfig.dwMusicChannels	= 1;
	m_hssConfig.dwSoundChannels	= 16;
}

CHSound::~CHSound(void)
{
	Destroy();
}


BOOL CHSound::Initialize(HINSTANCE hInst)
{
	Destroy();

	m_hInst = hInst;

	// Open sound
	if (m_hssConfig.dwSoundFrequency != 0)
	{
		if (m_hssConfig.dwSoundBPS == 0)
		{
			if (m_sound.open(m_hssConfig.dwSoundFrequency, 
				16, 
				m_hssConfig.bSoundStereo == TRUE, 
				m_hssConfig.dwMusicChannels, 
				m_hssConfig.dwSoundChannels) == false)
				if (m_sound.open(m_hssConfig.dwSoundFrequency, 
					8, 
					m_hssConfig.bSoundStereo == TRUE, 
					m_hssConfig.dwMusicChannels, 
					m_hssConfig.dwSoundChannels) == false) 
					return FALSE;
		}
		else 
		{
			if (m_sound.open(m_hssConfig.dwSoundFrequency, 
				m_hssConfig.dwSoundBPS, 
				m_hssConfig.bSoundStereo == TRUE, 
				m_hssConfig.dwMusicChannels, 
				m_hssConfig.dwSoundChannels) == false) 
				return FALSE;
		}
	}
	return TRUE;
}

void CHSound::Destroy()
{
	m_hInst = NULL;
	m_sound.close();

	for(int i=0; i<m_arrSFX.GetSize(); i++)
	{
		hssSound* sFX = m_arrSFX[i];
		if(sFX)
			delete sFX;
	}
	m_arrSFX.RemoveAll();
}


int CHSound::LoadSFX(TCHAR* szFX)
{	
	hssSound* sNew = new hssSound;

	if(sNew->load(szFX) == false)
	{
		delete sNew;
		return -1;
	}

	m_arrSFX.AddElement(sNew);
	return (m_arrSFX.GetSize()-1);
}

int CHSound::LoadSFX(UINT uiSFX)
{
	hssSound* sNew = new hssSound;

	DWORD dwID = uiSFX;

	if(sNew->load(m_hInst, dwID, HSS_LOAD_DEFAULT) != HSS_OK)
	{
		delete sNew;
		return -1;
	}

	m_arrSFX.AddElement(sNew);
	return (m_arrSFX.GetSize()-1);
}

void CHSound::PlaySFX(int iIndex)
{
	hssSound* sFX = m_arrSFX[iIndex];
	if(!sFX)
		return;

	m_sound.playSound(sFX);
}

int	CHSound::GetVolumeSFX()
{
	// will return a value between 0 - 64
	return m_sound.volumeSounds();
}

void CHSound::SetVolumeSFX(int iVolume)
{
	// must be between 0 and 64
	if(iVolume < 0 || iVolume > 64)
		return;

	m_sound.volumeSounds(iVolume);
}