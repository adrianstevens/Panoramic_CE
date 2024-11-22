#include "WndSlider.h"
#include "IssGDIEffects.h"
#include "IssCommon.h"

#define HEIGHT_Button		GetSystemMetrics(SM_CYICON)
#define HEIGHT_BackBar		GetSystemMetrics(SM_CXSMICON)/4

#define SLIDER_Indent		GetSystemMetrics(SM_CXSMICON)


CWndSlider::CWndSlider(void)
:m_iMin(0)
,m_iMax(100)
,m_iStep(10)
,m_iPos(0)
,m_iNumSteps(5)
,m_bLockToStep(FALSE)
,m_bMouseDown(FALSE)
{
	m_ptSliderLoc.x = 0;
	m_ptSliderLoc.y = 0;
}

CWndSlider::~CWndSlider(void)
{
}

void CWndSlider::Init(UINT uiNotif)
{
	m_uiNotif				= uiNotif;
}

BOOL CWndSlider::Create(HWND hWndParent, HINSTANCE hInst)
{
	m_bHandlePaint	= TRUE;
	m_eWndType		= WND_Window;
	m_hInst			= hInst;

	m_hWnd = CreateWindowEx(0, _T("Static"), NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, 0, 0, 0, hWndParent, NULL, m_hInst, NULL);
	if(!m_hWnd)
		return FALSE;

	//and now override the procs
	SetWindowLong( m_hWnd, GWL_USERDATA, reinterpret_cast<long>( this ) ); 
	m_lpfnStatic = (WNDPROC)::GetWindowLong(m_hWnd, GWL_WNDPROC);
	SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)ProcWnd);

	ShowWindow(m_hWnd, SW_SHOW);
	return TRUE;
}

BOOL CWndSlider::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CWndSlider::OnPaint(HWND hWnd, HDC hDC, RECT& rcClient)
{
	if(m_gdiMem.GetDC() == NULL)
		m_gdiMem.Create(hDC, rcClient, FALSE, FALSE, FALSE);

	DrawBackground(m_gdiMem.GetDC(), rcClient);
	DrawButton(m_gdiMem.GetDC(), rcClient);

	if(GetFocus() == m_hWnd)
		DrawFocusRect(m_gdiMem, &rcClient);

	// now draw it all to the screen
	BitBlt(hDC,
		rcClient.left, rcClient.top,
		WIDTH(rcClient), HEIGHT(rcClient),
		m_gdiMem.GetDC(),
		rcClient.left, rcClient.top,
		SRCCOPY);
	return TRUE;
}

BOOL CWndSlider::OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_gdiBackground.Destroy();
	m_gdiMem.Destroy();
	m_gdiSlider.Destroy();

	CalcStepSize();
	return TRUE;
}

BOOL CWndSlider::OnLButtonDown(HWND hWnd, POINT& pt)
{
	SetCapture(hWnd);
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	m_bMouseDown = TRUE;
	m_ptSliderLoc.x = pt.x - m_gdiSlider.GetWidth()/2;
	/*if(m_ptSliderLoc.x < SLIDER_Indent + rcClient.left - m_gdiSlider.GetWidth()/2)
		m_ptSliderLoc.x = SLIDER_Indent + rcClient.left - m_gdiSlider.GetWidth()/2;
	else if(m_ptSliderLoc.x > rcClient.right - SLIDER_Indent  - m_gdiSlider.GetWidth()/2)
		m_ptSliderLoc.x = rcClient.right - SLIDER_Indent  - m_gdiSlider.GetWidth()/2;*/

	if(m_ptSliderLoc.x > 60000)	// this is the check for slider going out of bounds
		m_ptSliderLoc.x = SLIDER_Indent - m_gdiSlider.GetWidth()/2;
	else if(m_ptSliderLoc.x > (m_iStepSize*(m_iMax-m_iMin) + SLIDER_Indent - m_gdiSlider.GetWidth()/2))
		m_ptSliderLoc.x = m_iStepSize*(m_iMax-m_iMin) + SLIDER_Indent - m_gdiSlider.GetWidth()/2;
	else if (m_ptSliderLoc.x < (SLIDER_Indent - m_gdiSlider.GetWidth()/2))
		m_ptSliderLoc.x = SLIDER_Indent - m_gdiSlider.GetWidth()/2;

	InvalidateRect(m_hWnd, NULL, FALSE);
	return TRUE;
}

BOOL CWndSlider::OnLButtonUp(HWND hWnd, POINT& pt)
{
	if(m_bMouseDown)
	{
		m_bMouseDown = FALSE;
		CalcSliderValue();
		CalcSliderLocation();
		InvalidateRect(m_hWnd, NULL, FALSE);
		ReleaseCapture();
	}
	return TRUE;
}

