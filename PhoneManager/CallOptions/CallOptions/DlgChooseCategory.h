#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssGDIDraw.h"
#include "CtrlList.h"
#include "IssString.h"
#include "DlgBaseOptions.h"


class CDlgChooseCategory : public CDlgBaseOptions
{
public:
	CDlgChooseCategory();
	~CDlgChooseCategory(void);

	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

	void			LoadCategories();
	BOOL			GetCategoryFromSelection();
	TCHAR*			GetCategory() {return m_szCategory;}

private:		// functions
	static int		CompareItems(const void* lp1, const void* lp2);
    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            OnMenuLeft();
    void            OnMenuRight();
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);


private:		// variables
	TCHAR			m_szCategory[STRING_LARGE];

};
