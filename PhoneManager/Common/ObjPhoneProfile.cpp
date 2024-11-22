#include "ObjPhoneProfile.h"
#include "IssRegistry.h"
#include "IssCommon.h"
#include "CommonDefines.h"
#include "snapi.h"
#include <regext.h>
#include "IssTapi.h"
#include "IssDebug.h"


#define REG_PhoneProfile	_T("PhoneProfile")
#define REG_EndSilentTime	_T("EndSilent")

#define REG_None            _T("*none*")
#define REG_Vibrate         _T("*vibrate*")

CObjPhoneProfile::CObjPhoneProfile()
:m_eCurrProfile(PP_Normal)
,m_oStr(CIssString::Instance())
{
	GetLocalTime(&m_sysEndSilentTime);
	LoadRegistry();	// read values from the registry
}

CObjPhoneProfile::~CObjPhoneProfile()
{
}

void CObjPhoneProfile::LoadRegistry()
{
    DBG_OUT((_T("CObjPhoneProfile::LoadRegistry")));
	DWORD dwValue;
	if(S_OK == GetKey(REG_KEY_ISS_PATH, REG_PhoneProfile, dwValue))
		m_eCurrProfile = (EnumPhoneProfile)dwValue;

	dwValue = sizeof(SYSTEMTIME);
	GetKey(REG_KEY_ISS_PATH, REG_EndSilentTime, (LPBYTE)&m_sysEndSilentTime, dwValue);

    // have to check the phone profile so we can sync up
    EnumPhoneProfile eProfile = m_eCurrProfile;
    CheckPhoneDefinedProfiles();

    // save if if there is a change
    if(eProfile != m_eCurrProfile)
        SaveRegistry();
}

void CObjPhoneProfile::SaveRegistry()
{
    DBG_OUT((_T("CObjPhoneProfile::SaveRegistry")));
	DWORD dwValue = (DWORD)m_eCurrProfile;
	SetKey(REG_KEY_ISS_PATH, REG_PhoneProfile, dwValue);
	DeleteValue(REG_KEY_ISS_PATH, REG_EndSilentTime);

	if(m_eCurrProfile == PP_Silent2hours || m_eCurrProfile == PP_Silent3hours || m_eCurrProfile == PP_Silent1hour)
		SetKey(REG_KEY_ISS_PATH, REG_EndSilentTime, (LPBYTE)&m_sysEndSilentTime, sizeof(SYSTEMTIME));
}

EnumPhoneProfile CObjPhoneProfile::GetPhoneProfile(BOOL bCheckPhoneProfile)
{
    if(bCheckPhoneProfile)
    {
        CheckPhoneDefinedProfiles();
    }

	// do a check here to see if the silent for two hours is up
	switch(m_eCurrProfile)
	{
    case PP_Silent1hour:
	case PP_Silent2hours:
	case PP_Silent3hours:
	{
		SYSTEMTIME sysTime;
		int iCurTime, iEndTime;
		GetLocalTime(&sysTime);
		SystemTimeToUtc(sysTime, iCurTime);
		SystemTimeToUtc(m_sysEndSilentTime, iEndTime);

		// if we've passed that time
		if(iCurTime > iEndTime)
		{
			// set it back to normal
			m_eCurrProfile = PP_Normal;
			SaveRegistry();
		}
	}
	break;
	}

	return m_eCurrProfile;
}

