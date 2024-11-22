#pragma once

#include "IssString.h"
#include "IssGDIEx.h"
#include "IssVector.h"
#include "IssPicLoader.h"

#define     WM_PicLoaded        WM_USER + 7100
#define		WM_PicItemChanged	WM_USER + 7101

#define OPTION_DrawTitle        0x01
#define OPTION_DrawDots         0x01 << 1
#define OPTION_SnapCenter       0x01 << 2
#define OPTION_SlidePerPage		0x01 << 3

struct TypePicItems
{
	TCHAR*		    szTitle;    // title for the item
	TypePLHolder*   sPic;       // loaded pic item
	UINT            uiMenuID;   // MENU id used when clicked on
	LPARAM          lParam;     // extra params to pass through
	PFN_LOADPIC     pfnLoadPic; // our dynamic load function (will be called within a thread)
    LPVOID          lpItem;     // can be basically anything (NOTE: Make sure that this is fully self deleting)
	TypePicItems();
	~TypePicItems();
	void Destroy();
};

typedef void (*DRAWPICITEM)(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
typedef void (*DELETEPICITEM)(LPVOID lpItem);
typedef void (*DRAWTITLE)(CIssGDIEx& gdi, RECT& rcDraw, TypePicItems* sItem, LPVOID lpClass);
typedef void (*DRAWDOTS)(CIssGDIEx& gdi, RECT& rcDraw, int iCurrentIndex, int iTotalCount, LPVOID lpClass);

enum EnumPicScroll
{
	PICSCROLL_Scrolling,       // we are grabbing and dragging the list
	PICSCROLL_Moving,          // the list is sliding away
	PICSCROLL_Stopped,         // list is not moving at all
	PICSCROLL_To,              // list is sliding to a particular destination
};

enum EnumPicScrollDir
{
	DIR_Left,
	DIR_Right,
};

enum EnumPicMouseGrab
{
    PICGRAB_None,
    PICGRAB_List,
};

class CIssPicControl
{
public:
	CIssPicControl(void);
	~CIssPicControl(void);

	HRESULT         Initialize( HWND hWndNotif,
								HWND hWndParent, 
								HINSTANCE hInst,
								DWORD dwFlags);
	void			Destroy();
	void			ResetContent();
    HRESULT         SetCustomDrawFunc(LPVOID lpClass, DRAWPICITEM pDrawFunc = NULL, DRAWTITLE pDrawTitle = NULL, DRAWDOTS pDrawDots = NULL);
    HRESULT			SetDeleteItemFunc(DELETEPICITEM pDeleteItemFunc);
	HRESULT         SetDefaultImage(UINT uiDefaultImage);
	HRESULT         SetColors(COLORREF crText);
	HRESULT			SetItemSize(int iWidth, int iHeight);
    HRESULT         SetIndentSize(int iWidth);
	HRESULT         AddItem(TCHAR* szTitle, 
                            UINT uiMenu, 
                            LPARAM lParam = 0,
                            TCHAR*  szURL = NULL,
                            TCHAR*  szFilename = NULL,
                            UINT    uiItem = 0,
                            PFN_LOADPIC pfnLoadPic = NULL);
    HRESULT         AddItem(TCHAR* szTitle,
                            UINT uiMenu,
                            LPARAM lParam = 0,
                            LPVOID lpItem = NULL);

    int             GetTitleHeight();
	int             GetItemCount(){return m_arrItems.GetSize();};
	TypePicItems*   GetItem(int iIndex){return m_arrItems[iIndex];}; 
	HRESULT         RemoveItem(int iIndex);
    TypePicItems*   GetSelectedItem(){return m_arrItems[m_iCurSelector];}; 
    int             GetSelectedItemIndex(){return m_iCurSelector;};
    HRESULT         SetSelectedItemIndex(int iIndex, BOOL bDisplaySelected);
    void		    ShowSelectedItem(BOOL bAnimated = TRUE);
    EnumPicScroll   GetMovingState(){return m_eScroll;};
    RECT            GetDrawRect(int iItemIndex);
    RECT&           GetClipRect(){return m_rcClip;};

	BOOL			OnLButtonDown(POINT& pt);
	BOOL			OnLButtonUp(POINT& pt);
	BOOL			OnMouseMove(POINT& pt);
	BOOL            OnKeyDown(WPARAM wParam, LPARAM lParam);
	BOOL			OnTimer(WPARAM wParam, LPARAM lParam);
	BOOL            OnSize(int iX, int iY, int iWidth, int iHeight);
	BOOL            OnDraw(CIssGDIEx& gdiMem, RECT& rcClient, RECT& rcClip);

protected:	// functions
	void			DrawEntries(CIssGDIEx& gdi, RECT& rcClip);
	void			DrawItem(CIssGDIEx& gdi, RECT& rc, TypePicItems* sItem, BOOL bIsHighlighted);
    void            DrawSelector(CIssGDIEx& gdi, RECT& rcDraw);
    void            DrawTitle(CIssGDIEx& gdi);
    void            DrawDots(CIssGDIEx& gdi);
	void			DrawDefaultItem(CIssGDIEx& gdi, RECT& rc);

    virtual void    GrabItem(POINT& pt, TypePicItems* sItem){};

	int				GetNextScrollToPos();
	BOOL			ScrollToDestReached();
	float			CalcScrollIncrement();	// in pixels
	void			ReDrawList(void);
	void			ReDrawList(int iScrollAmount);
	void			EnsureWithinBounds();
    void            EnsureSelectedItemCentered();
	int				GetListWidth();
	void			ScrollTo(int iXPos, BOOL bNoWait = FALSE);
	int				GetNextItemIndex(int iItemIndex, BOOL bSearchUp = FALSE);
	int				GetNextSelectableItem(int iItem, BOOL bSearchUp);
	BOOL			IsItemSelectable(TypePicItems* sItem);
	BOOL			IsItemSelectable(int iIndex);
	int				GetXStartPosition(TypePicItems* sItem);
	int				GetXStartPosition(int iIndex);
	void            StopScrolling();
    int             GetItemIndexAtPos(int iXPos);
    BOOL            OnMouseMoveList(POINT& pt);
    BOOL            OnLButtonUpList(POINT& pt);
    BOOL            SelecteItem(int iIndex);
    HRESULT         StartThreadEngine();
    HRESULT         DeleteItem(int iIndex);


private:	// variables
	CIssString*		        m_oStr;
	CIssVector<TypePicItems> m_arrItems;		// our list of items
    CIssPicLoader           m_oPics;
	CIssGDIEx		        m_gdiDefaultImage;
	//CIssGDIEx		        m_gdiList;
	UINT			        m_uiDefaultImage;			// default image resource
	COLORREF		        m_crText;					// text color
	SIZE			        m_sizeItem;					// width and height of Items
	RECT			        m_rcLoc;					// location of the list
	RECT			        m_rcList;					// location where to draw the actual entries
	RECT			        m_rcDots;					// our dots
	RECT			        m_rcTitle;
    RECT                    m_rcClip;
    HFONT                   m_hFontTitle;

	DWORD					m_dwTickLastKeyDown;
	int				        m_iScrollPos;			//in pixels
	int						m_iCurSelector;
	int						m_iLastReDrawInc;
	int						m_iCurrentWidth;
    int                     m_iIndent;
	double			        m_dbSpeed;
	double			        m_dbFriction;
	EnumPicScroll	        m_eScroll;
	EnumPicScrollDir        m_eScrollDir;
	int				        m_iScrollXStart;
	int				        m_iScrollXPos;
	int				        m_iScrollToStart;
	int				        m_iScrollToDest;
	HWND                    m_hWndNotif;        // window to send notifications to
	HWND                    m_hWndMyParent;     // Parent window we always draw to
	HINSTANCE               m_hInstance;        // Resource Instance to load our images
	DWORD					m_dwFlags;
    EnumPicMouseGrab        m_eMouse;
    POINT                   m_ptXStartGrab;
    DWORD                   m_dwTickCountGrab;
    BOOL                    m_bWasScrolling;
    DWORD                   m_dwStartTime;
    DWORD                   m_dwStopTime;
    DRAWPICITEM             m_pDrawFunc;        // our custom draw function
    DELETEPICITEM			m_pDeleteItemFunc;	// function to delete our LPVOID item
    DRAWTITLE               m_pDrawTitle;
    DRAWDOTS                m_pDrawDots;
    LPVOID                  m_lpClass;          // used with the custom draw function
};


