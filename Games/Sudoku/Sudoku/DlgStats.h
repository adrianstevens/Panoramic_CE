#pragma once

#include "IssWnd.h"
#include "ObjGui.h"

class CDlgStats:public CIssWnd
{
public:
	CDlgStats(void);
	~CDlgStats(void);

    void    Init(CSEngine* oEngine);


protected:
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL    OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL    OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnMouseMove(HWND hWnd, POINT& pt);

private:	// functions
	BOOL	FadeInScreen(HDC hDC, RECT rcClient);

    void    PopulateList();

    void    ResetStats();

    void        DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    static void DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void DeleteListItem(LPVOID lpItem);

private:	// variables
    CIssKineticList     m_oMenu;
    CSEngine*       m_eEngine;

	BOOL			m_bFirstTime;
	
	int				m_iLogoHeight;
	HFONT			m_hFontText;				// Font
	HFONT			m_hFontSelected;			// Selected Font size
};