void CObjPhoneProfile::CheckPhoneDefinedProfiles()
{
    DBG_OUT((_T("CObjPhoneProfile::CheckPhoneDefinedProfiles")));
    if(IsInFlightMode())
    {
        m_eCurrProfile = PP_Airplane;
        return;
    }
    else if(m_eCurrProfile == PP_Airplane)
    {
        // if we were in Airplane mode then make sure we aren't anymore because the IsInFlightMode came back false
        m_eCurrProfile = PP_Normal;
    }

    DWORD dwVolume = GetPhoneRingerVolume();
    if(0 == dwVolume)
    {
        // this means only PP_Vibrate, PP_Silent, PP_Silent1hour, PP_Silent2hours, PP_Silent3hours are acceptable
        // check the ringtone name right now
        TCHAR szRing[STRING_MAX] = _T("");
        if(GetRingtoneName(szRing))
        {
            if(0 == m_oStr->Compare(szRing, REG_None))
            {
                // only PP_Silent, PP_Silent1hour, PP_Silent2hour, PP_Silent3hour are acceptable
                if(m_eCurrProfile != PP_Silent && m_eCurrProfile != PP_Silent1hour && m_eCurrProfile != PP_Silent2hours && m_eCurrProfile != PP_Silent3hours)
                    m_eCurrProfile = PP_Silent;
            }
            else if(0 == m_oStr->Compare(szRing, REG_Vibrate))
            {
                // only PP_Vibrate is acceptable
                m_eCurrProfile = PP_Vibrate;
            }
        }
        else
        {
            if(m_eCurrProfile != PP_Vibrate &&
               m_eCurrProfile != PP_Silent && 
               m_eCurrProfile != PP_Silent1hour && 
               m_eCurrProfile != PP_Silent2hours && 
               m_eCurrProfile != PP_Silent3hours)
                m_eCurrProfile = PP_Silent;
        }
    }
    else
    {
        // we have volume to PP_Normal, PP_Loud, PP_Quiet, or PP_Airplane are only acceptable
        if(dwVolume == 5 && 
            m_eCurrProfile != PP_Loud &&
            m_eCurrProfile != PP_Airplane)
            m_eCurrProfile = PP_Loud;
        else if(dwVolume == 1 &&
            m_eCurrProfile != PP_Quiet &&
            m_eCurrProfile != PP_Airplane)
            m_eCurrProfile = PP_Quiet;
        else if(dwVolume != 5 &&
            dwVolume != 1 &&
            m_eCurrProfile != PP_Normal &&
            m_eCurrProfile != PP_Airplane)
           m_eCurrProfile = PP_Normal;
    }
}

DWORD CObjPhoneProfile::GetPhoneRingerVolume()
{
    DWORD dwValue = 3;
    if(S_OK != GetKey(REG_SoundCategories , REG_SoundInitVolume, dwValue))
        return 3;
    return dwValue;
}

void CObjPhoneProfile::SetPhoneRingerVolume(DWORD dwValue)
{
    if(dwValue > 5)
        dwValue = 5;

    DBG_OUT((_T("CObjPhoneProfile::SetPhoneRingerVolume")));
    DBG_OUT((_T("setting InitVol to %d"), dwValue));
    SetKey(REG_SoundCategories, REG_SoundInitVolume, dwValue);

    DWORD dwFull    = 0;
    switch(dwValue)
    {
    case 1:
        dwFull      = 858993459;
        break;
    case 2:
        dwFull      = 1717986918;
        break;
    case 3:
        dwFull      = 2576980377;
        break;
    case 4:
        dwFull      = 3435973836;
        break;
    case 5:
        dwFull      = 4294967295;
        break;
    }

    DBG_OUT((_T("setting Master Volume to %d"), dwFull));
    SetKey(REG_MasterSound, REG_Volume, dwFull);
    SetKey(REG_MasterSound, REG_Ringer, dwFull);
}

BOOL CObjPhoneProfile::GetRingtoneName(TCHAR* szName)
{
    m_oStr->Empty(szName);
    if(S_OK == GetKey(REG_Ringtones, REG_Sound, szName, STRING_MAX))
        return TRUE;
    return FALSE;
}

BOOL CObjPhoneProfile::GetSavedRingtoneName(TCHAR* szName)
{
    m_oStr->Empty(szName);
    if(S_OK == GetKey(REG_Ringtones, REG_SavedSound, szName, STRING_MAX))
        return TRUE;
    return FALSE;
}

void CObjPhoneProfile::SetSavedRingtoneName(TCHAR* szName)
{
    SetKey(REG_Ringtones, REG_SavedSound, szName, m_oStr->GetLength(szName));
}

void CObjPhoneProfile::SetRingtoneName(TCHAR* szName)
{
    TCHAR szCurrent[STRING_MAX] = _T("");
    if(!GetRingtoneName(szCurrent))
        return;
    if(szName == NULL)
    {
        // this means we want to set it to a real ringtone
        if(m_oStr->Compare(szCurrent, REG_Vibrate) == 0 || m_oStr->Compare(szCurrent, REG_None) == 0)
        {
            // we are currently in vibrate or none so put it back to the original
            TCHAR szSaved[STRING_MAX];
            if(GetSavedRingtoneName(szSaved))
                SetKey(REG_Ringtones, REG_Sound, szSaved, m_oStr->GetLength(szSaved));
        }
    }
    else 
    {
        if(m_oStr->Compare(szCurrent, REG_Vibrate) == 0 || m_oStr->Compare(szCurrent, REG_None) == 0)
        {}
        else
        {
            SetSavedRingtoneName(szCurrent);
        }
        SetKey(REG_Ringtones, REG_Sound, szName, m_oStr->GetLength(szName));
    }
    
}

