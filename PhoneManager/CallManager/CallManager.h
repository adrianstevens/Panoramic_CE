#pragma once

#include "IssString.h"
#include "CommonDefines.h"
#include "IssDebug.h"

#include "ObjMediaPlayer.h"
#include "ObjRingtoneEngine.h"
#include "ObjPhoneProfile.h"

#include "ObjRingtoneEngine.h"
#include "ObjBlocklist.h"
#include "ObjRingtoneRuleList.h"
#include "ObjCallOptionList.h"


struct CALLIDINFO
{
    TCHAR    szNumber[STRING_MAX];
    TCHAR    szName[STRING_MAX];
    long	 lOid;
};

enum EnumAllowCall
{
    ACALL_Allow,
    ACALL_Block,
    ACALL_Private, //ie ... busy
    ACALL_Unknown,
};


class CCallManager
{
public:
    CCallManager(void);
    ~CCallManager(void);

    void            ReceiveNotification(UINT uiMessage, WPARAM wParam, LPARAM lParam);
    EnumAllowCall   IsCallAllowed(DWORD& dwCallPermissions);
    BOOL            HandleRingTones(DWORD& dwCallPermissions);
    BOOL            IsRingtoneHandlingEnabled();

    void            OnIncomingCall(TCHAR* szNumber);
    void            SetCallerName(TCHAR* szName){m_oStr->StringCopy(m_sCallerID.szName, szName);};
    void            SetCallerID(long lOid){m_sCallerID.lOid = lOid;};
    void            OnEndCall();

    void            StopRingTone();

    CObjBlocklist*   GetBlockList(){return &m_objBlockList;};//lazyness ... pure lazyness

    BOOL            GetUserPhoneNum(TCHAR* szPhoneNum, int iSize);

private:
    void            ClearCallerID();
    BOOL            UseIncoming();


private:
    CObjPhoneProfile        m_objPhoneProfile;
    CObjRingtoneEngine	    m_objRingtoneEngine;
    CObjBlocklist		    m_objBlockList;
    CObjRingtoneRuleList    m_objRingtoneRules;
    CObjCallOptionList	    m_objCallOptionList;

    CIssString*             m_oStr;

    CALLIDINFO              m_sCallerID;            // Caller ID structure

};
