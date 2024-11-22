#pragma once

#include "IssGDIEx.h"
#include "IssString.h"
#include "IssVector.h"
#include "IssRect.h"
#include "IssImageSliced.h"

#define IDMENU_Separator		80

struct	TypeMenuEntry
{
	UINT	uItemID;		// Item ID used
	HMENU	hMenu;			// Handle to the menu for this item
};

class CIssMenuHandler
{
public:
	CIssMenuHandler(void);
	~CIssMenuHandler(void);

	BOOL	Initialize(HWND hWnd, HINSTANCE hInst, COLORREF crText, COLORREF crSelector, BOOL bDropShadowText, UINT uiBevelArray = 0);
	BOOL	Destroy();
	BOOL	AppendMenu(HMENU hMenu, UINT uFlags, UINT uIDNewItem, UINT uiText);
	BOOL	PopupMenu(HMENU hMenu, UINT uFlags,	int x, int y);
	BOOL	OnEnterMenuLoop(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	IsMenuUp(){return m_bIsMenuUp;};

protected:	// functions
	static BOOL ProcDefault(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	ProcMenu(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	FindMenuItem(UINT uItemID, LPMENUITEMINFO lpMenuItem);
	void	DrawText(HDC dc, UINT uiText, UINT uiItemState, RECT& rcLocation);
	void	DrawMenuSeparator(CIssRect& rc);


protected:	// variables
	CIssString* m_oStr;
	WNDPROC		m_wndProcMenu;		// default menu proc
	HWND		m_hWnd;				// window handle
	HWND		m_hWndMenu;			// window handle for the menu
	HINSTANCE	m_hInst;			// instance
	CIssGDIEx	m_gdiScreen;		// screenshot of the full background for transparencies
	CIssGDIEx	m_gdiMem;			// backbuffer
	CIssGDIEx	m_gdiBackground;	// for bit drawing of the background area (used for alpha)
	CIssGDIEx	m_gdiBackSelected;	// background image when it's selected
	CIssVector<TypeMenuEntry> m_arrItems;	// the array of items 
	COLORREF	m_crText;			// text color
	COLORREF	m_crSelector;		// selector color
	BOOL		m_bDropShadowText;	// Should we put a drop shadow on the text
	UINT		m_uiBevel;			// bevel array if there is one defined
	HFONT		m_hFontText;		// text font
	RECT		m_rcMenuWindow;		// full size of the menu window
	BOOL		m_bIsMenuUp;
};
