#pragma once
#include "isswnd.h"
#include "DlgGuitarPro.h"
#include "IssKineticList.h"
#include "ObjGui.h"

enum EnumOption
{
    OPTION_Skin,
    OPTION_FretMarkers,
    OPTION_Left,
    OPTION_PlaySounds,
    OPTION_TunerSampleFreq,
    OPTION_TunerTrigger,
    OPTION_Strum,
    OPTION_MetroSounds,
	OPTION_Instrument,
    OPTION_Language,
	OPTION_Count,
};

class CDlgOptions :	public CIssWnd
{
public:
	CDlgOptions(void);
	~CDlgOptions(void);

    void	Init(HINSTANCE hInst){m_hInst = hInst;};

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
    //this can stay ... it'll never actually get called unless they've mapped help to a hardware button
	BOOL	OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("PanoGuitarSuite.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};

    DWORD		GetOriginalLanguage(){return m_dwOrigLanguage;};

private:	// functions
    BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption);
    static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void         DeleteItem(LPVOID lpItem);
    
    void        GetSkinText(EnumSkin eSkin, TCHAR* szInfo);
	void		GetInstrumentText(EnumInstrument eInst, TCHAR* szInfo);
    void        GetItemTitle(EnumOption eOption, TCHAR* szTitle);
    void        PopulateOptions();
    void        LaunchPopupMenu();
    void        SetMenuSelection();
    void		OnMenuLeft();
    void		OnMenuRight();

    void        SaveSettings();
    void        CancelSettings();//for language

    BOOL        DrawMenuText(RECT& rcClip, CIssGDIEx& gdi);
    BOOL        DrawMenuHilite(BOOL bLeft);

    void        FadeInScreen();

public:


private:	// variables
    CIssKineticList     m_oMenu;

    HFONT				m_hFontBtnText;
    HFONT               m_hFontLabel;

    RECT                m_rcMenuLeft;
    RECT                m_rcMenuRight;

    BOOL                m_bTransition;

    DWORD               m_dwOrigLanguage;


    //settings/options
    //general
    EnumSkin            m_eSkin;
    EnumFretMarkers     m_eFretmarkers;
    BOOL                m_bLeftHanded;
    BOOL                m_bPlaySounds;
	EnumInstrument		m_eInst;

    //Tuner
    EnumSampleFreqID    m_eSampleFreq;
    int                 m_iTrigger; //trigger level ... between 0 & 32 inclusive

    //chords
    EnumStrum           m_eStrum;

    //metronome
    EnumMetroSounds     m_eMetSounds;

};
