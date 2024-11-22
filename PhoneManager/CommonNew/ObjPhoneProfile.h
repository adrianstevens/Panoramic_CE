#pragma once

#include "stdafx.h"
#include "IssString.h"

enum EnumPhoneProfile
{
	PP_Normal = 0,					// normal operating mode
    PP_Loud,                    // normal + max volume
    PP_Quiet,                   // quiet profile
	PP_Airplane,				// airplane mode, turn off tapi
	PP_Vibrate,					// vibrate (ie. no ringer but still vibrate)
    PP_Silent,                  // no ringer no vibrate
    PP_Silent1hour,             // silent for 1 hour
	PP_Silent2hours,			// silent for 2 hours
	PP_Silent3hours,			// silent for 3 hours
	PP_Unknown,
};

#define REG_SoundCategories _T("ControlPanel\\SoundCategories\\Ring")
#define REG_SoundInitVolume _T("InitVol")
#define REG_Ringtones       _T("ControlPanel\\Sounds\\RingTone0")
#define REG_Sound           _T("Sound")
#define REG_SavedSound      _T("SavedSound")
#define REG_MasterSound     _T("ControlPanel\\Volume")
#define REG_Volume          _T("Volume")
#define REG_Ringer          _T("Ringer")

class CObjPhoneProfile
{
public:
	CObjPhoneProfile(void);
	~CObjPhoneProfile(void);

	void				LoadRegistry();
	void				SaveRegistry();
	EnumPhoneProfile	GetPhoneProfile(BOOL bCheckPhoneProfile = FALSE);
	void				SetPhoneProfile(EnumPhoneProfile eProfile);
	SYSTEMTIME			GetEndSilentTime(){return m_sysEndSilentTime;};
	void				CheckPhoneProfile(DWORD& dwCallPermissions);
    static BOOL         IsInFlightMode();
    void                SetPhoneRingerVolume(DWORD dwValue);
    
private:	// functions
	int					GetTimeDiff();
    void                CheckPhoneDefinedProfiles();
    DWORD               GetPhoneRingerVolume();
    BOOL                GetRingtoneName(TCHAR* szName);
    void                SetRingtoneName(TCHAR* szName);
    BOOL                GetSavedRingtoneName(TCHAR* szName);
    void                SetSavedRingtoneName(TCHAR* szName);

private:	// variables
    CIssString*         m_oStr;
	EnumPhoneProfile	m_eCurrProfile;		// current profile we are in
	SYSTEMTIME			m_sysEndSilentTime;	// the ending silent time
	
};
