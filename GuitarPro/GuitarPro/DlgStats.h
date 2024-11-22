#pragma once
#include "isswnd.h"
#include "ObjGui.h"

class CDlgStats :
    public CIssWnd
{
public:
    CDlgStats(void);
    ~CDlgStats(void);
	BOOL				OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL				OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL                OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL                OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL                OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				DrawText(HDC hDC);

    void                SetText(TCHAR* szText){m_oStr->StringCopy(m_szStats, szText);};
    
private:
    CIssString*			m_oStr;

    TCHAR               m_szStats[STRING_MAX*2];
};
