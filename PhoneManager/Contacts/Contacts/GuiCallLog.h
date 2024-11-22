#pragma once
#include "guibase.h"
#include "CallLog.h"
#include "DlgContactDetails.h"
#include "IssKineticList.h"
#include "SMSMessaging.h"

class CPoomContacts;

#define NUM_HISTORY_ICONS   5
#define IDMENU_SelectSMS        5000
#define IDMENU_SelectCall       5001

enum EnumHistoryItemType
{
    HI_Call = 0,       
    HI_SMS,         
    HI_Email,    
};

class CObjHistoryItem;

class CGuiCallLog: public CGuiBase, CIssKineticList
{
public:
	CGuiCallLog(void);
	~CGuiCallLog(void);

    BOOL			Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContactDetails* dlgContactDetails);
	BOOL			SetPosition(RECT& rc);
	void			ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance);

    void            DrawQuickSetButtons(CIssGDIEx& gdi, RECT& rcClient);
    BOOL            IsQuickSetDown(){return m_bDropBarDown;};
	BOOL			Draw(CIssGDIEx& gdiMem, HDC hDC, RECT& rcClient, RECT& rcClip);
	BOOL			OnLButtonDown(POINT pt);
	BOOL			OnLButtonUp(POINT pt);
	BOOL			OnMouseMove(POINT pt);
	BOOL			OnTimer(WPARAM wParam, LPARAM lParam);
	BOOL			OnChar(WPARAM wParam, LPARAM lParam);
	BOOL	        OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL			OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL			MoveGui(RECT rcLocation);
    virtual void    Show();
    void            OnFocus();

    static CIssKineticList* GetCallList();

	void			RefreshList();	        // called if list has changed
	BOOL 	        AddMenuItems();
    BOOL            HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam);

protected:
    void            DrawSelector(CIssGDIEx& gdi, RECT& rcDraw);

private:
    int             FindFullIndex(CObjHistoryItem* oItem);
    void            DestroyHistory(int iStart = 0);
    void            DeleteHistoryItem(int iItem);
    void            SynchronizeHistory();
	void		    MarkAllAsRead();
    void            OnSelectSMS();
    void            OnSelectCall();
    void            OnSelectConv();
	void			PopulateList(BOOL bIncludeCalls, BOOL bIncludeSMSRecieved, BOOL bIncludeSMSSent, BOOL bPreventSync = FALSE);
	//void			EraseList();
    BOOL            NeedMoreHistoryItems(int iCount);
    BOOL            CheckFilters(CALLLOGENTRY& ce);
    BOOL            CheckCallDate(CALLLOGENTRY& ce);
    void            DestroyCallEntry(CALLLOGENTRY& ce);
    void            CollectCallItems(void);
    void            CollectSMSMessages(BOOL bIncludeRecieved, BOOL bIncludeSent);
    void            RemoveCallItems();
    void            RemoveSMSMessage(BOOL bIncludeRecieved, BOOL bIncludeSent);
    DWORD           GetCalllogCount(void);
    DWORD           GetSMSReceivedCount(void);
    DWORD           GetSMSSentCount(void);
    FILETIME        GetCallRecentTime();
    FILETIME        GetSMSReceivedRecentTime();
    FILETIME        GetSMSSentRecentTime();
    BOOL            CheckReadSMSMessages();
    void            SmartPopulateList(BOOL bCall, BOOL bRecieved, BOOL bSent);
    void            DebugFileTime(TCHAR* szText, FILETIME& ft);
    void            OnDelete();
    void            OnDeleteEntry();
    void            OnDeleteAllSMS(BOOL bUseMsg = TRUE);
    void            OnDeleteAllCalls(BOOL bUseMsg = TRUE);
    void            OnDeleteAll();
    void            AnimateQuickSetIn();
    void            AnimateQuickSetOut();
    void            ToggleQuickSet(int iQuadrant);

    TCHAR*          GetSelectedNumber();
    
    void            ResetCounts(BOOL bCalls, BOOL bSMS);
    void            SortLastItem();
    BOOL            FindItem(class CObjHistoryItem* oItem);
    void            SmartCollectCallItems(int iStartIndex);
    void            SmartCollectSMSMessages(BOOL bIncoming, int iNumberToGet);

    CIssGDIEx&      GetIconArray();

    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
	void			ScrollTo(TCHAR szChar);

	static int		CompareItems( const void *arg1, const void *arg2 );
	static void		DeleteCallItem(LPVOID lpItem);

    virtual BOOL    HasFocus();

    void            LoseFocus(){KillAllTimers();};
	
private:
    CDlgContactDetails* m_dlgContactDetails;

	POINT				m_ptScreenSize;
	POINT				m_ptMouseDown;

	int					m_iLineHeight;
    int                 m_iCallHistoryCount;
    int                 m_iSMSSentCount;
    int                 m_iSMSReceivedCount;
    FILETIME            m_ftRecentCall;
    FILETIME            m_ftRecentSMSReceived;
    FILETIME            m_ftRecentSMSSent;

    RECT				m_rcHistoryList;
    RECT                m_rcDropBar;
    RECT                m_rcDropFull;
    BOOL                m_bDropBarDown;
    HFONT               m_hFontToggleHistory;
    CIssGDIEx           m_gdiArrows;
    CIssVector<CObjHistoryItem> m_arrHistoryFull;
};

