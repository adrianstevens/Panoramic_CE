#pragma once
#include "isswnd.h"
#include "IssCallManager.h"
#include "IssSound.h"
#include "IssString.h"
#include "ObjStateAndNotifyMgr.h"


class CDlgPhoneServer:public CIssWnd
{
public:
	CDlgPhoneServer(void);
	~CDlgPhoneServer(void);

	void	Init(HINSTANCE hInst){m_hInst = hInst;};

protected:
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnMouseMove(HWND hWnd, POINT& pt);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:	// functions
	void	AddOutput(TCHAR* szText);
	//void	RegisterWindowNotifications();



private:	// variables
	CIssString*				m_oStr;
	HWND					m_hWndOutput;

	CIssCallManager			m_oCallManager;

	CIssSound	m_oSound;

};
