#pragma once

#include "IssWnd.h"
#include "IssDynBtn.h"
#include "DlgBaseOptions.h"
#include "ObjBlocklist.h"


class CDlgBlocklist:public CDlgBaseOptions
{
public:
	CDlgBlocklist();
	~CDlgBlocklist(void);
	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL 			OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

private:	// functions
	void			CreateButtons();
	static int		CompareItems(const void* lp1, const void* lp2);
	void			RefreshList(BOOL bPreventSorting = FALSE);
	void			AddNew();
	void			AddContact();
	void			AddItem(long oid);
	void			DeleteItem();
	BOOL			OnMenu();
    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            OnMenuLeft();
    void            OnMenuRight();
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            ScrollTo(TCHAR szChar);

private:
	CIssDynBtn		m_btnAddNew;
	CIssDynBtn		m_btnAddContact;
	CIssDynBtn		m_btnRemove;

	CObjBlocklist	m_objBlockList;

	HFONT			m_hBtnFont;
};
