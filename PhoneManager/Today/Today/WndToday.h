#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "ObjTodayItems.h"
#include "IssStateAndNotify.h"
#include "IssKey.h"


class CWndToday:public CIssWnd
{
public:
	CWndToday(void);
	~CWndToday(void);
	
	BOOL 			CreateHome(HWND hWndParent, HINSTANCE hInst);
	BOOL 			DestroyHome();
    void            SetDefaultLayout(){m_objItems.SetDefaultLayout();};
#ifdef DEBUG
	void 			RefreshRegistry();
#endif

protected:
	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL 			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL 			OnTodayClearCache(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnTodayQueryRefreshCache(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL 			OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL 			OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);				
	BOOL 			OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnMouseMove(HWND hWnd, POINT& pt);
    BOOL            OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);	
	BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:		// functions
	void			OnContextMenu(POINT pt);
	void			DrawBackground(CIssGDIEx& gdi, RECT& rcClip);
	void			CheckBounds(POINT& pt);
	BOOL			IsSwapStraight(POINT pt);
	BOOL			IsSwapAndFindRoom(POINT pt);
    void            CheckDemo();

private:		// variables
	CIssGDIEx		m_gdiMem;				// background DC
	//CIssGDIEx		m_gdiBackground;		// bg image
	CIssGDIEx		m_gdiMovingBack;		// saving back buffer for moving icons
	CIssGDIEx		m_gdiMovingItem;		// save the item image that we are moving

	int				m_iHoldingIndex;		// icon we are moving

	COLORREF		m_crText;		
	COLORREF		m_crTextHighlight;
	COLORREF		m_crHighlight;
	
	POINT			m_ptLastHolding;		// last point we were holding
	POINT			m_ptLastLButtonDown;	// last point we clicked
	SIZE			m_sizeOffset;			// offset from for moving an item

	BOOL			m_bFocus;				// current selection	
	BOOL			m_bContextMenu;			// context menu is up
	BOOL			m_bHoldAndMove;			// are we currently moving an icon
    BOOL            m_bPreventUsage;
    DWORD           m_dwTickCountGrab;      // used to tell the difference between a move or select
	
	CObjTodayItems	m_objItems;

};