BOOL CWndSlider::OnMouseMove(HWND hWnd, POINT& pt)
{
	if(m_bMouseDown)
	{
		RECT rcClient;
		GetClientRect(m_hWnd, &rcClient);

		m_ptSliderLoc.x = pt.x - m_gdiSlider.GetWidth()/2;
		//keep it within the "steps"
		/*if(m_ptSliderLoc.x < SLIDER_Indent + rcClient.left - m_gdiSlider.GetWidth()/2)
			m_ptSliderLoc.x = SLIDER_Indent + rcClient.left - m_gdiSlider.GetWidth()/2;
		else if(m_ptSliderLoc.x > rcClient.right - SLIDER_Indent  - m_gdiSlider.GetWidth()/2)
			m_ptSliderLoc.x = rcClient.right - SLIDER_Indent  - m_gdiSlider.GetWidth()/2;*/

		//DebugOut(_T("x: %d, max: %d"), m_ptSliderLoc.x, m_iStepSize*(m_iMax-m_iMin) + SLIDER_Indent - m_gdiSlider.GetWidth()/2);

		if(m_ptSliderLoc.x > 60000)	// this is the check for slider going out of bounds
			m_ptSliderLoc.x = SLIDER_Indent - m_gdiSlider.GetWidth()/2;
		else if(m_ptSliderLoc.x > (m_iStepSize*(m_iMax-m_iMin) + SLIDER_Indent - m_gdiSlider.GetWidth()/2))
			m_ptSliderLoc.x = m_iStepSize*(m_iMax-m_iMin) + SLIDER_Indent - m_gdiSlider.GetWidth()/2;
		else if (m_ptSliderLoc.x < (SLIDER_Indent - m_gdiSlider.GetWidth()/2))
			m_ptSliderLoc.x = SLIDER_Indent - m_gdiSlider.GetWidth()/2;

		

		InvalidateRect(m_hWnd, NULL, FALSE);
	}
	return TRUE;
}

BOOL CWndSlider::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case VK_RIGHT:
		AnimateStep(m_iPos, m_iPos+1);
		break;
	case VK_LEFT:
		AnimateStep(m_iPos, m_iPos-1);
		break;
	case VK_DOWN:
	{
		HWND hWndN = GetNextDlgTabItem(GetParent(m_hWnd), m_hWnd, FALSE);
		SetFocus(hWndN);
		break;
	}
	case VK_UP:
		{
			HWND hWndN = GetNextDlgTabItem(GetParent(m_hWnd), m_hWnd, TRUE);
			SetFocus(hWndN);
			break;
		}
	case VK_RETURN:
		break;
	default:
		return UNHANDLED;
	}
	return TRUE;
}

BOOL CWndSlider::ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_GETDLGCODE)
		return (BOOL) (DLGC_WANTARROWS);

	//return CallWindowProc(m_lpfnStatic, hWnd, message, wParam, lParam);
	return (BOOL)DefWindowProc(hWnd, message, wParam, lParam);
}

void CWndSlider::SetPos(int iValue)
{
	if(iValue < m_iMin || iValue > m_iMax || m_iPos == iValue)
		return;
	m_iPos = iValue;
	if(m_hWnd)
		InvalidateRect(m_hWnd, NULL, FALSE);
}

void CWndSlider::SetRange(int iMin, int iMax)
{
	m_iMin	= iMin;
	m_iMax	= iMax;
	m_gdiBackground.Destroy();
	CalcStepSize();
	if(m_hWnd)
		InvalidateRect(m_hWnd, NULL, FALSE);
}

void CWndSlider::SetStep(int iValue)
{
	m_iStep = iValue;
	m_gdiBackground.Destroy();
	CalcStepSize();
	if(m_hWnd)
		InvalidateRect(m_hWnd, NULL, FALSE);

}

void CWndSlider::DrawBackground(HDC hDC, RECT& rcClient)
{
	if(m_gdiBackground.GetDC() == NULL)
	{
		CalcStepSize();//good enough

		m_gdiBackground.Create(hDC, rcClient, FALSE, TRUE, FALSE);

		RECT rcScreen;
		GetWindowRect(m_hWnd, &rcScreen);
		POINT ptLoc;
		ptLoc.x	= rcScreen.left;
		ptLoc.y = rcScreen.top;
		ScreenToClient(GetParent(m_hWnd), &ptLoc);

		// paint the background
		ForceUpdateBackground(m_gdiBackground.GetDC());

		// draw the divider lines
		POINT pt[2];
		
		HPEN hpNew		= CreatePen(PS_SOLID,1,0xFFFFFF);
		HPEN  hpOld		= (HPEN)SelectObject(m_gdiBackground.GetDC(), hpNew);

		for(int i=0; i<(m_iMax-m_iMin)/m_iStep+1; i++)
		{
			pt[0].x = pt[1].x = m_iStepSize*i + SLIDER_Indent;
			pt[0].y		= rcClient.top + HEIGHT_BackBar;
			pt[1].y		= HEIGHT(rcClient)/2 - HEIGHT_BackBar;
			Polyline(m_gdiBackground.GetDC(), pt, 2);

			pt[1].y		= rcClient.bottom - HEIGHT_BackBar;
			pt[0].y		= rcClient.top + HEIGHT(rcClient)/2 + HEIGHT_BackBar;

			Polyline(m_gdiBackground.GetDC(), pt, 2);
		}


		SelectObject(m_gdiBackground.GetDC(), hpOld);
		DeleteObject(hpNew);
	
		// create and draw the slider bar
		int iCurve		= HEIGHT_BackBar;
		CIssGDI gdiArrowBackground;
		RECT rc = rcClient;
		rc.top		= HEIGHT(rcScreen)/2 - HEIGHT_BackBar/2;
		rc.bottom	= rc.top + HEIGHT_BackBar;;

		CIssGDI::DrawGradientRoundRectangle(m_gdiBackground, rc, 0xEEEEEE, iCurve, iCurve, 0x444444, 0xEEEEEE);	
	}

	BitBlt(hDC,
			rcClient.left, rcClient.top,
			WIDTH(rcClient), HEIGHT(rcClient),
			m_gdiBackground.GetDC(),
			rcClient.left, rcClient.top,
			SRCCOPY);

}