//////////////////////////////////////////////////////////////////////////
// history object classes

//////////////////////////////////////////////////////////////////////////
// CObjHistoryItem - base class for all items
class CObjHistoryItem
{
public:
    CObjHistoryItem(EnumHistoryItemType eType);
    virtual             ~CObjHistoryItem();

    virtual HRESULT     Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText, BOOL bDrawName = TRUE);
    virtual int         GetIconIndex()  = 0; // derived classes must implement
    virtual FILETIME    GetTime()       = 0;
    virtual TCHAR*      GetName()       = 0;
    virtual TCHAR*      GetNumber()     = 0;
    virtual CObjHistoryItem* Clone()    = 0;

    EnumHistoryItemType GetType() { return m_eType; }
    HFONT               GetFontNormal();
    HFONT               GetFontBold();

protected:
    FILETIME            m_ftEvent;
	int				    m_iLineHeight;
    static SIZE         m_sizeDate;

private:
    EnumHistoryItemType m_eType;

};

//////////////////////////////////////////////////////////////////////////
class CObjHistoryCallItem : public CObjHistoryItem
{
public:
    CObjHistoryCallItem();
    ~CObjHistoryCallItem();
    virtual HRESULT Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText, BOOL bDrawName = TRUE);
    virtual int     GetIconIndex();
    virtual FILETIME    GetTime()      { return m_ftStartTime; }
    virtual CObjHistoryItem* Clone();

    void    SetProperties(BOOL bIsIncoming, BOOL bIsMissed, BOOL bRoaming, BOOL bConnected, BOOL bCallEnded) 
    { 
        m_bIsIncoming   = bIsIncoming; 
        m_bMissed       = bIsMissed; 
        m_bRoaming      = bRoaming; 
        m_bConnected    = bConnected;
        m_bCallEnded    = bCallEnded;
    }
    void    SetTimes(FILETIME ftStart, FILETIME ftEnd)
    {
        m_ftStartTime = ftStart;
        m_ftEndTime = ftEnd;

        SYSTEMTIME sysTime;
        FILETIME ftLocal;
        int iStart, iEnd;
        FileTimeToLocalFileTime(&ftStart, &ftLocal);
        FileTimeToSystemTime(&ftLocal, &sysTime);
        SystemTimeToUtc(sysTime, iStart);
        FileTimeToLocalFileTime(&ftEnd, &ftLocal);
        FileTimeToSystemTime(&ftLocal, &sysTime);
        SystemTimeToUtc(sysTime, iEnd);
        m_iDuration = iEnd - iStart;
    }
    void    SetName(TCHAR* szName)      { m_szName     = szName;   }
    void    SetNumber(TCHAR* szNumber)  { m_szNumber   = szNumber; }
    void    SetNameType(TCHAR* szType)  { m_szNameType = szType;   }
    void    SetUnknown(BOOL bUnknown)   { m_bUnknown   = bUnknown; }
    BOOL    IsIncoming(){return m_bIsIncoming;};
    BOOL    IsMissed(){return m_bMissed;};
    BOOL    IsRoaming(){return m_bRoaming;};
    BOOL    IsConnected(){return m_bConnected;};
    BOOL    IsCallEnded(){return m_bCallEnded;};
    BOOL    IsUnknown(){return m_bUnknown;};
    FILETIME GetStartTime(){return m_ftStartTime;};
    FILETIME GetEndTime(){return m_ftEndTime;};
    TCHAR*  GetName(){return m_szName;};
    TCHAR*  GetType(){return m_szNameType;};
    TCHAR*  GetNumber(){return m_szNumber;};
    TCHAR*  GetNameType(){return m_szNameType;};

private:
    BOOL                m_bIsIncoming;
    BOOL                m_bMissed;
    BOOL                m_bRoaming;
    BOOL                m_bConnected;
    BOOL                m_bCallEnded;
    BOOL                m_bUnknown;

    FILETIME            m_ftStartTime;
    FILETIME            m_ftEndTime;

    int                 m_iDuration; // in seconds

    TCHAR*              m_szName;
    TCHAR*              m_szNumber;
    TCHAR*              m_szNameType; // 'w' for work tel, 'h' for home tel, 'm' for mobile

};

//////////////////////////////////////////////////////////////////////////
class CObjHistorySMSItem : public CObjHistoryItem
{
public:
    CObjHistorySMSItem();
    ~CObjHistorySMSItem();
    virtual HRESULT Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText, BOOL bDrawName = TRUE);
    virtual int     GetIconIndex();
    virtual FILETIME    GetTime();
    virtual TCHAR*      GetName()   {return m_sMsg->szNumber; }
    virtual TCHAR*      GetNumber() {return m_sMsg->szNumber; }
    virtual CObjHistoryItem* Clone();

    TypeSMSMessage* GetSMS(){return m_sMsg;};

    void            SetMessage(TypeSMSMessage* sMsg) { m_sMsg = sMsg; }

private:
    TypeSMSMessage*     m_sMsg;
    int                 m_iTwoLineCount;
    int                 m_iWidthText;
};

