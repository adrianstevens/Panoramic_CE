#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"

enum EnumIconStyle
{
    ICON_Windows,
    ICON_Rocket,
    ICON_Default,
    ICON_Off,
};


class CDlgStart:public CIssWnd
{
public:
	CDlgStart(void);
	~CDlgStart(void);
    void    CloseAll();
    BOOL    LaunchLauncher();
    void    ResetHHProc();

protected:
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:	// functions
    static BOOL    EnumWindowsProc(HWND hWnd, DWORD lParam);
    static BOOL     EnumHelpProc(HWND hWnd, DWORD lParam);
	BOOL	ResetWindowPosition();
	BOOL	IsAutoLaunch();
	void	SetAutoLaunch();
	void	RemoveAutoLaunch();    
    
    void    SetHHProc();

public:
	
private:	// variables
    CIssGDIEx   m_gdiIcon;

    EnumIconStyle m_eIconStyle;

};
