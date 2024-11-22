#pragma once
#include "dlgpercontactbase.h"

enum EnumCalling
{
    CALL_Type = 0,          // missed, incoming, outgoing
    CALL_PhoneType,     // mobile, business, fax, etc
    CALL_Duration,      // duration of the call
    CALL_CallStatus,     // was the call ended successfully or dropped
//    CALL_Roaming,       // was the call local or roaming
    CALL_Location,
};

class CDlgMoreInfo : public CDlgPerContactBase
{
public:
    CDlgMoreInfo(void);
    ~CDlgMoreInfo(void);

    void        SetCallItem(TCHAR* szName,
                            TCHAR* szNumber,
                            TCHAR* szNameType,
                            TCHAR* szLocation,
                            BOOL   bIsRoaming,
                            BOOL   bIsMissed,
                            BOOL   bIsConnected,
                            BOOL   bIsCallEnded,
                            BOOL   bIsIncoming,
                            BOOL   bIsUnknown,
                            int    iDurration,
                            SYSTEMTIME sStartTime,
                            LONG   lOid);

private:
    static void DeleteMyItem(LPVOID lpItem);
    void        PopulateList();

    void        DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void        DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    static void DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    BOOL        DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumCalling* eCall);
    void        DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw);


private:
    int                     m_iDuration;    // in seconds

    // items to store
    TCHAR*                  m_szNumber;
    TCHAR*                  m_szNameType;//bad label but its the type of call
    TCHAR*                  m_szLocation;
    FILETIME                m_ftStart;
    BOOL                    m_bIsRoaming;
    BOOL                    m_bIsMissed;
    BOOL                    m_bIsConnected;
    BOOL                    m_bIsCallEnded;
    BOOL                    m_bIsIncoming;
    BOOL                    m_bUnknown;
};
