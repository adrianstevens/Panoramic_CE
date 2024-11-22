#pragma once
#include "isswnd.h"
#include "IssGDIEx.h"
#include "DlgBase.h"
#include "IssImageSliced.h"
#include "IssKineticList.h"
#include "IssWndTouchMenu.h"
#include "ObjSkinEngine.h"
#include "ObjAssociations.h"

enum EnumOption
{
    OPTION_ShowSliderBar,
    OPTION_SortContacts,
    OPTION_SMSHistory,
    OPTION_CallHistory,
    OPTION_ColorScheme,
    OPTION_FavSize,
    OPTION_SlideLeft,
    OPTION_SlideRight,
    OPTION_ShowHistory,
    OPTION_OverrideDefaultContacts,
    OPTION_TodayMenuItem,
    OPTION_AlwaysShowText,          //favorites
    OPTION_ContactsVerboseMode,
    OPTION_ShowAnimations,
    OPTION_ShowSIMContacts,
};

enum EnumFavSize
{
    FAVSIZE_Large   = 3,
    FAVSIZE_Normal  = 4,
    FAVSIZE_Small   = 5,
};

enum EnumSortContacts
{
    SORT_FirstName,
    SORT_LastName,
    SORT_Email,
    SORT_Company,
};

enum EnumShowSMS
{
    SMS_All,
    SMS_Incoming,
    SMS_Outgoing,
    SMS_Off,
};

enum EnumShowCall
{
    CALL_All,
    CALL_Incoming,
    CALL_Outgoing,
    CALL_Missed,
    CALL_Off,
    CALL_IncomingOutgoing,
    CALL_IncomingMissed,
    CALL_OutgoingMissed,
};

enum EnumShowHistory
{
    HISTORY_All,
    HISTORY_LastMonth,
    HISTORY_Last100,
    HISTORY_Last200,
};

enum EnumSlideName
{
    SLIDE_Personal = 0,
    SLIDE_Business,
    SLIDE_Friends,
    SLIDE_Family,
    SLIDE_Favorites,
};

struct TypeOptions
{
    BOOL                bShowSliderBar;     // should we show the slider bar in the favorites screen
    EnumSortContacts    eSortContacts;      // sorting the contacts list First Name/Last Name/Email/Company
    EnumShowSMS         eShowSMS;           // Showing SMS All/incoming/outgoing/Off
    EnumShowCall        eShowCall;          // Showing Call history All/incoming/outgoing/missed/off
    EnumShowHistory     eShowHistory;       // Show History list All/Last month/last 100 entries/last 200 entries
    BOOL                bAlwaysShowText;
    EnumFavSize         eFavSize;
    BOOL                bContactsVerboseMode;   // show contacts in big mode
    BOOL                bShowAnimations;    // transitions, fade ins, etc
    EnumSlideName       eSlideLeft;         // Left slider text
    EnumSlideName       eSlideRight;        // right slider text
    BOOL                bShowSimContacts;   // do we want to see SIM Contacts
};

class CDlgOptions : public CDlgBase
{
public:
	CDlgOptions(void);
	~CDlgOptions(void);
    void                SetOptions(TypeOptions sOptions){m_sOptions = sOptions;};
    TypeOptions         GetOptions(){return m_sOptions;};
    EnumColorScheme     GetColorScheme(){return m_eColorScheme;};
    static void         GetSliderName(EnumSlideName& eSlider, TCHAR* szName);

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

private:
	CIssImageSliced		m_imgButton;            // kinetic list button
    CIssImageSliced		m_imgButtonSel;         // kinetic list selected button
    CIssKineticList     m_oMenu;
    CIssWndTouchMenu    m_wndMenu;              // our popup menu
    CObjAssociations*   m_objAssociations;

    TypeOptions         m_sOptions;             // our options structure
    EnumColorScheme     m_eColorScheme;         // Current Color Scheme blue/green/orange/pink/purple/red

	HFONT				m_hFontText;
	HFONT				m_hFontBtnText;

	BOOL				m_bFadeIn;
    BOOL                m_bContactOverride;
    BOOL                m_bSetTodayMenu;
};