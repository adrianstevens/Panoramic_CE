#pragma once

#include "isswnd.h"
#include "GuiBanjoTuner.h"
#include "GuiGuitarChords.h"
#include "GuiGuitarTuner.h"
#include "GuiMetronome.h"

#define REG_KEY		        _T("SOFTWARE\\Pano\\GuitarTuner")

class CDlgGuitarTuner :
	public CIssWnd
{
public:
	CDlgGuitarTuner(void);
public:
	~CDlgGuitarTuner(void);
	void	Init(HINSTANCE hInst);

private:
	void	SaveRegSettings();
	void	LoadRegSettings();

	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnMouseMove(HWND hWnd, POINT& pt);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnHandleNavBtn(HWND hWnd, WPARAM wParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
    BOOL    OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL    OnMenuMenu();
    BOOL    OnMenuMode();

    BOOL    OnOptions();
    BOOL    OnAbout();


private:
    CGuiGuitarTuner     m_guiGuitarTuner;
    CGuiGuitarChords    m_guiGuitarChords;
    CGuiMetronome       m_guiMetronome;
    CGuiBase*           m_guiCurrent;//this should work just fine

    SIZE				m_sizeWindow;				// size of the Window, used to prevent reloading of images
};
