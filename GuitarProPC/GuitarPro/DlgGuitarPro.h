#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "ObjGui.h"

#include "GuiGuitarChords.h"
#include "GuiGuitarTuner.h"
#include "GuiMetronome.h"
#include "GuiPlay.h"
#include "GuiTunerEar.h"
#include "GuiPlayFav.h"

enum EnumMenuUp
{
    MENUUP_Chords,
    MENUUP_Tuner,
    MENUUP_None,
};

class CDlgGuitarPro:public CIssWnd
{
public:              
	CDlgGuitarPro(HINSTANCE hInst);
	~CDlgGuitarPro(void);

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
    BOOL    OnMouseMove(HWND hWnd, POINT& pt);
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnMCNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){LaunchHelp(); return TRUE;};

private:	// Functions
    void    LoadRegistry();
    void    SaveRegistry();


	BOOL	DrawText(HDC hDC, RECT& rcClip);
    BOOL    DrawMenuUp(CIssGDIEx& gdi, RECT& rcClip);
    BOOL    DrawMenuExtras(CIssGDIEx& gdi, RECT& rcClip);
    BOOL    DrawMenu(CIssGDIEx& gdi, RECT& rcClip, int iHLIndex = -1);

	BOOL	ShouldAbort();

	void	LoadImages(HWND hWndSplash, int iMaxPercent);

    BOOL    OnMenuChords();
    BOOL    OnMenuPlay();
    BOOL    OnMenuChordSel();//for the top menu ... good enough
    BOOL    OnMenuPlayFav();

	BOOL	OnMenuMoreApps();
    BOOL    OnMenuTuner();
    BOOL    OnMenuTunerEar();
    BOOL    OnMenuTunerDigital();


    BOOL    OnMenuMetronome();
    BOOL    OnMenuMenu();


	BOOL	OnOptions();
	BOOL	OnAbout();


    void    SetCurrentGui(EnumGUI eGui);

    void    PanRight();

    void    LaunchHelp();

	void	SetWindowSize();

private:	// Variables
    CGuiGuitarTuner     m_guiGuitarTuner;
    CGuiGuitarChords    m_guiGuitarChords;
    CGuiMetronome       m_guiMetronome;
    CGuiPlay            m_guiPlay;
    CGuiPlayFav         m_guiPlayFav;
    CGuiTunerEar        m_guiTunerEar;
    CGuiBase*           m_guiCurrent;//this should work just fine

	SIZE		        m_sizeWindow;			// size of the Window, used to prevent reloading of images

    RECT                m_rcBottomBar;
    RECT                m_rcMenu[4];
    RECT                m_rcMenuUp[4];
    RECT                m_rcMenuUpFull;

    EnumMenuUp          m_eMenuUp;

    BOOL                m_bPanRight;

    int                 m_iMenuGlow;
};