void CObjPhoneProfile::SetPhoneProfile(EnumPhoneProfile eProfile)
{
	BOOL bWasAirplaneMode = FALSE;
    if(m_eCurrProfile == PP_Airplane || IsInFlightMode())
        bWasAirplaneMode = TRUE;

	m_eCurrProfile = eProfile;

    // try and set the real phone profile so it matches
    switch(m_eCurrProfile)
    {
    case PP_Normal:
    case PP_Airplane:   // we don't care when in airplane mode
        SetPhoneRingerVolume(3);
        SetRingtoneName(NULL);  // set back to default
        break;
    case PP_Quiet:
        SetPhoneRingerVolume(1);
        SetRingtoneName(NULL);      // set back to default
        break;
    case PP_Loud:
        SetPhoneRingerVolume(5);
        SetRingtoneName(NULL);      // set back to default
        break;
    case PP_Vibrate:
        SetPhoneRingerVolume(0);
        SetRingtoneName(REG_Vibrate);
        break;
    case PP_Silent:
    case PP_Silent1hour:
    case PP_Silent2hours:
    case PP_Silent3hours:
        SetPhoneRingerVolume(0);
        SetRingtoneName(REG_None);
        break;
    }

	// if we have an end time then we want to save the time however many hours from now
	if(m_eCurrProfile == PP_Silent1hour || m_eCurrProfile == PP_Silent2hours || m_eCurrProfile == PP_Silent3hours)
	{
		SYSTEMTIME sysTime;
		int iTime;
		GetLocalTime(&sysTime);
		SystemTimeToUtc(sysTime, iTime);

		// add the time needed
		iTime += GetTimeDiff();

		UtcToSystemTime(iTime, m_sysEndSilentTime);
	}
	else if(m_eCurrProfile == PP_Airplane)
	{
#ifndef OMIT_TAPI
		// have to put in airplane mode right now
		CIssTapi tapi;
		tapi.Initialize(NULL);
		tapi.SetFlyMode(TRUE);
#endif
	}

	if(m_eCurrProfile != PP_Airplane && bWasAirplaneMode)
	{
#ifndef OMIT_TAPI
		// put into non-flight mode (turn on tapi)
		CIssTapi tapi;
		tapi.Initialize(NULL);
		tapi.SetFlyMode(FALSE);
#endif
	}

	// save it to the registry
	SaveRegistry();
}

BOOL CObjPhoneProfile::IsInFlightMode()
{
    DWORD dwResult;
    HRESULT hr = RegistryGetDWORD( SN_PHONERADIOOFF_ROOT,
        SN_PHONERADIOOFF_PATH,
        SN_PHONERADIOOFF_VALUE,
        &dwResult);

    if(hr == ERROR_SUCCESS && (dwResult & SN_PHONERADIOOFF_BITMASK))
        return TRUE;

    return FALSE;
}

void CObjPhoneProfile::CheckPhoneProfile(DWORD& dwCallPermissions)
{
	// do our check to see the silent for XX hours is up
	GetPhoneProfile(FALSE);

	switch(m_eCurrProfile)
	{
	case PP_Normal:
    case PP_Loud:
    case PP_Quiet:
		dwCallPermissions |= CALL_Allow;
		break;
	case PP_Airplane:
        // Note: if we are here we have an issue because the call came in but we think
        // we're in Airplane mode so let's switch modes and allow the call through
        m_eCurrProfile = PP_Normal;
        SaveRegistry();
		dwCallPermissions |= CALL_Allow;
		break;
	case PP_Silent:
    case PP_Silent1hour:
    case PP_Silent2hours:
    case PP_Silent3hours:
        dwCallPermissions |= CALL_NoVibrate;
        dwCallPermissions |= CALL_NoRing;
        break;
    case PP_Vibrate:    
		dwCallPermissions |= CALL_Allow;
		dwCallPermissions |= CALL_NoRing;
		break;	
	}
	return;
}

int	CObjPhoneProfile::GetTimeDiff()
{
	int iTimeDiff;
	switch(m_eCurrProfile)
	{
    case PP_Silent1hour:
        iTimeDiff = 60*60;
        break;
	case PP_Silent2hours:
		iTimeDiff = 60*60*2;
		break;
	case PP_Silent3hours:
		iTimeDiff = 60*60*3;
		break;
	default:
		iTimeDiff = 0;
	}
	return iTimeDiff;
}

