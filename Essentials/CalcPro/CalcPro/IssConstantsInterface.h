#pragma once
#include "calcinterace.h"
#include "ObjConstants.h"
#include "globals.h"

enum EnumConstButtons
{
    CSB_1,
    CSB_2,
    CSB_3,
    CSB_4,
    CSB_AddFav,
    CSB_Count,
};

enum EnumConstDisplay
{
    CD_Normal,
    CD_LastValue,
};

class CIssConstantsInterface :  public CCalcInterace
{
public:
    CIssConstantsInterface(void);
    ~CIssConstantsInterface(void);


    void        SetPosition(HWND hWnd, int iYMain, int iYAlt);

	void		Init(TypeCalcInit* sInit);
    BOOL		Draw(HDC hdc, RECT& rc);

	BOOL		OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL		OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL		OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);// for our dropdown menus

	BOOL		OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);//definitely need this action

    BOOL        OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

    BOOL        OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL        DrawSelector(HDC hdc, RECT& rc);

    BOOL        HandleDPad(int iVKKey);

    BOOL        DrawMainButtons();
private:
	void		CreateButtons();//we'll pump these on Draw to keep loading times fast
	void		CreateDisplays();
    void        LaunchMenu(int iBtn, UINT uiMenuID);
    void        SelectMenuItem(int iBtn, int iIndex);

	BOOL		DrawDisplays(HDC hdc, RECT& rcClip);
	BOOL		DrawDisplayText(HDC hdc, RECT& rcClip);
    
    BOOL		DrawConstBtns(HDC hdc, RECT& rcClip);


    void        LoadRegistry();
    void        SaveRegistry();

    void        OnBtnFavorites();
    void        OnBtn1();
    void        OnBtn2();
    void        OnBtn3();
    void        OnBtn4();

	int			GetSystemMetrics(__in int nIndex){return GetSysMets(nIndex);};

private:
    CCalcDynBtn     m_oConstBtns[(int)CSB_Count];
    CIssDynDisplay  m_oConstDisplay;

    EnumConstDisplay   m_eDisStyle;

    TCHAR*      m_szLastConst;

    HFONT       m_hFontButton;
    HFONT       m_hFontDisplay;

    RECT        m_rcDrawArea;

    RECT        m_rcBtns[4];
    RECT        m_rcConstDisplay;
    RECT        m_rcFavBtns;

    BOOL        m_bDrawPlus;
};
