#pragma once
#include "IssWnd.h"
#include "IssGDIEx.h"
#include "IssString.h"
#include "IssGDIDraw.h"
#include "DlgBaseOptions.h"

enum EnumTime
{
    TIME_Start,
    TIME_End,
};

class CDlgAddTimeSlot : public CDlgBaseOptions
{
public:
	CDlgAddTimeSlot();
	~CDlgAddTimeSlot(void);

	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

	int				GetStartTime() {return m_iStart;}
	int				GetEndTime() {return m_iEnd;}
	static void		FormatTimeString(TCHAR* szText, int iHour);

private:	// functions
    void            LaunchPopupMenu();
    void            SetSelection();
	void			RefreshList();
    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            OnMenuLeft();
    void            OnMenuRight();
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    BOOL            DrawButtonItem(CIssGDIEx& gdi, BOOL bIsHighlighted, RECT& rc, EnumTime* eTime);
    static void     DrawButtonItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static void     DeleteMyItem(LPVOID lpItem);

private:	// variables
	int				m_iStart;
	int				m_iEnd;

};
