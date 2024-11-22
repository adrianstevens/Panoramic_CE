#pragma once
#include "isswnd.h"
#include "IssDynBtn.h"
#include "winbase.h"
#include "Commdlg.h"
#include "windows.h"
#include "IssGDIEx.h"
#include "IssImageSliced.h"

#define REG_KEY			_T("SOFTWARE\\iSS\\BenchMark")

class CDlgSysInfo:public CIssWnd
{
public:
	CDlgSysInfo(void);
	~CDlgSysInfo(void);

	BOOL	Init(HINSTANCE hInstance){m_hInst = hInstance; return TRUE;};

protected:
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnMouseMove(HWND hWnd, POINT& pt);
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
//	BOOL	OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnGetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnMenuLeft();
	BOOL	OnMenuRight();
	BOOL	OnMenuContext(POINT pt);

		
private:	// Functions
    BOOL    DrawBackground(CIssGDIEx& gdiMem, RECT& rcClient, RECT& rcClip);

	BOOL	OnHelp();
	void	GetTime();

	BOOL	OnMenuSystem();
	BOOL	OnMenuCPU();
	BOOL	OnMenuMemory();
	BOOL	OnMenuStorage();
	BOOL	OnMenuSystemMetrics();
	BOOL	OnMenuNetworking();
	BOOL	OnMenuCommPorts();
	BOOL	OnMenuDisplay();
	BOOL	OnMenuTime();
	BOOL	OnMenuPower();
    BOOL    OnMenuBenchmark();
    BOOL    OnMenuHardwareTest();
    BOOL    OnMenuAudio();
    BOOL    OnMenuGPS();
	
private:	// Variables
	CIssGDIEx			m_gdiMem;
    CIssGDIEx           m_gdiBack;
	HFONT				m_hFontTime;
	HFONT				m_hFontText;			// normal text font
    HFONT               m_hFontBtnLabel;

	SIZE				m_sizeWindow;			// size of the Window, used to prevent reloading of images

	SYSTEMTIME			m_sSysTime;
	SYSTEMTIME			m_sAlarmTime;

    CIssImageSliced	    m_imgButton;            // kinetic list button
    CIssImageSliced	    m_imgButtonSel;         // kinetic list selected button
	
	TCHAR				m_szTime[STRING_LARGE];
	TCHAR				m_szAlarmTime[STRING_LARGE];
	TCHAR				m_szDate[STRING_LARGE];
	TCHAR				m_szTemp[STRING_LARGE];

	RECT				m_rcTime;
	RECT				m_rcDate;
	RECT				m_rcAlarm1Label;

    int                 m_iHiScore;
};

