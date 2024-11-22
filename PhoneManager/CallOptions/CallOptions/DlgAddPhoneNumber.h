#pragma once

#include "IssWnd.h"
#include "IssGDIEx.h"
#include "ObjBlocklist.h"
#include "IssGDIDraw.h"
#include "IssDynBtn.h"
#include "IssDynDisplay.h"
#include "DlgBaseOptions.h"

class CDlgAddPhoneNumber : public CDlgBaseOptions
{
public:
	CDlgAddPhoneNumber();
	~CDlgAddPhoneNumber(void);

	BOOL 			OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnLButtonDown(HWND hWnd, POINT& pt);
    BOOL            OnLButtonUp(HWND hWnd, POINT& pt);
	BOOL 			OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL            OnUser(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
    BOOL            OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);

	TypeBlocklist*	GetSelectedItem()	{return &m_sBlocklistItem; };

private:
	BOOL			FormatFullNumber(TCHAR* szSrc, TCHAR* szDest);
	BOOL			FormatNumberPrefix(TCHAR* szSrc, TCHAR* szDest);
	BOOL			GetAreaCode(TCHAR* szSrc, TCHAR* szDest);
	BOOL			MakeNumeric(TCHAR* szText);
    void            CreateButtons(int iTop);
    void            RefreshList();
    void			SetSelection();
    BOOL            OnDraw(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            OnMenuLeft();
    void            OnMenuRight();
    void            DrawTopBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            DrawBottomBar(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    void            AddNumber(int iNum);

private:
	TypeBlocklist	m_sBlocklistItem;
    TCHAR           m_szNumber[STRING_MAX];
    CIssDynDisplay  m_oDisplay;
    CIssDynBtn      m_btnClear;
    CIssDynBtn      m_btnNumber[10];
    HFONT           m_hBtnFont;
};
