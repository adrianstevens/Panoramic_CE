#pragma once

#include "GuiBase.h"
#include "IssGDIEx.h"
#include "ContactsGuiDefines.h"
#include "ObjGui.h"

#define NUM_TOOLBAR_BTNS	4

enum EnumMouseState
{
	MS_HoldAndDrag = 0,
	MS_SelectAndAni,
	MS_None,
};


class CGuiToolBar:public CGuiBase
{
public:
	CGuiToolBar();
	~CGuiToolBar(void);

	BOOL 	            Init(HWND hWndParent, HINSTANCE hInst);
    void                ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance);
	BOOL				Draw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	BOOL				MoveGui(RECT rcLocation);
	RECT				GetSelectorLocation(EnumCurrentGui eGui);

	BOOL				OnLButtonDown(POINT pt);
	BOOL				OnLButtonUp(POINT pt);
	BOOL 			    OnMouseMove(POINT& pt);
	BOOL				OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

    void                SetFocus(BOOL bHasFocus);
    virtual BOOL        HasFocus() { return m_bHasFocus; }
	virtual void	    LoseFocus();
	CIssGDIEx&			GetSelectorGDI(){return m_gdiImgSelector;};
	void				ResetSelector();

private:
	
	void				InitSelector();

private:	// variables
    DWORD               m_dwLockTicks;
	EnumMouseState		m_eMouseState;

	BOOL				m_bHasFocus;
	
	POINT               m_ptLastMousePos;      // for drawing drag button
    POINT               m_ptLButtonDown;      
    int                 m_iDragBtnOffset;
	int					m_iMaxMove;

	RECT				m_rcSelector;

    // images
    CIssGDIEx           m_gdiImgSelector;

};
