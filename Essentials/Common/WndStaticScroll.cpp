#include "WndStaticScroll.h"
#include "globals.h"

#define SCROLL_Step		3
#define ARROW_Right		1
#define ARROW_Left		2

CWndStaticScroll::CWndStaticScroll(void)
:m_szText(NULL)
,m_bCanScrollRight(FALSE)
,m_iCursorPos(0)
{
}

CWndStaticScroll::~CWndStaticScroll(void)
{
	m_oStr->Delete(&m_szText);
}

void CWndStaticScroll::SetText(TCHAR* szText)
{
	m_oStr->Delete(&m_szText);
	m_szText = m_oStr->CreateAndCopy(szText);
	InvalidateRect(m_hWnd, NULL, FALSE);
}

BOOL CWndStaticScroll::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(!m_szText)
		return UNHANDLED;

	int iOldCursorPos = m_iCursorPos;
//	HWND hWndTab;

	switch(LOWORD(wParam))
	{
	case VK_RIGHT:
		m_iCursorPos = min(m_iCursorPos+SCROLL_Step, m_oStr->GetLength(m_szText));
		break;
	case VK_LEFT:
		m_iCursorPos = max(m_iCursorPos-SCROLL_Step, 0);
		break;
	case VK_UP:
	//	hWndTab = GetTabbedWindow(GW_HWNDPREV);
	//	SetFocus(hWndTab);
		break;
	case VK_DOWN:
	//	hWndTab = GetTabbedWindow(GW_HWNDNEXT);
	//	SetFocus(hWndTab);
		break;
	default:
		return UNHANDLED;
	}

	if(iOldCursorPos!=m_iCursorPos)
		InvalidateRect(m_hWnd, NULL, FALSE);

	return TRUE;
}

BOOL CWndStaticScroll::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	COLORREF	dwColor;
	HPEN		hNewPen = NULL;
	HPEN		hBorderPen = NULL;
	HPEN		hSavePen = NULL;
	HBRUSH		hNewBrush = NULL;
	HBRUSH		hSaveBrush = NULL;
	HBRUSH		hStockBrush = NULL;
	RECT		rc;
	RECT		rcArrow;

	//
	// draw/clear the border
	//
	hStockBrush = (HBRUSH) GetStockObject(NULL_BRUSH);
	if (GetFocus() == m_hWnd)
	{
		dwColor = GetSysColor(COLOR_ACTIVEBORDER);
		hBorderPen = ::CreatePen(PS_SOLID, 1, dwColor);
	}
	else
	{
		dwColor = GetSysColor(COLOR_WINDOW);
		hBorderPen = ::CreatePen(PS_SOLID, 1, dwColor);
	}
	if (hBorderPen)
	{
		hSavePen = (HPEN) ::SelectObject(hDC, hBorderPen);
		if (hStockBrush)
		{
			hSaveBrush = (HBRUSH) ::SelectObject(hDC, hStockBrush);
		}

		GetClientRect(m_hWnd, &rc);
		Rectangle(hDC, rc.left, rc.top, RECT_Width(rc), RECT_Height(rc));

		::SelectObject(hDC, hSavePen);
		if (hStockBrush)
			::SelectObject(hDC, hSaveBrush);

		hSavePen = NULL;
		hSaveBrush = NULL;
		::DeleteObject(hBorderPen);
	}

	//
	// Draw contents
	//
	if (!m_oStr->IsEmpty(m_szText))
	{
		//
		// Draw scrolling arrows
		//

		// Get the color with which we want to draw our scrolling arrows.
		dwColor = GetSysColor(COLOR_WINDOWTEXT);

		// Get our pen and brush
		hNewPen = ::CreatePen(PS_SOLID, 1, dwColor);
		hNewBrush = ::CreateSolidBrush(dwColor);
		if (hNewPen && hNewBrush)
		{
			hSavePen = (HPEN) ::SelectObject(hDC, hNewPen);
			hSaveBrush = (HBRUSH) ::SelectObject(hDC, hNewBrush);
		}

		// Pad the top and bottom of the arrow
		GetClientRect(m_hWnd, &rc);
		rc.top			+= 1;
		rc.bottom		-= 1;

		int iIndent		= TEXT_INDENT;
		int iArrowWidth = RECT_Height(rc)/2;
		rcArrow			= rc;
		rcArrow.top		+= iIndent;
		rcArrow.bottom	-= iIndent;

		// First, the left one. Easy decision-- are we cursored in?
		if (m_iCursorPos > 0)
		{
			rcArrow.left	= rc.left + iIndent;
			rcArrow.right	= rcArrow.left + iArrowWidth;
			DrawArrow(hDC, rcArrow, ARROW_Left);

			// Must indent the text now, because we're making room for the arrow.
			rc.left += (rcArrow.right + iIndent);
		}
		else
		{
			rc.left += iIndent;
		}

		// Now, the right one.
		rc.right -= iIndent;
		if (NeedRightScroll(hDC, rc, (m_szText + m_iCursorPos)))
		{
			rcArrow.right	= rc.right;
			rcArrow.left	= rcArrow.right - iArrowWidth;
			DrawArrow(hDC, rcArrow, ARROW_Right);

			// Must cut short out text, to make room for the arrow.
			rc.right = rcArrow.left - iIndent;

			// Report out that we have a right scroll arrow.
			m_bCanScrollRight = TRUE;
		}
		else
		{
			m_bCanScrollRight = FALSE;
		}

		DrawText(hDC, m_szText + m_iCursorPos, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER| DT_NOPREFIX);

		if (hSavePen)
			::SelectObject(hDC, hSavePen);

		if (hSaveBrush)
			::SelectObject(hDC, hSaveBrush);

		::DeleteObject(hNewPen);
		::DeleteObject(hNewBrush);
	}
	return TRUE;
}

void CWndStaticScroll::DrawArrow(HDC hdc, RECT& rc, BYTE btArrow)
{
	POINT pt[3];

	// make we need to draw the arrow
	if(!(btArrow & ARROW_Left) && !(btArrow & ARROW_Right))
		return;

	// We assume the right pens and brushes are selected into the HDC.
	if (btArrow & ARROW_Left)
	{
		pt[0].x = pt[2].x = rc.right;
		pt[1].x = rc.left;
	}
	else
	{
		pt[0].x = pt[2].x = rc.left;
		pt[1].x = rc.right;
	}

	pt[0].y = rc.top;
	pt[1].y = rc.top + ((rc.bottom-rc.top) / 2);
	pt[2].y = rc.bottom;

	::Polygon(hdc, pt, 3);
}

BOOL CWndStaticScroll::NeedRightScroll(HDC hdc, RECT& rc, TCHAR* szText)
{
	RECT rcCalcRect = rc;

	// Basically we'll just do a DrawText(DT_CALCRECT) to see
	// how the string fits in the rc. We return TRUE if the text
	// doesn't completely fit.
	DrawText(hdc, szText, -1, &rcCalcRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER| DT_NOPREFIX | DT_CALCRECT);

	return (BOOL)(rcCalcRect.right > rc.right);
}
