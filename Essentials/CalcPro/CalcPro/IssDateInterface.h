#pragma once

#include "CalcInterace.h"
#include "IssFormatCalcDisplay.h"
#include "IssDateCalc.h"
#include "globals.h"

enum EnumDateDisplays
{
    DD_From,
    DD_Interval,
    DD_To,
    DD_Count,
};

class CIssDateInterface : public CCalcInterace
{
public:
	CIssDateInterface(void);
	~CIssDateInterface(void);

//#ifdef UNDER_CE
	void		Init(TypeCalcInit* sInit);

	BOOL		Draw(HDC hdc, RECT& rc);

	BOOL		OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL		OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);// for our dropdown menus

	BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);//definitely need this action

    void        LostFocus();
	
private:
	void		CreateButtons();
    void        CreateMainButtons();//this is pretty custom
    void        CreateControls();

    void        SetControls();

	BOOL		DrawDisplays(HDC hdc, RECT& rcClip);
	BOOL		DrawBtns(HDC hdc, RECT& rcClip);

	BOOL		DrawDisplayText(HDC hdc, RECT& rcClip);

    void        OnButtonFrom();
    void        OnButtonTo();
    void        OnButtonInt();

    void        HandleMainButtons(int iBtn);

    void        UpdateStart(); //start date
    void        UpdateInterval(); //interval time
    void        UpdateEnd();//end date
//#endif

    void        LoadRegistry();
    void        SaveRegistry();

	int			GetSystemMetrics(__in int nIndex){return GetSysMets(nIndex);};

private:
    CIssDateCalc        m_oDate;

    CCalcDynBtn         m_oMainBtns[12];//we should probably define these somewhere but I think we're going to custom handle it all
    CCalcDynBtn         m_oDateBtns[3];

    HWND                m_hWndDateFrom;
    HWND                m_hWndDateTo;
    HWND                m_hWndTimeFrom;
    HWND                m_hWndTimeTo;

    HWND                m_hWndDays;
    HWND                m_hWndHours;
    HWND                m_hWndMinutes;
    HWND                m_hWndSeconds;

    HFONT               m_hFontLabel;

    RECT                m_rcLabels[3];

    RECT                m_rcInterval;
    RECT                m_rcDays;
    RECT                m_rcHours;
    RECT                m_rcMin;
    RECT                m_rcSec;

    HFONT               m_hFontControl;
                    
	HFONT               m_hFontBtnText;
	HFONT               m_hFontBtnTextSm;

	HFONT				m_hFontTop;

	RECT				m_rcMainBtns;

    int                 m_iLabelSize;//size and spacing of labels

    BOOL                m_bLand;

};
