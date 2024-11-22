#include "StdAfx.h"
#include "CallManager.h"
#include "PoomContacts.h"
#include "GeneralOptions.h"
#include "IssRegistry.h"
#include "ObjStateAndNotifyMgr.h"

#define ONE_SHOT_MOD        0x2001
#define VK_TTALK_ID         0x07

#define IDT_TIMER_RULES     8

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

void CCallManager::Init(HWND hWndParent, HINSTANCE hInst)
{
    m_hWnd = hWndParent;

    //lets init our dialogs
    m_dlgCall.Init(&m_gdiMem, &m_guiBack, FALSE, FALSE);
    m_dlgCall.LoadImages(m_hWnd, hInst);
    m_dlgCall.CreateWin(NULL);

    m_dlgPost.Init(&m_gdiMem, &m_guiBack, FALSE, TRUE);
    m_dlgPost.LoadImages(m_hWnd, hInst);
    m_dlgPost.CreateWin(NULL);

    //and don't forget the area codes
    m_oArea.Init(hInst);
    m_oArea.LoadAreaCodes();

    OnNotifyBattery(0);
    OnNotifySignal(0);
    OnNotifyCarrier(0);
}

BOOL CCallManager::OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
    switch(uiMessage)
    {
    case WM_NOTIFY_INCOMING:
        OnNotifyIncoming(wParam);
        break;
    case WM_NOTIFY_CALLERNAME:	
        OnNotifyCallerName(wParam);        
        break;
    case WM_NOTIFY_INCOMINGOID:
        OnNotifyIncomingOID(wParam);
        break;
    case WM_NOTIFY_OUTGOINGOID:
        OnNotifyIncomingOID(wParam);
        break;
    case WM_NOTIFY_OUTGOING:
        OnNotifyOutgoing(wParam);
        break;
    case WM_NOTIFY_CONNECTED:
        OnNotifyConnected(wParam);
        break;
    case WM_NOTIFY_BATTLEVEL:
        OnNotifyBattery(wParam);
        break;
    case WM_NOTIFY_SIGNALLEVEL:
        OnNotifySignal(wParam);
        break;
    case WM_NOTIFY_CARRIER:
        OnNotifyCarrier(wParam);
        break;



    case WM_NOTIFY_BLOCKLISTCHANGE:
    case WM_NOTIFY_RINGRULECHANGE:
    case WM_NOTIFY_CALLOPTIONCHANGE:

    case WM_NOTIFY_PHONEPROFILE:
    case WM_NOTIFY_DATETIME:
    case WM_NOTIFY_PROFILE_RINGNAME:
    case WM_NOTIFY_AIRPLANE:
    case WM_NOTIFY_PROFILE_RINGVOLUME:
    case WM_NOTIFY_GENERALOPTIONS:
        //just let the manager handle em all
        ReceiveNotification(uiMessage, wParam, lParam);
        break;

    default:
        return UNHANDLED;
        break;
    }

    return UNHANDLED;
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

