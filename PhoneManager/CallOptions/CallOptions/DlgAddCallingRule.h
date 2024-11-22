#pragma once

#include "IssGDIDraw.h"
#include "IssWnd.h"
#include "IssGDIEx.h"
#include "ObjCallOptionList.h"
#include "DlgBaseOptions.h"


class CDlgAddCallingRule : public CDlgBaseOptions
{
public:
	CDlgAddCallingRule();
	~CDlgAddCallingRule(void);

	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

	TypeCallingRule* GetRule() {return &m_sRule;}

private:	// functions
	void			RefreshList();
	BOOL			TextIsSingleLine(CIssGDIEx& gdi, TCHAR* szText, RECT& rcTest);
	BOOL			AddCallingRule();
	static int		CompareItems(const void* lp1, const void* lp2);

    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            OnMenuLeft();
    void            OnMenuRight();
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    BOOL            DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, CallRuleType* eCall);
    static void     DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void     DeleteMyItem(LPVOID lpItem);

private:	// variables
	TypeCallingRule m_sRule;

};
