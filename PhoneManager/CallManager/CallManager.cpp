#include "StdAfx.h"
#include "CallManager.h"
#include "PoomContacts.h"
#include "GeneralOptions.h"
#include "IssRegistry.h"
#include "IssTapi.h"

CCallManager::CCallManager(void)
{
    m_oStr = CIssString::Instance();

    // load lists
    m_objBlockList.ReloadList();    
    m_objCallOptionList.ReloadList();
	m_objRingtoneRules.ReloadList();

    if(IsRingtoneHandlingEnabled())
    {
        m_objRingtoneEngine.SetRingTone0ToNone();
        m_objRingtoneEngine.SetIssRingTone();
    }
    else
    {
        m_objRingtoneEngine.SetRingTone0ToDefault();
        m_objRingtoneEngine.DestroyIssRingTone();
    }
}

CCallManager::~CCallManager(void)
{
}


void CCallManager::ReceiveNotification(UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    switch(uiMessage)
    {
    case WM_NOTIFY_GENERALOPTIONS:
        DBG_OUT((_T("ReceiveNotification() - WM_NOTIFY_GENERALOPTIONS")));
        if(IsRingtoneHandlingEnabled())
        {
            m_objRingtoneEngine.SetRingTone0ToNone();
            m_objRingtoneEngine.SetIssRingTone();
        }
        else
        {
            m_objRingtoneEngine.SetRingTone0ToDefault();
            m_objRingtoneEngine.DestroyIssRingTone();
        }
        break;
    case WM_NOTIFY_BLOCKLISTCHANGE:
        {
            DBG_OUT((_T("ReceiveNotification() - WM_NOTIFY_BLOCKLISTCHANGE")));
            m_objBlockList.ReloadList();
        }
        break;
    case WM_NOTIFY_RINGRULECHANGE:
        {
            DBG_OUT((_T("ReceiveNotification() - WM_NOTIFY_RINGRULECHANGE")));
            m_objRingtoneRules.ReloadList();
        }
        break;
    case WM_NOTIFY_CALLOPTIONCHANGE:
        {
            DBG_OUT((_T("ReceiveNotification() - WM_NOTIFY_CALLOPTIONCHANGE")));
            m_objCallOptionList.ReloadList();
        }
    case WM_NOTIFY_PHONEPROFILE:
    case WM_NOTIFY_DATETIME:
    case WM_NOTIFY_PROFILE_RINGNAME:
    case WM_NOTIFY_AIRPLANE:
    case WM_NOTIFY_PROFILE_RINGVOLUME:
        switch(uiMessage)
        {
        case WM_NOTIFY_PHONEPROFILE:
            DBG_OUT((_T("ReceiveNotification() - WM_NOTIFY_PHONEPROFILE")));
            break;
        case WM_NOTIFY_DATETIME:
            DBG_OUT((_T("ReceiveNotification() - WM_NOTIFY_DATETIME")));
            break;
        case WM_NOTIFY_PROFILE_RINGNAME:
            DBG_OUT((_T("ReceiveNotification() - WM_NOTIFY_PROFILE_RINGNAME")));
            break;
        case WM_NOTIFY_AIRPLANE:
            DBG_OUT((_T("ReceiveNotification() - WM_NOTIFY_AIRPLANE")));
            break;
        case WM_NOTIFY_PROFILE_RINGVOLUME:
            DBG_OUT((_T("ReceiveNotification() - WM_NOTIFY_PROFILE_RINGVOLUME")));
            break;
        }
        
        m_objPhoneProfile.LoadRegistry();
        m_objPhoneProfile.GetPhoneProfile(TRUE);
        break;
    }
}

EnumAllowCall CCallManager::IsCallAllowed(DWORD& dwCallPermissions)
{
    DBG_OUT((_T("CCallManager::IsCallAllowed()")));
    // check block list 
    if(m_objBlockList.CheckBlock(&m_sCallerID))
    {
        DBG_OUT((_T("m_objBlockList returns ACALL_Block")));
        return ACALL_Block;
    }

    // check calling rules 
    m_objCallOptionList.CheckCallOptions(&m_sCallerID, dwCallPermissions);
    if(dwCallPermissions == CALL_Reject)
    {
        DBG_OUT((_T("m_objCallOptionList returns CALL_Reject so returning ACALL_Private")));
        return ACALL_Private;
    }

    // check phone profile 
    m_objPhoneProfile.CheckPhoneProfile(dwCallPermissions);
    if(dwCallPermissions == CALL_Reject)
    {
        DBG_OUT((_T("m_objPhoneProfile returns CALL_Reject so returning ACALL_Private")));
        return ACALL_Private;
    }

    DBG_OUT((_T("Returning ACALL_Allow")));
    return ACALL_Allow;
}

