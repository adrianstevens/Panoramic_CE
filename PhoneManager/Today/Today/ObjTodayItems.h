#pragma once

#include "IssVector.h"
#include "ObjTodayComponent.h"
#include "IssGDIEx.h"
#include "DlgContactDetails.h"
#include "GuiBackground.h"
#include "IssWndTouchMenu.h"


struct RegTodayItem
{
	long lType;
	RECT rcPosition;
	long lID;
};

struct RegTodayLaunchItem
{
    long    lType;
    RECT    rcPosition;
    TCHAR   szPath[MAX_PATH];
    TCHAR   szFriendlyName[MAX_PATH];
};



class CObjTodayItems
{
public:
	CObjTodayItems(void);
	~CObjTodayItems(void);

	BOOL		Initialize(HWND hWnd, HINSTANCE hInst, CIssGDIEx& gdiMem);
	BOOL		Destroy();
	BOOL		Refresh(HWND hwnd);
    void        SetDefaultLayout();
    static void ResetLayout();
	int			GetWindowHeight();

	BOOL		Draw(CIssGDIEx& gdi, 
					RECT& rcClient, 
                    RECT& rcClip,
					COLORREF crText, 
					COLORREF crHighlight, 
					BOOL bHasFocus,
					int iExcludeIndex = -1);
    //CIssGDIEx&  GetGDI(){return m_gdiBits;};
	HFONT		GetFont(){return m_hFontText;};
    int         GetMenuItemHeight();
    void        DrawMenuItem(HDC hDC, RECT rcLocation, DWORD dwItemData, COLORREF crText);

	CObjTodayComponentBase* GetItem(int iIndex);
	CObjTodayComponentBase* GetSelectedItem();
	int			GetItemMouseOver(POINT& pt);
	int			GetItemConflicting(RECT& rc, CObjTodayComponentBase* pExclude);
	int			GetNumItemsRectOver(RECT& rc, CObjTodayComponentBase* pExclude);
	BOOL		ShuffleItemsHorizontal(CObjTodayComponentBase* pItem, RECT& rcDesired, int iRightBounds);
	void		RefreshItem(ComponentType eType, LONG lID = 0);	// update info for permanent items
	int			CountItems()	{return m_arrComponents.GetSize(); }
	BOOL		Save();			// to registry
	BOOL		ReadItems();	// from registry
	BOOL		AutoArrangeItems(int iRightBounds);
	BOOL		CheckAutoArrangeItems(int iRightBounds);
	BOOL		IsLocked()		{ return m_bLockItems; }
	void		ToggleLock()	{ m_bLockItems = !m_bLockItems;	Save();};
	BOOL 		OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void		SetSelection(int iSelected)	{	m_iSelectedIndex = iSelected;}
	void		RemoveItem(HWND hwnd);
	BOOL		SmartArrangeItems(HWND hWnd);
	BOOL		IsLoading(){return m_bLoading;};
	void		ResetLoading(){m_bLoading = FALSE;};

private:		// functions
	static BOOL	    SetTodayRegItem(TCHAR* szName, long lType, RECT& rc, long lID = 0);
    BOOL        SetTodayRegLauncherItem(EnumLaunchType eType, TCHAR* szFriendlyName, TCHAR* szPath, RECT& rcLocation);
	int			GetSelection()				{	return m_iSelectedIndex;}
	int			FindItemLessThan(int iX, int iY, int iXCheck, int iYCheck, RECT& rcClient);
	int			FindItemGreaterThan(int iX, int iY, int iXCheck, int iYCheck, RECT& rcClient);
	int			FindLastItem();
	BOOL		PlaceIconNextAvailablePos(CObjTodayComponentBase* pItem, int iMatrixRightBounds, BOOL bNewRow = FALSE);

private:		// variables
	CIssVector	<CObjTodayComponentBase> m_arrComponents;	
	//CIssGDIEx	m_gdiBits;
    CGuiBackground m_guiBackground;
    CDlgContactDetails m_dlgContactDetails;
    CIssWndTouchMenu m_wndMenu;

	HWND		m_hWndOwner;
    HINSTANCE   m_hInst;
	class CIssString* m_oStr;
    CIssGDIEx*  m_pgdiMem;

	BOOL		m_bLoading;
	BOOL		m_bLockItems;			// locked items, can't move them
	int			m_iSelectedIndex;		// Currently selected item
    HFONT       m_hFontText;

};
