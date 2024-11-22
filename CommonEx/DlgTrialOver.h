#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssVector.h"
#include "IssKineticList.h"
#include "DlgMoreApps.h"

struct TypePurchase
{
	TCHAR*	szTitle;
	TCHAR*	szDescription;
	TCHAR*	szURLLocation;
	int		iIconIndex;
	TypePurchase();
	~TypePurchase();
};

class CDlgTrialOver:public CIssWnd
{
public:
	CDlgTrialOver(void);
	~CDlgTrialOver(void);

	void	Initialize(UINT uiTitle,
					   UINT uiMessage,
					   UINT uiNoMoreAppsMessage,
					   UINT uiClose,
					   UINT uiImageArray,
					   UINT uiBuyBtn,
					   UINT uiMenuArray);
	void	AddItem(int iIconIndex, 
					TCHAR* szTitle,
					TCHAR* szDescription,
					TCHAR* szURL);
	BOOL	Launch(HINSTANCE hInst, HWND hWndParent = NULL);

protected:
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL    OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL    OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL    OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnMouseMove(HWND hWnd, POINT& pt);
	BOOL    OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL    OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};

private:
	void	CloseWindow();
	void	RefreshList();
	void	SelectItem();
	void	RecalculateHeights();
	void	DrawBackground(RECT& rcClip);
	void	DrawTitle(RECT& rcClip);
	void	DrawMessage(RECT& rcClip);
	void	DrawMenuBar(RECT& rcClip);
	int		GetFontHeight();
	int		GetFontSmallHeight();
	int		GetTextHeight(TCHAR* szText, int iWidth, HFONT hFont);

	static void DeleteMyListItem(LPVOID lpItem);
	static void DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
	void	DrawMyListItem(CIssGDIEx& gdi, RECT& rcDraw, TypePurchase* sItem, BOOL bIsHighlighted);

private:
	HFONT		m_hFont;
	HFONT		m_hFontSmall;
	int			m_iFontHeight;
	int			m_iFontSmallHeight;
	CIssGDIEx	m_gdiMem;
	CIssGDIEx	m_gdiBackground;
	CIssGDIEx	m_gdiImageArray;
	CIssGDIEx	m_gdiBuyApp;
	int			m_iNumImages;
	RECT		m_rcTitle;
	RECT		m_rcTrialOverMessage;
	RECT		m_rcMenuBar;	
	TCHAR*		m_szTitle;
	TCHAR*		m_szMessage;
	TCHAR*		m_szClose;
	CIssKineticList m_oList;
	CIssVector<TypePurchase> m_arrItems;
	BOOL		m_bBtnDown;
	UINT		m_uiTitle;
	UINT		m_uiMessage;
	UINT		m_uiNoMoreAppsMessage;
	UINT		m_uiClose;
	UINT		m_uiImageArray;
	UINT		m_uiBuyBtn;
	UINT		m_uiMenuArray;
};
