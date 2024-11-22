#pragma once
#include "guibase.h"
#include "CallLog.h"
//#include "ListScroller.h"
#include "IssKineticList.h"

#include "CallList.h"

#define IDMENU_ChangeItem			100
class CPoomContacts;

#define NEW_CALLLOG_GUI
#ifndef NEW_CALLLOG_GUI
class CCallList;

class CGuiCallLog : public CGuiBase
{
public:
	CGuiCallLog(void);
	~CGuiCallLog(void);

	BOOL 	        Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBackground, CGuiBackground* guiBackground, TypeOptions* sOptions);
	BOOL			MoveGui(RECT rcLocation);

	BOOL			Draw(HDC hDC, RECT& rcClient, RECT& rcClip);
	BOOL			OnLButtonDown(POINT pt);
	BOOL			OnLButtonUp(POINT pt);
	BOOL			OnMouseMove(POINT pt);
	BOOL			OnTimer(WPARAM wParam, LPARAM lParam);
	BOOL			OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
	BOOL			DrawHeader(HDC hdc, RECT rcClient);
	void			OnContextMenu(POINT pt);
	void			LaunchItem(int iYPos);
	void			DrawGradientGDI(HDC tdc, RECT rc, COLORREF StartRGB, COLORREF EndRGB);

private:
	CIssGDIEx		m_gdiHeader;
	CCallList*		m_oCallList;

	POINT			m_ptMouseDown;
	RECT			m_rcHeading;
	RECT			m_rcCallList;

	HFONT			m_hFontHeading;
	HFONT			m_hFontEntry;

	const int		m_iTitleHeight;
};
#else   //NEW_CALLLOG_GUI

enum EnumHistoryItemType
{
    HI_Call = 0,       
    HI_SMS,         
    HI_Email,    
};

struct TypeHistoryItem
{
    EnumHistoryItemType eType;
    int                 iItemIndex;
    BOOL                bIsIncoming;
    BOOL                bMissed;
    FILETIME            ftTime;
    TCHAR*              szName;
    TCHAR*              szNumber;
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
    //BOOL            CheckItemCategoryNeeded(TypeContact* sContact);

protected:
    void            DrawSelector(CIssGDIEx& gdi, RECT& rcDraw);

private:
	void			PopulateList();
	void			EraseList();
    BOOL            NeedMoreHistoryItems(FILETIME ftLast, int iCount);
    BOOL            GetNextHistoryItem(FILETIME& ftEntry, TypeHistoryItem* pItem, int& iCurCall, int& iCurSms, int& iCurEmail);
	//int				GetNumCallEntries();

    //TypeContact*    FindContact(long lOid, int& iIndex);
    //BOOL            CategoryExists(TCHAR* szText);
    //BOOL            ApplyContactChanges(TypeContact* pSrc, TypeContact* pDst);
    //HRESULT         PoomItemAdded(long lOid);
    //HRESULT         PoomItemChanged(long lOid);
    //HRESULT         PoomItemDeleted(long lOid);

    CIssImageSliced& GetBorder();
    CIssGDIEx&      GetBgGDI();
    CIssGDIEx&      GetIconArray();

    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    void            DrawListIcon(CIssGDIEx& gdi, EnumHistoryItemType eType, BOOL bIncoming, BOOL bMissed, RECT rc);
	BOOL			DrawBackground(HDC hdc, RECT& rcClient, RECT& rcClip);
	BOOL            DrawBorderOverlay(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	void			ScrollTo(TCHAR szChar);

	static int		CompareItems( const void *arg1, const void *arg2 );
    //static void     GetCompareString(TCHAR* pszCompare, TypeContact* sContact, int iCompareCount);

    virtual BOOL    SelecteItem(int iIndex);
	
    virtual BOOL    HasFocus();
	
private:
    //CDlgContactDetails* m_dlgContactDetails;
    //CCallLog			m_oCallLog;
	HANDLE	            m_hCallLog;				// Handle to the call log session

    CIssGDIEx           m_gdiImgIconArray;
	CIssGDIEx			m_gdiBackground;
    CIssImageSliced     m_imgBorder;              
	
	HFONT				m_hFontNormal;
	HFONT				m_hFontBold;

	POINT				m_ptScreenSize;
	POINT				m_ptMouseDown;

	int					m_iLineHeight;
	//int					m_iLetterHeight;
	RECT				m_rcHistoryList;
};



#endif  //NEW_CALLLOG_GUI