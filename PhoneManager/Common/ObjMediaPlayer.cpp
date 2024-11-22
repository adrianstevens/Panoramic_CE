#include "StdAfx.h"
#include "ObjMediaPlayer.h"
#include "IssCommon.h"
//#include "CommonDefines.h"


static CObjMediaPlayer * s_mediaPlayer = NULL;

CObjMediaPlayer::CObjMediaPlayer()
{
	ASSERT(s_mediaPlayer == NULL);	// someone already created this
									// use GetPlayer()
	s_mediaPlayer = this;

	HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
	CoInitializeEx(NULL, 0);

	HRESULT hr = m_spPlayer.CoCreateInstance( __uuidof(WindowsMediaPlayer), 0, CLSCTX_INPROC_SERVER );
	if(hr == S_OK)
	{
		hr = m_spPlayer->put_uiMode(_T("invisible"));
	}

	::SetCursor(hCursor);
}

CObjMediaPlayer::~CObjMediaPlayer(void)
{
	HCURSOR hCursor		= ::SetCursor(LoadCursor(NULL, IDC_WAIT));
	if(m_spPlayer)
		m_spPlayer.Release();

	CoUninitialize();
	::SetCursor(hCursor);

}

void CObjMediaPlayer::Destroy()
{
	if(s_mediaPlayer)
		delete s_mediaPlayer;

	s_mediaPlayer = 0;
}

CObjMediaPlayer & CObjMediaPlayer::GetPlayer()
{
	// singleton instance
	if(!s_mediaPlayer)
		s_mediaPlayer = new CObjMediaPlayer();

	return *s_mediaPlayer;
}

void CObjMediaPlayer::HandleTransportCommand(UINT cmd)
{
	switch(cmd)
	{
	case WM_Play:	return Play();
	case WM_Pause:	return Pause();	
	case WM_Stop:	return Stop();	
		break;
	}
}

void CObjMediaPlayer::Play()
{
	DebugOut(_T("try to play"));

	IWMPControls* pControls = NULL;
	if(SUCCEEDED(m_spPlayer->get_controls(&pControls)))
	{
		pControls->play();
		pControls->Release();
	}
}
void CObjMediaPlayer::Pause()
{
	DebugOut(_T("try to pause"));
	IWMPControls* pControls = NULL;
	if(SUCCEEDED(m_spPlayer->get_controls(&pControls)))
	{
		pControls->pause();
		pControls->Release();
	}
}
void CObjMediaPlayer::Stop()
{
	DebugOut(_T("try to stop"));
	IWMPControls* pControls = NULL;
	if(SUCCEEDED(m_spPlayer->get_controls(&pControls)))
	{
		pControls->stop();
		pControls->Release();
	}
}

BOOL CObjMediaPlayer::SetFile(BSTR bsPath)
{
	DWORD dwAttributes = GetFileAttributes(bsPath);
	if(dwAttributes == ((DWORD)-1))
	{
		DebugOut(_T("can't find file"));
		m_spPlayer->put_URL(NULL);
		return FALSE;
	}

	// don't play
	IWMPSettings* pSettings = NULL;
	if(SUCCEEDED(m_spPlayer->get_settings(&pSettings)))
	{
		pSettings->put_autoStart(FALSE);
        pSettings->put_volume(100);
		pSettings->Release();
	}

	HRESULT hr = m_spPlayer->put_URL(bsPath);
	if(hr != S_OK)
	{
		DebugOut(_T("can't load file"));
		return FALSE;
	}

	return TRUE;
}
