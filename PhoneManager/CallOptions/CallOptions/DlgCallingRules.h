#pragma once

#include "IssWnd.h"
#include "IssGDIeX.h"
#include "IssDynBtn.h"
#include "ObjCallOptionList.h"
#include "DlgBaseOptions.h"

class CDlgCallingRules : public CDlgBaseOptions
{
public:
	CDlgCallingRules();
	~CDlgCallingRules(void);

	void				InitControls();

	BOOL 				OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 				OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 				OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 				OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL 				OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL				OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

private:
	void				CreateButtons();
	BOOL				OnMenu();
	void				AddItem();
	void				DeleteItem();
    BOOL                OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                OnMenuLeft();
    void                OnMenuRight();
    void                DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);

	void				RefreshList(int iSelectedIndex = 0);
	static int			CompareItems(const void* lp1, const void* lp2);
	void				FormatDisplayText(TypeCallingRule* sItem, TCHAR* szText);

private:
	CIssDynBtn			m_btnAdd;
	CIssDynBtn			m_btnRemove;
	HFONT				m_hBtnFont;

	CObjCallOptionList	m_objCallOptionList;

	RECT				m_rcAddBtn;
	RECT				m_rcRemoveBtn;
};
