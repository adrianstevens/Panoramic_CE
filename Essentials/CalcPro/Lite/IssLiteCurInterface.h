#pragma once

#include "CalcInterace.h"
#include "IssFormatCalcDisplay.h"
#include "IssCurrency.h"
#include "IssInternet.h"
#include "IssConnection.h"


#define NUM_MAIN_BUTTONS 20 // one extra for landscape (3x7)

enum EnumCurButtons
{
	CBTN_From,
	CBTN_To,
	CBTN_Count,
};

enum EnumCurDisplays
{
	CDIS_From,
	CDIS_To,
};

enum EnumCurrencyBidAsk
{
    CBA_Both,
    CBA_Ask,
    CBA_Bid,
};


class CIssCurInterface : public CCalcInterace
{
public:
	CIssCurInterface(void);
	~CIssCurInterface(void);

	void		Init(TypeCalcInit* sInit);

	BOOL		Draw(HDC hdc, RECT& rc);

	BOOL		OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL		OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);// for our dropdown menus
    BOOL        OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);//definitely need this action

    BOOL        OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL        OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);

	BOOL        DrawSelector(HDC hdc, RECT& rc);
    BOOL        HandleDPad(int iVKKey);


private:
	void		CreateButtons();//we'll pump these on Draw to keep loading times fast
	void		CreateDisplays();

	BOOL		DrawDisplays(HDC hdc, RECT& rcClip);
	BOOL		DrawCurBtns(HDC hdc, RECT& rcClip);
	BOOL		DrawMainBtns(HDC hdc, RECT& rcClip);

	BOOL		DrawDisplayText(HDC hdc, RECT& rcClip);

	BOOL		OnBtnFrom();
	BOOL		OnBtnTo();

    void        LoadRegistry();
    void        SaveRegistry();
    BOOL        EstablishConnection();

private:
    CIssFormatCalcDisplay* m_oCalcDis;

    CIssCurrency        m_oCur;

	CCalcDynBtn	        m_oCurBtns[(int)CBTN_Count];
	CCalcDynBtn			m_oMainBtns[NUM_MAIN_BUTTONS];
	CIssDynDisplay		m_oDisplay[2];

    EnumCurrencyBidAsk  m_eBidAsk;

	HFONT               m_hFontBtnText;
	HFONT               m_hFontBtnTextSm;
	HFONT				m_hFontDisplay;
	HFONT				m_hFontExp;
	HFONT				m_hFontTop;

	RECT				m_rcDisplay;
	RECT				m_rcMainBtns;

    int                 m_iDisplayChars;
    int                 m_iSelector;

    CIssConnection      m_oConnection;
    CIssInternet        m_oInternet;
};
