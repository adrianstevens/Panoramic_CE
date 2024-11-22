#pragma once
#include "isswnd.h"
#include "DlgVideoPoker.h"
#include "IssKineticList.h"

enum EnumOption
{
    OPTION_Skin,
    OPTION_PlaySounds,
    OPTION_BetAmount,
    OPTION_Speed,
    OPTION_AutoHold,
    OPTION_Multihand,
};

class CDlgOptions :	public CIssWnd
{
public:
	CDlgOptions(void);
	~CDlgOptions(void);

    void	Init(HINSTANCE hInst, 
                 BOOL bPlaysounds, 
                 BOOL bAutoHold,
                 BOOL bQuickPlay,
                 EnumBackgroundColor eBG, 
                 EnumBetAmounts eBets, 
                 EnumMultiPlay eMulti);

	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnMouseMove(HWND hWnd, POINT& pt);
	BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL    OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};
	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("PanoVideoPoker.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};


private:	// functions
    BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption);
    static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void         DeleteItem(LPVOID lpItem);
    
    void        GetSkinText(EnumBackgroundColor eBGC, TCHAR* szInfo);
    void        GetBetText(EnumBetAmounts eBet, TCHAR* szInfo);
    void        GetMultiText(EnumMultiPlay eMulti, TCHAR* szInfo);
    void        GetItemTitle(EnumOption eOption, TCHAR* szTitle);
    void        PopulateOptions();
    void        LaunchPopupMenu();
    void        SetMenuSelection();
    void		OnMenuLeft();
    void		OnMenuRight();

    void        SaveSettings();

public:
    BOOL            m_bQuickPlay;
    BOOL            m_bAutoHold;
    EnumMultiPlay   m_eMultiplay;
	BOOL			m_bPlaySounds;
	EnumBackgroundColor m_eBGColor;
    EnumBetAmounts  m_eBet;
    
    
	

private:	// variables
    CIssKineticList     m_oMenu;

    HFONT				m_hFontBtnText;
    HFONT               m_hFontLabel;
};
