#include "StdAfx.h"
#include "ObjRingtoneEngine.h"
#include "IssRegistry.h"
#include "IssDebug.h"
#include "Soundfile.h"

#define REG_Sounds			_T("ControlPanel\\Sounds\\")
#define REG_RingToneDefault	_T("RingTone0")
#define REG_RingToneIss		_T("RingTonePano")
#define REG_Script			_T("Script")
#define REG_ScriptBackup	_T("PanoScriptBackup")
#define REG_Sound           _T("Sound")
#define REG_SoundBackup     _T("PanoSoundBackup")


//////////////////////////////////////////////////////////////////////////
// Sources:
// http://forum.xda-developers.com/archive/index.php/t-244673.html
// http://www.teksoftco.com/forum/viewtopic.php?t=550
// http://www.pocketpcaddict.com/forums/windows-mobile-5/16354-windows-mobile-5-0-change-ringtone-vibrate-led-flash-characteristics.html
// http://forum.xda-developers.com/showthread.php?t=319350&page=44
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// OSSVCS.dll
// Ordinal	Entry Point	Name
// 01a8		00026524	AuxDispEnable
// 01a9		0002652c	AuxDispNavigateViewByName
// 01a7		00000000	AuxDispSendButtonPress
// 0120		00000000	CopyToRingsDirectory
// 0104		00000000	CreateRingtoneFilterString
// 0103		00000000	FindEventSoundHandler
// 0115		00000000	GetCategoryInfo
// 0105		00000000	GetDefaultRing
// 0165		00000000	GetDefaultSound
// 009e		00000000	IsEventSoundEvent
// 004d		00000000	PlayEventSound
// 004e		00000000	PlayEventSoundScript
// 0051		00000000	PlaySoundScript
// 004c		00000000	PreviewEventSound
// 011f		00000000	SetAsRingtone
// 0050		00000000	StopAllEventSounds
// 004f		00000000	StopEventSounds
// 0116		00000000	ValidateSound
// 0062		00000000	WC_PlayClick
//////////////////////////////////////////////////////////////////////////

/************************************************************************
If you would like more control over how your device rings/vibrates/flashes its LEDs on events, you can can change the Script values of these events. For example, for an incoming call performing 'activate device, play ringtone, wait 3 seconds, repeat':

HKCU\ControlPanel\Sounds\RingTone0\Script = "apw3r" (REG_SZ string, no quotes)

For performing 'active device, set volume to 33%, play ringtone, set volume to 67%, play ringtone, set volume to 100%, play ringtone (no repeat)':

HKCU\ControlPanel\Sounds\RingTone0\Script = "ac33pc67pc100p" (REG_SZ string, no quotes)

The following are the full codes available to you. Please note that all the codes are executed simultaneously except after a ringtone play / wait code. E.g. 'v1p' will vibrate and play at the same time, while vibrating for 1 second. But 'pv1' will play the ringtone through all the way first, then start vibrating for 1 second.

a = activate device
cN = set volume to N in percentage max volume
fN = flash notification LED for N seconds
p = play ringtone. Note that this will play the ringtone all the way through before continuing with the next code.
r = repeat. Note that this should be the last code in your Script string, if used at all.
vN = vibrate for N seconds
wN = wait for N seconds. Note that the device will wait this long before continuing with the next code.
************************************************************************/

CObjRingtoneEngine::CObjRingtoneEngine(void)
:m_oStr(CIssString::Instance())
,m_hDll(NULL)
,m_pfnPlaySounds(NULL)
,m_pfnStopSounds(NULL)
,m_ePreviousProfile(PP_Unknown)
,m_pobjPhoneProfile(NULL)
,m_bPlayingDefaultRingtone(FALSE)
{
	// let's load the DLL
	m_hDll = LoadLibrary(_T("\\windows\\ossvcs.dll")); 
	if (m_hDll) 
	{ 
		m_pfnPlaySounds = (PLAYEVENTSOUND)GetProcAddress(m_hDll,(LPCTSTR)84); 
		m_pfnStopSounds = (STOPEVENTSOUND)GetProcAddress(m_hDll,(LPCTSTR)86); 		
	} 

	// let's set up the iSS RingTone Reg Key
	//SetIssRingTone();

	// also set the default ringtone to none right now
	//SetRingTone0ToNone();
}

