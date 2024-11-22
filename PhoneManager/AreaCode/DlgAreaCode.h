#pragma once
#include "DlgBase.h"
#include "IssString.h"
#include "IssDynDisplay.h"
#include "IssAreaCode.h"
#include "IssImageSliced.h"
#include "IssCommon.h"
#include "IssWndTouchMenu.h"

enum EnumAreaState
{
    AS_Search,
    AS_Display,
    AS_About, //its just easier
};

class CDlgAreaCode:public CDlgBase
{
public:
	CDlgAreaCode(void);
	~CDlgAreaCode(void);

//	void	Init(HINSTANCE hInst){m_hInst = hInst;};
    HRESULT LoadImages(HWND hWnd, HINSTANCE hInst);

protected:
	BOOL	OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL	OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL	OnMouseMove(HWND hWnd, POINT& pt);
	BOOL	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL	OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL    OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL    OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

    BOOL    OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
   
    
private:	// functions
    void    ChangeState(EnumAreaState eState);

    void    LoadCurrentFlag();
	void	LaunchMoreApps();

    BOOL    DrawBackgroundElements(CIssGDIEx& gdi, RECT& rcClip);
    BOOL    DrawBackgroundSearch(CIssGDIEx& gdi, RECT& rcClip);
    BOOL    DrawBackgroundDisplay(CIssGDIEx& gdi, RECT& rcClip);
    BOOL    DrawBackgroundAbout(CIssGDIEx& gdi, RECT& rcClip);
    BOOL    DrawFlag(CIssGDIEx& gdi, RECT& rcClip);


    BOOL    DrawText(CIssGDIEx& gdi, RECT& rcClip);
    BOOL    DrawGlobe(CIssGDIEx& gdi, RECT& rcClip);
    void    DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);

    BOOL    OnNumButton(int iNum);
    BOOL    OnClear();
    BOOL    OnLocate(BOOL bLookup = TRUE);

    BOOL    OnSearch();//change modes

    void    OnMenuLeft();
    void    OnMenuRight();
    void    OnSearchBy();

    void    Locate(int iIndex);
   

    
private:	// variables
    CIssDynDisplay      m_oDisplay;
    CIssAreaCode	    m_oArea;		// area codes and time zones

    CIssWndTouchMenu    m_wndMenu;      // we'll use this for the normal menus

    CIssGDIEx           m_gdiGlobe;
    CIssGDIEx           m_gdiDownArrow;
    CIssGDIEx           m_gdiFlag; //current flag

    CIssImageSliced     m_imgBtn;
    CIssImageSliced     m_imgBtnNum;
    CIssImageSliced     m_imgBtnDown;
    CIssImageSliced     m_imgBtnNumDown;

    CIssImageSliced     m_imgGreenBar;
    CIssImageSliced     m_imgShadow;
    CIssImageSliced     m_imgShadowLong;

    EnumAreaState       m_eState;
    EnumLookupType      m_eLookupType;

    TCHAR               m_szAreaCode[STRING_NORMAL];//big enough for error message
    TCHAR               m_szSearch[STRING_NORMAL];

    RECT                m_rcTopGreenBar;
    RECT                m_rcTopText;
    RECT                m_rcGlobe;
    RECT                m_rcAreaCodeDisplay;
    RECT                m_rcClearBtn;
    RECT                m_rcLocateBtn;
    RECT                m_rcShadow;
    RECT                m_rcShadowLong;
    RECT                m_rcSearchBar;
    RECT                m_rcSearchBy;
    RECT                m_rcSearchType;
    RECT                m_rcNumBtns[10];
    

    RECT                m_rcCountryCode;
    RECT                m_rcCountry;
    RECT                m_rcRegion;
    RECT                m_rcLocalTime;
    RECT                m_rcLanguage;
    RECT                m_rcCapital;
    RECT                m_rcCurrency;
    RECT                m_rcPopulation;
    RECT                m_rcCodes;

    RECT                m_rcMenuLeft;
    RECT                m_rcMenuRight;

    HFONT               m_hFontSmallerText;
    HFONT               m_hFontSmText;
    HFONT               m_hFontText;
    HFONT               m_hFontButton;
    HFONT               m_hFontAreaCode;

    int                 m_iGlobeFrame;

    BOOL                m_bNumBtns[10];//probably should have been a struct but oh well
    BOOL                m_bClearBtn;
    BOOL                m_bLocateBtn;

    BOOL                m_bShowSearch;
};
