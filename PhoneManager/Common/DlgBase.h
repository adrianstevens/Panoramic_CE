#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "GuiBackground.h"

class CDlgBase:public CIssWnd
{
public:
	CDlgBase(void);
	~CDlgBase(void);

    BOOL            Init(CIssGDIEx* gdiMem, 
                        CGuiBackground* guiBackground,
                        BOOL bDrawTopBar = TRUE,
                        BOOL bDrawBottomBar = TRUE);
    virtual BOOL            Launch(HWND hWndParent, 
                                    HINSTANCE hInst,
                                    BOOL bAnimate);

    //set this before showing the window if you don't want it full screen
    void            SetFullScreen(BOOL bFull){m_bFullScreen = bFull;};
protected:
    virtual BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL 			OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    virtual BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    virtual BOOL	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual BOOL    OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    virtual BOOL    OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL            OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};

    virtual void    DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip){};
    virtual void    DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip){};
    virtual BOOL    OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip){return TRUE;};
    virtual void    OnMenuLeft(){};
    virtual void    OnMenuRight(){};

protected:
    CGuiBackground*     m_guiBackground;
    CIssGDIEx*          m_gdiMem;
    HFONT               m_hFontLabel;

    RECT                m_rcArea;           // area where you can actually use
    RECT                m_rcTitle;          // top title area
    RECT                m_rcBottomBar;      // bottom bar area

    BOOL                m_bDrawTopBar;
    BOOL                m_bDrawBottomBar;
    BOOL                m_bAnimate;

    BOOL                m_bFullScreen;
   
};
