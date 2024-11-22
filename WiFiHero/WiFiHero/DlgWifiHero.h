#pragma once

#include "IssWnd.h"
#include "IssVector.h"
#include "IssGDIEx.h"
#include "IssKineticList.h"
#include "IssImageSliced.h"
#include "IssWndTouchMenu.h"
#include "DlgWifiItem.h"
#include "WirelessDevices.h"

struct PasswordType
{
    TCHAR szName[STRING_LARGE];
    TCHAR szPassword[STRING_LARGE];

};



class CDlgWifiHero:public CIssWnd
{
public:
	CDlgWifiHero(void);
	~CDlgWifiHero(void);

protected:
    BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL 			OnPaint(HWND hWnd, HDC hDC, RECT& rcClient);
    BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnLButtonDown(HWND hWnd, POINT& pt);
	BOOL 			OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL 			OnMouseMove(HWND hWnd, POINT& pt);
	BOOL 			OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);	
	BOOL			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL            OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnEraseBkgnd(HWND hWnd, WPARAM wParam, LPARAM lParam){return TRUE;};

private:    // functions
    BOOL            DeleteSelectedEntry();
    BOOL            RestoreSelectedEntry();

    void            DeletePassArray();

    BOOL            DeleteEntry(int iIndex);
    BOOL            RestoreEntry(int iIndex);

    BOOL            IsFileExists(TCHAR* szPath);
    BOOL            SaveFileInfo(TCHAR* szFileName, TCHAR* szContents);

    void            RefreshValues(BOOL bUpdateMsg = TRUE);
    BOOL            ReadFileValues(TCHAR* szFileName, CIssVector<TypeWifi>& arrItems);
    BOOL            SaveFileValues(TCHAR* szFileName);
    BOOL            SaveFileValues(TCHAR* szFileName, CIssVector<TypeWifi>& arrItems);

    BOOL            DrawBackground(CIssGDIEx& gdiMem, RECT& rcClient, RECT& rcClip);
    BOOL            DrawText(CIssGDIEx& m_gdiMem, RECT& rcClip);


    void            SynchronizeWifiProfiles(CIssVector<TypeWifi>& arrDevice, CIssVector<TypeWifi>& arrSaved, CIssVector<TypeWifi>& arrDiff);
    void            DeleteProfile(CIssVector<TypeWifi>& arrItems);
    BOOL            DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, TypeWifi* sWifi);
    static void     DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    void            OnUpdate();
    void            OnMenu();
    void            OnSaveProfile();
    void            OnDeleteProfile();
    BOOL            UpdateDevice(TCHAR* szFileName);

    void            LaunchWifiItem(int iIndex);
    BOOL            UpdateTitle(TCHAR* szMsg);

    void            CheckForPasswords();

    void            LoadRegistry();
    void            SaveRegistry();

    static void     DeleteMenuItem(LPVOID lpItem);


private:    // variables
    CIssVector<PasswordType> m_arrPasswords;

    CIssKineticList m_oList;
    CIssWndTouchMenu m_wndMenu;
    //CWirelessDevices m_oWireless;           // so we can turn off the WIFI
    
    CIssGDIEx       m_gdiMem;
    CIssGDIEx       m_gdiBack;

    CIssImageSliced	m_imgButton;            // kinetic list button
    CIssImageSliced	m_imgButtonSel;         // kinetic list selected button

    TCHAR           m_szTitle[STRING_MAX];      
    RECT            m_rcTitle;
    RECT            m_rcList;

    HFONT           m_hFontBtnLabel;

    BOOL            m_bFirstTime;
    BOOL            m_bShowAllProfiles;     // should we show ondevice and saved profiles together
};
