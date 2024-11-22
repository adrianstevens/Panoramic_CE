#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "PoomContacts.h"
#include "CallList.h"

#define IDMENU_ChangeItem			106

class DlgContactCallLog : public CIssWnd
{
public:
	DlgContactCallLog(void);
	~DlgContactCallLog(void);

	BOOL			Init(IContact* pIContact, TypeContact* psContact, CIssGDIEx* gdiBg);
	BOOL			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

	BOOL			OnLButtonUp(HWND hWnd, POINT& pt);	
	BOOL			OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnMouseMove(HWND hWnd, POINT& pt);
	BOOL			OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
	BOOL			DrawBackground(HDC hdc, RECT rcClient);
	BOOL			DrawNavButtons(HDC hdc, RECT rcClient);
	BOOL			DrawHeader(HDC hdc, RECT rcClient);
	BOOL			DrawContactPic(HDC hdc, RECT rcClient);

	void			DrawGradientGDI(HDC tdc, RECT rc, COLORREF StartRGB, COLORREF EndRGB);
	void			OnContextMenu(POINT pt);

private:
	CIssGDIEx			m_gdiMem;
	CIssGDIEx			m_gdiBackground;
	CIssGDIEx*		m_gdiFadeInBg;
	CIssGDIEx			m_gdiNavButton;
	CIssGDIEx			m_gdiPic;


	BOOL			m_bFadeIn;
	CCallList		m_oCallList;
	TypeContact*	m_sContact;


	HFONT			m_hFontNormal;
	HFONT			m_hFontBold;

	POINT			m_ptLButtonDown;
	RECT			m_rcBackBtn;
	RECT			m_rcHeader;
	RECT			m_rcListBox;
	RECT			m_rcFooter;
	RECT			m_rcPicture;
	RECT			m_rcContactName;
};
