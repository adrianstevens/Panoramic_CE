#pragma once
#include "isswnd.h"
#include "ObjGui.h"

class CDlgSelectGame : public CIssWnd
{
public:
	CDlgSelectGame(void);
	~CDlgSelectGame(void);

	BOOL				OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL				OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL                OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL                OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};


	int					GetGameNumber(){return m_iGameNum;};

private:
	BOOL				DrawText(HDC hDC);

private:
	CIssString*			m_oStr;
	HWND				m_hWndEdit;
	HFONT				m_hFont;

	RECT				m_rcTitle;
	RECT				m_rcEditCtrl;
	RECT				m_rcUnsolvable;

	int					m_iGameNum;

};
