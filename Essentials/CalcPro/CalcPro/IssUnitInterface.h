#pragma once

#include "CalcInterace.h"
#include "IssUnitConverter.h"
#include "IssFormatCalcDisplay.h"

#define NUM_MAIN_BUTTONS 20 // one extra for landscape (3x7)
#define NUM_KB_BUTTONS 27

enum EnumUnitButtons
{
	UBTN_From,
	UBTN_To,
	UBTN_Category,
	UBTN_Count,
};

enum EnumUnitDisplays
{
	UDIS_From,
	UDIS_To,
	UDIS_Category,
};

class CIssUnitInterface : public CCalcInterace
{
public:
	CIssUnitInterface(void);
	~CIssUnitInterface(void);

	void		Init(TypeCalcInit* sInit);

	BOOL		Draw(CIssGDIEx& gdiMem, RECT& rc);

	BOOL		OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL		OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);// for our dropdown menus
    BOOL        OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);//so we can update the display on keyboard presses

	BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);//definitely need this action

    BOOL        DrawSelector(HDC hdc, RECT& rc);
    BOOL        HandleDPad(int iVKKey);
	CIssUnitConverter& GetConverter(){return m_oUnit;};


private:
	void		CreateButtons();//we'll pump these on Draw to keep loading times fast
    void        CreateMainButtons();
    void        CreateKBButtons();//separate it out  ....
	void		CreateDisplays();

	BOOL		DrawDisplays(HDC hdc, RECT& rcClip);
	BOOL		DrawUnitBtns(HDC hdc, RECT& rcClip);
	BOOL		DrawMainBtns(CIssGDIEx& gdiMem, RECT& rcClip);
	BOOL        DrawKeyboardBtns(CIssGDIEx& gdiMem, RECT& rcClip);

	BOOL		DrawDisplayText(HDC hdc, RECT& rcClip);

	BOOL		OnBtnFrom();
	BOOL		OnBtnTo();
	BOOL		OnBtnConversion();

    void        LoadRegistry();
    void        SaveRegistry();

    BOOL        IsLandscape();
	BOOL		ShowKBButtons();

private:
	CIssUnitConverter   m_oUnit;

    CIssFormatCalcDisplay* m_oCalcDis;

	CCalcDynBtn	        m_oUnitBtns[(int)UBTN_Count];
	CCalcDynBtn			m_oMainBtns[NUM_KB_BUTTONS];
	CIssDynDisplay		m_oDisplay[3];

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






};
