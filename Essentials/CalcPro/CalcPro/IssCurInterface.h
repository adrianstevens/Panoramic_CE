#pragma once

#include "CalcInterace.h"
#include "IssFormatCalcDisplay.h"
#include "IssCurrency.h"

#include "IssInternet.h"

#ifdef UNDER_CE
#include "IssConnection.h"
#endif

#define NUM_MAIN_BUTTONS 20 // one extra for landscape (3x7)
#define NUM_KB_BUTTONS 27 

enum EnumCurButtons
{
	CBTN_From,
	CBTN_To,
	CBTN_Update,
	CBTN_Count,
};

enum EnumCurDisplays
{
	CDIS_From,
	CDIS_To,
	CDIS_Update,
};

enum EnumCurrencyBidAsk
{
    CBA_Both,
    CBA_Ask,
    CBA_Bid,
    CBA_None,
};

enum EnumCurDisplayState
{
    CDISPLAY_Downloading,
    CDISPLAY_Error,
    CDISPLAY_LastUpdated,

};

class CIssCurInterface : public CCalcInterace
{
public:
	CIssCurInterface(void);
	~CIssCurInterface(void);

	void		Init(TypeCalcInit* sInit);

	BOOL		Draw(CIssGDIEx& gdiMem, RECT& rc);

	BOOL		OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL		OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);// for our dropdown menus
    BOOL        OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);//definitely need this action

    BOOL        OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL        OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL        DownloadCurrency();
	BOOL        DrawSelector(HDC hdc, RECT& rc);
    BOOL        HandleDPad(int iVKKey);

    BOOL		OnBtnUpdate();

private:
	void		CreateButtons();//we'll pump these on Draw to keep loading times fast

    void        CreateKBButtons();//separate it out  ....
	void		CreateDisplays();

	BOOL		DrawDisplays(HDC hdc, RECT& rcClip);
	BOOL		DrawCurBtns(HDC hdc, RECT& rcClip);
	BOOL		DrawMainBtns(CIssGDIEx& gdiMem, RECT& rcClip);
	BOOL        DrawKeyboardBtns(CIssGDIEx& gdiMem, RECT& rcClip);

	BOOL		DrawDisplayText(HDC hdc, RECT& rcClip);

	BOOL		OnBtnFrom();
	BOOL		OnBtnTo();
	
    void        LoadRegistry();
    void        SaveRegistry();
    BOOL        EstablishConnection();

    BOOL        IsLandscape();
	BOOL		ShowKBButtons();

    
private:
    CIssFormatCalcDisplay* m_oCalcDis;

    CIssCurrency        m_oCur;

	CCalcDynBtn	        m_oCurBtns[(int)CBTN_Count];
	CCalcDynBtn			m_oMainBtns[NUM_KB_BUTTONS];
	CIssDynDisplay		m_oDisplay[3];

    EnumCurDisplayState	m_eCDisplayState;     
    EnumCurrencyBidAsk  m_eBidAsk;

	HFONT               m_hFontBtnText;
	HFONT               m_hFontBtnTextSm;
    HFONT               m_hFontBtnTextLong;
	HFONT				m_hFontDisplay;
	HFONT				m_hFontExp;
	HFONT				m_hFontTop;

	RECT				m_rcDisplay;
	RECT				m_rcMainBtns;

    int                 m_iDisplayChars;
    int                 m_iSelector;

    BOOL                m_bUpdating;

    CIssInternet        m_oInternet;

#ifdef UNDER_CE
    
    CIssConnection      m_oConnection;
#endif
};