void CWndSlider::DrawButton(HDC hDC, RECT& rcClient)
{
	if(m_gdiSlider.GetDC() == NULL)
	{
		RECT rc;
		rc.left		= 0;
		rc.right	= GetSystemMetrics(SM_CXSMICON);
		rc.top		= 0;
		rc.bottom	= GetSystemMetrics(SM_CXSMICON);

		m_gdiSlider.Create(hDC, rc, FALSE, TRUE, FALSE);
		CIssGDI::FillRect(m_gdiSlider.GetDC(), rc, TRANSPARENT_COLOR);

		int iCurve = GetSystemMetrics(SM_CXICON)/4;
		//	CIssGDI::DrawGradientRoundRectangle(m_gdiArrow, rc.Get(), RGB(255,255,255), iCurve, iCurve, RGB(255,255,255), RGB(200,200,200));
		CIssGDI::DrawShinyRoundRectangle(m_gdiSlider, rc, 0xD3BD84, iCurve, iCurve, 0xD3BD84, 0x9E6331, 0x7D3108, 0xDF425A);

		CalcSliderLocation();
	}

	TransparentBlt(	hDC,
					m_ptSliderLoc.x, m_ptSliderLoc.y + rcClient.top,
					m_gdiSlider.GetWidth(), m_gdiSlider.GetHeight(),
					m_gdiSlider.GetDC(),
					0,0,
					m_gdiSlider.GetWidth(), m_gdiSlider.GetHeight(),
					TRANSPARENT_COLOR);

}

void CWndSlider::CalcSliderLocation()
{
	m_ptSliderLoc.x		= m_iStepSize*(m_iPos-m_iMin) + SLIDER_Indent - m_gdiSlider.GetWidth()/2;
	m_ptSliderLoc.y		= m_gdiBackground.GetHeight()/2 - m_gdiSlider.GetWidth()/2;
}

void CWndSlider::CalcSliderValue()
{
	//lock to the nearest segment
	m_iPos				= (m_ptSliderLoc.x - SLIDER_Indent + m_iStepSize/2 + m_gdiSlider.GetWidth()/2)/m_iStepSize + m_iMin;
	if(m_hWnd)
		PostMessage(GetParent(m_hWnd), m_uiNotif, (WPARAM)m_iPos, 0);
}


void CWndSlider::CalcStepSize()
{
	if(!m_hWnd)
		return;

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	m_iNumSteps	= (m_iMax-m_iMin)/m_iStep;
	m_iStepSize	= (WIDTH(rcClient)-SLIDER_Indent*2)/m_iNumSteps;
}

void CWndSlider::AnimateStep(int iFromStep, int iToStep, BOOL bJustDraw)
{
	//range check ... we have to use the min and max values and convert em to steps
	//so I step has to be 0 or greater ... and less than the range divided by the step size
	if(iToStep < 0 || iToStep > m_iNumSteps)
		return;

//	if(iToStep < m_iMin || iToStep > m_iMax)
//		return;


/*	HDC dc = GetDC(m_hWnd);
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	POINT ptStart, ptEnd;
	
	ptStart.x	= m_ptSliderLoc.x;

	m_iPos = iToStep;
	

	ptEnd.x		= m_ptSliderLoc.x;
	ptStart.y	= ptEnd.y = m_ptSliderLoc.y;

	CIssGDIEffects::AnimateFromTo(dc, m_gdiBackground, m_gdiSlider, ptStart, ptEnd, 0,0,m_gdiSlider.GetWidth(), m_gdiSlider.GetHeight(), 100);

	ReleaseDC(m_hWnd, dc);*/

	// save the new position
	m_iPos = iToStep;
	PostMessage(GetParent(m_hWnd), m_uiNotif, (WPARAM)m_iPos, 0);
	CalcSliderLocation();
	InvalidateRect(m_hWnd, NULL, FALSE);
	UpdateWindow(m_hWnd);	

}