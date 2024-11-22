#pragma once
#include "wingdi.h"
#include "isswnd.h"
#include "IssGDIEx.h"
#include "IssDynDisplay.h"
#include "IssVector.h"
#include "IssKineticList.h"
#include "IssCommon.h"

#define BUTTON_Height                   (GetSystemMetrics(SM_CXICON)*2)
#define BUTTON_HeightSelected           (GetSystemMetrics(SM_CXICON)*3/2)

struct TypeEntry
{
    TCHAR	szEntryLabel[STRING_NORMAL];
    TCHAR	szEntryValue[STRING_NORMAL];
};

class CWndInfoEdit:public CIssWnd
{
public:
	CWndInfoEdit(void);
	~CWndInfoEdit(void);

	void		Init(HINSTANCE hInst){m_hInst = hInst;};
    
    BOOL        OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL        OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL        OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL        OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL        OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};

    BOOL        OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnMouseMove(HWND hWnd, POINT& pt);
    BOOL        OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

    void        Init(CIssGDIEx* gdiMem, 
                    CIssGDIEx* gdiBackground, 
                    CIssImageSliced* imgButton,
                    CIssImageSliced* imgButtonSel,
                    HFONT hFontLabel);

protected:
    BOOL            DrawBackground(CIssGDIEx& gdiMem, RECT& rcClip);
    BOOL            DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, int* iIndex);
    static void     DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    BOOL            GetItemInfo(TCHAR* szName, TCHAR* szValue);
    static void     DeleteMenuItem(LPVOID lpItem);


//    BOOL        DrawScreenEntries(HDC hDC);
	//edit control 
	LPCTSTR		StringFromResources(UINT uStringID);
	void		Clear();
	void		OutputString(LPCTSTR lpName, LPCTSTR lpValue);
	void        OutputSection(LPCTSTR lpSection);
    
    
    
    void		OutputResource(UINT uResourceID);
	void		OutputTitle(UINT uResourceID);
    void		OutputFormattedString(LPCTSTR tszFormat,...);

    void		OutputTime(const SYSTEMTIME* pcTime);


//	void		CreateEditControl();
	void		Destroy();

    void        DeleteArray();

	virtual void		OnMenuBack();
	virtual void		OnMenuRefresh();

	virtual void		Refresh(){;};

protected:
    CIssKineticList m_oList;
    HINSTANCE	m_hInst;

    TCHAR       m_szName[STRING_LARGE];

    CIssImageSliced*	m_imgButton;            // kinetic list button
    CIssImageSliced*	m_imgButtonSel;         // kinetic list selected button

    HFONT       m_hFontBtnLabel;
	HFONT		m_hFont;

	SIZE		m_sizeWindow;

    CIssVector<TypeEntry> m_arrEntries;

    CIssGDIEx*       m_gdiMem;
    CIssGDIEx*       m_gdiBack;

    CIssDynDisplay      m_oDisplay;

    HFONT               m_hFontDisplay;

    int					m_iEntryIndex; //Index for the value at the top of the screen
    int					m_iScreenIndex; //Index for the onscreen selected Value
    int                 m_iLinesOnscreen;
    int                 m_iTextHeight;

    int                 m_iCount;

    int                 m_iLeftTextEdge;//useful for the selector

    COLORREF            m_crLight;//if the text gets too long

    RECT                m_rcTitle;
    RECT                m_rcList;
};