CObjRingtoneEngine::~CObjRingtoneEngine(void)
{
	if(m_hDll)
	{
		FreeLibrary(m_hDll);
		m_hDll = NULL;
	}

	// return phone to previous state
	//SetRingTone0ToDefault();
	//DestroyIssRingTone();

}

BOOL CObjRingtoneEngine::PlayRingToneEvent(TCHAR* szRingTonePath, EnumRingType eRing, BOOL bFlashLED, BOOL bVibrate, BOOL bVibrateFirst)
{
	if(!m_pfnPlaySounds)
		return FALSE;

    DBG_OUT((_T("CObjRingtoneEngine::PlayRingToneEvent")));
    DBG_OUT((_T("szRingTonePath - %s"), szRingTonePath));
    DBG_OUT((_T("eRing - %d"), eRing));
    DBG_OUT((_T("bFlashLED - %d"), bFlashLED));
    DBG_OUT((_T("bVibrate - %d"), bVibrate));
    DBG_OUT((_T("bVibrateFirst - %d"), bVibrateFirst));

	BOOL bRet = FALSE;
	HKEY hKey = GetRingToneKey(REG_RingToneIss, TRUE);
	if(!hKey)
		return FALSE;

	// set the Ring Tone file
    /*if(eRing == RT_Vibrate)
        SetRegValue(hKey, _T("Sound"), _T("*vibrate*"));
    else if(eRing == RT_None)
        SetRegValue(hKey, _T("Sound"), _T("*none*"));
	else */if(!m_oStr->IsEmpty(szRingTonePath))
		SetRegValue(hKey, _T("Sound"), szRingTonePath);

	TCHAR szScript[STRING_MAX] = _T("a");

	// Should we Flash the LED
	if(bFlashLED)
		m_oStr->Concatenate(szScript, _T("f15"));	// flash for 15 seconds

	// should we vibrate
	if(bVibrate)
	{
		// do we want to vibrate before we play any ring tone?
		if(bVibrateFirst)
			m_oStr->Concatenate(szScript, _T("v3w3"));	// vibrate for 3 seconds, wait 3 seconds before moving onto the next code
		else
			m_oStr->Concatenate(szScript, _T("v0"));	// vibrate throughout
	}

	// choose the ring type
	switch(eRing)
	{
	case RT_Ring:
		m_oStr->Concatenate(szScript, _T("pw3r"));	// play, wait 3, repeat
		break;
	case RT_RingOnce:
		m_oStr->Concatenate(szScript, _T("p"));		// just play once
		break;
	case RT_IncreasingRing:
		//m_oStr->Concatenate(szScript, _T("c33pc67pc100pw2r")); // volume 33%, play, volume 67%, play, volume 100%, play, wait 2 seconds, repeat
		//m_oStr->Concatenate(szScript, _T("c50apw3c150r")); // windows script
		m_oStr->Concatenate(szScript, _T("c50apw1c100w1c150w2r")); // this sounds about the same as previous
		break;
	default:
		m_oStr->Concatenate(szScript, _T("w3r"));		// wait 3 seconds, repeat
		break;
	}

	// set the reg key
	SetRegValue(hKey, _T("Script"), szScript);	
	RegFlushKey(hKey);
	RegCloseKey(hKey);

	// go and play the ringtone event
	m_pfnPlaySounds(REG_RingToneIss, 0);

	return TRUE;
}

