#pragma once

#include "GuiBase.h"
#include "IssVector.h"
#include "PoomContacts.h"
#include "IssKineticList.h"
#include "DlgContactDetails.h"
#include "IssImgFont.h"
#include "DlgOptions.h"
#include "IssGDIEx.h"
#include "ObjProcesses.h"

enum EnumTodayItems
{
    TODAY_Indicators,
    TODAY_DateTime,
    TODAY_RunningPrograms,
    TODAY_ProgramItem,
    TODAY_Calendar,
};

class CGuiFavorites: public CGuiBase, CIssKineticList
{
public:
	CGuiFavorites(void);
	~CGuiFavorites(void);

	BOOL			Init(HWND hWndParent, HINSTANCE hInst, CIssGDIEx* gdiMem, CIssGDIEx* gdiBg, CGuiBackground* guiBackground, TypeOptions* sOptions, CDlgContactDetails* dlgContactDetails);
	void            ReloadColorSchemeItems(HWND hWnd, HINSTANCE hInstance);
    BOOL			SetPosition(RECT& rc);

	BOOL			Draw(HDC hDC, RECT& rcClient, RECT& rcClip);
	BOOL			OnLButtonDown(POINT pt);
	BOOL			OnLButtonUp(POINT pt);
	BOOL			OnMouseMove(POINT pt);
	BOOL			OnTimer(WPARAM wParam, LPARAM lParam);
	BOOL			OnChar(WPARAM wParam, LPARAM lParam);
	BOOL	        OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL			OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL 			OnActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL			MoveGui(RECT rcLocation);

	void			RefreshToday();	        // called if list has changed
	BOOL 	        AddMenuItems();


protected:


private:
	HRESULT			PopulateList();
	void			EraseList();
    CIssGDIEx&      GetBgGDI();
    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, EnumTodayItems* eTodayItem, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static int      CompareCPL( const void *arg1, const void *arg2 );

	BOOL			DrawBackground(HDC hdc, RECT& rcClient, RECT& rcClip);
    BOOL            HasFocus();
	
private:
	
    CIssGDIEx                   m_gdiBackground;
	HFONT						m_hFontLetters;
	HFONT						m_hFontContact;
	HFONT						m_hFontContactBold;
	POINT						m_ptScreenSize;
	POINT						m_ptMouseDown;

	int							m_iTitleHeight;
	int							m_iLetterHeight;
	int							m_iContactHeight;
    int                         m_iContactHeightSelected;
	int							m_iContactPicHeight;
	RECT						m_rcContactList;

    BOOL                        m_bShowRunningList;
};
