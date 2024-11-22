// DlgOptions.h: interface for the CDlgOptions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGOPTIONS_H__32EE099F_93E2_48D5_A7F9_878E267B0C39__INCLUDED_)
#define AFX_DLGOPTIONS_H__32EE099F_93E2_48D5_A7F9_878E267B0C39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IssWnd.h"
#include "IssDynBtn.h"
#include "GuiBackground.h"
#include "IssGDIEx.h"
 

class CDlgOptions: public CIssWnd
{
public:
	CDlgOptions();
	virtual ~CDlgOptions();
	
	BOOL			DoModal(HWND hWndParent, HINSTANCE hInst);
    void            SetInstance(HINSTANCE hInst){m_hInst = hInst;};


private:			// functions
	BOOL			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);					
	BOOL 			OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL 			OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL 			OnLButtonUp(HWND hWnd, POINT& pt);
	static int		CompareItems(void* pt1, void* pt2);

	void			RefreshList();
	
	BOOL			InitControls();
	void			CreateButtons();
    void            SetVoicemailNumber();
	
    void            OnMenu();
	void			AddItem();
	void			DeleteItem();

private:			// variables
	CIssGDIEx		m_gdiMem;
    CGuiBackground  m_guiBackground;

	HWND			m_hWndList;
	HFONT			m_hCaptionFont;
	HFONT			m_hBtnFont;

	CIssDynBtn		m_btnAdd;
	CIssDynBtn		m_btnRemove;

	RECT			m_rcCaption;
	RECT			m_rcAddBtn;
	RECT			m_rcRemoveBtn;

	BOOL			m_bHasInit;
    BOOL            m_bIsLocked;
};

#endif // !defined(AFX_DLGOPTIONS_H__32EE099F_93E2_48D5_A7F9_878E267B0C39__INCLUDED_)
