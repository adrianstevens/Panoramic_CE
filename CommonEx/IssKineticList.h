#pragma once

#include "windows.h"
#include "IssVector.h"
#include "IssGDIEx.h"
#include "IssImageSliced.h"
#include "IssVector.h"

#define WM_NOTIF_DeleteItem			9501

enum EnumScroll
{
	SCROLL_Scrolling,       // we are grabbing and dragging the list
	SCROLL_Moving,          // the list is sliding away
	SCROLL_Stopped,         // list is not moving at all
	SCROLL_To,              // list is sliding to a particular destination
};

enum EnumScrollDir
{
	DIR_Down,
	DIR_Up,
};

#define OPTION_DrawScrollArrows     0x01
#define OPTION_DrawScrollBar        0x01 << 1
#define OPTION_CircularList         0x01 << 2
#define OPTION_AlwaysShowSelector   0x01 << 3
#define OPTION_Bounce               0x01 << 4
//#define OPTION_UseSecondGDI			0x01 << 5
//#define OPTION_AnimateGrow          0x01 << 4
//#define OPTION_AnimateGenieUp       0x01 << 5
//#define OPTION_AnimateGenieDown     0x01 << 6

#define FLAG_Check                  0x01
#define FLAG_Radio                  0x01 << 1
#define FLAG_Grayed                 0x01 << 2

enum EnumImageArray
{
    IA_Radio = 0,
    IA_Check,
    IA_ScrollArrowDwnSel,
    IA_ScrollArrowDwnUnSel,
    IA_ScrollArrowUpSel,
    IA_ScrollArrowUpUnSel,
    IA_ScrollbarBackground,
    IA_Scrollbar,
};

enum EnumMouseGrab
{
    GRAB_None,
    GRAB_List,
    GRAB_ScrollArrowUp,
    GRAB_ScrollArrowDown,
    GRAB_Scrollbar,
};

enum EnumItemType
{
    ITEM_Unknown = 0,       // this is an LPVOID so can be anything
    ITEM_Text,          // just straight text
    ITEM_Separator,     // separator (non selectable)
    ITEM_Category,      // category with text (non selectable)
};

struct TypeItems
{
    LPVOID          lpItem;     // can be basically anything (NOTE: Make sure that this is fully self deleting)
    TCHAR*          szText;     // text to display
    EnumItemType    eType;      // type of item that this is
    int             iHeight;    // Regular height
    int             iHeightSel; // Height when it is selected
    UINT            uiMenuID;   // Send the WM_COMMAND with this ID
    DWORD           dwFlags;    // flags associated with this item
    LPARAM          lParam;     // extra parameter (usually an index in a list or a pointer)
    TypeItems();
    ~TypeItems();
};

