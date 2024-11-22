#pragma once

#include "stdafx.h"
#include "Resource.h"
#include "IssCalculator.h"
#include "HandleButtons.h"
#include "CalcBtnManager.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "IssDynDisplay.h"
#include "IssWndTouchMenu.h"
#include "ObjConstants.h"
#include "CalcMenu.h"
#include "IssDebug.h"

struct TypeCalcInit
{
    CIssCalculator*     oCalc;          // calculator engine
    CHandleButtons*     oBtnHand;       // button handler
    CCalcBtnManager*    oBtnMan;        // Button Manager
    CIssWndTouchMenu*   wndMenu;        // popup menu
    CCalcMenu*          oMenu;
    CObjConstants*      oConstants;     // constants class
    HWND hWnd;
    HINSTANCE hInst;
};

#define UNHANDLED -1
class CCalcInterace
{
public:
	CCalcInterace(void);
	virtual ~CCalcInterace(void);

//	virtual BOOL	Draw(HDC hdc, RECT& rc)												{return UNHANDLED;};
    virtual BOOL	Draw(CIssGDIEx& gdiMem, RECT& rc)									{return UNHANDLED;};
	virtual BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)						{return UNHANDLED;};
	virtual BOOL	OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)						{return UNHANDLED;};
	virtual BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam)				{return UNHANDLED;};
	virtual BOOL	OnLButtonUp(HWND hWnd, POINT& pt)									{return UNHANDLED;};
	virtual BOOL	OnLButtonDown(HWND hWnd, POINT& pt)									{return UNHANDLED;};
	virtual BOOL	OnMouseMove(HWND hWnd, POINT& pt)									{return UNHANDLED;};
	virtual BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual void	OnInit(HWND hWnd, HINSTANCE hInst){m_hInst = hInst; m_hWndDlg = hWnd;};
	virtual BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)		{return UNHANDLED;};
	virtual BOOL	OnMenuLeft()														{return	FALSE;};
	virtual BOOL	OnMenuRight()														{return FALSE;};
	virtual BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
	virtual BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};
    virtual BOOL    OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return UNHANDLED;};

	virtual BOOL	OnBackKey()															{return FALSE;};

    virtual void    Init(TypeCalcInit* sInit);
    virtual void    LostFocus(){}; //clean up .. hiding controls, etc

    virtual BOOL    HandleDPad(int iVKKey){return FALSE;};

    virtual void    SetExpired(BOOL bExpired){m_bExpired = bExpired;};

    virtual BOOL    GetFullscreen()                                                     {return FALSE;};

protected:

	int				GetSystemMetrics(__in int nIndex);


protected:
    CIssString*         m_oStr;
    CIssCalculator*		m_oCalc;
    CHandleButtons*		m_oBtnHand;     // makes sure we don't create more btns than we need
    CCalcBtnManager*	m_oBtnMan;      // reads the config files
    CIssWndTouchMenu*   m_wndMenu;      // Popup menu
    CCalcMenu*          m_oMenu;        // bottom menu
    CObjConstants*      m_oConstants;   // constants class

    BOOL                m_bExpired;

	HINSTANCE		    m_hInst;
	HWND			    m_hWndDlg;	    // passed in from the dlg

    int                 m_iSelector;
};
