#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "GuiToolBar.h"
#include "GuiPrograms.h"
#include "GuiFavorites.h"
#include "GuiSettings.h"
#include "IssWndTouchMenu.h"
#include "DlgOptions.h"
#include "IssKey.h"
#include "DlgStart.h"

class CDlgLauncher:public CIssWnd
{
public:
	CDlgLauncher();
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
    
    
    
private:    // function
    void            LaunchOptions();
	void            LoadOptions();
	void            SaveOptions();
	void			LaunchMenu();
	void            LaunchAbout();
	void            LaunchHelp();
    void            CheckDemo();
    void            UpdateStartIcon();

private:	// variables
	/*CIssGDIEx		m_gdiMem;		// shared off screen GDI
	CIssGDIEx		m_gdiBg;        // shared background GDI
	CIssGDIEx		m_gdiGlow;	    // shared glow image for lighting icons
    CIssGDIEx       m_gdiImgIconArray;*/

	CGuiBase*		m_pGui;			// pointer to the currently displayed gui
	CGuiPrograms 	m_guiPrograms;	// programs and games
	CGuiFavorites	m_guiFavorites;	// Favorites Screen
	CGuiSettings	m_guiSettings;	// Settings (control panel)
    CDlgStart       m_dlgStart;     // start icon

    
    EnumIconStyle   m_eIconStyle;

    BOOL            m_bPreventUsage;
};
