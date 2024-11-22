#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssRect.h"
#include "IssCallManager.h"

class CDlgCheekGuard:public CIssWnd
{
public:
	CDlgCheekGuard(void);
	~CDlgCheekGuard(void);

	void	Init(CIssCallManager* oCall);
	BOOL	Connected();
	BOOL	Disconnected();
	BOOL	Show();
	BOOL	Hide();
	EnumCallState	GetCallState();
	void	EndCall();

protected:
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnMouseMove(HWND hWnd, POINT& pt);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL    OnIncomingCall();



private:			// functions
	void			SlideRight();
	void			SlideLeft();
	void			SweepRight();
	void			SweepLeft();
	void			FadeIn();
	void			Trapazoid();
	void			CubeHorzUp();
	void			LoadImages();
	void			DrawBackground(CIssGDIEx& gdi, RECT rcClient);
	void			DrawButtons(HDC hdc, RECT rcClient, BOOL bDrawButton = TRUE);
	void			DrawImage(HDC hdc, RECT rcClient);
	void			DrawText(HDC hdc, RECT rcClient);

	BOOL			ShowSlider();
	void			UnlockScreen();
	BOOL			UseCheekGuard();
	

	void			OnTalkButton();
	void			OnEndButton();
	void			StartCallTimer();
	void			EndCallTimer();


private:	// variables
	CIssCallManager* m_oCall;

	CIssGDIEx		m_gdiMem;
	CIssGDIEx		m_gdiArrow;
	CIssGDIEx		m_gdiArrowBackground;
	CIssGDIEx		m_gdiTemp;
	CIssGDIEx		m_gdi1;
	CIssGDIEx		m_gdi2;

	//CIssGDI*		m_gdiPicture;
	CIssGDIEx		m_gdiBackground;

	CIssGDIEx		m_gdiGreen;
	CIssGDIEx		m_gdiRed;

	HFONT			m_hFontText;

	RECT			m_rcArrow;
	RECT			m_rcAnswer;
	RECT			m_rcEnd;

	RECT			m_rcArrowBackground;
	RECT			m_rcName;
	RECT			m_rcPhoneNum;
	RECT			m_rcLocation;
	RECT			m_rcTime;
	RECT			m_rcCallStatus;
	RECT			m_rcDurration;
	RECT			m_rcPicture;
	
	POINT			m_ptOffset;
	
	BOOL			m_bFirstPic;
	BOOL			m_bMovingArrow;

	int				m_iCallDurration;
	int				m_iRingCount;
	
	//EnumCallState	m_eState;
	EnumCallState	m_eLastState;
	BOOL			m_bScreenLocked;


	



};
