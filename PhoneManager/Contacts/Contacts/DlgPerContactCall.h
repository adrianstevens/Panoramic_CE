#pragma once

#include "DlgPerContactBase.h"
#include "DlgContactDetails.h"

class CObjHistoryCallItem;

enum EnumCalling
{
    CALL_Type = 0,          // missed, incoming, outgoing
    CALL_PhoneType,     // mobile, business, fax, etc
    CALL_Duration,      // duration of the call
    CALL_CallStatus,     // was the call ended successfully or dropped
    CALL_Roaming,       // was the call local or roaming
};


class CDlgPerContactCall:public CDlgPerContactBase
{
public:
    CDlgPerContactCall(BOOL bEnableDetails, CDlgContactDetails* dlg = NULL);
    ~CDlgPerContactCall(void);

    void                    SetCallItem(CObjHistoryCallItem* oCallItem);

protected:
    void                    DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                    PopulateList();
    void                    OnSelectPopupMenuItem(int iIndex, TypeItems* sItem);
    void                    OnSelectMenuItem(int iIndex, TypeItems* sItem);
    void                    OnMenu();
    void                    DrawContactOtherText(CIssGDIEx& gdi, RECT rcDraw);
    static void             DeleteMyItem(LPVOID lpItem);
    static void             DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    BOOL				    DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumCalling* eCall);

private: // variables
    int                     m_iDuration;    // in seconds
    BOOL                    m_bEnableDetails;
    CDlgContactDetails*     m_dlgDetails;

    // items to store
    TCHAR*                  m_szName;
    TCHAR*                  m_szNumber;
    TCHAR*                  m_szNameType;//bad label but its the type of call
    FILETIME                m_ftStart;
    FILETIME                m_ftEnd;
    BOOL                    m_bIsRoaming;
    BOOL                    m_bIsMissed;
    BOOL                    m_bIsConnected;
    BOOL                    m_bIsCallEnded;
    BOOL                    m_bIsIncoming;
    BOOL                    m_bUnknown;
};
