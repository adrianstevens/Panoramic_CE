#pragma once

#include "IssGDIDraw.h"
#include "IssWnd.h"
#include "IssGDIEx.h"
#include "DlgBaseOptions.h"

enum EnumTodayScreen
{
    TS_AddRemoveTodayScreen = 0,
    TS_AddRemoveItems,
    TS_NumItems,
};

class CDlgTodayScreen : public CDlgBaseOptions
{
public:
	CDlgTodayScreen();
	~CDlgTodayScreen(void);

	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:	// functions
	void			RefreshList();	
    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            OnMenuLeft();
    void            OnMenuRight();
    void            LaunchOption(EnumTodayScreen& eItem);
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    BOOL            DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumTodayScreen* eItem);
    static void     DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void     DeleteMyItem(LPVOID lpItem);

private:	// variables

};
