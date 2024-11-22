#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssDynBtn.h"
#include "GuiBackground.h"
#include "DlgChoosePrograms.h"

class CDlgAddTodayItem: public CIssWnd
{
public:
	CDlgAddTodayItem(CIssGDIEx* gdiMem, CGuiBackground* guiBackground);
	~CDlgAddTodayItem(void);

	//BOOL DoModal(HWND hWndParent, HINSTANCE hInst);


private:			// functions
	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);					
	BOOL 		OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL		OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL 		OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL 		OnLButtonUp(HWND hWnd, POINT& pt);
	static int	CompareItems(void* pt1, void* pt2);

	BOOL		InitControls();
	BOOL		GetTodayRegItem(TCHAR* szName, struct RegTodayItem& sItem);
	void		CreateButtons();
	void		AddItem();
	BOOL		SetTodayRegItem(TCHAR* szName, long lType, RECT& rc, long lID = 0);
    BOOL        SetTodayRegLauncherItem(EnumLaunchType eType, TCHAR* szFriendlyName, TCHAR* szPath);

private:		// variables
	CIssGDIEx*	m_gdiMem;
    CGuiBackground* m_guiBackground;
	HWND		m_hWndList;
	HFONT		m_hCaptionFont;
	HFONT		m_hBtnFont;

	CIssDynBtn	m_btnOK;
	CIssDynBtn	m_btnCancel;

	RECT		m_rcCaption;
	RECT		m_rcAddBtn;
	RECT		m_rcRemoveBtn;

	//BOOL		m_bHasInit;
};
