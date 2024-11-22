#include "WndStatic.h"
#include "IssGDI.h"

CWndStatic::CWndStatic(void)
:left(0)
,right(0)
,top(0)
,bottom(0)
,m_szText(NULL)
,m_hFont(NULL)
{
}

CWndStatic::~CWndStatic(void)
{
	m_oStr->Delete(&m_szText);
}

BOOL CWndStatic::Create(HWND hWndParent, HINSTANCE hInst, UINT uiFlags)
{
	m_bHandlePaint	= TRUE;
	m_eWndType		= WND_Window;
	m_hInst			= hInst;

	m_hWnd = CreateWindowEx(0, _T("Static"), NULL, WS_CHILD | WS_VISIBLE | uiFlags, 0, 0, 0, 0, hWndParent, NULL, m_hInst, NULL);
	if(!m_hWnd)
		return FALSE;

	//and now override the procs
	SetWindowLong( m_hWnd, GWL_USERDATA, reinterpret_cast<long>( this ) ); 
	m_lpfnStatic = (WNDPROC)::GetWindowLong(m_hWnd, GWL_WNDPROC);
	SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)ProcWnd);

	ShowWindow(m_hWnd, SW_SHOW);
	return TRUE;
}

BOOL CWndStatic::MoveWindow(RECT& rc)
{
	left	= rc.left;
	right	= rc.right;
	top		= rc.top;
	bottom	= rc.bottom;
	::MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
	return TRUE;
}

BOOL CWndStatic::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	CIssGDI gdiMem;
	gdiMem.Create(hDC, rcClient, FALSE, FALSE, FALSE);
	CIssGDI::FillRect(gdiMem, rcClient, RGB(255,255,255));

#ifdef WIN32_PLATFORM_WFSP
	if(GetFocus() == m_hWnd)
		DrawFocusRect(gdiMem, &rcClient);
#endif

	TypeStatic sStatic;
	sStatic.hWndFrom	= m_hWnd;
	sStatic.hDC			= gdiMem.GetDC();
	sStatic.rcClient	= rcClient;

	if(m_oStr->IsEmpty(m_szText))
	{
		// tell the parent to draw the content
		SendMessage(GetParent(m_hWnd), WM_DRAWSTATIC, NULL, (LPARAM)&sStatic);
	}
	else
	{
		DrawStaticText(&sStatic, m_szText, 0, NULL, m_hFont);
	}

	RECT rcClip;
	GetClipBox(hDC, &rcClip);

	// now draw it all to the screen
	BitBlt(hDC,
		   rcClip.left, rcClip.top,
		   rcClip.right-rcClip.left, rcClip.bottom-rcClip.top,
		   gdiMem.GetDC(),
		   rcClip.left, rcClip.top,
		   SRCCOPY);

	return TRUE;
}

BOOL CWndStatic::ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc(m_lpfnStatic, hWnd, message, wParam, lParam);
}

BOOL CWndStatic::OnGetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(m_hWnd, NULL, FALSE);
	return TRUE;
}

BOOL CWndStatic::OnLostFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(m_hWnd, NULL, FALSE);
	return TRUE;
}

BOOL CWndStatic::SetText(TCHAR* szText)
{
	m_oStr->Delete(&m_szText);
	m_szText = m_oStr->CreateAndCopy(szText);
	return TRUE;
}

BOOL CWndStatic::SetText(UINT uiText)
{
	if(!m_hWnd || !m_hInst)
		return FALSE;

	m_oStr->Delete(&m_szText);
	TCHAR szText[MAX_PATH] = _T("");
	m_oStr->StringCopy(szText, uiText, MAX_PATH, m_hInst);
	return SetText(szText);
}

BOOL CWndStatic::SetFont(HFONT hFont)
{
	if(!m_hWnd)
		return FALSE;
	m_hFont = hFont;
	return TRUE;
}

void CWndStatic::DrawStaticText(TypeStatic* sStatic, TCHAR* szText, UINT uiText /*= 0*/, HINSTANCE hInst /*= NULL*/, HFONT hFont /*= NULL*/)
{
	// if we supplied a string resource
	if(uiText && hInst)
	{
		TCHAR szNewText[STRING_LARGE];
		CIssString* oStr = CIssString::Instance();
		oStr->StringCopy(szNewText, uiText, STRING_LARGE, hInst);
		szText = szNewText;
	}

	CIssGDI::DrawText(sStatic->hDC, szText, sStatic->rcClient, DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS, hFont, (IsWindowEnabled(sStatic->hWndFrom)?0:GetSysColor(COLOR_GRAYTEXT)));
}

BOOL CWndStatic::IsTextToLong(TCHAR* szText, HFONT hFont, HDC hDC)
{
	if(m_hWnd == NULL || m_hInst == NULL)
		return FALSE;//don't want to do anything if these fail

    HFONT hOldFont = NULL;

	RECT rc;

	GetClientRect(m_hWnd, &rc);

	// set the drawing options
	if(hFont)
		hOldFont = (HFONT)SelectObject(hDC, hFont);


	SIZE sTemp;
	GetTextExtentPoint(hDC, szText, m_oStr->GetLength(szText), &sTemp);

	//put the font back yo
	if(hOldFont)
		SelectObject(hDC, hOldFont);    

	if(sTemp.cx > WIDTH(rc))
		return TRUE;//over sized

	return FALSE;
}