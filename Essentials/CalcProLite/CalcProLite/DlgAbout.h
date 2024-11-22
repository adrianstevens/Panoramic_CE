#pragma once
#include "isswnd.h"
#include "IssGDIDraw.h"
#include "IssGDIEx.h"
#include "CalcMenu.h"

class CDlgAbout : public CIssWnd
{
public:
	CDlgAbout(void);
	~CDlgAbout(void);

    void        Init(CCalcMenu* oMenu){m_oMenu = oMenu;};

	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
//	BOOL		OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("iSSNotepad.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};
    BOOL        OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
    BOOL        OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL        OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL        OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL        OnLButtonUp(HWND hWnd, POINT& pt);

private:
    BOOL        DrawBackground(HDC hdc, RECT rcClip);
    BOOL        DrawAboutText(HDC hdc, RECT rcClip);
    BOOL        DrawMenu(HDC hdc, RECT rcClip);

	void		OnMenuLeft();
	void		OnMenuRight();

private:
    CIssGDIEx   m_gdiMem;
    CIssGDIEx   m_gdiBack;//background image
    CIssGDIEx*  m_gdiMenu;
    CCalcMenu*   m_oMenu;

    HFONT       m_hFont;
};