typedef void (*DRAWITEM)(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
typedef void (*DELETEITEM)(LPVOID lpItem);


class CIssKineticList
{
public:
	CIssKineticList(void);
	~CIssKineticList(void);

    HRESULT         Initialize( HWND hWndNotif,
                                HWND hWndParent, 
                                HINSTANCE hInst,
                                DWORD dwFlags,
                                BOOL bDontResetFunctions = FALSE);
	static void		DeleteAllContent();
    void            DestroyList();
    HRESULT         SetCategory(UINT uiCategory);
    HRESULT         SetSelected(UINT uiSelected);
    HRESULT         SetImageArray(UINT uiImageArray);
    UINT            GetBackground(){return m_uiBackground;};
    UINT            GetCategory(){return m_uiCategory;};
    UINT            GetSelected(){return m_uiSelected;};
    UINT            GetImageArray(){return m_uiArray;};
    HRESULT         SetColors(COLORREF crText, COLORREF crHighlight, COLORREF crSelected);
    HRESULT         SetCustomDrawFunc(DRAWITEM pDrawFunc, LPVOID lpClass);
	HRESULT			SetDeleteItemFunc(DELETEITEM pDeleteItemFunc);
    DWORD           GetMenuFlags(){return m_dwFlags;};
    int             GetTotalListHeight(){return GetEndOfListYPos();};
    int				GetScrollPos()		{ return m_iScrollPos; };
    void            ResetlastTickUsed(){m_dwTickLastUsed = 0;};

    void            ShowSelectedItem(BOOL bAnimated = TRUE);
    void            SortNamedList();
    HRESULT         SortList(int (*compare)(const void* , const void*));
    HRESULT         Move(int iFromIndex, int iToIndex){return m_arrItems.Move(iFromIndex, iToIndex);};
    virtual HRESULT PreloadImages(HWND hWnd, HINSTANCE hInstance);
	virtual void    Reset();
    HRESULT         ResetContent();
    HRESULT         AddSeparator();
    HRESULT         AddCategory(TCHAR* szCategory);
    HRESULT         AddCategory(UINT uiCategory, HINSTANCE hInst);
    HRESULT         AddItem(LPVOID lpItem, UINT uiMenu, DWORD dwFlags = 0, LPARAM lParam = 0);
    HRESULT         AddItem(TCHAR* szItem, UINT uiMenu, DWORD dwFlags = 0, LPARAM lParam = 0, BOOL bAddSorted = FALSE);
    HRESULT         AddItem(UINT uiItem, HINSTANCE hInst, UINT uiMenu, DWORD dwFlags = 0, LPARAM lParam = 0, BOOL bAddSorted = FALSE);
    HRESULT         SetItemHeights(int iHeight, int iHeightSelected);
    HRESULT         SetItemHeight(int iIndex, int iHeight, int iHeightSelected);
    int             GetItemCount(){return m_arrItems.GetSize();};
    TypeItems*      GetItem(int iIndex){return m_arrItems[iIndex];}; 
    HRESULT         RemoveItem(int iIndex);
    HRESULT         SetSelectedItemIndex(int iIndex, BOOL bDisplaySelected);
    TypeItems*      GetSelectedItem(){return m_arrItems[m_iCurSelector];}; 
    int             GetSelectedItemIndex(){return m_iCurSelector;};
    CIssGDIEx&      GetImageArrayGDI(){return *m_gdiImageArray;};
    HWND	        GetOwnerWindow(void){return m_hWndMyParent;};
    int             GetScrollPercentageHeight(int iScrollbarHeight);
    void            SetScrollPercentage(int iPercent);
    EnumScroll      GetScrollType(){return m_eScroll;};
    RECT            GetDrawRect(int iItemIndex);
	POINT			GetLastButtonUp(){return m_ptLastButtonUp;};
    BOOL            IsItemVisible(int iItemIndex);
	

    BOOL			OnLButtonDown(POINT& pt);
    BOOL			OnLButtonUp(POINT& pt);
    BOOL            OnLButtonUpList(POINT& pt);
    BOOL			OnMouseMove(POINT& pt);
    BOOL            OnMouseMoveList(POINT& pt);
    BOOL            OnKeyDown(WPARAM wParam, LPARAM lParam);
    BOOL			OnTimer(WPARAM wParam, LPARAM lParam);
    BOOL            OnSize(int iX, int iY, int iWidth, int iHeight);
    BOOL            OnDraw(CIssGDIEx& gdiMem, RECT& rcClient, RECT& rcClip);

    void            KillAllTimers();//in case we lose focus ... 

protected:
    
	int             GetIndent();	
	void			ResetScrollPos()	{ m_iScrollPos = 0; }
	BOOL			IsStopped()			{ return (m_eScroll == SCROLL_Stopped); }//any type of "movement"
	EnumScroll		GetMovementState()	{ return m_eScroll;};
    void            SetMovementState(EnumScroll eScroll){m_eScroll = eScroll;};	
	void			StopScrolling();
    RECT&	        GetOwnerFrame(void){return m_rcList;};
    int		        GetEndOfListYPos(void);
    void	        ReDrawList(void);
    BOOL            IsItemSelectable(TypeItems* sItem);
    BOOL            IsItemSelectable(int iIndex);
    BOOL            IsItemSelected(int iIndex);
    virtual BOOL    SelecteItem(int iIndex);
    
	void			ScrollTo(int iYPos, BOOL bNoWait = FALSE);
	void			ReDrawList(int iScrollAmount);
    double          GetScrollSpeed(){return m_dbSpeed;};
    void            DrawEntries(CIssGDIEx& gdi, RECT& rcClip);
    void            DrawScrollArrows(CIssGDIEx& gdi, RECT& rcClip);
    void            DrawScrollBar(CIssGDIEx& gdi, RECT& rcClip);
    virtual void    DrawSelector(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem){DrawSelector(gdi, rcDraw);};
    virtual void    DrawSelector(CIssGDIEx& gdi, RECT& rcDraw);
    void            DrawItemText(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    void            DrawItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    void            DrawItemAttributes(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem);
    void            DrawCategory(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem);
    void            DrawSeparator(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem);
    HRESULT         DeleteItem(int iIndex);
    int             GetYStartPosition(int iIndex);
    int             GetYStartPosition(TypeItems* sItem);
    int             GetItemHeight(int iIndex, TypeItems* sItem = NULL);
    virtual int     GetNextItemIndex(int iItemIndex, BOOL bSearchUp = FALSE);
    virtual int     GetNextPageItemIndex(BOOL bSearchUp = FALSE);
    virtual void    GrabItem(POINT& pt, TypeItems* sItem){};
    int             GetNextSelectableItem(int iItem, BOOL bSearchUp = FALSE);
    int             GetItemIndexAtPos(int iYPos);
    static int      CompareNameItems(const void* lp1, const void* lp2);

private:
	void			ScrollList();
	int				GetNextScrollToPos();
	BOOL			ScrollToDestReached();
	float			CalcScrollIncrement();	// in pixels
    void            CheckFlags(DWORD dwFlags);
    void            EnsureWithinBounds();

protected:    // variables
    CIssVector<TypeItems>   m_arrItems;

	int				        m_iScrollPos;			//in pixels
	int				        m_iLastReDrawInc;
	double			        m_dbSpeed;
	double			        m_dbFriction;
	EnumScroll		        m_eScroll;
	EnumScrollDir	        m_eScrollDir;
	int				        m_iScrollYStart;
	int				        m_iScrollYPos;
	int				        m_iScrollToStart;
	int				        m_iScrollToDest;
	DWORD			        m_dwStartTime;
	DWORD			        m_dwStopTime;

    RECT                    m_rcLoc;            // location of the window
    RECT                    m_rcList;           // list scrolling bounds
    RECT                    m_rcScrollUp;       // the up scroll indicator
    RECT                    m_rcScrollDown;     // the down scroll indicator
    static CIssGDIEx*       m_gdiList;          // list DC
    static CIssGDIEx*       m_gdiImageArray;    // different indicators
    static CIssGDIEx*       m_gdiSeparator;     // separator
    CIssImageSliced         m_imgCategory;      // category image
    CIssImageSliced         m_imgSelected;      // selector
    UINT                    m_uiBackground;     // background resource image
    UINT                    m_uiSelected;       // selected resource image
    UINT                    m_uiCategory;       // Category resource image
    UINT                    m_uiArray;          // other image array's
    COLORREF                m_crText;           // normal text color
    COLORREF                m_crHighlight;      // text color when highlighted
    COLORREF                m_crSelected;       // text color when item is selected
    int                     m_iCurrentHeight;   
    HFONT                   m_hFontText;
    BOOL                    m_bDrawSelector;    // should we draw the selector?
    BOOL                    m_bWasScrolling;    // 
    int                     m_iCurSelector;     // current selector index
    DWORD                   m_dwFlags;          // flags that we use
    RECT                    m_rcScrollBar;      // Scrollbar location
    RECT                    m_rcScrollArea;     // full area you can scroll to
    EnumMouseGrab           m_eMouse;           // what did the stylus first select
    POINT                   m_ptYStartGrab;      // when grab this will be compared to check if it's an actual select
	POINT					m_ptLastButtonUp;
    DWORD                   m_dwTickCountGrab;  // there will also be a max time to select an item
    DWORD                   m_dwTickLastUsed;   // This is the last time this window was up
    DWORD                   m_dwTickLastKeyDown;// Last time key down was pressed
    DRAWITEM                m_pDrawFunc;        // our custom draw function
	DELETEITEM				m_pDeleteItemFunc;	// function to delete our LPVOID item
    LPVOID                  m_lpClass;          // used with the custom draw function
    HWND                    m_hWndNotif;        // window to send notifications to
    HWND                    m_hWndMyParent;     // Parent window we always draw to
    HINSTANCE               m_hInstance;        // Resource Instance to load our images
    BOOL                    m_bFlagsSet;        // are there any garnish flags (Radio or Check) set, this changes our rects
};
