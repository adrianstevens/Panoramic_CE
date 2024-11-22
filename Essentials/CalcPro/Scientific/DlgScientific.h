#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "CalcInterace.h"
#include "IssCalculator.h"
#include "EditCtrl.h"
#include "IssFormatCalcDisplay.h"
#include "globals.h"
#include "IssKey.h"

class CDlgScientific:public CIssWnd
{
public:
	CDlgScientific(HINSTANCE hInst);
	~CDlgScientific(void);

	BOOL	Init(HINSTANCE hInstance){m_hInst = hInstance; return TRUE;};
	BOOL	Destroy();

protected:
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
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
	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("PanoScientific.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};
	BOOL	OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
    

	BOOL	OnMenuLeft();
	BOOL	OnMenuRight();
	
private:	// Functions
	void	PlaySounds(TCHAR* szWave);
	BOOL	OnHandleNavBtn(WPARAM wParam);

private:	// Variables
    CIssKey         m_oKey;
	CEditCtrl		m_oEdit;

	CIssGDIEx   	m_gdiMem;				// main memory DC
    CIssGDIEx       m_gdiMenu;              // menu image
	CCalcInterace*	m_oInterface;
	CIssCalculator*  m_oCalc;
    CIssFormatCalcDisplay* m_oFormat;

	SIZE			m_sizeWindow;			// size of the Window, used to prevent reloading of images
	
	BOOL			m_bPlaySounds;			// Settings for playing sounds

};