BOOL CCallManager::HandleRingTones(DWORD& dwCallPermissions)
{
	DBG_OUT((_T("CCallManager::HandleRingTones")));
	DBG_OUT((_T("m_sCallerID szName - %s"), m_sCallerID.szName));
	DBG_OUT((_T("m_sCallerID szNumber - %s"), m_sCallerID.szNumber));
	DBG_OUT((_T("m_sCallerID lOID - %d"), m_sCallerID.lOid));

	if(!IsRingtoneHandlingEnabled())
    {
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

void CCallManager::StopRingTone()
{
    DBG_OUT((_T("CCallManager::StopRingTone()")));
    m_objRingtoneEngine.StopRingToneEvent();
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

BOOL CCallManager::UseCheekGuard()
{
    UINT uFlags = 0;
    DWORD dwSize = sizeof(UINT);
    if( S_OK != GetKey(/*REG_KEY*/REG_KEY_ISS_PATH, REG_GeneralOptions, (LPBYTE)&uFlags, dwSize))
        return TRUE;

    DBG_OUT((_T("CDlgCallScreen::UseCheekGuard - %s"), (uFlags&FLAG_SHOW_Cheekguard?_T("TRUE"):_T("FALSE"))));
    return uFlags & FLAG_SHOW_Cheekguard;
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

BOOL CCallManager::UsePostCall()
{
    UINT uFlags = 0;
    DWORD dwSize = sizeof(UINT);
    if( S_OK != GetKey(/*REG_KEY*/REG_KEY_ISS_PATH, REG_GeneralOptions, (LPBYTE)&uFlags, dwSize))
        return TRUE;

    DBG_OUT((_T("CDlgCallScreen::UsePostCall - %s"), (uFlags&FLAG_SHOW_PostCall?_T("TRUE"):_T("FALSE"))));
    return uFlags & FLAG_SHOW_PostCall;
}


/********************************************************************
Function    MakeCanonicalNum

Arguments:    szPhoneNum - retrieve and set number
szAreaCode - current area code
bUseSpaces - whether spaces should be used or not

Returns:    

Comments:    Return the phone number in canonical form
*********************************************************************/
BOOL CCallManager::MakeCanonicalNum(TCHAR* szPhoneNum, TCHAR* szAreaCode, BOOL bUseSpaces)
{
    int    iLength      = 0;
    int    iStartPos    = 0;

    CIssString* oStr = CIssString::Instance();

    if(oStr->IsEmpty(szPhoneNum))
        return FALSE;

    TCHAR szCanPhoneNum[TAPIMAXDESTADDRESSSIZE + 1];

    for(int i = 0; i < oStr->GetLength(szPhoneNum); i++)
    {
        if (iswdigit (szPhoneNum[i]))
        {
            szPhoneNum[iLength] = szPhoneNum[i];
            iLength += 1;
        }
    }

    // Terminate the string with NULL.
    szPhoneNum[iLength] = _T('\0');

    if (iLength == 10)
    {
        //Make the phone number in the format "+1 (xxx) xxx-xxxx"
        oStr->StringCopy(szCanPhoneNum, _T("+1 "));
        szCanPhoneNum[3]  = '(';
        szCanPhoneNum[4]  = szPhoneNum[iStartPos];
        szCanPhoneNum[5]  = szPhoneNum[iStartPos + 1];
        szCanPhoneNum[6]  = szPhoneNum[iStartPos + 2];
        szCanPhoneNum[7]  = _T(')');
        szCanPhoneNum[8]  = _T(' ');
        szCanPhoneNum[9]  = szPhoneNum[iStartPos + 3];
        szCanPhoneNum[10] = szPhoneNum[iStartPos + 4];
        szCanPhoneNum[11] = szPhoneNum[iStartPos + 5];
        szCanPhoneNum[12] = _T('-');
        szCanPhoneNum[13] = szPhoneNum[iStartPos + 6];
        szCanPhoneNum[14] = szPhoneNum[iStartPos + 7];
        szCanPhoneNum[15] = szPhoneNum[iStartPos + 8];
        szCanPhoneNum[16] = szPhoneNum[iStartPos + 9];
        szCanPhoneNum[17] = _T('\0');

        if(szAreaCode != NULL)
        {
            oStr->Empty(szAreaCode);
            szAreaCode[0] = szPhoneNum[iStartPos];
            szAreaCode[1] = szPhoneNum[iStartPos + 1];
            szAreaCode[2] = szPhoneNum[iStartPos + 2];
            szAreaCode[3] = _T('\0');
        }
    }
    else if(iLength == 11)
    {
        if (szPhoneNum[0] != '1') //if its 11 digits it better start with a 1 for now ... 
            //could also be xx xx xxx xxxx but we'll worry about that later
            return FALSE;      
        else
        {
            //Make the phone number in the format "+1 (xxx) xxx-xxxx"
            szCanPhoneNum[0]  = _T('+');
            szCanPhoneNum[1]  = szPhoneNum[iStartPos];
            szCanPhoneNum[2]  = _T(' ');
            szCanPhoneNum[3]  = _T('(');
            szCanPhoneNum[4]  = szPhoneNum[iStartPos + 1];
            szCanPhoneNum[5]  = szPhoneNum[iStartPos + 2];
            szCanPhoneNum[6]  = szPhoneNum[iStartPos + 3];
            szCanPhoneNum[7]  = _T(')');
            szCanPhoneNum[8]  = _T(' ');
            szCanPhoneNum[9]  = szPhoneNum[iStartPos + 4];
            szCanPhoneNum[10] = szPhoneNum[iStartPos + 5];
            szCanPhoneNum[11] = szPhoneNum[iStartPos + 6];
            szCanPhoneNum[12] = _T('-');
            szCanPhoneNum[13] = szPhoneNum[iStartPos + 7];
            szCanPhoneNum[14] = szPhoneNum[iStartPos + 8];
            szCanPhoneNum[15] = szPhoneNum[iStartPos + 9];
            szCanPhoneNum[16] = szPhoneNum[iStartPos + 10];
            szCanPhoneNum[17] = _T('\0');

            if(szAreaCode != NULL)
            {
                oStr->Empty(szAreaCode);
                szAreaCode[0] = szPhoneNum[iStartPos + 1];
                szAreaCode[1] = szPhoneNum[iStartPos + 2];
                szAreaCode[2] = szPhoneNum[iStartPos + 3];
                szAreaCode[3] = _T('\0');
            }
        }
    }
    else
    {
        return FALSE;
    }

    // Copy the newly created phone number back to lpszPhoneNum.
    oStr->StringCopy(szPhoneNum, szCanPhoneNum);

    return TRUE;
} 

void CCallManager::GetCallType()
{
    TCHAR szTemp[STRING_SMALL];

    m_oStr->Empty(szTemp);
    if(!CObjStateAndNotifyMgr::GetPhoneType(szTemp, STRING_SMALL))
        return;

//    m_dlgCall.SetCallType(szTemp);
//    m_dlgPost.SetCallType(szTemp);
}

void CCallManager::GetCallerName()
{
    TCHAR szTemp[STRING_MAX];
    
    if(!CObjStateAndNotifyMgr::GetIncomingCallerName(szTemp, STRING_MAX))
    {
        if(!CObjStateAndNotifyMgr::GetOutgoingCallerName(szTemp, STRING_MAX))
        {
            m_oStr->StringCopy(szTemp, _T("Unknown"));
        }
    }

    m_dlgCall.SetCallerName(szTemp);
    m_dlgPost.SetCallerName(szTemp);
}

void CCallManager::GetCallerNumber()
{
    if(!CObjStateAndNotifyMgr::GetIncomingCallerNumber(m_szPhoneNum, STRING_MAX))
    {
        if(!CObjStateAndNotifyMgr::GetOutgoingCallerNumber(m_szPhoneNum, STRING_MAX))
        {
            m_oStr->StringCopy(m_szPhoneNum, _T("Unknown"));
        }
    }

    m_dlgCall.SetCallerNumber(m_szPhoneNum);
    m_dlgPost.SetCallerNumber(m_szPhoneNum);
}

void CCallManager::GetCallerID()
{
    LONG lOid = -1;
    DBG_OUT((_T("CDlgCallScreen::SetCallerID()")));

    if(!CObjStateAndNotifyMgr::GetIncomingCallerContactOID(&lOid))
    {
        CObjStateAndNotifyMgr::GetOutgoingCallerContactOID(&lOid);
    }

    if(lOid != -1)
    {
        m_lOid = lOid; 
    }

    m_dlgCall.SetOID(m_lOid);
    m_dlgPost.SetOID(m_lOid);

    DBG_OUT((_T("New OID - %d"), m_lOid));
}


void CCallManager::OnNotifyBattery(WPARAM wParam)
{
    SYSTEM_POWER_STATUS_EX2 sNewBattery = {0};

    sNewBattery.BatteryFlag			= BATTERY_FLAG_UNKNOWN;
    sNewBattery.BatteryLifePercent	= 10;

    if(!GetSystemPowerStatusEx2(&sNewBattery, sizeof(SYSTEM_POWER_STATUS_EX2), TRUE))
        return;

    m_dlgCall.SetBattery(sNewBattery.BackupBatteryLifePercent);
    m_dlgPost.SetBattery(sNewBattery.BackupBatteryLifePercent);
}

void CCallManager::OnNotifyCarrier(WPARAM wParam)
{
    TCHAR szTemp[STRING_LARGE];

    if(!CObjStateAndNotifyMgr::GetPhoneOperator(szTemp, STRING_LARGE))
        return;

    m_dlgCall.SetCarrier(szTemp);
    m_dlgPost.SetCarrier(szTemp);
}

void CCallManager::OnNotifyCallerName(WPARAM wParam)
{

}

void CCallManager::OnNotifyConnected(WPARAM wParam)
{
    if(wParam & SN_PHONECALLTALKING_BITMASK)
    {
        m_eAllow = ACALL_Allow;

        //nice and simple
        if(m_eCallType == CTYPE_IncomingMissed)
            m_eCallType = CTYPE_IncomingAnswered;
        else
        {
            m_eCallType = CTYPE_OutgoingAnswered;

            //get the area code frame
            HandleAreaCode();
            AreaCodeType* sArea = m_oArea.GetAreaCodeInfo();
        }

        m_eCallState = CSTATE_Connected;
        m_iCallTimer = 0;

        //need the time regardless 

        GetLocalTime(&m_sCallStartTime);

        if(UseCheekGuard() == FALSE)
        {
            ShowWindow(m_hWnd, SW_HIDE);
            return;
        }
        else
        {
            m_dlgCall.Show(TRUE);
            InvalidateRect(m_hWnd, NULL, FALSE);
        }
    }    
    else
    {
        m_dlgCall.Hide();
        if(UsePostCall())
        {   //if its not a missed call
            if(m_eCallState == CSTATE_Connected)
            {
                if(m_eAllow == ACALL_Block)
                    return;

                //check if there's still an active call because of call waiting
                int iCount = 0;
                if(!CObjStateAndNotifyMgr::GetActiveCallCount(&iCount))
                    iCount = 0;

                if(iCount > 0)
                {
                    m_dlgCall.Show(FALSE);
                    return;
                }

                m_dlgPost.Show(FALSE);
            }
        }

        m_eCallType  = CTYPE_NoCall;
        m_eCallState = CSTATE_Disconnected;
    }
}

void CCallManager::OnNotifyIncoming(WPARAM wParam)
{
    //if its stopped ringing
    if(!(wParam & SN_PHONEINCOMINGCALL_BITMASK))
        StopRingTone();

    int iCount = 0;
    if(CObjStateAndNotifyMgr::GetActiveCallCount(&iCount))
    {
        if(iCount > 1)
        {
            m_dlgCall.Hide();
            return;
        }
    }

    SetCallDefaults();

    BOOL bShowIncoming = UseIncoming();

    //we need this information even if we're not showing the incoming call
    GetCallerNumber();
    GetCallerName();
    GetCallerID();
    CheckCallRules(TRUE);
    GetCallType();
    HandleAreaCode();

    //for the picture
    m_dlgCall.UpdateContact();
    m_dlgPost.UpdateContact();

    if(bShowIncoming == FALSE || m_eAllow == ACALL_Block || m_eAllow == ACALL_Private)
    {
        m_dlgCall.Hide();
    }
    else
    {
        ::RegisterHotKey(m_hWnd, VK_TTALK_ID, ONE_SHOT_MOD, VK_TTALK);

        m_eCallState = CSTATE_Ringing;
        m_dlgCall.SetCallState(m_eCallState);
        m_dlgCall.StartGlobeAnimation();
        InvalidateRect(m_hWnd, NULL, FALSE);
        m_dlgCall.Show(FALSE);
    }
    //yeah ... a lot happens on incoming calls
}

void CCallManager::OnNotifyIncomingOID(WPARAM wParam)
{

}

void CCallManager::OnNotifyOutgoing(WPARAM wParam)
{

}

void CCallManager::OnNotifyOutgoingOID(WPARAM wParam)
{

}

void CCallManager::OnNotifySignal(WPARAM wParam)
{
    int iTemp;
    if(!CObjStateAndNotifyMgr::GetSignalStrength(&iTemp))
        return;

    m_dlgCall.SetSignal(iTemp);
    m_dlgPost.SetSignal(iTemp);
}

void CCallManager::SetCallDefaults()
{
    GetLocalTime(&m_sCallStartTime);
//    KillTimer(m_hWnd, IDT_KILL_POPUP);
    //just in case
    m_dlgPost.Hide();

    m_dlgCall.SetDefaults();
    m_dlgPost.SetDefaults();

    m_dlgCall.SetCallerName(NULL);
    m_dlgPost.SetCallerName(NULL);

    m_dlgCall.SetCallerNumber(NULL);
    m_dlgPost.SetCallerNumber(NULL);

    m_eAllow            = ACALL_Allow;
    m_eCallType         = CTYPE_IncomingMissed;//until its answered it is 
    m_eLastAllow        = ACALL_Unknown;
    m_bRingToneStarted  = FALSE;
    m_dwLastRingTone    = 0;

    m_lOid              = -1;
    m_iCallTimer        = 0;

}



void CCallManager::CheckCallRules(BOOL bWait)
{
    static EnumAllowCall eLastAllow;

    KillTimer(m_hWnd, IDT_TIMER_RULES);
    if(m_lOid == -1)
        GetCallerID();//gotta try

    DBG_OUT((_T("CDlgCallScreen::CheckCallRules()")));
    DBG_OUT((_T("m_szPhoneNum - %s"), m_szPhoneNum));
    //DBG_OUT((_T("m_szName - %s"), m_szName));
    DBG_OUT((_T("m_lOid - %d"), m_lOid));

    GetCallerName();
    GetCallerID();

    DWORD dwTemp = 0;

    m_eAllow = IsCallAllowed(dwTemp); 
    DBG_OUT((_T("m_oCallMan.IsCallAllowed()")));
    switch(m_eAllow)
    {
    case ACALL_Allow:
        DBG_OUT((_T("m_eAllow - ACALL_Allow")));
        break;
    case ACALL_Block:
        DBG_OUT((_T("m_eAllow - ACALL_Block")));
        break;
    case ACALL_Private:
        DBG_OUT((_T("m_eAllow - ACALL_Private")));
        break;
    case ACALL_Unknown:
        DBG_OUT((_T("m_eAllow - ACALL_Unknown")));
        break;
    }

    if(bWait && m_lOid == -1 && m_eAllow != ACALL_Block)
    {
        SetTimer(m_hWnd, IDT_TIMER_RULES, 600, NULL);
        return;
    }

    if(m_eAllow == ACALL_Block)
    {
        if(m_bRingToneStarted)
            StopRingTone();
        //OnIgnore();
    }
    else if(m_eAllow == ACALL_Private)
    {
        if(m_bRingToneStarted)
            StopRingTone();
        //OnIgnore();
    }
    else
    {
        if(m_bRingToneStarted == FALSE || 
            m_eAllow != m_eLastAllow ||
            dwTemp != m_dwLastRingTone)
        {
            StopRingTone();
            HandleRingTones(dwTemp);    
            m_bRingToneStarted = TRUE;
        }
    }

    m_eLastAllow = m_eAllow;
    m_dwLastRingTone = dwTemp;
}

void CCallManager::HandleAreaCode()
{
    TCHAR szTemp[STRING_NORMAL];
    TCHAR szArea[STRING_SMALL];
    BOOL bIsCountryCode = TRUE;
    int iCode;

    m_oStr->StringCopy(szTemp, m_szPhoneNum);
    m_oArea.MakeStrictNum(szTemp);

    if(m_oStr->GetLength(szTemp) < 7)
        return;

    if(m_oStr->GetLength(szTemp) == 10 || (m_oStr->GetLength(szTemp) == 11 && szTemp[0] == _T('1')))
    {   //check for north American area codes first
        if(MakeCanonicalNum(m_szPhoneNum, szArea, TRUE))
        {
            iCode = m_oStr->StringToInt(szArea);
            if(m_oArea.SetAreaCode(iCode))
                //is it valid ? cause other countries may have 9 digits total asshole
                bIsCountryCode = FALSE;
            else
                m_oStr->StringCopy(m_szPhoneNum, szTemp);//crazy Europeans
        }
    }

    if(bIsCountryCode)
    {
        m_oArea.ExtractCode(szTemp, iCode, bIsCountryCode);
        m_oArea.SetAreaCode(iCode, bIsCountryCode);
    }

    AreaCodeType* sArea = m_oArea.GetAreaCodeInfo();

    if(sArea)
    {
        if(m_oStr->IsEmpty(sArea->szRegion))
            m_oStr->Format(m_szLocation, _T("%s"), sArea->szCountry);
        else
            m_oStr->Format(m_szLocation, _T("%s"), sArea->szRegion);

        m_dlgCall.SetTime();
        m_dlgPost.SetTime();

        m_dlgCall.SetAreaCode(sArea);
        m_dlgPost.SetAreaCode(sArea);
    }
}