BOOL CCallManager::UseIncoming()
{
    UINT uFlags = 0;
    DWORD dwSize = sizeof(UINT);
    if( S_OK != GetKey(/*REG_KEY*/REG_KEY_ISS_PATH, REG_GeneralOptions, (LPBYTE)&uFlags, dwSize))
        return TRUE;

    DBG_OUT((_T("CDlgCallScreen::UseIncoming - %s"), (uFlags&FLAG_SHOW_IncomingCall?_T("TRUE"):_T("FALSE"))));
    return uFlags & FLAG_SHOW_IncomingCall;
}

BOOL CCallManager::HandleRingTones(DWORD& dwCallPermissions)
{
	DBG_OUT((_T("CCallManager::HandleRingTones")));
	DBG_OUT((_T("m_sCallerID szName - %s"), m_sCallerID.szName));
	DBG_OUT((_T("m_sCallerID szNumber - %s"), m_sCallerID.szNumber));
	DBG_OUT((_T("m_sCallerID lOID - %d"), m_sCallerID.lOid));

	if(!IsRingtoneHandlingEnabled())
    {
        // only if we show the incoming call screen do we play ringtone events
        if(UseIncoming())
            m_objRingtoneEngine.PlayDefaultRingToneEvent();

        DBG_OUT((_T("Ringtone handling disabled")));
        return TRUE;
    }

    TypeRingtoneRule sRingtoneRule;
    if(m_objRingtoneRules.GetRingtoneRule(&m_sCallerID, &sRingtoneRule))
    {
        DBG_OUT((_T("Ringtone %s rule..."),
            sRingtoneRule.eRuleType == RR_Category ? _T("Category") : sRingtoneRule.eRuleType == RR_Contact ? _T("contact") : _T("default")));
        DBG_OUT((_T("ringtone: %s")));
        switch(sRingtoneRule.eRingType)
        {
        case RT_Ring:
            DBG_OUT((_T("RingType: Ring")));
            break;
        case RT_IncreasingRing:
            DBG_OUT((_T("RingType: Increasing ring")));
            break;
        case RT_RingOnce:
            DBG_OUT((_T("RingType: Ring Once")));
            break;
        case RT_Vibrate:
            DBG_OUT((_T("RingType: Vibrate")));
            break;
        case RT_VibrateAndRing:
            DBG_OUT((_T("RingType: Vibrate and Ring")));
            break;
        case RT_VibrateThenRing:
            DBG_OUT((_T("RingType: Vibrate then Ring")));
            break;
        case RT_None:
            DBG_OUT((_T("RingType: None")));
            break;
        default:
            DBG_OUT((_T("RingType: Unknown")));
            break;
        }


        BOOL bDefeatVibrate = (dwCallPermissions & CALL_NoVibrate);
        BOOL bDefeatRinger	= (dwCallPermissions & CALL_NoRing);

        m_objRingtoneEngine.PlayRingToneEvent(
			&m_objPhoneProfile,
            sRingtoneRule.ringToneInfo.szPathName, 
            sRingtoneRule.eRingType, 
            bDefeatRinger, bDefeatVibrate);
    }    


    return TRUE;
}

    
void CCallManager::ClearCallerID()
{
    m_oStr->Empty(m_sCallerID.szName);
    m_oStr->Empty(m_sCallerID.szNumber);
    m_sCallerID.lOid = 0;
}

void CCallManager::OnEndCall()
{
    DBG_OUT((_T("CCallManager::OnEndCall()")));
    ClearCallerID();
    
}

void CCallManager::StopRingTone()
{
    DBG_OUT((_T("CCallManager::StopRingTone()")));
    m_objRingtoneEngine.StopRingToneEvent();
}

void CCallManager::OnIncomingCall(TCHAR* szNumber)
{
    DBG_OUT((_T("CCallManager::OnIncomingCall - %s"), szNumber));
    ClearCallerID();
    m_oStr->StringCopy(m_sCallerID.szNumber, szNumber);
    
}

BOOL CCallManager::IsRingtoneHandlingEnabled()
{
    UINT uFlags = 0;
    DWORD dwSize = sizeof(UINT);
    if( S_OK != GetKey(/*REG_KEY*/REG_KEY_ISS_PATH, REG_GeneralOptions, (LPBYTE)&uFlags, dwSize))
        return TRUE;

    DBG_OUT((_T("CCallManager::IsRingtoneHandlingEnabled - %s"), (uFlags&FLAG_SHOW_HandleRingtone?_T("TRUE"):_T("FALSE"))));
    return uFlags & FLAG_SHOW_HandleRingtone;
}

BOOL CCallManager::GetUserPhoneNum(TCHAR* szPhoneNum, int iSize)
{
    if(iSize < 1 || szPhoneNum == NULL)
        return FALSE;

    CIssTapi::GetPhoneNumber(szPhoneNum, iSize, 1);

    return TRUE;
}