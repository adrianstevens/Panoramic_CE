#pragma once
#include "isswnd.h"
#include "IssString.h"
#include "ObjStateAndNotifyMgr.h"
#include "IssKey.h"
#include "CallManager.h"


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
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL	OnHibernate(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
    BOOL    OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
    
private:	// functions
	void	AddOutput(TCHAR* szText);
    void    CheckDemo();


private:	// variables
    CCallManager            m_oCallMan;

    BOOL                    m_bPreventUsage;
    HWND					m_hWndOutput;
};
