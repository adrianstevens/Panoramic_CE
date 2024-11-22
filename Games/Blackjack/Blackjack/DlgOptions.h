#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssBJGame.h"

class CDlgOptions:public CIssWnd
{
public:
	CDlgOptions(void);
	~CDlgOptions(void);
	void	Init(TypeOptions* psOptions){m_psOptions = psOptions;};

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
	void	MyDrawText(UINT uiText, RECT rcLocation, UINT uiFormat = DT_VCENTER, COLORREF crText = RGB(255,255,255), COLORREF crShadow = RGB(0,0,0));
	HBRUSH	CreatePlaySoundsBrush();
	void	UpdateCardCountingOptions();

private:	// variables
	TypeOptions*    m_psOptions;
	BOOL			m_bFirstTime;
	
	CIssGDIEx		m_gdiMem;
	CIssGDIEx		m_gdiBackground;
	
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
	
	HWND			m_chkPlaySounds;			// Play Sounds
	HWND			m_lstCardCountView;			// How should the card count be displayed
	HWND			m_hSpin1;					// spinner for smarpthone only
	HWND			m_lstCardCounting;			// How should the cards be counted
    HWND            m_hSpin2;
    HWND            m_lstGameDifficulty;        // Game difficulty
    HWND            m_hSpin3;
    HWND            m_lstLanguage;              // Language
    HWND            m_hSpin4;
	
	RECT			m_rcBack;					// location of the back button
	RECT			m_rcLabelPlaySounds;		// location of the Play Sounds label
	RECT			m_rcLabelCardCountView;		// location of the Card count view label
	RECT			m_rcLabelCardCounting;		// location of the Card counting label
	RECT			m_rcLabelDifficulty;		// location of the Game difficulty label		
    RECT			m_rcLabelLanguage;		// location of the Game difficulty label	

	HBRUSH			m_hBkPlaySounds;			// background brush for the Play sounds control
	
};
