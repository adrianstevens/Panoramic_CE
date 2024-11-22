#pragma once

#include "CalcInterace.h"
#include "CalcProSkins.h"
#include "IssVector.h"
#include "HandleButtons.h"
#include "ObjConstants.h"
#include "IssGDIEx.h"

#include "IssCurInterface.h"
#include "IssHardwareKeys.h"

#include "IssWndTouchMenu.h"

#include "CalcMenu.h"

struct TypeButtonBuffer
{
	byte		btWidth;
	byte		btHeight;
	byte		btColorIndex;
	CCalcDynBtn* oButton; //buffer to return
};

struct TypeMenu
{
    TCHAR*  szMenu;
    UINT    uiMenu;
    TypeMenu();
    ~TypeMenu();
};

class CIssCalcInterface : public CCalcInterace
{
public:
	CIssCalcInterface(HINSTANCE hInst);
	~CIssCalcInterface(void);

	BOOL		Draw(HDC hdc, RECT& rc);
	BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam)				{return FALSE;};
	BOOL		OnLButtonUp(HWND hWnd, POINT& pt)									;
	BOOL		OnLButtonDown(HWND hWnd, POINT& pt)									;
	BOOL		OnMouseMove(HWND hWnd, POINT& pt)									;
	BOOL		OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)					{return FALSE;};
	BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL		OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL		OnBackKey();

	BOOL		OnMenuLeft();
	BOOL		OnMenuRight();

	BOOL		OnMenuOptions();

    EnumDeviceType	GetDeviceType(){return m_oBtnHandler.GetDeviceType();};
    
    
private:
    BOOL        OnMenuContext(POINT pt);

	BOOL        InitMainButtons();
	HRESULT	    InitAltButtons();
	BOOL	    InitDisplay();
    BOOL        InitMenu();

	void		ChangeCalcModes(EnumCalcType eCalcType);
    void        UpdateCalcMode(CalcOperationType eMode);

	void		DrawBackground(HDC hDC, RECT& rc);
	void		DrawButtons(HDC hDC, RECT& rc);
	void		DrawSelector(HDC hDC, RECT &rc);
	void		DrawDisplay(HDC hDC, RECT& rc);
	void		DrawDisplayText(HDC hDC, RECT& rc);

	void		DrawDisplayTextExpression(HDC hDC, RECT& rc);
	void		DrawDisplayTextFraction(HDC hDC, RECT& rc);
	void		DrawDisplayTextFracExp(HDC hDC, RECT& rc);//possibly something more dynamic is in order soon ...
	
	CCalcDynBtn*	GetButton(int iWidth, int iHeight, int iButtonStyle);//returns NULL if it can't find it
	BOOL		SaveButton(CCalcDynBtn* oBtn, int iWidth, int iHeight, int iButtonStyle);

	void		DeleteButtonVector();

	void		LoadRegistry();
	void		SaveRegistry();

	//destroys the old, sets the colors, checks for a previous GDI, creates the button and saves the GDI if nesessary
	BOOL		InitButton(CCalcDynBtn* oDynButton, RECT* rc, CalcProSkins* oSkin, int iSkinIndex, int iWidth, int iHeight);//helper function ... keeps things tidy

	void		DeleteDynBtnVector(CIssVector<CCalcDynBtn>* arr);

	void		Destroy();

	BOOL		HandleDPad(int iVKKey);//up, down, left, right, return

	void		DetectAndSetDeviceType();

	void		PlaySounds();

	BOOL		HandleDisplayTaps(POINT& pt);

	void		SetDisplayRects();

	void		ChangeCalcState();
    void        NotifyLostFocus();

    void        UpdateWindowText();

    BOOL        OnLastCalc();

    BOOL        NextCalc(BOOL bForward);
    
    void        OnViewTape();
    void        OnMenuConstants();
    void        OnMenuConstantSelect(int iIndex);

    static void DrawIconItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    void        DrawIconItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);
    static void DrawConstantsItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    void        DrawConstantsItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted);

    static void DeleteConstantsItem(LPVOID lpItem);
    static void DeleteMenuItem(LPVOID lpItem);

//    BOOL        GoTo

	
private: 
    CIssGDIEx       m_gdiIcons;         // icons used in the "menu" menu
    CIssGDIEx       m_gdiVertBar;       // vertical bar in the icons menu
	CCalcMenu       m_oMenu;
    CHandleButtons	m_oBtnHandler;
	CIssCalculator*	m_oCalc;
	CCalcBtnManager	m_oBtnManger;

    CIssCurInterface m_oCurInt;

    CObjConstants   m_oConstants;
    
	CIssDynDisplay	m_oDisplay;
    

	CIssVector<TypeButtonBuffer>	m_arrButtons;//this saves pointers so we don't recreate the same GDIs over and over ... cleared by DeleteButtonVector()
	CIssVector<CCalcDynBtn>			m_arrMainButtons;
	CIssVector<CCalcDynBtn>			m_arrAltButtons;
	CIssVector<CCalcDynBtn>			m_arr2ndFButtons;
	CIssVector<CCalcDynBtn>			m_arrHypFButtons;
	CIssVector<CCalcDynBtn>			m_arr2ndHypFButtons;

    CIssWndTouchMenu    m_wndMenu;
	CalcStateType	    m_eState;
    CalcOperationType   m_eCalcMode;


	HFONT		m_hFontMainBtnText;
	HFONT		m_hFontMainLongText;
    
	HFONT		m_hFontAltBtnText;
	HFONT		m_hFontAltLongText;
    HFONT       m_hFontAltXLText;
	HFONT		m_hFontDisplay;
	HFONT		m_hFontExponent;
	HFONT		m_hFontDisplayTop;
	HFONT		m_hFontSmallTop;
    HFONT       m_hFontMenu;
  
	RECT		m_rcSize;

	RECT		m_rcDisplayArea;
	RECT		m_rcAltButtons;
	RECT		m_rcMainButtons;

	//a bunch of rects for the display rects
	RECT		m_rc2ndF;
	RECT		m_rcHyp;
	RECT		m_rcBase;
	RECT		m_rcDRG; //also used for base bits
	RECT		m_rcM1;	//we'll need more of these
	RECT		m_rcDisplay; //float, frac, eng, etc
	
	int			m_iYAltStart;
	int			m_iYMainStart;
    int         m_iMemoryIndex;

	//Some static buffers 
	TCHAR		m_szValue[STRING_LARGE];
	TCHAR		m_szResult[STRING_LARGE];
	TCHAR		m_szExp[STRING_SMALL];

	BOOL		m_bUseRegionalSettings;
	BOOL		m_bPlaySounds;
	BOOL		m_bShowSelector; //for smart phone

	BOOL		m_bBackPressed;			// hacking the back button ...
	BOOL		m_bHoldBack;

	BOOL		m_bAnimate2ndHyp;
};
