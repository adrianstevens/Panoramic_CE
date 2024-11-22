#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "GuiToolBar.h"
#include "GuiPrograms.h"
#include "GuiFavorites.h"
#include "GuiSettings.h"
#include "IssWndTouchMenu.h"
#include "DlgOptions.h"
#include "GuiBackground.h"
#include "IssKey.h"
#include "DlgStart.h"

class CDlgLauncher:public CIssWnd
{
public:
	CDlgLauncher(EnumCurrentGui eSelection = GUI_Favorites);
	~CDlgLauncher(void);

	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL 			OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL 			OnMouseMove(HWND hWnd, POINT& pt);
	BOOL 			OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
	BOOL 			OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);	
    BOOL	        OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam);
    //BOOL	        OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};		//uncomment for MS
	//BOOL	        OnHibernate(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;}; //uncomment for MS
    BOOL            OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnSettingChange(HWND hWnd, WPARAM wParam, LPARAM lParam);
    
    EnumCurrentGui  GetCurSelection();
    CIssGDIEx&      GetDrawGDI()    { return m_gdiMem; }
    CIssGDIEx&      GetGlowImg();
    CIssGDIEx&      GetIconArray();
    void            SetDirty(RECT& rc);
    void            NotifyToolbarFocus(BOOL bHasFocus);
	void            LoadOptions();
    void            SaveOptions();
	void			LaunchMenu();
    void            LaunchAbout();
    void            LaunchHelp();
    static TypeOptions& GetOptions();
    static CIssGDIEx& GetCallIcons();
    static HFONT    GetFontNormal();
    static HFONT    GetFontBold();

    HFONT           _GetFontNormal() { return m_hFontNormal; }
    HFONT           _GetFontBold()   { return m_hFontBold; }


private:    // function
    void            LaunchOptions();
    void            CheckDemo();
    void            UpdateStartIcon();

private:	// variables
	CIssGDIEx		m_gdiMem;		// shared off screen GDI
	CIssGDIEx		m_gdiBg;        // shared background GDI
	CIssGDIEx		m_gdiGlow;	    // shared glow image for lighting icons
    CIssGDIEx       m_gdiImgIconArray;
	CGuiToolBar		m_guiToolBar;	// the toolbar

	CGuiBase*		m_pGui;			// pointer to the currently displayed gui
    CGuiBackground  m_guiBackground;// handle background drawing
	CGuiPrograms 	m_guiPrograms;	// programs and games
	CGuiFavorites	m_guiFavorites;	// Favorites Screen
	CGuiSettings	m_guiSettings;	// Settings (control panel)
    CDlgStart       m_dlgStart;     // start icon

    TypeOptions     m_sOptions;     // options settings
    EnumIconStyle   m_eIconStyle;

    HFONT		    m_hFontNormal;
	HFONT		    m_hFontBold;

    BOOL            m_bPreventUsage;


	CIssWndTouchMenu m_wndMenu;
    RECT            m_rcClient;
};
