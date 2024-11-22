#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssInternet.h"

enum EnumHtmlState
{
	HTML_Loading,
	HTML_Connecting,
	HTML_ConnectionFailed,
	HTML_Downloading,
	HTML_DownloadingBytes,
	HTML_DownloadFailed,
	HTML_Installing,
	HTML_InstallingFailed,
	HTML_Done,
};

class CDlgMoreApps:public CIssWnd
{
public:
	CDlgMoreApps(void);
	~CDlgMoreApps(void);

	BOOL	Launch(HINSTANCE hInst, HWND hWndParent = NULL);
	static void	LaunchMarketplace(TCHAR* szURL, HWND hWnd);
	static BOOL IsMoreAppsLink(TCHAR* szURL);

protected:
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	//BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	//BOOL    OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL    OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL    OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL    OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL    OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};

private:
	void	DownloadCabFile(TCHAR* szURL);
	void	InstallFile();
	void	CloseWindow();

public:
	//HHOOK       m_hKeyboardHook; 

private:
	HWND		m_hWndHtml;
	HFONT		m_hFont;
	EnumHtmlState m_eState;
	CIssGDIEx	m_gdiMem;
	RECT		m_rcStatusBar;
	RECT		m_rcMenuBar;
	TCHAR		m_szURL[STRING_MAX];
	TCHAR		m_szTemp[STRING_MAX];
	BOOL		m_bFirstTime;
	CIssInternet m_oInet;
	BOOL		m_bBtnDown;
	
};
