#pragma once
#include "stdafx.h"
#include "IssGDI.h"
#include "IssWnd.h"
#include "HexGame.h"

#define REG_KEY			_T("SOFTWARE\\iSS\\Hexaminoes")

enum EnumColor
{
	COLOR_Color1,
	COLOR_Color2,
	COLOR_Outline,
};

enum EnumBGColor
{
	BGCOLOR_Grad1,
	BGCOLOR_Grad2,
	BGCOLOR_Hex,
	BGCOLOR_Grid,
};



class CDlgHextris:public CIssWnd
{
public:
	CDlgHextris(void);
	~CDlgHextris(void);

	BOOL	Init(HINSTANCE hInstance){m_hInst = hInstance; return TRUE;};

protected:
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnGetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnMenuLeft();
	BOOL	OnMenuRight();
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);


	BOOL	DrawHexagon(HDC hdc, POINT ptGridLocation, COLORREF rgbOutline, COLORREF rgbColor, int iSize, BOOL bDrawHiLite = FALSE);
	BOOL	DrawHexagon(HDC hdc, POINT ptLocation, int iColorIndex);
	BOOL	DrawBackground(HDC hdc, RECT rcClient);
	BOOL	DrawCurrentPiece(HDC hdc, RECT rcClient);
	BOOL	DrawGameBoard(HDC hDC, RECT rcClient);
	BOOL	DrawText(HDC hDC, RECT rcClient);
	BOOL	DrawPiecePreview(HDC hDC, RECT rcClient);

	BOOL	DrawGhost(HDC hdc, RECT rcClient);
			
private:	// Functions
	
	void	SaveRegistry();

	void	NewGame();

	void	AnimateLineCleared();
	void	AnimateLevelChangeChallenge();
	void	AnimateEndGame();

	void	Pause();
	void	Resume();

	void	PlaySounds(TCHAR* szWave);

	void	CreateGDIHex(HDC hdc);

	COLORREF	GetBackgroundColor(EnumBGColor eColor);

private:	// Variables
	CIssGDI				m_gdiTitle;
	CIssGDI				m_gdiMem;
	CIssGDI				m_gdiBackground;

	CIssGDI				m_gdiHex;

	CHexGame			m_oGame;

	HFONT				m_hFontText;
	
	RECT				m_rcTitle;
	RECT				m_rcScore;
	RECT				m_rcLevel;
	RECT				m_rcLines;


	int					m_iHexPreviewSize;	//this is actually half the width ... its the length of each of the six sides
	int					m_iHexSize;			//since it'll vary
	SIZE				m_sizeHex;
	SIZE				m_sizePreview;		//these are the the actual widths and heights ... cx is double m_iHexPrev....
	int					m_iHexSizeCos30;

	//from the top left corner ... so NOT absolute left (it'll be indented half the hax size
	POINT				m_ptGameField[GAME_WIDTH][MAX_GAME_HEIGHT];//we'll save em all when we draw the background

	POINT				m_ptPreview1[4][5];
	POINT				m_ptPreview2[4][5];
	POINT				m_ptSaved[4][5];

	BOOL				m_bPlaySounds;
	BOOL				m_bShowGhost;
	BOOL				m_bPause;


	//Controls
	WORD				m_wLeft;
	WORD				m_wRight;
	WORD				m_wDown;
	WORD				m_wDrop;
	WORD				m_wRotate;
	WORD				m_wRotateAlt;
	WORD				m_wSwitch;

	DWORD				m_dwStartTime;
	DWORD				m_dwFrameCount;
									


};