BOOL CObjRingtoneEngine::PlayRingToneEvent(CObjPhoneProfile* objProfile, TCHAR* szRingTonePath, EnumRingType eRing,  BOOL bFlashLED)
{
	if(!m_pfnPlaySounds || !objProfile)
		return FALSE;

    DBG_OUT((_T("CObjRingtoneEngine::PlayRingToneEvent")));
    DBG_OUT((_T("szRingTonePath - %s"), szRingTonePath));
    DBG_OUT((_T("eRing - %d"), eRing));
    DBG_OUT((_T("bFlashLED - %d"), bFlashLED));

	BOOL bRet = FALSE;
	HKEY hKey = GetRingToneKey(REG_RingToneIss, TRUE);
	if(!hKey)
		return FALSE;

	m_ePreviousProfile = objProfile->GetPhoneProfile(FALSE);
	m_pobjPhoneProfile = objProfile;
	m_pobjPhoneProfile->SetPhoneProfile(PP_Normal);

	// set the Ring Tone file
    // set the Ring Tone file
    if(eRing == RT_Vibrate)
        SetRegValue(hKey, _T("Sound"), _T("*vibrate*"));
    else if(eRing == RT_None)
        SetRegValue(hKey, _T("Sound"), _T("*none*"));
    else if(!m_oStr->IsEmpty(szRingTonePath))
        SetRegValue(hKey, _T("Sound"), szRingTonePath);

	//TCHAR szScript[STRING_MAX] = _T("a");
	TCHAR szScript[STRING_MAX] = _T("");

	// Should we Flash the LED
	if(bFlashLED)
		m_oStr->Concatenate(szScript, _T("f15"));	// flash for 15 seconds

	/*// should we vibrate
	if(bVibrate)
	{
		// do we want to vibrate before we play any ring tone?
		if(bVibrateFirst)
			m_oStr->Concatenate(szScript, _T("v3w3"));	// vibrate for 3 seconds, wait 3 seconds before moving onto the next code
		else
			m_oStr->Concatenate(szScript, _T("v0"));	// vibrate throughout
	}
	*/

	// choose the ring type
	switch(eRing)
	{
	case RT_Ring:
		m_oStr->Concatenate(szScript, _T("apw3r"));	//"pw3r"// play, wait 3, repeat
		break;
	case RT_RingOnce:
		m_oStr->Concatenate(szScript, _T("p"));		// just play once
		break;
	case RT_IncreasingRing:
		//m_oStr->Concatenate(szScript, _T("c33pc67pc100pw2r")); // volume 33%, play, volume 67%, play, volume 100%, play, wait 2 seconds, repeat
		//m_oStr->Concatenate(szScript, _T("c50apw3c150r")); // windows script
		m_oStr->Concatenate(szScript, _T("c50apw1c100w1c150w2r")); // this sounds about the same as previous
		break;

	case RT_Vibrate:
		m_oStr->Concatenate(szScript, _T("av3w3r"));	// play, wait 3, repeat
		break;
	case RT_VibrateAndRing:
		m_oStr->Concatenate(szScript, _T("av0pw3r"));	// play, wait 3, repeat
		break;
	case RT_VibrateThenRing:
		m_oStr->Concatenate(szScript, _T("v3w3apw3r"));	// play, wait 3, repeat
		break;
	default:
		m_oStr->Concatenate(szScript, _T("w3r"));		// wait 3 seconds, repeat
		break;
	}

	// set the reg key
	SetRegValue(hKey, _T("Script"), szScript);	
	RegFlushKey(hKey);
	RegCloseKey(hKey);

	// go and play the ringtone event
	m_pfnPlaySounds(REG_RingToneIss, 0);

	return TRUE;
}

