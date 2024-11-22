#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"


class CDlgBase:public CIssWnd
{
public:
	CDlgBase(void);
	~CDlgBase(void);

    virtual BOOL            Launch(HWND hWndParent, 
                                    HINSTANCE hInst,
                                    BOOL bAnimate);

protected:
    virtual BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL 			OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    virtual BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    virtual BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual BOOL    OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    virtual BOOL    OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL            OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};

	BOOL            DrawBackground(CIssGDIEx& gdiMem, RECT& rcClient, RECT& rcClip);
    virtual void    DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip){};
    virtual void    DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip){};
    virtual BOOL    OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip){return TRUE;};
    virtual void    OnMenuLeft(){};
    virtual void    OnMenuRight(){};

protected:
	CIssGDIEx			m_gdiBackground;
    RECT                m_rcArea;           // area where you can actually use
	RECT				m_rcTitle;
	RECT				m_rcBottomBar;
    BOOL                m_bAnimate;
   
};
