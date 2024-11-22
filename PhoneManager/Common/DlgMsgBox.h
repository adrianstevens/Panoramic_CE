#pragma once

#include "DlgBase.h"
#include "IssGDIEx.h"

class CDlgMsgBox:public CDlgBase
{
public:
	CDlgMsgBox(void);
	~CDlgMsgBox(void);

    int     PopupMessage(UINT uiMessage, UINT uiTitle, HWND hWndParent, HINSTANCE hInst, UINT uiFlag = MB_OK);
    int     PopupMessage(TCHAR* szMessage, TCHAR* szTitle, HWND hWndParent, HINSTANCE hInst, UINT uiFlag = MB_OK);

protected:
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

	virtual BOOL    OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	virtual void    DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	virtual void    DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	virtual void    OnMenuLeft();
	virtual void    OnMenuRight();

private:	// functions
	BOOL	FadeInScreen(HDC hDC, RECT rcClient);
	void	DrawText(CIssGDIEx& gdi);

private:	// variables
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
	
	RECT			m_rcYes;
	RECT			m_rcNo;
	RECT			m_rcText;
	
    TCHAR*          m_szText;
    TCHAR*          m_szTitle;
	BOOL			m_bFirstTime;
    BOOL            m_bYesNo;                   // are we doing yes/no or just OK
};
