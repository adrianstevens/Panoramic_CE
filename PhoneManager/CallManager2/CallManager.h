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

#include "DlgCallScreen.h"
#include "DlgPostCall.h"

#include "PoomContacts.h"
#include "IssTapi.h"
#include "IssAreaCode.h"

class CCallManager
{
public:
    CCallManager(void);
    ~CCallManager(void);

    void            Init(HWND hWndParent, HINSTANCE hInst);

    BOOL	        OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

    void            ReceiveNotification(UINT uiMessage, WPARAM wParam, LPARAM lParam);
    EnumAllowCall   IsCallAllowed(DWORD& dwCallPermissions);
    BOOL            HandleRingTones(DWORD& dwCallPermissions);
    BOOL            IsRingtoneHandlingEnabled();

    void            OnEndCall();

    void            StopRingTone();

    CObjBlocklist*   GetBlockList(){return &m_objBlockList;};

    BOOL            GetUserPhoneNum(TCHAR* szPhoneNum, int iSize);

private:
    void            ClearCallerID();

    BOOL            UseCheekGuard();
    BOOL            UseIncoming();
    BOOL            UsePostCall();

    BOOL            MakeCanonicalNum(TCHAR* szPhoneNum, TCHAR* szAreaCode, BOOL bUseSpaces);

    //state and notification
    void            OnNotifyBattery(WPARAM wParam);
    void            OnNotifySignal(WPARAM wParam);
    void            OnNotifyCarrier(WPARAM wParam);
    void            OnNotifyIncoming(WPARAM wParam);
    void            OnNotifyOutgoing(WPARAM wParam);
    void            OnNotifyConnected(WPARAM wParam);
    void            OnNotifyCallerName(WPARAM wParam);
    void            OnNotifyIncomingOID(WPARAM wParam);
    void            OnNotifyOutgoingOID(WPARAM wParam);

    //get values from SNAPI
    void            GetCallerNumber();
    void            GetCallerName();
    void            GetCallerID();
    void            GetCallType();


    void            SetCallDefaults();
    void            CheckCallRules(BOOL bWait);
    void            HandleAreaCode();
    
    
private:
    CObjPhoneProfile        m_objPhoneProfile;
    CObjRingtoneEngine	    m_objRingtoneEngine;
    CObjBlocklist		    m_objBlockList;
    CObjRingtoneRuleList    m_objRingtoneRules;
    CObjCallOptionList	    m_objCallOptionList;

    CALLIDINFO              m_sCallerID;            // Caller ID structure

    CIssAreaCode            m_oArea;

    CIssString*             m_oStr;

    EnumAllowCall           m_eAllow;
    EnumCallState           m_eCallState;
    EnumCallType            m_eCallType;
    EnumAllowCall           m_eLastAllow;

    TCHAR                   m_szPhoneNum[STRING_LARGE];
    TCHAR                   m_szLocation[STRING_MAX];

    long                    m_lOid;

    int                     m_iCallTimer;

    DWORD                   m_dwLastRingTone;
    BOOL                    m_bRingToneStarted;

    CDlgCallScreen          m_dlgCall;
    CDlgPostCall            m_dlgPost;

    CGuiBackground          m_guiBack;
    CIssGDIEx               m_gdiMem;

    
    SYSTEMTIME              m_sCallStartTime;

    HWND                    m_hWnd;



};
