#pragma once

#include "WndStatic.h"

class CWndStaticScroll:public CWndStatic
{
public:
	CWndStaticScroll(void);
	~CWndStaticScroll(void);

	void	SetText(TCHAR* szText);
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
	void	DrawArrow(HDC hdc, RECT& rc, BYTE btArrow);
	BOOL	NeedRightScroll(HDC hdc, RECT& rc, TCHAR* szText);

private:	// variables
	TCHAR* m_szText;
	int		m_iCursorPos;
	BOOL	m_bCanScrollRight;
};
