#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "IssCalculator.h"
#include "CalcBtnManager.h"
#include "IssWndTouchMenu.h"
#include "IssKineticList.h"
#include "IssMortgage.h"
#include "CalcMenu.h"
#include "globals.h"

#define IDMENU_Select           5000
#define IDMENU_SelectMenu       5001

#define BUTTON_Height           (GetSystemMetrics(SM_CXICON)*3/4)
#define HEIGHT_Text             (GetSystemMetrics(SM_CXICON)*4/9)
#define INDENT                  (GetSystemMetrics(SM_CXSMICON)/4)

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
    PAYMENT_SCHEDULE    ePaymentSched;          //mortgage calc
	EnumCalcSizes		eSize;					//screen size, pc only
    EnumKeyboard        eKB;
    int                 iKBOffset;
    BOOL                bKBLetters;
    BOOL                bFullScreen;
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
    OPTION_EndPeriodPayments, //TVM
    OPTION_PaymentSchedule, // Mortgage
	OPTION_Size,			//size on PC
    OPTION_Keyboard,
    OPTION_KeyboardOffset, //gotta figure out how to label this bad boy
    OPTION_KeyboardLetters, //do we draw the letters on the keyboard?
    OPTION_Language,
    OPTION_FullScreen,
};

class CDlgOptions : public CIssWnd
{
public:
	CDlgOptions(void);
	~CDlgOptions(void);

	BOOL		Init(TypeOptions& sOptions, CIssWndTouchMenu* wndMenu, CCalcMenu* oMenu, HWND hWnd);
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
	BOOL		OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL		OnMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL        OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);


	DWORD		GetOriginalLanguage(){return m_dwOrigLanguage;};
	DWORD		GetReturn(){return m_dwReturn;};

protected:
    void        GetPaymentSchedText(PAYMENT_SCHEDULE eSched, TCHAR* szInfo);
    void        GetFixedDigitsText(int iFixedDigits, TCHAR* szInfo);
    void        GetSkinsText(EnumCalcProSkins eSkin, TCHAR* szInfo);
    void        GetRPNStackSizeText(int iRPNStackSize, TCHAR* szInfo);
    void        GetConversionBufferText(int iConversionBuffer, TCHAR* szInfo);
    void        GetNotationText(DisplayType eNotation, TCHAR* szInfo);
    void        GetCalcModeText(CalcOperationType eCalculatorMode, TCHAR* szInfo);
    void        GetSoundModeText(BOOL bPlaySounds, TCHAR* szInfo);
	void		GetSizeText(EnumCalcSizes eSize, TCHAR* szInfo);
    void        GetKeyboardText(EnumKeyboard eKB, TCHAR* szInfo);
    void        GetKeyboardOffsetText(int iOffset, TCHAR* szInfo);
    void        GetItemTitle(EnumOption eOption, TCHAR* szTitle);
    virtual void        PopulateOptions();
    void        LaunchPopupMenu();
    void        SetMenuSelection();
	void		OnMenuLeft();
	void		OnMenuRight();
    BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption);
    static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);

    static void         DeleteItem(LPVOID lpItem);

    int			GetSystemMetrics(__in int nIndex);
	BOOL		IsVGA();
    BOOL        IsSPLand();



protected:
    CIssGDIEx       m_gdiMem;
    CIssKineticList m_oMenu;
    CIssWndTouchMenu* m_wndMenu;
    TypeOptions     m_sOptions;
	CCalcMenu*		m_oBottomMenu;
    
	HWND			m_hWndParent;

    HFONT			m_hFontBtnText;
    HFONT           m_hFontLabel;

	DWORD			m_dwReturn;
	DWORD           m_dwOrigLanguage;

};