BOOL CObjRingtoneEngine::PlayRingToneEvent(CObjPhoneProfile* objProfile,
										   TCHAR* szRingTonePath, 
										   EnumRingType eRing, 
										   BOOL bDefeatRinger, 
										   BOOL bDefeatVibrate)
{
	if(!m_pfnPlaySounds)
		return FALSE;

    DBG_OUT((_T("CObjRingtoneEngine::PlayRingToneEvent")));
    DBG_OUT((_T("szRingTonePath - %s"), szRingTonePath));
    DBG_OUT((_T("eRing - %d"), eRing));
    DBG_OUT((_T("bDefeatRinger - %d"), bDefeatRinger));
    DBG_OUT((_T("bDefeatVibrate - %d"), bDefeatVibrate));

	m_pobjPhoneProfile = NULL;
	m_ePreviousProfile = PP_Unknown;

	if(objProfile && objProfile->GetPhoneProfile(FALSE) == PP_Vibrate)
	{
		m_ePreviousProfile = PP_Vibrate;
		m_pobjPhoneProfile = objProfile;

		eRing = RT_Vibrate;

		// now set it to normal for the duration of the ringtone event
		m_pobjPhoneProfile->SetPhoneProfile(PP_Normal);
	}

    if(objProfile)
    {
        switch(objProfile->GetPhoneProfile(FALSE))
        {
        case PP_Normal:
            objProfile->SetPhoneRingerVolume(3);
            break;
        case PP_Loud:
            objProfile->SetPhoneRingerVolume(5);
            break;
		case PP_Silent:
		case PP_Silent1hour:
		case PP_Silent2hours:
		case PP_Silent3hours:
			eRing = RT_None;
			break;
        }
    }

	BOOL bRet = FALSE;
	HKEY hKey = GetRingToneKey(REG_RingToneIss);
	if(!hKey)
		return FALSE;

	// set the Ring Tone file
    // set the Ring Tone file
    if(eRing == RT_Vibrate)
        SetRegValue(hKey, _T("Sound"), _T("*vibrate*"));
    else if(eRing == RT_None)
        SetRegValue(hKey, _T("Sound"), _T("*none*"));
    else if(!m_oStr->IsEmpty(szRingTonePath) && !bDefeatRinger)
        SetRegValue(hKey, _T("Sound"), szRingTonePath);



	TCHAR szScript[STRING_MAX] = _T("");

	m_oStr->Concatenate(szScript, _T("f15"));	// flash for 15 seconds

	// choose the ring type
	switch(eRing)
	{
	case RT_Ring:
		m_oStr->Concatenate(szScript, _T("apw3r"));	//"pw3r"// play, wait 3, repeat
		break;
	case RT_RingOnce:
		m_oStr->Concatenate(szScript, _T("p"));		// just play once
		break;
	case RT_IncreasingRing:
		m_oStr->Concatenate(szScript, _T("c50apw1c100w1c150w2r")); // based on the windows script
		break;
	case RT_Vibrate:
		{
			if(!bDefeatVibrate)
				m_oStr->Concatenate(szScript, _T("av3w3r"));	// play, wait 3, repeat
		}
		break;
	case RT_VibrateAndRing:
		{
			if(bDefeatVibrate)
				m_oStr->Concatenate(szScript, _T("apw3r"));		// play, wait 3, repeat
			else
				m_oStr->Concatenate(szScript, _T("av0pw3r"));	// play, wait 3, repeat
		}
		break;
	case RT_VibrateThenRing:
		{
			if(bDefeatVibrate)
				m_oStr->Concatenate(szScript, _T("w3apw3r"));	// play, wait 3, repeat
			else
				m_oStr->Concatenate(szScript, _T("v3w3apw3r"));	// play, wait 3, repeat
		}
		break;
	default:
		m_oStr->Concatenate(szScript, _T("w3r"));		// wait 3 seconds, repeat
		break;
	}

	// set the reg key
	SetRegValue(hKey, _T("Script"), szScript);	
	RegFlushKey(hKey);
	RegCloseKey(hKey);

	// go and play the ringtone event
	m_pfnPlaySounds(REG_RingToneIss, 0);

	return TRUE;
}

BOOL CObjRingtoneEngine::PlayDefaultRingToneEvent()
{
    if(m_pfnPlaySounds)
    {
        m_bPlayingDefaultRingtone = TRUE;
        m_pfnPlaySounds(REG_RingToneDefault, 0);
    }
    return TRUE;
}

BOOL CObjRingtoneEngine::PlayRingToneEvent()
{
	BOOL bRet = FALSE;
	HKEY hKey = GetRingToneKey(REG_RingToneIss);
	if(!hKey)
		return FALSE;

	// get the default ringtone
	TCHAR szRingTonePath[STRING_MAX] = _T("");
	GetRegValue(hKey, _T("Sound"), szRingTonePath);

	PlayRingToneEvent(szRingTonePath, RT_Ring, TRUE, FALSE, FALSE);	// for now

	return TRUE;
}

