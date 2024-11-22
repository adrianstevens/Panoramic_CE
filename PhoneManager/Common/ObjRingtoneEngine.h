#pragma once

#include "IssString.h"
#include "ObjPhoneProfile.h"

enum EnumRingType
{
	RT_Ring,
	RT_IncreasingRing,
	RT_RingOnce,
	RT_Vibrate,
	RT_VibrateAndRing,
	RT_VibrateThenRing,
	RT_None,
    RT_Count,
};

typedef long (*PLAYEVENTSOUND)(LPCTSTR,int); 
typedef long (*STOPEVENTSOUND)(LPCTSTR);

class CObjRingtoneEngine
{
public:
	CObjRingtoneEngine(void);
	~CObjRingtoneEngine(void);

	BOOL	PlayRingToneEvent(
				TCHAR* szRingTonePath,	// full path to ringtone audio file
				 EnumRingType eRing,		// how the ringtone should be played
				BOOL bFlashLED,			// should the LED flash during the Ringtone sequence?
				BOOL bVibrate,			// should the phone vibrate during the Ringtone sequence?
				BOOL bVibrateFirst		// If the phone should vibrate, should it vibrate first before playing the ringtone?
			  );	
	BOOL	PlayRingToneEvent(
				CObjPhoneProfile* objProfile,
				TCHAR* szRingTonePath,	// full path to ringtone audio file
				EnumRingType eRing,		// how the ringtone should be played
				BOOL bFlashLED			// should the LED flash during the Ringtone sequence?
				);	
	
	BOOL	PlayRingToneEvent(
				CObjPhoneProfile* objProfile,
				TCHAR* szRingTonePath,	// full path to ringtone audio file
				EnumRingType eRing,		// how the ringtone should be played
				BOOL bDefeatRinger,		// exclude ring
				BOOL bDefeatVibrate		// exclude vibrate
				);	

	BOOL	PlayRingToneEvent();		// play the default ringtone/script
    BOOL    PlayDefaultRingToneEvent();

	BOOL	StopRingToneEvent();		// stop the ringtone
	BOOL	SetRingTone0ToNone();		// no default ringtone
	BOOL	SetRingTone0ToDefault();	// put it back to what it was
	BOOL	SetIssRingTone();			// set up the iSS RingTone Key
	BOOL	DestroyIssRingTone();		// clean out the iSS RingTone Key

private:	// functions
	HKEY	GetRingToneKey(TCHAR* szRingTone, BOOL bCreate = FALSE);
	BOOL	SetRegValue(HKEY hKey, TCHAR* szSubKey, TCHAR* szValue);
	BOOL	GetRegValue(HKEY hKey, TCHAR* szSubKey, TCHAR* szValue);
    BOOL    GetDefaultSoundPath(TCHAR* szSound);


private:	// variables
	CIssString*		m_oStr;
	HINSTANCE		m_hDll;				// dll pointing to OSSVCS.dll
	PLAYEVENTSOUND	m_pfnPlaySounds;
	STOPEVENTSOUND	m_pfnStopSounds;
	EnumPhoneProfile m_ePreviousProfile;
	CObjPhoneProfile* m_pobjPhoneProfile;
    BOOL            m_bPlayingDefaultRingtone;
};
