#pragma once
#include "atlbase.h"
#include "atlwin.h"
#include "wmp.h"


#define WM_Play			WM_USER + 301
#define WM_Pause		WM_USER + 302
#define WM_Stop			WM_USER + 303

class CObjMediaPlayer
{
public:
	CObjMediaPlayer();

	~CObjMediaPlayer(void);

	static CObjMediaPlayer & GetPlayer();
	static void		Destroy();
	void			HandleTransportCommand(UINT cmd);

	void			Play();
	void			Pause();
	void			Stop();

	BOOL			SetFile(BSTR bsPath);

private:
	CComPtr<IWMPPlayer> m_spPlayer;


};