BOOL CObjRingtoneEngine::StopRingToneEvent()
{
	if(!m_pfnStopSounds)
		return FALSE;

    if(m_bPlayingDefaultRingtone)
        m_pfnStopSounds(REG_RingToneDefault);
    else
	    m_pfnStopSounds(REG_RingToneIss);

    m_bPlayingDefaultRingtone = FALSE;

	if(m_ePreviousProfile != PP_Unknown && m_pobjPhoneProfile)
		m_pobjPhoneProfile->SetPhoneProfile(m_ePreviousProfile);

	m_pobjPhoneProfile = NULL;
	m_ePreviousProfile = PP_Unknown;
	return TRUE;
}

BOOL CObjRingtoneEngine::SetIssRingTone()
{
    DBG_OUT((_T("CObjRingtoneEngine::SetIssRingTone")));
	// go ahead and create the iSS RingTone Key
	HKEY hKey = GetRingToneKey(REG_RingToneIss, TRUE);
	if(!hKey)
		return FALSE;

	SetRegValue(hKey, _T("Category"), _T("Ring"));
    RegFlushKey(hKey);
	RegCloseKey(hKey);

	return TRUE;
}

BOOL CObjRingtoneEngine::DestroyIssRingTone()
{
	TCHAR szTemp[STRING_MAX];

    DBG_OUT((_T("CObjRingtoneEngine::DestroyIssRingTone")));
	m_oStr->StringCopy(szTemp, REG_RingToneIss);
	m_oStr->Insert(szTemp, REG_Sounds);

	RegDeleteKeyNT(HKEY_CURRENT_USER, szTemp);

	return TRUE;
}

BOOL CObjRingtoneEngine::SetRingTone0ToNone()
{
    DBG_OUT((_T("CObjRingtoneEngine::SetRingTone0ToNone")));
	BOOL bRet = FALSE;
	HKEY hKey = GetRingToneKey(REG_RingToneDefault);
	if(!hKey)
		return FALSE;

	TCHAR szScript[STRING_MAX]	= _T("");
    TCHAR szSound[STRING_MAX] = _T("");
    // make a backup but only once
    if(!GetRegValue(hKey, REG_ScriptBackup, szScript))
    {
	    // first we back up the value
	    if(GetRegValue(hKey, REG_Script, szScript))
        {
            if(0 == m_oStr->Compare(szScript, _T("a")) || m_oStr->IsEmpty(szScript))
                m_oStr->StringCopy(szScript, _T("apw3r"));  // default to normal ring
		    SetRegValue(hKey, REG_ScriptBackup, szScript);
        }
    }

    if(!GetRegValue(hKey, REG_SoundBackup, szSound))
    {
        // first we back up the value
        if(GetRegValue(hKey, REG_Sound, szSound))
        {
            if(0 == m_oStr->Compare(szSound, _T("*vibrate*")) || 0 == m_oStr->Compare(szSound, _T("*none*")) || m_oStr->IsEmpty(szSound))
            {
                // get a default sound
                if(GetDefaultSoundPath(szSound))
                    SetRegValue(hKey, REG_SoundBackup, szSound);
            }
            else
                SetRegValue(hKey, REG_SoundBackup, szSound);
        }
    }

	// now set to just turn on the device
	SetRegValue(hKey, REG_Script, _T("a"));

	bRet = TRUE;
//Exit:
    
	if(hKey) 
    {
        RegFlushKey(hKey);
        RegCloseKey(hKey);
    }

	return bRet;
}

BOOL CObjRingtoneEngine::GetDefaultSoundPath(TCHAR* szSound)
{
    if(!szSound)
        return FALSE;

    m_oStr->Empty(szSound);

    BOOL bResult = FALSE;

    HRESULT hr;
    SNDFILEINFO	sfRingtone1;		
    hr = SndGetSound(SND_EVENT_RINGTONELINE1, &sfRingtone1);
    if(SUCCEEDED(hr))
    {
        m_oStr->StringCopy(szSound, sfRingtone1.szPathName);
        bResult = TRUE;
    }
    else
    {
        SNDFILEINFO* sFileList = NULL;
        int iFileCount = 0;
        hr = SndGetSoundFileList(SND_EVENT_RINGTONELINE1 ,SND_LOCATION_ALL, &sFileList, &iFileCount);
        if(SUCCEEDED(hr) && sFileList && iFileCount > 0)
        {
            // just copy the first one over
            m_oStr->StringCopy(szSound, sFileList[0].szPathName);
            bResult = TRUE;
        }
        if(sFileList)
            LocalFree(sFileList);
    }

    return bResult;
}

