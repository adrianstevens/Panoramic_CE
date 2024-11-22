#pragma once
#include "isswnd.h"
#include "DlgFreeCell.h"
#include "IssKineticList.h"

enum EnumGameMode
{
	GM_Default,
	GM_Timed,
};

enum EnumOption
{
    OPTION_Skin,
    OPTION_PlaySounds,
    OPTION_AutoComplete,
    OPTION_Strict,
    OPTION_ShowTimer,
    OPTION_GameType,
    OPTION_Inverse,
    OPTION_FreeCellCount,
    OPTION_NoDragging, //disable card dragging ...
	OPTION_Language,
};

class CDlgOptions :	public CIssWnd
{
public:
	CDlgOptions(void);
	~CDlgOptions(void);

    void	Init(HINSTANCE hInst){m_hInst = hInst;};
    void    SetGame(CIssFreeCell* oGame){m_oGame = oGame;};

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
	//BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("PanoFreecell.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};


private:	// functions
    BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption);
    static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void         DeleteItem(LPVOID lpItem);
    
    void        GetSkinText(EnumBackgroundColor eBGC, TCHAR* szInfo);
    void        GetAutoCompleteText(EnumAutoPlay eAP, TCHAR* szInfo);
    void        GetGameTypeText(EnumFreecellType eType, TCHAR* szInfo);
    void        GetFreeCellCountText(EnumFreeCellCount eCount, TCHAR* szInfo);
    void        GetItemTitle(EnumOption eOption, TCHAR* szTitle);
    void        PopulateOptions();
    void        LaunchPopupMenu();
    void        SetMenuSelection();
    void		OnMenuLeft();
    void		OnMenuRight();

    void        SaveSettings();

    void        OnOk();
    void        OnCancel();

public:
    BOOL            m_bInverse;
    BOOL            m_bStrict;
    BOOL            m_bShowTimer;
	BOOL			m_bPlaySounds;
	BOOL			m_bShowAni;
	EnumAutoPlay	m_eAutoPlay;
	EnumBackgroundColor m_eBGColor;
    EnumBackgroundColor m_eBGColorOrig;//so we can put it back on cancel
    EnumFreecellType m_eFCType;
    EnumFreeCellCount m_eFCCount;

private:	// variables
    CIssKineticList     m_oMenu;

    CIssFreeCell*       m_oGame;
    	

    HFONT				m_hFontBtnText;
    HFONT               m_hFontLabel;
};
