#include "IssSoundFX.h"

CIssSoundFX::CIssSoundFX(void)
:m_hInst(NULL)
{
	//::ZeroMemory(&m_hssConfig, sizeof(HSSCONFIG));

	// set config.dwSoundFrequency = 0 to disable sound
	m_hssConfig.dwSoundFrequency= 22050;

	// if you set config.dwSoundBPS to 0 then the apps try to open Hekkus with 
	// 16bit output, if it fails then it will use 8bit
	m_hssConfig.dwSoundBPS		= 0;
	m_hssConfig.bSoundStereo	= true;
	m_hssConfig.dwMODChannels	= 1;
	m_hssConfig.dwSFXChannels	= 16;
}

CIssSoundFX::~CIssSoundFX(void)
{
	Destroy();
}

BOOL CIssSoundFX::Initialize(HINSTANCE hInst)
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
							 m_hssConfig.dwMODChannels, 
							 m_hssConfig.dwSFXChannels) == false)
				if (m_sound.open(m_hssConfig.dwSoundFrequency, 
								 8, 
								 m_hssConfig.bSoundStereo == TRUE, 
								 m_hssConfig.dwMODChannels, 
								 m_hssConfig.dwSFXChannels) == false) 
						return FALSE;
		}
		else 
		{
			if (m_sound.open(m_hssConfig.dwSoundFrequency, 
							 m_hssConfig.dwSoundBPS, 
							 m_hssConfig.bSoundStereo == TRUE, 
							 m_hssConfig.dwMODChannels, 
							 m_hssConfig.dwSFXChannels) == false) 
				return FALSE;
		}
	}
	return TRUE;
}

void CIssSoundFX::Destroy()
{
	m_hInst = NULL;
	m_sound.close();
	
	for(int i=0; i<m_arrSFX.GetSize(); i++)
	{
		SoundFX* sFX = m_arrSFX[i];
		if(sFX)
			delete sFX;
	}
	m_arrSFX.RemoveAll();
}

int CIssSoundFX::LoadSFX(TCHAR* szFX)
{
	SoundFX* sNew = new SoundFX;

	if(sNew->load(szFX) == false)
	{
		delete sNew;
		return -1;
	}

	m_arrSFX.AddElement(sNew);
	return (m_arrSFX.GetSize()-1);
}

int CIssSoundFX::LoadSFX(UINT uiSFX)
{
	SoundFX* sNew = new SoundFX;

	if(sNew->load(m_hInst, uiSFX) == false)
	{
		delete sNew;
		return -1;
	}

	m_arrSFX.AddElement(sNew);
	return (m_arrSFX.GetSize()-1);
}

void CIssSoundFX::PlaySFX(int iIndex)
{
	SoundFX* sFX = m_arrSFX[iIndex];
	if(!sFX)
		return;

	m_sound.playSFX(sFX);
}

BOOL CIssSoundFX::LoadMOD(UINT uiMOD, BOOL bLoop)
{
	if(false == m_MOD.load(m_hInst, uiMOD))
		return FALSE;

	if(bLoop)
		m_MOD.loop(true);

	return TRUE;
}

void CIssSoundFX::PlayMOD()
{
	m_sound.playMod(&m_MOD);
}

void CIssSoundFX::PauseMOD()
{
	m_sound.pauseMod();
}

void CIssSoundFX::ResumeMOD()
{
	m_sound.resumeMod();
}

int	CIssSoundFX::GetVolumeSFX()
{
	// will return a value between 0 - 64
	return m_sound.volumeSFX();
}

void CIssSoundFX::SetVolumeSFX(int iVolume)
{
	// must be between 0 and 64
	if(iVolume < 0 || iVolume > 64)
		return;

	m_sound.volumeSFX(iVolume);
}

int	CIssSoundFX::GetVolumeMOD()
{
	// will return a value between 0 - 64
	return m_sound.volumeMod();
}

void CIssSoundFX::SetVolumeMOD(int iVolume)
{
	// must be between 0 and 64
	if(iVolume < 0 || iVolume > 64)
		return;
	m_sound.volumeMod(iVolume);
}
