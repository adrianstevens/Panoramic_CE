#pragma once

#include "IssWnd.h"
#include "IssGDIeX.h"
#include "IssDynBtn.h"
#include "ObjRingtoneRuleList.h"
#include "DlgBaseOptions.h"

class CDlgRingtones : public CDlgBaseOptions
{
public:
	CDlgRingtones();
	~CDlgRingtones(void);

	void			CreateButtons();


	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL 			OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL 			OnLButtonUp(HWND hWnd, POINT& pt);

private:
	void			RefreshList(BOOL bPreventSorting = FALSE);
	BOOL			AddItem();
	BOOL			RemoveItem();
	BOOL			EditItem();
    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            OnMenuLeft();
    void            OnMenuRight();
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    static void     DeleteMyItem(LPVOID lpItem);
    static void     DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    BOOL			DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, TypeRingtoneRule* sRing);

	static int		CompareItems(const void* lp1, const void* lp2);
    void            CheckHandleRingtones();

private:
	CIssDynBtn		m_btnAdd;
	CIssDynBtn		m_btnEdit;
	CIssDynBtn		m_btnRemove;
    HFONT			m_hBtnFont;
    HFONT           m_hFontBtnText;

	CObjRingtoneRuleList	m_objRingtoneRuleList;

};
