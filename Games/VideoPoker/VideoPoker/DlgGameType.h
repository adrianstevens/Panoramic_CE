#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "IssKineticList.h"
#include "stdafx.h"
#include "IssPokerHandEval.h"//game type enum

class CDlgGameType : public CIssWnd
{
public:
    CDlgGameType(void);
    ~CDlgGameType(void);

    void            Init(BOOL bGameInProgress, EnumPokerType eGame);

    BOOL	        OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL	        OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	        OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	        OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	        OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	        OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
    BOOL	        OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	        OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	        OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL	        OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL            OnMouseMove(HWND hWnd, POINT& pt);
    BOOL	        OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	        OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);



private:
    void            LoadImages();

    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void     DeleteListItem(LPVOID lpItem);

    void            PopulateList();

private:	// variables
    static CIssKineticList* m_oMenu;

    CIssGDIEx       m_gdiGames[6];
    CIssGDIEx		m_gdiSeparator;
    CIssImageSliced m_imgSelector;

    static DWORD    m_dwFullListCount;

    static BOOL     m_bPreventDeleteList;//not really needed

    HFONT			m_hFontText;

    EnumPokerType   m_eGameType;
    BOOL            m_bGameInProgress;
};
