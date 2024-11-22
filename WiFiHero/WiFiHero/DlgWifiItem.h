#pragma once

#include "IssWnd.h"
#include "IssVector.h"
#include "IssGDIEx.h"
#include "IssKineticList.h"
#include "IssImageSliced.h"
#include "IssWndTouchMenu.h"

struct TypeWifi
{
    BOOL    bOnDevice;                  // is this wifi item on the device already
    BOOL    bAccessPoint;               // is this an access point
    TCHAR*  szName;                     // name of wifi profile
    CIssVector<TCHAR> arrParmNames;     // names of parameters
    CIssVector<TCHAR> arrParmValues;    // value of parameters

    TypeWifi();
    ~TypeWifi();
    void Clone(TypeWifi* sWifi);
};

class CDlgWifiItem:public CIssWnd
{
public:
	CDlgWifiItem(void);
	~CDlgWifiItem(void);
    void            Init(CIssGDIEx* gdiMem, 
                         CIssGDIEx* gdiBackground, 
                         CIssImageSliced* imgButton,
                         CIssImageSliced* imgButtonSel,
                         HFONT      hFontLabel,
                         TypeWifi*  sWifiItem);

protected:
    BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL 			OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL 			OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL 			OnMouseMove(HWND hWnd, POINT& pt);
	BOOL 			OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);	
	BOOL			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL            OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};

private:    // functions
    BOOL            DrawBackground(CIssGDIEx& gdiMem, RECT& rcClip);
    BOOL            DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, int* iIndex);
    static void     DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void     DeleteMenuItem(LPVOID lpItem); 
    BOOL            GetItemInfo(TCHAR* szName, TCHAR* szValue);
 

private:    // variables
    CIssKineticList m_oList;
    TypeWifi*        m_sWifiItem;
    CIssGDIEx*       m_gdiMem;
    CIssGDIEx*       m_gdiBack;

    CIssImageSliced*	m_imgButton;            // kinetic list button
    CIssImageSliced*	m_imgButtonSel;         // kinetic list selected button

    RECT            m_rcTitle;
    RECT            m_rcList;

    HFONT           m_hFontBtnLabel;

    BOOL            m_bFirstTime;
    TCHAR           m_szName[STRING_LARGE];
    TCHAR           m_szValue[STRING_LARGE];
};
