#pragma once

#include "GuiBase.h"
#include "IssVector.h"
#include "PoomContacts.h"
#include "IssKineticList.h"
#include "DlgContactDetails.h"
#include "IssImgFont.h"
#include "DlgOptions.h"
#include "IssGDIEx.h"

enum EnumCPLGroups
{
    GROUP_Personal = 0,
    GROUP_Settings,
    GROUP_Connection,
    GROUP_Unknown,
};

struct TypeCPL
{
	TCHAR*	        szName;				// friendly name of the app
	EnumCPLGroups   eGroup;             // group it belongs in
    TCHAR*          szLaunch;           // launch string
    int             iIconIndex;
	TypeCPL();
	~TypeCPL();
};


class CGuiCallLog: public CGuiBase, CIssKineticList
{
public:
	CGuiCallLog(void);
	~CGuiCallLog(void);

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

	void			RefreshCPL();	        // called if list has changed
	BOOL 	        AddMenuItems();
    BOOL            HandlePoomMessage(UINT uiMessage, WPARAM wParam, LPARAM lParam);

protected:


private:
	HRESULT			PopulateList(CIssVector<TypeCPL>& arrCPL);
	void			EraseList();
    CIssImageSliced& GetBorder();
    CIssGDIEx&      GetBgGDI();
    void            DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeCPL* sLink, BOOL bIsHighlighted);
    static void     DrawListItem(CIssGDIEx& gdi, RECT& rcDraw, TypeItems* sItem, BOOL bIsHighlighted, LPVOID lpClass);
    static int      CompareCPL( const void *arg1, const void *arg2 );

	BOOL			DrawBackground(HDC hdc, RECT& rcClient, RECT& rcClip);
	BOOL            DrawBorderOverlay(CIssGDIEx& gdi, RECT& rcClient, RECT& rcClip);
    BOOL            HasFocus();
    EnumCPLGroups   GetCPLGroup(TCHAR* szRegName);
	
private:
    CDlgContactDetails* m_dlgContactDetails;

	CIssGDIEx					m_gdiBackground;
    CIssGDIEx                   m_gdiForwardArrow;
    CIssGDIEx                   m_gdiBackArrow;
    CIssImageSliced             m_imgBorder;  
    HIMAGELIST                  m_hImageList;
	
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
};
