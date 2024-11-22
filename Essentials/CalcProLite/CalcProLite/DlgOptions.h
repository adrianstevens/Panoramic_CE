#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "IssCalculator.h"
#include "CalcBtnManager.h"
#include "IssWndTouchMenu.h"
#include "IssKineticList.h"

struct TypeOptions
{
	BOOL                bUseRegionalSettings;
	BOOL                bPlaySounds;
	BOOL                bRecordTape;
    BOOL                bAdvancedMemory;
    CalcOperationType   eCalculatorMode;
    DisplayType         eNotation;
    int                 iConversionBuffer;      // setBasebits
    int                 iRPNStackSize;
	EnumCalcProSkins    eSkin;
    int                 iFixedDigits;           // SetFixedDecDigits
    BOOL                bGraphFast;         
    BOOL                bGraphUsePoints;
    BOOL                bTVMEndPeriod;
     
};

enum EnumOption
{
    OPTION_UseRegionalSettings,
    OPTION_PlaySounds,
    OPTION_RecordTape,
    OPTION_AdvancedMemory,
    OPTION_CalculatorMode,
    OPTION_Notation,
    OPTION_ConversionBuffer,
    OPTION_RPNStackSize,
    OPTION_Skin,
    OPTION_FixedDigits,
    OPTION_GraphAccuracy,
    OPTION_GraphUsePoints,
    OPTION_EndPeriodPayments,
};

class CDlgOptions : public CIssWnd
{
public:
	CDlgOptions(void);
	~CDlgOptions(void);

	BOOL		Init(TypeOptions& sOptions, CIssWndTouchMenu* wndMenu);
    TypeOptions& GetOptions(){return m_sOptions;};

	BOOL		OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
//	BOOL		OnHelp(HWND hWnd, WPARAM wParam, LPARAM lParam){CreateProcess(_T("peghelp"), _T("iSSNotepad.htm#HowTo"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);return TRUE;};
    BOOL        OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL		OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL		OnLButtonUp(HWND hWnd, POINT& pt);
    BOOL		OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL		OnMouseMove(HWND hWnd, POINT& pt);
    BOOL		OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};

private:
    void        GetFixedDigitsText(int iFixedDigits, TCHAR* szInfo);
    void        GetSkinsText(EnumCalcProSkins eSkin, TCHAR* szInfo);
    void        GetRPNStackSizeText(int iRPNStackSize, TCHAR* szInfo);
    void        GetConversionBufferText(int iConversionBuffer, TCHAR* szInfo);
    void        GetNotationText(DisplayType eNotation, TCHAR* szInfo);
    void        GetCalcModeText(CalcOperationType eCalculatorMode, TCHAR* szInfo);
    void        GetSoundModeText(BOOL bPlaySounds, TCHAR* szInfo);
    void        GetItemTitle(EnumOption eOption, TCHAR* szTitle);
    void        PopulateOptions();
    void        LaunchPopupMenu();
    void        SetMenuSelection();
	void		OnMenuLeft();
	void		OnMenuRight();
    BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption);
    static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);

    static void         DeleteItem(LPVOID lpItem);


private:
    CIssGDIEx       m_gdiMem;
    CIssKineticList m_oMenu;
    CIssWndTouchMenu* m_wndMenu;
    TypeOptions     m_sOptions;

    HFONT				m_hFontBtnText;
    HFONT               m_hFontLabel;

};
