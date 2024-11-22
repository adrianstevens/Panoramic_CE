#pragma once

#include "IssString.h"
#include "IssGDIEx.h"
#include "DlgOptions.h"
//#include "GuiBackground.h"

enum EnumKeyType
{
    KEY_Up = 0,
    KEY_Down,
    KEY_Left,
    KEY_Right,
    KEY_TakeFocus,
    KEY_LoseFocus
};

#ifdef DEBUG
//#define DEBUG_CONTACTS
#endif

#if defined(DEBUG) && defined(DEBUG_CONTACTS)
#define DBG_CONTACTS(exp, b) DBG_PROFILE(exp, b)
#else
#define DBG_CONTACTS(exp, b) (void)0
#endif

class CGuiBase
{
public:
	CGuiBase(void);
	~CGuiBase(void);

	virtual BOOL 	Init(HWND hWndParent, HINSTANCE hInst);
	virtual BOOL 	MoveGui(RECT rcLocation);
	virtual BOOL 	Draw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip){return FALSE;};
	virtual BOOL 	OnLButtonDown(POINT pt){return FALSE;};
	virtual BOOL 	OnLButtonUp(POINT pt){return FALSE;};
	virtual BOOL 	OnMouseMove(POINT pt){return FALSE;};
	virtual BOOL 	OnTimer(WPARAM wParam, LPARAM lParam){return FALSE;};
	virtual BOOL 	OnChar(WPARAM wParam, LPARAM lParam){return FALSE;};
	virtual BOOL 	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam){return FALSE;};
	virtual BOOL 	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam){return FALSE;};
	virtual BOOL 	OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam) { return FALSE;};
	virtual BOOL 	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam){ return FALSE;};

	virtual BOOL 	OnCommand(WPARAM wParam, LPARAM lParam){return FALSE;};
	virtual BOOL 	AddMenuItems(){return FALSE;};
    virtual UINT    GetVKey(EnumKeyType eWhich);
    virtual BOOL    IsLandscape();
    virtual BOOL    HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam) { return FALSE; }
    virtual void    Show()  { SetAllDirty();  }

	RECT			GetFrame()	{ return m_rcLocation; }
    void            SetAllDirty(){m_rcDirty = m_rcLocation;}
    void            SetDirty(RECT& rc);

	virtual	BOOL	HasFocus()	{return FALSE;}
	virtual void	LoseFocus() {}

protected:	// variables
	CIssString*	    m_oStr;
  	HWND		    m_hWndParent;
	HINSTANCE	    m_hInst;
	RECT		    m_rcLocation;
	BOOL		    m_bFadeIn;	
    RECT            m_rcDirty;

};
