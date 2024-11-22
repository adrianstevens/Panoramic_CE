#pragma once

#include "DlgBaseOptions.h"

enum EnumOption
{
    OPTION_GeneralOptions = 0,
    OPTION_BlockList,
    OPTION_CallingRules,
    OPTION_Ringtones,
    OPTION_TodayScreen,
    OPTION_Help,
    OPTION_Count,
};

class CDlgCallOptions:public CDlgBaseOptions
{
public:
	CDlgCallOptions(void);
	~CDlgCallOptions(void);

	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);	
	BOOL			OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

protected:
    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            OnMenuLeft();
    void            OnMenuRight();
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);

private:
    static void         DeleteMyItem(LPVOID lpItem);
    static void         DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    BOOL				DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumOption* eOption);
    void                LaunchOption();
    void                PopulateContent(int iSelectedIndex = 0);


private:    // variables
    HFONT    m_hFont;

};
