#pragma once
#include "issWndTrans.h"
#include "IssGDI.h"

class CWndSlider : public CIssWndTrans
{
public:
	CWndSlider(void);
	~CWndSlider(void);

	BOOL		Create(HWND hWndParent, HINSTANCE hInst);
	void		Init(UINT uiNotif);
	void		SetRange(int iMin, int iMax);
	void		SetStep(int iValue);
	void		SetPos(int iValue);
	int			GetPos(){return m_iPos;};
protected:
	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL		OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL		OnMouseMove(HWND hWnd, POINT& pt);
	BOOL		OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL		ProcDefault(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL		OnGetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam){InvalidateRect(m_hWnd, NULL, FALSE);return TRUE;};
	BOOL		OnLostFocus(HWND hWnd, WPARAM wParam, LPARAM lParam){InvalidateRect(m_hWnd, NULL, FALSE);return TRUE;};

private:
	void		DrawBackground(HDC hDC, RECT& rcClient);
	void		DrawButton(HDC hDC, RECT& rcClient);
	void		CalcStepSize();
	void		AnimateStep(int iFromStep, int iToStep, BOOL bJustDraw = FALSE);

	void		CalcSliderLocation();//based off of m_iPos
	void		CalcSliderValue();//set m_iPos based off of m_SliderLoc;
	
private:
	WNDPROC		m_lpfnStatic;
	CIssGDI		m_gdiBackground;
	CIssGDI		m_gdiSlider;
	CIssGDI		m_gdiMem;

	POINT		m_ptSliderLoc;	// the location in screen co-ordinates

	int			m_iMin;			// minimum value of slider (can be almost anything)
	int			m_iMax;			// max value of slider
	int			m_iStep;		// current "step" .. ie increment
	int			m_iPos;			// physical position - ie screen co-ordinates
	int			m_iStepSize;	// spacing in pixels between the divisions
	int			m_iNumSteps;	// the number of divisions
	UINT		m_uiNotif;		// notification message

	BOOL		m_bLockToStep;
	BOOL		m_bMouseDown;

};
