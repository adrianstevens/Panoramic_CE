#pragma once

#include "windows.h"
#include "IssGDIEx.h"
#include "IssString.h"
#include "IssImageSliced.h"

#define REG_Key							_T("SOFTWARE\\Pano\\Launcher\\Launcher")
#define TEXT_HEIGHT_Large				(GetSystemMetrics(SM_CXICON)*13/32)
#define TITLE_Height					(IsVGA()?39:22)
#define TITLE_TEXT_Height				TEXT_HEIGHT_Large
#define INDENT                          (GetSystemMetrics(SM_CXSMICON)/4)
#define UND								-1
#define SLICE_Height                    (IsVGA()?10:5)
#define BAR_TOP_Height                  (GetSystemMetrics(SM_CXICON)/2+SLICE_Height)
#define BAR_BOTTOM_Height               (GetSystemMetrics(SM_CXICON)*3/4)
#define REGVAL_Background				_T("BK")


enum EnumCurrentGui
{
	GUI_Favorites = 0,
	GUI_Programs,
	GUI_Settings,
	GUI_Quit,
};

struct TypeSkin
{
	UINT	uiToolBarBack;
	UINT	uiToolBarBackLan;
	UINT	uiToolBarSelector;
	UINT	uiToolBarSelectorLan;
	UINT	uiToolBarArrow;
	UINT	uiToolBarArrowLan;
	UINT	uiGlow;
	UINT	uiImageArray;
	UINT	uiSelector;
	UINT	uiSelectorGlass;
	UINT	uiDetailsSelector;

	COLORREF crSelected;
	COLORREF crDrawText;
};

class CObjGui
{
public:
    CObjGui(void);
    ~CObjGui(void);

    void    Init(HWND hWndParent, HINSTANCE hInst);
    BOOL	OnSize(RECT& rcClient);
    void    DrawScreen(RECT& rcClip, HDC dc);
    void    Destroy();
    void    AdjustWindow(HWND hWnd, BOOL bShowSIP);
	void	DeleteBackground();
    void    DrawBackground(RECT& rcClip, CIssGDIEx* gdi = NULL);
	void	DrawGlow(CIssGDIEx& gdi, RECT& rcLocation);
	void	DrawToolbarItem(CIssGDIEx& gdi, EnumCurrentGui eGui, RECT& rcLocation, BOOL bDrawGlow = FALSE);
	CIssGDIEx* GetGDI();
	CIssGDIEx* GetIconArray();
	CIssGDIEx* GetGlow();
	CIssGDIEx* GetBackground(){return &m_gdiBackground;};
	void	SetSkin();
	TypeSkin& GetSkin(){return m_sSkin;};
	EnumCurrentGui& GetCurrentGui(){return m_eCurrent;};
	void	SetCurrentGui(EnumCurrentGui eCurrent){m_eCurrent = eCurrent;};
	BOOL&	IsLandscape(){return m_bIsLandscape;};

	HFONT&	GetFontLetters(){return m_hFontLetters;};
	HFONT&	GetFontSearch(){return m_hFontSearch;};
	HFONT&	GetFontTitle(){return m_hFontTitle;};
	HFONT&	GetFontText(){return m_hFontToolbar;};

	RECT&	RectScreen(){return m_rcScreen;};
	RECT&	RectToolbar(){return m_rcToolbar;};
	RECT&	RectFavorites(){return m_rcFavorites;};
	RECT&	RectPrograms(){return m_rcPrograms;};
	RECT&	RectSettings(){return m_rcSettings;};
	RECT&	RectBack(){return m_rcBack;};

private:    // functions
    void    InitBackground(RECT& rcClient);
	int		GetLineTextHeightTitle();
	int		GetLineTextHeightToolbar();
	HRESULT LoadDefaultBackground(RECT& rcClient);
	HRESULT LoadPictureBackground(RECT& rcClient);
	HRESULT LoadColorBackground(RECT& rcClient, COLORREF cr);


private:     // variables
    CIssGDIEx       m_gdiMem;
    CIssGDIEx       m_gdiBackground;
	CIssGDIEx		m_gdiIconArray;
	CIssGDIEx		m_gdiGlow;

    RECT            m_rcScreen;
	RECT			m_rcToolbar;
	RECT			m_rcFavorites;
	RECT			m_rcPrograms;
	RECT			m_rcSettings;
	RECT			m_rcBack;

	HFONT			m_hFontTitle;
	HFONT			m_hFontToolbar;
	HFONT			m_hFontLetters;
	HFONT           m_hFontSearch;

	BOOL			m_bIsLandscape;

	int				m_iLineTextHeightTitle;
	int				m_iLineTextHeightToolbar;

	TypeSkin		m_sSkin;
	EnumCurrentGui  m_eCurrent;
 
    CIssString*     m_oStr;    
    HWND            m_hWndParent;
    HINSTANCE       m_hInst;
};