BOOL CObjRingtoneEngine::SetRingTone0ToDefault()
{

    DBG_OUT((_T("CObjRingtoneEngine::SetRingTone0ToDefault")));
	BOOL bRet = FALSE;
	HKEY hKey = GetRingToneKey(REG_RingToneDefault);
	if(!hKey)
		return FALSE;

	TCHAR szScript[STRING_MAX]	= _T("");
    TCHAR szSound[STRING_MAX]	= _T("");
	// first we back up the value
	if(GetRegValue(hKey, REG_ScriptBackup, szScript))
    {
        if(0 == m_oStr->Compare(szScript, _T("a")) || m_oStr->IsEmpty(szScript))
            m_oStr->StringCopy(szScript, _T("apw3r"));  // default to normal ring

		SetRegValue(hKey, REG_Script, szScript);
    }

    if(GetRegValue(hKey, REG_SoundBackup, szSound))
    {
        
        if(0 == m_oStr->Compare(szSound, _T("*vibrate*")) || 0 == m_oStr->Compare(szSound, _T("*none*")) || m_oStr->IsEmpty(szSound))
        {
            // get a default sound
            if(GetDefaultSoundPath(szSound))
                SetRegValue(hKey, REG_Sound, szSound);
        }
        else
            SetRegValue(hKey, REG_Sound, szSound);
    }	

	bRet = TRUE;


	if(hKey) 
    {
        RegFlushKey(hKey);
        RegCloseKey(hKey);
    }

	return bRet;
}

HKEY CObjRingtoneEngine::GetRingToneKey(TCHAR* szRingTone, BOOL bCreate)
{
	TCHAR	szTemp[MAX_PATH] = _T("");

	if(m_oStr->IsEmpty(szRingTone))
		return NULL;

	m_oStr->StringCopy(szTemp, szRingTone);
	m_oStr->Insert(szTemp, REG_Sounds);

	HKEY hKey;
	if(bCreate)
	{
        DBG_OUT((_T("RegCreateKeyEx - %s"), szTemp));

		DWORD dwDisposition;
		if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, szTemp, NULL, NULL, REG_OPTION_NON_VOLATILE, 0, NULL, &hKey, &dwDisposition))
			return NULL;
	}
	else
	{
        DBG_OUT((_T("RegOpenKeyEx - %s"), szTemp));

		if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, szTemp, NULL, NULL, &hKey))
			return NULL;
	}

	return hKey;
}

BOOL CObjRingtoneEngine::GetRegValue(HKEY hKey, TCHAR* szSubKey, TCHAR* szValue)
{
	if(!hKey || m_oStr->IsEmpty(szSubKey) || szValue == NULL)
		return FALSE;

	m_oStr->Empty(szValue);

	DWORD dwType	= REG_SZ;
	DWORD dwDataSize= STRING_MAX*sizeof(TCHAR);
	TCHAR szTemp[STRING_MAX];
	ZeroMemory(szTemp, sizeof(TCHAR)*STRING_MAX);
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, szSubKey, NULL, &dwType, (LPBYTE)szTemp, &dwDataSize))
		return FALSE; 

    DBG_OUT((_T("RegQueryValueEx - %s = %s"), szSubKey, szTemp));

	m_oStr->StringCopy(szValue, szTemp);

	return TRUE;
}

BOOL CObjRingtoneEngine::SetRegValue(HKEY hKey, TCHAR* szSubKey, TCHAR* szValue)
{
	if(!hKey || m_oStr->IsEmpty(szSubKey) || m_oStr->IsEmpty(szValue))
		return FALSE;

	DWORD dwType	= REG_SZ;
	DWORD dwDataSize= sizeof(TCHAR)*(m_oStr->GetLength(szValue)+1);
	if(ERROR_SUCCESS != RegSetValueEx(hKey, szSubKey, NULL, dwType, (LPBYTE)szValue, dwDataSize))
		return FALSE; 

    DBG_OUT((_T("RegSetValueEx - %s = %s"), szSubKey, szValue));

	return TRUE;
}


