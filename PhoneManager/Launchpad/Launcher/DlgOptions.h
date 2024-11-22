#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "DlgBase.h"
#include "DlgStart.h"
#include "IssImageSliced.h"
#include "IssKineticList.h"
#include "IssWndTouchMenu.h"
#include "ObjAssociations.h"

enum EnumSkin
{
	SKIN_Glass,
};

enum EnumColorScheme
{
	COLOR_Green,
	COLOR_Blue,
	COLOR_Orange,
	COLOR_Pink,
	COLOR_Purple,
	COLOR_Red,
};

enum EnumOption
{
    OPTION_ColorScheme,
    OPTION_IconStyle,
    OPTION_FavSize,
    OPTION_TodayMenuItem,
    OPTION_AlwaysShowText,          //favorites
    OPTION_SlideLeft,
    OPTION_SlideRight,
    OPTION_StartMenu,
    OPTION_SettingsHeaders,
    OPTION_HideOnAppLaunch,
    OPTION_Language,
	OPTION_Background,
	OPTION_Fullscreen,
	OPTION_DrawTiles,
	OPTION_StartButton,
};

enum EnumFavSize
{
    FAVSIZE_Large   = 3,
    FAVSIZE_Normal  = 4,
    FAVSIZE_Small   = 5,
	FAVSIZE_XSmall  = 6,
};

enum EnumSlideName
{
    SLIDE_Programs = 0,
    SLIDE_Games,
    SLIDE_Apps,
    SLIDE_Settings,
    SLIDE_Favorites,
    SLIDE_Utilities,
    SLIDE_Music,
    SLIDE_Folders,
    SLIDE_Files,
    SLIDE_Videos,
    SLIDE_Pictures,
};

enum EnumBackground
{
	BK_DefaultToday,
	BK_Picture,
	BK_Camera,
	BK_Normal,
	BK_Off,
	BK_Blue,
	BK_Orange,
	BK_Red,
	BK_Green,
	BK_Count,
};

struct TypeOptions
{
    EnumFavSize     eFavSize;
    BOOL            bAlwaysShowText;
    EnumSlideName   eSlideLeft;
    EnumSlideName   eSlideRight;
    BOOL            bShowSettingsHeaders;
    BOOL            bHideOnAppLaunch;
	EnumSkin	    eSkin;
	EnumColorScheme eColorScheme;
	BOOL			bShowFullScreen;
	EnumBackground	eBackground;
	BOOL			bDrawTiles;
	BOOL			bStartButton;
};


class CDlgOptions : public CDlgBase
{
public:
	CDlgOptions(void);
	~CDlgOptions(void);
    void                SetOptions(TypeOptions sOptions){m_sOptions = sOptions;};
    TypeOptions         GetOptions(){return m_sOptions;};
    void                SetIconStyle(EnumIconStyle eStyle){m_eIconStyle = eStyle;};
    EnumIconStyle       GetIconStyle(){return m_eIconStyle;};
	BOOL				IsBackgroundChanged(){return m_bBackgroundChanged;};
	TCHAR*				GetBackgroundFileName(){return m_szBackgroundFile;};
    //EnumColorScheme     GetColorScheme(){return m_eColorScheme;};
    static void         TurnStartMenuOff();
    static void         GetSliderName(EnumSlideName& eSlider, TCHAR* szName);

    void                RestartStartMenu();
    
protected:
	BOOL				OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL				OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL				OnMouseMove(HWND hWnd, POINT& pt);
	BOOL				OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL				OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
    BOOL                OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	BOOL				DrawContactPic(CIssGDIEx& gdi, RECT rcClient, RECT rcClip);
	void                DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void                DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
	BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption);
    void                OnMenuLeft();
    void                OnMenuRight();
	static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void         DeleteMyItem(LPVOID lpItem);
    void                PopulateOptions();
    void                LaunchPopupMenu();
    void                SetMenuSelection();
    void                FadeInScreen();
    void                GetItemTitle(EnumOption eOption, TCHAR* szTitle);
    BOOL                IsStartMenuOn();
    void                TurnStartMenuOn();
    static void         RemoveAutoLaunch(TCHAR* szName);
    void                SetAutoLaunch(TCHAR* szFileName, TCHAR* szName);
	void				SetBackgroundOption(EnumBackground eBackground);

private:
	CIssImageSliced		m_imgButton;            // kinetic list button
    CIssImageSliced		m_imgButtonSel;         // kinetic list selected button
    CIssKineticList     m_oMenu;

    CObjAssociations*   m_objAssociations;
    DWORD               m_dwOrigLanguage;           // current language

    TypeOptions         m_sOptions;             // our options structure
    EnumIconStyle       m_eIconStyle;
    
	HFONT				m_hFontText;
	HFONT				m_hFontBtnText;

	BOOL				m_bFadeIn;
    BOOL                m_bSetTodayMenu;
    BOOL                m_bStartMenuOn;

	// for Background
	TCHAR*				m_szBackgroundFile;
	BOOL				m_bBackgroundChanged;
};
