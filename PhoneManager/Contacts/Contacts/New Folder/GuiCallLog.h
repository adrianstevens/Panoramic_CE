#pragma once
#include "guibase.h"
#include "CallLog.h"
//#include "ListScroller.h"
#include "IssKineticList.h"
#include "SMSMessaging.h"


#define IDMENU_ChangeItem			100
class CPoomContacts;


#define NUM_HISTORY_ICONS   7

enum EnumHistoryItemType
{
    HI_Call = 0,       
    HI_SMS,         
    HI_Email,    
};

class CGuiCallLog: public CGuiBase, CIssKineticList
{
public:
	CGuiCallLog(void);
	~CGuiCallLog(void);

    BOOL			Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions/*, CDlgContactDetails* dlgContactDetails*/);
	BOOL			SetPosition(RECT& rc);

	BOOL			Draw(HDC hDC, RECT& rcClient, RECT& rcClip);
	BOOL			OnLButtonDown(POINT pt);
	BOOL			OnLButtonUp(POINT pt);
	BOOL			OnMouseMove(POINT pt);
	BOOL			OnTimer(WPARAM wParam, LPARAM lParam);
	BOOL			OnChar(WPARAM wParam, LPARAM lParam);
	BOOL	        OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL			OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			MoveGui(RECT rcLocation);

	void			RefreshList();	        // called if list has changed
	BOOL 	        AddMenuItems();
    BOOL            HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam);

protected:
    void            DrawSelector(CIssGDIEx& gdi, RECT& rcDraw);

private:
	void			PopulateList();
	void			EraseList();
    BOOL            NeedMoreHistoryItems(int iCount);
    BOOL            CheckFilters(CALLLOGENTRY& ce);
    BOOL            CheckCallDate(CALLLOGENTRY& ce);
    void            DestroyCallEntry(CALLLOGENTRY& ce);
    void            CollectCallItems(void);
    void            CollectSMSMessages(void);

    CIssImageSliced& GetBorder();
    //CIssGDIEx&      GetBgGDI();
    CIssGDIEx&      GetIconArray();

    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    //void            DrawListIcon(CIssGDIEx& gdi, EnumHistoryItemType eType, BOOL bIncoming, BOOL bMissed, RECT rc);
	BOOL            DrawBorderOverlay(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	void			ScrollTo(TCHAR szChar);

	static int		CompareItems( const void *arg1, const void *arg2 );
	static void		DeleteCallItem(LPVOID lpItem);

    virtual BOOL    SelecteItem(int iIndex);
	
    virtual BOOL    HasFocus();
	
private:
	HANDLE	            m_hCallLog;				// Handle to the call log session

    CIssGDIEx           m_gdiImgIconArray;
    CIssImageSliced     m_imgBorder;              
	

	POINT				m_ptScreenSize;
	POINT				m_ptMouseDown;

	int					m_iLineHeight;
	//int					m_iLetterHeight;
	RECT				m_rcHistoryList;
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

    virtual HRESULT     Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText);
    virtual int         GetIconIndex()  = 0; // derived classes must implement
    virtual FILETIME    GetTime()       = 0;

    EnumHistoryItemType GetType() { return m_eType; }

protected:
    FILETIME        m_ftEvent;
	int				m_iLineHeight;
	HFONT				m_hFontNormal;
	HFONT				m_hFontBold;

private:
    EnumHistoryItemType m_eType;

};

//////////////////////////////////////////////////////////////////////////
class CObjHistoryCallItem : public CObjHistoryItem
{
public:
    CObjHistoryCallItem();
    ~CObjHistoryCallItem();
    virtual HRESULT Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText);
    virtual int     GetIconIndex();
    virtual FILETIME    GetTime()      { return m_ftStartTime; }

    void    SetProperties(BOOL bIsIncoming, BOOL bIsMissed, BOOL m_bRoaming) 
    { 
        m_bIsIncoming = bIsIncoming; 
        m_bMissed = bIsMissed; 
        m_bRoaming = m_bRoaming; 
    }
    void    SetTimes(FILETIME ftStart, FILETIME ftEnd)
    {
        m_ftStartTime = ftStart;
        m_ftEndTime = ftEnd;
    }
    void    SetName(TCHAR* szName)      { m_szName     = szName;   }
    void    SetNumber(TCHAR* szNumber)  { m_szNumber   = szNumber; }
    void    SetNameType(TCHAR szType)   { m_szNameType = szType;   }
private:
    BOOL                m_bIsIncoming;
    BOOL                m_bMissed;
    BOOL                m_bRoaming;
    FILETIME            m_ftStartTime;
    FILETIME            m_ftEndTime;
    TCHAR*              m_szName;
    TCHAR*              m_szNumber;
    TCHAR               m_szNameType; // 'w' for work tel, 'h' for home tel, 'm' for mobile

};

//////////////////////////////////////////////////////////////////////////
class CObjHistorySMSItem : public CObjHistoryItem
{
public:
    CObjHistorySMSItem();
    ~CObjHistorySMSItem();
    virtual HRESULT Draw(CIssGDIEx& gdiDest, CIssGDIEx& gdiIcon, RECT& rcDraw, BOOL bIsHighlighted, COLORREF crText);
    virtual int     GetIconIndex();
    virtual FILETIME    GetTime();

    void            SetMessage(TypeSMSMessage* sMsg) { m_sMsg = sMsg; }

private:
    TypeSMSMessage*     m_sMsg;
};

