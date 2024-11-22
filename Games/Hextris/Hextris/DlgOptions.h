#pragma once

#include "IssWnd.h"
#include "IssGDI.h"
#include "WndSlider.h"
#include "IssSoundFX.h"
#include "HexGame.h"
#include "IssHardwareKeys.h"

class CDlgOptions:public CIssWnd
{
public:
	CDlgOptions(void);
	~CDlgOptions(void);
	void	Init(CIssSoundFX* oSoundFX, CHexGame* oGame, CIssHardwareKeys* oKeys){m_oSoundFX = oSoundFX;m_oGame = oGame; m_oKeys = oKeys;};

protected:
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	OnCtlColorStatic(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:	// functions
	BOOL	FadeInScreen(HDC hDC, RECT rcClient);
	void	InitBackground();
	void	DrawBackground(RECT rcLocation);
	void	DrawText();
	void	MoveControls(RECT rcClient);
	void	CreateControls();
	int		ConvertTo10(int iValue);
	int		ConvertTo64(int iValue);
	void	MyDrawText(UINT uiText, RECT rcLocation, UINT uiFormat = DT_VCENTER, COLORREF crText = RGB(255,255,255), COLORREF crShadow = RGB(0,0,0));
	HBRUSH	CreateFPSBrush();
	HBRUSH	CreatePowerBrush();
	void	UpdateGameDescription();

private:	// variables
	CIssHardwareKeys* m_oKeys;
	CIssSoundFX*	m_oSoundFX;
	CHexGame*		m_oGame;
	BOOL			m_bFirstTime;
	
	CIssGDI			m_gdiMem;
	CIssGDI			m_gdiBackground;
	
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
	
	CWndSlider		m_sldMusic;					// music slider
	CWndSlider		m_sldEffects;				// sound effects slider
	
	HWND			m_chkFPS;					// frames per second checkbox
	HWND			m_chkPower;					// power saving
	HWND			m_lstGameMode;				// Game mode drop down box
	HWND			m_hSpin1;					// spinner for smarpthone only
	HWND			m_lstStartLevel;			// starting level
    HWND            m_hSpin2;
	
	RECT			m_rcBack;					// location of the back button
	RECT			m_rcLabelMusic;				// location of the music label
	RECT			m_rcLabelEffects;			// location of the effects label
	RECT			m_rcLabelFPS;				// location of the FPS label
	RECT			m_rcLabelPowerSaving;		// location of the Power Saving Mode		
//	RECT			m_rcLabelGameMode;			// location of the game mode label
	RECT			m_rcLabelDesc;				// location of the description for the game mode

	HBRUSH			m_hBkFPS;					// background brush for the FPS control
	HBRUSH			m_hBkPower;					// background brush for the FPS control

	
};
